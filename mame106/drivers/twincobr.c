/****************************************************************************

        ToaPlan game hardware from 1987
        -------------------------------
        Driver by: Quench
        Flying Shark details: Carl-Henrik Skarstedt  &  Magnus Danielsson
        Flying Shark bootleg info: Ruben Panossian


Supported games:

    Toaplan Board Number:   TP-007
    Taito game number:      B02
        Flying Shark (World)
        Sky Shark (USA Romstar license)
        Hishou Zame (Flying Shark Japan license)
        Flying Shark bootleg (USA Romstar license)

    Toaplan Board Number:   TP-011
    Taito game number:      B30
        Twin Cobra (World)
        Twin Cobra (USA license)
        Kyukyoku Tiger (Japan license)

    Comad Board Number:     ??????
    Comad game number:      ???
        GulfWar II (Game play very similar to Twin cobra)

Difference between Twin Cobra and Kyukyoko Tiger:
    T.C. supports two simultaneous players.
    K.T. supports two players, but only one at a time.
         for this reason, it also supports Table Top cabinets.
    T.C. stores 3 characters for high scores.
    K.T. stores 6 characters for high scores.
    T.C. heros are Red and Blue for player 1 and 2 respectively.
    K.T. heros are grey for both players.
    T.C. dead remains of ground tanks are circular.
    K.T. dead remains of ground tanks always vary in shape.
    T.C. does not use DSW1-1 and DSW2-8.
    K.T. uses DSW1-1 for cabinet type, and DSW2-8 for allow game continue.
    T.C. continues new hero and continued game at current position.
    K.T. continues new hero and continued game at predefined positions.
         After dying, and your new hero appears, if you do not travel more
         than your helicopter length forward, you are penalised and moved
         back further when your next hero appears.
    K.T. Due to this difference in continue sequence, Kyukyoko Tiger is MUCH
         harder, challenging, and nearly impossible to complete !

**************************** Memory & I/O Maps *****************************
68000: Main CPU

00000-1ffff ROM for Flying Shark
00000-2ffff ROM for Twin Cobra
30000-33fff RAM shared with TMS320C10NL-14 protection microcontroller
40000-40fff RAM sprite display properties (co-ordinates, character, color - etc)
50000-50dff Palette RAM
7a000-7abff RAM shared with Z80; 16-bit on this side, 8-bit on Z80 side

read:
78001       DSW1 (Flying Shark)
78003       DSW2 (Flying Shark)

78005       Player 1 Joystick and Buttons input port
78007       Player 2 Joystick and Buttons input port
78009       bit 7 vblank, coin and control/service inputs (Flying shark)
                Flying Shark implements Tilt as 'freeze system' and also has
                a reset button, but its not implelemted here (not needed)

7e000-7e005 read data from video RAM (see below)

write:
60000-60003 CRT 6845 controller. 0 = register offset , 2 = register data
70000-70001 scroll   y   for character page (centre normally 0x01c9)
70002-70003 scroll < x > for character page (centre normally 0x00e2)
70004-70005 offset in character page to write character (7e000)

72000-72001 scroll   y   for foreground page (starts from     0x03c9)
72002-72003 scroll < x > for foreground page (centre normally 0x002a)
72004-72005 offset in character page to write character (7e002)

74000-74001 scroll   y   for background page (starts from     0x03c9)
74002-74003 scroll < x > for background page (centre normally 0x002a)
74004-74005 offset in character page to write character (7e004)

76000-76003 as above but for another layer maybe ??? (Not used here)
7800a       This activates INT line for Flying shark. (Not via 7800C)
            00      Activate INTerrupt line to the TMS320C10 DSP.
            01      Inhibit  INTerrupt line to the TMS320C10 DSP.

7800c       Control register (Byte write access).
            bits 7-4 always 0
            bits 3-1 select the control signal to drive.
            bit   0  is the value passed to the control signal.

            Value (hex):
            00-03   ????
            04      Clear IPL2 line to 68000 inactive hi (Interrupt priority 4)
            05      Set   IPL2 line to 68000 active  low (Interrupt priority 4)
            06      Dont flip display
            07      Flip display
            08      Switch to background layer ram bank 0
            09      Switch to background layer ram bank 1
            0A      Switch to foreground layer rom bank 0
            0B      Switch to foreground layer rom bank 1
            0C      Activate INTerrupt line to the TMS320C10 DSP  (Twin Cobra)
            0D      Inhibit  INTerrupt line to the TMS320C10 DSP  (Twin Cobra)
            0E      Turn screen off
            0F      Turn screen on

7e000-7e001 data to write in text video RAM (70000)
7e002-7e003 data to write in bg video RAM (72004)
7e004-7e005 data to write in fg video RAM (74004)

Z80: Sound CPU
0000-7fff ROM
8000-87ff shared with 68000; 8-bit on this side, 16-bit on 68000 side

in:
00        YM3812 status
10        Coin inputs and control/service inputs (Twin Cobra)
40        DSW1 (Twin Cobra)
50        DSW2 (Twin Cobra)

out:
00        YM3812 control
01        YM3812 data
20        Coin counters / Coin lockouts

TMS320C10 DSP: Harvard type architecture. RAM and ROM on seperate data buses.
0000-07ff ROM 16-bit opcodes (word access only). Moved to $8000-8fff for
                 MAME compatibility. View this ROM in the debugger at $8000h
0000-0090 Internal RAM (words).


in:
01        data read from addressed 68K address space (Main RAM/Sprite RAM)

out:
00        address of 68K to read/write to
01        data to write to addressed 68K address space (Main RAM/Sprite RAM)
03        bit 15 goes to BIO line of TMS320C10. BIO is a polled input line.


MCUs used with this hardware: (TMS320C10 in custom Toaplan/Taito disguise)

Twin Cobra                  Sky Shark                   Wardner
D70016U                     D70012U                     D70012U
GXC-04                      GXC-02                      GXC-02
MCU (delta) 74000           MCU 71400                   MCU (delta) 71900



68K writes the following to $30000 to tell DSP to do the following:
Twin  Kyukyoku
Cobra Tiger
00      00   do nothing
01      0C   run self test, and report DSP ROM checksum     from 68K PC:23CA6
02      07   control all enemy shots                        from 68K PC:23BFA
04      0B   start the enemy helicopters                    from 68K PC:23C66
05      08   check for colision with enemy fire ???         from 68K PC:23C20
06      09   check for colision with enemy ???              from 68K PC:23C44
07      01   control enemy helicopter shots                 from 68K PC:23AB2
08      02   control all ground enemy shots
0A      04   read hero position and send enemy to it ?      from 68K PC:23B58

03      0A  \
09      03   \ These functions within the DSP never seem to be called ????
0B      05   /
0C      06  /

68K writes the following to $30004 to tell DSP to do the following:
Flying  Hishou
Shark   Zame
00      00   do nothing
03      0B   Write sprite to sprite RAM
05      01   Get angle
06      02   Rotate towards direction
09      05   Check collision between 2 spheres!??
0A      06   Polar coordinates add
0B      07   run self test, and report DSP ROM checksum

01      09  \
02      0A   \
04      08    > These functions within the DSP never seem to be called ????
07      03   /
08      04  /
*****************************************************************************/


#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "cpu/tms32010/tms32010.h"
#include "twincobr.h"
#include "sound/3812intf.h"





/***************************** 68000 Memory Map *****************************/

