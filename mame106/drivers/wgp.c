/***************************************************************************

World Grand Prix    (c) Taito Corporation 1989
================

David Graves

(Thanks to Richard Bush and the Raine team for their
preliminary driver.)

It seems likely there are a LOT of undumped versions of this game...
If you have access to a board, please check the rom numbers to see if
any are different from the ones listed below.

                *****

World Grand Prix runs on hardware which is pretty different from the
system Taito commonly used for their pseudo-3d racing games of the
time, the Z system. Different screen and sprite hardware is used.
There's also a LAN hookup (for multiple machines).

As well as a TC0100SCN tilemap generator (two 64x64 layers of 8x8
tiles and a layer of 8x8 tiles with graphics data taken from RAM)
there's a "piv" tilemap generator, which creates three scrollable
row-scrollable zoomable 64x64 tilemaps composed of 16x16 tiles.

As well as these six tilemap layers, there is a sprite layer with
individually zoomable / rotatable sprites. Big sprites are created
from 16x16 gfx chunks via a sprite mapping area in RAM.

The piv and sprite layers are rotatable (but not individually, only
together).

World Grand Prix has twin 68K CPUs which communicate via $4000 bytes
of shared ram.

There is a Z80 as well, which takes over sound duties. Commands are
written to it by the one of the 68000s (the same as Taito F2 games).

Dumper's info (Raine)
-------------

    TC0100SCN - known
    TC0140SYT - known
    TC0170ABT - ?
    TC0220IOC - known
    TC0240PBJ - motion objects ??
    TC0250SCR - piv tilemaps ?? [TC0280GRD was a rotatable tilemap chip in DonDokoD, also 1989]
    TC0260DAR - color related, paired with TC0360PRI in many F2 games
    TC0330CHL - ? (perhaps lan related)
This game has LAN interface board, it uses uPD72105C.

Video Map
---------

400000 area is used for sprites. The game does tile mapping
    in ram to create big sprites from the 16x16 tiles of gfx0.
    See notes in \vidhrdw\ for details.

500000 area is for the "piv" tilemaps.
    See notes in \vidhrdw\ for details.


TODO
====

Offer fake-dip selectable analogue steer

Is piv/sprite layers rotation control at 0x600000 ?

Verify y-zoom is correct on the stages that use it (including Wgp2
default course). Row zoom may be hard to verify, but Wgp2 course
selection screen is probably a good test.

Implement proper positioning/zoom/rotation for sprites.

(The current sprite coord calculations are kludgy and flawed and
ignore four control words. The sprites also seem jerky
and have [int?] timing glitches.)

DIP coinage


Wgp
---

Analogue brake pedal works but won't register in service mode.

$28336 is code that shows brake value in service mode.
$ac3e sub (called off int4) at $ac78 does three calcs to the six
    AD values: results are stored at ($d22 / $d24 / $d26,A5)
    It is the third one, ($d26,A5) which gets displayed
    in service mode as brake.


Wgp2
----

Piv y zoom may be imperfect. Check the up/down hill part of the
default course. The road looks a little odd.

Sprite colors seem ok except smoke after you crash. (And one sign on
first bend of default course doesn't go yellow for a few frames.)

[Used to die with common ram error. When CPUA enables CPUB, CPUB
writes to $140000/2 - unfortunately while CPUA is in the middle of
testing that ram. We hack prog for CPUB to disable the writes.]


                *****

[Wgp stopped with LAN error. (Looks like CPUB tells CPUA what is wrong
with LAN in shared ram $142048. Examined at $e57c which prints out
relevant lan error message). Ended up at $e57c from $b14e-xx code
section. CPUA does PEA of $e57c which is the fallback if CPUB doesn't
respond in timely fashion to command 0x1 poked by code at $b1a6.

CPUB $830c-8332 loops waiting for command 1-8 in $140002 from CPUA.
it executes this then clears $140002 to indicate command completed.
It wasn't clearing this fast enough for CPUA, because $142048 wasn't
clear:- and that affects the amount of code command 0x1 runs.

CPUB code at $104d8 had already detected error on LAN and set $142048
to 4. We now return correct lan status read from $380000, so
no LAN problem is detected.]


Code Documentation
------------------

CPUA
----

$1064e main game loop starting with a trap#$5

Calls $37e78 sub off which spriteram eventually gets updated

Strangely main loop does not seem to be synced to any interrupt.
This may be why the sprites are so glitchy and don't seem to
update every frame. Maybe trap#$5 should be getting us to a known
point in the frame ??

$21f4 copies sprite tilemapping data from data rom to tilemap
area then flows into:

$223c code that fills a sprite entry in spriteram

[$12770 - $133cd seems to be an irregular lookup table used to
calculate extra zoom word poked into each sprite entry.]

$23a8 picks data out of data rom and stores it to sprite tile-
mapping area by heading through to $21f4. (May also enter sprite
details into spriteram.) It uses $a0000 area of data rom.

(Alternate entry point at $23c2 uses $90000 area of data rom.)

$25ee routine stores data from rom into sprite tilemapping area
including the bad parts that produce junk sprites.

It calls interesting sub at $25be which has a table of the number
of sequential words to be pulled out of the data rom, depending
on the first word value in the data rom for that entry ("code").
Each code will pull out the following multiple of 16 words:

    Code  Words   Tiles    Actual data rom entry
     0      1      4x4      [same]
     1      2      8x4      [same]
     2      4      8x8      [same]
     3      3      12x4     [same, see $98186]
     4      6      12x8
     5      9      12x12    [same]
     6      2      4x8      [WRONG! says 8x12 in data rom, see $982bf]
     7      6      8x12     [WRONG! says 4x8 in data rom]
     8      1      (2x2)*4  [2x2 in data rom]  (copies 12 unwanted
                                      words - causing junk sprites)

$4083c0-47f in sprite mapping area has junk words - due to code 7
making it read 6*16 words. 0x60 words are copied from the data rom
when 0x20 would be correct. Careless programming: in the lookup
table Taito got codes 6 and 7 back to front. Enable the patch in
init_wgp to correct this... I can't see what changes.

I'm guessing sprites may be variable size, and the junk sprites
mapped +0x9b80-9d80 are 2x2 tiles rather than 4x4.

If we only use the first 4 tilemapping words, then the junk sprites
look fine. But their spacing is bad: they have gaps left between
them. They'll need to be magnified to 2x size - the pixel zoom
value must do this automatically.

This ties in with the lookup table we need to draw the sprites:
it makes sense if our standard 4x4 tile sprite is actually 4 2x2
sprites.

But what tells the sprite hardware if a sprite is 2x2 or 4x4 ??


Data rom entry
--------------

+0x00  Control word
        (determines how many words copied to tilemapping area)

+0x01  Sprite X size  [tiles]
        (2,4,8 or 12)

+0x02  Sprite Y size  [tiles]
        (2,4,8 or 12)

+0x03  sprite tile words, total (X size * Y size)
 on...

The X size and Y size values don't seem to be used by the game, and
may be a hangover from the gfx development system used.


Data Rom
--------

$80000-$8ffff   piv tilemaps, preceded by lookup table
$90000-$9ffff   sprite tilemap data, preceded by lookup table
$a0000-$affff   sprite tilemap data, preceded by lookup table
$b0000-$cffff   TC0100SCN tilemaps, preceded by lookup table

    Note that only just over half this space is used, rest is
    0xff fill.

$d0000-$dffff is the pivot port data table

    Four separate start points are available, contained in the
    first 4 long words.

    (Data words up to $da410 then 0xff fill)

$e0000-$e7ffe is a logarithmic curve table

    This is used to look up perceived height of an object at
    "distance" (offset/2).

    ffff 8000 5555 4000 3333 2aaa  etc.
    (tapering to value of 4 towards the end)

    The sprite routine shifts this one bit right and subtracts one
    before poking into spriteram. Hence 4 => 1

$e7fff-$e83fe is an unknown word table
$e83ff-$effff empty (0xff fill)
$f0000-$f03ff unknown lookup table
$f0400-$fcbff series of unknown lookup tables

    Seems to be a series of (zoom/rotate ??) lookup
    tables all $400 words long. (Total no. of tables = 25?)

    Each table starts at zero and tends upwards: the first reaches 0xfe7.
    The final one reaches 0x3ff (i.e. each successive word is 1 higher
    than the last). The values in the tables tend to go up smoothly but
    with discontinuities at regular intervals. The intervals change
    between tables.

$fcc00-$fffff empty (0xff fill)


Additional notes :

1) 'wgp' and 'wgpj'

LAN stuff :

LAN RAM seems to be 0x4000 bytes wide (0x380000-0x383fff in CPUB)

Lan tests start at 0x00f86a (CPUB) where a copy of the 256 bytes from
0x00f8d4 is made to 0x383f00. This is text about the version of the LAN
stuff ("1990 VER 1.06") . Note that at least version 1.05 is required.

Dip Switches :

To see the effect of the "Communication" Dip Switch you must add the memory
read/write handlers for 0x380000 to 0x383fff instead of using the 'lan_status_r'
one. Of course, there will be an error message, but this might help in
finding the useful addresses in the LAN RAM.

Note that the first time you run the game with the new handlers (I've put
standard RAM, let me know if you have a better idea), you'll need to reset
the game ! Is it because the tests in CPUB are too late ?

In the "test mode", if "Communication" Dip Switch is ON, you'll see "NG"
("Not Good") under each machine ID (this is logical).

Be aware that the "Machine ID" Dip Switch must be set to 1, or the tests are
NOT performed (I can't explain why for the moment) !

The "Machine ID" determines which motorbike you will use and has an incidence
on the start position on the grid. Motorbike 2 seems to be the best one as
it is always on 1st position (against 2nd position for motorbike 1).

The "Slave / Master" Dip Switch is only a guess according to some other linked
machines (eg: "Cadash"), and due to incomplete LAN RAM emulation, I haven't
seen any visible effect 8(

The "Motor Test" Dip Switch is a feature that is available (or not) in the
"test mode". I can't tell for the moment if it has any effect during gameplay.

2) 'wgpj'

LAN stuff :

Same stuff for the LAN RAM as in 'wgp' (the ROMS for CPUB are the same) ...

Dip Switches :

Same as the ones for 'wgp' if you except the "Coinage" Dip Switches.

3) 'wgp2'

LAN stuff :

I haven't tested the LAN stuff for the moment.

Dip Switches :

They are almost the same as for 'wgpj' : the only is that there is no
"Motor Test" Dip Switch has this feature is ALWAYS available in the "test mode".

4) 'wgpjoy'

LAN stuff :

It is very surprising, but you also find the text about the LAN stuff in
the ROMS (still version 1.06), but you can't perform lan tests in the
"test mode".

5) 'wgpjoya'

LAN stuff :

It is very surprising, but you also find the text about the LAN stuff in
the ROMS (still version 1.06), but you can't perform lan tests in the
"test mode".

As the LAN version is the same, I'll have to look at the code to see where
the differences are.

***************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "vidhrdw/taitoic.h"
#include "sndhrdw/taitosnd.h"
#include "sound/2610intf.h"

VIDEO_START( wgp );
VIDEO_START( wgp2 );
VIDEO_UPDATE( wgp );

extern UINT16 *wgp_spritemap;
extern size_t    wgp_spritemap_size;

extern UINT16 *wgp_pivram;
READ16_HANDLER ( wgp_pivram_word_r );
WRITE16_HANDLER( wgp_pivram_word_w );

extern UINT16 *wgp_piv_ctrlram;
READ16_HANDLER ( wgp_piv_ctrl_word_r );
WRITE16_HANDLER( wgp_piv_ctrl_word_w );

static UINT16 cpua_ctrl = 0xff;
static UINT16 port_sel=0;
extern UINT16 wgp_rotate_ctrl[8];

static UINT16 *sharedram;
static size_t sharedram_size;

static READ16_HANDLER( sharedram_r )
{
	return sharedram[offset];
}

static WRITE16_HANDLER( sharedram_w )
{
	COMBINE_DATA(&sharedram[offset]);
}

static void parse_control(void)
{
	/* bit 0 enables cpu B */
	/* however this fails when recovering from a save state
       if cpu B is disabled !! */
	cpunum_set_input_line(2, INPUT_LINE_RESET, (cpua_ctrl &0x1) ? CLEAR_LINE : ASSERT_LINE);

	/* bit 1 is "vibration" acc. to test mode */
}

