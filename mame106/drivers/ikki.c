/*****************************************************************************

Ikki (c) 1985 Sun Electronics

    Driver by Uki

    20/Jun/2001 -

*****************************************************************************/

#include "driver.h"
#include "sound/sn76496.h"

PALETTE_INIT( ikki );
VIDEO_UPDATE( ikki );

/****************************************************************************/

WRITE8_HANDLER( ikki_scroll_w );
WRITE8_HANDLER( ikki_scrn_ctrl_w );

READ8_HANDLER( ikki_e000_r )
{
/* bit1: interrupt type?, bit0: CPU2 busack? */

	if (cpu_getiloops() == 0)
		return 0;
	return 2;
}

WRITE8_HANDLER( ikki_coin_counters )
{
	coin_counter_w( 0, data & 0x01 );
	coin_counter_w( 1, data & 0x02 );
}

/****************************************************************************/

static ADDRESS_MAP_START( ikki_cpu1, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x9fff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
	AM_RANGE(0xc800, 0xcfff) AM_RAM AM_SHARE(1)
	AM_RANGE(0xd000, 0xd7ff) AM_READWRITE(videoram_r, videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0xe000, 0xe000) AM_READ(ikki_e000_r)
	AM_RANGE(0xe001, 0xe001) AM_READ(input_port_0_r) /* dsw 1 */
	AM_RANGE(0xe002, 0xe002) AM_READ(input_port_1_r) /* dsw 2 */
	AM_RANGE(0xe003, 0xe003) AM_READ(input_port_4_r) /* other inputs */
	AM_RANGE(0xe004, 0xe004) AM_READ(input_port_2_r) /* player1 */
	AM_RANGE(0xe005, 0xe005) AM_READ(input_port_3_r) /* player2 */
	AM_RANGE(0xe008, 0xe008) AM_WRITE(ikki_scrn_ctrl_w)
	AM_RANGE(0xe009, 0xe009) AM_WRITE(ikki_coin_counters)
	AM_RANGE(0xe00a, 0xe00b) AM_WRITE(ikki_scroll_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ikki_cpu2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xc800, 0xcfff) AM_RAM AM_SHARE(1)
	AM_RANGE(0xd801, 0xd801) AM_WRITE(SN76496_0_w)
	AM_RANGE(0xd802, 0xd802) AM_WRITE(SN76496_1_w)
ADDRESS_MAP_END


/****************************************************************************/

INPUT_PORTS_START( ikki )
	PORT_START  /* dsw1 */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPNAME( 0x02, 0x00, "2 Players Game" )
	PORT_DIPSETTING(    0x00, "2 Credits" )
	PORT_DIPSETTING(    0x02, "1 Credit" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Unknown 1-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0xf0, 0x00, "Coin1 / Coin2" )
	PORT_DIPSETTING(    0x00, "1C 1C / 1C 1C" )
	PORT_DIPSETTING(    0x10, "2C 1C / 2C 1C" )
	PORT_DIPSETTING(    0x20, "2C 1C / 1C 3C" )
	PORT_DIPSETTING(    0x30, "1C 1C / 1C 2C" )
	PORT_DIPSETTING(    0x40, "1C 1C / 1C 3C" )
	PORT_DIPSETTING(    0x50, "1C 1C / 1C 4C" )
	PORT_DIPSETTING(    0x60, "1C 1C / 1C 5C" )
	PORT_DIPSETTING(    0x70, "1C 1C / 1C 6C" )
	PORT_DIPSETTING(    0x80, "1C 2C / 1C 2C" )
	PORT_DIPSETTING(    0x90, "1C 2C / 1C 4C" )
	PORT_DIPSETTING(    0xa0, "1C 2C / 1C 5C" )
	PORT_DIPSETTING(    0xb0, "1C 2C / 1C 10C" )
	PORT_DIPSETTING(    0xc0, "1C 2C / 1C 11C" )
	PORT_DIPSETTING(    0xd0, "1C 2C / 1C 12C" )
	PORT_DIPSETTING(    0xe0, "1C 2C / 1C 6C" )
	PORT_DIPSETTING(    0xf0, DEF_STR( Free_Play ) )

	PORT_START  /* dsw2 */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, "1 (Normal)" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x06, "4 (Difficult)" )
	PORT_DIPNAME( 0x08, 0x00, "Unknown 2-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Unknown 2-5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "Unknown 2-6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "Unknown 2-7" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Freeze" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START /* e004 */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY

	PORT_START /* e005 */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL

	PORT_START /* e003 */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
INPUT_PORTS_END

/****************************************************************************/

static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	2048,   /* 2048 characters */
	3,      /* 3 bits per pixel */
	{0,16384*8,16384*8*2},
	{7,6,5,4,3,2,1,0},
	{8*0, 8*1, 8*2, 8*3, 8*4, 8*5, 8*6, 8*7},
	8*8
};

