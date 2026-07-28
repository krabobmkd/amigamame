/*===================================================================*/
/*               TX-1/Buggy Boy (Tatsumi) Hardware                   */
/*         SN74S516 Arithmetic Unit and Interface Emulation          */
/*                                                                    */
/*  Buggy Boy port: the sequencer/state-machine algorithm below is   */
/*  ported from the completed MAME 0.144 tx1 driver (Phil Bennett),  */
/*  reimplemented against 0.106's byte-wide handler / global-static  */
/*  style instead of 0.144's 16-bit-bus / state-class style.         */
/*  TX-1 itself is not wired to this code (still plain AM_RAM in     */
/*  tx1_slave) - only Buggy Boy/Buggy Boy Jr use it so far.          */
/*===================================================================*/

#include "driver.h"

/* Shared with drivers/tx1.c: slave RAM backing both the plain
   0x0000-0x07ff window and its /SPCS-decoded mirror at 0x0800-0x0fff. */
extern UINT8 *bb_math_ram;

/*
    SN74S516 16x16 multiplier/divider
*/
typedef struct
{
	INT16 X;
	INT16 Y;

	union
	{
		#ifdef LSB_FIRST
		struct { UINT16 W; INT16 Z; } ZW16;
		#else
		struct { INT16 Z; UINT16 W; } ZW16;
		#endif
		INT32 ZW32;
	} ZW;

	int code;
	int state;
	int ZWfl;
} sn74s516_t;

/* Buggy Boy AU sequencer state */
typedef struct
{
	UINT16 cpulatch;
	UINT16 promaddr;
	UINT16 inslatch;
	int    mux;
	UINT16 ppshift;
	UINT32 i0ff;
	UINT16 retval;
} math_t;

static sn74s516_t SN74S516;
static math_t math;

/*
    Combined function-data + instruction PROM image, built by
    ROM_START(buggyb1)/(buggyboy) into REGION_USER1 (mirrors 0.144's
    single "au_data" region layout exactly, same physical ROM files):

      word 0x0000-0x3fff : function data ROMs (bug9.138 low byte, bug10.95 high byte)
      word 0x4000-0x41ff : instruction PROM   (bb1.163 low byte,  bb2.162 high byte)

    Read as separate bytes (not a UINT16* cast) so this doesn't depend
    on host endianness.
*/
#define AU_INS_PROM_BASE	0x4000	/* word index of instruction PROM within REGION_USER1 */

static UINT16 au_word(const UINT8 *rom, int word_index)
{
	int off = word_index * 2;
	return rom[off] | (rom[off + 1] << 8);
}

#define INC_PROM_ADDR		( math.promaddr = (math.promaddr + 1) & 0x1ff )
#define ROR16(val, shift)	( ((UINT16)(val) >> (shift)) | ((UINT16)(val) << (16 - (shift))) )
#define ROL16(val, shift)	( ((UINT16)(val) << (shift)) | ((UINT16)(val) >> (16 - (shift))) )
#define SWAP16(val)		( (((UINT16)(val) << 8) & 0xff00) | ((UINT16)(val) >> 8) )
/* BIT(x,n) already provided by mamecore.h */

static UINT8 reverse_nibble(UINT8 nibble)
{
	return	(nibble & 1) << 3 |
		(nibble & 2) << 1 |
		(nibble & 4) >> 1 |
		(nibble & 8) >> 3;
}