static WRITE16_HANDLER( cpua_ctrl_w )	/* assumes Z80 sandwiched between 68Ks */
{
	if ((data &0xff00) && ((data &0xff) == 0))
		data = data >> 8;	/* for Wgp */
	cpua_ctrl = data;

	parse_control();

	loginfo(2,"CPU #0 PC %06x: write %04x to cpu control\n",activecpu_get_pc(),data);
}


/***********************************************************
                        INTERRUPTS
***********************************************************/

/* 68000 A */

/*
void wgp_interrupt4(int x)
{
    cpunum_set_input_line(0,4,HOLD_LINE);
}
*/

void wgp_interrupt6(int x)
{
	cpunum_set_input_line(0,6,HOLD_LINE);
}

/* 68000 B */

void wgp_cpub_interrupt6(int x)
{
	cpunum_set_input_line(2,6,HOLD_LINE);	/* assumes Z80 sandwiched between the 68Ks */
}



/***** Routines for particular games *****/

/* FWIW offset of 10000,10500 on ints can get CPUB obeying the
   first CPUA command the same frame; probably not necessary */

static INTERRUPT_GEN( wgp_cpub_interrupt )
{
	timer_set(TIME_IN_CYCLES(200000-500,0),0, wgp_cpub_interrupt6);
	cpunum_set_input_line(2, 4, HOLD_LINE);
}


/**********************************************************
                         GAME INPUTS
**********************************************************/

static READ16_HANDLER( lan_status_r )
{
	loginfo(2,"CPU #2 PC %06x: warning - read lan status\n",activecpu_get_pc());

	return  (0x4 << 8);	/* CPUB expects this in code at $104d0 (Wgp) */
}

static WRITE16_HANDLER( rotate_port_w )
{
	/* This port may be for piv/sprite layer rotation.

    Wgp2 pokes a single set of values (see 2 routines from
    $4e4a), so if this is rotation then Wgp2 *doesn't* use
    it.

    Wgp pokes a wide variety of values here, which appear
    to move up and down as rotation control words might.
    See $ae06-d8 which pokes piv ctrl words, then pokes
    values to this port.

    There is a lookup area in the data rom from $d0000-$da400
    which contains sets of 4 words (used for ports 0-3).
    NB: port 6 is not written.
    */

	switch (offset)
	{
		case 0x00:
		{
//loginfo(2,"CPU #0 PC %06x: warning - port %04x write %04x\n",activecpu_get_pc(),port_sel,data);

			wgp_rotate_ctrl[port_sel] = data;
			return;
		}

		case 0x01:
		{
			port_sel = data &0x7;
		}
	}
}


