/***************************************************************************

    Gun.Smoke
    Capcom

    driver by Paul Leaman

    Games supported:
        * Gun.Smoke (World)
        * Gun.Smoke (Japan)
        * Gun.Smoke (US set 1)
        * Gun.Smoke (US set 2)

****************************************************************************

  GUNSMOKE
  ========

  Driver provided by Paul Leaman


Stephh's notes (based on the games Z80 code and some tests) :

0) all games

  - There is some code that allows you to select your starting level
    (at 0x08dc in 'gunsmoka' and at 0x08d2 in the other sets).
    To do so, once the game has booted (after the "notice" screen),
    turn the "service" mode Dip Switch ON, and change Dip Switches
    DSW 1-0 to 1-3 (which are used by coinage).
  - About the ingame bug at the end of level 2 : enemy's energy
    (stored at 0xf790) is in fact not infinite, but it turns back to
    0xff, so when it reaches 0 again, the boss is dead.


1) 'gunsmoke'

  - World version.
    You can enter 3 chars for your initials.


2) 'gunsmokj'

  - Japan version (but English text though).
    You can enter 8 chars for your initials.


3) 'gunsmoku'

  - US version licenced to Romstar.
    You can enter 3 chars for your initials.


4) 'gunsmoka'

  - US version licenced to Romstar.
    You can enter 3 chars for your initials.
  - This is probably a later version of the game because some code
    has been added for the "Lives" Dip Switch that replaces the
    "Demonstation" one (so demonstration is always OFF).
  - Other changes :
      * Year is 1986 instead of 1985.
      * High score is 110000 instead of 100000.
      * Levels 3 and 6 are swapped.


***************************************************************************/

#include "driver.h"
#include "sound/2203intf.h"

extern UINT8 *gunsmoke_scrollx;
extern UINT8 *gunsmoke_scrolly;

extern WRITE8_HANDLER( gunsmoke_c804_w );
extern WRITE8_HANDLER( gunsmoke_d806_w );
extern WRITE8_HANDLER( gunsmoke_videoram_w );
extern WRITE8_HANDLER( gunsmoke_colorram_w );

extern PALETTE_INIT( gunsmoke );
extern VIDEO_START( gunsmoke );
extern VIDEO_UPDATE( gunsmoke );

/* Read/Write Handlers */

static READ8_HANDLER( gunsmoke_protection_r )
{
    /*
        The routine at 0x0e69 tries to read data starting at 0xc4c9.
        If this value is zero, it interprets the next two bytes as a
        jump address.

        This was resulting in a reboot which happens at the end of level 3
        if you go too far to the right of the screen when fighting the level boss.

        A non-zero for the first byte seems to be harmless
        (although it may not be the correct behaviour).

        This could be some devious protection or it could be a bug in the
        arcade game.  It's hard to tell without pulling the code apart.
    */

	static int gunsmoke_fixed_data[] = { 0xff, 0x00, 0x00 };

    return gunsmoke_fixed_data[offset];
}

/* Memory Maps */

static ADDRESS_MAP_START( gunsmoke_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xc000, 0xc000) AM_READ(input_port_0_r)
	AM_RANGE(0xc001, 0xc001) AM_READ(input_port_1_r)
	AM_RANGE(0xc002, 0xc002) AM_READ(input_port_2_r)
	AM_RANGE(0xc003, 0xc003) AM_READ(input_port_3_r)
	AM_RANGE(0xc004, 0xc004) AM_READ(input_port_4_r)
	AM_RANGE(0xc4c9, 0xc4cb) AM_READ(gunsmoke_protection_r)
	AM_RANGE(0xc800, 0xc800) AM_WRITE(soundlatch_w)
	AM_RANGE(0xc804, 0xc804) AM_WRITE(gunsmoke_c804_w)	// ROM bank switch, screen flip
	AM_RANGE(0xc806, 0xc806) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0xd000, 0xd3ff) AM_RAM AM_WRITE(gunsmoke_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xd400, 0xd7ff) AM_RAM AM_WRITE(gunsmoke_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xd800, 0xd801) AM_RAM AM_BASE(&gunsmoke_scrollx)
	AM_RANGE(0xd802, 0xd802) AM_RAM AM_BASE(&gunsmoke_scrolly)
	AM_RANGE(0xd806, 0xd806) AM_WRITE(gunsmoke_d806_w)	// sprites and bg enable
	AM_RANGE(0xe000, 0xefff) AM_RAM
	AM_RANGE(0xf000, 0xffff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
	AM_RANGE(0xc800, 0xc800) AM_READ(soundlatch_r)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(YM2203_control_port_0_w)
	AM_RANGE(0xe001, 0xe001) AM_WRITE(YM2203_write_port_0_w)
	AM_RANGE(0xe002, 0xe002) AM_WRITE(YM2203_control_port_1_w)
	AM_RANGE(0xe003, 0xe003) AM_WRITE(YM2203_write_port_1_w)
ADDRESS_MAP_END

/* Input Ports */

INPUT_PORTS_START( gunsmoke )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SPECIAL )	// VBLANK
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START_TAG("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START_TAG("DSW0")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x01, "30K 80K 80K+" )
	PORT_DIPSETTING(    0x03, "30K 100K 100K+" )
	PORT_DIPSETTING(    0x00, "30K 100K 150K+" )
	PORT_DIPSETTING(    0x02, "30K 100K")
	PORT_DIPNAME( 0x04, 0x04, "Demo" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, "Difficult" )
	PORT_DIPSETTING(    0x00, "Very Difficult" )
	PORT_DIPNAME( 0x40, 0x40, "Freeze" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )	// Also "debug mode"

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( gunsmoka )
	PORT_INCLUDE(gunsmoke)

	// Same as 'gunsmoke', but "Lives" Dip Switch instead of "Demonstration" Dip Switch
	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x00, "5" )
