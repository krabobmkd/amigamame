/***************************************************************************

The Pit/Round Up/Intrepid/Super Mouse memory map (preliminary)

Driver by Zsolt Vasvari

Main CPU:

0000-4fff ROM
8000-87ff RAM
8800-8bff Color RAM        (Not used in Intrepid/Super Mouse)
8c00-8fff Mirror for above (Not used in Intrepid/Super Mouse)
9000-93ff Video RAM
9400-97ff Mirror for above (Color RAM in Intrepid/Super Mouse)
9800-983f Attributes RAM
9840-985f Sprite RAM

Read:

a000      Input Port 0
a800      Input Port 1
b000      DIP Switches
b800      Watchdog Reset

Write:

b000      NMI Enable
b002      Coin Lockout
b003      Sound Enable
b005      Intrepid graphics bank select
b006      Flip Screen X
b007      Flip Screen Y
b800      Sound Command


Sound CPU:

0000-0fff ROM  (0000-07ff in The Pit)
3800-3bff RAM


Port I/O Read:

8f  AY8910 Read Port


Port I/O Write:

00  Reset Sound Command
8c  AY8910 #2 Control Port    (Intrepid/Super Mouse only)
8d  AY8910 #2 Write Port      (Intrepid/Super Mouse only)
8e  AY8910 #1 Control Port
8f  AY8910 #1 Write Port

***********************************************************

The Pit
Taito, 1982

PCB Layout
----------

HT-01A
HRK001188
|-------------------------------------------------------|
| MB3730               5MHz                             |
|                                 |---------------|    |-|
|         AY3-8910     Z80(1)     |               |    | |
|                                 |     EPOXY     |    | |
|                                 |    MODULE     |    | |
|                                 |               |    | |
|2  82S123.IC4                    |               |    | |
|2  VOL                           |---------------|    |-|
|W                                                      |
|A                                                      |
|Y    DSW1(4)                                          |-|
|     DSW2(4)     PIT07.IC30   PIT01.IC38              | |
|            2114                                      | |
|            2114 PIT06.IC31   PIT02.IC39              | |
|            2114                                      | |
|            2114              PIT03.IC40  Z80(2)      | |
|                                                      |-|
|                 PIT05.IC33   PIT04.IC41               |
|-------------------------------------------------------|
Notes:
      Z80(1)clock- 2.500MHz [5/2]
      Z80(2)clock- 3.072MHz [18.432/6]
      8910 clock - 1.536MHz [18.432/12]
      HSync      - 15.5kHz
      VSync      - 60Hz

HT-01B
|-------------------------------------------------------|
|                                                       |
|                                            PIT08.IC9 |-|
|                                                      | |
| 18.432MHz                                  PIT09.IC8 | |
|                                                      | |
|                                               2114   | |
|                                                      | |
|                     2125                      2114   |-|
|                                                       |
|                     2125                              |
|                                                      |-|
|                     2125                             | |
|                                                      | |
|                     2125                             | |
|                                                      | |
|                     2125                      2114   | |
|                                                      |-|
|                     2125                      2114    |
|-------------------------------------------------------|


***************************************************************************/

#include "driver.h"
#include "sound/ay8910.h"

extern unsigned char *thepit_attributesram;
extern unsigned char *intrepid_sprite_bank_select;
WRITE8_HANDLER( thepit_attributes_w );

PALETTE_INIT( thepit );
PALETTE_INIT( suprmous );
VIDEO_UPDATE( thepit );
READ8_HANDLER( thepit_input_port_0_r );
WRITE8_HANDLER( thepit_sound_enable_w );
WRITE8_HANDLER( intrepid_graphics_bank_select_w );

static WRITE8_HANDLER( flip_screen_x_w )
{
	flip_screen_x_set(data);
}

static WRITE8_HANDLER( flip_screen_y_w )
{
	flip_screen_y_set(data);
}


