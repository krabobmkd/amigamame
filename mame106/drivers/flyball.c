/***************************************************************************

Atari Flyball Driver

***************************************************************************/

#include "driver.h"

extern VIDEO_START( flyball );
extern VIDEO_UPDATE( flyball );

extern UINT8 flyball_pitcher_pic;
extern UINT8 flyball_pitcher_vert;
extern UINT8 flyball_pitcher_horz;
extern UINT8 flyball_ball_vert;
extern UINT8 flyball_ball_horz;

extern UINT8* flyball_playfield_ram;

static UINT8 flyball_potmask;
static UINT8 flyball_potsense;

static UINT8 *rombase;


static void flyball_joystick_callback(int potsense)
{
	if (potsense & ~flyball_potmask)
	{
		cpunum_set_input_line(0, 0, PULSE_LINE);
	}

	flyball_potsense |= potsense;
}


static void flyball_quarter_callback(int scanline)
{
	int potsense[64], i;

	memset(potsense, 0, sizeof potsense);

	potsense[readinputport(1)] |= 1;
	potsense[readinputport(2)] |= 2;
	potsense[readinputport(3)] |= 4;
	potsense[readinputport(4)] |= 8;

	for (i = 0; i < 64; i++)
	{
		if (potsense[i] != 0)
		{
			timer_set(cpu_getscanlinetime(scanline + i), potsense[i], flyball_joystick_callback);
		}
	}

	scanline += 0x40;
	scanline &= 0xff;

	timer_set(cpu_getscanlinetime(scanline), scanline, flyball_quarter_callback);

	flyball_potsense = 0;
	flyball_potmask = 0;
}


static MACHINE_RESET( flyball )
{
	int i;

	/* address bits 0 through 8 are inverted */

	UINT8* ROM = memory_region(REGION_CPU1) + 0x2000;

	for (i = 0; i < 0x1000; i++)
		rombase[i] = ROM[i ^ 0x1ff];

	timer_set(cpu_getscanlinetime(0), 0, flyball_quarter_callback);
}


/* two physical buttons (start game and stop runner) share the same port bit */

READ8_HANDLER( flyball_input_r )
{
	return readinputport(0) & readinputport(5);
}

READ8_HANDLER( flyball_scanline_r )
{
	return cpu_getscanline() & 0x3f;
}

READ8_HANDLER( flyball_potsense_r )
{
	return flyball_potsense & ~flyball_potmask;
}

WRITE8_HANDLER( flyball_potmask_w )
{
	flyball_potmask |= data & 0xf;
}

WRITE8_HANDLER( flyball_pitcher_pic_w )
{
	flyball_pitcher_pic = data & 0xf;
}

WRITE8_HANDLER( flyball_ball_vert_w )
{
	flyball_ball_vert = data;
}

WRITE8_HANDLER( flyball_ball_horz_w )
{
	flyball_ball_horz = data;
}

WRITE8_HANDLER( flyball_pitcher_vert_w )
{
	flyball_pitcher_vert = data;
}

WRITE8_HANDLER( flyball_pitcher_horz_w )
{
	flyball_pitcher_horz = data;
}

WRITE8_HANDLER( flyball_misc_w )
{
	int bit = ~data & 1;

	switch (offset)
	{
	case 0:
		set_led_status(0, bit);
		break;
	case 1:
		/* crowd very loud */
		break;
	case 2:
		/* footstep off-on */
		break;
	case 3:
		/* crowd off-on */
		break;
	case 4:
		/* crowd soft-loud */
		break;
	case 5:
		/* bat hit */
		break;
	}
}


static ADDRESS_MAP_START( flyball_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(13) )
	AM_RANGE(0x0000, 0x00ff) AM_MIRROR(0x100) AM_RAM
	AM_RANGE(0x0800, 0x0800) AM_NOP
	AM_RANGE(0x0801, 0x0801) AM_WRITE(flyball_pitcher_pic_w)
	AM_RANGE(0x0802, 0x0802) AM_READ(flyball_scanline_r)
	AM_RANGE(0x0803, 0x0803) AM_READ(flyball_potsense_r)
	AM_RANGE(0x0804, 0x0804) AM_WRITE(flyball_ball_vert_w)
	AM_RANGE(0x0805, 0x0805) AM_WRITE(flyball_ball_horz_w)
	AM_RANGE(0x0806, 0x0806) AM_WRITE(flyball_pitcher_vert_w)
	AM_RANGE(0x0807, 0x0807) AM_WRITE(flyball_pitcher_horz_w)
	AM_RANGE(0x0900, 0x0900) AM_WRITE(flyball_potmask_w)
	AM_RANGE(0x0a00, 0x0a07) AM_WRITE(flyball_misc_w)
	AM_RANGE(0x0b00, 0x0b00) AM_READ(flyball_input_r)
	AM_RANGE(0x0d00, 0x0eff) AM_WRITE(MWA8_RAM) AM_BASE(&flyball_playfield_ram)
	AM_RANGE(0x1000, 0x1fff) AM_ROM AM_BASE(&rombase) /* program */
ADDRESS_MAP_END