INPUT_PORTS_END

/* Graphics Layouts */

static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	1024,	/* 1024 characters */
	2,	/* 2 bits per pixel */
	{ 4, 0 },
	{ 8+3, 8+2, 8+1, 8+0, 3, 2, 1, 0 },
	{ 7*16, 6*16, 5*16, 4*16, 3*16, 2*16, 1*16, 0*16 },
	16*8	/* every char takes 16 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	32,32,  /* 32*32 tiles */
	512,    /* 512 tiles */
	4,      /* 4 bits per pixel */
	{ 512*256*8+4, 512*256*8+0, 4, 0 },
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			64*8+0, 64*8+1, 64*8+2, 64*8+3, 65*8+0, 65*8+1, 65*8+2, 65*8+3,
			128*8+0, 128*8+1, 128*8+2, 128*8+3, 129*8+0, 129*8+1, 129*8+2, 129*8+3,
			192*8+0, 192*8+1, 192*8+2, 192*8+3, 193*8+0, 193*8+1, 193*8+2, 193*8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16,
			16*16, 17*16, 18*16, 19*16, 20*16, 21*16, 22*16, 23*16,
			24*16, 25*16, 26*16, 27*16, 28*16, 29*16, 30*16, 31*16 },
	256*8	/* every tile takes 256 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,	/* 16*16 sprites */
	2048,	/* 2048 sprites */
	4,      /* 4 bits per pixel */
	{ 2048*64*8+4, 2048*64*8+0, 4, 0 },
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			32*8+0, 32*8+1, 32*8+2, 32*8+3, 33*8+0, 33*8+1, 33*8+2, 33*8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	64*8	/* every sprite takes 64 consecutive bytes */
};

/* Graphics Decode Info */

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,            0, 32 },
	{ REGION_GFX2, 0, &tilelayout,         32*4, 16 },
	{ REGION_GFX3, 0, &spritelayout, 32*4+16*16, 16 },
	{ -1 }
};

/* Machine Driver */

static MACHINE_DRIVER_START( gunsmoke )
	// basic machine hardware
	MDRV_CPU_ADD(Z80, 4000000)	// 4 MHz
	MDRV_CPU_PROGRAM_MAP(gunsmoke_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)

	MDRV_CPU_ADD(Z80, 3000000)	// 3 MHz
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 4)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	// video hardware
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)
	MDRV_COLORTABLE_LENGTH(32*4+16*16+16*16)

	MDRV_PALETTE_INIT(gunsmoke)
	MDRV_VIDEO_START(gunsmoke)
	MDRV_VIDEO_UPDATE(gunsmoke)

	// sound hardware
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.22)
	MDRV_SOUND_ROUTE(1, "mono", 0.22)
	MDRV_SOUND_ROUTE(2, "mono", 0.22)
	MDRV_SOUND_ROUTE(3, "mono", 0.14)

	MDRV_SOUND_ADD(YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.22)
	MDRV_SOUND_ROUTE(1, "mono", 0.22)
	MDRV_SOUND_ROUTE(2, "mono", 0.22)
	MDRV_SOUND_ROUTE(3, "mono", 0.14)
MACHINE_DRIVER_END

/* ROMs */

