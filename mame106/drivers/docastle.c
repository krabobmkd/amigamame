/***************************************************************************

Mr. Do's Castle memory map (preliminary)

FIRST CPU:
0000-7fff ROM
8000-97ff RAM
9800-99ff Sprites
b000-b3ff Video RAM
b400-b7ff Color RAM

read:
a000-a008 data from second CPU

write:
a000-a008 data for second CPU
a800      Watchdog reset?
e000      Trigger NMI on second CPU

SECOND CPU:
0000-3fff ROM
8000-87ff RAM

read:
a000-a008 data from first CPU
all the following ports can be read both from c00x and from c08x. I don't know
what's the difference.
c001      DSWB
c081      coins per play
c002      DSWA
          bit 6-7 = lives
          bit 5 = upright/cocktail (0 = upright)
          bit 4 = difficulty of EXTRA (1 = easy)
          bit 3 = unused?
          bit 2 = RACK TEST
          bit 0-1 = difficulty
c003      IN0
          bit 4-7 = joystick player 2
          bit 0-3 = joystick player 1
c004      flipscreen (proper cocktail mode implemented by Chad Hendrickson Aug 1, 1999)
c005      IN1
          bit 7 = START 2
          bit 6 = unused
          bit 5 = jump player 2
          bit 4 = fire player 2
          bit 3 = START 1
          bit 2 = unused
          bit 1 = jump player 1(same effect as fire)
          bit 0 = fire player 1
c085      during the boot sequence, clearing any of bits 0, 1, 3, 4, 5, 7 enters the
          test mode, while clearing bit 2 or 6 seems to lock the machine.
c007      IN2
          bit 7 = unused
          bit 6 = unused
          bit 5 = COIN 2
          bit 4 = COIN 1
          bit 3 = PAUSE
          bit 2 = SERVICE (keep pressed)
          bit 1 = TEST (doesn't work?)
          bit 0 = TILT

write:
a000-a008 data for first CPU
e000      sound port 1
e400      sound port 2
e800      sound port 3
ec00      sound port 4


Mr. Do Wild Ride / Mr. Do Run Run memory map (preliminary)

0000-1fff ROM
2000-37ff RAM
3800-39ff Sprites
4000-9fff ROM
a000-a008 Shared with second CPU
b000-b3ff Video RAM
b400-b7ff Color RAM

write:
a800      Watchdog reset?
b800      Trigger NMI on second CPU (?)

SECOND CPU:
0000-3fff ROM
8000-87ff RAM

read:
e000-e008 data from first CPU
c003      bit 0-3 = joystick
          bit 4-7 = ?
c004      flipscreen (proper cocktail mode implemented by Chad Hendrickson Aug 1, 1999)
c005      bit 0 = fire
          bit 1 = fire (again?!)
          bit 2 = ?
          bit 3 = START 1
          bit 4-6 = ?
          bit 4 = START 2
c081      coins per play

write:
e000-e008 data for first CPU
a000      sound port 1
a400      sound port 2
a800      sound port 3
ac00      sound port 4

Note:
idsoccer seems to run on a modified version of this board which allows for
more sprite tiles, it also has a MSM5205 chip for sample playback.

***************************************************************************/

/*

    TODO:

    - third CPU
    - dip switch reading bug
    - unknown ports 0 and 2
    - bad communication in idsoccer
    - adpcm status in idsoccer
    - real values for the adpcm interface in idsoccer
    - handle flipscreen on/off based on address line A7 (cX0X/cX8X)

*/

#include "driver.h"
#include "sound/msm5205.h"
#include "sound/sn76496.h"


extern READ8_HANDLER( docastle_shared0_r );
extern READ8_HANDLER( docastle_shared1_r );
extern WRITE8_HANDLER( docastle_shared0_w );
extern WRITE8_HANDLER( docastle_shared1_w );
extern WRITE8_HANDLER( docastle_nmitrigger_w );

extern WRITE8_HANDLER( docastle_videoram_w );
extern WRITE8_HANDLER( docastle_colorram_w );
extern READ8_HANDLER( docastle_flipscreen_off_r );
extern READ8_HANDLER( docastle_flipscreen_on_r );
extern WRITE8_HANDLER( docastle_flipscreen_off_w );
extern WRITE8_HANDLER( docastle_flipscreen_on_w );

extern PALETTE_INIT( docastle );
extern PALETTE_INIT( dorunrun );
extern VIDEO_START( docastle );
extern VIDEO_UPDATE( docastle );


/* Read/Write Handlers */

static int adpcm_pos, adpcm_idle;

static void idsoccer_adpcm_int(int chip)
{
	static int adpcm_data = -1;

	if (adpcm_pos >= memory_region_length(REGION_SOUND1))
	{
		adpcm_idle = 1;
		MSM5205_reset_w(0, 1);
	}
	else if (adpcm_data != -1)
	{
		MSM5205_data_w(0, adpcm_data & 0x0f);
		adpcm_data = -1;
	}
	else
	{
		adpcm_data = memory_region(REGION_SOUND1)[adpcm_pos++];
		MSM5205_data_w(0, adpcm_data >> 4);
	}
}

static READ8_HANDLER( idsoccer_adpcm_status_r )
{
	// this is wrong, but the samples work anyway!!
	static int i;
	i ^= 0x80;
	return i;
}

static WRITE8_HANDLER( idsoccer_adpcm_w )
{
	if (data & 0x80)
	{
		adpcm_idle = 1;
		MSM5205_reset_w(0, 1);
	}
	else
	{
		adpcm_pos = (data & 0x7f) * 0x200;
		adpcm_idle = 0;
		MSM5205_reset_w(0, 0);
	}
}

/* Memory Maps */

