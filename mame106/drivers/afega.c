/***************************************************************************

  NOTE: this hardware is a bootleg of the hardware in nmk16.c
        twinactn is a bootleg of mustang with different graphics, different
        sound hardware, and some code shifted around

  todo: either merge this with nmk16.c, or split the bootlegs / non nmk
        hardware from nmk16.c into an nmk16 bootlegs driver

        understand the 'protection' it may just be some kind of mirroring.

****************************************************************************

                              -= Afega Games =-

                    driver by   Luca Elia (l.elia@tin.it)


Main  CPU   :   M68000
Video Chips :   AFEGA AFI-GFSK  (68 Pin PLCC)
                AFEGA AFI-GFLK (208 Pin PQFP)

Sound CPU   :   Z80
Sound Chips :   M6295 (AD-65)  +  YM2151 (BS901)  +  YM3012 (BS902)

---------------------------------------------------------------------------
Year + Game                     Notes
---------------------------------------------------------------------------
95 Twin Action                  Does not work for unknown reasons. Protection?
97 Red Hawk                     US Version of Stagger 1
98 Sen Jin - Guardian Storm     Some text missing (protection, see service mode)
98 Stagger I
98 Bubble 2000                  By Tuning, but it seems to be the same HW
00 Spectrum 2000                By YomaTech -- NOTE sprite bugs happen on real hw!!
01 Fire Hawk                    By ESD with different sound hardware: 2 M6295
---------------------------------------------------------------------------

Notes:

- Afega is a Korean company. Its name is an acronym of "Art-Fiction Electronic Game".
- Sen Jin: protection supplies some 68k code seen in the 2760-29cf range.
    - or service mode is just complete, these are korean hacks afterall
- Twinactn: to enter the hidden test mode press both P2 buttons during boot, then
  P1 button 1 repeatedly.

***************************************************************************/

#include "driver.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"

/* Variables defined in vidhrdw: */

extern UINT16 *afega_vram_0, *afega_scroll_0;
extern UINT16 *afega_vram_1, *afega_scroll_1;

/* Functions defined in vidhrdw: */

WRITE16_HANDLER( afega_vram_0_w );
WRITE16_HANDLER( afega_vram_1_w );
WRITE16_HANDLER( afega_palette_w );

PALETTE_INIT( grdnstrm );

VIDEO_START( afega );
VIDEO_START( firehawk );
VIDEO_UPDATE( afega );
VIDEO_UPDATE( redhawkb );
VIDEO_UPDATE( bubl2000 );
VIDEO_UPDATE( firehawk );
VIDEO_UPDATE( twinactn );

static UINT16 *ram;

/***************************************************************************


                            Memory Maps - Main CPU


***************************************************************************/

READ16_HANDLER( afega_unknown_r )
{
	/* This fixes the text in Service Mode. */
	return 0x0100;
}

WRITE16_HANDLER( afega_soundlatch_w )
{
	if (ACCESSING_LSB)
	{
		soundlatch_w(0,data&0xff);
		cpunum_set_input_line(1, 0, PULSE_LINE);
	}
}

static WRITE16_HANDLER( afega_scroll0_w )
{
	COMBINE_DATA(&afega_scroll_0[offset]);
}

static WRITE16_HANDLER( afega_scroll1_w )
{
	COMBINE_DATA(&afega_scroll_1[offset]);
}

/*
 Lines starting with an empty comment in the following MemoryReadAddress
 arrays are there for debug (e.g. the game does not read from those ranges
 AFAIK)
*/


static ADDRESS_MAP_START( afega, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(20) )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x080000, 0x080001) AM_READ(input_port_0_word_r)		// Buttons
	AM_RANGE(0x080002, 0x080003) AM_READ(input_port_1_word_r)		// P1 + P2
	AM_RANGE(0x080004, 0x080005) AM_READ(input_port_2_word_r)		// 2 x DSW
	AM_RANGE(0x080012, 0x080013) AM_READ(afega_unknown_r)
	AM_RANGE(0x080000, 0x08001d) AM_WRITE(MWA16_RAM)				//
	AM_RANGE(0x08001e, 0x08001f) AM_WRITE(afega_soundlatch_w)		// To Sound CPU
/**/AM_RANGE(0x084000, 0x084003) AM_RAM AM_WRITE(afega_scroll0_w)	// Scroll on redhawkb (mirror or changed?..)
/**/AM_RANGE(0x084004, 0x084007) AM_RAM AM_WRITE(afega_scroll1_w)   // Scroll on redhawkb (mirror or changed?..)
	AM_RANGE(0x080020, 0x087fff) AM_WRITE(MWA16_RAM)				//
/**/AM_RANGE(0x088000, 0x0885ff) AM_READWRITE(MRA16_RAM, afega_palette_w) AM_BASE(&paletteram16) // Palette
	AM_RANGE(0x088600, 0x08bfff) AM_WRITE(MWA16_RAM)				//
/**/AM_RANGE(0x08c000, 0x08c003) AM_RAM AM_WRITE(afega_scroll0_w) AM_BASE(&afega_scroll_0)	// Scroll
/**/AM_RANGE(0x08c004, 0x08c007) AM_RAM AM_WRITE(afega_scroll1_w) AM_BASE(&afega_scroll_1)	//
	AM_RANGE(0x08c008, 0x08ffff) AM_WRITE(MWA16_RAM)				//
/**/AM_RANGE(0x090000, 0x091fff) AM_READWRITE(MRA16_RAM, afega_vram_0_w) AM_BASE(&afega_vram_0)	// Layer 0
/**/AM_RANGE(0x092000, 0x093fff) AM_RAM								// ?
/**/AM_RANGE(0x09c000, 0x09c7ff) AM_READWRITE(MRA16_RAM, afega_vram_1_w) AM_BASE(&afega_vram_1)	// Layer 1
	AM_RANGE(0x0c0000, 0x0c7fff) AM_RAM								// RAM
	AM_RANGE(0x0c8000, 0x0c8fff) AM_RAM AM_SHARE(1) AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)	// Sprites
	AM_RANGE(0x0c9000, 0x0cffff) AM_RAM	AM_SHARE(2) AM_BASE(&ram)				// RAM
	AM_RANGE(0x0f8000, 0x0f8fff) AM_RAM AM_SHARE(1)					// Sprites Mirror
	AM_RANGE(0x0f9000, 0x0fffff) AM_RAM	AM_SHARE(2)				// RAM Mirror (MangChi)
ADDRESS_MAP_END


// The high byte of the word written is the address to write to (byte offset), the low byte is data
static WRITE16_HANDLER( twinactn_scroll0_w )
{
	int byte = ((data >> 8) & 3) ^ 2;
	int bit  = ((byte & 1) ? 0 : 8);
	afega_scroll_0[byte / 2] = (afega_scroll_0[byte / 2] & (0xff << (8-bit))) | ((data & 0xff) << bit);
}
static WRITE16_HANDLER( twinactn_scroll1_w )
{
	int byte = ((data >> 8) & 3) ^ 2;
	int bit  = ((byte & 1) ? 0 : 8);
	afega_scroll_1[byte / 2] = (afega_scroll_1[byte / 2] & (0xff << (8-bit))) | ((data & 0xff) << bit);
}