static ADDRESS_MAP_START( thepit_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x4fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8800, 0x8bff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8c00, 0x8fff) AM_READ(colorram_r)
	AM_RANGE(0x9000, 0x93ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x9400, 0x97ff) AM_READ(videoram_r)
	AM_RANGE(0x9800, 0x98ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xa000, 0xa000) AM_READ(thepit_input_port_0_r)
	AM_RANGE(0xa800, 0xa800) AM_READ(input_port_1_r)
	AM_RANGE(0xb000, 0xb000) AM_READ(input_port_2_r)
	AM_RANGE(0xb800, 0xb800) AM_READ(watchdog_reset_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( thepit_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x4fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8800, 0x8bff) AM_WRITE(colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x8c00, 0x8fff) AM_WRITE(colorram_w)
	AM_RANGE(0x9000, 0x93ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x9400, 0x97ff) AM_WRITE(videoram_w)
	AM_RANGE(0x9800, 0x983f) AM_WRITE(thepit_attributes_w) AM_BASE(&thepit_attributesram)
	AM_RANGE(0x9840, 0x985f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x9860, 0x98ff) AM_WRITE(MWA8_RAM) // Probably unused
	AM_RANGE(0xa000, 0xa000) AM_WRITE(MWA8_NOP) // Not hooked up according to the schematics
	AM_RANGE(0xb000, 0xb000) AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xb001, 0xb001) AM_WRITE(MWA8_NOP) // Unused, but initialized
	AM_RANGE(0xb002, 0xb002) AM_WRITE(MWA8_NOP) // coin_lockout_w
	AM_RANGE(0xb003, 0xb003) AM_WRITE(thepit_sound_enable_w)
	AM_RANGE(0xb004, 0xb005) AM_WRITE(MWA8_NOP) // Unused, but initialized
	AM_RANGE(0xb006, 0xb006) AM_WRITE(flip_screen_x_w)
	AM_RANGE(0xb007, 0xb007) AM_WRITE(flip_screen_y_w)
	AM_RANGE(0xb800, 0xb800) AM_WRITE(soundlatch_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( intrepid_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x4fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8c00, 0x8fff) AM_READ(colorram_r)
	AM_RANGE(0x9000, 0x98ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xa000, 0xa000) AM_READ(thepit_input_port_0_r)
	AM_RANGE(0xa800, 0xa800) AM_READ(input_port_1_r)
	AM_RANGE(0xb000, 0xb000) AM_READ(input_port_2_r)
	AM_RANGE(0xb800, 0xb800) AM_READ(watchdog_reset_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( intrepid_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x4fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8c00, 0x8fff) AM_WRITE(colorram_w)
	AM_RANGE(0x9000, 0x93ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x9400, 0x97ff) AM_WRITE(colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x9800, 0x983f) AM_WRITE(thepit_attributes_w) AM_BASE(&thepit_attributesram)
	AM_RANGE(0x9840, 0x985f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x9860, 0x98ff) AM_WRITE(MWA8_RAM) // Probably unused
	AM_RANGE(0xb000, 0xb000) AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xb001, 0xb001) AM_WRITE(MWA8_NOP) // Unused, but initialized
	AM_RANGE(0xb002, 0xb002) AM_WRITE(MWA8_NOP) // coin_lockout_w
	AM_RANGE(0xb003, 0xb003) AM_WRITE(thepit_sound_enable_w)
	AM_RANGE(0xb004, 0xb004) AM_WRITE(MWA8_NOP) // Unused, but initialized
	AM_RANGE(0xb005, 0xb005) AM_WRITE(intrepid_graphics_bank_select_w)
	AM_RANGE(0xb006, 0xb006) AM_WRITE(flip_screen_x_w)
	AM_RANGE(0xb007, 0xb007) AM_WRITE(flip_screen_y_w)
	AM_RANGE(0xb800, 0xb800) AM_WRITE(soundlatch_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x3800, 0x3bff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x3800, 0x3bff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x8f, 0x8f) AM_READ(AY8910_read_port_0_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(soundlatch_clear_w)
	AM_RANGE(0x8c, 0x8c) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x8d, 0x8d) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x8e, 0x8e) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x8f, 0x8f) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END


#define IN0_REAL\
	PORT_START_TAG("IN0")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define IN2_FAKE\
	PORT_START_TAG("IN2")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

INPUT_PORTS_START( thepit )
IN0_REAL
	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x03, 0x00, "Coinage P1/P2" )
	PORT_DIPSETTING(    0x00, "1Cr/2Cr" )
	PORT_DIPSETTING(    0x01, "2Cr/3Cr" )
	PORT_DIPSETTING(    0x02, "2Cr/4Cr" )
	PORT_DIPSETTING(    0x03, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x04, 0x00, "Game Speed" )
	PORT_DIPSETTING(    0x04, "Slow" )
	PORT_DIPSETTING(    0x00, "Fast" )
	PORT_DIPNAME( 0x08, 0x00, "Time Limit" )
	PORT_DIPSETTING(    0x00, "Long" )
	PORT_DIPSETTING(    0x08, "Short" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
	  IN2_FAKE
INPUT_PORTS_END


INPUT_PORTS_START( roundup )
IN0_REAL

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x20, "30000" )
	PORT_DIPNAME( 0x40, 0x40, "Gly Boys Wake Up" )
	PORT_DIPSETTING(    0x40, "Slow" )
	PORT_DIPSETTING(    0x00, "Fast" )
	PORT_SERVICE( 0x80, IP_ACTIVE_HIGH )

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
IN2_FAKE
INPUT_PORTS_END


INPUT_PORTS_START( fitter )
IN0_REAL

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x20, "30000" )
	PORT_DIPNAME( 0x40, 0x40, "Gly Boys Wake Up" )
	PORT_DIPSETTING(    0x40, "Slow" )
	PORT_DIPSETTING(    0x00, "Fast" )
	PORT_DIPNAME( 0x80, 0x00, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
IN2_FAKE
INPUT_PORTS_END


INPUT_PORTS_START( intrepid )
IN0_REAL
/* The bit at 0x80 in IN0 Starts a timer, which, after it runs down, doesn't seem to do anything. See $0105 */

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x10, "30000" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x20, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
IN2_FAKE

INPUT_PORTS_END


INPUT_PORTS_START( dockman )
IN0_REAL

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x10, "30000" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )	/* not used? */
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )	/* not used? */
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
IN2_FAKE
INPUT_PORTS_END


INPUT_PORTS_START( suprmous )
IN0_REAL
	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x07, 0x01, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_7C ) )
	PORT_DIPNAME( 0x18, 0x00, DEF_STR( Lives ) )  /* The game reads these together */
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x08, "5" )
  //PORT_DIPSETTING(    0x10, "5" )
  //PORT_DIPSETTING(    0x18, "5" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x10, "5000" )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x00, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
IN2_FAKE
INPUT_PORTS_END