ROM_START( gunsmoke )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "09n_gs03.bin", 0x00000, 0x8000, CRC(40a06cef) SHA1(3e2a52d476298b7252f0adaefdb42090351e921c) ) /* Code 0000-7fff */
	ROM_LOAD( "10n_gs04.bin", 0x10000, 0x8000, CRC(8d4b423f) SHA1(149274c2ed1526ca1f419fdf8a24059ff138f7f2) ) /* Paged code */
	ROM_LOAD( "12n_gs05.bin", 0x18000, 0x8000, CRC(2b5667fb) SHA1(5b689bca1e76d803b4cae22feaa7744fa528e93f) ) /* Paged code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "14h_gs02.bin", 0x00000, 0x8000, CRC(cd7a2c38) SHA1(c76c471f694b76015370f0eacf5350e652f526ff) )

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "11f_gs01.bin", 0x00000, 0x4000, CRC(b61ece9b) SHA1(eb3fc62644cc5b5a2b9cbe67c393d4a0e2a59ca9) ) /* Characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "06c_gs13.bin", 0x00000, 0x8000, CRC(f6769fc5) SHA1(d192ec176425327ca4b7e25fc8432fc47837ba29) ) /* 32x32 tiles planes 2-3 */
	ROM_LOAD( "05c_gs12.bin", 0x08000, 0x8000, CRC(d997b78c) SHA1(3b4a9b6f9e57ecfb4ab9734379bd0ee765fd6daa) )
	ROM_LOAD( "04c_gs11.bin", 0x10000, 0x8000, CRC(125ba58e) SHA1(cf6931653cebd051564bed8121ab8713a55095c5) )
	ROM_LOAD( "02c_gs10.bin", 0x18000, 0x8000, CRC(f469c13c) SHA1(54eda52d6fce58771c0adfe2c88292a41d5a9b99) )
	ROM_LOAD( "06a_gs09.bin", 0x20000, 0x8000, CRC(539f182d) SHA1(4190c0adbecc57b92f4d002e121acb77e8c5d8d8) ) /* 32x32 tiles planes 0-1 */
	ROM_LOAD( "05a_gs08.bin", 0x28000, 0x8000, CRC(e87e526d) SHA1(d10068addf30322424a85bbc6382cb762ae3fbe2) )
	ROM_LOAD( "04a_gs07.bin", 0x30000, 0x8000, CRC(4382c0d2) SHA1(8615e62bc57b40d082f6ca211d64f22185bed1fd) )
	ROM_LOAD( "02a_gs06.bin", 0x38000, 0x8000, CRC(4cafe7a6) SHA1(fe501f3a5e9ce9e82e9708f1cd297f4c94ef0f81) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "06n_gs22.bin", 0x00000, 0x8000, CRC(dc9c508c) SHA1(920505dd4c63b177918feb4e54cca8a7948ec9d9) ) /* Sprites planes 2-3 */
	ROM_LOAD( "04n_gs21.bin", 0x08000, 0x8000, CRC(68883749) SHA1(c7bf2bf49c53feddf8f30b4001dc2d59b52b1c28) ) /* Sprites planes 2-3 */
	ROM_LOAD( "03n_gs20.bin", 0x10000, 0x8000, CRC(0be932ed) SHA1(1c5af5884a23112dbc36579515d1cb497992da2f) ) /* Sprites planes 2-3 */
	ROM_LOAD( "01n_gs19.bin", 0x18000, 0x8000, CRC(63072f93) SHA1(cb3a2729782cf2855558d081fe92d28366228b8e) ) /* Sprites planes 2-3 */
	ROM_LOAD( "06l_gs18.bin", 0x20000, 0x8000, CRC(f69a3c7c) SHA1(e9eb9dfa7d53aa7b728150f91d05bfc3bf6f1e75) ) /* Sprites planes 0-1 */
	ROM_LOAD( "04l_gs17.bin", 0x28000, 0x8000, CRC(4e98562a) SHA1(0341b8a79be1d71a57d0d76ed890e15f9f92259e) ) /* Sprites planes 0-1 */
	ROM_LOAD( "03l_gs16.bin", 0x30000, 0x8000, CRC(0d99c3b3) SHA1(436c566b76f632242448671e3b6319f7d9f65322) ) /* Sprites planes 0-1 */
	ROM_LOAD( "01l_gs15.bin", 0x38000, 0x8000, CRC(7f14270e) SHA1(dd06c333c2ea097e25185a1423cd61e1b7afc42b) ) /* Sprites planes 0-1 */

	ROM_REGION( 0x8000, REGION_GFX4, 0 )	/* background tilemaps */
	ROM_LOAD( "11c_gs14.bin", 0x00000, 0x8000, CRC(0af4f7eb) SHA1(24a98fdeedeeaf1035b4af52d5a8dd5e47a5e62d) )

	ROM_REGION( 0x0a00, REGION_PROMS, 0 )
	ROM_LOAD( "03b_g-01.bin", 0x0000, 0x0100, CRC(02f55589) SHA1(8a3f98304aedf3aba1c08b615bf457752a480edc) )	/* red component */
	ROM_LOAD( "04b_g-02.bin", 0x0100, 0x0100, CRC(e1e36dd9) SHA1(5bd88a35898a2d973045bdde8311aac3a12826de) )	/* green component */
	ROM_LOAD( "05b_g-03.bin", 0x0200, 0x0100, CRC(989399c0) SHA1(e408e391f49ed0c7b9e16479fea44b809440fefc) )	/* blue component */
	ROM_LOAD( "09d_g-04.bin", 0x0300, 0x0100, CRC(906612b5) SHA1(7b727a6200c088538180758320ede84aa7e5b96d) )	/* char lookup table */
	ROM_LOAD( "14a_g-06.bin", 0x0400, 0x0100, CRC(4a9da18b) SHA1(fed3b81b56aab2ed0a21ed1fcebe3f1ae095a13b) )	/* tile lookup table */
	ROM_LOAD( "15a_g-07.bin", 0x0500, 0x0100, CRC(cb9394fc) SHA1(8ad0fde6a8ef8326d2da4b6dbf3b51f5f6c668c8) )	/* tile palette bank */
	ROM_LOAD( "09f_g-09.bin", 0x0600, 0x0100, CRC(3cee181e) SHA1(3f95bdb12391cb9b3673191bda8d09c84b36b4d3) )	/* sprite lookup table */
	ROM_LOAD( "08f_g-08.bin", 0x0700, 0x0100, CRC(ef91cdd2) SHA1(90b9191c9f10a153d64055a4238eb6e15b8c12bc) )	/* sprite palette bank */
	ROM_LOAD( "02j_g-10.bin", 0x0800, 0x0100, CRC(0eaf5158) SHA1(bafd4108708f66cd7b280e47152b108f3e254fc9) )	/* video timing (not used) */
	ROM_LOAD( "01f_g-05.bin", 0x0900, 0x0100, CRC(25c90c2a) SHA1(42893572bab757ec01e181fc418cb911638d37e0) )	/* priority? (not used) */
