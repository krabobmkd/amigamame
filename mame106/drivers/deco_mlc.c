/***************************************************************************

    Data East MLC Hardware:

    The MLC system is basically an 8" x 6" x 2" plastic box with a JAMMA connector on it.
    The PCB's are very compact and have almost entirely surface mounted components on both
    sides of both boards. One board contains the RAM, sound hardware and I/O, the other
    board contains the CPU and ROMs. All main boards are identical between the MLC games and
    can be changed just by plugging in another game and pressing test to reset the EEPROM
    defaults.

    PCB Layout
    ----------

    Jamma Side:

    DE-0444-1
    |-----------------------------|
    |                             |
    | W24257            DE150     |
    |   W24257                    |
    |                             |
    |J                      93C45 |
    |A                            |
    |M                            |
    |M               DE223        |
    |A                            |
    |                             |
    |                42MHz  W24257|
    |                       W24257|
    |                       W24257|
    |            XILINK     W24257|
    |5            XC3130          |
    |0                            |
    |P                            |
    |I YMZ280B   YAC513           |
    |N                            |
    |-----------------------------|
    Notes:
        - Yamaha YMZ280B clock: 14.000MHz (42 / 3)
        - DE150 custom (GFX)
        - DE223 custom (GFX)
        - Xilinx XC3130 (TQFP100, Bus Controller?)
        - YAC513 D/A converter
        - 93C45 EEPROM, 128 bytes x 8 bit (equivalent to 93C46)
        - All SRAM is Winbond W24257S-70LL (32kx8)
        - 50 PIN connector looks like flat cable SCSI connector used on any regular PC SCSI controller card. It appears
        to be used for extra controls and hookup of a 2nd speaker for stereo output. It could also be used for externally
        programming some IC's or factory diagnostic/repairs?
        - Bottom side of the JAMMA side contains nothing significant except a sound AMP, test SW, LED, some
        logic chips and connectors for joining the PCBs together.
        - Vsync: 58Hz

    As the CPU is stored on the game board it is possible that each game could
    have a different CPU - however there are only two known configurations.  Avengers
    in Galactic Storm uses a SH2 processor, whereas all the others use a custom Deco
    processor (156 - encrypted ARM).

    Skull Fang:

    DE-0445-1 (top)                     DE-0445-1 (bottom)
    |-----------------------------|     |-----------------------------|
    |                             |     |                             |
    |                             |     |                             |
    |                             |     |           DE156     MCH-07  |
    |  MCH-06             SH00-0  |     |                             |
    |                             |     |                             |
    |                     SH01-0  |     |                             |
    |                             |     |                             |
    |                             |     |                             |
    |                             |     |                             |
    |                             |     |                             |
    |                             |     |                             |
    | MCH-04    MCH-02    MCH-00  |     |                             |
    |                             |     |                             |
    |                             |     | MCH-01    MCH-03    MCH-05  |
    |                             |     |                             |
    | SH02-0                      |     |                             |
    |                             |     |                             |
    |                             |     |                             |
    |                             |     |                             |
    |-----------------------------|     |-----------------------------|

    Notes:
        - DE156 clock: 7.000MHz (42MHz / 6, QFP100, clock measured on pin 90)

        Stadium Hero contains a '146' protection chip on the ROM/CPU pcb, but
        it is barely used by the game (only checked at startup).  See decoprot.c

    Driver todo:
        stadhr96 seems to require raster IRQ video update support.
        ddream95 seems to have a dual screen mode(??)

    Driver by Bryan McPhail, bmcphail@tendril.co.uk, thank you to Avedis and The Guru.

***************************************************************************/

#include "driver.h"
#include "machine/eeprom.h"
#include "sound/ymz280b.h"
#include "decoprot.h"
#include "cpu/arm/arm.h"

extern void decrypt156(void);

VIDEO_START( mlc );
VIDEO_UPDATE( mlc );
VIDEO_EOF( mlc );

extern UINT32 *mlc_vram, *mlc_clip_ram;
static UINT32 *mlc_ram, *irq_ram;
static void *raster_irq_timer;
static int mainCpuIsArm=1;
int mlc_raster_table[9][256];

/***************************************************************************/

static READ32_HANDLER( avengrs_control_r )
{
	return (readinputport(1)<<16) | (EEPROM_read_bit()<<23) | readinputport(0);
}

static READ32_HANDLER(test2_r)
{
//  if (offset==0)
//      return readinputport(0); //0xffffffff;
//   logerror("%08x:  Test2_r %d\n",activecpu_get_pc(),offset);
	return rand(); //0xffffffff;
}

static READ32_HANDLER(test3_r)
{
/*
    test3 7 - vbl loop on 0x10 0000 at end of IRQ

*/
//if (offset==0)
//  return rand()|(rand()<<16);
//  logerror("%08x:  Test3_r %d\n",activecpu_get_pc(),offset);
	return 0xffffffff;
}

