/***************************************************************************

    Atari Football hardware

    driver by Mike Balfour, Patrick Lawrence, Brad Oliver

    Games supported:
        * Atari Football
        * Atari Baseball
        * Atari Soccer

    Known issues:
        * The down marker sprite is multiplexed so that it will be drawn at the
          top and bottom of the screen. We fake this feature. Additionally, we
          draw it at a different location which seems to make more sense.

        * The play which is chosen is drawn in text at the top of the screen;
          no backdrop/overlay is supported yet. High quality artwork would be
          appreciated.

        * I'm not good at reading the schematics, so I'm unsure about the
          exact vblank duration. I'm pretty sure it is one of two values though.

        * The 4-player variation is slightly broken. I'm unsure of the
          LED multiplexing.

****************************************************************************

    Memory Map:
        0000-01FF   Working RAM
        0200-025F   Playfield - Player 1
        03A0-03FF   Playfield - Player 2
        1000-13BF   Scrollfield
        13C0-13FF   Motion Object Parameters:

        13C0        Motion Object 1 Picture #
        13C1        Motion Object 1 Vertical Position
        13C2        Motion Object 2 Picture #
        13C3        Motion Object 2 Vertical Position
        ...
        13DE        Motion Object 16 Picture #
        13DF        Motion Object 16 Vertical Position

        13E0        Motion Object 1 Horizontal Position
        13E1        Spare
        13E2        Motion Object 2 Horizontal Position
        13E3        Spare
        ...
        13FE        Motion Object 16 Horizontal Position
        13FF        Spare

        2000-2003   Output ports:

        2000        (OUT 0) Scrollfield Offset (8 bits)
        2001        (OUT 1)
                    D0 = Whistle
                    D1 = Hit
                    D2 = Kicker
                    D5 = CTRLD
        2002        (OUT 2)
                    D0-D3 = Noise Amplitude
                    D4 = Coin Counter
                    D5 = Attract
        2003        (OUT 3)
                    D0-D3 = LED Cathodes
                    D4-D5 Spare

        3000        Interrupt Acknowledge
        4000-4003   Input ports:

        4000        (IN 0) = 0
                    D0 = Trackball Direction PL2VD
                    D1 = Trackball Direction PL2HD
                    D2 = Trackball Direction PL1VD
                    D3 = Trackball Direction PL1HD
                    D4 = Select 1
                    D5 = Slam
                    D6 = End Screen
                    D7 = Coin 1
        4000        (CTRLD) = 1
                    D0-D3 = Track-ball Horiz. 1
                    D4-D7 = Track-ball Vert. 1
        4002        (IN 2) = 0
                    D0-D3 = Option Switches
                    D4 = Select 2
                    D5 = Spare
                    D6 = Test
                    D7 = Coin 2
        4002        (CTRLD) = 1
                    D0-D3 = Track-ball Horiz. 2
                    D4-D7 = Track-ball Vert. 2

        5000        Watchdog
        6800-7FFF   Program
        (F800-FFFF) - only needed for the 6502 vectors

    If you have any questions about how this driver works, don't hesitate to
    ask.  - Mike Balfour (mab22@po.cwru.edu)

    Changes:
        LBO - lots of cleanup, now it's playable.

***************************************************************************/

#include "driver.h"
#include "atarifb.h"
#include "sound/discrete.h"


int atarifb_game;
int atarifb_lamp1, atarifb_lamp2;



/*************************************
 *
 *  Palette generation
 *
 *************************************/

static const unsigned short colortable_source[] =
{
	0x02, 0x00, /* chars */
	0x03, 0x02, /* sprites */
	0x03, 0x00,
	0x03, 0x01, /* sprite masks */
	0x03, 0x00,
	0x03, 0x02,
};

