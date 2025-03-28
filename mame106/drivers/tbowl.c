/*** Tecmo Bowl (c)1987 Tecmo

driver by David Haywood
wip 20/01/2002

Tecmo Bowl was a popular 4 player American Football game with two screens and
attractive graphics

--- Current Issues

Might be some priority glitches

***/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/3812intf.h"
#include "sound/msm5205.h"

/* in vidhrdw/tbowl.c */
extern UINT8 *tbowl_txvideoram, *tbowl_bgvideoram, *tbowl_bg2videoram;
extern UINT8 *tbowl_spriteram;

WRITE8_HANDLER (tbowl_bg2videoram_w);
WRITE8_HANDLER (tbowl_bgvideoram_w);
WRITE8_HANDLER (tbowl_txvideoram_w);

WRITE8_HANDLER (tbowl_bg2xscroll_lo); WRITE8_HANDLER (tbowl_bg2xscroll_hi);
WRITE8_HANDLER (tbowl_bg2yscroll_lo); WRITE8_HANDLER (tbowl_bg2yscroll_hi);
WRITE8_HANDLER (tbowl_bgxscroll_lo);  WRITE8_HANDLER (tbowl_bgxscroll_hi);
WRITE8_HANDLER (tbowl_bgyscroll_lo);  WRITE8_HANDLER (tbowl_bgyscroll_hi);

VIDEO_START( tbowl );
VIDEO_UPDATE( tbowl );

static WRITE8_HANDLER( tbowl_coin_counter_w )
{
	coin_counter_w(0, data & 1);
}

/*** Banking

note: check this, its borrowed from tecmo.c / wc90.c at the moment and could well be wrong

***/

static WRITE8_HANDLER( tbowlb_bankswitch_w )
{
	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU1);


	bankaddress = 0x10000 + ((data & 0xf8) << 8);
	memory_set_bankptr(1,&RAM[bankaddress]);
}

static WRITE8_HANDLER( tbowlc_bankswitch_w )
{
	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU2);


	bankaddress = 0x10000 + ((data & 0xf8) << 8);


	memory_set_bankptr(2,&RAM[bankaddress]);
}

/*** Shared Ram Handlers

***/

static unsigned char *shared_ram;

static READ8_HANDLER( shared_r )
{
	return shared_ram[offset];
}

static WRITE8_HANDLER( shared_w )
{
	shared_ram[offset] = data;
}

static WRITE8_HANDLER( tbowl_sound_command_w )
{
	soundlatch_w(offset,data);
	cpunum_set_input_line(2,INPUT_LINE_NMI,PULSE_LINE);
}


/*** Memory Structures

    Board B is the main board, reading inputs, and in control of the 2 bg layers & text layer etc.
    Board C is the sub board, main job is the sprites
    Board A is for the sound

***/


/* Board B */

static ADDRESS_MAP_START( readmem_6206B, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x9fff) AM_READ(MRA8_RAM) /* RAM 1 */
	AM_RANGE(0xa000, 0xbfff) AM_READ(MRA8_RAM) /* RAM 1 */
	AM_RANGE(0xc000, 0xdfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xefff) AM_READ(MRA8_RAM)
	AM_RANGE(0xf000, 0xf7ff) AM_READ(MRA8_BANK1) /* Banked ROM */
	AM_RANGE(0xf800, 0xfbff) AM_READ(shared_r) /* RAM 2 */
	AM_RANGE(0xfc00, 0xfc00) AM_READ(input_port_0_r) // Player 1 inputs
	AM_RANGE(0xfc01, 0xfc01) AM_READ(input_port_1_r) // Player 2 inputs
	AM_RANGE(0xfc02, 0xfc02) AM_READ(input_port_2_r) // Player 3 inputs
	AM_RANGE(0xfc03, 0xfc03) AM_READ(input_port_3_r) // Player 4 inputs
//  AM_RANGE(0xfc06, 0xfc06) AM_READ(dummy_r) // Read During NMI
	AM_RANGE(0xfc07, 0xfc07) AM_READ(input_port_4_r) // System inputs
	AM_RANGE(0xfc08, 0xfc08) AM_READ(input_port_5_r) // DSW1
	AM_RANGE(0xfc09, 0xfc09) AM_READ(input_port_6_r) // DSW2
	AM_RANGE(0xfc0a, 0xfc0a) AM_READ(input_port_7_r) // DSW3
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_6206B, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x8000, 0x9fff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xa000, 0xbfff) AM_WRITE(tbowl_bg2videoram_w) AM_BASE(&tbowl_bg2videoram)
	AM_RANGE(0xc000, 0xdfff) AM_WRITE(tbowl_bgvideoram_w) AM_BASE(&tbowl_bgvideoram)
	AM_RANGE(0xe000, 0xefff) AM_WRITE(tbowl_txvideoram_w) AM_BASE(&tbowl_txvideoram)