static WRITE16_HANDLER( twinactn_flipscreen_w )
{
	if (ACCESSING_LSB)
		flip_screen_set(data & 1);

	if (data & (~1))
		logerror("%06x: unknown flip screen bit written %04x\n", activecpu_get_pc(), data);
}

static ADDRESS_MAP_START( twinactn, ADDRESS_SPACE_PROGRAM, 16 )
//  ADDRESS_MAP_FLAGS( AMEF_ABITS(20) )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x080000, 0x080001) AM_READ(input_port_0_word_r)		// Buttons
	AM_RANGE(0x080002, 0x080003) AM_READ(input_port_1_word_r)		// P1 + P2
	AM_RANGE(0x080004, 0x080005) AM_READ(input_port_2_word_r)		// 2 x DSW
	AM_RANGE(0x080014, 0x080015) AM_WRITE(twinactn_flipscreen_w)
	AM_RANGE(0x080016, 0x080017) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x08001e, 0x08001f) AM_WRITE(afega_soundlatch_w)		// To Sound CPU
/**/AM_RANGE(0x088000, 0x0885ff) AM_READWRITE(MRA16_RAM, afega_palette_w) AM_BASE(&paletteram16) // Palette
/**/AM_RANGE(0x08c000, 0x08c003) AM_RAM AM_WRITE(twinactn_scroll0_w) AM_BASE(&afega_scroll_0)	// Scroll
/**/AM_RANGE(0x08c004, 0x08c007) AM_RAM AM_WRITE(twinactn_scroll1_w) AM_BASE(&afega_scroll_1)	//
	AM_RANGE(0x08c008, 0x08c087) AM_WRITE(MWA16_RAM)				//
/**/AM_RANGE(0x090000, 0x091fff) AM_READWRITE(MRA16_RAM, afega_vram_0_w) AM_BASE(&afega_vram_0)	// Layer 0
/**/AM_RANGE(0x092000, 0x093fff) AM_RAM								// ?
/**/AM_RANGE(0x09c000, 0x09c7ff) AM_READWRITE(MRA16_RAM, afega_vram_1_w) AM_BASE(&afega_vram_1)	// Layer 1
	AM_RANGE(0x0f0000, 0x0f7fff) AM_RAM								// Work RAM
	AM_RANGE(0x0f8000, 0x0f8fff) AM_RAM AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)	// Sprites
	AM_RANGE(0x0f9000, 0x0fffff) AM_RAM	AM_BASE(&ram)							// Work RAM
ADDRESS_MAP_END

/***************************************************************************


                            Memory Maps - Sound CPU


***************************************************************************/
static WRITE8_HANDLER( spec2k_oki1_banking_w )
{
 	if(data == 0xfe)
 		OKIM6295_set_bank_base(1, 0);
 	else if(data == 0xff)
 		OKIM6295_set_bank_base(1, 0x40000);
}

static ADDRESS_MAP_START( afega_sound_cpu, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0003, 0x0003) AM_WRITE(MWA8_NOP) // bug in sound prg?
	AM_RANGE(0x0004, 0x0004) AM_WRITE(MWA8_NOP) // bug in sound prg?
	AM_RANGE(0x0000, 0xefff) AM_ROM
	AM_RANGE(0xf000, 0xf7ff) AM_RAM									// RAM
	AM_RANGE(0xf800, 0xf800) AM_READ(soundlatch_r)					// From Main CPU
	AM_RANGE(0xf808, 0xf808) AM_WRITE(YM2151_register_port_0_w)		// YM2151
	AM_RANGE(0xf809, 0xf809) AM_READWRITE(YM2151_status_port_0_r, YM2151_data_port_0_w)	// YM2151
	AM_RANGE(0xf80a, 0xf80a) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)		// M6295
ADDRESS_MAP_END

static ADDRESS_MAP_START( firehawk_sound_cpu, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xefff) AM_ROM
	AM_RANGE(0xf000, 0xf7ff) AM_RAM
	AM_RANGE(0xfff0, 0xfff0) AM_READ(soundlatch_r)
	AM_RANGE(0xfff2, 0xfff2) AM_WRITE(spec2k_oki1_banking_w )
	AM_RANGE(0xfff8, 0xfff8) AM_READWRITE(OKIM6295_status_1_r, OKIM6295_data_1_w)
	AM_RANGE(0xfffa, 0xfffa) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)
	AM_RANGE(0xf800, 0xffff) AM_RAM // not used, only tested
ADDRESS_MAP_END


static WRITE8_HANDLER( twinactn_oki_bank_w )
{
	OKIM6295_set_bank_base(0, (data & 3) * 0x40000);

	if (data & (~3))
		logerror("%04x: invalid oki bank %02x\n", activecpu_get_pc(), data);

//  logerror("%04x: oki bank %02x\n", activecpu_get_pc(), data);
}

static ADDRESS_MAP_START( twinactn_sound_cpu, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0x9000, 0x9000) AM_WRITE(twinactn_oki_bank_w)
	AM_RANGE(0x9800, 0x9800) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)
	AM_RANGE(0xa000, 0xa000) AM_READ(soundlatch_r)		// From Main CPU
ADDRESS_MAP_END

/***************************************************************************


                                Input Ports


***************************************************************************/

/***************************************************************************
                                Stagger I
***************************************************************************/

INPUT_PORTS_START( stagger1 )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN2")	// $080004.w
	PORT_SERVICE( 0x0001, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0080, "2" )
	PORT_DIPSETTING(      0x00c0, "3" )
	PORT_DIPSETTING(      0x0040, "5" )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPSETTING(      0x0200, "Horizontally" )
	PORT_DIPSETTING(      0x0100, "Vertically" )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END

/* everything seems active high.. not low */
INPUT_PORTS_START( redhawkb )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_HIGH, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_HIGH, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_HIGH, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_HIGH, IPT_START2   )
	PORT_BIT(  0x0020, IP_ACTIVE_HIGH, IPT_UNKNOWN  )
	PORT_BIT(  0x0040, IP_ACTIVE_HIGH, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT(  0x0100, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("IN2")	// $080004.w  -- probably just redhawk but inverted
	PORT_SERVICE( 0x0001, IP_ACTIVE_HIGH )    /* not working */
	PORT_DIPNAME( 0x0002, 0x0000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x00c0, 0x0000, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x00c0, "1" )
	PORT_DIPSETTING(      0x0040, "2" )
	PORT_DIPSETTING(      0x0000, "3" )
	PORT_DIPSETTING(      0x0080, "5" )
	PORT_DIPNAME( 0x0300, 0x0000, DEF_STR( Flip_Screen ) )    /* not supported */
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( On ) )
	PORT_DIPSETTING(      0x0100, "Horizontally" )
	PORT_DIPSETTING(      0x0200, "Vertically" )
	PORT_DIPNAME( 0x0400, 0x0000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x0000, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xe000, 0x0000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END

/***************************************************************************
                            Sen Jin - Guardian Storm
***************************************************************************/

INPUT_PORTS_START( grdnstrm )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// IN1 - $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN2")	// $080004.w
	PORT_SERVICE( 0x0001, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, "Bombs" )
	PORT_DIPSETTING(      0x0008, "2" )
	PORT_DIPSETTING(      0x0000, "3" )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0080, "2" )
	PORT_DIPSETTING(      0x00c0, "3" )
	PORT_DIPSETTING(      0x0040, "5" )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPSETTING(      0x0200, "Horizontally" )
	PORT_DIPSETTING(      0x0100, "Vertically" )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END

/* Pop's Pop's */

INPUT_PORTS_START( popspops )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// IN1 - $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	/* the dips on this are a mess.. service mode doesn't seem to be 100% trustable */
	PORT_START_TAG("IN2")	// $080004.w
	PORT_SERVICE( 0x0001, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0000, DEF_STR( Unknown ) ) // if ON it tells you the answers?!
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END



/***************************************************************************
                                Bubble 2000
***************************************************************************/

INPUT_PORTS_START( bubl2000 )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN2")	// $080004.w
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00c0, 0x00c0, "Free Credit" )
	PORT_DIPSETTING(      0x0080, "500k" )
	PORT_DIPSETTING(      0x00c0, "800k" )
	PORT_DIPSETTING(      0x0040, "1000k" )
	PORT_DIPSETTING(      0x0000, "1500k" )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( On ) )
	PORT_DIPNAME( 0x1c00, 0x1c00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x1c00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x1400, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 1C_4C ) )
