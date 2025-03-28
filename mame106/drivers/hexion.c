/***************************************************************************

Hexion (GX122) (c) 1992 Konami

driver by Nicola Salmoria

Notes:
- There are probably palette PROMs missing. Palette data doesn't seem to be
  written anywhere in RAM.
- The board has a 052591, which is used for protection in Thunder Cross and
  S.P.Y. In this game, however, the only thing it seems to do is clear the
  screen.
  This is the program for the 052591:
00: 5f 80 01 e0 08
01: df 80 00 e0 0c
02: df 90 02 e0 0c
03: df a0 03 e0 0c
04: df b0 0f e0 0c
05: df c0 ff bf 0c
06: 5c 02 00 33 0c
07: 5f 80 04 80 0c
08: 5c 0e 00 2b 0c
09: df 70 00 cb 08
0a: 5f 80 00 80 0c
0b: 5c 04 00 2b 0c
0c: df 60 00 cb 08
0d: 5c 0c 1f e9 0c
0e: 4c 0c 2d e9 08
0f: 5f 80 03 80 0c
10: 5c 04 00 2b 0c
11: 5f 00 00 cb 00
12: 5f 80 02 a0 0c
13: df d0 00 c0 04
14: 01 3a 00 f3 0a
15: 5c 08 00 b3 0c
16: 5c 0e 00 13 0c
17: 5f 80 00 a0 0c
18: 5c 00 00 13 0c
19: 5c 08 00 b3 0c
1a: 5c 00 00 13 0c
1b: 84 5a 00 b3 0c
1c: 48 0a 5b d1 0c
1d: 5f 80 00 e0 08
1e: 5f 00 1e fd 0c
1f: 5f 80 01 a0 0c
20: df 20 00 cb 08
21: 5c 08 00 b3 0c
22: 5f 80 03 00 0c
23: 5c 08 00 b3 0c
24: 5f 80 00 80 0c
25: 5c 00 00 33 0c
26: 5c 08 00 93 0c
27: 9f 91 ff cf 0e
28: 5c 84 00 20 0c
29: 84 00 00 b3 0c
2a: 49 10 69 d1 0c
2b: 5f 80 00 e0 08
2c: 5f 00 2c fd 0c
2d: 5f 80 01 a0 0c
2e: df 20 00 cb 08
2f: 5c 08 00 b3 0c
30: 5f 80 03 00 0c
31: 5c 00 00 b3 0c
32: 5f 80 01 00 0c
33: 5c 08 00 b3 0c
34: 5f 80 00 80 0c
35: 5c 00 00 33 0c
36: 5c 08 00 93 0c
37: 9f 91 ff cf 0e
38: 5c 84 00 20 0c
39: 84 00 00 b3 0c
3a: 49 10 79 d1 0c
3b: 5f 80 00 e0 08
3c: 5f 00 3c fd 0c
3d: ff ff ff ff ff
3e: ff ff ff ff ff
3f: ff ff ff ff ff

***************************************************************************/

#include "driver.h"
#include "sound/okim6295.h"
#include "sound/k051649.h"


VIDEO_START( hexion );
VIDEO_UPDATE( hexion );

WRITE8_HANDLER( hexion_bankswitch_w );
READ8_HANDLER( hexion_bankedram_r );
WRITE8_HANDLER( hexion_bankedram_w );
WRITE8_HANDLER( hexion_bankctrl_w );
WRITE8_HANDLER( hexion_gfxrom_select_w );



static WRITE8_HANDLER( coincntr_w )
{
//logerror("%04x: coincntr_w %02x\n",activecpu_get_pc(),data);

	/* bits 0/1 = coin counters */
	coin_counter_w(0,data & 0x01);
	coin_counter_w(1,data & 0x02);

	/* bit 5 = flip screen */
	flip_screen_set(data & 0x20);

	/* other bit unknown */
if ((data & 0xdc) != 0x10) ui_popup("coincntr %02x",data);
}