ROM_END

ROM_START( gunsmokj )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "gs03_9n.rom",  0x00000, 0x8000, CRC(b56b5df6) SHA1(0295a3ef491b6b8ee9c198fd08dddc29d88bbef6) ) /* Code 0000-7fff */
	ROM_LOAD( "10n_gs04.bin", 0x10000, 0x8000, CRC(8d4b423f) SHA1(149274c2ed1526ca1f419fdf8a24059ff138f7f2) ) /* Paged code */
	ROM_LOAD( "12n_gs05.bin", 0x18000, 0x8000, CRC(2b5667fb) SHA1(5b689bca1e76d803b4cae22feaa7744fa528e93f) ) /* Paged code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "14h_gs02.bin", 0x00000, 0x8000, CRC(cd7a2c38) SHA1(c76c471f694b76015370f0eacf5350e652f526ff) )

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "11f_gs01.bin", 0x00000, 0x4000, CRC(b61ece9b) SHA1(eb3fc62644cc5b5a2b9cbe67c393d4a0e2a59ca9) ) /* Characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "06c_gs13.bin", 0x00000, 0x8000, CRC(f6769fc5) SHA1(d192ec176425327ca4b7e25fc8432fc47837ba29) ) /* 32x32 tiles planes 2-3 */
	ROM_LOAD( "05c_gs12.bin", 0x08000, 0x8000, CRC(d997b78c) SHA1(3b4a9b6f9e57ecfb4ab9734379bd0ee765fd6daa) )
	ROM_LOAD( "04c_gs11.bin", 0x10000, 0x8000, CRC(125ba58e) SHA1(cf6931653cebd051564bed8121ab8713a55095c5) )
	ROM_LOAD( "02c_gs10.bin", 0x18000, 0x8000, CRC(f469c13c) SHA1(54eda52d6fce58771c0adfe2c88292a41d5a9b99) )
	ROM_LOAD( "06a_gs09.bin", 0x20000, 0x8000, CRC(539f182d) SHA1(4190c0adbecc57b92f4d002e121acb77e8c5d8d8) ) /* 32x32 tiles planes 0-1 */
	ROM_LOAD( "05a_gs08.bin", 0x28000, 0x8000, CRC(e87e526d) SHA1(d10068addf30322424a85bbc6382cb762ae3fbe2) )
	ROM_LOAD( "04a_gs07.bin", 0x30000, 0x8000, CRC(4382c0d2) SHA1(8615e62bc57b40d082f6ca211d64f22185bed1fd) )
	ROM_LOAD( "02a_gs06.bin", 0x38000, 0x8000, CRC(4cafe7a6) SHA1(fe501f3a5e9ce9e82e9708f1cd297f4c94ef0f81) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "06n_gs22.bin", 0x00000, 0x8000, CRC(dc9c508c) SHA1(920505dd4c63b177918feb4e54cca8a7948ec9d9) ) /* Sprites planes 2-3 */
	ROM_LOAD( "04n_gs21.bin", 0x08000, 0x8000, CRC(68883749) SHA1(c7bf2bf49c53feddf8f30b4001dc2d59b52b1c28) ) /* Sprites planes 2-3 */
	ROM_LOAD( "03n_gs20.bin", 0x10000, 0x8000, CRC(0be932ed) SHA1(1c5af5884a23112dbc36579515d1cb497992da2f) ) /* Sprites planes 2-3 */
	ROM_LOAD( "01n_gs19.bin", 0x18000, 0x8000, CRC(63072f93) SHA1(cb3a2729782cf2855558d081fe92d28366228b8e) ) /* Sprites planes 2-3 */
	ROM_LOAD( "06l_gs18.bin", 0x20000, 0x8000, CRC(f69a3c7c) SHA1(e9eb9dfa7d53aa7b728150f91d05bfc3bf6f1e75) ) /* Sprites planes 0-1 */
	ROM_LOAD( "04l_gs17.bin", 0x28000, 0x8000, CRC(4e98562a) SHA1(0341b8a79be1d71a57d0d76ed890e15f9f92259e) ) /* Sprites planes 0-1 */
	ROM_LOAD( "03l_gs16.bin", 0x30000, 0x8000, CRC(0d99c3b3) SHA1(436c566b76f632242448671e3b6319f7d9f65322) ) /* Sprites planes 0-1 */
	ROM_LOAD( "01l_gs15.bin", 0x38000, 0x8000, CRC(7f14270e) SHA1(dd06c333c2ea097e25185a1423cd61e1b7afc42b) ) /* Sprites planes 0-1 */

	ROM_REGION( 0x8000, REGION_GFX4, 0 )	/* background tilemaps */
	ROM_LOAD( "11c_gs14.bin", 0x00000, 0x8000, CRC(0af4f7eb) SHA1(24a98fdeedeeaf1035b4af52d5a8dd5e47a5e62d) )

	ROM_REGION( 0x0a00, REGION_PROMS, 0 )
	ROM_LOAD( "03b_g-01.bin", 0x0000, 0x0100, CRC(02f55589) SHA1(8a3f98304aedf3aba1c08b615bf457752a480edc) )	/* red component */
	ROM_LOAD( "04b_g-02.bin", 0x0100, 0x0100, CRC(e1e36dd9) SHA1(5bd88a35898a2d973045bdde8311aac3a12826de) )	/* green component */
	ROM_LOAD( "05b_g-03.bin", 0x0200, 0x0100, CRC(989399c0) SHA1(e408e391f49ed0c7b9e16479fea44b809440fefc) )	/* blue component */
	ROM_LOAD( "09d_g-04.bin", 0x0300, 0x0100, CRC(906612b5) SHA1(7b727a6200c088538180758320ede84aa7e5b96d) )	/* char lookup table */
	ROM_LOAD( "14a_g-06.bin", 0x0400, 0x0100, CRC(4a9da18b) SHA1(fed3b81b56aab2ed0a21ed1fcebe3f1ae095a13b) )	/* tile lookup table */
	ROM_LOAD( "15a_g-07.bin", 0x0500, 0x0100, CRC(cb9394fc) SHA1(8ad0fde6a8ef8326d2da4b6dbf3b51f5f6c668c8) )	/* tile palette bank */
	ROM_LOAD( "09f_g-09.bin", 0x0600, 0x0100, CRC(3cee181e) SHA1(3f95bdb12391cb9b3673191bda8d09c84b36b4d3) )	/* sprite lookup table */
	ROM_LOAD( "08f_g-08.bin", 0x0700, 0x0100, CRC(ef91cdd2) SHA1(90b9191c9f10a153d64055a4238eb6e15b8c12bc) )	/* sprite palette bank */
	ROM_LOAD( "02j_g-10.bin", 0x0800, 0x0100, CRC(0eaf5158) SHA1(bafd4108708f66cd7b280e47152b108f3e254fc9) )	/* video timing (not used) */
	ROM_LOAD( "01f_g-05.bin", 0x0900, 0x0100, CRC(25c90c2a) SHA1(42893572bab757ec01e181fc418cb911638d37e0) )	/* priority? (not used) */