//  PORT_DIPSETTING(      0x0000, "Disabled" )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 1C_4C ) )
//  PORT_DIPSETTING(      0x0000, "Disabled" )
INPUT_PORTS_END

/***************************************************************************
                                Mang Chi
***************************************************************************/

INPUT_PORTS_START( mangchi )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN2")	// $080004.w
    PORT_DIPNAME( 0x0001, 0x0001, "DSWS" )		/* Setting to on cuases screen issues, Flip Screen? or unfinished test mode? */
    PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
    PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
    PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0018, 0x0018, "Vs Rounds" )
	PORT_DIPSETTING(      0x0018, "2" )
	PORT_DIPSETTING(      0x0010, "3" )
	PORT_DIPSETTING(      0x0008, "4" )
	PORT_DIPSETTING(      0x0000, "5" )
    PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
    PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
    PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
    PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
    PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
    PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
    PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
    PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
    PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
    PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
    PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, DEF_STR( Difficulty ) )	/* Hard to tell levels of difficulty by play :-( */
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END


/***************************************************************************
                                Fire Hawk
***************************************************************************/

INPUT_PORTS_START( firehawk )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN2") 	// $080004.w
	PORT_DIPNAME( 0x0001, 0x0001, "Show Dip-Switch Settings" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x000e, 0x000e, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Easy ) )
//  PORT_DIPSETTING(      0x000a, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x000e, DEF_STR( Normal ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Hardest ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "Number of Bombs" )
	PORT_DIPSETTING(      0x0020, "2" )
	PORT_DIPSETTING(      0x0000, "3" )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0080, "2" )
	PORT_DIPSETTING(      0x00c0, "3" )
	PORT_DIPSETTING(      0x0040, "4" )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Region ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( English ) )
	PORT_DIPSETTING(      0x0000, "China" )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, "Continue Coins" )
	PORT_DIPSETTING(      0x1800, "1 Coin" )
	PORT_DIPSETTING(      0x0800, "2 Coins" )
	PORT_DIPSETTING(      0x1000, "3 Coins" )
	PORT_DIPSETTING(      0x0000, "4 Coins" )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END


/***************************************************************************
                             Spectrum 2000
***************************************************************************/

INPUT_PORTS_START( spec2k )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN2") 	// $080004.w
	PORT_SERVICE( 0x0001, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, "Number of Bombs" )
	PORT_DIPSETTING(      0x0008, "2" )
	PORT_DIPSETTING(      0x0000, "3" )
	PORT_DIPNAME( 0x0010, 0x0010, "Copyright Notice" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0080, "2" )
	PORT_DIPSETTING(      0x00c0, "3" )
	PORT_DIPSETTING(      0x0040, "5" )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xe000, 0xe000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_3C ) )
INPUT_PORTS_END


/***************************************************************************
                                Twin Action
***************************************************************************/

INPUT_PORTS_START( twinactn )
	PORT_START_TAG("IN0")	// $080000.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_SERVICE_NO_TOGGLE(0x0020, IP_ACTIVE_LOW   )	// Test in service mode
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START_TAG("IN1")	// $080002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x0080, IP_ACTIVE_HIGH,IPT_UNKNOWN )	// Tested at boot
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x8000, IP_ACTIVE_HIGH,IPT_UNKNOWN )	// Tested at boot

	PORT_START_TAG("IN2")	// $080004.w
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x001c, 0x001c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0018, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x001c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0014, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x00e0, 0x00e0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x00c0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x00e0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0060, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x00a0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )

	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0xc000, 0xc000, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x4000, "2" )
	PORT_DIPSETTING(      0xc000, "3" )
	PORT_DIPSETTING(      0x8000, "4" )
	PORT_DIPSETTING(      0x0000, "5" )
INPUT_PORTS_END


/***************************************************************************


                            Graphics Layouts


***************************************************************************/

static const gfx_layout layout_8x8x4 =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1)	},
	{ STEP8(0,4)	},
	{ STEP8(0,8*4)	},
	8*8*4
};

static const gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1)	},
	{ STEP8(0,4),   STEP8(8*8*4*2,4)	},
	{ STEP8(0,8*4), STEP8(8*8*4*1,8*4)	},
	16*16*4
};

static const gfx_layout layout_16x16x8 =
{
	16,16,
	RGN_FRAC(1,2),
	8,
	{ STEP4(RGN_FRAC(0,2),1), STEP4(RGN_FRAC(1,2),1)	},
	{ STEP8(0,4),   STEP8(8*8*4*2,4)	},
	{ STEP8(0,8*4), STEP8(8*8*4*1,8*4)	},
	16*16*4
};


static const gfx_layout layout_16x16x4_swapped =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1)	},
	{ 4,0,12,8,20,16,28,24, 512+4,512+0,512+12,512+8,512+20,512+16,512+28,512+24},
	{ STEP8(0,8*4), STEP8(8*8*4*1,8*4)	},
	16*16*4
};

static const gfx_decode grdnstrm_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_16x16x4,	256*1, 16 }, // [0] Sprites
	{ REGION_GFX2, 0, &layout_16x16x8,	256*3, 16 }, // [1] Layer 0
	{ REGION_GFX3, 0, &layout_8x8x4,	256*2, 16 }, // [2] Layer 1
	{ -1 }
};

static const gfx_decode stagger1_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_16x16x4,	256*1, 16 }, // [0] Sprites
	{ REGION_GFX2, 0, &layout_16x16x4,	256*0, 16 }, // [1] Layer 0
	{ REGION_GFX3, 0, &layout_8x8x4,	256*2, 16 }, // [2] Layer 1
	{ -1 }
};

static const gfx_decode redhawkb_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_16x16x4_swapped,	256*1, 16 }, // [0] Sprites
	{ REGION_GFX2, 0, &layout_16x16x4_swapped,	256*0, 16 }, // [1] Layer 0
	{ REGION_GFX3, 0, &layout_8x8x4,			256*2, 16 }, // [2] Layer 1
	{ -1 }
};


/***************************************************************************


                                Machine Drivers


***************************************************************************/

