/***************************************************************************

                            -=  SunA 8 Bit Games =-

                    driver by   Luca Elia (l.elia@tin.it)


Main  CPU:      Encrypted Z80 (Epoxy Module)
Sound CPU:      Z80 [Music]  +  Z80 [8 Bit PCM, Optional]
Sound Chips:    AY8910  +  YM3812/YM2203  + DAC x 4 [Optional]


---------------------------------------------------------------------------
Year + Game         Game     PCB         Epoxy CPU    Notes
---------------------------------------------------------------------------
88  Hard Head       KRB-14   60138-0083  S562008      Encryption + Protection
88  Rough Ranger    K030087  ?           S562008
89  Spark Man       KRB-16   60136-081   T568009      Not Working (Protection)
90  Star Fighter    ?        ?           ?            Not Working
91  Hard Head 2     ?        ?           T568009      Encryption + Protection
92  Brick Zone      ?        ?           Yes          Not Working
---------------------------------------------------------------------------

To Do:

- Samples playing in rranger, starfigh, sparkman (AY8910 ports A&B)

Notes:

- sparkman: to get past the roms test screen put a watchpoint at ca40.
  When hit, clear ca41. Most of the garbage you'll see is probably due
  to imperfect graphics emulation (e.g. gfx banking) than protection.

- hardhea2: in test mode press P1&P2 button 2 to see a picture of each level

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"
#include "sound/2203intf.h"
#include "sound/3812intf.h"
#include "sound/dac.h"
#include "sound/samples.h"

extern int suna8_text_dim; /* specifies format of text layer */

extern UINT8 suna8_rombank, suna8_spritebank, suna8_palettebank;
extern UINT8 suna8_unknown;

/* Functions defined in vidhrdw: */

WRITE8_HANDLER( suna8_spriteram_w );			// for debug
WRITE8_HANDLER( suna8_banked_spriteram_w );	// for debug

READ8_HANDLER( suna8_banked_paletteram_r );
READ8_HANDLER( suna8_banked_spriteram_r );

WRITE8_HANDLER( suna8_banked_paletteram_w );
WRITE8_HANDLER( brickzn_banked_paletteram_w );

VIDEO_START( suna8_textdim0 );
VIDEO_START( suna8_textdim8 );
VIDEO_START( suna8_textdim12 );
VIDEO_UPDATE( suna8 );

/* Functions defined in sndhrdw: */

WRITE8_HANDLER( suna8_play_samples_w );
WRITE8_HANDLER( suna8_samples_number_w );
void suna8_sh_start(void);

/***************************************************************************


                                ROMs Decryption


***************************************************************************/

/***************************************************************************
                                Hard Head
***************************************************************************/

DRIVER_INIT( hardhead )
{
	UINT8 *rom = memory_region(REGION_CPU1);
	int i;

	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[8] =
		{
			1,1,0,1,1,1,1,0,
		};
		int table = ((i & 0x0c00) >> 10) | ((i & 0x4000) >> 12);

		if (swaptable[table])
			rom[i] = BITSWAP8(rom[i], 7,6,5,3,4,2,1,0) ^ 0x58;
	}

	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
}

/* Non encrypted bootleg */
static DRIVER_INIT( hardhedb )
{
	memory_set_decrypted_region(0, 0x0000, 0x7fff, memory_region(REGION_CPU1) + 0x48000);
	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
}

/***************************************************************************
                                Brick Zone
***************************************************************************/

/* !! BRICKZN3 !! */

static UINT8 *brickzn_decrypt(void)
{
	UINT8	*RAM	=	memory_region(REGION_CPU1);
	size_t	size	=	memory_region_length(REGION_CPU1);
	UINT8   *decrypt = auto_malloc(size);
	int i;

	memory_set_decrypted_region(0, 0x0000, 0x7fff, decrypt);

	/* Opcodes and data */
	for (i = 0; i < 0x50000; i++)
	{
		static const UINT8 opcode_swaptable[8] =
		{
			1,1,1,0,0,1,1,0,
		};
		static const UINT8 data_swaptable[16] =
		{
			1,1,1,0,0,1,1,1,1,0,1,1,1,1,1,1,
		};
		int opcode_swap = opcode_swaptable[((i & 0x00c) >> 2) | ((i & 0x040) >> 4)];
		int data_swap = (i >= 0x8000) ? 0 : data_swaptable[(i & 0x003) | ((i & 0x008) >> 1) | ((i & 0x400) >> 7)];
		UINT8 x = RAM[i];

		if (data_swap)
		{
			x = BITSWAP8(x, 7,6,5,4,3,2,0,1);
			RAM[i] = BITSWAP8(x, 7,2,3,4,5,6,1,0) ^ 0x10;
		}

		if (opcode_swap)
			x ^= 0x80;

		if (opcode_swap || data_swap)
			x = BITSWAP8(x, 7,2,3,4,5,6,1,0) ^ 0x10;

		decrypt[i] = x;
	}

	return decrypt;
}

DRIVER_INIT( brickzn )
{
	UINT8	*RAM	=	memory_region(REGION_CPU1);
	UINT8   *decrypt = brickzn_decrypt();
	int i;

	// restore opcodes which for some reason shouldn't be decrypted... */
	for (i = 0; i < 0x8000; i++)
	{
		if (	((i >= 0x0730) && (i <= 0x076f)) ||
				((i >= 0x45c5) && (i <= 0x45e4)) ||
				((i >= 0x7393) && (i <= 0x73ba)) ||
				((i >= 0x7a79) && (i <= 0x7aa9)) )
		{
			decrypt[i] = RAM[i];
		}
	}


	/* !!!!!! PATCHES !!!!!! */

	decrypt[0x3349] = 0xc9;	// RET Z -> RET (to avoid: jp $C800)

	decrypt[0x1431] = 0x00;	// HALT -> NOP (NMI source??)
	decrypt[0x24b5] = 0x00;	// HALT -> NOP
	decrypt[0x2583] = 0x00;	// HALT -> NOP

	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
	memory_configure_bank_decrypted(1, 0, 16, decrypt + 0x10000, 0x4000);
}

DRIVER_INIT( brickzn3 )
{
	UINT8	*RAM	=	memory_region(REGION_CPU1);
	UINT8   *decrypt = brickzn_decrypt();
	int i;

	// restore opcodes which for some reason shouldn't be decrypted... */
	for (i = 0; i < 0x8000; i++)
	{
		if (	((i >= 0x0730) && (i <= 0x076f)) ||
				((i >= 0x4541) && (i <= 0x4560)) ||
				((i >= 0x72f3) && (i <= 0x731a)) ||
				((i >= 0x79d9) && (i <= 0x7a09)) )
		{
			decrypt[i] = RAM[i];
		}
	}


	/* !!!!!! PATCHES !!!!!! */

	decrypt[0x3337] = 0xc9;	// RET Z -> RET (to avoid: jp $C800)

	decrypt[0x1406] = 0x00;	// HALT -> NOP (NMI source??)
	decrypt[0x2487] = 0x00;	// HALT -> NOP
	decrypt[0x256c] = 0x00;	// HALT -> NOP

	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
	memory_configure_bank_decrypted(1, 0, 16, decrypt + 0x10000, 0x4000);
}


/***************************************************************************
                                Hard Head 2
***************************************************************************/

DRIVER_INIT( hardhea2 )
{
	UINT8	*RAM	=	memory_region(REGION_CPU1);
	size_t	size	=	memory_region_length(REGION_CPU1);
	UINT8   *decrypt = 	auto_malloc(size);
	UINT8 x;
	int i;

	memory_set_decrypted_region(0, 0x0000, 0x7fff, decrypt);

	/* Address lines scrambling */
	memcpy(decrypt, RAM, size);
	for (i = 0x00000; i < 0x50000; i++)
	{
/*
0x1000 to scramble:
        dump                screen
rom10:  0y, 1y, 2n, 3n      0y,1y,2n,3n
        4n?,5n, 6n, 7n      4n,5n,6n,7n
        8?, 9n, an, bn      8n,9n,an,bn
        cy, dy, ey?,        cy,dy,en,fn
rom11:                      n
rom12:                      n
rom13:  0?, 1y, 2n, 3n      ?,?,?,? (palettes)
        4n, 5n, 6n, 7?      ?,?,n,n (intro anim)
        8?, 9n?,an, bn      y,y,?,? (player anims)
        cn, dy, en, fn      y,y,n,n
*/
		static const UINT8 swaptable[80] =
		{
			1,1,1,1,0,0,1,1,    0,0,0,0,0,0,0,0,	// 8000-ffff not used
			1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,
		};
		int addr = i;

		if (swaptable[(i & 0xff000) >> 12])
			addr = (addr & 0xf0000) | BITSWAP16(addr, 15,14,13,12,11,10,9,8,6,7,5,4,3,2,1,0);

		RAM[i] = decrypt[addr];
	}

	/* Opcodes */
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[32] =
		{
			1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,
			1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,0,
		};
		static const UINT8 xortable[32] =
		{
			0x04,0x04,0x00,0x04,0x00,0x04,0x00,0x00,0x04,0x45,0x00,0x04,0x00,0x04,0x00,0x00,
			0x04,0x45,0x00,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x04,0x00,0x00,
		};
		int table = (i & 1) | ((i & 0x400) >> 9) | ((i & 0x7000) >> 10);

		x = RAM[i];

		x = BITSWAP8(x, 7,6,5,3,4,2,1,0) ^ 0x41 ^ xortable[table];
		if (swaptable[table])
			x = BITSWAP8(x, 5,6,7,4,3,2,1,0);

		decrypt[i] = x;
	}

	/* Data */
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[8] = { 1,1,0,1,0,1,1,0 };

		if (swaptable[(i & 0x7000) >> 12])
			RAM[i] = BITSWAP8(RAM[i], 5,6,7,4,3,2,1,0) ^ 0x41;
	}

	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
	memory_configure_bank(2, 0, 2, auto_malloc(0x2000 * 2), 0x2000);
}


/***************************************************************************
                                Star Fighter
***************************************************************************/

DRIVER_INIT( starfigh )
{
	UINT8	*RAM	=	memory_region(REGION_CPU1);
	size_t	size	=	memory_region_length(REGION_CPU1);
	UINT8   *decrypt = 	auto_malloc(size);
	UINT8 x;
	int i;

	memory_set_decrypted_region(0, 0x0000, 0x7fff, decrypt);

	/* Address lines scrambling */
	memcpy(decrypt, RAM, size);
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[8] =
		{
			1,1,1,1,1,1,0,0,
		};
		int addr = i;

		if (swaptable[(i & 0x7000) >> 12])
			addr = BITSWAP16(addr, 15,14,13,12,11,10,9,8,6,7,5,4,3,2,1,0);

		RAM[i] = decrypt[addr];
	}

	/* Opcodes */
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[32] =
		{
			0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
			0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		};
		static const UINT8 xortable[32] =
		{
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x41,0x01,0x00,0x00,0x00,0x00,
			0x01,0x01,0x41,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		};
		int table = (i & 0x7c00) >> 10;

		x = RAM[i];

		x = BITSWAP8(x, 5,6,7,3,4,2,1,0) ^ 0x45 ^ xortable[table];
		if (swaptable[table])
			x = BITSWAP8(x, 5,6,7,4,3,2,1,0) ^ 0x04;

		decrypt[i] = x;
	}

	/* Data */
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[8] = { 1,1,0,1,0,1,1,0 };

		if (swaptable[(i & 0x7000) >> 12])
			RAM[i] = BITSWAP8(RAM[i], 5,6,7,4,3,2,1,0) ^ 0x45;
	}

	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
}


/***************************************************************************
                                Spark Man
***************************************************************************/