static PALETTE_INIT( atarifb )
{
	palette_set_color(0,0x00,0x00,0x00); /* black  */
	palette_set_color(1,0x80,0x80,0x80); /* grey  */
	palette_set_color(2,0xff,0xff,0xff); /* white  */
	palette_set_color(3,0x40,0x40,0x40); /* dark grey (?) - used in Soccer only */
	memcpy(colortable,colortable_source,sizeof(colortable_source));
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( atarifb_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(15) )
	AM_RANGE(0x0000, 0x01ff) AM_RAM
	AM_RANGE(0x0200, 0x025f) AM_WRITE(atarifb_alphap1_vram_w) AM_BASE(&atarifb_alphap1_vram) AM_SIZE(&atarifb_alphap1_vram_size)
	AM_RANGE(0x0260, 0x039f) AM_RAM
	AM_RANGE(0x03a0, 0x03ff) AM_WRITE(atarifb_alphap2_vram_w) AM_BASE(&atarifb_alphap2_vram) AM_SIZE(&atarifb_alphap2_vram_size)
	AM_RANGE(0x1000, 0x13bf) AM_READWRITE(MRA8_RAM, videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x13c0, 0x13ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(atarifb_scroll_w) AM_BASE(&atarifb_scroll_register) /* OUT 0 */
	AM_RANGE(0x2001, 0x2001) AM_WRITE(atarifb_out1_w) /* OUT 1 */
	AM_RANGE(0x2002, 0x2002) AM_WRITE(atarifb_out2_w) /* OUT 2 */
	AM_RANGE(0x2003, 0x2003) AM_WRITE(atarifb_out3_w) /* OUT 3 */
	AM_RANGE(0x3000, 0x3000) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x3000) AM_WRITE(MWA8_NOP) /* Interrupt Acknowledge */
	AM_RANGE(0x4000, 0x4000) AM_READ(atarifb_in0_r)
	AM_RANGE(0x4002, 0x4002) AM_READ(atarifb_in2_r)
	AM_RANGE(0x5000, 0x5000) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x6000, 0x7fff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( soccer_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(14) )
	AM_RANGE(0x0000, 0x01ff) AM_RAM
	AM_RANGE(0x0200, 0x025f) AM_WRITE(atarifb_alphap1_vram_w) AM_BASE(&atarifb_alphap1_vram) AM_SIZE(&atarifb_alphap1_vram_size)
	AM_RANGE(0x0260, 0x039f) AM_RAM
	AM_RANGE(0x03a0, 0x03ff) AM_WRITE(atarifb_alphap2_vram_w) AM_BASE(&atarifb_alphap2_vram) AM_SIZE(&atarifb_alphap2_vram_size)
	AM_RANGE(0x0800, 0x0bbf) AM_READWRITE(MRA8_RAM, videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x0bc0, 0x0bff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x1000, 0x1000) AM_WRITE(atarifb_scroll_w) AM_BASE(&atarifb_scroll_register) /* OUT 0 */
	AM_RANGE(0x1001, 0x1001) AM_WRITE(atarifb_out1_w) /* OUT 1 */
	AM_RANGE(0x1002, 0x1002) AM_WRITE(atarifb_out2_w) /* OUT 2 */
	AM_RANGE(0x1004, 0x1004) AM_WRITE(MWA8_NOP) /* Interrupt Acknowledge */
	AM_RANGE(0x1005, 0x1005) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x1800, 0x1800) AM_READ(atarifb4_in0_r)
	AM_RANGE(0x1801, 0x1801) AM_READ(input_port_1_r)
	AM_RANGE(0x1802, 0x1802) AM_READ(atarifb4_in2_r)
	AM_RANGE(0x1803, 0x1803) AM_READ(input_port_11_r)
	AM_RANGE(0x2000, 0x3fff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( atarifb )
	PORT_START_TAG("IN0")
	PORT_BIT ( 0x0F, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON1 )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_TILT )
	PORT_BIT ( 0x40, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_COIN1 )

	PORT_START_TAG("IN1")
	PORT_DIPNAME( 0x03, 0x00, "Time per coin" )
	PORT_DIPSETTING(	0x00, "1:30" )
	PORT_DIPSETTING(	0x01, "2:00" )
	PORT_DIPSETTING(	0x02, "2:30" )
	PORT_DIPSETTING(	0x03, "3:00" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Atari logo" )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START_TAG("IN2")	/* IN2 - Player 1 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN3")	/* IN3 - Player 1 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN4")	/* IN4 - Player 2 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(2)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN5")	/* IN5 - Player 2 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)
	/* The lower 4 bits are the input */
INPUT_PORTS_END


