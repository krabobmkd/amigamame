/* Jaleco's Big Striker (bootleg version)

 Driver by David Haywood
 Inputs by Stephh

 maybe it could be merged with megasys1.c, could be messy

 todo:

 complete sound (YM2151 like megasys1?)
 sprite lag (buffers spriteram?)

*/

#include "driver.h"
#include "sound/okim6295.h"

/*

68k interrupts
lev 1 : 0x64 : 0000 0406 - ?
lev 2 : 0x68 : 0000 0434 - ?
lev 3 : 0x6c : 0000 05be - xxx
lev 4 : 0x70 : 0000 04d2 - ?
lev 5 : 0x74 : 0000 05be - xxx
lev 6 : 0x78 : 0004 0000 - vblank?
lev 7 : 0x7c : 0000 05be - xxx

*/

UINT16 *bsb_videoram, *bsb_videoram2, *bsb_videoram3;
UINT16 *bsb_vidreg1, *bsb_vidreg2;
UINT16 *bigstrkb_spriteram;

WRITE16_HANDLER( bsb_videoram_w );
WRITE16_HANDLER( bsb_videoram2_w );
WRITE16_HANDLER( bsb_videoram3_w );
VIDEO_START(bigstrkb);
VIDEO_UPDATE(bigstrkb);


/* Memory Maps */

/* some regions might be too large */

static ADDRESS_MAP_START( bigstrkb_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_READ(MRA16_ROM)
	/* most region sizes are unknown */
//  AM_RANGE(0x0c0000, 0x0cffff) AM_READ(megasys1_vregs_C_r)
	AM_RANGE(0x0D0000, 0x0dffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0E0000, 0x0E3fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0e8000, 0x0ebfff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0ec000, 0x0effff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0f0000, 0x0fffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x1f0000, 0x1fffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x700000, 0x700001) AM_READ(input_port_0_word_r) /* Dip 1 */
	AM_RANGE(0x700002, 0x700003) AM_READ(input_port_1_word_r) /* Dip 2 */
	AM_RANGE(0x700004, 0x700005) AM_READ(input_port_2_word_r) /* System */
	AM_RANGE(0x70000a, 0x70000b) AM_READ(input_port_4_word_r) /* Player 1 */
	AM_RANGE(0x70000c, 0x70000d) AM_READ(input_port_3_word_r) /* Player 2 */

	AM_RANGE(0xE00000, 0xE00001) AM_READ(OKIM6295_status_0_lsb_r)
	AM_RANGE(0xE00002, 0xE00003) AM_READ(OKIM6295_status_1_lsb_r)

	AM_RANGE(0xF00000, 0xFFFFFF) AM_READ(MRA16_RAM)	// main RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( bigstrkb_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_WRITE(MWA16_ROM)
//  AM_RANGE(0x0c0000, 0x0cffff) AM_WRITE(megasys1_vregs_C_w) AM_BASE(&megasys1_vregs)

	AM_RANGE(0x0C2004, 0x0C2005) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x0C200C, 0x0C200d) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x0C2104, 0x0C2105) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x0C2108, 0x0C2109) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x0C2200, 0x0C2201) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x0C2208, 0x0C2209) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x0c2308, 0x0c2309) AM_WRITE(MWA16_NOP)	// bit 0 of DSW1 (flip screen) - use vregs

	AM_RANGE(0x0D0000, 0x0dffff) AM_WRITE(MWA16_RAM)  // 0xd2000 - 0xd3fff?   0xd8000?

	AM_RANGE(0x0e0000, 0x0e3fff) AM_WRITE(bsb_videoram2_w) AM_BASE(&bsb_videoram2)
	AM_RANGE(0x0e8000, 0x0ebfff) AM_WRITE(bsb_videoram3_w) AM_BASE(&bsb_videoram3)
	AM_RANGE(0x0ec000, 0x0effff) AM_WRITE(bsb_videoram_w) AM_BASE(&bsb_videoram)

	AM_RANGE(0x0f0000, 0x0f7fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x0f8000, 0x0f87ff) AM_WRITE(paletteram16_RRRRGGGGBBBBRGBx_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x0f8800, 0x0fffff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0x1f0000, 0x1f7fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x1f8000, 0x1f87ff) AM_WRITE(MWA16_RAM) AM_BASE(&bigstrkb_spriteram)
	AM_RANGE(0x1f8800, 0x1fffff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0x700020, 0x700027) AM_WRITE(MWA16_RAM) AM_BASE(&bsb_vidreg1)
	AM_RANGE(0x700030, 0x700037) AM_WRITE(MWA16_RAM) AM_BASE(&bsb_vidreg2)

	AM_RANGE(0xB00000, 0xB00001) AM_WRITE(MWA16_NOP)

	AM_RANGE(0xE00000, 0xE00001) AM_WRITE(OKIM6295_data_0_lsb_w)
	AM_RANGE(0xE00002, 0xE00003) AM_WRITE(OKIM6295_data_1_lsb_w)

	AM_RANGE(0xE00008, 0xE00009) AM_WRITE(MWA16_NOP)
	AM_RANGE(0xE0000c, 0xE0000d) AM_WRITE(MWA16_NOP)

	AM_RANGE(0xF00000, 0xFFFFFF) AM_WRITE(MWA16_RAM)