static ADDRESS_MAP_START( main_program_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x02ffff) AM_ROM
	AM_RANGE(0x030000, 0x033fff) AM_RAM		/* 68K and DSP shared RAM */
	AM_RANGE(0x040000, 0x040fff) AM_RAM AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0x050000, 0x050dff) AM_READWRITE(paletteram16_word_r, paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x060000, 0x060001) AM_WRITE(twincobr_crtc_reg_sel_w)	/* 6845 CRT controller */
	AM_RANGE(0x060002, 0x060003) AM_WRITE(twincobr_crtc_data_w)		/* 6845 CRT controller */
	AM_RANGE(0x070000, 0x070003) AM_WRITE(twincobr_txscroll_w)	/* text layer scroll */
	AM_RANGE(0x070004, 0x070005) AM_WRITE(twincobr_txoffs_w)	/* offset in text video RAM */
	AM_RANGE(0x072000, 0x072003) AM_WRITE(twincobr_bgscroll_w)	/* bg layer scroll */
	AM_RANGE(0x072004, 0x072005) AM_WRITE(twincobr_bgoffs_w)	/* offset in bg video RAM */
	AM_RANGE(0x074000, 0x074003) AM_WRITE(twincobr_fgscroll_w)	/* fg layer scroll */
	AM_RANGE(0x074004, 0x074005) AM_WRITE(twincobr_fgoffs_w)	/* offset in fg video RAM */
	AM_RANGE(0x076000, 0x076003) AM_WRITE(twincobr_exscroll_w)	/* Spare layer scroll */
	AM_RANGE(0x078000, 0x078001) AM_READ(input_port_3_word_r)	/* Flying Shark - DSW A */
	AM_RANGE(0x078002, 0x078003) AM_READ(input_port_4_word_r)	/* Flying Shark - DSW B */
	AM_RANGE(0x078004, 0x078005) AM_READ(input_port_1_word_r)	/* Player 1 inputs */
	AM_RANGE(0x078006, 0x078007) AM_READ(input_port_2_word_r)	/* Player 2 inputs */
	AM_RANGE(0x078008, 0x078009) AM_READ(input_port_0_word_r)	/* V-Blank & FShark Coin/Start */
	AM_RANGE(0x07800a, 0x07800b) AM_WRITE(fshark_coin_dsp_w)	/* Flying Shark DSP Comms & coin stuff */
	AM_RANGE(0x07800c, 0x07800d) AM_WRITE(twincobr_control_w)	/* Twin Cobra DSP Comms & system control */
	AM_RANGE(0x07a000, 0x07afff) AM_READWRITE(twincobr_sharedram_r, twincobr_sharedram_w)	/* 16-bit on 68000 side, 8-bit on Z80 side */
	AM_RANGE(0x07e000, 0x07e001) AM_READWRITE(twincobr_txram_r, twincobr_txram_w)	/* data for text video RAM */
	AM_RANGE(0x07e002, 0x07e003) AM_READWRITE(twincobr_bgram_r, twincobr_bgram_w)	/* data for bg video RAM */
	AM_RANGE(0x07e004, 0x07e005) AM_READWRITE(twincobr_fgram_r, twincobr_fgram_w)	/* data for fg video RAM */
ADDRESS_MAP_END


/***************************** Z80 Memory Map *******************************/

static ADDRESS_MAP_START( sound_program_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM AM_BASE(&twincobr_sharedram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READWRITE(YM3812_status_port_0_r, YM3812_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(YM3812_write_port_0_w)
	AM_RANGE(0x10, 0x10) AM_READ(input_port_5_r)		/* Twin Cobra - Coin/Start */
	AM_RANGE(0x20, 0x20) AM_WRITE(twincobr_coin_w)		/* Twin Cobra coin count-lockout */
	AM_RANGE(0x40, 0x40) AM_READ(input_port_3_r)		/* Twin Cobra - DSW A */
	AM_RANGE(0x50, 0x50) AM_READ(input_port_4_r)		/* Twin Cobra - DSW B */
ADDRESS_MAP_END


/***************************** TMS32010 Memory Map **************************/

static ADDRESS_MAP_START( DSP_program_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000, 0x7ff) AM_ROM
ADDRESS_MAP_END

	/* $000 - 08F  TMS32010 Internal Data RAM in Data Address Space */

static ADDRESS_MAP_START( DSP_io_map, ADDRESS_SPACE_IO, 16 )
	AM_RANGE(0, 0) AM_WRITE(twincobr_dsp_addrsel_w)
	AM_RANGE(1, 1) AM_READWRITE(twincobr_dsp_r, twincobr_dsp_w)
	AM_RANGE(2, 2) AM_READWRITE(fsharkbt_dsp_r, fsharkbt_dsp_w)
	AM_RANGE(3, 3) AM_WRITE(twincobr_dsp_bio_w)
	AM_RANGE(TMS32010_BIO, TMS32010_BIO) AM_READ(twincobr_BIO_r)
ADDRESS_MAP_END




/*****************************************************************************
    Input Port definitions
*****************************************************************************/

#define  TOAPLAN_PLAYER_INPUT( player )										 \
	PORT_START 																 \
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(player) PORT_8WAY \
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(player)					 \
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(player)					 \
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )							 \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define  TOAPLAN_JAPAN_DSW_A							\
	PORT_START		/* DSW A */							\
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )		\
	PORT_DIPSETTING(	0x01, DEF_STR( Upright ) )		\
	PORT_DIPSETTING(	0x00, DEF_STR( Cocktail ) )		\
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) )	\
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )			\
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )			\
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )	\
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )			\
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )			\
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )	\
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )			\
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )			\
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )		\
	PORT_DIPSETTING(	0x20, DEF_STR( 2C_1C ) )		\
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )		\
	PORT_DIPSETTING(	0x30, DEF_STR( 2C_3C ) )		\
	PORT_DIPSETTING(	0x10, DEF_STR( 1C_2C ) )		\
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )		\
	PORT_DIPSETTING(	0x80, DEF_STR( 2C_1C ) )		\
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )		\
	PORT_DIPSETTING(	0xc0, DEF_STR( 2C_3C ) )		\
	PORT_DIPSETTING(	0x40, DEF_STR( 1C_2C ) )

#define  TWINCOBR_VBLANK_INPUT( active_level )		\
	PORT_START										\
	PORT_BIT( 0x7f, active_level, IPT_UNKNOWN )		\
	PORT_BIT( 0x80, active_level, IPT_VBLANK )

#define  TWINCOBR_SYSTEM_INPUTS							\
	PORT_START											\
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN3 )			\
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_TILT )			\
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )	\
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )			\
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )			\
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )			\
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )			\
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )		\
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )		\
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TWINCOBR_DSW_B		/* not KTIGER */			\
	PORT_START		/* DSW B */							\
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Difficulty ) )	\
	PORT_DIPSETTING(	0x01, DEF_STR( Easy ) )					\
	PORT_DIPSETTING(	0x00, DEF_STR( Normal ) )				\
	PORT_DIPSETTING(	0x02, DEF_STR( Hard ) )					\
	PORT_DIPSETTING(	0x03, DEF_STR( Hardest ) )				\
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )	\
	PORT_DIPSETTING(	0x00, "50K, then every 150K" )	\
	PORT_DIPSETTING(	0x04, "70K, then every 200K" )	\
	PORT_DIPSETTING(	0x08, "50000" )					\
	PORT_DIPSETTING(	0x0c, "100000" )				\
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Lives ) )		\
	PORT_DIPSETTING(	0x30, "2" )						\
	PORT_DIPSETTING(	0x00, "3" )						\
	PORT_DIPSETTING(	0x20, "4" )						\
	PORT_DIPSETTING(	0x10, "5" )						\
	PORT_DIPNAME( 0x40, 0x00, "Show DIP SW Settings" )	\
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )			\
	PORT_DIPSETTING(	0x40, DEF_STR( Yes ) )			\
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )		\
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )			\
	PORT_DIPSETTING(	0x80, DEF_STR( On ) )

#define  FSHARK_SYSTEM_INPUTS		/* V-Blank is also here */				 \
	PORT_START																 \
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN3 )								 \
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_TILT )		/* tilt causes freeze */ \
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )	/* reset button */		 \
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )								 \
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )								 \
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )							 \
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )							 \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )

