/****************************************************************************

    Preliminary driver for Samurai, Nunchackun, Yuke Yuke Yamaguchi-kun
    (c) Taito 1985

    Known Issues:
    - some color problems (need screenshots)
    - Nunchackun has wrong colors; sprites look better if you subtract sprite color from 0x2d
    - Yuke Yuke Yamaguchi-kun isn't playable (sprite problem only?)

driver by Phil Stroffolino

Mission 660 extensions by Paul Swan (swan@easynet.co.uk)
--------------------------------------------------------
The game appears to use the same video board as Samurai et al. There is a
character column scroll feature that I have added. Its used to scroll in
the "660" logo on the title screen at the beginning. Not sure if Samurai
at al use it but it's likely their boards have the feature. Extra banking
of the forground is done using an extra register. A bit in the background
video RAM selects the additional background character space.

The sound board is similar. There are 3 CPU's instead of the 2 of Samurai.
2 are still used for sample-like playback (as Samurai) and the other is used
to drive the AY-3-8910 (that was driven directly by the video CPU on Samurai).
The memory maps are different over Samurai, probably to allow larger capacity
ROMS though only the AY driver uses 27256 on Mission 660. A picture of the board
I have suggests that the AY CPU could have a DAC as well but the circuit is not
populated on Mission 660.

There is some kind of protection. There is code in there to do the same "unknown"
reads and writes as Samurai and the original M660 won't run with the Samurai value.
The bootleg M660 has the protection code patched out to use a fixed value. I've
used this same value on the original M660 and it seems to work.

I'm guessing the bootleg is of a "world" release and the original is from
the "America" release.

TODO:
1) Colours.
2) A few unknown regs.

****************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"
#include "sound/dac.h"

WRITE8_HANDLER( vsgongf_color_w );

WRITE8_HANDLER( tsamurai_bgcolor_w );
WRITE8_HANDLER( tsamurai_textbank1_w );
WRITE8_HANDLER( tsamurai_textbank2_w );

WRITE8_HANDLER( tsamurai_scrolly_w );
WRITE8_HANDLER( tsamurai_scrollx_w );
VIDEO_UPDATE( tsamurai );
WRITE8_HANDLER( tsamurai_bg_videoram_w );
WRITE8_HANDLER( tsamurai_fg_videoram_w );
WRITE8_HANDLER( tsamurai_fg_colorram_w );
extern VIDEO_START( tsamurai );
extern unsigned char *tsamurai_videoram;

extern VIDEO_START( vsgongf );
extern VIDEO_UPDATE( vsgongf );

static int nmi_enabled;
static int sound_command1, sound_command2, sound_command3;

static WRITE8_HANDLER( nmi_enable_w ){
	nmi_enabled = data;
}

static INTERRUPT_GEN( samurai_interrupt ){
	if (nmi_enabled) cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
}

READ8_HANDLER( unknown_d803_r )
{
	return 0x6b;
}

READ8_HANDLER( unknown_d803_m660_r )
{
	return 0x53;     // this is what the bootleg patches in.
}

READ8_HANDLER( unknown_d806_r )
{
	return 0x40;
}

READ8_HANDLER( unknown_d900_r )
{
	return 0x6a;
}

READ8_HANDLER( unknown_d938_r )
{
	return 0xfb;
}

static WRITE8_HANDLER( sound_command1_w )
{
	sound_command1 = data;
	cpunum_set_input_line( 1, 0, HOLD_LINE );
}

static WRITE8_HANDLER( sound_command2_w )
{
	sound_command2 = data;
	cpunum_set_input_line( 2, 0, HOLD_LINE );
}

static WRITE8_HANDLER( sound_command3_w )
{
	sound_command3 = data;
	cpunum_set_input_line( 3, 0, HOLD_LINE );
}

static WRITE8_HANDLER( flip_screen_w )
{
	flip_screen_set(data);
}

static WRITE8_HANDLER( tsamurai_coin_counter_w )
{
	coin_counter_w(offset,data);
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_READ(MRA8_RAM)

	/* protection? - there are writes as well...*/
	AM_RANGE(0xd803, 0xd803) AM_READ(unknown_d803_r)
	AM_RANGE(0xd806, 0xd806) AM_READ(unknown_d806_r)
	AM_RANGE(0xd900, 0xd900) AM_READ(unknown_d900_r)
	AM_RANGE(0xd938, 0xd938) AM_READ(unknown_d938_r)

	AM_RANGE(0xe000, 0xe3ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe400, 0xe7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe800, 0xefff) AM_READ(MRA8_RAM)
	AM_RANGE(0xf000, 0xf3ff) AM_READ(MRA8_RAM)

	AM_RANGE(0xf800, 0xf800) AM_READ(input_port_0_r)
	AM_RANGE(0xf801, 0xf801) AM_READ(input_port_1_r)
	AM_RANGE(0xf802, 0xf802) AM_READ(input_port_2_r)
	AM_RANGE(0xf804, 0xf804) AM_READ(input_port_3_r)
	AM_RANGE(0xf805, 0xf805) AM_READ(input_port_4_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_WRITE(MWA8_RAM)

	AM_RANGE(0xe000, 0xe3ff) AM_WRITE(tsamurai_fg_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xe400, 0xe43f) AM_WRITE(tsamurai_fg_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe440, 0xe7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe800, 0xefff) AM_WRITE(tsamurai_bg_videoram_w) AM_BASE(&tsamurai_videoram)
	AM_RANGE(0xf000, 0xf3ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram)

	AM_RANGE(0xf400, 0xf400) AM_WRITE(MWA8_NOP)
	AM_RANGE(0xf401, 0xf401) AM_WRITE(sound_command1_w)
	AM_RANGE(0xf402, 0xf402) AM_WRITE(sound_command2_w)

	AM_RANGE(0xf801, 0xf801) AM_WRITE(tsamurai_bgcolor_w)
	AM_RANGE(0xf802, 0xf802) AM_WRITE(tsamurai_scrolly_w)
	AM_RANGE(0xf803, 0xf803) AM_WRITE(tsamurai_scrollx_w)

	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(flip_screen_w)
	AM_RANGE(0xfc01, 0xfc01) AM_WRITE(nmi_enable_w)
	AM_RANGE(0xfc02, 0xfc02) AM_WRITE(tsamurai_textbank1_w)
	AM_RANGE(0xfc03, 0xfc04) AM_WRITE(tsamurai_coin_counter_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readmem_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_READ(MRA8_RAM)

	/* protection? - there are writes as well...*/
	AM_RANGE(0xd803, 0xd803) AM_READ(unknown_d803_m660_r)
	AM_RANGE(0xd806, 0xd806) AM_READ(unknown_d806_r)
	AM_RANGE(0xd900, 0xd900) AM_READ(unknown_d900_r)
	AM_RANGE(0xd938, 0xd938) AM_READ(unknown_d938_r)

	AM_RANGE(0xe000, 0xe3ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe400, 0xe7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe800, 0xefff) AM_READ(MRA8_RAM)
	AM_RANGE(0xf000, 0xf3ff) AM_READ(MRA8_RAM)

	AM_RANGE(0xf800, 0xf800) AM_READ(input_port_0_r)
	AM_RANGE(0xf801, 0xf801) AM_READ(input_port_1_r)
	AM_RANGE(0xf802, 0xf802) AM_READ(input_port_2_r)
	AM_RANGE(0xf804, 0xf804) AM_READ(input_port_3_r)
	AM_RANGE(0xf805, 0xf805) AM_READ(input_port_4_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_WRITE(MWA8_RAM)

	AM_RANGE(0xe000, 0xe3ff) AM_WRITE(tsamurai_fg_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xe400, 0xe43f) AM_WRITE(tsamurai_fg_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe440, 0xe7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe800, 0xefff) AM_WRITE(tsamurai_bg_videoram_w) AM_BASE(&tsamurai_videoram)
	AM_RANGE(0xf000, 0xf3ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram)

	AM_RANGE(0xf400, 0xf400) AM_WRITE(MWA8_NOP)/* This is always written with F401, F402 & F403 data */
	AM_RANGE(0xf401, 0xf401) AM_WRITE(sound_command3_w)
	AM_RANGE(0xf402, 0xf402) AM_WRITE(sound_command2_w)
	AM_RANGE(0xf403, 0xf403) AM_WRITE(sound_command1_w)


	AM_RANGE(0xf801, 0xf801) AM_WRITE(tsamurai_bgcolor_w)
	AM_RANGE(0xf802, 0xf802) AM_WRITE(tsamurai_scrolly_w)
	AM_RANGE(0xf803, 0xf803) AM_WRITE(tsamurai_scrollx_w)

	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(flip_screen_w)
	AM_RANGE(0xfc01, 0xfc01) AM_WRITE(nmi_enable_w)
	AM_RANGE(0xfc02, 0xfc02) AM_WRITE(tsamurai_textbank1_w)
	AM_RANGE(0xfc03, 0xfc04) AM_WRITE(tsamurai_coin_counter_w)
	AM_RANGE(0xfc07, 0xfc07) AM_WRITE(tsamurai_textbank2_w)/* Mission 660 uses a bit here */
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_writeport_m660, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(MWA8_NOP)		       /* ? */
	AM_RANGE(0x01, 0x01) AM_WRITE(MWA8_NOP)               /* Written continuously. Increments with level. */
	AM_RANGE(0x02, 0x02) AM_WRITE(MWA8_NOP)               /* Always follows above with 0x01 data */
ADDRESS_MAP_END

static READ8_HANDLER( sound_command1_r )
{
	return sound_command1;
}

static WRITE8_HANDLER( sound_out1_w )
{
	DAC_data_w(0,data);
}

static READ8_HANDLER( sound_command2_r ){
	return sound_command2;
}

static WRITE8_HANDLER( sound_out2_w )
{
	DAC_data_w(1,data);
}

static READ8_HANDLER( sound_command3_r ){
	return sound_command3;
}

/*******************************************************************************/
static ADDRESS_MAP_START( readmem_sound1, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x6000, 0x6000) AM_READ(sound_command1_r)
	AM_RANGE(0x7f00, 0x7fff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound1, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x6001, 0x6001) AM_WRITE(MWA8_NOP) /* ? - probably clear IRQ */
	AM_RANGE(0x6002, 0x6002) AM_WRITE(sound_out1_w)
	AM_RANGE(0x7f00, 0x7fff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

/*******************************************************************************/

static ADDRESS_MAP_START( readmem_sound2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x6000, 0x6000) AM_READ(sound_command2_r)
	AM_RANGE(0x7f00, 0x7fff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x6001, 0x6001) AM_WRITE(MWA8_NOP) /* ? - probably clear IRQ */
	AM_RANGE(0x6002, 0x6002) AM_WRITE(sound_out2_w)
	AM_RANGE(0x7f00, 0x7fff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

/*******************************************************************************/

static ADDRESS_MAP_START( readmem_sound1_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc000) AM_READ(sound_command1_r)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound1_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc001, 0xc001) AM_WRITE(MWA8_NOP) /* ? - probably clear IRQ */
	AM_RANGE(0xc002, 0xc002) AM_WRITE(sound_out1_w)
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

/*******************************************************************************/

static ADDRESS_MAP_START( readmem_sound2_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc000) AM_READ(sound_command2_r)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound2_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc001, 0xc001) AM_WRITE(MWA8_NOP) /* ? - probably clear IRQ */
	AM_RANGE(0xc002, 0xc002) AM_WRITE(sound_out2_w)
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

/*******************************************************************************/

static ADDRESS_MAP_START( readport_sound3_m660, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport_sound3_m660, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readmem_sound3_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc000) AM_READ(sound_command3_r)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound3_m660, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc001, 0xc001) AM_WRITE(MWA8_NOP) /* ? - probably clear IRQ */
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

/*******************************************************************************/

static int vsgongf_sound_nmi_enabled;
static WRITE8_HANDLER( vsgongf_sound_nmi_enable_w ){
	vsgongf_sound_nmi_enabled = data;
}

static INTERRUPT_GEN( vsgongf_sound_interrupt ){
	if (vsgongf_sound_nmi_enabled) cpunum_set_input_line(1, INPUT_LINE_NMI, PULSE_LINE);
}

/* what are these, protection of some kind? */

static READ8_HANDLER( vsgongf_a006_r ){
	/* sound CPU busy? */
	if (!strcmp(Machine->gamedrv->name,"vsgongf"))  return 0x80;
	if (!strcmp(Machine->gamedrv->name,"ringfgt"))  return 0x80;
	if (!strcmp(Machine->gamedrv->name,"ringfgt2")) return 0xc0;

	loginfo (2, "unhandled read from a006\n");
	return 0x00;
}

static READ8_HANDLER( vsgongf_a100_r ){
	/* protection? */
	if (!strcmp(Machine->gamedrv->name,"vsgongf"))  return 0xaa;
	if (!strcmp(Machine->gamedrv->name,"ringfgt"))  return 0x63;
	if (!strcmp(Machine->gamedrv->name,"ringfgt2")) return 0x6a;

	loginfo (2, "unhandled read from a100\n");
	return 0x00;
}

static WRITE8_HANDLER( vsgongf_sound_command_w ){
	soundlatch_w( offset, data );
	cpunum_set_input_line( 1, INPUT_LINE_NMI, PULSE_LINE );
}

static ADDRESS_MAP_START( readmem_vsgongf, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xa003, 0xa003) AM_READ(MRA8_RAM)
	AM_RANGE(0xa006, 0xa006) AM_READ(vsgongf_a006_r) /* protection */
	AM_RANGE(0xa100, 0xa100) AM_READ(vsgongf_a100_r) /* protection */
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM) /* work ram */
	AM_RANGE(0xe000, 0xe7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xf800, 0xf800) AM_READ(input_port_0_r) /* joy1 */
	AM_RANGE(0xf801, 0xf801) AM_READ(input_port_1_r) /* joy2 */
	AM_RANGE(0xf802, 0xf802) AM_READ(input_port_2_r) /* coin */
	AM_RANGE(0xf804, 0xf804) AM_READ(input_port_3_r) /* dsw1 */
	AM_RANGE(0xf805, 0xf805) AM_READ(input_port_4_r) /* dsw2 */
	AM_RANGE(0xfc00, 0xffff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_vsgongf, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM) /* work ram */
	AM_RANGE(0xe000, 0xe3ff) AM_WRITE(tsamurai_fg_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xe400, 0xe43f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram)
	AM_RANGE(0xe440, 0xe47b) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe800, 0xe800) AM_WRITE(vsgongf_sound_command_w)
	AM_RANGE(0xec00, 0xec06) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xf000, 0xf000) AM_WRITE(vsgongf_color_w)
	AM_RANGE(0xf400, 0xf400) AM_WRITE(MWA8_RAM) /* vreg? always 0 */
	AM_RANGE(0xf800, 0xf800) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xf801, 0xf801) AM_WRITE(MWA8_RAM) /* vreg? always 0 */
	AM_RANGE(0xf803, 0xf803) AM_WRITE(MWA8_RAM) /* vreg? always 0 */
	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(MWA8_RAM) /* vreg? always 0 */
	AM_RANGE(0xfc01, 0xfc01) AM_WRITE(nmi_enable_w)
	AM_RANGE(0xfc02, 0xfc03) AM_WRITE(tsamurai_coin_counter_w)
	AM_RANGE(0xfc04, 0xfc04) AM_WRITE(tsamurai_textbank1_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readmem_sound_vsgongf, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x6000, 0x63ff) AM_READ(MRA8_RAM) /* work RAM */
	AM_RANGE(0x8000, 0x8000) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound_vsgongf, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x6000, 0x63ff) AM_WRITE(MWA8_RAM) /* work RAM */
	AM_RANGE(0x8000, 0x8000) AM_WRITE(vsgongf_sound_nmi_enable_w) /* NMI enable */
	AM_RANGE(0xa000, 0xa000) AM_WRITE(sound_out1_w)
