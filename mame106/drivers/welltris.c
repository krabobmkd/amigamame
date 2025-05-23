/*******************************************************************************
 Welltris (c)1991 Video System

********************************************************************************
 hardware is similar to aerofgt.c but with slightly different sprites, sound,
 and an additional 'pixel' layer used for the backdrops

 Driver by David Haywood, with help from Steph from The Ultimate Patchers
 Thanks to the authors of aerofgt.c and fromance.c on which most of this is
 based
********************************************************************************
OW-13 CPU

CPU  : MC68000P10
Sound: Z80 YM2610 YM3016
OSC  : 14.31818MHz (X1), 12.000MHz (X2),
       8.000MHz (X3), 20.0000MHz (OSC1)

ROMs:
j1.7 - Main programs (271000 compatible onetime)
j2.8 /

lh532j11.9  - Data
lh532j10.10 /

3.144 - Sound program (27c1000)

lh534j09.123 - Samples
lh534j10.124 |
lh534j11.126 /

lh534j12.77 - BG chr.

046.93 - OBJ chr.
048.94 /

PALs (16L8):
ow13-1.1
ow13-2.2
ow13-3.97
ow13-4.115

Custom chips:
V-SYSTEM C7-01 GGA
VS8905 6620 9039 ABBA
V-SYSTEM VS8904 GGB
V-SYSTEM VS8803 6082 9040 EBBB

********************************************************************************

 its impossible to know what some of the video registers do due to lack of
 evidence (bg palette has a selector, but i'm not sure which ... test mode
 colours use different palette on rgb test

********************************************************************************

 Info from Steph (a lot of thanks to him for looking at this)
 ---------------


The main thing is that, as in 'ridleofp', there is some code that could be used
if there was no "brute hack" in the code to never use it ...

The "brute hack" is there :

00B91C: 0000 0030                ori.b   #$30, D0

Replace with 0x4e714e71 and you'll be able to test the "hidden features" .
* See #define in driver, maybe there are other versions of the game?..

Dip Switch 1 is read from $f00d and is stored at $8802, while Switch 2 is read from
$f00f and is stored at $8803 ...

"DIPSW 2-5" (bit 4 of DSW2) is tested at address 0x007a18 :

00A718: 0838 0004 8803           btst    #$4, $8803.w

I haven't been able to figure out what this Dip Switch does as I haven't found a way
to call the routine that seems to start at 0x00a710 8( If you find ANY infos, please
let me know ...

"DIPSW 2-6" (bit 5 of DSW2) is probably one of the most "important" Dip Switch, as
it sets the maximum player from 2 (when OFF) to 4 (when ON) ... Watch the "attract
mode" to see how the game looks like ...


Differences between 2 players and 4 players mode :

1) 2 players mode (DIPSW 2-5 is OFF)

There are 2 coin slots (each one has its own coinage), there are only 2 "Start" buttons
and 2 sets of controls :

  1  "P1 Start"
  2  "P2 Start"
  3  no effect
  4  no effect
  5  "Coin 1"  (reads "DIPSW 1-1" to "DIPSW 1-4")
  6  "Coin 2"  (reads "DIPSW 1-5" to "DIPSW 1-8")
  7  no effect (in fact, calls the routine (see below) but doesn't add credits)
  8  no effect
  0  "Service" (adds 1 credit)

  P1 and P2 controls are OK, while P3 and P4 controls have no effect (and you can't
  even test them !) ...

In a 2 players game, the players can move their pieces on the following walls :

  P1  West and South walls
  P2  East and North walls

"DIPSW 2-3" determines how many credits are needed for a 2 players game :

  ON   2 (this should be the default value)
  OFF  1

Note that in a 2 players game, "PLAYER 1" is displayed twice ...

2) 4 players mode  (DIPSW 2-5 is ON)

There are 4 coin slots (each one using the SAME coinage), there are 4 "Start" buttons
and 4 sets of controls :

  1  used only in "test mode"
  2  used only in "test mode"
  3  used only in "test mode"
  4  used only in "test mode"
  5  "Coin 1"  (reads "DIPSW 1-1" to "DIPSW 1-4")
  6  "Coin 2"  (reads "DIPSW 1-1" to "DIPSW 1-4")
  7  "Coin 3"  (reads "DIPSW 1-1" to "DIPSW 1-4")
  8  "Coin 4"  (reads "DIPSW 1-1" to "DIPSW 1-4")
  0  "Service" (adds 1 credit)

  P1 to P4 controls are OK ...

You can test ALL these inputs when you are in the "test mode" : even if you don't see
the ones for players 3 and 4 when you reboot, they will "appear" once you press the key !
However "DISPW 2-4" and ""DISPW 2-5" will still display "N.C." ("Not Connected" ?)
followed by OFF or ON ...

While in the "test mode", the controls will be shown with an arrow :

  MAME key    player 3 display    player 4 display

    Up              Right               Left
    Down            Left                Right
    Left            Up                  Down
    Right           Down                Up

Note that I haven't been able to find where the "Tilt" key was mapped ...

In a 4 players game, each player can move his pieces on ONE following wall :

  P1  North wall
  P2  East  wall
  P3  South wall
  P4  West  wall

When 1 credit is inserted, a timer appears to wait for players to enter the game, and
it's IMPOSSIBLE to start a DEF_STR( Normal ) 1 player game (with the 4 walls) ...

To select a player, press one of his 2 buttons ...

"DIPSW 2-3" is VERY important here :

  - When it's OFF, only player 1 can play, the number of credits is decremented when
    you press the player buttons ... If you wait until timer reaches 0 without selecting
    a player, 1 credit will be substracted, and you'll start a game with player 1 ...
  - When it's ON, 1 credit will be automatically substracted, then the 4 players can
    play by pressing one of their buttons ... If you wait until timer reaches 0 without
    selecting a player, you'll start a game with player 1 ...

"Unfortunately", it's an endless game, has you have NO penalty when a piece can't be
placed 8( Also note that there is NO possibility to "join in" once the game has started ...


Some useful addresses :

  0xff803a ($803a) : credits

Even if display is limited to 9, there doesn't seem to be any limit to the real number
of credits ...

  0xff803b ($803b) : credits that will be added to $803a when you press '6'
  0xff803c ($803c) : credits that will be added to $803a when you press '7'
  0xff803d ($803d) : credits that will be added to $803a when you press '8'
  0xff803e ($803e) : credits that will be added to $803a when you press '0'

These credits will be added by routine described below ...

  0xff8959 ($8959) : "handicap" for player 1 (in a 1 or 2 players game)
  0xff89d9 ($89d9) : "handicap" for player 2 (in a 2 players game only)

"Handicap" range is 0x00 (piece fall from the top of the well) to 0x0c (when the game
is over) ... To end a game quickly for a player, set his value to 0x0b and wait until
a piece can't be placed ...


Some useful routines :

  $b080 : initialisation

When this routine is called with A4 = $400 (at $2466), it determines which routine will
be called when a coin is inserted ... It stores 0x0490 at 0xff8026 ($8026), then it
calls routine at $414 which tests "DIPSW 2-5" ... If it's ON, it stores 0x04a2 ...

Then it stores addresses of routines to add credits according to "DIPSW 1-1" to
"DIPSW 1-4" (stored at 0xff802a ($802a)), and to "DIPSW 1-5" to "DIPSW 1-8" (stored at
0xff802e ($802e)) ...

  $490 : reads coins inputs ("2 players mode")

This routines reads "Coin 1" status, and adds credits according to routine stored at
0xff802a ($802a) ... Then it reads "Coin 2" status, and adds credits according to
routine stored at 0xff802e ($802e) ...

  $4a2 : reads coins inputs ("4 players mode")

This routines reads status of the 4 "Coin", and adds credits according to routine
stored at 0xff802a ($802a) ...

  $50c : read status of a "Coin" button

This routines checks if value is <> 0x00 ... If this is the case, the routine that adds
credits is called, and the value is reset to 0x00 ...

  $ba36 : determines "Coin" status

This routines splits the inputs into consecutive addresses : if the button is pressed,
0x01 will be added :

  - status of "Coin 1"  is stored at address 0xff8030 ($8030)
  - status of "Coin 2"  is stored at address 0xff8031 ($8031)
  - status of "Coin 3"  is stored at address 0xff8032 ($8032)
  - status of "Coin 4"  is stored at address 0xff8033 ($8033)
  - status of "Service" is stored at address 0xff8034 ($8034)

  $9002 : checks "1 Player Start" and "2 Players Start" buttons

  $9018 : "1 Player Start" button is pressed

If enough credits, the number of credits is decremented by 1 ...

  $9056 : "2 Player Start" button is pressed

If enough credits, the number of credits is decremented by 1 or 2, depending of
"DIPSW 2-3" ...

  $90ce : reads status of "DIPSW 2-3" Dip Switch

This routine determines how many credits (1 or 2) are needed for a 2 players game ...

  $b908 : stores Dip Switches in memory

  $b91c : "brute hack" to disable "4 players mode" features

  $b924 : reads status of "DIPSW 2-7" Dip Switch for "screen flipping" support ...

  $9962 : reads status of "DIPSW 2-4" Dip Switch for "demo sounds" support ...

  $cf70 : sound routine

There are read/writes on bit 7 of 0xfff009 ($f009) ...

*/
/*******************************************************************************

    Miyasu Nonki no Quiz 18-Kin (Japan)
    (c)1992 EIM

    Added by Takahiro Nogi <nogi@kt.rim.or.jp> 2003/08/15 -


Board:  OW-13 CPU
CPU:    68000-10
        Z80-B
Sound:  YM2610
OSC:    20.00000MHz
        14.31818MHz
        12.000MHz
        8.000MHz
Custom: C7-01 GGA
        VS8803
        VS8904
        VS8905


1-IC8.BIN    main prg.
2-IC7.BIN
IC10.BIN
IC9.BIN

3-IC144.BIN  sound prg.

IC123.BIN    samples
IC124.BIN
IC126.BIN

IC77.BIN     BG chr.
IC78.BIN
IC79.BIN

IC93.BIN     OBJ chr.
IC94.BIN


TODO:

- Couldn't figure out sprite table initialize routine, so I initialize it manually.

*******************************************************************************/