static WRITE32_HANDLER( avengrs_eprom_w )
{
	if (mem_mask==0xffff00ff) {
		UINT8 ebyte=(data>>8)&0xff;
//      if (ebyte&0x80) {
			EEPROM_set_clock_line((ebyte & 0x2) ? ASSERT_LINE : CLEAR_LINE);
			EEPROM_write_bit(ebyte & 0x1);
			EEPROM_set_cs_line((ebyte & 0x4) ? CLEAR_LINE : ASSERT_LINE);
//      }
	}
	else if (mem_mask==0xffffff00) {
		//volume control todo
	}
	else
		logerror("%08x:  eprom_w %08x mask %08x\n",activecpu_get_pc(),data,mem_mask);
}

static WRITE32_HANDLER( avengrs_palette_w )
{
	int r,g,b;

	COMBINE_DATA(&paletteram32[offset]);

	/* x bbbbb ggggg rrrrr */
	b = (paletteram32[offset] >> 10) & 0x1f;
	g = (paletteram32[offset] >> 5) & 0x1f;
	r = (paletteram32[offset] >> 0) & 0x1f;

	palette_set_color(offset,r*8,g*8,b*8);
}

static READ32_HANDLER( avengrs_sound_r )
{
	if (mem_mask==0x00ffffff) {
		return YMZ280B_status_0_r(0)<<24;
	} else {
		logerror("%08x:  non-byte read from sound mask %08x\n",activecpu_get_pc(),mem_mask);
	}

	return 0;
}

static WRITE32_HANDLER( avengrs_sound_w )
{
	if (mem_mask==0x00ffffff) {
		if (offset)
			YMZ280B_data_0_w(0,data>>24);
		else
			YMZ280B_register_0_w(0,data>>24);
	} else {
		logerror("%08x:  non-byte written to sound %08x mask %08x\n",activecpu_get_pc(),data,mem_mask);
	}
}

static READ32_HANDLER( decomlc_vbl_r )
{
	static int i=0xffffffff;
	i ^=0xffffffff;
//logerror("vbl r %08x\n", activecpu_get_pc());
	// Todo: Vblank probably in $10
	return i;
}

static READ32_HANDLER( mlc_scanline_r )
{
//  logerror("read scanline counter (%d)\n", cpu_getscanline());
	return cpu_getscanline();
}

static void interrupt_gen(int scanline)
{
//  logerror("hit scanline IRQ %d (%08x)\n", scanline, info.i);
	cpunum_set_input_line(0, mainCpuIsArm ? ARM_IRQ_LINE : 1, HOLD_LINE);
	timer_adjust(raster_irq_timer,TIME_NEVER,0,0);
}

