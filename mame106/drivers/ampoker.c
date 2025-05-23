/*
    American Poker

    WB 5300 IS SAME AS Winbond WF19054 and YM2149F (AY891X?)

    CPU TMPZ84C00AP OR LH0080A

    XTAL 6.000

    1 DIP x 8

    RAM 6116

    LH 5116D X2

    ---

    Crystal 6.000 MHz
    CPU         Sharp LH0080B Z80B-CPU 9241 1 B
    Sound       OKI M5255 9203
    RAM         2x TMM2015AP-15 (2048x8)
    PAL         PAL?????16VE??? 022J????  P.M.REG
    PROM        Philips N82S147AN PTH6708 9518nl
    EPROM       Intel D27128-4 T8180760S
                AMD AM27C512-205DC 916LADL
    Chip?       Sanyo ?????

    7 unmarked chips

    ---

    AGEMAME driver by Curt Coder

Any fixes for this driver should be forwarded to the AGEMAME forum at (http://www.mameworld.info)
*/

#include "driver.h"
#include "sound/ay8910.h"

static tilemap *bg_tilemap;

WRITE8_HANDLER( ampoker2_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset / 2);
	}
}

static void get_bg_tile_info(int tile_index)
{
	int offs = tile_index * 2;
	int attr = videoram[offs + 1];
	int code = videoram[offs];
	int color = attr;
	color=0;

	SET_TILE_INFO(0, code, color, 0)
}

VIDEO_START(ampoker2)
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 64, 32);

	if ( !bg_tilemap )
		return 1;

	return 0;
}

VIDEO_UPDATE(ampoker2)
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
}

static ADDRESS_MAP_START( ampoker2_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
	AM_RANGE(0xe000, 0xefff) AM_RAM AM_WRITE(ampoker2_videoram_w) AM_BASE(&videoram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ampoker2_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
//  AM_RANGE(0x00, 0x1f) AM_WRITENOP
	AM_RANGE(0x10, 0x10) AM_READ(input_port_0_r)
	AM_RANGE(0x11, 0x11) AM_READ(input_port_1_r)
	AM_RANGE(0x12, 0x12) AM_READ(input_port_2_r)
	AM_RANGE(0x13, 0x13) AM_READ(input_port_3_r)
	AM_RANGE(0x14, 0x14) AM_READ(input_port_4_r)
	AM_RANGE(0x15, 0x15) AM_READ(input_port_5_r)
	AM_RANGE(0x16, 0x16) AM_READ(input_port_6_r)
	AM_RANGE(0x17, 0x17) AM_READ(input_port_7_r)
//  AM_RANGE(0x21, 0x21) AM_WRITENOP
//  AM_RANGE(0x30, 0x39) AM_WRITENOP
ADDRESS_MAP_END

INPUT_PORTS_START( ampoker2 )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 0") PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 1") PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 2") PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 3") PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 4") PORT_CODE(KEYCODE_T)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 5") PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 6") PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 7") PORT_CODE(KEYCODE_I)

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 0") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 1") PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 2") PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 3") PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 4") PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 5") PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 6") PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 7") PORT_CODE(KEYCODE_K)

	PORT_START_TAG("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 0") PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 1") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 2") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 3") PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 4") PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 5") PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 6") PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 7") PORT_CODE(KEYCODE_L)

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW4")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8, 8,
	1024,
	2,
	{ 0, 4 },
	{ 0, 1, 2, 3, 8, 9, 10, 11 },
	{ 0*8, 2*8, 4*8, 6*8, 8*8, 10*8, 12*8, 14*8 },
	16*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,	   0, 16 },
	{ -1 }
};

static struct AY8910interface ay8910_interface =
{
input_port_3_r,	//
input_port_4_r,	//
0,
0
};