static ADDRESS_MAP_START( docastle_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x97ff) AM_RAM
	AM_RANGE(0x9800, 0x99ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xa000, 0xa008) AM_READWRITE(docastle_shared0_r, docastle_shared1_w)
	AM_RANGE(0xa800, 0xa800) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0xb000, 0xb3ff) AM_MIRROR(0x0800) AM_READWRITE(MRA8_RAM, docastle_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xb400, 0xb7ff) AM_MIRROR(0x0800) AM_READWRITE(MRA8_RAM, docastle_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(docastle_nmitrigger_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( docastle_map2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0xa000, 0xa008) AM_READWRITE(docastle_shared1_r, docastle_shared0_w)
	AM_RANGE(0xc001, 0xc001) AM_MIRROR(0x0080) AM_READ(input_port_4_r)
	AM_RANGE(0xc002, 0xc002) AM_MIRROR(0x0080) AM_READ(input_port_3_r)
	AM_RANGE(0xc003, 0xc003) AM_MIRROR(0x0080) AM_READ(input_port_0_r)
	AM_RANGE(0xc004, 0xc004) AM_READWRITE(docastle_flipscreen_off_r, docastle_flipscreen_off_w)
	AM_RANGE(0xc005, 0xc005) AM_MIRROR(0x0080) AM_READ(input_port_1_r)
	AM_RANGE(0xc007, 0xc007) AM_MIRROR(0x0080) AM_READ(input_port_2_r)
	AM_RANGE(0xc084, 0xc084) AM_READWRITE(docastle_flipscreen_on_r, docastle_flipscreen_on_w)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(SN76496_0_w)
	AM_RANGE(0xe400, 0xe400) AM_WRITE(SN76496_1_w)
	AM_RANGE(0xe800, 0xe800) AM_WRITE(SN76496_2_w)
	AM_RANGE(0xec00, 0xec00) AM_WRITE(SN76496_3_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( docastle_map3, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x00ff) AM_ROM
	AM_RANGE(0x4000, 0x47ff) AM_RAM
	AM_RANGE(0x8000, 0x8008) AM_READ(docastle_shared1_r)	// ???
	AM_RANGE(0xc003, 0xc003) AM_NOP // EP according to schematics
	AM_RANGE(0xc432, 0xc435) AM_NOP	// ???
ADDRESS_MAP_END

static ADDRESS_MAP_START( docastle_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_NOP // goes to CRT 46505S
	AM_RANGE(0x02, 0x02) AM_NOP // goes to CRT 46505S
ADDRESS_MAP_END


static ADDRESS_MAP_START( dorunrun_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x37ff) AM_RAM
	AM_RANGE(0x3800, 0x39ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x4000, 0x9fff) AM_ROM
	AM_RANGE(0xa000, 0xa008) AM_READWRITE(docastle_shared0_r, docastle_shared1_w)
	AM_RANGE(0xa800, 0xa800) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0xb000, 0xb3ff) AM_READWRITE(MRA8_RAM, docastle_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xb400, 0xb7ff) AM_READWRITE(MRA8_RAM, docastle_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xb800, 0xb800) AM_WRITE(docastle_nmitrigger_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( dorunrun_map2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0xa000, 0xa000) AM_WRITE(SN76496_0_w)
	AM_RANGE(0xa400, 0xa400) AM_WRITE(SN76496_1_w)
	AM_RANGE(0xa800, 0xa800) AM_WRITE(SN76496_2_w)
	AM_RANGE(0xac00, 0xac00) AM_WRITE(SN76496_3_w)
	AM_RANGE(0xc001, 0xc001) AM_MIRROR(0x0080) AM_READ(input_port_4_r)
	AM_RANGE(0xc002, 0xc002) AM_MIRROR(0x0080) AM_READ(input_port_3_r)
	AM_RANGE(0xc003, 0xc003) AM_MIRROR(0x0080) AM_READ(input_port_0_r)
	AM_RANGE(0xc004, 0xc004) AM_READWRITE(docastle_flipscreen_off_r, docastle_flipscreen_off_w)
	AM_RANGE(0xc005, 0xc005) AM_MIRROR(0x0080) AM_READ(input_port_1_r)
	AM_RANGE(0xc007, 0xc007) AM_MIRROR(0x0080) AM_READ(input_port_2_r)
	AM_RANGE(0xc084, 0xc084) AM_READWRITE(docastle_flipscreen_on_r, docastle_flipscreen_on_w)
	AM_RANGE(0xe000, 0xe008) AM_READWRITE(docastle_shared1_r, docastle_shared0_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( idsoccer_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x57ff) AM_RAM
	AM_RANGE(0x5800, 0x59ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x6000, 0x9fff) AM_ROM
	AM_RANGE(0xa000, 0xa008) AM_READWRITE(docastle_shared0_r, docastle_shared1_w)
	AM_RANGE(0xa800, 0xa800) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0xb000, 0xb3ff) AM_MIRROR(0x0800) AM_READWRITE(MRA8_RAM, docastle_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xb400, 0xb7ff) AM_MIRROR(0x0800) AM_READWRITE(MRA8_RAM, docastle_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xc000, 0xc000) AM_READWRITE(idsoccer_adpcm_status_r, idsoccer_adpcm_w)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(docastle_nmitrigger_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( idsoccer_map2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0xa000, 0xa008) AM_READWRITE(docastle_shared1_r, docastle_shared0_w)
	AM_RANGE(0xc001, 0xc001) AM_MIRROR(0x0080) AM_READ(input_port_4_r)
	AM_RANGE(0xc002, 0xc002) AM_MIRROR(0x0080) AM_READ(input_port_3_r)
	AM_RANGE(0xc003, 0xc003) AM_MIRROR(0x0080) AM_READ(input_port_0_r)
	AM_RANGE(0xc004, 0xc004) AM_READWRITE(input_port_5_r, docastle_flipscreen_off_w)
	AM_RANGE(0xc005, 0xc005) AM_MIRROR(0x0080) AM_READ(input_port_1_r)
	AM_RANGE(0xc007, 0xc007) AM_MIRROR(0x0080) AM_READ(input_port_2_r)
	AM_RANGE(0xc084, 0xc084) AM_READWRITE(input_port_5_r, docastle_flipscreen_on_w)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(SN76496_0_w)
	AM_RANGE(0xe400, 0xe400) AM_WRITE(SN76496_1_w)
	AM_RANGE(0xe800, 0xe800) AM_WRITE(SN76496_2_w)
	AM_RANGE(0xec00, 0xec00) AM_WRITE(SN76496_3_w)
ADDRESS_MAP_END

/* Input Ports */

// Coinage used for all games
#define COINAGE_PORT \
	PORT_START \
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0x06, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x07, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	/* 0x01, 0x02, 0x03, 0x04, 0x05 all give 1 Coin/1 Credit */ \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	/* 0x10, 0x20, 0x30, 0x40, 0x50 all give 1 Coin/1 Credit */

#define DOCASTLE_IN0 \
	PORT_START \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL

#define DOCASTLE_IN1 \
	PORT_START \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

#define DOCASTLE_IN2 \
	PORT_START \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_TILT ) \
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_IMPULSE(32) \
	PORT_DIPNAME( 0x08, 0x08, "Freeze" ) \
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN1 ) \
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

INPUT_PORTS_START( docastle )
	DOCASTLE_IN0

	DOCASTLE_IN1

	DOCASTLE_IN2

	PORT_START	// DSW0
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x03, "1 (Beginner)" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "4 (Advanced)" )
	PORT_DIPNAME( 0x04, 0x04, "Rack Test" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Advance Level on Getting Diamond" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Difficulty of EXTRA" )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, "Difficult" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x80, "4" )
	PORT_DIPSETTING(    0x40, "5" )

	COINAGE_PORT
INPUT_PORTS_END

INPUT_PORTS_START( dorunrun )
	DOCASTLE_IN0

	DOCASTLE_IN1

	DOCASTLE_IN2

	PORT_START	// DSW0
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x03, "1 (Beginner)" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "4 (Advanced)" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Difficulty of EXTRA" )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, "Difficult" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x40, "Special" )
	PORT_DIPSETTING(    0x40, "Given" )
	PORT_DIPSETTING(    0x00, "Not Given" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "3" )
	PORT_DIPSETTING(    0x00, "5" )

	COINAGE_PORT
INPUT_PORTS_END

INPUT_PORTS_START( dowild )
	DOCASTLE_IN0

	DOCASTLE_IN1

	DOCASTLE_IN2

	PORT_START	// DSW0
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x03, "1 (Beginner)" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "4 (Advanced)" )
	PORT_DIPNAME( 0x04, 0x04, "Rack Test" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Difficulty of EXTRA" )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, "Difficult" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x40, "Special" )
	PORT_DIPSETTING(    0x40, "Given" )
	PORT_DIPSETTING(    0x00, "Not Given" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "3" )
	PORT_DIPSETTING(    0x00, "5" )

	COINAGE_PORT
INPUT_PORTS_END

INPUT_PORTS_START( jjack )
	DOCASTLE_IN0

	DOCASTLE_IN1

	DOCASTLE_IN2

	PORT_START	// DSW0
	PORT_DIPNAME( 0x03, 0x03, "Difficulty?" )
	PORT_DIPSETTING(    0x03, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x04, 0x04, "Rack Test" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Extra?" )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x80, "4" )
	PORT_DIPSETTING(    0x40, "5" )

	COINAGE_PORT
INPUT_PORTS_END

INPUT_PORTS_START( kickridr )
	PORT_START	// IN0
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL

	DOCASTLE_IN1

	DOCASTLE_IN2

	PORT_START	// DSW0
	PORT_DIPNAME( 0x03, 0x03, "Difficulty?" )
	PORT_DIPSETTING(    0x03, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x04, 0x04, "Rack Test" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "DSW4" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x40, "DSW2" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DSW1" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	COINAGE_PORT
INPUT_PORTS_END

INPUT_PORTS_START( idsoccer )
	PORT_START	// IN0
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_8WAY PORT_PLAYER(2)

	DOCASTLE_IN1

	DOCASTLE_IN2

	PORT_START	// DSW0
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x04, 0x04, "One Player vs. Computer" )	// Additional time extended for winning score
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Player 2 Time Extension" )	// Player may play same game with additional credit
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Player 1 Time Extension" )	// Player may play same game with additional credit
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, "Real Game Time" )			// Indicator always shows 3:00 and counts down
	PORT_DIPSETTING(    0xc0, "3:00" )
	PORT_DIPSETTING(    0x80, "2:30" )
	PORT_DIPSETTING(    0x40, "2:00" )
	PORT_DIPSETTING(    0x00, "1:00" )

	COINAGE_PORT

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_8WAY PORT_PLAYER(2)
INPUT_PORTS_END

/* Graphics Layouts */

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0, 4, 8, 12, 16, 20, 24, 28 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0, 4, 8, 12, 16, 20, 24, 28,
			32, 36, 40, 44, 48, 52, 56, 60 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
			8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8
};