static READ16_HANDLER( wgp_adinput_r )
{
	int steer = 0x40;
	int fake = input_port_5_word_r(0,0);

	if (!(fake &0x10))	/* Analogue steer (the real control method) */
	{
		/* Reduce span to 0x80 */
		steer = ((input_port_6_word_r(0,0)) * 0x80) / 0x100;
	}
	else	/* Digital steer */
	{
		if (fake & 0x8)	/* pressing down */
			steer = 0x20;

		if (fake & 0x4)	/* pressing up */
			steer = 0x60;

		if (fake & 0x2)	/* pressing right */
			steer = 0x00;

		if (fake & 0x1)	/* pressing left */
			steer = 0x80;
	}

	switch (offset)
	{
		case 0x00:
		{
			if (input_port_5_word_r(0,0) &0x40)	/* pressing accel */
				return 0xff;
			else
				return 0x00;
		}

		case 0x01:
			return steer;

		case 0x02:
			return 0xc0; 	/* steer offset, correct acc. to service mode */

		case 0x03:
			return 0xbf;	/* accel offset, correct acc. to service mode */

		case 0x04:
		{
			if (input_port_5_word_r(0,0) &0x80)	/* pressing brake */
				return 0xcf;
			else
				return 0xff;
		}

		case 0x05:
			return input_port_7_word_r(0,0);	/* unknown */
	}

loginfo(2,"CPU #0 PC %06x: warning - read unmapped a/d input offset %06x\n",activecpu_get_pc(),offset);

	return 0xff;
}

static WRITE16_HANDLER( wgp_adinput_w )
{
	/* Each write invites a new interrupt as soon as the
       hardware has got the next a/d conversion ready. We set a token
       delay of 10000 cycles although our inputs are always ready. */

	timer_set(TIME_IN_CYCLES(10000,0),0, wgp_interrupt6);
}


/**********************************************************
                          SOUND
**********************************************************/

static INT32 banknum = -1;

static void reset_sound_region(void)	/* assumes Z80 sandwiched between the 68Ks */
{
	memory_set_bankptr( 10, memory_region(REGION_CPU2) + (banknum * 0x4000) + 0x10000 );
}

static WRITE8_HANDLER( sound_bankswitch_w )
{
	banknum = (data - 1) & 7;
	reset_sound_region();
}

WRITE16_HANDLER( wgp_sound_w )
{
	if (offset == 0)
		taitosound_port_w (0, data & 0xff);
	else if (offset == 1)
		taitosound_comm_w (0, data & 0xff);
}

READ16_HANDLER( wgp_sound_r )
{
	if (offset == 1)
		return ((taitosound_comm_r (0) & 0xff));
	else return 0;
}


/*****************************************************************
                         MEMORY STRUCTURES
*****************************************************************/