INPUT_PORTS_START( atarifb4 )
	PORT_START_TAG("IN0")
	PORT_BIT ( 0xff, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START_TAG("IN1")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT ( 0x38, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT ( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )

	PORT_START_TAG("IN2")
	PORT_DIPNAME( 0x03, 0x00, "Time per coin" )
	PORT_DIPSETTING(	0x00, "1:30" )
	PORT_DIPSETTING(	0x01, "2:00" )
	PORT_DIPSETTING(	0x02, "2:30" )
	PORT_DIPSETTING(	0x03, "3:00" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Atari logo" )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 )

	PORT_START_TAG("IN3")	/* IN3 - Player 1 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN4")	/* IN4 - Player 1 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN5")	/* IN5 - Player 2 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(2)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN6")	/* IN6 - Player 2 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN7")	/* IN7 - Player 3 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(3)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN8")	/* IN8 - Player 3 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(3)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN9")	/* IN9 - Player 4 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(4)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN10")	/* IN10 - Player 4 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(4)
	/* The lower 4 bits are the input */
INPUT_PORTS_END


INPUT_PORTS_START( abaseb )
	PORT_START_TAG("IN0")
	PORT_BIT ( 0x0F, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON1 )
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_TILT )
	PORT_BIT ( 0x40, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_COIN1 )

	PORT_START_TAG("IN1")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Hardest ) )
	PORT_DIPSETTING(	0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(	0x02, "Fair" )
	PORT_DIPSETTING(	0x03, DEF_STR( Easy ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x08, DEF_STR( On ) )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START_TAG("IN2")	/* IN2 - Player 1 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN3")	/* IN3 - Player 1 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN4")	/* IN4 - Player 2 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(2)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN5")	/* IN5 - Player 2 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)
	/* The lower 4 bits are the input */
INPUT_PORTS_END


INPUT_PORTS_START( soccer )
	PORT_START_TAG("IN0")
	PORT_BIT ( 0xff, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START_TAG("IN1")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_UNKNOWN ) /* unused on schematics */
	PORT_BIT ( 0x02, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN ) /* unused on schematics */
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_TILT )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT ( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )

	PORT_START_TAG("IN2")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("2/4 Player Toggle")
	PORT_DIPNAME( 0x02, 0x00, "Rule Switch" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Language ) )
	PORT_DIPSETTING(	0x00, DEF_STR( English ) )
	PORT_DIPSETTING(	0x04, DEF_STR( German ) )
	PORT_DIPSETTING(	0x08, DEF_STR( French ) )
	PORT_DIPSETTING(	0x0c, DEF_STR( Spanish ) )
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)

	PORT_START_TAG("IN3")	/* IN3 - Player 1 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN4")	/* IN4 - Player 1 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN5")	/* IN5 - Player 2 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(2)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN6")	/* IN6 - Player 2 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN7")	/* IN7 - Player 3 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(3)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN8")	/* IN8 - Player 3 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(3)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN9")	/* IN9 - Player 4 trackball, y */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(4)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN10")	/* IN10 - Player 4 trackball, x */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(4)
	/* The lower 4 bits are the input */

	PORT_START_TAG("IN11")
	PORT_DIPNAME( 0x07, 0x00, "Time per coin" )
	PORT_DIPSETTING(	0x00, "1:00" )
	PORT_DIPSETTING(	0x01, "1:20" )
	PORT_DIPSETTING(	0x02, "1:40" )
	PORT_DIPSETTING(	0x03, "2:00" )
	PORT_DIPSETTING(	0x04, "2:30" )
	PORT_DIPSETTING(	0x05, "3:00" )
	PORT_DIPSETTING(	0x06, "3:30" )
	PORT_DIPSETTING(	0x07, "4:00" )
	PORT_DIPNAME( 0x18, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x08, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(	0x18, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(	0x00, "1 Coin Minimum" )
	PORT_DIPSETTING(	0x40, "2 Coin Minimum" )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN ) /* unused on schematics */
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,
	64,
	1,
	{ 0 },
	{ 15, 14, 13, 12, 7, 6, 5, 4 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};


static const gfx_layout fieldlayout =
{
	8,8,
	64,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static const gfx_layout soccer_fieldlayout =
{
	8,8,
	64,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8
};


static const gfx_layout spritelayout =
{
	8,16,
	64,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*8
};


static const gfx_layout spritemasklayout =
{
	8,16,
	64,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*8
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,  0x00, 0x01 }, /* offset into colors, # of colors */
	{ REGION_GFX2, 0, &fieldlayout, 0x02, 0x01 }, /* offset into colors, # of colors */
	{ -1 } /* end of array */
};


static const gfx_decode soccer_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,         0x00, 0x01 }, /* offset into colors, # of colors */
	{ REGION_GFX3, 0x0400, &soccer_fieldlayout, 0x06, 0x01 }, /* offset into colors, # of colors */
	{ REGION_GFX2, 0x0000, &spritelayout,       0x02, 0x02 }, /* offset into colors, # of colors */
	{ REGION_GFX3, 0x0000, &spritemasklayout,   0x06, 0x03 }, /* offset into colors, # of colors */
	{ -1 } /* end of array */
};