static void irq_handler(int irq)
{
	cpunum_set_input_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2151interface afega_ym2151_intf =
{
	irq_handler
};

INTERRUPT_GEN( interrupt_afega )
{
	switch ( cpu_getiloops() )
	{
		case 0:		irq2_line_hold();	break;
		case 1:		irq4_line_hold();	break;
	}
}

static MACHINE_DRIVER_START( stagger1 )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,12000000)
	MDRV_CPU_PROGRAM_MAP(afega,0)
	MDRV_CPU_VBLANK_INT(interrupt_afega,2)

	MDRV_CPU_ADD(Z80, 4000000)
	/* audio CPU */	/* ? */
	MDRV_CPU_PROGRAM_MAP(afega_sound_cpu,0)

	MDRV_FRAMES_PER_SECOND(56)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0, 256-1)	/* fixed */
	MDRV_GFXDECODE(stagger1_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(768)

	MDRV_VIDEO_START(afega)
	MDRV_VIDEO_UPDATE(afega)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2151, 4000000)
	MDRV_SOUND_CONFIG(afega_ym2151_intf)
	MDRV_SOUND_ROUTE(0, "left", 0.30)
	MDRV_SOUND_ROUTE(1, "right", 0.30)

	MDRV_SOUND_ADD(OKIM6295, 1000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.70)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.70)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( redhawkb )
	/* basic machine hardware */
	MDRV_IMPORT_FROM(stagger1)
	/* video hardware */
	MDRV_GFXDECODE(redhawkb_gfxdecodeinfo)
	MDRV_VIDEO_UPDATE(redhawkb)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( grdnstrm )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(stagger1)

	/* video hardware */
	MDRV_GFXDECODE(grdnstrm_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(768)
	MDRV_COLORTABLE_LENGTH(768 + 16*256)

	MDRV_PALETTE_INIT(grdnstrm)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( popspops )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(stagger1)

	/* video hardware */
	MDRV_GFXDECODE(grdnstrm_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(768)
	MDRV_COLORTABLE_LENGTH(768 + 16*256)

	MDRV_PALETTE_INIT(grdnstrm)

	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_VIDEO_UPDATE(bubl2000)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( bubl2000 )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(grdnstrm)

	/* video hardware */
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)	/* added to do not break other games */
	MDRV_VIDEO_UPDATE(bubl2000)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( firehawk )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,12000000)
	MDRV_CPU_PROGRAM_MAP(afega,0)
	MDRV_CPU_VBLANK_INT(interrupt_afega,2)

	MDRV_CPU_ADD(Z80,4000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(firehawk_sound_cpu,0)

	MDRV_FRAMES_PER_SECOND(56)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(8, 256-8-1, 16, 256-16-1)
	MDRV_GFXDECODE(grdnstrm_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(768)
	MDRV_COLORTABLE_LENGTH(768 + 16*256)

	MDRV_PALETTE_INIT(grdnstrm)
	MDRV_VIDEO_START(firehawk)
	MDRV_VIDEO_UPDATE(firehawk)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 1000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(OKIM6295, 1000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


INTERRUPT_GEN( interrupt_twinactn )
{
	switch ( cpu_getiloops() )
	{
		case 0:		irq1_line_hold();	break;
		case 1:		irq2_line_hold();	break;
		case 2:		irq4_line_hold();	break;
	}
}

static MACHINE_DRIVER_START( twinactn )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,12000000)
	MDRV_CPU_PROGRAM_MAP(twinactn,0)
	MDRV_CPU_VBLANK_INT(interrupt_twinactn,3)

	MDRV_CPU_ADD(Z80, 4000000)
	/* audio CPU */	/* ? */
	MDRV_CPU_PROGRAM_MAP(twinactn_sound_cpu,0)

	MDRV_FRAMES_PER_SECOND(56)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(stagger1_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(768)

	MDRV_VIDEO_START(afega)
	MDRV_VIDEO_UPDATE(twinactn)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 1000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


/***************************************************************************


                                ROMs Loading


***************************************************************************/

/* Address lines scrambling */

static void decryptcode( int a23, int a22, int a21, int a20, int a19, int a18, int a17, int a16, int a15, int a14, int a13, int a12,
	int a11, int a10, int a9, int a8, int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0 )
{
	int i;
	UINT8 *RAM = memory_region( REGION_CPU1 );
	size_t  size = memory_region_length( REGION_CPU1 );
	UINT8 *buffer = malloc( size );

	if( buffer )
	{
		memcpy( buffer, RAM, size );
		for( i = 0; i < size; i++ )
		{
			RAM[ i ] = buffer[ BITSWAP24( i, a23, a22, a21, a20, a19, a18, a17, a16, a15, a14, a13, a12,
				a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, a0 ) ];
		}
		free( buffer );
	}
}

/***************************************************************************

                                    Stagger I
(AFEGA 1998)

Parts:

1 MC68HC000P10
1 Z80
2 Lattice ispLSI 1032E

***************************************************************************/

ROM_START( stagger1 )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "2.bin", 0x000000, 0x020000, CRC(8555929b) SHA1(b405d81c2a45191111b1a4458ac6b5c0a129b8f1) )
	ROM_LOAD16_BYTE( "3.bin", 0x000001, 0x020000, CRC(5b0b63ac) SHA1(239f793b6845a88d1630da790a2762da730a450d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "1.bin", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD16_BYTE( "7.bin", 0x00000, 0x80000, CRC(048f7683) SHA1(7235b7dcfbb72abf44e60b114e3f504f16d29ebf) )
	ROM_LOAD16_BYTE( "6.bin", 0x00001, 0x80000, CRC(051d4a77) SHA1(664182748e72b3e44202caa20f337d02e946ca62) )

	ROM_REGION( 0x080000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x4 */
	ROM_LOAD( "4.bin", 0x00000, 0x80000, CRC(46463d36) SHA1(4265bc4d24ff64e39d9273965701c740d7e3fee0) )

	ROM_REGION( 0x00100, REGION_GFX3, ROMREGION_DISPOSE | ROMREGION_ERASEFF )	/* Layer 1, 8x8x4 */
	// Unused

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "5", 0x00000, 0x40000, CRC(e911ce33) SHA1(a29c4dea98a22235122303325c63c15fadd3431d) )
ROM_END


/***************************************************************************

                            Red Hawk (c)1997 Afega



    6116         ym2145(?)  MSM6295   5    4MHz
    1
    Z80        pLSI1032    4
                                      76C88
       6116   76C256                  76C88
       6116   76C256
    2  76C256 76C256
    3  76C256 76C256

    68000-10        6116
                    6116
   SW1                             6
   SW21                pLSI1032    7
    12MHz

***************************************************************************/

ROM_START( redhawk )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "2", 0x000000, 0x020000, CRC(3ef5f326) SHA1(e89c7c24a05886a14995d7c399958dc00ad35d63) )
	ROM_LOAD16_BYTE( "3", 0x000001, 0x020000, CRC(9b3a10ef) SHA1(d03480329b23474e5a9e42a75b09d2140eed4443) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "1.bin", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD16_BYTE( "6", 0x000001, 0x080000, CRC(61560164) SHA1(d727ab2d037dab40745dec9c4389744534fdf07d) )
	ROM_LOAD16_BYTE( "7", 0x000000, 0x080000, CRC(66a8976d) SHA1(dd9b89cf29eb5557845599d55ef3a15f53c070a4) )

	ROM_REGION( 0x080000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "4", 0x000000, 0x080000, CRC(d6427b8a) SHA1(556de1b5ce29d1c3c54bb315dcaa4dd0848ca462) )

	ROM_REGION( 0x00100, REGION_GFX3, ROMREGION_DISPOSE | ROMREGION_ERASEFF )	/* Layer 1, 8x8x4 */
	// Unused

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "5", 0x00000, 0x40000, CRC(e911ce33) SHA1(a29c4dea98a22235122303325c63c15fadd3431d) )
ROM_END

static DRIVER_INIT( redhawk )
{
	decryptcode( 23, 22, 21, 20, 19, 18, 16, 15, 14, 17, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 );
}

ROM_START( redhawkb )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "rhb-1.bin", 0x000000, 0x020000, CRC(e733ea07) SHA1(b1ffeda633d5e701f0e97c79930a54d7b89a85c5) )
	ROM_LOAD16_BYTE( "rhb-2.bin", 0x000001, 0x020000, CRC(f9fa5684) SHA1(057ea3eebbaa1a208a72beef21b9368df7032ce1) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "1.bin", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD( "rhb-3.bin", 0x000000, 0x080000, CRC(0318d68b) SHA1(c773de7b6f9c706e62349dc73af4339d1a3f9af6) )
	ROM_LOAD( "rhb-4.bin", 0x080000, 0x080000, CRC(ba21c1ef) SHA1(66b0dee67acb5b3a21c7dba057be4093a92e10a9) )

	ROM_REGION( 0x080000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "rhb-5.bin", 0x000000, 0x080000, CRC(d0eaf6f2) SHA1(6e946e13b06df897a63e885c9842816ec908a709) )

	ROM_REGION( 0x080000, REGION_GFX3, ROMREGION_DISPOSE | ROMREGION_ERASEFF )	/* Layer 1, 8x8x4 */

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "5", 0x00000, 0x40000, CRC(e911ce33) SHA1(a29c4dea98a22235122303325c63c15fadd3431d) )
ROM_END


/***************************************************************************

                            Sen Jin - Guardian Storm

(C) Afega 1998

CPU: 68HC000FN10 (68000, 68 pin PLCC)
SND: Z84C000FEC (Z80, 44 pin PQFP), AD-65 (OKI M6295),
     BS901 (YM2151, 24 pin DIP), BS901 (YM3012, 16 pin DIP)
OSC: 12.000MHz (near 68000), 4.000MHz (Near Z84000)
RAM: LH52B256 x 8, 6116 x 7
DIPS: 2 x 8 position

Other Chips: AFEGA AFI-GFSK (68 pin PLCC, located next to 68000)
             AFEGA AFI-GFLK (208 pin PQFP)

ROMS:
GST-01.U92   27C512, \
GST-02.U95   27C2000  > Sound Related, all located near Z80
GST-03.U4    27C512  /

GST-04.112   27C2000 \
GST-05.107   27C2000  /Main Program

GST-06.C13   read as 27C160  label = AF1-SP (Sprites?)
GST-07.C08   read as 27C160  label = AF1=B2 (Backgrounds?)
GST-08.C03   read as 27C160  label = AF1=B1 (Backgrounds?)

***************************************************************************/

ROM_START( grdnstrm )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "gst-04.112", 0x000000, 0x040000, CRC(922c931a) SHA1(1d1511033c8c424535a73f5c5bf58560a8b1842e) )
	ROM_LOAD16_BYTE( "gst-05.107", 0x000001, 0x040000, CRC(d22ca2dc) SHA1(fa21c8ec804570d64f4b167b7f65fd5811435e46) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "gst-01.u92", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD( "gst-06.c13", 0x000000, 0x200000, CRC(7d4d4985) SHA1(15c6c1aecd3f12050c1db2376f929f1a26a1d1cf) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "gst-07.c08", 0x000000, 0x200000, CRC(d68588c2) SHA1(c5f397d74a6ecfd2e375082f82e37c5a330fba62) )
	ROM_LOAD( "gst-08.c03", 0x200000, 0x200000, CRC(f8b200a8) SHA1(a6c43dd57b752d87138d7125b47dc0df83df8987) )

	ROM_REGION( 0x10000, REGION_GFX3, ROMREGION_DISPOSE )	/* Layer 1, 8x8x4 */
	ROM_LOAD( "gst-03.u4",  0x00000, 0x10000, CRC(a1347297) SHA1(583f4da991eeedeb523cf4fa3b6900d40e342063) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "gst-02.u95", 0x00000, 0x40000, CRC(e911ce33) SHA1(a29c4dea98a22235122303325c63c15fadd3431d) )
ROM_END

static DRIVER_INIT( grdnstrm )
{
	decryptcode( 23, 22, 21, 20, 19, 18, 16, 17, 14, 15, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 );
}

/* Pop's Pop's */

/*

Pop's Pop's by Afega (1999)

The pcb might be missing an eprom in a socket
  --- i just think it uses a generic PCB but no sprites in this case,.

1x 68k
1x z80
1x Ad65 (oki 6295)
1x OSC 12mhz (near 68k)
1x OSC 4mhz (near z80)
1x ym2151
1x Afega AF1-CFLK custom chip Smt
1x Afega AF1-CF5K custom chip socketed
2x dipswitch banks

*/

ROM_START( popspops )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "afega4.u112", 0x000000, 0x040000, CRC(db191762) SHA1(901fdc20374473127d694513d4291e29e65eafe8) )
	ROM_LOAD16_BYTE( "afega5.u107", 0x000001, 0x040000, CRC(17e0c48b) SHA1(833c61c4b3ee293b0bcddfa86dfa9c1014375115) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "afega1.u92", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) )

	ROM_REGION( 0x400000, REGION_GFX1, ROMREGION_ERASEFF | ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	/* no sprite roms? */

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "afega6.uc8", 0x000000, 0x200000, CRC(6d506c97) SHA1(4909c0b530f9526c8bf76e502c914ef10a50d1fc) )
	ROM_LOAD( "afega7.uc3", 0x200000, 0x200000, CRC(02d7f9de) SHA1(10102ffbf37a57afa300b01cb5067b7e672f4999) )

	ROM_REGION( 0x10000, REGION_GFX3, ROMREGION_DISPOSE )	/* Layer 1, 8x8x4 */
	ROM_LOAD( "afega3.u4",  0x00000, 0x10000, CRC(f39dd5d2) SHA1(80d05d57a621b0063f63ce05be9314f718b3c111) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "afega2.u95", 0x00000, 0x40000, CRC(ecd8eeac) SHA1(849beba8f04cc322bb8435fa4c26551a6d0dec64) )