static ADDRESS_MAP_START( wgp_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x100000, 0x10ffff) AM_READ(MRA16_RAM)	/* main CPUA ram */
	AM_RANGE(0x140000, 0x143fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x180000, 0x18000f) AM_READ(TC0220IOC_halfword_byteswap_r)
	AM_RANGE(0x200000, 0x20000f) AM_READ(wgp_adinput_r)
	AM_RANGE(0x300000, 0x30ffff) AM_READ(TC0100SCN_word_0_r)	/* tilemaps */
	AM_RANGE(0x320000, 0x32000f) AM_READ(TC0100SCN_ctrl_word_0_r)
	AM_RANGE(0x400000, 0x40bfff) AM_READ(MRA16_RAM)	/* sprite tilemaps */
	AM_RANGE(0x40c000, 0x40dfff) AM_READ(MRA16_RAM)	/* sprite ram */
	AM_RANGE(0x500000, 0x501fff) AM_READ(MRA16_RAM)	/* unknown/unused */
	AM_RANGE(0x502000, 0x517fff) AM_READ(wgp_pivram_word_r)	/* piv tilemaps */
	AM_RANGE(0x520000, 0x52001f) AM_READ(wgp_piv_ctrl_word_r)
	AM_RANGE(0x700000, 0x701fff) AM_READ(paletteram16_word_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( wgp_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x100000, 0x10ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x140000, 0x143fff) AM_WRITE(MWA16_RAM) AM_BASE(&sharedram) AM_SIZE(&sharedram_size)
	AM_RANGE(0x180000, 0x18000f) AM_WRITE(TC0220IOC_halfword_byteswap_w)
	AM_RANGE(0x1c0000, 0x1c0001) AM_WRITE(cpua_ctrl_w)
	AM_RANGE(0x200000, 0x20000f) AM_WRITE(wgp_adinput_w)
	AM_RANGE(0x300000, 0x30ffff) AM_WRITE(TC0100SCN_word_0_w)	/* tilemaps */
	AM_RANGE(0x320000, 0x32000f) AM_WRITE(TC0100SCN_ctrl_word_0_w)
	AM_RANGE(0x400000, 0x40bfff) AM_WRITE(MWA16_RAM) AM_BASE(&wgp_spritemap) AM_SIZE(&wgp_spritemap_size)
	AM_RANGE(0x40c000, 0x40dfff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)	/* sprite ram */
	AM_RANGE(0x40fff0, 0x40fff1) AM_WRITE(MWA16_NOP)	// ?? (writes 0x8000 and 0 alternately - Wgp2 just 0)
	AM_RANGE(0x500000, 0x501fff) AM_WRITE(MWA16_RAM)	/* unknown/unused */
	AM_RANGE(0x502000, 0x517fff) AM_WRITE(wgp_pivram_word_w) AM_BASE(&wgp_pivram)	/* piv tilemaps */
	AM_RANGE(0x520000, 0x52001f) AM_WRITE(wgp_piv_ctrl_word_w) AM_BASE(&wgp_piv_ctrlram)
	AM_RANGE(0x600000, 0x600003) AM_WRITE(rotate_port_w)	/* rotation control ? */
	AM_RANGE(0x700000, 0x701fff) AM_WRITE(paletteram16_RRRRGGGGBBBBxxxx_word_w) AM_BASE(&paletteram16)
ADDRESS_MAP_END

static ADDRESS_MAP_START( wgp_cpub_readmem, ADDRESS_SPACE_PROGRAM, 16 )	/* LAN areas not mapped... */
	AM_RANGE(0x000000, 0x03ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x100000, 0x103fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x140000, 0x143fff) AM_READ(sharedram_r)
	AM_RANGE(0x200000, 0x200003) AM_READ(wgp_sound_r)
//  AM_RANGE(0x380000, 0x383fff) AM_READ(MRA16_RAM) // LAN RAM
	AM_RANGE(0x380000, 0x380001) AM_READ(lan_status_r)	// ??
	// a lan input area is read somewhere above the status
	// (make the status return 0 and log)...
ADDRESS_MAP_END

static ADDRESS_MAP_START( wgp_cpub_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x100000, 0x103fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x140000, 0x143fff) AM_WRITE(sharedram_w)
	AM_RANGE(0x200000, 0x200003) AM_WRITE(wgp_sound_w)
//  AM_RANGE(0x380000, 0x383fff) AM_WRITE(MWA16_RAM)    // LAN RAM
ADDRESS_MAP_END


/***************************************************************************/

static ADDRESS_MAP_START( z80_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x4000, 0x7fff) AM_READ(MRA8_BANK10)
	AM_RANGE(0xc000, 0xdfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xe000) AM_READ(YM2610_status_port_0_A_r)
	AM_RANGE(0xe001, 0xe001) AM_READ(YM2610_read_port_0_r)
	AM_RANGE(0xe002, 0xe002) AM_READ(YM2610_status_port_0_B_r)
	AM_RANGE(0xe200, 0xe200) AM_READ(MRA8_NOP)
	AM_RANGE(0xe201, 0xe201) AM_READ(taitosound_slave_comm_r)
	AM_RANGE(0xea00, 0xea00) AM_READ(MRA8_NOP)
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xdfff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(YM2610_control_port_0_A_w)
	AM_RANGE(0xe001, 0xe001) AM_WRITE(YM2610_data_port_0_A_w)
	AM_RANGE(0xe002, 0xe002) AM_WRITE(YM2610_control_port_0_B_w)
	AM_RANGE(0xe003, 0xe003) AM_WRITE(YM2610_data_port_0_B_w)
	AM_RANGE(0xe200, 0xe200) AM_WRITE(taitosound_slave_port_w)
	AM_RANGE(0xe201, 0xe201) AM_WRITE(taitosound_slave_comm_w)
	AM_RANGE(0xe400, 0xe403) AM_WRITE(MWA8_NOP) /* pan */
	AM_RANGE(0xee00, 0xee00) AM_WRITE(MWA8_NOP) /* ? */
	AM_RANGE(0xf000, 0xf000) AM_WRITE(MWA8_NOP) /* ? */
	AM_RANGE(0xf200, 0xf200) AM_WRITE(sound_bankswitch_w)
ADDRESS_MAP_END


/***********************************************************
                      INPUT PORTS, DIPs
***********************************************************/

/* Duplicated macros from most other Taito drivers :

    asuka.c
    exzisus.c
    ninjaw.c
    opwolf.c
    othunder.c
    taito_b.c
    taito_f2.c
    taito_h.c
    taito_l.c
    taito_x.c
    taitoair.c
    topspeed.c
    warriorb.c
*/

/* Same as TAITO_DIFFICULTY_8 in most Taito drivers. */
#define TAITO_DIFFICULTY_8 \
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) ) \
	PORT_DIPSETTING(    0x03, DEF_STR( Medium ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

/* Same as TAITO_COINAGE_JAPAN_NEW_8 in most Taito drivers. */
#define TAITO_COINAGE_JAPAN_NEW_8 \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )

/* Same as TAITO_COINAGE_US_8 in most Taito drivers. */
#define TAITO_COINAGE_US_8 \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coinage ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, "Price to Continue" ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0xc0, "Same as Start" )


/* Duplicated macros from some other Taito drivers */

/* Same as TAITO_B_DSWA_2_4 in taito_b.c and TAITO_L_DSWA_2_4 in taito_l.c */
#define TAITO_WGP_DSWA_2_4 \
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Flip_Screen ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_SERVICE( 0x04, IP_ACTIVE_LOW ) \
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )

/* Same as TAITO_X_SYSTEM_INPUT in taito_x.c */
#define TAITO_WGP_SYSTEM_INPUT \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_TILT ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )


/* Driver specific macros */

/* Slightly different from TAITO_COINAGE_WORLD_8 in MANY Taito drivers :
   1C_7C instead of 1C_6C for "Coin B". */
#define WGP_COINAGE_WORLD_8 \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_7C ) )

#define WGP_MACHINE_ID_8 \
	PORT_DIPNAME( 0xe0, 0xe0, "Machine ID" ) \
	PORT_DIPSETTING(    0xe0, "1" ) \
	PORT_DIPSETTING(    0xc0, "2" ) \
	PORT_DIPSETTING(    0xa0, "3" ) \
	PORT_DIPSETTING(    0x80, "4" ) \
	PORT_DIPSETTING(    0x60, "5" ) \
	PORT_DIPSETTING(    0x40, "6" ) \
	PORT_DIPSETTING(    0x20, "7" ) \
	PORT_DIPSETTING(    0x00, "8" )


INPUT_PORTS_START( wgp )
	PORT_START /* DSW A */
	PORT_DIPNAME( 0x01, 0x01, "Motor Test" )				// Only available in "test mode"
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	TAITO_WGP_DSWA_2_4
	TAITO_COINAGE_US_8

	PORT_START /* DSW B */
	TAITO_DIFFICULTY_8
	PORT_DIPNAME( 0x04, 0x04, "Shift Pattern Select" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Slave / Master ???" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Communication" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	WGP_MACHINE_ID_8

	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_BUTTON7 ) PORT_PLAYER(1)	/* freeze */
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_BUTTON4 ) PORT_PLAYER(1)	/* shift up */
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(1)	/* shift down */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(1)	/* "start lump" (lamp?) */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(1)	/* "brake lump" (lamp?) */
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN2 */
	TAITO_WGP_SYSTEM_INPUT
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START      /* fake inputs, allowing digital steer etc. */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_DIPNAME( 0x10, 0x10, "Steering type" )
	PORT_DIPSETTING(    0x10, "Digital" )
	PORT_DIPSETTING(    0x00, "Analogue" )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)	/* accel */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)	/* brake */

/* It's not clear for accel and brake which is the input and
   which the offset, but that doesn't matter. These continuous
   inputs are replaced by discrete values derived from the fake
   input port above, so keyboard control is feasible. */

//  PORT_START  /* accel, 0-255 */
//  PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(25) PORT_REVERSE PORT_PLAYER(1)

	PORT_START	/* steer */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(25) PORT_REVERSE PORT_PLAYER(1)

//  PORT_START  /* steer offset */

//  PORT_START  /* accel offset */

//  PORT_START  /* brake, 0-0x30: needs to start at 0xff; then 0xcf is max brake */
//  PORT_BIT( 0xff, 0xff, IPT_AD_STICK_X ) PORT_MINMAX(0xcf,0xff) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START	/* unknown */
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(2)
INPUT_PORTS_END

/* Same as 'wgp', but different coinage */
INPUT_PORTS_START( wgpj )
	PORT_START /* DSW A */
	PORT_DIPNAME( 0x01, 0x01, "Motor Test" )				// Only available in "test mode"
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	TAITO_WGP_DSWA_2_4
	TAITO_COINAGE_JAPAN_NEW_8

	PORT_START /* DSW B */
	TAITO_DIFFICULTY_8
	PORT_DIPNAME( 0x04, 0x04, "Shift Pattern Select" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Slave / Master ???" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Communication" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	WGP_MACHINE_ID_8

	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_BUTTON7 ) PORT_PLAYER(1)	/* freeze */
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_BUTTON4 ) PORT_PLAYER(1)	/* shift up */
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(1)	/* shift down */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(1)	/* "start lump" (lamp?) */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(1)	/* "brake lump" (lamp?) */
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN2 */
	TAITO_WGP_SYSTEM_INPUT
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START      /* fake inputs, allowing digital steer etc. */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_DIPNAME( 0x10, 0x10, "Steering type" )
	PORT_DIPSETTING(    0x10, "Digital" )
	PORT_DIPSETTING(    0x00, "Analogue" )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)	/* accel */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)	/* brake */