static WRITE32_HANDLER( mlc_irq_w )
{
	static int lastScanline[9]={ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int scanline=cpu_getscanline();
	irq_ram[offset]=data&0xffff;

	switch (offset*4)
	{
	case 0x10: /* IRQ ack.  Value written doesn't matter */
		cpunum_set_input_line(0, mainCpuIsArm ? ARM_IRQ_LINE : 1, CLEAR_LINE);
		return;
		break;
	case 0x14: /* Prepare scanline interrupt */
		timer_adjust(raster_irq_timer,cpu_getscanlinetime(irq_ram[0x14/4]),irq_ram[0x14/4],TIME_NEVER);
		//logerror("prepare scanline to fire at %d (currently on %d)\n", irq_ram[0x14/4], cpu_getscanline());
		return;
		break;
	case 0x18:
	case 0x1c:
	case 0x20:
	case 0x24:
	case 0x28:
	case 0x2c:
	case 0x30:
	case 0x34:
	case 0x38:
		if (scanline > 255)
			scanline = 255;
		/* Update scanlines up to present line */
		while (lastScanline[offset-6]<scanline)
		{
			mlc_raster_table[offset-6][lastScanline[offset-6]+1]=mlc_raster_table[offset-6][lastScanline[offset-6]];
			lastScanline[offset-6]++;
		}

		if (lastScanline[offset-6] > scanline)
			lastScanline[offset-6]=0;

		/* Set current scanline value */
		mlc_raster_table[offset-6][scanline]=data&0xffff;
		break;

	default:
		break;
	};

//  logerror("irqw %04x %04x (%d)\n", offset * 4, data&0xffff, scanline);
}

static READ32_HANDLER(mlc_spriteram_r)
{
	return spriteram32[offset]&0xffff;
}

static READ32_HANDLER(mlc_vram_r)
{
	return mlc_vram[offset]&0xffff;
}

static READ32_HANDLER(stadhr96_prot_146_r)
{
	/*
        cpu #0 (PC=00041BD0): unmapped program memory dword write to 00708004 = 000F0000 & FFFFFFFF
        cpu #0 (PC=00041BFC): unmapped program memory dword write to 0070F0C8 = 00028800 & FFFFFFFF
        cpu #0 (PC=00041C08): unmapped program memory dword write to 0070F010 = 00081920 & FFFFFFFF
        cpu #0 (PC=00041C14): unmapped program memory dword write to 0070F020 = 00040960 & FFFFFFFF
        cpu #0 (PC=00041C20): unmapped program memory dword write to 0070F03C = 5A5A5A5A & FFFFFFFF
    */
	offset<<=1;

	logerror("%08x:  Read prot %04x\n", activecpu_get_pc(), offset);

	if (offset==0x5c4)
		return 0xaa55 << 16;
	if (offset==0x7a4)
		return 0x0002 << 16;
	if (offset==0x53c)
		return 0x0008 << 16;
	if (offset==0x304)
		return 0x0001 << 16; // Unknown, is either 0,1,2,3

	return 0;
}

/******************************************************************************/

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 32 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(24) )
	AM_RANGE(0x0000000, 0x00fffff) AM_READ(MRA32_ROM)
	AM_RANGE(0x0100000, 0x011ffff) AM_READ(MRA32_RAM)
	AM_RANGE(0x0200000, 0x020000f) AM_READ(MRA32_NOP) /* IRQ control? */
	AM_RANGE(0x0200070, 0x0200073) AM_READ(decomlc_vbl_r)
	AM_RANGE(0x0200074, 0x0200077) AM_READ(mlc_scanline_r)
	AM_RANGE(0x0200078, 0x020007f) AM_READ(test2_r)
	AM_RANGE(0x0200080, 0x02000ff) AM_READ(MRA32_RAM)
	AM_RANGE(0x0204000, 0x0206fff) AM_READ(mlc_spriteram_r)
	AM_RANGE(0x0200080, 0x02000ff) AM_READ(MRA32_RAM)
	AM_RANGE(0x0280000, 0x029ffff) AM_READ(mlc_vram_r)
	AM_RANGE(0x0300000, 0x0307fff) AM_READ(MRA32_RAM)
	AM_RANGE(0x0400000, 0x0400003) AM_READ(avengrs_control_r)
	AM_RANGE(0x0440000, 0x044001f) AM_READ(test3_r)
	AM_RANGE(0x0600004, 0x0600007) AM_READ(avengrs_sound_r)
	AM_RANGE(0x070f000, 0x070ffff) AM_READ(stadhr96_prot_146_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 32 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(24) )
	AM_RANGE(0x0000000, 0x00fffff) AM_WRITE(MWA32_ROM)
	AM_RANGE(0x0100000, 0x011ffff) AM_WRITE(MWA32_RAM) AM_BASE(&mlc_ram)
	AM_RANGE(0x0200000, 0x020007f) AM_WRITE(mlc_irq_w) AM_BASE(&irq_ram)
	AM_RANGE(0x0200080, 0x02000ff) AM_WRITE(MWA32_RAM) AM_BASE(&mlc_clip_ram)
	AM_RANGE(0x0204000, 0x0206fff) AM_WRITE(MWA32_RAM) AM_BASE(&spriteram32) AM_SIZE(&spriteram_size)
	AM_RANGE(0x0280000, 0x029ffff) AM_WRITE(MWA32_RAM) AM_BASE(&mlc_vram)
	AM_RANGE(0x0300000, 0x0307fff) AM_WRITE(avengrs_palette_w) AM_BASE(&paletteram32)
	AM_RANGE(0x044001c, 0x044001f) AM_WRITE(MWA32_NOP)
	AM_RANGE(0x0500000, 0x0500003) AM_WRITE(avengrs_eprom_w)
	AM_RANGE(0x0600000, 0x0600007) AM_WRITE(avengrs_sound_w)
//  AM_RANGE(0x070f000, 0x070ffff) AM_READ(stadhr96_prot_146_w) AM_BASE(&deco32_prot_ram)
ADDRESS_MAP_END

/******************************************************************************/

INPUT_PORTS_START( mlc )
	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE_NO_TOGGLE( 0x08, IP_ACTIVE_LOW )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* Eprom */
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout spritelayout_4bpp =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+16, RGN_FRAC(1,2)+0, 16, 0 },
	{ 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	16*32
};

static const gfx_layout spritelayout_5bpp =
{
	16,16,
	RGN_FRAC(1,3),
	5,
	{ RGN_FRAC(2,3)+0, RGN_FRAC(1,3)+16, RGN_FRAC(1,3)+0, 16, 0 },
	{ 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	16*32
};

static const gfx_layout spritelayout_6bpp =
{
	16,16,
	RGN_FRAC(1,3),
	6,
	{ RGN_FRAC(2,3)+16, RGN_FRAC(2,3)+0, RGN_FRAC(1,3)+16, RGN_FRAC(1,3)+0, 16, 0 },
	{ 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	16*32
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &spritelayout_4bpp,   0, 256 },
	{ -1 } /* end of array */
};

static const gfx_decode gfxdecodeinfo_5bpp[] =
{
	{ REGION_GFX1, 0, &spritelayout_5bpp,   0, 128 },
	{ -1 } /* end of array */
};

static const gfx_decode gfxdecodeinfo_6bpp[] =
{
	{ REGION_GFX1, 0, &spritelayout_6bpp,   0,  64 },
	{ -1 } /* end of array */
};

/******************************************************************************/

static MACHINE_RESET( mlc )
{
	raster_irq_timer = timer_alloc(interrupt_gen);
}

static struct YMZ280Binterface ymz280b_intf =
{
	REGION_SOUND1,
	0
};

static MACHINE_DRIVER_START( avengrgs )

	/* basic machine hardware */
	MDRV_CPU_ADD(SH2,42000000/2) /* 21 MHz clock confirmed on real board */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)

	MDRV_MACHINE_RESET(mlc)
	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_NVRAM_HANDLER(93C46) /* Actually 93c45 */

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_RGB_DIRECT | VIDEO_NEEDS_6BITS_PER_GUN)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(mlc)
	MDRV_VIDEO_UPDATE(mlc)
	MDRV_VIDEO_EOF(mlc)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YMZ280B, 42000000 / 3)
	MDRV_SOUND_CONFIG(ymz280b_intf)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( mlc )

	/* basic machine hardware */
	MDRV_CPU_ADD(ARM,42000000/6) /* 42 MHz -> 7MHz clock confirmed on real board */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)

	MDRV_MACHINE_RESET(mlc)
	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_NVRAM_HANDLER(93C46) /* Actually 93c45 */

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_RGB_DIRECT | VIDEO_NEEDS_6BITS_PER_GUN)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(mlc)
	MDRV_VIDEO_UPDATE(mlc)
	MDRV_VIDEO_EOF(mlc)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YMZ280B, 42000000 / 3)
	MDRV_SOUND_CONFIG(ymz280b_intf)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( mlc_6bpp )
	MDRV_IMPORT_FROM(mlc)
	MDRV_GFXDECODE(gfxdecodeinfo_6bpp)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( mlc_5bpp )
	MDRV_IMPORT_FROM(mlc)
	MDRV_GFXDECODE(gfxdecodeinfo_5bpp)