/* Graphics Decode Information */

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,       0, 64 },
	{ REGION_GFX2, 0, &spritelayout, 64*16, 32*2 },
	{ -1 }
};

/* Sound Interfaces */

static struct MSM5205interface msm5205_interface =
{
	idsoccer_adpcm_int,	// interrupt function
	MSM5205_S64_4B		// 6 kHz    ???
};

/* Machine Drivers */

static MACHINE_DRIVER_START( docastle )
	// basic machine hardware
//  MDRV_CPU_ADD_TAG("main", Z80, 4000000)  // 4 MHz
	MDRV_CPU_ADD_TAG("main", Z80, 3900000)	// make dip switches work in docastle and dorunrun
	MDRV_CPU_PROGRAM_MAP(docastle_map, 0)
	MDRV_CPU_IO_MAP(docastle_io_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)

	MDRV_CPU_ADD_TAG("slave", Z80, 4000000)	// 4 MHz
	MDRV_CPU_PROGRAM_MAP(docastle_map2, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 8)

	MDRV_CPU_ADD(Z80, 4000000)	// 4 MHz
	MDRV_CPU_PROGRAM_MAP(docastle_map3, 0)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse, 1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	// video hardware
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(1*8, 31*8-1, 4*8, 28*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(258)
	MDRV_COLORTABLE_LENGTH(64*16+2*32*16)

	MDRV_PALETTE_INIT(docastle)
	MDRV_VIDEO_START(docastle)
	MDRV_VIDEO_UPDATE(docastle)

	// sound hardware
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(SN76496, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(SN76496, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(SN76496, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(SN76496, 4000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( dorunrun )
	// basic machine hardware
	MDRV_IMPORT_FROM(docastle)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(dorunrun_map, 0)

	MDRV_CPU_MODIFY("slave")
	MDRV_CPU_PROGRAM_MAP(dorunrun_map2, 0)

	// video hardware
	MDRV_PALETTE_INIT(dorunrun)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( idsoccer )
	// basic machine hardware
	MDRV_IMPORT_FROM(docastle)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(idsoccer_map, 0)

	MDRV_CPU_MODIFY("slave")
	MDRV_CPU_PROGRAM_MAP(idsoccer_map2, 0)

	// video hardware
	MDRV_PALETTE_INIT(dorunrun)

	// sound hardware
	MDRV_SOUND_ADD(MSM5205, 384000)
	MDRV_SOUND_CONFIG(msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)
MACHINE_DRIVER_END

/* ROMs */

ROM_START( docastle )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "01p_a1.bin",   0x0000, 0x2000, CRC(17c6fc24) SHA1(e397d283e8b8e1c512495b777c0fe16f66bb1862) )
	ROM_LOAD( "01n_a2.bin",   0x2000, 0x2000, CRC(1d2fc7f4) SHA1(f7b0c7466cd6a3854eda818c63663e3559dc7bc2) )
	ROM_LOAD( "01l_a3.bin",   0x4000, 0x2000, CRC(71a70ba9) SHA1(0c9e4c402f82d61d573eb55b3e2e0c8b7c8b7028) )
	ROM_LOAD( "01k_a4.bin",   0x6000, 0x2000, CRC(479a745e) SHA1(7195036727990932bc94b30405ebc2b8ea5b37a8) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "07n_a0.bin",   0x0000, 0x4000, CRC(f23b5cdb) SHA1(de71d9f142f8d420aa097d7e56b03a06db2f85fd) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "01d.bin",      0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "03a_a5.bin",   0x0000, 0x4000, CRC(0636b8f4) SHA1(8b34773bf7b7071fb5c8ff443a08befae3b545ea) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "04m_a6.bin",   0x0000, 0x2000, CRC(3bbc9b26) SHA1(1b1f79717e5c1a23eefb2d6df71ad95aac9f8be4) )
	ROM_LOAD( "04l_a7.bin",   0x2000, 0x2000, CRC(3dfaa9d1) SHA1(cc016019efe0a2bc38c66fadf792c05f6cabeeaa) )
	ROM_LOAD( "04j_a8.bin",   0x4000, 0x2000, CRC(9afb16e9) SHA1(f951e75af658623f3b0e18ff388990b2870fad53) )
	ROM_LOAD( "04h_a9.bin",   0x6000, 0x2000, CRC(af24bce0) SHA1(0f06c5d248c9c92f2a4636d259ab843339737969) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "09c.bin",      0x0000, 0x0200, CRC(066f52bc) SHA1(99f4f2d0181bcaf389c16f127cc3e632d62ee417) ) // color prom
ROM_END

ROM_START( docastl2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "a1",           0x0000, 0x2000, CRC(0d81fafc) SHA1(938c4470b022063ae66b607ca086fad98174248f) )
	ROM_LOAD( "a2",           0x2000, 0x2000, CRC(a13dc4ac) SHA1(f096d5ac8e26444ebdb4c3fb5c71592058fb6c79) )
	ROM_LOAD( "a3",           0x4000, 0x2000, CRC(a1f04ffb) SHA1(8cb53992cc679278a0bd33e5b728f27c585e38e1) )
	ROM_LOAD( "a4",           0x6000, 0x2000, CRC(1fb14aa6) SHA1(45b50db61ca2c9ace182ddba3817257308f07c89) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "a10",          0x0000, 0x4000, CRC(45f7f69b) SHA1(1c614c29be4950314ab04e0b828e691fd0907eff) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "01d.bin",      0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "03a_a5.bin",   0x0000, 0x4000, CRC(0636b8f4) SHA1(8b34773bf7b7071fb5c8ff443a08befae3b545ea) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "04m_a6.bin",   0x0000, 0x2000, CRC(3bbc9b26) SHA1(1b1f79717e5c1a23eefb2d6df71ad95aac9f8be4) )
	ROM_LOAD( "04l_a7.bin",   0x2000, 0x2000, CRC(3dfaa9d1) SHA1(cc016019efe0a2bc38c66fadf792c05f6cabeeaa) )
	ROM_LOAD( "04j_a8.bin",   0x4000, 0x2000, CRC(9afb16e9) SHA1(f951e75af658623f3b0e18ff388990b2870fad53) )
	ROM_LOAD( "04h_a9.bin",   0x6000, 0x2000, CRC(af24bce0) SHA1(0f06c5d248c9c92f2a4636d259ab843339737969) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "09c.bin",      0x0000, 0x0200, CRC(066f52bc) SHA1(99f4f2d0181bcaf389c16f127cc3e632d62ee417) ) // color prom
ROM_END

ROM_START( docastlo )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "c1.bin", 	0x0000, 0x2000, CRC(c9ce96ab) SHA1(3166aef4556ce334ecef27dceb285f51de371c35) )
	ROM_LOAD( "c2.bin",     0x2000, 0x2000, CRC(42b28369) SHA1(8c9a618984db52cdc4ec3a6bcfa7659866d2709c) )
	ROM_LOAD( "c3.bin",     0x4000, 0x2000, CRC(c8c13124) SHA1(2cfc15b232744b40350c174b0d89677495c077eb) )
	ROM_LOAD( "c4.bin",     0x6000, 0x2000, CRC(7ca78471) SHA1(2804f9be825973e69bc35aa703145b3ef22a5ecd) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "dorev10.bin",  0x0000, 0x4000, CRC(4b1925e3) SHA1(1b229dd73eede3853d23576dfe397a4d2d952991) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "01d.bin",      0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "03a_a5.bin",   0x0000, 0x4000, CRC(0636b8f4) SHA1(8b34773bf7b7071fb5c8ff443a08befae3b545ea) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "dorev6.bin",   0x0000, 0x2000, CRC(9e335bf8) SHA1(30ccfb959c1d3069739280711ba3d7d8b7a1a2b0) )
	ROM_LOAD( "dorev7.bin",   0x2000, 0x2000, CRC(f5d5701d) SHA1(f2731aaa7be3e269cf929aa14db1c967c9198dc6) )
	ROM_LOAD( "dorev8.bin",   0x4000, 0x2000, CRC(7143ca68) SHA1(b5b45e3cf7d0287d93231eb01395a03f39f473fc) )
	ROM_LOAD( "dorev9.bin",   0x6000, 0x2000, CRC(893fc004) SHA1(15559d11cc14341d2fec190d12205a649bdd484f) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	/* which prom? this set has the same gfx as douni so i'm using that prom */
//  ROM_LOAD( "09c.bin",      0x0000, 0x0200, CRC(066f52bc) SHA1(99f4f2d0181bcaf389c16f127cc3e632d62ee417) ) // color prom
	ROM_LOAD( "dorevc9.bin",  0x0000, 0x0200, CRC(96624ebe) SHA1(74ff21dc85dcb013c941ec6c06cafdb5bcc16960) ) // color prom
ROM_END

ROM_START( douni )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "dorev1.bin",   0x0000, 0x2000, CRC(1e2cbb3c) SHA1(f3c6eab7f7f43a067d432d47f3403ab1d07575fe) )
	ROM_LOAD( "dorev2.bin",   0x2000, 0x2000, CRC(18418f83) SHA1(1be77b0b53e6d243484d942108e84b950f1a4901) )
	ROM_LOAD( "dorev3.bin",   0x4000, 0x2000, CRC(7b9e2061) SHA1(2b21af54018a2ff756d80ba0b53b71108c0ce043) )
	ROM_LOAD( "dorev4.bin",   0x6000, 0x2000, CRC(e013954d) SHA1(1e05937f3b6eaef77c36b485da091ebb22e50f85) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "dorev10.bin",  0x0000, 0x4000, CRC(4b1925e3) SHA1(1b229dd73eede3853d23576dfe397a4d2d952991) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "01d.bin",      0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "03a_a5.bin",   0x0000, 0x4000, CRC(0636b8f4) SHA1(8b34773bf7b7071fb5c8ff443a08befae3b545ea) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "dorev6.bin",   0x0000, 0x2000, CRC(9e335bf8) SHA1(30ccfb959c1d3069739280711ba3d7d8b7a1a2b0) )
	ROM_LOAD( "dorev7.bin",   0x2000, 0x2000, CRC(f5d5701d) SHA1(f2731aaa7be3e269cf929aa14db1c967c9198dc6) )
	ROM_LOAD( "dorev8.bin",   0x4000, 0x2000, CRC(7143ca68) SHA1(b5b45e3cf7d0287d93231eb01395a03f39f473fc) )
	ROM_LOAD( "dorev9.bin",   0x6000, 0x2000, CRC(893fc004) SHA1(15559d11cc14341d2fec190d12205a649bdd484f) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "dorevc9.bin",  0x0000, 0x0200, CRC(96624ebe) SHA1(74ff21dc85dcb013c941ec6c06cafdb5bcc16960) ) // color prom
ROM_END

ROM_START( dorunruc )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "rev-0-1.p1",   0x0000, 0x2000, CRC(49906ebd) SHA1(03afb7e3107038d5a052e497b8a206334514536f) )
	ROM_LOAD( "rev-0-2.n1",   0x2000, 0x2000, CRC(dbe3e7db) SHA1(168026aacce73941329a72e78423f83f7c4f0f04) )
	ROM_LOAD( "rev-0-3.l1",   0x4000, 0x2000, CRC(e9b8181a) SHA1(6b960c3503589e62b61f9a2af372b98c48412bc0) )
	ROM_LOAD( "rev-0-4.k1",   0x6000, 0x2000, CRC(a63d0b89) SHA1(d2ab3b76149e6620f1eb93a051c802b208b8d6dc) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "rev-0-2.n7",   0x0000, 0x4000, CRC(6dac2fa3) SHA1(cd583f379f01788ce20f611f17689105d32ef97a) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "bprom2.bin",   0x0200, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "rev-0-5.a3",   0x0000, 0x4000, CRC(e20795b7) SHA1(ae4366d2c45580f3e60ae36f81a5fc912d1eb899) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "2764.m4",      0x0000, 0x2000, CRC(4bb231a0) SHA1(350423a1e602e23b229095021942d4b14a4736a7) )
	ROM_LOAD( "2764.l4",      0x2000, 0x2000, CRC(0c08508a) SHA1(1e235a0f44207c53af2c8da631e5a8e08b231258) )
	ROM_LOAD( "2764.j4",      0x4000, 0x2000, CRC(79287039) SHA1(e2e3c056f35a22e48115557e10fcd172ad2f91f1) )
	ROM_LOAD( "2764.h4",      0x6000, 0x2000, CRC(523aa999) SHA1(1d4aa0af79a2ed7b935d4ce92d978bf738f08eb3) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "dorunrun.clr", 0x0000, 0x0100, CRC(d5bab5d5) SHA1(7a465fe30b6008793d33f6e07086c89111e1e407) )
ROM_END

ROM_START( dorunrca )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "runrunc_p1",   0x0000, 0x2000, CRC(a8d789c6) SHA1(42eb5fb6a63300ea4ec71817b5cc9cbc7b0b4150) )
	ROM_LOAD( "runrunc_n1",   0x2000, 0x2000, CRC(f8c8a9f4) SHA1(0efd91758f951d8aadab4f5f0086413a8160d603) )
	ROM_LOAD( "runrunc_l1",   0x4000, 0x2000, CRC(223927ca) SHA1(8ae5587b4266d54ba7e8756ad07c867869ce3364) )
	ROM_LOAD( "runrunc_k1",   0x6000, 0x2000, CRC(5edd6752) SHA1(c7483755e2115420ab8a17287e4adfcd9bd1b5c4) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "rev-0-2.n7",   0x0000, 0x4000, CRC(6dac2fa3) SHA1(cd583f379f01788ce20f611f17689105d32ef97a) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "bprom2.bin",   0x0200, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "rev-0-5.a3",   0x0000, 0x4000, CRC(e20795b7) SHA1(ae4366d2c45580f3e60ae36f81a5fc912d1eb899) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "2764.m4",      0x0000, 0x2000, CRC(4bb231a0) SHA1(350423a1e602e23b229095021942d4b14a4736a7) )
	ROM_LOAD( "2764.l4",      0x2000, 0x2000, CRC(0c08508a) SHA1(1e235a0f44207c53af2c8da631e5a8e08b231258) )
	ROM_LOAD( "2764.j4",      0x4000, 0x2000, CRC(79287039) SHA1(e2e3c056f35a22e48115557e10fcd172ad2f91f1) )
	ROM_LOAD( "2764.h4",      0x6000, 0x2000, CRC(523aa999) SHA1(1d4aa0af79a2ed7b935d4ce92d978bf738f08eb3) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "dorunrun.clr", 0x0000, 0x0100, CRC(d5bab5d5) SHA1(7a465fe30b6008793d33f6e07086c89111e1e407) )
ROM_END

ROM_START( dorunrun )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "2764.p1",      0x0000, 0x2000, CRC(95c86f8e) SHA1(9fe44911e0aa8d4c7299472a31c401e064d63d17) )
	ROM_LOAD( "2764.l1",      0x4000, 0x2000, CRC(e9a65ba7) SHA1(fbee57d68352fd4062aac55cd1070f001714d0a3) )
	ROM_LOAD( "2764.k1",      0x6000, 0x2000, CRC(b1195d3d) SHA1(095ad2ee1f53be3203830263cb0c9efbe4710c56) )
	ROM_LOAD( "2764.n1",      0x8000, 0x2000, CRC(6a8160d1) SHA1(12101c351bf800319172c459b5e7c69cb5603806) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "27128.p7",     0x0000, 0x4000, CRC(8b06d461) SHA1(2434478810c6301197997be76505f5fc6beba5d3) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "bprom2.bin",   0x0200, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "27128.a3",     0x0000, 0x4000, CRC(4be96dcf) SHA1(f9b45e6297cbbc4d1ee2df7ac377c5daf5181b0f) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "2764.m4",      0x0000, 0x2000, CRC(4bb231a0) SHA1(350423a1e602e23b229095021942d4b14a4736a7) )
	ROM_LOAD( "2764.l4",      0x2000, 0x2000, CRC(0c08508a) SHA1(1e235a0f44207c53af2c8da631e5a8e08b231258) )
	ROM_LOAD( "2764.j4",      0x4000, 0x2000, CRC(79287039) SHA1(e2e3c056f35a22e48115557e10fcd172ad2f91f1) )
	ROM_LOAD( "2764.h4",      0x6000, 0x2000, CRC(523aa999) SHA1(1d4aa0af79a2ed7b935d4ce92d978bf738f08eb3) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "dorunrun.clr", 0x0000, 0x0100, CRC(d5bab5d5) SHA1(7a465fe30b6008793d33f6e07086c89111e1e407) )