ADDRESS_MAP_END

/*******************************************************************************/

#define TS_IN0\
	PORT_START_TAG("IN0")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 )\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TS_IN1\
	PORT_START_TAG("IN1")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TS_IN2\
	PORT_START_TAG("IN2")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE )\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TS_DSW1\
	PORT_START_TAG("DSW1")\
	PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coin_A ) )\
	PORT_DIPSETTING(    0x07, DEF_STR( 6C_1C ) )\
	PORT_DIPSETTING(    0x06, DEF_STR( 3C_1C ) )\
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )\
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_3C ) )\
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )\
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )\
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )\
	PORT_DIPNAME( 0x38, 0x00, DEF_STR( Coin_B ) )\
	PORT_DIPSETTING(    0x38, DEF_STR( 6C_1C ) )\
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_1C ) )\
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )\
	PORT_DIPSETTING(    0x28, DEF_STR( 2C_3C ) )\
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )\
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_3C ) )\
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_6C ) )\
	PORT_DIPNAME( 0x40, 0x00, "Freeze" )\
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )\
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )\
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Cabinet ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )\
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )

INPUT_PORTS_START( tsamurai )
TS_IN0
TS_IN1
TS_IN2
TS_DSW1

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x03, "254 (Cheat)")
	PORT_DIPNAME( 0x0c, 0x0c, "DSW2 Unknown 1" )
	PORT_DIPSETTING(    0x00, "00" )
	PORT_DIPSETTING(    0x04, "30" )
	PORT_DIPSETTING(    0x08, "50" )
	PORT_DIPSETTING(    0x0c, "70" )
	PORT_DIPNAME( 0x30, 0x30, "DSW2 Unknown 2" )
	PORT_DIPSETTING(    0x00, "0x00" )
	PORT_DIPSETTING(    0x10, "0x01" )
	PORT_DIPSETTING(    0x20, "0x02" )
	PORT_DIPSETTING(    0x30, "0x03" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DSW2 Unknown 3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( nunchaku )
TS_IN0
TS_IN1
TS_IN2
TS_DSW1

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x03, "255 (Cheat)")
	PORT_DIPNAME( 0x0c, 0x0c, "DSW2 Unknown 1" )
	PORT_DIPSETTING(    0x00, "00" )
	PORT_DIPSETTING(    0x04, "30" )
	PORT_DIPSETTING(    0x08, "50" )
	PORT_DIPSETTING(    0x0c, "70" )
	PORT_DIPNAME( 0x30, 0x30, "DSW2 Unknown 2" )
	PORT_DIPSETTING(    0x00, "0x00" )
	PORT_DIPSETTING(    0x10, "0x01" )
	PORT_DIPSETTING(    0x20, "0x02" )
	PORT_DIPSETTING(    0x30, "0x03" )
	PORT_DIPNAME( 0x40, 0x40, "DSW2 Unknown 3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DSW2 Unknown 4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( vsgongf )
TS_IN0
TS_IN1
TS_IN2
TS_DSW1

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

#define YAMINS\
	PORT_START_TAG("IN0")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2 )\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 )\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_START_TAG("IN1")\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

