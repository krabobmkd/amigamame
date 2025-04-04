/***************************************************************************

Atari Starship 1 driver

  "starshp1" -> regular version, bonus time for 3500 points
  "starshpp" -> possible prototype, bonus time for 2700 points

***************************************************************************/

#include "driver.h"

int starshp1_attract;

extern unsigned char *starshp1_playfield_ram;
extern unsigned char *starshp1_hpos_ram;
extern unsigned char *starshp1_vpos_ram;
extern unsigned char *starshp1_obj_ram;

extern int starshp1_ship_explode;
extern int starshp1_ship_picture;
extern int starshp1_ship_hoffset;
extern int starshp1_ship_voffset;
extern int starshp1_ship_size;

extern int starshp1_circle_hpos;
extern int starshp1_circle_vpos;
extern int starshp1_circle_size;
extern int starshp1_circle_mod;
extern int starshp1_circle_kill;

extern int starshp1_phasor;
extern int starshp1_collision_latch;
extern int starshp1_starfield_kill;
extern int starshp1_mux;

extern READ8_HANDLER( starshp1_rng_r );

extern WRITE8_HANDLER( starshp1_sspic_w );
extern WRITE8_HANDLER( starshp1_ssadd_w );
extern WRITE8_HANDLER( starshp1_playfield_w );

extern VIDEO_UPDATE( starshp1 );
extern VIDEO_EOF( starshp1 );
extern VIDEO_START( starshp1 );

static int starshp1_analog_in_select;


static INTERRUPT_GEN( starshp1_interrupt )
{
	if ((readinputport(0) & 0x90) != 0x90)
	{
		cpunum_set_input_line(0, 0, PULSE_LINE);
	}
}


static void starshp1_write_palette(int inverse)
{
	palette_set_color(inverse ? 7 : 0, 0x00, 0x00, 0x00);
	palette_set_color(inverse ? 6 : 1, 0x1e, 0x1e, 0x1e);
	palette_set_color(inverse ? 5 : 2, 0x4e, 0x4e, 0x4e);
	palette_set_color(inverse ? 4 : 3, 0x6c, 0x6c, 0x6c);
	palette_set_color(inverse ? 3 : 4, 0x93, 0x93, 0x93);
	palette_set_color(inverse ? 2 : 5, 0xb1, 0xb1, 0xb1);
	palette_set_color(inverse ? 1 : 6, 0xe1, 0xe1, 0xe1);
	palette_set_color(inverse ? 0 : 7, 0xff, 0xff, 0xff);
}


static PALETTE_INIT( starshp1 )
{
	static const UINT16 colortable_source[] =
	{
		0, 3,       /* for the alpha numerics */
		0, 2,       /* for the sprites (Z=0) */
		0, 5,       /* for the sprites (Z=1) */
		0, 2, 4, 6, /* for the spaceship (EXPLODE=0) */
		0, 6, 6, 7  /* for the spaceship (EXPLODE=1) */
	};

	starshp1_write_palette(0);

	memcpy(colortable, colortable_source, sizeof(colortable_source));
}


static WRITE8_HANDLER( starshp1_audio_w )
{
	data &= 1;

	switch (offset & 7)
	{
	case 0:
		starshp1_attract = data;
		break;
	case 1:
		starshp1_phasor = data;
		break;
	case 2:
		/* KICKER */
		break;
	case 3:
		/* SL1 */
		break;
	case 4:
		/* SL2 */
		break;
	case 5:
		/* MOLVL */
		break;
	case 6:
		/* NOISE FREQ */
		break;
	}

	coin_lockout_w(0, !starshp1_attract);
	coin_lockout_w(1, !starshp1_attract);
}


static WRITE8_HANDLER( starshp1_collision_reset_w )
{
	starshp1_collision_latch = 0;
}


static READ8_HANDLER( starshp1_port_1_r )
{
	int val = 0;

	switch (starshp1_analog_in_select)
	{
	case 0:
		val = readinputport(4);
		break;
	case 1:
		val = readinputport(5);
		break;
	case 2:
		val = 0x20; /* DAC feedback, not used */
		break;
	case 3:
		val = readinputport(3);
		break;
	}

	return (val & 0x3f) | readinputport(1);
}


static READ8_HANDLER( starshp1_port_2_r )
{
	return readinputport(2) | (starshp1_collision_latch & 0x0f);
}


static WRITE8_HANDLER( starshp1_analog_in_w )
{
	starshp1_analog_in_select = offset & 3;
}


static WRITE8_HANDLER( starshp1_analog_out_w )
{
	switch (offset & 7)
	{
	case 1:
		starshp1_ship_size = data;
		break;
	case 2:
		/* NOISE AMPLITUDE */
		break;
	case 3:
		/* TONE PITCH */
		break;
	case 4:
		/* MOTOR SPEED */
		break;
	case 5:
		starshp1_circle_hpos = data;
		break;
	case 6:
		starshp1_circle_vpos = data;
		break;
	case 7:
		starshp1_circle_size = data;
		break;
	}
}