ROM_END

ROM_START( dorunru2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "p1",           0x0000, 0x2000, CRC(12a99365) SHA1(12a1ab76182faa4f76cc5020913ca5706313fe72) )
	ROM_LOAD( "l1",           0x4000, 0x2000, CRC(38609287) SHA1(85f5cd707d620780436e4bed00753acef08f83cd) )
	ROM_LOAD( "k1",           0x6000, 0x2000, CRC(099aaf54) SHA1(c0419db2a2349ecb97c31256811993d1dcf3dc6e) )
	ROM_LOAD( "n1",           0x8000, 0x2000, CRC(4f8fcbae) SHA1(c1558664e081252141530e1932403df1fbf5f8a0) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "27128.p7",     0x0000, 0x4000, CRC(8b06d461) SHA1(2434478810c6301197997be76505f5fc6beba5d3) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "bprom2.bin",   0x0200, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "27128.a3",     0x0000, 0x4000, CRC(4be96dcf) SHA1(f9b45e6297cbbc4d1ee2df7ac377c5daf5181b0f) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "2764.m4",      0x0000, 0x2000, CRC(4bb231a0) SHA1(350423a1e602e23b229095021942d4b14a4736a7) )
	ROM_LOAD( "2764.l4",      0x2000, 0x2000, CRC(0c08508a) SHA1(1e235a0f44207c53af2c8da631e5a8e08b231258) )
	ROM_LOAD( "2764.j4",      0x4000, 0x2000, CRC(79287039) SHA1(e2e3c056f35a22e48115557e10fcd172ad2f91f1) )
	ROM_LOAD( "2764.h4",      0x6000, 0x2000, CRC(523aa999) SHA1(1d4aa0af79a2ed7b935d4ce92d978bf738f08eb3) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "dorunrun.clr", 0x0000, 0x0100, CRC(d5bab5d5) SHA1(7a465fe30b6008793d33f6e07086c89111e1e407) )
