/***********************************************************************

Namco System 1

Shadow Land / Yokai Douchuuki      (c) 1987 Namco
Dragon Spirit                      (c) 1987 Namco
Blazer                             (c) 1987 Namco
Quester                            (c) 1987 Namco
Pac-Mania                          (c) 1987 Namco
Galaga '88                         (c) 1987 Namco
World Stadium                      (c) 1988 Namco
Beraboh Man                        (c) 1988 Namco
Alice In Wonderland / Marchen Maze (c) 1988 Namco
Bakutotsu Kijuutei                 (c) 1988 Namco
World Court                        (c) 1988 Namco
Splatter House                     (c) 1988 Namco
Face Off                           (c) 1988 Namco
Rompers                            (c) 1989 Namco
Blast Off                          (c) 1989 Namco
World Stadium '89                  (c) 1989 Namco
Dangerous Seed                     (c) 1989 Namco
World Stadium '90                  (c) 1990 Namco
Pistol Daimyo no Bouken            (c) 1990 Namco
Boxy Boy / Souko Ban Deluxe        (c) 1990 Namco
Puzzle Club                        (c) 1990 Namco (prototype)
Tank Force                         (c) 1991 Namco

We are missing some alternate versions:
- Alice In Wonderland (English version of Marchen Maze)
- Face Off (6 sticks)
- Tank Force (4 players)



An important thing to note about the custom chips is that the sprite generator
is the same as System 86.

From Pac-Mania schematics (but they show the Dragon Spirit key custom, and the
Galaga '88 schematics are the same again, so these three should be identical)
Custom ICs:
----------
CUS27       clock divider
CUS30       sound control
CUS39       sprite generator
CUS48       sprite address generator
CUS64       MCU (63701)
CUS95(x5)   I/O interface
CUS99(x2)   sound volume
CUS116      display output generator
CUS117      main/sub CPU MMU
CUS120      sprite/tilemap mixer and palette address interface
CUS121      sound CPU address decoder
CUS123      scrolling tilemap address generator
CUS133      tilemap generator
CUS151      protection [1]

[1] Changes from game to game


Memory map
----------
Main, Sub CPU:
Address decoding is entirely handled by CUS117, which is a simple MMU providing a
virtual address space 23 bits wide. The chip outputs the various enable lines for
RAM, ROM, etc., and bits 12-21 of the virtual address (therefore bit 22 is handled
only internally). There are 8 banks in the 6809 address space, each one redirectable
to a portion of the virtual address space. The main and sub CPUs are independent,
each one can set up its own banks.

Main & sub CPU memory map:

Address          Dir Data     Name      Description
---------------- --- -------- --------- -----------------------
000xxxxxxxxxxxxx R/W xxxxxxxx           bank #0
001xxxxxxxxxxxxx R/W xxxxxxxx           bank #1
010xxxxxxxxxxxxx R/W xxxxxxxx           bank #2
011xxxxxxxxxxxxx R/W xxxxxxxx           bank #3
100xxxxxxxxxxxxx R/W xxxxxxxx           bank #4
101xxxxxxxxxxxxx R/W xxxxxxxx           bank #5
110xxxxxxxxxxxxx R/W xxxxxxxx           bank #6
111xxxxxxxxxxxxx R   xxxxxxxx           bank #7
1110bbb--------0   W xxxxxxxx           set bits 14-21 of bank #bbb
1110bbb--------1   W ------xx           set bits 22-23 of bank #bbb
1111000---------   W -------x SUBRES    sub CPUs reset (SRES)
1111001---------   W --------           watchdog reset (MRES, SRES)
1111010---------   W --------           ???
1111011---------   W --------           IRQ acknowledge (MIRQ, SIRQ)
1111100---------     --------           FIRQ acknowledge (MFIRQ, MSIRQ)
1111101---------   W --------           trigger FIRQ on sub CPU (SFIRQ) (bakutotu)
1111110---------   W xxxxxxxx           select bank #7 (ROM) for sub CPU
1111111---------     --------           not used?


virtual memory map:

Address                 Dir Data     Name      Description
----------------------- --- -------- --------- -----------------------
010110----------------x   W -------- 3DCS      side select for 3D glasses
01011100xxxxxxxxxxxxxxx R/W xxxxxxxx COLCS     palette + video window (CUS116 & CUS120)
01011110xxxxxxxxxxxxxxx R/W xxxxxxxx CHAR      tilemap RAM (CUS133)
0101111100---xxxxxxxxxx R/W xxxxxxxx KEY       protection key custom [1]
01011111100xxxxxxxxxxxx R/W xxxxxxxx OBJECT    work RAM (CUS48)
010111111001xxxxxxxxxxx R/W xxxxxxxx           portion holding sprite registers
01011111101-------xxxxx R/W xxxxxxxx SCRDT     tilemap control (CUS123 & CUS133)
01011111110--xxxxxxxxxx R/W xxxxxxxx SOUND     sound RAM (through CUS30) [2]
01011111111-xxxxxxxxxxx R/W xxxxxxxx IOEN      TRIRAM (shared with all other CPUs)
01100000xxxxxxxxxxxxxxx R/W xxxxxxxx RAM       work RAM
1000xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM H10   program ROM
1001xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM K10   program ROM
1010xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM L10   program ROM
1011xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM M10   program ROM
1100xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM N10   program ROM
1101xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM P10   program ROM
1110xxxxxxxxxxxxxxxxxxx R   xxxxxxxx ROM S10   program ROM
1111--xxxxxxxxxxxxxxxxx R   xxxxxxxx ROM T10   program ROM [3]

[1] different in every game
[2] from the schematics, it would seem that only 000-0FF is shared, but shadowld
    proves that the whole 000-3FF is shared
[3] address bit 16 is inverted


sound CPU memory map:
address deconding is done for the most part by CUS121, and partially by CUS30

Address          Dir Data     Name      Description
---------------- --- -------- --------- -----------------------
00xxxxxxxxxxxxxx R   xxxxxxxx ROM A10/B10 program ROM (banked)
0100-----------x R/W xxxxxxxx YCS       YM2151
0101--xxxxxxxxxx R/W xxxxxxxx RAM 3F    sound RAM (through CUS30, shared with main & sub CPU) [1]
0101--00xxxxxxxx R/W xxxxxxxx           portion holding the sound wave data
0101--0100xxxxxx R/W xxxxxxxx           portion holding the sound registers
0111-xxxxxxxxxxx R/W xxxxxxxx VOICEG    TRIRAM (shared with the other CPUs)
100xxxxxxxxxxxxx R/W xxxxxxxx RAMC      work RAM
11xxxxxxxxxxxxxx R   xxxxxxxx ROM A10   program ROM
1100------------   W xxxx----           ROM bank select
1101------------   W --------           watchdog (RESOUT?)
1110------------   W --------           IRQ acknowledge (SNDIRQ)

[1] from the schematics, it would seem that only 000-0FF is shared, but e.g. shadowld
    proves that the whole 000-3FF RAM is shared


MCU memory map:
address decoding is done by a 16L8 PAL @ K3 which hasn't been read, so memory
map is inferred by program behaviour

Address          Dir Data     Name      Description
---------------- --- -------- --------- -----------------------
00000000xxxxxxxx                        MCU internal registers, timers, ports and RAM
000100--------x- R   xxxxxxxx DIPSW     dip switches
000101---------x R   xxxxxxxx CONTROL   switch inputs
000110----------                        n.c.
000111----------                        n.c.
01xxxxxxxxxxxxxx R   xxxxxxxx ROM A4/B4/E4/F4/H4/K4 (lower half) (banked, see below)
10xxxxxxxxxxxxxx R   xxxxxxxx ROM A4/B4/E4/F4/H4/K4 (upper half) (banked, see below)
11000xxxxxxxxxxx R/W xxxxxxxx TRIRAM    work RAM (shared with the other CPUs)
11001xxxxxxxxxxx R/W xxxxxxxx EEROM     non volatile memory
110100----------   W xxxxxxxx LT0       DAC #0
110101----------   W xxxxxxxx LT1       DAC #1
110110----------   W x-------           \select ROM K4
110110----------   W -x------           \select ROM H4
110110----------   W --x-----           \select ROM F4
110110----------   W ---x----           \select ROM E4
110110----------   W ----x---           \select ROM B4
110110----------   W -----x--           \select ROM A4
110110----------   W ------x-           to A16 of active ROM (inverted for ROM A4)
110110----------   W -------x           to A15 of active ROM
110111----------                        n.c.
1110------------                        n.c.
1111xxxxxxxxxxxx R   xxxxxxxx           MCU internal ROM
1111------------   W                    IRQ acknowledge




Preliminary driver by:
Ernesto Corvi
ernesto@imagina.com

Updates by:
Vernon C. Brooks, Acho A. Tang, Nicola Salmoria


Notes:
- The ROM/RAM test is NOT performed by default. It is only done if the test mode
  switch is on when the game powers up (setting it and resetting is not enough).
  You can manage to make it work if you press F2 quickly enough after the MAME
  startup screen, without having to exit MAME and restarting.

- There are three watchdogs, one per CPU. Handling them separately is necessary
  to allow entering service mode without manually resetting: only one of the CPUs
  stops writing to the watchdog.

- berabohm and bakutotu have a strange demo play on startup, the player doesn't
  move. This seems to be by design, because after you play a game a few seconds
  from the last game are replayed. It is weird that they didn't provide some
  default, though. And in bakutotu you can fire during the demo.
  If you play and then reset, the demo still does nothing. This should prove that
  RAM is cleared on purpose during boot and there is no default.

- bakutotu:
  The main and sub processors work closely together and the game
  quickly runs into trouble when one of them lacks behind. Heavy
  sync and overclocking will help but there's a particularly nasty
  loop appearing in more than 40 places of the code:

  1) kick the watchdog
  2) raise a flag
  3) do some dummy tasks
  4) check whether the flag has been reset by the sub CPU
  5) go back to step 1 if not

- berabohm has special pressure sensitive button, with two switches. The harder
  you push the button, the faster the two switches are closed one after the other.
  The speed is measured by an external board and reported to the game.
  Due to MAME's limited input sample rate (once per frame) it is difficult
  to reproduce that. Instead, I've hooked 6 fake buttons: weak, medium and strong
  punch, and weak, medium and strong jump.


TODO:
- There is still a big mistery about the first location of tri port ram, which is
  shared among all four CPUs. See namcos1_mcu_patch_w() for the kludge: essentially,
  this location has to be 0xA6 for the games to work. However, the MCU first sets it
  to 0xA6, then zeroes it - and there doesn't seem to be any code anywhere for any CPU
  that would set it back to 0xA6. Se, we ignore the zeroing write.

- Blast Off fails the ROM test (IO ERROR 64). This is caused by concurrent access
  to the same memory during the RAM tests: CPU #2 writes to $7101 before the MCU
  has read it back ($c101).

- dspirit sometimes resets during area 3. This was with invincibility cheat turned
  on, however.

- Most of the dip switches are undocumented debug controls. There might be still
  more marked as "unknown" that have a function.



Namco System 1 hardware
=======================

Processors:

6809  - Main CPU
6809  - Sub CPU
6809  - Sound CPU (PSG,FM)
63701 - MCU (input,EEPROM,DAC)

Inter-processor communication is done via a 2K shared memory area.

Bankswitching:

Main/Sub - a 10-bit value is written to location Ex00 to select the 8K
bank (RAM or PRG0-PRG7) which is accessed at offset x000. (x is even)

Sound - a 3-bit value is written to location C000 or C001, bits 4-6 to
select the 16K bank (SND0-SND1) which is accessed at offset 0000.

MCU - a 8-bit value is written to location D800 to select the 32K bank
(VOI0-VOI5) which is accessed at offset 4000. Bits 2-7 are a bitmask
which specify the ROM to access and bits 0-1 specify the ROM offset.

Graphics:

Visible screen resolution: 288x244 pixels (36x28 tiles)

3 scrolling 64x64 tilemapped playfields
1 scrolling 64x32 tilemapped playfield
2 fixed 36x28 tilemapped playfields

Each playfield uses one of 8 color palettes, can be enabled or disabled,
and has programmable priorities.

Each tile is a 8x8, 8 bit-per-pixel character from a selection of up to
16384 characters (CHR0-CHR7). A separate 1 bit-per-pixel character mask
(CHR8) defines the character shape.

127 displayable 32x32, 4 bit-per-pixel sprites from a selection of up to
2048 sprites (OBJ0-OBJ7). Each sprite uses one of 127 color palettes or
a special shadow/highlight effect and has programmable priorities and
x-y flipping. Sprites may also be displayed as a smaller portion of a
32x32 object with a programmable size and position. The height and width
are programmed separately and may be 4,8,16,or 32 pixels.

3 24-bit programmable RGB palette tables, 8 bits per color as follows:

127 16-color entries for the sprites
8 256-color entries for the playfields
8 256-color entries for the playfields shadow/highlight effects

Sound:

Namco custom 8 channel 16-bit stereo PSG for sound effects
registor array based 2 channel 8-bit DAC for voice
Yamaha YM2151+YM3012 FM chip for background music

Controls:

The standard hardware supports one or two 8-way joysticks with up to
three buttons for each player, two start buttons, a service switch, two
coin slots, and one dipswitch block. Game settings are accessed via
service mode and are saved in EEPROM.

Games:

Date  Name                                  Key  Screen
----- ------------------------------------- ---- ------
 4/87 Yokai Douchuuki / Shadowland          NONE H
 6/87 Dragon Spirit (old version)           136  V
??/87 Dragon Spirit (new version)           136  V
 7/87 Blazer                                144  V
 9/87 Quester                               A    V
??/87 Quester (special edition)             A    V
11/87 Pac-Mania                             151  V-FLIP
11/87 Pac-Mania (Japanese version)          151  V
12/87 Galaga '88                            153  V-FLIP
12/87 Galaga '88 (Japanese version)         153  V
 3/88 World Stadium                         154  H
 5/88 Beraboh Man                           B    H
??/88 Beraboh Man (standard NS1 hardware)   NONE H
 7/88 Marchen Maze / Alice In Wonderland    152  H
 8/88 Bakutotsu Kijuutei / Baraduke 2       155  H
10/88 World Court                           143  H
11/88 Splatter House                        181  H
12/88 Face Off                              C    H
 2/89 Rompers                               182  V
 3/89 Blast Off                             183  V
 7/89 World Stadium '89                     184  H
12/89 Dangerous Seed                        308  V
 7/90 World Stadium '90                     310  H
10/90 Pistol Daimyo no Bouken               309  H-FLIP
11/90 Souko Ban Deluxe                      311  H-FLIP
??/90 Puzzle Club (prototype)               ?    V
12/91 Tank Force                            185  H-FLIP

A - uses sub board with paddle control(s)
B - uses sub board with pressure sensitive controls
C - uses sub board with support for player 3 and 4 controls

***********************************************************************/

#include "driver.h"
#include "cpu/m6809/m6809.h"
#include "cpu/m6800/m6800.h"
#include "sound/2151intf.h"
#include "sound/namco.h"
#include "sound/dac.h"

/* from vidhrdw */
VIDEO_START( namcos1 );
VIDEO_UPDATE( namcos1 );
VIDEO_EOF( namcos1 );

/* from machine */
WRITE8_HANDLER( namcos1_bankswitch_w );
WRITE8_HANDLER( namcos1_subcpu_bank_w );

WRITE8_HANDLER( namcos1_cpu_control_w );
WRITE8_HANDLER( namcos1_watchdog_w );
WRITE8_HANDLER( namcos1_sound_bankswitch_w );

WRITE8_HANDLER( namcos1_mcu_bankswitch_w );
WRITE8_HANDLER( namcos1_mcu_patch_w );

MACHINE_RESET( namcos1 );

DRIVER_INIT( shadowld );
DRIVER_INIT( dspirit );
DRIVER_INIT( quester );
DRIVER_INIT( blazer );
DRIVER_INIT( pacmania );
DRIVER_INIT( galaga88 );
DRIVER_INIT( ws );
DRIVER_INIT( berabohm );
DRIVER_INIT( alice );
DRIVER_INIT( bakutotu );
DRIVER_INIT( wldcourt );
DRIVER_INIT( splatter );
DRIVER_INIT( faceoff );
DRIVER_INIT( rompers );
DRIVER_INIT( blastoff );
DRIVER_INIT( ws89 );
DRIVER_INIT( dangseed );
DRIVER_INIT( ws90 );
DRIVER_INIT( pistoldm );
DRIVER_INIT( soukobdx );
DRIVER_INIT( puzlclub );
DRIVER_INIT( tankfrce );


/**********************************************************************/

static WRITE8_HANDLER( namcos1_sub_firq_w )
{
	cpunum_set_input_line(1, M6809_FIRQ_LINE, ASSERT_LINE);
}

static WRITE8_HANDLER( irq_ack_w )
{
	cpunum_set_input_line(cpu_getactivecpu(), 0, CLEAR_LINE);
}

static WRITE8_HANDLER( firq_ack_w )
{
	cpunum_set_input_line(cpu_getactivecpu(), M6809_FIRQ_LINE, CLEAR_LINE);
}



static READ8_HANDLER( dsw_r )
{
	int ret = readinputportbytag("DIPSW");
	if (!(offset & 2)) ret >>= 4;
	return 0xf0 | ret;
}

static WRITE8_HANDLER( namcos1_coin_w )
{
	coin_lockout_global_w(~data & 1);
	coin_counter_w(0,data & 2);
	coin_counter_w(1,data & 4);
}

static int dac0_value ,dac1_value, dac0_gain=0x80, dac1_gain=0x80;

static void namcos1_update_DACs(void)
{
	DAC_signed_data_16_w(0,0x8000 + (dac0_value * dac0_gain) + (dac1_value * dac1_gain));
}

static WRITE8_HANDLER( namcos1_dac_gain_w )
{
	int value;

	/* DAC0 (bits 0,2) */
	value = (data & 1) | ((data >> 1) & 2); /* GAIN0,GAIN1 */
	dac0_gain = 0x20 * (value+1);

	/* DAC1 (bits 3,4) */
	value = (data >> 3) & 3; /* GAIN2,GAIN3 */
	dac1_gain = 0x20 * (value+1);

	namcos1_update_DACs();
}

static WRITE8_HANDLER( namcos1_dac0_w )
{
	dac0_value = data - 0x80; /* shift zero point */
	namcos1_update_DACs();
}

static WRITE8_HANDLER( namcos1_dac1_w )
{
	dac1_value = data - 0x80; /* shift zero point */
	namcos1_update_DACs();
}



static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_RAMBANK(1)
	AM_RANGE(0x2000, 0x3fff) AM_RAMBANK(2)
	AM_RANGE(0x4000, 0x5fff) AM_RAMBANK(3)
	AM_RANGE(0x6000, 0x7fff) AM_RAMBANK(4)
	AM_RANGE(0x8000, 0x9fff) AM_RAMBANK(5)
	AM_RANGE(0xa000, 0xbfff) AM_RAMBANK(6)
	AM_RANGE(0xc000, 0xdfff) AM_RAMBANK(7)
	AM_RANGE(0xe000, 0xefff) AM_WRITE(namcos1_bankswitch_w)
	AM_RANGE(0xf000, 0xf000) AM_WRITE(namcos1_cpu_control_w)
	AM_RANGE(0xf200, 0xf200) AM_WRITE(namcos1_watchdog_w)