#define WELLTRIS_4P_HACK 0

#include "driver.h"
#include "sound/2610intf.h"

UINT16 *welltris_spriteram;
size_t welltris_spriteram_size;
UINT16 *welltris_pixelram;
UINT16 *welltris_charvideoram;

READ16_HANDLER( welltris_spriteram_r );
WRITE16_HANDLER( welltris_spriteram_w );
WRITE16_HANDLER( welltris_palette_bank_w );
WRITE16_HANDLER( welltris_gfxbank_w );
WRITE16_HANDLER( welltris_charvideoram_w );
WRITE16_HANDLER( welltris_scrollreg_w );
VIDEO_START( welltris );
VIDEO_UPDATE( welltris );



static WRITE8_HANDLER( welltris_sh_bankswitch_w )
{
	UINT8 *rom = memory_region(REGION_CPU2) + 0x10000;

	memory_set_bankptr(1,rom + (data & 0x03) * 0x8000);
}


static int pending_command;

static WRITE16_HANDLER( sound_command_w )
{
	if (ACCESSING_LSB)
	{
		pending_command = 1;
		soundlatch_w(0, data & 0xff);
		cpunum_set_input_line(1, INPUT_LINE_NMI, PULSE_LINE);
	}
}

static READ16_HANDLER( in0_r )
{
	return readinputport(0) | (pending_command ? 0x80 : 0);
}

