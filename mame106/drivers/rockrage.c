/***************************************************************************

Rock'n'Rage(GX620) (c) 1986 Konami

Driver by Manuel Abadia <manu@teleline.es>

GX620 PWB302109A
|------------------------------------------------------|
| LA4445  VOL1 JP1  LM324    6809    6309              |
|         VOL2 JP2  YM2151                             |
|         YM3012 3.579545MHz                           |
|                                                      |
|       CN1                                            |
|         LM324                007420                  |
|                    6116 620G3.11C   6264  620N2.15C  |
|                                             620N1.16C|
|J                                                     |
|A                                                     |
|M                                   24MHz             |
|M        620D4.6E                                     |
|A                             |-------|               |
|            VLM5030           |007342 |               |
|  DSW3(4) DSW2(8)             |       |         6264  |
|          DSW1(8) 620D11B.7F  |       |620D6B.15F     |
|    |-----------|   620G10B.8F|-------|  620D5B.16F   |
|    |           |                                     |
|    |  007327   |            620D9.11G                |
|    |           | 620G11A.7G   620D8.12G  620D6A.15G  |
|    |-----------|    620D10A.8G  620D7.13G  620D5A.16G|
|------------------------------------------------------|

Notes:
      6809 clock 1.500MHz [24/16]
      6309 clock 3.000MHz [24/8]
      VLM5030 clock 3.579545MHz
      YM2151 clock 3.579545MHz
      VSync 60Hz
      HSync 15.16kHz
      JP1/JP2 - 4-pin jumper to set stereo/mono output
      CN1 - 4 pin right speaker sound output connector
      6116 - 2k x8 SRAM (DIP24)
      6264 - 8k x8 SRAM (DIP28)
      Konami custom ICs -
                          007342 3905 67 3147B (PGA181)
                          007420 3916 67 23 52 A (SDIP64)
                          007327 (custom ceramic wide DIP40)

***************************************************************************/

#include "driver.h"
#include "cpu/m6809/m6809.h"
#include "cpu/hd6309/hd6309.h"
#include "vidhrdw/konamiic.h"
#include "sound/2151intf.h"
#include "sound/vlm5030.h"

extern int rockrage_irq_enable;

/* from vidhrdw */
VIDEO_START( rockrage );
VIDEO_UPDATE( rockrage );
WRITE8_HANDLER( rockrage_vreg_w );
PALETTE_INIT( rockrage );

static INTERRUPT_GEN( rockrage_interrupt )
{
	if (K007342_is_INT_enabled())
        cpunum_set_input_line(0, HD6309_IRQ_LINE, HOLD_LINE);
}

static WRITE8_HANDLER( rockrage_bankswitch_w )
{
	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU1);

	/* bits 4-6 = bank number */
	bankaddress = 0x10000 + ((data & 0x70) >> 4) * 0x2000;
	memory_set_bankptr(1,&RAM[bankaddress]);

	/* bits 0 & 1 = coin counters */
	coin_counter_w(0,data & 0x01);
	coin_counter_w(1,data & 0x02);

	/* other bits unknown */
}

static WRITE8_HANDLER( rockrage_sh_irqtrigger_w )
{
	soundlatch_w(offset, data);
	cpunum_set_input_line(1,M6809_IRQ_LINE,HOLD_LINE);
}

static READ8_HANDLER( rockrage_VLM5030_busy_r ) {
	return ( VLM5030_BSY() ? 1 : 0 );
}

static WRITE8_HANDLER( rockrage_speech_w ) {
	/* bit2 = data bus enable */
	VLM5030_RST( ( data >> 1 ) & 0x01 );
	VLM5030_ST(  ( data >> 0 ) & 0x01 );
}