#define FSHARK_DSW_B									\
	PORT_START		/* DSW B */							\
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Difficulty ) )	\
	PORT_DIPSETTING(	0x01, DEF_STR( Easy ) )					\
	PORT_DIPSETTING(	0x00, DEF_STR( Normal ) )				\
	PORT_DIPSETTING(	0x02, DEF_STR( Hard ) )					\
	PORT_DIPSETTING(	0x03, DEF_STR( Hardest ) )				\
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )	\
	PORT_DIPSETTING(	0x00, "50K, then every 150K" )	\
	PORT_DIPSETTING(	0x04, "70K, then every 200K" )	\
	PORT_DIPSETTING(	0x08, "50000" )					\
	PORT_DIPSETTING(	0x0c, "100000" )				\
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Lives ) )		\
	PORT_DIPSETTING(	0x20, "1" )						\
	PORT_DIPSETTING(	0x30, "2" )						\
	PORT_DIPSETTING(	0x00, "3" )						\
	PORT_DIPSETTING(	0x10, "5" )						\
	PORT_DIPNAME( 0x40, 0x00, "Show DIP SW Settings" )	\
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )			\
	PORT_DIPSETTING(	0x40, DEF_STR( Yes ) )			\
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Allow_Continue ) )		\
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )			\
	PORT_DIPSETTING(	0x80, DEF_STR( Yes ) )




INPUT_PORTS_START( twincobr )
	TWINCOBR_VBLANK_INPUT( IP_ACTIVE_HIGH )
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )

	PORT_START		/* DSW A */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x30, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(	0x80, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(	0xc0, DEF_STR( 1C_6C ) )

	TWINCOBR_DSW_B
	TWINCOBR_SYSTEM_INPUTS
INPUT_PORTS_END

INPUT_PORTS_START( twincobu )
	TWINCOBR_VBLANK_INPUT( IP_ACTIVE_HIGH )
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )

	PORT_START		/* DSW A */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x30, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0xc0, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 1C_2C ) )

	TWINCOBR_DSW_B
	TWINCOBR_SYSTEM_INPUTS
INPUT_PORTS_END

INPUT_PORTS_START( ktiger )
	TWINCOBR_VBLANK_INPUT( IP_ACTIVE_HIGH )
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )
	TOAPLAN_JAPAN_DSW_A

	PORT_START		/* DSW B */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	0x01, DEF_STR( Easy ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(	0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(	0x03, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(	0x00, "70K, then every 200K" )
	PORT_DIPSETTING(	0x04, "50K, then every 150K" )
	PORT_DIPSETTING(	0x08, "100000" )
	PORT_DIPSETTING(	0x0c, "No Extend" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(	0x30, "2" )
	PORT_DIPSETTING(	0x00, "3" )
	PORT_DIPSETTING(	0x20, "4" )
	PORT_DIPSETTING(	0x10, "5" )
	PORT_DIPNAME( 0x40, 0x00, "Show DIP SW Settings" )
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )
	PORT_DIPSETTING(	0x40, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )
	PORT_DIPSETTING(	0x80, DEF_STR( Yes ) )

	TWINCOBR_SYSTEM_INPUTS
INPUT_PORTS_END

INPUT_PORTS_START( fshark )
	FSHARK_SYSTEM_INPUTS
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )

	PORT_START		/* DSW A */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(	0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x30, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(	0x80, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(	0xc0, DEF_STR( 1C_6C ) )

	FSHARK_DSW_B
INPUT_PORTS_END

INPUT_PORTS_START( skyshark )
	FSHARK_SYSTEM_INPUTS
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )

	PORT_START		/* DSW A */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(	0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 1C_2C ) )
/*  PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )    Same as previous */
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x80, DEF_STR( 1C_2C ) )
/*  PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) )    Same as previous */

	FSHARK_DSW_B
INPUT_PORTS_END

INPUT_PORTS_START( hishouza )
	FSHARK_SYSTEM_INPUTS
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )
	TOAPLAN_JAPAN_DSW_A
	FSHARK_DSW_B
INPUT_PORTS_END

INPUT_PORTS_START( gulfwar2 )
	TWINCOBR_VBLANK_INPUT( IP_ACTIVE_LOW )
	TOAPLAN_PLAYER_INPUT( 1 )
	TOAPLAN_PLAYER_INPUT( 2 )

	PORT_START		/* DSW A */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Cross Hatch Pattern" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x30, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(	0x80, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(	0xc0, DEF_STR( 1C_6C ) )

	TWINCOBR_DSW_B
	TWINCOBR_SYSTEM_INPUTS
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,			/* 8*8 characters */
	RGN_FRAC(1,3),	/* 2048 characters */
	3,				/* 3 bits per pixel */
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8				/* every char takes 8 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	8,8,			/* 8*8 tiles */
	RGN_FRAC(1,4),	/* 4096/8192 tiles */
	4,				/* 4 bits per pixel */
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8				/* every tile takes 8 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,			/* 16*16 sprites */
	RGN_FRAC(1,4),	/* 2048 sprites */
	4,				/* 4 bits per pixel */
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	32*8			/* every sprite takes 32 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x00000, &charlayout,	1536, 32 },	/* colors 1536-1791 */
	{ REGION_GFX2, 0x00000, &tilelayout,	1280, 16 },	/* colors 1280-1535 */
	{ REGION_GFX3, 0x00000, &tilelayout,	1024, 16 },	/* colors 1024-1079 */
	{ REGION_GFX4, 0x00000, &spritelayout,	   0, 64 },	/* colors    0-1023 */
	{ -1 } /* end of array */
};



/* handler called by the 3812 emulator when the internal timers cause an IRQ */
static void irqhandler(int linestate)
{
	cpunum_set_input_line(1,0,linestate);
}

static struct YM3812interface ym3812_interface =
{
	irqhandler
};



static MACHINE_DRIVER_START( twincobr )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,28000000/4)			/* 7.0MHz - Main board Crystal is 28MHz */
	MDRV_CPU_PROGRAM_MAP(main_program_map, 0)
	MDRV_CPU_VBLANK_INT(twincobr_interrupt,1)

	MDRV_CPU_ADD(Z80,28000000/8)			/* 3.5MHz */
	MDRV_CPU_PROGRAM_MAP(sound_program_map, 0)
	MDRV_CPU_IO_MAP(sound_io_map, 0)

	MDRV_CPU_ADD(TMS32010,(28000000/2)/TMS32010_CLOCK_DIVIDER)	/* 14MHz CLKin */
	MDRV_CPU_PROGRAM_MAP(DSP_program_map, 0)
	/* Data Map is internal to the CPU */
	MDRV_CPU_IO_MAP(DSP_io_map, 0)

	MDRV_FRAMES_PER_SECOND( (28000000.0 / 4) / (446 * 286) )
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(100)

	MDRV_MACHINE_RESET(twincobr_reset)	/* Reset fshark bootleg 8741 MCU data */

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_UPDATE_BEFORE_VBLANK | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 0*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1792)

	MDRV_VIDEO_START(toaplan0)
	MDRV_VIDEO_EOF(toaplan0)
	MDRV_VIDEO_UPDATE(toaplan0)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3812, 28000000/8)
	MDRV_SOUND_CONFIG(ym3812_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( twincobr )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "tc16",		0x00000, 0x10000, CRC(07f64d13) SHA1(864ce0f9369c40c3ae792fc4ab2444a168214749) )
	ROM_LOAD16_BYTE( "tc14",		0x00001, 0x10000, CRC(41be6978) SHA1(4784804b738a332c7f24a43bcbb7a1e607365735) )
	ROM_LOAD16_BYTE( "tc15",		0x20000, 0x08000, CRC(3a646618) SHA1(fc1ed8f3c491f5cf16a17e5ce08c5d8f3ce03683) )
	ROM_LOAD16_BYTE( "tc13",		0x20001, 0x08000, CRC(d7d1e317) SHA1(57b8433b1677a390a7c7e00a1464bb8ed9cbfc73) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "tc12",			0x00000, 0x08000, CRC(e37b3c44) SHA1(5fed10b29c14e27aee0cd92ecde5c5cb422273b1) )	/* slightly different from the other two sets */

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROM_LOAD16_BYTE( "dsp_22.bin",	0x0001, 0x0800, CRC(79389a71) SHA1(14ec4c1c9b06702319e89a7a250d0038393437f4) )
	ROM_LOAD16_BYTE( "dsp_21.bin",	0x0000, 0x0800, CRC(2d135376) SHA1(67a2cc774d272ee1cd6e6bc1c5fc33fc6968837e) )