//  AM_RANGE(0xf400, 0xf400) AM_WRITE(MWA8_NOP) // unknown
	AM_RANGE(0xf600, 0xf600) AM_WRITE(irq_ack_w)
	AM_RANGE(0xf800, 0xf800) AM_WRITE(firq_ack_w)
	AM_RANGE(0xfa00, 0xfa00) AM_WRITE(namcos1_sub_firq_w) // asserts FIRQ on CPU1
	AM_RANGE(0xfc00, 0xfc01) AM_WRITE(namcos1_subcpu_bank_w)
	AM_RANGE(0xe000, 0xffff) AM_ROMBANK(8)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sub_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_RAMBANK(9)
	AM_RANGE(0x2000, 0x3fff) AM_RAMBANK(10)
	AM_RANGE(0x4000, 0x5fff) AM_RAMBANK(11)
	AM_RANGE(0x6000, 0x7fff) AM_RAMBANK(12)
	AM_RANGE(0x8000, 0x9fff) AM_RAMBANK(13)
	AM_RANGE(0xa000, 0xbfff) AM_RAMBANK(14)
	AM_RANGE(0xc000, 0xdfff) AM_RAMBANK(15)
	AM_RANGE(0xe000, 0xefff) AM_WRITE(namcos1_bankswitch_w)
//  AM_RANGE(0xf000, 0xf000) AM_WRITE(MWA8_NOP) // IO Chip
	AM_RANGE(0xf200, 0xf200) AM_WRITE(namcos1_watchdog_w)
//  AM_RANGE(0xf400, 0xf400) AM_WRITE(MWA8_NOP) // ?
	AM_RANGE(0xf600, 0xf600) AM_WRITE(irq_ack_w)
	AM_RANGE(0xf800, 0xf800) AM_WRITE(firq_ack_w)
	AM_RANGE(0xe000, 0xffff) AM_ROMBANK(16)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROMBANK(17)	/* Banked ROMs */
	AM_RANGE(0x4000, 0x4001) AM_READ(YM2151_status_port_0_r)
	AM_RANGE(0x4000, 0x4000) AM_WRITE(YM2151_register_port_0_w)
	AM_RANGE(0x4001, 0x4001) AM_WRITE(YM2151_data_port_0_w)
	AM_RANGE(0x5000, 0x53ff) AM_READWRITE(namcos1_cus30_r, namcos1_cus30_w) AM_MIRROR(0x400) AM_BASE(&namco_wavedata) /* PSG ( Shared ) */
	AM_RANGE(0x7000, 0x77ff) AM_RAMBANK(18)	/* TRIRAM (shared) */
	AM_RANGE(0x8000, 0x9fff) AM_RAM	/* Sound RAM 3 */
	AM_RANGE(0xc000, 0xc001) AM_WRITE(namcos1_sound_bankswitch_w) /* ROM bank selector */
	AM_RANGE(0xd001, 0xd001) AM_WRITE(namcos1_watchdog_w)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(irq_ack_w)
	AM_RANGE(0xc000, 0xffff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( mcu_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x001f) AM_READWRITE(hd63701_internal_registers_r, hd63701_internal_registers_w)
	AM_RANGE(0x0080, 0x00ff) AM_RAM /* built in RAM */
	AM_RANGE(0x1000, 0x1003) AM_READ(dsw_r)
	AM_RANGE(0x1400, 0x1400) AM_READ(port_tag_to_handler8("CONTROL0"))
	AM_RANGE(0x1401, 0x1401) AM_READ(port_tag_to_handler8("CONTROL1"))
	AM_RANGE(0x4000, 0xbfff) AM_ROMBANK(20) /* banked ROM */
	AM_RANGE(0xc000, 0xc000) AM_WRITE(namcos1_mcu_patch_w)	/* kludge! see notes */
	AM_RANGE(0xc000, 0xc7ff) AM_RAMBANK(19)	/* TRIRAM (shared) */
	AM_RANGE(0xc800, 0xcfff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size) /* EEPROM */
	AM_RANGE(0xd000, 0xd000) AM_WRITE(namcos1_dac0_w)
	AM_RANGE(0xd400, 0xd400) AM_WRITE(namcos1_dac1_w)
	AM_RANGE(0xd800, 0xd800) AM_WRITE(namcos1_mcu_bankswitch_w) /* ROM bank selector */
	AM_RANGE(0xf000, 0xf000) AM_WRITE(irq_ack_w)
	AM_RANGE(0xf000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( mcu_port_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(HD63701_PORT1, HD63701_PORT1) AM_READWRITE(port_tag_to_handler8("COIN"), namcos1_coin_w)
	AM_RANGE(HD63701_PORT2, HD63701_PORT2) AM_READWRITE(MRA8_NOP, namcos1_dac_gain_w)
ADDRESS_MAP_END



/* Standard Namco System 1 input port definition */
INPUT_PORTS_START( ns1 )
	PORT_START_TAG( "CONTROL0" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START_TAG( "CONTROL1" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )  PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )  PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )    PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )        PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )        PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )        PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START_TAG( "DIPSW" )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG( "COIN" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SPECIAL )   /* OUT:coin lockout */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL )   /* OUT:coin counter 1 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL )   /* OUT:coin counter 2 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Service Button") PORT_CODE(KEYCODE_F1)	// service switch from the edge connector
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



INPUT_PORTS_START( shadowld )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x40, 0x40, "Freeze" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Alternate sound effects" )	// e.g. the red bird
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( dspirit )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x7f, 0x7f, "Life" )
	PORT_DIPSETTING(    0x7f, "2" )
	PORT_DIPSETTING(    0x16, "3" )
INPUT_PORTS_END


INPUT_PORTS_START( quester )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "CONTROL0" )
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNUSED )     /* paddle */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_MODIFY( "CONTROL1" )
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNUSED )     /* paddle */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x40, 0x40, "Unk 1" )	// read @ fac7
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Freeze" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Brightness" )
	PORT_DIPSETTING(    0x04, DEF_STR( Low ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Level_Select ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG( "PADDLE0" )	/* fake input port for player 1 paddle */
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(15)

	PORT_START_TAG( "PADDLE1" )	/* fake input port for player 2 paddle */
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(15) PORT_PLAYER(2)
INPUT_PORTS_END


INPUT_PORTS_START( pacmania )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x40, 0x40, "Freeze" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	/* this doesn't seem to have much use... */
	PORT_DIPNAME( 0x20, 0x20, "Kick Watchdog in IRQ" )
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	/* to enter the A.D.S. menu, set the dip switch and reset with service coin pressed */
	PORT_DIPNAME( 0x08, 0x08, "Auto Data Sampling" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( galaga88 )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x7f, 0x7f, "Auto Data Sampling" )
	PORT_DIPSETTING(    0x7f, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x57, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( berabohm )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "CONTROL0" )
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY( "CONTROL1" )
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_SPECIAL )    /* timing from the buttons interface */

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x20, 0x20, "Invulnerability" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

#ifdef PRESSURE_SENSITIVE
	/*
    buttons (pressure sensitive)
    each button has two switches: the first is closed as soon as the button is
    pressed, the second a little later, depending on how hard the button is
    pressed.
    bits 0-5 control strength (0x00 = max 0x3f = min)
    bit 6 indicates the button is pressed
    bit 7 is not actually read by the game but I use it to simulate the second
          switch
    */
	PORT_START_TAG( "IN0" )
	PORT_BIT( 0x3f, 0x00, IPT_SPECIAL )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON4 )

	PORT_START_TAG( "IN1" )
	PORT_BIT( 0x3f, 0x00, IPT_SPECIAL )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 )

	PORT_START_TAG( "IN2" )
	PORT_BIT( 0x3f, 0x00, IPT_SPECIAL )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(2)

	PORT_START_TAG( "IN3" )
	PORT_BIT( 0x3f, 0x00, IPT_SPECIAL )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