static ADDRESS_MAP_START( rockrage_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_READ(K007342_r)			/* Color RAM + Video RAM */
	AM_RANGE(0x2000, 0x21ff) AM_READ(K007420_r)			/* Sprite RAM */
	AM_RANGE(0x2200, 0x23ff) AM_READ(K007342_scroll_r)	/* Scroll RAM */
	AM_RANGE(0x2400, 0x247f) AM_READ(paletteram_r)		/* Palette */
	AM_RANGE(0x2e01, 0x2e01) AM_READ(input_port_3_r)		/* 1P controls */
	AM_RANGE(0x2e02, 0x2e02) AM_READ(input_port_4_r)		/* 2P controls */
	AM_RANGE(0x2e03, 0x2e03) AM_READ(input_port_1_r)		/* DISPW #2 */
	AM_RANGE(0x2e40, 0x2e40) AM_READ(input_port_0_r)		/* DIPSW #1 */
	AM_RANGE(0x2e00, 0x2e00) AM_READ(input_port_2_r)		/* coinsw, testsw, startsw */
	AM_RANGE(0x4000, 0x5fff) AM_READ(MRA8_RAM)			/* RAM */
	AM_RANGE(0x6000, 0x7fff) AM_READ(MRA8_BANK1)			/* banked ROM */
	AM_RANGE(0x8000, 0xffff) AM_READ(MRA8_ROM)			/* ROM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( rockrage_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_WRITE(K007342_w)				/* Color RAM + Video RAM */
	AM_RANGE(0x2000, 0x21ff) AM_WRITE(K007420_w)				/* Sprite RAM */
	AM_RANGE(0x2200, 0x23ff) AM_WRITE(K007342_scroll_w)		/* Scroll RAM */
	AM_RANGE(0x2400, 0x247f) AM_WRITE(paletteram_xBBBBBGGGGGRRRRR_le_w) AM_BASE(&paletteram)/* palette */
	AM_RANGE(0x2600, 0x2607) AM_WRITE(K007342_vreg_w)			/* Video Registers */
	AM_RANGE(0x2e80, 0x2e80) AM_WRITE(rockrage_sh_irqtrigger_w)/* cause interrupt on audio CPU */
	AM_RANGE(0x2ec0, 0x2ec0) AM_WRITE(watchdog_reset_w)		/* watchdog reset */
	AM_RANGE(0x2f00, 0x2f00) AM_WRITE(rockrage_vreg_w)		/* ??? */
	AM_RANGE(0x2f40, 0x2f40) AM_WRITE(rockrage_bankswitch_w)	/* bankswitch control */
	AM_RANGE(0x4000, 0x5fff) AM_WRITE(MWA8_RAM)				/* RAM */
	AM_RANGE(0x6000, 0x7fff) AM_WRITE(MWA8_RAM)				/* banked ROM */
	AM_RANGE(0x8000, 0xffff) AM_WRITE(MWA8_ROM)				/* ROM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( rockrage_readmem_sound, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x3000, 0x3000) AM_READ(rockrage_VLM5030_busy_r)/* VLM5030 */
	AM_RANGE(0x5000, 0x5000) AM_READ(soundlatch_r)			/* soundlatch_r */
	AM_RANGE(0x6001, 0x6001) AM_READ(YM2151_status_port_0_r)	/* YM 2151 */
	AM_RANGE(0x7000, 0x77ff) AM_READ(MRA8_RAM)				/* RAM */
	AM_RANGE(0x8000, 0xffff) AM_READ(MRA8_ROM)				/* ROM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( rockrage_writemem_sound, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x2000, 0x2000) AM_WRITE(VLM5030_data_w) 			/* VLM5030 */
	AM_RANGE(0x4000, 0x4000) AM_WRITE(rockrage_speech_w)			/* VLM5030 */
	AM_RANGE(0x6000, 0x6000) AM_WRITE(YM2151_register_port_0_w)	/* YM 2151 */
	AM_RANGE(0x6001, 0x6001) AM_WRITE(YM2151_data_port_0_w)		/* YM 2151 */
	AM_RANGE(0x7000, 0x77ff) AM_WRITE(MWA8_RAM)					/* RAM */
	AM_RANGE(0x8000, 0xffff) AM_WRITE(MWA8_ROM)					/* ROM */
ADDRESS_MAP_END

/***************************************************************************

    Input Ports

***************************************************************************/

INPUT_PORTS_START( rockrage )
	PORT_START	/* DSW #1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
//  PORT_DIPSETTING(    0x00, "Invalid" )

	PORT_START	/* DSW #2 */
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "1" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x08, "30k and every 70k" )
	PORT_DIPSETTING(    0x00, "40k and every 80k" )
	PORT_DIPNAME( 0x10, 0x10, "Freeze Screen" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* COINSW */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START	/* PLAYER 1 INPUTS */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* PLAYER 2 INPUTS */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,			/* 8*8 characters */
	0x20000/16,		/* 8192 characters */
	4,				/* 4 bpp */
	{ 0, 1, 2, 3 },	/* the four bitplanes are packed in one nibble */
	{ 0*4, 1*4, 0x20000*8+0*4, 0x20000*8+1*4, 2*4, 3*4, 0x20000*8+2*4, 0x20000*8+3*4 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8		/* every char takes 16 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	8,8,			/* 8*8 sprites */
	0x40000/32,	/* 8192 sprites */
	4,				/* 4 bpp */
	{ 0, 1, 2, 3 },	/* the four bitplanes are packed in one nibble */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8			/* every sprite takes 32 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   64, 32 },	/* colors 00..31, but using 2 lookup tables */
	{ REGION_GFX2, 0, &spritelayout, 32,  1 },	/* colors 32..63 */
	{ -1 } /* end of array */
};

/***************************************************************************

    Machine Driver

***************************************************************************/

static struct VLM5030interface vlm5030_interface =
{
	REGION_SOUND1,	/* memory region of speech rom */
	0
};

static MACHINE_DRIVER_START( rockrage )

	/* basic machine hardware */
	MDRV_CPU_ADD(HD6309, 3000000)		/* 24MHz/8 */
	MDRV_CPU_PROGRAM_MAP(rockrage_readmem,rockrage_writemem)
	MDRV_CPU_VBLANK_INT(rockrage_interrupt,1)

	MDRV_CPU_ADD(M6809, 1500000)		/* 24MHz/16 */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(rockrage_readmem_sound,rockrage_writemem_sound)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64)
	MDRV_COLORTABLE_LENGTH(64 + 2*16*16)

	MDRV_PALETTE_INIT(rockrage)
	MDRV_VIDEO_START(rockrage)
	MDRV_VIDEO_UPDATE(rockrage)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2151, 3579545)
	MDRV_SOUND_ROUTE(0, "mono", 0.60)
	MDRV_SOUND_ROUTE(1, "mono", 0.60)

	MDRV_SOUND_ADD(VLM5030, 3579545)
	MDRV_SOUND_CONFIG(vlm5030_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)