ROM_END

/*
Mang-chi by Afega

1x osc 4mhz
1x osc 12mhz
1x tmp68hc0000p-10
1x z80c006
1x AD65 (MSM6295)
1x CY5001 (YM2151 rebadged)
2x dipswitch
1x fpga
1x smd ASIC not marked

Dumped by Corrado Tomaselli
*/

ROM_START( mangchi )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "afega9.u112", 0x00000, 0x40000, CRC(0b1517a5) SHA1(50e307641759bb2a35aff56ef9598364740803a0) )
	ROM_LOAD16_BYTE( "afega10.u107", 0x00001, 0x40000, CRC(b1d0f33d) SHA1(68b5be3f7911f7299566c5bf5801e90099433613) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "sound.u92", 0x00000, 0x10000, CRC(bec4f9aa) SHA1(18fb2ee06892983c117a62b70cd72a98f60a08b6) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD16_BYTE( "afega6.uc11", 0x000000, 0x040000, CRC(979efc30) SHA1(227fe1e20137253aac04585d2bbf67091d032e56) )
	ROM_LOAD16_BYTE( "afega7.uc14", 0x000001, 0x040000, CRC(c5cbcc38) SHA1(86070a9598e80f90ec7892d623e1a975ccc68178) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "afega5.uc6",  0x000000, 0x80000, CRC(c73261e0) SHA1(0bb66aa315aaecb26169812cf47a6504a74f0db5) )
	ROM_LOAD( "afega4.uc1",  0x080000, 0x80000, CRC(73940917) SHA1(070305c81de959c9d00b6cf1cc20bbafa204976a) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE | ROMREGION_ERASEFF )	/* Layer 1, 8x8x4 */

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "afega2.u95", 0x00000, 0x40000, CRC(78c8c1f9) SHA1(eee0d03164a0ac0ddc5186ab56090320e9d33aa7) )
ROM_END