/*************************************
 *
 *  Machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( atarifb )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M6502, 750000)
	MDRV_CPU_PROGRAM_MAP(atarifb_map,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(2037)	/* 16.3ms * 1/8 = 2037.5. Is it 1/8th or 3/32nds? (1528?) */

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(38*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 38*8-1, 0*8, 31*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4)
	MDRV_COLORTABLE_LENGTH(sizeof(colortable_source) / sizeof(colortable_source[0]))

	MDRV_PALETTE_INIT(atarifb)
	MDRV_VIDEO_START(atarifb)
	MDRV_VIDEO_UPDATE(atarifb)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD_TAG("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(atarifb_discrete_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( atarifb4 )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(atarifb)

	/* video hardware */
	MDRV_VISIBLE_AREA(0*8, 38*8-1, 0*8, 32*8-1)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( abaseb )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(atarifb)

	/* sound hardware */
	MDRV_SOUND_REPLACE("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(abaseb_discrete_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( soccer )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(atarifb)
	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(soccer_map,0)

	/* video hardware */
	MDRV_VISIBLE_AREA(0*8, 38*8-1, 2*8, 32*8-1)
	MDRV_GFXDECODE(soccer_gfxdecodeinfo)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( atarifb )
	ROM_REGION( 0x8000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "03302602.m1", 0x6800, 0x0800, CRC(352e35db) SHA1(ae3f1bdb274858edf203dbffe4ba2912c065cff2) )
	ROM_LOAD( "03302801.p1", 0x7000, 0x0800, CRC(a79c79ca) SHA1(7791b431e9aadb09fd286ae56699c4beda54830a) )
	ROM_LOAD( "03302702.n1", 0x7800, 0x0800, CRC(e7e916ae) SHA1(d3a188809e83c311699cb103040c4525b36a56e3) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "033029.n7", 0x0000, 0x0400, CRC(12f43dca) SHA1(a463f5068d5522ddf74052429aa6da23e5475844) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "033030.c5", 0x0000, 0x0200, CRC(eac9ef90) SHA1(0e6284392852695ab7323be82105d32f57ad00f1) )
	ROM_LOAD_NIB_HIGH( "033031.d5", 0x0000, 0x0200, CRC(89d619b8) SHA1(0af5d1f4e6f9a377dc2d49a8039866b1857af01f) )
ROM_END


ROM_START( atarifb1 )
	ROM_REGION( 0x8000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "03302601.m1", 0x6800, 0x0800, CRC(f8ce7ed8) SHA1(54520d7d31c6c8f9028b7253a33aba3b2c35ae7c) )
	ROM_LOAD( "03302801.p1", 0x7000, 0x0800, CRC(a79c79ca) SHA1(7791b431e9aadb09fd286ae56699c4beda54830a) )
	ROM_LOAD( "03302701.n1", 0x7800, 0x0800, CRC(7740be51) SHA1(3f610061f081eb5589b00a496877bc58f6e0f09f) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "033029.n7", 0x0000, 0x0400, CRC(12f43dca) SHA1(a463f5068d5522ddf74052429aa6da23e5475844) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "033030.c5", 0x0000, 0x0200, CRC(eac9ef90) SHA1(0e6284392852695ab7323be82105d32f57ad00f1) )
	ROM_LOAD_NIB_HIGH( "033031.d5", 0x0000, 0x0200, CRC(89d619b8) SHA1(0af5d1f4e6f9a377dc2d49a8039866b1857af01f) )
ROM_END


ROM_START( atarifb4 )
	ROM_REGION( 0x8000, REGION_CPU1, 0 ) /* 64k for code, the ROMs are nibble-wide */
	ROM_LOAD_NIB_LOW ( "34889.m1", 0x6000, 0x0400, CRC(5c63974a) SHA1(e91f318be80d985a09ff92f4db5792290a06dc0f) )
	ROM_LOAD_NIB_HIGH( "34891.m2", 0x6000, 0x0400, CRC(9d03baa1) SHA1(1b57f39fa4d43e3f3d22f2d9a5478b5f5e4d0cb1) )
	ROM_LOAD_NIB_LOW ( "34890.n1", 0x6400, 0x0400, CRC(2deb5844) SHA1(abc7cc80d5fcac13f50f6cc550ea7a8f322434c9) )
	ROM_LOAD_NIB_HIGH( "34892.n2", 0x6400, 0x0400, CRC(ad212d2d) SHA1(df77ed3d59b497d0f4fe7b275f1cce6c4a5aa0b2) )
	ROM_LOAD_NIB_LOW ( "34885.k1", 0x6800, 0x0400, CRC(fdd272a1) SHA1(619c7b1ced1e397a4aa5fcaf0afe84c2b39ba5fd) )
	ROM_LOAD_NIB_HIGH( "34887.k2", 0x6800, 0x0400, CRC(fa2b8b52) SHA1(aff26efcf70fe63819a80977853e8f58c17cb32b) )
	ROM_LOAD_NIB_LOW ( "34886.l1", 0x6c00, 0x0400, CRC(be912ccb) SHA1(6ed05d011a1fe06831883fdbdf7153b0ec624de9) )
	ROM_LOAD_NIB_HIGH( "34888.l2", 0x6c00, 0x0400, CRC(3f8e96c1) SHA1(c188eb39a00943d9eb62b8a70ad3bd108fc768e9) )
	ROM_LOAD_NIB_LOW ( "34877.e1", 0x7000, 0x0400, CRC(fd8832fa) SHA1(83f874d5c178846bdfb7609c2738c03e3369743b) )
	ROM_LOAD_NIB_HIGH( "34879.e2", 0x7000, 0x0400, CRC(7053ffbc) SHA1(cec5efb005833da448f67b9811719099d6980dcd) )
	ROM_LOAD_NIB_LOW ( "34878.f1", 0x7400, 0x0400, CRC(329eb720) SHA1(fa9e8c25c9e20fea72d1314297b77ffe599a5a74) )
	ROM_LOAD_NIB_HIGH( "34880.f2", 0x7400, 0x0400, CRC(e0c9b4c2) SHA1(1cc0900bb62c672a870fc465f5691039bb487571) )
	ROM_LOAD_NIB_LOW ( "34881.h1", 0x7800, 0x0400, CRC(d9055541) SHA1(ffbf86c5cc325587d89e17da0560518244d3d8e9) )
	ROM_LOAD_NIB_HIGH( "34883.h2", 0x7800, 0x0400, CRC(8a912448) SHA1(1756874964eedb75e066a4d6dccecf16a652f6bb) )
	ROM_LOAD_NIB_LOW ( "34882.j1", 0x7c00, 0x0400, CRC(060c9cdb) SHA1(3c6d04c535195dfa8f8405ff8e80f4693844d1a1) )
	ROM_LOAD_NIB_HIGH( "34884.j2", 0x7c00, 0x0400, CRC(aa699a3a) SHA1(2c13eb9cda3fe9cfd348ef5cf309625f77c75056) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "033029.n7", 0x0000, 0x0400, CRC(12f43dca) SHA1(a463f5068d5522ddf74052429aa6da23e5475844) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "033030.c5", 0x0000, 0x0200, CRC(eac9ef90) SHA1(0e6284392852695ab7323be82105d32f57ad00f1) )
	ROM_LOAD_NIB_HIGH( "033031.d5", 0x0000, 0x0200, CRC(89d619b8) SHA1(0af5d1f4e6f9a377dc2d49a8039866b1857af01f) )
ROM_END


ROM_START( abaseb )
	ROM_REGION( 0x8000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "34738-01.n0", 0x6000, 0x0800, CRC(edcfffe8) SHA1(a445668352da5039ed1a090bcdf2ce092215f165) )
	ROM_LOAD( "34737-03.m1", 0x6800, 0x0800, CRC(7250863f) SHA1(83ec735a60d74ca9c3e3f5d4b248071f3e3330af) )
	ROM_LOAD( "34735-01.p1", 0x7000, 0x0800, CRC(54854d7c) SHA1(536d57b00929bf9d1cd1b209b41004cb78e2cd93) )
	ROM_LOAD( "34736-01.n1", 0x7800, 0x0800, CRC(af444eb0) SHA1(783293426cec6938a2cd9c66c491f073cfb2683f) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "034710.d5", 0x0000, 0x0400, CRC(31275d86) SHA1(465ff2032e62bcd5a7bb5c947212da4ea4d59353) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "034708.n7", 0x0000, 0x0200, CRC(8a0f971b) SHA1(f7de50eeb15c8291f1560e299e3b1b29bba58422) )
	ROM_LOAD_NIB_HIGH( "034709.c5", 0x0000, 0x0200, CRC(021d1067) SHA1(da0fa8e4f6c0240a4feb41312fa057c65d809e62) )
ROM_END


ROM_START( abaseb2 )
	ROM_REGION( 0x8000, REGION_CPU1, 0 ) /* 64k for code, the ROMs are nibble-wide */
	ROM_LOAD_NIB_LOW ( "034725.c0", 0x6000, 0x0400, CRC(95912c58) SHA1(cb15b60e31ee212e30a81c170611be1e36d2a6dd) )
	ROM_LOAD_NIB_HIGH( "034723.m0", 0x6000, 0x0400, CRC(5eb1597f) SHA1(78f83d4e79de13d3723732d68738660c3f8d4787) )
	ROM_LOAD_NIB_LOW ( "034726.b0", 0x6400, 0x0400, CRC(1f8d506c) SHA1(875464ca2ee50b36ceb5989cd40a28c69953c641) )
	ROM_LOAD_NIB_HIGH( "034724.l0", 0x6400, 0x0400, CRC(ecd18ed2) SHA1(6ffbc9a4108ebf190455fad3725b72dda4125ac7) )
	ROM_LOAD_NIB_LOW ( "034721.d1", 0x6800, 0x0400, CRC(1a0541f2) SHA1(ba74f024deb173678166262c4c6b1c328248aa9a) )
	ROM_LOAD_NIB_HIGH( "034715.j1", 0x6800, 0x0400, CRC(accb96f5) SHA1(1cd6603c818dacf4f71fc350ebd3adf3369056b2) ) /* created from 8-bit set */
	ROM_LOAD_NIB_LOW ( "034722.d0", 0x6c00, 0x0400, CRC(f9c1174e) SHA1(9d1be9ce4985edd19e0969d8998946d05fbbdf1f) ) /* The code in these 2 differs */
	ROM_LOAD_NIB_HIGH( "034716.j0", 0x6c00, 0x0400, CRC(d5622749) SHA1(6a48d428751939857be6869b44a61b8f054d4206) ) /* from the 8-bit set */
	ROM_LOAD_NIB_LOW ( "034717.e1", 0x7000, 0x0400, CRC(c941f64b) SHA1(e4d309c8ae71adc42dab0ffeea8f58da310c52f3) )
	ROM_LOAD_NIB_HIGH( "034711.k1", 0x7000, 0x0400, CRC(fab61782) SHA1(01b6de2822d09ebe0725307eeeaeb667f53ca8f1) )
	ROM_LOAD_NIB_LOW ( "034718.e0", 0x7400, 0x0400, CRC(3fe7dc1c) SHA1(91c3af7d8acdb5c4275f5fa57c19dc589f4a63aa) )
	ROM_LOAD_NIB_HIGH( "034712.k0", 0x7400, 0x0400, CRC(0e368e1a) SHA1(29bbe4be07d8d441a4251ed6fbfa9e225487c2d8) )
	ROM_LOAD_NIB_LOW ( "034719.h1", 0x7800, 0x0400, CRC(85046ee5) SHA1(2e8559349460a44734c95a1440a84713c5344495) )
	ROM_LOAD_NIB_HIGH( "034713.f1", 0x7800, 0x0400, CRC(0c67c48d) SHA1(eec24da32632c1ba00aee22f1b9abb144b38cc8a) )
	ROM_LOAD_NIB_LOW ( "034720.h0", 0x7c00, 0x0400, CRC(37c5f149) SHA1(89ad4471b949f8318abbdb38c4f373f711130198) )
	ROM_LOAD_NIB_HIGH( "034714.f0", 0x7c00, 0x0400, CRC(920979ea) SHA1(aba499376c084b8ceb6f0cc6599bd51cec133cc7) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "034710.d5", 0x0000, 0x0400, CRC(31275d86) SHA1(465ff2032e62bcd5a7bb5c947212da4ea4d59353) )

	ROM_REGION( 0x0200, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "034708.n7", 0x0000, 0x0200, CRC(8a0f971b) SHA1(f7de50eeb15c8291f1560e299e3b1b29bba58422) )
	ROM_LOAD_NIB_HIGH( "034709.c5", 0x0000, 0x0200, CRC(021d1067) SHA1(da0fa8e4f6c0240a4feb41312fa057c65d809e62) )
ROM_END


ROM_START( soccer )
	ROM_REGION( 0x4000, REGION_CPU1, 0 ) /* 64k for code, the ROMs are nibble-wide */
	ROM_LOAD_NIB_LOW ( "035222.e1", 0x2000, 0x0400, CRC(03ec6bce) SHA1(f81f2ac3bab5f1ae687543427e0187ca51d3be7e) )
	ROM_LOAD_NIB_HIGH( "035224.e2", 0x2000, 0x0400, CRC(a1aeaa70) SHA1(2018318a0e652b1dbea7696ef3dc2b7f12ebd632) )
	ROM_LOAD_NIB_LOW ( "035223.f1", 0x2400, 0x0400, CRC(9c600726) SHA1(f652b42b93e43124b0363b52f0f13cb9154987e3) )
	ROM_LOAD_NIB_HIGH( "035225.f2", 0x2400, 0x0400, CRC(2aa06521) SHA1(c03b02f62346a8e395f8c4b15f6f89fd96b790a4) )
	ROM_LOAD_NIB_LOW ( "035226.h1", 0x2800, 0x0400, CRC(d57c0cfb) SHA1(9ce05d9b30e8014137e20e4b0bbe414a3b9fa600) )
	ROM_LOAD_NIB_HIGH( "035228.h2", 0x2800, 0x0400, CRC(594574cb) SHA1(c8b42a44520e6a2a3e8831e9f9002c3c532f5fca) )
	ROM_LOAD_NIB_LOW ( "035227.j1", 0x2c00, 0x0400, CRC(4112b257) SHA1(997f4681a5cd4ca12977c52133e847afe61c58e1) )
	ROM_LOAD_NIB_HIGH( "035229.j2", 0x2c00, 0x0400, CRC(412d129c) SHA1(2680af645aa6935114e59c657e49b131e48661fc) )

	ROM_LOAD_NIB_LOW ( "035230.k1", 0x3000, 0x0400, CRC(747f6e4a) SHA1(b0cd8097e064ba6b0e22e97a7907bc287006aa8c) )
	ROM_LOAD_NIB_HIGH( "035232.k2", 0x3000, 0x0400, CRC(55f43e7f) SHA1(db44f658a521f859f11f9a638ba19e84bbb75d2d) )
	ROM_LOAD_NIB_LOW ( "035231.l1", 0x3400, 0x0400, CRC(d584c199) SHA1(55e86e4f1737bf02d5706f1e757d9c97007549ac) )
	ROM_LOAD_NIB_HIGH( "035233.l2", 0x3400, 0x0400, CRC(b343f500) SHA1(d15413759563bec2bc8f3fa28ae84e4ae902910b) )
	ROM_LOAD_NIB_LOW ( "035234.m1", 0x3800, 0x0400, CRC(83524bb7) SHA1(d45233b666463f789257c7366c3dfb4d9b55f87e) )
	ROM_LOAD_NIB_HIGH( "035236.m2", 0x3800, 0x0400, CRC(c53f4d13) SHA1(ebba48e50c98e7f74d19826cf559cf6633e24f3b) )
	ROM_LOAD_NIB_LOW ( "035235.n1", 0x3c00, 0x0400, CRC(d6855b0e) SHA1(379d010ebebde6f1b5fec5519a3c0aa4380be28b) )
	ROM_LOAD_NIB_HIGH( "035237.n2", 0x3c00, 0x0400, CRC(1d01b054) SHA1(7f3dc1130b2aadb13813e223420672c5baf25ad8) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "035250.r2", 0x0000, 0x0400, CRC(12f43dca) SHA1(a463f5068d5522ddf74052429aa6da23e5475844) ) /* characters */

	ROM_REGION( 0x0800, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_LOW ( "035247.n7", 0x0000, 0x0400, CRC(3adb5f4e) SHA1(859df5dc97b06e0c06e4f71a511313aef1f08d87) ) /* sprites */
	ROM_LOAD_NIB_HIGH( "035248.m7", 0x0000, 0x0400, CRC(a890cd48) SHA1(34f52bc4b610491d3b81caae25ec3cafbc429373) )

	ROM_REGION( 0x0800, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "035246.r6", 0x0000, 0x0800, CRC(4a996136) SHA1(535b6d5f70ab5bc2a47263a1c16877ba4c82b3ff) ) /* spritemask - playfield */

	/* ROM_LOAD_NIB_LOW ( "35242-01.r7", 0x0000, 0x0400, CRC(d783e3d3) SHA1(1753b907735ca75c4622e003ec0df5f32e886783) ) */
	/* ROM_LOAD_NIB_HIGH( "35243-01.r8", 0x0000, 0x0400, CRC(b63ae27c) SHA1(6106765871832e9fbe440fca2d8df1b6700d8b85) ) */
	/* ROM_LOAD_NIB_LOW ( "35244-01.p7", 0x0400, 0x0400, CRC(5236ae38) SHA1(472b51635a050ea1913d745fcfc07f30ce9afa09) ) */
	/* ROM_LOAD_NIB_HIGH( "35245-01.p8", 0x0400, 0x0400, CRC(e4c6fd5c) SHA1(62c80735726c4e11229438c27153203105b9cb66) ) */

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "34006-01.n4", 0x0000, 0x0100, CRC(0da9918e) SHA1(405ee4396a20d05ecde27388093139d087d81f19) ) /* sync (not used) */
ROM_END


/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( atarifb )
{
	/* Tell the video code to draw the plays for this version */
	atarifb_game = 1;
}


static DRIVER_INIT( atarifb4 )
{
	/* Tell the video code to draw the plays for this version */
	atarifb_game = 2;

	memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4000, 0x4000, 0, 0, atarifb4_in0_r);
	memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4001, 0x4001, 0, 0, input_port_1_r);
	memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4002, 0x4002, 0, 0, atarifb4_in2_r);
}