static const gfx_layout spritelayout =
{
	16,32,  /* 16*32 characters */
	256,    /* 256 characters */
	3,      /* 3 bits per pixel */
	{16384*8*2,16384*8,0},
	{7,6,5,4,3,2,1,0,
		8*16+7,8*16+6,8*16+5,8*16+4,8*16+3,8*16+2,8*16+1,8*16+0},
	{8*0, 8*1, 8*2, 8*3, 8*4, 8*5, 8*6, 8*7,
	8*8,8*9,8*10,8*11,8*12,8*13,8*14,8*15,
	8*32,8*33,8*34,8*35,8*36,8*37,8*38,8*39,
	8*40,8*41,8*42,8*43,8*44,8*45,8*46,8*47},
	8*8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX2, 0x0000, &charlayout,   512, 64 },
	{ REGION_GFX1, 0x0000, &spritelayout, 0,   64 },
	{ -1 } /* end of array */
};


static MACHINE_DRIVER_START( ikki )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,8000000/2) /* 4.000MHz */
	MDRV_CPU_PROGRAM_MAP(ikki_cpu1,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,2)

	MDRV_CPU_ADD(Z80,8000000/2) /* 4.000MHz */
	MDRV_CPU_PROGRAM_MAP(ikki_cpu2,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,2)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(1*8, 31*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256+1)
	MDRV_COLORTABLE_LENGTH(1024)

	MDRV_PALETTE_INIT(ikki)
	MDRV_VIDEO_START(generic)
	MDRV_VIDEO_UPDATE(ikki)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(SN76496, 8000000/4)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	MDRV_SOUND_ADD(SN76496, 8000000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)
MACHINE_DRIVER_END

/****************************************************************************/