MACHINE_DRIVER_END

/***************************************************************************/

ROM_START( avengrgs )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD_SWAP( "sf_00-0.7k", 0x000002, 0x80000, CRC(7d20e2df) SHA1(e8be1751029aea74680ac00cd7f3cf84e1adfc56) )
	ROM_LOAD32_WORD_SWAP( "sf_01-0.7l", 0x000000, 0x80000, CRC(f37c0a01) SHA1(8c4e28cde9e93457197b1849e6c9ef9516b5732f) )

	ROM_REGION( 0x1800000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "mcg-00.1j", 0x000001, 0x200000, CRC(99129d9a) SHA1(1d1574e2326dca1043e05c229b54497df6ed5a35) )
	ROM_LOAD16_BYTE( "mcg-02.1f", 0x000000, 0x200000, CRC(29af9866) SHA1(56531911f8724975a7f81e61b7dec7fa72d50747) )
	ROM_LOAD16_BYTE( "mcg-01.1d", 0x400001, 0x200000, CRC(3638861b) SHA1(0896110acdb4442e4819f73285b9e725fc787b7a) )
	ROM_LOAD16_BYTE( "mcg-03.7m", 0x400000, 0x200000, CRC(4a0c965f) SHA1(b658ae5e6e2ff6f42b605bb6c49ad8a67507f2ab) )
	ROM_LOAD16_BYTE( "mcg-08.7p", 0x800001, 0x200000, CRC(c253943e) SHA1(b97a1d565ffbf2190ba0b25de5ef0bb3b9c9248b) )
	ROM_LOAD16_BYTE( "mcg-09.7n", 0x800000, 0x200000, CRC(8fb9870b) SHA1(046b6d07610cf09f008d1595605139071671d95c) )
	ROM_LOAD16_BYTE( "mcg-04.3j", 0xc00001, 0x200000, CRC(a4954c0e) SHA1(897a7313505f562879578941931a39afd34c9eef) )
	ROM_LOAD16_BYTE( "mcg-06.3f", 0xc00000, 0x200000, CRC(01571cf6) SHA1(74f85d523f2783374f041aa95abe6d1b8c872127) )
	ROM_LOAD16_BYTE( "mcg-05.3d", 0x1000001, 0x200000, CRC(182c2b49) SHA1(e53c06e95508e6c7e746f81668a4f7c08bfc6d36) )
	ROM_LOAD16_BYTE( "mcg-07.8m", 0x1000000, 0x200000, CRC(d09a3635) SHA1(8e184f3a3046bd8401762bbb480f5832fde91dde) )
	ROM_LOAD16_BYTE( "mcg-10.8p", 0x1400001, 0x200000, CRC(1383f524) SHA1(eadd8b579cc21ae119b7439c7882e39f22ac3b8c) )
	ROM_LOAD16_BYTE( "mcg-11.8n", 0x1400000, 0x200000, CRC(8f7fc281) SHA1(8cac51036088dbf4ff3c2b91ef88ef30a30b0be1) )

	ROM_REGION( 0x80000, REGION_GFX2, 0 )
	ROM_LOAD( "sf_02-0.6j", 0x000000, 0x80000, CRC(c98585dd) SHA1(752e246e2c72eb2b786c49d69f7ee4401a15c8aa) )

	ROM_REGION( 0x800000, REGION_SOUND1, ROMREGION_ERASE00 )
	ROM_LOAD( "mcg-12.5a",  0x000000, 0x200000, CRC(bef9b28f) SHA1(b7a2a0539ea4d22b48ce3f3eb367017f219da2c1) ) // basic coin sounds etc.
	ROM_LOAD( "mcg-13.9k",  0x200000, 0x200000, CRC(92301551) SHA1(a7891e7a3c8d7f165ca73f5d5a034501df46e9a2) ) // music
	ROM_LOAD( "mcg-14.6a",  0x400000, 0x200000, CRC(c0d8b5f0) SHA1(08eecf6e7d0273e41cda3472709a67e2b16068c9) ) // music
