/***************************************************************************

    Atari Klax hardware

    driver by Aaron Giles

    Games supported:
        * Klax (1989) [4 sets]

    Known bugs:
        * none at this time

****************************************************************************

    Memory map (TBA)

***************************************************************************/


#include "driver.h"
#include "machine/atarigen.h"
#include "klax.h"
#include "sound/okim6295.h"



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

static void update_interrupts(void)
{
	int newstate = 0;

	if (atarigen_video_int_state || atarigen_scanline_int_state)
		newstate = 4;

	if (newstate)
		cpunum_set_input_line(0, newstate, ASSERT_LINE);
	else
		cpunum_set_input_line(0, 7, CLEAR_LINE);
}


static void scanline_update(int scanline)
{
	/* generate 32V signals */
	if ((scanline & 32) == 0 && !(readinputport(0) & 0x800))
		atarigen_scanline_int_gen();
}


static WRITE16_HANDLER( interrupt_ack_w )
{
	atarigen_scanline_int_ack_w(offset, data, mem_mask);
	atarigen_video_int_ack_w(offset, data, mem_mask);
}



/*************************************
 *
 *  Initialization
 *
 *************************************/

static MACHINE_RESET( klax )
{
	atarigen_eeprom_reset();
	atarigen_interrupt_reset(update_interrupts);
	atarigen_scanline_timer_reset(scanline_update, 32);
}



/*************************************
 *
 *  Sound I/O
 *
 *************************************/

static READ16_HANDLER( adpcm_r )
{
	return OKIM6295_status_0_r(offset) | 0xff00;
}


