/***************************************************************************

Ramtek Star Cruiser Driver

(no known issues)

Frank Palazzolo
palazzol@home.com

***************************************************************************/

#include "driver.h"
#include "cpu/i8085/i8085.h"
#include "sound/samples.h"

/* included from vidhrdw/starcrus.c */
WRITE8_HANDLER( starcrus_s1_x_w );
WRITE8_HANDLER( starcrus_s1_y_w );
WRITE8_HANDLER( starcrus_s2_x_w );
WRITE8_HANDLER( starcrus_s2_y_w );
WRITE8_HANDLER( starcrus_p1_x_w );
WRITE8_HANDLER( starcrus_p1_y_w );
WRITE8_HANDLER( starcrus_p2_x_w );
WRITE8_HANDLER( starcrus_p2_y_w );
WRITE8_HANDLER( starcrus_ship_parm_1_w );
WRITE8_HANDLER( starcrus_ship_parm_2_w );
WRITE8_HANDLER( starcrus_proj_parm_1_w );
WRITE8_HANDLER( starcrus_proj_parm_2_w );
READ8_HANDLER( starcrus_coll_det_r );
extern VIDEO_START( starcrus );
extern VIDEO_UPDATE( starcrus );
extern int p1_sprite;
extern int p2_sprite;
extern int s1_sprite;
extern int s2_sprite;

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_ROM) /* Program ROM */
	AM_RANGE(0x1000, 0x10ff) AM_READ(MRA8_RAM) /* RAM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
    AM_RANGE(0x0000, 0x0fff) AM_WRITE(MWA8_ROM) /* Program ROM */
    AM_RANGE(0x1000, 0x10ff) AM_WRITE(MWA8_RAM) /* RAM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
    AM_RANGE(0x00, 0x00) AM_READ(input_port_0_r)
    AM_RANGE(0x01, 0x01) AM_READ(input_port_1_r)
    AM_RANGE(0x02, 0x02) AM_READ(starcrus_coll_det_r)
    AM_RANGE(0x03, 0x03) AM_READ(input_port_2_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
    AM_RANGE(0x00, 0x00) AM_WRITE(starcrus_s1_x_w)
    AM_RANGE(0x01, 0x01) AM_WRITE(starcrus_s1_y_w)
    AM_RANGE(0x02, 0x02) AM_WRITE(starcrus_s2_x_w)
    AM_RANGE(0x03, 0x03) AM_WRITE(starcrus_s2_y_w)
    AM_RANGE(0x04, 0x04) AM_WRITE(starcrus_p1_x_w)
    AM_RANGE(0x05, 0x05) AM_WRITE(starcrus_p1_y_w)
    AM_RANGE(0x06, 0x06) AM_WRITE(starcrus_p2_x_w)
    AM_RANGE(0x07, 0x07) AM_WRITE(starcrus_p2_y_w)
    AM_RANGE(0x08, 0x08) AM_WRITE(starcrus_ship_parm_1_w)
    AM_RANGE(0x09, 0x09) AM_WRITE(starcrus_ship_parm_2_w)
    AM_RANGE(0x0a, 0x0a) AM_WRITE(starcrus_proj_parm_1_w)
    AM_RANGE(0x0b, 0x0b) AM_WRITE(starcrus_proj_parm_2_w)
ADDRESS_MAP_END



INPUT_PORTS_START( starcrus )
		PORT_START	/* player 1 */
		PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY /* ccw */
		PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) /* engine */
		PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY /* cw */
        PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
        PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_BUTTON3 ) /* torpedo */
        PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
        PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) /* phaser */
        PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

        PORT_START  /* player 2 */
        PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY PORT_PLAYER(2) /* ccw */
        PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) /* engine */
        PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_PLAYER(2) /* cw */
        PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
        PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) /* torpedo */
        PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
        PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) /* phaser */
        PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

		PORT_START
        PORT_DIPNAME ( 0x03, 0x02, DEF_STR( Game_Time ) )
        PORT_DIPSETTING ( 0x03, "60 secs" )
        PORT_DIPSETTING ( 0x02, "90 secs" )
        PORT_DIPSETTING ( 0x01, "120 secs" )
        PORT_DIPSETTING ( 0x00, "150 secs" )
        PORT_DIPNAME ( 0x04, 0x00, DEF_STR( Coinage ))
        PORT_DIPSETTING ( 0x04, DEF_STR( 2C_1C ))
        PORT_DIPSETTING ( 0x00, DEF_STR( 1C_1C ))
        PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_COIN2 )
        PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_COIN1 )
        PORT_DIPNAME ( 0x20, 0x20, "Mode" )
        PORT_DIPSETTING ( 0x20, DEF_STR( Standard ) )
        PORT_DIPSETTING ( 0x00, DEF_STR( Alternate ) )
        PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
        PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



