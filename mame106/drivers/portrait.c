/**************************************************************************
 Portraits
 (c) 1983 Olympia

Preliminary Driver by Steve Ellenoff & Peo

Changes:

Pierpaolo Prazzoli, xx-07-2004
  - fixed scrolling
  - fixed screen resolution
  - added NVRAM
  - added fake photo when you get the best score
  - fixed service switches and coins
  - added missing roms and the 2nd set

  SW = service switch

  SW1 - SW2
   ON   OFF -> grid test
   ON    ON -> camera test

TODO:
 - add sound
 - add colors
 - fix sprites positions

RAM Location 9240: Controls what level you are on: 0-3 (for each scene)

-------------------------------------------------------------------------

Board layout


Top board

8039                                   rom p3f

              74s288                   rom p2f

rom SA                                 rom p1f

rom M/A                                rom p0f

rom W         18318 18318      rom 15  rom 14

8253          18318 18318      rom 05  rom 04
              18318 18318
8253          18318 18318      rom 13  rom 12
              18318 18318
TMS5200       18318 18318      rom 03  rom 02
              18318 18318
                               rom 11  rom 10
              18318 18318
                               rom 01  rom 00

Bottom board

93Z511DC      93425
DM81LS95      93425
              93425            18318
              2148
              2148             18318

                               18318

                               18318

                               18318

              74s288           18318
                     2114
                     2114 4016
                     2114
                     2114 4016

        Z80
DIP1
DIP2    XD2210


XD2210 or 8202
DM81LS95 = TriState buffer
**************************************************************************/

#include "driver.h"
#include "artwork.h"
#include "cpu/i8039/i8039.h"
#include "sound/5220intf.h"

extern UINT8 *portrait_bgvideoram,*portrait_fgvideoram;

extern int portrait_scroll;

PALETTE_INIT( portrait );
VIDEO_START( portrait );
VIDEO_UPDATE( portrait );
WRITE8_HANDLER( portrait_bgvideo_write );
WRITE8_HANDLER( portrait_fgvideo_write );

static WRITE8_HANDLER( portrait_ctrl_w )
{
	/* bits 4 and 5 are unknown */

	coin_counter_w(0, data & 0x01);
	coin_counter_w(1, data & 0x02);
	coin_counter_w(2, data & 0x04);

	/* the 2 lamps near the camera */
	set_led_status(0, data & 0x08);
	set_led_status(1, data & 0x40);

	/* shows the black and white photo from the camera */
	artwork_show("photo", data & 0x80);
}

static WRITE8_HANDLER( portrait_positive_scroll_w )
{
	portrait_scroll = data;
}

static WRITE8_HANDLER( portrait_negative_scroll_w )
{
	portrait_scroll = - (data ^ 0xff);
}

static ADDRESS_MAP_START( portrait_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_READWRITE(MRA8_RAM, portrait_bgvideo_write) AM_BASE(&portrait_bgvideoram)
	AM_RANGE(0x8800, 0x8fff) AM_READWRITE(MRA8_RAM, portrait_fgvideo_write) AM_BASE(&portrait_fgvideoram)
	AM_RANGE(0x9000, 0x91ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x9200, 0x97ff) AM_RAM
	AM_RANGE(0xa000, 0xa000) AM_WRITE(soundlatch_w)
	AM_RANGE(0xa010, 0xa010) AM_WRITENOP // ?
	AM_RANGE(0xa000, 0xa000) AM_READ(input_port_0_r)
	AM_RANGE(0xa004, 0xa004) AM_READ(input_port_1_r)
	AM_RANGE(0xa008, 0xa008) AM_READWRITE(input_port_2_r, portrait_ctrl_w)
	AM_RANGE(0xa010, 0xa010) AM_READ(input_port_3_r)
	AM_RANGE(0xa018, 0xa018) AM_READWRITE(MRA8_NOP, portrait_positive_scroll_w)
	AM_RANGE(0xa019, 0xa019) AM_WRITE(portrait_negative_scroll_w)
	AM_RANGE(0xa800, 0xa83f) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xffff, 0xffff) AM_READNOP
ADDRESS_MAP_END


