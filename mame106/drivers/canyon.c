/***************************************************************************

    Atari Canyon Bomber hardware

    driver by Mike Balfour

    Games supported:
        * Canyon Bomber

    Known issues:
        * none at this time

****************************************************************************

    Memory Map:
        0000-01FF       WRAM
        0400-04FF       W A0=0:MOTOR1, A0=1:MOTOR2
        0500-05FF       W A0=0:EXPLODE, A0=1:TIMER RESET
        0600-067F       W A0=0:WHISTLE1, A0=1:WHISTLE2
        0680-06FF       W A0=0:LED1, A0=1:LED2
        0700-077F       W A0=0:ATTRACT1, A0=1:ATTRACT2
        0800-0FFF       DISPLAY / RAM
        1000-17FF       SWITCHES
        1800-1FFF       OPTIONS
        2000-27FF       ROM1
        2800-2FFF       ROM2
        3000-37FF       ROM3 (Language ROM)
        3800-3FFF       ROM4 (Program ROM)

    If you have any questions about how this driver works, don't hesitate to
    ask.  - Mike Balfour (mab22@po.cwru.edu)

***************************************************************************/

#include "driver.h"
#include "canyon.h"
#include "sound/discrete.h"




/*************************************
 *
 *  Palette generation
 *
 *************************************/

static PALETTE_INIT( canyon )
{
	palette_set_color(0, 0x00, 0x00, 0x00); /* BLACK */
	palette_set_color(1, 0xff, 0xff, 0xff); /* WHITE */
	palette_set_color(2, 0x80, 0x80, 0x80); /* GREY  */

	colortable[0] = 2;
	colortable[1] = 0;
	colortable[2] = 2;
	colortable[3] = 1;
}


/*************************************
 *
 *  Read handlers
 *
 *************************************/

static READ8_HANDLER( canyon_switches_r )
{
	UINT8 val = 0;

	if ((readinputport(2) >> (offset & 7)) & 1)
	{
		val |= 0x80;
	}
	if ((readinputport(1) >> (offset & 3)) & 1)
	{
		val |= 0x01;
	}

	return val;
}


static READ8_HANDLER( canyon_options_r )
{
	return (readinputport(0) >> (2 * (~offset & 3))) & 3;
}




/*************************************
 *
 *  Write handlers
 *
 *************************************/

static WRITE8_HANDLER( canyon_led_w )
{
	set_led_status(offset & 0x01, offset & 0x02);
}