ROM_START( ikki )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* main CPU */
	ROM_LOAD( "tvg17_1",  0x0000,  0x2000, CRC(cb28167c) SHA1(6843553faee0d3bbe432689fdf5f5454470e2b09) )
	ROM_CONTINUE(         0x8000,  0x2000 )
	ROM_LOAD( "tvg17_2",  0x2000,  0x2000, CRC(756c7450) SHA1(043e4f3085d1800b569ee397a968229d547ffbe1) )
	ROM_LOAD( "tvg17_3",  0x4000,  0x2000, CRC(91f0a8b6) SHA1(529fee561c26aa9da75ee58488070329c459540c) )
	ROM_LOAD( "tvg17_4",  0x6000,  0x2000, CRC(696fcf7d) SHA1(6affec60490012fdc762e7a104c0031d44f95bbd) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* sub CPU */
	ROM_LOAD( "tvg17_5",  0x0000,  0x2000, CRC(22bdb40e) SHA1(265801ad660a5a3fc5bb187fa92dbe6098b390f5) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE ) /* sprite */
	ROM_LOAD( "tvg17_6",  0x0000,  0x4000, CRC(dc8aa269) SHA1(fd8b5c2bead52e1e136d4df4c26f136d8992d9be) )
	ROM_LOAD( "tvg17_7",  0x4000,  0x4000, CRC(0e9efeba) SHA1(d922c4276a988b78b9a2a3ca632136e64a80d995) )
	ROM_LOAD( "tvg17_8",  0x8000,  0x4000, CRC(45c9087a) SHA1(9db82fc194096588fde5048e922a654e2ad12c23) )

	ROM_REGION( 0xc000, REGION_GFX2, ROMREGION_DISPOSE ) /* bg */
	ROM_LOAD( "tvg17_9",  0x8000,  0x4000, CRC(c594f3c5) SHA1(6fe19d9ccbe6934a210eb2cab441cd0ba83cbcf4) )
	ROM_LOAD( "tvg17_10", 0x4000,  0x4000, CRC(2e510b4e) SHA1(c0ff4515e66ab4959b597a4d930cbbcc31c53cda) )
	ROM_LOAD( "tvg17_11", 0x0000,  0x4000, CRC(35012775) SHA1(c90386660755c85fb9f020f8161805dd02a16271) )

	ROM_REGION( 0x0700, REGION_PROMS, 0 ) /* color PROMs */
	ROM_LOAD( "prom17_3", 0x0000,  0x0100, CRC(dbcd3bec) SHA1(1baeec277b16c82b67e10da9d4c84cf383ef4a82) ) /* R */
	ROM_LOAD( "prom17_4", 0x0100,  0x0100, CRC(9eb7b6cf) SHA1(86451e8a510f8cfbc0be7d4e7bb1ee7dfd67f1f4) ) /* G */
	ROM_LOAD( "prom17_5", 0x0200,  0x0100, CRC(9b30a7f3) SHA1(a0aefc2c8325b95ea227e404583d14622b04a3b9) ) /* B */
	ROM_LOAD( "prom17_6", 0x0300,  0x0200, CRC(962e619d) SHA1(d2cbcd7b2f1438d1d3759cc1ef6b76b42d9952fe) ) /* sprite */
	ROM_LOAD( "prom17_7", 0x0500,  0x0200, CRC(b1f5148c) SHA1(251ddaabf65a87306970b79918849da95beb8ee7) ) /* bg */

	ROM_REGION( 0x0200, REGION_USER1, 0 )
	ROM_LOAD( "prom17_1", 0x0000,  0x0100, CRC(ca0af30c) SHA1(6d7cfeb16daf61c6e7f93172809b0983bf13cd6c) ) /* video attribute */
	ROM_LOAD( "prom17_2", 0x0100,  0x0100, CRC(f3c55174) SHA1(936c5432c4fccfcb2601c1e08b98d5509202fe5b) ) /* unknown */
ROM_END

ROM_START( farmer )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* main CPU */
	ROM_LOAD( "tvg-1.10", 0x0000, 0x2000, CRC(2c0bd392) SHA1(138efa9bc2e40c847f5ac3d31bd62021fd894f49) )
	ROM_CONTINUE(         0x8000, 0x2000 )
	ROM_LOAD( "tvg-2.9",  0x2000, 0x2000, CRC(b86efe02) SHA1(a11cabd001b1577b5708c3f8b1f2717761096c75) )
	ROM_LOAD( "tvg-3.8",  0x4000, 0x2000, CRC(fd686ff4) SHA1(0857b3061126c8dc18c0cd4bd43431f5f5551aef) )
	ROM_LOAD( "tvg-4.7",  0x6000, 0x2000, CRC(1415355d) SHA1(5a3adcb6d03270b4139fbbd0097b6a089cf8c2e1) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* sub CPU */
	ROM_LOAD( "tvg-5.30",  0x0000, 0x2000, CRC(22bdb40e) SHA1(265801ad660a5a3fc5bb187fa92dbe6098b390f5) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE ) /* sprite */
	ROM_LOAD( "tvg-6.104", 0x0000, 0x4000, CRC(dc8aa269) SHA1(fd8b5c2bead52e1e136d4df4c26f136d8992d9be) )
	ROM_LOAD( "tvg-7.103", 0x4000, 0x4000, CRC(0e9efeba) SHA1(d922c4276a988b78b9a2a3ca632136e64a80d995) )
	ROM_LOAD( "tvg-8.102", 0x8000, 0x4000, CRC(45c9087a) SHA1(9db82fc194096588fde5048e922a654e2ad12c23) )

	ROM_REGION( 0xc000, REGION_GFX2, ROMREGION_DISPOSE ) /* bg */
	ROM_LOAD( "tvg17_9",  0x8000,  0x4000, CRC(c594f3c5) SHA1(6fe19d9ccbe6934a210eb2cab441cd0ba83cbcf4) )
	ROM_LOAD( "tvg17_10", 0x4000,  0x4000, CRC(2e510b4e) SHA1(c0ff4515e66ab4959b597a4d930cbbcc31c53cda) )
	ROM_LOAD( "tvg17_11", 0x0000,  0x4000, CRC(35012775) SHA1(c90386660755c85fb9f020f8161805dd02a16271) )

	ROM_REGION( 0x0700, REGION_PROMS, 0 ) /* color PROMs */
	ROM_LOAD( "prom17_3", 0x0000,  0x0100, CRC(dbcd3bec) SHA1(1baeec277b16c82b67e10da9d4c84cf383ef4a82) ) /* R */
	ROM_LOAD( "prom17_4", 0x0100,  0x0100, CRC(9eb7b6cf) SHA1(86451e8a510f8cfbc0be7d4e7bb1ee7dfd67f1f4) ) /* G */
	ROM_LOAD( "prom17_5", 0x0200,  0x0100, CRC(9b30a7f3) SHA1(a0aefc2c8325b95ea227e404583d14622b04a3b9) ) /* B */
	ROM_LOAD( "prom17_6", 0x0300,  0x0200, CRC(962e619d) SHA1(d2cbcd7b2f1438d1d3759cc1ef6b76b42d9952fe) ) /* sprite */
	ROM_LOAD( "prom17_7", 0x0500,  0x0200, CRC(b1f5148c) SHA1(251ddaabf65a87306970b79918849da95beb8ee7) ) /* bg */

	ROM_REGION( 0x0200, REGION_USER1, 0 )
	ROM_LOAD( "prom17_1", 0x0000,  0x0100, CRC(ca0af30c) SHA1(6d7cfeb16daf61c6e7f93172809b0983bf13cd6c) ) /* video attribute */
	ROM_LOAD( "prom17_2", 0x0100,  0x0100, CRC(f3c55174) SHA1(936c5432c4fccfcb2601c1e08b98d5509202fe5b) ) /* unknown */
ROM_END

GAME( 1985, ikki,   0,    ikki, ikki, 0, ROT0, "Sun Electronics", "Ikki (Japan)", 0 ,0)
GAME( 1985, farmer, ikki, ikki, ikki, 0, ROT0, "Sun Electronics", "Farmers Rebellion", 0 ,0)