static ADDRESS_MAP_START( portrait_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_ROM
ADDRESS_MAP_END


INPUT_PORTS_START( portrait )
	PORT_START /* DSW 1 */
	PORT_DIPNAME( 0x0f, 0x08, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x0e, "1 Coin / 10 Credits" )
	PORT_DIPSETTING(    0x0f, "1 Coin / 12 Credits" )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0x01, "3 Coins / 5 Credits" )
	PORT_DIPSETTING(    0x02, "3 Coins / 7 Credits" )
	PORT_DIPSETTING(    0x03, "3 Coins / 10 Credits" )
	PORT_DIPNAME( 0x70, 0x40, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x70, "1 Coin / 10 Credits" )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_5C ) )
	PORT_DIPNAME( 0x80, 0x00, "Service Coin" )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )

	PORT_START /* DSW 2 */
	PORT_DIPNAME( 0x01, 0x00, "Game Play" )
	PORT_DIPSETTING(    0x00, "Normal Play" )
	PORT_DIPSETTING(    0x01, "Freeplay (255 Cameras)" )
	PORT_DIPNAME( 0x02, 0x00, "High Score" )
	PORT_DIPSETTING(    0x00, "11.350 Points" )
	PORT_DIPSETTING(    0x02, "1.350 Points" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x30, 0x30, "Extra Camera" )
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPSETTING(    0x10, "Every 10.000 Points" )
	PORT_DIPSETTING(    0x20, "Every 20.000 Points" )
	PORT_DIPSETTING(    0x30, "Every 30.000 Points" )
	PORT_DIPNAME( 0x40, 0x00, "Ostrich Speed" )
	PORT_DIPSETTING(    0x00, "Slow" )
	PORT_DIPSETTING(    0x40, "Quick" )
	PORT_DIPNAME( 0x80, 0x80, "Obstacles" )
	PORT_DIPSETTING(    0x80, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )

	PORT_START /* IN 0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )    PORT_IMPULSE(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )    PORT_IMPULSE(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_IMPULSE(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_DIPNAME( 0x40, 0x40, "Service Switch 1" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START /* IN 1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_JOYSTICK_UP    ) PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_JOYSTICK_DOWN  ) PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_JOYSTICK_LEFT  ) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_SERVICE2 ) // hold during boot to clear the NVRAM
	PORT_DIPNAME( 0x40, 0x40, "Service Switch 2" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )
INPUT_PORTS_END

static const gfx_layout tile_layout =
{
	16,16, /* tile width, height   */
	1024,  /* number of characters  */
	3,     /* bits per pixel */
	{ 0, 0x4000*8, 0x8000*8 }, /* bitplane offsets */
	{
		RGN_FRAC(1,2)+7, RGN_FRAC(1,2)+6, RGN_FRAC(1,2)+5, RGN_FRAC(1,2)+4,
		RGN_FRAC(1,2)+3, RGN_FRAC(1,2)+2, RGN_FRAC(1,2)+1, RGN_FRAC(1,2)+0,
		0, 1, 2, 3, 4, 5, 6, 7
	},
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 8*9, 8*10, 8*11, 8*12, 8*13, 8*14, 8*15 },
	8*16 /* character offset */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x00000, &tile_layout, 0, 0x100 },
	{ -1 } /* end of array */
};