/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(14) )
	AM_RANGE(0x0000, 0x00ff) AM_MIRROR(0x100) AM_RAM
	AM_RANGE(0x0400, 0x0401) AM_WRITE(canyon_motor_w)
	AM_RANGE(0x0500, 0x0500) AM_WRITE(canyon_explode_w)
	AM_RANGE(0x0501, 0x0501) AM_WRITE(watchdog_reset_w) /* watchdog, disabled in service mode */
	AM_RANGE(0x0600, 0x0603) AM_WRITE(canyon_whistle_w)
	AM_RANGE(0x0680, 0x0683) AM_WRITE(canyon_led_w)
	AM_RANGE(0x0700, 0x0703) AM_WRITE(canyon_attract_w)
	AM_RANGE(0x0800, 0x0bff) AM_READWRITE(MRA8_RAM, canyon_videoram_w) AM_BASE(&canyon_videoram)
	AM_RANGE(0x1000, 0x17ff) AM_READWRITE(canyon_switches_r, MWA8_NOP)  /* sloppy code writes here */
	AM_RANGE(0x1800, 0x1fff) AM_READ(canyon_options_r)
	AM_RANGE(0x2000, 0x3fff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( canyon )
	PORT_START      /* DSW */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Spanish ) )
	PORT_DIPSETTING(    0x02, DEF_STR( French ) )
	PORT_DIPSETTING(    0x03, DEF_STR( German ) )
	PORT_DIPNAME( 0x30, 0x00, "Misses Per Play" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x20, "5" )
	PORT_DIPSETTING(    0x30, "6" )
	PORT_DIPNAME( 0xC0, 0x80, DEF_STR( Coinage ))
	PORT_DIPSETTING(    0xC0, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

	PORT_START      /* IN1 */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)

	PORT_START      /* IN2 */
	PORT_BIT ( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT ( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_SERVICE( 0x10, IP_ACTIVE_HIGH )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_NAME("Hiscore Reset") PORT_CODE(KEYCODE_H)
	PORT_BIT ( 0x80, IP_ACTIVE_HIGH, IPT_TILT ) /* SLAM */

	PORT_START
	PORT_ADJUSTER( 20, "Motor 1 RPM" )

	PORT_START
	PORT_ADJUSTER( 30, "Motor 2 RPM" )

	PORT_START
	PORT_ADJUSTER( 70, "Whistle 1 Freq" )

	PORT_START
	PORT_ADJUSTER( 80, "Whistle 2 Freq" )
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout tile_layout =
{
	8, 8,
    64,
    1,
    { 0 },
    {
		0x4, 0x5, 0x6, 0x7, 0xC, 0xD, 0xE, 0xF
	},
	{
		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70
	},
	0x80
};


static const gfx_layout sprite_layout =
{
	32, 16,
	4,
	1,
	{ 0 },
	{
		0x007, 0x006, 0x005, 0x004, 0x003, 0x002, 0x001, 0x000,
		0x00F, 0x00E, 0x00D, 0x00C, 0x00B, 0x00A, 0x009, 0x008,
		0x107, 0x106, 0x105, 0x104, 0x103, 0x102, 0x101, 0x100,
		0x10F, 0x10E, 0x10D, 0x10C, 0x10B, 0x10A, 0x109, 0x108
	},
	{
		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
		0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0
	},
	0x200
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tile_layout,   0, 2 },
	{ REGION_GFX2, 0, &sprite_layout, 0, 2 },
	{ -1 }
};



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( canyon )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 12096000 / 16)
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse, 1)
	MDRV_WATCHDOG_VBLANK_INIT(8)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(22 * 1000000 / 15750)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 240)
	MDRV_VISIBLE_AREA(0, 255, 0, 239)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(3)
	MDRV_COLORTABLE_LENGTH(4)

	MDRV_PALETTE_INIT(canyon)
	MDRV_VIDEO_START(canyon)
	MDRV_VIDEO_UPDATE(canyon)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD_TAG("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(canyon_discrete_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( canyon )
	ROM_REGION( 0x4000, REGION_CPU1, 0 )
	ROM_LOAD_NIB_LOW ( "9499-01.j1", 0x3000, 0x0400, CRC(31800767) SHA1(d4aebe12d3c45a2a8a361dc6f63e1a6230a78c17) )
	ROM_LOAD_NIB_HIGH( "9503-01.p1", 0x3000, 0x0400, CRC(1eddbe28) SHA1(7d30280bf9edff743c16386d7cdec78094477996) )
	ROM_LOAD         ( "9496-01.d1", 0x3800, 0x0800, CRC(8be15080) SHA1(095c15e9ac91623b2d514858dca2e4c261d36fd0) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "9492-01.n8", 0x0000, 0x0400, CRC(7449f754) SHA1(a8ffc39e1a86c94487551f5026eedbbd066b12c9) )

	ROM_REGION( 0x0100, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "9506-01.m5", 0x0000, 0x0100, CRC(0d63396a) SHA1(147fae3b02a86310c8d022a7e7cfbf71ea511616) )
	ROM_LOAD_NIB_HIGH( "9505-01.n5", 0x0000, 0x0100, CRC(60507c07) SHA1(fcb76890cbaa37e02392bf8b97f7be9a6fe6a721) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "9491-01.j6", 0x0000, 0x0100, CRC(b8094b4c) SHA1(82dc6799a19984f3b204ee3aeeb007e55afc8be3) )	/* sync (not used) */
ROM_END


ROM_START( canyonp )
	ROM_REGION( 0x4000, REGION_CPU1, 0 )
	ROM_LOAD_NIB_LOW ( "cbp3000l.j1", 0x3000, 0x0800, CRC(49cf29a0) SHA1(b58f024f45f85e5c2a48a95c60e80fd1be60eaac) )
	ROM_LOAD_NIB_HIGH( "cbp3000m.p1", 0x3000, 0x0800, CRC(b4385c23) SHA1(b550dfe9182f2b29aedba160a0917ca78b82f0e7) )
	ROM_LOAD_NIB_LOW ( "cbp3800l.h1", 0x3800, 0x0800, CRC(c7ee4431) SHA1(7a0f4454a981c4e9ee27e273e9a8379458e660e5) )
	ROM_LOAD_NIB_HIGH( "cbp3800m.r1", 0x3800, 0x0800, CRC(94246a9a) SHA1(5ff8b69fb744a5f62d4cf291e8f25e3620b479e7) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "9492-01.n8", 0x0000, 0x0400, CRC(7449f754) SHA1(a8ffc39e1a86c94487551f5026eedbbd066b12c9) )

	ROM_REGION( 0x0100, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "9506-01.m5", 0x0000, 0x0100, CRC(0d63396a) SHA1(147fae3b02a86310c8d022a7e7cfbf71ea511616) )
	ROM_LOAD_NIB_HIGH( "9505-01.n5", 0x0000, 0x0100, CRC(60507c07) SHA1(fcb76890cbaa37e02392bf8b97f7be9a6fe6a721) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "9491-01.j6", 0x0000, 0x0100, CRC(b8094b4c) SHA1(82dc6799a19984f3b204ee3aeeb007e55afc8be3) )	/* sync (not used) */
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1977, canyon,  0,      canyon, canyon, 0, ROT0, "Atari", "Canyon Bomber", 0 ,2)
GAME( 1977, canyonp, canyon, canyon, canyon, 0, ROT0, "Atari", "Canyon Bomber (prototype)", 0 ,2)
