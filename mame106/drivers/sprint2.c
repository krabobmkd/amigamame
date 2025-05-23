/***************************************************************************

    Atari Sprint 2 hardware

    driver by Mike Balfour

    Games supported:
        * Sprint 1
        * Sprint 2
        * Dominos

    All three games run on the same PCB but with minor modifications (some
    chips removed, some wire-wrap connections added).

    If you have any questions about how this driver works, don't hesitate to
    ask.  - Mike Balfour (mab22@po.cwru.edu)

***************************************************************************/

#include "driver.h"
#include "sprint2.h"
#include "sound/discrete.h"

#define GAME_IS_SPRINT1   (game == 1)
#define GAME_IS_SPRINT2   (game == 2)
#define GAME_IS_DOMINOS   (game == 3)



static int attract;
static int steering[2];
static int gear[2];
static int game;


static DRIVER_INIT( sprint1 )
{
	game = 1;
}
static DRIVER_INIT( sprint2 )
{
	game = 2;
}
static DRIVER_INIT( dominos )
{
	game = 3;
}


static int service_mode(void)
{
	UINT8 v = readinputport(2);

	if (GAME_IS_SPRINT1)
	{
		return (v & 0x10) == 0;
	}
	if (GAME_IS_SPRINT2)
	{
		return (v & 0x04) == 0;
	}
	if (GAME_IS_DOMINOS)
	{
		return (v & 0x40) == 0;
	}

	return 0;
}


static INTERRUPT_GEN( sprint2 )
{
	static UINT8 dial[2];

	/* handle steering wheels */

	if (GAME_IS_SPRINT1 || GAME_IS_SPRINT2)
	{
		int i;

		for (i = 0; i < 2; i++)
		{
			signed char delta = readinputport(6 + i) - dial[i];

			if (delta < 0)
			{
				steering[i] = 0x00;
			}
			if (delta > 0)
			{
				steering[i] = 0x40;
			}

			dial[i] += delta;

			switch (readinputport(4 + i) & 15)
			{
			case 1: gear[i] = 1; break;
			case 2: gear[i] = 2; break;
			case 4: gear[i] = 3; break;
			case 8: gear[i] = 4; break;
			}
		}
	}

	discrete_sound_w(SPRINT2_MOTORSND1_DATA, sprint2_video_ram[0x394] & 15);	// also DOMINOS_FREQ_DATA
	discrete_sound_w(SPRINT2_MOTORSND2_DATA, sprint2_video_ram[0x395] & 15);
	discrete_sound_w(SPRINT2_CRASHSND_DATA, sprint2_video_ram[0x396] & 15);	// also DOMINOS_AMP_DATA

	/* interrupts and watchdog are disabled during service mode */

	watchdog_enable(!service_mode());

	if (!service_mode())
	{
		cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	}
}


static PALETTE_INIT( sprint2 )
{
	palette_set_color(0, 0x00, 0x00, 0x00);
	palette_set_color(1, 0x5b, 0x5b, 0x5b);
	palette_set_color(2, 0xa4, 0xa4, 0xa4);
	palette_set_color(3, 0xff, 0xff, 0xff);

	colortable[0x0] = 1;	/* black playfield */
	colortable[0x1] = 0;
	colortable[0x2] = 1;	/* white playfield */
	colortable[0x3] = 3;

	colortable[0x4] = 1;	/* car #1 */
	colortable[0x5] = 3;
	colortable[0x6] = 1;	/* car #2 */
	colortable[0x7] = 0;
	colortable[0x8] = 1;	/* car #3 */
	colortable[0x9] = 2;
	colortable[0xa] = 1;	/* car #4 */
	colortable[0xb] = 2;
}


static READ8_HANDLER( sprint2_wram_r )
{
	return sprint2_video_ram[0x380 + offset % 0x80];
}


static READ8_HANDLER( sprint2_dip_r )
{
	return (readinputport(0) << (2 * ((offset & 3) ^ 3))) & 0xc0;
}