/***************************************************************************

                            Bubble 2000 (c)1998 Tuning

Bubble 2000
Tuning, 1998

CPU   : TMP68HC000P-10 (68000)
SOUND : Z840006 (Z80, 44 pin QFP), YM2151, OKI M6295
OSC   : 4.000MHZ, 12.000MHz
DIPSW : 8 position (x2)
RAM   : 6116 (x5, gfx related?) 6116 (x1, sound program ram), 6116 (x1, near rom3)
        64256 (x4, gfx related?), 62256 (x2, main program ram), 6264 (x2, gfx related?)
PALs/PROMs: None
Custom: Unknown 208 pin QFP labelled LTC2 (Graphics generator)
        Unknown 68 pin PLCC labelled LTC1 (?, near rom 2 and rom 3)
ROMs  :

Filename    Type        Possible Use
----------------------------------------------
rom01.92    27C512      Sound Program
rom02.95    27C020      Oki Samples
rom03.4     27C512      ? (located near rom 1 and 2 and near LTC1)
rom04.1     27C040   \
rom05.3     27C040    |
rom06.6     27C040    |
rom07.9     27C040    | Gfx
rom08.11    27C040    |
rom09.14    27C040    |
rom12.2     27C040    |
rom13.7     27C040   /

rom10.112   27C040   \  Main Program
rom11.107   27C040   /




***************************************************************************/

ROM_START( bubl2000 )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "rom10.112", 0x00000, 0x20000, CRC(87f960d7) SHA1(d22fe1740217ac20963bd9003245850598ccecf2) )
	ROM_LOAD16_BYTE( "rom11.107", 0x00001, 0x20000, CRC(b386041a) SHA1(cac36e22a39b5be0c5cd54dce5c912ff811edb28) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "rom01.92", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) ) /* same as the other games on this driver */

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD16_BYTE( "rom08.11", 0x000000, 0x040000, CRC(519dfd82) SHA1(116b06f6e7b283a5417338f716bbaab6cfadb41d) )
	ROM_LOAD16_BYTE( "rom09.14", 0x000001, 0x040000, CRC(04fcb5c6) SHA1(7594fa6bf98fc01b8848473a222a621c7c9ff00d) )

	ROM_REGION( 0x300000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "rom06.6",  0x000000, 0x080000, CRC(ac1aabf5) SHA1(abce6ba381b189ab3ec703a8ef74bccbe10876e0) )
	ROM_LOAD( "rom07.9",  0x080000, 0x080000, CRC(69aff769) SHA1(89b98c1023710861e622c8a186b6ec48f5109d42) )
	ROM_LOAD( "rom13.7",  0x100000, 0x080000, CRC(3a5b7226) SHA1(1127740c5bc2f830d73a77c8831e1b0db6606375) )
	ROM_LOAD( "rom04.1",  0x180000, 0x080000, CRC(46acd054) SHA1(1bd7a1b6b2ce6a3daa8c92843c546beb377af8fb) )
	ROM_LOAD( "rom05.3",  0x200000, 0x080000, CRC(37deb6a1) SHA1(3a8a3d961800bb15fd389429b92fa1e5b5f416df) )
	ROM_LOAD( "rom12.2",  0x280000, 0x080000, CRC(1fdc59dd) SHA1(d38e21c878241b4315a36e0590397211ca63f2c4) )

	ROM_REGION( 0x10000, REGION_GFX3, ROMREGION_DISPOSE )	/* Layer 1, 8x8x4 */
	ROM_LOAD( "rom03.4",  0x00000, 0x10000, CRC(f4c15588) SHA1(a21ae71c0a8c7c1df63f9905fd86303bc2d3991c) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "rom02.95", 0x00000, 0x40000, CRC(859a86e5) SHA1(7b51964227411a40aac54b9cd9ff64f091bdf2b0) )
ROM_END


/*

Hot Bubble
Afega, 1998

PCB Layout
----------

Bottom Board

|------------------------------------------|
| BS902  BS901   Z80                  4MHz |
|                                          |
|        6116    6295                      |
|                                   62256  |
|      6116                         62256  |
|      6116                                |
|J           6116           |------------| |
|A           6116           |   68000    | |
|M                          |------------| |
|M  DSW2   6264                            |
|A         6264                            |
|                                          |
|         |-------|                        |
|         |       |                        |
|         |       |                        |
| DSW1    |       |     62256   62256      |
|         |-------|                        |
|              6116     62256   62256      |
|12MHz         6116                        |
|------------------------------------------|
Notes:
      68000 - running at 12.000MHz
      Z80   - running at 4.000MHz
      62256 - 32K x8 SRAM
      6264  - 8K x8 SRAM
      6116  - 2K x8 SRAM
      BS901 - YM2151, running at 4.000MHz
      BS902 - YM3012
      6295  - OKI MSM6295 running at 1.000MHz [4/4], sample rate = 1000000 / 132
      *     - Unknown QFP208
      VSync - 56.2Hz (measured on 68000 IPL1)

Top Board

|---------------------------|
|                           |
|   S1     S2        T1     |
|                           |
|   CR5    CR7       C1     |
|                           |
|   CR6   +CR8       C2     |
|                           |
|          BR1       BR3    |
|                           |
|         +BR2      +BR4    |
|                           |
|  CR1     CR3     |------| |
|                  |  *   | |
|  CR2    +CR4     |      | |
|                  |------| |
|---------------------------|
Notes:
      * - Unknown PLCC68 IC
      + - Not populated

*/