INPUT_PORTS_START( yamagchi )
	YAMINS
	TS_IN2
	TS_DSW1

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x03, "255 (Cheat)")
	PORT_DIPNAME( 0x0c, 0x0c, "DSW2 Unknown 1" )
	PORT_DIPSETTING(    0x00, "00" )
	PORT_DIPSETTING(    0x04, "30" )
	PORT_DIPSETTING(    0x08, "50" )
	PORT_DIPSETTING(    0x0c, "70" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x10, DEF_STR( English ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Japanese ) )
	PORT_DIPNAME( 0x20, 0x20, "DSW2 Unknown 2" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DSW2 Unknown 3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( m660 )
	YAMINS
	TS_IN2

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x38, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x40, 0x00, "Freeze" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Continues ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x0c, 0x0c, "Bonus" )
	PORT_DIPSETTING(    0x00, "10,30,50" )
	PORT_DIPSETTING(    0x04, "20,50,80" )
	PORT_DIPSETTING(    0x08, "30,70,110" )
	PORT_DIPSETTING(    0x0c, "50,100,150" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) ) /* listed as screen flip (hardware) */
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout char_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0,1,2,3, 4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};

static const gfx_layout sprite_layout =
{
	32,32,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{
		0,1,2,3,4,5,6,7,
		64+0,64+1,64+2,64+3,64+4,64+5,64+6,64+7,
		128+0,128+1,128+2,128+3,128+4,128+5,128+6,128+7,
		64*3+0,64*3+1,64*3+2,64*3+3,64*3+4,64*3+5,64*3+6,64*3+7
	},
	{
		0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8,
		1*256+0*8,1*256+1*8,1*256+2*8,1*256+3*8,1*256+4*8,1*256+5*8,1*256+6*8,1*256+7*8,
		2*256+0*8,2*256+1*8,2*256+2*8,2*256+3*8,2*256+4*8,2*256+5*8,2*256+6*8,2*256+7*8,
		3*256+0*8,3*256+1*8,3*256+2*8,3*256+3*8,3*256+4*8,3*256+5*8,3*256+6*8,3*256+7*8
	},
	4*256
};

static const gfx_layout tile_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tile_layout,   0, 32 },
	{ REGION_GFX2, 0, &char_layout,   0, 32 },
	{ REGION_GFX3, 0, &sprite_layout, 0, 32 },
	{ -1 }
};


/*******************************************************************************/

