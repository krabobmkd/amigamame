/***************************************************************************

    Atari Subs hardware

    driver by Mike Balfour

    Games supported:
        * Subs

    Known issues:
        * none at this time

****************************************************************************

    If you have any questions about how this driver works, don't hesitate to
    ask.  - Mike Balfour (mab22@po.cwru.edu)

***************************************************************************/

#include "driver.h"
#include "subs.h"
#include "sound/discrete.h"


/*************************************
 *
 *  Palette generation
 *
 *************************************/

static unsigned short colortable_source[] =
{
	0x00, 0x01,		/* Right screen */
	0x02, 0x03		/* Left screen */
};

static PALETTE_INIT( subs )
{
	palette_set_color(0,0x00,0x00,0x00); /* BLACK - modified on video invert */
	palette_set_color(1,0xff,0xff,0xff); /* WHITE - modified on video invert */
	palette_set_color(2,0x00,0x00,0x00); /* BLACK - modified on video invert */
	palette_set_color(3,0xff,0xff,0xff); /* WHITE - modified on video invert*/
	memcpy(colortable,colortable_source,sizeof(colortable_source));
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0007) AM_READ(subs_control_r)
	AM_RANGE(0x0020, 0x0027) AM_READ(subs_coin_r)
	AM_RANGE(0x0060, 0x0063) AM_READ(subs_options_r)
	AM_RANGE(0x0000, 0x01ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x0800, 0x0bff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xf000, 0xffff) AM_READ(MRA8_ROM) /* A14/A15 unused, so mirror ROM */
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0000) AM_WRITE(subs_noise_reset_w)
	AM_RANGE(0x0020, 0x0020) AM_WRITE(subs_steer_reset_w)
//  AM_RANGE(0x0040, 0x0040) AM_WRITE(subs_timer_reset_w)
	AM_RANGE(0x0060, 0x0061) AM_WRITE(subs_lamp1_w)
	AM_RANGE(0x0062, 0x0063) AM_WRITE(subs_lamp2_w)
	AM_RANGE(0x0064, 0x0065) AM_WRITE(subs_sonar2_w)
	AM_RANGE(0x0066, 0x0067) AM_WRITE(subs_sonar1_w)