static WRITE8_HANDLER( pending_command_clear_w )
{
	pending_command = 0;
}


static ADDRESS_MAP_START( welltris_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x100000, 0x17ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x800000, 0x81ffff) AM_READ(MRA16_RAM) /* Graph_1 & 2*/
	AM_RANGE(0xff8000, 0xffbfff) AM_READ(MRA16_RAM) /* Work */
	AM_RANGE(0xffc000, 0xffc3ff) AM_READ(MRA16_RAM) /* Sprite */
	AM_RANGE(0xffd000, 0xffdfff) AM_READ(MRA16_RAM) /* Char */
	AM_RANGE(0xffe000, 0xffefff) AM_READ(MRA16_RAM) /* Palette */

	AM_RANGE(0xfff000, 0xfff001) AM_READ(input_port_1_word_r) /* Bottom Controls */
	AM_RANGE(0xfff002, 0xfff003) AM_READ(input_port_2_word_r) /* Top Controls */
	AM_RANGE(0xfff004, 0xfff005) AM_READ(input_port_3_word_r) /* Left Side Ctrls */
	AM_RANGE(0xfff006, 0xfff007) AM_READ(input_port_4_word_r) /* Right Side Ctrls */

	AM_RANGE(0xfff008, 0xfff009) AM_READ(in0_r) /* Coinage, Start Buttons, pending sound command etc. */  /* Bit 5 Tested at start of irq 1 */
	AM_RANGE(0xfff00a, 0xfff00b) AM_READ(input_port_5_word_r) /* P3+P4 Coin + Start Buttons */
	AM_RANGE(0xfff00c, 0xfff00d) AM_READ(input_port_6_word_r) /* DSW0 Coinage */
	AM_RANGE(0xfff00e, 0xfff00f) AM_READ(input_port_7_word_r) /* DSW1 Game Options */