static MACHINE_DRIVER_START( tsamurai )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(z80_readport,z80_writeport)
	MDRV_CPU_VBLANK_INT(samurai_interrupt,1)

	MDRV_CPU_ADD(Z80, 2000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_sound1,writemem_sound1)

	MDRV_CPU_ADD(Z80, 2000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_sound2,writemem_sound2)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0, 255, 8, 255-8)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(tsamurai)
	MDRV_VIDEO_UPDATE(tsamurai)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 2000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.10)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( m660 )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(readmem_m660,writemem_m660)
	MDRV_CPU_IO_MAP(z80_readport,z80_writeport_m660)
	MDRV_CPU_VBLANK_INT(samurai_interrupt,1)

	MDRV_CPU_ADD(Z80, 2000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_sound1_m660,writemem_sound1_m660)

	MDRV_CPU_ADD(Z80, 2000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_sound2_m660,writemem_sound2_m660)

	MDRV_CPU_ADD(Z80, 2000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_sound3_m660,writemem_sound3_m660)
	MDRV_CPU_IO_MAP(readport_sound3_m660,writeport_sound3_m660)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0, 255, 8, 255-8)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(tsamurai)
	MDRV_VIDEO_UPDATE(tsamurai)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 2000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.10)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( vsgongf )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(readmem_vsgongf,writemem_vsgongf)
	MDRV_CPU_VBLANK_INT(samurai_interrupt,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(readmem_sound_vsgongf,writemem_sound_vsgongf)
	MDRV_CPU_IO_MAP(0,z80_writeport)
	MDRV_CPU_VBLANK_INT(vsgongf_sound_interrupt,3)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0, 255, 8, 255-8)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(vsgongf)
	MDRV_VIDEO_UPDATE(vsgongf)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 2000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.10)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)
MACHINE_DRIVER_END

/*******************************************************************************/