static WRITE16_HANDLER( adpcm_w )
{
	if (ACCESSING_LSB)
		OKIM6295_data_0_w(offset, data & 0xff);
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x0e0000, 0x0e0fff) AM_READ(atarigen_eeprom_r)
	AM_RANGE(0x260000, 0x260001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x260002, 0x260003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x270000, 0x270001) AM_READ(adpcm_r)
	AM_RANGE(0x3e0000, 0x3e07ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x3f0000, 0x3f3fff) AM_READ(MRA16_RAM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( main_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x0e0000, 0x0e0fff) AM_WRITE(atarigen_eeprom_w) AM_BASE(&atarigen_eeprom) AM_SIZE(&atarigen_eeprom_size)
	AM_RANGE(0x1f0000, 0x1fffff) AM_WRITE(atarigen_eeprom_enable_w)
	AM_RANGE(0x260000, 0x260001) AM_WRITE(klax_latch_w)
	AM_RANGE(0x270000, 0x270001) AM_WRITE(adpcm_w)
	AM_RANGE(0x2e0000, 0x2e0001) AM_WRITE(watchdog_reset16_w)
	AM_RANGE(0x360000, 0x360001) AM_WRITE(interrupt_ack_w)
	AM_RANGE(0x3e0000, 0x3e07ff) AM_WRITE(atarigen_expanded_666_paletteram_w) AM_BASE(&paletteram16)
	AM_RANGE(0x3f0000, 0x3f0f7f) AM_WRITE(atarigen_playfield_w) AM_BASE(&atarigen_playfield)
	AM_RANGE(0x3f0f80, 0x3f0fff) AM_WRITE(atarimo_0_slipram_w) AM_BASE(&atarimo_0_slipram)
	AM_RANGE(0x3f1000, 0x3f1fff) AM_WRITE(atarigen_playfield_upper_w) AM_BASE(&atarigen_playfield_upper)
	AM_RANGE(0x3f2000, 0x3f27ff) AM_WRITE(atarimo_0_spriteram_w) AM_BASE(&atarimo_0_spriteram)
	AM_RANGE(0x3f2800, 0x3f3fff) AM_WRITE(MWA16_RAM)
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( klax )
	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x00fc, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0600, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)

	PORT_START
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0600, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x0800, IP_ACTIVE_LOW )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout pfmolayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ 0, 1, 2, 3 },
	{ 0, 4, RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 8, 12, RGN_FRAC(1,2)+8, RGN_FRAC(1,2)+12 },
	{ 0*8, 2*8, 4*8, 6*8, 8*8, 10*8, 12*8, 14*8 },
	16*8
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &pfmolayout,  256, 16 },		/* sprites & playfield */
	{ REGION_GFX2, 0, &pfmolayout,    0, 16 },		/* sprites & playfield */
	{ -1 }
};



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( klax )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, ATARI_CLOCK_14MHz/2)
	MDRV_CPU_PROGRAM_MAP(main_readmem,main_writemem)
	MDRV_CPU_VBLANK_INT(atarigen_video_int_gen,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(klax)
	MDRV_NVRAM_HANDLER(atarigen)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_NEEDS_6BITS_PER_GUN | VIDEO_UPDATE_BEFORE_VBLANK)
	MDRV_SCREEN_SIZE(42*8, 30*8)
	MDRV_VISIBLE_AREA(0*8, 42*8-1, 0*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(512)

	MDRV_VIDEO_START(klax)
	MDRV_VIDEO_UPDATE(klax)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, ATARI_CLOCK_14MHz/4/4/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( klax )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-6.006", 0x00000, 0x10000, CRC(e8991709) SHA1(90d69b0712e68e842a8b946539f1f43ef165e8de) )
	ROM_LOAD16_BYTE( "136075-6.005", 0x00001, 0x10000, CRC(72b8c510) SHA1(f79d3a2de4deaabbcec632e8be9a1d5f6c0c3740) )
	ROM_LOAD16_BYTE( "136075-6.008", 0x20000, 0x10000, CRC(c7c91a9d) SHA1(9f79ca689ec635f8113a74162e81f253c88992f5) )
	ROM_LOAD16_BYTE( "136075-6.007", 0x20001, 0x10000, CRC(d2021a88) SHA1(0f8a0dcc3bb5ca433601b1abfc796c98791facf6) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.010", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD( "136075-2.012", 0x10000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD( "136075-2.009", 0x20000, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD( "136075-2.011", 0x30000, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.014", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD( "136075-2.013", 0x10000, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM data */
	ROM_LOAD( "136075-1.015", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1.016", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )
ROM_END


ROM_START( klax2 )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075.006",   0x00000, 0x10000, CRC(05c98fc0) SHA1(84880d3d65c46c96c739063b3f61b1663989c56e) )
	ROM_LOAD16_BYTE( "136075.005",   0x00001, 0x10000, CRC(d461e1ee) SHA1(73e8615a742555f74c1086c0b745afc7e94a478f) )
	ROM_LOAD16_BYTE( "136075.008",   0x20000, 0x10000, CRC(f1b8e588) SHA1(080511f90aecb7526ab2107c196e73cb881a2bb5) )
	ROM_LOAD16_BYTE( "136075.007",   0x20001, 0x10000, CRC(adbe33a8) SHA1(c6c4f9ea5224169dbf4dda1062954563ebab18d4) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.010", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD( "136075-2.012", 0x10000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD( "136075-2.009", 0x20000, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD( "136075-2.011", 0x30000, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.014", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD( "136075-2.013", 0x10000, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM data */
	ROM_LOAD( "136075-1.015", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1.016", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )
ROM_END


ROM_START( klax3 )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-5006", 0x00000, 0x10000, CRC(65eb9a31) SHA1(3f47d58fe9eb154ab14ac282919f92679b5c7922) )
	ROM_LOAD16_BYTE( "136075-5005", 0x00001, 0x10000, CRC(7be27349) SHA1(79eef2b7f4a0fb6991d81f6543d5ae00de9f2452) )
	ROM_LOAD16_BYTE( "136075-4008", 0x20000, 0x10000, CRC(f3c79106) SHA1(c315159020d5bc6f919c3fb975fb8b228584f88c) )
	ROM_LOAD16_BYTE( "136075-4007", 0x20001, 0x10000, CRC(a23cde5d) SHA1(51afadc900524d73ff7906b003fdf801f5d1f1fd) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.010", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD( "136075-2.012", 0x10000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD( "136075-2.009", 0x20000, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD( "136075-2.011", 0x30000, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.014", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD( "136075-2.013", 0x10000, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM data */
	ROM_LOAD( "136075-1.015", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1.016", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )
ROM_END


ROM_START( klaxj )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-3.406", 0x00000, 0x10000, CRC(ab2aa50b) SHA1(0ebffc8b4724eb8c4423e0b1f62b0fff7cc30aab) )
	ROM_LOAD16_BYTE( "136075-3.405", 0x00001, 0x10000, CRC(9dc9a590) SHA1(4c77b1ad9c083325f33520f2b6aa598dde247ad8) )
	ROM_LOAD16_BYTE( "136075-2.408", 0x20000, 0x10000, CRC(89d515ce) SHA1(4991b859a53f34776671f660dbdb18a746259549) )
	ROM_LOAD16_BYTE( "136075-2.407", 0x20001, 0x10000, CRC(48ce4edb) SHA1(014f879298408295a338c19c2d518524b41491cb) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.010", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD( "136075-2.012", 0x10000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD( "136075-2.009", 0x20000, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD( "136075-2.011", 0x30000, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.014", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD( "136075-2.013", 0x10000, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM data */
	ROM_LOAD( "136075-1.015", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1.016", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )
ROM_END


ROM_START( klaxd )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "2206.bin",     0x00000, 0x10000, CRC(9d1a713b) SHA1(6e60a43934bd8959c5c07dd12e087c63ea791bb9) )
	ROM_LOAD16_BYTE( "1205.bin",     0x00001, 0x10000, CRC(45065a5a) SHA1(77339ca04e54a04489ce9d6e11816475e57d1311) )
	ROM_LOAD16_BYTE( "1208.bin",     0x20000, 0x10000, CRC(b4019b32) SHA1(83fba82a9100af14cddd812be9f3dbd58d8511d2) )
	ROM_LOAD16_BYTE( "1207.bin",     0x20001, 0x10000, CRC(14550a75) SHA1(35599a339e6978682a09db4fb78c76bb3d3b6bc7) )

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.010", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD( "136075-2.012", 0x10000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD( "136075-2.009", 0x20000, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD( "136075-2.011", 0x30000, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "136075-2.014", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD( "136075-2.013", 0x10000, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM data */
	ROM_LOAD( "136075-1.015", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1.016", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( klax )
{
	atarigen_eeprom_default = NULL;
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1989, klax,  0,    klax, klax, klax, ROT0, "Atari Games", "Klax (set 1)", 0 ,1)
GAME( 1989, klax2, klax, klax, klax, klax, ROT0, "Atari Games", "Klax (set 2)", 0 ,1)
GAME( 1989, klax3, klax, klax, klax, klax, ROT0, "Atari Games", "Klax (set 3)", 0 ,1)
GAME( 1989, klaxj, klax, klax, klax, klax, ROT0, "Atari Games", "Klax (Japan)", 0 ,1)
GAME( 1989, klaxd, klax, klax, klax, klax, ROT0, "Atari Games", "Klax (Germany)", 0 ,1)