ADDRESS_MAP_END

static ADDRESS_MAP_START( welltris_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x100000, 0x17ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x800000, 0x81ffff) AM_WRITE(MWA16_RAM) AM_BASE(&welltris_pixelram)
	AM_RANGE(0xff8000, 0xffbfff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0xffc000, 0xffc3ff) AM_WRITE(welltris_spriteram_w) AM_BASE(&welltris_spriteram)
	AM_RANGE(0xffd000, 0xffdfff) AM_WRITE(welltris_charvideoram_w) AM_BASE(&welltris_charvideoram)
	AM_RANGE(0xffe000, 0xffefff) AM_WRITE(paletteram16_xRRRRRGGGGGBBBBB_word_w) AM_BASE(&paletteram16)

	AM_RANGE(0xfff000, 0xfff001) AM_WRITE(welltris_palette_bank_w)
	AM_RANGE(0xfff002, 0xfff003) AM_WRITE(welltris_gfxbank_w)
	AM_RANGE(0xfff004, 0xfff007) AM_WRITE(welltris_scrollreg_w)
	AM_RANGE(0xfff008, 0xfff009) AM_WRITE(sound_command_w)
	AM_RANGE(0xfff00c, 0xfff00d) AM_WRITE(MWA16_NOP)		// ??
	AM_RANGE(0xfff00e, 0xfff00f) AM_WRITE(MWA16_NOP)		// ??
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x77ff) AM_READ(MRA8_ROM)
	AM_RANGE(0x7800, 0x7fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8000, 0xffff) AM_READ(MRA8_BANK1)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x77ff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x7800, 0x7fff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x08, 0x08) AM_READ(YM2610_status_port_0_A_r)
	AM_RANGE(0x0a, 0x0a) AM_READ(YM2610_status_port_0_B_r)
	AM_RANGE(0x10, 0x10) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(welltris_sh_bankswitch_w)
	AM_RANGE(0x08, 0x08) AM_WRITE(YM2610_control_port_0_A_w)
	AM_RANGE(0x09, 0x09) AM_WRITE(YM2610_data_port_0_A_w)
	AM_RANGE(0x0a, 0x0a) AM_WRITE(YM2610_control_port_0_B_w)
	AM_RANGE(0x0b, 0x0b) AM_WRITE(YM2610_data_port_0_B_w)
	AM_RANGE(0x18, 0x18) AM_WRITE(pending_command_clear_w)
ADDRESS_MAP_END



INPUT_PORTS_START( welltris )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE2 )	/* Test (used to go through tests in service mode) */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_TILT )		/* Tested at start of irq 1 */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE1 )	/* Service (adds a coin) */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL )	/* pending sound command */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#if WELLTRIS_4P_HACK
	/* These can actually be read in the test mode even if they're not used by the game without patching the code
       might be useful if a real 4 player version ever turns up if it was ever produced */
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
#else
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
#endif

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN4 )
#if WELLTRIS_4P_HACK
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START4 )
#else
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
#endif
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_DIPNAME( 0x000f, 0x000f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0009, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0004, "2-1, 4-2, 5-3, 6-4" )
	PORT_DIPSETTING(      0x0003, "2-1, 4-3" )
	PORT_DIPSETTING(      0x000f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0001, "1-1, 2-2, 3-3, 4-5" )
	PORT_DIPSETTING(      0x0002, "1-1, 2-2, 3-3, 4-4, 5-6" )
	PORT_DIPSETTING(      0x0000, "1-1, 2-3" )
	PORT_DIPSETTING(      0x0005, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x000e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x000d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x000b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x000a, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x00f0, 0x00f0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0060, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0070, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0090, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0040, "2-1, 4-2, 5-3, 6-4" )
	PORT_DIPSETTING(      0x0030, "2-1, 4-3" )
	PORT_DIPSETTING(      0x00f0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0010, "1-1, 2-2, 3-3, 4-5" )
	PORT_DIPSETTING(      0x0020, "1-1, 2-2, 3-3, 4-4, 5-6" )
	PORT_DIPSETTING(      0x0000, "1-1, 2-3" )
	PORT_DIPSETTING(      0x0050, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x00e0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x00d0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x00c0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x00b0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x00a0, DEF_STR( 1C_6C ) )

	PORT_START
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )			// "Super" in test mode
	PORT_DIPNAME( 0x0004, 0x0000, "Coin Mode" )
	PORT_DIPSETTING(      0x0004, "Mono Player" )
	PORT_DIPSETTING(      0x0000, "Many Player" )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) )
