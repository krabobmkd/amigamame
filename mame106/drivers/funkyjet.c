/***************************************************************************

  Funky Jet                               (c) 1992 Data East / Mitchell Corporation
  Sotsugyo Shousho                        (c) 1995 Mitchell Corporation

  But actually a Data East pcb...  Hardware is pretty close to Super Burger
  Time but with a different graphics chip.

  Emulation by Bryan McPhail, mish@tendril.co.uk


Stephh's notes :

0) 'sotsugyo'

  - COIN2 doesn't work due to code at 0x0001f0 :

        0001F0: 1228 004A                move.b  ($4a,A0), D1

    It should be (as code in 0x00019e) :

        0001F0: 3228 004A                move.w  ($4a,A0), D1

  - SERVICE1 is very strange : instead of adding 1 credit, EACH time it is
    pressed, n credits are added depending on the "Coinage A" Dip Switch :

        Coin_A    n

         3C_1C    1
         2C_1C    1
         1C_1C    1
         1C_2C    2
         1C_3C    3
         1C_4C    4
         1C_5C    5
         1C_6C    6

  - When the "Unused" Dip Switch is ON, the palette is modified.


Funky Jet
Data East, 1992

PCB Layout
----------

DE-0372-0
|-------------------------------------------------------------|
|                                               32.200MHz     |
|   3014   2151     JH02.15F            |-----|               |
|                                       | 45  |  PAL          |
|VOL                6264                |-----|               |
|          JH03.14J             |-----|                       |
|                               | 59  |                       |
| M6295    6264     JH01-2.13F  |-----|    PAL                |
|                                          PAL   63           |
|          6264     JH00-2.11F             PAL                |
|                                     6264               PAL  |
|                                                             |
|    6116                             6264                    |
|    6116                                     28MHz |-------| |
|                     |-------|                     |       | |
|J                    |       |               6116  |  52   | |
|A       DSW2(8)      |  74   |               6116  |       | |
|M                    |       |         6116        |-------| |
|M       DSW1(8)      |-------|         6116                  |
|A      |-----|                                     MAT-01.4A |
|       | 146 |                                               |
|       |-----|     MAT-02.2F                       MAT-00.2A |
|        PAL                                                  |
|-------------------------------------------------------------|
Notes:
      68000 clock     - 14.000MHz [28/2]
      YM2151 clock    - 3.58MHz [32.220/9]
      HuC6280 clock   - 8.050MHz [32.200/4]
      Oki M6295 clock - 1.000MHz [28/28], Sample Rate = 1000000/132
      VSync           - 58Hz
      HSync           - 15.68kHz

      Custom ICs -
                   74 (QFP160)
                   52 (QFP128)
                   45 (QFP80) - HuC6280
                   59 (QFP64) - 68000
                   146(QFP100)
                   63 (SOP28)

***************************************************************************/

#include "driver.h"
#include "cpu/h6280/h6280.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"

#include "decocrpt.h"
#include "decoprot.h"
#include "deco16ic.h"

VIDEO_START( funkyjet );
VIDEO_UPDATE( funkyjet );

/******************************************************************************/

static ADDRESS_MAP_START( funkyjet_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x120000, 0x1207ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x140000, 0x143fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x160000, 0x1607ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x180000, 0x1807ff) AM_READ(deco16_146_funkyjet_prot_r)
	AM_RANGE(0x320000, 0x321fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x322000, 0x323fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x340000, 0x340bff) AM_READ(MRA16_RAM)
	AM_RANGE(0x342000, 0x342bff) AM_READ(MRA16_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( funkyjet_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x120000, 0x1207ff) AM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x140000, 0x143fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x160000, 0x1607ff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16)
	AM_RANGE(0x180000, 0x1807ff) AM_WRITE(deco16_146_funkyjet_prot_w) AM_BASE(&deco16_prot_ram)
	AM_RANGE(0x184000, 0x184001) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x188000, 0x188001) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x300000, 0x30000f) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf12_control)
	AM_RANGE(0x320000, 0x321fff) AM_WRITE(deco16_pf1_data_w) AM_BASE(&deco16_pf1_data)
	AM_RANGE(0x322000, 0x323fff) AM_WRITE(deco16_pf2_data_w) AM_BASE(&deco16_pf2_data)
	AM_RANGE(0x340000, 0x340bff) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf1_rowscroll)
	AM_RANGE(0x342000, 0x342bff) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf2_rowscroll)