/****** The following are from a bootleg board. ******
    A0 and A1 are swapped between the TMS320C10 and these BPROMs on the board.
    ROM_LOAD16_BYTE( "tc1b",        0x0000, 0x0800, CRC(1757cc33) )
    ROM_LOAD16_BYTE( "tc2a",        0x0001, 0x0800, CRC(d6d878c9) )
*/

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "tc11",			0x00000, 0x04000, CRC(0a254133) SHA1(17e9cc5e36fb4696012d0f9229fa172034cd843a) )
	ROM_LOAD( "tc03",			0x04000, 0x04000, CRC(e9e2d4b1) SHA1(e0a19dd46a9ba85d95bba7fbf81d8dc36dbfeabd) )
	ROM_LOAD( "tc04",			0x08000, 0x04000, CRC(a599d845) SHA1(732001f2d378d890f148e6b616c287d71fae832a) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "tc01",			0x00000, 0x10000, CRC(15b3991d) SHA1(f5e7ed7a7721ed7e6dfd440634160390b7a294e4) )
	ROM_LOAD( "tc02",			0x10000, 0x10000, CRC(d9e2e55d) SHA1(0409e6df836d1d5198b64b21b42192631aa6d096) )
	ROM_LOAD( "tc06",			0x20000, 0x10000, CRC(13daeac8) SHA1(1cb103f434e2ecf193fa936ca7ea9194064c5b39) )
	ROM_LOAD( "tc05",			0x30000, 0x10000, CRC(8cc79357) SHA1(31064df2b796ca85ad3caccf626b684dff1104a1) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "tc07",			0x00000, 0x08000, CRC(b5d48389) SHA1(a00c5b9c231d3d580fa20c7ad3f8b6fd990e6594) )
	ROM_LOAD( "tc08",			0x08000, 0x08000, CRC(97f20fdc) SHA1(7cb3cd0637b0db889a3d552fd7c1a916eee5ca27) )
	ROM_LOAD( "tc09",			0x10000, 0x08000, CRC(170c01db) SHA1(f4c5a1600f6cbb48abbace66c6f7514f79138e8b) )
	ROM_LOAD( "tc10",			0x18000, 0x08000, CRC(44f5accd) SHA1(2f9bdebe71c8be195332356df68992fd38d86994) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "tc20",			0x00000, 0x10000, CRC(cb4092b8) SHA1(35b1d1e04af760fa106124bd5a94174d63ff9705) )
	ROM_LOAD( "tc19",			0x10000, 0x10000, CRC(9cb8675e) SHA1(559c21d505c60401f7368d4ab2b686b15075c5c5) )
	ROM_LOAD( "tc18",			0x20000, 0x10000, CRC(806fb374) SHA1(3eebefadcbdf713bf2a65b438092746b07edd3f0) )
	ROM_LOAD( "tc17",			0x30000, 0x10000, CRC(4264bff8) SHA1(3271b8b23f51346d1928ae01f8b547fed49181e6) )

	ROM_REGION( 0x260, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "82s129.d3",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "82s129.d4",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "82s123.d2",	0x200, 0x020, CRC(f72482db) SHA1(b0cb911f9c81f6088a5aa8760916ddae1f8534d7) )	/* sprite control ?? */
	ROM_LOAD( "82s123.e18",	0x220, 0x020, CRC(bc88cced) SHA1(5055362710c0f58823c05fb4c0e0eec638b91e3d) )	/* sprite attribute (flip/position) ?? */
	ROM_LOAD( "82s123.b24",	0x240, 0x020, CRC(4fb5df2a) SHA1(506ef2c8e4cf45c256d6831a0a5760732f2de422) )	/* tile to sprite priority ?? */
ROM_END

ROM_START( twincobu )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "tc16",			0x00000, 0x10000, CRC(07f64d13) SHA1(864ce0f9369c40c3ae792fc4ab2444a168214749) )
	ROM_LOAD16_BYTE( "tc14",			0x00001, 0x10000, CRC(41be6978) SHA1(4784804b738a332c7f24a43bcbb7a1e607365735) )
	ROM_LOAD16_BYTE( "tcbra26.bin",	0x20000, 0x08000, CRC(bdd00ba4) SHA1(b76b22f03eb4b821a8c555edd9fcee814f2e66a7) )
	ROM_LOAD16_BYTE( "tcbra27.bin",	0x20001, 0x08000, CRC(ed600907) SHA1(e5964db9eab2c334940795d71cb90f6679490227) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "b30-05",				0x00000, 0x08000, CRC(1a8f1e10) SHA1(0c37a7a50b2523506ad77ac03ae752eb94092ff6) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROM_LOAD16_BYTE( "dsp_22.bin",	0x0001, 0x0800, CRC(79389a71) SHA1(14ec4c1c9b06702319e89a7a250d0038393437f4) )
	ROM_LOAD16_BYTE( "dsp_21.bin",	0x0000, 0x0800, CRC(2d135376) SHA1(67a2cc774d272ee1cd6e6bc1c5fc33fc6968837e) )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "tc11",			0x00000, 0x04000, CRC(0a254133) SHA1(17e9cc5e36fb4696012d0f9229fa172034cd843a) )
	ROM_LOAD( "tc03",			0x04000, 0x04000, CRC(e9e2d4b1) SHA1(e0a19dd46a9ba85d95bba7fbf81d8dc36dbfeabd) )
	ROM_LOAD( "tc04",			0x08000, 0x04000, CRC(a599d845) SHA1(732001f2d378d890f148e6b616c287d71fae832a) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "tc01",			0x00000, 0x10000, CRC(15b3991d) SHA1(f5e7ed7a7721ed7e6dfd440634160390b7a294e4) )
	ROM_LOAD( "tc02",			0x10000, 0x10000, CRC(d9e2e55d) SHA1(0409e6df836d1d5198b64b21b42192631aa6d096) )
	ROM_LOAD( "tc06",			0x20000, 0x10000, CRC(13daeac8) SHA1(1cb103f434e2ecf193fa936ca7ea9194064c5b39) )
	ROM_LOAD( "tc05",			0x30000, 0x10000, CRC(8cc79357) SHA1(31064df2b796ca85ad3caccf626b684dff1104a1) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "tc07",			0x00000, 0x08000, CRC(b5d48389) SHA1(a00c5b9c231d3d580fa20c7ad3f8b6fd990e6594) )
	ROM_LOAD( "tc08",			0x08000, 0x08000, CRC(97f20fdc) SHA1(7cb3cd0637b0db889a3d552fd7c1a916eee5ca27) )
	ROM_LOAD( "tc09",			0x10000, 0x08000, CRC(170c01db) SHA1(f4c5a1600f6cbb48abbace66c6f7514f79138e8b) )
	ROM_LOAD( "tc10",			0x18000, 0x08000, CRC(44f5accd) SHA1(2f9bdebe71c8be195332356df68992fd38d86994) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "tc20",			0x00000, 0x10000, CRC(cb4092b8) SHA1(35b1d1e04af760fa106124bd5a94174d63ff9705) )
	ROM_LOAD( "tc19",			0x10000, 0x10000, CRC(9cb8675e) SHA1(559c21d505c60401f7368d4ab2b686b15075c5c5) )
	ROM_LOAD( "tc18",			0x20000, 0x10000, CRC(806fb374) SHA1(3eebefadcbdf713bf2a65b438092746b07edd3f0) )
	ROM_LOAD( "tc17",			0x30000, 0x10000, CRC(4264bff8) SHA1(3271b8b23f51346d1928ae01f8b547fed49181e6) )

	ROM_REGION( 0x260, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "82s129.d3",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "82s129.d4",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "82s123.d2",	0x200, 0x020, CRC(f72482db) SHA1(b0cb911f9c81f6088a5aa8760916ddae1f8534d7) )	/* sprite control ?? */
	ROM_LOAD( "82s123.e18",	0x220, 0x020, CRC(bc88cced) SHA1(5055362710c0f58823c05fb4c0e0eec638b91e3d) )	/* sprite attribute (flip/position) ?? */
	ROM_LOAD( "82s123.b24",	0x240, 0x020, CRC(4fb5df2a) SHA1(506ef2c8e4cf45c256d6831a0a5760732f2de422) )	/* tile to sprite priority ?? */