#if WELLTRIS_4P_HACK
	/* again might be handy if a real 4 player version shows up */
	PORT_DIPNAME( 0x0010, 0x0010, "DIPSW 2-5 (see notes)" )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "4 Players Mode (see notes)" )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
#else
	PORT_DIPNAME( 0x0010, 0x0010, "DIPSW 2-5 (unused)" )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "DIPSW 2-6 (unused)" )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
#endif
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) /* Flip Screen Not Currently Supported */
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
  	PORT_SERVICE( 0x0080, IP_ACTIVE_LOW )
INPUT_PORTS_END

INPUT_PORTS_START( quiz18k )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL )	/* pending sound command */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT (0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT (0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT (0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, "DIPSW 1-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "DIPSW 1-2" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "DIPSW 1-3" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "DIPSW 1-4" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "DIPSW 1-5" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "DIPSW 1-6" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "DIPSW 1-7" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DIPSW 1-8" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) ) /* Flip Screen Not Currently Supported */
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "DIPSW 2-3" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "DIPSW 2-4" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "DIPSW 2-5" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Title Logo Type" )
	PORT_DIPSETTING(    0x20, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPNAME( 0x40, 0x40, "DIPSW 2-7" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
  	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )
INPUT_PORTS_END



static const gfx_layout welltris_charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 1*4, 0*4, 3*4, 2*4, 5*4, 4*4, 7*4, 6*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout welltris_spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ 0, 1, 2, 3 },
	{ 1*4, 0*4, 3*4, 2*4, RGN_FRAC(1,2)+1*4, RGN_FRAC(1,2)+0*4, RGN_FRAC(1,2)+3*4, RGN_FRAC(1,2)+2*4,
			5*4, 4*4, 7*4, 6*4, RGN_FRAC(1,2)+5*4, RGN_FRAC(1,2)+4*4, RGN_FRAC(1,2)+7*4, RGN_FRAC(1,2)+6*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	64*8
};

static const gfx_decode welltris_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &welltris_charlayout,   16* 0, 4*16 },
	{ REGION_GFX2, 0, &welltris_spritelayout, 16*96, 2*16 },
	{ -1 } /* end of array */
};