INPUT_PORTS_START( rtriv )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x04, 0x04, "Show Correct Answer" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x10, 0x10, "Monitor" )
	PORT_DIPSETTING(    0x10, "Vertical" )
	PORT_DIPSETTING(    0x00, "Horizontal" )
	PORT_DIPNAME( 0x20, 0x20, "Gaming Option" )
	PORT_DIPSETTING(    0x20, "Number of Wrong Answer" )
	PORT_DIPSETTING(    0x00, "Number of Good Answer for Bonus Question" )
	PORT_DIPNAME( 0xc0, 0x40, "Gaming Option Number" )
	PORT_DIPSETTING(    0x00, "2" ) PORT_CONDITION("DSW",0x20,PORTCOND_EQUALS,0x20)
	PORT_DIPSETTING(    0x40, "3" ) PORT_CONDITION("DSW",0x20,PORTCOND_EQUALS,0x20)
	PORT_DIPSETTING(    0x80, "4" ) PORT_CONDITION("DSW",0x20,PORTCOND_EQUALS,0x20)
	PORT_DIPSETTING(    0xc0, "5" ) PORT_CONDITION("DSW",0x20,PORTCOND_EQUALS,0x20)
	PORT_DIPSETTING(    0x00, "4" ) PORT_CONDITION("DSW",0x20,PORTCOND_NOTEQUALS,0x20)
	PORT_DIPSETTING(    0x40, "5" ) PORT_CONDITION("DSW",0x20,PORTCOND_NOTEQUALS,0x20)
	PORT_DIPSETTING(    0x80, "6" ) PORT_CONDITION("DSW",0x20,PORTCOND_NOTEQUALS,0x20)
	PORT_DIPSETTING(    0xc0, "7" ) PORT_CONDITION("DSW",0x20,PORTCOND_NOTEQUALS,0x20)

	/* Since the real inputs are multiplexed, we used this fake port
       to read the 2nd player controls when the screen is flipped */
	PORT_START_TAG("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	256,    /* 256 characters */
	2,      /* 2 bits per pixel */
	{ 0x1000*8, 0 }, /* the two bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8     /* every char takes 8 consecutive bytes */
};


static const gfx_layout spritelayout =
{
	16,16,  /* 16*16 sprites */
	64,     /* 64 sprites */
	2,      /* 2 bits per pixel */
	{ 0x1000*8, 0 },	/* the two bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
	  8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
	  16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8    /* every sprite takes 32 consecutive bytes */
};


static const gfx_layout suprmous_charlayout =
{
	8,8,	/* 8*8 characters */
	256,	/* 256 characters */
	3,	    /* 3 bits per pixel */
	{ 0x2000*8, 0x1000*8, 0 },	/* the three bitplanes for 4 pixels are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8	    /* every char takes 8 consecutive bytes */
};


static const gfx_layout suprmous_spritelayout =
{
	16,16,	/* 16*16 sprites */
	64,		/* 64 sprites */
	3,	    /* 3 bits per pixel */
	{ 0x2000*8, 0x1000*8, 0 },	/* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
	  8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
	  16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8	/* every sprite takes 32 consecutive bytes */
};


static const gfx_decode thepit_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,     0, 8 },
	{ REGION_GFX1, 0, &spritelayout,   0, 8 },
	{ -1 } /* end of array */
};

static const gfx_decode intrepid_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,     0, 8 },
	{ REGION_GFX1, 0x0000, &spritelayout,   0, 8 },
	{ REGION_GFX1, 0x0800, &charlayout,     0, 8 },
	{ REGION_GFX1, 0x0800, &spritelayout,   0, 8 },
	{ -1 } /* end of array */
};

static const gfx_decode suprmous_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &suprmous_charlayout,   0, 4 },
	{ REGION_GFX1, 0x0800, &suprmous_spritelayout, 0, 4 },
	{ -1 } /* end of array */
};


static struct AY8910interface ay8910_interface =
{
	soundlatch_r
};

static MACHINE_DRIVER_START( thepit )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", Z80, 18432000/6)     /* 3.072 MHz */
	MDRV_CPU_PROGRAM_MAP(thepit_readmem,thepit_writemem)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,1)

	MDRV_CPU_ADD(Z80, 10000000/4)     /* 2.5 MHz */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_IO_MAP(sound_readport,sound_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)       /* frames per second, vblank duration */

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(thepit_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(32+8)
	MDRV_COLORTABLE_LENGTH(32)

	MDRV_PALETTE_INIT(thepit)
	MDRV_VIDEO_START(generic)
	MDRV_VIDEO_UPDATE(thepit)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 18432000/12)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 18432000/12)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( intrepid )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(thepit)
	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(intrepid_readmem,intrepid_writemem)

	/* video hardware */
	MDRV_GFXDECODE(intrepid_gfxdecodeinfo)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( suprmous )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(intrepid)

	/* video hardware */
	MDRV_GFXDECODE(suprmous_gfxdecodeinfo)
	MDRV_PALETTE_INIT(suprmous)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( thepit )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "pit01.ic38",         0x0000, 0x1000, CRC(87c269bf) SHA1(78a85a637cbf0dcfde7ccaa9a2d543078655b566) )
	ROM_LOAD( "pit02.ic39",         0x1000, 0x1000, CRC(e1dfd360) SHA1(fd18f12edd39574d20b1e053dd3e7131d49f3db2) )
	ROM_LOAD( "pit03.ic40",         0x2000, 0x1000, CRC(3674ccac) SHA1(876de2994ffdbc1f2226b5d672b5e92125d879da) )
	ROM_LOAD( "pit04.ic41",         0x3000, 0x1000, CRC(bddde829) SHA1(267fc2a65a906bcd5786a7a97af986adac0588c7) )
	ROM_LOAD( "pit05.ic33",         0x4000, 0x1000, CRC(584d1546) SHA1(9758e4b012bf93c6d847cc2cac890febe5d8335e) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "pit07.ic30",          0x0000, 0x0800, CRC(2d4881f9) SHA1(4773235d427ab88116e07599d0d5b130377548e7) )
	ROM_LOAD( "pit06.ic31",          0x0800, 0x0800, CRC(c9d8c1cc) SHA1(66d0840182ede356c53cd1f930ea8abf86094ab7) )

	ROM_REGION( 0x1800, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "pit08.ic9",           0x0000, 0x0800, CRC(00dce65f) SHA1(ba0cce484d1f8693a85b85e0689d107588df9043) )
	ROM_LOAD( "pit09.ic8",           0x1000, 0x0800, CRC(a2e2b218) SHA1(1aa293a9503f3cbbc2fbd84b6b1d30124ef462e7) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "82s123.ic4",   0x0000, 0x0020, CRC(a758b567) SHA1(d188c90dba10fe3abaae92488786b555b35218c5) ) /* Color prom */
ROM_END