/* It's not clear for accel and brake which is the input and
   which the offset, but that doesn't matter. These continuous
   inputs are replaced by discrete values derived from the fake
   input port above, so keyboard control is feasible. */

//  PORT_START  /* accel, 0-255 */
//  PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(25) PORT_REVERSE PORT_PLAYER(1)

	PORT_START	/* steer */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(25) PORT_REVERSE PORT_PLAYER(1)

//  PORT_START  /* steer offset */

//  PORT_START  /* accel offset */

//  PORT_START  /* brake, 0-0x30: needs to start at 0xff; then 0xcf is max brake */
//  PORT_BIT( 0xff, 0xff, IPT_AD_STICK_X ) PORT_MINMAX(0xcf,0xff) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START	/* unknown */
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(2)
INPUT_PORTS_END

INPUT_PORTS_START( wgpjoy )
	PORT_START /* DSW A */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	TAITO_WGP_DSWA_2_4
	TAITO_COINAGE_JAPAN_NEW_8

	PORT_START /* DSW B */
	TAITO_DIFFICULTY_8
	PORT_DIPNAME( 0x04, 0x04, "Shift Pattern Select" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN1, is it read? */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* doesn't exist */

	PORT_START	/* doesn't exist */
INPUT_PORTS_END

/* Same as 'wgpj', but no "Motor Test" Dip Switch (DSWA 0) */
INPUT_PORTS_START( wgp2 )	/* Wgp2 has no "lumps" ? */
	PORT_START /* DSW A */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	TAITO_WGP_DSWA_2_4
	TAITO_COINAGE_JAPAN_NEW_8

	PORT_START /* DSW B */
	TAITO_DIFFICULTY_8
	PORT_DIPNAME( 0x04, 0x04, "Shift Pattern Select" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Slave / Master ???" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Communication" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	WGP_MACHINE_ID_8

	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_BUTTON7 ) PORT_PLAYER(1)	/* freeze */
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_BUTTON4 ) PORT_PLAYER(1)	/* shift up */
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(1)	/* shift down */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(1)	/* "start lump" (lamp?) */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(1)	/* "brake lump" (lamp?) */
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START      /* IN2 */
	TAITO_WGP_SYSTEM_INPUT
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START      /* fake inputs, allowing digital steer etc. */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_DIPNAME( 0x10, 0x10, "Steering type" )
	PORT_DIPSETTING(    0x10, "Digital" )
	PORT_DIPSETTING(    0x00, "Analogue" )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)	/* accel */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)	/* brake */

/* It's not clear for accel and brake which is the input and
   which the offset, but that doesn't matter. These continuous
   inputs are replaced by discrete values derived from the fake
   input port above, so keyboard control is feasible. */

//  PORT_START  /* accel, 0-255 */
//  PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(25) PORT_REVERSE PORT_PLAYER(1)

	PORT_START	/* steer */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(25) PORT_REVERSE PORT_PLAYER(1)

//  PORT_START  /* steer offset */

//  PORT_START  /* accel offset */

//  PORT_START  /* brake, 0-0x30: needs to start at 0xff; then 0xcf is max brake */
//  PORT_BIT( 0xff, 0xff, IPT_AD_STICK_X ) PORT_MINMAX(0xcf,0xff) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START	/* unknown */
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(2)
INPUT_PORTS_END


/***********************************************************
                        GFX DECODING
***********************************************************/

static const gfx_layout wgp_tilelayout =
{
	16,16,	/* 16*16 sprites */
	RGN_FRAC(1,1),
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 1*4, 0*4, 5*4, 4*4, 3*4, 2*4, 7*4, 6*4, 9*4, 8*4, 13*4, 12*4, 11*4, 10*4, 15*4, 14*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8	/* every sprite takes 128 consecutive bytes */
};

static const gfx_layout wgp_tile2layout =
{
	16,16,	/* 16*16 sprites */
	RGN_FRAC(1,1),
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 7*4, 6*4, 15*4, 14*4, 5*4, 4*4, 13*4, 12*4, 3*4, 2*4, 11*4, 10*4, 1*4, 0*4, 9*4, 8*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8	/* every sprite takes 128 consecutive bytes */
};

static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	RGN_FRAC(1,1),
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8	/* every sprite takes 32 consecutive bytes */
};

/* taitoic.c TC0100SCN routines expect scr stuff to be in second gfx
   slot */

static const gfx_decode wgp_gfxdecodeinfo[] =
{
	{ REGION_GFX3, 0x0, &wgp_tilelayout,  0, 256 },		/* sprites */
	{ REGION_GFX1, 0x0, &charlayout,  0, 256 },		/* sprites & playfield */
	{ REGION_GFX2, 0x0, &wgp_tile2layout,  0, 256 },	/* piv */
	{ -1 } /* end of array */
};


/**************************************************************
                           YM2610 (SOUND)
**************************************************************/

/* handler called by the YM2610 emulator when the internal timers cause an IRQ */
static void irqhandler(int irq)	// assumes Z80 sandwiched between 68Ks
{
	cpunum_set_input_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2610interface ym2610_interface =
{
	irqhandler,
	REGION_SOUND2,	/* Delta-T */
	REGION_SOUND1	/* ADPCM */
};


/***********************************************************
                      MACHINE DRIVERS

Wgp has high interleaving to prevent "common ram error".
However sync to vblank is lacking, which is causing the
graphics glitches.
***********************************************************/

static MACHINE_START( wgp )
{
	state_save_register_global(cpua_ctrl);
	state_save_register_func_postload(parse_control);

	state_save_register_global(banknum);
	state_save_register_func_postload(reset_sound_region);

	return 0;
}

static MACHINE_DRIVER_START( wgp )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 12000000)	/* 12 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(wgp_readmem,wgp_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD(Z80, 16000000/4)	/* 4 MHz ??? */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(z80_sound_readmem,z80_sound_writemem)

	MDRV_CPU_ADD(M68000, 12000000)	/* 12 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(wgp_cpub_readmem,wgp_cpub_writemem)
	MDRV_CPU_VBLANK_INT(wgp_cpub_interrupt,1)

	MDRV_MACHINE_START(wgp)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(250)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 2*8, 32*8-1)
	MDRV_GFXDECODE(wgp_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(wgp)
	MDRV_VIDEO_UPDATE(wgp)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2610, 16000000/2)
	MDRV_SOUND_CONFIG(ym2610_interface)
	MDRV_SOUND_ROUTE(0, "left",  0.25)
	MDRV_SOUND_ROUTE(0, "right", 0.25)
	MDRV_SOUND_ROUTE(1, "left",  1.0)
	MDRV_SOUND_ROUTE(2, "right", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( wgp2 )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 12000000)	/* 12 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(wgp_readmem,wgp_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD(Z80, 16000000/4)	/* 4 MHz ??? */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(z80_sound_readmem,z80_sound_writemem)

	MDRV_CPU_ADD(M68000, 12000000)	/* 12 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(wgp_cpub_readmem,wgp_cpub_writemem)
	MDRV_CPU_VBLANK_INT(wgp_cpub_interrupt,1)

	MDRV_MACHINE_START(wgp)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(200)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 2*8, 32*8-1)
	MDRV_GFXDECODE(wgp_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(wgp2)
	MDRV_VIDEO_UPDATE(wgp)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2610, 16000000/2)
	MDRV_SOUND_CONFIG(ym2610_interface)
	MDRV_SOUND_ROUTE(0, "left",  0.25)
	MDRV_SOUND_ROUTE(0, "right", 0.25)
	MDRV_SOUND_ROUTE(1, "left",  1.0)
	MDRV_SOUND_ROUTE(2, "right", 1.0)
MACHINE_DRIVER_END


/***************************************************************************
                                   DRIVERS
***************************************************************************/