ADDRESS_MAP_END

#define BIGSTRKB_PLAYER_INPUT( player, start ) \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(player) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(player) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(player) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, start )

INPUT_PORTS_START( bigstrkb )
	PORT_START	/* DSW0 (0x700000.w) */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )	// also set "Coin B" to "Free Play"
	/* 0x01 to 0x05 gives 2C_3C */
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )	// also set "Coin A" to "Free Play"
	/* 0x10 to 0x50 gives 2C_3C */

	PORT_START	/* DSW1 (0x700002.w) */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )	// Check code at 0x00097c (flip screen)
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x06, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x06, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x18, 0x18, "Timer Speed" )
	PORT_DIPSETTING(    0x08, "Slow" )				// 65
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )			// 50
	PORT_DIPSETTING(    0x10, "Fast" )				// 35
	PORT_DIPSETTING(    0x00, "Fastest" )			// 25
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "2 Players Game" )
	PORT_DIPSETTING(    0x00, "1 Credit" )
	PORT_DIPSETTING(    0x40, "2 Credits" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		// Check code at 0x000c50 (test mode ?)
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* System inputs (0x700004.w) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* Player 1 controls (0x70000c.w) */
	BIGSTRKB_PLAYER_INPUT( 1, IPT_START1 )

	PORT_START	/* Player 2 controls (0x70000a.w) */
	BIGSTRKB_PLAYER_INPUT( 2, IPT_START2 )
INPUT_PORTS_END

/* GFX Decode */

static const gfx_layout bigstrkb_charlayout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout bigstrkb_char16layout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
	  8, 9,10,11,12,13,14,15 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
	  8*16, 9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	16*16
};



static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &bigstrkb_charlayout,   0x200, 16 },
	{ REGION_GFX2, 0, &bigstrkb_char16layout,   0, 32 },
	{ REGION_GFX3, 0, &bigstrkb_char16layout,   0x300, 16 },
	{ -1 } /* end of array */
};


/* Machine Driver */

static MACHINE_DRIVER_START( bigstrkb )
	MDRV_CPU_ADD(M68000, 12000000)
	MDRV_CPU_PROGRAM_MAP(bigstrkb_readmem,bigstrkb_writemem)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER )
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)


	MDRV_PALETTE_LENGTH(0x400)

	MDRV_VIDEO_START(bigstrkb)
	MDRV_VIDEO_UPDATE(bigstrkb)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")
//  MDRV_SOUND_ADD(YM2151, ym2151_interface)

	MDRV_SOUND_ADD(OKIM6295, 4000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.30)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.30)

	MDRV_SOUND_ADD(OKIM6295, 4000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.30)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.30)
MACHINE_DRIVER_END

/* Rom Loading */