static void irqhandler(int irq)
{
	cpunum_set_input_line(1, 0, irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2610interface ym2610_interface =
{
	irqhandler,
	REGION_SOUND1,
	REGION_SOUND2
};



static void init_welltris(void)
{
#if WELLTRIS_4P_HACK
	/* A Hack which shows 4 player mode in code which is disabled */
	UINT16 *RAM = (UINT16 *)memory_region(REGION_CPU1);
	RAM[0xB91C/2] = 0x4e71;
	RAM[0xB91E/2] = 0x4e71;
#endif
}

static void init_quiz18k(void)
{
	;
}



static MACHINE_DRIVER_START( welltris )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,20000000/2)	/* 10 MHz */
	MDRV_CPU_PROGRAM_MAP(welltris_readmem,welltris_writemem)
	MDRV_CPU_VBLANK_INT(irq1_line_hold,1)

	MDRV_CPU_ADD(Z80,8000000/2)		/* 4 MHz ??? */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_IO_MAP(sound_readport,sound_writeport)
								/* IRQs are triggered by the YM2610 */
	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(512, 256)
	MDRV_VISIBLE_AREA(15, 367-1, 8, 248-1)
	MDRV_GFXDECODE(welltris_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(welltris)
	MDRV_VIDEO_UPDATE(welltris)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2610, 8000000)
	MDRV_SOUND_CONFIG(ym2610_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.25)
	MDRV_SOUND_ROUTE(1, "mono", 0.75)
	MDRV_SOUND_ROUTE(2, "mono", 0.75)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( quiz18k )

	/* basic machine hardware */
	MDRV_IMPORT_FROM( welltris )

	MDRV_VISIBLE_AREA(15, 335-1, 0, 224-1)
MACHINE_DRIVER_END



ROM_START( welltris )
	ROM_REGION( 0x180000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "j2u.8", 0x000000, 0x20000, BAD_DUMP CRC(00a382a4) SHA1(b6a7e1b360e3cefc582983c8dff6aa28a7c77124) )
	ROM_LOAD16_BYTE( "j1u.7", 0x000001, 0x20000, BAD_DUMP CRC(abcb008b) SHA1(c71ed431cbd8aea0b4396029203a7b735a8fdac3) )
	/* Space */
	ROM_LOAD16_BYTE( "lh532j10.10", 0x100000, 0x40000, CRC(1187c665) SHA1(c6c55016e46805694348b386e521a3ef1a443621) )
	ROM_LOAD16_BYTE( "lh532j11.9",  0x100001, 0x40000, CRC(18eda9e5) SHA1(c01d1dc6bfde29797918490947c89440b58d5372) )

	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* 64k for the audio CPU + banks */
	ROM_LOAD( "3.144", 0x00000, 0x20000, CRC(ae8f763e) SHA1(255419e02189c2e156c1fbcb0cd4aedd14ed8ffa) )
	ROM_RELOAD(        0x10000, 0x20000 )

	ROM_REGION( 0x0a0000, REGION_GFX1, ROMREGION_DISPOSE ) /* CHAR Tiles */
	ROM_LOAD( "lh534j12.77", 0x000000, 0x80000, CRC(b61a8b74) SHA1(e17f7355375bdc166ef8131f7de9dbda5453f570) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE ) /* SPRITE Tiles */
	ROM_LOAD( "046.93", 0x000000, 0x40000, CRC(31d96d77) SHA1(5613ef9e9e38406b4e64fc8983ea50b57613923e) )
	ROM_LOAD( "048.94", 0x040000, 0x40000, CRC(bb4643da) SHA1(38d54f8c3dba09b528df05d748ab5bdf5d028453) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 ) /* sound samples */
	ROM_LOAD( "lh534j11.126", 0x00000, 0x80000, CRC(bf85fb0d) SHA1(358f91bbff2d3260f83b5a0422c0d985d1735cef) )

	ROM_REGION( 0x100000, REGION_SOUND2, 0 ) /* sound samples */
	ROM_LOAD( "lh534j09.123", 0x00000, 0x80000, CRC(6c2ce9a5) SHA1(a4011ecfb505191c9934ba374933cd11b331d55a) )
	ROM_LOAD( "lh534j10.124", 0x80000, 0x80000, CRC(e3682221) SHA1(3e1cda07cf451955dc473eabe007854e5148ae27) )
ROM_END

ROM_START( welltrij )
	ROM_REGION( 0x180000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "j2.8", 0x000000, 0x20000, CRC(68ec5691) SHA1(8615415c5c98aa9caa0878a8251da7985f050f94) )
	ROM_LOAD16_BYTE( "j1.7", 0x000001, 0x20000, CRC(1598ea2c) SHA1(e9150c3ab9b5c0eb9a5fee3e071358f92a005078) )
	/* Space */
	ROM_LOAD16_BYTE( "lh532j10.10", 0x100000, 0x40000, CRC(1187c665) SHA1(c6c55016e46805694348b386e521a3ef1a443621) )
	ROM_LOAD16_BYTE( "lh532j11.9",  0x100001, 0x40000, CRC(18eda9e5) SHA1(c01d1dc6bfde29797918490947c89440b58d5372) )

	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* 64k for the audio CPU + banks */
	ROM_LOAD( "3.144", 0x00000, 0x20000, CRC(ae8f763e) SHA1(255419e02189c2e156c1fbcb0cd4aedd14ed8ffa) )
	ROM_RELOAD(        0x10000, 0x20000 )

	ROM_REGION( 0x0a0000, REGION_GFX1, ROMREGION_DISPOSE ) /* CHAR Tiles */
	ROM_LOAD( "lh534j12.77", 0x000000, 0x80000, CRC(b61a8b74) SHA1(e17f7355375bdc166ef8131f7de9dbda5453f570) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE ) /* SPRITE Tiles */
	ROM_LOAD( "046.93", 0x000000, 0x40000, CRC(31d96d77) SHA1(5613ef9e9e38406b4e64fc8983ea50b57613923e) )
	ROM_LOAD( "048.94", 0x040000, 0x40000, CRC(bb4643da) SHA1(38d54f8c3dba09b528df05d748ab5bdf5d028453) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 ) /* sound samples */
	ROM_LOAD( "lh534j11.126", 0x00000, 0x80000, CRC(bf85fb0d) SHA1(358f91bbff2d3260f83b5a0422c0d985d1735cef) )

	ROM_REGION( 0x100000, REGION_SOUND2, 0 ) /* sound samples */
	ROM_LOAD( "lh534j09.123", 0x00000, 0x80000, CRC(6c2ce9a5) SHA1(a4011ecfb505191c9934ba374933cd11b331d55a) )
	ROM_LOAD( "lh534j10.124", 0x80000, 0x80000, CRC(e3682221) SHA1(3e1cda07cf451955dc473eabe007854e5148ae27) )
ROM_END

ROM_START( quiz18k )
	ROM_REGION( 0x180000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "1-ic8.bin", 0x000000, 0x20000, CRC(10a64336) SHA1(d63c0752385e1d66b09a7197e267dcd0e5e93be8) )
	ROM_LOAD16_BYTE( "2-ic7.bin", 0x000001, 0x20000, CRC(8b21b431) SHA1(278238ab4a5d11577c5ab3c7462b429f510a1d50) )
	/* Space */
	ROM_LOAD16_BYTE( "ic10.bin", 0x100000, 0x40000, CRC(501453a3) SHA1(d127f417f1c52333e478ac397fbe8a2f223b1ce7) )
	ROM_LOAD16_BYTE( "ic9.bin",  0x100001, 0x40000, CRC(99b6840f) SHA1(8409a33c64729066bfed6e49dcd84f30906274cb) )

	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* 64k for the audio CPU + banks */
	ROM_LOAD( "3-ic144.bin", 0x00000, 0x20000, CRC(72d372e3) SHA1(d077e34947de1050b68d76506cc8926b06a94a76) )
	ROM_RELOAD(              0x10000, 0x20000 )

	ROM_REGION( 0x180000, REGION_GFX1, ROMREGION_DISPOSE ) /* CHAR Tiles */
	ROM_LOAD( "ic77.bin", 0x000000, 0x80000, CRC(af3b6fd1) SHA1(d22f7cf62a94ae3a2dcb0236630e9ac88d5e528b) )
	ROM_LOAD( "ic78.bin", 0x080000, 0x80000, CRC(44bbdef3) SHA1(cd91eaf98602ef3448f49c8287591aa845afb874) )
	ROM_LOAD( "ic79.bin", 0x100000, 0x80000, CRC(d721e169) SHA1(33ec819c4e7b4dbab41756af9eca857107d96c8b) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* SPRITE Tiles */
	ROM_LOAD( "ic93.bin", 0x000000, 0x80000, CRC(4d387c5e) SHA1(e77aea06b9b2dc8ada5618aaf83bb80f63670363) )
	ROM_LOAD( "ic94.bin", 0x080000, 0x80000, CRC(6be2f164) SHA1(6a3ca63d6238d587a50718d2a6c76f01932c76c3) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 ) /* sound samples */
	ROM_LOAD( "ic126.bin", 0x00000, 0x40000, CRC(7a92fbc9) SHA1(c13be1e84fc8e74c85d25d3357e078bc9e264682) )

	ROM_REGION( 0x140000, REGION_SOUND2, 0 ) /* sound samples */
	ROM_LOAD( "ic123.bin", 0x00000, 0x80000, CRC(ee4995cf) SHA1(1b47938ddc87709f8d118b86fe62602972c77ced) )
	ROM_LOAD( "ic124.bin", 0x80000, 0x40000, CRC(076f58c3) SHA1(bd78f39b85b2697e733896705355e21b8d2a141d) )
ROM_END



GAME( 1991, welltris, 0,        welltris, welltris, welltris, ROT0,   "Video System Co.", "Welltris (World?, 2 players)", GAME_NO_COCKTAIL ,0)
GAME( 1991, welltrij, welltris, welltris, welltris, welltris, ROT0,   "Video System Co.", "Welltris (Japan, 2 players)", GAME_NO_COCKTAIL ,0)
GAME( 1992, quiz18k,  0,        quiz18k,  quiz18k,  quiz18k,  ROT0,   "EIM", "Miyasu Nonki no Quiz 18-Kin", GAME_NO_COCKTAIL ,0)