static READ8_HANDLER( sprint2_input_A_r )
{
	UINT8 val = readinputport(1);

	if (GAME_IS_SPRINT2)
	{
		if (gear[0] == 1) val &= ~0x01;
		if (gear[1] == 1) val &= ~0x02;
		if (gear[0] == 2) val &= ~0x04;
		if (gear[1] == 2) val &= ~0x08;
		if (gear[0] == 3) val &= ~0x10;
		if (gear[1] == 3) val &= ~0x20;
	}

	return (val << (offset ^ 7)) & 0x80;
}


static READ8_HANDLER( sprint2_input_B_r )
{
	UINT8 val = readinputport(2);

	if (GAME_IS_SPRINT1)
	{
		if (gear[0] == 1) val &= ~0x01;
		if (gear[0] == 2) val &= ~0x02;
		if (gear[0] == 3) val &= ~0x04;
	}

	return (val << (offset ^ 7)) & 0x80;
}


static READ8_HANDLER( sprint2_sync_r )
{
	UINT8 val = 0;

	if (attract != 0)
	{
		val |= 0x10;
	}
	if (cpu_getscanline() == 261)
	{
		val |= 0x20; /* VRESET */
	}
	if (cpu_getscanline() >= 224)
	{
		val |= 0x40; /* VBLANK */
	}
	if (cpu_getscanline() >= 131)
	{
		val |= 0x80; /* 60 Hz? */
	}

	return val;
}


static READ8_HANDLER( sprint2_steering1_r )
{
	return steering[0];
}
static READ8_HANDLER( sprint2_steering2_r )
{
	return steering[1];
}


static WRITE8_HANDLER( sprint2_steering_reset1_w )
{
	steering[0] |= 0x80;
}
static WRITE8_HANDLER( sprint2_steering_reset2_w )
{
	steering[1] |= 0x80;
}


static WRITE8_HANDLER( sprint2_wram_w )
{
	sprint2_video_ram[0x380 + offset % 0x80] = data;
}


static WRITE8_HANDLER( sprint2_attract_w )
{
	attract = offset & 1;

	// also DOMINOS_ATTRACT_EN
	discrete_sound_w(SPRINT2_ATTRACT_EN, attract);
}


static WRITE8_HANDLER( sprint2_noise_reset_w )
{
	discrete_sound_w(SPRINT2_NOISE_RESET, 0);
}


static WRITE8_HANDLER( sprint2_skid1_w )
{
	// also DOMINOS_TUMBLE_EN
	discrete_sound_w(SPRINT2_SKIDSND1_EN, offset & 1);
}

static WRITE8_HANDLER( sprint2_skid2_w )
{
	discrete_sound_w(SPRINT2_SKIDSND2_EN, offset & 1);
}