/*
    State transition table.

    A little different to the real thing in that there are no states
    between final input and multiplication/division.
*/
static const INT8 state_table[16][8] =
{
	{  4,  4,  4,  4,  5,  1,  1,  0 },
	{  4,  4,  4,  4,  5,  5,  3,  0 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{  4,  4,  4,  4,  5,  5, 11,  0 },
	{  8,  8,  8,  8,  8,  8,  8,  8 },
	{ 10, 10, 10, 10, 10, 10, 10, 10 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{  4,  4,  4,  4,  5,  0,  1,  0 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{  4,  4,  4,  4,  4,  5,  1,  0 },
	{  4,  4,  4,  4,  5,  5,  1,  0 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1, -1 },
};

static void sn_multiply(void)
{
	switch (SN74S516.code)
	{
		case 0:
			SN74S516.ZW.ZW32 = SN74S516.X * SN74S516.Y;
			break;

		case 2:
			SN74S516.ZW.ZW32 += SN74S516.X * SN74S516.Y;
			break;

		case 3:
			SN74S516.ZW.ZW32 += -SN74S516.X * SN74S516.Y;
			break;

		case 0x60:
			SN74S516.ZW.ZW32 = SN74S516.X * SN74S516.Y;
			break;

		case 0x61:
			SN74S516.ZW.ZW32 = -SN74S516.X * SN74S516.Y;
			break;

		case 0x62:
			SN74S516.ZW.ZW32 += SN74S516.X * SN74S516.Y;
			break;

		case 0x63:
			SN74S516.ZW.ZW32 += -SN74S516.X * SN74S516.Y;
			break;

		case 0x660:
			SN74S516.ZW.ZW32 = (SN74S516.X * SN74S516.Y) + (SN74S516.ZW.ZW32 & 0xffff0000);
			break;

		case 0x661:
			SN74S516.ZW.ZW32 = (-SN74S516.X * SN74S516.Y) + (SN74S516.ZW.ZW32 & 0xffff0000);
			break;

		case 0x662:
			SN74S516.ZW.ZW32 = (-SN74S516.X * SN74S516.Y) + (SN74S516.ZW.ZW32 & 0xffff0000);
			break;

		case 0x6660:
			SN74S516.ZW.ZW32 += (SN74S516.X * SN74S516.Y);
			break;

		default:
			break;
	}

	/* Seems a good enough place to clear it. */
	SN74S516.ZWfl = 0;
}

static void sn_divide(void)
{
	INT32 Z = 0;
	INT32 W = 0;

	if (SN74S516.X == 0)
	{
		SN74S516.ZW.ZW16.Z = (INT16)0xffff;
		SN74S516.ZW.ZW16.W = 0xffff;
		SN74S516.ZWfl = 0;
		return;
	}

	switch (SN74S516.code)
	{
		case 4:
			Z = SN74S516.ZW.ZW32 / SN74S516.X;
			W = SN74S516.ZW.ZW32 % SN74S516.X;
			break;

		case 0x664:
			Z = SN74S516.ZW.ZW32 / SN74S516.X;
			W = SN74S516.ZW.ZW32 % SN74S516.X;
			break;

		case 0x6664:
			Z = SN74S516.ZW.ZW16.W / SN74S516.X;
			W = SN74S516.ZW.ZW16.W % SN74S516.X;
			break;

		default:
			break;
	}

	/* Divide overflow. Only happens during chip test anyway */
	if (Z > 0xffff)
		Z |= 0xff00;

	SN74S516.ZW.ZW16.Z = (INT16)Z;
	SN74S516.ZW.ZW16.W = (UINT16)W;
	SN74S516.ZWfl = 0;
}

static void sn74s516_update(int ins)
{
	SN74S516.state = state_table[SN74S516.state][ins];

	if (SN74S516.state == 4)
	{
		sn_multiply();
		SN74S516.state = 8;
	}
	else if (SN74S516.state == 5)
	{
		sn_divide();
		SN74S516.state = 10;
	}
}

static void kick_sn74s516(UINT16 *data, int ins)
{
#define LOAD_X		(SN74S516.X = *data)
#define LOAD_Y		(SN74S516.Y = *data)
#define LOAD_Z		(SN74S516.ZW.ZW16.Z = *data)
#define LOAD_W		(SN74S516.ZW.ZW16.W = *data)
#define READ_ZW		*data = SN74S516.ZWfl ? SN74S516.ZW.ZW16.W : SN74S516.ZW.ZW16.Z; \
			SN74S516.ZWfl ^= 1;

#define UPDATE_SEQUENCE (SN74S516.code = (SN74S516.code << 4) | ins)
#define CLEAR_SEQUENCE	(SN74S516.code = 0)

	switch (SN74S516.state)
	{
		case 0:
			CLEAR_SEQUENCE;
			UPDATE_SEQUENCE;

			if (ins < 4)
			{
				LOAD_Y;
				sn74s516_update(ins);
			}
			else if (ins == 4)
			{
				sn74s516_update(ins);
			}
			else if (ins < 7)
			{
				LOAD_X;
				sn74s516_update(ins);
			}
			else if (ins == 7)
			{
				READ_ZW;
			}
			break;

		case 8:
		case 10:
			CLEAR_SEQUENCE;
			UPDATE_SEQUENCE;

			if (ins < 4)
			{
				LOAD_Y;
				sn74s516_update(ins);
			}
			else if (ins == 4)
			{
				sn74s516_update(ins);
			}
			else if (ins == 5)
			{
				/* Rounding operation */
				sn74s516_update(ins);
			}
			else if (ins == 6)
			{
				LOAD_X;
				sn74s516_update(ins);
			}
			else if (ins == 7)
			{
				READ_ZW;
				sn74s516_update(ins);
			}
			break;

		case 1:
			/* 6666 represents an incomplete state - clear it. */
			if (SN74S516.code == 0x6666)
				CLEAR_SEQUENCE;

			UPDATE_SEQUENCE;
			if (ins < 4)
			{
				LOAD_Y;
				sn74s516_update(ins);
			}
			else if (ins < 6)
			{
				sn74s516_update(ins);
			}
			else if (ins == 6)
			{
				LOAD_Z;
				sn74s516_update(ins);
			}
			else if (ins == 7)
			{
				/* Pointless operation. */
				sn74s516_update(ins);
			}
			break;

		case 3:
			UPDATE_SEQUENCE;
			if (ins < 4)
			{
				LOAD_Y;
				sn74s516_update(ins);
			}
			else if (ins == 4)
			{
				LOAD_W;
				sn74s516_update(ins);
			}
			else if (ins == 5)
			{
				sn74s516_update(ins);
			}
			else if (ins == 6)
			{
				LOAD_W;
				sn74s516_update(ins);
			}
			else if (ins == 7)
			{
				READ_ZW;
				sn74s516_update(ins);
			}
			break;

		case 11:
			UPDATE_SEQUENCE;
			if (ins < 4)
			{
				LOAD_Y;
				sn74s516_update(ins);
			}
			else if (ins < 6)
			{
				sn74s516_update(ins);
			}
			else if (ins == 6)
			{
				sn74s516_update(ins);
			}
			else if (ins == 7)
			{
				/* 6667 = Load X, Load Z, Load W, Clear Z */
				SN74S516.ZW.ZW16.Z = 0;
				sn74s516_update(ins);
			}
			break;

		default:
			break;
	}
}


/****************************/
/*    Buggy Boy AU wiring   */
/****************************/

#define BB_INSLD	0x100
#define BB_CNTST	0x80
#define BB_RADCHG	0x20
#define BB_DSEL		0x03

enum
{
	BB_MUX_MULEN = 0x00,
	BB_MUX_PPSEN,
	BB_MUX_PSSEN,
	BB_MUX_LMSEL,
	BB_MUX_DPROE,
	BB_MUX_PPOE,
	BB_MUX_INSCL,
	BB_MUX_ILDEN
};

#define BB_SET_INS0_BIT		do { if (!(ins & 0x4) && math.i0ff) ins |= math.i0ff; } while (0)

static UINT16 get_bb_datarom_addr(void)
{
	UINT16 addr;

	addr = ((math.inslatch & 0x1c00) << 1) | (math.ppshift & 0xff);

	if ((math.inslatch >> 8) & BB_RADCHG)
		addr |= (math.ppshift & 0x0700);
	else
		addr |= (math.promaddr << 3) & 0x0700;

	return addr & 0x3fff;
}

static void buggyboy_update_state(void)
{
#define LHIEN(a)	!((a) & 0x80)
#define LLOEN(a)	!((a) & 0x40)
#define GO_EN(a)	!((a) & 0x4000)

	const UINT8 *au_rom = (const UINT8 *)memory_region(REGION_USER1);

	for (;;)
	{
		int go = 0;
		UINT16 prom_word = au_word(au_rom, AU_INS_PROM_BASE + math.promaddr);

		if (!GO_EN(math.inslatch) && GO_EN(prom_word))
			go = 1;
		else if ((GO_EN(math.inslatch) && GO_EN(prom_word)) && (LHIEN(math.inslatch) && LLOEN(prom_word)))
			go = 1;

		/* Now update the latch */
		math.inslatch = prom_word & 0x7fff;
		math.mux = (math.inslatch >> 3) & 7;

		if (math.mux == BB_MUX_INSCL)
			math.i0ff = 0;

		if (go)
		{
			int ins = math.inslatch & 7;

			BB_SET_INS0_BIT;

			if (math.mux == BB_MUX_DPROE)
			{
				UINT16 addr = get_bb_datarom_addr();
				UINT16 data = au_word(au_rom, addr);
				kick_sn74s516(&data, ins);
			}
			else if (math.mux == BB_MUX_PPOE)
			{
				kick_sn74s516(&math.ppshift, ins);
			}
			else if (LHIEN(math.inslatch) || LLOEN(math.inslatch))
			{
				UINT16 data;

				kick_sn74s516(&data, ins);

				if (LHIEN(math.inslatch) && LLOEN(math.inslatch))
				{
					math.ppshift = data;
				}
				else if (math.mux == BB_MUX_LMSEL)
				{
					if (LLOEN(math.inslatch))
					{
						math.ppshift &= 0x000f;
						math.ppshift |= data & 0xfff0;
					}
					else if (LHIEN(math.inslatch))
					{
						math.ppshift &= 0xfff0;
						math.ppshift |= data & 0x000f;
					}
				}
				else
				{
					if (LLOEN(math.inslatch))
					{
						math.ppshift &= 0x0fff;
						math.ppshift |= data & 0xf000;
					}
					else if (LHIEN(math.inslatch))
					{
						math.ppshift &= 0xf000;
						math.ppshift |= data & 0x0fff;
					}
				}
			}
			else
			{
				if (math.mux == BB_MUX_PPSEN)
				{
					kick_sn74s516(&math.ppshift, ins);
				}
				else
				{
					/* Bus pullups give 0xffff */
					UINT16 data = 0xffff;
					kick_sn74s516(&data, ins);
				}
			}
		}

		/* Handle rotation */
		if (((math.inslatch >> 8) & BB_DSEL) == 1)
			math.ppshift = ROR16(math.ppshift, 4);
		else if (((math.inslatch >> 8) & BB_DSEL) == 2)
			math.ppshift = ROL16(math.ppshift, 4);

		/* Is there another instruction in the sequence? */
		if (au_word(au_rom, AU_INS_PROM_BASE + math.promaddr) & 0x8000)
			break;
		else
			INC_PROM_ADDR;
	}
}

/* 0x3000-0x3fff: /MLPCS, /PPSEN, /PSSEN, /DPROE (word_offset: 0-0x7ff) */
static UINT16 bb_math_r(int word_offset)
{
	int offset = word_offset << 1;

	/* /MLPCS */
	if (offset < 0x400)
	{
		int ins;

		if (offset & 0x200)
		{
			ins = math.inslatch & 7;
			BB_SET_INS0_BIT;
		}
		else
		{
			ins = (offset >> 1) & 7;
		}

		kick_sn74s516(&math.retval, ins);
	}
	/* /PPSEN */
	else if (offset < 0x800)
	{
		math.retval = math.ppshift;
	}
	/* /DPROE */
	else if ((offset & 0xc00) == 0xc00)
	{
		const UINT8 *au_rom = (const UINT8 *)memory_region(REGION_USER1);
		UINT16 addr = get_bb_datarom_addr();

		math.retval = au_word(au_rom, addr);

		if (math.mux == BB_MUX_PPSEN)
			math.ppshift = math.retval;

		if (offset < 0xe00)
		{
			if (math.mux != BB_MUX_ILDEN)
			{
				INC_PROM_ADDR;
				buggyboy_update_state();
			}
		}
	}
	else
	{
		if (math.mux == BB_MUX_PPSEN)
			math.retval = math.ppshift;
		else
			math.retval = 0xffff;
	}

	if (offset & BB_INSLD)
	{
		math.promaddr = (offset << 2) & 0x1ff;
		buggyboy_update_state();
	}
	else if (offset & BB_CNTST)
	{
		INC_PROM_ADDR;
		buggyboy_update_state();
	}

	return math.retval;
}

static void bb_math_w(int word_offset, UINT16 data)
{
	int offset = word_offset << 1;

	math.cpulatch = data;

	/* /MLPCS */
	if (offset < 0x400)
	{
		int ins;

		if (offset & 0x200)
		{
			ins = math.inslatch & 7;
			BB_SET_INS0_BIT;
		}
		else
		{
			ins = (offset >> 1) & 7;
		}

		kick_sn74s516(&math.cpulatch, ins);
	}
	/* /PPSEN */
	else if ((offset & 0xc00) == 0x400)
	{
		math.ppshift = math.cpulatch;
	}
	/* /PSSEN */
	else if ((offset & 0xc00) == 0x800)
	{
		if (((math.inslatch >> 8) & BB_DSEL) == 3)
		{
			int shift;
			UINT16 val = math.ppshift;

			if (math.cpulatch & 0x3800)
			{
				shift = (math.cpulatch >> 11) & 0x7;
				while (shift)
				{
					val = ROR16(val, 1);
					shift >>= 1;
				}
			}
			else
			{
				shift = (math.cpulatch >> 7) & 0xf;
				shift = reverse_nibble((UINT8)shift);
				shift >>= 1;
				while (shift)
				{
					val = ROL16(val, 1);
					shift >>= 1;
				}
			}
			math.ppshift = val;
		}
	}

	if (offset & BB_INSLD)
	{
		math.promaddr = (offset << 2) & 0x1ff;
		buggyboy_update_state();
	}
	else if (offset & BB_CNTST)
	{
		INC_PROM_ADDR;
		buggyboy_update_state();
	}
}

/*
    /SPCS RAM (0x0800-0x0fff) and /SPCS ROM (0x5000-0x7fff): the slave
    CPU's own RAM and ROM, decoded a second time through the AU's chip
    select so that reading them also feeds math.cpulatch into the AU
    pipeline (real PCB behaviour, not a MAME modelling artifact).
*/
static UINT16 bb_spcs_apply(UINT16 data)
{
	math.cpulatch = data;

	if (math.mux == BB_MUX_ILDEN)
	{
		math.i0ff = (math.cpulatch & (1 << 14)) ? 1 : 0;
	}
	else if (math.mux == BB_MUX_MULEN)
	{
		int ins = math.inslatch & 7;
		BB_SET_INS0_BIT;
		kick_sn74s516(&math.cpulatch, ins);
	}
	else if (math.mux == BB_MUX_PPSEN)
	{
		math.ppshift = math.cpulatch;
	}
	else if (math.mux == BB_MUX_PSSEN)
	{
		if (((math.inslatch >> 8) & BB_DSEL) == 3)
		{
			int shift;
			UINT16 val = math.ppshift;

			if (math.cpulatch & 0x3800)
			{
				shift = (math.cpulatch >> 11) & 0x7;
				while (shift)
				{
					val = ROR16(val, 1);
					shift >>= 1;
				}
			}
			else
			{
				shift = (math.cpulatch >> 7) & 0xf;
				shift = reverse_nibble((UINT8)shift);
				shift >>= 1;
				while (shift)
				{
					val = ROL16(val, 1);
					shift >>= 1;
				}
			}
			math.ppshift = val;
		}
	}

	if (math.mux != BB_MUX_ILDEN)
	{
		INC_PROM_ADDR;
		buggyboy_update_state();
	}

	return math.cpulatch;
}

/* Public byte-wide handlers, wired into buggyboy_slave in drivers/tx1.c.

   The master/slave 8086s here issue 16-bit word accesses; 0.106's byte-wide
   ADDRESS_SPACE_PROGRAM (see mame106/cpu/i86/i86.h ReadWord/WriteWord) always
   splits those into two byte accesses, low byte (even offset) first, then
   high byte (odd offset) second. So: writes accumulate into a latch and only
   perform the real 16-bit operation once the high byte lands; reads perform
   the operation once (low-byte access) and cache the result for the
   following high-byte access. */

static UINT16 au_r_latch;
static UINT16 au_w_latch;

READ8_HANDLER( BB_AU_R )
{
	int word_offset = offset >> 1;

	if (!(offset & 1))
	{
		au_r_latch = bb_math_r(word_offset);
		return au_r_latch & 0xff;
	}

	return au_r_latch >> 8;
}

WRITE8_HANDLER( BB_AU_W )
{
	if (!(offset & 1))
	{
		au_w_latch = data & 0xff;
	}
	else
	{
		au_w_latch |= data << 8;
		bb_math_w(offset >> 1, au_w_latch);
	}
}

static UINT16 spcs_r_latch;

READ8_HANDLER( BB_SPCS_RAM_R )
{
	int base = offset & ~1;

	if (!(offset & 1))
	{
		UINT16 word = bb_math_ram[base] | (bb_math_ram[base + 1] << 8);
		spcs_r_latch = bb_spcs_apply(word);
		return spcs_r_latch & 0xff;
	}

	return spcs_r_latch >> 8;
}

WRITE8_HANDLER( BB_SPCS_RAM_W )
{
	bb_math_ram[offset] = data;
}

READ8_HANDLER( BB_SPCS_ROM_R )
{
	const UINT8 *rom = (const UINT8 *)memory_region(REGION_CPU2);
	int base = 0xfd000 + (offset & ~1);

	if (!(offset & 1))
	{
		UINT16 word = rom[base] | (rom[base + 1] << 8);
		spcs_r_latch = bb_spcs_apply(word);
		return spcs_r_latch & 0xff;
	}

	return spcs_r_latch >> 8;
}

void bb_math_reset(void)
{
	memset(&math, 0, sizeof(math));
}
