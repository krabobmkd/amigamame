/*
 Super Draw Poker (c) Stern 1983

 driver by Pierpaolo Prazzoli


A2-1C   8910
A2-1D   Z80
A1-1E
A1-1H
A3-1J

        A1-4K
        A1-4L
        A1-4N
        A1-4P           A1-9N (6301)
                        A1-9P

 To do:
 - finish and fix inputs
 - colours ?
 - proms ?

*/

#include "driver.h"
#include "sound/ay8910.h"

static tilemap *fg_tilemap;
static unsigned char *char_bank;

WRITE8_HANDLER( supdrapo_videoram_w )
{
	if( videoram[offset] != data )
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap, offset);
	}
}

WRITE8_HANDLER( supdrapo_char_bank_w )
{
	if( char_bank[offset] != data )
	{
		char_bank[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap, offset);
	}
}

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x4fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x5800, 0x5817) AM_READ(MRA8_RAM)
	AM_RANGE(0x6000, 0x67ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x6c00, 0x6fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8000, 0x8000) AM_READ(input_port_4_r) //?
	AM_RANGE(0x8001, 0x8001) AM_READ(input_port_0_r) //input
	AM_RANGE(0x8002, 0x8002) AM_READ(input_port_1_r) //input
	AM_RANGE(0x8003, 0x8003) AM_READ(input_port_2_r) //input
	AM_RANGE(0x8004, 0x8004) AM_READ(input_port_3_r) //input
	AM_RANGE(0x8005, 0x8005) AM_READ(input_port_6_r) //?
	AM_RANGE(0x8006, 0x8006) AM_READ(input_port_5_r) //dip?
	AM_RANGE(0x9000, 0x9097) AM_READ(MRA8_RAM)
	AM_RANGE(0x909d, 0x909d) AM_READ(MRA8_RAM)
	AM_RANGE(0x9400, 0x9400) AM_READ(input_port_7_r) //need check
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x4fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x5000, 0x50ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x5800, 0x5837) AM_WRITE(MWA8_RAM) //every 2, starts 5801
	AM_RANGE(0x583b, 0x583b) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x6000, 0x67ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x6800, 0x6bff) AM_WRITE(supdrapo_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x6c00, 0x6fff) AM_WRITE(MWA8_RAM) AM_BASE(&char_bank)
	AM_RANGE(0x7000, 0x77ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x7800, 0x7c00) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8000, 0x8000) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8003, 0x8003) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8004, 0x8004) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x9000, 0x9097) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x9081, 0x9081) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x909d, 0x909d) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x9800, 0x9800) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x9801, 0x9801) AM_WRITE(AY8910_control_port_0_w)
ADDRESS_MAP_END

INPUT_PORTS_START( supdrapo )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1) //win
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1) //cancel
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(1) //deal
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(1) //bet
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN4 ) //10 scores
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN3 ) // 5 scores
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN2 ) // 2 scores
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN1 ) // 1 score

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_PLAYER(1) //hold 5 p1
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_PLAYER(1) //hold 4 p1
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_PLAYER(1) //hold 3 p1
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON9 ) PORT_PLAYER(1) //hold 2 p1
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON8 ) PORT_PLAYER(1) //hold 1 p1
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_PLAYER(1) //black p1
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(1) //red p1
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(1) //double p1

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2) //win
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2) //cancel
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2) //deal
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(2) //bet
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN4 ) //10 scores
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN3 ) // 5 scores
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN2 ) // 2 scores
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN1 ) // 1 score

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2) //hold 5 p2
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2) //hold 4 p2
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2) //hold 3 p2
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(2) //hold 2 p2
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN4 ) // hold 1 p2
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN3 ) // black p2
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN2 ) // red p2
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN1 ) // double p2

	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "4-1" ) //select player 1
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "4-2" ) //select player 2
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "4-3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "4-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "4-5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "4-6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_SERVICE_NO_TOGGLE( 0x40, IP_ACTIVE_HIGH )
	PORT_DIPNAME( 0x80, 0x00, "4-8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )


	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "5-1" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "5-2" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "5-3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "5-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "5-5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "5-6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x40, DEF_STR( French ) )
	PORT_DIPNAME( 0x80, 0x00, "5-8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "6-1" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "6-2" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "6-3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "6-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "6-5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "6-6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "6-7" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "6-8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "7-1" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "7-2" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "7-3" ) //reveal
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "7-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "7-5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "7-6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "7-7" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "7-8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ 0, RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0, 16 },
	{ -1 }
};