ROM_END

ROM_START( spiero )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "sp1.bin",      0x0000, 0x2000, CRC(08d23e38) SHA1(0810b0ecaa1bd7f16f78ce08054f5d24a57b1266) )
	ROM_LOAD( "sp3.bin",      0x4000, 0x2000, CRC(faa0c18c) SHA1(0dadea03b529bb889f45bd710bca0b4333cbbba8) )
	ROM_LOAD( "sp4.bin",      0x6000, 0x2000, CRC(639b4e5d) SHA1(cf252869fa0c5351f093026996c4e372f19a52a9) )
	ROM_LOAD( "sp2.bin",      0x8000, 0x2000, CRC(3a29ccb0) SHA1(37d6ce598a9c3b5dbb23c19a4bd94265287f83f7) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "27128.p7",     0x0000, 0x4000, CRC(8b06d461) SHA1(2434478810c6301197997be76505f5fc6beba5d3) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "bprom2.bin",   0x0200, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "sp5.bin",      0x0000, 0x4000, CRC(1b704bb0) SHA1(db3b2f120d632b5f897c47aee115916ec6c52a69) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "sp6.bin",      0x0000, 0x2000, CRC(00f893a7) SHA1(9680d59065a18ea25734bdcd9a1dd1d5d721a47b) )
	ROM_LOAD( "sp7.bin",      0x2000, 0x2000, CRC(173e5c6a) SHA1(5d06b8702e90af122c347e20b01811994165e727) )
	ROM_LOAD( "sp8.bin",      0x4000, 0x2000, CRC(2e66525a) SHA1(50e7b5e5f01d961eb311c65321fc536d8e4eb7b0) )
	ROM_LOAD( "sp9.bin",      0x6000, 0x2000, CRC(9c571525) SHA1(c7f1c22c6decd6326ef188bbf440115c1e2b16f4) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "bprom1.bin",   0x0000, 0x0200, CRC(fc1b66ff) SHA1(0a73f7e00501c638f017473b1e0786d7bcbbe82a) ) // color prom