ROM_START( hotbubl )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "c1.uc1",  0x00001, 0x40000, CRC(7bb240e9) SHA1(99048fa275182c3da3bfb0dedd790f4b5858bd92) )
	ROM_LOAD16_BYTE( "c2.uc9",  0x00000, 0x40000, CRC(7917b95d) SHA1(0344bae9c373c5943e7693720e5e531bc2e0d7ee) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "s1.uc14", 0x00000, 0x10000, CRC(5d8cf28e) SHA1(2a440bf5136f95af137b6688e566a14e65be94b1) ) /* same as the other games on this driver */

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD16_BYTE( "br1.uc3",  0x000000, 0x080000, CRC(6fc18de4) SHA1(57b4823fc41637780f64eadd1ddf61db531a2599) )
	ROM_LOAD16_BYTE( "br3.uc10", 0x000001, 0x080000, CRC(bb677240) SHA1(d7a26bcd33d491cee441edda6d092a1d08308b0e) )

	ROM_REGION( 0x300000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "cr6.uc16",  0x100000, 0x080000, CRC(99d6523c) SHA1(0b628585d749e175d5a4dc600af1ba9cb936bfeb) )
	ROM_LOAD( "cr7.uc19",  0x080000, 0x080000, CRC(a89d9ce4) SHA1(5965b2b4b67bc91bc0e7474e593c7e1953b75adc) )
	ROM_LOAD( "cr5.uc15",  0x000000, 0x080000, CRC(65bd5159) SHA1(627ccc0ab131e643c3c52ee9bb41c7a85153c35e) )

	ROM_LOAD( "cr2.uc7",  0x280000, 0x080000, CRC(27ad6fc8) SHA1(00b1a5c5e1a245590b300b9baf71585d41813e3e) )
	ROM_LOAD( "cr3.uc12", 0x200000, 0x080000, CRC(c841a4f6) SHA1(9b0ee5623c87a0cfc63d3741a65d399bd6593f18) )
	ROM_LOAD( "cr1.uc6",  0x180000, 0x080000, CRC(fc9101d2) SHA1(1d5b8484264b6d73fe032946096a469226cce901) )

	ROM_REGION( 0x10000, REGION_GFX3, ROMREGION_DISPOSE )	/* Layer 1, 8x8x4 */
	ROM_LOAD( "t1.uc2",  0x00000, 0x10000, CRC(ce683a93) SHA1(aeee2671051f1badf2255375cd7c5fa847d1746c) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "s2.uc18", 0x00000, 0x40000, CRC(401c980f) SHA1(e47710c47cfeecce3ccf87f845b219a9c9f21ee3) )
ROM_END

static DRIVER_INIT( bubl2000 )
{
	decryptcode( 23, 22, 21, 20, 19, 18, 13, 14, 15, 16, 17, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 );
}

/*

Fire Hawk - ESD
---------------

- To enter test mode, hold on button 1 at boot up


PCB Layout
----------

ESD-PROT-002
|------------------------------------------------|
|      FHAWK_S1.U40   FHAWK_S2.U36               |
|      6116     6295  FHAWK_S3.U41               |
|               6295                 FHAWK_G1.UC6|
|    PAL   Z80                       FHAWK_G2.UC5|
|    4MHz                             |--------| |
|                                     | ACTEL  | |
|J   6116             62256           |A54SX16A| |
|A   6116             62256           |        | |
|M                                    |(QFP208)| |
|M                                    |--------| |
|A     DSW1              FHAWK_G3.UC2            |
|      DSW2                           |--------| |
|      DSW3                           | ACTEL  | |
|                     6116            |A54SX16A| |
|                     6116            |        | |
|      62256                          |(QFP208)| |
| FHAWK_P1.U59                        |--------| |
| FHAWK_P2.U60  PAL                  62256  62256|
|                                                |
|12MHz 62256   68000                 62256  62256|
|------------------------------------------------|
Notes:
      68000 clock: 12.000MHz
        Z80 clock: 4.000MHz
      6295 clocks: 1.000MHz (both), sample rate = 1000000 / 132 (both)
            VSync: 56Hz

*/

ROM_START( firehawk )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 68000 Code */
	ROM_LOAD16_BYTE( "fhawk_p1.u59", 0x00001, 0x80000, CRC(d6d71a50) SHA1(e947720a0600d049b7ea9486442e1ba5582536c2) )
	ROM_LOAD16_BYTE( "fhawk_p2.u60", 0x00000, 0x80000, CRC(9f35d245) SHA1(5a22146f16bff7db924550970ed2a3048bc3edab) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 )	/* Z80 Code */
	ROM_LOAD( "fhawk_s1.u40", 0x00000, 0x20000, CRC(c6609c39) SHA1(fe9b5f6c3ab42c48cb493fecb1181901efabdb58) )

	ROM_REGION( 0x200000, REGION_GFX1,ROMREGION_DISPOSE ) /* Sprites, 16x16x4 */
	ROM_LOAD( "fhawk_g3.uc2", 0x00000, 0x200000,  CRC(cae72ff4) SHA1(7dca7164015228ea039deffd234778d0133971ab) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE ) /* Layer 0, 16x16x8 */
	ROM_LOAD( "fhawk_g1.uc6", 0x000000, 0x200000, CRC(2ab0b06b) SHA1(25362f6a517f188c62bac28b1a7b7b49622b1518) )
	ROM_LOAD( "fhawk_g2.uc5", 0x200000, 0x200000, CRC(d11bfa20) SHA1(15142004ab49f7f1e666098211dff0835c61df8d) )

	ROM_REGION( 0x00100, REGION_GFX3, ROMREGION_DISPOSE | ROMREGION_ERASEFF )	/* Layer 1, 8x8x4 */
	// Unused

	ROM_REGION( 0x040000, REGION_SOUND1, 0 ) /* Samples */
	ROM_LOAD( "fhawk_s2.u36", 0x00000, 0x40000, CRC(d16aaaad) SHA1(96ca173ca433164ed0ae51b41b42343bd3cfb5fe) )

	ROM_REGION( 0x040000, REGION_SOUND2, 0 ) /* Samples */
	ROM_LOAD( "fhawk_s3.u41", 0x00000, 0x40000, CRC(3fdcfac2) SHA1(c331f2ea6fd682cfb00f73f9a5b995408eaab5cf) )
ROM_END

/*

CPU
1x unknown custom chip
1x FPGA
1x oscillator 4.000MHz
1x oscillator 12.000MHz
2x AD-65

ROMs
1x TMS27C512 (1)
1x MX27C4000 (3)
2x TMS27C010A (2,4)
2x TMS27C020 (5,6)
3x MX29F1610ML (uc1,uc2,uc3)

Note
1x JAMMA connector
1x trimmer (volume)
2x8 dip switches

*/

ROM_START( spec2k )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 68000 Code */
	ROM_LOAD16_BYTE( "yonatech5.u124", 0x00000, 0x40000, CRC(72ab5c05) SHA1(182a811982b89b8cda0677547ef0625c274f5c6b) )
	ROM_LOAD16_BYTE( "yonatech6.u120", 0x00001, 0x40000, CRC(7e44bd9c) SHA1(da59685be14a09ec037743fcec34fb293f7d588d) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 )	/* Z80 Code */
	ROM_LOAD( "yonatech1.u103", 0x00000, 0x10000, CRC(ef5acda7) SHA1(e55b36a1598ecbbbad984997d61599dfa3958f60) )

	ROM_REGION( 0x200000, REGION_GFX1,ROMREGION_DISPOSE ) /* Sprites, 16x16x4 */
	ROM_LOAD( "u154.bin", 0x00000, 0x200000, CRC(f77b764e) SHA1(37e249bd4d7174c5232261880ce8debf42723716) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE ) /* Layer 0, 16x16x8 */
	ROM_LOAD( "u153.bin", 0x000000, 0x200000, CRC(a00bbf8f) SHA1(622f52ef50d52cdd5e6b250d68439caae5c13404) )
	ROM_LOAD( "u152.bin", 0x200000, 0x200000, CRC(f6423fab) SHA1(253e0791eb58efa1df42e9c74d397e6e65c8c252) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE | ROMREGION_ERASEFF )	/* Layer 1, 8x8x4 */
	ROM_LOAD( "yonatech4.u3", 0x00000, 0x20000, CRC(5626b08e) SHA1(63207ed6b4fc8684690bf3fe1991a4f3babd73e8) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 ) /* Samples */
	ROM_LOAD( "yonatech2.u101", 0x00000, 0x20000, CRC(4160f172) SHA1(0478a5a4bbba115e6cfb5501aa55aa2836c963bf) )

	ROM_REGION( 0x080000, REGION_SOUND2, 0 ) /* Samples */
	ROM_LOAD( "yonatech3.u106", 0x00000, 0x80000, CRC(6644c404) SHA1(b7ad3f9f08971432d024ef8be3fa3140f0bbae67) )