static WRITE8_HANDLER( starshp1_misc_w )
{
	data &= 1;

	switch (offset & 7)
	{
	case 0:
		starshp1_ship_explode = data;
		break;
	case 1:
		starshp1_circle_mod = data;
		break;
	case 2:
		starshp1_circle_kill = !data;
		break;
	case 3:
		starshp1_starfield_kill = data;
		break;
	case 4:
		starshp1_write_palette(data);
		break;
	case 5:
		/* BLACK HOLE, not used */
		break;
	case 6:
		starshp1_mux = data;
		break;
	case 7:
		set_led_status(0, !data);
		break;
	}
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x00ff) AM_READ(MRA8_RAM) AM_MIRROR(0x100)
	AM_RANGE(0x2c00, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xa000, 0xa000) AM_READ(input_port_0_r)
	AM_RANGE(0xb000, 0xb000) AM_READ(starshp1_port_1_r)
	AM_RANGE(0xc400, 0xc400) AM_READ(starshp1_port_2_r)
	AM_RANGE(0xd800, 0xd800) AM_READ(starshp1_rng_r)
	AM_RANGE(0xf000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x00ff) AM_WRITE(MWA8_RAM) AM_MIRROR(0x100)
	AM_RANGE(0x2c00, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc300, 0xc3ff) AM_WRITE(starshp1_sspic_w) /* spaceship picture */
	AM_RANGE(0xc400, 0xc4ff) AM_WRITE(starshp1_ssadd_w) /* spaceship address */
	AM_RANGE(0xc800, 0xc9ff) AM_WRITE(starshp1_playfield_w) AM_BASE(&starshp1_playfield_ram)
	AM_RANGE(0xcc00, 0xcc0f) AM_WRITE(MWA8_RAM) AM_BASE(&starshp1_hpos_ram)
	AM_RANGE(0xd000, 0xd00f) AM_WRITE(MWA8_RAM) AM_BASE(&starshp1_vpos_ram)
	AM_RANGE(0xd400, 0xd40f) AM_WRITE(MWA8_RAM) AM_BASE(&starshp1_obj_ram)
	AM_RANGE(0xd800, 0xd80f) AM_WRITE(starshp1_collision_reset_w)
	AM_RANGE(0xdc00, 0xdc0f) AM_WRITE(starshp1_misc_w)
	AM_RANGE(0xdd00, 0xdd0f) AM_WRITE(starshp1_analog_in_w)
	AM_RANGE(0xde00, 0xde0f) AM_WRITE(starshp1_audio_w)
	AM_RANGE(0xdf00, 0xdf0f) AM_WRITE(starshp1_analog_out_w)
	AM_RANGE(0xf000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END


INPUT_PORTS_START( starshp1 )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN ) /* SWA1? */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_DIPNAME( 0x20, 0x20, "Extended Play" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_TOGGLE
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_HIGH, IPT_UNUSED ) /* analog in */
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )

	PORT_START
	PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_UNUSED ) /* collision latch */
	PORT_DIPNAME( 0x70, 0x20, DEF_STR( Coinage ))
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ))
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED ) /* ground */

	PORT_START
	PORT_DIPNAME( 0x3f, 0x20, "Play Time" ) /* potentiometer */
	PORT_DIPSETTING(    0x00, "60 Seconds" )
	PORT_DIPSETTING(    0x20, "90 Seconds" )
	PORT_DIPSETTING(    0x3f, "120 Seconds" )

	PORT_START
	PORT_BIT( 0x3f, 0x20, IPT_AD_STICK_Y ) PORT_MINMAX(0,63) PORT_SENSITIVITY(10) PORT_KEYDELTA(10) PORT_REVERSE

	PORT_START
	PORT_BIT( 0x3f, 0x20, IPT_AD_STICK_X ) PORT_MINMAX(0,63) PORT_SENSITIVITY(10) PORT_KEYDELTA(10) PORT_REVERSE
INPUT_PORTS_END


static const gfx_layout tilelayout =
{
	16, 8,  /* 16x8 tiles      */
	64,     /* 64 tiles        */
	1,      /* 1 bit per pixel */
	{ 0 },
	{
		0x204, 0x204, 0x205, 0x205, 0x206, 0x206, 0x207, 0x207,
		0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007
	},
	{
		0x0000, 0x0400, 0x0800, 0x0c00,
		0x1000, 0x1400, 0x1800, 0x1c00
	},
	8	    /* step */
};


