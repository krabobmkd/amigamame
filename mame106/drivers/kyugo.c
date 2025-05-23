/***************************************************************************

    Kyugo hardware games

    driver by:
    Ernesto Corvi
    someone@secureshell.com

    Games supported:
        * Gyrodine - (c) 1984 Taito Corporation.
        * Son of Phoenix - (c) 1985 Associated Overseas MFR, Inc.
        * Repulse - (c) 1985 Sega
        * '99 The last war - (c) 1985 Proma
        * Flashgal - (c) 1985 Sega
        * SRD Mission - (c) 1986 Taito Corporation.
        * Legend - no copyright, but readme says: (c) 1986 SEGA/Coreland
        * Airwolf - (c) 1987 Kyugo

    Known issues:
        * attract mode in Son of Phoenix doesn't work

***************************************************************************/

#include "driver.h"
#include "kyugo.h"
#include "sound/ay8910.h"


static UINT8 *shared_ram;


/*************************************
 *
 *  Machine initialization
 *
 *************************************/

MACHINE_RESET( kyugo )
{
	// must start with interrupts and sub CPU disabled
	cpu_interrupt_enable(0, 0);
	kyugo_sub_cpu_control_w(0, 0);
}


WRITE8_HANDLER( kyugo_sub_cpu_control_w )
{
	cpunum_set_input_line(1, INPUT_LINE_HALT, data ? CLEAR_LINE : ASSERT_LINE);
}


/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_READWRITE(MRA8_RAM, kyugo_bgvideoram_w) AM_BASE(&kyugo_bgvideoram)
	AM_RANGE(0x8800, 0x8fff) AM_READWRITE(MRA8_RAM, kyugo_bgattribram_w) AM_BASE(&kyugo_bgattribram)
	AM_RANGE(0x9000, 0x97ff) AM_READWRITE(MRA8_RAM, kyugo_fgvideoram_w) AM_BASE(&kyugo_fgvideoram)
	AM_RANGE(0x9800, 0x9fff) AM_READWRITE(kyugo_spriteram_2_r, MWA8_RAM) AM_BASE(&kyugo_spriteram_2)
	AM_RANGE(0xa000, 0xa7ff) AM_RAM AM_BASE(&kyugo_spriteram_1)
	AM_RANGE(0xa800, 0xa800) AM_WRITE(kyugo_scroll_x_lo_w)
	AM_RANGE(0xb000, 0xb000) AM_WRITE(kyugo_gfxctrl_w)
	AM_RANGE(0xb800, 0xb800) AM_WRITE(kyugo_scroll_y_w)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM AM_SHARE(1) AM_BASE(&shared_ram)
ADDRESS_MAP_END



/*************************************
 *
 *  Main CPU port handlers
 *
 *************************************/

#define Main_PortMap( name, base )										\
static ADDRESS_MAP_START( name##_portmap, ADDRESS_SPACE_IO, 8 )			\
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )									\
	AM_RANGE(base+0, base+0) AM_WRITE(interrupt_enable_w)				\
	AM_RANGE(base+1, base+1) AM_WRITE(kyugo_flipscreen_w)				\
	AM_RANGE(base+2, base+2) AM_WRITE(kyugo_sub_cpu_control_w)			\
ADDRESS_MAP_END

Main_PortMap( gyrodine, 0x00 )
Main_PortMap( flashgal, 0x40 )
Main_PortMap( flashgla, 0xc0 )
Main_PortMap( srdmissn, 0x08 )



/*************************************
 *
 *  Sub CPU memory handlers
 *
 *************************************/

#define Sub_MemMap( name, rom_end, shared, in0, in1, in2 )					\
static ADDRESS_MAP_START( name##_sub_map, ADDRESS_SPACE_PROGRAM, 8 )		\
	AM_RANGE(0x0000, rom_end) AM_ROM										\
	AM_RANGE(shared, shared+0x7ff) AM_RAM AM_SHARE(1)						\
	AM_RANGE(in0, in0) AM_READ(input_port_2_r)								\
	AM_RANGE(in1, in1) AM_READ(input_port_3_r)								\
	AM_RANGE(in2, in2) AM_READ(input_port_4_r)								\
ADDRESS_MAP_END

Sub_MemMap( gyrodine, 0x1fff, 0x4000, 0x8080, 0x8040, 0x8000 )
Sub_MemMap( sonofphx, 0x7fff, 0xa000, 0xc080, 0xc040, 0xc000 )
Sub_MemMap( srdmissn, 0x7fff, 0x8000, 0xf400, 0xf401, 0xf402 )
Sub_MemMap( legend,   0x7fff, 0xc000, 0xf800, 0xf801, 0xf802 )
Sub_MemMap( flashgla, 0x7fff, 0xe000, 0xc040, 0xc080, 0xc0c0 )



/*************************************
 *
 *  Sub CPU port handlers
 *
 *************************************/

#define Sub_PortMap( name, ay0_base, ay1_base )								\
static ADDRESS_MAP_START( name##_sub_portmap, ADDRESS_SPACE_IO, 8 )			\
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )										\
	AM_RANGE(ay0_base+0, ay0_base+0) AM_WRITE(AY8910_control_port_0_w)		\
	AM_RANGE(ay0_base+1, ay0_base+1) AM_WRITE(AY8910_write_port_0_w)		\
	AM_RANGE(ay0_base+2, ay0_base+2) AM_READ(AY8910_read_port_0_r)			\
	AM_RANGE(ay1_base+0, ay1_base+0) AM_WRITE(AY8910_control_port_1_w)		\
	AM_RANGE(ay1_base+1, ay1_base+1) AM_WRITE(AY8910_write_port_1_w)		\
ADDRESS_MAP_END																\

Sub_PortMap( gyrodine, 0x00, 0xc0 )
Sub_PortMap( sonofphx, 0x00, 0x40 )
Sub_PortMap( srdmissn, 0x80, 0x84 )
Sub_PortMap( flashgla, 0x40, 0x80 )


/*************************************
 *
 *  Port definitions
 *
 *************************************/

#define START_COINS \
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) 	\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) 	\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )	\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START1 )	\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )

#define JOYSTICK_1 \
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY \
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY \
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY \
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY \
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )					\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 )

#define JOYSTICK_2 \
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL	\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL	\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL	\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL	\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL 					\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL

#define COIN_A_B \
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )		\
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )		\
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_2C ) )		\
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )		\
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )		\
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )		\
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )		\
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )		\
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )	\
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )		\
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )		\
	PORT_DIPSETTING(    0x08, DEF_STR( 4C_1C ) )		\
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )		\
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_1C ) )		\
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )		\
	PORT_DIPSETTING(    0x20, DEF_STR( 3C_4C ) )		\
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )		\
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )

#define COMMON_DSW2 \
	PORT_START_TAG("DSW2")\
	COIN_A_B\
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unused ) )\
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )\
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) )\
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

#define COMMON_END \
	PORT_START_TAG("IN0")\
	START_COINS\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_START_TAG("IN1")\
	JOYSTICK_1\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_START_TAG("IN2")\
	JOYSTICK_2\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