static MACHINE_DRIVER_START( portrait )
	MDRV_CPU_ADD(Z80, 4000000)     /* 4 MHz ? */
	MDRV_CPU_PROGRAM_MAP(portrait_map,0)
	MDRV_CPU_VBLANK_INT(irq0_line_pulse,1)

	MDRV_CPU_ADD(I8039, 3120000)  /* ? */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(portrait_sound_map,0)

	MDRV_FRAMES_PER_SECOND(50)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)

	MDRV_NVRAM_HANDLER(generic_0fill)

	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_VISIBLE_AREA(0*8, 54*8-1, 0*8, 40*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(0x800)
//  MDRV_PALETTE_INIT(portrait)

	MDRV_VIDEO_START(portrait)
	MDRV_VIDEO_UPDATE(portrait)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(TMS5220, 640000) // actually it uses a tms5200 chip
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


ROM_START( portrait )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for the cpu */
	ROM_LOAD( "prt-p0.bin",  0x0000, 0x2000, CRC(a21874fa) SHA1(3db863f465a35d7d14dd71b47aa7dfe7b39fccf0) )
	ROM_LOAD( "prt-p1.bin",  0x2000, 0x2000, CRC(4d4d7793) SHA1(f828950ebbf285fc92c65f24421a20ceacef1cb9) )
	ROM_LOAD( "prt-p2.bin",  0x4000, 0x2000, CRC(83d88c9c) SHA1(c876f72b66537a49620fa27a5cb8a4aecd378f0a) )
	ROM_LOAD( "prt-p3.bin",  0x6000, 0x2000, CRC(bd32d007) SHA1(cdf814b00c22f9a4503fa54d43fb5781251b67a7) )

	ROM_REGION( 0x1000, REGION_CPU2, 0 )
	ROM_LOAD( "port_w.bin",  0x0000, 0x0800, CRC(d3a4e950) SHA1(0a399d43c7690d568874f3b1d55135f803fc223f) )
	ROM_LOAD( "port_ma.bin", 0x0800, 0x0800, CRC(ee242e4f) SHA1(fb67e0d136927e04f4fa819f684c97b0d52ee48c) )

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )
	ROM_LOAD( "port_00.a1", 0x00000, 0x2000, CRC(eb3e1c12) SHA1(2d38b66f52546b40553244c8a5c961279559f5b6) )	/*bit plane 1*/
	ROM_LOAD( "port_10.b1", 0x02000, 0x2000, CRC(0f44e377) SHA1(1955f9f4deab2166f637f43c1f326bd65fc90f6a) )	/*bit plane 1*/
	ROM_LOAD( "port_02.d1", 0x04000, 0x2000, CRC(bd93a3f9) SHA1(9cb479b8840cafd6043ff0cb9d5ca031dcd332ba) )	/*bit plane 2*/
	ROM_LOAD( "port_12.e1", 0x06000, 0x2000, CRC(656b9f20) SHA1(c1907aba3d19be79d92cd73784b8e7ae94910da6) )	/*bit plane 2*/
	ROM_LOAD( "port_04.g1", 0x08000, 0x2000, CRC(2a99feb5) SHA1(b373d2a2bd28aad6dd7a15a2166e03a8b7a34d9b) )	/*bit plane 3*/
	ROM_LOAD( "port_14.g1", 0x0a000, 0x2000, CRC(224b7a58) SHA1(b84e70d22d1cab41e5773fc9daa2e4e55ec9d96e) )	/*bit plane 3*/

	ROM_LOAD( "port_01.a2", 0x10000, 0x2000, CRC(70d27508) SHA1(d011f85b31bb3aa6f386e8e0edb91df10f4c4eb6) )	/*bit plane 1*/
	ROM_LOAD( "port_11.b2", 0x12000, 0x2000, CRC(f498e395) SHA1(beb1d12433a350e5b773126de3f2803a9f5620c1) )	/*bit plane 1*/
	ROM_LOAD( "port_03.d2", 0x14000, 0x2000, CRC(03d4153a) SHA1(7ce69ce6a101870dbfca1a9787fb1e660024bc02) )	/*bit plane 2*/
	ROM_LOAD( "port_13.e2", 0x16000, 0x2000, CRC(10fa22b8) SHA1(e8f4c24fcdda0ce5e33bc600acd574a232a9bb21) )	/*bit plane 2*/
	ROM_LOAD( "port_05.g2", 0x18000, 0x2000, CRC(43ea7951) SHA1(df0ae7fa802365979514063e1d67cdd45ecada90) )	/*bit plane 3*/
	ROM_LOAD( "port_15.h2", 0x1a000, 0x2000, CRC(ab20b438) SHA1(ea5d60f6a9f06397bd0c6ee028b463c684090c01) )	/*bit plane 3*/

	ROM_REGION( 0x0800, REGION_USER1, 0 ) // sound related?
	ROM_LOAD( "port_sa.bin", 0x0000, 0x0800, CRC(50510897) SHA1(8af0f42699602a5b33500968c958e3784e03377f) )

	ROM_REGION( 0x840, REGION_PROMS, 0 )
	/* color prom? */
	ROM_LOAD( "93z511.bin",   0x0000, 0x0800, CRC(d66d9036) SHA1(7a25efbd8f2f94a01aad9e2be9cb18da7b9ec1d1) )
	ROM_LOAD( "port_pr1.bin", 0x0800, 0x0020, CRC(1e2deabb) SHA1(8357e53dba26bca9bc5d7a25c715836f0b3700b9) )
	ROM_LOAD( "port_pr2.n4",  0x0820, 0x0020, CRC(008634f3) SHA1(7cde6b09ede672d562569866d944428198f2ba9c) )
ROM_END