MACHINE_DRIVER_END


/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( rockrage )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* code + banked roms */
	ROM_LOAD( "620q01.16c", 0x08000, 0x08000, CRC(0ddb5ef5) SHA1(71b38c9f957858371f0ac95720d3c6d07339e5c5) )	/* fixed ROM */
	ROM_LOAD( "620q02.15c", 0x10000, 0x10000, CRC(b4f6e346) SHA1(43fded4484836ff315dd6e40991f909dad73f1ed) )	/* banked ROM */

	ROM_REGION(  0x10000 , REGION_CPU2, 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "620k03.11c", 0x08000, 0x08000, CRC(9fbefe82) SHA1(ab42b7e519a0dd08f2249dad0819edea0976f39a) )

	ROM_REGION( 0x040000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "620k06.15g",	0x000000, 0x20000, BAD_DUMP CRC(c0e2b35c) SHA1(fb37a151188f27f883fed5fdfb0094c3efa9470d)  )	/* tiles */
	ROM_LOAD( "620k05.16g",	0x020000, 0x20000, BAD_DUMP CRC(ca9d9346) SHA1(fee8d98def802f312c6cd0ec751c67aa18acfacd)  )

	ROM_REGION( 0x040000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "620k11.rom",	0x000000, 0x20000, CRC(70449239) SHA1(07653ea3bfe0063c9d2b2102ac52a1b50fc2971e) )	/* sprites */
	ROM_LOAD( "620l10.8g",	0x020000, 0x20000, CRC(06d108e0) SHA1(cae8c5f2fc4e84bc7adbf27f71a18a74968c4296) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "620k09.11g", 0x00000, 0x00100, CRC(9f0e0608) SHA1(c95bdb370e4a91f27afbd5ff3b39b2e0ad87da73) )	/* layer 0 lookup table */
	ROM_LOAD( "620k08.12g", 0x00100, 0x00100, CRC(b499800c) SHA1(46fa4e071ebceed12027de109be1e16dde5e846e) )	/* layer 1 lookup table */
	ROM_LOAD( "620k07.13g", 0x00200, 0x00100, CRC(b6135ee0) SHA1(248a978987cff86c2bbad10ef332f63a6abd5bee) )	/* sprite lookup table, but its not used */
															/* because it's always 0 1 2 ... f */
	ROM_REGION( 0x08000, REGION_SOUND1, 0 ) /* VLM3050 data */
	ROM_LOAD( "620k04.6e", 0x00000, 0x08000, CRC(8be969f3) SHA1(9856b4c13fac77b645aed67a08cb4965b4966492) )