static DRIVER_INIT( sparkman )
{
	UINT8	*RAM	=	memory_region(REGION_CPU1);
	size_t	size	=	memory_region_length(REGION_CPU1);
	UINT8   *decrypt = 	auto_malloc(size);
	UINT8 x;
	int i;

	memory_set_decrypted_region(0, 0x0000, 0x7fff, decrypt);

	/* Address lines scrambling */
	memcpy(decrypt, RAM, size);
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[8] =
		{
			1,1,1,1,0,0,1,1,
		};
		int addr = i;

		if (swaptable[(i & 0x7000) >> 12])
			addr = BITSWAP16(addr, 15,14,13,12,11,10,9,7,8,6,5,4,3,2,1,0);

		RAM[i] = decrypt[addr];
	}

	/* Opcodes */
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[32] =
		{
			0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,
			0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
		};
		static const UINT8 xortable[32] =
		{
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
		};
		int table = (i & 0x7c00) >> 10;

		x = RAM[i];

		x = BITSWAP8(x, 5,6,7,3,4,2,1,0) ^ 0x44 ^ xortable[table];
		if (swaptable[table])
			x = BITSWAP8(x, 5,6,7,4,3,2,1,0) ^ 0x04;

		decrypt[i] = x;
	}

	/* Data */
	for (i = 0; i < 0x8000; i++)
	{
		static const UINT8 swaptable[8] = { 1,1,1,0,1,1,0,1 };

		if (swaptable[(i & 0x7000) >> 12])
			RAM[i] = BITSWAP8(RAM[i], 5,6,7,4,3,2,1,0) ^ 0x44;
	}

	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
}

/***************************************************************************


                                Protection


***************************************************************************/

/***************************************************************************
                                Hard Head
***************************************************************************/

static UINT8 protection_val;

static READ8_HANDLER( hardhead_protection_r )
{
	if (protection_val & 0x80)
		return	((~offset & 0x20)			?	0x20 : 0) |
				((protection_val & 0x04)	?	0x80 : 0) |
				((protection_val & 0x01)	?	0x04 : 0);
	else
		return	((~offset & 0x20)					?	0x20 : 0) |
				(((offset ^ protection_val) & 0x01)	?	0x84 : 0);
}

static WRITE8_HANDLER( hardhead_protection_w )
{
	if (data & 0x80)	protection_val = data;
	else				protection_val = offset & 1;
}


/***************************************************************************


                            Memory Maps - Main CPU


***************************************************************************/

/***************************************************************************
                                Hard Head
***************************************************************************/

static UINT8 *hardhead_ip;

static READ8_HANDLER( hardhead_ip_r )
{
	switch (*hardhead_ip)
	{
		case 0:	return readinputport(0);
		case 1:	return readinputport(1);
		case 2:	return readinputport(2);
		case 3:	return readinputport(3);
		default:
			logerror("CPU #0 - PC %04X: Unknown IP read: %02X\n",activecpu_get_pc(),*hardhead_ip);
			return 0xff;
	}
}

/*
    765- ----   Unused (eg. they go into hardhead_flipscreen_w)
    ---4 ----
    ---- 3210   ROM Bank
*/
static WRITE8_HANDLER( hardhead_bankswitch_w )
{
	int bank = data & 0x0f;

	if (data & ~0xef) 	logerror("CPU #0 - PC %04X: unknown bank bits: %02X\n",activecpu_get_pc(),data);
	memory_set_bank(1, bank);
}


/*
    765- ----
    ---4 3---   Coin Lockout
    ---- -2--   Flip Screen
    ---- --10
*/
static WRITE8_HANDLER( hardhead_flipscreen_w )
{
	flip_screen_set(    data & 0x04);
	coin_lockout_w ( 0,	data & 0x08);
	coin_lockout_w ( 1,	data & 0x10);
}

static ADDRESS_MAP_START( hardhead_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM				)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1				)	// Banked ROM
	AM_RANGE(0xc000, 0xd7ff) AM_READ(MRA8_RAM				)	// RAM
	AM_RANGE(0xd800, 0xd9ff) AM_READ(MRA8_RAM				)	// Palette
	AM_RANGE(0xda00, 0xda00) AM_READ(hardhead_ip_r			)	// Input Ports
	AM_RANGE(0xda80, 0xda80) AM_READ(soundlatch2_r			)	// From Sound CPU
	AM_RANGE(0xdd80, 0xddff) AM_READ(hardhead_protection_r	)	// Protection
	AM_RANGE(0xe000, 0xffff) AM_READ(MRA8_RAM				)	// Sprites
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhead_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM				)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_WRITE(MWA8_ROM				)	// Banked ROM
	AM_RANGE(0xc000, 0xd7ff) AM_WRITE(MWA8_RAM				)	// RAM
	AM_RANGE(0xd800, 0xd9ff) AM_WRITE(paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&paletteram	)	// Palette
	AM_RANGE(0xda00, 0xda00) AM_WRITE(MWA8_RAM) AM_BASE(&hardhead_ip	)	// Input Port Select
	AM_RANGE(0xda80, 0xda80) AM_WRITE(hardhead_bankswitch_w	)	// ROM Banking
	AM_RANGE(0xdb00, 0xdb00) AM_WRITE(soundlatch_w			)	// To Sound CPU
	AM_RANGE(0xdb80, 0xdb80) AM_WRITE(hardhead_flipscreen_w	)	// Flip Screen + Coin Lockout
	AM_RANGE(0xdc00, 0xdc00) AM_WRITE(MWA8_NOP				)	// <- R (after bank select)
	AM_RANGE(0xdc80, 0xdc80) AM_WRITE(MWA8_NOP				)	// <- R (after bank select)
	AM_RANGE(0xdd00, 0xdd00) AM_WRITE(MWA8_NOP				)	// <- R (after ip select)
	AM_RANGE(0xdd80, 0xddff) AM_WRITE(hardhead_protection_w	)	// Protection
	AM_RANGE(0xe000, 0xffff) AM_WRITE(suna8_spriteram_w) AM_BASE(&spriteram	)	// Sprites
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhead_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(MRA8_NOP	)	// ? IRQ Ack
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhead_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END


/***************************************************************************
                                Rough Ranger
***************************************************************************/

/*
    76-- ----   Coin Lockout
    --5- ----   Flip Screen
    ---4 ----   ROM Bank
    ---- 3---
    ---- -210   ROM Bank
*/
static WRITE8_HANDLER( rranger_bankswitch_w )
{
	int bank = data & 0x07;
	if ((~data & 0x10) && (bank >= 4))	bank += 4;

	if (data & ~0xf7) 	logerror("CPU #0 - PC %04X: unknown bank bits: %02X\n",activecpu_get_pc(),data);

	memory_set_bank(1, bank);

	flip_screen_set(    data & 0x20);
	coin_lockout_w ( 0,	data & 0x40);
	coin_lockout_w ( 1,	data & 0x80);
}

/*
    7--- ----   1 -> Garbled title (another romset?)
    -654 ----
    ---- 3---   1 -> No sound (soundlatch full?)
    ---- -2--
    ---- --1-   1 -> Interlude screens
    ---- ---0
*/
static READ8_HANDLER( rranger_soundstatus_r )
{
	return 0x02;
}

static ADDRESS_MAP_START( rranger_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM				)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1				)	// Banked ROM
	AM_RANGE(0xc000, 0xc000) AM_READ(watchdog_reset_r		)	// Watchdog (Tested!)
	AM_RANGE(0xc002, 0xc002) AM_READ(input_port_0_r		)	// P1 (Inputs)
	AM_RANGE(0xc003, 0xc003) AM_READ(input_port_1_r		)	// P2
	AM_RANGE(0xc004, 0xc004) AM_READ(rranger_soundstatus_r	)	// Latch Status?
	AM_RANGE(0xc200, 0xc200) AM_READ(MRA8_NOP				)	// Protection?
	AM_RANGE(0xc280, 0xc280) AM_READ(input_port_2_r		)	// DSW 1
	AM_RANGE(0xc2c0, 0xc2c0) AM_READ(input_port_3_r		)	// DSW 2
	AM_RANGE(0xc600, 0xc7ff) AM_READ(MRA8_RAM				)	// Palette
	AM_RANGE(0xc800, 0xdfff) AM_READ(MRA8_RAM				)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_READ(MRA8_RAM				)	// Sprites
ADDRESS_MAP_END

static ADDRESS_MAP_START( rranger_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM				)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_WRITE(MWA8_ROM				)	// Banked ROM
	AM_RANGE(0xc000, 0xc000) AM_WRITE(soundlatch_w			)	// To Sound CPU
	AM_RANGE(0xc002, 0xc002) AM_WRITE(rranger_bankswitch_w	)	// ROM Banking
	AM_RANGE(0xc200, 0xc200) AM_WRITE(MWA8_NOP				)	// Protection?
	AM_RANGE(0xc280, 0xc280) AM_WRITE(MWA8_NOP				)	// ? NMI Ack
	AM_RANGE(0xc600, 0xc7ff) AM_WRITE(paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&paletteram	)	// Palette
	AM_RANGE(0xc800, 0xdfff) AM_WRITE(MWA8_RAM				)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_WRITE(suna8_spriteram_w) AM_BASE(&spriteram	)	// Sprites
ADDRESS_MAP_END