ADDRESS_MAP_END

/******************************************************************************/

/* Physical memory map (21 bits) */
static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_READ(MRA8_ROM)
	AM_RANGE(0x100000, 0x100001) AM_READ(MRA8_NOP)
	AM_RANGE(0x110000, 0x110001) AM_READ(YM2151_status_port_0_r)
	AM_RANGE(0x120000, 0x120001) AM_READ(OKIM6295_status_0_r)
	AM_RANGE(0x130000, 0x130001) AM_READ(MRA8_NOP) /* This board only has 1 oki chip */
	AM_RANGE(0x140000, 0x140001) AM_READ(soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_READ(MRA8_BANK8)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x100000, 0x100001) AM_WRITE(MWA8_NOP) /* YM2203 - this board doesn't have one */
	AM_RANGE(0x110000, 0x110001) AM_WRITE(YM2151_word_0_w)
	AM_RANGE(0x120000, 0x120001) AM_WRITE(OKIM6295_data_0_w)
	AM_RANGE(0x130000, 0x130001) AM_WRITE(MWA8_NOP)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_WRITE(MWA8_BANK8)
	AM_RANGE(0x1fec00, 0x1fec01) AM_WRITE(H6280_timer_w)
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(H6280_irq_status_w)
ADDRESS_MAP_END

/******************************************************************************/

INPUT_PORTS_START( funkyjet )
	PORT_START	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )	/* Button 3 only in "test mode" */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )	/* Button 3 only in "test mode" */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* System Inputs */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VBLANK )

	/* Dips seem inverted with respect to other Deco games */

	PORT_START_TAG("DSWA")	/* Dip switch bank 1 */
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )

	PORT_START_TAG("DSWB")	/* Dip switch bank 2 */
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "1" )
	PORT_DIPSETTING(    0xc0, "2" )
	PORT_DIPSETTING(    0x40, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x08, 0x08, "Freeze" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Yes ) )
  	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( funkyjej )
	PORT_INCLUDE(funkyjet)

	PORT_MODIFY("DSWB")

  	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( sotsugyo )
	PORT_START	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)	/* only in "test mode" */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)	/* only in "test mode" */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* System Inputs */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )		// Not working - see notes
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )		// See notes
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VBLANK )

	/* Dips seem inverted with respect to other Deco games */

	PORT_START	/* Dip switch bank 1 */
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )

	PORT_START	/* Dip switch bank 2 */
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x20, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "1" )
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unused ) )		// See notes
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8, RGN_FRAC(1,2)+0, 8, 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static const gfx_layout tile_layout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8, RGN_FRAC(1,2)+0, 8, 0 },
	{ 32*8+0, 32*8+1, 32*8+2, 32*8+3, 32*8+4, 32*8+5, 32*8+6, 32*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	64*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,  256, 32 },	/* Characters 8x8 */
	{ REGION_GFX1, 0, &tile_layout, 256, 32 },	/* Tiles 16x16 */
	{ REGION_GFX2, 0, &tile_layout,   0, 16 },	/* Sprites 16x16 */
	{ -1 } /* end of array */
};

/******************************************************************************/

static void sound_irq(int state)
{
	cpunum_set_input_line(1,1,state); /* IRQ 2 */
}

static struct YM2151interface ym2151_interface =
{
	sound_irq
};