static WRITE8_HANDLER( sprint2_lamp1_w )
{
	set_led_status(0, offset & 1);
}
static WRITE8_HANDLER( sprint2_lamp2_w )
{
	set_led_status(1, offset & 1);
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x03ff) AM_READ(sprint2_wram_r)
	AM_RANGE(0x0400, 0x07ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x0818, 0x081f) AM_READ(sprint2_input_A_r)
	AM_RANGE(0x0828, 0x082f) AM_READ(sprint2_input_B_r)
	AM_RANGE(0x0830, 0x0837) AM_READ(sprint2_dip_r)
	AM_RANGE(0x0840, 0x087f) AM_READ(input_port_3_r)
	AM_RANGE(0x0880, 0x08bf) AM_READ(sprint2_steering1_r)
	AM_RANGE(0x08c0, 0x08ff) AM_READ(sprint2_steering2_r)
	AM_RANGE(0x0c00, 0x0fff) AM_READ(sprint2_sync_r)
	AM_RANGE(0x1000, 0x13ff) AM_READ(sprint2_collision1_r)
	AM_RANGE(0x1400, 0x17ff) AM_READ(sprint2_collision2_r)
	AM_RANGE(0x1800, 0x1800) AM_READ(MRA8_NOP)  /* debugger ROM location? */
	AM_RANGE(0x2000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xe000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x03ff) AM_WRITE(sprint2_wram_w)
	AM_RANGE(0x0400, 0x07ff) AM_WRITE(sprint2_video_ram_w) AM_BASE(&sprint2_video_ram)
	AM_RANGE(0x0c00, 0x0c0f) AM_WRITE(sprint2_attract_w)
	AM_RANGE(0x0c10, 0x0c1f) AM_WRITE(sprint2_skid1_w)
	AM_RANGE(0x0c20, 0x0c2f) AM_WRITE(sprint2_skid2_w)
	AM_RANGE(0x0c30, 0x0c3f) AM_WRITE(sprint2_lamp1_w)
	AM_RANGE(0x0c40, 0x0c4f) AM_WRITE(sprint2_lamp2_w)
	AM_RANGE(0x0c60, 0x0c6f) AM_WRITE(MWA8_NOP) /* SPARE */
	AM_RANGE(0x0c80, 0x0cff) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x0d00, 0x0d7f) AM_WRITE(sprint2_collision_reset1_w)
	AM_RANGE(0x0d80, 0x0dff) AM_WRITE(sprint2_collision_reset2_w)
	AM_RANGE(0x0e00, 0x0e7f) AM_WRITE(sprint2_steering_reset1_w)
	AM_RANGE(0x0e80, 0x0eff) AM_WRITE(sprint2_steering_reset2_w)
	AM_RANGE(0x0f00, 0x0f7f) AM_WRITE(sprint2_noise_reset_w)
	AM_RANGE(0x2000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xe000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END


INPUT_PORTS_START( sprint2 )
	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "Tracks on Demo" )
	PORT_DIPSETTING(    0x00, "Easy Track Only" )
	PORT_DIPSETTING(    0x01, "Cycle 12 Tracks" )
	PORT_DIPNAME( 0x02, 0x00, "Oil Slicks" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "Extended Play" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0x00, "Play Time" )
	PORT_DIPSETTING(    0xc0, "60 seconds" )
	PORT_DIPSETTING(    0x80, "90 seconds" )
	PORT_DIPSETTING(    0x40, "120 seconds" )
	PORT_DIPSETTING(    0x00, "150 seconds" )

	PORT_START /* input A */
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_UNUSED ) /* P1 1st gear */
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_UNUSED ) /* P2 1st gear */
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_UNUSED ) /* P1 2nd gear */
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_UNUSED ) /* P2 2nd gear */
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_UNUSED ) /* P1 3rd gear */
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_UNUSED ) /* P2 3rd gear */

	PORT_START /* input B */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Player 1 Gas") PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Player 2 Gas") PORT_PLAYER(2)
	PORT_SERVICE( 0x04, IP_ACTIVE_LOW )
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Track Select") PORT_CODE(KEYCODE_SPACE)

	PORT_START
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Player 1 Gear 1") PORT_CODE(KEYCODE_Z) PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Player 1 Gear 2") PORT_CODE(KEYCODE_X) PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Player 1 Gear 3") PORT_CODE(KEYCODE_C) PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Player 1 Gear 4") PORT_CODE(KEYCODE_V) PORT_PLAYER(1)

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Player 2 Gear 1") PORT_CODE(KEYCODE_Q) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Player 2 Gear 2") PORT_CODE(KEYCODE_W) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Player 2 Gear 3") PORT_CODE(KEYCODE_E) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Player 2 Gear 4") PORT_CODE(KEYCODE_R) PORT_PLAYER(2)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START
	PORT_ADJUSTER( 30, "Motor 1 RPM" )

	PORT_START
	PORT_ADJUSTER( 40, "Motor 2 RPM" )
INPUT_PORTS_END


INPUT_PORTS_START( sprint1 )
	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "Change Track" )
	PORT_DIPSETTING(    0x01, "Every Lap" )
	PORT_DIPSETTING(    0x00, "Every 2 Laps" )
	PORT_DIPNAME( 0x02, 0x00, "Oil Slicks" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "Extended Play" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0x00, "Play Time" )
	PORT_DIPSETTING(    0xc0, "60 seconds" )
	PORT_DIPSETTING(    0x80, "90 seconds" )
	PORT_DIPSETTING(    0x40, "120 seconds" )
	PORT_DIPSETTING(    0x00, "150 seconds" )

	PORT_START /* input A */

	PORT_START /* input B */
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_UNUSED ) /* 1st gear */
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_UNUSED ) /* 2nd gear */
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_UNUSED ) /* 3rd gear */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Gas")
	PORT_SERVICE( 0x10, IP_ACTIVE_LOW )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Gear 1") PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Gear 2") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Gear 3") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Gear 4") PORT_CODE(KEYCODE_V)

	PORT_START

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10)

	PORT_START

	PORT_START
	PORT_ADJUSTER( 30, "Motor RPM" )