ROM_END

ROM_START( ktiger )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "tc16",		0x00000, 0x10000, CRC(07f64d13) SHA1(864ce0f9369c40c3ae792fc4ab2444a168214749) )
	ROM_LOAD16_BYTE( "tc14",		0x00001, 0x10000, CRC(41be6978) SHA1(4784804b738a332c7f24a43bcbb7a1e607365735) )
	ROM_LOAD16_BYTE( "b30-02",	0x20000, 0x08000, CRC(1d63e9c4) SHA1(bdf013487a6fe8f8cbb03fda5f4fae881064831c) )
	ROM_LOAD16_BYTE( "b30-04",	0x20001, 0x08000, CRC(03957a30) SHA1(d809881a16b05595b6f184e44a36e592f46ba04a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "b30-05",			0x00000, 0x08000, CRC(1a8f1e10) SHA1(0c37a7a50b2523506ad77ac03ae752eb94092ff6) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROM_LOAD16_BYTE( "dsp-22",	0x0001, 0x0800, BAD_DUMP CRC(8a1d48d9) SHA1(f345c95a97adfbe89676f81ac83fbbec25703440) )
	ROM_LOAD16_BYTE( "dsp-21",	0x0000, 0x0800, BAD_DUMP CRC(33d99bc2) SHA1(9372dcfc2313abc4835365ae99842f732329d4e6) )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "tc11",			0x00000, 0x04000, CRC(0a254133) SHA1(17e9cc5e36fb4696012d0f9229fa172034cd843a) )
	ROM_LOAD( "tc03",			0x04000, 0x04000, CRC(e9e2d4b1) SHA1(e0a19dd46a9ba85d95bba7fbf81d8dc36dbfeabd) )
	ROM_LOAD( "tc04",			0x08000, 0x04000, CRC(a599d845) SHA1(732001f2d378d890f148e6b616c287d71fae832a) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "tc01",			0x00000, 0x10000, CRC(15b3991d) SHA1(f5e7ed7a7721ed7e6dfd440634160390b7a294e4) )
	ROM_LOAD( "tc02",			0x10000, 0x10000, CRC(d9e2e55d) SHA1(0409e6df836d1d5198b64b21b42192631aa6d096) )
	ROM_LOAD( "tc06",			0x20000, 0x10000, CRC(13daeac8) SHA1(1cb103f434e2ecf193fa936ca7ea9194064c5b39) )
	ROM_LOAD( "tc05",			0x30000, 0x10000, CRC(8cc79357) SHA1(31064df2b796ca85ad3caccf626b684dff1104a1) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "tc07",			0x00000, 0x08000, CRC(b5d48389) SHA1(a00c5b9c231d3d580fa20c7ad3f8b6fd990e6594) )
	ROM_LOAD( "tc08",			0x08000, 0x08000, CRC(97f20fdc) SHA1(7cb3cd0637b0db889a3d552fd7c1a916eee5ca27) )
	ROM_LOAD( "tc09",			0x10000, 0x08000, CRC(170c01db) SHA1(f4c5a1600f6cbb48abbace66c6f7514f79138e8b) )
	ROM_LOAD( "tc10",			0x18000, 0x08000, CRC(44f5accd) SHA1(2f9bdebe71c8be195332356df68992fd38d86994) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "tc20",			0x00000, 0x10000, CRC(cb4092b8) SHA1(35b1d1e04af760fa106124bd5a94174d63ff9705) )
	ROM_LOAD( "tc19",			0x10000, 0x10000, CRC(9cb8675e) SHA1(559c21d505c60401f7368d4ab2b686b15075c5c5) )
	ROM_LOAD( "tc18",			0x20000, 0x10000, CRC(806fb374) SHA1(3eebefadcbdf713bf2a65b438092746b07edd3f0) )
	ROM_LOAD( "tc17",			0x30000, 0x10000, CRC(4264bff8) SHA1(3271b8b23f51346d1928ae01f8b547fed49181e6) )

	ROM_REGION( 0x260, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "82s129.d3",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "82s129.d4",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "82s123.d2",	0x200, 0x020, CRC(f72482db) SHA1(b0cb911f9c81f6088a5aa8760916ddae1f8534d7) )	/* sprite control ?? */
	ROM_LOAD( "82s123.e18",	0x220, 0x020, CRC(bc88cced) SHA1(5055362710c0f58823c05fb4c0e0eec638b91e3d) )	/* sprite attribute (flip/position) ?? */
	ROM_LOAD( "82s123.b24",	0x240, 0x020, CRC(4fb5df2a) SHA1(506ef2c8e4cf45c256d6831a0a5760732f2de422) )	/* tile to sprite priority ?? */
ROM_END

ROM_START( fshark )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "b02_18-1.rom",	0x00000, 0x10000, CRC(04739e02) SHA1(8a14284adb0f0f33adf9affdec081c90de85d594) )
	ROM_LOAD16_BYTE( "b02_17-1.rom",	0x00001, 0x10000, CRC(fd6ef7a8) SHA1(ddbc05ce694ab4d929f5f621d95800b612bc5f66) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "b02_16.rom",		0x0000, 0x8000, CRC(cdd1a153) SHA1(de9827a959039cf753ecac6756fb1925c37466d8) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROMX_LOAD( "82s137-1.mcu",  0x0000, 0x0400, CRC(cc5b3f53) SHA1(33589665ac995cc4645b56bbcd6d1c1cd5368f88), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-2.mcu",  0x0000, 0x0400, CRC(47351d55) SHA1(826add3ea3987f2c9ba2d3fc69a4ad2d9b033c89), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-3.mcu",  0x0001, 0x0400, CRC(70b537b9) SHA1(5211ec4605894727747dda66b70c9427652b16b4), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-4.mcu",  0x0001, 0x0400, CRC(6edb2de8) SHA1(48459037c3b865f0c0d63a416fa71ba1119f7a09), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-5.mcu",  0x0800, 0x0400, CRC(f35b978a) SHA1(90da4ab12126727cd9510fdfe4f626452116c543), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-6.mcu",  0x0800, 0x0400, CRC(0459e51b) SHA1(b673f5e1fcf60c0ba668aeb98d545d17b988945d), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-7.mcu",  0x0801, 0x0400, CRC(cbf3184b) SHA1(a3eafadc319183ed688dc081c4dfcbe8d476abea), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-8.mcu",  0x0801, 0x0400, CRC(8246a05c) SHA1(2511fa99fbdd6c75281fa85ecca2a617d36eb360), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "b02_07-1.rom",	0x00000, 0x04000, CRC(e669f80e) SHA1(05c1a4ff9adaa6c8035f38a76c5ee333fafba2bf) )
	ROM_LOAD( "b02_06-1.rom",	0x04000, 0x04000, CRC(5e53ae47) SHA1(55bde4133deebb59a87d9b96c6d0fd7b4bbc0e02) )
	ROM_LOAD( "b02_05-1.rom",	0x08000, 0x04000, CRC(a8b05bd0) SHA1(37317838ea57cb98cf9599cedf8e72bcae913d29) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "b02_12.rom",		0x00000, 0x08000, CRC(733b9997) SHA1(75e874a1d148fcc8fa09bb724ce8346565ace4e5) )
	ROM_LOAD( "b02_15.rom",		0x08000, 0x08000, CRC(8b70ef32) SHA1(e1f988d650dce17e3bfbea12e5fddbb671df18d4) )
	ROM_LOAD( "b02_14.rom",		0x10000, 0x08000, CRC(f711ba7d) SHA1(49644a264c09fc2d743e4f801b8b82e980f2def9) )
	ROM_LOAD( "b02_13.rom",		0x18000, 0x08000, CRC(62532cd3) SHA1(df483db7604c0135130f92b08bad3fbffb4f5c47) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "b02_08.rom",		0x00000, 0x08000, CRC(ef0cf49c) SHA1(6fd5727462cd6c5dab4c5d780bd7504e48583894) )
	ROM_LOAD( "b02_11.rom",		0x08000, 0x08000, CRC(f5799422) SHA1(3f79dd849db787695a587f0db19a6782153b5955) )
	ROM_LOAD( "b02_10.rom",		0x10000, 0x08000, CRC(4bd099ff) SHA1(9326075f83549b0a9656f69bd4436fb1be2ac805) )
	ROM_LOAD( "b02_09.rom",		0x18000, 0x08000, CRC(230f1582) SHA1(0fd4156a46ed64cb6e5c59b8836382dd86c229cf) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "b02_01.512",		0x00000, 0x10000, CRC(2234b424) SHA1(bd6242b9dcdb0f582565df588106cd1ce2aad53b) )
	ROM_LOAD( "b02_02.512",		0x10000, 0x10000, CRC(30d4c9a8) SHA1(96ce4f41207c5487e801a8444030ec4dc7b58b23) )
	ROM_LOAD( "b02_03.512",		0x20000, 0x10000, CRC(64f3d88f) SHA1(d0155cfb0a8885d58e34141f9696b9aa208440ca) )
	ROM_LOAD( "b02_04.512",		0x30000, 0x10000, CRC(3b23a9fc) SHA1(2ac34445618e17371b5eed7eb6f43da4dbb99e28) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "clr2.bpr",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "clr1.bpr",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "clr3.bpr",	0x200, 0x100, CRC(016fe2f7) SHA1(909f815a61e759fdf998674ee383512ecd8fee65) )	/* ?? */
ROM_END

ROM_START( skyshark )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "18-2",		0x00000, 0x10000, CRC(888e90f3) SHA1(3a40d7e7653cc929af8186e48f272989fb332e14) )
	ROM_LOAD16_BYTE( "17-2",		0x00001, 0x10000, CRC(066d67be) SHA1(a66be35b956da2c2ddf97cae66d79c0efd228621) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "b02_16.rom",		0x0000, 0x8000, CRC(cdd1a153) SHA1(de9827a959039cf753ecac6756fb1925c37466d8) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROMX_LOAD( "82s137-1.mcu",  0x0000, 0x0400, CRC(cc5b3f53) SHA1(33589665ac995cc4645b56bbcd6d1c1cd5368f88), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-2.mcu",  0x0000, 0x0400, CRC(47351d55) SHA1(826add3ea3987f2c9ba2d3fc69a4ad2d9b033c89), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-3.mcu",  0x0001, 0x0400, CRC(70b537b9) SHA1(5211ec4605894727747dda66b70c9427652b16b4), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-4.mcu",  0x0001, 0x0400, CRC(6edb2de8) SHA1(48459037c3b865f0c0d63a416fa71ba1119f7a09), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-5.mcu",  0x0800, 0x0400, CRC(f35b978a) SHA1(90da4ab12126727cd9510fdfe4f626452116c543), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-6.mcu",  0x0800, 0x0400, CRC(0459e51b) SHA1(b673f5e1fcf60c0ba668aeb98d545d17b988945d), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-7.mcu",  0x0801, 0x0400, CRC(cbf3184b) SHA1(a3eafadc319183ed688dc081c4dfcbe8d476abea), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "82s137-8.mcu",  0x0801, 0x0400, CRC(8246a05c) SHA1(2511fa99fbdd6c75281fa85ecca2a617d36eb360), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "7-2",			0x00000, 0x04000, CRC(af48c4e6) SHA1(07e8bb6cb92f812990112063f87073df9a346ff4) )
	ROM_LOAD( "6-2",			0x04000, 0x04000, CRC(9a29a862) SHA1(5742f1f5a9c8d644d2a48496466039d18f192929) )
	ROM_LOAD( "5-2",			0x08000, 0x04000, CRC(fb7cad55) SHA1(91815a717511cc97477f08f0fed568247c7fd143) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "b02_12.rom",		0x00000, 0x08000, CRC(733b9997) SHA1(75e874a1d148fcc8fa09bb724ce8346565ace4e5) )
	ROM_LOAD( "b02_15.rom",		0x08000, 0x08000, CRC(8b70ef32) SHA1(e1f988d650dce17e3bfbea12e5fddbb671df18d4) )
	ROM_LOAD( "b02_14.rom",		0x10000, 0x08000, CRC(f711ba7d) SHA1(49644a264c09fc2d743e4f801b8b82e980f2def9) )
	ROM_LOAD( "b02_13.rom",		0x18000, 0x08000, CRC(62532cd3) SHA1(df483db7604c0135130f92b08bad3fbffb4f5c47) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "b02_08.rom",		0x00000, 0x08000, CRC(ef0cf49c) SHA1(6fd5727462cd6c5dab4c5d780bd7504e48583894) )
	ROM_LOAD( "b02_11.rom",		0x08000, 0x08000, CRC(f5799422) SHA1(3f79dd849db787695a587f0db19a6782153b5955) )
	ROM_LOAD( "b02_10.rom",		0x10000, 0x08000, CRC(4bd099ff) SHA1(9326075f83549b0a9656f69bd4436fb1be2ac805) )
	ROM_LOAD( "b02_09.rom",		0x18000, 0x08000, CRC(230f1582) SHA1(0fd4156a46ed64cb6e5c59b8836382dd86c229cf) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "b02_01.512",		0x00000, 0x10000, CRC(2234b424) SHA1(bd6242b9dcdb0f582565df588106cd1ce2aad53b) )
	ROM_LOAD( "b02_02.512",		0x10000, 0x10000, CRC(30d4c9a8) SHA1(96ce4f41207c5487e801a8444030ec4dc7b58b23) )
	ROM_LOAD( "b02_03.512",		0x20000, 0x10000, CRC(64f3d88f) SHA1(d0155cfb0a8885d58e34141f9696b9aa208440ca) )
	ROM_LOAD( "b02_04.512",		0x30000, 0x10000, CRC(3b23a9fc) SHA1(2ac34445618e17371b5eed7eb6f43da4dbb99e28) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "clr2.bpr",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "clr1.bpr",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "clr3.bpr",	0x200, 0x100, CRC(016fe2f7) SHA1(909f815a61e759fdf998674ee383512ecd8fee65) )	/* ?? */
ROM_END

ROM_START( hishouza )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "b02-18.rom",	0x00000, 0x10000, CRC(4444bb94) SHA1(5ff955a5190d1b356187de105cfb8ea181fc1282) )
	ROM_LOAD16_BYTE( "b02-17.rom",	0x00001, 0x10000, CRC(cdac7228) SHA1(6b0d67e4b0661a858653d2eabb8936af9148167e) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "b02_16.rom",		0x0000, 0x8000, CRC(cdd1a153) SHA1(de9827a959039cf753ecac6756fb1925c37466d8) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROMX_LOAD( "dsp-a1.bpr", 0x0000, 0x0400, CRC(45d4d1b1) SHA1(e776a056f0f72cbeb309c5a23f803330cb8b3763), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-a2.bpr", 0x0000, 0x0400, CRC(edd227fa) SHA1(34aba84b5216ecbe462e7166d0f66785ca049a34), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-a3.bpr", 0x0001, 0x0400, CRC(df88e79b) SHA1(661b057fa2eef37b9d794151381d7d74a7bfa93a), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-a4.bpr", 0x0001, 0x0400, CRC(a2094a7f) SHA1(0f1c173643046c76aa89eab66fba6ea51c3f2223), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-b5.bpr", 0x0800, 0x0400, CRC(85ca5d47) SHA1(3c6e21e2897fd35834021ec9f81f57bebfd13ef8), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-b6.bpr", 0x0800, 0x0400, CRC(81816b2c) SHA1(1e58ab7aef2a34f42267debf4cad9558d5e14159), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-b7.bpr", 0x0801, 0x0400, CRC(e87540cd) SHA1(bb6e98c47ed46abbbfa06571806cb2d663880419), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "dsp-b8.bpr", 0x0801, 0x0400, CRC(d3c16c5c) SHA1(a24d9536914734c1875c8a39938a346ff4418dd0), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "b02-07.rom",		0x00000, 0x04000, CRC(c13a775e) SHA1(b60d26126ec6ebc24a7ca87dd0234e4d9d3e78fc) )
	ROM_LOAD( "b02-06.rom",		0x04000, 0x04000, CRC(ad5f1371) SHA1(feae9d7bb75bfab5353be4c5931d78a530bd9bcd) )
	ROM_LOAD( "b02-05.rom",		0x08000, 0x04000, CRC(85a7bff6) SHA1(38cd89aa0800e3796f7ecac657d14119543057c2) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "b02_12.rom",		0x00000, 0x08000, CRC(733b9997) SHA1(75e874a1d148fcc8fa09bb724ce8346565ace4e5) )
	ROM_LOAD( "b02_15.rom",		0x08000, 0x08000, CRC(8b70ef32) SHA1(e1f988d650dce17e3bfbea12e5fddbb671df18d4) )
	ROM_LOAD( "b02_14.rom",		0x10000, 0x08000, CRC(f711ba7d) SHA1(49644a264c09fc2d743e4f801b8b82e980f2def9) )
	ROM_LOAD( "b02_13.rom",		0x18000, 0x08000, CRC(62532cd3) SHA1(df483db7604c0135130f92b08bad3fbffb4f5c47) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "b02_08.rom",		0x00000, 0x08000, CRC(ef0cf49c) SHA1(6fd5727462cd6c5dab4c5d780bd7504e48583894) )
	ROM_LOAD( "b02_11.rom",		0x08000, 0x08000, CRC(f5799422) SHA1(3f79dd849db787695a587f0db19a6782153b5955) )
	ROM_LOAD( "b02_10.rom",		0x10000, 0x08000, CRC(4bd099ff) SHA1(9326075f83549b0a9656f69bd4436fb1be2ac805) )
	ROM_LOAD( "b02_09.rom",		0x18000, 0x08000, CRC(230f1582) SHA1(0fd4156a46ed64cb6e5c59b8836382dd86c229cf) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "b02_01.512",		0x00000, 0x10000, CRC(2234b424) SHA1(bd6242b9dcdb0f582565df588106cd1ce2aad53b) )
	ROM_LOAD( "b02_02.512",		0x10000, 0x10000, CRC(30d4c9a8) SHA1(96ce4f41207c5487e801a8444030ec4dc7b58b23) )
	ROM_LOAD( "b02_03.512",		0x20000, 0x10000, CRC(64f3d88f) SHA1(d0155cfb0a8885d58e34141f9696b9aa208440ca) )
	ROM_LOAD( "b02_04.512",		0x30000, 0x10000, CRC(3b23a9fc) SHA1(2ac34445618e17371b5eed7eb6f43da4dbb99e28) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "clr2.bpr",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "clr1.bpr",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "clr3.bpr",	0x200, 0x100, CRC(016fe2f7) SHA1(909f815a61e759fdf998674ee383512ecd8fee65) )	/* ?? */
ROM_END

ROM_START( fsharkbt )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "r18",		0x00000, 0x10000, CRC(ef30f563) SHA1(755d6ce4c1e631d7c11d3fab99dae300b6a3452e) )
	ROM_LOAD16_BYTE( "r17",		0x00001, 0x10000, CRC(0e18d25f) SHA1(82fc94830b3087c826d07cff699af9a3638e8087) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "b02_16.rom",		0x0000, 0x8000, CRC(cdd1a153) SHA1(de9827a959039cf753ecac6756fb1925c37466d8) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	ROMX_LOAD( "mcu-1.bpr",  0x0000, 0x0400, CRC(45d4d1b1) SHA1(e776a056f0f72cbeb309c5a23f803330cb8b3763), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-2.bpr",  0x0000, 0x0400, CRC(651336d1) SHA1(3c968d5cb58abe35794b7c88520a22fc0b45a449), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-3.bpr",  0x0001, 0x0400, CRC(df88e79b) SHA1(661b057fa2eef37b9d794151381d7d74a7bfa93a), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-4.bpr",  0x0001, 0x0400, CRC(a2094a7f) SHA1(0f1c173643046c76aa89eab66fba6ea51c3f2223), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-5.bpr",  0x0800, 0x0400, CRC(f97a58da) SHA1(77a659943d95d5b859fab50f827f11222c3dbf1f), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-6.bpr",  0x0800, 0x0400, CRC(ffcc422d) SHA1(9b4331e8bb5fe37bb8efcccc500a1d7cd026bf93), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-7.bpr",  0x0801, 0x0400, CRC(0cd30d49) SHA1(65d65a199bfb740b94af19843640e625a5e67f46), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1) )
	ROMX_LOAD( "mcu-8.bpr",  0x0801, 0x0400, CRC(3379bbff) SHA1(2f577b8de6d523087b472691cdde2eb525877878), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1) )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "b02_07-1.rom",	0x00000, 0x04000, CRC(e669f80e) SHA1(05c1a4ff9adaa6c8035f38a76c5ee333fafba2bf) )
	ROM_LOAD( "b02_06-1.rom",	0x04000, 0x04000, CRC(5e53ae47) SHA1(55bde4133deebb59a87d9b96c6d0fd7b4bbc0e02) )
	ROM_LOAD( "b02_05-1.rom",	0x08000, 0x04000, CRC(a8b05bd0) SHA1(37317838ea57cb98cf9599cedf8e72bcae913d29) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "b02_12.rom",		0x00000, 0x08000, CRC(733b9997) SHA1(75e874a1d148fcc8fa09bb724ce8346565ace4e5) )
	ROM_LOAD( "b02_15.rom",		0x08000, 0x08000, CRC(8b70ef32) SHA1(e1f988d650dce17e3bfbea12e5fddbb671df18d4) )
	ROM_LOAD( "b02_14.rom",		0x10000, 0x08000, CRC(f711ba7d) SHA1(49644a264c09fc2d743e4f801b8b82e980f2def9) )
	ROM_LOAD( "b02_13.rom",		0x18000, 0x08000, CRC(62532cd3) SHA1(df483db7604c0135130f92b08bad3fbffb4f5c47) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "b02_08.rom",		0x00000, 0x08000, CRC(ef0cf49c) SHA1(6fd5727462cd6c5dab4c5d780bd7504e48583894) )
	ROM_LOAD( "b02_11.rom",		0x08000, 0x08000, CRC(f5799422) SHA1(3f79dd849db787695a587f0db19a6782153b5955) )
	ROM_LOAD( "b02_10.rom",		0x10000, 0x08000, CRC(4bd099ff) SHA1(9326075f83549b0a9656f69bd4436fb1be2ac805) )
	ROM_LOAD( "b02_09.rom",		0x18000, 0x08000, CRC(230f1582) SHA1(0fd4156a46ed64cb6e5c59b8836382dd86c229cf) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "b02_01.512",		0x00000, 0x10000, CRC(2234b424) SHA1(bd6242b9dcdb0f582565df588106cd1ce2aad53b) )
	ROM_LOAD( "b02_02.512",		0x10000, 0x10000, CRC(30d4c9a8) SHA1(96ce4f41207c5487e801a8444030ec4dc7b58b23) )
	ROM_LOAD( "b02_03.512",		0x20000, 0x10000, CRC(64f3d88f) SHA1(d0155cfb0a8885d58e34141f9696b9aa208440ca) )
	ROM_LOAD( "b02_04.512",		0x30000, 0x10000, CRC(3b23a9fc) SHA1(2ac34445618e17371b5eed7eb6f43da4dbb99e28) )

	ROM_REGION( 0x300, REGION_PROMS, 0 )	/* nibble bproms, lo/hi order to be determined */
	ROM_LOAD( "clr2.bpr",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "clr1.bpr",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "clr3.bpr",	0x200, 0x100, CRC(016fe2f7) SHA1(909f815a61e759fdf998674ee383512ecd8fee65) )	/* ?? */