INPUT_PORTS_START( flyball )
	PORT_START /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_SERVICE( 0x08, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING( 0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING( 0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING( 0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x40, 0x40, "Innings Per Game" )
	PORT_DIPSETTING( 0x00, "1" )
	PORT_DIPSETTING( 0x40, "2" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING( 0x00, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x80, DEF_STR( On ) )

	PORT_START /* IN1 */
	PORT_BIT( 0x3f, 0x20, IPT_AD_STICK_Y ) PORT_MINMAX(1,63) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START /* IN2 */
	PORT_BIT( 0x3f, 0x20, IPT_AD_STICK_X ) PORT_MINMAX(1,63) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START /* IN3 */
	PORT_BIT( 0x3f, 0x20, IPT_AD_STICK_Y ) PORT_MINMAX(1,63) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START /* IN4 */
	PORT_BIT( 0x3f, 0x20, IPT_AD_STICK_X ) PORT_MINMAX(1,63) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START /* IN5 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0xFE, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static const gfx_layout flyball_tiles_layout =
{
	8, 16,    /* width, height */
	128,      /* total         */
	1,        /* planes        */
	{ 0 },    /* plane offsets */
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
	},
	{
		0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
		0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78
	},
	0x80      /* increment */
};

static const gfx_layout flyball_sprites_layout =
{
	16, 16,   /* width, height */
	16,       /* total         */
	1,        /* planes        */
	{ 0 },    /* plane offsets */
	{
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
		0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
	},
	{
		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
		0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0
	},
	0x100     /* increment */
};

static const gfx_decode flyball_gfx_decode_info[] =
{
	{ REGION_GFX1, 0, &flyball_tiles_layout, 0, 2 },
	{ REGION_GFX2, 0, &flyball_sprites_layout, 2, 2 },
	{ -1 } /* end of array */
};


PALETTE_INIT( flyball )
{
	palette_set_color(0, 0x3F, 0x3F, 0x3F);  /* tiles, ball */
	palette_set_color(1, 0xFF, 0xFF, 0xFF);
	palette_set_color(2, 0xFF ,0xFF, 0xFF);  /* sprites */
	palette_set_color(3, 0x00, 0x00, 0x00);
}


static MACHINE_DRIVER_START( flyball )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 12096000 / 16)
	MDRV_CPU_PROGRAM_MAP(flyball_map, 0)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse, 1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION((int) ((22. * 1000000) / (262. * 60) + 0.5))

	MDRV_MACHINE_RESET(flyball)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 240)
	MDRV_VISIBLE_AREA(0, 255, 0, 239)
	MDRV_GFXDECODE(flyball_gfx_decode_info)
	MDRV_PALETTE_LENGTH(4)

	MDRV_PALETTE_INIT(flyball)
	MDRV_VIDEO_UPDATE(flyball)
	MDRV_VIDEO_START(flyball)

	/* sound hardware */
MACHINE_DRIVER_END


ROM_START( flyball )
	ROM_REGION( 0x3000, REGION_CPU1, 0 )                  /* program */
	ROM_LOAD( "6129.d5", 0x2000, 0x0200, CRC(17eda069) SHA1(e4ef0bf4546cf00668d759a188e0989a4f003825) )
	ROM_LOAD( "6130.f5", 0x2200, 0x0200, CRC(a756955b) SHA1(220b7f1789bba4481d595b36b4bae25f98d3ad8d) )
	ROM_LOAD( "6131.h5", 0x2400, 0x0200, CRC(a9c7e858) SHA1(aee4a359d6a5729dc1be5b8ce8fbe54d032d12b0) )
	ROM_LOAD( "6132.j5", 0x2600, 0x0200, CRC(31fefd8a) SHA1(97e3ef278ce2175cd33c0f3147bdf7974752c836) )
	ROM_LOAD( "6133.k5", 0x2800, 0x0200, CRC(6fdb09b1) SHA1(04ad412b437bb24739b3e31fa5a413e63d5897f8) )
	ROM_LOAD( "6134.m5", 0x2A00, 0x0200, CRC(7b526c73) SHA1(e47c8f33b7edc143ab1713556c59b93571933daa) )
	ROM_LOAD( "6135.n5", 0x2C00, 0x0200, CRC(b352cb51) SHA1(39b9062fb51d0a78a47dcd470ceae47fcdbd7891) )
	ROM_LOAD( "6136.r5", 0x2E00, 0x0200, CRC(1622d890) SHA1(9ad342aefdc02e022eb79d84d1c856bed538bebe) )

	ROM_REGION( 0x0C00, REGION_GFX1, ROMREGION_DISPOSE )   /* tiles */
	ROM_LOAD( "6142.l2", 0x0000, 0x0200, CRC(65650cfa) SHA1(7d17455146fc9def22c7bd06f7fde32df0a0c2bc) )
	ROM_LOAD( "6139.j2", 0x0200, 0x0200, CRC(a5d1358e) SHA1(33cecbe40ae299549a3395e3dffbe7b6021803ba) )
	ROM_LOAD( "6141.m2", 0x0400, 0x0200, CRC(98b5f803) SHA1(c4e323ced2393fa4a9720ff0086c559fb9b3a9f8) )
	ROM_LOAD( "6140.k2", 0x0600, 0x0200, CRC(66aeec61) SHA1(f577bad015fe9e3708fd95d5d2bc438997d14d2c) )

	ROM_REGION( 0x0400, REGION_GFX2, ROMREGION_DISPOSE )   /* sprites */
	ROM_LOAD16_BYTE( "6137.e2", 0x0000, 0x0200, CRC(68961fda) SHA1(a06c7b453cce04716f49bd65ecfe1ba67cb8681e) )
	ROM_LOAD16_BYTE( "6138.f2", 0x0001, 0x0200, CRC(aab314f6) SHA1(6625c719fdc000d6af94bc9474de8f7e977cee97) )
ROM_END


GAME( 1976, flyball, 0, flyball, flyball, 0, 0, "Atari", "Flyball", GAME_NO_SOUND ,0)