INPUT_PORTS_END


INPUT_PORTS_START( dominos )
	PORT_START
	PORT_DIPNAME( 0x03, 0x01, "Points to Win" )
	PORT_DIPSETTING(	0x03, "6" )
	PORT_DIPSETTING(	0x02, "5" )
	PORT_DIPSETTING(	0x01, "4" )
	PORT_DIPSETTING(	0x00, "3" )
	PORT_DIPNAME( 0x0C, 0x08, DEF_STR( Coinage ) )
	PORT_DIPSETTING(	0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x08, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x0c, DEF_STR( 1C_2C ) )
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

	PORT_START /* input A */
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)

	PORT_START /* input B */
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )

	PORT_START
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START
	PORT_ADJUSTER( 50, "R23 - Tone Freq" )
INPUT_PORTS_END


static const gfx_layout tile_layout =
{
	16, 8,
	64,
	1,
	{ 0 },
	{
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7
	},
	{
		0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38
	},
	0x40
};


static const gfx_layout car_layout =
{
	16, 8,
	32,
	1,
	{ 0 },
	{
		0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
		0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8
	},
	{
		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70
	},
	0x80
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tile_layout, 0, 2 },
	{ REGION_GFX2, 0, &car_layout, 4, 4 },
	{ -1 }
};