ROM_END

ROM_START( avengrgj )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD_SWAP( "sd_00-2.7k", 0x000002, 0x80000, CRC(136be46a) SHA1(7679f5f78f7983d43ecdb9bdd04e45792a13d9f2) )
	ROM_LOAD32_WORD_SWAP( "sd_01-2.7l", 0x000000, 0x80000, CRC(9d87f576) SHA1(dd20cd060d020d81f4e012be10d0211be7526641) )

	ROM_REGION( 0x1800000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "mcg-00.1j", 0x000001, 0x200000, CRC(99129d9a) SHA1(1d1574e2326dca1043e05c229b54497df6ed5a35) )
	ROM_LOAD16_BYTE( "mcg-02.1f", 0x000000, 0x200000, CRC(29af9866) SHA1(56531911f8724975a7f81e61b7dec7fa72d50747) )
	ROM_LOAD16_BYTE( "mcg-01.1d", 0x400001, 0x200000, CRC(3638861b) SHA1(0896110acdb4442e4819f73285b9e725fc787b7a) )
	ROM_LOAD16_BYTE( "mcg-03.7m", 0x400000, 0x200000, CRC(4a0c965f) SHA1(b658ae5e6e2ff6f42b605bb6c49ad8a67507f2ab) )
	ROM_LOAD16_BYTE( "mcg-08.7p", 0x800001, 0x200000, CRC(c253943e) SHA1(b97a1d565ffbf2190ba0b25de5ef0bb3b9c9248b) )
	ROM_LOAD16_BYTE( "mcg-09.7n", 0x800000, 0x200000, CRC(8fb9870b) SHA1(046b6d07610cf09f008d1595605139071671d95c) )
	ROM_LOAD16_BYTE( "mcg-04.3j", 0xc00001, 0x200000, CRC(a4954c0e) SHA1(897a7313505f562879578941931a39afd34c9eef) )
	ROM_LOAD16_BYTE( "mcg-06.3f", 0xc00000, 0x200000, CRC(01571cf6) SHA1(74f85d523f2783374f041aa95abe6d1b8c872127) )
	ROM_LOAD16_BYTE( "mcg-05.3d", 0x1000001, 0x200000, CRC(182c2b49) SHA1(e53c06e95508e6c7e746f81668a4f7c08bfc6d36) )
	ROM_LOAD16_BYTE( "mcg-07.8m", 0x1000000, 0x200000, CRC(d09a3635) SHA1(8e184f3a3046bd8401762bbb480f5832fde91dde) )
	ROM_LOAD16_BYTE( "mcg-10.8p", 0x1400001, 0x200000, CRC(1383f524) SHA1(eadd8b579cc21ae119b7439c7882e39f22ac3b8c) )
	ROM_LOAD16_BYTE( "mcg-11.8n", 0x1400000, 0x200000, CRC(8f7fc281) SHA1(8cac51036088dbf4ff3c2b91ef88ef30a30b0be1) )

	ROM_REGION( 0x80000, REGION_GFX2, 0 )
	ROM_LOAD( "sd_02-0.6j", 0x000000, 0x80000, CRC(24fc2b3c) SHA1(805eaa8e8ba49320ba83bda6307cc1d15d619358) )

	ROM_REGION( 0x800000, REGION_SOUND1, ROMREGION_ERASE00 )
	ROM_LOAD( "mcg-12.5a",  0x000000, 0x200000, CRC(bef9b28f) SHA1(b7a2a0539ea4d22b48ce3f3eb367017f219da2c1) ) // basic coin sounds etc.
	ROM_LOAD( "mcg-13.9k",  0x200000, 0x200000, CRC(92301551) SHA1(a7891e7a3c8d7f165ca73f5d5a034501df46e9a2) ) // music
	ROM_LOAD( "mcg-14.6a",  0x400000, 0x200000, CRC(c0d8b5f0) SHA1(08eecf6e7d0273e41cda3472709a67e2b16068c9) ) // music
ROM_END