static MACHINE_DRIVER_START( funkyjet )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 14000000) /* 28 MHz crystal */
	MDRV_CPU_PROGRAM_MAP(funkyjet_readmem,funkyjet_writemem)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1)

	MDRV_CPU_ADD(H6280,32220000/4)	/* Custom chip 45, Audio section crystal is 32.220 MHz */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(529)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(funkyjet)
	MDRV_VIDEO_UPDATE(funkyjet)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2151, 32220000/9)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "left", 0.45)
	MDRV_SOUND_ROUTE(1, "right", 0.45)

	MDRV_SOUND_ADD(OKIM6295, 1000000/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END

/******************************************************************************/

ROM_START( funkyjet )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "jk00.12f", 0x00000, 0x40000, CRC(712089c1) SHA1(84167c90303a228107f55596e2ff8b9f111d1bc2) )
	ROM_LOAD16_BYTE( "jk01.13f", 0x00001, 0x40000, CRC(be3920d7) SHA1(6627956d148681bc49991c544a09b07271ea4c7f) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "jk02.16f",    0x00000, 0x10000, CRC(748c0bd8) SHA1(35910e6a4c4f198fb76bde0f5b053e2c66cfa0ff) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mat02", 0x000000, 0x80000, CRC(e4b94c7e) SHA1(7b6ddd0bd388c8d32277fce4b3abb102724bc7d1) ) /* Encrypted chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mat01", 0x000000, 0x80000, CRC(24093a8d) SHA1(71f76ddd8a4b6e05ceb2fff4e20b6edb5e011e79) ) /* sprites */
  	ROM_LOAD( "mat00", 0x080000, 0x80000, CRC(fbda0228) SHA1(815d49898d02e699393e370209181f2ca8301949) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "jk03.15h",    0x00000, 0x20000, CRC(69a0eaf7) SHA1(05038e82ee03106625f05082fe9912e16be181ee) )
ROM_END

ROM_START( funkyjej )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "jh00-2.11f", 0x00000, 0x40000, CRC(5b98b700) SHA1(604bd04f4031b0a3b53db2fab4a0e160dff6936d) )
	ROM_LOAD16_BYTE( "jh01-2.13f", 0x00001, 0x40000, CRC(21280220) SHA1(b365b6c8aa778e21a14b2813e93b9c9d02e14995) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "jk02.16f",    0x00000, 0x10000, CRC(748c0bd8) SHA1(35910e6a4c4f198fb76bde0f5b053e2c66cfa0ff) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mat02", 0x000000, 0x80000, CRC(e4b94c7e) SHA1(7b6ddd0bd388c8d32277fce4b3abb102724bc7d1) ) /* Encrypted chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mat01", 0x000000, 0x80000, CRC(24093a8d) SHA1(71f76ddd8a4b6e05ceb2fff4e20b6edb5e011e79) ) /* sprites */
  	ROM_LOAD( "mat00", 0x080000, 0x80000, CRC(fbda0228) SHA1(815d49898d02e699393e370209181f2ca8301949) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "jk03.15h",    0x00000, 0x20000, CRC(69a0eaf7) SHA1(05038e82ee03106625f05082fe9912e16be181ee) )
ROM_END

ROM_START( sotsugyo )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "03.12f", 0x00000, 0x40000, CRC(d175dfd1) SHA1(61c91d5e20b0492e6ac3b19fe9639eb4f169ae77) )
	ROM_LOAD16_BYTE( "04.13f", 0x00001, 0x40000, CRC(2072477c) SHA1(23820a519e4503854e63ab3ad7eec58178c8d822) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "sb020.16f",    0x00000, 0x10000, CRC(baf5ec93) SHA1(82b22a0b565e51cd40733f21fa876dd7064eb604) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "02.2f", 0x000000, 0x80000, CRC(337b1451) SHA1(ab3a4526e683c23b7634ac3304fb073f6ce98e82) ) /* chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "01.4a", 0x000000, 0x80000, CRC(fa10dd54) SHA1(5dfe66df0bbab5eb151bf65f7e767a2325a50b36) ) /* sprites */
  	ROM_LOAD( "00.2a", 0x080000, 0x80000, CRC(d35a14ef) SHA1(b8d27766db7e183aee208c690364e4383f3c6882) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "sb030.15h",    0x00000, 0x20000, CRC(1ea43f48) SHA1(74cc8c740f1c7fa94c2cb460ea4ee7aa0c490ed7) )
ROM_END

static DRIVER_INIT( funkyjet )
{
	deco74_decrypt(REGION_GFX1);
}

/******************************************************************************/

GAME( 1992, funkyjet, 0,        funkyjet, funkyjet, funkyjet, ROT0, "[Data East] (Mitchell license)", "Funky Jet (World)", 0 ,0)
GAME( 1992, funkyjej, funkyjet, funkyjet, funkyjej, funkyjet, ROT0, "Data East Corporation", "Funky Jet (Japan)", 0 ,0)
GAME( 1995, sotsugyo, 0,        funkyjet, sotsugyo, funkyjet, ROT0, "Mitchell (Atlus license)", "Sotsugyo Shousho", 0 ,0)