static const gfx_layout spritelayout =
{
	16, 8,  /* 16x8 sprites    */
	8,      /* 8 sprites       */
	1,      /* 1 bit per pixel */
	{ 0 },
	{
		0x04, 0x05, 0x06, 0x07, 0x0c, 0x0d, 0x0e, 0x0f,
		0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f
	},
	{
		0x00, 0x20, 0x40, 0x60, 0x80, 0xa0, 0xc0, 0xe0
	},
	0x100	/* step */
};

static const UINT32 shiplayout_xoffset[64] =
{
		0x04, 0x05, 0x06, 0x07, 0x0c, 0x0d, 0x0e, 0x0f,
		0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f,
		0x24, 0x25, 0x26, 0x27, 0x2c, 0x2d, 0x2e, 0x2f,
		0x34, 0x35, 0x36, 0x37, 0x3c, 0x3d, 0x3e, 0x3f,
		0x44, 0x45, 0x46, 0x47, 0x4c, 0x4d, 0x4e, 0x4f,
		0x54, 0x55, 0x56, 0x57, 0x5c, 0x5d, 0x5e, 0x5f,
		0x64, 0x65, 0x66, 0x67, 0x6c, 0x6d, 0x6e, 0x6f,
		0x74, 0x75, 0x76, 0x77, 0x7c, 0x7d, 0x7e, 0x7f
};

static const gfx_layout shiplayout =
{
	64, 16, /* 64x16 sprites    */
	4,      /* 4 sprites        */
	2,      /* 2 bits per pixel */
	{ 0, 0x2000 },
	EXTENDED_XOFFS,
	{ STEP16(0x000, 0x080) },
	0x800,  /* step */
	shiplayout_xoffset,
	NULL
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tilelayout,   0, 1 },
	{ REGION_GFX2, 0, &spritelayout, 2, 2 },
	{ REGION_GFX3, 0, &shiplayout,   6, 2 },
	{ -1 } /* end of array */
};