ROM_END

ROM_START( gunsmoku )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "9n_gs03.bin",  0x00000, 0x8000, CRC(592f211b) SHA1(8de44b3cafa3d2ce9aba515cf3ec4bac0bcdeb5b) ) /* Code 0000-7fff */
	ROM_LOAD( "10n_gs04.bin", 0x10000, 0x8000, CRC(8d4b423f) SHA1(149274c2ed1526ca1f419fdf8a24059ff138f7f2) ) /* Paged code */
	ROM_LOAD( "12n_gs05.bin", 0x18000, 0x8000, CRC(2b5667fb) SHA1(5b689bca1e76d803b4cae22feaa7744fa528e93f) ) /* Paged code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "14h_gs02.bin", 0x00000, 0x8000, CRC(cd7a2c38) SHA1(c76c471f694b76015370f0eacf5350e652f526ff) )

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "11f_gs01.bin", 0x00000, 0x4000, CRC(b61ece9b) SHA1(eb3fc62644cc5b5a2b9cbe67c393d4a0e2a59ca9) ) /* Characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "06c_gs13.bin", 0x00000, 0x8000, CRC(f6769fc5) SHA1(d192ec176425327ca4b7e25fc8432fc47837ba29) ) /* 32x32 tiles planes 2-3 */
	ROM_LOAD( "05c_gs12.bin", 0x08000, 0x8000, CRC(d997b78c) SHA1(3b4a9b6f9e57ecfb4ab9734379bd0ee765fd6daa) )
	ROM_LOAD( "04c_gs11.bin", 0x10000, 0x8000, CRC(125ba58e) SHA1(cf6931653cebd051564bed8121ab8713a55095c5) )
	ROM_LOAD( "02c_gs10.bin", 0x18000, 0x8000, CRC(f469c13c) SHA1(54eda52d6fce58771c0adfe2c88292a41d5a9b99) )
	ROM_LOAD( "06a_gs09.bin", 0x20000, 0x8000, CRC(539f182d) SHA1(4190c0adbecc57b92f4d002e121acb77e8c5d8d8) ) /* 32x32 tiles planes 0-1 */
	ROM_LOAD( "05a_gs08.bin", 0x28000, 0x8000, CRC(e87e526d) SHA1(d10068addf30322424a85bbc6382cb762ae3fbe2) )
	ROM_LOAD( "04a_gs07.bin", 0x30000, 0x8000, CRC(4382c0d2) SHA1(8615e62bc57b40d082f6ca211d64f22185bed1fd) )
	ROM_LOAD( "02a_gs06.bin", 0x38000, 0x8000, CRC(4cafe7a6) SHA1(fe501f3a5e9ce9e82e9708f1cd297f4c94ef0f81) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "06n_gs22.bin", 0x00000, 0x8000, CRC(dc9c508c) SHA1(920505dd4c63b177918feb4e54cca8a7948ec9d9) ) /* Sprites planes 2-3 */
	ROM_LOAD( "04n_gs21.bin", 0x08000, 0x8000, CRC(68883749) SHA1(c7bf2bf49c53feddf8f30b4001dc2d59b52b1c28) ) /* Sprites planes 2-3 */
	ROM_LOAD( "03n_gs20.bin", 0x10000, 0x8000, CRC(0be932ed) SHA1(1c5af5884a23112dbc36579515d1cb497992da2f) ) /* Sprites planes 2-3 */
	ROM_LOAD( "01n_gs19.bin", 0x18000, 0x8000, CRC(63072f93) SHA1(cb3a2729782cf2855558d081fe92d28366228b8e) ) /* Sprites planes 2-3 */
	ROM_LOAD( "06l_gs18.bin", 0x20000, 0x8000, CRC(f69a3c7c) SHA1(e9eb9dfa7d53aa7b728150f91d05bfc3bf6f1e75) ) /* Sprites planes 0-1 */
	ROM_LOAD( "04l_gs17.bin", 0x28000, 0x8000, CRC(4e98562a) SHA1(0341b8a79be1d71a57d0d76ed890e15f9f92259e) ) /* Sprites planes 0-1 */
	ROM_LOAD( "03l_gs16.bin", 0x30000, 0x8000, CRC(0d99c3b3) SHA1(436c566b76f632242448671e3b6319f7d9f65322) ) /* Sprites planes 0-1 */
	ROM_LOAD( "01l_gs15.bin", 0x38000, 0x8000, CRC(7f14270e) SHA1(dd06c333c2ea097e25185a1423cd61e1b7afc42b) ) /* Sprites planes 0-1 */

	ROM_REGION( 0x8000, REGION_GFX4, 0 )	/* background tilemaps */
	ROM_LOAD( "11c_gs14.bin", 0x00000, 0x8000, CRC(0af4f7eb) SHA1(24a98fdeedeeaf1035b4af52d5a8dd5e47a5e62d) )

	ROM_REGION( 0x0a00, REGION_PROMS, 0 )
	ROM_LOAD( "03b_g-01.bin", 0x0000, 0x0100, CRC(02f55589) SHA1(8a3f98304aedf3aba1c08b615bf457752a480edc) )	/* red component */
	ROM_LOAD( "04b_g-02.bin", 0x0100, 0x0100, CRC(e1e36dd9) SHA1(5bd88a35898a2d973045bdde8311aac3a12826de) )	/* green component */
	ROM_LOAD( "05b_g-03.bin", 0x0200, 0x0100, CRC(989399c0) SHA1(e408e391f49ed0c7b9e16479fea44b809440fefc) )	/* blue component */
	ROM_LOAD( "09d_g-04.bin", 0x0300, 0x0100, CRC(906612b5) SHA1(7b727a6200c088538180758320ede84aa7e5b96d) )	/* char lookup table */
	ROM_LOAD( "14a_g-06.bin", 0x0400, 0x0100, CRC(4a9da18b) SHA1(fed3b81b56aab2ed0a21ed1fcebe3f1ae095a13b) )	/* tile lookup table */
	ROM_LOAD( "15a_g-07.bin", 0x0500, 0x0100, CRC(cb9394fc) SHA1(8ad0fde6a8ef8326d2da4b6dbf3b51f5f6c668c8) )	/* tile palette bank */
	ROM_LOAD( "09f_g-09.bin", 0x0600, 0x0100, CRC(3cee181e) SHA1(3f95bdb12391cb9b3673191bda8d09c84b36b4d3) )	/* sprite lookup table */
	ROM_LOAD( "08f_g-08.bin", 0x0700, 0x0100, CRC(ef91cdd2) SHA1(90b9191c9f10a153d64055a4238eb6e15b8c12bc) )	/* sprite palette bank */
	ROM_LOAD( "02j_g-10.bin", 0x0800, 0x0100, CRC(0eaf5158) SHA1(bafd4108708f66cd7b280e47152b108f3e254fc9) )	/* video timing (not used) */
	ROM_LOAD( "01f_g-05.bin", 0x0900, 0x0100, CRC(25c90c2a) SHA1(42893572bab757ec01e181fc418cb911638d37e0) )	/* priority? (not used) */
