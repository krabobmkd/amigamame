/*********************************************************
Sega hardware based on their SG-1000 console
Driver by Tomasz Slanina  dox@space.pl


Supported games :
- Champion Boxing
- Champion Pro Wrestling

Memory map :
0x0000 - 0xBFFF ROM
0xC000 - 0xC3FF RAM

CPU:
Z80 A                          3.57954 MHz (Champion Boxing)
315-5114 (encrypted Z80) 3.57954 MHz (Champion Pro Wrestling)

8255 for I/O port work
3 Eproms for program and graphics
TMM2064 for program RAM
TMS9928 for graphics ( 3.57954 MHz? )
8 8118 dynamic RAMs for the graphics
74LS139 and 74LS32 for logic gating
ULN2003 for coin counter output
76489 for music
7808 voltage regulator to a transistorized circuit for TV output
secondary crystal, numbers unknown for the TMS9928

******************************************************/

#include "driver.h"
#include "sound/sn76496.h"
#include "vidhrdw/tms9928a.h"
#include "machine/segacrpt.h"

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
    AM_RANGE(0x0000, 0x7FFF) AM_READ(MRA8_ROM)
    AM_RANGE(0x8000, 0xbFFF) AM_READ(MRA8_ROM)
    AM_RANGE(0xc000, 0xc3ff) AM_READ(MRA8_RAM)

ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
    AM_RANGE(0x0000, 0x7FFF) AM_WRITE(MWA8_ROM)
    AM_RANGE(0x8000, 0xbFFF) AM_WRITE(MWA8_ROM)
    AM_RANGE(0xc000, 0xc3ff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
    AM_RANGE(0xBE, 0xBE) AM_READ(TMS9928A_vram_r)
    AM_RANGE(0xBF, 0xBF) AM_READ(TMS9928A_register_r)
    AM_RANGE(0xDC, 0xDC) AM_READ(input_port_0_r)
    AM_RANGE(0xDD, 0xDD) AM_READ(input_port_1_r)
    AM_RANGE(0xDE, 0xDE) AM_READ(input_port_2_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
    AM_RANGE(0xBE, 0xBE) AM_WRITE(TMS9928A_vram_w)
    AM_RANGE(0xBF, 0xBF) AM_WRITE(TMS9928A_register_w)
    AM_RANGE(0xDF, 0xDF) AM_WRITE(MWA8_NOP)  //? 8255 ?
    AM_RANGE(0x7f, 0x7F) AM_WRITE(SN76496_0_w)
ADDRESS_MAP_END

INPUT_PORTS_START( chwrestl )
    PORT_START
    PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
    PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
    PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
    PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
    PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
    PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
    PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	  PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_START2 )

    PORT_START
    PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
    PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
    PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
    PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
    PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
    PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
    PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	  PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

    PORT_START
    PORT_DIPNAME( 0x80, 0x80, DEF_STR( Language ) )
		PORT_DIPSETTING(    0x00, DEF_STR( Japanese ) )
		PORT_DIPSETTING(    0x80, DEF_STR( English ) )
		PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) )
		PORT_DIPSETTING(    0x40, DEF_STR( Off ))
		PORT_DIPSETTING(    0x00, DEF_STR( On ))
		PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_B ) )
		PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
		PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
		PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
		PORT_DIPSETTING(    0x10, DEF_STR( 1C_3C ) )

INPUT_PORTS_END


INPUT_PORTS_START( chboxing )
    PORT_START
    PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
    PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
    PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
    PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
    PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
    PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
    PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	  PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_START2 )

    PORT_START
    PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
    PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
    PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
    PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
    PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
    PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
    PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	  PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

    PORT_START
    PORT_DIPNAME( 0x80, 0x00, DEF_STR( Language ) )
		PORT_DIPSETTING(    0x80, DEF_STR( Japanese ) )
		PORT_DIPSETTING(    0x00, DEF_STR( English ) )
		PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) )
		PORT_DIPSETTING(    0x40, DEF_STR( Off ))
		PORT_DIPSETTING(    0x00, DEF_STR( On ))
		PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_B ) )
		PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
		PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
		PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
		PORT_DIPSETTING(    0x10, DEF_STR( 1C_3C ) )
INPUT_PORTS_END


static INTERRUPT_GEN( sg100a_interrupt )
{
    TMS9928A_interrupt();
}

static void vdp_interrupt (int state)
{
	cpunum_set_input_line(0,0, HOLD_LINE);
}

static const TMS9928a_interface tms9928a_interface =
{
	TMS99x8A,
	0x4000,
	vdp_interrupt
};

static MACHINE_DRIVER_START( sg1000a )
	MDRV_CPU_ADD(Z80, 3579545)       /* 3.579545 Mhz */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_FRAMES_PER_SECOND(60)
	MDRV_CPU_VBLANK_INT(sg100a_interrupt,1)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_TMS9928A( &tms9928a_interface )

	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(SN76496, 3579545)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


ROM_START( chwrestl )
	ROM_REGION( 2*0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "5732",	0x0000, 0x4000, CRC(a4e44370) SHA1(a9dbf60e77327dd2bec6816f3142b42ad9ca4d09) ) /* encrypted */
	ROM_LOAD( "5733",	0x4000, 0x4000, CRC(4f493538) SHA1(467862fe9337497e3cdebb29bf28f6cfe3066ccd) ) /* encrypted */
	ROM_LOAD( "5734",	0x8000, 0x4000, CRC(d99b6301) SHA1(5e762ed45cde08d5223828c6b1d3569b2240462c) )
ROM_END

ROM_START( chboxing )
  ROM_REGION( 0x10000, REGION_CPU1, 0 )
  ROM_LOAD( "cb6105.bin",	0x0000, 0x4000, CRC(43516f2e) SHA1(e3a9bbe914b5bfdcd1f85ca5fae922c4cae3c106) )
	ROM_LOAD( "cb6106.bin",	0x4000, 0x4000, CRC(65e2c750) SHA1(843466b8d6baebb4d5e434fbdafe3ae8fed03475) )
	ROM_LOAD( "cb6107.bin",	0x8000, 0x2000, CRC(c2f8e522) SHA1(932276e7ad33aa9efbb4cd10bc3071d88cb082cb) )
ROM_END


DRIVER_INIT(chwrestl)
{
	regulus_decode();
}

GAME( 1984, chboxing, 0, sg1000a, chboxing, 0, ROT0, "Sega", "Champion Boxing", 0,0)
GAME( 1985, chwrestl, 0, sg1000a, chwrestl, chwrestl, ROT0, "Sega", "Champion Pro Wrestling", 0,0)

