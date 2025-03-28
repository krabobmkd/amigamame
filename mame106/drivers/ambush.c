/***************************************************************************

Ambush Memory Map (preliminary)

driver by Zsolt Vasvari


Memory Mapped:

0000-7fff   R   ROM
8000-87ff   R/W RAM
a000        R   Watchdog Reset
c080-c09f   W   Scroll RAM (1 byte for each column)
c100-c1ff   W   Color RAM (1 line corresponds to 4 in the video ram)
c200-c3ff   W   Sprite RAM
c400-c7ff   W   Video RAM
c800        R   DIP Switches
cc00-cc03   W   ??? (Maybe analog sound triggers?)
cc04        W   Flip Screen
cc05        W   Color Bank Select
cc07        W   Coin Counter


I/O Ports:

00-01       R/W AY8910 #0 (Port A = Input Port #0)
80-81       R/W AY8910 #1 (Port A = Input Port #1)


TODO:

- Verify Z80 and AY8910 clock speeds

***************************************************************************/

#include "driver.h"
#include "sound/ay8910.h"


extern unsigned char *ambush_scrollram;
extern unsigned char *ambush_colorbank;

PALETTE_INIT( ambush );
VIDEO_UPDATE( ambush );


static WRITE8_HANDLER( ambush_coin_counter_w )
{
	coin_counter_w(0, data & 0x01);
	coin_counter_w(1, data & 0x02);
}

static WRITE8_HANDLER( flip_screen_w )
{
	flip_screen_set(data);
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xa000, 0xa000) AM_READ(watchdog_reset_r)
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xc800, 0xc800) AM_READ(input_port_2_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xc080, 0xc09f) AM_WRITE(MWA8_RAM) AM_BASE(&ambush_scrollram)
	AM_RANGE(0xc100, 0xc1ff) AM_WRITE(MWA8_RAM) AM_BASE(&colorram)
	AM_RANGE(0xc200, 0xc3ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xc400, 0xc7ff) AM_WRITE(MWA8_RAM) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0xcc00, 0xcc03) AM_WRITE(MWA8_NOP)
	AM_RANGE(0xcc04, 0xcc04) AM_WRITE(flip_screen_w)
	AM_RANGE(0xcc05, 0xcc05) AM_WRITE(MWA8_RAM) AM_BASE(&ambush_colorbank)
	AM_RANGE(0xcc07, 0xcc07) AM_WRITE(ambush_coin_counter_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(AY8910_read_port_0_r)
	AM_RANGE(0x80, 0x80) AM_READ(AY8910_read_port_1_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x80, 0x80) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x81, 0x81) AM_WRITE(AY8910_write_port_1_w)
ADDRESS_MAP_END