ROM_START( thepitc )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "p38b",         0x0000, 0x1000, CRC(7315e1bc) SHA1(a07f252efcc81b40ef273007e9ce74db140b1bee) )
	ROM_LOAD( "p39b",         0x1000, 0x1000, CRC(c9cc30fe) SHA1(27938ebc27480e8cf40bfdd930a4899984cfeb83) )
	ROM_LOAD( "p40b",         0x2000, 0x1000, CRC(986738b5) SHA1(5e5f326f589814251e3815babb9de425605f7ece) )
	ROM_LOAD( "p41b",         0x3000, 0x1000, CRC(31ceb0a1) SHA1(d027fc23ff5848506e3c912760977feb0c778716) )
	ROM_LOAD( "p33b",         0x4000, 0x1000, CRC(614ec454) SHA1(8089ffd92d1a6245f9b0162f5297ee4800d07a1b) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "p30",          0x0000, 0x0800, CRC(1b79dfb6) SHA1(ba78b035a91a67732414ba327640fb771d4323c5) )

	ROM_REGION( 0x1800, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "p9",           0x0000, 0x0800, CRC(69502afc) SHA1(9baf094baab8325af659879cfb6984eeca0d94bd) )
	ROM_LOAD( "p8",           0x1000, 0x0800, CRC(2ddd5045) SHA1(baa962a874f00e56c15c264980b1e31a2c9dc270) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "82s123.ic4",   0x0000, 0x0020, CRC(a758b567) SHA1(d188c90dba10fe3abaae92488786b555b35218c5) ) /* Color prom */
ROM_END

ROM_START( thepitb ) /* Bootleg based on the Centuri version by AW Zilec Elc Ltd */
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "pit1.bin",     0x0000, 0x1000, CRC(71affecc) SHA1(e64cb2f8d546f5d44dc10a4178f3d211882c45a9) )
	ROM_LOAD( "pit2.bin",     0x1000, 0x1000, CRC(894063cd) SHA1(772ff81cf44d21981f9768f017af5cb81ff57be3) )
	ROM_LOAD( "pit3.bin",     0x2000, 0x1000, CRC(1b488543) SHA1(8991c6424f008ddd15edac953635aecdba4ea696) )
	ROM_LOAD( "pit4.bin",     0x3000, 0x1000, CRC(e941e848) SHA1(1dd9ded69121d28b674209abe1efd60afde6926d) )
	ROM_LOAD( "pit5.bin",     0x4000, 0x1000, CRC(e0643c95) SHA1(bb5133784b2b6a5c217d9ea42641daba2a797fff) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "pit6.bin",     0x0000, 0x0800, CRC(1b79dfb6) SHA1(ba78b035a91a67732414ba327640fb771d4323c5) )

	ROM_REGION( 0x1800, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "pit8.bin",     0x0000, 0x0800, CRC(69502afc) SHA1(9baf094baab8325af659879cfb6984eeca0d94bd) )
	ROM_LOAD( "pit7.bin",     0x1000, 0x0800, CRC(d901b353) SHA1(4a35dd857ca352e0260361376fe666af4b3315af) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "82s123.ic4",   0x0000, 0x0020, CRC(a758b567) SHA1(d188c90dba10fe3abaae92488786b555b35218c5) ) /* Color prom was a MMI6331 and is compatible with the 82s123 prom type */
ROM_END

ROM_START( roundup )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "roundup.u38",  0x0000, 0x1000, CRC(d62c3b7a) SHA1(b6dc7fa001b79706583a40250c8a1e07a639c77a) )
	ROM_LOAD( "roundup.u39",  0x1000, 0x1000, CRC(37bf554b) SHA1(773279fb21c56221d5f29fd31c2149e68dcf3909) )
	ROM_LOAD( "roundup.u40",  0x2000, 0x1000, CRC(5109d0c5) SHA1(b3d1fa9a9b78dcc74fd055fd617c01fcfbc8dad1) )
	ROM_LOAD( "roundup.u41",  0x3000, 0x1000, CRC(1c5ed660) SHA1(6729ecb8072b1ea0bd8557fd0b484d086b94c4b1) )
	ROM_LOAD( "roundup.u33",  0x4000, 0x1000, CRC(2fa711f3) SHA1(8d9f84ee666ec2defca654046e4f7472aabe0767) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "roundup.u30",  0x0000, 0x0800, CRC(1b18faee) SHA1(b4002e2fdaa6bb966da4faa46ac56751a3841f5f) )
	ROM_LOAD( "roundup.u31",  0x0800, 0x0800, CRC(76cf4394) SHA1(5dc13bd5fc92ce4ce12bab60576292a6028891c3) )

	ROM_REGION( 0x1800, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "roundup.u9",   0x0000, 0x0800, CRC(394676a2) SHA1(5bd26d717e25b7c192af8173db9ae18371dbcfbe) )
	ROM_LOAD( "roundup.u10",  0x1000, 0x0800, CRC(a38d708d) SHA1(6632392cece34332a2a4427ec14d95f201319c67) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "roundup.clr",  0x0000, 0x0020, CRC(a758b567) SHA1(d188c90dba10fe3abaae92488786b555b35218c5) )
ROM_END