static const gfx_layout spritelayout1 =
{
	16,16,    /* 16x16 sprites */
	4,          /* 4 sprites */
	1,      /* 1 bits per pixel */
	{ 0 },  /* 1 chip */
	{ 0*8+4,  0*8+4,  1*8+4,  1*8+4, 2*8+4, 2*8+4, 3*8+4, 3*8+4,
	  4*8+4,  4*8+4,  5*8+4,  5*8+4, 6*8+4, 6*8+4, 7*8+4, 7*8+4 },
	{ 0, 0, 1*64, 1*64, 2*64, 2*64, 3*64, 3*64,
	  4*64, 4*64, 5*64, 5*64, 6*64, 6*64, 7*64, 7*64 },
	1  /* every sprite takes 1 consecutive bit */
};
static const gfx_layout spritelayout2 =
{
    16,16,   /* 16x16 sprites */
    4,       /* 4 sprites */
    1,       /* 1 bits per pixel */
    { 0 },   /* 1 chip */
    { 0*8+4,  1*8+4,  2*8+4,  3*8+4, 4*8+4, 5*8+4, 6*8+4, 7*8+4,
      8*8+4,  9*8+4,  10*8+4,  11*8+4, 12*8+4, 13*8+4, 14*8+4, 15*8+4 },
    { 0, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128,
      8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
    1 /* every sprite takes 1 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &spritelayout1, 0, 1 },
    { REGION_GFX1, 0x0040, &spritelayout1, 0, 1 },
    { REGION_GFX1, 0x0080, &spritelayout1, 0, 1 },
    { REGION_GFX1, 0x00c0, &spritelayout1, 0, 1 },
    { REGION_GFX2, 0x0000, &spritelayout1, 0, 1 },
    { REGION_GFX2, 0x0040, &spritelayout1, 0, 1 },
    { REGION_GFX2, 0x0080, &spritelayout1, 0, 1 },
    { REGION_GFX2, 0x00c0, &spritelayout1, 0, 1 },
    { REGION_GFX3, 0x0000, &spritelayout2, 0, 1 },
    { REGION_GFX3, 0x0100, &spritelayout2, 0, 1 },
    { REGION_GFX3, 0x0200, &spritelayout2, 0, 1 },
    { REGION_GFX3, 0x0300, &spritelayout2, 0, 1 },
	{ -1 } /* end of array */
};


static unsigned short colortable_source[] =
{
	0x00, 0x01, /* White on Black */
};
static PALETTE_INIT( starcrus )
{
	palette_set_color(0,0x00,0x00,0x00); /* Black */
    palette_set_color(1,0xff,0xff,0xff); /* White */
	memcpy(colortable,colortable_source,sizeof(colortable_source));
}

static const char *starcrus_sample_names[] =
{
    "*starcrus",
    "engine.wav",	/* engine sound, channel 0 */
    "explos1.wav",	/* explosion sound, first part, channel 1 */
    "explos2.wav",	/* explosion sound, second part, channel 1 */
    "launch.wav",	/* launch sound, channels 2 and 3 */
    0   /* end of array */
};

static struct Samplesinterface samples_interface =
{
    4,	/* 4 channels */
	starcrus_sample_names
};


static MACHINE_DRIVER_START( starcrus )

	/* basic machine hardware */
	MDRV_CPU_ADD(8080,9750000/9)  /* 8224 chip is a divide by 9 */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_FRAMES_PER_SECOND(57)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 0*8, 32*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2)
	MDRV_COLORTABLE_LENGTH(sizeof(colortable_source) / sizeof(colortable_source[0]))

	MDRV_PALETTE_INIT(starcrus)
	MDRV_VIDEO_START(starcrus)
	MDRV_VIDEO_UPDATE(starcrus)

    /* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(SAMPLES, 0)
	MDRV_SOUND_CONFIG(samples_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( starcrus )
    ROM_REGION( 0x10000, REGION_CPU1, 0 )  /* code */
	ROM_LOAD( "starcrus.j1",   0x0000, 0x0200, CRC(0ee60a50) SHA1(7419e7cb4c589da53d4a10ad129373502682464e) )
	ROM_LOAD( "starcrus.k1",   0x0200, 0x0200, CRC(a7bc3bc4) SHA1(0e38076e921856608b1dd712687bef1c2522b4b8) )
	ROM_LOAD( "starcrus.l1",   0x0400, 0x0200, CRC(10d233ec) SHA1(8933cf9fc51716a9e8f75a4444e7d7070cf5834d) )
	ROM_LOAD( "starcrus.m1",   0x0600, 0x0200, CRC(2facbfee) SHA1(d78fb38de49da938fce2b55c8decc244efee6f94) )
	ROM_LOAD( "starcrus.n1",   0x0800, 0x0200, CRC(42083247) SHA1(b32d67c914833f18e9955cd1c3cb1d948be0a7d5) )
	ROM_LOAD( "starcrus.p1",   0x0a00, 0x0200, CRC(61dfe581) SHA1(e1802fedf94541e9ccd9786b60e90890485f422f) )
	ROM_LOAD( "starcrus.r1",   0x0c00, 0x0200, CRC(010cdcfe) SHA1(ae76f1739b468e2987ce949470b36f1a873e061d) )
	ROM_LOAD( "starcrus.s1",   0x0e00, 0x0200, CRC(da4e276b) SHA1(3298f7cb259803f118a47292cbb413df253ef74d) )

    ROM_REGION( 0x0200, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "starcrus.e6",   0x0000, 0x0200, CRC(54887a25) SHA1(562bf85cd063c2cc0a2f803095aaa6138dfb5bff) )

    ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "starcrus.l2",   0x0000, 0x0200, CRC(54887a25) SHA1(562bf85cd063c2cc0a2f803095aaa6138dfb5bff) )

    ROM_REGION( 0x0400, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "starcrus.j4",   0x0000, 0x0200, CRC(25f15ae1) SHA1(7528edaa01ad5a167191c7e72394cb6009db1b27) )
	ROM_LOAD( "starcrus.g5",   0x0200, 0x0200, CRC(73b27f6e) SHA1(4a6cf9244556a2c2647d594c7a19fe1a374a57e6) )
ROM_END


GAME( 1977, starcrus, 0, starcrus, starcrus, 0, ROT0, "RamTek", "Star Cruiser", 0 ,0)
