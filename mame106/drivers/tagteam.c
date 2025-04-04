/***************************************************************************

Tag Team Wrestling hardware description:

This hardware is very similar to the BurgerTime/Lock N Chase family of games
but there are just enough differences to make it a pain to share the
codebase. It looks like this hardware is a bridge between the BurgerTime
family and the later Technos games, like Mat Mania and Mysterious Stones.

The video hardware supports 3 sprite banks instead of 1
The sound hardware appears nearly identical to Mat Mania


Stephh's notes :

  - When the "Cabinet" Dip Switch is set to "Cocktail", the screen status
    depends on which player is the main wrestler on the ring :

      * player 1 : normal screen
      * player 2 : inverted screen

TODO:
        * fix hi-score (reset) bug

***************************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "sound/ay8910.h"
#include "sound/dac.h"

extern WRITE8_HANDLER( tagteam_videoram_w );
extern WRITE8_HANDLER( tagteam_colorram_w );
extern READ8_HANDLER( tagteam_mirrorvideoram_r );
extern WRITE8_HANDLER( tagteam_mirrorvideoram_w );
extern READ8_HANDLER( tagteam_mirrorcolorram_r );
extern WRITE8_HANDLER( tagteam_mirrorcolorram_w );
extern WRITE8_HANDLER( tagteam_video_control_w );
extern WRITE8_HANDLER( tagteam_control_w );
extern WRITE8_HANDLER( tagteam_flipscreen_w );

extern PALETTE_INIT( tagteam );
extern VIDEO_START( tagteam );
extern VIDEO_UPDATE( tagteam );

static WRITE8_HANDLER( sound_command_w )
{
	soundlatch_w(offset,data);
	cpunum_set_input_line(1,M6502_IRQ_LINE,HOLD_LINE);
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2000, 0x2000) AM_READ(input_port_1_r)     /* IN1 */
	AM_RANGE(0x2001, 0x2001) AM_READ(input_port_0_r)     /* IN0 */
	AM_RANGE(0x2002, 0x2002) AM_READ(input_port_2_r)     /* DSW2 */
	AM_RANGE(0x2003, 0x2003) AM_READ(input_port_3_r)     /* DSW1 */
	AM_RANGE(0x4000, 0x43ff) AM_READ(tagteam_mirrorvideoram_r)
	AM_RANGE(0x4400, 0x47ff) AM_READ(tagteam_mirrorcolorram_r)
	AM_RANGE(0x4800, 0x4fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(tagteam_flipscreen_w)
	AM_RANGE(0x2001, 0x2001) AM_WRITE(tagteam_control_w)
	AM_RANGE(0x2002, 0x2002) AM_WRITE(sound_command_w)
//  AM_RANGE(0x2003, 0x2003) AM_WRITE(MWA8_NOP) /* Appears to increment when you're out of the ring */
	AM_RANGE(0x4000, 0x43ff) AM_WRITE(tagteam_mirrorvideoram_w)
	AM_RANGE(0x4400, 0x47ff) AM_WRITE(tagteam_mirrorcolorram_w)
	AM_RANGE(0x4800, 0x4bff) AM_WRITE(tagteam_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x4c00, 0x4fff) AM_WRITE(tagteam_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x8000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x03ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2007, 0x2007) AM_READ(soundlatch_r)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x03ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x2001, 0x2001) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x2002, 0x2002) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x2003, 0x2003) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x2004, 0x2004) AM_WRITE(DAC_0_data_w)
	AM_RANGE(0x2005, 0x2005) AM_WRITE(interrupt_enable_w)
ADDRESS_MAP_END



static INTERRUPT_GEN( tagteam_interrupt )
{
	static int coin;
	int port;

	port = readinputportbytag("IN0") & 0xc0;

	if (port != 0xc0)    /* Coin */
	{
		if (coin == 0)
		{
			coin = 1;
			cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
		}
	}
	else coin = 0;
}