ROM_START( fitter )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "ic38.bin",     0x0000, 0x1000, CRC(6bf6cca4) SHA1(230864155c323c3e40aed0beaece8ff6d6005bb4) )
	ROM_LOAD( "roundup.u39",  0x1000, 0x1000, CRC(37bf554b) SHA1(773279fb21c56221d5f29fd31c2149e68dcf3909) )
	ROM_LOAD( "ic40.bin",     0x2000, 0x1000, CRC(572e2157) SHA1(030ad888d7fc9b61df6749592934d55de449de8c) )
	ROM_LOAD( "roundup.u41",  0x3000, 0x1000, CRC(1c5ed660) SHA1(6729ecb8072b1ea0bd8557fd0b484d086b94c4b1) )
	ROM_LOAD( "ic33.bin",     0x4000, 0x1000, CRC(ab47c6c2) SHA1(2beb24e2e8661029fc8637e6e1f714cb3e7638a2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "ic30.bin",     0x0000, 0x0800, CRC(4055b5ca) SHA1(abf8f9e830b1190fb87896e1fb3adca8f9e18df1) )
	ROM_LOAD( "ic31.bin",     0x0800, 0x0800, CRC(c9d8c1cc) SHA1(66d0840182ede356c53cd1f930ea8abf86094ab7) )

	ROM_REGION( 0x1800, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "ic9.bin",      0x0000, 0x0800, CRC(a6799a37) SHA1(7864cb255bff976630b6e03b1683f7d3ccd0a80f) )
	ROM_LOAD( "ic8.bin",      0x1000, 0x0800, CRC(a8256dfe) SHA1(b3dfb915ba4367c8c73a8cc6fb02d98ec148f5a1) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "roundup.clr",  0x0000, 0x0020, CRC(a758b567) SHA1(d188c90dba10fe3abaae92488786b555b35218c5) )
ROM_END

ROM_START( intrepid )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "ic19.1",       0x0000, 0x1000, CRC(7d927b23) SHA1(5a8f5a3bd5df423f0e61f96ebdf4adbea534f9ba) )
	ROM_LOAD( "ic18.2",       0x1000, 0x1000, CRC(dcc22542) SHA1(1acddb6a4cb7623ee63f661e9ef14bba7f25b22c) )
	ROM_LOAD( "ic17.3",       0x2000, 0x1000, CRC(fd11081e) SHA1(9f6fcbe8b018c35d05939cddad3ac9078dcf0f11) )
	ROM_LOAD( "ic16.4",       0x3000, 0x1000, CRC(74a51841) SHA1(b5d4d332ed6f2b1e827b7341c2bb34057cbbeff4) )
	ROM_LOAD( "ic15.5",       0x4000, 0x1000, CRC(4fef643d) SHA1(b6dd57e86f26b989341973639cca00c78c9c800d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "ic22.7",       0x0000, 0x0800, CRC(1a7cc392) SHA1(bb800eb1c9f22f5f9c3a2636964f5ab78ddcd2fb) )
	ROM_LOAD( "ic23.6",       0x0800, 0x0800, CRC(91ca7097) SHA1(98e40f3059dfd972e38db5642479dc22cdc4a302) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "ic9.9",        0x0000, 0x1000, CRC(8c70d18d) SHA1(785099c947ee1fe19196dfb02752cc849640fe21) )
	ROM_LOAD( "ic8.8",        0x1000, 0x1000, CRC(04d067d3) SHA1(aeb763e658cd3d0bd849cdae6af55cb1008b2143) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "ic3.prm",      0x0000, 0x0020, CRC(927ff40a) SHA1(3d699d981851989e9190505b0dede5202d688f2b) )
ROM_END

ROM_START( intrepi2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "intrepid.001", 0x0000, 0x1000, CRC(9505df1e) SHA1(83ad91e92038231c9351d11e5471e6ef6bb4b743) )
	ROM_LOAD( "intrepid.002", 0x1000, 0x1000, CRC(27e9f53f) SHA1(06efd4482971b00632dae2d528f96371e98e5e2a) )
	ROM_LOAD( "intrepid.003", 0x2000, 0x1000, CRC(da082ed7) SHA1(0fec8dddce26126ee6af42eca25b09db8ee7b031) )
	ROM_LOAD( "intrepid.004", 0x3000, 0x1000, CRC(60acecd9) SHA1(2f94f1e28908f23f934abc006441f6a367c760c1) )
	ROM_LOAD( "intrepid.005", 0x4000, 0x1000, CRC(7c868725) SHA1(dca370c835fdd0564d42ecca69b9ad2600b1ce31) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "intrepid.007", 0x0000, 0x0800, CRC(f85ead07) SHA1(72479a9b49dd9c629480a2ce72bdd09fbb12b25d) )
	ROM_LOAD( "intrepid.006", 0x0800, 0x0800, CRC(9eb6c61b) SHA1(a168fa634b6909c2ea484c2bbaa5afee2a5fe616) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "ic9.9",        0x0000, 0x1000, CRC(8c70d18d) SHA1(785099c947ee1fe19196dfb02752cc849640fe21) )
	ROM_LOAD( "ic8.8",        0x1000, 0x1000, CRC(04d067d3) SHA1(aeb763e658cd3d0bd849cdae6af55cb1008b2143) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "ic3.prm",      0x0000, 0x0020, CRC(927ff40a) SHA1(3d699d981851989e9190505b0dede5202d688f2b) )
ROM_END

ROM_START( zaryavos )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "zv1.rom",      0x0000, 0x1000, CRC(b7eec75d) SHA1(cf7ab3a411cf126f01b8ed96c3bd4dfb3d76886a) )
	ROM_LOAD( "zv2.rom",      0x1000, 0x1000, CRC(000aa722) SHA1(037e9b946a8abf559a4fa2ac960bd32d3c40e865) )
	ROM_LOAD( "zv3.rom",      0x2000, 0x1000, CRC(9b8b431a) SHA1(5742442b63d96bcc3cb445bbc741301f7aed0644) )
	ROM_LOAD( "zv4.rom",      0x3000, 0x1000, CRC(3636d5bf) SHA1(b5b3fc263004fe59f8c958bc860a88987ae78e4f) )
	ROM_LOAD( "zv5.rom",      0x4000, 0x1000, CRC(c5d405a7) SHA1(d8fd96825b923ac8868e8e396e8e44701904ee76) )
	ROM_LOAD( "zv6.rom",      0x5000, 0x1000, CRC(d07778a1) SHA1(528ad465e1a9029c6eb4511ce8a278e1d90abef0) )
	ROM_LOAD( "zv7.rom",      0x6000, 0x1000, CRC(63d75e5e) SHA1(e7f8e6ebb4348228b4b98289de516d4c009c84ca) )
	ROM_LOAD( "zv8.rom",      0x7000, 0x1000, CRC(b87a286a) SHA1(0da5c126908fb5782772957814395d203c8a8d58) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "ic22.7",       0x0000, 0x0800, NO_DUMP )
	ROM_LOAD( "ic23.6",       0x0800, 0x0800, NO_DUMP )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "ic9.9",        0x0000, 0x1000, NO_DUMP )
	ROM_LOAD( "ic8.8",        0x1000, 0x1000, NO_DUMP )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "zvprom.rom",   0x0000, 0x0020, CRC(364e5700) SHA1(d47f7acf2bbb348dec3e26528d6c56f962e08c09) )