ROM_START( stadhr96 )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD( "ead00-4.2a", 0x000000, 0x80000, CRC(b0adfc39) SHA1(3094dfb7c7f8fa9d7e10d98dff8fb8aba285d710) )
	ROM_LOAD32_WORD( "ead01-4.2b", 0x000002, 0x80000, CRC(0b332820) SHA1(28b757fe529250711fcb82424ba63c222a9329b9) )

	ROM_REGION( 0x1800000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "mcm-00.2e", 0x0000001, 0x400000, CRC(c1919c3c) SHA1(168000ff1512a147d7029ee8878dd70de680fb08) )
	ROM_LOAD16_BYTE( "mcm-01.8m", 0x0000000, 0x400000, CRC(2255d47d) SHA1(ba3298e781fce1c84f68290bc464f2bc991382c0) )
	ROM_LOAD16_BYTE( "mcm-02.4e", 0x0800001, 0x400000, CRC(38c39822) SHA1(393d2c1c3c0bcb99df706d32ee3f8b681891dcac) )
	ROM_LOAD16_BYTE( "mcm-03.10m",0x0800000, 0x400000, CRC(4bd84ca7) SHA1(43dad8ced344f8d629d36f30ab2332879ba067d2) )
	ROM_LOAD16_BYTE( "mcm-04.6e", 0x1000001, 0x400000, CRC(7c0bd84c) SHA1(730b085a893d3c70592a8b4aecaeeaf4aceede56) )
	ROM_LOAD16_BYTE( "mcm-05.11m",0x1000000, 0x400000, CRC(476f03d7) SHA1(5c58ab4fc0e29f76619827bc27fa64cce2627e48) )

	ROM_REGION( 0x80000, REGION_GFX2, 0 )
	ROM_LOAD( "ead02-0.6h", 0x000000, 0x80000, CRC(f95ad7ce) SHA1(878dcc1d5f76c8523c788e66bb4a8c5740d515e5) )

	ROM_REGION( 0x800000, REGION_SOUND1, 0 )
	ROM_LOAD( "mcm-06.6a",  0x000000, 0x400000,  CRC(fbc178f3) SHA1(f44cb913177b6552b30c139505c3284bc445ba13) )
ROM_END

ROM_START( stadh96a )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD( "sh-eaj.2a", 0x000000, 0x80000, CRC(10d1496a) SHA1(1dc151547463a38d717159b3dfce7ffd78a943ad) )
	ROM_LOAD32_WORD( "sh-eaj.2b", 0x000002, 0x80000, CRC(608a9144) SHA1(15e2fa99dc96e8ebd9868713ae7708cb824fc6c5) )

	ROM_REGION( 0x1800000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "mcm-00.2e", 0x0000001, 0x400000, CRC(c1919c3c) SHA1(168000ff1512a147d7029ee8878dd70de680fb08) )
	ROM_LOAD16_BYTE( "mcm-01.8m", 0x0000000, 0x400000, CRC(2255d47d) SHA1(ba3298e781fce1c84f68290bc464f2bc991382c0) )
	ROM_LOAD16_BYTE( "mcm-02.4e", 0x0800001, 0x400000, CRC(38c39822) SHA1(393d2c1c3c0bcb99df706d32ee3f8b681891dcac) )
	ROM_LOAD16_BYTE( "mcm-03.10m",0x0800000, 0x400000, CRC(4bd84ca7) SHA1(43dad8ced344f8d629d36f30ab2332879ba067d2) )
	ROM_LOAD16_BYTE( "mcm-04.6e", 0x1000001, 0x400000, CRC(7c0bd84c) SHA1(730b085a893d3c70592a8b4aecaeeaf4aceede56) )
	ROM_LOAD16_BYTE( "mcm-05.11m",0x1000000, 0x400000, CRC(476f03d7) SHA1(5c58ab4fc0e29f76619827bc27fa64cce2627e48) )

	ROM_REGION( 0x80000, REGION_GFX2, 0 )
	ROM_LOAD( "sh-eaf.6h", 0x000000, 0x80000, CRC(f074a5c8) SHA1(72709cc0ac2d0df19393b405d8f927834f563e69) )

	ROM_REGION( 0x400000, REGION_SOUND1, 0 )
	ROM_LOAD( "mcm-06.6a",  0x000000, 0x400000,  CRC(fbc178f3) SHA1(f44cb913177b6552b30c139505c3284bc445ba13) )
ROM_END

ROM_START( hoops96 )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD( "sz00-0.2a", 0x000000, 0x80000, CRC(971b4376) SHA1(e60d8d628bd1dc95d7f2b8840b0b188e68905c12) )
	ROM_LOAD32_WORD( "sz01-0.2b", 0x000002, 0x80000, CRC(b9679d7b) SHA1(3510b97390f2214cedb3387d32c7a7fd639a0a6e) )

	ROM_REGION( 0x0c00000, REGION_GFX1,ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "mce-00.2e", 0x0000001, 0x200000, CRC(11b9bd96) SHA1(ed17fa9008b8e42951fd1f4c50939f1dd99cfeaf) )
	ROM_LOAD16_BYTE( "mce-01.8m", 0x0000000, 0x200000, CRC(6817d0c6) SHA1(ac1ee407b3981e0a9d45c429d301a93997f52c35) )
	ROM_LOAD16_BYTE( "mce-02.4e", 0x0400001, 0x200000, CRC(be7ff8ba) SHA1(40991d000dfbe7fc7f4f053e14c1b7b0b3cf2865) )
	ROM_LOAD16_BYTE( "mce-03.10m",0x0400000, 0x200000, CRC(756c282e) SHA1(5095bf8d8aae8133543bdc3f5b787efd403a5cf6) )
	ROM_LOAD32_WORD_SWAP( "mce-04.8n", 0x0800000, 0x200000, CRC(91da9b4f) SHA1(25c3a7abbaca006ad345150b5d689faf8b13affb) ) // extra plane of gfx, needs rearranging to decode

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_ERASEFF ) /* Code Lookup */
	ROM_LOAD( "rr02-0.6h", 0x020000, 0x20000, CRC(9490041c) SHA1(febedd0683dbcb080d304d03e4a3b501caeb6bb8) )

	ROM_REGION( 0x400000, REGION_SOUND1, 0 )
	ROM_LOAD( "mce-05.6a",  0x000000, 0x400000,  CRC(e7a9355a) SHA1(039b23666e224c33ebb02baa80e496f8bce0514f) )