#define TAGTEAM_PLAYERS\
	PORT_START_TAG("IN0")\
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY\
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )\
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )\
	PORT_START_TAG("IN1")\
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )\
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

INPUT_PORTS_START( bigprowr )
	TAGTEAM_PLAYERS

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x60, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )			// "Upright, Single Controls"
	PORT_DIPSETTING(    0x40, "Upright, Dual Controls" )
//  PORT_DIPSETTING(    0x20, "Cocktail, Single Controls" ) // IMPOSSIBLE !
	PORT_DIPSETTING(    0x60, DEF_STR( Cocktail ) )			// "Cocktail, Dual Controls"
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK  )

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

/* Same as 'bigprowr', but additional "Coin Mode" Dip Switch */
INPUT_PORTS_START( tagteam )
	TAGTEAM_PLAYERS
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )PORT_CONDITION("DSW2",0xe0,PORTCOND_NOTEQUALS,0x80) //Mode 1
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_6C ) )PORT_CONDITION("DSW2",0xe0,PORTCOND_EQUALS,0x80) //Mode 2
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )PORT_CONDITION("DSW2",0xe0,PORTCOND_NOTEQUALS,0x80) //Mode 1
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_6C ) )PORT_CONDITION("DSW2",0xe0,PORTCOND_EQUALS,0x80) //Mode 2
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x60, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )			// "Upright, Single Controls"
	PORT_DIPSETTING(    0x40, "Upright, Dual Controls" )
//  PORT_DIPSETTING(    0x20, "Cocktail, Single Controls" ) // IMPOSSIBLE !
	PORT_DIPSETTING(    0x60, DEF_STR( Cocktail ) )			// "Cocktail, Dual Controls"
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK  )

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0xe0, 0x00, "Coin Mode" )				// Check code at 0xff5c
	PORT_DIPSETTING(    0x00, "Mode 1" )
	PORT_DIPSETTING(    0x80, "Mode 2" )
	/* Other values (0x20, 0x40, 0x60, 0xa0, 0xc0, 0xe0) : "Mode 1" */
	/* Therefore the logic for DIPCONDITION is '=0x80' not '<>0x00'*/
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	3072,   /* 3072 characters */
	3,      /* 3 bits per pixel */
	{ 2*3072*8*8, 3072*8*8, 0 },    /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8     /* every char takes 8 consecutive bytes */
};


static const gfx_layout spritelayout =
{
	16,16,  /* 16*16 sprites */
	768,    /* 768 sprites */
	3,      /* 3 bits per pixel */
	{ 2*768*16*16, 768*16*16, 0 },  /* the bitplanes are separated */
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static const gfx_decode tagteam_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0, 4 }, /* chars */
	{ REGION_GFX1, 0, &spritelayout, 0, 4 }, /* sprites */
	{ -1 } /* end of array */
};