ROM_START( wgp )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 256K for 68000 code (CPU A) */
	ROM_LOAD16_BYTE( "c32-25.12",      0x00000, 0x20000, CRC(0cc81e77) SHA1(435190bc24423e1e34134dff3cd4b79e120852d1) )
	ROM_LOAD16_BYTE( "c32-29.13",      0x00001, 0x20000, CRC(fab47cf0) SHA1(c0129c0290b48f24c25e4dd7c6c937675e31842a) )
	ROM_LOAD16_WORD_SWAP( "c32-10.9",  0x80000, 0x80000, CRC(a44c66e9) SHA1(b5fa978e43303003969033b8096fd68885cfc202) )	/* data rom */

	ROM_REGION( 0x40000, REGION_CPU3, 0 )	/* 256K for 68000 code (CPU B) */
	ROM_LOAD16_BYTE( "c32-28.64", 0x00000, 0x20000, CRC(38f3c7bf) SHA1(bfcaa036e5ff23f2bbf74d738498eda7d6ccd554) )
	ROM_LOAD16_BYTE( "c32-27.63", 0x00001, 0x20000, CRC(be2397fb) SHA1(605a02d56ae6007b36299a2eceb7ca180cbf6df9) )

	ROM_REGION( 0x1c000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "c32-24.34",   0x00000, 0x04000, CRC(e9adb447) SHA1(8b7044b6ea864e4cfd60b87abd28c38caecb147d) )
	ROM_CONTINUE(            0x10000, 0x0c000 )	/* banked stuff */

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "c32-09.16", 0x00000, 0x80000, CRC(96495f35) SHA1(ce99b4d8aeb98304e8ae3aa4966289c76ae4ff69) )	/* SCR */

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "c32-04.9",  0x000000, 0x80000, CRC(473a19c9) SHA1(4c632f4d5b725790a1be9d1143318d2f682fe9be) )	/* PIV */
	ROM_LOAD32_BYTE( "c32-03.10", 0x000001, 0x80000, CRC(9ec3e134) SHA1(e82a50927e10e551124a3b81399b052974cfba12) )
	ROM_LOAD32_BYTE( "c32-02.11", 0x000002, 0x80000, CRC(c5721f3a) SHA1(4a8e9412de23001b09eb3425ba6006b4c09a907b) )
	ROM_LOAD32_BYTE( "c32-01.12", 0x000003, 0x80000, CRC(d27d7d93) SHA1(82ae5856bbdb49cb8c2ca20eef86f6b617ea2c45) )

	ROM_REGION( 0x200000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "c32-05.71", 0x000000, 0x80000, CRC(3698d47a) SHA1(71978f9e1f58fa1259e67d8a7ea68e3ec1314c6b) )	/* OBJ */
	ROM_LOAD16_BYTE( "c32-06.70", 0x000001, 0x80000, CRC(f0267203) SHA1(7fd7b8d7a9efa405fc647c16fb99ffcb1fe985c5) )
	ROM_LOAD16_BYTE( "c32-07.69", 0x100000, 0x80000, CRC(743d46bd) SHA1(6b655b3fbfad8b52e38d7388aab564f5fa3e778c) )
	ROM_LOAD16_BYTE( "c32-08.68", 0x100001, 0x80000, CRC(faab63b0) SHA1(6e1aaf2642bee7d7bc9e21a7bf7f81d9ff766c50) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "c32-11.8",  0x00000, 0x80000, CRC(2b326ff0) SHA1(3c442e3c97234e4514a7bed31644212586869bd0) )

	ROM_REGION( 0x80000, REGION_SOUND2, 0 )	/* Delta-T samples */
	ROM_LOAD( "c32-12.7",  0x00000, 0x80000, CRC(df48a37b) SHA1(c0c191f4b8a5f55c0f1e52dac9cd3f7d15adace6) )

//  Pals (Guru dump)
//  ROM_LOAD( "c32-13.14", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-14.19", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-15.52", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-16.54", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-17.53", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-18.64", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-19.27", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-20.67", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-21.85", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-22.24", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-23.13", 0x00000, 0x00???, NO_DUMP )

//  Pals on lan interface board
//  ROM_LOAD( "c32-34", 0x00000, 0x00???, NO_DUMP )
//  ROM_LOAD( "c32-35", 0x00000, 0x00???, NO_DUMP )
ROM_END

ROM_START( wgpj )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 256K for 68000 code (CPU A) */
	ROM_LOAD16_BYTE( "c32-48.12",      0x00000, 0x20000, CRC(819cc134) SHA1(501bb1038979117586f6d8202ca6e1e44191f421) )
	ROM_LOAD16_BYTE( "c32-49.13",      0x00001, 0x20000, CRC(4a515f02) SHA1(d0be52bbb5cc8151b23363092ac04e27b2d20a50) )
	ROM_LOAD16_WORD_SWAP( "c32-10.9",  0x80000, 0x80000, CRC(a44c66e9) SHA1(b5fa978e43303003969033b8096fd68885cfc202) )	/* data rom */

	ROM_REGION( 0x40000, REGION_CPU3, 0 )	/* 256K for 68000 code (CPU B) */
	ROM_LOAD16_BYTE( "c32-28.64", 0x00000, 0x20000, CRC(38f3c7bf) SHA1(bfcaa036e5ff23f2bbf74d738498eda7d6ccd554) )
	ROM_LOAD16_BYTE( "c32-27.63", 0x00001, 0x20000, CRC(be2397fb) SHA1(605a02d56ae6007b36299a2eceb7ca180cbf6df9) )

	ROM_REGION( 0x1c000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "c32-24.34",   0x00000, 0x04000, CRC(e9adb447) SHA1(8b7044b6ea864e4cfd60b87abd28c38caecb147d) )
	ROM_CONTINUE(            0x10000, 0x0c000 )	/* banked stuff */

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "c32-09.16", 0x00000, 0x80000, CRC(96495f35) SHA1(ce99b4d8aeb98304e8ae3aa4966289c76ae4ff69) )	/* SCR */

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "c32-04.9",  0x000000, 0x80000, CRC(473a19c9) SHA1(4c632f4d5b725790a1be9d1143318d2f682fe9be) )	/* PIV */
	ROM_LOAD32_BYTE( "c32-03.10", 0x000001, 0x80000, CRC(9ec3e134) SHA1(e82a50927e10e551124a3b81399b052974cfba12) )
	ROM_LOAD32_BYTE( "c32-02.11", 0x000002, 0x80000, CRC(c5721f3a) SHA1(4a8e9412de23001b09eb3425ba6006b4c09a907b) )
	ROM_LOAD32_BYTE( "c32-01.12", 0x000003, 0x80000, CRC(d27d7d93) SHA1(82ae5856bbdb49cb8c2ca20eef86f6b617ea2c45) )

	ROM_REGION( 0x200000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "c32-05.71", 0x000000, 0x80000, CRC(3698d47a) SHA1(71978f9e1f58fa1259e67d8a7ea68e3ec1314c6b) )	/* OBJ */
	ROM_LOAD16_BYTE( "c32-06.70", 0x000001, 0x80000, CRC(f0267203) SHA1(7fd7b8d7a9efa405fc647c16fb99ffcb1fe985c5) )
	ROM_LOAD16_BYTE( "c32-07.69", 0x100000, 0x80000, CRC(743d46bd) SHA1(6b655b3fbfad8b52e38d7388aab564f5fa3e778c) )
	ROM_LOAD16_BYTE( "c32-08.68", 0x100001, 0x80000, CRC(faab63b0) SHA1(6e1aaf2642bee7d7bc9e21a7bf7f81d9ff766c50) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "c32-11.8", 0x00000, 0x80000, CRC(2b326ff0) SHA1(3c442e3c97234e4514a7bed31644212586869bd0) )

	ROM_REGION( 0x80000, REGION_SOUND2, 0 )	/* Delta-T samples */
	ROM_LOAD( "c32-12.7", 0x00000, 0x80000, CRC(df48a37b) SHA1(c0c191f4b8a5f55c0f1e52dac9cd3f7d15adace6) )