static MACHINE_DRIVER_START( starshp1 )

	/* basic machine hardware */

	MDRV_CPU_ADD(M6502, 750000)
	MDRV_CPU_PROGRAM_MAP(readmem, writemem)
	MDRV_CPU_VBLANK_INT(starshp1_interrupt, 1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(22 * 1000000 / 15750)

	/* video hardware */

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(512, 240)
	MDRV_VISIBLE_AREA(0, 511, 0, 239)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(8)
	MDRV_COLORTABLE_LENGTH(14)
	MDRV_PALETTE_INIT(starshp1)

	MDRV_VIDEO_START(starshp1)
	MDRV_VIDEO_UPDATE(starshp1)
	MDRV_VIDEO_EOF(starshp1)

	/* sound hardware */
MACHINE_DRIVER_END


/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( starshp1 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD_NIB_HIGH( "7529-02.c2", 0x2c00, 0x0400, CRC(f191c328) SHA1(5d44be879bcf16a142a69e4f1501533e02720fe5) )
	ROM_LOAD_NIB_LOW ( "7528-02.c1", 0x2c00, 0x0400, CRC(605ed4df) SHA1(b0d892bcd08b611d2c01ab23b491c1d9db498e7b) )
	ROM_LOAD(          "7530-02.h3", 0x3000, 0x0800, CRC(4b2d466c) SHA1(2104c4d163adbf53f9853334868622752ccb01b8) )
	ROM_RELOAD(                      0xf000, 0x0800 )
	ROM_LOAD(          "7531-02.e3", 0x3800, 0x0800, CRC(b35b2c0e) SHA1(e52240cdfbba3dc380ba63f24cfc07b44feafd53) )
	ROM_RELOAD(                      0xf800, 0x0800 )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "7513-01.n7",	 0x0000, 0x0400, CRC(8fb0045d) SHA1(fb311c6977dec6e2a04179406e9ffdb920989a47) )

	ROM_REGION( 0x0100, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "7515-01.j5",	 0x0000, 0x0100, CRC(fcbcbf2e) SHA1(adf3cc43b77ad18eddbe39ee11625e552d1abab9) )

	ROM_REGION( 0x0800, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "7517-01.r1",	 0x0000, 0x0400, CRC(1531f85f) SHA1(291822614fc6d3a71bf56607c796e18779f8cfc9) )
	ROM_LOAD( "7516-01.p1",	 0x0400, 0x0400, CRC(64fbfe4c) SHA1(b2dfdcc1c9927c693fe43b2e1411d0f14375fdeb) )

	ROM_REGION( 0x0220, REGION_PROMS, ROMREGION_DISPOSE )
	ROM_LOAD( "7518-01.r10", 0x0000, 0x0100, CRC(80877f7e) SHA1(8b28f48936a4247c583ca6713bfbaf4772c7a4f5) ) /* video output */
	ROM_LOAD( "7514-01.n9",  0x0100, 0x0100, CRC(3610b453) SHA1(9e33ee04f22a9174c29fafb8e71781fa330a7a08) ) /* sync */
	ROM_LOAD( "7519-01.b5",  0x0200, 0x0020, CRC(23b9cd3c) SHA1(220f9f73d86cdcf1b390c52c591750a73402af50) ) /* address */
ROM_END

ROM_START( starshpp )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD_NIB_HIGH( "7529-02.c2", 0x2c00, 0x0400, CRC(f191c328) SHA1(5d44be879bcf16a142a69e4f1501533e02720fe5) )
	ROM_LOAD_NIB_LOW ( "7528-02.c1", 0x2c00, 0x0400, CRC(605ed4df) SHA1(b0d892bcd08b611d2c01ab23b491c1d9db498e7b) )
	ROM_LOAD_NIB_HIGH( "7521.h2", 0x3000, 0x0400, CRC(6e3525db) SHA1(b615c60e4958d6576f4c179bbead9e8d330bba99) )
	ROM_RELOAD(                   0xf000, 0x0400 )
	ROM_LOAD_NIB_LOW ( "7520.h1", 0x3000, 0x0400, CRC(2fbed61b) SHA1(5cbe1aee82a32edbf33780a46e4166ec45c88170) )
	ROM_RELOAD(                   0xf000, 0x0400 )
	ROM_LOAD_NIB_HIGH( "f2",      0x3400, 0x0400, CRC(590ea913) SHA1(4baf5a6f6c9dcc5916163f85cec01d78a339ae20) )
	ROM_RELOAD(                   0xf400, 0x0400 )
	ROM_LOAD_NIB_LOW ( "f1",      0x3400, 0x0400, CRC(84fce404) SHA1(edd78f5439c4087c4a853d66446433f9a356b17f) )
	ROM_RELOAD(                   0xf400, 0x0400 )
	ROM_LOAD_NIB_HIGH( "7525.e2", 0x3800, 0x0400, CRC(5c6d12d9) SHA1(7078b685d859fd4122b814e473c83647b81ef7cd) )
	ROM_RELOAD(                   0xf800, 0x0400 )
	ROM_LOAD_NIB_LOW ( "7524.e1", 0x3800, 0x0400, CRC(6193a7bd) SHA1(3c9eab14481cb29ba2627bc73434f579d6b96a6e) )
	ROM_RELOAD(                   0xf800, 0x0400 )
	ROM_LOAD_NIB_HIGH( "d2",      0x3c00, 0x0400, CRC(a17df2ea) SHA1(ec488f4af47594e20b3d51882ee862a92e2f38fd) )
	ROM_RELOAD(                   0xfc00, 0x0400 )
	ROM_LOAD_NIB_LOW ( "d1",      0x3c00, 0x0400, CRC(be4050b6) SHA1(03ca4833769efb10f18f52b7ba4d016568d3cab9) )
	ROM_RELOAD(                   0xfc00, 0x0400 )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "7513-01.n7", 0x0000, 0x0400, CRC(8fb0045d) SHA1(fb311c6977dec6e2a04179406e9ffdb920989a47) )

	ROM_REGION( 0x0100, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "7515-01.j5", 0x0000, 0x0100, CRC(fcbcbf2e) SHA1(adf3cc43b77ad18eddbe39ee11625e552d1abab9) )

	ROM_REGION( 0x0800, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "7517-01.r1", 0x0000, 0x0400, CRC(1531f85f) SHA1(291822614fc6d3a71bf56607c796e18779f8cfc9) )
	ROM_LOAD( "7516-01.p1", 0x0400, 0x0400, CRC(64fbfe4c) SHA1(b2dfdcc1c9927c693fe43b2e1411d0f14375fdeb) )

	ROM_REGION( 0x0220, REGION_PROMS, ROMREGION_DISPOSE )
	ROM_LOAD( "7518-01.r10", 0x0000, 0x0100, CRC(80877f7e) SHA1(8b28f48936a4247c583ca6713bfbaf4772c7a4f5) ) /* video output */
	ROM_LOAD( "7514-01.n9",  0x0100, 0x0100, CRC(3610b453) SHA1(9e33ee04f22a9174c29fafb8e71781fa330a7a08) ) /* sync */
	ROM_LOAD( "7519-01.b5",  0x0200, 0x0020, CRC(23b9cd3c) SHA1(220f9f73d86cdcf1b390c52c591750a73402af50) ) /* address */
ROM_END


GAME( 1977, starshp1, 0,        starshp1, starshp1, 0, ORIENTATION_FLIP_X, "Atari", "Starship 1",              GAME_NO_SOUND ,0)
GAME( 1977, starshpp, starshp1, starshp1, starshp1, 0, ORIENTATION_FLIP_X, "Atari", "Starship 1 (prototype?)", GAME_NO_SOUND ,0)