#else
	PORT_START_TAG( "IN0" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON4 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON5 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6 )

	PORT_START_TAG( "IN1" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 )

	PORT_START_TAG( "IN2" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(2)

	PORT_START_TAG( "IN3" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2)
#endif
INPUT_PORTS_END


INPUT_PORTS_START( mmaze )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x10, 0x10, "Freeze" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Level_Select ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( bakutotu )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x20, 0x20, "Show Coordinates" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Sprite Viewer" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( wldcourt )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	/* see code @ e331. The lines this draws can't even be seen because they are erased afterwards */
	PORT_DIPNAME( 0x7e, 0x7e, "Draw Debug Lines" )
	PORT_DIPSETTING(    0x7e, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x5c, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( splatter )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	/* these two don't seem to have much use... */
	PORT_DIPNAME( 0x11, 0x11, "CPU #0 Kick Watchdog in IRQ" )
	PORT_DIPSETTING(    0x11, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x06, 0x06, "CPU #0&1 Kick Watchdog in IRQ" )
	PORT_DIPSETTING(    0x06, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
INPUT_PORTS_END


INPUT_PORTS_START( faceoff )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "CONTROL0" )
	PORT_BIT( 0x7f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_MODIFY( "CONTROL1" )
	PORT_BIT( 0x7f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START_TAG( "IN0" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START_TAG( "IN1" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START_TAG( "IN2" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START_TAG( "IN3" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


INPUT_PORTS_START( ws89 )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	/* see code @ e90c. The lines this draws can't even be seen because they are erased afterwards */
	PORT_DIPNAME( 0x02, 0x02, "Draw Debug Lines" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( dangseed )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x40, 0x40, "Freeze" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	/* this doesn't seem to have much use... */
	PORT_DIPNAME( 0x20, 0x20, "Kick Watchdog in IRQ" )
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	/* to enter the A.D.S. menu, set the dip switch, keep 1p start pressed and press service coin */
	PORT_DIPNAME( 0x04, 0x04, "Auto Data Sampling" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( ws90 )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	/* see code @ e8ff. The lines this draws can't even be seen because they are erased afterwards */
	PORT_DIPNAME( 0x02, 0x02, "Draw Debug Lines" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( boxyboy )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x40, 0x40, "Freeze" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


INPUT_PORTS_START( puzlclub )
	PORT_INCLUDE( ns1 )

	PORT_MODIFY( "DIPSW" )
	PORT_DIPNAME( 0x40, 0x40, "Auto Data Sampling" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout tilelayout =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64 },
	64*8
};

static const gfx_layout spritelayout =
{
	32,32,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{  0*4,  1*4,  2*4,  3*4,  4*4,  5*4,  6*4,  7*4,
	   8*4,  9*4, 10*4, 11*4, 12*4, 13*4, 14*4, 15*4,
	 256*4,257*4,258*4,259*4,260*4,261*4,262*4,263*4,
	 264*4,265*4,266*4,267*4,268*4,269*4,270*4,271*4},
	{ 0*64, 1*64,  2*64,  3*64,  4*64,  5*64,  6*64,  7*64,
	  8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64,
	 32*64,33*64, 34*64, 35*64, 36*64, 37*64, 38*64, 39*64,
	 40*64,41*64, 42*64, 43*64, 44*64, 45*64, 46*64, 47*64 },
	32*32*4
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX2, 0, &tilelayout,   0x0800,   8 },  /* characters */
	{ REGION_GFX3, 0, &spritelayout, 0x0000, 128 },  /* sprites 32/16/8/4 dots */
	{ -1 } /* end of array */
};



static void namcos1_sound_interrupt( int irq )
{
	cpunum_set_input_line( 2, M6809_FIRQ_LINE, irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2151interface ym2151_interface =
{
	namcos1_sound_interrupt
};

static struct namco_interface namco_interface =
{
	8,          /* number of voices */
	-1,         /* memory region */
	1           /* stereo */
};

/*
    namcos1 has two 8bit dac channel. But They are mixed before pre-amp.
    And,they are connected with pre-amp through active LPF.
    LPF info : Fco = 3.3KHz , g = -12dB/oct
*/

static MACHINE_DRIVER_START( ns1 )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6809,49152000/32)
	MDRV_CPU_PROGRAM_MAP(main_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_assert,1)

	MDRV_CPU_ADD(M6809,49152000/32)
	MDRV_CPU_PROGRAM_MAP(sub_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_assert,1)

	MDRV_CPU_ADD(M6809,49152000/32)
	MDRV_CPU_PROGRAM_MAP(sound_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_assert,1)

	MDRV_CPU_ADD_TAG("MCU",HD63701,49152000/32)
	MDRV_CPU_PROGRAM_MAP(mcu_map, 0)
	MDRV_CPU_IO_MAP(mcu_port_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_assert,1)

	MDRV_FRAMES_PER_SECOND(60.606060)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	// heavy sync required to prevent CPUs from fighting for video RAM access and going into deadlocks
	MDRV_INTERLEAVE(640)

	MDRV_MACHINE_RESET(namcos1)
	MDRV_NVRAM_HANDLER(generic_0fill)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_HAS_SHADOWS | VIDEO_NEEDS_6BITS_PER_GUN)
	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_VISIBLE_AREA(9 + 8*8, 9 + 44*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(0x2000)

	MDRV_VIDEO_START(namcos1)
	MDRV_VIDEO_UPDATE(namcos1)
	MDRV_VIDEO_EOF(namcos1)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2151, 3579580)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "left", 0.50)
	MDRV_SOUND_ROUTE(1, "right", 0.50)

	MDRV_SOUND_ADD(NAMCO_CUS30, 49152000/2048/2)
	MDRV_SOUND_CONFIG(namco_interface)
	MDRV_SOUND_ROUTE(0, "left", 0.50)
	MDRV_SOUND_ROUTE(1, "right", 0.50)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)
MACHINE_DRIVER_END



/***********************************************************************

  Game drivers

***********************************************************************/
/* load half size ROM to full size space */
#define ROM_LOAD_HS(name,start,length,crc) \
	ROM_LOAD(name,start,length,crc) \
	ROM_RELOAD(start+length,length)
#define ROM_LOAD_512(name,start,crc) \
	ROM_LOAD(name,start,0x10000,crc) \
	ROM_RELOAD(start+  0x10000,0x10000) \
	ROM_RELOAD(start+2*0x10000,0x10000) \
	ROM_RELOAD(start+3*0x10000,0x10000) \
	ROM_RELOAD(start+4*0x10000,0x10000) \
	ROM_RELOAD(start+5*0x10000,0x10000) \
	ROM_RELOAD(start+6*0x10000,0x10000) \
	ROM_RELOAD(start+7*0x10000,0x10000)

#define ROM_LOAD_1024(name,start,crc) \
	ROM_LOAD(name,start,0x20000,crc) \
	ROM_RELOAD(start+  0x20000,0x20000) \
	ROM_RELOAD(start+2*0x20000,0x20000) \
	ROM_RELOAD(start+3*0x20000,0x20000)

/* Shadowland */
ROM_START( shadowld )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "yd1_s0.bin",         0x0c000, 0x10000, CRC(a9cb51fb) SHA1(c46345b36306d35f73e25d0c8b1af53936927f0b) )
	ROM_LOAD( "yd1_s1.bin",         0x1c000, 0x10000, CRC(65d1dc0d) SHA1(e758fa5279c1a36c7dad941091694daed13f8b9a) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "yd1_p0.bin",      0x000000, CRC(07e49883) SHA1(b1e720b4d134893d3d9768d3f59162b31488a079) )
	ROM_LOAD_512( "yd1_p1.bin",      0x080000, CRC(a8ea6bd3) SHA1(d8c34084c90ff9f5627d432359a1c64959372195) )
	ROM_LOAD_512( "yd1_p2.bin",      0x100000, CRC(62e5bbec) SHA1(748482389a7e49d35d6c566e9d73e3bc4ab0e7c6) )
	ROM_LOAD_512( "yd1_p3.bin",      0x180000, CRC(a4f27c24) SHA1(9a4eea9f50f62a5653015539f8933676a37a61cb) )
	/* 180000-1fffff empty */
	ROM_LOAD_512( "yd1_p5.bin",      0x280000, CRC(29a78bd6) SHA1(99f022f205dcc0d4c24bf406a61034e7a6d0cfaf) )
	ROM_LOAD_512( "yd3_p6.bin",      0x300000, CRC(93d6811c) SHA1(87de3367bb4abdb6b8e9dc986378af7d3a52e02d) )
	ROM_LOAD_512( "yd3_p7.bin",      0x380000, CRC(f1c271a0) SHA1(a5d6b856367127a1ee900e7339f29763c06029c1) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "yd_voi-0.bin",       0x10000, 0x20000, CRC(448bc6a1) SHA1(89033eb023bb770bfedf925040bbe32f4bea4937) ) // yd1.v0 + yd1.v3
	ROM_LOAD( "yd_voi-1.bin",       0x30000, 0x20000, CRC(7809035c) SHA1(d1d12db8f1d2c25545ccb92c0a2f2af2d0267161) ) // yd1.v1 + yd1.v4
	ROM_LOAD( "yd_voi-2.bin",       0x50000, 0x20000, CRC(73bffc16) SHA1(a927e503bf8650e6b638d5c357cb48586cfa025b) ) // yd1.v2 + yd1.v5

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "yd_chr-8.bin",       0x00000, 0x20000, CRC(0c8e69d0) SHA1(cedf12db2d9b14396cc8a15ccb025b96c92e190d) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "yd_chr-0.bin",       0x00000, 0x20000, CRC(717441dd) SHA1(eb4d7e8293c4e404422bec79813a782c3373ef76) )
	ROM_LOAD( "yd_chr-1.bin",       0x20000, 0x20000, CRC(c1be6e35) SHA1(910fa2bbb708811413758a07ba472ce8dc772ee4) )
	ROM_LOAD( "yd_chr-2.bin",       0x40000, 0x20000, CRC(2df8d8cc) SHA1(44b7fef479a726cad6908d33e27999d7ad679c6f) )
	ROM_LOAD( "yd_chr-3.bin",       0x60000, 0x20000, CRC(d4e15c9e) SHA1(9915abecd8f82e34961cddaeba8823d1d7a277dc) )
	ROM_LOAD( "yd_chr-4.bin",       0x80000, 0x20000, CRC(c0041e0d) SHA1(959344b1bbcf7c480fbc2d521eaa8b892651b92e) )
	ROM_LOAD( "yd_chr-5.bin",       0xa0000, 0x20000, CRC(7b368461) SHA1(025ae116b6a74eef20c629e5402b45160778a416) )
	ROM_LOAD( "yd_chr-6.bin",       0xc0000, 0x20000, CRC(3ac6a90e) SHA1(7cf13b1998976c4f73d7c36c57afde1d1a6c71ec) )
	ROM_LOAD( "yd_chr-7.bin",       0xe0000, 0x20000, CRC(8d2cffa5) SHA1(2b293ed35bb3d9a81b5dc8cb06ec3aa9260dcf27) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "yd_obj-0.bin",       0x00000, 0x20000, CRC(efb8efe3) SHA1(fad3ca72932c4318e8062708739dd80cacd2e019) )
	ROM_LOAD( "yd_obj-1.bin",       0x20000, 0x20000, CRC(bf4ee682) SHA1(45aa98737f898a0ef105c9525c2edc9a1b425da1) )
	ROM_LOAD( "yd_obj-2.bin",       0x40000, 0x20000, CRC(cb721682) SHA1(2305e5950cefe8d6b569d966728c9c5c8b4299a1) )
	ROM_LOAD( "yd_obj-3.bin",       0x60000, 0x20000, CRC(8a6c3d1c) SHA1(e9cb9c859596de753de37368d186feb857a4d08f) )
	ROM_LOAD( "yd_obj-4.bin",       0x80000, 0x20000, CRC(ef97bffb) SHA1(35bf2102cad41e50e59a31279e69c26d337f2353) )
	ROM_LOAD_HS( "yd3_obj5.bin",    0xa0000, 0x10000, CRC(1e4aa460) SHA1(c70391c3fba13e270e3cf105bb6d466b9ffedb9a) )
ROM_END

/* Youkai Douchuuki (Shadowland Japan) */
ROM_START( youkaidk )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "yd1.sd0",            0x0c000, 0x10000, CRC(a9cb51fb) SHA1(c46345b36306d35f73e25d0c8b1af53936927f0b) )
	ROM_LOAD( "yd1.sd1",            0x1c000, 0x10000, CRC(65d1dc0d) SHA1(e758fa5279c1a36c7dad941091694daed13f8b9a) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "yd1_p0.bin",      0x000000, CRC(07e49883) SHA1(b1e720b4d134893d3d9768d3f59162b31488a079) )
	ROM_LOAD_512( "yd1_p1.bin",      0x080000, CRC(a8ea6bd3) SHA1(d8c34084c90ff9f5627d432359a1c64959372195) )
	ROM_LOAD_512( "yd1_p2.bin",      0x100000, CRC(62e5bbec) SHA1(748482389a7e49d35d6c566e9d73e3bc4ab0e7c6) )
	ROM_LOAD_512( "yd1_p3.bin",      0x180000, CRC(a4f27c24) SHA1(9a4eea9f50f62a5653015539f8933676a37a61cb) )
	/* 180000-1fffff empty */
	ROM_LOAD_512( "yd1_p5.bin",      0x280000, CRC(29a78bd6) SHA1(99f022f205dcc0d4c24bf406a61034e7a6d0cfaf) )
	ROM_LOAD_512( "yd1_p6.bin",      0x300000, CRC(785a2772) SHA1(80c0a628bc834ff03460188b7fc63b6464c09476) )
	ROM_LOAD_512( "yd2_p7b.bin",     0x380000, CRC(a05bf3ae) SHA1(3477eee9a09b1998e72b31f19a92c89c5033b0f7) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "yd_voi-0.bin",       0x10000, 0x20000, CRC(448bc6a1) SHA1(89033eb023bb770bfedf925040bbe32f4bea4937) ) // yd1.v0 + yd1.v3
	ROM_LOAD( "yd_voi-1.bin",       0x30000, 0x20000, CRC(7809035c) SHA1(d1d12db8f1d2c25545ccb92c0a2f2af2d0267161) ) // yd1.v1 + yd1.v4
	ROM_LOAD( "yd_voi-2.bin",       0x50000, 0x20000, CRC(73bffc16) SHA1(a927e503bf8650e6b638d5c357cb48586cfa025b) ) // yd1.v2 + yd1.v5

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "yd_chr-8.bin",       0x00000, 0x20000, CRC(0c8e69d0) SHA1(cedf12db2d9b14396cc8a15ccb025b96c92e190d) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "yd_chr-0.bin",       0x00000, 0x20000, CRC(717441dd) SHA1(eb4d7e8293c4e404422bec79813a782c3373ef76) )
	ROM_LOAD( "yd_chr-1.bin",       0x20000, 0x20000, CRC(c1be6e35) SHA1(910fa2bbb708811413758a07ba472ce8dc772ee4) )
	ROM_LOAD( "yd_chr-2.bin",       0x40000, 0x20000, CRC(2df8d8cc) SHA1(44b7fef479a726cad6908d33e27999d7ad679c6f) )
	ROM_LOAD( "yd_chr-3.bin",       0x60000, 0x20000, CRC(d4e15c9e) SHA1(9915abecd8f82e34961cddaeba8823d1d7a277dc) )
	ROM_LOAD( "yd_chr-4.bin",       0x80000, 0x20000, CRC(c0041e0d) SHA1(959344b1bbcf7c480fbc2d521eaa8b892651b92e) )
	ROM_LOAD( "yd_chr-5.bin",       0xa0000, 0x20000, CRC(7b368461) SHA1(025ae116b6a74eef20c629e5402b45160778a416) )
	ROM_LOAD( "yd_chr-6.bin",       0xc0000, 0x20000, CRC(3ac6a90e) SHA1(7cf13b1998976c4f73d7c36c57afde1d1a6c71ec) )
	ROM_LOAD( "yd_chr-7.bin",       0xe0000, 0x20000, CRC(8d2cffa5) SHA1(2b293ed35bb3d9a81b5dc8cb06ec3aa9260dcf27) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "yd_obj-0.bin",       0x00000, 0x20000, CRC(efb8efe3) SHA1(fad3ca72932c4318e8062708739dd80cacd2e019) )
	ROM_LOAD( "yd_obj-1.bin",       0x20000, 0x20000, CRC(bf4ee682) SHA1(45aa98737f898a0ef105c9525c2edc9a1b425da1) )
	ROM_LOAD( "yd_obj-2.bin",       0x40000, 0x20000, CRC(cb721682) SHA1(2305e5950cefe8d6b569d966728c9c5c8b4299a1) )
	ROM_LOAD( "yd_obj-3.bin",       0x60000, 0x20000, CRC(8a6c3d1c) SHA1(e9cb9c859596de753de37368d186feb857a4d08f) )
	ROM_LOAD( "yd_obj-4.bin",       0x80000, 0x20000, CRC(ef97bffb) SHA1(35bf2102cad41e50e59a31279e69c26d337f2353) )
ROM_END

/* Youkai Douchuuki (Shadowland Japan old version) */
ROM_START( yokaidko )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "yd1.sd0",            0x0c000, 0x10000, CRC(a9cb51fb) SHA1(c46345b36306d35f73e25d0c8b1af53936927f0b) )
	ROM_LOAD( "yd1.sd1",            0x1c000, 0x10000, CRC(65d1dc0d) SHA1(e758fa5279c1a36c7dad941091694daed13f8b9a) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "yd1_p0.bin",      0x000000, CRC(07e49883) SHA1(b1e720b4d134893d3d9768d3f59162b31488a079) )
	ROM_LOAD_512( "yd1_p1.bin",      0x080000, CRC(a8ea6bd3) SHA1(d8c34084c90ff9f5627d432359a1c64959372195) )
	ROM_LOAD_512( "yd1_p2.bin",      0x100000, CRC(62e5bbec) SHA1(748482389a7e49d35d6c566e9d73e3bc4ab0e7c6) )
	ROM_LOAD_512( "yd1_p3.bin",      0x180000, CRC(a4f27c24) SHA1(9a4eea9f50f62a5653015539f8933676a37a61cb) )
	/* 180000-1fffff empty */
	ROM_LOAD_512( "yd1_p5.bin",      0x280000, CRC(29a78bd6) SHA1(99f022f205dcc0d4c24bf406a61034e7a6d0cfaf) )
	ROM_LOAD_512( "yd1_p6.bin",      0x300000, CRC(785a2772) SHA1(80c0a628bc834ff03460188b7fc63b6464c09476) )
	ROM_LOAD_512( "yd2_p7.bin",      0x380000, CRC(3d39098c) SHA1(acdb5ea53358676d1b71c2a456cabaa9e46aed3f) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "yd_voi-0.bin",       0x10000, 0x20000, CRC(448bc6a1) SHA1(89033eb023bb770bfedf925040bbe32f4bea4937) ) // yd1.v0 + yd1.v3
	ROM_LOAD( "yd_voi-1.bin",       0x30000, 0x20000, CRC(7809035c) SHA1(d1d12db8f1d2c25545ccb92c0a2f2af2d0267161) ) // yd1.v1 + yd1.v4
	ROM_LOAD( "yd_voi-2.bin",       0x50000, 0x20000, CRC(73bffc16) SHA1(a927e503bf8650e6b638d5c357cb48586cfa025b) ) // yd1.v2 + yd1.v5

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "yd_chr-8.bin",       0x00000, 0x20000, CRC(0c8e69d0) SHA1(cedf12db2d9b14396cc8a15ccb025b96c92e190d) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "yd_chr-0.bin",       0x00000, 0x20000, CRC(717441dd) SHA1(eb4d7e8293c4e404422bec79813a782c3373ef76) )
	ROM_LOAD( "yd_chr-1.bin",       0x20000, 0x20000, CRC(c1be6e35) SHA1(910fa2bbb708811413758a07ba472ce8dc772ee4) )
	ROM_LOAD( "yd_chr-2.bin",       0x40000, 0x20000, CRC(2df8d8cc) SHA1(44b7fef479a726cad6908d33e27999d7ad679c6f) )
	ROM_LOAD( "yd_chr-3.bin",       0x60000, 0x20000, CRC(d4e15c9e) SHA1(9915abecd8f82e34961cddaeba8823d1d7a277dc) )
	ROM_LOAD( "yd_chr-4.bin",       0x80000, 0x20000, CRC(c0041e0d) SHA1(959344b1bbcf7c480fbc2d521eaa8b892651b92e) )
	ROM_LOAD( "yd_chr-5.bin",       0xa0000, 0x20000, CRC(7b368461) SHA1(025ae116b6a74eef20c629e5402b45160778a416) )
	ROM_LOAD( "yd_chr-6.bin",       0xc0000, 0x20000, CRC(3ac6a90e) SHA1(7cf13b1998976c4f73d7c36c57afde1d1a6c71ec) )
	ROM_LOAD( "yd_chr-7.bin",       0xe0000, 0x20000, CRC(8d2cffa5) SHA1(2b293ed35bb3d9a81b5dc8cb06ec3aa9260dcf27) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "yd_obj-0.bin",       0x00000, 0x20000, CRC(efb8efe3) SHA1(fad3ca72932c4318e8062708739dd80cacd2e019) )
	ROM_LOAD( "yd_obj-1.bin",       0x20000, 0x20000, CRC(bf4ee682) SHA1(45aa98737f898a0ef105c9525c2edc9a1b425da1) )
	ROM_LOAD( "yd_obj-2.bin",       0x40000, 0x20000, CRC(cb721682) SHA1(2305e5950cefe8d6b569d966728c9c5c8b4299a1) )
	ROM_LOAD( "yd_obj-3.bin",       0x60000, 0x20000, CRC(8a6c3d1c) SHA1(e9cb9c859596de753de37368d186feb857a4d08f) )
	ROM_LOAD( "yd_obj-4.bin",       0x80000, 0x20000, CRC(ef97bffb) SHA1(35bf2102cad41e50e59a31279e69c26d337f2353) )
ROM_END

/* Dragon Spirit */
ROM_START( dspirit )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "ds1_s0.bin",         0x0c000, 0x10000, CRC(27100065) SHA1(e8fbacaa43a5b858fce2ca3b579b90c1e016396b) )
	ROM_LOAD( "ds1_s1.bin",         0x1c000, 0x10000, CRC(b398645f) SHA1(e83208e2aea7b57b4a26f123a43c112e30495aca) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "ds1_p0.bin",      0x000000, CRC(b22a2856) SHA1(8dea168e341460757c924bb510df4d4e9cdd908d) )
	ROM_LOAD_512( "ds1_p1.bin",      0x080000, CRC(f7e3298a) SHA1(76c924ed1311e7e292bd67f57c1e831054625bb6) )
	ROM_LOAD_512( "ds1_p2.bin",      0x100000, CRC(3c9b0100) SHA1(1def48a28b68e1e36cd1a165eb7127b05982c54d) )
	ROM_LOAD_512( "ds1_p3.bin",      0x180000, CRC(c6e5954b) SHA1(586fc108f264e91a4bbbb05153dd1aa19be81b5b) )
	ROM_LOAD_512( "ds1_p4.bin",      0x200000, CRC(f3307870) SHA1(a85d28c5dc55cbfa6c384d71e724db44c547d976) )
	ROM_LOAD_512( "ds1_p5.bin",      0x280000, CRC(9a3a1028) SHA1(505808834677c433e0a4cfbf387b2874e2d0fc47) )
	ROM_LOAD_512( "ds3_p6.bin",      0x300000, CRC(fcc01bd1) SHA1(dd95388d2ccc5ab51b86da2242776dc82ac86901) )
	ROM_LOAD_512( "ds3_p7.bin",      0x380000, CRC(820bedb2) SHA1(d05254c982635f9d184959065aacb10a077fcd34) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "ds1_v0.bin",      0x10000, 0x10000, CRC(313b3508) SHA1(4770fb28b45abc4967534face0bf6794d30df71a) )
	ROM_LOAD( "ds_voi-1.bin",       0x30000, 0x20000, CRC(54790d4e) SHA1(d327b1c65c487dd691389920789c59f0eb1ecee1) )
	ROM_LOAD( "ds_voi-2.bin",       0x50000, 0x20000, CRC(05298534) SHA1(5ffd9018a5c1d5ce992dd3c7575b5e25945f14fa) )
	ROM_LOAD( "ds_voi-3.bin",       0x70000, 0x20000, CRC(13e84c7e) SHA1(6ad0eb50eb3312f614a891ae8d66faca6b48d204) )
	ROM_LOAD( "ds_voi-4.bin",       0x90000, 0x20000, CRC(34fbb8cd) SHA1(3f56f136e9d54d45924802f7149bfbc319e0933a) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "ds_chr-8.bin",       0x00000, 0x20000, CRC(946eb242) SHA1(6964fff430fe306c575ff07e4c054c70aa7d96ca) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "ds_chr-0.bin",       0x00000, 0x20000, CRC(7bf28ac3) SHA1(af42b568be0c3919dbbc0e0c7a9d5ea39f84481a) )
	ROM_LOAD( "ds_chr-1.bin",       0x20000, 0x20000, CRC(03582fea) SHA1(c6705702bdb55ec6f993fc2582f116a8729c14d0) )
	ROM_LOAD( "ds_chr-2.bin",       0x40000, 0x20000, CRC(5e05f4f9) SHA1(ededa62c261ca75fc5e79c80840fc15b27cb3989) )
	ROM_LOAD( "ds_chr-3.bin",       0x60000, 0x20000, CRC(dc540791) SHA1(02c5c44bae878b22608cc6f9b09b0dd97c03e51a) )
	ROM_LOAD( "ds_chr-4.bin",       0x80000, 0x20000, CRC(ffd1f35c) SHA1(39f19676edc01059dfcee6b5c04527da3ef78ffa) )
	ROM_LOAD( "ds_chr-5.bin",       0xa0000, 0x20000, CRC(8472e0a3) SHA1(cece9fec70421fb09107890f108fb47ea770890d) )
	ROM_LOAD( "ds_chr-6.bin",       0xc0000, 0x20000, CRC(a799665a) SHA1(70cd630e63714b544f3f61a6663a41e3439639d5) )
	ROM_LOAD( "ds_chr-7.bin",       0xe0000, 0x20000, CRC(a51724af) SHA1(85728add3f773c4d814202208bee3d9a053e50de) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "ds_obj-0.bin",       0x00000, 0x20000, CRC(03ec3076) SHA1(9926ec974ba3472d765531f6add4a8d6e398c46e) )
	ROM_LOAD( "ds_obj-1.bin",       0x20000, 0x20000, CRC(e67a8fa4) SHA1(c45070b929f3e4df0e3809cf33cb4538b0067464) )
	ROM_LOAD( "ds_obj-2.bin",       0x40000, 0x20000, CRC(061cd763) SHA1(df079052fddb60ea5618ed5a6c41fb4db0e313af) )
	ROM_LOAD( "ds_obj-3.bin",       0x60000, 0x20000, CRC(63225a09) SHA1(8fffcd5b6baaa9ced16fbac58f6a3eeb11183c8b) )
	ROM_LOAD_HS( "ds1_o4.bin",      0x80000, 0x10000, CRC(a6246fcb) SHA1(39de4fdb175ab16b791cdc5d757ff7bfee8e3d2b) )
ROM_END

/* Dragon Spirit (old version) */
ROM_START( dspirito )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "ds1_s0.bin",         0x0c000, 0x10000, CRC(27100065) SHA1(e8fbacaa43a5b858fce2ca3b579b90c1e016396b) )
	ROM_LOAD( "ds1_s1.bin",         0x1c000, 0x10000, CRC(b398645f) SHA1(e83208e2aea7b57b4a26f123a43c112e30495aca) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "ds1_p0.bin",      0x000000, CRC(b22a2856) SHA1(8dea168e341460757c924bb510df4d4e9cdd908d) )
	ROM_LOAD_512( "ds1_p1.bin",      0x080000, CRC(f7e3298a) SHA1(76c924ed1311e7e292bd67f57c1e831054625bb6) )
	ROM_LOAD_512( "ds1_p2.bin",      0x100000, CRC(3c9b0100) SHA1(1def48a28b68e1e36cd1a165eb7127b05982c54d) )
	ROM_LOAD_512( "ds1_p3.bin",      0x180000, CRC(c6e5954b) SHA1(586fc108f264e91a4bbbb05153dd1aa19be81b5b) )
	ROM_LOAD_512( "ds1_p4.bin",      0x200000, CRC(f3307870) SHA1(a85d28c5dc55cbfa6c384d71e724db44c547d976) )
	ROM_LOAD_512( "ds1_p5.bin",      0x280000, CRC(9a3a1028) SHA1(505808834677c433e0a4cfbf387b2874e2d0fc47) )
	ROM_LOAD_512( "ds1_p6.bin",      0x300000, CRC(a82737b4) SHA1(13865eb05a5d7b5cf06316ad8e71da3abbad335a) )
	ROM_LOAD_512( "ds1_p7.bin",      0x380000, CRC(f4c0d75e) SHA1(87ac334b5d66b9b66ee0d4fe222afb76ff190534) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "ds1_v0.bin",      0x10000, 0x10000, CRC(313b3508) SHA1(4770fb28b45abc4967534face0bf6794d30df71a) )
	ROM_LOAD( "ds_voi-1.bin",       0x30000, 0x20000, CRC(54790d4e) SHA1(d327b1c65c487dd691389920789c59f0eb1ecee1) )
	ROM_LOAD( "ds_voi-2.bin",       0x50000, 0x20000, CRC(05298534) SHA1(5ffd9018a5c1d5ce992dd3c7575b5e25945f14fa) )
	ROM_LOAD( "ds_voi-3.bin",       0x70000, 0x20000, CRC(13e84c7e) SHA1(6ad0eb50eb3312f614a891ae8d66faca6b48d204) )
	ROM_LOAD( "ds_voi-4.bin",       0x90000, 0x20000, CRC(34fbb8cd) SHA1(3f56f136e9d54d45924802f7149bfbc319e0933a) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "ds_chr-8.bin",       0x00000, 0x20000, CRC(946eb242) SHA1(6964fff430fe306c575ff07e4c054c70aa7d96ca) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "ds_chr-0.bin",       0x00000, 0x20000, CRC(7bf28ac3) SHA1(af42b568be0c3919dbbc0e0c7a9d5ea39f84481a) )
	ROM_LOAD( "ds_chr-1.bin",       0x20000, 0x20000, CRC(03582fea) SHA1(c6705702bdb55ec6f993fc2582f116a8729c14d0) )
	ROM_LOAD( "ds_chr-2.bin",       0x40000, 0x20000, CRC(5e05f4f9) SHA1(ededa62c261ca75fc5e79c80840fc15b27cb3989) )
	ROM_LOAD( "ds_chr-3.bin",       0x60000, 0x20000, CRC(dc540791) SHA1(02c5c44bae878b22608cc6f9b09b0dd97c03e51a) )
	ROM_LOAD( "ds_chr-4.bin",       0x80000, 0x20000, CRC(ffd1f35c) SHA1(39f19676edc01059dfcee6b5c04527da3ef78ffa) )
	ROM_LOAD( "ds_chr-5.bin",       0xa0000, 0x20000, CRC(8472e0a3) SHA1(cece9fec70421fb09107890f108fb47ea770890d) )
	ROM_LOAD( "ds_chr-6.bin",       0xc0000, 0x20000, CRC(a799665a) SHA1(70cd630e63714b544f3f61a6663a41e3439639d5) )
	ROM_LOAD( "ds_chr-7.bin",       0xe0000, 0x20000, CRC(a51724af) SHA1(85728add3f773c4d814202208bee3d9a053e50de) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "ds_obj-0.bin",       0x00000, 0x20000, CRC(03ec3076) SHA1(9926ec974ba3472d765531f6add4a8d6e398c46e) )
	ROM_LOAD( "ds_obj-1.bin",       0x20000, 0x20000, CRC(e67a8fa4) SHA1(c45070b929f3e4df0e3809cf33cb4538b0067464) )
	ROM_LOAD( "ds_obj-2.bin",       0x40000, 0x20000, CRC(061cd763) SHA1(df079052fddb60ea5618ed5a6c41fb4db0e313af) )
	ROM_LOAD( "ds_obj-3.bin",       0x60000, 0x20000, CRC(63225a09) SHA1(8fffcd5b6baaa9ced16fbac58f6a3eeb11183c8b) )
	ROM_LOAD_HS( "ds1_o4.bin",      0x80000, 0x10000, CRC(a6246fcb) SHA1(39de4fdb175ab16b791cdc5d757ff7bfee8e3d2b) )