static void get_tile_info(int tile_index)
{
	int code = videoram[tile_index] + char_bank[tile_index] * 0x100;

	SET_TILE_INFO( 0, code, 0, 0)
}

VIDEO_START( supdrapo )
{
	fg_tilemap = tilemap_create(get_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,32,32);

	if( !fg_tilemap )
		return 1;

	return 0;
}

VIDEO_UPDATE( supdrapo )
{
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);

}

static MACHINE_DRIVER_START( supdrapo )
	MDRV_CPU_ADD(Z80,8000000/2)		 /* ??? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_pulse,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER )
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 0*8, 32*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(16)

	MDRV_VIDEO_START(supdrapo)
	MDRV_VIDEO_UPDATE(supdrapo)

	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 8000000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END

ROM_START( supdrapo )
	ROM_REGION( 0x010000, REGION_CPU1, 0 )
	ROM_LOAD( "a2-1c",        0x0000, 0x1000, CRC(b65af689) SHA1(b45cd15ca8f9c931d83a90f3cdbebf218070b195) )
	ROM_LOAD( "a2-1d",        0x1000, 0x1000, CRC(9ccc4347) SHA1(ea8f4d17aaacc7091ca0a66247b55eb12155c9d7) )
	ROM_LOAD( "a1-1e",        0x2000, 0x1000, CRC(44f2b75d) SHA1(615d0acd3f8a109334f415732b6b4fe7b810d91c) ) //a2-1e
	ROM_LOAD( "a1-1h",        0x3000, 0x1000, CRC(9c1a10ff) SHA1(243dd64f0b29f9bed4cfa8ecb801ddd973d9e3c3) )
	ROM_LOAD( "a3-1j",        0x4000, 0x1000, CRC(71c2bf1c) SHA1(cb98bbf88b8a410075a074ec8619c6e703c6c582) )

	ROM_REGION( 0x04000, REGION_GFX1, 0 )
	ROM_LOAD( "a1-4p",        0x0000, 0x1000, CRC(5ac096cc) SHA1(60173a83d0e60fd4d0eb40b7b4e80a74ac5fb23d) )
	ROM_LOAD( "a1-4n",        0x1000, 0x1000, CRC(6985fac9) SHA1(c6357fe0f042b67f8559ec9da03106d1ff08dc66) )
	ROM_LOAD( "a1-4l",        0x2000, 0x1000, CRC(534f7b94) SHA1(44b83053827b27b9e45f6fc50d3878984ef5c5cc) )
	ROM_LOAD( "a1-4k",        0x3000, 0x1000, CRC(3d881f5b) SHA1(53d8800a098e4393224de0b82f8e516f73fd6b62) )

	ROM_REGION( 0x00100, REGION_PROMS, 0 )
	ROM_LOAD( "a1-9n",        0x0000, 0x0100, CRC(e62529e3) SHA1(176f2069b0c06c1d088909e81658652af06c8eec) )
	ROM_LOAD( "a1-9p",        0x0000, 0x0100, CRC(a0547746) SHA1(747c8aef5afa26124fe0763e7f96c4ff6be31863) )

ROM_END


GAME( 1983, supdrapo, 0, supdrapo, supdrapo, 0, ROT90, "Stern", "Super Draw Poker", GAME_NOT_WORKING ,0)