static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x9fff) AM_READ(MRA8_BANK1)
	AM_RANGE(0xa000, 0xbfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xc000, 0xdffe) AM_READ(hexion_bankedram_r)
	AM_RANGE(0xf400, 0xf400) AM_READ(input_port_0_r)
	AM_RANGE(0xf401, 0xf401) AM_READ(input_port_1_r)
	AM_RANGE(0xf402, 0xf402) AM_READ(input_port_3_r)
	AM_RANGE(0xf403, 0xf403) AM_READ(input_port_4_r)
	AM_RANGE(0xf440, 0xf440) AM_READ(input_port_2_r)
	AM_RANGE(0xf441, 0xf441) AM_READ(input_port_5_r)
	AM_RANGE(0xf540, 0xf540) AM_READ(watchdog_reset_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xa000, 0xbfff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xc000, 0xdffe) AM_WRITE(hexion_bankedram_w)
	AM_RANGE(0xdfff, 0xdfff) AM_WRITE(hexion_bankctrl_w)
	AM_RANGE(0xe800, 0xe87f) AM_WRITE(K051649_waveform_w)
	AM_RANGE(0xe880, 0xe889) AM_WRITE(K051649_frequency_w)
	AM_RANGE(0xe88a, 0xe88e) AM_WRITE(K051649_volume_w)
	AM_RANGE(0xe88f, 0xe88f) AM_WRITE(K051649_keyonoff_w)
	AM_RANGE(0xf000, 0xf00f) AM_WRITE(MWA8_NOP)	/* 053252? f00e = IRQ ack, f00f = NMI ack */
	AM_RANGE(0xf200, 0xf200) AM_WRITE(OKIM6295_data_0_w)
	AM_RANGE(0xf480, 0xf480) AM_WRITE(hexion_bankswitch_w)
	AM_RANGE(0xf4c0, 0xf4c0) AM_WRITE(coincntr_w)
	AM_RANGE(0xf500, 0xf500) AM_WRITE(hexion_gfxrom_select_w)
ADDRESS_MAP_END



INPUT_PORTS_START( hexion )
	PORT_START
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x70, 0x70, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x70, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x60, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0x50, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x30, "Medium Hard" )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SPECIAL )	/* 052591? game waits for it to be 0 */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ 0, 1, 2, 3 },
	{ RGN_FRAC(1,2)+0*4, RGN_FRAC(1,2)+1*4, 0*4, 1*4, RGN_FRAC(1,2)+2*4, RGN_FRAC(1,2)+3*4, 2*4, 3*4 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout, 0, 16 },
	{ -1 } /* end of array */
};



static INTERRUPT_GEN( hexion_interrupt )
{
	/* NMI handles start and coin inputs, origin unknown */
	if (cpu_getiloops())
		cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	else
		cpunum_set_input_line(0, 0, HOLD_LINE);
}

static MACHINE_DRIVER_START( hexion )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,24000000/4)	/* Z80B 6 MHz */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(hexion_interrupt,3)	/* both IRQ and NMI are used */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_PIXEL_ASPECT_RATIO_1_2)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 64*8-1, 0*8, 32*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(hexion)
	MDRV_VIDEO_UPDATE(hexion)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 8000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(K051649, 24000000/16)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( hexion )
	ROM_REGION( 0x34800, REGION_CPU1, 0 )	/* ROMs + space for additional RAM */
	ROM_LOAD( "122jab01.bin", 0x00000, 0x20000, CRC(eabc6dd1) SHA1(e74c1f1f2fcf8973f0741a2d544f25c8639448bf) )
	ROM_RELOAD(               0x10000, 0x20000 )	/* banked at 8000-9fff */

	ROM_REGION( 0x80000, REGION_GFX1, 0 )	/* addressable by the main CPU */
	ROM_LOAD( "122a07.bin",   0x00000, 0x40000, CRC(22ae55e3) SHA1(41bdc990f69416b639542e2186a3610c16389063) )
	ROM_LOAD( "122a06.bin",   0x40000, 0x40000, CRC(438f4388) SHA1(9e23805c9642a237daeaf106187d1e1e0692434d) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* OKIM6295 samples */
	ROM_LOAD( "122a05.bin",   0x0000, 0x40000, CRC(bcc831bf) SHA1(c3382065dd0069a4dc0bde2d9931ec85b0bffc73) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "122a04.10b",   0x0000, 0x0100, CRC(506eb8c6) SHA1(3bff7cf286942d8bdbc3998245c3de20981fbecb) )
	ROM_LOAD( "122a03.11b",   0x0100, 0x0100, CRC(590c4f64) SHA1(db4b34f8c5fdfea034a94d65873f6fb842f123e9) )
	ROM_LOAD( "122a02.13b",   0x0200, 0x0100, CRC(5734305c) SHA1(c72e59acf79a4db1a5a9d827eef899c0675336f2) )
ROM_END


GAME( 1992, hexion, 0, hexion, hexion, 0, ROT0, "Konami", "Hexion (Japan)", 0 ,0)