INPUT_PORTS_START( gyrodine )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x20, "20000 50000" )
	PORT_DIPSETTING(    0x00, "40000 70000" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

COMMON_DSW2
COMMON_END

INPUT_PORTS_END

INPUT_PORTS_START( sonofphx )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x04, "Every 50000" )
	PORT_DIPSETTING(    0x00, "Every 70000" )
	PORT_DIPNAME( 0x08, 0x08, "Slow Motion" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME(0x10,  0x10, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Sound Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW2")
	COIN_A_B
	PORT_DIPNAME( 0xc0, 0x80, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

COMMON_END
INPUT_PORTS_END

INPUT_PORTS_START( airwolf )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x01, "6" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x04, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, "Slow Motion" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Sound Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

COMMON_DSW2
COMMON_END
INPUT_PORTS_END

/* Same as 'airwolf', but different "Lives" Dip Switch */
INPUT_PORTS_START( skywolf )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x04, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, "Slow Motion" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Sound Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

COMMON_DSW2
COMMON_END
INPUT_PORTS_END

INPUT_PORTS_START( flashgal )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x04, "Every 50000" )
	PORT_DIPSETTING(    0x00, "Every 70000" )
	PORT_DIPNAME( 0x08, 0x08, "Slow Motion" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x10, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x20, 0x20, "Sound Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

COMMON_DSW2
COMMON_END
INPUT_PORTS_END

INPUT_PORTS_START( srdmissn )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x04, 0x04, "Bonus Life/Continue" )
	PORT_DIPSETTING(    0x04, "Every 50000/No" )
	PORT_DIPSETTING(    0x00, "Every 70000/Yes" )
	PORT_DIPNAME( 0x08, 0x08, "Slow Motion" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME(0x10,  0x10, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Sound Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

COMMON_DSW2
COMMON_END
INPUT_PORTS_END

INPUT_PORTS_START( legend )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x04, 0x04, "Bonus Life/Continue" )
	PORT_DIPSETTING(    0x04, "Every 50000/No" )
	PORT_DIPSETTING(    0x00, "Every 70000/Yes" )
	PORT_DIPNAME( 0x08, 0x08, "Slow Motion" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )	/* probably unused */
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Sound Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

COMMON_DSW2
COMMON_END
INPUT_PORTS_END


/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout fg_tilelayout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 0, 4 },
	{ 0, 1, 2, 3, 8*8+0, 8*8+1, 8*8+2, 8*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8*2
};

static const gfx_layout bg_tilelayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{     0,     1,     2,     3,     4,     5,     6,     7,
	  8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{  0*8,  1*8,  2*8,  3*8,  4*8,  5*8,  6*8,  7*8,
	  16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	16*16
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &fg_tilelayout, 0, 64 },
	{ REGION_GFX2, 0, &bg_tilelayout, 0, 32 },
	{ REGION_GFX3, 0, &spritelayout,  0, 32 },
	{ -1 }
};


/*************************************
 *
 *  Sound definition
 *
 *************************************/

static struct AY8910interface ay8910_interface =
{
	input_port_0_r,
	input_port_1_r
};


/*************************************
 *
 *  Machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( gyrodine )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", Z80, 18432000 / 4)	/* 18.432 MHz crystal */
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_IO_MAP(0,gyrodine_portmap)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,1)

	MDRV_CPU_ADD_TAG("sub", Z80, 18432000 / 4)	/* 18.432 MHz crystal */
	MDRV_CPU_PROGRAM_MAP(gyrodine_sub_map,0)
	MDRV_CPU_IO_MAP(gyrodine_sub_portmap,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(100)

	MDRV_MACHINE_RESET(kyugo)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 36*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(kyugo)
	MDRV_VIDEO_UPDATE(kyugo)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( sonofphx )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(gyrodine)
	MDRV_CPU_MODIFY("sub")
	MDRV_CPU_PROGRAM_MAP(sonofphx_sub_map,0)
	MDRV_CPU_IO_MAP(sonofphx_sub_portmap,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( srdmissn )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(gyrodine)
	MDRV_CPU_MODIFY("main")
	MDRV_CPU_IO_MAP(0,srdmissn_portmap)

	MDRV_CPU_MODIFY("sub")
	MDRV_CPU_PROGRAM_MAP(srdmissn_sub_map,0)
	MDRV_CPU_IO_MAP(srdmissn_sub_portmap,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( flashgal )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(sonofphx)
	MDRV_CPU_MODIFY("main")
	MDRV_CPU_IO_MAP(0,flashgal_portmap)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( flashgla )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(gyrodine)
	MDRV_CPU_MODIFY("main")
	MDRV_CPU_IO_MAP(0,flashgla_portmap)

	MDRV_CPU_MODIFY("sub")
	MDRV_CPU_PROGRAM_MAP(flashgla_sub_map,0)
	MDRV_CPU_IO_MAP(flashgla_sub_portmap,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( legend )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(gyrodine)
	MDRV_CPU_MODIFY("sub")
	MDRV_CPU_PROGRAM_MAP(legend_sub_map,0)
	MDRV_CPU_IO_MAP(srdmissn_sub_portmap,0)
MACHINE_DRIVER_END


/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( gyrodine )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "a21.02", 0x0000, 0x2000, CRC(c5ec4a50) SHA1(4d012aabdc248143a4d3bab190ecb6e335c93427) )
	ROM_LOAD( "a21.03", 0x2000, 0x2000, CRC(4e9323bd) SHA1(86ae4c6a29898fdb0e559ec2aac99fc874910fea) )
	ROM_LOAD( "a21.04", 0x4000, 0x2000, CRC(57e659d4) SHA1(4c0e73d0661360731691a32a6e94f41b69315f93) )
	ROM_LOAD( "a21.05", 0x6000, 0x2000, CRC(1e7293f3) SHA1(64695b80b409b02314334fb325f4d0c42a6d4d5b) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "a21.01", 0x0000, 0x2000, CRC(b2ce0aa2) SHA1(576754105819aec64781a5c8e8540b21fcfd346b) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "a21.15", 0x00000, 0x1000, CRC(adba18d0) SHA1(b1afd7d8f2a8545a00525a23e087d9ca975a6401) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "a21.08", 0x00000, 0x2000, CRC(a57df1c9) SHA1(63505f63e978c52c43fe863dca056b52f7ebd501) ) /* tiles - plane 0 */
	ROM_LOAD( "a21.07", 0x02000, 0x2000, CRC(63623ba3) SHA1(bcb80fc0edf7c4d1f82a2ff6d0bad9d2ccaf48c6) ) /* tiles - plane 1 */
	ROM_LOAD( "a21.06", 0x04000, 0x2000, CRC(4cc969a9) SHA1(0b89f4142c2fcc0a882fbc3514d8d11027e78e01) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "a21.14", 0x00000, 0x2000, CRC(9c5c4d5b) SHA1(0d0c9efb63b761acfaf51d17c525f94430ad703d) ) /* sprites - plane 0 */
	/* 0x03000-0x04fff empty */
	ROM_LOAD( "a21.13", 0x04000, 0x2000, CRC(d36b5aad) SHA1(8fac23474ffd3a62e2283eadbc8d278cd9c70105) ) /* sprites - plane 0 */
	/* 0x07000-0x08fff empty */
	ROM_LOAD( "a21.12", 0x08000, 0x2000, CRC(f387aea2) SHA1(de4aad7ad9ecc6a058b88c67bb18ee02605d9951) ) /* sprites - plane 1 */
	/* 0x0b000-0x0cfff empty */
	ROM_LOAD( "a21.11", 0x0c000, 0x2000, CRC(87967d7d) SHA1(d8026df749947a16f643d9b28640c9d293edd4a7) ) /* sprites - plane 1 */
	/* 0x0f000-0x10fff empty */
	ROM_LOAD( "a21.10", 0x10000, 0x2000, CRC(59640ab4) SHA1(747cb265f4504399837111c0dd48f07e05a57cc4) ) /* sprites - plane 2 */
	/* 0x13000-0x14fff empty */
	ROM_LOAD( "a21.09", 0x14000, 0x2000, CRC(22ad88d8) SHA1(3bdf93ca582d7454fc9e70bd6ce3cd076e0762aa) ) /* sprites - plane 2 */
	/* 0x17000-0x18fff empty */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "a21.16", 0x0000, 0x0100, CRC(cc25fb56) SHA1(5f533c4b4f49ba147c83d6a20d1e795c71db3c41) ) /* red */
	ROM_LOAD( "a21.17", 0x0100, 0x0100, CRC(ca054448) SHA1(4bad8147905cbe9ec8bb5bcd8016e9950c5d95a9) ) /* green */
	ROM_LOAD( "a21.18", 0x0200, 0x0100, CRC(23c0c449) SHA1(4a37821a6a16ae0cfdcfb0fa64733c03ba9e4815) ) /* blue */
	ROM_LOAD( "a21.20", 0x0300, 0x0020, CRC(efc4985e) SHA1(b2fa02e388fbbe1077e79699efccb2d47cb83ba5) ) /* char lookup table */
	ROM_LOAD( "m1.2c",  0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( gyrodinc )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "rom2",   0x0000, 0x2000, CRC(85ddea38) SHA1(fe7e8d7962850b17c39cac627994d78768b094f8) )
	ROM_LOAD( "a21.03", 0x2000, 0x2000, CRC(4e9323bd) SHA1(86ae4c6a29898fdb0e559ec2aac99fc874910fea) )
	ROM_LOAD( "a21.04", 0x4000, 0x2000, CRC(57e659d4) SHA1(4c0e73d0661360731691a32a6e94f41b69315f93) )
	ROM_LOAD( "a21.05", 0x6000, 0x2000, CRC(1e7293f3) SHA1(64695b80b409b02314334fb325f4d0c42a6d4d5b) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "a21.01", 0x0000, 0x2000, CRC(b2ce0aa2) SHA1(576754105819aec64781a5c8e8540b21fcfd346b) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "a21.15", 0x00000, 0x1000, CRC(adba18d0) SHA1(b1afd7d8f2a8545a00525a23e087d9ca975a6401) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "a21.08", 0x00000, 0x2000, CRC(a57df1c9) SHA1(63505f63e978c52c43fe863dca056b52f7ebd501) ) /* tiles - plane 0 */
	ROM_LOAD( "a21.07", 0x02000, 0x2000, CRC(63623ba3) SHA1(bcb80fc0edf7c4d1f82a2ff6d0bad9d2ccaf48c6) ) /* tiles - plane 1 */
	ROM_LOAD( "a21.06", 0x04000, 0x2000, CRC(4cc969a9) SHA1(0b89f4142c2fcc0a882fbc3514d8d11027e78e01) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "a21.14", 0x00000, 0x2000, CRC(9c5c4d5b) SHA1(0d0c9efb63b761acfaf51d17c525f94430ad703d) ) /* sprites - plane 0 */
	/* 0x03000-0x04fff empty */
	ROM_LOAD( "a21.13", 0x04000, 0x2000, CRC(d36b5aad) SHA1(8fac23474ffd3a62e2283eadbc8d278cd9c70105) ) /* sprites - plane 0 */
	/* 0x07000-0x08fff empty */
	ROM_LOAD( "a21.12", 0x08000, 0x2000, CRC(f387aea2) SHA1(de4aad7ad9ecc6a058b88c67bb18ee02605d9951) ) /* sprites - plane 1 */
	/* 0x0b000-0x0cfff empty */
	ROM_LOAD( "a21.11", 0x0c000, 0x2000, CRC(87967d7d) SHA1(d8026df749947a16f643d9b28640c9d293edd4a7) ) /* sprites - plane 1 */
	/* 0x0f000-0x10fff empty */
	ROM_LOAD( "a21.10", 0x10000, 0x2000, CRC(59640ab4) SHA1(747cb265f4504399837111c0dd48f07e05a57cc4) ) /* sprites - plane 2 */
	/* 0x13000-0x14fff empty */
	ROM_LOAD( "a21.09", 0x14000, 0x2000, CRC(22ad88d8) SHA1(3bdf93ca582d7454fc9e70bd6ce3cd076e0762aa) ) /* sprites - plane 2 */
	/* 0x17000-0x18fff empty */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "a21.16", 0x0000, 0x0100, CRC(cc25fb56) SHA1(5f533c4b4f49ba147c83d6a20d1e795c71db3c41) ) /* red */
	ROM_LOAD( "a21.17", 0x0100, 0x0100, CRC(ca054448) SHA1(4bad8147905cbe9ec8bb5bcd8016e9950c5d95a9) ) /* green */
	ROM_LOAD( "a21.18", 0x0200, 0x0100, CRC(23c0c449) SHA1(4a37821a6a16ae0cfdcfb0fa64733c03ba9e4815) ) /* blue */
	ROM_LOAD( "a21.20", 0x0300, 0x0020, CRC(efc4985e) SHA1(b2fa02e388fbbe1077e79699efccb2d47cb83ba5) ) /* char lookup table */
	ROM_LOAD( "m1.2c",  0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END


ROM_START( sonofphx )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "5.f4",   0x0000, 0x2000, CRC(e0d2c6cf) SHA1(87befaefa3e4f07523e9c4db19f13ff9309a7dcc) )
	ROM_LOAD( "6.h4",   0x2000, 0x2000, CRC(3a0d0336) SHA1(8e538d45d27ad881fb2ed71647353c6535646047) )
	ROM_LOAD( "7.j4",   0x4000, 0x2000, CRC(57a8e900) SHA1(bc878e27130f0a9afb50c1926b47621e5e58d8b2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "1.f2",   0x0000, 0x2000, CRC(c485c621) SHA1(14fa1b1403f4f2513e1a824f79b750cedf24a31e) )
	ROM_LOAD( "2.h2",   0x2000, 0x2000, CRC(b3c6a886) SHA1(efb136fc1671092fabc2fb2aff189a61bac90ca4) )
	ROM_LOAD( "3.j2",   0x4000, 0x2000, CRC(197e314c) SHA1(6921cd1bc3571b0ac7d8d7eb19b256daca85f17e) )
	ROM_LOAD( "4.k2",   0x6000, 0x2000, CRC(4f3695a1) SHA1(63443d0a0c52e9a761934f8fd43792579fb9966b) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "14.4a",  0x00000, 0x1000, CRC(b3859b8b) SHA1(9afec14bcee6093ff466ae00b721b177dfdac392) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "15.9h",  0x00000, 0x2000, CRC(c9213469) SHA1(03bd7a86f9cbb28ecf39e7ac643a186cfeb38a35) ) /* tiles - plane 0 */
	ROM_LOAD( "16.10h", 0x02000, 0x2000, CRC(7de5d39e) SHA1(47fc5740a972e105d282873b4d72774a4405dfff) ) /* tiles - plane 1 */
	ROM_LOAD( "17.11h", 0x04000, 0x2000, CRC(0ba5f72c) SHA1(79292e16e2f6079f160d957a22e355457599669d) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "8.6a",   0x00000, 0x4000, CRC(0e9f757e) SHA1(1d4a46b3f18fe5099cdc889ba5e55c1d171a0430) ) /* sprites - plane 0 */
	ROM_LOAD( "9.7a",   0x04000, 0x4000, CRC(f7d2e650) SHA1(eac715e09ad22b1a1d18e5cade4955cb8d4156f4) ) /* sprites - plane 0 */
	ROM_LOAD( "10.8a",  0x08000, 0x4000, CRC(e717baf4) SHA1(d52a6c5f8b915769cc6dfb50d34922c1a3cd1333) ) /* sprites - plane 1 */
	ROM_LOAD( "11.9a",  0x0c000, 0x4000, CRC(04b2250b) SHA1(d9948277d3ba3cb8188de647e25848f5222d066a) ) /* sprites - plane 1 */
	ROM_LOAD( "12.10a", 0x10000, 0x4000, CRC(d110e140) SHA1(eb528b437e7967ecbe56de51274f286e563f7100) ) /* sprites - plane 2 */
	ROM_LOAD( "13.11a", 0x14000, 0x4000, CRC(8fdc713c) SHA1(c8933d1c45c886c22ee89d02b8941bbbb963d7b1) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "b.1j",   0x0000, 0x0100, CRC(3ea35431) SHA1(b45318ce898f03a338435a3f6109483d246ff914) ) /* blue */
	ROM_LOAD( "g.1h",   0x0100, 0x0100, CRC(acd7a69e) SHA1(b18eab8f669f0a8105a4bbffa346c4b19491c451) ) /* green */
	ROM_LOAD( "r.1f",   0x0200, 0x0100, CRC(b7f48b41) SHA1(2d84dc29c0ab43729014129e6392207db0f56e9e) ) /* red */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",  0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( repulse )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "repulse.b5",   0x0000, 0x2000, CRC(fb2b7c9d) SHA1(7a6867a9deda7eb399bf5b01c5422400d443faea) )
	ROM_LOAD( "repulse.b6",   0x2000, 0x2000, CRC(99129918) SHA1(9beba6ef62102d6a28cf7a52ce5ce2a2113f8dfc) )
	ROM_LOAD( "7.j4",         0x4000, 0x2000, CRC(57a8e900) SHA1(bc878e27130f0a9afb50c1926b47621e5e58d8b2) )

	ROM_REGION( 0x10000, REGION_CPU2 , 0 ) /* 64k for code */
	ROM_LOAD( "1.f2",         0x0000, 0x2000, CRC(c485c621) SHA1(14fa1b1403f4f2513e1a824f79b750cedf24a31e) )
	ROM_LOAD( "2.h2",         0x2000, 0x2000, CRC(b3c6a886) SHA1(efb136fc1671092fabc2fb2aff189a61bac90ca4) )
	ROM_LOAD( "3.j2",         0x4000, 0x2000, CRC(197e314c) SHA1(6921cd1bc3571b0ac7d8d7eb19b256daca85f17e) )
	ROM_LOAD( "repulse.b4",   0x6000, 0x2000, CRC(86b267f3) SHA1(5e352737e0ea0ca4a025d002b75c821c55660b4f) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "repulse.a11",  0x00000, 0x1000, CRC(8e1de90a) SHA1(5e655e6d282f6c8ae8bdfb72db64212e9262f717) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "15.9h",        0x00000, 0x2000, CRC(c9213469) SHA1(03bd7a86f9cbb28ecf39e7ac643a186cfeb38a35) ) /* tiles - plane 0 */
	ROM_LOAD( "16.10h",       0x02000, 0x2000, CRC(7de5d39e) SHA1(47fc5740a972e105d282873b4d72774a4405dfff) ) /* tiles - plane 1 */
	ROM_LOAD( "17.11h",       0x04000, 0x2000, CRC(0ba5f72c) SHA1(79292e16e2f6079f160d957a22e355457599669d) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "8.6a",         0x00000, 0x4000, CRC(0e9f757e) SHA1(1d4a46b3f18fe5099cdc889ba5e55c1d171a0430) ) /* sprites - plane 0 */
	ROM_LOAD( "9.7a",         0x04000, 0x4000, CRC(f7d2e650) SHA1(eac715e09ad22b1a1d18e5cade4955cb8d4156f4) ) /* sprites - plane 0 */
	ROM_LOAD( "10.8a",        0x08000, 0x4000, CRC(e717baf4) SHA1(d52a6c5f8b915769cc6dfb50d34922c1a3cd1333) ) /* sprites - plane 1 */
	ROM_LOAD( "11.9a",        0x0c000, 0x4000, CRC(04b2250b) SHA1(d9948277d3ba3cb8188de647e25848f5222d066a) ) /* sprites - plane 1 */
	ROM_LOAD( "12.10a",       0x10000, 0x4000, CRC(d110e140) SHA1(eb528b437e7967ecbe56de51274f286e563f7100) ) /* sprites - plane 2 */
	ROM_LOAD( "13.11a",       0x14000, 0x4000, CRC(8fdc713c) SHA1(c8933d1c45c886c22ee89d02b8941bbbb963d7b1) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "b.1j",         0x0000, 0x0100, CRC(3ea35431) SHA1(b45318ce898f03a338435a3f6109483d246ff914) ) /* blue */
	ROM_LOAD( "g.1h",         0x0100, 0x0100, CRC(acd7a69e) SHA1(b18eab8f669f0a8105a4bbffa346c4b19491c451) ) /* green */
	ROM_LOAD( "r.1f",         0x0200, 0x0100, CRC(b7f48b41) SHA1(2d84dc29c0ab43729014129e6392207db0f56e9e) ) /* red */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",        0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( 99lstwar )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "1999.4f",      0x0000, 0x2000, CRC(e3cfc09f) SHA1(e48905726c6308194c596117dd30444dcb748908) )
	ROM_LOAD( "1999.4h",      0x2000, 0x2000, CRC(fd58c6e1) SHA1(005f3114425fd2bfb9452c790d40653661b3d1d9) )
	ROM_LOAD( "7.j4",         0x4000, 0x2000, CRC(57a8e900) SHA1(bc878e27130f0a9afb50c1926b47621e5e58d8b2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "1.f2",         0x0000, 0x2000, CRC(c485c621) SHA1(14fa1b1403f4f2513e1a824f79b750cedf24a31e) )
	ROM_LOAD( "2.h2",         0x2000, 0x2000, CRC(b3c6a886) SHA1(efb136fc1671092fabc2fb2aff189a61bac90ca4) )
	ROM_LOAD( "3.j2",         0x4000, 0x2000, CRC(197e314c) SHA1(6921cd1bc3571b0ac7d8d7eb19b256daca85f17e) )
	ROM_LOAD( "repulse.b4",   0x6000, 0x2000, CRC(86b267f3) SHA1(5e352737e0ea0ca4a025d002b75c821c55660b4f) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "1999.4a",      0x00000, 0x1000, CRC(49a2383e) SHA1(b4be929abbde034df5ef12342fbcecb14772886a) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "15.9h",        0x00000, 0x2000, CRC(c9213469) SHA1(03bd7a86f9cbb28ecf39e7ac643a186cfeb38a35) ) /* tiles - plane 0 */
	ROM_LOAD( "16.10h",       0x02000, 0x2000, CRC(7de5d39e) SHA1(47fc5740a972e105d282873b4d72774a4405dfff) ) /* tiles - plane 1 */
	ROM_LOAD( "17.11h",       0x04000, 0x2000, CRC(0ba5f72c) SHA1(79292e16e2f6079f160d957a22e355457599669d) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "8.6a",         0x00000, 0x4000, CRC(0e9f757e) SHA1(1d4a46b3f18fe5099cdc889ba5e55c1d171a0430) ) /* sprites - plane 0 */
	ROM_LOAD( "9.7a",         0x04000, 0x4000, CRC(f7d2e650) SHA1(eac715e09ad22b1a1d18e5cade4955cb8d4156f4) ) /* sprites - plane 0 */
	ROM_LOAD( "10.8a",        0x08000, 0x4000, CRC(e717baf4) SHA1(d52a6c5f8b915769cc6dfb50d34922c1a3cd1333) ) /* sprites - plane 1 */
	ROM_LOAD( "11.9a",        0x0c000, 0x4000, CRC(04b2250b) SHA1(d9948277d3ba3cb8188de647e25848f5222d066a) ) /* sprites - plane 1 */
	ROM_LOAD( "12.10a",       0x10000, 0x4000, CRC(d110e140) SHA1(eb528b437e7967ecbe56de51274f286e563f7100) ) /* sprites - plane 2 */
	ROM_LOAD( "13.11a",       0x14000, 0x4000, CRC(8fdc713c) SHA1(c8933d1c45c886c22ee89d02b8941bbbb963d7b1) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "b.1j",         0x0000, 0x0100, CRC(3ea35431) SHA1(b45318ce898f03a338435a3f6109483d246ff914) ) /* blue */
	ROM_LOAD( "g.1h",         0x0100, 0x0100, CRC(acd7a69e) SHA1(b18eab8f669f0a8105a4bbffa346c4b19491c451) ) /* green */
	ROM_LOAD( "r.1f",         0x0200, 0x0100, CRC(b7f48b41) SHA1(2d84dc29c0ab43729014129e6392207db0f56e9e) ) /* red */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",        0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( 99lstwra )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "4f.bin",       0x0000, 0x2000, CRC(efe2908d) SHA1(4de8661f523f002c6a9368f81d865c7cc98926dd) )
	ROM_LOAD( "4h.bin",       0x2000, 0x2000, CRC(5b79c342) SHA1(293990dab3360139727a5c90aad0826d4a3746b7) )
	ROM_LOAD( "4j.bin",       0x4000, 0x2000, CRC(d2a62c1b) SHA1(eef9103945db8cfc4c1e3a58d8ad222f8dc58492) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "2f.bin",       0x0000, 0x2000, CRC(cb9d8291) SHA1(f26687edda70a8678708b14f4eb4dfd1b3cb8582) )
	ROM_LOAD( "2h.bin",       0x2000, 0x2000, CRC(24dbddc3) SHA1(76aa9e8b59747436650b6629ff5acea81d1d76da) )
	ROM_LOAD( "2j.bin",       0x4000, 0x2000, CRC(16879c4c) SHA1(5195abdd5ad8cf9aa081f96ed5d16c14af603289) )
	ROM_LOAD( "repulse.b4",   0x6000, 0x2000, CRC(86b267f3) SHA1(5e352737e0ea0ca4a025d002b75c821c55660b4f) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "1999.4a",      0x00000, 0x1000, CRC(49a2383e) SHA1(b4be929abbde034df5ef12342fbcecb14772886a) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "9h.bin",       0x00000, 0x2000, CRC(59993c27) SHA1(e7de7fbea4f09718f0a13d4a7f60360e6431c3a7) ) /* tiles - plane 0 */
	ROM_LOAD( "10h.bin",      0x02000, 0x2000, CRC(dfbf0280) SHA1(c97923fcdd01bdcfbbc6308b04de8bb610e9b5d2) ) /* tiles - plane 1 */
	ROM_LOAD( "11h.bin",      0x04000, 0x2000, CRC(e4f29fc0) SHA1(8ef393d5292b0eb1d6c253589e37b3ab2eaeb402) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "6a.bin",       0x00000, 0x4000, CRC(98d44410) SHA1(3d3ca9d2d28038402daedc6b9c01977a1897d8b2) ) /* sprites - plane 0 */
	ROM_LOAD( "7a.bin",       0x04000, 0x4000, CRC(4c54d281) SHA1(be7d14d2a8910e0b38d5614c1eba5e71a6bf7a6c) ) /* sprites - plane 0 */
	ROM_LOAD( "8a.bin",       0x08000, 0x4000, CRC(81018101) SHA1(fa94b6bc07beb70bd6535c3f5620897c2ad49240) ) /* sprites - plane 1 */
	ROM_LOAD( "9a.bin",       0x0c000, 0x4000, CRC(347b91fd) SHA1(6ae29d2c075c72d3435d0f3cc095e0bf9657f36b) ) /* sprites - plane 1 */
	ROM_LOAD( "10a.bin",      0x10000, 0x4000, CRC(f07de4fa) SHA1(4b2e8386634205c84d8d32febd57efdb93d86e99) ) /* sprites - plane 2 */
	ROM_LOAD( "11a.bin",      0x14000, 0x4000, CRC(34a04f48) SHA1(6c3f735469a6d97b6aaece69c955c4bd5e324c49) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "b.1j",         0x0000, 0x0100, CRC(3ea35431) SHA1(b45318ce898f03a338435a3f6109483d246ff914) ) /* blue */
	ROM_LOAD( "g.1h",         0x0100, 0x0100, CRC(acd7a69e) SHA1(b18eab8f669f0a8105a4bbffa346c4b19491c451) ) /* green */
	ROM_LOAD( "r.1f",         0x0200, 0x0100, CRC(b7f48b41) SHA1(2d84dc29c0ab43729014129e6392207db0f56e9e) ) /* red */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",        0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( 99lstwrk )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "1999.4f",      0x0000, 0x2000, CRC(e3cfc09f) SHA1(e48905726c6308194c596117dd30444dcb748908) )
	ROM_LOAD( "1999.4h",      0x2000, 0x2000, CRC(fd58c6e1) SHA1(005f3114425fd2bfb9452c790d40653661b3d1d9) )
	ROM_LOAD( "7.j4",         0x4000, 0x2000, CRC(57a8e900) SHA1(bc878e27130f0a9afb50c1926b47621e5e58d8b2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "1.f2",         0x0000, 0x2000, CRC(c485c621) SHA1(14fa1b1403f4f2513e1a824f79b750cedf24a31e) )
	ROM_LOAD( "2.h2",         0x2000, 0x2000, CRC(b3c6a886) SHA1(efb136fc1671092fabc2fb2aff189a61bac90ca4) )
	ROM_LOAD( "3.j2",         0x4000, 0x2000, CRC(197e314c) SHA1(6921cd1bc3571b0ac7d8d7eb19b256daca85f17e) )
	ROM_LOAD( "repulse.b4",   0x6000, 0x2000, CRC(86b267f3) SHA1(5e352737e0ea0ca4a025d002b75c821c55660b4f) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "1999-14.rom",  0x00000, 0x1000, CRC(b4995072) SHA1(5127a979c7f6897c5c53927012c679653920e56f) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "15.9h",        0x00000, 0x2000, CRC(c9213469) SHA1(03bd7a86f9cbb28ecf39e7ac643a186cfeb38a35) ) /* tiles - plane 0 */
	ROM_LOAD( "16.10h",       0x02000, 0x2000, CRC(7de5d39e) SHA1(47fc5740a972e105d282873b4d72774a4405dfff) ) /* tiles - plane 1 */
	ROM_LOAD( "17.11h",       0x04000, 0x2000, CRC(0ba5f72c) SHA1(79292e16e2f6079f160d957a22e355457599669d) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "8.6a",         0x00000, 0x4000, CRC(0e9f757e) SHA1(1d4a46b3f18fe5099cdc889ba5e55c1d171a0430) ) /* sprites - plane 0 */
	ROM_LOAD( "9.7a",         0x04000, 0x4000, CRC(f7d2e650) SHA1(eac715e09ad22b1a1d18e5cade4955cb8d4156f4) ) /* sprites - plane 0 */
	ROM_LOAD( "10.8a",        0x08000, 0x4000, CRC(e717baf4) SHA1(d52a6c5f8b915769cc6dfb50d34922c1a3cd1333) ) /* sprites - plane 1 */
	ROM_LOAD( "11.9a",        0x0c000, 0x4000, CRC(04b2250b) SHA1(d9948277d3ba3cb8188de647e25848f5222d066a) ) /* sprites - plane 1 */
	ROM_LOAD( "12.10a",       0x10000, 0x4000, CRC(d110e140) SHA1(eb528b437e7967ecbe56de51274f286e563f7100) ) /* sprites - plane 2 */
	ROM_LOAD( "13.11a",       0x14000, 0x4000, CRC(8fdc713c) SHA1(c8933d1c45c886c22ee89d02b8941bbbb963d7b1) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "b.1j",   0x0000, 0x0100, CRC(3ea35431) SHA1(b45318ce898f03a338435a3f6109483d246ff914) ) /* blue */
	ROM_LOAD( "g.1h",   0x0100, 0x0100, CRC(acd7a69e) SHA1(b18eab8f669f0a8105a4bbffa346c4b19491c451) ) /* green */
	ROM_LOAD( "r.1f",   0x0200, 0x0100, CRC(b7f48b41) SHA1(2d84dc29c0ab43729014129e6392207db0f56e9e) ) /* red */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",  0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */

	ROM_REGION( 0x0800, REGION_USER1, 0 )
	ROM_LOAD( "1999-00.rom",  0x0000, 0x0800, CRC(0c0c449f) SHA1(efa4a8ac4c341ca5cdc3b5d2803eda43daf1bc93) ) /* unknown */
ROM_END

ROM_START( flashgal )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "epr-7167.4f",  0x0000, 0x2000, CRC(cf5ad733) SHA1(24561db9a3d72c7a69a7ce5a85aaa78254788675) )
	ROM_LOAD( "epr-7168.4h",  0x2000, 0x2000, CRC(00c4851f) SHA1(f29ef123702bb3506ac3740b2779ae2757d884c2) )
	ROM_LOAD( "epr-7169.4j",  0x4000, 0x2000, CRC(1ef0b8f7) SHA1(9c3ded1f985f4fb6b38843e0ca90ec458633d145) )
	ROM_LOAD( "epr-7170.4k",  0x6000, 0x2000, CRC(885d53de) SHA1(14c8d4d07574e2dc3fba9fc92483a810649e100a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "epr-7163.2f",  0x0000, 0x2000, CRC(eee2134d) SHA1(4d67a969ff033576988de73942717c84068a291d) )
	ROM_LOAD( "epr-7164.2h",  0x2000, 0x2000, CRC(e5e0cd22) SHA1(ad17f5a207a6d74cf0bd1cfebb061c8f65309563) )
	ROM_LOAD( "epr-7165.2j",  0x4000, 0x2000, CRC(4cd3fe5e) SHA1(a69e02acbcad825101f96dcabfada667ae25799c) )
	ROM_LOAD( "epr-7166.2k",  0x6000, 0x2000, CRC(552ca339) SHA1(2313c6aaec47957bce00d0d04f89012eafc014fd) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "epr-7177.4a",  0x00000, 0x1000, CRC(dca9052f) SHA1(cbeb997db01b875a73a67e08c0b94ab74de77b7b) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "epr-7178.9h",  0x00000, 0x2000, CRC(2f5b62c0) SHA1(8be09e4ef1c3e2b1494431d4f07570a109e93dfd) ) /* tiles - plane 0 */
	ROM_LOAD( "epr-7179.10h", 0x02000, 0x2000, CRC(8fbb49b5) SHA1(111f0c42cee6ab2766a5322dd90e1516ffbbc35c) ) /* tiles - plane 1 */
	ROM_LOAD( "epr-7180.11h", 0x04000, 0x2000, CRC(26a8e5c3) SHA1(9a2bc4de87e16bbb777b39564c08d99fd3ccd4ff) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "epr-7171.6a",  0x00000, 0x4000, CRC(62caf2a1) SHA1(6fb2c1882630b32af27638d2e9b306d4fc028d62) ) /* sprites - plane 0 */
	ROM_LOAD( "epr-7172.7a",  0x04000, 0x4000, CRC(10f78a10) SHA1(8745da857bdb873d5ffebf753dd66321e1fabc59) ) /* sprites - plane 0 */
	ROM_LOAD( "epr-7173.8a",  0x08000, 0x4000, CRC(36ea1d59) SHA1(cba9117de745049c06c703dc1782b83376523ce3) ) /* sprites - plane 1 */
	ROM_LOAD( "epr-7174.9a",  0x0c000, 0x4000, CRC(f527d837) SHA1(a674e1e71c98d263670962d60a9ac6e205df1204) ) /* sprites - plane 1 */
	ROM_LOAD( "epr-7175.10a", 0x10000, 0x4000, CRC(ba76e4c1) SHA1(e521fbf6a6a8f0e9866c143a576b9dbbc19c4ffd) ) /* sprites - plane 2 */
	ROM_LOAD( "epr-7176.11a", 0x14000, 0x4000, CRC(f095d619) SHA1(7f278c124141ae03f1d8a03b4ea06c81391f16eb) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "7161.1j",      0x0000, 0x0100, CRC(02c4043f) SHA1(bc2af8e054d71a3d0835c795b0f2263c32f2bc42) ) /* red */
	ROM_LOAD( "7160.1h",      0x0100, 0x0100, CRC(225938d1) SHA1(22bf02832b9f08e4811f9d74a6007bf0ff030eef) ) /* green */
	ROM_LOAD( "7159.1f",      0x0200, 0x0100, CRC(1e0a1cd3) SHA1(cc120d8fba3e4fb5e18d789981ece77e589ee5a2) ) /* blue */
	ROM_LOAD( "7162.5j",      0x0300, 0x0020, CRC(cce2e29f) SHA1(787c65b7d69bcd224b45138fdbbf3fdae296dda6) ) /* char lookup table */
	ROM_LOAD( "bpr.2c",       0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( flashgla )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "flashgal.5",   0x0000, 0x2000, CRC(aa889ace) SHA1(7CAAC8FAE723485ADB6990367BDB8A94BD273322) )
	ROM_LOAD( "epr-7168.4h",  0x2000, 0x2000, CRC(00c4851f) SHA1(f29ef123702bb3506ac3740b2779ae2757d884c2) )
	ROM_LOAD( "epr-7169.4j",  0x4000, 0x2000, CRC(1ef0b8f7) SHA1(9c3ded1f985f4fb6b38843e0ca90ec458633d145) )
	ROM_LOAD( "epr-7170.4k",  0x6000, 0x2000, CRC(885d53de) SHA1(14c8d4d07574e2dc3fba9fc92483a810649e100a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "flashgal.1",   0x0000, 0x2000, CRC(55171cc1) SHA1(6780BE187288BD7354769FA29A4250E065A91E28) )
	ROM_LOAD( "flashgal.2",   0x2000, 0x2000, CRC(3fd21aac) SHA1(C0D99DE50537AA76CD1D2FBA3DBF55E7101905DB) )
	ROM_LOAD( "flashgal.3",   0x4000, 0x2000, CRC(A1223B74) SHA1(13E2A33E3968F23D60393D950660BE4E6B8AE46D) )
	ROM_LOAD( "flashgal.4",   0x6000, 0x2000, CRC(04D2A05F) SHA1(009B8EAFC850A100D3A2CDE8F6FBBA9C98828DDD) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "epr-7177.4a",  0x00000, 0x1000, CRC(dca9052f) SHA1(cbeb997db01b875a73a67e08c0b94ab74de77b7b) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "epr-7178.9h",  0x00000, 0x2000, CRC(2f5b62c0) SHA1(8be09e4ef1c3e2b1494431d4f07570a109e93dfd) ) /* tiles - plane 0 */
	ROM_LOAD( "epr-7179.10h", 0x02000, 0x2000, CRC(8fbb49b5) SHA1(111f0c42cee6ab2766a5322dd90e1516ffbbc35c) ) /* tiles - plane 1 */
	ROM_LOAD( "epr-7180.11h", 0x04000, 0x2000, CRC(26a8e5c3) SHA1(9a2bc4de87e16bbb777b39564c08d99fd3ccd4ff) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "epr-7171.6a",  0x00000, 0x4000, CRC(62caf2a1) SHA1(6fb2c1882630b32af27638d2e9b306d4fc028d62) ) /* sprites - plane 0 */
	ROM_LOAD( "epr-7172.7a",  0x04000, 0x4000, CRC(10f78a10) SHA1(8745da857bdb873d5ffebf753dd66321e1fabc59) ) /* sprites - plane 0 */
	ROM_LOAD( "epr-7173.8a",  0x08000, 0x4000, CRC(36ea1d59) SHA1(cba9117de745049c06c703dc1782b83376523ce3) ) /* sprites - plane 1 */
	ROM_LOAD( "epr-7174.9a",  0x0c000, 0x4000, CRC(f527d837) SHA1(a674e1e71c98d263670962d60a9ac6e205df1204) ) /* sprites - plane 1 */
	ROM_LOAD( "epr-7175.10a", 0x10000, 0x4000, CRC(ba76e4c1) SHA1(e521fbf6a6a8f0e9866c143a576b9dbbc19c4ffd) ) /* sprites - plane 2 */
	ROM_LOAD( "epr-7176.11a", 0x14000, 0x4000, CRC(f095d619) SHA1(7f278c124141ae03f1d8a03b4ea06c81391f16eb) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "7161.1j",      0x0000, 0x0100, CRC(02c4043f) SHA1(bc2af8e054d71a3d0835c795b0f2263c32f2bc42) ) /* red */
	ROM_LOAD( "7160.1h",      0x0100, 0x0100, CRC(225938d1) SHA1(22bf02832b9f08e4811f9d74a6007bf0ff030eef) ) /* green */
	ROM_LOAD( "7159.1f",      0x0200, 0x0100, CRC(1e0a1cd3) SHA1(cc120d8fba3e4fb5e18d789981ece77e589ee5a2) ) /* blue */
	ROM_LOAD( "7162.5j",      0x0300, 0x0020, CRC(cce2e29f) SHA1(787c65b7d69bcd224b45138fdbbf3fdae296dda6) ) /* char lookup table */
	ROM_LOAD( "bpr.2c",       0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? (not used) */
ROM_END

ROM_START( srdmissn )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "5.t2",   0x0000, 0x4000, CRC(a682b48c) SHA1(c7348cbe42e45cd336e0d03052e839781d1481d1) )
	ROM_LOAD( "7.t3",   0x4000, 0x4000, CRC(1719c58c) SHA1(32faae584d0ada0a39b96655b1a9d7c449af4996) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "1.t7",   0x0000, 0x4000, CRC(dc48595e) SHA1(cbf357db4555ba57eb0a696bf1ecfcfeb9871409) )
	ROM_LOAD( "3.t8",   0x4000, 0x4000, CRC(216be1e8) SHA1(90016f74ebad1aafb9bd086e28ca3d400839b0a2) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "15.4a",  0x00000, 0x1000, CRC(4961f7fd) SHA1(8b08b9df0c3f71ceffd9b17a364aba611c05c774) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "17.9h",  0x00000, 0x2000, CRC(41211458) SHA1(bb080f4d7b92a1dadb7e554fd640e14ddbbcfcb6) ) /* tiles - plane 1 */
	ROM_LOAD( "18.10h", 0x02000, 0x2000, CRC(740eccd4) SHA1(a1ce4b1a9e7c26a7322b7cdd6c734889900485be) ) /* tiles - plane 0 */
	ROM_LOAD( "16.11h", 0x04000, 0x2000, CRC(c1f4a5db) SHA1(b9a0e57fac6317dceec3d4fbcde25b6b883fbbb1) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "14.6a",  0x00000, 0x4000, CRC(3d4c0447) SHA1(a064d43f5e8e0ac6ce441148057623f4a32f6056) ) /* sprites - plane 0 */
	ROM_LOAD( "13.7a",  0x04000, 0x4000, CRC(22414a67) SHA1(aaeb1bd196967d201d0dfc06de88419d6651f788) ) /* sprites - plane 0 */
	ROM_LOAD( "12.8a",  0x08000, 0x4000, CRC(61e34283) SHA1(35fff04aae4d5ab3269c1cd2c306c21507dde073) ) /* sprites - plane 1 */
	ROM_LOAD( "11.9a",  0x0c000, 0x4000, CRC(bbbaffef) SHA1(f2de9eebd9f566a4aea9a30bfd4d0a7643ea320c) ) /* sprites - plane 1 */
	ROM_LOAD( "10.10a", 0x10000, 0x4000, CRC(de564f97) SHA1(fa5e9a807ef170f01df9c1d7c9124e418a29c3bb) ) /* sprites - plane 2 */
	ROM_LOAD( "9.11a",  0x14000, 0x4000, CRC(890dc815) SHA1(61a1f7a3ff9bde31e0f7427cea115a4c140798af) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "mr.1j",  0x0000, 0x0100, CRC(110a436e) SHA1(1559a3bfc80c0e13f94dc929171af12010e1de28) ) /* red */
	ROM_LOAD( "mg.1h",  0x0100, 0x0100, CRC(0fbfd9f0) SHA1(eb69f076c2f12f17238c5189645da54a85bbc2be) ) /* green */
	ROM_LOAD( "mb.1f",  0x0200, 0x0100, CRC(a342890c) SHA1(6f2223ed68392b15a8751dba9bb28b85fd1d8dc0) ) /* blue */
	ROM_LOAD( "m2.5j",  0x0300, 0x0020, CRC(190a55ad) SHA1(de8a847bff8c343d69b853a215e6ee775ef2ef96) ) /* char lookup table */
	ROM_LOAD( "m1.2c",  0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? not used */
ROM_END

ROM_START( fx )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "fx.01", 0x0000, 0x4000, CRC(b651754b) SHA1(05024276aeac0c2a3d62f3a6f1027518fe206784) )
	ROM_LOAD( "fx.02", 0x4000, 0x4000, CRC(f3d2dcc1) SHA1(466bed28ecf25f9e2653662d7cc382ceb916d8db) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "fx.03", 0x0000, 0x4000, CRC(8907df6b) SHA1(a6d3e804fdaaeddca6f6f1e29dc35f25ed9490ae) )
	ROM_LOAD( "fx.04", 0x4000, 0x4000, CRC(c665834f) SHA1(91ffb32790df324b5f47f8b88f47a1b6ec689cce) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "fx.05", 0x0000, 0x1000, CRC(4a504286) SHA1(0d5ca6ce24ebaad466235fdb22471ac1ad7703db) )

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "17.9h",  0x00000, 0x2000, CRC(41211458) SHA1(bb080f4d7b92a1dadb7e554fd640e14ddbbcfcb6) ) /* tiles - plane 1 */
	ROM_LOAD( "18.10h", 0x02000, 0x2000, CRC(740eccd4) SHA1(a1ce4b1a9e7c26a7322b7cdd6c734889900485be) ) /* tiles - plane 0 */
	ROM_LOAD( "16.11h", 0x04000, 0x2000, CRC(c1f4a5db) SHA1(b9a0e57fac6317dceec3d4fbcde25b6b883fbbb1) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "14.6a",  0x00000, 0x4000, CRC(3d4c0447) SHA1(a064d43f5e8e0ac6ce441148057623f4a32f6056) ) /* sprites - plane 0 */
	ROM_LOAD( "13.7a",  0x04000, 0x4000, CRC(22414a67) SHA1(aaeb1bd196967d201d0dfc06de88419d6651f788) ) /* sprites - plane 0 */
	ROM_LOAD( "12.8a",  0x08000, 0x4000, CRC(61e34283) SHA1(35fff04aae4d5ab3269c1cd2c306c21507dde073) ) /* sprites - plane 1 */
	ROM_LOAD( "11.9a",  0x0c000, 0x4000, CRC(bbbaffef) SHA1(f2de9eebd9f566a4aea9a30bfd4d0a7643ea320c) ) /* sprites - plane 1 */
	ROM_LOAD( "10.10a", 0x10000, 0x4000, CRC(de564f97) SHA1(fa5e9a807ef170f01df9c1d7c9124e418a29c3bb) ) /* sprites - plane 2 */
	ROM_LOAD( "9.11a",  0x14000, 0x4000, CRC(890dc815) SHA1(61a1f7a3ff9bde31e0f7427cea115a4c140798af) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "mr.1j",  0x0000, 0x0100, CRC(110a436e) SHA1(1559a3bfc80c0e13f94dc929171af12010e1de28) ) /* red */
	ROM_LOAD( "mg.1h",  0x0100, 0x0100, CRC(0fbfd9f0) SHA1(eb69f076c2f12f17238c5189645da54a85bbc2be) ) /* green */
	ROM_LOAD( "mb.1f",  0x0200, 0x0100, CRC(a342890c) SHA1(6f2223ed68392b15a8751dba9bb28b85fd1d8dc0) ) /* blue */
	ROM_LOAD( "m2.5j",  0x0300, 0x0020, CRC(190a55ad) SHA1(de8a847bff8c343d69b853a215e6ee775ef2ef96) ) /* char lookup table */
	ROM_LOAD( "m1.2c",  0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? not used */
ROM_END


ROM_START( airwolf )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "b.2s",        0x0000, 0x8000, CRC(8c993cce) SHA1(925a5a9a2ee382556e2c2e928fd483344eba72c3) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "a.7s",        0x0000, 0x8000, CRC(a3c7af5c) SHA1(0f70ca94f3d168d38e0e93252e9441973f72441a) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "f.4a",        0x00000, 0x1000, CRC(4df44ce9) SHA1(145986009d4ae6f7dd98ce715838d0331dea005d) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "09h_14.bin",  0x00000, 0x2000, CRC(25e57e1f) SHA1(bef24bced102cd470e10bd4aa19da3c608211258) ) /* tiles - plane 1 */
	ROM_LOAD( "10h_13.bin",  0x02000, 0x2000, CRC(cf0de5e9) SHA1(32f3eb4c9298d59aca1dc2530b0e92f64311946d) ) /* tiles - plane 0 */
	ROM_LOAD( "11h_12.bin",  0x04000, 0x2000, CRC(4050c048) SHA1(ca21e0750f01342d9791067160339eec436c9458) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "e.6a",        0x00000, 0x2000, CRC(e8fbc7d2) SHA1(a9a651b4a714f490a051a89fd0327a665353d64b) ) /* sprites - plane 0 */
	ROM_CONTINUE(            0x04000, 0x2000 )
	ROM_CONTINUE(            0x02000, 0x2000 )
	ROM_CONTINUE(            0x06000, 0x2000 )
	ROM_LOAD( "d.8a",        0x08000, 0x2000, CRC(c5d4156b) SHA1(f66ec33b67e39f3df016231b00e48c9757e322f3) ) /* sprites - plane 1 */
	ROM_CONTINUE(            0x0c000, 0x2000 )
	ROM_CONTINUE(            0x0a000, 0x2000 )
	ROM_CONTINUE(            0x0e000, 0x2000 )
	ROM_LOAD( "c.10a",       0x10000, 0x2000, CRC(de91dfb1) SHA1(9f338542f44905d0b895d99510475113eb860f0d) ) /* sprites - plane 2 */
	ROM_CONTINUE(            0x14000, 0x2000 )
	ROM_CONTINUE(            0x12000, 0x2000 )
	ROM_CONTINUE(            0x16000, 0x2000 )

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "01j.bin",     0x0000, 0x0100, CRC(6a94b2a3) SHA1(b1f9bd97aa26c9fb6377ef32d5dd125583361f48) ) /* red */
	ROM_LOAD( "01h.bin",     0x0100, 0x0100, CRC(ec0923d3) SHA1(26f9eda4260a8b767893b8dea42819f192ef0b20) ) /* green */
	ROM_LOAD( "01f.bin",     0x0200, 0x0100, CRC(ade97052) SHA1(cc1b4cd57d7bc55ce44de6b89a322ff08eabb1a0) ) /* blue */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",       0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? not used */
ROM_END

ROM_START( skywolf )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "02s_03.bin",  0x0000, 0x4000, CRC(a0891798) SHA1(c1b1e1fce529509fb1dd921a0022d5367c3c495c) )
	ROM_LOAD( "03s_04.bin",  0x4000, 0x4000, CRC(5f515d46) SHA1(ec12bddf72e98aeef5cd17d00f0fa6f2df59cf00) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "07s_01.bin",  0x0000, 0x4000, CRC(c680a905) SHA1(9a65bade18d0340d428ee12d2e505def2339e3c3) )
	ROM_LOAD( "08s_02.bin",  0x4000, 0x4000, CRC(3d66bf26) SHA1(10a9f9888c1da12e2ba71748b8608b18e46e8db6) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "04a_11.bin",  0x00000, 0x1000, CRC(219de9aa) SHA1(7f79b718427310f8725b64cb1953879d7277b212) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "09h_14.bin",  0x00000, 0x2000, CRC(25e57e1f) SHA1(bef24bced102cd470e10bd4aa19da3c608211258) ) /* tiles - plane 1 */
	ROM_LOAD( "10h_13.bin",  0x02000, 0x2000, CRC(cf0de5e9) SHA1(32f3eb4c9298d59aca1dc2530b0e92f64311946d) ) /* tiles - plane 0 */
	ROM_LOAD( "11h_12.bin",  0x04000, 0x2000, CRC(4050c048) SHA1(ca21e0750f01342d9791067160339eec436c9458) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "06a_10.bin",  0x00000, 0x4000, CRC(1c809383) SHA1(955aed87a8e9ae6ef7f0daa27a0cc4d85493ba90) ) /* sprites - plane 0 */
	ROM_LOAD( "07a_09.bin",  0x04000, 0x4000, CRC(5665d774) SHA1(cd4359d2f970e2b9d09f5ddadcddf8e77caea6e9) ) /* sprites - plane 0 */
	ROM_LOAD( "08a_08.bin",  0x08000, 0x4000, CRC(6dda8f2a) SHA1(db9fc81094fa8384da89f8f3349f09fc8e4f3c92) ) /* sprites - plane 1 */
	ROM_LOAD( "09a_07.bin",  0x0c000, 0x4000, CRC(6a21ddb8) SHA1(f47b952f513ebe7202b219bfe29f20368f40dc70) ) /* sprites - plane 1 */
	ROM_LOAD( "10a_06.bin",  0x10000, 0x4000, CRC(f2e548e0) SHA1(82b6a86eaa6dfbc4547a1e1009929fede7ba9f61) ) /* sprites - plane 2 */
	ROM_LOAD( "11a_05.bin",  0x14000, 0x4000, CRC(8681b112) SHA1(2d6f580dcc0b5c2803c20cece01a896d41e2c8b6) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "01j.bin",     0x0000, 0x0100, CRC(6a94b2a3) SHA1(b1f9bd97aa26c9fb6377ef32d5dd125583361f48) ) /* red */
	ROM_LOAD( "01h.bin",     0x0100, 0x0100, CRC(ec0923d3) SHA1(26f9eda4260a8b767893b8dea42819f192ef0b20) ) /* green */
	ROM_LOAD( "01f.bin",     0x0200, 0x0100, CRC(ade97052) SHA1(cc1b4cd57d7bc55ce44de6b89a322ff08eabb1a0) ) /* blue */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",       0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? not used */
ROM_END

ROM_START( skywolf2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* 64k for code */
	ROM_LOAD( "z80_2.bin",   0x0000, 0x8000, CRC(34db7bda) SHA1(1a98d5cf97063453a0351f7dbe339c32d59a3d20) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for code */
	ROM_LOAD( "07s_01.bin",  0x0000, 0x4000, CRC(c680a905) SHA1(9a65bade18d0340d428ee12d2e505def2339e3c3) )
	ROM_LOAD( "08s_02.bin",  0x4000, 0x4000, CRC(3d66bf26) SHA1(10a9f9888c1da12e2ba71748b8608b18e46e8db6) )

	ROM_REGION( 0x01000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "04a_11.bin",  0x00000, 0x1000, CRC(219de9aa) SHA1(7f79b718427310f8725b64cb1953879d7277b212) ) /* chars */

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "09h_14.bin",  0x00000, 0x2000, CRC(25e57e1f) SHA1(bef24bced102cd470e10bd4aa19da3c608211258) ) /* tiles - plane 1 */
	ROM_LOAD( "10h_13.bin",  0x02000, 0x2000, CRC(cf0de5e9) SHA1(32f3eb4c9298d59aca1dc2530b0e92f64311946d) ) /* tiles - plane 0 */
	ROM_LOAD( "11h_12.bin",  0x04000, 0x2000, CRC(4050c048) SHA1(ca21e0750f01342d9791067160339eec436c9458) ) /* tiles - plane 2 */

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "06a_10.bin",  0x00000, 0x4000, CRC(1c809383) SHA1(955aed87a8e9ae6ef7f0daa27a0cc4d85493ba90) ) /* sprites - plane 0 */
	ROM_LOAD( "07a_09.bin",  0x04000, 0x4000, CRC(5665d774) SHA1(cd4359d2f970e2b9d09f5ddadcddf8e77caea6e9) ) /* sprites - plane 0 */
	ROM_LOAD( "08a_08.bin",  0x08000, 0x4000, CRC(6dda8f2a) SHA1(db9fc81094fa8384da89f8f3349f09fc8e4f3c92) ) /* sprites - plane 1 */
	ROM_LOAD( "09a_07.bin",  0x0c000, 0x4000, CRC(6a21ddb8) SHA1(f47b952f513ebe7202b219bfe29f20368f40dc70) ) /* sprites - plane 1 */
	ROM_LOAD( "10a_06.bin",  0x10000, 0x4000, CRC(f2e548e0) SHA1(82b6a86eaa6dfbc4547a1e1009929fede7ba9f61) ) /* sprites - plane 2 */
	ROM_LOAD( "11a_05.bin",  0x14000, 0x4000, CRC(8681b112) SHA1(2d6f580dcc0b5c2803c20cece01a896d41e2c8b6) ) /* sprites - plane 2 */

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "01j.bin",     0x0000, 0x0100, CRC(6a94b2a3) SHA1(b1f9bd97aa26c9fb6377ef32d5dd125583361f48) ) /* red */
	ROM_LOAD( "01h.bin",     0x0100, 0x0100, CRC(ec0923d3) SHA1(26f9eda4260a8b767893b8dea42819f192ef0b20) ) /* green */
	ROM_LOAD( "01f.bin",     0x0200, 0x0100, CRC(ade97052) SHA1(cc1b4cd57d7bc55ce44de6b89a322ff08eabb1a0) ) /* blue */
	/* 0x0300-0x031f empty - looks like there isn't a lookup table PROM */
	ROM_LOAD( "m1.2c",       0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? not used */
ROM_END

ROM_START( legend )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "a_r2.rom",    0x0000, 0x4000, CRC(0cc1c4f4) SHA1(33f6a1b31eed75a92e06cb29f912321fe75c31e6) )
	ROM_LOAD( "a_r3.rom",    0x4000, 0x4000, CRC(4b270c6b) SHA1(95ad79a9de037b6aaca325da75c8aef9a72dbfed) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "a_r7.rom",    0x0000, 0x2000, CRC(abfe5eb4) SHA1(fbeb5ee14aaebb6321fe97fe523f08833fad9c7c) )
	ROM_LOAD( "a_r8.rom",    0x2000, 0x2000, CRC(7e7b9ba9) SHA1(897779129108b0f3936234ea797d47cf46cb7a16) )
	ROM_LOAD( "a_r9.rom",    0x4000, 0x2000, CRC(66737f1e) SHA1(5eac6606ed3a11a00eb1172e35487b0d95b1d739) )
	ROM_LOAD( "a_n7.rom",    0x6000, 0x2000, CRC(13915a53) SHA1(25ba3babc8eb0df413bdfe7dbcd8642e4c658120) )

	ROM_REGION(  0x1000, REGION_GFX1, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "b_a4.rom",    0x0000, 0x1000, CRC(c7dd3cf7) SHA1(87f31c639d840e781d0f56f98f00d0642c6b87b4) )

	ROM_REGION(  0x6000, REGION_GFX2, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "b_h9.rom",    0x0000, 0x2000, CRC(1fe8644a) SHA1(42f5b93ffb3277321969a0bd805ec78796583d37) )
	ROM_LOAD( "b_h10.rom",   0x2000, 0x2000, CRC(5f7dc82e) SHA1(3e70be650b8046c2b34a2405a2fbc2a16bf73bf1) )
	ROM_LOAD( "b_h11.rom",   0x4000, 0x2000, CRC(46741643) SHA1(3fba31bac5a7d94af80035304647f39af3a9484f) )

	ROM_REGION( 0x18000, REGION_GFX3, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "b_a6.rom",   0x00000, 0x4000, CRC(1689f21c) SHA1(fafb13dc8ca27a7506065bbf08102afc6d722843) )
	ROM_LOAD( "b_a7.rom",   0x04000, 0x4000, CRC(f527c909) SHA1(40f44828502018c73283965eb7a2a68ed25ebfe5) )
	ROM_LOAD( "b_a8.rom",   0x08000, 0x4000, CRC(8d618629) SHA1(3cc49fd8066464ee940de010da3f33ed8573df3d) )
	ROM_LOAD( "b_a9.rom",   0x0c000, 0x4000, CRC(7d7e2d55) SHA1(efd4817a0f5e14cb5a3d0c1f69e6ad408a813202) )
	ROM_LOAD( "b_a10.rom",  0x10000, 0x4000, CRC(f12232fe) SHA1(2d8accc10f0703eeb075c4053d4165b90552e6a7) )
	ROM_LOAD( "b_a11.rom",  0x14000, 0x4000, CRC(8c09243d) SHA1(8f0f68921f8ab6c016b7481714febb68abb7ce79) )

	ROM_REGION( 0x0340, REGION_PROMS, 0 )
	ROM_LOAD( "82s129.1j",   0x0000, 0x0100, CRC(40590ac0) SHA1(30a8e24e34c4ee0a7df91c0633becfce1c8d856c) ) /* red */
	ROM_LOAD( "82s129.1h",   0x0100, 0x0100, CRC(e542b363) SHA1(6775209b9a4aaf374878c06cf4dc693b921abd87) ) /* green */
	ROM_LOAD( "82s129.1f",   0x0200, 0x0100, CRC(75536fc8) SHA1(e713efafcdc7f2a595444af75d2083eb3e38a480) ) /* blue */
	ROM_LOAD( "82s123.5j",   0x0300, 0x0020, CRC(c98f0651) SHA1(4f1b95213c28ad017c8d6542e8d522e4d69f91e3) ) /* char lookup table */
	ROM_LOAD( "m1.2c",       0x0320, 0x0020, CRC(83a39201) SHA1(4fdc722c9e20ee152c890342ef0dce18e35e2ef8) ) /* timing? not used */
ROM_END


/*************************************
 *
 *  Game specific initialization
 *
 *************************************/

static DRIVER_INIT( gyrodine )
{
	/* add watchdog */
	memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0xe000, 0xe000, 0, 0, watchdog_reset_w);
}


static DRIVER_INIT( srdmissn )
{
	/* shared RAM is mapped at 0xe000 as well  */
	memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0xe000, 0xe7ff, 0, 0, MRA8_BANK1);
	memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0xe000, 0xe7ff, 0, 0, MWA8_BANK1);
	memory_set_bankptr(1, shared_ram);

	/* extra RAM on sub CPU  */
	memory_install_read8_handler(1, ADDRESS_SPACE_PROGRAM, 0x8800, 0x8fff, 0, 0, MRA8_BANK2);
	memory_install_write8_handler(1, ADDRESS_SPACE_PROGRAM, 0x8800, 0x8fff, 0, 0, MWA8_BANK2);
	memory_set_bankptr(2, auto_malloc(0x800));
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1984, gyrodine, 0,        gyrodine, gyrodine, gyrodine, ROT90, "Taito Corporation", "Gyrodine", 0 ,0)
GAME( 1984, gyrodinc, gyrodine, gyrodine, gyrodine, gyrodine, ROT90, "Taito Corporation (Crux license)", "Gyrodine (Crux)", 0 ,0)
GAME( 1985, sonofphx, 0,        sonofphx, sonofphx, 0,        ROT90, "Associated Overseas MFR, Inc", "Son of Phoenix", 0 ,0)
GAME( 1985, repulse,  sonofphx, sonofphx, sonofphx, 0,        ROT90, "Sega", "Repulse", 0 ,0)
GAME( 1985, 99lstwar, sonofphx, sonofphx, sonofphx, 0,        ROT90, "Proma", "'99: The Last War", 0 ,0)
GAME( 1985, 99lstwra, sonofphx, sonofphx, sonofphx, 0,        ROT90, "Proma", "'99: The Last War (alternate)", 0 ,0)
GAME( 1985, 99lstwrk, sonofphx, sonofphx, sonofphx, 0,        ROT90, "Kyugo", "'99: The Last War (Kyugo)", 0 ,0)
GAME( 1985, flashgal, 0,        flashgal, flashgal, 0,        ROT0,  "Sega", "Flashgal (set 1)", 0 ,0)
GAME( 1985, flashgla, flashgal, flashgla, flashgal, 0,        ROT0,  "Sega", "Flashgal (set 2)", 0 ,0)
GAME( 1986, srdmissn, 0,        srdmissn, srdmissn, srdmissn, ROT90, "Taito Corporation", "S.R.D. Mission", 0 ,0)
GAME( 1986, fx,       srdmissn, srdmissn, srdmissn, srdmissn, ROT90, "bootleg", "F-X", 0 ,0)
GAME( 1986?,legend,   0,        legend,   legend,   srdmissn, ROT0,  "Sega / Coreland ?", "Legend", 0 ,0)
GAME( 1987, airwolf,  0,        srdmissn, airwolf,  srdmissn, ROT0,  "Kyugo", "Airwolf", 0 ,2)
GAME( 1987, skywolf,  airwolf,  srdmissn, skywolf,  srdmissn, ROT0,  "bootleg", "Sky Wolf (set 1)", 0 ,2)
GAME( 1987, skywolf2, airwolf,  srdmissn, airwolf,  srdmissn, ROT0,  "bootleg", "Sky Wolf (set 2)", 0 ,2)