static DRIVER_INIT( abaseb )
{
	/* Tell the video code to draw the plays for this version */
	atarifb_game = 3;
}


static DRIVER_INIT( soccer )
{
	/* Tell the video code to draw the plays for this version */
	atarifb_game = 4;
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

/*    YEAR  NAME      PARENT   MACHINE   INPUT     INIT      MONITOR  */
GAME( 1978, atarifb,  0,       atarifb,  atarifb,  atarifb,  ROT0, "Atari", "Atari Football (revision 2)", 0 ,2)
GAME( 1978, atarifb1, atarifb, atarifb,  atarifb,  atarifb,  ROT0, "Atari", "Atari Football (revision 1)", 0 ,2)
GAME( 1979, atarifb4, atarifb, atarifb4, atarifb4, atarifb4, ROT0, "Atari", "Atari Football (4 players)", 0 ,2)
GAME( 1979, abaseb,   0,       abaseb,   abaseb,   abaseb,   ROT0, "Atari", "Atari Baseball (set 1)", 0 ,2)
GAME( 1979, abaseb2,  abaseb,  abaseb,   abaseb,   abaseb,   ROT0, "Atari", "Atari Baseball (set 2)", 0 ,2)
GAME( 1980, soccer,   0,       soccer,   soccer,   soccer,   ROT0, "Atari", "Atari Soccer", 0 ,0)