// Schematics show crash and explode reversed.  But this is proper.
	AM_RANGE(0x0068, 0x0069) AM_WRITE(subs_explode_w)
	AM_RANGE(0x006a, 0x006b) AM_WRITE(subs_crash_w)
	AM_RANGE(0x006c, 0x006d) AM_WRITE(subs_invert1_w)
	AM_RANGE(0x006e, 0x006f) AM_WRITE(subs_invert2_w)
	AM_RANGE(0x0090, 0x009f) AM_WRITE(spriteram_w) AM_BASE(&spriteram)
	AM_RANGE(0x0000, 0x07ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x0800, 0x0bff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2000, 0x3fff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( subs )
	PORT_START /* OPTIONS */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "Credit/Time" )
	PORT_DIPSETTING(    0x00, "Each Coin Buys Time" )
	PORT_DIPSETTING(    0x02, "Fixed Time" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x04, DEF_STR( French ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Spanish) )
	PORT_DIPSETTING(    0x0c, DEF_STR( German) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0xe0, 0x40, "Game Length" )
	PORT_DIPSETTING(    0x00, "0:30 Minutes" )
	PORT_DIPSETTING(    0x20, "1:00 Minutes" )
	PORT_DIPSETTING(    0x40, "1:30 Minutes" )
	PORT_DIPSETTING(    0x60, "2:00 Minutes" )
	PORT_DIPSETTING(    0x80, "2:30 Minutes" )
	PORT_DIPSETTING(    0xa0, "3:00 Minutes" )
	PORT_DIPSETTING(    0xc0, "3:30 Minutes" )
	PORT_DIPSETTING(    0xe0, "4:00 Minutes" )

	PORT_START /* IN1 */
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN ) /* Diag Step */
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN ) /* Diag Hold */
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_TILT )    /* Slam */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )  /* Spare */
	PORT_BIT ( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED ) /* Filled in with steering information */

	PORT_START /* IN2 */
	PORT_BIT ( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Self Test") PORT_CODE(KEYCODE_F2) PORT_TOGGLE
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 )

	PORT_START      /* IN3 */
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20) PORT_PLAYER(2)

	PORT_START      /* IN4 */
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout playfield_layout =
{
	8,8,
	256,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static const gfx_layout motion_layout =
{
	16,16,
	64,
	1,
	{ 0 },
	{ 3 + 0x400*8, 2 + 0x400*8, 1 + 0x400*8, 0 + 0x400*8,
	  7 + 0x400*8, 6 + 0x400*8, 5 + 0x400*8, 4 + 0x400*8,
	  3, 2, 1, 0, 7, 6, 5, 4 },
	{ 0, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
	  8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*8
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &playfield_layout, 0, 2 }, 	/* playfield graphics */
	{ REGION_GFX2, 0, &motion_layout,    0, 2 }, 	/* motion graphics */
	{ -1 }
};


/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( subs )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502,12096000/16)		/* clock input is the "4H" signal */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(subs_interrupt,4)

	MDRV_FRAMES_PER_SECOND(57)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(subs)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_ASPECT_RATIO(8,3)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 64*8-1, 0*8, 28*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4)
	MDRV_COLORTABLE_LENGTH(sizeof(colortable_source) / sizeof(colortable_source[0]))

	MDRV_PALETTE_INIT(subs)
	MDRV_VIDEO_START(generic)
	MDRV_VIDEO_UPDATE(subs)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD_TAG("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(subs_discrete_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( subs )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "34190.p1",     0x2800, 0x0800, CRC(a88aef21) SHA1(3811c137041ca43a6e49fbaf7d9d8ef37ba190a2) )
	ROM_LOAD( "34191.p2",     0x3000, 0x0800, CRC(2c652e72) SHA1(097b665e803cbc57b5a828403a8d9a258c19e97f) )
	ROM_LOAD( "34192.n2",     0x3800, 0x0800, CRC(3ce63d33) SHA1(a413cb3e0d03dc40a50f5b03b76a4edbe7906f3e) )
	ROM_RELOAD(               0xf800, 0x0800 )
	/* Note: These are being loaded into a bogus location, */
	/*       They are nibble wide rom images which will be */
	/*       merged and loaded into the proper place by    */
	/*       subs_rom_init()                               */
	ROM_LOAD( "34196.e2",     0x8000, 0x0100, CRC(7c7a04c3) SHA1(269d9f7573cc5da4412f53d647127c4884435353) )	/* ROM 0 D4-D7 */
	ROM_LOAD( "34194.e1",     0x9000, 0x0100, CRC(6b1c4acc) SHA1(3a743b721d9e7e9bdc4533aeeab294eb0ea27500) )	/* ROM 0 D0-D3 */

	ROM_REGION( 0x0800, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "34211.m4",     0x0000, 0x0800, CRC(fa8d4409) SHA1(a83b7a835212d31fe421d537fa0d78f234c26f5b) )	/* Playfield */

	ROM_REGION( 0x0800, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "34216.d7",     0x0000, 0x0200, CRC(941d28b4) SHA1(89388ec06546dc567aa5dbc6a7898974f2871ecc) )	/* Motion */
	ROM_LOAD( "34218.e7",     0x0200, 0x0200, CRC(f4f4d874) SHA1(d99ad9a74611f9851f6bfa6000ebd70e1a364f5d) )	/* Motion */
	ROM_LOAD( "34217.d8",     0x0400, 0x0200, CRC(a7a60da3) SHA1(34fc21cc1ca69d58d3907094dc0a3faaf6f461b3) )	/* Motion */
	ROM_LOAD( "34219.e8",     0x0600, 0x0200, CRC(99a5a49b) SHA1(2cb429f8de73c7d78dc83e47f1448ea4340c333d) )	/* Motion */
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( subs )
{
	unsigned char *rom = memory_region(REGION_CPU1);
	int i;

	/* Merge nibble-wide roms together,
       and load them into 0x2000-0x20ff */

	for(i=0;i<0x100;i++)
		rom[0x2000+i] = (rom[0x8000+i]<<4)+rom[0x9000+i];
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1977, subs, 0, subs, subs, subs, ROT0, "Atari", "Subs", GAME_IMPERFECT_SOUND ,2)