ROM_END

static DRIVER_INIT( spec2k )
{
	decryptcode( 23, 22, 21, 20,
		         19, 18, 17, 13,
	             14, 15, 16, 12,
		         11, 10, 9,  8,
		         7,  6,  5,  4,
	             3,  2,  1,  0 );
}

/*
    1995, Afega

    1x TMP68000P-10 (main)
    1x GOLDSTAR Z8400A (sound)
    1x AD-65 (equivalent to OKI6295)
    1x LATTICE pLSI 1032 60LJ A428A48
    1x oscillator 8.000MHz
    1x oscillator 12.000MHz

    1x 27256 (SU6)
    1x 27C010 (SU12)
    1x 27C020 (SU13)
    2x 27c4001 (UB11, UB13)
    3x 27C010 (UJ11, UJ12, UJ13)
    1x 27C4001 (UI20)

    1x JAMMA edge connector
    1x trimmer (volume)
*/

ROM_START( twinactn )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "afega.uj13", 0x00000, 0x20000, CRC(9187701d) SHA1(1da8d1e3969f60c7b0521cd22c723cb51619df9d) )
	ROM_LOAD16_BYTE( "afega.uj12", 0x00001, 0x20000, CRC(fe8cff9c) SHA1(a1a04deff9e2cb54c69601898cf4e5133c2bc437) )

	ROM_REGION( 0x8000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "afega.su6", 0x0000, 0x8000, CRC(3a52dc88) SHA1(87941987d34d93df6df9ff33ccfbd1f5d4a39c51) )	// 1111xxxxxxxxxxx = 0x00

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )	/* Sprites, 16x16x4 */
	ROM_LOAD16_BYTE( "afega.ub11", 0x00000, 0x80000, CRC(287f20d8) SHA1(11faa36b97593c0b5cee70343750ae1ecd2f5b71) )
	ROM_LOAD16_BYTE( "afega.ub13", 0x00001, 0x80000, CRC(f525f819) SHA1(78ffcb709a3a900d3851392630a11ab58fc0bc75) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 0, 16x16x8 */
	ROM_LOAD( "afega.ui20", 0x00000, 0x80000, CRC(237c8f92) SHA1(bb3131b450bd78d03b789626a465fb9e7a4604a7) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* Layer 1, 8x8x4 */
	ROM_LOAD( "afega.uj11", 0x00000, 0x20000, CRC(3f439e92) SHA1(27e5b1b0aa3b13fa35e3f83793037314b2942aa2) )

	ROM_REGION( 0x100000, REGION_SOUND1, 0 )	/* Samples */
	ROM_LOAD( "afega.su12", 0x000000, 0x20000, CRC(91d665f3) SHA1(10b5b07ed28ea78b6d3493afc03e003a8468c007) )
	ROM_RELOAD(             0x040000, 0x20000 )
	ROM_RELOAD(             0x080000, 0x20000 )
	ROM_RELOAD(             0x0c0000, 0x20000 )
	ROM_RELOAD(             0x020000, 0x20000 )
	ROM_RELOAD(             0x060000, 0x20000 )
	ROM_LOAD( "afega.su13", 0x0a0000, 0x20000, CRC(30e1c306) SHA1(c859f11fd329793b11e96264e91c79a557b488a4) )
	ROM_CONTINUE(           0x0e0000, 0x20000 )
ROM_END

static WRITE16_HANDLER ( test_2a_mustang_w )
{
	data = data >> 8;

	ram[0x2a/2] = (data << 8) | data;

	if (data == 1 || data == 2) {
		ram[0x2e/2] = 10;
		ram[0x2c/2] = 0;
	}

}

static DRIVER_INIT( twinactn )
{
#if 0
	UINT16 *rom = (UINT16 *) memory_region(REGION_CPU1);
	rom[0x4a2a/2]	=	0x4e71;	// no freeze on sound test + enable hidden "object test"
#endif
	UINT16 *rom = (UINT16 *)memory_region(REGION_CPU1);

	// these are the patches for mustang from nmk16.c modified for twinactn
	// we need to figure out how this _really_ works
	rom[0x85c/2] = 0x4e71; // same as mustang
	rom[0x85e/2] = 0x4e71; // same as mustang
	rom[0x860/2] = 0x4e71; // same as mustang
	rom[0x8c0/2] = 0x0300; // same as mustang
	rom[0xbee/2] = 0x0300; // offset shifted..
	rom[0x30c0/2] = 0x0300; // offset shifted..

	memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0xf902a, 0xf902b, 0, 0, test_2a_mustang_w );
}


/***************************************************************************


                                Game Drivers


***************************************************************************/

GAME( 1995, twinactn, 0,        twinactn, twinactn, twinactn, ROT0,               "Afega",    "Twin Action",                      0 ,0)
GAME( 1997, redhawk,  stagger1, stagger1, stagger1, redhawk,  ROT270,             "Afega",    "Red Hawk (US)",                    GAME_NOT_WORKING ,0)
GAME( 1997, redhawkb, stagger1, redhawkb, redhawkb, 0,        ROT0,               "bootleg",  "Red Hawk (bootleg)",               GAME_NOT_WORKING ,0)
GAME( 1998, stagger1, 0,        stagger1, stagger1, 0,        ROT270,             "Afega",    "Stagger I (Japan)",                GAME_NOT_WORKING ,0)
GAME( 1998, grdnstrm, 0,        grdnstrm, grdnstrm, grdnstrm, ROT270,             "Afega",    "Sen Jin - Guardian Storm (Korea)", GAME_NOT_WORKING ,0)
GAME( 1998, bubl2000, 0,        bubl2000, bubl2000, bubl2000, ROT0,               "Tuning",   "Bubble 2000",                      0 ,0) // on a tuning board (bootleg?)
GAME( 1998, hotbubl,  bubl2000, bubl2000, bubl2000, bubl2000, ROT0,               "Pandora",  "Hot Bubble" ,                      0 ,0) // on an afega board ..
GAME( 1999, popspops, 0,        popspops, popspops, grdnstrm, ROT0,               "Afega",    "Pop's Pop's",                      0 ,0)
GAME( 2000, mangchi,  0,        bubl2000, mangchi,  bubl2000, ROT0,               "Afega",    "Mang-Chi",                         0 ,0)
GAME( 2000, spec2k,   0,        firehawk, spec2k,   spec2k,   ORIENTATION_FLIP_Y, "Yonatech", "Spectrum 2000 (Euro)",             GAME_NOT_WORKING ,0)
GAME( 2001, firehawk, 0,        firehawk, firehawk, 0,        ORIENTATION_FLIP_Y, "ESD",      "Fire Hawk",                        GAME_NOT_WORKING ,0)