ROM_END

ROM_START( dockman )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "pe1.19",          0x0000, 0x1000, CRC(eef2ec54) SHA1(509e39141fc2dbb707873d4f88dca3510f66e829) )
	ROM_LOAD( "pe2.18",          0x1000, 0x1000, CRC(bc48d16b) SHA1(0e0cb8ab47cbd06371d15e5ac5d7b5a5a3bd3af0) )
	ROM_LOAD( "pe3.17",          0x2000, 0x1000, CRC(1c923057) SHA1(031c6aff47f2337ddc10e74d3de80105e854258d) )
	ROM_LOAD( "pe4.16",          0x3000, 0x1000, CRC(23af1cba) SHA1(4149367cc5198f4c38fe9665db7aed070cb8f95f) )
	ROM_LOAD( "pe5.15",          0x4000, 0x1000, CRC(39dbe429) SHA1(c98f4b82517f7125a68e59c7a403dfb6c630cc79) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "pe7.22",          0x0000, 0x0800, CRC(d2094e4a) SHA1(57c12555e36017e217c5d4e12d0da1ef1990bc3c) )
	ROM_LOAD( "pe6.23",          0x0800, 0x0800, CRC(1cf447f4) SHA1(d06e31805e13c868faed32358e2158e9ad18baf4) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "pe8.9",          0x0000, 0x1000, CRC(4d8c2974) SHA1(417b8af3011ff1c4c92d680814cd8f0d902f2b1e) )
	ROM_LOAD( "pe9.8",          0x1000, 0x1000, CRC(4e4ea162) SHA1(42ad2c82ce6a6eaae52efb75607552ca98e72a2a) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "mb7051.3",          0x0000, 0x0020, CRC(6440dc61) SHA1(cf0e794626ad7d9d58095485b782f007436fd446) )
ROM_END

ROM_START( portman )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "pe1",          0x0000, 0x1000, CRC(a5cf6083) SHA1(0daa5ff2931c56241fdeb4c48511b9508440554f) )
	ROM_LOAD( "pe2",          0x1000, 0x1000, CRC(0b53d48a) SHA1(2be59e0c40e6c60dce6b45bff325fdbbe0bec069) )
	ROM_LOAD( "pe3.17",       0x2000, 0x1000, CRC(1c923057) SHA1(031c6aff47f2337ddc10e74d3de80105e854258d) )
	ROM_LOAD( "pe4",          0x3000, 0x1000, CRC(555c71ef) SHA1(7c99e08b9c253744d73ed908fcb1cb047a687f7a) )
	ROM_LOAD( "pe5",          0x4000, 0x1000, CRC(f749e2d4) SHA1(2470cee59555a37f94ea7502e29ae92edca6c473) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "pe7.22",          0x0000, 0x0800, CRC(d2094e4a) SHA1(57c12555e36017e217c5d4e12d0da1ef1990bc3c) )
	ROM_LOAD( "pe6.23",          0x0800, 0x0800, CRC(1cf447f4) SHA1(d06e31805e13c868faed32358e2158e9ad18baf4) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "pe8.9",          0x0000, 0x1000, CRC(4d8c2974) SHA1(417b8af3011ff1c4c92d680814cd8f0d902f2b1e) )
	ROM_LOAD( "pe9.8",          0x1000, 0x1000, CRC(4e4ea162) SHA1(42ad2c82ce6a6eaae52efb75607552ca98e72a2a) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "mb7051.3",          0x0000, 0x0020, CRC(6440dc61) SHA1(cf0e794626ad7d9d58095485b782f007436fd446) )
ROM_END