static ADDRESS_MAP_START( rranger_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(MRA8_NOP	)	// ? IRQ Ack
ADDRESS_MAP_END

static ADDRESS_MAP_START( rranger_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END


/***************************************************************************
                                Brick Zone
***************************************************************************/

/*
?
*/
static READ8_HANDLER( brickzn_c140_r )
{
	return 0xff;
}

/*
*/
static WRITE8_HANDLER( brickzn_palettebank_w )
{
	suna8_palettebank = (data >> 1) & 1;
	if (data & ~0x02) 	logerror("CPU #0 - PC %04X: unknown palettebank bits: %02X\n",activecpu_get_pc(),data);

	/* Also used as soundlatch - depending on c0c0? */
	soundlatch_w(0,data);
}

/*
    7654 32--
    ---- --1-   Ram Bank
    ---- ---0   Flip Screen
*/
static WRITE8_HANDLER( brickzn_spritebank_w )
{
	suna8_spritebank = (data >> 1) & 1;
	if (data & ~0x03) 	logerror("CPU #0 - PC %04X: unknown spritebank bits: %02X\n",activecpu_get_pc(),data);
	flip_screen_set( data & 0x01 );
}

static WRITE8_HANDLER( brickzn_unknown_w )
{
	suna8_unknown = data;
}

/*
    7654 ----
    ---- 3210   ROM Bank
*/
static WRITE8_HANDLER( brickzn_rombank_w )
{
	int bank = data & 0x0f;

	if (data & ~0x0f) 	logerror("CPU #0 - PC %04X: unknown rom bank bits: %02X\n",activecpu_get_pc(),data);

	memory_set_bank(1, bank);
	suna8_rombank = data;
}

static ADDRESS_MAP_START( brickzn_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM					)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1					)	// Banked ROM
	AM_RANGE(0xc100, 0xc100) AM_READ(input_port_0_r			)	// P1 (Buttons)
	AM_RANGE(0xc101, 0xc101) AM_READ(input_port_1_r			)	// P2
	AM_RANGE(0xc102, 0xc102) AM_READ(input_port_2_r			)	// DSW 1
	AM_RANGE(0xc103, 0xc103) AM_READ(input_port_3_r			)	// DSW 2
	AM_RANGE(0xc108, 0xc108) AM_READ(input_port_4_r			)	// P1 (Analog)
	AM_RANGE(0xc10c, 0xc10c) AM_READ(input_port_5_r			)	// P2
	AM_RANGE(0xc140, 0xc140) AM_READ(brickzn_c140_r			)	// ???
	AM_RANGE(0xc600, 0xc7ff) AM_READ(suna8_banked_paletteram_r	)	// Palette (Banked)
	AM_RANGE(0xc800, 0xdfff) AM_READ(MRA8_RAM					)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_READ(suna8_banked_spriteram_r	)	// Sprites (Banked)
ADDRESS_MAP_END

static ADDRESS_MAP_START( brickzn_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM						)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_WRITE(MWA8_ROM						)	// Banked ROM
	AM_RANGE(0xc040, 0xc040) AM_WRITE(brickzn_rombank_w				)	// ROM Bank
	AM_RANGE(0xc060, 0xc060) AM_WRITE(brickzn_spritebank_w			)	// Sprite  RAM Bank + Flip Screen
	AM_RANGE(0xc0a0, 0xc0a0) AM_WRITE(brickzn_palettebank_w			)	// Palette RAM Bank + ?
	AM_RANGE(0xc0c0, 0xc0c0) AM_WRITE(brickzn_unknown_w				)	// ???
	AM_RANGE(0xc600, 0xc7ff) AM_WRITE(brickzn_banked_paletteram_w	)	// Palette (Banked)
	AM_RANGE(0xc800, 0xdfff) AM_WRITE(MWA8_RAM						)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_WRITE(suna8_banked_spriteram_w		)	// Sprites (Banked)
ADDRESS_MAP_END

static ADDRESS_MAP_START( brickzn_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( brickzn_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END


/***************************************************************************
                                Hard Head 2
***************************************************************************/

static UINT8 suna8_nmi_enable;

/* Probably wrong: */
static WRITE8_HANDLER( hardhea2_nmi_w )
{
	suna8_nmi_enable = data & 0x01;
//  if (data & ~0x01)   logerror("CPU #0 - PC %04X: unknown nmi bits: %02X\n",activecpu_get_pc(),data);
}

/*
    7654 321-
    ---- ---0   Flip Screen
*/
static WRITE8_HANDLER( hardhea2_flipscreen_w )
{
	flip_screen_set(data & 0x01);
	if (data & ~0x01) 	logerror("CPU #0 - PC %04X: unknown flipscreen bits: %02X\n",activecpu_get_pc(),data);
}

WRITE8_HANDLER( hardhea2_leds_w )
{
	set_led_status(0, data & 0x01);
	set_led_status(1, data & 0x02);
	coin_counter_w(0, data & 0x04);
	if (data & ~0x07)	logerror("CPU#0  - PC %06X: unknown leds bits: %02X\n",activecpu_get_pc(),data);
}

/*
    7654 32--
    ---- --1-   Ram Bank
    ---- ---0   Ram Bank?
*/
static WRITE8_HANDLER( hardhea2_spritebank_w )
{
	suna8_spritebank = (data >> 1) & 1;
	if (data & ~0x02) 	logerror("CPU #0 - PC %04X: unknown spritebank bits: %02X\n",activecpu_get_pc(),data);
}

/*
    7654 ----
    ---- 3210   ROM Bank
*/
static WRITE8_HANDLER( hardhea2_rombank_w )
{
	int bank = data & 0x0f;

	if (data & ~0x0f) 	logerror("CPU #0 - PC %04X: unknown rom bank bits: %02X\n",activecpu_get_pc(),data);

	memory_set_bank(1, bank);
	suna8_rombank = data;
}

static WRITE8_HANDLER( hardhea2_spritebank_0_w )
{
	suna8_spritebank = 0;
}
static WRITE8_HANDLER( hardhea2_spritebank_1_w )
{
	suna8_spritebank = 1;
}

static WRITE8_HANDLER( hardhea2_rambank_0_w )
{
	memory_set_bank(2, 0);
}
static WRITE8_HANDLER( hardhea2_rambank_1_w )
{
	memory_set_bank(2, 1);
}


static ADDRESS_MAP_START( hardhea2_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM						)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1						)	// Banked ROM
	AM_RANGE(0xc000, 0xc000) AM_READ(input_port_0_r					)	// P1 (Inputs)
	AM_RANGE(0xc001, 0xc001) AM_READ(input_port_1_r					)	// P2
	AM_RANGE(0xc002, 0xc002) AM_READ(input_port_2_r					)	// DSW 1
	AM_RANGE(0xc003, 0xc003) AM_READ(input_port_3_r					)	// DSW 2
	AM_RANGE(0xc080, 0xc080) AM_READ(input_port_4_r					)	// vblank?
	AM_RANGE(0xc600, 0xc7ff) AM_READ(paletteram_r					)	// Palette (Banked??)
	AM_RANGE(0xc800, 0xdfff) AM_READ(MRA8_BANK2						)	// RAM (Banked?)
	AM_RANGE(0xe000, 0xffff) AM_READ(suna8_banked_spriteram_r		)	// Sprites (Banked)
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhea2_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM					)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_WRITE(MWA8_ROM					)	// Banked ROM
	AM_RANGE(0xc200, 0xc200) AM_WRITE(hardhea2_spritebank_w		)	// Sprite RAM Bank
	AM_RANGE(0xc280, 0xc280) AM_WRITE(hardhea2_rombank_w			)	// ROM Bank (?mirrored up to c2ff?)

	// *** Protection
	AM_RANGE(0xc28c, 0xc28c) AM_WRITE(hardhea2_rombank_w		)
	// Protection ***

	AM_RANGE(0xc300, 0xc300) AM_WRITE(hardhea2_flipscreen_w		)	// Flip Screen
	AM_RANGE(0xc380, 0xc380) AM_WRITE(hardhea2_nmi_w				)	// ? NMI related ?
	AM_RANGE(0xc400, 0xc400) AM_WRITE(hardhea2_leds_w				)	// Leds + Coin Counter
	AM_RANGE(0xc480, 0xc480) AM_WRITE(MWA8_NOP					)	// ~ROM Bank
	AM_RANGE(0xc500, 0xc500) AM_WRITE(soundlatch_w				)	// To Sound CPU

	// *** Protection
	AM_RANGE(0xc50f, 0xc50f) AM_WRITE(hardhea2_spritebank_1_w )
	AM_RANGE(0xc508, 0xc508) AM_WRITE(hardhea2_spritebank_0_w )

	AM_RANGE(0xc507, 0xc507) AM_WRITE(hardhea2_rambank_1_w )
	AM_RANGE(0xc522, 0xc522) AM_WRITE(hardhea2_rambank_0_w )

	AM_RANGE(0xc556, 0xc556) AM_WRITE(hardhea2_rambank_1_w )
	AM_RANGE(0xc528, 0xc528) AM_WRITE(hardhea2_rambank_0_w )

	AM_RANGE(0xc560, 0xc560) AM_WRITE(hardhea2_rambank_1_w )
	AM_RANGE(0xc533, 0xc533) AM_WRITE(hardhea2_rambank_0_w )
	// Protection ***

	AM_RANGE(0xc600, 0xc7ff) AM_WRITE(paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&paletteram	)	// Palette (Banked??)
	AM_RANGE(0xc800, 0xdfff) AM_WRITE(MWA8_BANK2					)	// RAM (Banked?)
	AM_RANGE(0xe000, 0xffff) AM_WRITE(suna8_banked_spriteram_w	)	// Sprites (Banked)
ADDRESS_MAP_END


/***************************************************************************
                                Star Fighter
***************************************************************************/

static UINT8 spritebank_latch;
static WRITE8_HANDLER( starfigh_spritebank_latch_w )
{
	spritebank_latch = (data >> 2) & 1;
	if (data & ~0x04) 	logerror("CPU #0 - PC %04X: unknown spritebank bits: %02X\n",activecpu_get_pc(),data);
}

static WRITE8_HANDLER( starfigh_spritebank_w )
{
	suna8_spritebank = spritebank_latch;
}

static ADDRESS_MAP_START( starfigh_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM					)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1					)	// Banked ROM
	AM_RANGE(0xc000, 0xc000) AM_READ(input_port_0_r			)	// P1 (Inputs)
	AM_RANGE(0xc001, 0xc001) AM_READ(input_port_1_r			)	// P2
	AM_RANGE(0xc002, 0xc002) AM_READ(input_port_2_r			)	// DSW 1
	AM_RANGE(0xc003, 0xc003) AM_READ(input_port_3_r			)	// DSW 2
	AM_RANGE(0xc600, 0xc7ff) AM_READ(suna8_banked_paletteram_r	)	// Palette (Banked??)
	AM_RANGE(0xc800, 0xdfff) AM_READ(MRA8_RAM					)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_READ(suna8_banked_spriteram_r	)	// Sprites (Banked)
ADDRESS_MAP_END

static ADDRESS_MAP_START( starfigh_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM						)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_WRITE(MWA8_ROM						)	// Banked ROM
	AM_RANGE(0xc200, 0xc200) AM_WRITE(starfigh_spritebank_w			)	// Sprite RAM Bank
	AM_RANGE(0xc380, 0xc3ff) AM_WRITE(starfigh_spritebank_latch_w	)	// Sprite RAM Bank
	AM_RANGE(0xc280, 0xc280) AM_WRITE(hardhea2_rombank_w			)	// ROM Bank (?mirrored up to c2ff?)
	AM_RANGE(0xc300, 0xc300) AM_WRITE(hardhea2_flipscreen_w			)	// Flip Screen
	AM_RANGE(0xc400, 0xc400) AM_WRITE(hardhea2_leds_w				)	// Leds + Coin Counter
	AM_RANGE(0xc500, 0xc500) AM_WRITE(soundlatch_w					)	// To Sound CPU
	AM_RANGE(0xc600, 0xc7ff) AM_WRITE(paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&paletteram	)	// Palette (Banked??)
	AM_RANGE(0xc800, 0xdfff) AM_WRITE(MWA8_RAM						)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_WRITE(suna8_banked_spriteram_w		)	// Sprites (Banked)
ADDRESS_MAP_END

static ADDRESS_MAP_START( starfigh_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( starfigh_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END


/***************************************************************************
                                Spark Man
***************************************************************************/

/* Probably wrong: */
static WRITE8_HANDLER( sparkman_nmi_w )
{
	suna8_nmi_enable = data & 0x01;
	if (data & ~0x01) 	logerror("CPU #0 - PC %04X: unknown nmi bits: %02X\n",activecpu_get_pc(),data);
}

/*
    7654 321-
    ---- ---0   Flip Screen
*/
static WRITE8_HANDLER( sparkman_flipscreen_w )
{
	flip_screen_set(data & 0x01);
	if (data & ~0x01) 	logerror("CPU #0 - PC %04X: unknown flipscreen bits: %02X\n",activecpu_get_pc(),data);
}

WRITE8_HANDLER( sparkman_leds_w )
{
	set_led_status(0, data & 0x01);
	set_led_status(1, data & 0x02);
	coin_counter_w(0, data & 0x04);
	if (data & ~0x07)	logerror("CPU#0  - PC %06X: unknown leds bits: %02X\n",activecpu_get_pc(),data);
}

/*
    7654 32--
    ---- --1-   Ram Bank
    ---- ---0   Ram Bank?
*/
static WRITE8_HANDLER( sparkman_spritebank_w )
{
	suna8_spritebank = (data >> 1) & 1;
	if (data & ~0x02) 	logerror("CPU #0 - PC %04X: unknown spritebank bits: %02X\n",activecpu_get_pc(),data);
}

/*
    7654 ----
    ---- 3210   ROM Bank
*/
static WRITE8_HANDLER( sparkman_rombank_w )
{
	int bank = data & 0x0f;

	if (data & ~0x0f) 	logerror("CPU #0 - PC %04X: unknown rom bank bits: %02X\n",activecpu_get_pc(),data);

	memory_set_bank(1, bank);
	suna8_rombank = data;
}

static READ8_HANDLER( sparkman_c0a3_r )
{
	return (cpu_getcurrentframe() & 1) ? 0x80 : 0;
}

static ADDRESS_MAP_START( sparkman_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM					)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1					)	// Banked ROM
	AM_RANGE(0xc000, 0xc000) AM_READ(input_port_0_r			)	// P1 (Inputs)
	AM_RANGE(0xc001, 0xc001) AM_READ(input_port_1_r			)	// P2
	AM_RANGE(0xc002, 0xc002) AM_READ(input_port_2_r			)	// DSW 1
	AM_RANGE(0xc003, 0xc003) AM_READ(input_port_3_r			)	// DSW 2
	AM_RANGE(0xc080, 0xc080) AM_READ(input_port_4_r			)	// Buttons
	AM_RANGE(0xc0a3, 0xc0a3) AM_READ(sparkman_c0a3_r			)	// ???
	AM_RANGE(0xc600, 0xc7ff) AM_READ(paletteram_r				)	// Palette (Banked??)
	AM_RANGE(0xc800, 0xdfff) AM_READ(MRA8_RAM					)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_READ(suna8_banked_spriteram_r	)	// Sprites (Banked)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sparkman_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM					)	// ROM
	AM_RANGE(0x8000, 0xbfff) AM_WRITE(MWA8_ROM					)	// Banked ROM
	AM_RANGE(0xc200, 0xc200) AM_WRITE(sparkman_spritebank_w		)	// Sprite RAM Bank
	AM_RANGE(0xc280, 0xc280) AM_WRITE(sparkman_rombank_w		)	// ROM Bank (?mirrored up to c2ff?)
	AM_RANGE(0xc300, 0xc300) AM_WRITE(sparkman_flipscreen_w		)	// Flip Screen
	AM_RANGE(0xc380, 0xc380) AM_WRITE(sparkman_nmi_w			)	// ? NMI related ?
	AM_RANGE(0xc400, 0xc400) AM_WRITE(sparkman_leds_w			)	// Leds + Coin Counter
	AM_RANGE(0xc500, 0xc500) AM_WRITE(soundlatch_w				)	// To Sound CPU
	AM_RANGE(0xc600, 0xc7ff) AM_WRITE(paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&paletteram	)	// Palette (Banked??)
	AM_RANGE(0xc800, 0xdfff) AM_WRITE(MWA8_RAM					)	// RAM
	AM_RANGE(0xe000, 0xffff) AM_WRITE(suna8_banked_spriteram_w	)	// Sprites (Banked)
ADDRESS_MAP_END


/***************************************************************************


                            Memory Maps - Sound CPU(s)


***************************************************************************/

/***************************************************************************
                                Hard Head
***************************************************************************/

static ADDRESS_MAP_START( hardhead_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM					)	// ROM
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM					)	// RAM
	AM_RANGE(0xc800, 0xc800) AM_READ(YM3812_status_port_0_r 	)	// ? unsure
	AM_RANGE(0xd800, 0xd800) AM_READ(soundlatch_r				)	// From Main CPU
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhead_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM					)	// ROM
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM					)	// RAM
	AM_RANGE(0xd000, 0xd000) AM_WRITE(soundlatch2_w				)	//
	AM_RANGE(0xa000, 0xa000) AM_WRITE(YM3812_control_port_0_w	)	// YM3812
	AM_RANGE(0xa001, 0xa001) AM_WRITE(YM3812_write_port_0_w		)
	AM_RANGE(0xa002, 0xa002) AM_WRITE(AY8910_control_port_0_w	)	// AY8910
	AM_RANGE(0xa003, 0xa003) AM_WRITE(AY8910_write_port_0_w		)
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhead_sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x01, 0x01) AM_READ(MRA8_NOP	)	// ? IRQ Ack
ADDRESS_MAP_END