ROM_END

/* Blazer */
ROM_START( blazer )
	ROM_REGION( 0x1c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "bz1_s0.bin",         0x0c000, 0x10000, CRC(6c3a580b) SHA1(2b76ea0005245e30eb72fba3b044a885e47d588d) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512 ( "bz1_p0.bin",      0x000000, CRC(a7dd195b) SHA1(bd867ca54d25a4045c1f0a2bfd6c673982f88033) )
	ROM_LOAD_512 ( "bz1_p1.bin",      0x080000, CRC(c54bbbf4) SHA1(82ec5b72203a80b44657bee73d4a7a3e522a86ae) )
	ROM_LOAD_512 ( "bz1_p2.bin",      0x100000, CRC(5d700aed) SHA1(13ee900e73137dd5f09d54f2ee97faf696b16b8f) )
	ROM_LOAD_512 ( "bz1_p3.bin",      0x180000, CRC(81b32a1a) SHA1(cb691adadd24667a6ae0a31531f4025ee26b96b9) )
	ROM_LOAD_1024( "bz_prg-4.bin",    0x200000, CRC(65ef6f05) SHA1(1da6bca6a095496a38ca9be8730d62f2978d69d9) )
	ROM_LOAD_1024( "bz_prg-5.bin",    0x280000, CRC(900da191) SHA1(0f31584f623f8c48aa2d8b670da79f8f36af0d92) )
	ROM_LOAD_1024( "bz_prg-6.bin",    0x300000, CRC(81c48fc0) SHA1(3b93465e707d19d9eb4a1f2cef142a9ca06edf01) )
	ROM_LOAD_512 ( "bz1_p7.bin",      0x380000, CRC(2d4cbb95) SHA1(86918ccf5c3b59061891253d3419267a38b187b1) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "bz1_v0.bin",      0x10000, 0x10000, CRC(3d09d32e) SHA1(d29c6140f871633b5fbb6849b003629ff22e13f6) )
	ROM_LOAD( "bz_voi-1.bin",       0x30000, 0x20000, CRC(2043b141) SHA1(f8be0584026365e092be37fffa2e52a6a2c3ff0b) )
	ROM_LOAD( "bz_voi-2.bin",       0x50000, 0x20000, CRC(64143442) SHA1(bee3b98c0289b7c443450c551d791f7ffcee0b60) )
	ROM_LOAD( "bz_voi-3.bin",       0x70000, 0x20000, CRC(26cfc510) SHA1(749680eaf3072db5331cc76a21cd022c50f95647) )
	ROM_LOAD( "bz_voi-4.bin",       0x90000, 0x20000, CRC(d206b1bd) SHA1(32702fa67339ab337a2a70946e3861420a07b11b) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "bz_chr-8.bin",       0x00000, 0x20000, CRC(db28bfca) SHA1(510dd204da389db7eb5d9ce34dc0daf60bad1219) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "bz_chr-0.bin",       0x00000, 0x20000, CRC(d346ba61) SHA1(eaae8557e77574a7692319b8819bd5078855ddc0) )
	ROM_LOAD( "bz_chr-1.bin",       0x20000, 0x20000, CRC(e45eb2ea) SHA1(b3a7b7df93544c6b76b4000d55e178b52f46c5c8) )
	ROM_LOAD( "bz_chr-2.bin",       0x40000, 0x20000, CRC(599079ee) SHA1(ceb57e5f352a740fc2a90175a73ef318cd3ffb6f) )
	ROM_LOAD( "bz_chr-3.bin",       0x60000, 0x20000, CRC(d5182e36) SHA1(1a913247d13c8ea9f9969ea7e2c69b17f15ba5ba) )
	ROM_LOAD( "bz_chr-4.bin",       0x80000, 0x20000, CRC(e788259e) SHA1(6654d424c1d6c9fd44b72603ae870ef3692af413) )
	ROM_LOAD( "bz_chr-5.bin",       0xa0000, 0x20000, CRC(107e6814) SHA1(b5b85e8e6995330c965e568743fa86f723231bbb) )
	ROM_LOAD( "bz_chr-6.bin",       0xc0000, 0x20000, CRC(0312e2ba) SHA1(c7332dddca71d819987d0c723bad35a39ed54d42) )
	ROM_LOAD( "bz_chr-7.bin",       0xe0000, 0x20000, CRC(d9d9a90f) SHA1(f62ea399afa38b2c44f6aa51bcf9a49151ccf70a) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "bz_obj-0.bin",       0x00000, 0x20000, CRC(22aee927) SHA1(f8cc0ea6469c2dec9c853269c3bf40e90c32d806) )
	ROM_LOAD( "bz_obj-1.bin",       0x20000, 0x20000, CRC(7cb10112) SHA1(83bc9af6d72b2d7894ac632ea486c2f603f8d08f) )
	ROM_LOAD( "bz_obj-2.bin",       0x40000, 0x20000, CRC(34b23bb7) SHA1(fb288d089270b0e41fccd0da03e019eb9e8cad0c) )
	ROM_LOAD( "bz_obj-3.bin",       0x60000, 0x20000, CRC(9bc1db71) SHA1(36faaec83decb9d92bb9ffe5c8a482a168de18d8) )
	ROM_FILL(                       0x80000, 0x80000, 0xff )
ROM_END

/* Quester */
ROM_START( quester )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "qs1_s0.bin",         0x0c000, 0x10000, CRC(c2ef3af9) SHA1(aa0766aad450660e216d817e41e030141e8d1f48) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	ROM_LOAD_512( "qs1_p5.bin",      0x280000, CRC(c8e11f30) SHA1(33589ceb723c3b3cfbe36d58ca9426d26bd28568) )
	/* 300000-37ffff empty */
	ROM_LOAD_512( "qs1_p7b.bin",     0x380000, CRC(f358a944) SHA1(317adf846c6dbf6d00b13020b5078e2263056f93) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "qs1_v0.bin",      0x10000, 0x10000, CRC(6a2f3038) SHA1(00870da9b7f65536ff052c32da2d553f8c6b994b) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "qs1_c8.bin",         0x00000, 0x10000, CRC(06730d54) SHA1(53d79c27e2f1b192b1de781b6b5024eb1e8126ad) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "qs1_c0.bin",         0x00000, 0x20000, CRC(ca69bd7a) SHA1(98130f06e3d7b480b76bf28a4e252b4aaa1e1b3e) )
	ROM_LOAD( "qs1_c1.bin",         0x20000, 0x20000, CRC(d660ba71) SHA1(738d225c0e2b91c785fde0c33a0520a672933659) )
	ROM_LOAD( "qs1_c2.bin",         0x40000, 0x20000, CRC(4686f656) SHA1(8628a18bf7154b8edaf7cfbee2d8881a28690bff) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "qs1_o0.bin",         0x00000, 0x10000, CRC(e24f0bf1) SHA1(31f37f853fe27c24cfeaa059f8959dfea37911cb) )
	ROM_LOAD( "qs1_o1.bin",         0x20000, 0x10000, CRC(e4aab0ca) SHA1(e4765dd369b02492dbb9955cc082f24665a01635) )
ROM_END

/* Pac-Mania */
ROM_START( pacmania )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "pnx_s0.bin",         0x0c000, 0x10000, CRC(c10370fa) SHA1(f819a31075d3c8df5deee2919cd446b9e678c47d) )
	ROM_LOAD( "pnx_s1.bin",         0x1c000, 0x10000, CRC(f761ed5a) SHA1(1487932c86a6094ed01d5032904fd7ae3435d09c) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	ROM_LOAD_1024( "pn_prg-6.bin",    0x300000, CRC(fe94900c) SHA1(5ce726baafc5ed24ea4cae33232c97637afb486b) )
	ROM_LOAD_512 ( "pnx_p7.bin",      0x380000, CRC(462fa4fd) SHA1(b27bee1ac64ac204c85703c3822de7dbda11b75e) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "pnx_v0.bin",      0x10000, 0x10000, CRC(1ad5788f) SHA1(f6b1ccdcc3db11c0ab83e3ff24e772cd2b491468) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "pn1_c8.bin",         0x00000, 0x10000, CRC(f3afd65d) SHA1(51daefd8685b49c464130b9e7d93e31cfdda724e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "pn_chr-0.bin",       0x00000, 0x20000, CRC(7c57644c) SHA1(77d9cc9ffbed47a941e4c5e5645d7d1126f6c302) )
	ROM_LOAD( "pn_chr-1.bin",       0x20000, 0x20000, CRC(7eaa67ed) SHA1(4ad6cfa31d781fa7169663bbc319fc79d8965290) )
	ROM_LOAD( "pn_chr-2.bin",       0x40000, 0x20000, CRC(27e739ac) SHA1(be9dbd22d988b76f7044328e2056c3f26b703401) )
	ROM_LOAD( "pn_chr-3.bin",       0x60000, 0x20000, CRC(1dfda293) SHA1(fa01b0b8a820c1d24ff0f84b857d2784f0dac1cf) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "pn_obj-0.bin",       0x00000, 0x20000, CRC(fda57e8b) SHA1(16a72585268159d24f881dbb1aa6ae82bcc4bde7) )
	ROM_LOAD( "pnx_obj1.bin",       0x20000, 0x20000, CRC(4c08affe) SHA1(f8f60fdc31779e2abe496e36ba0e4f27546cbc54) )
ROM_END

/* Pac-Mania (Japan) diff o1,s0,s1,p7,v0 */
ROM_START( pacmanij )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "pn1_s0.bin",         0x0c000, 0x10000, CRC(d5ef5eee) SHA1(6f263955662defe7a03cc89368b70d5fcb06ee3e) )
	ROM_LOAD( "pn1_s1.bin",         0x1c000, 0x10000, CRC(411bc134) SHA1(89960596def3580d19d9121d1efffbba2d1bdd94) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	ROM_LOAD_1024( "pn_prg-6.bin",    0x300000, CRC(fe94900c) SHA1(5ce726baafc5ed24ea4cae33232c97637afb486b) )
	ROM_LOAD_512 ( "pn1_p7.bin",      0x380000, CRC(2aa99e2b) SHA1(1d5e8ce6eac03696d51b32c1d0f6c3e82f604422) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "pn1_v0.bin",      0x10000, 0x10000, CRC(e2689f79) SHA1(b88e3435f2932901cc0a3b379b31a764bb9b2e2b) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "pn1_c8.bin",         0x00000, 0x10000, CRC(f3afd65d) SHA1(51daefd8685b49c464130b9e7d93e31cfdda724e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "pn_chr-0.bin",       0x00000, 0x20000, CRC(7c57644c) SHA1(77d9cc9ffbed47a941e4c5e5645d7d1126f6c302) )
	ROM_LOAD( "pn_chr-1.bin",       0x20000, 0x20000, CRC(7eaa67ed) SHA1(4ad6cfa31d781fa7169663bbc319fc79d8965290) )
	ROM_LOAD( "pn_chr-2.bin",       0x40000, 0x20000, CRC(27e739ac) SHA1(be9dbd22d988b76f7044328e2056c3f26b703401) )
	ROM_LOAD( "pn_chr-3.bin",       0x60000, 0x20000, CRC(1dfda293) SHA1(fa01b0b8a820c1d24ff0f84b857d2784f0dac1cf) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "pn_obj-0.bin",       0x00000, 0x20000, CRC(fda57e8b) SHA1(16a72585268159d24f881dbb1aa6ae82bcc4bde7) )
	ROM_LOAD( "pn_obj-1.bin",       0x20000, 0x20000, CRC(27bdf440) SHA1(0be16dc73590eb71090e6a0e6ddd6e7f4f3dbfba) )
ROM_END

/* Galaga '88 */
ROM_START( galaga88 )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "g81_s0.bin",         0x0c000, 0x10000, CRC(164a3fdc) SHA1(d7b026f6a617bb444e3bce80cec2cbb4772cb533) )
	ROM_LOAD( "g81_s1.bin",         0x1c000, 0x10000, CRC(16a4b784) SHA1(a0d6f6ad4a68c9e10f2662e940ffaee691cafcac) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "g81_p0.bin",      0x000000, CRC(0f0778ca) SHA1(17cc03c6ff138cf947dafe05dc0759ff968a399e) )
	ROM_LOAD_512( "g81_p1.bin",      0x080000, CRC(e68cb351) SHA1(1087c0d9a53f3a4d238f19d479856b502bde7b77) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	ROM_LOAD_512( "g81_p5.bin",      0x280000, CRC(4fbd3f6c) SHA1(40d8dadc0a36b4c1886778cfc8d380a34aea2505) )
	ROM_LOAD_512( "g8x_p6.bin",      0x300000, CRC(403d01c1) SHA1(86109087b10c4fbcc940df6a84f7546de56303d2) )
	ROM_LOAD_512( "g8x_p7.bin",      0x380000, CRC(df75b7fc) SHA1(cb810e7ba05bd8e873559e529e25a99adbf6307d) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "g81_v0.bin",      0x10000, 0x10000, CRC(86921dd4) SHA1(7048fd5b6ed5f4ddf6788958c30604418a6613ff) )
	ROM_LOAD_HS( "g81_v1.bin",      0x30000, 0x10000, CRC(9c300e16) SHA1(6f3c82dc83290426068acef0b8fabba452421e8f) )
	ROM_LOAD_HS( "g81_v2.bin",      0x50000, 0x10000, CRC(5316b4b0) SHA1(353c06e0e7c8dd9d609f8b341663bbf0ca60f6b5) )
	ROM_LOAD_HS( "g81_v3.bin",      0x70000, 0x10000, CRC(dc077af4) SHA1(560090a335dfd345a6ae0eef8f1fd4d8098881f3) )
	ROM_LOAD_HS( "g81_v4.bin",      0x90000, 0x10000, CRC(ac0279a7) SHA1(8d25292eec9953516fc5d25a94e30acc8159b360) )
	ROM_LOAD_HS( "g81_v5.bin",      0xb0000, 0x10000, CRC(014ddba1) SHA1(26590b77a0c386dc076a8f8eccf6244c7e5a1e10) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "g8_chr-8.bin",       0x00000, 0x20000, CRC(3862ed0a) SHA1(4cae42bbfa434c7dce63fdceaa569fcb28768420) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "g8_chr-0.bin",       0x00000, 0x20000, CRC(68559c78) SHA1(28f6284acbf1fc263c2d38ae464ee77f367b0af5) )
	ROM_LOAD( "g8_chr-1.bin",       0x20000, 0x20000, CRC(3dc0f93f) SHA1(0db9f37cf6e06013b402df23e615b0ab0d32b9ee) )
	ROM_LOAD( "g8_chr-2.bin",       0x40000, 0x20000, CRC(dbf26f1f) SHA1(e52723647a8fe6db0b9c5e11c02486b20a549506) )
	ROM_LOAD( "g8_chr-3.bin",       0x60000, 0x20000, CRC(f5d6cac5) SHA1(3d098b8219de4a7729ec95547eebff17c9b505b9) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "g8_obj-0.bin",       0x00000, 0x20000, CRC(d7112e3f) SHA1(476f9e1b636b257e517fc789508dac923d05ef67) )
	ROM_LOAD( "g8_obj-1.bin",       0x20000, 0x20000, CRC(680db8e7) SHA1(84a68c27aaae27c0540f68f9c7d490a416c8f027) )
	ROM_LOAD( "g8_obj-2.bin",       0x40000, 0x20000, CRC(13c97512) SHA1(9c5f39bcfe28abe1faa67bbe829a61fbcec98ec8) )
	ROM_LOAD( "g8_obj-3.bin",       0x60000, 0x20000, CRC(3ed3941b) SHA1(5404aed795536ce6b37b8292d6a4446222bb50bf) )
	ROM_LOAD( "g8_obj-4.bin",       0x80000, 0x20000, CRC(370ff4ad) SHA1(61d7306325103c6a03def619c21877faadf12699) )
	ROM_LOAD( "g8_obj-5.bin",       0xa0000, 0x20000, CRC(b0645169) SHA1(e55dc9bd532b6bd821b7bf6994c35175600c317c) )
ROM_END

/* Galaga '88 (Japan) */
ROM_START( galag88j )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "g81_s0.bin",         0x0c000, 0x10000, CRC(164a3fdc) SHA1(d7b026f6a617bb444e3bce80cec2cbb4772cb533) )
	ROM_LOAD( "g81_s1.bin",         0x1c000, 0x10000, CRC(16a4b784) SHA1(a0d6f6ad4a68c9e10f2662e940ffaee691cafcac) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "g81_p0.bin",      0x000000, CRC(0f0778ca) SHA1(17cc03c6ff138cf947dafe05dc0759ff968a399e) )
	ROM_LOAD_512( "g81_p1.bin",      0x080000, CRC(e68cb351) SHA1(1087c0d9a53f3a4d238f19d479856b502bde7b77) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	ROM_LOAD_512( "g81_p5.bin",      0x280000, CRC(4fbd3f6c) SHA1(40d8dadc0a36b4c1886778cfc8d380a34aea2505) )
	ROM_LOAD_512( "g81_p6.bin",      0x300000, CRC(e7203707) SHA1(1171196029ebf0734211e1cc2521db7aa8594f31) )
	ROM_LOAD_512( "g81_p7.bin",      0x380000, CRC(7c10965d) SHA1(35f2e4ef66525c2b60975b799014d60cc15f83a5) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "g81_v0.bin",      0x10000, 0x10000, CRC(86921dd4) SHA1(7048fd5b6ed5f4ddf6788958c30604418a6613ff) )
	ROM_LOAD_HS( "g81_v1.bin",      0x30000, 0x10000, CRC(9c300e16) SHA1(6f3c82dc83290426068acef0b8fabba452421e8f) )
	ROM_LOAD_HS( "g81_v2.bin",      0x50000, 0x10000, CRC(5316b4b0) SHA1(353c06e0e7c8dd9d609f8b341663bbf0ca60f6b5) )
	ROM_LOAD_HS( "g81_v3.bin",      0x70000, 0x10000, CRC(dc077af4) SHA1(560090a335dfd345a6ae0eef8f1fd4d8098881f3) )
	ROM_LOAD_HS( "g81_v4.bin",      0x90000, 0x10000, CRC(ac0279a7) SHA1(8d25292eec9953516fc5d25a94e30acc8159b360) )
	ROM_LOAD_HS( "g81_v5.bin",      0xb0000, 0x10000, CRC(014ddba1) SHA1(26590b77a0c386dc076a8f8eccf6244c7e5a1e10) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "g8_chr-8.bin",       0x00000, 0x20000, CRC(3862ed0a) SHA1(4cae42bbfa434c7dce63fdceaa569fcb28768420) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "g8_chr-0.bin",       0x00000, 0x20000, CRC(68559c78) SHA1(28f6284acbf1fc263c2d38ae464ee77f367b0af5) )
	ROM_LOAD( "g8_chr-1.bin",       0x20000, 0x20000, CRC(3dc0f93f) SHA1(0db9f37cf6e06013b402df23e615b0ab0d32b9ee) )
	ROM_LOAD( "g8_chr-2.bin",       0x40000, 0x20000, CRC(dbf26f1f) SHA1(e52723647a8fe6db0b9c5e11c02486b20a549506) )
	ROM_LOAD( "g8_chr-3.bin",       0x60000, 0x20000, CRC(f5d6cac5) SHA1(3d098b8219de4a7729ec95547eebff17c9b505b9) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "g8_obj-0.bin",       0x00000, 0x20000, CRC(d7112e3f) SHA1(476f9e1b636b257e517fc789508dac923d05ef67) )
	ROM_LOAD( "g8_obj-1.bin",       0x20000, 0x20000, CRC(680db8e7) SHA1(84a68c27aaae27c0540f68f9c7d490a416c8f027) )
	ROM_LOAD( "g8_obj-2.bin",       0x40000, 0x20000, CRC(13c97512) SHA1(9c5f39bcfe28abe1faa67bbe829a61fbcec98ec8) )
	ROM_LOAD( "g8_obj-3.bin",       0x60000, 0x20000, CRC(3ed3941b) SHA1(5404aed795536ce6b37b8292d6a4446222bb50bf) )
	ROM_LOAD( "g8_obj-4.bin",       0x80000, 0x20000, CRC(370ff4ad) SHA1(61d7306325103c6a03def619c21877faadf12699) )
	ROM_LOAD( "g8_obj-5.bin",       0xa0000, 0x20000, CRC(b0645169) SHA1(e55dc9bd532b6bd821b7bf6994c35175600c317c) )
ROM_END

/* World Stadium */
ROM_START( ws )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "ws1_snd0.bin",       0x0c000, 0x10000, CRC(45a87810) SHA1(b6537500cc6e862d97074f636248446d6fae5d07) )
	ROM_LOAD( "ws1_snd1.bin",       0x1c000, 0x10000, CRC(31bf74c1) SHA1(ddb7a91d6f3ae93be79914a435178a540fe05bfb) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "ws1_prg0.bin",    0x000000, CRC(b0234298) SHA1(b46a70109801d85332fb6658426bd795e03f492a) )
	ROM_LOAD_512( "ws1_prg1.bin",    0x080000, CRC(dfd72bed) SHA1(5985e7112cb994b016b0027a933413d7edeba1f6) )
	ROM_LOAD_512( "ws1_prg2.bin",    0x100000, CRC(bb09fa9b) SHA1(af5223daee89cf55dceb838d2f812efd74d21d23) )
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_512( "ws1_prg7.bin",    0x380000, CRC(28712eba) SHA1(01ffb75af07eccd42426c4f4f933a3d562fdd165) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "ws1_voi0.bin",    0x10000, 0x10000, CRC(f6949199) SHA1(ef596b02060f8e58eac37765663dd16377244391) )
	ROM_LOAD( "ws_voi-1.bin",       0x30000, 0x20000, CRC(210e2af9) SHA1(f8a1f8c6b9fbb8a9b3f298674600c1fbb9c5840e) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "ws_chr-8.bin",       0x00000, 0x20000, CRC(d1897b9b) SHA1(29906614b879e5623b49bc925e80006aee3997b9) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "ws_chr-0.bin",       0x00000, 0x20000, CRC(3e3e96b4) SHA1(7b5f72e7e2aab9c78f452171ac6ebe8c634a88c4) )
	ROM_LOAD( "ws_chr-1.bin",       0x20000, 0x20000, CRC(897dfbc1) SHA1(3e7425a456bada77591abcf1567ec75a99440334) )
	ROM_LOAD( "ws_chr-2.bin",       0x40000, 0x20000, CRC(e142527c) SHA1(606206b7b9916e3f456595344b70c17d44731fbb) )
	ROM_LOAD( "ws_chr-3.bin",       0x60000, 0x20000, CRC(907d4dc8) SHA1(16c31cbccef4d8a81f2150cec874a01688e46a59) )
	ROM_LOAD( "ws_chr-4.bin",       0x80000, 0x20000, CRC(afb11e17) SHA1(cf4aa124f0d4fe737686ccda72c55c199189ca49) )
	ROM_LOAD( "ws_chr-6.bin",       0xc0000, 0x20000, CRC(a16a17c2) SHA1(52917165d50a03db8ad6783a95731e0b438c2a6c) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "ws_obj-0.bin",       0x00000, 0x20000, CRC(12dc83a6) SHA1(f3d0ebda14af81ff99642ceaa7b9c0e4a4bfbbcb) )
	ROM_LOAD( "ws_obj-1.bin",       0x20000, 0x20000, CRC(68290a46) SHA1(c05f31e1c2332cba5d5b8cafc0e9f616aadee5b7) )
	ROM_LOAD( "ws_obj-2.bin",       0x40000, 0x20000, CRC(cd5ba55d) SHA1(9b5a655909457bcffdaaa95842740a6f0f8f6cec) )
	ROM_LOAD_HS( "ws1_obj3.bin",    0x60000, 0x10000, CRC(f2ed5309) SHA1(b7d9c0a617660ecceaf7db3fd53bc0377ed1b6c1) )
ROM_END

/* Beraboh Man */
ROM_START( berabohm )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "bm1_s0.bin",         0x0c000, 0x10000, CRC(d5d53cb1) SHA1(af5db529550382dab61197eb46e02110efc4c21b) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "bm1_p0.bin",      0x000000, CRC(b57ff8c1) SHA1(8169c95e83ada1016eb070aa6b4b99b153656615) )
	ROM_LOAD_1024( "bm1_p1.bin",      0x080000, CRC(b15f6407) SHA1(7d24510a663c8c647fe52f413c580dbbd08d0ddc) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	ROM_LOAD_1024( "bm1_p4.bin",      0x200000, CRC(f6cfcb8c) SHA1(22c78c9afbe71a66d2022778309463dbe28419aa) )
	/* 280000-2fffff empty */
	ROM_LOAD_512 ( "bm1-p6.bin",      0x300000, CRC(a51b69a5) SHA1(d04a52feb95f8b65978af88bd4b338883228fd93) )
	ROM_LOAD_1024( "bm1_p7c.bin",     0x380000, CRC(9694d7b2) SHA1(2953a7029457a8afb2767560f05c064aade28bca) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "bm1_v0.bin",      0x10000, 0x10000, CRC(4e40d0ca) SHA1(799c4becd2e5877719d7a5eb9b610f91a7a637af) )
	ROM_LOAD(    "bm_voi-1.bin",    0x30000, 0x20000, CRC(be9ce0a8) SHA1(a211216125615cb14e515317f56976c4ebe13f5f) )
	ROM_LOAD_HS( "bm1_v2.bin",      0x50000, 0x10000, CRC(41225d04) SHA1(a670c5ce63ff1d2ed94aa5ea17cb2c91eb768f14) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "bm_chr-8.bin",       0x00000, 0x20000, CRC(92860e95) SHA1(d8c8d5aed956c876809f287700f33bc70a1b58a3) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "bm_chr-0.bin",       0x00000, 0x20000, CRC(eda1d92e) SHA1(b9423a9feafb08c0f3d1f359783d8e4fb17bde98) )
	ROM_LOAD( "bm_chr-1.bin",       0x20000, 0x20000, CRC(8ae1891e) SHA1(95308defaf564680028abc3a0fd406cf35e461ce) )
	ROM_LOAD( "bm_chr-2.bin",       0x40000, 0x20000, CRC(774cdf4e) SHA1(a4663dec7bfb43bee4e6aaacb3b203d71e3e3c9e) )
	ROM_LOAD( "bm_chr-3.bin",       0x60000, 0x20000, CRC(6d81e6c9) SHA1(6837adf0b82c7a6cba973932083db33a0ba0b422) )
	ROM_LOAD( "bm_chr-4.bin",       0x80000, 0x20000, CRC(f4597683) SHA1(ea585ff6e998e647580bf75dc128c1c2892947eb) )
	ROM_LOAD( "bm_chr-5.bin",       0xa0000, 0x20000, CRC(0e0abde0) SHA1(1e17767ccf55bebf681b078b405bedd47fe502d5) )
	ROM_LOAD( "bm_chr-6.bin",       0xc0000, 0x20000, CRC(4a61f08c) SHA1(1136819780e1f3ed150663d4853b6caf835e1c3e) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "bm_obj-0.bin",       0x00000, 0x20000, CRC(15724b94) SHA1(2f40425138c574a51e742ff610ce88c8a47c0c16) )
	ROM_LOAD( "bm_obj-1.bin",       0x20000, 0x20000, CRC(5d21f962) SHA1(2cfeda30dd7e10bcb66fe8858470a9db7577a59d) )
	ROM_LOAD( "bm_obj-2.bin",       0x40000, 0x20000, CRC(5d48e924) SHA1(6642b9cc632bba216f341a37325a826808aa3f34) )
	ROM_LOAD( "bm_obj-3.bin",       0x60000, 0x20000, CRC(cbe56b7f) SHA1(6387d04f0f42cde3504fb85ebb702de334bb0e5a) )
	ROM_LOAD( "bm_obj-4.bin",       0x80000, 0x20000, CRC(76dcc24c) SHA1(78deeb6efca1d2a0d52ee047bdd869cb9c9e2816) )
	ROM_LOAD( "bm_obj-5.bin",       0xa0000, 0x20000, CRC(fe70201d) SHA1(49a621e169f096572583bbbdee2ab1a7916bf95f) )
	ROM_LOAD( "bm_obj-7.bin",       0xe0000, 0x20000, CRC(377c81ed) SHA1(85f4682c6079863793f5575d261a33309c221ba7) )
ROM_END

ROM_START( beraboho )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "bm1_s0.bin",         0x0c000, 0x10000, CRC(d5d53cb1) SHA1(af5db529550382dab61197eb46e02110efc4c21b) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "bm1_p0.bin",      0x000000, CRC(b57ff8c1) SHA1(8169c95e83ada1016eb070aa6b4b99b153656615) )
	ROM_LOAD_1024( "bm1_p1.bin",      0x080000, CRC(b15f6407) SHA1(7d24510a663c8c647fe52f413c580dbbd08d0ddc) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	ROM_LOAD_1024( "bm1_p4.bin",      0x200000, CRC(f6cfcb8c) SHA1(22c78c9afbe71a66d2022778309463dbe28419aa) )
	/* 280000-2fffff empty */
	ROM_LOAD_512 ( "bm1-p6.bin",      0x300000, CRC(a51b69a5) SHA1(d04a52feb95f8b65978af88bd4b338883228fd93) )
	ROM_LOAD_1024( "bm1_p7b.bin",     0x380000, CRC(e0c36ddd) SHA1(e949da36524add3ab70d5dd5dcc7c6f42e3799e7) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "bm1_v0.bin",      0x10000, 0x10000, CRC(4e40d0ca) SHA1(799c4becd2e5877719d7a5eb9b610f91a7a637af) )
	ROM_LOAD(    "bm_voi-1.bin",    0x30000, 0x20000, CRC(be9ce0a8) SHA1(a211216125615cb14e515317f56976c4ebe13f5f) )
	ROM_LOAD_HS( "bm1_v2.bin",      0x50000, 0x10000, CRC(41225d04) SHA1(a670c5ce63ff1d2ed94aa5ea17cb2c91eb768f14) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "bm_chr-8.bin",       0x00000, 0x20000, CRC(92860e95) SHA1(d8c8d5aed956c876809f287700f33bc70a1b58a3) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "bm_chr-0.bin",       0x00000, 0x20000, CRC(eda1d92e) SHA1(b9423a9feafb08c0f3d1f359783d8e4fb17bde98) )
	ROM_LOAD( "bm_chr-1.bin",       0x20000, 0x20000, CRC(8ae1891e) SHA1(95308defaf564680028abc3a0fd406cf35e461ce) )
	ROM_LOAD( "bm_chr-2.bin",       0x40000, 0x20000, CRC(774cdf4e) SHA1(a4663dec7bfb43bee4e6aaacb3b203d71e3e3c9e) )
	ROM_LOAD( "bm_chr-3.bin",       0x60000, 0x20000, CRC(6d81e6c9) SHA1(6837adf0b82c7a6cba973932083db33a0ba0b422) )
	ROM_LOAD( "bm_chr-4.bin",       0x80000, 0x20000, CRC(f4597683) SHA1(ea585ff6e998e647580bf75dc128c1c2892947eb) )
	ROM_LOAD( "bm_chr-5.bin",       0xa0000, 0x20000, CRC(0e0abde0) SHA1(1e17767ccf55bebf681b078b405bedd47fe502d5) )
	ROM_LOAD( "bm_chr-6.bin",       0xc0000, 0x20000, CRC(4a61f08c) SHA1(1136819780e1f3ed150663d4853b6caf835e1c3e) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "bm_obj-0.bin",       0x00000, 0x20000, CRC(15724b94) SHA1(2f40425138c574a51e742ff610ce88c8a47c0c16) )
	ROM_LOAD( "bm_obj-1.bin",       0x20000, 0x20000, CRC(5d21f962) SHA1(2cfeda30dd7e10bcb66fe8858470a9db7577a59d) )
	ROM_LOAD( "bm_obj-2.bin",       0x40000, 0x20000, CRC(5d48e924) SHA1(6642b9cc632bba216f341a37325a826808aa3f34) )
	ROM_LOAD( "bm_obj-3.bin",       0x60000, 0x20000, CRC(cbe56b7f) SHA1(6387d04f0f42cde3504fb85ebb702de334bb0e5a) )
	ROM_LOAD( "bm_obj-4.bin",       0x80000, 0x20000, CRC(76dcc24c) SHA1(78deeb6efca1d2a0d52ee047bdd869cb9c9e2816) )
	ROM_LOAD( "bm_obj-5.bin",       0xa0000, 0x20000, CRC(fe70201d) SHA1(49a621e169f096572583bbbdee2ab1a7916bf95f) )
	ROM_LOAD( "bm_obj-7.bin",       0xe0000, 0x20000, CRC(377c81ed) SHA1(85f4682c6079863793f5575d261a33309c221ba7) )
ROM_END

/* Marchen Maze */
ROM_START( mmaze )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "mm_snd-0.bin",       0x0c000, 0x10000, CRC(25d25e07) SHA1(b2293bfc380fd767ac2a51e8b32e24bbea866be2) )
	ROM_LOAD( "mm_snd-1.bin",       0x1c000, 0x10000, CRC(2c5849c8) SHA1(1073719c9f4d4e41cbfd7c749bff42a0be460baf) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "mm_prg-0.bin",    0x000000, CRC(e169a911) SHA1(0537536f5278a9e7ebad03b55d9904ccbac7b3b6) )
	ROM_LOAD_1024( "mm_prg-1.bin",    0x080000, CRC(6ba14e41) SHA1(54d53a5653eb943210f519c85d190482957b3533) )
	ROM_LOAD_1024( "mm_prg-2.bin",    0x100000, CRC(91bde09f) SHA1(d7f6f644f526e36b6fd930d80f78ad1aa646fdfb) )
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	ROM_LOAD_512 ( "mm1_p6.bin",      0x300000, CRC(eaf530d8) SHA1(4c62f86b58ff2c62b269f2cef7982a3d49490ffa) )
	ROM_LOAD_512 ( "mm1_p7.bin",      0x380000, CRC(085e58cc) SHA1(3b83943e93eacae61a0e762d568cf7bc64128e37) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "mm_voi-0.bin",       0x10000, 0x20000, CRC(ee974cff) SHA1(f211c461a36dae9ce5ee614aaaabf92556181a85) )
	ROM_LOAD( "mm_voi-1.bin",       0x30000, 0x20000, CRC(d09b5830) SHA1(954be797e30f7d126b4fc2b04f190bfd7dc23bff) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "mm_chr-8.bin",       0x00000, 0x20000, CRC(a3784dfe) SHA1(7bcd71e0c675cd16587b61c23b470abb8ba434d3) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "mm_chr-0.bin",       0x00000, 0x20000, CRC(43ff2dfc) SHA1(4cf6834071f408eac5a7a67570bd11cb61a83b54) )
	ROM_LOAD( "mm_chr-1.bin",       0x20000, 0x20000, CRC(b9b4b72d) SHA1(cc11496a27cd94503eb3a16c95c49d60ed092e62) )
	ROM_LOAD( "mm_chr-2.bin",       0x40000, 0x20000, CRC(bee28425) SHA1(90e8aaf4bcb1af6239404bc05b9e6a1b25f61754) )
	ROM_LOAD( "mm_chr-3.bin",       0x60000, 0x20000, CRC(d9f41e5c) SHA1(c4fd2245ee02d8479209e07b8fe32d46b66de6ee) )
	ROM_LOAD( "mm_chr-4.bin",       0x80000, 0x20000, CRC(3484f4ae) SHA1(0cc177637e3fc8ef26bcde0f15ab507143745ff9) )
	ROM_LOAD( "mm_chr-5.bin",       0xa0000, 0x20000, CRC(c863deba) SHA1(cc2b8cd156cf11ee289c68b0a583e7bb4250414b) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "mm_obj-0.bin",       0x00000, 0x20000, CRC(d4b7e698) SHA1(c73ef73574a52d06e12e21047529b09854e1ba21) )
	ROM_LOAD( "mm_obj-1.bin",       0x20000, 0x20000, CRC(1ce49e04) SHA1(fc30a03e443bece11bd86771ebd1fcb40d15b0b9) )
	ROM_LOAD( "mm_obj-2.bin",       0x40000, 0x20000, CRC(3d3d5de3) SHA1(aa8032f1d99af1d92b0afaa11933548e0d39f03b) )
	ROM_LOAD( "mm_obj-3.bin",       0x60000, 0x20000, CRC(dac57358) SHA1(5175b66d3622cb56ed7be3568b247195d1485579) )
ROM_END

/* Bakutotsu Kijuutei */
ROM_START( bakutotu )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "bk1_s0.bin",         0x0c000, 0x10000, CRC(c35d7df6) SHA1(9ea534fc700581171536ad1df60263d31e7239a6) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "bk_prg-0.bin",    0x000000, CRC(4529c362) SHA1(beae2119fb6a5752885766fb5fba9c4fac5dd38f) )
	ROM_LOAD_512 ( "bk1_p1.bin",      0x080000, CRC(d389d6d4) SHA1(04502f1670d96fb4c2369ca2f05edfd3181d63cf) )
	ROM_LOAD_512 ( "bk1_p2.bin",      0x100000, CRC(7a686daa) SHA1(1313603f12e06eb2384bf156aee1bfb40e8fa39c) )
	ROM_LOAD_1024( "bk1_p3.bin",      0x180000, CRC(e608234f) SHA1(0445321e19666effb4784f577bbeb5761822edb8) )
	ROM_LOAD_512 ( "bk1_p4.bin",      0x200000, CRC(96446d48) SHA1(22a3f0689b272df3e773509b2ff72d2801d25cfc) )
	ROM_LOAD_512 ( "bk1_p5.bin",      0x280000, CRC(dceed7cb) SHA1(af6a763a40a987e31a071debb85c42b45ec28644) )
	ROM_LOAD_1024( "bk1_p6.bin",      0x300000, CRC(57a3ce42) SHA1(773d5f93e75ffe4b114cbcd1093c7cb43e1d6362) )
	ROM_LOAD_1024( "bk1_prg7.bin",    0x380000, CRC(fac1c1bf) SHA1(59e2612d0f4b3aea5cf5f5652e7cbee89cde860f) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "bk1_v0.bin",      0x10000, 0x10000, CRC(008e290e) SHA1(87ac7291088f0d6a7179b1a5f3567a72dc92177c) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "bk_chr-8.bin",       0x00000, 0x20000, CRC(6c8d4029) SHA1(2eb6fd89ffaecfa53f9adcdebbe8f550199d067f) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "bk_chr-0.bin",       0x00000, 0x20000, CRC(4e011058) SHA1(bcefa12c1ca2486065ad648239704bed7f48dcfd) )
	ROM_LOAD( "bk_chr-1.bin",       0x20000, 0x20000, CRC(496fcb9b) SHA1(0321bbc6957a34533d85008adbe1315a22f9292b) )
	ROM_LOAD( "bk_chr-2.bin",       0x40000, 0x20000, CRC(dc812e28) SHA1(42c71a840f3f885f1d53285156c098d00a202ab1) )
	ROM_LOAD( "bk_chr-3.bin",       0x60000, 0x20000, CRC(2b6120f4) SHA1(2cd52ed82556f4177ad427a063b6a0867ad3d3ad) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "bk_obj-0.bin",       0x00000, 0x20000, CRC(88c627c1) SHA1(10b331d8eec2cba327a8fa6f3213880e8e50a38d) )

	// obj1 and 2 may not exist on the real board
	ROM_LOAD( "bk_obj-3.bin",       0x20000, 0x20000, CRC(f7d1909a) SHA1(3f74a186f29b8f13a4c33de8f5b3d241bd6ded89) ) // dummy mirroring obj3
	ROM_LOAD( "bk_obj-4.bin",       0x40000, 0x20000, CRC(27ed1441) SHA1(74fa3ad01cbf01ee3be9d5cc2d241885c8a90b18) ) // dummy mirroring obj4

	ROM_LOAD( "bk_obj-3.bin",       0x60000, 0x20000, CRC(f7d1909a) SHA1(3f74a186f29b8f13a4c33de8f5b3d241bd6ded89) )
	ROM_LOAD( "bk_obj-4.bin",       0x80000, 0x20000, CRC(27ed1441) SHA1(74fa3ad01cbf01ee3be9d5cc2d241885c8a90b18) )
	ROM_LOAD( "bk_obj-5.bin",       0xa0000, 0x20000, CRC(790560c0) SHA1(b3b0be53a3dccada9b7c059c1f10342a16cc7e1a) )
	ROM_LOAD( "bk_obj-6.bin",       0xc0000, 0x20000, CRC(2cd4d2ea) SHA1(5a367841488304a93595757a653f9b484cfb252e) )
	ROM_LOAD( "bk_obj-7.bin",       0xe0000, 0x20000, CRC(809aa0e6) SHA1(d5dbc04037001a0808e79da742f7c4a8f5d3bc2f) )