ROM_END

ROM_START( hoops95 )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD( "hoops.a2", 0x000000, 0x80000, CRC(02b8c61a) SHA1(ae49f6bd8a3bffa181fa6a2740d92287e9d5dc02) )
	ROM_LOAD32_WORD( "hoops.b2", 0x000002, 0x80000, CRC(b54c63d1) SHA1(21d9327675226a6e60d0447d461d7897212db33a) )

	ROM_REGION( 0x0c00000, REGION_GFX1,ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "mce-00.2e", 0x0000001, 0x200000, CRC(11b9bd96) SHA1(ed17fa9008b8e42951fd1f4c50939f1dd99cfeaf) )
	ROM_LOAD16_BYTE( "mce-01.8m", 0x0000000, 0x200000, CRC(6817d0c6) SHA1(ac1ee407b3981e0a9d45c429d301a93997f52c35) )
	ROM_LOAD16_BYTE( "mce-02.4e", 0x0400001, 0x200000, CRC(be7ff8ba) SHA1(40991d000dfbe7fc7f4f053e14c1b7b0b3cf2865) )
	ROM_LOAD16_BYTE( "mce-03.10m",0x0400000, 0x200000, CRC(756c282e) SHA1(5095bf8d8aae8133543bdc3f5b787efd403a5cf6) )
	ROM_LOAD32_WORD_SWAP( "mce-04.8n", 0x0800000, 0x200000, CRC(91da9b4f) SHA1(25c3a7abbaca006ad345150b5d689faf8b13affb) ) // extra plane of gfx, needs rearranging to decode

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_ERASEFF ) /* Code Lookup */
	ROM_LOAD( "hoops.h6", 0x020000, 0x20000, CRC(bc702733) SHA1(830d5da5eb53727ff4221c59c5676e01e444aa27) )

	ROM_REGION( 0x400000, REGION_SOUND1, 0 )
	ROM_LOAD( "mce-05.6a",  0x000000, 0x400000,  CRC(e7a9355a) SHA1(039b23666e224c33ebb02baa80e496f8bce0514f) )
ROM_END

ROM_START( ddream95 )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD( "rl00-2.2a", 0x000000, 0x80000, CRC(07645092) SHA1(5f24bd6102b7e6212888b703f86bed5a19e08e85) )
	ROM_LOAD32_WORD( "rl01-2.2b", 0x000002, 0x80000, CRC(cfc629fc) SHA1(c0bcfa75c6446def4af99b14a1a869b5576c244f) )

	ROM_REGION( 0x0c00000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "mce-00.2e", 0x0000001, 0x200000, CRC(11b9bd96) SHA1(ed17fa9008b8e42951fd1f4c50939f1dd99cfeaf) )
	ROM_LOAD16_BYTE( "mce-01.8m", 0x0000000, 0x200000, CRC(6817d0c6) SHA1(ac1ee407b3981e0a9d45c429d301a93997f52c35) )
	ROM_LOAD16_BYTE( "mce-02.4e", 0x0400001, 0x200000, CRC(be7ff8ba) SHA1(40991d000dfbe7fc7f4f053e14c1b7b0b3cf2865) )
	ROM_LOAD16_BYTE( "mce-03.10m",0x0400000, 0x200000, CRC(756c282e) SHA1(5095bf8d8aae8133543bdc3f5b787efd403a5cf6) )
	ROM_LOAD32_WORD_SWAP( "mce-04.8n", 0x0800000, 0x200000, CRC(91da9b4f) SHA1(25c3a7abbaca006ad345150b5d689faf8b13affb) ) // extra plane of gfx, needs rearranging to decode

	ROM_REGION( 0x80000, REGION_GFX2, 0 )
	ROM_LOAD( "rl02-0.6h", 0x020000, 0x20000, CRC(9490041c) SHA1(febedd0683dbcb080d304d03e4a3b501caeb6bb8) )

	ROM_REGION( 0x400000, REGION_SOUND1, 0 )
	ROM_LOAD( "mce-05.6a",  0x000000, 0x400000,  CRC(e7a9355a) SHA1(039b23666e224c33ebb02baa80e496f8bce0514f) )
ROM_END