static MACHINE_DRIVER_START( sprint2 )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 12096000 / 16)
	MDRV_CPU_PROGRAM_MAP(readmem, writemem)
	MDRV_CPU_VBLANK_INT(sprint2, 1)
	MDRV_WATCHDOG_VBLANK_INIT(8)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(38 * 1000000 / 15750)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(512, 262)
	MDRV_VISIBLE_AREA(0, 511, 0, 223)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4)
	MDRV_COLORTABLE_LENGTH(12)

	MDRV_PALETTE_INIT(sprint2)
	MDRV_VIDEO_START(sprint2)
	MDRV_VIDEO_UPDATE(sprint2)
	MDRV_VIDEO_EOF(sprint2)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD_TAG("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(sprint2_discrete_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( sprint1 )

	MDRV_IMPORT_FROM(sprint2)

	/* sound hardware */
	MDRV_SPEAKER_REMOVE("left")
	MDRV_SPEAKER_REMOVE("right")
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_REPLACE("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(sprint1_discrete_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( dominos )

	MDRV_IMPORT_FROM(sprint2)

	/* sound hardware */
	MDRV_SPEAKER_REMOVE("left")
	MDRV_SPEAKER_REMOVE("right")
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_REPLACE("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(dominos_discrete_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


ROM_START( sprint1 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "6290-01.b1", 0x2000, 0x0800, CRC(41fc985e) SHA1(7178846480cbf8d15955ccd987d0b0e902ab9f90) )
	ROM_RELOAD(             0xe000, 0x0800 )
	ROM_LOAD( "6291-01.c1", 0x2800, 0x0800, CRC(07f7a920) SHA1(845f65d2bd290eb295ca6bae2575f27aaa08c0dd) )
	ROM_RELOAD(             0xe800, 0x0800 )
	ROM_LOAD( "6442-01.d1", 0x3000, 0x0800, CRC(e9ff0124) SHA1(42fe028e2e595573ccc0821de3bb6970364c585d) )
	ROM_RELOAD(             0xf000, 0x0800 )
	ROM_LOAD( "6443-01.e1", 0x3800, 0x0800, CRC(d6bb00d0) SHA1(cdcd4bb7b32be7a11480d3312fcd8d536e2d0caf) )
	ROM_RELOAD(             0xf800, 0x0800 )

	ROM_REGION( 0x0200, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD_NIB_HIGH( "6396-01.p4", 0x0000, 0x0200, CRC(801b42dd) SHA1(1db58390d803f404253cbf36d562016441ca568d) )
	ROM_LOAD_NIB_LOW ( "6397-01.r4", 0x0000, 0x0200, CRC(135ba1aa) SHA1(0465259440f73e1a2c8d8101f29e99b4885420e4) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE ) /* cars */
	ROM_LOAD_NIB_HIGH( "6399-01.j6", 0x0000, 0x0200, CRC(63d685b2) SHA1(608746163e25dbc14cde43c17aecbb9a14fac875) )
	ROM_LOAD_NIB_LOW ( "6398-01.k6", 0x0000, 0x0200, CRC(c9e1017e) SHA1(e7279a13e4a812d2e0218be0bc5162f2e56c6b66) )

	ROM_REGION( 0x0120, REGION_PROMS, 0 )
	ROM_LOAD( "6400-01.m2", 0x0000, 0x0100, CRC(b8094b4c) SHA1(82dc6799a19984f3b204ee3aeeb007e55afc8be3) )	/* SYNC */
	ROM_LOAD( "6401-01.e2", 0x0100, 0x0020, CRC(857df8db) SHA1(06313d5bde03220b2bc313d18e50e4bb1d0cfbbb) )	/* address */
ROM_END


ROM_START( sprint2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "6290-01.b1", 0x2000, 0x0800, CRC(41fc985e) SHA1(7178846480cbf8d15955ccd987d0b0e902ab9f90) )
	ROM_RELOAD(             0xe000, 0x0800 )
	ROM_LOAD( "6291-01.c1", 0x2800, 0x0800, CRC(07f7a920) SHA1(845f65d2bd290eb295ca6bae2575f27aaa08c0dd) )
	ROM_RELOAD(             0xe800, 0x0800 )
	ROM_LOAD( "6404.d1",    0x3000, 0x0800, CRC(d2878ff6) SHA1(b742a8896c1bf1cfacf48d06908920d88a2c9ea8) )
	ROM_RELOAD(             0xf000, 0x0800 )
	ROM_LOAD( "6405.e1",    0x3800, 0x0800, CRC(6c991c80) SHA1(c30a5b340f05dd702c7a186eb62607a48fa19f72) )
	ROM_RELOAD(             0xf800, 0x0800 )

	ROM_REGION( 0x0200, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD_NIB_HIGH( "6396-01.p4", 0x0000, 0x0200, CRC(801b42dd) SHA1(1db58390d803f404253cbf36d562016441ca568d) )
	ROM_LOAD_NIB_LOW ( "6397-01.r4", 0x0000, 0x0200, CRC(135ba1aa) SHA1(0465259440f73e1a2c8d8101f29e99b4885420e4) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE ) /* cars */
	ROM_LOAD_NIB_HIGH( "6399-01.j6", 0x0000, 0x0200, CRC(63d685b2) SHA1(608746163e25dbc14cde43c17aecbb9a14fac875) )
	ROM_LOAD_NIB_LOW ( "6398-01.k6", 0x0000, 0x0200, CRC(c9e1017e) SHA1(e7279a13e4a812d2e0218be0bc5162f2e56c6b66) )

	ROM_REGION( 0x0120, REGION_PROMS, 0 )
	ROM_LOAD( "6400-01.m2", 0x0000, 0x0100, CRC(b8094b4c) SHA1(82dc6799a19984f3b204ee3aeeb007e55afc8be3) )	/* SYNC */
	ROM_LOAD( "6401-01.e2", 0x0100, 0x0020, CRC(857df8db) SHA1(06313d5bde03220b2bc313d18e50e4bb1d0cfbbb) )	/* address */
ROM_END


ROM_START( sprint2a )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "6290-01.b1", 0x2000, 0x0800, CRC(41fc985e) SHA1(7178846480cbf8d15955ccd987d0b0e902ab9f90) )
	ROM_RELOAD(             0xe000, 0x0800 )
	ROM_LOAD( "6291-01.c1", 0x2800, 0x0800, CRC(07f7a920) SHA1(845f65d2bd290eb295ca6bae2575f27aaa08c0dd) )
	ROM_RELOAD(             0xe800, 0x0800 )
	ROM_LOAD( "6404.d1",    0x3000, 0x0800, CRC(d2878ff6) SHA1(b742a8896c1bf1cfacf48d06908920d88a2c9ea8) )
	ROM_RELOAD(             0xf000, 0x0800 )
	ROM_LOAD( "6405-02.e1", 0x3800, 0x0800, CRC(e80fd249) SHA1(7bcf7dfd72ca83fdd80593eaf392570da1f71298) )
	ROM_RELOAD(             0xf800, 0x0800 )

	ROM_REGION( 0x0200, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD_NIB_HIGH( "6396-01.p4", 0x0000, 0x0200, CRC(801b42dd) SHA1(1db58390d803f404253cbf36d562016441ca568d) )
	ROM_LOAD_NIB_LOW ( "6397-01.r4", 0x0000, 0x0200, CRC(135ba1aa) SHA1(0465259440f73e1a2c8d8101f29e99b4885420e4) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE ) /* cars */
	ROM_LOAD_NIB_HIGH( "6399-01.j6", 0x0000, 0x0200, CRC(63d685b2) SHA1(608746163e25dbc14cde43c17aecbb9a14fac875) )
	ROM_LOAD_NIB_LOW ( "6398-01.k6", 0x0000, 0x0200, CRC(c9e1017e) SHA1(e7279a13e4a812d2e0218be0bc5162f2e56c6b66) )

	ROM_REGION( 0x0120, REGION_PROMS, 0 )
	ROM_LOAD( "6400-01.m2", 0x0000, 0x0100, CRC(b8094b4c) SHA1(82dc6799a19984f3b204ee3aeeb007e55afc8be3) )	/* SYNC */
	ROM_LOAD( "6401-01.e2", 0x0100, 0x0020, CRC(857df8db) SHA1(06313d5bde03220b2bc313d18e50e4bb1d0cfbbb) )	/* address */
ROM_END


ROM_START( dominos )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "7352-02.d1",   0x3000, 0x0800, CRC(738b4413) SHA1(3a90ab25bb5f65504692f97da43f03e21392dcd8) )
	ROM_RELOAD(               0xf000, 0x0800 )
	ROM_LOAD( "7438-02.e1",   0x3800, 0x0800, CRC(c84e54e2) SHA1(383b388a1448a195f28352fc5e4ff1a2af80cc95) )
	ROM_RELOAD(               0xf800, 0x0800 )

	ROM_REGION( 0x200, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD_NIB_HIGH( "7439-01.p4",   0x0000, 0x0200, CRC(4f42fdd6) SHA1(f8ea4b582e26cad37b746174cdc9f1c7ae0819c3) )
	ROM_LOAD_NIB_LOW ( "7440-01.r4",   0x0000, 0x0200, CRC(957dd8df) SHA1(280457392f40cd66eae34d2fcdbd4d2142793402) )

	ROM_REGION( 0x200, REGION_GFX2, ROMREGION_DISPOSE ) /* sprites, not used */
	ROM_FILL( 0x0000, 0x0200, 0 )

	ROM_REGION( 0x0120, REGION_PROMS, 0 )
	ROM_LOAD( "6400-01.m2", 0x0000, 0x0100, CRC(b8094b4c) SHA1(82dc6799a19984f3b204ee3aeeb007e55afc8be3) )	/* SYNC */
	ROM_LOAD( "6401-01.e2", 0x0100, 0x0020, CRC(857df8db) SHA1(06313d5bde03220b2bc313d18e50e4bb1d0cfbbb) )	/* address */
ROM_END


GAME( 1978, sprint1,  0,       sprint1, sprint1, sprint1, ROT0, "Atari", "Sprint 1", 0 ,1)
GAME( 1976, sprint2,  sprint1, sprint2, sprint2, sprint2, ROT0, "Atari", "Sprint 2 (set 1)", 0 ,1)
GAME( 1976, sprint2a, sprint1, sprint2, sprint2, sprint2, ROT0, "Atari", "Sprint 2 (set 2)", 0 ,1)
GAME( 1977, dominos,  0,       dominos, dominos, dominos, ROT0, "Atari", "Dominos", 0 ,2)