ROM_END

ROM_START( gunsmoka )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "gs03.9n",      0x00000, 0x8000, CRC(51dc3f76) SHA1(2a188fee73c3662b665b56a825eb908b7b42dcd0) ) /* Code 0000-7fff */
	ROM_LOAD( "gs04.10n",     0x10000, 0x8000, CRC(5ecf31b8) SHA1(34ec9727330821a45b497c78c970a1a4f14ff4ee) ) /* Paged code */
	ROM_LOAD( "gs05.12n",     0x18000, 0x8000, CRC(1c9aca13) SHA1(eb92c373d2241aea4c59248e1b82717733105ac0) ) /* Paged code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "14h_gs02.bin", 0x00000, 0x8000, CRC(cd7a2c38) SHA1(c76c471f694b76015370f0eacf5350e652f526ff) )

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "11f_gs01.bin", 0x00000, 0x4000, CRC(b61ece9b) SHA1(eb3fc62644cc5b5a2b9cbe67c393d4a0e2a59ca9) ) /* Characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "06c_gs13.bin", 0x00000, 0x8000, CRC(f6769fc5) SHA1(d192ec176425327ca4b7e25fc8432fc47837ba29) ) /* 32x32 tiles planes 2-3 */
	ROM_LOAD( "05c_gs12.bin", 0x08000, 0x8000, CRC(d997b78c) SHA1(3b4a9b6f9e57ecfb4ab9734379bd0ee765fd6daa) )
	ROM_LOAD( "04c_gs11.bin", 0x10000, 0x8000, CRC(125ba58e) SHA1(cf6931653cebd051564bed8121ab8713a55095c5) )
	ROM_LOAD( "02c_gs10.bin", 0x18000, 0x8000, CRC(f469c13c) SHA1(54eda52d6fce58771c0adfe2c88292a41d5a9b99) )
	ROM_LOAD( "06a_gs09.bin", 0x20000, 0x8000, CRC(539f182d) SHA1(4190c0adbecc57b92f4d002e121acb77e8c5d8d8) ) /* 32x32 tiles planes 0-1 */
	ROM_LOAD( "05a_gs08.bin", 0x28000, 0x8000, CRC(e87e526d) SHA1(d10068addf30322424a85bbc6382cb762ae3fbe2) )
	ROM_LOAD( "04a_gs07.bin", 0x30000, 0x8000, CRC(4382c0d2) SHA1(8615e62bc57b40d082f6ca211d64f22185bed1fd) )
	ROM_LOAD( "02a_gs06.bin", 0x38000, 0x8000, CRC(4cafe7a6) SHA1(fe501f3a5e9ce9e82e9708f1cd297f4c94ef0f81) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "06n_gs22.bin", 0x00000, 0x8000, CRC(dc9c508c) SHA1(920505dd4c63b177918feb4e54cca8a7948ec9d9) ) /* Sprites planes 2-3 */
	ROM_LOAD( "04n_gs21.bin", 0x08000, 0x8000, CRC(68883749) SHA1(c7bf2bf49c53feddf8f30b4001dc2d59b52b1c28) ) /* Sprites planes 2-3 */
	ROM_LOAD( "03n_gs20.bin", 0x10000, 0x8000, CRC(0be932ed) SHA1(1c5af5884a23112dbc36579515d1cb497992da2f) ) /* Sprites planes 2-3 */
	ROM_LOAD( "01n_gs19.bin", 0x18000, 0x8000, CRC(63072f93) SHA1(cb3a2729782cf2855558d081fe92d28366228b8e) ) /* Sprites planes 2-3 */
	ROM_LOAD( "06l_gs18.bin", 0x20000, 0x8000, CRC(f69a3c7c) SHA1(e9eb9dfa7d53aa7b728150f91d05bfc3bf6f1e75) ) /* Sprites planes 0-1 */
	ROM_LOAD( "04l_gs17.bin", 0x28000, 0x8000, CRC(4e98562a) SHA1(0341b8a79be1d71a57d0d76ed890e15f9f92259e) ) /* Sprites planes 0-1 */
	ROM_LOAD( "03l_gs16.bin", 0x30000, 0x8000, CRC(0d99c3b3) SHA1(436c566b76f632242448671e3b6319f7d9f65322) ) /* Sprites planes 0-1 */
	ROM_LOAD( "01l_gs15.bin", 0x38000, 0x8000, CRC(7f14270e) SHA1(dd06c333c2ea097e25185a1423cd61e1b7afc42b) ) /* Sprites planes 0-1 */

	ROM_REGION( 0x8000, REGION_GFX4, 0 )	/* background tilemaps */
	ROM_LOAD( "11c_gs14.bin", 0x00000, 0x8000, CRC(0af4f7eb) SHA1(24a98fdeedeeaf1035b4af52d5a8dd5e47a5e62d) )

	ROM_REGION( 0x0a00, REGION_PROMS, 0 )
	ROM_LOAD( "03b_g-01.bin", 0x0000, 0x0100, CRC(02f55589) SHA1(8a3f98304aedf3aba1c08b615bf457752a480edc) )	/* red component */
	ROM_LOAD( "04b_g-02.bin", 0x0100, 0x0100, CRC(e1e36dd9) SHA1(5bd88a35898a2d973045bdde8311aac3a12826de) )	/* green component */
	ROM_LOAD( "05b_g-03.bin", 0x0200, 0x0100, CRC(989399c0) SHA1(e408e391f49ed0c7b9e16479fea44b809440fefc) )	/* blue component */
	ROM_LOAD( "09d_g-04.bin", 0x0300, 0x0100, CRC(906612b5) SHA1(7b727a6200c088538180758320ede84aa7e5b96d) )	/* char lookup table */
	ROM_LOAD( "14a_g-06.bin", 0x0400, 0x0100, CRC(4a9da18b) SHA1(fed3b81b56aab2ed0a21ed1fcebe3f1ae095a13b) )	/* tile lookup table */
	ROM_LOAD( "15a_g-07.bin", 0x0500, 0x0100, CRC(cb9394fc) SHA1(8ad0fde6a8ef8326d2da4b6dbf3b51f5f6c668c8) )	/* tile palette bank */
	ROM_LOAD( "09f_g-09.bin", 0x0600, 0x0100, CRC(3cee181e) SHA1(3f95bdb12391cb9b3673191bda8d09c84b36b4d3) )	/* sprite lookup table */
	ROM_LOAD( "08f_g-08.bin", 0x0700, 0x0100, CRC(ef91cdd2) SHA1(90b9191c9f10a153d64055a4238eb6e15b8c12bc) )	/* sprite palette bank */
	ROM_LOAD( "02j_g-10.bin", 0x0800, 0x0100, CRC(0eaf5158) SHA1(bafd4108708f66cd7b280e47152b108f3e254fc9) )	/* video timing (not used) */
	ROM_LOAD( "01f_g-05.bin", 0x0900, 0x0100, CRC(25c90c2a) SHA1(42893572bab757ec01e181fc418cb911638d37e0) )	/* priority? (not used) */
ROM_END

/* Game Drivers */

GAME( 1985, gunsmoke, 0,        gunsmoke, gunsmoke, 0, ROT270, "Capcom", "Gun.Smoke (World)", 0 ,2)
GAME( 1985, gunsmokj, gunsmoke, gunsmoke, gunsmoke, 0, ROT270, "Capcom", "Gun.Smoke (Japan)", 0 ,2)
GAME( 1985, gunsmoku, gunsmoke, gunsmoke, gunsmoke, 0, ROT270, "Capcom (Romstar license)", "Gun.Smoke (US set 1)", 0 ,2)
GAME( 1986, gunsmoka, gunsmoke, gunsmoke, gunsmoka, 0, ROT270, "Capcom (Romstar license)", "Gun.Smoke (US set 2)", 0 ,2)