ROM_END

/* World Court */
ROM_START( wldcourt )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "wc1_snd0.bin",       0x0c000, 0x10000, CRC(17a6505d) SHA1(773636173947a656c3b5a21049c28eedc40e4654) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	ROM_LOAD_512( "wc1_prg6.bin",    0x300000, CRC(e9216b9e) SHA1(dc2e0b7ca1b0de01ae7e05a4098eb6f2d1042211) )
	ROM_LOAD_512( "wc1_prg7.bin",    0x380000, CRC(8a7c6cac) SHA1(da3b1682c4aa756a8f8d06c15110ee8d23c215f1) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "wc1_voi0.bin",    0x10000, 0x10000, CRC(b57919f7) SHA1(5305c479513943a5d92988a63ad1671744e944b5) )
	ROM_LOAD( "wc1_voi1.bin",       0x30000, 0x20000, CRC(97974b4b) SHA1(1e4d10ce28cabc01f1f233a0edc05e20874e0285) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "wc1_chr8.bin",       0x00000, 0x20000, CRC(23e1c399) SHA1(2d22da5c68c0924767f18fb19576cb76a016ae8e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "wc1_chr0.bin",       0x00000, 0x20000, CRC(9fb07b9b) SHA1(5e98da8a4800be264d219e7b47433531ae2bbb79) )
	ROM_LOAD( "wc1_chr1.bin",       0x20000, 0x20000, CRC(01bfbf60) SHA1(648bdabd0e9b5e98f20863335177ad2df455c022) )
	ROM_LOAD( "wc1_chr2.bin",       0x40000, 0x20000, CRC(7e8acf45) SHA1(0bc5206be76a8f44881a1bc5f559412c9a8ff57f) )
	ROM_LOAD( "wc1_chr3.bin",       0x60000, 0x20000, CRC(924e9c81) SHA1(4b5eb94c8da9e8397c5f1d03f338a5fa8895931c) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "wc1_obj0.bin",       0x00000, 0x20000, CRC(70d562f8) SHA1(4445e2dd3eeaef79803d2b06e5b32782f6c4acbc) )
	ROM_LOAD( "wc1_obj1.bin",       0x20000, 0x20000, CRC(ba8b034a) SHA1(024a4dd4497c17792648f90b59da9bc3da884f13) )
	ROM_LOAD( "wc1_obj2.bin",       0x40000, 0x20000, CRC(c2bd5f0f) SHA1(9c29144fb77290a4d2aaa43c4c0b2f6757ed1f8c) )
	ROM_LOAD( "wc1_obj3.bin",       0x60000, 0x10000, CRC(1aa2dbc8) SHA1(dc100fd85aca8b4c29d2100dba43dd4093976633) )
ROM_END

/* Splatter House */
ROM_START( splatter )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "sh1_snd0.bin",       0x0c000, 0x10000, CRC(90abd4ad) SHA1(caeba5befcf57d90671786c7ef1ce49d54821949) )
	ROM_LOAD( "sh1_snd1.bin",       0x1c000, 0x10000, CRC(8ece9e0a) SHA1(578da932a7684c6f633dde1d6412011c727c2380) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "sh1_prg0.bin",    0x000000, CRC(4e07e6d9) SHA1(9bca8aca0041c311c403cf3b9a2365d704b39769) )
	ROM_LOAD_512( "sh1_prg1.bin",    0x080000, CRC(7a3efe09) SHA1(2271356be580e29cf70dbb70f797fb3c49666ada) )
	ROM_LOAD_512( "sh1_prg2.bin",    0x100000, CRC(434dbe7d) SHA1(40bb1d4ed8e6563f98732501e212d7324c714af2) )
	ROM_LOAD_512( "sh1_prg3.bin",    0x180000, CRC(955ce93f) SHA1(5ba493769595bc9ebf5404a50435aaf0918d7dd3) )
	ROM_LOAD_512( "sh1_prg4.bin",    0x200000, CRC(350dee5b) SHA1(8928a7453ff52b3c7abbf28b2ab08b2e63b16d28) )
	ROM_LOAD_512( "sh1_prg5.bin",    0x280000, CRC(0187de9a) SHA1(9b6c5bbdb81fb5cbb9a93b2b6d2358711dfded31) )
	ROM_LOAD_512( "sh2_prg6.bin",    0x300000, CRC(054d6275) SHA1(5f28985897e35dca33a8063e98938c0f6d8fe9d8) )
	ROM_LOAD_512( "sh2_prg7.bin",    0x380000, CRC(942cb61e) SHA1(405804b85eed62150667116c500222f591a25dd3) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "sh_voi-0.bin",       0x10000, 0x20000, CRC(2199cb66) SHA1(f1c4e3fb0e7c6eae50c698cded8c85a3cbd36672) )
	ROM_LOAD( "sh_voi-1.bin",       0x30000, 0x20000, CRC(9b6472af) SHA1(b7cde805a4d25f9c332c2c13ffa474e683ec76d5) )
	ROM_LOAD( "sh_voi-2.bin",       0x50000, 0x20000, CRC(25ea75b6) SHA1(aafebbdddf4a2924d9e5a850ffb6861cb5c4a769) )
	ROM_LOAD( "sh_voi-3.bin",       0x70000, 0x20000, CRC(5eebcdb4) SHA1(973e95a49cb1dda14e4c61580501c997fc7bc015) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "sh_chr-8.bin",       0x00000, 0x20000, CRC(321f483b) SHA1(84d75367d2e3ae210ecd17c163b336f609628a91) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "sh_chr-0.bin",       0x00000, 0x20000, CRC(4dd2ef05) SHA1(c756c0986db9e61cf44379fafb00295f0d434667) )
	ROM_LOAD( "sh_chr-1.bin",       0x20000, 0x20000, CRC(7a764999) SHA1(836aee2c9abe2a8e3806b53e051f76dcf4266212) )
	ROM_LOAD( "sh_chr-2.bin",       0x40000, 0x20000, CRC(6e6526ee) SHA1(f96a672b4293657398cfc6c1cf4ab4e4b223f7f9) )
	ROM_LOAD( "sh_chr-3.bin",       0x60000, 0x20000, CRC(8d05abdb) SHA1(cdf6ae9edc880a1d750a4c36b9dc7150aab8e249) )
	ROM_LOAD( "sh_chr-4.bin",       0x80000, 0x20000, CRC(1e1f8488) SHA1(3649564746516260b8b2df404e0ccf874d3441f9) )
	ROM_LOAD( "sh_chr-5.bin",       0xa0000, 0x20000, CRC(684cf554) SHA1(77d3b6f03c8a96bd6e8bb7d2264fbfeb43bcde3d) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "sh_obj-0.bin",       0x00000, 0x20000, CRC(1cedbbae) SHA1(22811a9376195794f18ee8d377b2548d27fc868e) )
	ROM_LOAD( "sh_obj-1.bin",       0x20000, 0x20000, CRC(e56e91ee) SHA1(12b823ca92fb518d84c0432925ae8a4b33a1354d) )
	ROM_LOAD( "sh_obj-2.bin",       0x40000, 0x20000, CRC(3dfb0230) SHA1(bf68a20544931b9673ce1f9e81b5f82e99f50bd0) )
	ROM_LOAD( "sh_obj-3.bin",       0x60000, 0x20000, CRC(e4e5a581) SHA1(e6cebef3dafbcdfd8e9e25b0796af47c8673473f) )
	ROM_LOAD( "sh_obj-4.bin",       0x80000, 0x20000, CRC(b2422182) SHA1(84cba3b7552756a206793f4c7b90fc6b8821963d) )
	ROM_LOAD( "sh_obj-5.bin",       0xa0000, 0x20000, CRC(24d0266f) SHA1(5c768343554209a7d7858afe41489a58809dd5f1) )
	ROM_LOAD( "sh_obj-6.bin",       0xc0000, 0x20000, CRC(80830b0e) SHA1(f9d69ece0827f5ec46473142d24f3191d40a0d57) )
	ROM_LOAD( "sh_obj-7.bin",       0xe0000, 0x20000, CRC(08b1953a) SHA1(f84f97e8e14ed6fcf99565d2603651831101ed2f) )