ROM_END

ROM_START( rockraga )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* code + banked roms */
	ROM_LOAD( "620n01.16c", 0x08000, 0x10000, CRC(f89f56ea) SHA1(64ba2575e09af257b242d913eab69130f7341894) )	/* fixed ROM */
	ROM_LOAD( "620n02.15c", 0x10000, 0x10000, CRC(5bc1f1cf) SHA1(d5bb9971d778449e0c01495f9888c0da7ac617a7) )	/* banked ROM */

	ROM_REGION(  0x10000 , REGION_CPU2, 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "620k03.11c", 0x08000, 0x08000, CRC(9fbefe82) SHA1(ab42b7e519a0dd08f2249dad0819edea0976f39a) ) /* Same rom but labeled as ver "G" */

	ROM_REGION( 0x040000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "620d06a.15g", 0x000000, 0x10000, CRC(8cc05d4b) SHA1(0d6fef98bdc4d299229de4e0044241aedee83b85)  )	/* tiles */
	ROM_LOAD( "620d06b.15f", 0x010000, 0x10000, CRC(3892d41d) SHA1(c49f2e61f24a59be4e59e2f3c60e731b8a05ddd3) )
	ROM_LOAD( "620d05a.16g", 0x020000, 0x10000, CRC(4d53fde9) SHA1(941fb6c94922727516945330b4b738aa052f7734) )
	ROM_LOAD( "620d05b.16f", 0x030000, 0x10000, CRC(69f4599f) SHA1(664581874d74ed7bf59bde6730799e15f4e0144d) )

	ROM_REGION( 0x040000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "620g11a.7g",	0x000000, 0x10000, CRC(0ef40c2c) SHA1(2c0b7e611333a072ebcef60c1985211d5936bf66) )	/* sprites */
	ROM_LOAD( "620d11b.7f",	0x010000, 0x10000, CRC(8f116cbf) SHA1(0400609aadde39c6f02ab954c78bc67a1d23da1d) )
	ROM_LOAD( "620d10a.8g",	0x020000, 0x10000, CRC(4789ae7b) SHA1(8885ca20bf746fb3ed229486c0e3903ababfacc9) )
	ROM_LOAD( "620g10b.8f",	0x030000, 0x10000, CRC(1618854a) SHA1(0afb34a9ed97f13c1910acd7767cb8546ea7e6cd) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "620k09.11g", 0x00000, 0x00100, CRC(9f0e0608) SHA1(c95bdb370e4a91f27afbd5ff3b39b2e0ad87da73) )	/* layer 0 lookup table */
	ROM_LOAD( "620k08.12g", 0x00100, 0x00100, CRC(b499800c) SHA1(46fa4e071ebceed12027de109be1e16dde5e846e) )	/* layer 1 lookup table */
	ROM_LOAD( "620k07.13g", 0x00200, 0x00100, CRC(b6135ee0) SHA1(248a978987cff86c2bbad10ef332f63a6abd5bee) )	/* sprite lookup table, but its not used */
															/* because it's always 0 1 2 ... f */
	ROM_REGION( 0x08000, REGION_SOUND1, 0 ) /* VLM3050 data */
	ROM_LOAD( "620k04.6e", 0x00000, 0x08000, CRC(8be969f3) SHA1(9856b4c13fac77b645aed67a08cb4965b4966492) ) /* Same rom but labeled as ver "G" */