static MACHINE_DRIVER_START( ampoker2 )
	// basic machine hardware
	MDRV_CPU_ADD(Z80, 6000000/2)	//???
	MDRV_CPU_PROGRAM_MAP(ampoker2_map, 0)
	MDRV_CPU_IO_MAP(ampoker2_io_map, 0)
	//MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)
	MDRV_CPU_PERIODIC_INT(irq0_line_hold, TIME_IN_HZ(1))

	MDRV_FRAMES_PER_SECOND(50)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	// video hardware
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 64*8-1, 0*8, 32*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(32)
	MDRV_COLORTABLE_LENGTH(32)

	MDRV_VIDEO_START(ampoker2)
	MDRV_VIDEO_UPDATE(ampoker2)

	// sound hardware
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD(AY8910,12000000/12)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END

ROM_START( ampoker2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "ampoker.u6", 0x0000, 0x10000, CRC(d7b055bd) SHA1(f5231d2ec80f740eabedaba07547ccbb977accc1) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ampoker.u47", 0x0000, 0x4000, CRC(cefed6c7) SHA1(79591339eab2712b432dfe89929dbc97000a13d2) )

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD( "82s147an.u48", 0x0000, 0x0200, NO_DUMP )
ROM_END

ROM_START( ampokr2a )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "rom9.u6", 0x0000, 0x10000, CRC(820a491d) SHA1(36654aacac010e7c086dd18d4e0ca5d959b9044f) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "rom0.u47", 0x0000, 0x4000, CRC(cefed6c7) SHA1(79591339eab2712b432dfe89929dbc97000a13d2) )

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD( "82s147an.u48", 0x0000, 0x0200, NO_DUMP )
ROM_END

ROM_START( ampokr2b )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "poker9.003", 0x4000, 0x8000, CRC(a31221fc) SHA1(4a8bdd8ce8d5bff7e7cfc4ae91e27c1d366dc54d) )
	ROM_COPY( REGION_CPU1, 0x8000, 0x0000, 0x4000 ) // poker9.003 contains the 16K halves swapped around
	ROM_LOAD( "poker9.002", 0x8000, 0x4000, CRC(bfde5bce) SHA1(c7c7ca2268694015e8ec673e8fa5c48043086d3f) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "poker9.028", 0x0000, 0x4000, CRC(65bccb40) SHA1(75f154a2aaf9f9be62e0e1dd8cbe630b9ea0145c) )
ROM_END

ROM_START( ampokr2c )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )
	ROM_LOAD( "poker7.001", 0x0000, 0x10000, CRC(eca16b9e) SHA1(5063d733721457ab3b08caafbe8d33b2cbe4f88b) )
	ROM_COPY( REGION_CPU1, 0x4000, 0x0000, 0x4000 ) // poker7.001 contains the 1st and 2nd 16K quarters swapped
	ROM_COPY( REGION_CPU1, 0x0000, 0x4000, 0x4000 ) // poker7.001 contains the 1st and 2nd 16K quarters swapped

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "poker7.002", 0x0000, 0x4000, CRC(65bccb40) SHA1(75f154a2aaf9f9be62e0e1dd8cbe630b9ea0145c) )
ROM_END

static DRIVER_INIT( ampoker2 )
{
	UINT8 *ROM = memory_region(REGION_CPU1);
	ROM[0x009f] = 0xc9;
	ROM[0x139c] = 0x00;
	ROM[0x139d] = 0x00;
	ROM[0x139e] = 0x00;
/*  ROM[0x0094] = 0x00;
    ROM[0x0095] = 0x00;
    ROM[0x0096] = 0x00;*/
}

GAME( 198?, ampoker2, 0,        ampoker2, ampoker2, ampoker2, ROT0, "Novomatic", "American Poker II (set 1)", GAME_NOT_WORKING ,0)
GAME( 198?, ampokr2a, ampoker2, ampoker2, ampoker2, ampoker2, ROT0, "Novomatic", "American Poker II (set 2)", GAME_NOT_WORKING ,0)
GAME( 198?, ampokr2b, ampoker2, ampoker2, ampoker2, ampoker2, ROT0, "Novomatic", "American Poker II (set 3)", GAME_NOT_WORKING ,0)
GAME( 198?, ampokr2c, ampoker2, ampoker2, ampoker2, ampoker2, ROT0, "Novomatic", "American Poker II (set 4)", GAME_NOT_WORKING ,0)