ROM_END

/* Splatter House */
ROM_START( splattej )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "sh1_snd0.bin",       0x0c000, 0x10000, CRC(90abd4ad) SHA1(caeba5befcf57d90671786c7ef1ce49d54821949) )
	ROM_LOAD( "sh1_snd1.bin",       0x1c000, 0x10000, CRC(8ece9e0a) SHA1(578da932a7684c6f633dde1d6412011c727c2380) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "sh1_prg0.bin",    0x000000, CRC(4e07e6d9) SHA1(9bca8aca0041c311c403cf3b9a2365d704b39769) )
	ROM_LOAD_512( "sh1_prg1.bin",    0x080000, CRC(7a3efe09) SHA1(2271356be580e29cf70dbb70f797fb3c49666ada) )
	ROM_LOAD_512( "sh1_prg2.bin",    0x100000, CRC(434dbe7d) SHA1(40bb1d4ed8e6563f98732501e212d7324c714af2) )
	ROM_LOAD_512( "sh1_prg3.bin",    0x180000, CRC(955ce93f) SHA1(5ba493769595bc9ebf5404a50435aaf0918d7dd3) )
	ROM_LOAD_512( "sh1_prg4.bin",    0x200000, CRC(350dee5b) SHA1(8928a7453ff52b3c7abbf28b2ab08b2e63b16d28) )
	ROM_LOAD_512( "sh1_prg5.bin",    0x280000, CRC(0187de9a) SHA1(9b6c5bbdb81fb5cbb9a93b2b6d2358711dfded31) )
	ROM_LOAD_512( "sh1_prg6.bin",    0x300000, CRC(97a3e664) SHA1(397907cedea1cd9ae25427df5d41b942933c3ca9) )
	ROM_LOAD_512( "sh1_prg7.bin",    0x380000, CRC(24c8cbd7) SHA1(72a2f008840b1b5cf026dd51e2797d87d92040fd) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "sh_voi-0.bin",       0x10000, 0x20000, CRC(2199cb66) SHA1(f1c4e3fb0e7c6eae50c698cded8c85a3cbd36672) )
	ROM_LOAD( "sh_voi-1.bin",       0x30000, 0x20000, CRC(9b6472af) SHA1(b7cde805a4d25f9c332c2c13ffa474e683ec76d5) )
	ROM_LOAD( "sh_voi-2.bin",       0x50000, 0x20000, CRC(25ea75b6) SHA1(aafebbdddf4a2924d9e5a850ffb6861cb5c4a769) )
	ROM_LOAD( "sh_voi-3.bin",       0x70000, 0x20000, CRC(5eebcdb4) SHA1(973e95a49cb1dda14e4c61580501c997fc7bc015) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "sh_chr-8.bin",       0x00000, 0x20000, CRC(321f483b) SHA1(84d75367d2e3ae210ecd17c163b336f609628a91) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "sh_chr-0.bin",       0x00000, 0x20000, CRC(4dd2ef05) SHA1(c756c0986db9e61cf44379fafb00295f0d434667) )
	ROM_LOAD( "sh_chr-1.bin",       0x20000, 0x20000, CRC(7a764999) SHA1(836aee2c9abe2a8e3806b53e051f76dcf4266212) )
	ROM_LOAD( "sh_chr-2.bin",       0x40000, 0x20000, CRC(6e6526ee) SHA1(f96a672b4293657398cfc6c1cf4ab4e4b223f7f9) )
	ROM_LOAD( "sh_chr-3.bin",       0x60000, 0x20000, CRC(8d05abdb) SHA1(cdf6ae9edc880a1d750a4c36b9dc7150aab8e249) )
	ROM_LOAD( "sh_chr-4.bin",       0x80000, 0x20000, CRC(1e1f8488) SHA1(3649564746516260b8b2df404e0ccf874d3441f9) )
	ROM_LOAD( "sh_chr-5.bin",       0xa0000, 0x20000, CRC(684cf554) SHA1(77d3b6f03c8a96bd6e8bb7d2264fbfeb43bcde3d) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "sh_obj-0.bin",       0x00000, 0x20000, CRC(1cedbbae) SHA1(22811a9376195794f18ee8d377b2548d27fc868e) )
	ROM_LOAD( "sh_obj-1.bin",       0x20000, 0x20000, CRC(e56e91ee) SHA1(12b823ca92fb518d84c0432925ae8a4b33a1354d) )
	ROM_LOAD( "sh_obj-2.bin",       0x40000, 0x20000, CRC(3dfb0230) SHA1(bf68a20544931b9673ce1f9e81b5f82e99f50bd0) )
	ROM_LOAD( "sh_obj-3.bin",       0x60000, 0x20000, CRC(e4e5a581) SHA1(e6cebef3dafbcdfd8e9e25b0796af47c8673473f) )
	ROM_LOAD( "sh_obj-4.bin",       0x80000, 0x20000, CRC(b2422182) SHA1(84cba3b7552756a206793f4c7b90fc6b8821963d) )
	ROM_LOAD( "sh_obj-5.bin",       0xa0000, 0x20000, CRC(24d0266f) SHA1(5c768343554209a7d7858afe41489a58809dd5f1) )
	ROM_LOAD( "sh_obj-6.bin",       0xc0000, 0x20000, CRC(80830b0e) SHA1(f9d69ece0827f5ec46473142d24f3191d40a0d57) )
	ROM_LOAD( "sh_obj-7.bin",       0xe0000, 0x20000, CRC(08b1953a) SHA1(f84f97e8e14ed6fcf99565d2603651831101ed2f) )
ROM_END

/* Face Off */
ROM_START( faceoff )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "fo1_s0.bin",         0x0c000, 0x10000, CRC(9a00d97d) SHA1(f1dcad7b6c9adcdce720d7b336d9c34f37975b31) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	ROM_LOAD_512( "fo1_p6.bin",      0x300000, CRC(a48ee82b) SHA1(d6dbcb6d84a1290185388fb7278e0b2fbb46a0e5) )
	ROM_LOAD_512( "fo1_p7.bin",      0x380000, CRC(6791d221) SHA1(e3a95bd4ff36df5fccd5168491beeb18b1a10d95) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "fo1_v0.bin",      0x10000, 0x10000, CRC(e6edf63e) SHA1(095f7fa93233e4b4f25e728868c212170be48550) )
	ROM_LOAD_HS( "fo1_v1.bin",      0x30000, 0x10000, CRC(132a5d90) SHA1(d5ceae68d7aea7cdde43600453f9724f35834519) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "fo1_c8.bin",         0x00000, 0x10000, CRC(d397216c) SHA1(baa3747bf3e12246e2629eaf0abdb3df05e423bd) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "fo1_c0.bin",         0x00000, 0x20000, CRC(27884ac0) SHA1(b80444553e87d9dc0e3fdccee00ac9424a686a2a) )
	ROM_LOAD( "fo1_c1.bin",         0x20000, 0x20000, CRC(4d423499) SHA1(9c06f468562e7c2dced3514b8a94db4146a16989) )
	ROM_LOAD( "fo1_c2.bin",         0x40000, 0x20000, CRC(d62d86f1) SHA1(db526b9e5c701bb6264549581a00aabf6c95e7be) )
	ROM_LOAD( "fo1_c3.bin",         0x60000, 0x20000, CRC(c2a08694) SHA1(8a78956a7e9f43c93aa63d80bb8b984c0ab22ef9) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "fo1_o0.bin",         0x00000, 0x20000, CRC(41af669d) SHA1(299563252d1340fd6745d9c04d7193ab29290b27) )
	ROM_LOAD( "fo1_o1.bin",         0x20000, 0x20000, CRC(ad5fbaa7) SHA1(aebfd451e7666cfd49b9875b711a317356a0ea87) )
	ROM_LOAD( "fo1_o2.bin",         0x40000, 0x20000, CRC(c1f7eb52) SHA1(f87a59afa19bbc7a9b876f50a787df8148b63738) )
	ROM_LOAD( "fo1_o3.bin",         0x60000, 0x20000, CRC(aa95d2e0) SHA1(adb1acf5c032abc37c76445fd17f4bb02f0952f6) )
	ROM_LOAD( "fo1_o4.bin",         0x80000, 0x20000, CRC(985f04c7) SHA1(03fcb84c9134aa4521ce9549d09925a4f7a82318) )
ROM_END

/* Rompers */
ROM_START( rompers )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "rp1_snd0.bin",       0x0c000, 0x10000, CRC(c7c8d649) SHA1(a60a58b4fc8e3f65e4e686b51fd2c17c9d74c444) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	ROM_LOAD_512( "rp1_prg4.bin",    0x200000, CRC(0918f06d) SHA1(0a9055e8e7d6940e1ff89b4bb83180184b4c63cd) )
	ROM_LOAD_512( "rp1_prg5.bin",    0x280000, CRC(98bd4133) SHA1(82b128eef2b6c9bd39816203feb60035556a09ee) )
	ROM_LOAD_512( "rp1prg6b.bin",    0x300000, CRC(80821065) SHA1(76e0dd774e064d8ed9399116d50230e7d7f38216) )
	ROM_LOAD_512( "rp1prg7b.bin",    0x380000, CRC(49d057e2) SHA1(c1e19218b1897827b3de7912a08f1677510a8b09) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "rp_voi-0.bin",       0x10000, 0x20000, CRC(11caef7e) SHA1(c6470cbbc6402872794e0a4e822a5d08ca2448ef) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "rp1_chr8.bin",       0x00000, 0x10000, CRC(69cfe46a) SHA1(01c5af1b7fc337ec06a5afabd87b9a6a7dcf3503) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "rp_chr-0.bin",       0x00000, 0x20000, CRC(41b10ef3) SHA1(b1230ef55db91bc92d251332434ca7d5c9e7f879) )
	ROM_LOAD( "rp_chr-1.bin",       0x20000, 0x20000, CRC(c18cd24e) SHA1(cfcf7ff4810ce18aaf188fbb552c926db9577c00) )
	ROM_LOAD( "rp_chr-2.bin",       0x40000, 0x20000, CRC(6c9a3c79) SHA1(7d70a87b411ebafd372eb3f46bb09c6bfbfafb04) )
	ROM_LOAD( "rp_chr-3.bin",       0x60000, 0x20000, CRC(473aa788) SHA1(8b3fcc7511bdc5ab443ac67a6bd0c05fb3463f8c) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "rp_obj-0.bin",       0x00000, 0x20000, CRC(1dcbf8bb) SHA1(aec5d43b1a8c4af9a3002c815662179fa6324e2d) )
	ROM_LOAD( "rp_obj-1.bin",       0x20000, 0x20000, CRC(cb98e273) SHA1(f13fe6f19ff812a2dcb614343f040fb9c4131b56) )
	ROM_LOAD( "rp_obj-2.bin",       0x40000, 0x20000, CRC(6ebd191e) SHA1(e7ff23ac2586f4c38f3c5107b789e313a35e054e) )
	ROM_LOAD( "rp_obj-3.bin",       0x60000, 0x20000, CRC(7c9828a1) SHA1(0b0abd18b17b0cfe9308d4282400cc8bc89b640c) )
	ROM_LOAD( "rp_obj-4.bin",       0x80000, 0x20000, CRC(0348220b) SHA1(453c86d27490397c49d2708d5afc5a12c4bc99d1) )
	ROM_LOAD( "rp1_obj5.bin",       0xa0000, 0x10000, CRC(9e2ba243) SHA1(36721dbebfef5f7fb3935ff89462b52ee286df82) )
	ROM_LOAD( "rp1_obj6.bin",       0xc0000, 0x10000, CRC(6bf2aca6) SHA1(ecd6022c0e4022ebfc571ee20d89369921cf3f1d) )