ROM_START( tsamurai )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "01.3r",      0x0000, 0x4000, CRC(d09c8609) SHA1(66b51897704250f520b4c58cb6f6f3aef8913459) )
	ROM_LOAD( "02.3t",      0x4000, 0x4000, CRC(d0f2221c) SHA1(6cfa9a52b35d17776cfa3e14e679b1a6218d54fa) )
	ROM_LOAD( "03.3v",      0x8000, 0x4000, CRC(eee8b0c9) SHA1(91dd47cdcd36d804e178b70d4338292ac36517f0) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player#1 */
	ROM_LOAD( "14.4e",      0x0000, 0x2000, CRC(220e9c04) SHA1(660351c866995147d2ba69940707879d6cf11718) )
	ROM_LOAD( "a35-15.4c",  0x2000, 0x2000, CRC(1e0d1e33) SHA1(02612f10c264f06f59f61f0de4df0ef84249e963) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player#2 */
	ROM_LOAD( "13.4j",      0x0000, 0x2000, CRC(73feb0e2) SHA1(7c650d0cdc517a60e14614083ab42aa934338556) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "a35-04.10a", 0x0000, 0x2000, CRC(b97ce9b1) SHA1(3993001bd98758fd5673d91786846ae019c64027) ) // tiles
	ROM_LOAD( "a35-05.10b", 0x2000, 0x2000, CRC(55a17b08) SHA1(29427b51b780fb622ac093ea4604caf77cb587ed) )
	ROM_LOAD( "a35-06.10d", 0x4000, 0x2000, CRC(f5ee6f8f) SHA1(0ec90c5edd4d8a9ef614d60bafb002be3daf34ee) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "a35-10.11n", 0x0000, 0x1000, CRC(0b5a0c45) SHA1(4b8ea5dd58c437e7d7929b93ff795803422cb505) ) // characters
	ROM_LOAD( "a35-11.11q", 0x2000, 0x1000, CRC(93346d75) SHA1(b7936ff6f08e4d01bf7d4d2f06ef6b5d84b5097d) )
	ROM_LOAD( "a35-12.11r", 0x4000, 0x1000, CRC(f4c69d8a) SHA1(fb308146d4f4b01bd1318c6986e5124725b2a98f) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "a35-07.12h", 0x0000, 0x4000, CRC(38fc349f) SHA1(d9fdb5bc84808d065e84e077f0e78d8f71b4f0ca) ) // sprites
	ROM_LOAD( "a35-08.12j", 0x4000, 0x4000, CRC(a07d6dc3) SHA1(2ab50ea462a63548d401f85627d85de2e4867303) )
	ROM_LOAD( "a35-09.12k", 0x8000, 0x4000, CRC(c0784a0e) SHA1(e8303bb274361910746eba5e3d61b6ea862a6416) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "a35-16.2j",  0x000, 0x0100, CRC(72d8b332) SHA1(623b1f3fd0008ad92fd1f6fc2b07215da0c26207) )
	ROM_LOAD( "a35-17.2l",  0x100, 0x0100, CRC(9bf1829e) SHA1(7ee47a1a0aa2e4592896d0fc5959343452c224a8) )
	ROM_LOAD( "a35-18.2m",  0x200, 0x0100, CRC(918e4732) SHA1(a38686b32d5ac0ebcba59fdba3201fe35c83d4d0) )
ROM_END

ROM_START( tsamura2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "a35-01.3r",  0x0000, 0x4000, CRC(282d96ad) SHA1(c9d7a9b7acbe6431c061a9b50c05fab3ae664094) )
	ROM_LOAD( "a35-02.3t",  0x4000, 0x4000, CRC(e3fa0cfa) SHA1(3ed8a67789f666fe12d7597014d39deea3c12506) )
	ROM_LOAD( "a35-03.3v",  0x8000, 0x4000, CRC(2fff1e0a) SHA1(0d54b0c9c4760a02bfe5b5d77fff4c858b15dbd8) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player#1 */
	ROM_LOAD( "a35-14.4e",  0x0000, 0x2000, CRC(f10aee3b) SHA1(c4fa2bd626b15b9ea1d5d7e6eaab4f1674841b02) )
	ROM_LOAD( "a35-15.4c",  0x2000, 0x2000, CRC(1e0d1e33) SHA1(02612f10c264f06f59f61f0de4df0ef84249e963) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player#2 */
	ROM_LOAD( "a35-13.4j",  0x0000, 0x2000, CRC(3828f4d2) SHA1(646477c431b123c031ed452b65e96a33e78a2bac) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "a35-04.10a", 0x0000, 0x2000, CRC(b97ce9b1) SHA1(3993001bd98758fd5673d91786846ae019c64027) ) // tiles
	ROM_LOAD( "a35-05.10b", 0x2000, 0x2000, CRC(55a17b08) SHA1(29427b51b780fb622ac093ea4604caf77cb587ed) )
	ROM_LOAD( "a35-06.10d", 0x4000, 0x2000, CRC(f5ee6f8f) SHA1(0ec90c5edd4d8a9ef614d60bafb002be3daf34ee) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "a35-10.11n", 0x0000, 0x1000, CRC(0b5a0c45) SHA1(4b8ea5dd58c437e7d7929b93ff795803422cb505) ) // characters
	ROM_LOAD( "a35-11.11q", 0x2000, 0x1000, CRC(93346d75) SHA1(b7936ff6f08e4d01bf7d4d2f06ef6b5d84b5097d) )
	ROM_LOAD( "a35-12.11r", 0x4000, 0x1000, CRC(f4c69d8a) SHA1(fb308146d4f4b01bd1318c6986e5124725b2a98f) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "a35-07.12h", 0x0000, 0x4000, CRC(38fc349f) SHA1(d9fdb5bc84808d065e84e077f0e78d8f71b4f0ca) ) // sprites
	ROM_LOAD( "a35-08.12j", 0x4000, 0x4000, CRC(a07d6dc3) SHA1(2ab50ea462a63548d401f85627d85de2e4867303) )
	ROM_LOAD( "a35-09.12k", 0x8000, 0x4000, CRC(c0784a0e) SHA1(e8303bb274361910746eba5e3d61b6ea862a6416) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "a35-16.2j",  0x000, 0x0100, CRC(72d8b332) SHA1(623b1f3fd0008ad92fd1f6fc2b07215da0c26207) )
	ROM_LOAD( "a35-17.2l",  0x100, 0x0100, CRC(9bf1829e) SHA1(7ee47a1a0aa2e4592896d0fc5959343452c224a8) )
	ROM_LOAD( "a35-18.2m",  0x200, 0x0100, CRC(918e4732) SHA1(a38686b32d5ac0ebcba59fdba3201fe35c83d4d0) )
ROM_END

ROM_START( nunchaku )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "nunchack.p1", 0x0000, 0x4000, CRC(4385aca6) SHA1(bf6b40340b773929189fb2a0a271040c79a405a1) )
	ROM_LOAD( "nunchack.p2", 0x4000, 0x4000, CRC(f9beb72c) SHA1(548dc9187f87d7a47958691391d2494c2306d767) )
	ROM_LOAD( "nunchack.p3", 0x8000, 0x4000, CRC(cde5d674) SHA1(0360fe81acb6fd77ef581a36a756db550de73732) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "nunchack.m3", 0x0000, 0x2000, CRC(9036c945) SHA1(8e7cb6313b32a78ca0a7fa8595fb872e0f27d8c7) )
	ROM_LOAD( "nunchack.m4", 0x2000, 0x2000, CRC(e7206724) SHA1(fb7f9bfe1e04e1f6af733fc3a79f88e942f3b0b1) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "nunchack.m1", 0x0000, 0x2000, CRC(b53d73f6) SHA1(20b333646a1374fa566b6d608723296e6ded7bc8) )
	ROM_LOAD( "nunchack.m2", 0x2000, 0x2000, CRC(f37d7c49) SHA1(e26d32bf1ecf23b55511260596daf676d9824d37) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "nunchack.b1", 0x0000, 0x2000, CRC(48c88fea) SHA1(2ab27fc69f060e8923f88f9e878e3911d670f5a8) ) // tiles
	ROM_LOAD( "nunchack.b2", 0x2000, 0x2000, CRC(eec818e4) SHA1(1d806dbf6589737e3a4fb52f17bc4c6766a6d6a1) )
	ROM_LOAD( "nunchack.b3", 0x4000, 0x2000, CRC(5f16473f) SHA1(32d8b4a0a7152d86f161d0f30496c25ceff46af3) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "nunchack.v1", 0x0000, 0x1000, CRC(358a3714) SHA1(bf54bf4059cd344d4d861e172b5df5b7763d49d8) ) // characters
	ROM_LOAD( "nunchack.v2", 0x2000, 0x1000, CRC(54c18d8e) SHA1(edcd0a6b5fb1efa2f3693e170cb398574280f7fa) )
	ROM_LOAD( "nunchack.v3", 0x4000, 0x1000, CRC(f7ac203a) SHA1(148a003b48d858eb33f0fee295350483cef42481) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "nunchack.c1", 0x0000, 0x4000, CRC(797cbc8a) SHA1(28ad936318d8b671d4031927c2ea666891a1a408) ) // sprites
	ROM_LOAD( "nunchack.c2", 0x4000, 0x4000, CRC(701a0cc3) SHA1(323ee1cba3da0ccb2c4d542c497de0e1c047f532) )
	ROM_LOAD( "nunchack.c3", 0x8000, 0x4000, CRC(ffb841fc) SHA1(c1285cf093360923307bc86f6a5473d689b16a2c) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "nunchack.016", 0x000, 0x100, CRC(a7b077d4) SHA1(48c3e68d67de067c0ead0dbd34769b755fb5952f) )
	ROM_LOAD( "nunchack.017", 0x100, 0x100, CRC(1c04c087) SHA1(7179edf96f59a469353d9652900b99fef25f4054) )
	ROM_LOAD( "nunchack.018", 0x200, 0x100, CRC(f5ce3c45) SHA1(f2dcdaf95b55b8fd713bdbb965731c064b4a0757) )
ROM_END

ROM_START( yamagchi )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "a38-01.3s", 0x0000, 0x4000, CRC(1a6c8498) SHA1(5c343ff09733507a1518e5a3cab315d9a51ae289) )
	ROM_LOAD( "a38-02.3t", 0x4000, 0x4000, CRC(fa66b396) SHA1(7594549d0c90f5937d11b7ffe80f229df2cea352) )
	ROM_LOAD( "a38-03.3v", 0x8000, 0x4000, CRC(6a4239cf) SHA1(7883f3a7ed18cc0dc0ebb3d929eb6f92df9200de) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "a38-14.4e", 0x0000, 0x2000, CRC(5a758992) SHA1(ee30037ffddf45e9374ab01867c9f6604dede952) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "a38-13.4j", 0x0000, 0x2000, CRC(a26445bb) SHA1(bf09e3f8cf36563bda9ef5a7d01b76155f505b98) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "a38-04.10a", 0x0000, 0x2000, CRC(6bc69d4d) SHA1(245303851ff1850eb68c73b033bb4253afc92f2e) ) // tiles
	ROM_LOAD( "a38-05.10b", 0x2000, 0x2000, CRC(047fb315) SHA1(3689c3cf4c005791fd76574171d14add2bc046a6) )
	ROM_LOAD( "a38-06.10d", 0x4000, 0x2000, CRC(a636afb2) SHA1(bff04d696e54a91b5e16c9d98769970af2184b13) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "a38-10.11n", 0x0000, 0x1000, CRC(51ab4671) SHA1(8e1bf5c995dbda3038f889a15282974b265d50ef) ) // characters
	ROM_LOAD( "a38-11.11p", 0x2000, 0x1000, CRC(27890169) SHA1(ded7dc8d2e738e337965b548c2289d2a7acdb663) )
	ROM_LOAD( "a38-12.11r", 0x4000, 0x1000, CRC(c98d5cf2) SHA1(e4fc8a46790a596de789ddb3ce1caa3400a84c74) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "a38-07.12h", 0x0000, 0x4000, CRC(a3a521b6) SHA1(267e718e4f288b264b814edde4f72bd917190203) ) // sprites
	ROM_LOAD( "a38-08.12j", 0x4000, 0x4000, CRC(553afc66) SHA1(3215be8642ae011b5a8f2ada5799eeb32a01cac1) )
	ROM_LOAD( "a38-09.12l", 0x8000, 0x4000, CRC(574156ae) SHA1(b82cca25fca7b64778bf461e61ff04fec846c6fa) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114e.2k", 0x000, 0x100, CRC(e7648110) SHA1(17327bc8dacbaa8f03e14c8b59bded117be46e68) )
	ROM_LOAD( "mb7114e.2l", 0x100, 0x100, CRC(7b874ee6) SHA1(d23bc6dfe882cfa9a2c84bf8037b47759427060f) )
	ROM_LOAD( "mb7114e.2m", 0x200, 0x100, CRC(938d0fce) SHA1(9aa14fdee23b1ed50300fe8f82525bb363914d93) )
ROM_END

ROM_START( m660 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "660l.bin",    0x0000, 0x4000, CRC(57c0d1cc) SHA1(3d71d0554e445f27f5b57a185acddd58f70e95f4) )
	ROM_LOAD( "660m.bin",    0x4000, 0x4000, CRC(628c6686) SHA1(e695ccfb1251bc7571122de30e682b135e773f20) )
	ROM_LOAD( "660n.bin",    0x8000, 0x4000, CRC(1b418a97) SHA1(a9afa341c790e650fb91b6e9df4959c3bd7ab5c7) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "14.4n",       0x0000, 0x4000, CRC(5734db5a) SHA1(ff99bf618018be20a4b38fcfbe75d9c5bb6fd176) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "13.4j",       0x0000, 0x4000, CRC(fba51cf7) SHA1(b18571112dcbe3214a803d0898b0a21957dc5e5f) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 ) /* Z80 code AY driver */
	ROM_LOAD( "660x.bin",    0x0000, 0x8000, CRC(b82f0cfa) SHA1(7c74f3d57fccc020d3a99cbd676480ea6625b2a1) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "4.7k",        0x0000, 0x4000, CRC(e24e431a) SHA1(b5558550782dc7452dcd50b72390408ffc17e5e7) ) // tiles
	ROM_LOAD( "5.6k",        0x4000, 0x4000, CRC(b2c93d46) SHA1(f81b5aadb2d4af8b3bb4e386fe408aaff7360225) )
	ROM_LOAD( "6.5k",        0x8000, 0x4000, CRC(763c5983) SHA1(d4a62dab71e88d90dbd16ed4e152e1a5a01cf64f) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "660u.bin",    0x0000, 0x2000, CRC(030af716) SHA1(a9c664274a3d6127934273bb5b45a4a374b244dd) ) // characters
	ROM_LOAD( "660v.bin",    0x2000, 0x2000, CRC(51a6e160) SHA1(26689b1de33dde2c6053dbdf44f004cbcc46d292) )
	ROM_LOAD( "660w.bin",    0x4000, 0x2000, CRC(8a45b469) SHA1(7c6d895b23846fcf16285ec542c263b6168a46c0) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "7.15e",       0x0000, 0x4000, CRC(990c0cee) SHA1(20aebf64f62c16069df752124da509359d5d1af2) ) // sprites
	ROM_LOAD( "8.15d",       0x4000, 0x4000, CRC(d9aa7834) SHA1(0921c16a195d9e2d77a1f1591c5e77d87ca65c04) )
	ROM_LOAD( "9.15b",       0x8000, 0x4000, CRC(27b26905) SHA1(58715e5792ec5388e68510b9cd8846fb4fb3caf8) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "4r.bpr",      0x000, 0x100, CRC(cd16d0f1) SHA1(4b0a68f28329fb86d252f4170edd2ab0488805e5) )
	ROM_LOAD( "4p.bpr",      0x100, 0x100, CRC(22e8b22c) SHA1(2934ca96495fca72a33fa2881dc65ab21342c410) )
	ROM_LOAD( "5r.bpr",      0x200, 0x100, CRC(b7d6fdb5) SHA1(67d3bb16470f5d4ec35164a391ad6b65850f824a) )
ROM_END

ROM_START( m660j )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "1.3c",        0x0000, 0x4000, CRC(4c8f96aa) SHA1(cc076bdc7ecc206f7e0b9c17fbba59507f515df1) )
	ROM_LOAD( "2.3d",        0x4000, 0x4000, CRC(e6661504) SHA1(4ec208d49f95f378f3dbeb375e6c220d02a35092) )
	ROM_LOAD( "3.3f",        0x8000, 0x4000, CRC(3a389ccd) SHA1(3d99312d7fa9d269fdd218917ceafdd61890617f) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "14.4n",       0x0000, 0x4000, CRC(5734db5a) SHA1(ff99bf618018be20a4b38fcfbe75d9c5bb6fd176) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "13.4j",       0x0000, 0x4000, CRC(fba51cf7) SHA1(b18571112dcbe3214a803d0898b0a21957dc5e5f) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 ) /* Z80 code AY driver */
	ROM_LOAD( "d.4e",        0x0000, 0x4000, CRC(93f3d852) SHA1(074f6899ff9064e29002c830cfae000973d8b89e) )
	ROM_LOAD( "e.4d",        0x4000, 0x4000, CRC(12f5c077) SHA1(3bb3f9c1a84744b8849dd25591ac186e23803542) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "4.7k",        0x0000, 0x4000, CRC(e24e431a) SHA1(b5558550782dc7452dcd50b72390408ffc17e5e7) ) // tiles
	ROM_LOAD( "5.6k",        0x4000, 0x4000, CRC(b2c93d46) SHA1(f81b5aadb2d4af8b3bb4e386fe408aaff7360225) )
	ROM_LOAD( "6.5k",        0x8000, 0x4000, CRC(763c5983) SHA1(d4a62dab71e88d90dbd16ed4e152e1a5a01cf64f) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "a.16j",       0x0000, 0x2000, CRC(06f44c8c) SHA1(b4820a051fcf4f1cc17a4d94cce0e8ab04aaafb5) ) // characters
	ROM_LOAD( "b.16k",       0x2000, 0x2000, CRC(94b8b69f) SHA1(1c28d674023323bcec3ef752b3614b1581c0b551) )
	ROM_LOAD( "c.16m",       0x4000, 0x2000, CRC(d6768c68) SHA1(dadbece88e53cd80de2a9dba8ded694176f432c9) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "7.15e",       0x0000, 0x4000, CRC(990c0cee) SHA1(20aebf64f62c16069df752124da509359d5d1af2) ) // sprites
	ROM_LOAD( "8.15d",       0x4000, 0x4000, CRC(d9aa7834) SHA1(0921c16a195d9e2d77a1f1591c5e77d87ca65c04) )
	ROM_LOAD( "9.15b",       0x8000, 0x4000, CRC(27b26905) SHA1(58715e5792ec5388e68510b9cd8846fb4fb3caf8) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "4r.bpr",      0x000, 0x100, CRC(cd16d0f1) SHA1(4b0a68f28329fb86d252f4170edd2ab0488805e5) )
	ROM_LOAD( "4p.bpr",      0x100, 0x100, CRC(22e8b22c) SHA1(2934ca96495fca72a33fa2881dc65ab21342c410) )
	ROM_LOAD( "5r.bpr",      0x200, 0x100, CRC(b7d6fdb5) SHA1(67d3bb16470f5d4ec35164a391ad6b65850f824a) )
ROM_END

ROM_START( m660b )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "m660-1.bin",  0x0000, 0x4000, CRC(18f6c4be) SHA1(29a66be9216347b40ab0ccb95baf3e0a6207da8e) )
	ROM_LOAD( "2.3d",        0x4000, 0x4000, CRC(e6661504) SHA1(4ec208d49f95f378f3dbeb375e6c220d02a35092) )
	ROM_LOAD( "3.3f",        0x8000, 0x4000, CRC(3a389ccd) SHA1(3d99312d7fa9d269fdd218917ceafdd61890617f) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "14.4n",       0x0000, 0x4000, CRC(5734db5a) SHA1(ff99bf618018be20a4b38fcfbe75d9c5bb6fd176) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "13.4j",       0x0000, 0x4000, CRC(fba51cf7) SHA1(b18571112dcbe3214a803d0898b0a21957dc5e5f) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 ) /* Z80 code AY driver */
	ROM_LOAD( "660x.bin",    0x0000, 0x8000, CRC(b82f0cfa) SHA1(7c74f3d57fccc020d3a99cbd676480ea6625b2a1) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "4.7k",        0x0000, 0x4000, CRC(e24e431a) SHA1(b5558550782dc7452dcd50b72390408ffc17e5e7) ) // tiles
	ROM_LOAD( "5.6k",        0x4000, 0x4000, CRC(b2c93d46) SHA1(f81b5aadb2d4af8b3bb4e386fe408aaff7360225) )
	ROM_LOAD( "6.5k",        0x8000, 0x4000, CRC(763c5983) SHA1(d4a62dab71e88d90dbd16ed4e152e1a5a01cf64f) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "m660-10.bin", 0x0000, 0x2000, CRC(b11405a6) SHA1(c093ba567ce95abc4f08c8449e366cd7d2abc8d2) ) // characters
	ROM_LOAD( "b.16k",       0x2000, 0x2000, CRC(94b8b69f) SHA1(1c28d674023323bcec3ef752b3614b1581c0b551) )
	ROM_LOAD( "c.16m",       0x4000, 0x2000, CRC(d6768c68) SHA1(dadbece88e53cd80de2a9dba8ded694176f432c9) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "7.15e",       0x0000, 0x4000, CRC(990c0cee) SHA1(20aebf64f62c16069df752124da509359d5d1af2) ) // sprites
	ROM_LOAD( "8.15d",       0x4000, 0x4000, CRC(d9aa7834) SHA1(0921c16a195d9e2d77a1f1591c5e77d87ca65c04) )
	ROM_LOAD( "9.15b",       0x8000, 0x4000, CRC(27b26905) SHA1(58715e5792ec5388e68510b9cd8846fb4fb3caf8) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "4r.bpr",      0x000, 0x100, CRC(cd16d0f1) SHA1(4b0a68f28329fb86d252f4170edd2ab0488805e5) )
	ROM_LOAD( "4p.bpr",      0x100, 0x100, CRC(22e8b22c) SHA1(2934ca96495fca72a33fa2881dc65ab21342c410) )
	ROM_LOAD( "5r.bpr",      0x200, 0x100, CRC(b7d6fdb5) SHA1(67d3bb16470f5d4ec35164a391ad6b65850f824a) )
ROM_END

ROM_START( alphaxz )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "az-01.bin",   0x0000, 0x4000, CRC(5336f842) SHA1(cf43c87fad9131120ac75dfd4e2aec260611af7b) )
	ROM_LOAD( "az-02.bin",   0x4000, 0x4000, CRC(a0779b6b) SHA1(146c967253031d2bbbdbc49b5854c0676e458af1) )
	ROM_LOAD( "az-03.bin",   0x8000, 0x4000, CRC(2797bc7b) SHA1(137ef537917a9f243208d2befee0c09a36782647) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "14.4n",       0x0000, 0x4000, CRC(5734db5a) SHA1(ff99bf618018be20a4b38fcfbe75d9c5bb6fd176) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* Z80 code - sample player */
	ROM_LOAD( "13.4j",       0x0000, 0x4000, CRC(fba51cf7) SHA1(b18571112dcbe3214a803d0898b0a21957dc5e5f) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 ) /* Z80 code AY driver */
	ROM_LOAD( "660x.bin",    0x0000, 0x8000, CRC(b82f0cfa) SHA1(7c74f3d57fccc020d3a99cbd676480ea6625b2a1) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "az-04.bin",   0x00000, 0x4000, CRC(23da4e3d) SHA1(fd48c86769360a778057a673d5c1dfdfe00f6c18) ) // tiles
	ROM_LOAD( "az-05.bin",   0x04000, 0x4000, CRC(8746ff69) SHA1(6b95f209d931601155ad492652e3016df3e5dfb0) )
	ROM_LOAD( "az-06.bin",   0x08000, 0x4000, CRC(6e494964) SHA1(9f42935281502a1e73f602cb93ca241aeaa03201) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "az-10.bin",   0x00000, 0x2000, CRC(10b499bb) SHA1(270702d2ba0313548b78ae3967edb10cf6b80610) ) // characters
	ROM_LOAD( "az-11.bin",   0x02000, 0x2000, CRC(d91993f6) SHA1(250e3ba1e694fedf4152c86f2ce88593866160f2) )
	ROM_LOAD( "az-12.bin",   0x04000, 0x2000, CRC(8ea48ef3) SHA1(f6a5477d28ab2d2bac655a02f96f31b8db9c44f8) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "az-07.bin",   0x00000, 0x4000, CRC(5f9cc65e) SHA1(235b4a8762ba429855cc9db07477efde2a62e03d) ) // sprites
	ROM_LOAD( "az-08.bin",   0x04000, 0x4000, CRC(23e3a6ba) SHA1(d6d035b7b7530a909669ac045aea51e297ba784e) )
	ROM_LOAD( "az-09.bin",   0x08000, 0x4000, CRC(7096fa71) SHA1(f9697b30d7eec5ee9122d783f9ee7b9cdbab9262) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "4r.bpr",      0x000, 0x100, CRC(cd16d0f1) SHA1(4b0a68f28329fb86d252f4170edd2ab0488805e5) )
	ROM_LOAD( "4p.bpr",      0x100, 0x100, CRC(22e8b22c) SHA1(2934ca96495fca72a33fa2881dc65ab21342c410) )
	ROM_LOAD( "5r.bpr",      0x200, 0x100, CRC(b7d6fdb5) SHA1(67d3bb16470f5d4ec35164a391ad6b65850f824a) )
ROM_END

ROM_START( vsgongf )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "1.5a",	0x0000, 0x2000, CRC(2c056dee) SHA1(f063fdd571949a1b7ac36f88e17feec7354ea894) ) /* good? */
	ROM_LOAD( "2",		0x2000, 0x2000, CRC(1a634daf) SHA1(d282fbb2ca2c8db70cbbbf640ce507d4c142cc39) ) /* good? */
	ROM_LOAD( "3.5d",	0x4000, 0x2000, CRC(5ac16861) SHA1(2af51811285fb2de44b023872e42aae37bfbf105) )
	ROM_LOAD( "4.5f",	0x6000, 0x2000, CRC(1d1baf7b) SHA1(b05d3d7bca299c219a02966b3af2ac517472d0a5) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sound CPU */
	ROM_LOAD( "6.5n",	0x0000, 0x2000, CRC(785b9000) SHA1(9eb32724b0611a93959485a7f9c806eb6d7ab013) )
	ROM_LOAD( "5.5l",	0x2000, 0x2000, CRC(76dbfde9) SHA1(fe6e02f4b5a0a5baa59506636226c8ea7b551ef6) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles (N/A) */

	ROM_REGION( 0x3000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "7.6f",	0x0000, 0x1000, CRC(6ec68692) SHA1(9c0742749ca71c888abbc7eb7ed8a538a9465ed2) )
	ROM_LOAD( "8.7f",	0x1000, 0x1000, CRC(afba16c8) SHA1(bfa03d95e8c4372efe2864b423bf32cda79760ce) )
	ROM_LOAD( "9.8f",	0x2000, 0x1000, CRC(536bf710) SHA1(43f653b21deac58b66b9df267ea44cbd99aff694) )

	ROM_REGION( 0x6000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "13.15j",  0x0000, 0x2000, CRC(a2451a31) SHA1(e416d8c5ae18596b2619618b4666fa306204ca71) )
	ROM_LOAD( "14.15h",  0x2000, 0x2000, CRC(b387403e) SHA1(7aff3d7c5a9861f3e30244706ac303fbc240b4bd) )
	ROM_LOAD( "15.15f",  0x4000, 0x2000, CRC(0e649334) SHA1(31f22b511c7b73056139f17a30012cdc0a0c7d52) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "clr.6s",  0x000, 0x0100, CRC(578bfbea) SHA1(12a97de0f8012ccce75e14abf743bcec6857684c) )
	ROM_LOAD( "clr.6r",  0x100, 0x0100, CRC(3ec00739) SHA1(614d1799fe197df389f6155f86fe113e1b0b018a) )
	ROM_LOAD( "clr.6p",  0x200, 0x0100, CRC(0e4fd17a) SHA1(d4e32bd9dd903177af61f77976a25c5db1467bba) )
ROM_END


ROM_START( ringfgt )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "rft04-1",	0x0000, 0x2000, CRC(11030866) SHA1(b95b231c241e5bdc002de3f6a732cd627c7dc145) )
	ROM_LOAD( "rft03-1",	0x2000, 0x2000, CRC(357a2085) SHA1(0534f6c1a876dacfcff09a547290354eeddb3126) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sound CPU */
	ROM_LOAD( "6.5n",	0x0000, 0x2000, CRC(785b9000) SHA1(9eb32724b0611a93959485a7f9c806eb6d7ab013) )
	ROM_LOAD( "5.5l",	0x2000, 0x2000, CRC(76dbfde9) SHA1(fe6e02f4b5a0a5baa59506636226c8ea7b551ef6) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles (N/A) */

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "rft05",	0x0000, 0x1000, CRC(a7b732fd) SHA1(2abe4b422a7cd32cd12c6d6acba1872afe4a2ecc) )
	ROM_LOAD( "rft06",	0x2000, 0x1000, CRC(ff2721f7) SHA1(ae75103a7663a190da36e0bb2d46a333f830eba5) )
	ROM_LOAD( "rft07",	0x4000, 0x1000, CRC(ec1d7ba4) SHA1(047aa3c6c92126ac623fddbe0adc50a450910d6e) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "rft08",  0x0000, 0x2000, CRC(80d67d28) SHA1(f4016159201abdfe0c0441f78ed6a1a12b7ba34b) )
	ROM_LOAD( "rft09",  0x4000, 0x2000, CRC(ea8f0656) SHA1(c4d2302d046c41bae9946a5ddc0087349a421b35) )
	ROM_LOAD( "rft10",  0x8000, 0x2000, CRC(833ca89f) SHA1(b43416e783e6109d19de81a124fae84f1afb8440) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "clr.6s",  0x000, 0x0100, CRC(578bfbea) SHA1(12a97de0f8012ccce75e14abf743bcec6857684c) )
	ROM_LOAD( "clr.6r",  0x100, 0x0100, CRC(3ec00739) SHA1(614d1799fe197df389f6155f86fe113e1b0b018a) )
	ROM_LOAD( "clr.6p",  0x200, 0x0100, CRC(0e4fd17a) SHA1(d4e32bd9dd903177af61f77976a25c5db1467bba) )
ROM_END

ROM_START( ringfgt2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 ) /* Z80 code  - main CPU */
	ROM_LOAD( "rft.04",	0x0000, 0x2000, CRC(6b9b3f3d) SHA1(ea75e77e0e3379a22381b1d0aae7f96b53cd7562) )
	ROM_LOAD( "rft.03",	0x2000, 0x2000, CRC(1821974b) SHA1(1ce52f20bf49c111000f870bbe3416d27673b91d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 code - sound CPU */
	ROM_LOAD( "6.5n",	0x0000, 0x2000, CRC(785b9000) SHA1(9eb32724b0611a93959485a7f9c806eb6d7ab013) )
	ROM_LOAD( "5.5l",	0x2000, 0x2000, CRC(76dbfde9) SHA1(fe6e02f4b5a0a5baa59506636226c8ea7b551ef6) )

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles (N/A) */

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE ) /* characters */
	ROM_LOAD( "rft05",	0x0000, 0x1000, CRC(a7b732fd) SHA1(2abe4b422a7cd32cd12c6d6acba1872afe4a2ecc) )
	ROM_LOAD( "rft06",	0x2000, 0x1000, CRC(ff2721f7) SHA1(ae75103a7663a190da36e0bb2d46a333f830eba5) )
	ROM_LOAD( "rft07",	0x4000, 0x1000, CRC(ec1d7ba4) SHA1(047aa3c6c92126ac623fddbe0adc50a450910d6e) )

	ROM_REGION( 0xc000, REGION_GFX3, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "rft08",  0x0000, 0x2000, CRC(80d67d28) SHA1(f4016159201abdfe0c0441f78ed6a1a12b7ba34b) )
	ROM_LOAD( "rft09",  0x4000, 0x2000, CRC(ea8f0656) SHA1(c4d2302d046c41bae9946a5ddc0087349a421b35) )
	ROM_LOAD( "rft10",  0x8000, 0x2000, CRC(833ca89f) SHA1(b43416e783e6109d19de81a124fae84f1afb8440) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )
	ROM_LOAD( "clr.6s",  0x000, 0x0100, CRC(578bfbea) SHA1(12a97de0f8012ccce75e14abf743bcec6857684c) )
	ROM_LOAD( "clr.6r",  0x100, 0x0100, CRC(3ec00739) SHA1(614d1799fe197df389f6155f86fe113e1b0b018a) )
	ROM_LOAD( "clr.6p",  0x200, 0x0100, CRC(0e4fd17a) SHA1(d4e32bd9dd903177af61f77976a25c5db1467bba) )
ROM_END

GAME( 1984, vsgongf,  0,        vsgongf,  vsgongf,  0, ROT90, "Kaneko", "VS Gong Fight", GAME_IMPERFECT_COLORS ,2)
GAME( 1984, ringfgt,  vsgongf,  vsgongf,  vsgongf,  0, ROT90, "Taito", "Ring Fighter (set 1)", 0 ,2)
GAME( 1984, ringfgt2, vsgongf,  vsgongf,  vsgongf,  0, ROT90, "Taito", "Ring Fighter (set 2)", 0 ,2)
GAME( 1985, tsamurai, 0,        tsamurai, tsamurai, 0, ROT90, "Taito", "Samurai Nihon-ichi (set 1)", 0 ,0)
GAME( 1985, tsamura2, tsamurai, tsamurai, tsamurai, 0, ROT90, "Taito", "Samurai Nihon-ichi (set 2)", 0 ,0)
GAME( 1985, nunchaku, 0,        tsamurai, nunchaku, 0, ROT90, "Taito", "Nunchackun", GAME_IMPERFECT_COLORS ,0)
GAME( 1985, yamagchi, 0,        tsamurai, yamagchi, 0, ROT90, "Taito", "Go Go Mr. Yamaguchi / Yuke Yuke Yamaguchi-kun", GAME_IMPERFECT_COLORS ,0)

GAME( 1986, m660,     0,        m660,     m660,     0, ROT90, "[Woodplace Inc.] Taito America Corporation", "Mission 660 (US)", 0 ,0)
GAME( 1986, m660j,    m660,     m660,     m660,     0, ROT90, "[Woodplace Inc.] Taito Corporation", "Mission 660 (Japan)", 0 ,0)
GAME( 1986, m660b,    m660,     m660,     m660,     0, ROT90, "bootleg", "Mission 660 (bootleg)", 0 ,0)
GAME( 1986, alphaxz,  m660,     m660,     m660,     0, ROT90, "Ed / Woodplace Inc.", "The Alphax Z (Japan)", 0 ,0)