ROM_END

ROM_START( rockragj )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* code + banked roms */
	ROM_LOAD( "620k01.16c", 0x08000, 0x08000, CRC(4f5171f7) SHA1(5bce9e3f9d01c113c697853763cd891b91297eb2) )	/* fixed ROM */
	ROM_LOAD( "620k02.15c", 0x10000, 0x10000, CRC(04c4d8f7) SHA1(2a1a024fc38bb934c454092b0aed74d0f1d1c4af) )	/* banked ROM */

	ROM_REGION(  0x10000 , REGION_CPU2, 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "620k03.11c", 0x08000, 0x08000, CRC(9fbefe82) SHA1(ab42b7e519a0dd08f2249dad0819edea0976f39a) )

	ROM_REGION( 0x040000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "620k06.15g",	0x000000, 0x20000, CRC(c0e2b35c) SHA1(fb37a151188f27f883fed5fdfb0094c3efa9470d) )	/* tiles */
	ROM_LOAD( "620k05.16g",	0x020000, 0x20000, CRC(ca9d9346) SHA1(fee8d98def802f312c6cd0ec751c67aa18acfacd) )

	ROM_REGION( 0x040000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "620k11.7g",	0x000000, 0x20000, CRC(7430f6e9) SHA1(5d488c7b7b0eb4e502b3e566ac102cd3267e8568) )	/* sprites */
	ROM_LOAD( "620k10.8g",	0x020000, 0x20000, CRC(0d1a95ab) SHA1(be565424f17af31dcd07004c6be03bbb00aef514) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "620k09.11g", 0x00000, 0x00100, CRC(9f0e0608) SHA1(c95bdb370e4a91f27afbd5ff3b39b2e0ad87da73) )	/* layer 0 lookup table */
	ROM_LOAD( "620k08.12g", 0x00100, 0x00100, CRC(b499800c) SHA1(46fa4e071ebceed12027de109be1e16dde5e846e) )	/* layer 1 lookup table */
	ROM_LOAD( "620k07.13g", 0x00200, 0x00100, CRC(b6135ee0) SHA1(248a978987cff86c2bbad10ef332f63a6abd5bee) )	/* sprite lookup table, but its not used */
															/* because it's always 0 1 2 ... f */
	ROM_REGION( 0x08000, REGION_SOUND1, 0 ) /* VLM3050 data */
	ROM_LOAD( "620k04.6e", 0x00000, 0x08000, CRC(8be969f3) SHA1(9856b4c13fac77b645aed67a08cb4965b4966492) )
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

GAME( 1986, rockrage, 0,        rockrage, rockrage, 0, ROT0, "Konami", "Rock 'n Rage (World?)", 0 ,0)
GAME( 1986, rockraga, rockrage, rockrage, rockrage, 0, ROT0, "Konami", "Rock 'n Rage (Prototype?)", 0 ,0)
GAME( 1986, rockragj, rockrage, rockrage, rockrage, 0, ROT0, "Konami", "Koi no Hotrock (Japan)", 0 ,0)