ROM_END

/* Rompers (old version) */
ROM_START( romperso )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "rp1_snd0.bin",       0x0c000, 0x10000, CRC(c7c8d649) SHA1(a60a58b4fc8e3f65e4e686b51fd2c17c9d74c444) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	ROM_LOAD_512( "rp1_prg4.bin",    0x200000, CRC(0918f06d) SHA1(0a9055e8e7d6940e1ff89b4bb83180184b4c63cd) )
	ROM_LOAD_512( "rp1_prg5.bin",    0x280000, CRC(98bd4133) SHA1(82b128eef2b6c9bd39816203feb60035556a09ee) )
	ROM_LOAD_512( "rp1_prg6.bin",    0x300000, CRC(fc183345) SHA1(1660233d8971236d059ac9c40ee430b269abdc8f) )
	ROM_LOAD_512( "rp1_prg7.bin",    0x380000, CRC(8d49f28a) SHA1(3b86757da6e3f81794a7c18907b14a555b99bb10) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "rp_voi-0.bin",       0x10000, 0x20000, CRC(11caef7e) SHA1(c6470cbbc6402872794e0a4e822a5d08ca2448ef) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "rp1_chr8.bin",       0x00000, 0x10000, CRC(69cfe46a) SHA1(01c5af1b7fc337ec06a5afabd87b9a6a7dcf3503) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "rp_chr-0.bin",       0x00000, 0x20000, CRC(41b10ef3) SHA1(b1230ef55db91bc92d251332434ca7d5c9e7f879) )
	ROM_LOAD( "rp_chr-1.bin",       0x20000, 0x20000, CRC(c18cd24e) SHA1(cfcf7ff4810ce18aaf188fbb552c926db9577c00) )
	ROM_LOAD( "rp_chr-2.bin",       0x40000, 0x20000, CRC(6c9a3c79) SHA1(7d70a87b411ebafd372eb3f46bb09c6bfbfafb04) )
	ROM_LOAD( "rp_chr-3.bin",       0x60000, 0x20000, CRC(473aa788) SHA1(8b3fcc7511bdc5ab443ac67a6bd0c05fb3463f8c) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "rp_obj-0.bin",       0x00000, 0x20000, CRC(1dcbf8bb) SHA1(aec5d43b1a8c4af9a3002c815662179fa6324e2d) )
	ROM_LOAD( "rp_obj-1.bin",       0x20000, 0x20000, CRC(cb98e273) SHA1(f13fe6f19ff812a2dcb614343f040fb9c4131b56) )
	ROM_LOAD( "rp_obj-2.bin",       0x40000, 0x20000, CRC(6ebd191e) SHA1(e7ff23ac2586f4c38f3c5107b789e313a35e054e) )
	ROM_LOAD( "rp_obj-3.bin",       0x60000, 0x20000, CRC(7c9828a1) SHA1(0b0abd18b17b0cfe9308d4282400cc8bc89b640c) )
	ROM_LOAD( "rp_obj-4.bin",       0x80000, 0x20000, CRC(0348220b) SHA1(453c86d27490397c49d2708d5afc5a12c4bc99d1) )
	ROM_LOAD( "rp1_obj5.bin",       0xa0000, 0x10000, CRC(9e2ba243) SHA1(36721dbebfef5f7fb3935ff89462b52ee286df82) )
	ROM_LOAD( "rp1_obj6.bin",       0xc0000, 0x10000, CRC(6bf2aca6) SHA1(ecd6022c0e4022ebfc571ee20d89369921cf3f1d) )
ROM_END

/* Blast Off */
ROM_START( blastoff )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "bo1-snd0.bin",       0x0c000, 0x10000, CRC(2ecab76e) SHA1(592f1f9ac06cea81517ad0ab7d2fd65bccf6a8d8) )
	ROM_LOAD( "bo1-snd1.bin",       0x1c000, 0x10000, CRC(048a6af1) SHA1(97b839c7c92053b1058f4c91fc5b6e398ee73045) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	ROM_LOAD_1024( "bo1_prg6.bin",       0x300000, CRC(d60da63e) SHA1(525e7d87e240d08f83e5d69b47eb185283b396d4) )
	ROM_LOAD_1024( "bo1prg7b.bin",       0x380000, CRC(b630383c) SHA1(eef9ae4e84f41bf5d137936d8eb033cb89374b86) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "bo_voi-0.bin",       0x10000, 0x20000, CRC(47065e18) SHA1(234e7a40be4def6846040a0fca3e9d7eff3754e3) )
	ROM_LOAD( "bo_voi-1.bin",       0x30000, 0x20000, CRC(0308b18e) SHA1(7196abdf36d660089d739e3f3a362648768a6127) )
	ROM_LOAD( "bo_voi-2.bin",       0x50000, 0x20000, CRC(88cab230) SHA1(659c4efeb8aa24f8b32509ee563407ed1e17d564) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "bo_chr-8.bin",       0x00000, 0x20000, CRC(e8b5f2d4) SHA1(70dd2898dcfed5f43f6c50f852660f24a9d7ec9d) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "bo_chr-0.bin",       0x00000, 0x20000, CRC(bdc0afb5) SHA1(09d01640bd0ff8d9d96282411ec9176e5fc57cae) )
	ROM_LOAD( "bo_chr-1.bin",       0x20000, 0x20000, CRC(963d2639) SHA1(1bb936631c17e7695e087c95a72f1871fabb6647) )
	ROM_LOAD( "bo_chr-2.bin",       0x40000, 0x20000, CRC(acdb6894) SHA1(a0d712e4883cda7068aa678d698abbe9c910363b) )
	ROM_LOAD( "bo_chr-3.bin",       0x60000, 0x20000, CRC(214ec47f) SHA1(0f314740bdb2f3cd0ed8ab9a98a47eb8f8b1480b) )
	ROM_LOAD( "bo_chr-4.bin",       0x80000, 0x20000, CRC(08397583) SHA1(e2c0ce46982a1f9035f0a3c4f08dbf3a5e11a871) )
	ROM_LOAD( "bo_chr-5.bin",       0xa0000, 0x20000, CRC(20402429) SHA1(a84760f693ec15f8f8238653d1761a34e2499da9) )
	ROM_LOAD( "bo_chr-7.bin",       0xe0000, 0x20000, CRC(4c5c4603) SHA1(aa0c4f651ae38513f0a2b685dcc62c1897d0f310) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "bo_obj-0.bin",       0x00000, 0x20000, CRC(b3308ae7) SHA1(543ce5b41d922e0299ea691437d7d1a1741c1e0f) )
	ROM_LOAD( "bo_obj-1.bin",       0x20000, 0x20000, CRC(c9c93c47) SHA1(ed3e0e54a5602e2ec6d4e0ee9f7632bc0d4b99fe) )
	ROM_LOAD( "bo_obj-2.bin",       0x40000, 0x20000, CRC(eef77527) SHA1(ba1931a37e4ad06e57e73d61f985bf6267f92d76) )
	ROM_LOAD( "bo_obj-3.bin",       0x60000, 0x20000, CRC(e3d9ed58) SHA1(ddb68274957f52882331b4ab13c55f096a0b1f76) )
	ROM_LOAD( "bo1_obj4.bin",       0x80000, 0x20000, CRC(c2c1b9cb) SHA1(754bf6136f4b1b4958474072dca5c6dbf54517bd) )
ROM_END

/* World Stadium '89 */
ROM_START( ws89 )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "w91_snd0.bin",       0x0c000, 0x10000, CRC(52b84d5a) SHA1(efe7921a565faa42793d581868aa3fa634d81103) )
	ROM_LOAD( "ws1_snd1.bin",       0x1c000, 0x10000, CRC(31bf74c1) SHA1(ddb7a91d6f3ae93be79914a435178a540fe05bfb) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "ws1_prg0.bin",    0x000000, CRC(b0234298) SHA1(b46a70109801d85332fb6658426bd795e03f492a) )
	ROM_LOAD_512( "w91_prg1.bin",    0x080000, CRC(7ad8768f) SHA1(7698b005e2c371266f390b8e0992666c822577d0) )
	ROM_LOAD_512( "w91_prg2.bin",    0x100000, CRC(522e5441) SHA1(e8448aabf3527e268b7b0722825be36978d32cf4) )
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_512( "w91_prg7.bin",    0x380000, CRC(611ed964) SHA1(855a9173bdc707f9cc0fa599ed5e6ccc4897f4e5) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "ws1_voi0.bin",    0x10000, 0x10000, CRC(f6949199) SHA1(ef596b02060f8e58eac37765663dd16377244391) )
	ROM_LOAD( "ws_voi-1.bin",       0x30000, 0x20000, CRC(210e2af9) SHA1(f8a1f8c6b9fbb8a9b3f298674600c1fbb9c5840e) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "ws_chr-8.bin",       0x00000, 0x20000, CRC(d1897b9b) SHA1(29906614b879e5623b49bc925e80006aee3997b9) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "ws_chr-0.bin",       0x00000, 0x20000, CRC(3e3e96b4) SHA1(7b5f72e7e2aab9c78f452171ac6ebe8c634a88c4) )
	ROM_LOAD( "ws_chr-1.bin",       0x20000, 0x20000, CRC(897dfbc1) SHA1(3e7425a456bada77591abcf1567ec75a99440334) )
	ROM_LOAD( "ws_chr-2.bin",       0x40000, 0x20000, CRC(e142527c) SHA1(606206b7b9916e3f456595344b70c17d44731fbb) )
	ROM_LOAD( "ws_chr-3.bin",       0x60000, 0x20000, CRC(907d4dc8) SHA1(16c31cbccef4d8a81f2150cec874a01688e46a59) )
	ROM_LOAD( "ws_chr-4.bin",       0x80000, 0x20000, CRC(afb11e17) SHA1(cf4aa124f0d4fe737686ccda72c55c199189ca49) )
	ROM_LOAD( "ws_chr-6.bin",       0xc0000, 0x20000, CRC(a16a17c2) SHA1(52917165d50a03db8ad6783a95731e0b438c2a6c) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "ws_obj-0.bin",       0x00000, 0x20000, CRC(12dc83a6) SHA1(f3d0ebda14af81ff99642ceaa7b9c0e4a4bfbbcb) )
	ROM_LOAD( "ws_obj-1.bin",       0x20000, 0x20000, CRC(68290a46) SHA1(c05f31e1c2332cba5d5b8cafc0e9f616aadee5b7) )
	ROM_LOAD( "ws_obj-2.bin",       0x40000, 0x20000, CRC(cd5ba55d) SHA1(9b5a655909457bcffdaaa95842740a6f0f8f6cec) )
	ROM_LOAD_HS( "w91_obj3.bin",    0x60000, 0x10000, CRC(8ee76105) SHA1(8b567631250981fd9ec15ef8beb1df469cc70c4d) )
ROM_END

/* Dangerous Seed */
ROM_START( dangseed )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "dr1_snd0.bin",       0x0c000, 0x20000, CRC(bcbbb21d) SHA1(0ec3e43b94733af69c0a68fd6f5ce5cda916aef7) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	/* 000000-07ffff empty */
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	ROM_LOAD_1024( "dr_prg-5.bin",    0x280000, CRC(7986bbdd) SHA1(7d6c99326469b5b6d88c2c9b000f03b5df390fdb) )
	ROM_LOAD_512 ( "dr1_prg6.bin",    0x300000, CRC(cc68262b) SHA1(34a6860a6e74e4060c118d06da7ecfb2530b8f97) )
	ROM_LOAD_1024( "dr1_prg7.bin",    0x380000, CRC(d7d2f653) SHA1(a08ca7ac0c36c8f06f516bbfb9f541b77e53e864) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "dr_voi-0.bin",       0x10000, 0x20000, CRC(de4fdc0e) SHA1(b5e952aaf5a81a2b4ff1c7cae141d50360545770) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "dr_chr-8.bin",       0x00000, 0x20000, CRC(0fbaa10e) SHA1(18ea77544678d889aded927a96a11bc04ad42fa6) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "dr_chr-0.bin",       0x00000, 0x20000, CRC(419bacc7) SHA1(a89c48caa4df66eeaf76dca33c1fc34f789ac36f) )
	ROM_LOAD( "dr_chr-1.bin",       0x20000, 0x20000, CRC(55ce77e1) SHA1(159027e45a298b65b0608852d666cc88ef834d2b) )
	ROM_LOAD( "dr_chr-2.bin",       0x40000, 0x20000, CRC(6f913419) SHA1(91906bad859f254b3aa3700bc62d71faccd48ad5) )
	ROM_LOAD( "dr_chr-3.bin",       0x60000, 0x20000, CRC(fe1f1a25) SHA1(c654df76b68f157c95b26294bd98fc392bca5a0a) )
	ROM_LOAD( "dr_chr-4.bin",       0x80000, 0x20000, CRC(c34471bc) SHA1(86a422c4ef4da538bf7964da0b5e920a4be85cfb) )
	ROM_LOAD( "dr_chr-5.bin",       0xa0000, 0x20000, CRC(715c0720) SHA1(65484358ff090fbe8e7925d97b6a39105ef00018) )
	ROM_LOAD( "dr_chr-6.bin",       0xc0000, 0x20000, CRC(5c1b71fa) SHA1(e6e75bce2d2564bf95c5911e208ec4d1705beeef) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "dr_obj-0.bin",       0x00000, 0x20000, CRC(abb95644) SHA1(f96f7aa3d1f6742f2460e3fc6aebc3b5ac4cdad7) )
	ROM_LOAD( "dr_obj-1.bin",       0x20000, 0x20000, CRC(24d6db51) SHA1(6ca66548cca53019dc242e39584b987f45e139f7) )
	ROM_LOAD( "dr_obj-2.bin",       0x40000, 0x20000, CRC(7e3a78c0) SHA1(d0026f70c05ef84dd9fc0588869ad7920949624a) )
ROM_END

/* World Stadium '90 */
ROM_START( ws90 )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "w91_snd0.bin",       0x0c000, 0x10000, CRC(52b84d5a) SHA1(efe7921a565faa42793d581868aa3fa634d81103) )
	ROM_LOAD( "ws1_snd1.bin",       0x1c000, 0x10000, CRC(31bf74c1) SHA1(ddb7a91d6f3ae93be79914a435178a540fe05bfb) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "ws1_prg0.bin",    0x000000, CRC(b0234298) SHA1(b46a70109801d85332fb6658426bd795e03f492a) )
	ROM_LOAD_512( "w91_prg1.bin",    0x080000, CRC(7ad8768f) SHA1(7698b005e2c371266f390b8e0992666c822577d0) )
	ROM_LOAD_512( "w901prg2.bin",    0x100000, CRC(b9e98e2f) SHA1(65750e5c5073d35aa7c68b769afcfc3da7213041) )
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_512( "w901prg7.bin",    0x380000, CRC(37ae1b25) SHA1(e3f9d8abdfa68929495993d0842d64a8fd323d91) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "ws1_voi0.bin",    0x10000, 0x10000, CRC(f6949199) SHA1(ef596b02060f8e58eac37765663dd16377244391) )
	ROM_LOAD( "ws_voi-1.bin",       0x30000, 0x20000, CRC(210e2af9) SHA1(f8a1f8c6b9fbb8a9b3f298674600c1fbb9c5840e) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "ws_chr-8.bin",       0x00000, 0x20000, CRC(d1897b9b) SHA1(29906614b879e5623b49bc925e80006aee3997b9) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "ws_chr-0.bin",       0x00000, 0x20000, CRC(3e3e96b4) SHA1(7b5f72e7e2aab9c78f452171ac6ebe8c634a88c4) )
	ROM_LOAD( "ws_chr-1.bin",       0x20000, 0x20000, CRC(897dfbc1) SHA1(3e7425a456bada77591abcf1567ec75a99440334) )
	ROM_LOAD( "ws_chr-2.bin",       0x40000, 0x20000, CRC(e142527c) SHA1(606206b7b9916e3f456595344b70c17d44731fbb) )
	ROM_LOAD( "ws_chr-3.bin",       0x60000, 0x20000, CRC(907d4dc8) SHA1(16c31cbccef4d8a81f2150cec874a01688e46a59) )
	ROM_LOAD( "ws_chr-4.bin",       0x80000, 0x20000, CRC(afb11e17) SHA1(cf4aa124f0d4fe737686ccda72c55c199189ca49) )
	ROM_LOAD( "ws_chr-6.bin",       0xc0000, 0x20000, CRC(a16a17c2) SHA1(52917165d50a03db8ad6783a95731e0b438c2a6c) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "ws_obj-0.bin",       0x00000, 0x20000, CRC(12dc83a6) SHA1(f3d0ebda14af81ff99642ceaa7b9c0e4a4bfbbcb) )
	ROM_LOAD( "ws_obj-1.bin",       0x20000, 0x20000, CRC(68290a46) SHA1(c05f31e1c2332cba5d5b8cafc0e9f616aadee5b7) )
	ROM_LOAD( "ws_obj-2.bin",       0x40000, 0x20000, CRC(cd5ba55d) SHA1(9b5a655909457bcffdaaa95842740a6f0f8f6cec) )
	ROM_LOAD_HS( "w901obj3.bin",    0x60000, 0x10000, CRC(7d0b8961) SHA1(b64e28e2a784bcc5f370436411da6c9fa8900c3d) )
ROM_END