ROM_START( skullfng )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )
	ROM_LOAD32_WORD( "sh00-0.2a", 0x000000, 0x80000, CRC(e50358e8) SHA1(e66ac5e1b16273cb905254c99b2bce435145a414) )
	ROM_LOAD32_WORD( "sh01-0.2b", 0x000002, 0x80000, CRC(2c288bcc) SHA1(4ed1d5818362383240378056bf575f6acf8a593a) )

	ROM_REGION( 0xc00000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "mch-00.2e", 0x000001, 0x200000, CRC(d5cc4238) SHA1(f1bd86386e44a3f600475aeab310f7ea632998df) )
	ROM_LOAD16_BYTE( "mch-01.8m", 0x000000, 0x200000, CRC(d37cf0cd) SHA1(c2fe7062a123ca2df65217c6dced857b803d8a8d) )
	ROM_LOAD16_BYTE( "mch-02.4e", 0x400001, 0x200000, CRC(4046314d) SHA1(32e3b7ddbe20ffa6ba6ebe9bd55a32e3b3a120f6) )
	ROM_LOAD16_BYTE( "mch-03.10m",0x400000, 0x200000, CRC(1dea8f6c) SHA1(c2ad59592385a00e323aac9057906c9384b67078) )
	ROM_LOAD16_BYTE( "mch-04.6e", 0x800001, 0x200000, CRC(4869dfe8) SHA1(296df6274ecb3eed485de24258cf462e3942f1fa) )
	ROM_LOAD16_BYTE( "mch-05.11m",0x800000, 0x200000, CRC(ef0b54ba) SHA1(3be56c064ac81686096be5f31ad2aad948ba6701) )

	ROM_REGION( 0x80000, REGION_GFX2, 0 )
	ROM_LOAD( "sh02-0.6h", 0x000000, 0x80000, CRC(0d3ae757) SHA1(480fc3855d330380b75a47a271f3571a59aee10c) )

	ROM_REGION( 0x800000, REGION_SOUND1, ROMREGION_ERASE00 )
	ROM_LOAD( "mch-06.6a",  0x200000, 0x200000, CRC(b2efe4ae) SHA1(5a9dab74c2ba73a65e8f1419b897467804734fa2) )
	ROM_LOAD( "mch-07.11j", 0x400000, 0x200000, CRC(bc1a50a1) SHA1(3de191fbc92d2ae84e54263f1c70afec6ff7cc3c) )
ROM_END

/***************************************************************************/

static void descramble_sound( void )
{
	/* the same as simpl156 / heavy smash? */
	UINT8 *rom = memory_region(REGION_SOUND1);
	int length = memory_region_length(REGION_SOUND1);
	UINT8 *buf1 = malloc(length);

	UINT32 x;

	for (x=0;x<length;x++)
	{
		UINT32 addr;

		addr = BITSWAP24 (x,23,22,21,0, 20,
		                    19,18,17,16,
		                    15,14,13,12,
		                    11,10,9, 8,
		                    7, 6, 5, 4,
		                    3, 2, 1 );

		buf1[addr] = rom[x];
	}

	memcpy(rom,buf1,length);

	free (buf1);
}

static READ32_HANDLER( avengrgs_speedup_r )
{
	UINT32 a=mlc_ram[0x89a0/4];
	UINT32 p=activecpu_get_pc();

	if ((p==0x3236 || p==0x32de) && (a&1)) cpu_spinuntil_int();

	return a;
}

static DRIVER_INIT( avengrgs )
{
	mainCpuIsArm=0;
	memory_install_read32_handler(0, ADDRESS_SPACE_PROGRAM, 0x01089a0, 0x01089a3, 0, 0, avengrgs_speedup_r );
	descramble_sound();
}

static DRIVER_INIT( mlc )
{
	/* The timing in the ARM core isn't as accurate as it should be, so bump up the
        effective clock rate here to compensate otherwise we have slowdowns in
        Skull Fung where there probably shouldn't be. */
	cpunum_set_clockscale(0, 2.0f);
	mainCpuIsArm=1;
	decrypt156();
	descramble_sound();
}

/***************************************************************************/

GAME( 1995, avengrgs, 0,        avengrgs, mlc, avengrgs, ROT0,   "Data East Corporation", "Avengers In Galactic Storm (US)", 0 ,0)
GAME( 1995, avengrgj, avengrgs, avengrgs, mlc, avengrgs, ROT0,   "Data East Corporation", "Avengers In Galactic Storm (Japan)", 0 ,0)
GAME( 1996, stadhr96, 0,        mlc_6bpp, mlc, mlc,      ROT0,   "Data East Corporation", "Stadium Hero 96 (Version EAD)", GAME_UNEMULATED_PROTECTION | GAME_NOT_WORKING ,0)
GAME( 1996, stadh96a, stadhr96, mlc_6bpp, mlc, mlc,      ROT0,   "Data East Corporation", "Stadium Hero 96 (Version EAJ)", GAME_UNEMULATED_PROTECTION | GAME_NOT_WORKING ,0)
GAME( 1996, skullfng, 0,        mlc_6bpp, mlc, mlc,      ROT270, "Data East Corporation", "Skull Fang (Japan)", 0 ,0)
GAME( 1996, hoops96,  0,        mlc_5bpp, mlc, mlc,      ROT0,   "Data East Corporation", "Hoops '96 (Europe/Asia 2.0)", 0 ,0)
GAME( 1995, ddream95, hoops96,  mlc_5bpp, mlc, mlc,      ROT0,   "Data East Corporation", "Dunk Dream '95 (Japan 1.4 EAM)", 0 ,0)
GAME( 1995, hoops95,  hoops96,  mlc_5bpp, mlc, mlc,      ROT0,   "Data East Corporation", "Hoops (Europe/Asia 1.7)", 0 ,0)