ROM_END

ROM_START( gulfwar2 )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* Main 68K code */
	ROM_LOAD16_BYTE( "08-u119.bin", 0x00000, 0x20000, CRC(41ebf9c0) SHA1(85207dda76abded727ed95717024a2ea2bd85dac) )
	ROM_LOAD16_BYTE( "07-u92.bin",  0x00001, 0x20000, CRC(b73e6b25) SHA1(53cde41e5a2e8f721c3f43abf1fff46479f658d8) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound Z80 code */
	ROM_LOAD( "06-u51.bin",			0x00000, 0x08000, CRC(75504f95) SHA1(5bd23e700e1bd4f0fac622dfb7c8cc69ba764956) )

	ROM_REGION( 0x2000, REGION_CPU3, 0 )	/* Co-Processor TMS320C10 MCU code */
	/* ROMs are duplicated 4 times */
	ROM_LOAD16_BYTE( "01-u2.rom",	0x000, 0x800, CRC(01399b65) SHA1(4867ec815e22c9124c7aa00ebb6089c2611fa31f) ) // Same code as Twin Cobra
	ROM_CONTINUE(                   0x000, 0x800 )
	ROM_CONTINUE(                   0x000, 0x800 )
	ROM_CONTINUE(                   0x000, 0x800 )
	ROM_LOAD16_BYTE( "02-u1.rom",	0x001, 0x800, CRC(abefe4ca) SHA1(f05f12a1ff19411f34f4eee98ce9ba450fec38f2) ) // Same code as Twin Cobra
	ROM_CONTINUE(                   0x001, 0x800 )
	ROM_CONTINUE(                   0x001, 0x800 )
	ROM_CONTINUE(                   0x001, 0x800 )

	ROM_REGION( 0x0c000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "03-u9.bin",			0x00000, 0x04000, CRC(1b7934b3) SHA1(c7f5ac364dec4c7843c30e098fd02e0901bdf4b7) )
	ROM_LOAD( "04-u10.bin",			0x04000, 0x04000, CRC(6f7bfb58) SHA1(4c5602668938a52321b70cd971326fe1a4930889) )
	ROM_LOAD( "05-u11.bin",			0x08000, 0x04000, CRC(31814724) SHA1(bdcf270e6219555a7f776167f6bf971c6ff18a83) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )	/* fg tiles */
	ROM_LOAD( "16-u202.bin",			0x00000, 0x10000, CRC(d815d175) SHA1(917043d0731226d18bcc22dfe27e5a5a18b03c06) )
	ROM_LOAD( "13-u199.bin",			0x10000, 0x10000, CRC(d949b0d9) SHA1(1974d3b54e082baa9084dd619c8a879d954644cd) )
	ROM_LOAD( "14-u200.bin",			0x20000, 0x10000, CRC(c109a6ac) SHA1(3a13ec802e5bafcf599c273a0bb0fd078e01e171) )
	ROM_LOAD( "15-u201.bin",			0x30000, 0x10000, CRC(ad21f2ab) SHA1(0ab6eeb4dc9c2531c6f19479e7f9bc54fc1c1fdf) )

	ROM_REGION( 0x20000, REGION_GFX3, ROMREGION_DISPOSE )	/* bg tiles */
	ROM_LOAD( "09-u195.bin",			0x00000, 0x08000, CRC(b7be3a6d) SHA1(68b9223fd07e81d443a1ae3ff04b2af105b27548) )
	ROM_LOAD( "12-u198.bin",			0x08000, 0x08000, CRC(fd7032a6) SHA1(8be6315d732b154163a3573e2017fdfc77c92e54) )
	ROM_LOAD( "11-u197.bin",			0x10000, 0x08000, CRC(7b721ed3) SHA1(afd10229414c65a56e184d56a69460ca3a502a27) )
	ROM_LOAD( "10-u196.rom",			0x18000, 0x08000, CRC(160f38ab) SHA1(da310ec387d439b26c8b6b881e5dcc07c2b9bb00) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD( "20-u262.bin",			0x00000, 0x10000, CRC(10665ca0) SHA1(0c552c3807e00a7ef4f9fd28c7988a232628a1f5) )
	ROM_LOAD( "19-u261.bin",			0x10000, 0x10000, CRC(cfa6d417) SHA1(f6c17d938b58dc5756ecf617f00fbfaf701602a7) )
	ROM_LOAD( "18-u260.bin",			0x20000, 0x10000, CRC(2e6a0c49) SHA1(0b7ddad8775dcebe240a8246ef7816113f517f87) )
	ROM_LOAD( "17-u259.bin",			0x30000, 0x10000, CRC(66c1b0e6) SHA1(82f3659245913f835c4434131c179b49ee195961) )

	ROM_REGION( 0x260, REGION_PROMS, 0 )
	ROM_LOAD( "82s129.d3",	0x000, 0x100, CRC(24e7d62f) SHA1(1c06a1ef1b6a722794ca1d5ee2c476ecaa5178a3) )	/* sprite priority control ?? */
	ROM_LOAD( "82s129.d4",	0x100, 0x100, CRC(a50cef09) SHA1(55cafb5b2551b80ae708e9b966cf37c70a16d310) )	/* sprite priority control ?? */
	ROM_LOAD( "82s123.d2",	0x200, 0x020, CRC(f72482db) SHA1(b0cb911f9c81f6088a5aa8760916ddae1f8534d7) )	/* sprite control ?? */
	ROM_LOAD( "82s123.e18",	0x220, 0x020, CRC(bc88cced) SHA1(5055362710c0f58823c05fb4c0e0eec638b91e3d) )	/* sprite attribute (flip/position) ?? */
	ROM_LOAD( "82s123.b24",	0x240, 0x020, CRC(4fb5df2a) SHA1(506ef2c8e4cf45c256d6831a0a5760732f2de422) )	/* tile to sprite priority ?? */
ROM_END


static DRIVER_INIT( twincobr )
{
	twincobr_driver_savestate();
}


GAME( 1987, twincobr, 0,        twincobr, twincobr, twincobr, ROT270, "[Toaplan] Taito Corporation", "Twin Cobra (World)", 0 ,2)
GAME( 1987, twincobu, twincobr, twincobr, twincobu, twincobr, ROT270, "[Toaplan] Taito America Corporation (Romstar license)", "Twin Cobra (US)", 0 ,2)
GAME( 1987, ktiger,   twincobr, twincobr, ktiger,   twincobr, ROT270, "[Toaplan] Taito Corporation", "Kyukyoku Tiger (Japan)", 0 ,2)
GAME( 1987, fshark,   0,        twincobr, fshark,   twincobr, ROT270, "[Toaplan] Taito Corporation", "Flying Shark (World)", 0 ,2)
GAME( 1987, skyshark, fshark,   twincobr, skyshark, twincobr, ROT270, "[Toaplan] Taito America Corporation (Romstar license)", "Sky Shark (US)", 0 ,2)
GAME( 1987, hishouza, fshark,   twincobr, hishouza, twincobr, ROT270, "[Toaplan] Taito Corporation", "Hishou Zame (Japan)", 0 ,2)
GAME( 1987, fsharkbt, fshark,   twincobr, skyshark, twincobr, ROT270, "bootleg", "Flying Shark (bootleg)", 0 ,2)
GAME( 1991, gulfwar2, 0,        twincobr, gulfwar2, twincobr, ROT270, "Comad", "Gulf War II", 0 ,0)