/* Pistol Daimyo no Bouken */
ROM_START( pistoldm )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "pd1_snd0.bin",       0x0c000, 0x20000, CRC(026da54e) SHA1(ffd710c57e59184b93eff864730123e672a0089d) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "pd1_prg0.bin",       0x000000, CRC(9db9b89c) SHA1(30eeaec74454e8401ce16aeb85613448984b6eac) )
	/* 080000-0fffff empty */
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_1024( "pd1prg7b.bin",       0x380000, CRC(7189b797) SHA1(3652ee6b4a459946f61db8629a44b9675b082119) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "pd_voi-0.bin",       0x10000, 0x20000, CRC(ad1b8128) SHA1(f2112aa129abd7a243c0c329319d9d2ebf7869f7) )
	ROM_LOAD( "pd_voi-1.bin",       0x30000, 0x20000, CRC(2871c494) SHA1(9ac0dc559c22ac5083025c32f28e353b04348155) )
	ROM_LOAD( "pd_voi-2.bin",       0x50000, 0x20000, CRC(e783f0c4) SHA1(6a43f22226d1637d507c8194244058e8d96f8692) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "pd_chr-8.bin",       0x00000, 0x20000, CRC(a5f516db) SHA1(262c3a99cfa3061b58331d8ed254b49a06bfdd9f) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "pd_chr-0.bin",       0x00000, 0x20000, CRC(adbbaf5c) SHA1(89d39be53f4d2584f28ffd12391d91af2d54652b) )
	ROM_LOAD( "pd_chr-1.bin",       0x20000, 0x20000, CRC(b4e4f554) SHA1(2102ea8f4d5b8ce84f100dd6a18bdc95287f9c81) )
	ROM_LOAD( "pd_chr-2.bin",       0x40000, 0x20000, CRC(84592540) SHA1(c54cac48a103d35f002f934107f09c1c8e58d0e7) )
	ROM_LOAD( "pd_chr-3.bin",       0x60000, 0x20000, CRC(450bdaa9) SHA1(ca9f749e4a1f44e92b01a860b7903c89ceaa1d1a) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "pd_obj-0.bin",       0x00000, 0x20000, CRC(7269821d) SHA1(0f2e24540a2deae0d36a8a23a67f81fec26141b5) )
	ROM_LOAD( "pd_obj-1.bin",       0x20000, 0x20000, CRC(4f9738e5) SHA1(d15a9bfe0bfabbce4fe131cdbaeb3cf0ea239f22) )
	ROM_LOAD( "pd_obj-2.bin",       0x40000, 0x20000, CRC(33208776) SHA1(c98d78cdc34e27b798768132a77df8c554fa85c9) )
	ROM_LOAD( "pd_obj-3.bin",       0x60000, 0x20000, CRC(0dbd54ef) SHA1(d2e2b1c4e8318bc9bc4febdced9cb1c2248aafb0) )
	ROM_LOAD( "pd_obj-4.bin",       0x80000, 0x20000, CRC(58e838e2) SHA1(eb869cd72bd025ed25cd50f2540a1d09d4745c1b) )
	ROM_LOAD( "pd_obj-5.bin",       0xa0000, 0x20000, CRC(414f9a9d) SHA1(e48c308152cf038117997caacb93b46e2ea2f481) )
	ROM_LOAD( "pd_obj-6.bin",       0xc0000, 0x20000, CRC(91b4e6e0) SHA1(aca45e1d3a8ee11b4feb50a8ada9c4e8e4820224) )
	ROM_LOAD( "pd_obj-7.bin",       0xe0000, 0x20000, CRC(00d4a8f0) SHA1(2312bea93272974b944ea5566b5cd2631761dfef) )
ROM_END

/* Souko Ban Deluxe */
ROM_START( boxyboy )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "sb1_snd0.bin",       0x0c000, 0x10000, CRC(bf46a106) SHA1(cbc95759902c45869346973860cf27792860f781) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "sb1_prg0.bin",    0x000000, CRC(8af8cb73) SHA1(7c89ca0383e601a48d2f83449b2faf7b66a7a94d) )
	ROM_LOAD_1024( "sb1_prg1.bin",    0x080000, CRC(5d1fdd94) SHA1(df1f1f33df3041c7eb46dc9287427785c7264c2a) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_512 ( "sbx_prg7.bin",    0x380000, CRC(7787c72e) SHA1(84b064165788481fbfb4836291c9c4000abcf7ce) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "sb1_voi0.bin",    0x10000, 0x10000, CRC(63d9cedf) SHA1(117767c6b25325bf3005756d74196da56008498c) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "sb1_chr8.bin",       0x00000, 0x10000, CRC(5692b297) SHA1(ed20a0f4ce80674d01cd2a30571ffeff9f9066fd) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "sb1_chr0.bin",       0x00000, 0x20000, CRC(267f1331) SHA1(46a41ee3553cc7c73a47a339365e27b814067674) )
	ROM_LOAD( "sb1_chr1.bin",       0x20000, 0x20000, CRC(e5ff61ad) SHA1(30aff224dd5c5a85b057aa648677747b1b0e14a8) )
	ROM_LOAD( "sb1_chr2.bin",       0x40000, 0x20000, CRC(099b746b) SHA1(11125ee9cb988b23e9192dc37444fb59b60ab3ce) )
	ROM_LOAD( "sb1_chr3.bin",       0x60000, 0x20000, CRC(1551bb7c) SHA1(eaebc333e0bb8fddd6f48ab157eb492631e5e959) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "sb1_obj0.bin",       0x00000, 0x10000, CRC(ed810da4) SHA1(b3172b50b15d0e2fd40d38d32abf4de22b6f7a85) )
ROM_END

ROM_START( soukobdx )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "sb1_snd0.bin",       0x0c000, 0x10000, CRC(bf46a106) SHA1(cbc95759902c45869346973860cf27792860f781) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "sb1_prg0.bin",    0x000000, CRC(8af8cb73) SHA1(7c89ca0383e601a48d2f83449b2faf7b66a7a94d) )
	ROM_LOAD_1024( "sb1_prg1.bin",    0x080000, CRC(5d1fdd94) SHA1(df1f1f33df3041c7eb46dc9287427785c7264c2a) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_512 ( "sb1_prg7.bin",    0x380000, CRC(c3bd418a) SHA1(f2c9cc3b5b115ee1b342517897bfa979dac11544) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD_HS( "sb1_voi0.bin",    0x10000, 0x10000, CRC(63d9cedf) SHA1(117767c6b25325bf3005756d74196da56008498c) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "sb1_chr8.bin",       0x00000, 0x10000, CRC(5692b297) SHA1(ed20a0f4ce80674d01cd2a30571ffeff9f9066fd) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "sb1_chr0.bin",       0x00000, 0x20000, CRC(267f1331) SHA1(46a41ee3553cc7c73a47a339365e27b814067674) )
	ROM_LOAD( "sb1_chr1.bin",       0x20000, 0x20000, CRC(e5ff61ad) SHA1(30aff224dd5c5a85b057aa648677747b1b0e14a8) )
	ROM_LOAD( "sb1_chr2.bin",       0x40000, 0x20000, CRC(099b746b) SHA1(11125ee9cb988b23e9192dc37444fb59b60ab3ce) )
	ROM_LOAD( "sb1_chr3.bin",       0x60000, 0x20000, CRC(1551bb7c) SHA1(eaebc333e0bb8fddd6f48ab157eb492631e5e959) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "sb1_obj0.bin",       0x00000, 0x10000, CRC(ed810da4) SHA1(b3172b50b15d0e2fd40d38d32abf4de22b6f7a85) )
ROM_END

/* Puzzle Club */
ROM_START( puzlclub )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "pc1_s0.bin",         0x0c000, 0x10000, CRC(44737c02) SHA1(bcacfed1c3522d6ecddd3ac79ded620e5334df35) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_512( "pc1_p0.bin",      0x000000, CRC(2db477c8) SHA1(4f34750b08a72d1a46fe5caa56ee1209fde4accd) )
	ROM_LOAD_512( "pc1_p1.bin",      0x080000, CRC(dfd9108a) SHA1(07d246d50cdb5bc2c75490d21f87a60fbf559e72) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_512( "pc1_p7.bin",      0x380000, CRC(f0638260) SHA1(9ea33e2352ebeea42aa077ed049bec1037397431) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	/* no voices */

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "pc1-c8.bin",         0x00000, 0x20000, CRC(4e196bcd) SHA1(2e27538bbbebeda32353c0c02f98d52f0d1994f9) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "pc1-c0.bin",         0x00000, 0x20000, CRC(ad7b134e) SHA1(e9b16cdbc990f3e1ab1eed87db61819683d45864) )
	ROM_LOAD( "pc1-c1.bin",         0x20000, 0x20000, CRC(10cb3207) SHA1(ecd62744eba88a3a0c40d943888ec3497ef8bb04) )
	ROM_LOAD( "pc1-c2.bin",         0x40000, 0x20000, CRC(d98d2c8f) SHA1(c626cae84e712b0c8b66cf6923e77c75c137dd52) )
	ROM_LOAD( "pc1-c3.bin",         0x60000, 0x20000, CRC(91a61d96) SHA1(a645b272e1bd2fa4657e3e4b06c85b93150b9e9e) )
	ROM_LOAD( "pc1-c4.bin",         0x80000, 0x20000, CRC(f1c95296) SHA1(f093c4227b4f6f524a76d0b9409c2c6ce33e560b) )
	ROM_LOAD( "pc1-c5.bin",         0xa0000, 0x20000, CRC(bc443c27) SHA1(af841b6a2b783b0d9b9bbc33083afbb56e8bff69) )
	ROM_LOAD( "pc1-c6.bin",         0xc0000, 0x20000, CRC(ec0a3dc5) SHA1(a5148e99f3198196fd635ff4ac0275393e6f7033) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	/* no sprites */
ROM_END

/* Tank Force */
ROM_START( tankfrce )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "tf1_snd0.bin",       0x0c000, 0x20000, CRC(4d9cf7aa) SHA1(de51b9b36e9a530a7f3c35672ec72c19b607af04) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "tf1_prg0.bin",       0x000000, CRC(2ae4b9eb) SHA1(569d2754398b4276cf78a3dd038b5884778dc82e) )
	ROM_LOAD_1024( "tf1_prg1.bin",       0x080000, CRC(4a8bb251) SHA1(1df46ccf0ad7260398b7965e3825e936ba357062) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_1024( "tf1prg7.bin",        0x380000, CRC(2ec28a87) SHA1(91d2c1efbe156982beab24e437852d1c79dab412) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "tf1_voi0.bin",       0x10000, 0x20000, CRC(f542676a) SHA1(38d54db0807c58152bd120c393bf63b68754e8ff) )
	ROM_LOAD( "tf1_voi1.bin",       0x30000, 0x20000, CRC(615d09cd) SHA1(0aecf7ca6b65ddfcdcf74f8d412169ec800ba3a3) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "tf1_chr8.bin",       0x00000, 0x20000, CRC(7d53b31e) SHA1(7e4b5fc92f7956477392f1e14c6edfc0cada2be0) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "tf1_chr0.bin",       0x00000, 0x20000, CRC(9e91794e) SHA1(e873ed31fafae02a3c3d9127801a13f67cbdb5f4) )
	ROM_LOAD( "tf1_chr1.bin",       0x20000, 0x20000, CRC(76e1bc56) SHA1(f95e4100cad29a8972e4f435b4f9ee3cdc4f4504) )
	ROM_LOAD( "tf1_chr2.bin",       0x40000, 0x20000, CRC(fcb645d9) SHA1(afedb4c6c1b02444ddb1757ac9808df1b94de2d8) )
	ROM_LOAD( "tf1_chr3.bin",       0x60000, 0x20000, CRC(a8dbf080) SHA1(115f3b21e0546000076c3c4d588e5bdcc4cf50d5) )
	ROM_LOAD( "tf1_chr4.bin",       0x80000, 0x20000, CRC(51fedc8c) SHA1(4a0c337fe614ebf66ee0fc3cfb05cad51a048fcf) )
	ROM_LOAD( "tf1_chr5.bin",       0xa0000, 0x20000, CRC(e6c6609a) SHA1(0c05cdd6b1dc4f082b9ac0e4de252c3d72392bb0) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "tf1_obj0.bin",       0x00000, 0x20000, CRC(4bedd51a) SHA1(fedb972986efe5ff3bd6c75fa16f22f8267ee0b7) )
	ROM_LOAD( "tf1_obj1.bin",       0x20000, 0x20000, CRC(df674d6d) SHA1(65935c2d6072228fcb2fbca8808fc921db9e0ddd) )
ROM_END

/* Tank Force (Japan) */
ROM_START( tankfrcj )
	ROM_REGION( 0x2c000, REGION_CPU3, 0 )       /* 176k for the sound cpu */
	ROM_LOAD( "tf1_snd0.bin",       0x0c000, 0x20000, CRC(4d9cf7aa) SHA1(de51b9b36e9a530a7f3c35672ec72c19b607af04) )

	ROM_REGION( 0x400000, REGION_USER1, 0 ) /* 4M for ROMs */
	ROM_LOAD_1024( "tf1_prg0.bin",       0x000000, CRC(2ae4b9eb) SHA1(569d2754398b4276cf78a3dd038b5884778dc82e) )
	ROM_LOAD_1024( "tf1_prg1.bin",       0x080000, CRC(4a8bb251) SHA1(1df46ccf0ad7260398b7965e3825e936ba357062) )
	/* 100000-17ffff empty */
	/* 180000-1fffff empty */
	/* 200000-27ffff empty */
	/* 280000-2fffff empty */
	/* 300000-37ffff empty */
	ROM_LOAD_1024( "tf1_prg7.bin",       0x380000, CRC(9dfa0dd5) SHA1(eeb6904225e7dedb0d134e5634aa5321237f68ac) )

	ROM_REGION( 0xd0000, REGION_CPU4, 0 )       /* the MCU & voice */
	ROM_LOAD( "ns1-mcu.bin",        0x0f000, 0x01000, CRC(ffb5c0bd) SHA1(7a38c0cc2553c627f4ec507fb6e807cf7d537c02) )
	ROM_LOAD( "tf1_voi0.bin",       0x10000, 0x20000, CRC(f542676a) SHA1(38d54db0807c58152bd120c393bf63b68754e8ff) )
	ROM_LOAD( "tf1_voi1.bin",       0x30000, 0x20000, CRC(615d09cd) SHA1(0aecf7ca6b65ddfcdcf74f8d412169ec800ba3a3) )

	ROM_REGION( 0x20000, REGION_GFX1, 0 )  /* character mask */
	ROM_LOAD( "tf1_chr8.bin",       0x00000, 0x20000, CRC(7d53b31e) SHA1(7e4b5fc92f7956477392f1e14c6edfc0cada2be0) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "tf1_chr0.bin",       0x00000, 0x20000, CRC(9e91794e) SHA1(e873ed31fafae02a3c3d9127801a13f67cbdb5f4) )
	ROM_LOAD( "tf1_chr1.bin",       0x20000, 0x20000, CRC(76e1bc56) SHA1(f95e4100cad29a8972e4f435b4f9ee3cdc4f4504) )
	ROM_LOAD( "tf1_chr2.bin",       0x40000, 0x20000, CRC(fcb645d9) SHA1(afedb4c6c1b02444ddb1757ac9808df1b94de2d8) )
	ROM_LOAD( "tf1_chr3.bin",       0x60000, 0x20000, CRC(a8dbf080) SHA1(115f3b21e0546000076c3c4d588e5bdcc4cf50d5) )
	ROM_LOAD( "tf1_chr4.bin",       0x80000, 0x20000, CRC(51fedc8c) SHA1(4a0c337fe614ebf66ee0fc3cfb05cad51a048fcf) )
	ROM_LOAD( "tf1_chr5.bin",       0xa0000, 0x20000, CRC(e6c6609a) SHA1(0c05cdd6b1dc4f082b9ac0e4de252c3d72392bb0) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "tf1_obj0.bin",       0x00000, 0x20000, CRC(4bedd51a) SHA1(fedb972986efe5ff3bd6c75fa16f22f8267ee0b7) )
	ROM_LOAD( "tf1_obj1.bin",       0x20000, 0x20000, CRC(df674d6d) SHA1(65935c2d6072228fcb2fbca8808fc921db9e0ddd) )
ROM_END



GAME( 1987, shadowld, 0,        ns1,     shadowld, shadowld, ROT180, "Namco", "Shadowland", 0 ,0)
GAME( 1987, youkaidk, shadowld, ns1,     shadowld, shadowld, ROT180, "Namco", "Yokai Douchuuki (Japan new version)", 0 ,0)
GAME( 1987, yokaidko, shadowld, ns1,     shadowld, shadowld, ROT180, "Namco", "Yokai Douchuuki (Japan old version)", 0 ,0)
GAME( 1987, dspirit,  0,        ns1,     dspirit,  dspirit,  ROT90,  "Namco", "Dragon Spirit (new version)", 0 ,2)
GAME( 1987, dspirito, dspirit,  ns1,     dspirit,  dspirit,  ROT90,  "Namco", "Dragon Spirit (old version)", 0 ,2)
GAME( 1987, blazer,   0,        ns1,     ns1,      blazer,   ROT90,  "Namco", "Blazer (Japan)", 0 ,0)
GAME( 1987, quester,  0,        ns1,     quester,  quester,  ROT90,  "Namco", "Quester (Japan)", 0 ,0)
GAME( 1987, pacmania, 0,        ns1,     pacmania, pacmania, ROT270, "Namco", "Pac-Mania", 0 ,1)
GAME( 1987, pacmanij, pacmania, ns1,     pacmania, pacmania, ROT90,  "Namco", "Pac-Mania (Japan)", 0 ,1)
GAME( 1987, galaga88, 0,        ns1,     galaga88, galaga88, ROT270, "Namco", "Galaga '88", 0 ,2)
GAME( 1987, galag88j, galaga88, ns1,     galaga88, galaga88, ROT90,  "Namco", "Galaga '88 (Japan)", 0 ,2)
GAME( 1988, ws,       0,        ns1,     ns1,      ws,       ROT180, "Namco", "World Stadium (Japan)", 0 ,0)
GAME( 1988, berabohm, 0,        ns1,     berabohm, berabohm, ROT180, "Namco", "Beraboh Man (Japan version C)", 0 ,2)
GAME( 1988, beraboho, berabohm, ns1,     berabohm, berabohm, ROT180, "Namco", "Beraboh Man (Japan version B)", 0 ,2)
GAME( 1988, mmaze,    0,        ns1,     mmaze,    alice,    ROT180, "Namco", "Marchen Maze (Japan)", 0 ,0)
GAME( 1988, bakutotu, 0,        ns1,     bakutotu, bakutotu, ROT180, "Namco", "Bakutotsu Kijuutei", 0 ,2)
GAME( 1988, wldcourt, 0,        ns1,     wldcourt, wldcourt, ROT180, "Namco", "World Court (Japan)", 0 ,0)
GAME( 1988, splatter, 0,        ns1,     splatter, splatter, ROT180, "Namco", "Splatter House (World)", 0 ,0)
GAME( 1988, splattej, splatter, ns1,     splatter, splatter, ROT180, "Namco", "Splatter House (Japan)", 0 ,0)
GAME( 1988, faceoff,  0,        ns1,     faceoff,  faceoff,  ROT180, "Namco", "Face Off (Japan)", 0 ,0)
GAME( 1989, rompers,  0,        ns1,     ns1,      rompers,  ROT90,  "Namco", "Rompers (Japan)", 0 ,0)
GAME( 1989, romperso, rompers,  ns1,     ns1,      rompers,  ROT90,  "Namco", "Rompers (Japan old version)", 0 ,0)
GAME( 1989, blastoff, 0,        ns1,     ns1,      blastoff, ROT90,  "Namco", "Blast Off (Japan)", 0 ,2)
GAME( 1989, ws89,     ws,       ns1,     ws89,     ws89,     ROT180, "Namco", "World Stadium '89 (Japan)", 0 ,0)
GAME( 1989, dangseed, 0,        ns1,     dangseed, dangseed, ROT90,  "Namco", "Dangerous Seed (Japan)", 0 ,0)
GAME( 1990, ws90,     ws,       ns1,     ws90,     ws90,     ROT180, "Namco", "World Stadium '90 (Japan)", 0 ,0)
GAME( 1990, pistoldm, 0,        ns1,     ns1,      pistoldm, ROT0,   "Namco", "Pistol Daimyo no Bouken (Japan)", 0 ,0)
GAME( 1990, boxyboy,  0,        ns1,     boxyboy,  soukobdx, ROT0,   "Namco", "Boxy Boy (US)", 0 ,0)
GAME( 1990, soukobdx, boxyboy,  ns1,     boxyboy,  soukobdx, ROT0,   "Namco", "Souko Ban Deluxe (Japan)", 0 ,0)
GAME( 1990, puzlclub, 0,        ns1,     puzlclub, puzlclub, ROT90,  "Namco", "Puzzle Club (Japan prototype)", 0 ,0)
GAME( 1991, tankfrce, 0,        ns1,     ns1,      tankfrce, ROT0,   "Namco", "Tank Force (US)", 0 ,0)
GAME( 1991, tankfrcj, tankfrce, ns1,     ns1,      tankfrce, ROT0,   "Namco", "Tank Force (Japan)", 0 ,0)