static ADDRESS_MAP_START( hardhead_sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END



/***************************************************************************
                                Rough Ranger
***************************************************************************/

static ADDRESS_MAP_START( rranger_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM					)	// ROM
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM					)	// RAM
	AM_RANGE(0xd800, 0xd800) AM_READ(soundlatch_r				)	// From Main CPU
ADDRESS_MAP_END

static ADDRESS_MAP_START( rranger_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM					)	// ROM
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM					)	// RAM
	AM_RANGE(0xd000, 0xd000) AM_WRITE(soundlatch2_w				)	//
	AM_RANGE(0xa000, 0xa000) AM_WRITE(YM2203_control_port_0_w	)	// YM2203
	AM_RANGE(0xa001, 0xa001) AM_WRITE(YM2203_write_port_0_w		)
	AM_RANGE(0xa002, 0xa002) AM_WRITE(YM2203_control_port_1_w	)	// AY8910
	AM_RANGE(0xa003, 0xa003) AM_WRITE(YM2203_write_port_1_w		)
ADDRESS_MAP_END

static ADDRESS_MAP_START( rranger_sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( rranger_sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END


/***************************************************************************
                                Brick Zone
***************************************************************************/

static ADDRESS_MAP_START( brickzn_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READ(MRA8_ROM					)	// ROM
	AM_RANGE(0xe000, 0xe7ff) AM_READ(MRA8_RAM					)	// RAM
	AM_RANGE(0xf800, 0xf800) AM_READ(soundlatch_r				)	// From Main CPU
ADDRESS_MAP_END

static ADDRESS_MAP_START( brickzn_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM					)	// ROM
	AM_RANGE(0xc000, 0xc000) AM_WRITE(YM3812_control_port_0_w	)	// YM3812
	AM_RANGE(0xc001, 0xc001) AM_WRITE(YM3812_write_port_0_w		)
	AM_RANGE(0xc002, 0xc002) AM_WRITE(AY8910_control_port_0_w	)	// AY8910
	AM_RANGE(0xc003, 0xc003) AM_WRITE(AY8910_write_port_0_w		)
	AM_RANGE(0xe000, 0xe7ff) AM_WRITE(MWA8_RAM					)	// RAM
	AM_RANGE(0xf000, 0xf000) AM_WRITE(soundlatch2_w				)	// To PCM CPU
ADDRESS_MAP_END

static ADDRESS_MAP_START( brickzn_sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( brickzn_sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END


/* PCM Z80 , 4 DACs (4 bits per sample), NO RAM !! */

static ADDRESS_MAP_START( brickzn_pcm_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xffff) AM_READ(MRA8_ROM	)	// ROM
ADDRESS_MAP_END
static ADDRESS_MAP_START( brickzn_pcm_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xffff) AM_WRITE(MWA8_ROM	)	// ROM
ADDRESS_MAP_END


static WRITE8_HANDLER( brickzn_pcm_w )
{
	DAC_signed_data_w( offset, (data & 0xf) * 0x11 );
}

static ADDRESS_MAP_START( brickzn_pcm_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(soundlatch2_r		)	// From Sound CPU
ADDRESS_MAP_END
static ADDRESS_MAP_START( brickzn_pcm_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x03) AM_WRITE(brickzn_pcm_w			)	// 4 x DAC
ADDRESS_MAP_END



/***************************************************************************


                                Input Ports


***************************************************************************/

#define JOY(_n_) \
	PORT_BIT(  0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_PLAYER(_n_) \
	PORT_BIT(  0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(_n_) \
	PORT_BIT(  0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(_n_) \
	PORT_BIT(  0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(_n_) \
	PORT_BIT(  0x10, IP_ACTIVE_LOW, IPT_BUTTON1        ) PORT_PLAYER(_n_) \
	PORT_BIT(  0x20, IP_ACTIVE_LOW, IPT_BUTTON2        ) PORT_PLAYER(_n_) \
	PORT_BIT(  0x40, IP_ACTIVE_LOW, IPT_START##_n_ ) \
	PORT_BIT(  0x80, IP_ACTIVE_LOW, IPT_COIN##_n_  )

/***************************************************************************
                                Hard Head
***************************************************************************/

INPUT_PORTS_START( hardhead )

	PORT_START_TAG("IN0")	// Player 1 - $da00 (ip = 0)
	JOY(1)

	PORT_START_TAG("IN1")	// Player 2 - $da00 (ip = 1)
	JOY(2)

	PORT_START_TAG("IN2")	// DSW 1 - $da00 (ip = 2)
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0e, 0x0e, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x0e, "No Bonus" )
	PORT_DIPSETTING(    0x0c, "10K" )
	PORT_DIPSETTING(    0x0a, "20K" )
	PORT_DIPSETTING(    0x08, "50K" )
	PORT_DIPSETTING(    0x06, "50K, Every 50K" )
	PORT_DIPSETTING(    0x04, "100K, Every 50K" )
	PORT_DIPSETTING(    0x02, "100K, Every 100K" )
	PORT_DIPSETTING(    0x00, "200K, Every 100K" )
	PORT_DIPNAME( 0x70, 0x70, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x80, 0x80, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("IN3") //DSW 2 - $da00 (ip = 3)
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x04, "Play Together" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x18, "2" )
	PORT_DIPSETTING(    0x10, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0xa0, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x80, "Moderate" )
	PORT_DIPSETTING(    0x60, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END

/***************************************************************************
                                Rough Ranger
***************************************************************************/

INPUT_PORTS_START( rranger )

	PORT_START_TAG("IN0")	// Player 1 - $c002
	JOY(1)

	PORT_START_TAG("IN1") // Player 2 - $c003
	JOY(2)

	PORT_START_TAG("IN2") //DSW 1 - $c280
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "10K" )
	PORT_DIPSETTING(    0x28, "30K" )
	PORT_DIPSETTING(    0x20, "50K" )
	PORT_DIPSETTING(    0x18, "50K, Every 50K" )
	PORT_DIPSETTING(    0x10, "100K, Every 50K" )
	PORT_DIPSETTING(    0x08, "100K, Every 100K" )
	PORT_DIPSETTING(    0x00, "100K, Every 200K" )
	PORT_DIPSETTING(    0x38, DEF_STR( None ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START_TAG("IN3") // DSW 2 - $c2c0
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x04, "Play Together" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x30, "2" )
	PORT_DIPSETTING(    0x20, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

INPUT_PORTS_END


/***************************************************************************
                                Brick Zone
***************************************************************************/

INPUT_PORTS_START( brickzn )

	PORT_START_TAG("IN0") // Player 1 - $c100
	JOY(1)

	PORT_START_TAG("IN1") // Player 2 - $c101
	JOY(2)

	PORT_START_TAG("IN2") // DSW 1 - $c102
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )	// rom 38:b840
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x38, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0x28, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, "Moderate" )
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
//  PORT_DIPNAME( 0x40, 0x40, "Invulnerability (Cheat)")
//  PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
//  PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE(       0x40, IP_ACTIVE_LOW )	// + Invulnerability
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("IN3") // DSW 2 - $c103
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x04, "Play Together" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "10K" )
	PORT_DIPSETTING(    0x28, "30K" )
	PORT_DIPSETTING(    0x18, "50K, Every 50K" )
	PORT_DIPSETTING(    0x20, "50K" )
	PORT_DIPSETTING(    0x10, "100K, Every 50K" )
	PORT_DIPSETTING(    0x08, "100K, Every 100K" )
	PORT_DIPSETTING(    0x00, "200K, Every 100K" )
	PORT_DIPSETTING(    0x38, DEF_STR( None ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPSETTING(    0x00, "5" )

	PORT_START_TAG("IN4") // Player 1 - $c108
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(0) PORT_REVERSE

	PORT_START_TAG("IN5") // Player 2 - $c10c
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(0) PORT_REVERSE

INPUT_PORTS_END


/***************************************************************************
                        Hard Head 2 / Star Fighter
***************************************************************************/

INPUT_PORTS_START( hardhea2 )

	PORT_START_TAG("IN0") // Player 1 - $c000
	JOY(1)

	PORT_START_TAG("IN1") // Player 2 - $c001
	JOY(2)

	PORT_START_TAG("IN2") // DSW 1 - $c002
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x18, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x38, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0x28, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, "Moderate" )
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_SERVICE(       0x40, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("IN3") // DSW 2 - $c003
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x04, "Play Together" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "10K" )
	PORT_DIPSETTING(    0x28, "30K" )
	PORT_DIPSETTING(    0x18, "50K, Every 50K" )
	PORT_DIPSETTING(    0x20, "50K" )
	PORT_DIPSETTING(    0x10, "100K, Every 50K" )
	PORT_DIPSETTING(    0x08, "100K, Every 100K" )
	PORT_DIPSETTING(    0x00, "200K, Every 100K" )
	PORT_DIPSETTING(    0x38, DEF_STR( None ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPSETTING(    0x00, "5" )

	PORT_START_TAG("IN4") // Buttons - $c080
	PORT_BIT(  0x01, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT(  0x02, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT(  0x04, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT(  0x08, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT(  0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT(  0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT(  0x40, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT(  0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

INPUT_PORTS_END


/***************************************************************************
                                Spark Man
***************************************************************************/

INPUT_PORTS_START( sparkman )

	PORT_START_TAG("IN0") // Player 1 - $c000
	JOY(1)

	PORT_START_TAG("IN1") // Player 2 - $c001
	JOY(2)

	PORT_START_TAG("IN2") // DSW 1 - $c002
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0x28, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x38, "Moderate" )
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_SERVICE(       0x40, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("IN3") // DSW 2 - $c003
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x04, "Play Together" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x38, "10K" )
	PORT_DIPSETTING(    0x28, "30K" )
	PORT_DIPSETTING(    0x18, "50K, Every 50K" )
	PORT_DIPSETTING(    0x20, "50K" )
	PORT_DIPSETTING(    0x10, "100K, Every 50K" )
	PORT_DIPSETTING(    0x08, "100K, Every 100K" )
	PORT_DIPSETTING(    0x00, "200K, Every 100K" )
	PORT_DIPSETTING(    0x30, DEF_STR( None ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPSETTING(    0x00, "5" )

	PORT_START_TAG("IN4") // Buttons - $c080
	PORT_BIT(  0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT(  0x02, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT(  0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

INPUT_PORTS_END


/***************************************************************************


                                Graphics Layouts


***************************************************************************/

/* 8x8x4 tiles (2 bitplanes per ROM) */
static const gfx_layout layout_8x8x4 =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2) + 0, RGN_FRAC(1,2) + 4, 0, 4 },
	{ 3,2,1,0, 11,10,9,8},
	{ STEP8(0,16) },
	8*8*2
};

static const gfx_decode suna8_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_8x8x4, 0, 16 }, // [0] Sprites
	{ -1 }
};



/***************************************************************************


                                Machine Drivers


***************************************************************************/

static void soundirq(int state)
{
	cpunum_set_input_line(1, 0, state);
}

/* In games with only 2 CPUs, port A&B of the AY8910 are used
   for sample playing. */

/***************************************************************************
                                Hard Head
***************************************************************************/

/* 1 x 24 MHz crystal */

static struct AY8910interface hardhead_ay8910_interface =
{
	0,
	0,
	suna8_play_samples_w,
	suna8_samples_number_w
};

static struct Samplesinterface custom_interface =
{
	1,
	NULL,
	suna8_sh_start
};

static MACHINE_DRIVER_START( hardhead )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 24000000 / 4)			/* ? */
	MDRV_CPU_PROGRAM_MAP(hardhead_readmem,hardhead_writemem)
	MDRV_CPU_IO_MAP(hardhead_readport,hardhead_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)	/* No NMI */

	MDRV_CPU_ADD(Z80, 24000000 / 4)
	/* audio CPU */					/* ? */
	MDRV_CPU_PROGRAM_MAP(hardhead_sound_readmem,hardhead_sound_writemem)
	MDRV_CPU_IO_MAP(hardhead_sound_readport,hardhead_sound_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)	/* No NMI */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(suna8_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(suna8_textdim12)
	MDRV_VIDEO_UPDATE(suna8)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(AY8910, 24000000 / 16)
	MDRV_SOUND_CONFIG(hardhead_ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.30)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.30)

	MDRV_SOUND_ADD(SAMPLES, 0)
	MDRV_SOUND_CONFIG(custom_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END



/***************************************************************************
                                Rough Ranger
***************************************************************************/

/* 1 x 24 MHz crystal */

/* 2203 + 8910 */
static MACHINE_DRIVER_START( rranger )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 24000000 / 4)					/* ? */
	MDRV_CPU_PROGRAM_MAP(rranger_readmem,rranger_writemem)
	MDRV_CPU_IO_MAP(rranger_readport,rranger_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)	/* IRQ & NMI ! */

	MDRV_CPU_ADD(Z80, 24000000 / 4)
	/* audio CPU */					/* ? */
	MDRV_CPU_PROGRAM_MAP(rranger_sound_readmem,rranger_sound_writemem)
	MDRV_CPU_IO_MAP(rranger_sound_readport,rranger_sound_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)	/* NMI = retn */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(suna8_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(suna8_textdim8)
	MDRV_VIDEO_UPDATE(suna8)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2203, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)

	MDRV_SOUND_ADD(YM2203, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END


/***************************************************************************
                                Brick Zone
***************************************************************************/

/* 1 x 24 MHz crystal */

static struct YM3812interface brickzn_ym3812_interface =
{
	soundirq	/* IRQ Line */
};

INTERRUPT_GEN( brickzn_interrupt )
{
	if (cpu_getiloops()) cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	else				 cpunum_set_input_line(0, 0, HOLD_LINE);
}

static MACHINE_DRIVER_START( brickzn )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", Z80, 24000000 / 4)		/* SUNA PROTECTION BLOCK */
	MDRV_CPU_PROGRAM_MAP(brickzn_readmem,brickzn_writemem)
	MDRV_CPU_IO_MAP(brickzn_readport,brickzn_writeport)
//  MDRV_CPU_VBLANK_INT(brickzn_interrupt, 2)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)	// nmi breaks ramtest but is needed!

	MDRV_CPU_ADD_TAG("sound", Z80, 24000000 / 4)	/* Z0840006PSC */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(brickzn_sound_readmem,brickzn_sound_writemem)
	MDRV_CPU_IO_MAP(brickzn_sound_readport,brickzn_sound_writeport)

	MDRV_CPU_ADD_TAG("pcm", Z80, 24000000 / 4)	/* Z0840006PSC */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(brickzn_pcm_readmem,brickzn_pcm_writemem)
	MDRV_CPU_IO_MAP(brickzn_pcm_readport,brickzn_pcm_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)	// we're using IPT_VBLANK

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(suna8_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(512)

	MDRV_VIDEO_START(suna8_textdim0)
	MDRV_VIDEO_UPDATE(suna8)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_CONFIG(brickzn_ym3812_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(AY8910, 24000000 / 16)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.33)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.33)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.17)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.17)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.17)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.17)
MACHINE_DRIVER_END


/***************************************************************************
                                Hard Head 2
***************************************************************************/

/* 1 x 24 MHz crystal */

INTERRUPT_GEN( hardhea2_interrupt )
{
	if (cpu_getiloops())
	{
		if (suna8_nmi_enable)	cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	}
	else cpunum_set_input_line(0, 0, HOLD_LINE);
}

static MACHINE_RESET( hardhea2 )
{
	hardhea2_rambank_0_w(0,0);
}

static MACHINE_DRIVER_START( hardhea2 )

	MDRV_IMPORT_FROM( brickzn )
	MDRV_CPU_MODIFY("main")			/* SUNA T568009 */
	MDRV_CPU_PROGRAM_MAP(hardhea2_readmem,hardhea2_writemem)
	MDRV_CPU_VBLANK_INT(hardhea2_interrupt,2)	/* IRQ & NMI */

	MDRV_MACHINE_RESET(hardhea2)
	MDRV_PALETTE_LENGTH(256)
MACHINE_DRIVER_END


/***************************************************************************
                                Star Fighter
***************************************************************************/

static struct AY8910interface starfigh_ay8910_interface =
{
	0,
	0,
	suna8_play_samples_w,
	suna8_samples_number_w
};

static MACHINE_DRIVER_START( starfigh )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 24000000 / 4)					/* ? */
	MDRV_CPU_PROGRAM_MAP(starfigh_readmem,starfigh_writemem)
	MDRV_CPU_IO_MAP(starfigh_readport,starfigh_writeport)
	MDRV_CPU_VBLANK_INT(brickzn_interrupt,2)	/* IRQ & NMI */

	/* The sound section is identical to that of hardhead */
	MDRV_CPU_ADD(Z80, 24000000 / 4)
	/* audio CPU */					/* ? */
	MDRV_CPU_PROGRAM_MAP(hardhead_sound_readmem,hardhead_sound_writemem)
	MDRV_CPU_IO_MAP(hardhead_sound_readport,hardhead_sound_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)	/* No NMI */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(suna8_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(suna8_textdim0)
	MDRV_VIDEO_UPDATE(suna8)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(AY8910, 24000000 / 16)
	MDRV_SOUND_CONFIG(starfigh_ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)

	MDRV_SOUND_ADD(SAMPLES, 0)
	MDRV_SOUND_CONFIG(custom_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END


/***************************************************************************
                                Spark Man
***************************************************************************/

static INTERRUPT_GEN( sparkman_interrupt )
{
	if (cpu_getiloops())
	{
		if (suna8_nmi_enable)	cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	}
	else cpunum_set_input_line(0, 0, HOLD_LINE);
}

static MACHINE_DRIVER_START( sparkman )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 24000000 / 4)					/* ? */
	MDRV_CPU_PROGRAM_MAP(sparkman_readmem,sparkman_writemem)
	MDRV_CPU_VBLANK_INT(sparkman_interrupt,2)	/* IRQ & NMI */

	MDRV_CPU_ADD(Z80, 24000000 / 4)
	/* audio CPU */					/* ? */
	MDRV_CPU_PROGRAM_MAP(hardhead_sound_readmem,hardhead_sound_writemem)
	MDRV_CPU_IO_MAP(hardhead_sound_readport,hardhead_sound_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)	/* No NMI */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(suna8_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(512)

	MDRV_VIDEO_START(suna8_textdim0)
	MDRV_VIDEO_UPDATE(suna8)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(AY8910, 24000000 / 16)
	MDRV_SOUND_CONFIG(hardhead_ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.30)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.30)

	MDRV_SOUND_ADD(SAMPLES, 0)
	MDRV_SOUND_CONFIG(custom_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END


/***************************************************************************


                                ROMs Loading


***************************************************************************/

/***************************************************************************

                                    Hard Head

Location  Type    File ID  Checksum
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
L5       27C256     P1       1327   [ main program ]
K5       27C256     P2       50B1   [ main program ]
J5       27C256     P3       CF73   [ main program ]
I5       27C256     P4       DE86   [ main program ]
D5       27C256     P5       94D1   [  background  ]
A5       27C256     P6       C3C7   [ motion obj.  ]
L7       27C256     P7       A7B8   [ main program ]
K7       27C256     P8       5E53   [ main program ]
J7       27C256     P9       35FC   [ main program ]
I7       27C256     P10      8F9A   [ main program ]
D7       27C256     P11      931C   [  background  ]
A7       27C256     P12      2EED   [ motion obj.  ]
H9       27C256     P13      5CD2   [ snd program  ]
M9       27C256     P14      5576   [  sound data  ]

Note:  Game   No. KRB-14
       PCB    No. 60138-0083

Main processor  -  Custom security block (battery backed) CPU No. S562008

Sound processor -  Z80
                -  YM3812
                -  AY-3-8910

24 MHz crystal

***************************************************************************/

ROM_START( hardhead )
	ROM_REGION( 0x48000, REGION_CPU1, 0 ) /* Main Z80 Code */
	ROM_LOAD( "p1",  0x00000, 0x8000, CRC(c6147926) SHA1(8d1609aaeac344c6aec102e92d34caab22a8ec64) )	// 1988,9,14
	ROM_LOAD( "p2",  0x10000, 0x8000, CRC(faa2cf9a) SHA1(5987f146b58fcbc3aaa9c010d86022b5172bcfb4) )
	ROM_LOAD( "p3",  0x18000, 0x8000, CRC(3d24755e) SHA1(519a179594956f7c3ddfaca362c42b453c928e25) )
	ROM_LOAD( "p4",  0x20000, 0x8000, CRC(0241ac79) SHA1(b3c3b98fb29836cbc9fd35ac49e02bfefd3b0c79) )
	ROM_LOAD( "p7",  0x28000, 0x8000, CRC(beba8313) SHA1(20aa4e07ec560a89d07ec73cc93311ceaed899a3) )
	ROM_LOAD( "p8",  0x30000, 0x8000, CRC(211a9342) SHA1(85bdafe1a2c683eea391cc663caabd958fdf5197) )
	ROM_LOAD( "p9",  0x38000, 0x8000, CRC(2ad430c4) SHA1(286a5b1042e077c3ae741d01311d4c91f8f87054) )
	ROM_LOAD( "p10", 0x40000, 0x8000, CRC(b6894517) SHA1(e114a5f92b83d98215aab6e2cd943a110d118f56) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "p13", 0x0000, 0x8000, CRC(493c0b41) SHA1(994a334253e905c39ec912765e8b0f4b1be900bc) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "p5",  0x00000, 0x8000, CRC(e9aa6fba) SHA1(f286727541f08b136a7d45e13975652bdc8fd663) )
	ROM_RELOAD(      0x08000, 0x8000             )
	ROM_LOAD( "p6",  0x10000, 0x8000, CRC(15d5f5dd) SHA1(4441344701fcdb2be55bdd76a8a5fd59f5de813c) )
	ROM_RELOAD(      0x18000, 0x8000             )
	ROM_LOAD( "p11", 0x20000, 0x8000, CRC(055f4c29) SHA1(0eee5db50504a3d37d9291ccd29863ba71da85e1) )
	ROM_RELOAD(      0x28000, 0x8000             )
	ROM_LOAD( "p12", 0x30000, 0x8000, CRC(9582e6db) SHA1(a2b34d740e07bd35a3184365e7f3ab7476075d70) )
	ROM_RELOAD(      0x38000, 0x8000             )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "p14", 0x0000, 0x8000, CRC(41314ac1) SHA1(1ac9213b0ac4ce9fe6256e93875672e128a5d069) )
ROM_END

ROM_START( hardhedb )
	ROM_REGION( 0x48000+0x8000, REGION_CPU1, 0 ) /* Main Z80 Code */
	ROM_LOAD( "1_27512.l6",  0x48000, 0x8000, CRC(bb4aa9ac) SHA1(da6310a1034cf610139d74fc30dd13e5fbd1d8dd) ) // 1988,9,14 (already decrypted)
	ROM_CONTINUE(			 0x00000, 0x8000 )
	ROM_LOAD( "p2",			 0x10000, 0x8000, CRC(faa2cf9a) SHA1(5987f146b58fcbc3aaa9c010d86022b5172bcfb4) )
	ROM_LOAD( "p3",			 0x18000, 0x8000, CRC(3d24755e) SHA1(519a179594956f7c3ddfaca362c42b453c928e25) )
	ROM_LOAD( "p4",			 0x20000, 0x8000, CRC(0241ac79) SHA1(b3c3b98fb29836cbc9fd35ac49e02bfefd3b0c79) )
	ROM_LOAD( "p7",			 0x28000, 0x8000, CRC(beba8313) SHA1(20aa4e07ec560a89d07ec73cc93311ceaed899a3) )
	ROM_LOAD( "p8",			 0x30000, 0x8000, CRC(211a9342) SHA1(85bdafe1a2c683eea391cc663caabd958fdf5197) )
	ROM_LOAD( "p9",			 0x38000, 0x8000, CRC(2ad430c4) SHA1(286a5b1042e077c3ae741d01311d4c91f8f87054) )
	ROM_LOAD( "p10",		 0x40000, 0x8000, CRC(b6894517) SHA1(e114a5f92b83d98215aab6e2cd943a110d118f56) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "p13", 0x0000, 0x8000, CRC(493c0b41) SHA1(994a334253e905c39ec912765e8b0f4b1be900bc) )
//  ROM_LOAD( "2_13_9h.rom", 0x00000, 0x8000, CRC(1b20e5ec) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "p5",  0x00000, 0x8000, CRC(e9aa6fba) SHA1(f286727541f08b136a7d45e13975652bdc8fd663) )
	ROM_RELOAD(      0x08000, 0x8000             )
	ROM_LOAD( "p6",  0x10000, 0x8000, CRC(15d5f5dd) SHA1(4441344701fcdb2be55bdd76a8a5fd59f5de813c) )
	ROM_RELOAD(      0x18000, 0x8000             )
	ROM_LOAD( "p11", 0x20000, 0x8000, CRC(055f4c29) SHA1(0eee5db50504a3d37d9291ccd29863ba71da85e1) )
	ROM_RELOAD(      0x28000, 0x8000             )
	ROM_LOAD( "p12", 0x30000, 0x8000, CRC(9582e6db) SHA1(a2b34d740e07bd35a3184365e7f3ab7476075d70) )
	ROM_RELOAD(      0x38000, 0x8000             )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "p14", 0x0000, 0x8000, CRC(41314ac1) SHA1(1ac9213b0ac4ce9fe6256e93875672e128a5d069) )
ROM_END

ROM_START( pop_hh )
	ROM_REGION( 0x48000+0x8000, REGION_CPU1, 0 ) /* Main Z80 Code */
	ROM_LOAD( "1_27512.l6",  0x48000, 0x8000, CRC(bb4aa9ac) SHA1(da6310a1034cf610139d74fc30dd13e5fbd1d8dd) ) // 1988,9,14 (already decrypted)
	ROM_CONTINUE(			 0x00000, 0x8000 )
	ROM_LOAD( "2_27256.k6",  0x10000, 0x8000, CRC(8fcc1248) SHA1(5da0b7dc63f7bc00e81e9e5bac02ee6b0076ffaa) )
	ROM_LOAD( "p3",          0x18000, 0x8000, CRC(3d24755e) SHA1(519a179594956f7c3ddfaca362c42b453c928e25) ) // 3_27256.j6
	ROM_LOAD( "p4",          0x20000, 0x8000, CRC(0241ac79) SHA1(b3c3b98fb29836cbc9fd35ac49e02bfefd3b0c79) ) // 4_27256.i6
	ROM_LOAD( "p7",          0x28000, 0x8000, CRC(beba8313) SHA1(20aa4e07ec560a89d07ec73cc93311ceaed899a3) ) // 7_27256.l8
	ROM_LOAD( "8_27256.k8",  0x30000, 0x8000, CRC(87a8b4b4) SHA1(83d30cf184c5dccdf2666c0ef9e078541d6a146e) )
	ROM_LOAD( "p9",          0x38000, 0x8000, CRC(2ad430c4) SHA1(286a5b1042e077c3ae741d01311d4c91f8f87054) ) // 9_27256.j8
	ROM_LOAD( "10_27256.i8", 0x40000, 0x8000, CRC(84fc6574) SHA1(ab33e6c656f25e65bb08d0a2689693df83cab43d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "p13", 0x0000, 0x8000, CRC(493c0b41) SHA1(994a334253e905c39ec912765e8b0f4b1be900bc) ) // 13_27256.i10

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "p5",  0x00000, 0x8000, CRC(e9aa6fba) SHA1(f286727541f08b136a7d45e13975652bdc8fd663) ) // 5_27256.d6
	ROM_RELOAD(      0x08000, 0x8000             )
	ROM_LOAD( "p6",  0x10000, 0x8000, CRC(15d5f5dd) SHA1(4441344701fcdb2be55bdd76a8a5fd59f5de813c) ) // 6_27256.a6
	ROM_RELOAD(      0x18000, 0x8000             )
	ROM_LOAD( "11_27256.d8", 0x20000, 0x8000, CRC(3751b99d) SHA1(dc4082e481a79f0389e59b4b38698df8f7b94053) )
	ROM_RELOAD(      0x28000, 0x8000             )
	ROM_LOAD( "p12", 0x30000, 0x8000, CRC(9582e6db) SHA1(a2b34d740e07bd35a3184365e7f3ab7476075d70) ) // 12_27256.a8
	ROM_RELOAD(      0x38000, 0x8000             )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "p14", 0x0000, 0x8000, CRC(41314ac1) SHA1(1ac9213b0ac4ce9fe6256e93875672e128a5d069) ) // 14_27256.m11
ROM_END


/***************************************************************************

                            Rough Ranger / Super Ranger

(SunA 1988)
K030087

 24MHz    6  7  8  9  - 10 11 12 13   sw1  sw2



   6264
   5    6116
   4    6116                         6116
   3    6116                         14
   2    6116                         Z80A
   1                        6116     8910
                 6116  6116          2203
                                     15
 Epoxy CPU
                            6116


---------------------------
Super Ranger by SUNA (1988)
---------------------------

Location   Type    File ID  Checksum
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
E2        27C256     R1      28C0    [ main program ]
F2        27C256     R2      73AD    [ main program ]
H2        27C256     R3      8B7A    [ main program ]
I2        27C512     R4      77BE    [ main program ]
J2        27C512     R5      6121    [ main program ]
P5        27C256     R6      BE0E    [  background  ]
P6        27C256     R7      BD5A    [  background  ]
P7        27C256     R8      4605    [ motion obj.  ]
P8        27C256     R9      7097    [ motion obj.  ]
P9        27C256     R10     3B9F    [  background  ]
P10       27C256     R11     2AE8    [  background  ]
P11       27C256     R12     8B6D    [ motion obj.  ]
P12       27C256     R13     927E    [ motion obj.  ]
J13       27C256     R14     E817    [ snd program  ]
E13       27C256     R15     54EE    [ sound data   ]

Note:  Game model number K030087

Hardware:

Main processor  -  Custom security block (battery backed)  CPU No. S562008

Sound processor - Z80
                - YM2203C
                - AY-3-8910

***************************************************************************/

ROM_START( rranger )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "1",  0x00000, 0x8000, CRC(4fb4f096) SHA1(c5ac3e04080cdcf570769918587e8cf8d455fc30) )	// V 2.0 1988,4,15
	ROM_LOAD( "2",  0x10000, 0x8000, CRC(ff65af29) SHA1(90f9a0c862e2a9da0343446a325961ab29d26b4b) )
	ROM_LOAD( "3",  0x18000, 0x8000, CRC(64e09436) SHA1(077f0d38d489562532d5f7678434a85ca04d373c) )
	ROM_LOAD( "r4", 0x30000, 0x8000, CRC(4346fae6) SHA1(a9f000e4427a1e9902627402dce14dc8ee04dbf8) )
	ROM_CONTINUE(   0x20000, 0x8000             )
	ROM_LOAD( "r5", 0x38000, 0x8000, CRC(6a7ca1c3) SHA1(0f0b508e9b20909e9efa07b42d67732082b6940b) )
	ROM_CONTINUE(   0x28000, 0x8000             )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "14", 0x0000, 0x8000, CRC(11c83aa1) SHA1(d1f75096528b220a3f858eac62e3b4111fa013de) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "15", 0x0000, 0x8000, CRC(28c2c87e) SHA1(ec0d92140ef44df822f2229e79b915e051caa033) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "6",  0x00000, 0x8000, CRC(57543643) SHA1(59c7717321314678e61b50767e168eb2a73147d3) )
	ROM_LOAD( "7",  0x08000, 0x8000, CRC(9f35dbfa) SHA1(8a8f158ad7f0bc6b43eaa95959af3ab58cf14d6d) )
	ROM_LOAD( "8",  0x10000, 0x8000, CRC(f400db89) SHA1(a07b226af40cac5a20739bb8f4226909724fda86) )
	ROM_LOAD( "9",  0x18000, 0x8000, CRC(fa2a11ea) SHA1(ea29ade1254caa2a3bd4b4816fe338f238025284) )
	ROM_LOAD( "10", 0x20000, 0x8000, CRC(42c4fdbf) SHA1(fd8b267d5098b640e731942b922149866ece1dc6) )
	ROM_LOAD( "11", 0x28000, 0x8000, CRC(19037a7b) SHA1(a6843b0220bab5c47307a0c761d5bd638716aef0) )
	ROM_LOAD( "12", 0x30000, 0x8000, CRC(c59c0ec7) SHA1(80003f3e33610a84f6e194918276d5f60145b00e) )
	ROM_LOAD( "13", 0x38000, 0x8000, CRC(9809fee8) SHA1(b7e0664702d0c1f77247d7c76a381b24687a09ea) )
ROM_END

ROM_START( sranger )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "r1", 0x00000, 0x8000, CRC(4eef1ede) SHA1(713074400e27f6983f97ce73e522a1d687961317) )	// V 2.0 1988,4,15
	ROM_LOAD( "2",  0x10000, 0x8000, CRC(ff65af29) SHA1(90f9a0c862e2a9da0343446a325961ab29d26b4b) )
	ROM_LOAD( "3",  0x18000, 0x8000, CRC(64e09436) SHA1(077f0d38d489562532d5f7678434a85ca04d373c) )
	ROM_LOAD( "r4", 0x30000, 0x8000, CRC(4346fae6) SHA1(a9f000e4427a1e9902627402dce14dc8ee04dbf8) )
	ROM_CONTINUE(   0x20000, 0x8000             )
	ROM_LOAD( "r5", 0x38000, 0x8000, CRC(6a7ca1c3) SHA1(0f0b508e9b20909e9efa07b42d67732082b6940b) )
	ROM_CONTINUE(   0x28000, 0x8000             )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "14", 0x0000, 0x8000, CRC(11c83aa1) SHA1(d1f75096528b220a3f858eac62e3b4111fa013de) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "15", 0x0000, 0x8000, CRC(28c2c87e) SHA1(ec0d92140ef44df822f2229e79b915e051caa033) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "r6",  0x00000, 0x8000, CRC(4f11fef3) SHA1(f48f3051a5ab681da0fd0a7107ea0c833993247a) )
	ROM_LOAD( "7",   0x08000, 0x8000, CRC(9f35dbfa) SHA1(8a8f158ad7f0bc6b43eaa95959af3ab58cf14d6d) )
	ROM_LOAD( "8",   0x10000, 0x8000, CRC(f400db89) SHA1(a07b226af40cac5a20739bb8f4226909724fda86) )
	ROM_LOAD( "9",   0x18000, 0x8000, CRC(fa2a11ea) SHA1(ea29ade1254caa2a3bd4b4816fe338f238025284) )
	ROM_LOAD( "r10", 0x20000, 0x8000, CRC(1b204d6b) SHA1(8649d552dff08bb01ac7ca6cb873124e05646041) )
	ROM_LOAD( "11",  0x28000, 0x8000, CRC(19037a7b) SHA1(a6843b0220bab5c47307a0c761d5bd638716aef0) )
	ROM_LOAD( "12",  0x30000, 0x8000, CRC(c59c0ec7) SHA1(80003f3e33610a84f6e194918276d5f60145b00e) )
	ROM_LOAD( "13",  0x38000, 0x8000, CRC(9809fee8) SHA1(b7e0664702d0c1f77247d7c76a381b24687a09ea) )
ROM_END

ROM_START( srangerb )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "r1bt", 0x00000, 0x8000, CRC(40635e7c) SHA1(741290ad640e941774d496a329cd29198ab83463) )	// NYWACORPORATION LTD 88-1-07
	ROM_LOAD( "2",    0x10000, 0x8000, CRC(ff65af29) SHA1(90f9a0c862e2a9da0343446a325961ab29d26b4b) )
	ROM_LOAD( "3",    0x18000, 0x8000, CRC(64e09436) SHA1(077f0d38d489562532d5f7678434a85ca04d373c) )
	ROM_LOAD( "r4",   0x30000, 0x8000, CRC(4346fae6) SHA1(a9f000e4427a1e9902627402dce14dc8ee04dbf8) )
	ROM_CONTINUE(     0x20000, 0x8000             )
	ROM_LOAD( "r5",   0x38000, 0x8000, CRC(6a7ca1c3) SHA1(0f0b508e9b20909e9efa07b42d67732082b6940b) )
	ROM_CONTINUE(     0x28000, 0x8000             )
	ROM_LOAD( "r5bt", 0x28000, 0x8000, BAD_DUMP CRC(f7f391b5) SHA1(a0a8de1d9d7876f5c4b26e34d5e54ec79529c2da) )	// wrong length

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "14", 0x0000, 0x8000, CRC(11c83aa1) SHA1(d1f75096528b220a3f858eac62e3b4111fa013de) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "15", 0x0000, 0x8000, CRC(28c2c87e) SHA1(ec0d92140ef44df822f2229e79b915e051caa033) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "r6",  0x00000, 0x8000, CRC(4f11fef3) SHA1(f48f3051a5ab681da0fd0a7107ea0c833993247a) )
	ROM_LOAD( "7",   0x08000, 0x8000, CRC(9f35dbfa) SHA1(8a8f158ad7f0bc6b43eaa95959af3ab58cf14d6d) )
	ROM_LOAD( "8",   0x10000, 0x8000, CRC(f400db89) SHA1(a07b226af40cac5a20739bb8f4226909724fda86) )
	ROM_LOAD( "9",   0x18000, 0x8000, CRC(fa2a11ea) SHA1(ea29ade1254caa2a3bd4b4816fe338f238025284) )
	ROM_LOAD( "r10", 0x20000, 0x8000, CRC(1b204d6b) SHA1(8649d552dff08bb01ac7ca6cb873124e05646041) )
	ROM_LOAD( "11",  0x28000, 0x8000, CRC(19037a7b) SHA1(a6843b0220bab5c47307a0c761d5bd638716aef0) )
	ROM_LOAD( "12",  0x30000, 0x8000, CRC(c59c0ec7) SHA1(80003f3e33610a84f6e194918276d5f60145b00e) )
	ROM_LOAD( "13",  0x38000, 0x8000, CRC(9809fee8) SHA1(b7e0664702d0c1f77247d7c76a381b24687a09ea) )
ROM_END

ROM_START( srangerw )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "w1", 0x00000, 0x8000, CRC(2287d3fc) SHA1(cc2dab587ca50fc4371d2861ac842cd81370f868) )	// 88,2,28
	ROM_LOAD( "2",  0x10000, 0x8000, CRC(ff65af29) SHA1(90f9a0c862e2a9da0343446a325961ab29d26b4b) )
	ROM_LOAD( "3",  0x18000, 0x8000, CRC(64e09436) SHA1(077f0d38d489562532d5f7678434a85ca04d373c) )
	ROM_LOAD( "r4", 0x30000, 0x8000, CRC(4346fae6) SHA1(a9f000e4427a1e9902627402dce14dc8ee04dbf8) )
	ROM_CONTINUE(   0x20000, 0x8000             )
	ROM_LOAD( "r5", 0x38000, 0x8000, CRC(6a7ca1c3) SHA1(0f0b508e9b20909e9efa07b42d67732082b6940b) )
	ROM_CONTINUE(   0x28000, 0x8000             )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Sound Z80 Code */
	ROM_LOAD( "14", 0x0000, 0x8000, CRC(11c83aa1) SHA1(d1f75096528b220a3f858eac62e3b4111fa013de) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "15", 0x0000, 0x8000, CRC(28c2c87e) SHA1(ec0d92140ef44df822f2229e79b915e051caa033) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "w6",  0x00000, 0x8000, CRC(312ecda6) SHA1(db11259e10da5f7f2b7b306482a08835597dbde4) )
	ROM_LOAD( "7",   0x08000, 0x8000, CRC(9f35dbfa) SHA1(8a8f158ad7f0bc6b43eaa95959af3ab58cf14d6d) )
	ROM_LOAD( "8",   0x10000, 0x8000, CRC(f400db89) SHA1(a07b226af40cac5a20739bb8f4226909724fda86) )
	ROM_LOAD( "9",   0x18000, 0x8000, CRC(fa2a11ea) SHA1(ea29ade1254caa2a3bd4b4816fe338f238025284) )
	ROM_LOAD( "w10", 0x20000, 0x8000, CRC(8731abc6) SHA1(05c13b359106b4ead1326f2e53d0585a2f0019ac) )
	ROM_LOAD( "11",  0x28000, 0x8000, CRC(19037a7b) SHA1(a6843b0220bab5c47307a0c761d5bd638716aef0) )
	ROM_LOAD( "12",  0x30000, 0x8000, CRC(c59c0ec7) SHA1(80003f3e33610a84f6e194918276d5f60145b00e) )
	ROM_LOAD( "13",  0x38000, 0x8000, CRC(9809fee8) SHA1(b7e0664702d0c1f77247d7c76a381b24687a09ea) )
ROM_END


/***************************************************************************

                                    Brick Zone

SUNA ELECTRONICS IND CO., LTD

CPU Z0840006PSC (ZILOG)

Chrystal : 24.000 MHz

Sound CPU : Z084006PSC (ZILOG) + AY3-8910A

Warning ! This game has a 'SUNA' protection block :-(

-

(c) 1992 Suna Electronics

2 * Z80B

AY-3-8910
YM3812

24 MHz crystal

Large epoxy(?) module near the cpu's.

***************************************************************************/

ROM_START( brickzn )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "brickzon.009", 0x00000, 0x08000, CRC(1ea68dea) SHA1(427152a26b062c5e77089de49c1da69369d4d557) )	// V5.0 1992,3,3
	ROM_LOAD( "brickzon.008", 0x10000, 0x20000, CRC(c61540ba) SHA1(08c0ede591b229427b910ca6bb904a6146110be8) )
	ROM_LOAD( "brickzon.007", 0x30000, 0x20000, CRC(ceed12f1) SHA1(9006726b75a65455afb1194298bade8fa2207b4a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Music Z80 Code */
	ROM_LOAD( "brickzon.010", 0x00000, 0x10000, CRC(4eba8178) SHA1(9a214a1acacdc124529bc9dde73a8e884fc70293) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )		/* PCM Z80 Code */
	ROM_LOAD( "brickzon.011", 0x00000, 0x10000, CRC(6c54161a) SHA1(ea216d9f45b441acd56b9fed81a83e3bfe299fbd) )

	ROM_REGION( 0xc0000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "brickzon.002", 0x00000, 0x20000, CRC(241f0659) SHA1(71b577bf7097b3b367d068df42f991d515f9003a) )
	ROM_LOAD( "brickzon.001", 0x20000, 0x20000, CRC(6970ada9) SHA1(5cfe5dcf25af7aff67ee5d78eb963d598251025f) )
	ROM_LOAD( "brickzon.003", 0x40000, 0x20000, CRC(2e4f194b) SHA1(86da1a582ea274f2af96d3e3e2ac72bcaf3638cb) )
	ROM_LOAD( "brickzon.005", 0x60000, 0x20000, CRC(118f8392) SHA1(598fa4df3ae348ec9796cd6d90c3045bec546da3) )
	ROM_LOAD( "brickzon.004", 0x80000, 0x20000, CRC(2be5f335) SHA1(dc870a3c5303cb2ea1fea4a25f53db016ed5ecee) )
	ROM_LOAD( "brickzon.006", 0xa0000, 0x20000, CRC(bbf31081) SHA1(1fdbd0e0853082345225e18df340184a7a604b78) )
ROM_END

ROM_START( brickzn3 )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "39",           0x00000, 0x08000, CRC(043380bd) SHA1(7eea7cc7d754815df233879b4a9d3d88eac5b28d) )	// V3.0 1992,1,23
	ROM_LOAD( "38",           0x10000, 0x20000, CRC(e16216e8) SHA1(e88ae97e8a632823d5f1fe500954b6f6542407d5) )
	ROM_LOAD( "brickzon.007", 0x30000, 0x20000, CRC(ceed12f1) SHA1(9006726b75a65455afb1194298bade8fa2207b4a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Music Z80 Code */
	ROM_LOAD( "brickzon.010", 0x00000, 0x10000, CRC(4eba8178) SHA1(9a214a1acacdc124529bc9dde73a8e884fc70293) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )		/* PCM Z80 Code */
	ROM_LOAD( "brickzon.011", 0x00000, 0x10000, CRC(6c54161a) SHA1(ea216d9f45b441acd56b9fed81a83e3bfe299fbd) )

	ROM_REGION( 0xc0000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "35",           0x00000, 0x20000, CRC(b463dfcf) SHA1(35c8a4a0c5b62a087a2cb70bc5b7815f5bb2d973) )
	ROM_LOAD( "brickzon.004", 0x20000, 0x20000, CRC(2be5f335) SHA1(dc870a3c5303cb2ea1fea4a25f53db016ed5ecee) )
	ROM_LOAD( "brickzon.006", 0x40000, 0x20000, CRC(bbf31081) SHA1(1fdbd0e0853082345225e18df340184a7a604b78) )
	ROM_LOAD( "32",           0x60000, 0x20000, CRC(32dbf2dd) SHA1(b9ab8b93a062b871b7f824e4be2f214cc832d585) )
	ROM_LOAD( "brickzon.001", 0x80000, 0x20000, CRC(6970ada9) SHA1(5cfe5dcf25af7aff67ee5d78eb963d598251025f) )
	ROM_LOAD( "brickzon.003", 0xa0000, 0x20000, CRC(2e4f194b) SHA1(86da1a582ea274f2af96d3e3e2ac72bcaf3638cb) )
ROM_END



/***************************************************************************

                                Hard Head 2

These ROMS are all 27C512

ROM 1 is at Location 1N
ROM 2 ..............1o
ROM 3 ..............1Q
ROM 4...............3N
ROM 5.............. 4N
ROM 6...............4o
ROM 7...............4Q
ROM 8...............6N
ROM 10..............H5
ROM 11..............i5
ROM 12 .............F7
ROM 13..............H7
ROM 15..............N10

These ROMs are 27C256

ROM 9...............F5
ROM 14..............C8

Game uses 2 Z80B processors and a Custom Sealed processor (assumed)
Labeled "SUNA T568009"

Sound is a Yamaha YM3812 and a  AY-3-8910A

3 RAMS are 6264LP- 10   and 5) HM6116K-90 rams  (small package)

24 MHz

***************************************************************************/

ROM_START( hardhea2 )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "hrd-hd9",  0x00000, 0x08000, CRC(69c4c307) SHA1(0dfde1dcda51b5b1740aff9e96cb877a428a3e04) )	// V 2.0 1991,2,12
	ROM_LOAD( "hrd-hd10", 0x10000, 0x10000, CRC(77ec5b0a) SHA1(2d3e24c208904a7884e585e08e5818fd9f8b5391) )
	ROM_LOAD( "hrd-hd11", 0x20000, 0x10000, CRC(12af8f8e) SHA1(1b33a060b70900042fdae00f7dec325228d566f5) )
	ROM_LOAD( "hrd-hd12", 0x30000, 0x10000, CRC(35d13212) SHA1(2fd03077b89ec9e55d2758b7f9cada970f0bdd91) )
	ROM_LOAD( "hrd-hd13", 0x40000, 0x10000, CRC(3225e7d7) SHA1(2da9d1ce182dab8d9e09772e6899676b84c7458c) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Music Z80 Code */
	ROM_LOAD( "hrd-hd14", 0x00000, 0x08000, CRC(79a3be51) SHA1(30bc67cd3a936615c6931f8e15953425dff59611) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )		/* PCM Z80 Code */
	ROM_LOAD( "hrd-hd15", 0x00000, 0x10000, CRC(bcbd88c3) SHA1(79782d598d9d764de70c54fc07ff9bf0f7d13d62) )

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "hrd-hd1",  0x00000, 0x10000, CRC(7e7b7a58) SHA1(1a74260dda64aafcb046c8add92a54655bbc74e4) )
	ROM_LOAD( "hrd-hd2",  0x10000, 0x10000, CRC(303ec802) SHA1(533c29d9bb54415410c5d3c5af234b8b040190de) )
	ROM_LOAD( "hrd-hd3",  0x20000, 0x10000, CRC(3353b2c7) SHA1(a3ec0fc2a97e7e0bc72fafd5897cb1dd4cd32197) )
	ROM_LOAD( "hrd-hd4",  0x30000, 0x10000, CRC(dbc1f9c1) SHA1(720c729d7825635584632d033b4b46eea2fb1291) )
	ROM_LOAD( "hrd-hd5",  0x40000, 0x10000, CRC(f738c0af) SHA1(7dda657acd1d6fb7064e8dbd5ce386e9eae3d36a) )
	ROM_LOAD( "hrd-hd6",  0x50000, 0x10000, CRC(bf90d3ca) SHA1(2d0533d93fc5155fe879c1890bc7bc4581308e16) )
	ROM_LOAD( "hrd-hd7",  0x60000, 0x10000, CRC(992ce8cb) SHA1(21c0dd227138ec64003c7cb090855ec27d41719e) )
	ROM_LOAD( "hrd-hd8",  0x70000, 0x10000, CRC(359597a4) SHA1(ae024dd61c5d12813a661abe8ea63ae6112ddc9c) )
ROM_END


/***************************************************************************

                                Star Fighter

***************************************************************************/

ROM_START( starfigh )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "starfgtr.l1", 0x00000, 0x08000, CRC(f93802c6) SHA1(4005b06b69dd440dfb6385766386a1168e73288f) )	// V.1
	ROM_LOAD( "starfgtr.j1", 0x10000, 0x10000, CRC(fcfcf08a) SHA1(65fe1666aa5092f820b337bcbcbed7accdec440d) )
	ROM_LOAD( "starfgtr.i1", 0x20000, 0x10000, CRC(6935fcdb) SHA1(f47812f6716ccf52dd7ab8522c29e059f1e38f31) )
	ROM_LOAD( "starfgtr.l3", 0x30000, 0x10000, CRC(50c072a4) SHA1(e48ec5a786ef245e5b2b72390824b6b7c449a74b) )	// 0xxxxxxxxxxxxxxx = 0xFF (ROM Test: OK)
	ROM_LOAD( "starfgtr.j3", 0x40000, 0x10000, CRC(3fe3c714) SHA1(ccc9a33cf29c0e43ae8ab91f08438a89c777c186) )	// clear text here

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Music Z80 Code */
	ROM_LOAD( "starfgtr.m8", 0x0000, 0x8000, CRC(ae3b0691) SHA1(41e004d09522cf7ddce6e4adc68841ad5553264a) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "starfgtr.q10", 0x0000, 0x8000, CRC(fa510e94) SHA1(e2742385a4ba152dbc89534e4350d1d9ad49730f) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "starfgtr.e4", 0x00000, 0x10000, CRC(54c0ca3d) SHA1(87f785502beb8a52d47bd48275d695ee303054f8) )
	ROM_RELOAD(              0x20000, 0x10000             )
	ROM_LOAD( "starfgtr.d4", 0x10000, 0x10000, CRC(4313ba40) SHA1(3c41f99dc40136517f172b3525987d8909f877c3) )
	ROM_RELOAD(              0x30000, 0x10000             )
	ROM_LOAD( "starfgtr.b4", 0x40000, 0x10000, CRC(ad8d0f21) SHA1(ffdb407c7fe76b5f290de6bbed2fec34e40daf3f) )
	ROM_RELOAD(              0x60000, 0x10000             )
	ROM_LOAD( "starfgtr.a4", 0x50000, 0x10000, CRC(6d8f74c8) SHA1(c40b77e27bd29d6c3a9b4d43189933c10543786b) )
	ROM_RELOAD(              0x70000, 0x10000             )
	ROM_LOAD( "starfgtr.e6", 0x80000, 0x10000, CRC(ceff00ff) SHA1(5e7df7f33f36f4bc511be48266eaec274dfb8706) )
	ROM_RELOAD(              0xa0000, 0x10000             )
	ROM_LOAD( "starfgtr.d6", 0x90000, 0x10000, CRC(7aaa358a) SHA1(56d75f4abe626de7923d5bcc9ad18c02ce162907) )
	ROM_RELOAD(              0xb0000, 0x10000             )
	ROM_LOAD( "starfgtr.b6", 0xc0000, 0x10000, CRC(47d6049c) SHA1(cae0795a19cb6bb8bdabc10c200aa6f8d78dd347) )
	ROM_RELOAD(              0xe0000, 0x10000             )
	ROM_LOAD( "starfgtr.a6", 0xd0000, 0x10000, CRC(4a33f6f3) SHA1(daa0a1a43b1b60e2f05b9934fdd6b5f285a0b93a) )
	ROM_RELOAD(              0xf0000, 0x10000             )
ROM_END


/***************************************************************************

                                Spark Man

Suna Electronics IND. CO., LTD 1989 KRB-16 60136-0081  Pinout = JAMMA

Game uses a Custom Sealed processor labeled "SUNA T568009" and a z80 processor for sound

Sound is a Yamaha YM3812 and a  AY-3-8910A

24mhz crystal

***************************************************************************/

ROM_START( sparkman )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )		/* Main Z80 Code */
	ROM_LOAD( "sparkman.e7", 0x00000, 0x08000, CRC(d89c5780) SHA1(177f0ae21c00575a7eb078e86f3a790fc95211e4) )	/* "SPARK MAN MAIN PROGRAM 1989,8,12 K.H.T (SUNA ELECTRPNICS) V 2.0 SOULE KOREA" */
	ROM_LOAD( "sparkman.g7", 0x10000, 0x10000, CRC(48b4a31e) SHA1(771d1f1a2ce950ce2b661a4081471e98a7a7d53e) )
	ROM_LOAD( "sparkman.g8", 0x20000, 0x10000, CRC(b8a4a557) SHA1(10251b49fb44fb1e7c71fde8fe9544df29d27346) )
	ROM_LOAD( "sparkman.i7", 0x30000, 0x10000, CRC(f5f38e1f) SHA1(25f0abbac1298fad1f8e7202db05e48c3598bc88) )
	ROM_LOAD( "sparkman.i8", 0x40000, 0x10000,  CRC(e54eea25) SHA1(b8ea884ee1a24953b6406f2d1edf103700f542d2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Music Z80 Code */
	ROM_LOAD( "sparkman.h11", 0x00000, 0x08000, CRC(06822f3d) SHA1(d30592cecbcd4dbf67e5a8d9c151d60b3232a54d) )

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )	/* Sprites */
	ROM_LOAD( "sparkman.u4", 0x00000, 0x10000, CRC(17c16ce4) SHA1(b4127e9aedab69193bef1d85e68003e225913417) )
	ROM_LOAD( "sparkman.t1", 0x10000, 0x10000, CRC(2e474203) SHA1(a407126d92e529568129d5246f89d51330ff5d32) )
	ROM_LOAD( "sparkman.r1", 0x20000, 0x08000, CRC(7115cfe7) SHA1(05fde6279a1edc97e79b1ff3f72b2da400a6a409) )
	ROM_LOAD( "sparkman.u1", 0x30000, 0x10000, CRC(39dbd414) SHA1(03fe938ed1191329b6a2f7ed54c6ef69273998df) )

	ROM_LOAD( "sparkman.u6", 0x40000, 0x10000, CRC(414222ea) SHA1(e05f0504c6e735c73027312a85cc55fc98728e53) )
	ROM_LOAD( "sparkman.t2", 0x50000, 0x10000, CRC(0df5da2a) SHA1(abbd5ba22b30f17d203ecece7afafa0cbe78352c) )
	ROM_LOAD( "sparkman.r2", 0x60000, 0x08000, CRC(6904bde2) SHA1(c426fa0c29b1874c729b981467f219c422f863aa) )
	ROM_LOAD( "sparkman.u2", 0x70000, 0x10000, CRC(e6551db9) SHA1(bed2a9ba72895f3ba876b4e0a41c33ea8a3c5af2) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )		/* Samples */
	ROM_LOAD( "sparkman.b10", 0x0000, 0x8000, CRC(46c7d4d8) SHA1(99f38cc044390ee4646498667ad2bf536ce91e8f) )

	ROM_REGION( 0x8000, REGION_SOUND2, 0 )		/* Samples */
	ROM_LOAD( "sprkman.b11", 0x0000, 0x8000, CRC(d6823a62) SHA1(f8ce748aa7bdc9c95799dd111fd872717e46d416) )
ROM_END


/***************************************************************************


                                Games Drivers


***************************************************************************/

static DRIVER_INIT( suna8 )
{
	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
}

/* Working Games */
GAME( 1988, rranger,  0,        rranger,  rranger,  suna8,    ROT0,  "SunA (Sharp Image license)", "Rough Ranger (v2.0)", GAME_IMPERFECT_SOUND ,2)
GAME( 1988, hardhead, 0,        hardhead, hardhead, hardhead, ROT0,  "SunA",                       "Hard Head"									, 0,0)
GAME( 1988, hardhedb, hardhead, hardhead, hardhead, hardhedb, ROT0,  "bootleg",                    "Hard Head (bootleg)"						, 0,0)
GAME( 1988, pop_hh,   hardhead, hardhead, hardhead, hardhedb, ROT0,  "bootleg",                    "Popper (Hard Head bootleg)"				, 0,0)
GAME( 1991, hardhea2, 0,        hardhea2, hardhea2, hardhea2, ROT0,  "SunA",                       "Hard Head 2 (v2.0)"                       , 0 ,0)

/* Non Working Games */
GAME( 1988, sranger,  rranger, rranger,  rranger,	suna8,        ROT0,  "SunA",    "Super Ranger (v2.0)",           GAME_NOT_WORKING ,2)
GAME( 1988, srangerb, rranger, rranger,  rranger,	suna8,        ROT0,  "bootleg", "Super Ranger (bootleg)",        GAME_NOT_WORKING ,2)
GAME( 1988, srangerw, rranger, rranger,  rranger,	suna8,        ROT0,  "SunA (WDK license)", "Super Ranger (WDK)", GAME_NOT_WORKING ,2)
GAME( 1989, sparkman, 0,       sparkman, sparkman, sparkman, ROT0,  "SunA",    "Spark Man (v 2.0)",             GAME_NOT_WORKING ,0)
GAME( 1990, starfigh, 0,       starfigh, hardhea2, starfigh, ROT90, "SunA",    "Star Fighter (v1)",             GAME_NOT_WORKING ,0)
GAME( 1992, brickzn,  0,       brickzn,  brickzn,  brickzn,  ROT90, "SunA",    "Brick Zone (v5.0)",             GAME_NOT_WORKING ,0)
GAME( 1992, brickzn3, brickzn, brickzn,  brickzn,  brickzn3, ROT90, "SunA",    "Brick Zone (v4.0)",             GAME_NOT_WORKING ,0)