ROM_END

ROM_START( wgpjoy )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 256K for 68000 code (CPU A) */
	ROM_LOAD16_BYTE( "c32-57.12",      0x00000, 0x20000, CRC(13a78911) SHA1(d3ace25dddce56cc35e93992f4fae01e87693d36) )
	ROM_LOAD16_BYTE( "c32-58.13",      0x00001, 0x20000, CRC(326d367b) SHA1(cbfb15841f61fa856876d4321fbce190f89a5020) )
	ROM_LOAD16_WORD_SWAP( "c32-10.9",  0x80000, 0x80000, CRC(a44c66e9) SHA1(b5fa978e43303003969033b8096fd68885cfc202) )	/* data rom */

	ROM_REGION( 0x40000, REGION_CPU3, 0 )	/* 256K for 68000 code (CPU B) */
	ROM_LOAD16_BYTE( "c32-60.64", 0x00000, 0x20000, CRC(7a980312) SHA1(c85beff4c8201061b99d87f8db67e2b85dff00e3) )
	ROM_LOAD16_BYTE( "c32-59.63", 0x00001, 0x20000, CRC(ed75b333) SHA1(fa47ea38f7ba1cb3463065357db9a9b0f0eeab77) )

	ROM_REGION( 0x1c000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "c32-61.34",   0x00000, 0x04000, CRC(2fcad5a3) SHA1(f0f658490655b521af631af763c07e37834dc5a0) )
	ROM_CONTINUE(            0x10000, 0x0c000 )	/* banked stuff */

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "c32-09.16", 0x00000, 0x80000, CRC(96495f35) SHA1(ce99b4d8aeb98304e8ae3aa4966289c76ae4ff69) )	/* SCR */

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "c32-04.9",  0x000000, 0x80000, CRC(473a19c9) SHA1(4c632f4d5b725790a1be9d1143318d2f682fe9be) )	/* PIV */
	ROM_LOAD32_BYTE( "c32-03.10", 0x000001, 0x80000, CRC(9ec3e134) SHA1(e82a50927e10e551124a3b81399b052974cfba12) )
	ROM_LOAD32_BYTE( "c32-02.11", 0x000002, 0x80000, CRC(c5721f3a) SHA1(4a8e9412de23001b09eb3425ba6006b4c09a907b) )
	ROM_LOAD32_BYTE( "c32-01.12", 0x000003, 0x80000, CRC(d27d7d93) SHA1(82ae5856bbdb49cb8c2ca20eef86f6b617ea2c45) )

	ROM_REGION( 0x200000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "c32-05.71", 0x000000, 0x80000, CRC(3698d47a) SHA1(71978f9e1f58fa1259e67d8a7ea68e3ec1314c6b) )	/* OBJ */
	ROM_LOAD16_BYTE( "c32-06.70", 0x000001, 0x80000, CRC(f0267203) SHA1(7fd7b8d7a9efa405fc647c16fb99ffcb1fe985c5) )
	ROM_LOAD16_BYTE( "c32-07.69", 0x100000, 0x80000, CRC(743d46bd) SHA1(6b655b3fbfad8b52e38d7388aab564f5fa3e778c) )
	ROM_LOAD16_BYTE( "c32-08.68", 0x100001, 0x80000, CRC(faab63b0) SHA1(6e1aaf2642bee7d7bc9e21a7bf7f81d9ff766c50) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "c32-11.8", 0x00000, 0x80000, CRC(2b326ff0) SHA1(3c442e3c97234e4514a7bed31644212586869bd0) )

	ROM_REGION( 0x80000, REGION_SOUND2, 0 )	/* delta-t samples */
	ROM_LOAD( "c32-12.7", 0x00000, 0x80000, CRC(df48a37b) SHA1(c0c191f4b8a5f55c0f1e52dac9cd3f7d15adace6) )
ROM_END

ROM_START( wgpjoya )	/* Older joystick version ??? */
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 256K for 68000 code (CPU A) */
	ROM_LOAD16_BYTE( "c32-57.12",      0x00000, 0x20000, CRC(13a78911) SHA1(d3ace25dddce56cc35e93992f4fae01e87693d36) )
	ROM_LOAD16_BYTE( "c32-58.13",      0x00001, 0x20000, CRC(326d367b) SHA1(cbfb15841f61fa856876d4321fbce190f89a5020) )
	ROM_LOAD16_WORD_SWAP( "c32-10.9",  0x80000, 0x80000, CRC(a44c66e9) SHA1(b5fa978e43303003969033b8096fd68885cfc202) )	/* data rom */

	ROM_REGION( 0x40000, REGION_CPU3, 0 )	/* 256K for 68000 code (CPU B) */
	ROM_LOAD16_BYTE( "c32-46.64", 0x00000, 0x20000, CRC(64191891) SHA1(91d1d51478f1c2785470de0ac2a048e367f7ea48) )	// older rev?
	ROM_LOAD16_BYTE( "c32-45.63", 0x00001, 0x20000, CRC(759b39d5) SHA1(ed4ccd295c5595bdcac965b59293efb3c21ce48a) )	// older rev?

	ROM_REGION( 0x1c000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "c32-61.34",   0x00000, 0x04000, CRC(2fcad5a3) SHA1(f0f658490655b521af631af763c07e37834dc5a0) )
	ROM_CONTINUE(            0x10000, 0x0c000 )	/* banked stuff */

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "c32-09.16", 0x00000, 0x80000, CRC(96495f35) SHA1(ce99b4d8aeb98304e8ae3aa4966289c76ae4ff69) )	/* SCR */

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "c32-04.9",  0x000000, 0x80000, CRC(473a19c9) SHA1(4c632f4d5b725790a1be9d1143318d2f682fe9be) )	/* PIV */
	ROM_LOAD32_BYTE( "c32-03.10", 0x000001, 0x80000, CRC(9ec3e134) SHA1(e82a50927e10e551124a3b81399b052974cfba12) )
	ROM_LOAD32_BYTE( "c32-02.11", 0x000002, 0x80000, CRC(c5721f3a) SHA1(4a8e9412de23001b09eb3425ba6006b4c09a907b) )
	ROM_LOAD32_BYTE( "c32-01.12", 0x000003, 0x80000, CRC(d27d7d93) SHA1(82ae5856bbdb49cb8c2ca20eef86f6b617ea2c45) )

	ROM_REGION( 0x200000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "c32-05.71", 0x000000, 0x80000, CRC(3698d47a) SHA1(71978f9e1f58fa1259e67d8a7ea68e3ec1314c6b) )	/* OBJ */
	ROM_LOAD16_BYTE( "c32-06.70", 0x000001, 0x80000, CRC(f0267203) SHA1(7fd7b8d7a9efa405fc647c16fb99ffcb1fe985c5) )
	ROM_LOAD16_BYTE( "c32-07.69", 0x100000, 0x80000, CRC(743d46bd) SHA1(6b655b3fbfad8b52e38d7388aab564f5fa3e778c) )
	ROM_LOAD16_BYTE( "c32-08.68", 0x100001, 0x80000, CRC(faab63b0) SHA1(6e1aaf2642bee7d7bc9e21a7bf7f81d9ff766c50) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "c32-11.8", 0x00000, 0x80000, CRC(2b326ff0) SHA1(3c442e3c97234e4514a7bed31644212586869bd0) )

	ROM_REGION( 0x80000, REGION_SOUND2, 0 )	/* delta-t samples */
	ROM_LOAD( "c32-12.7", 0x00000, 0x80000, CRC(df48a37b) SHA1(c0c191f4b8a5f55c0f1e52dac9cd3f7d15adace6) )