ROM_START( bigstrkb )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "footgaa.015", 0x00001, 0x40000, CRC(33b1d7f3) SHA1(7a48c3c7f5ed61099d07b3259778ad382b7660a2) )
	ROM_LOAD16_BYTE( "footgaa.016", 0x00000, 0x40000, CRC(1c6b8709) SHA1(b371cb1421877247d88ffc52ad090b3c6279b78f) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE  ) /* 8x8x4 FG Tiles */
	ROM_LOAD( "footgaa.005", 0x00000, 0x10000, CRC(d97c9bfe) SHA1(03410a6b5348362575b2463ac9968975eeb0bc39) ) // FIRST AND SECOND HALF IDENTICAL
	ROM_LOAD( "footgaa.006", 0x10000, 0x10000, CRC(1ae56e8b) SHA1(632ef5ca0ba043115d94e925d23a48cc28eeeb40) ) // FIRST AND SECOND HALF IDENTICAL
	ROM_LOAD( "footgaa.007", 0x20000, 0x10000, CRC(a45fa6b6) SHA1(95ea6cf98b1fb7600c034f4cedda3cc46a51e199) ) // FIRST AND SECOND HALF IDENTICAL
	ROM_LOAD( "footgaa.008", 0x30000, 0x10000, CRC(2700888c) SHA1(ef3b4393cd36f5bbe7fdb8a78c8d0bc15022d027) )

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_INVERT | ROMREGION_DISPOSE  ) /* 16x16x4 BG Tiles */
	ROM_LOAD( "footgaa.001", 0x000000, 0x80000, CRC(0e440841) SHA1(169ce2ba3ace707466fa1138c0841b7a6c90629f) ) // x1xxxxxxxxxxxxxxxxx = 0xFF
	ROM_LOAD( "footgaa.002", 0x080000, 0x80000, CRC(92a15164) SHA1(31f641a6ab3a6115fbbdf89d65e8316c92bddf2a) ) // x1xxxxxxxxxxxxxxxxx = 0xFF
	ROM_LOAD( "footgaa.003", 0x100000, 0x80000, CRC(da127b89) SHA1(085c201abcbd7ba3c87e4cf066f7928daebedd5d) ) // x1xxxxxxxxxxxxxxxxx = 0xFF
	ROM_LOAD( "footgaa.004", 0x180000, 0x80000, CRC(3e6b0d92) SHA1(d8bf2e2d82dc985e8912b23620b19391396bc1af) ) // x1xxxxxxxxxxxxxxxxx = 0xFF

	ROM_REGION( 0x080000, REGION_GFX3, ROMREGION_INVERT | ROMREGION_DISPOSE ) /* 16x16x4 Sprites */
	ROM_LOAD( "footgaa.011", 0x000000, 0x20000, CRC(c3924fea) SHA1(85b6775b5aa8c518a1e169b97379a210e25e67c9) )
	ROM_LOAD( "footgaa.012", 0x020000, 0x20000, CRC(a581e9d7) SHA1(b894186f07612f9372e6d3bc037c65696c070d04) )
	ROM_LOAD( "footgaa.013", 0x040000, 0x20000, CRC(26ce4b7f) SHA1(4bfd1de6d73dc5e720972bba477081dba0b05ab3) )
	ROM_LOAD( "footgaa.014", 0x060000, 0x20000, CRC(c3cfc500) SHA1(5dc5780b9977b0544601471004c656c2fd738bcd) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* Samples? */
	ROM_LOAD( "footgaa.010", 0x00000, 0x40000, CRC(53014576) SHA1(7f3402b33ef5992a6ae51ce07f0fcdc267c51beb) )

	ROM_REGION( 0x40000, REGION_SOUND2, 0 ) /* Samples? */
	ROM_LOAD( "footgaa.009", 0x00000, 0x40000, CRC(19bf0896) SHA1(30c8e030d7dbcd38f213010596c8f9c5b8089f62) )
ROM_END

/* GAME drivers */

GAME( 1992, bigstrkb, bigstrik, bigstrkb, bigstrkb, 0, ROT0, "bootleg", "Big Striker (bootleg)", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL ,2)