ROM_START( funnymou )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	    /* 64k for main CPU */
	ROM_LOAD( "suprmous.x1",  0x0000, 0x1000, CRC(ad72b467) SHA1(98c79424bc98f2f1af79a04dabdd3985a71d761c) )
	ROM_LOAD( "suprmous.x2",  0x1000, 0x1000, CRC(53f5be5e) SHA1(9ed0a04fb19f93336fa3a9882c6842062d841201) )
	ROM_LOAD( "suprmous.x3",  0x2000, 0x1000, CRC(b5b8d34d) SHA1(e0edcdb7f070061f6f86991e22c0ea0808d4fbe4) )
	ROM_LOAD( "suprmous.x4",  0x3000, 0x1000, CRC(603333df) SHA1(04723fbd912e3d8fabf88643742c3553f4bb603b) )
	ROM_LOAD( "suprmous.x5",  0x4000, 0x1000, CRC(2ef9cbf1) SHA1(02323499ddcf4dcbbe432e2dbf5d305e5f9e15ad) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	   /* 64k for audio CPU */
	ROM_LOAD( "sm.6",         0x0000, 0x1000, CRC(fba71785) SHA1(56537a64a1e6cffedb8a6bd77e3edfa8aca94822) )

	ROM_REGION( 0x3000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "suprmous.x8",  0x0000, 0x1000, CRC(dbef9db8) SHA1(2bb070603f79e4acb7821cfa61ea1b4aed6d8e1f) )
	ROM_LOAD( "suprmous.x9",  0x1000, 0x1000, CRC(700d996e) SHA1(31884ec80b5eb70dc8e96712b5541754997b0ca8) )
	ROM_LOAD( "suprmous.x7",  0x2000, 0x1000, CRC(e9295071) SHA1(6034b7bc86bf070464af82bf1b9a55da81e864d9) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "smouse2.clr",  0x0000, 0x0020, CRC(8c295553) SHA1(7b43a4f023a163c233f6d9cf13fa4beee95d19d6) )
	ROM_LOAD( "smouse1.clr",  0x0020, 0x0020, CRC(d815504b) SHA1(5d11a650a885c7035e303c6758702baa8f0e7615) )
ROM_END

ROM_START( suprmous )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	    /* 64k for main CPU */
	ROM_LOAD( "sm.1",         0x0000, 0x1000, CRC(9db2b786) SHA1(ece6c267e45e0bfd430b94539737a7f8498273ea) )
	ROM_LOAD( "sm.2",         0x1000, 0x1000, CRC(0a3d91d3) SHA1(be32e49a6002e91b4d49b568d7c2b78fb10df2be) )
	ROM_LOAD( "sm.3",         0x2000, 0x1000, CRC(32af6285) SHA1(1f904614e6f16333e9bc1721bd6042509e8e87f3) )
	ROM_LOAD( "sm.4",         0x3000, 0x1000, CRC(46091524) SHA1(9ce05d80df276f227a852bfd3d3cfe26f4f2da85) )
	ROM_LOAD( "sm.5",         0x4000, 0x1000, CRC(f15fd5d2) SHA1(ce05893aad23d634956ef2c0ae62b5be823fd229) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	   /* 64k for audio CPU */
	ROM_LOAD( "sm.6",         0x0000, 0x1000, CRC(fba71785) SHA1(56537a64a1e6cffedb8a6bd77e3edfa8aca94822) )

	ROM_REGION( 0x3000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "sm.8",         0x0000, 0x1000, CRC(2f81ab5f) SHA1(9106255f37398c9d0c7cdc69b13765f5e4daa3bc) )
	ROM_LOAD( "sm.9",         0x1000, 0x1000, CRC(8463af89) SHA1(d29a2a30727d9bdb21b900c8543541cef49127dc) )
	ROM_LOAD( "sm.7",         0x2000, 0x1000, CRC(1d476696) SHA1(4ecb06297a29e279e31b9dd3a46642578a893c0b) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "smouse2.clr",  0x0000, 0x0020, CRC(8c295553) SHA1(7b43a4f023a163c233f6d9cf13fa4beee95d19d6) )
	ROM_LOAD( "smouse1.clr",  0x0020, 0x0020, CRC(d815504b) SHA1(5d11a650a885c7035e303c6758702baa8f0e7615) )
ROM_END

ROM_START( machomou )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	    /* 64k for main CPU */
	ROM_LOAD( "mm1.2g",       0x0000, 0x1000, CRC(91f116be) SHA1(3e838c009ad8f29703d7b72ece18be7b81dfcf4e) )
	ROM_LOAD( "mm2.2h",       0x1000, 0x1000, CRC(3aa88c9b) SHA1(6f626bf94012ab2366612cafacec3142722e72ee) )
	ROM_LOAD( "mm3.2i",       0x2000, 0x1000, CRC(3b66b519) SHA1(e5819fe2c9503b077ae7bb12bed9382a6cf82174) )
	ROM_LOAD( "mm4.2j",       0x3000, 0x1000, CRC(d4f99896) SHA1(a11e74701a46d7a59d91ee21c05ac743e03316a8) )
	ROM_LOAD( "mm5.3f",       0x4000, 0x1000, CRC(5bfc3874) SHA1(7ba38104980eabc0a30b0b4ac66b26e0b9834e37) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	   /* 64k for audio CPU */
	ROM_LOAD( "mm6.e6",       0x0000, 0x1000, CRC(20816913) SHA1(aed524b54d6ed802f3dd0170b3d9943e2d71b546) )

	ROM_REGION( 0x3000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "mm8.3c",       0x0000, 0x1000, CRC(062e77cb) SHA1(5fcb509af611d163a2a5c4908959ca6d5df49b37) )
	ROM_LOAD( "mm9.3a",       0x1000, 0x1000, CRC(a2f0cfb3) SHA1(bfae294cfa2ec9e18141dcda029c4471077df76a) )
	ROM_LOAD( "mm7.3d",       0x2000, 0x1000, CRC(a6f60ed2) SHA1(7ce12a10546144ce529d41159b593f1bac9b900b) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "mmouse2.clr",  0x0000, 0x0020, NO_DUMP )
	ROM_LOAD( "mmouse1.clr",  0x0020, 0x0020, NO_DUMP )
ROM_END

ROM_START( rtriv )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "rtriv-e.p1",   0x0000, 0x1000, CRC(f3c74f58) SHA1(015715689e25864a38f21075e30491590741c100) )
	ROM_LOAD( "rtriv-e.p2",   0x1000, 0x1000, CRC(d6ba213f) SHA1(cca42b87692620661d120b8b02d2be83268b0e38) )
	ROM_LOAD( "rtriv-e.p3",   0x2000, 0x1000, CRC(b8cf20cd) SHA1(23ffcc27cd19b4e9c1d30bcd318f11a9d6278a08) )
	ROM_LOAD( "rtriv-i.fc1",  0x3000, 0x1000, CRC(be5dca69) SHA1(9844f4a3d500df41a980f90d7450285740709b93) )
	/* 0x4000 - 0x4fff question rom space */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for audio CPU */
	ROM_LOAD( "ngames7.22",   0x0000, 0x0800, CRC(871e5a03) SHA1(d2105a8ae1829d493e85bcbbcd152a28f68eb035) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars and sprites */
	ROM_LOAD( "ngames8.8",    0x1000, 0x1000, BAD_DUMP CRC(f7644e1d) SHA1(d58d0d5739906b602f4c08a2fb9a16c32fcc245b) )
	ROM_LOAD( "ngames9.9",    0x0000, 0x1000, CRC(db553afc) SHA1(e7561ca0b2a4543c41bf41c96d17784b299ab367) )
	ROM_RELOAD(				  0x1000, 0x1000 ) // reload it until the other rom is re-dumped

	ROM_REGION( 0x40000, REGION_USER1, 0 ) /* Question roms */
	ROM_LOAD( "rtriv-1f.d0",  0x00000, 0x8000, CRC(84787af0) SHA1(5c1c74128af2b2d62ae9ba730da500e818b3dbd8) )
	ROM_LOAD( "rtriv-1f.d1",  0x08000, 0x8000, CRC(ff718059) SHA1(c2620b9116e42c56bf8c9260453f34ce19052601) )
	ROM_LOAD( "rtriv-1f.l0",  0x10000, 0x8000, CRC(ea43fdea) SHA1(fba341f11649891df4bed20dafda3b4a84756679) )
	ROM_LOAD( "rtriv-1f.l1",  0x18000, 0x8000, CRC(6e15f4e2) SHA1(355c208f32d87361d4c89287db3e2fb403fc2bfd) )
	ROM_LOAD( "rtriv-1f.l2",  0x20000, 0x8000, CRC(ecb9fc3c) SHA1(ed9b7768956a30af50058425a67ae44822563c6b) )
	ROM_LOAD( "rtriv-1f.t0",  0x28000, 0x8000, CRC(ca82b8a6) SHA1(a61026b242561996668adef2876c92e0fc06ac26) )
	ROM_LOAD( "rtriv-1f.t1",  0x30000, 0x8000, CRC(56c24a9c) SHA1(df773fedeff14fc5bbd29f9d06a9f6c62259bcb3) )
	ROM_LOAD( "rtriv-1f.t2",  0x38000, 0x8000, CRC(e62917cf) SHA1(e9e11e3f47f3278f13773b9b556dd122ba735c0b) )

	ROM_REGION( 0x0020, REGION_PROMS, 0 )
	ROM_LOAD( "rtriv.ic3",    0x0000, 0x0020, CRC(927ff40a) SHA1(3d699d981851989e9190505b0dede5202d688f2b) )
ROM_END

/*
    Romar Triv questions read handler
*/

static int question_address = 0;
static int question_rom = 0;
static int remap_address[16];

static READ8_HANDLER( rtriv_question_r )
{
	// Set-up the remap table for every 16 bytes
	if((offset & 0xc00) == 0x800)
	{
		remap_address[offset & 0x0f] = ((offset & 0xf0) >> 4) ^ 0x0f;
	}
	// Select which rom to read and the high 5 bits of address
	else if((offset & 0xc00) == 0x400)
	{
		question_rom = (offset & 0x70) >> 4;
		question_address = ((offset & 0x80) << 3) | ((offset & 0x0f) << 11);
	}
	// Read the actual byte from question roms
	else if((offset & 0xc00) == 0xc00)
	{
		UINT8 *ROM = memory_region(REGION_USER1);
		int real_address;

		real_address = (0x8000 * question_rom) | question_address | (offset & 0x3f0) | remap_address[offset & 0x0f];

		return ROM[real_address];
	}

	return 0; // the value read from the configuration reads is discarded
}

DRIVER_INIT( rtriv )
{
	// Set-up the weirdest questions read ever done
	memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4000, 0x4fff, 0, 0, rtriv_question_r);
}


GAME( 1981, roundup,  0,        thepit,   roundup,  0,     ROT90, "Amenip/Centuri", "Round-Up", 0 ,2)
GAME( 1981, fitter,   roundup,  thepit,   fitter,   0,     ROT90, "Taito Corporation", "Fitter", 0 ,2)
GAME( 1982, thepit,   0,        thepit,   thepit,   0,     ROT90, "Taito", "The Pit", 0 ,2)
GAME( 1982, thepitc,  thepit,   thepit,   thepit,   0,     ROT90, "Centuri", "The Pit (Centuri)", 0 ,2)
GAME( 1982, thepitb,  thepit,   thepit,   thepit,   0,     ROT90, "Bootleg", "The Pit (Bootleg)", 0 ,2) /* based off the Centuri version */
GAME( 1982, dockman,  0,        intrepid, dockman,  0,     ROT90, "Taito Corporation", "Dock Man", 0 ,0)
GAME( 1982, portman,  dockman,  intrepid, dockman,  0,     ROT90, "Nova Games Ltd.", "Port Man", 0 ,0)
GAME( 1982, funnymou, 0,        suprmous, suprmous, 0,     ROT90, "Chuo Co. Ltd", "Funny Mouse", 0 ,0)
GAME( 1982, suprmous, funnymou, suprmous, suprmous, 0,     ROT90, "Taito Corporation", "Super Mouse", 0 ,0)
GAME( 1982, machomou, 0,        suprmous, suprmous, 0,     ROT90, "Techstar", "Macho Mouse", GAME_WRONG_COLORS ,0)
GAME( 1983, intrepid, 0,        intrepid, intrepid, 0,     ROT90, "Nova Games Ltd.", "Intrepid (set 1)", 0 ,0)
GAME( 1983, intrepi2, intrepid, intrepid, intrepid, 0,     ROT90, "Nova Games Ltd.", "Intrepid (set 2)", 0 ,0)
GAME( 1984, zaryavos, 0,        intrepid, intrepid, 0,     ROT90, "Nova Games of Canada", "Zarya Vostoka", GAME_NOT_WORKING ,0)
GAME( 198?, rtriv,    0,        intrepid, rtriv,    rtriv, ROT90, "Romar", "Romar Triv", GAME_WRONG_COLORS ,0)