ROM_END

ROM_START( dowild )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "w1",           0x0000, 0x2000, CRC(097de78b) SHA1(8d0cedde09a893ff67db0cb8e239babeb2cb3701) )
	ROM_LOAD( "w3",           0x4000, 0x2000, CRC(fc6a1cbb) SHA1(4cf59459d521c725e41bbd9363fb58bffdad13a2) )
	ROM_LOAD( "w4",           0x6000, 0x2000, CRC(8aac1d30) SHA1(c746f5506a541b25a7ca6fc754fbdb94212f7178) )
	ROM_LOAD( "w2",           0x8000, 0x2000, CRC(0914ab69) SHA1(048a68976d313015ff40f411d5c89d318fd9bb04) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "w10",          0x0000, 0x4000, CRC(d1f37fba) SHA1(827e2e3b140c4df2fd8780d7d05dc45694cf8f02) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "8300b-2",      0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "w5",           0x0000, 0x4000, CRC(b294b151) SHA1(c8c15bf9ab2401052ec80fdfc8fe124c6aa52521) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "w6",           0x0000, 0x2000, CRC(57e0208b) SHA1(52c1157d7df84fef754cd9466a70df7e53bc837f) )
	ROM_LOAD( "w7",           0x2000, 0x2000, CRC(5001a6f7) SHA1(ef37a26f0c2960fc55ff800da5d4d9c3d54270c2) )
	ROM_LOAD( "w8",           0x4000, 0x2000, CRC(ec503251) SHA1(ea7bb11c8e51fe69b5ecba4e4806ec8a8e4961d7) )
	ROM_LOAD( "w9",           0x6000, 0x2000, CRC(af7bd7eb) SHA1(1cc017b4606d1f6d70cc6cdaf9cf7797dd552b4b) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "dowild.clr",   0x0000, 0x0100, CRC(a703dea5) SHA1(159abdb62cb8bf3167d9fdc26038fb485219af7c) )