//  AM_RANGE(0xf000, 0xf000) AM_WRITE(unknown_write)* written during start-up, not again */
	AM_RANGE(0xf000, 0xf7ff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(shared_w) AM_BASE(&shared_ram) /* check */
	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(tbowlb_bankswitch_w)
//  AM_RANGE(0xfc01, 0xfc01) AM_WRITE(unknown_write) /* written during start-up, not again */
//  AM_RANGE(0xfc02, 0xfc02) AM_WRITE(unknown_write) /* written during start-up, not again */
	AM_RANGE(0xfc03, 0xfc03) AM_WRITE(tbowl_coin_counter_w)
	AM_RANGE(0xfc0d, 0xfc0d) AM_WRITE(tbowl_sound_command_w) /* not sure, used quite a bit */
//  AM_RANGE(0xfc05, 0xfc05) AM_WRITE(unknown_write) /* no idea */
//  AM_RANGE(0xfc08, 0xfc08) AM_WRITE(unknown_write) /* hardly used .. */
//  AM_RANGE(0xfc0a, 0xfc0a) AM_WRITE(unknown_write) /* hardly used .. */
	AM_RANGE(0xfc10, 0xfc10) AM_WRITE(tbowl_bg2xscroll_lo)
	AM_RANGE(0xfc11, 0xfc11) AM_WRITE(tbowl_bg2xscroll_hi)
	AM_RANGE(0xfc12, 0xfc12) AM_WRITE(tbowl_bg2yscroll_lo)
	AM_RANGE(0xfc13, 0xfc13) AM_WRITE(tbowl_bg2yscroll_hi)
	AM_RANGE(0xfc14, 0xfc14) AM_WRITE(tbowl_bgxscroll_lo)
	AM_RANGE(0xfc15, 0xfc15) AM_WRITE(tbowl_bgxscroll_hi)
	AM_RANGE(0xfc16, 0xfc16) AM_WRITE(tbowl_bgyscroll_lo)
	AM_RANGE(0xfc17, 0xfc17) AM_WRITE(tbowl_bgyscroll_hi)
ADDRESS_MAP_END

/* Board C */
static WRITE8_HANDLER ( tbowl_trigger_nmi )
{
	/* trigger NMI on 6206B's Cpu? (guess but seems to work..) */
	cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
}

static ADDRESS_MAP_START( readmem_6206C, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xdfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xefff) AM_READ(MRA8_RAM) /* not read? */
	AM_RANGE(0xf000, 0xf7ff) AM_READ(MRA8_BANK2) /* Banked ROM */
	AM_RANGE(0xf800, 0xfbff) AM_READ(shared_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_6206C, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xd7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xd800, 0xdfff) AM_WRITE(MWA8_RAM) AM_BASE(&tbowl_spriteram)
	AM_RANGE(0xe000, 0xefff) AM_WRITE(paletteram_xxxxBBBBRRRRGGGG_be_w) AM_BASE(&paletteram) // 2x palettes, one for each monitor?
	AM_RANGE(0xf000, 0xf7ff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(shared_w)
	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(tbowlc_bankswitch_w)
	AM_RANGE(0xfc01, 0xfc01) AM_WRITE(MWA8_NOP) /* ? */
	AM_RANGE(0xfc02, 0xfc02) AM_WRITE(tbowl_trigger_nmi) /* ? */
	AM_RANGE(0xfc03, 0xfc03) AM_WRITE(MWA8_NOP) /* ? */
	AM_RANGE(0xfc06, 0xfc06) AM_WRITE(MWA8_NOP) /* ? */
ADDRESS_MAP_END

/* Board A */

static int adpcm_pos[2],adpcm_end[2];

static WRITE8_HANDLER( tbowl_adpcm_start_w )
{
	adpcm_pos[offset & 1] = data << 8;
	MSM5205_reset_w(offset & 1,0);
}

static WRITE8_HANDLER( tbowl_adpcm_end_w )
{
	adpcm_end[offset & 1] = (data + 1) << 8;
}

static WRITE8_HANDLER( tbowl_adpcm_vol_w )
{
	MSM5205_set_volume(offset & 1, (data & 0x7f) * 100 / 0x7f);
}

static void tbowl_adpcm_int(int num)
{
	static int adpcm_data[2] = { -1, -1 };

	if (adpcm_pos[num] >= adpcm_end[num] ||
				adpcm_pos[num] >= memory_region_length(REGION_SOUND1)/2)
		MSM5205_reset_w(num,1);
	else if (adpcm_data[num] != -1)
	{
		MSM5205_data_w(num,adpcm_data[num] & 0x0f);
		adpcm_data[num] = -1;
	}
	else
	{
		unsigned char *ROM = memory_region(REGION_SOUND1) + 0x10000 * num;

		adpcm_data[num] = ROM[adpcm_pos[num]++];
		MSM5205_data_w(num,adpcm_data[num] >> 4);
	}
}

static ADDRESS_MAP_START( readmem_6206A, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe010, 0xe010) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_6206A, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xd000, 0xd000) AM_WRITE(YM3812_control_port_0_w)
	AM_RANGE(0xd001, 0xd001) AM_WRITE(YM3812_write_port_0_w)
	AM_RANGE(0xd800, 0xd800) AM_WRITE(YM3812_control_port_1_w)
	AM_RANGE(0xd801, 0xd801) AM_WRITE(YM3812_write_port_1_w)
	AM_RANGE(0xe000, 0xe001) AM_WRITE(tbowl_adpcm_end_w)
	AM_RANGE(0xe002, 0xe003) AM_WRITE(tbowl_adpcm_start_w)
	AM_RANGE(0xe004, 0xe005) AM_WRITE(tbowl_adpcm_vol_w)
	AM_RANGE(0xe006, 0xe006) AM_WRITE(MWA8_NOP)
	AM_RANGE(0xe007, 0xe007) AM_WRITE(MWA8_NOP)	/* NMI acknowledge */
ADDRESS_MAP_END

/*** Input Ports

Haze's notes :

There are controls for 4 players, each player has 4 directions and 2 buttons as
well as coin and start.  The service switch acts as inserting a coin for all
4 players, the dipswitches are listed in the manual


Steph's notes (2002.02.12) :

I haven't found any manual, so my notes only rely on the Z80 code ...

  -- Inputs --

According to the Z80 code, here is the list of controls for each player :

  - NO START button (you need to press BUTTON1n to start a game for player n)
  - 4 or 8 directions (I can't tell for the moment, so I've chosen 8 directions)
  - 2 buttons (I can't tell for the moment what they do)

There are also 1 coin slot and a "Service" button for each player.

1 "credit" will mean <<time defined by the "Player Time" Dip Switch>>.

COINn adds one COIN for player n. When the number of coins fit the "Coinage"
Dip Switch, 1 "credit" will be added.

SERVICEn adds 1 "credit" for player n.

There is also a GENERAL "Service" switch that adds 1 "credit" for ALL players.
I've mapped it to the F1 key. If you have a better key and/or a better
description for it, feel free to change it.

  -- Dip Switches --

According to the Z80 code, what is called "Difficulty" Dip Switch (DSW 1
bits 0 and 1) doesn't seem to be tested. BTW, where did you find such info ?

I haven't been able to determine yet the effect(s) of DSW3 bits 2 and 3.
Could it be the "Difficulty" you mentioned that is HERE instead of DSW1
bits 0 and 1 ? I'll try to have another look when the sprites stuff is finished.

***/


#define TBOWL_PLAYER_INPUT(_n_) \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(_n_) PORT_8WAY \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(_n_) PORT_8WAY \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(_n_) PORT_8WAY \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(_n_) PORT_8WAY \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(_n_) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(_n_) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE##_n_ ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START##_n_ )


INPUT_PORTS_START( tbowl )
	PORT_START	/* player 1 inputs (0xfc00) */
	TBOWL_PLAYER_INPUT(1)

	PORT_START	/* player 2 inputs (0xfc01) */
	TBOWL_PLAYER_INPUT(2)

	PORT_START	/* player 3 inputs (0xfc02) */
	TBOWL_PLAYER_INPUT(3)

	PORT_START	/* player 4 inputs (0xfc03) */
	TBOWL_PLAYER_INPUT(4)

	PORT_START	/* system inputs (0xfc07 -> 0x80f9) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Service (General)") PORT_CODE(KEYCODE_F1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW1 (0xfc08 -> 0xffb4) */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING (   0x00, DEF_STR( 8C_1C ) )
	PORT_DIPSETTING (   0x01, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING (   0x02, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING (   0x03, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING (   0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING (   0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING (   0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING (   0x07, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xf8, 0xb8, "Time (Players)" )
	PORT_DIPSETTING (   0x00, "7:00" )
	PORT_DIPSETTING (   0x08, "6:00" )
	PORT_DIPSETTING (   0x10, "5:00" )
	PORT_DIPSETTING (   0x18, "4:30" )
	PORT_DIPSETTING (   0x20, "3:40" )
	PORT_DIPSETTING (   0x28, "3:20" )
	PORT_DIPSETTING (   0x30, "3:00" )
	PORT_DIPSETTING (   0x38, "2:50" )
	PORT_DIPSETTING (   0x40, "2:40" )
	PORT_DIPSETTING (   0x48, "2:30" )
	PORT_DIPSETTING (   0x50, "2:20" )
	PORT_DIPSETTING (   0x58, "2:10" )
	PORT_DIPSETTING (   0x60, "2:00" )
	PORT_DIPSETTING (   0x68, "1:55" )
	PORT_DIPSETTING (   0x70, "1:50" )
	PORT_DIPSETTING (   0x78, "1:45" )
	PORT_DIPSETTING (   0x80, "1:40" )
	PORT_DIPSETTING (   0x88, "1:35" )
	PORT_DIPSETTING (   0x90, "1:25" )
	PORT_DIPSETTING (   0x98, "1:20" )
	PORT_DIPSETTING (   0xa0, "1:15" )
	PORT_DIPSETTING (   0xa8, "1:10" )
	PORT_DIPSETTING (   0xb0, "1:05" )
	PORT_DIPSETTING (   0xb8, "1:00" )
	PORT_DIPSETTING (   0xc0, "0:55" )
	PORT_DIPSETTING (   0xc8, "0:50" )
	PORT_DIPSETTING (   0xd0, "0:45" )
	PORT_DIPSETTING (   0xd8, "0:40" )
	PORT_DIPSETTING (   0xe0, "0:35" )
	PORT_DIPSETTING (   0xe8, "0:30" )
	PORT_DIPSETTING (   0xf0, "0:25" )
//  PORT_DIPSETTING (   0xf8, "1:00" )

	PORT_START	/* DSW2 (0xfc09 -> 0xffb5) */
	PORT_DIPNAME( 0x03, 0x03, "Difficulty (unused ?)" )	// To be checked again
	PORT_DIPSETTING (   0x00, "0x00" )
	PORT_DIPSETTING (   0x01, "0x01" )
	PORT_DIPSETTING (   0x02, "0x02" )
	PORT_DIPSETTING (   0x03, "0x03" )
	PORT_DIPNAME( 0x0c, 0x0c, "Extra Time (Players)" )	// For multiple "credits"
	PORT_DIPSETTING (   0x00, "0:30" )
	PORT_DIPSETTING (   0x04, "0:20" )
	PORT_DIPSETTING (   0x08, "0:10" )
	PORT_DIPSETTING (   0x0c, DEF_STR( None ) )
	PORT_DIPNAME( 0x30, 0x30, "Timer Speed" )
	PORT_DIPSETTING (   0x00, "56/60" )
	PORT_DIPSETTING (   0x10, "51/60" )
	PORT_DIPSETTING (   0x30, "47/60" )
	PORT_DIPSETTING (   0x20, "42/60" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )	// Check code at 0x0393
	PORT_DIPSETTING (   0x00, DEF_STR( Off ) )
	PORT_DIPSETTING (   0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Hi-Score Reset" )		// Only if P1 buttons 1 and 2 are pressed during P.O.S.T. !
	PORT_DIPSETTING (   0x00, DEF_STR( Off ) )
	PORT_DIPSETTING (   0x80, DEF_STR( On ) )

	PORT_START	/* DSW3 (0xfc0a -> 0xffb6) */
	PORT_DIPNAME( 0x03, 0x03, "Time (Quarter)" )
	PORT_DIPSETTING (   0x00, "8:00" )
	PORT_DIPSETTING (   0x01, "5:00" )
	PORT_DIPSETTING (   0x03, "4:00" )
	PORT_DIPSETTING (   0x02, "3:00" )
	PORT_DIPNAME( 0x0c, 0x0c, "Unknown (in 0x8126.w)" )	// Check code at 0x6e16
	PORT_DIPSETTING (   0x00, "0x00 = 0x54f3" )
	PORT_DIPSETTING (   0x04, "0x04 = 0x54e1" )
	PORT_DIPSETTING (   0x08, "0x08 = 0x54cf" )
	PORT_DIPSETTING (   0x0c, "0x0c = 0x54bd" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

/* same as 'tbowl', but different "Quarter Time" Dip Switch
   ("3:00" and "4:00" are inverted) */
INPUT_PORTS_START( tbowlj )
	PORT_START	/* player 1 inputs (0xfc00) */
	TBOWL_PLAYER_INPUT(1)

	PORT_START	/* player 2 inputs (0xfc01) */
	TBOWL_PLAYER_INPUT(2)

	PORT_START	/* player 3 inputs (0xfc02) */
	TBOWL_PLAYER_INPUT(3)

	PORT_START	/* player 4 inputs (0xfc03) */
	TBOWL_PLAYER_INPUT(4)

	PORT_START	/* system inputs (0xfc07 -> 0x80f9) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Service (General)") PORT_CODE(KEYCODE_F1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW1 (0xfc08 -> 0xffb4) */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING (   0x00, DEF_STR( 8C_1C ) )
	PORT_DIPSETTING (   0x01, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING (   0x02, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING (   0x03, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING (   0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING (   0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING (   0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING (   0x07, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xf8, 0xb8, "Time (Players)" )
	PORT_DIPSETTING (   0x00, "7:00" )
	PORT_DIPSETTING (   0x08, "6:00" )
	PORT_DIPSETTING (   0x10, "5:00" )
	PORT_DIPSETTING (   0x18, "4:30" )
	PORT_DIPSETTING (   0x20, "3:40" )
	PORT_DIPSETTING (   0x28, "3:20" )
	PORT_DIPSETTING (   0x30, "3:00" )
	PORT_DIPSETTING (   0x38, "2:50" )
	PORT_DIPSETTING (   0x40, "2:40" )
	PORT_DIPSETTING (   0x48, "2:30" )
	PORT_DIPSETTING (   0x50, "2:20" )
	PORT_DIPSETTING (   0x58, "2:10" )
	PORT_DIPSETTING (   0x60, "2:00" )
	PORT_DIPSETTING (   0x68, "1:55" )
	PORT_DIPSETTING (   0x70, "1:50" )
	PORT_DIPSETTING (   0x78, "1:45" )
	PORT_DIPSETTING (   0x80, "1:40" )
	PORT_DIPSETTING (   0x88, "1:35" )
	PORT_DIPSETTING (   0x90, "1:25" )
	PORT_DIPSETTING (   0x98, "1:20" )
	PORT_DIPSETTING (   0xa0, "1:15" )
	PORT_DIPSETTING (   0xa8, "1:10" )
	PORT_DIPSETTING (   0xb0, "1:05" )
	PORT_DIPSETTING (   0xb8, "1:00" )
	PORT_DIPSETTING (   0xc0, "0:55" )
	PORT_DIPSETTING (   0xc8, "0:50" )
	PORT_DIPSETTING (   0xd0, "0:45" )
	PORT_DIPSETTING (   0xd8, "0:40" )
	PORT_DIPSETTING (   0xe0, "0:35" )
	PORT_DIPSETTING (   0xe8, "0:30" )
	PORT_DIPSETTING (   0xf0, "0:25" )
//  PORT_DIPSETTING (   0xf8, "1:00" )

	PORT_START	/* DSW2 (0xfc09 -> 0xffb5) */
	PORT_DIPNAME( 0x03, 0x03, "Difficulty (unused ?)" )	// To be checked again
	PORT_DIPSETTING (   0x00, "0x00" )
	PORT_DIPSETTING (   0x01, "0x01" )
	PORT_DIPSETTING (   0x02, "0x02" )
	PORT_DIPSETTING (   0x03, "0x03" )
	PORT_DIPNAME( 0x0c, 0x0c, "Extra Time (Players)" )	// For multiple "credits"
	PORT_DIPSETTING (   0x00, "0:30" )
	PORT_DIPSETTING (   0x04, "0:20" )
	PORT_DIPSETTING (   0x08, "0:10" )
	PORT_DIPSETTING (   0x0c, DEF_STR( None ) )
	PORT_DIPNAME( 0x30, 0x30, "Timer Speed" )
	PORT_DIPSETTING (   0x00, "56/60" )
	PORT_DIPSETTING (   0x10, "51/60" )
	PORT_DIPSETTING (   0x30, "47/60" )
	PORT_DIPSETTING (   0x20, "42/60" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )	// Check code at 0x0393
	PORT_DIPSETTING (   0x00, DEF_STR( Off ) )
	PORT_DIPSETTING (   0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Hi-Score Reset" )		// Only if P1 buttons 1 and 2 are pressed during P.O.S.T. !
	PORT_DIPSETTING (   0x00, DEF_STR( Off ) )
	PORT_DIPSETTING (   0x80, DEF_STR( On ) )

	PORT_START	/* DSW3 (0xfc0a -> 0xffb6) */
	PORT_DIPNAME( 0x03, 0x03, "Time (Quarter)" )
	PORT_DIPSETTING (   0x00, "8:00" )
	PORT_DIPSETTING (   0x01, "5:00" )
	PORT_DIPSETTING (   0x02, "4:00" )
	PORT_DIPSETTING (   0x03, "3:00" )
	PORT_DIPNAME( 0x0c, 0x0c, "Unknown (in 0x8126.w)" )	// Check code at 0x6e37
	PORT_DIPSETTING (   0x00, "0x00 = 0x5414" )
	PORT_DIPSETTING (   0x04, "0x04 = 0x5402" )
	PORT_DIPSETTING (   0x08, "0x08 = 0x54f0" )
	PORT_DIPSETTING (   0x0c, "0x0c = 0x54de" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


/*** Graphic Decodes

***/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout bgtilelayout =
{
	16,16,	/* tile size */
	RGN_FRAC(1,1),	/* number of tiles */
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },	/* the bitplanes are packed in one nibble */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4, 32*8+0*4, 32*8+1*4, 32*8+2*4, 32*8+3*4, 32*8+4*4, 32*8+5*4, 32*8+6*4, 32*8+7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
	  16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32},
	128*8	/* offset to next tile */
};

static const gfx_layout sprite8layout =
{
	8,8,	/* tile size */
	RGN_FRAC(1,1),	/* number of tiles */
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },	/* the bitplanes are packed in one nibble */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	8*32	/* offset to next tile */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   256, 16 },
	{ REGION_GFX2, 0, &bgtilelayout, 768, 16 },
	{ REGION_GFX2, 0, &bgtilelayout, 512, 16 },
	{ REGION_GFX3, 0, &sprite8layout, 0,   16 },

	{ -1 } /* end of array */
};

/*** Sound Bits

*/

static void irqhandler(int linestate)
{
	cpunum_set_input_line(2,0,linestate);
}

static struct YM3526interface ym3812_interface =
{
	irqhandler
};

static struct MSM5205interface msm5205_interface =
{
	tbowl_adpcm_int,	/* interrupt function */
	MSM5205_S48_4B		/* 8KHz               */
};

/*** Machine Driver

there are 3 boards, each with a cpu, boards b and c contain
NEC D70008AC-8's which is just a Z80, board a (the sound board)
has an actual Z80 chip

Sound Hardware should be 2 YM3812's + 2 MSM5205's

The game is displayed on 2 monitors

***/

static MACHINE_DRIVER_START( tbowl )

	/* CPU on Board '6206B' */
	MDRV_CPU_ADD(Z80, 8000000) /* NEC D70008AC-8 (Z80 Clone) */
	MDRV_CPU_PROGRAM_MAP(readmem_6206B,writemem_6206B)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	/* CPU on Board '6206C' */
	MDRV_CPU_ADD(Z80, 8000000) /* NEC D70008AC-8 (Z80 Clone) */
	MDRV_CPU_PROGRAM_MAP(readmem_6206C,writemem_6206C)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	/* CPU on Board '6206A' */
	MDRV_CPU_ADD(Z80, 4000000) /* Actual Z80 */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_6206A,writemem_6206A)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(100)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_ASPECT_RATIO(8,3)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 64*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024*2)

	MDRV_VIDEO_START(tbowl)
	MDRV_VIDEO_UPDATE(tbowl)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_CONFIG(ym3812_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	/* something for the samples? */
	MDRV_SOUND_ADD(MSM5205, 384000)
	MDRV_SOUND_CONFIG(msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD(MSM5205, 384000)
	MDRV_SOUND_CONFIG(msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


/* Board Layout from readme.txt

6206A
+-----------------------------------+
|                                   |
|                                   |
|                                   |
|       Z80                         |
|                                   |
|       1                           |
|                                   |
| 3                                 |
|                                   |
| 2                                 |
+-----------------------------------+

6206B
+-----------------------------------+
|                                   |
|                                   |
|                                   |
|   10          6                   |
|   11          7                   |
|   12          8                   |
|   13          9                   |
|                                   |
|                                   |
|                   NEC D70008AC-8  |
|                       4           |
|                       5           |
| 14                                |
| 15                                |
|                                   |
+-----------------------------------+

6206C
+-----------------------------------+
|                                   |
|                                   |
|                                   |
|   D70008AC-8                      |
|   24                              |
|   25                              |
|                                   |
|                   20  16          |
|                   21  17          |
|                   22  18          |
|                   23  19          |
+-----------------------------------+

*/

/*** Rom Loading ***

we currently have two dumps, one appears to be a world/us version, the
other is clearly a Japan version as it displays a regional warning

there is also a bad dump which for reference has the following roms
different to the world dump

    "24.rom" 0x10000 0x39a2d923 (code)
    "25.rom" 0x10000 0x9a0a9cd6 (code / data)

    "21.rom" 0x10000 0x93651858 (gfx)
    "22.rom" 0x10000 0xee7561d9 (gfx)
    "23.rom" 0x10000 0x46b3c186 (gfx)

this fails its rom check so I assume its corrupt

***/


ROM_START( tbowl )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* NEC D70008AC-8 (Z80 Clone) */
	ROM_LOAD( "4.11b",	    0x00000, 0x08000, CRC(db8a4f5d) SHA1(730dee040c18ed8736c07a7de0b986f667b0f2f5) )
	ROM_LOAD( "6206b.5",	0x10000, 0x10000, CRC(133c5c11) SHA1(7d4e76db3505ccf033d0d9b8d21feaf09b76dcc4) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 ) /* NEC D70008AC-8 (Z80 Clone) */
	ROM_LOAD( "6206c.24",	0x00000, 0x10000, CRC(040c8138) SHA1(f6fea192bf2ef0a3f0876133c761488184f54f50) )
	ROM_LOAD( "6206c.25",	0x10000, 0x10000, CRC(92c3cef5) SHA1(75883663b309bf46be544114c6e9086ab222300d) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 */
	ROM_LOAD( "6206a.1",	0x00000, 0x08000, CRC(4370207b) SHA1(2c929b571c86d35e646870644751e86bd16b5e22) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE ) /* 8x8 Characters inc. Alphanumerics */
	ROM_LOAD16_BYTE( "14.13l",	    0x00000, 0x08000, CRC(f9cf60b9) SHA1(0a79ed29f82ac7bd08062f922f79e439c194f30a) )
	ROM_LOAD16_BYTE( "15.15l",	    0x00001, 0x08000, CRC(a23f6c53) SHA1(0bb64894a27f41d74117ec492aafd52bc5b16ca4) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE ) /* BG GFX */
	ROM_LOAD16_BYTE( "6206b.6",	    0x40001, 0x10000, CRC(b9615ffa) SHA1(813896387291f5325ed7e4058347fe35c0d7b839) )
	ROM_LOAD16_BYTE( "6206b.8",	    0x40000, 0x10000, CRC(6389c719) SHA1(8043907d6f5b37228c09f05bbf12b4b9bb9bc130) )
	ROM_LOAD16_BYTE( "6206b.7",	    0x00001, 0x10000, CRC(d139c397) SHA1(4093220e6bddb95d0af445944bead7a064b64c39) )
	ROM_LOAD16_BYTE( "6206b.9",	    0x00000, 0x10000, CRC(975ded4c) SHA1(4045ee12f43dd23dadf6f9d0f7b25d04f9fda3d8) )
	ROM_LOAD16_BYTE( "6206b.10",    0x60001, 0x10000, CRC(9b4fa82e) SHA1(88df18985a04c6653a71db07fbbe0ce0670fe540) )
	ROM_LOAD16_BYTE( "6206b.12",    0x60000, 0x10000, CRC(7d0030f6) SHA1(24f0eca87ce38b974b9f359dd5f12f3be1ae7ff1) )
	ROM_LOAD16_BYTE( "6206b.11",    0x20001, 0x10000, CRC(06bf07bb) SHA1(9f12a39b8832bff2ffd84b7e6c1ddb2855ff924b) )
	ROM_LOAD16_BYTE( "6206b.13",    0x20000, 0x10000, CRC(4ad72c16) SHA1(554474987349b5b11e181ee8a2d1308777b030c1) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE ) /* SPR GFX */
	ROM_LOAD16_BYTE( "6206c.16",	0x60001, 0x10000, CRC(1a2fb925) SHA1(bc96ee87372826d5bee2b4d2aefde4c47b9ee80a) )
	ROM_LOAD16_BYTE( "6206c.20",	0x60000, 0x10000, CRC(70bb38a3) SHA1(5145b246f7720dd0359b97be35aa027af07cb6da) )
	ROM_LOAD16_BYTE( "6206c.17",	0x40001, 0x10000, CRC(de16bc10) SHA1(88e2452c7caf44cd541c27fc56c99703f3330bd7) )
	ROM_LOAD16_BYTE( "6206c.21",	0x40000, 0x10000, CRC(41b2a910) SHA1(98bf0fc9728240f35385ab0370bb47108f2d2bc2) )
	ROM_LOAD16_BYTE( "6206c.18",	0x20001, 0x10000, CRC(0684e188) SHA1(3d3c71c915cff62021baa17df37d0a68847d57cf) )
	ROM_LOAD16_BYTE( "6206c.22",	0x20000, 0x10000, CRC(cf660ebc) SHA1(3ca9577a36708c44a1bc9238faf14dbab1a0c3ca) )
	ROM_LOAD16_BYTE( "6206c.19",	0x00001, 0x10000, CRC(71795604) SHA1(57ef4f14dfe1829d5dddeba81bf2f7354d971d27) )
	ROM_LOAD16_BYTE( "6206c.23",	0x00000, 0x10000, CRC(97fba168) SHA1(107de19614d57453a37462e1a4d499d14633d50b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )
	ROM_LOAD( "6206a.3",	0x00000, 0x10000, CRC(3aa24744) SHA1(06de3f9a2431777218cc67f59230fddbfa01cf2d) )
	ROM_LOAD( "6206a.2",	0x10000, 0x10000, CRC(1e9e5936) SHA1(60370d1de28b1c5ffeff7843702aaddb19ff1f58) )
ROM_END

ROM_START( tbowlj )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* NEC D70008AC-8 (Z80 Clone) */
	ROM_LOAD( "6206b.4",	0x00000, 0x08000, CRC(7ed3eff7) SHA1(4a17f2838e9bbed8b1638783c62d07d1074e2b35) )
	ROM_LOAD( "6206b.5",	0x10000, 0x10000, CRC(133c5c11) SHA1(7d4e76db3505ccf033d0d9b8d21feaf09b76dcc4) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 ) /* NEC D70008AC-8 (Z80 Clone) */
	ROM_LOAD( "6206c.24",	0x00000, 0x10000, CRC(040c8138) SHA1(f6fea192bf2ef0a3f0876133c761488184f54f50) )
	ROM_LOAD( "6206c.25",	0x10000, 0x10000, CRC(92c3cef5) SHA1(75883663b309bf46be544114c6e9086ab222300d) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 */
	ROM_LOAD( "6206a.1",	0x00000, 0x08000, CRC(4370207b) SHA1(2c929b571c86d35e646870644751e86bd16b5e22) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE ) /* 8x8 Characters inc. Alphanumerics */
	ROM_LOAD16_BYTE( "6206b.14",	0x00000, 0x08000, CRC(cf99d0bf) SHA1(d1f23e23c2ebd26e2ffe8b23a02d86e4d32c6f11) )
	ROM_LOAD16_BYTE( "6206b.15",	0x00001, 0x08000, CRC(d69248cf) SHA1(4dad6a3fdc36b2fe625df0a43fd9e82d1dfd2af6) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE ) /* BG GFX */
	ROM_LOAD16_BYTE( "6206b.6",	    0x40001, 0x10000, CRC(b9615ffa) SHA1(813896387291f5325ed7e4058347fe35c0d7b839) )
	ROM_LOAD16_BYTE( "6206b.8",	    0x40000, 0x10000, CRC(6389c719) SHA1(8043907d6f5b37228c09f05bbf12b4b9bb9bc130) )
	ROM_LOAD16_BYTE( "6206b.7",	    0x00001, 0x10000, CRC(d139c397) SHA1(4093220e6bddb95d0af445944bead7a064b64c39) )
	ROM_LOAD16_BYTE( "6206b.9",	    0x00000, 0x10000, CRC(975ded4c) SHA1(4045ee12f43dd23dadf6f9d0f7b25d04f9fda3d8) )
	ROM_LOAD16_BYTE( "6206b.10",    0x60001, 0x10000, CRC(9b4fa82e) SHA1(88df18985a04c6653a71db07fbbe0ce0670fe540) )
	ROM_LOAD16_BYTE( "6206b.12",    0x60000, 0x10000, CRC(7d0030f6) SHA1(24f0eca87ce38b974b9f359dd5f12f3be1ae7ff1) )
	ROM_LOAD16_BYTE( "6206b.11",    0x20001, 0x10000, CRC(06bf07bb) SHA1(9f12a39b8832bff2ffd84b7e6c1ddb2855ff924b) )
	ROM_LOAD16_BYTE( "6206b.13",    0x20000, 0x10000, CRC(4ad72c16) SHA1(554474987349b5b11e181ee8a2d1308777b030c1) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE ) /* SPR GFX */
	ROM_LOAD16_BYTE( "6206c.16",	0x60001, 0x10000, CRC(1a2fb925) SHA1(bc96ee87372826d5bee2b4d2aefde4c47b9ee80a) )
	ROM_LOAD16_BYTE( "6206c.20",	0x60000, 0x10000, CRC(70bb38a3) SHA1(5145b246f7720dd0359b97be35aa027af07cb6da) )
	ROM_LOAD16_BYTE( "6206c.17",	0x40001, 0x10000, CRC(de16bc10) SHA1(88e2452c7caf44cd541c27fc56c99703f3330bd7) )
	ROM_LOAD16_BYTE( "6206c.21",	0x40000, 0x10000, CRC(41b2a910) SHA1(98bf0fc9728240f35385ab0370bb47108f2d2bc2) )
	ROM_LOAD16_BYTE( "6206c.18",	0x20001, 0x10000, CRC(0684e188) SHA1(3d3c71c915cff62021baa17df37d0a68847d57cf) )
	ROM_LOAD16_BYTE( "6206c.22",	0x20000, 0x10000, CRC(cf660ebc) SHA1(3ca9577a36708c44a1bc9238faf14dbab1a0c3ca) )
	ROM_LOAD16_BYTE( "6206c.19",	0x00001, 0x10000, CRC(71795604) SHA1(57ef4f14dfe1829d5dddeba81bf2f7354d971d27) )
	ROM_LOAD16_BYTE( "6206c.23",	0x00000, 0x10000, CRC(97fba168) SHA1(107de19614d57453a37462e1a4d499d14633d50b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )
	ROM_LOAD( "6206a.3",	0x00000, 0x10000, CRC(3aa24744) SHA1(06de3f9a2431777218cc67f59230fddbfa01cf2d) )
	ROM_LOAD( "6206a.2",	0x10000, 0x10000, CRC(1e9e5936) SHA1(60370d1de28b1c5ffeff7843702aaddb19ff1f58) )
ROM_END

GAME( 1987, tbowl,    0,        tbowl,    tbowl,    0, ROT0,  "Tecmo", "Tecmo Bowl (World?)", 0 ,4)
GAME( 1987, tbowlj,   tbowl,    tbowl,    tbowlj,   0, ROT0,  "Tecmo", "Tecmo Bowl (Japan)", 0 ,4)