static MACHINE_DRIVER_START( tagteam )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1500000)	/* 1.5 MHz ?? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(tagteam_interrupt,1)

	MDRV_CPU_ADD(M6502, 975000)
	/* audio CPU */  /* 975 kHz ?? */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,16)   /* IRQs are triggered by the main CPU */

	MDRV_FRAMES_PER_SECOND(57)
	MDRV_VBLANK_DURATION(3072)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(tagteam_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(32)
	MDRV_COLORTABLE_LENGTH(32)

	MDRV_PALETTE_INIT(tagteam)
	MDRV_VIDEO_START(tagteam)
	MDRV_VIDEO_UPDATE(tagteam)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



ROM_START( bigprowr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "bf00-1.20",    0x08000, 0x2000, CRC(8aba32c9) SHA1(9228082a8251feaf25849311c3de63ca42cf659e) )
	ROM_LOAD( "bf01.33",      0x0a000, 0x2000, CRC(0a41f3ae) SHA1(1b82cd864f0bd7f16f961fec0b88307996abb166) )
	ROM_LOAD( "bf02.34",      0x0c000, 0x2000, CRC(a28b0a0e) SHA1(50b40048a3e2efb2afb7acfb4efde6dbc25fc009) )
	ROM_LOAD( "bf03.46",      0x0e000, 0x2000, CRC(d4cf7ec7) SHA1(cfabe40adb05f6239c3e2f002a78efb50150d27d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for audio code */
	ROM_LOAD( "bf4.8",        0x04000, 0x2000, CRC(0558e1d8) SHA1(317011c0e3a9d5f73c67d044c1fab315ff8049fb) )
	ROM_LOAD( "bf5.7",        0x06000, 0x2000, CRC(c1073f24) SHA1(0337c259c10fae3067e5e0e0acf54e6d0891b29f) )
	ROM_LOAD( "bf6.6",        0x08000, 0x2000, CRC(208cd081) SHA1(e5f6379e7f7bc80cdea12de7e0a2bb232bb16b5a) )
	ROM_LOAD( "bf7.3",        0x0a000, 0x2000, CRC(34a033dc) SHA1(01e4c331233a2337c7c53edd221bb87859278b04) )
	ROM_LOAD( "bf8.2",        0x0c000, 0x2000, CRC(eafe8056) SHA1(4a2d1c903e4acee962aeb0f0f18333252790f686) )
	ROM_LOAD( "bf9.1",        0x0e000, 0x2000, CRC(d589ce1b) SHA1(c2aca1cc6867d4d6d6e02ac29a4c53c667bf6d89) )

	ROM_REGION( 0x12000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "bf10.89",      0x00000, 0x2000, CRC(b1868746) SHA1(a3eff1b00f9ac2512d6ca81f9223723642d4f06a) )
	ROM_LOAD( "bf11.94",      0x02000, 0x2000, CRC(c3fe99c1) SHA1(beb3056b37f26a52f3c0907868054b3cc3c4e3ea) )
	ROM_LOAD( "bf12.103",     0x04000, 0x2000, CRC(c8717a46) SHA1(6de0238071aacb443234d9a7ef250ddfaa9dd1a8) )
	ROM_LOAD( "bf13.91",      0x06000, 0x2000, CRC(23ee34d3) SHA1(2af79845c2d4f06eb85db14d67ec4a499fc272b1) )
	ROM_LOAD( "bf14.95",      0x08000, 0x2000, CRC(a6721142) SHA1(200058d7b688dccda0ab0f568ab6c6c215a23e0a) )
	ROM_LOAD( "bf15.105",     0x0a000, 0x2000, CRC(60ae1078) SHA1(f9c162ff0830aff26d121f04c107dadb060d4bd5) )
	ROM_LOAD( "bf16.93",      0x0c000, 0x2000, CRC(d33dc245) SHA1(3a40ca7f7e17eaaea6e3f90fcf4cb9a72fc4ba8f) )
	ROM_LOAD( "bf17.96",      0x0e000, 0x2000, CRC(ccf42380) SHA1(6a8958201125c1b13b1354c98adc573dbea64d56) )
	ROM_LOAD( "bf18.107",     0x10000, 0x2000, CRC(fd6f006d) SHA1(ad100ac8c0fed24f922a2cc908c88b4fced07eb0) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "fko.8",        0x0000, 0x0020, CRC(b6ee1483) SHA1(b2ea7be533e29da6cd7302532da2eb0410490e6a) )
	ROM_LOAD( "fjo.25",       0x0020, 0x0020, CRC(24da2b63) SHA1(4db7e1ff1b9fd5ae4098cd7ca66cf1fa2574501a) ) /* What is this prom for? */
ROM_END

ROM_START( tagteam )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "prowbf0.bin",  0x08000, 0x2000, CRC(6ec3afae) SHA1(8ae11cb41a72bda053ce8b79c383503da5324cd1) )
	ROM_LOAD( "prowbf1.bin",  0x0a000, 0x2000, CRC(b8fdd176) SHA1(afa8e890ac54101eef0274c8aabe25d188085a18) )
	ROM_LOAD( "prowbf2.bin",  0x0c000, 0x2000, CRC(3d33a923) SHA1(e6402290fca72f4fa3a76e37957b9d4f5b4aeddb) )
	ROM_LOAD( "prowbf3.bin",  0x0e000, 0x2000, CRC(518475d2) SHA1(b26bb0bb658bfd5ac24ee8ebb7fc11a79917aeda) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for audio code */
	ROM_LOAD( "bf4.8",        0x04000, 0x2000, CRC(0558e1d8) SHA1(317011c0e3a9d5f73c67d044c1fab315ff8049fb) )
	ROM_LOAD( "bf5.7",        0x06000, 0x2000, CRC(c1073f24) SHA1(0337c259c10fae3067e5e0e0acf54e6d0891b29f) )
	ROM_LOAD( "bf6.6",        0x08000, 0x2000, CRC(208cd081) SHA1(e5f6379e7f7bc80cdea12de7e0a2bb232bb16b5a) )
	ROM_LOAD( "bf7.3",        0x0a000, 0x2000, CRC(34a033dc) SHA1(01e4c331233a2337c7c53edd221bb87859278b04) )
	ROM_LOAD( "bf8.2",        0x0c000, 0x2000, CRC(eafe8056) SHA1(4a2d1c903e4acee962aeb0f0f18333252790f686) )
	ROM_LOAD( "bf9.1",        0x0e000, 0x2000, CRC(d589ce1b) SHA1(c2aca1cc6867d4d6d6e02ac29a4c53c667bf6d89) )

	ROM_REGION( 0x12000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "prowbf10.bin", 0x00000, 0x2000, CRC(48165902) SHA1(3145fc83f17712b460a08b882677cfcac08fc272) )
	ROM_LOAD( "bf11.94",      0x02000, 0x2000, CRC(c3fe99c1) SHA1(beb3056b37f26a52f3c0907868054b3cc3c4e3ea) )
	ROM_LOAD( "prowbf12.bin", 0x04000, 0x2000, CRC(69de1ea2) SHA1(7b696c74e29c0bae33b386da463365e7e796c6a0) )
	ROM_LOAD( "prowbf13.bin", 0x06000, 0x2000, CRC(ecfa581d) SHA1(1352c6a5f8e6f2d3fbe9f9e74c542ea2467f1438) )
	ROM_LOAD( "bf14.95",      0x08000, 0x2000, CRC(a6721142) SHA1(200058d7b688dccda0ab0f568ab6c6c215a23e0a) )
	ROM_LOAD( "prowbf15.bin", 0x0a000, 0x2000, CRC(d0de7e03) SHA1(79267c09621f8f5255c82a69790d5b52b1c5dd6e) )
	ROM_LOAD( "prowbf16.bin", 0x0c000, 0x2000, CRC(75ee5705) SHA1(95fdea3768f2d5b81ba5dafd3b13a061cd96689a) )
	ROM_LOAD( "bf17.96",      0x0e000, 0x2000, CRC(ccf42380) SHA1(6a8958201125c1b13b1354c98adc573dbea64d56) )
	ROM_LOAD( "prowbf18.bin", 0x10000, 0x2000, CRC(e73a4bba) SHA1(6dbc2d741ebf8fcce9144cfe6fe6f35acd25ceef) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "fko.8",        0x0000, 0x0020, CRC(b6ee1483) SHA1(b2ea7be533e29da6cd7302532da2eb0410490e6a) )
	ROM_LOAD( "fjo.25",       0x0020, 0x0020, CRC(24da2b63) SHA1(4db7e1ff1b9fd5ae4098cd7ca66cf1fa2574501a) ) /* What is this prom for? */
ROM_END



GAME( 1983, bigprowr, 0,        tagteam, bigprowr, 0, ROT270, "Technos", "The Big Pro Wrestling!", 0 ,0)
GAME( 1983, tagteam,  bigprowr, tagteam, tagteam,  0, ROT270, "Technos (Data East license)", "Tag Team Wrestling", 0 ,0)