ROM_END

ROM_START( jjack )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "j1.bin",       0x0000, 0x2000, CRC(87f29bd2) SHA1(12b0a6f84439b84dd09fe54436c765fc58f928e1) )
	ROM_LOAD( "j3.bin",       0x4000, 0x2000, CRC(35b0517e) SHA1(756d7627f4dc2e9b24be7b0c4c80a9043cb4c322) )
	ROM_LOAD( "j4.bin",       0x6000, 0x2000, CRC(35bb316a) SHA1(8461503179eb7798de6f9f6677eb18ebcd22d470) )
	ROM_LOAD( "j2.bin",       0x8000, 0x2000, CRC(dec52e80) SHA1(1620a070a75115d7c35f44574a8b53ce137ce21a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "j0.bin",       0x0000, 0x4000, CRC(ab042f04) SHA1(06412dbdc43ebd6d376a811f240a4c9ec43ca6e7) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "bprom2.bin",   0x0200, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "j5.bin",       0x0000, 0x4000, CRC(75038ff9) SHA1(2a92e0d0adb1abd029c5ee6e350a859fed3f0ae9) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "j6.bin",       0x0000, 0x2000, CRC(5937bd7b) SHA1(4d399370422e22448d1680113d5bbe9894086a2b) )
	ROM_LOAD( "j7.bin",       0x2000, 0x2000, CRC(cf8ae8e7) SHA1(81b881eda0bf60f52dadf22aafc9ca7162fd1540) )
	ROM_LOAD( "j8.bin",       0x4000, 0x2000, CRC(84f6fc8c) SHA1(be631af9525262cf98c982b88cb97d3287366731) )
	ROM_LOAD( "j9.bin",       0x6000, 0x2000, CRC(3f9bb09f) SHA1(a38f7c9a21e37c7b903497e8170f64a378df730e) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "bprom1.bin",   0x0000, 0x0200, CRC(2f0955f2) SHA1(5eb417478669560f447a0a0e6fe93af27804590f) ) // color prom
ROM_END

ROM_START( kickridr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "k1",           0x0000, 0x2000, CRC(dfdd1ab4) SHA1(2f1ad3a8b2c1cbca2dc4a9a0c9a5f79af8712999) )
	ROM_LOAD( "k3",           0x4000, 0x2000, CRC(412244da) SHA1(2d4efad88c27db00c18626a667bd00531f4cc4fb) )
	ROM_LOAD( "k4",           0x6000, 0x2000, CRC(a67dd2ec) SHA1(d35eefd314c7a7d9badffc8a19270380f01e263c) )
	ROM_LOAD( "k2",           0x8000, 0x2000, CRC(e193fb5c) SHA1(3719c012bb1d75e79aa111093864b6e6bb46bc8c) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "k10",          0x0000, 0x4000, CRC(6843dbc0) SHA1(8a83f785e1fc2fa34a9955c19e27a1968a6d3f08) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "8300b-2",      0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "k5",           0x0000, 0x4000, CRC(3f7d7e49) SHA1(ed9697656a46c3e036665659a008396b323c9c2b) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "k6",           0x0000, 0x2000, CRC(94252ed3) SHA1(ec7a31d786104a8ebff9229d89e78a074b1b5205) )
	ROM_LOAD( "k7",           0x2000, 0x2000, CRC(7ef2420e) SHA1(b04443ba9f7d19b0b8c15fab2aa7d86febb2add3) )
	ROM_LOAD( "k8",           0x4000, 0x2000, CRC(29bed201) SHA1(f44b2a65d3bd6612a726dd17c3e5e29e7f8196b2) )
	ROM_LOAD( "k9",           0x6000, 0x2000, CRC(847584d3) SHA1(14aa799a2d060f49fc955c760b0e97e9c1ac1662) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "kickridr.clr", 0x0000, 0x0100, CRC(73ec281c) SHA1(1af32653e03c1e0aadef47b91bdc3f3c56ef7b23) )
ROM_END

ROM_START( idsoccer )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "id01",           0x0000, 0x2000, CRC(f1c3bf09) SHA1(446d373671f122d8131d2ec2d80c2110ec68a19b) )
	ROM_LOAD( "id02",           0x2000, 0x2000, CRC(184e6af0) SHA1(1664ca6fa0efae8496d051ac5f19596239e7cbcb) )
	ROM_LOAD( "id03",           0x6000, 0x2000, CRC(22524661) SHA1(363528a1135d11ead03c76064042a72e0ac93533) )
	ROM_LOAD( "id04",           0x8000, 0x2000, CRC(e8cd95fd) SHA1(2e272c154bd5dd2dca58d3fe12c6ba5e01a62477) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "id10",           0x0000, 0x4000, CRC(6c8b2037) SHA1(718d680186623d5af23ed272f04e726fbb17f078) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )
	ROM_LOAD( "id_8p",          0x0000, 0x0200, CRC(2747ca77) SHA1(abc0ca05925974c4b852827605ee2f1caefb8524) )

	ROM_REGION( 0x8000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "id05",          0x0000, 0x4000, CRC(a57c7a11) SHA1(9faebad0050da05101811427f350e163a7811396) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "id06",           0x00000, 0x8000, CRC(b42a6f4a) SHA1(ddce4438b3649610bd3703cbd7592aaa9a3eda0e) )
	ROM_LOAD( "id07",           0x08000, 0x8000, CRC(fa2b1c77) SHA1(0d8e9db065c76621deb58575f01c6ec5ee6cf6b0) )
	ROM_LOAD( "id08",           0x10000, 0x8000, CRC(5e97eab9) SHA1(40d261a0255c594353816c18aa6c0c245aeb68a8) )
	ROM_LOAD( "id09",           0x18000, 0x8000, CRC(a2a69223) SHA1(6bd9b76e0119643450c9f64c80b52e9056da82d6) )

	ROM_REGION( 0x10000, REGION_SOUND1, 0 )
	ROM_LOAD( "is1",            0x0000, 0x4000, CRC(9eb76196) SHA1(c15331dd8c3efaa83a95245210d05eaaa64b3161) )
	ROM_LOAD( "is3",            0x8000, 0x4000, CRC(27bebba3) SHA1(cf752b22603c1e2a0b33958481c652d6d56ebf68) )
	ROM_LOAD( "is4",            0xc000, 0x4000, CRC(dd5ffaa2) SHA1(4bc4330a54ca93448a8fe05207d3fb1a3a9872e1) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "id_3d.clr",   0x0000, 0x0200, CRC(a433ff62) SHA1(db9afe5fc917d25aafa21576cb1cecec7481d4cb) )