ROM_START( portrata )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for the cpu */
	ROM_LOAD( "portp0f.m1",   0x0000, 0x2000, CRC(333eace3) SHA1(8f02df09d8b50d7e37d5abf7d539624c59a7201e) )
	ROM_LOAD( "portp0f.p1",   0x2000, 0x2000, CRC(fe258052) SHA1(f453eb05c68d61dfd644688732ff5c07366c68c0) )
	ROM_LOAD( "portp2f.r1",   0x4000, 0x2000, CRC(bc0104d5) SHA1(7707b85cde2dc9bd95391d4e1dbed219c52618cd) )
	ROM_LOAD( "portp3f.s1",   0x6000, 0x2000, CRC(3f5a3bdf) SHA1(cc4b5d24d0df0962b0cfd4d5c66baac5e4718237) )

	ROM_REGION( 0x1000, REGION_CPU2, 0 )
	ROM_LOAD( "port_w.bin",  0x0000, 0x0800, CRC(d3a4e950) SHA1(0a399d43c7690d568874f3b1d55135f803fc223f) )
	ROM_LOAD( "port_ma.bin", 0x0800, 0x0800, CRC(ee242e4f) SHA1(fb67e0d136927e04f4fa819f684c97b0d52ee48c) )

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )
	ROM_LOAD( "port_00.a1", 0x00000, 0x2000, CRC(eb3e1c12) SHA1(2d38b66f52546b40553244c8a5c961279559f5b6) )	/*bit plane 1*/
	ROM_LOAD( "port_10.b1", 0x02000, 0x2000, CRC(0f44e377) SHA1(1955f9f4deab2166f637f43c1f326bd65fc90f6a) )	/*bit plane 1*/
	ROM_LOAD( "port_02.d1", 0x04000, 0x2000, CRC(bd93a3f9) SHA1(9cb479b8840cafd6043ff0cb9d5ca031dcd332ba) )	/*bit plane 2*/
	ROM_LOAD( "port_12.e1", 0x06000, 0x2000, CRC(656b9f20) SHA1(c1907aba3d19be79d92cd73784b8e7ae94910da6) )	/*bit plane 2*/
	ROM_LOAD( "port_04.g1", 0x08000, 0x2000, CRC(2a99feb5) SHA1(b373d2a2bd28aad6dd7a15a2166e03a8b7a34d9b) )	/*bit plane 3*/
	ROM_LOAD( "port_14.g1", 0x0a000, 0x2000, CRC(224b7a58) SHA1(b84e70d22d1cab41e5773fc9daa2e4e55ec9d96e) )	/*bit plane 3*/

	ROM_LOAD( "port_01.a2", 0x10000, 0x2000, CRC(70d27508) SHA1(d011f85b31bb3aa6f386e8e0edb91df10f4c4eb6) )	/*bit plane 1*/
	ROM_LOAD( "port_11.b2", 0x12000, 0x2000, CRC(f498e395) SHA1(beb1d12433a350e5b773126de3f2803a9f5620c1) )	/*bit plane 1*/
	ROM_LOAD( "port_03.d2", 0x14000, 0x2000, CRC(03d4153a) SHA1(7ce69ce6a101870dbfca1a9787fb1e660024bc02) )	/*bit plane 2*/
	ROM_LOAD( "port_13.e2", 0x16000, 0x2000, CRC(10fa22b8) SHA1(e8f4c24fcdda0ce5e33bc600acd574a232a9bb21) )	/*bit plane 2*/
	ROM_LOAD( "port_05.g2", 0x18000, 0x2000, CRC(43ea7951) SHA1(df0ae7fa802365979514063e1d67cdd45ecada90) )	/*bit plane 3*/
	ROM_LOAD( "port_15.h2", 0x1a000, 0x2000, CRC(ab20b438) SHA1(ea5d60f6a9f06397bd0c6ee028b463c684090c01) )	/*bit plane 3*/

	ROM_REGION( 0x0800, REGION_USER1, 0 ) // sound related?
	ROM_LOAD( "port_sa.bin", 0x0000, 0x0800, CRC(50510897) SHA1(8af0f42699602a5b33500968c958e3784e03377f) )

	ROM_REGION( 0x840, REGION_PROMS, 0 )
	/* color prom? */
	ROM_LOAD( "93z511.bin",   0x0000, 0x0800, CRC(d66d9036) SHA1(7a25efbd8f2f94a01aad9e2be9cb18da7b9ec1d1) )
	ROM_LOAD( "port_pr1.bin", 0x0800, 0x0020, CRC(1e2deabb) SHA1(8357e53dba26bca9bc5d7a25c715836f0b3700b9) )
	ROM_LOAD( "port_pr2.n4",  0x0820, 0x0020, CRC(008634f3) SHA1(7cde6b09ede672d562569866d944428198f2ba9c) )
ROM_END

GAME( 1983, portrait, 0,        portrait, portrait,  0, ROT270, "Olympia", "Portraits (set 1)", GAME_NO_SOUND | GAME_IMPERFECT_GRAPHICS | GAME_WRONG_COLORS ,0)
GAME( 1983, portrata, portrait, portrait, portrait,  0, ROT270, "Olympia", "Portraits (set 2)", GAME_NO_SOUND | GAME_IMPERFECT_GRAPHICS | GAME_WRONG_COLORS ,0)