INPUT_PORTS_START( ambush )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x1c, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x1c, "Service Mode/Free Play" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x40, "80000" )
	PORT_DIPSETTING(    0x00, "120000" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8,8,    /* 8*8 chars */
	1024,   /* 2048 characters */
	2,      /* 2 bits per pixel */
	{ 0, 0x2000*8 },  /* The bitplanes are seperate */
	{ 0, 1, 2, 3, 4, 5, 6, 7},
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8},
	8*8     /* every char takes 8 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,  /* 8*8 chars */
	256,    /* 2048 characters */
	2,      /* 2 bits per pixel */
	{ 0, 0x2000*8 },  /* The bitplanes are seperate */
	{     0,     1,     2,     3,     4,     5,     6,     7,
	  8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{  0*8,  1*8,  2*8,  3*8,  4*8,  5*8,  6*8,  7*8,
	  16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8     /* every char takes 32 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,   0, 64 },
	{ REGION_GFX1, 0x0000, &spritelayout, 0, 64 },
	{ -1 } /* end of array */
};


static struct AY8910interface ay8910_interface_1 =
{
	input_port_0_r
};

static struct AY8910interface ay8910_interface_2 =
{
	input_port_1_r
};


static MACHINE_DRIVER_START( ambush )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 4000000)        /* 4.00 MHz??? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-3)  /* The -3 makes the cocktail mode perfect */
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(ambush)
	MDRV_VIDEO_START(generic_bitmapped)
	MDRV_VIDEO_UPDATE(ambush)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8910_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8910_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( ambush )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )       /* 64k for code */
	ROM_LOAD( "ambush.h7",    0x0000, 0x2000, CRC(ce306563) SHA1(c69b5c4465187a8eda6367d6cd3e0b71a57588d1) )
	ROM_LOAD( "ambush.g7",    0x2000, 0x2000, CRC(90291409) SHA1(82f1e109bd066ad9fdea1ce0086be6c334e2658a) )
	ROM_LOAD( "ambush.f7",    0x4000, 0x2000, CRC(d023ca29) SHA1(1ac44960cf6d79936517a9ad4bae6ccd825c9496) )
	ROM_LOAD( "ambush.e7",    0x6000, 0x2000, CRC(6cc2d3ee) SHA1(dccb417d156460ca745d7b62f1df733cbf85d092) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ambush.n4",    0x0000, 0x2000, CRC(ecc0dc85) SHA1(577304bb575293b97b50eea4faafb5394e3da0f5) )
	ROM_LOAD( "ambush.m4",    0x2000, 0x2000, CRC(e86ca98a) SHA1(fae0786bb78ead81653adddd2edb3058371ca5bc) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )
	ROM_LOAD( "a.bpr",        0x0000, 0x0100, CRC(5f27f511) SHA1(fe3ae701443ff50d3d03c0a5d0e0ab0e716b05cc) )  /* color PROMs */

	ROM_LOAD( "b.bpr",        0x0100, 0x0100, CRC(1b03fd3b) SHA1(1a58b212476cacace6065056f23b59a69053a2f6) )	/* How is this selected, */
	ROM_LOAD( "13.bpr",		  0x0200, 0x0100, CRC(547e970f) SHA1(e2ec0bece49fb283e43549703d6d5d6f561c69a6) )  /* I'm not sure what these do. */
	ROM_LOAD( "14.bpr",		  0x0300, 0x0100, CRC(622a8ce7) SHA1(6834f67874251f2ef3a33aec893311f5d10e496f) )  /* They don't look like color PROMs */
ROM_END

/* displays an M next to ROM 1 during the test, why? */
ROM_START( ambusht )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )       /* 64k for code */
	ROM_LOAD( "a1.i7",    0x0000, 0x2000, CRC(a7cd149d) SHA1(470ebe60bc23a7908fb96caef8074d65f8c57625) )
	ROM_LOAD( "a2.g7",    0x2000, 0x2000, CRC(8328d88a) SHA1(690f0af10a0550566b67ee570f849b2764448d15) )
	ROM_LOAD( "a3.f7",    0x4000, 0x2000, CRC(8db57ab5) SHA1(5cc7d7ebdfc91fb8d9ed52836d70c1de68001402) )
	ROM_LOAD( "a4.e7",    0x6000, 0x2000, CRC(4a34d2a4) SHA1(ad623161cd6031cb6503ff7445fdd9fb4ea83c8c) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "fa2.n4",    0x0000, 0x2000, CRC(e7f134ba) SHA1(c38321f3da049f756337cba5b3c71f6935922f80) )
	ROM_LOAD( "fa1.m4",    0x2000, 0x2000, CRC(ad10969e) SHA1(4cfccdc4ca377693e92d77cde16f88bbdb840b38) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )
	ROM_LOAD( "a.bpr",        0x0000, 0x0100, CRC(5f27f511) SHA1(fe3ae701443ff50d3d03c0a5d0e0ab0e716b05cc) )  /* color PROMs */

	ROM_LOAD( "b.bpr",        0x0100, 0x0100, CRC(1b03fd3b) SHA1(1a58b212476cacace6065056f23b59a69053a2f6) )	/* How is this selected, */
	ROM_LOAD( "13.bpr",		  0x0200, 0x0100, CRC(547e970f) SHA1(e2ec0bece49fb283e43549703d6d5d6f561c69a6) )  /* I'm not sure what these do. */
	ROM_LOAD( "14.bpr",		  0x0300, 0x0100, CRC(622a8ce7) SHA1(6834f67874251f2ef3a33aec893311f5d10e496f) )  /* They don't look like color PROMs */
ROM_END

ROM_START( ambushv )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )       /* 64k for code */
	ROM_LOAD( "n1_h7.bin",    0x0000, 0x2000, CRC(3c0833b4) SHA1(dd0cfb6da281742114abfe652d38038b078b959e) )
	ROM_LOAD( "ambush.g7",    0x2000, 0x2000, CRC(90291409) SHA1(82f1e109bd066ad9fdea1ce0086be6c334e2658a) )
	ROM_LOAD( "ambush.f7",    0x4000, 0x2000, CRC(d023ca29) SHA1(1ac44960cf6d79936517a9ad4bae6ccd825c9496) )
	ROM_LOAD( "ambush.e7",    0x6000, 0x2000, CRC(6cc2d3ee) SHA1(dccb417d156460ca745d7b62f1df733cbf85d092) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ambush.n4",    0x0000, 0x2000, CRC(ecc0dc85) SHA1(577304bb575293b97b50eea4faafb5394e3da0f5) )
	ROM_LOAD( "ambush.m4",    0x2000, 0x2000, CRC(e86ca98a) SHA1(fae0786bb78ead81653adddd2edb3058371ca5bc) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )
	ROM_LOAD( "a.bpr",        0x0000, 0x0100, CRC(5f27f511) SHA1(fe3ae701443ff50d3d03c0a5d0e0ab0e716b05cc) )  /* color PROMs */

	ROM_LOAD( "b.bpr",        0x0100, 0x0100, CRC(1b03fd3b) SHA1(1a58b212476cacace6065056f23b59a69053a2f6) )	/* How is this selected, */
	ROM_LOAD( "13.bpr",		  0x0200, 0x0100, CRC(547e970f) SHA1(e2ec0bece49fb283e43549703d6d5d6f561c69a6) )  /* I'm not sure what these do. */
	ROM_LOAD( "14.bpr",		  0x0300, 0x0100, CRC(622a8ce7) SHA1(6834f67874251f2ef3a33aec893311f5d10e496f) )  /* They don't look like color PROMs */
ROM_END

GAME( 1983, ambush, 0,      ambush, ambush, 0, ROT0, "Nippon Amuse Co-Ltd", "Ambush", 0 ,0)
GAME( 1983, ambusht,ambush, ambush, ambush, 0, ROT0, "Tecfri", "Ambush (Tecfri)", 0 ,0)
GAME( 1983, ambushv,ambush, ambush, ambush, 0, ROT0, "Volt Elec co-ltd", "Ambush (Volt Elec co-ltd)", 0 ,0)