ROM_END

/* Game Drivers */

GAME( 1983, docastle, 0,        docastle, docastle, 0, ROT270, "Universal", "Mr. Do's Castle (set 1)", 0 ,2)
GAME( 1983, docastl2, docastle, docastle, docastle, 0, ROT270, "Universal", "Mr. Do's Castle (set 2)", 0 ,2)
GAME( 1983, docastlo, docastle, docastle, docastle, 0, ROT270, "Universal", "Mr. Do's Castle (older)", 0 ,2)
GAME( 1983, douni,    docastle, docastle, docastle, 0, ROT270, "Universal", "Mr. Do vs. Unicorns", 0 ,2)
GAME( 1984, dorunrun, 0,        dorunrun, dorunrun, 0, ROT0,   "Universal", "Do! Run Run (set 1)", 0 ,2)
GAME( 1984, dorunru2, dorunrun, dorunrun, dorunrun, 0, ROT0,   "Universal", "Do! Run Run (set 2)", 0 ,2)
GAME( 1984, dorunruc, dorunrun, docastle, dorunrun, 0, ROT0,   "Universal", "Do! Run Run (Do's Castle hardware, set 1)", 0 ,2)
GAME( 1984, dorunrca, dorunrun, docastle, dorunrun, 0, ROT0,   "Universal", "Do! Run Run (Do's Castle hardware, set 2)", 0 ,2)
GAME( 1987, spiero,   dorunrun, dorunrun, dorunrun, 0, ROT0,   "Universal", "Super Pierrot (Japan)", 0 ,2)
GAME( 1984, dowild,   0,        dorunrun, dowild,   0, ROT0,   "Universal", "Mr. Do's Wild Ride", 0 ,2)
GAME( 1984, jjack,    0,        dorunrun, jjack,    0, ROT270, "Universal", "Jumping Jack", 0 ,0)
GAME( 1984, kickridr, 0,        dorunrun, kickridr, 0, ROT0,   "Universal", "Kick Rider", 0 ,0)
GAME( 1985, idsoccer, 0,        idsoccer, idsoccer, 0, ROT0,   "Universal", "Indoor Soccer", GAME_NO_COCKTAIL ,0)