ROM_END

ROM_START( wgp2 )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 256K for 68000 code (CPU A) */
	ROM_LOAD16_BYTE( "c73-01.12",      0x00000, 0x20000, CRC(c6434834) SHA1(75b2937a9bf18d268fa7bbfb3e822fba510ec2f1) )
	ROM_LOAD16_BYTE( "c73-02.13",      0x00001, 0x20000, CRC(c67f1ed1) SHA1(c30dc3fd46f103a75aa71f87c1fd6c0e7fed9214) )
	ROM_LOAD16_WORD_SWAP( "c32-10.9",  0x80000, 0x80000, CRC(a44c66e9) SHA1(b5fa978e43303003969033b8096fd68885cfc202) )	/* data rom */

	ROM_REGION( 0x40000, REGION_CPU3, 0 )	/* 256K for 68000 code (CPU B) */
	ROM_LOAD16_BYTE( "c73-04.64", 0x00000, 0x20000, CRC(383aa776) SHA1(bad18f0506e99a07d53e50abe7a548ff3d745e09) )
	ROM_LOAD16_BYTE( "c73-03.63", 0x00001, 0x20000, CRC(eb5067ef) SHA1(08d9d921c7a74877d7bb7641ae30c82d4d0653e3) )

	ROM_REGION( 0x1c000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "c73-05.34",   0x00000, 0x04000, CRC(7e00a299) SHA1(93696a229f17a15a92a8d9ef3b34d340de5dec44) )
	ROM_CONTINUE(            0x10000, 0x0c000 )	/* banked stuff */

	ROM_REGION( 0x80000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "c32-09.16", 0x00000, 0x80000, CRC(96495f35) SHA1(ce99b4d8aeb98304e8ae3aa4966289c76ae4ff69) )	/* SCR */

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "c32-04.9",  0x000000, 0x80000, CRC(473a19c9) SHA1(4c632f4d5b725790a1be9d1143318d2f682fe9be) )	/* PIV */
	ROM_LOAD32_BYTE( "c32-03.10", 0x000001, 0x80000, CRC(9ec3e134) SHA1(e82a50927e10e551124a3b81399b052974cfba12) )
	ROM_LOAD32_BYTE( "c32-02.11", 0x000002, 0x80000, CRC(c5721f3a) SHA1(4a8e9412de23001b09eb3425ba6006b4c09a907b) )
	ROM_LOAD32_BYTE( "c32-01.12", 0x000003, 0x80000, CRC(d27d7d93) SHA1(82ae5856bbdb49cb8c2ca20eef86f6b617ea2c45) )

	ROM_REGION( 0x200000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "c32-05.71", 0x000000, 0x80000, CRC(3698d47a) SHA1(71978f9e1f58fa1259e67d8a7ea68e3ec1314c6b) )	/* OBJ */
	ROM_LOAD16_BYTE( "c32-06.70", 0x000001, 0x80000, CRC(f0267203) SHA1(7fd7b8d7a9efa405fc647c16fb99ffcb1fe985c5) )
	ROM_LOAD16_BYTE( "c32-07.69", 0x100000, 0x80000, CRC(743d46bd) SHA1(6b655b3fbfad8b52e38d7388aab564f5fa3e778c) )
	ROM_LOAD16_BYTE( "c32-08.68", 0x100001, 0x80000, CRC(faab63b0) SHA1(6e1aaf2642bee7d7bc9e21a7bf7f81d9ff766c50) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "c32-11.8", 0x00000, 0x80000, CRC(2b326ff0) SHA1(3c442e3c97234e4514a7bed31644212586869bd0) )

	ROM_REGION( 0x80000, REGION_SOUND2, 0 )	/* delta-t samples */
	ROM_LOAD( "c32-12.7", 0x00000, 0x80000, CRC(df48a37b) SHA1(c0c191f4b8a5f55c0f1e52dac9cd3f7d15adace6) )

//  WGP2 security board (has TC0190FMC)
//  ROM_LOAD( "c73-06", 0x00000, 0x00???, NO_DUMP )
ROM_END


DRIVER_INIT( wgp )
{
#if 0
	/* Patch for coding error that causes corrupt data in
       sprite tilemapping area from $4083c0-847f */
	UINT16 *ROM = (UINT16 *)memory_region(REGION_CPU1);
	ROM[0x25dc / 2] = 0x0602;	// faulty value is 0x0206
#endif

//  taitosnd_setz80_soundcpu( 2 );
	cpua_ctrl = 0xff;
}

DRIVER_INIT( wgp2 )
{
	/* Code patches to prevent failure in memory checks */
	UINT16 *ROM = (UINT16 *)memory_region(REGION_CPU3);
	ROM[0x8008 / 2] = 0x0;
	ROM[0x8010 / 2] = 0x0;

	init_wgp();
}

/* Working Games with some graphics problems - e.g. missing rotation */

GAME( 1989, wgp,      0,      wgp,    wgp,    wgp,    ROT0, "Taito America Corporation", "World Grand Prix (US)", GAME_IMPERFECT_GRAPHICS ,0)
GAME( 1989, wgpj,     wgp,    wgp,    wgpj,   wgp,    ROT0, "Taito Corporation", "World Grand Prix (Japan)", GAME_IMPERFECT_GRAPHICS ,0)
GAME( 1989, wgpjoy,   wgp,    wgp,    wgpjoy, wgp,    ROT0, "Taito Corporation", "World Grand Prix (joystick version set 1) (Japan)", GAME_IMPERFECT_GRAPHICS ,0)
GAME( 1989, wgpjoya,  wgp,    wgp,    wgpjoy, wgp,    ROT0, "Taito Corporation", "World Grand Prix (joystick version set 2) (Japan)", GAME_IMPERFECT_GRAPHICS ,0)
GAME( 1990, wgp2,     wgp,    wgp2,   wgp2,   wgp2,   ROT0, "Taito Corporation", "World Grand Prix 2 (Japan)", GAME_IMPERFECT_GRAPHICS ,0)

