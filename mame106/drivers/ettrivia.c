/*

 Enerdyne Technologies Inc. (El Cajon, CA 92020) hardware

 CPU: Z80
 Sound: AY-3-8912 (x3)
 Other: Dallas DS1220Y NVRAM, N8T245N (x2), PAL16L8A-2CN (x2,protected)

 XTAL = 12 MHz

Supported games:

- Progressive Music Trivia  (c) 1985
- Super Trivia Master       (c) 1986

 driver by Pierpaolo Prazzoli, thanks to Tomasz Slanina too.

*/

#include "driver.h"
#include "sound/ay8910.h"

/* it uses the same palette layout as in naughtyb */
PALETTE_INIT( naughtyb );

static int palreg = 0;
static int gfx_bank = 0;
static int question_bank = 0;

static int b000_val = 0;
static int b000_ret = 0;
static int b800_prev = 0;

static UINT8 *bg_videoram, *fg_videoram;
static tilemap *bg_tilemap, *fg_tilemap;

static WRITE8_HANDLER( ettrivia_fg_w )
{
	fg_videoram[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap,offset);
}

static WRITE8_HANDLER( ettrivia_bg_w )
{
	bg_videoram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap,offset);
}

static WRITE8_HANDLER( ettrivia_control_w )
{
	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);

	palreg  = (data >> 1) & 3;
	gfx_bank = (data >> 2) & 1;

	question_bank = (data >> 3) & 3;

	coin_counter_w(0, data & 0x80);

	flip_screen_set(data & 1);
}

static READ8_HANDLER( ettrivia_question_r )
{
	UINT8 *QUESTIONS = memory_region(REGION_USER1);
	return QUESTIONS[offset + 0x10000 * question_bank];
}

static WRITE8_HANDLER( b000_w )
{
	b000_val = data;
}

static READ8_HANDLER( b000_r )
{
	if(b800_prev)
		return b000_ret;
	else
		return b000_val;
}

static WRITE8_HANDLER( b800_w )
{
	switch(data)
	{
		/* special case to return the value written to 0xb000 */
		/* does it reset the chips too ? */
		case 0:	break;
		case 0xc4: b000_ret = AY8910_read_port_0_r(0);	break;
		case 0x94: b000_ret = AY8910_read_port_1_r(0);	break;
		case 0x86: b000_ret = AY8910_read_port_2_r(0);	break;

		case 0x80:
			switch(b800_prev)
			{
				case 0xe0: AY8910_control_port_0_w(0,b000_val);	break;
				case 0x98: AY8910_control_port_1_w(0,b000_val);	break;
				case 0x83: AY8910_control_port_2_w(0,b000_val);	break;

				case 0xa0: AY8910_write_port_0_w(0,b000_val);	break;
				case 0x88: AY8910_write_port_1_w(0,b000_val);	break;
				case 0x81: AY8910_write_port_2_w(0,b000_val);	break;

			}
		break;
	}

	b800_prev = data;
}

static ADDRESS_MAP_START( cpu_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x9000, 0x9000) AM_WRITE(ettrivia_control_w)
	AM_RANGE(0x9800, 0x9800) AM_WRITENOP
	AM_RANGE(0xa000, 0xa000) AM_WRITENOP
	AM_RANGE(0xb000, 0xb000) AM_READ(b000_r) AM_WRITE(b000_w)
	AM_RANGE(0xb800, 0xb800) AM_WRITE(b800_w)
	AM_RANGE(0xc000, 0xc7ff) AM_RAM AM_WRITE(ettrivia_fg_w) AM_BASE(&fg_videoram)
	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_WRITE(ettrivia_bg_w) AM_BASE(&bg_videoram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( io_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x0000, 0xffff) AM_READ(ettrivia_question_r)
ADDRESS_MAP_END

INPUT_PORTS_START( ettrivia )
	PORT_START
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(1)
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(1,2), RGN_FRAC(0,2) },
	{ 7, 6, 5, 4, 3, 2, 1, 0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,    0, 32 },
	{ REGION_GFX2, 0, &charlayout, 32*4, 32 },
	{ -1 }
};

INLINE void get_tile_info(int tile_index, UINT8 *vidram, int gfx_code)
{
	int code = vidram[tile_index];
	int color = (code >> 5) + 8 * palreg;

	code += gfx_bank * 0x100;

	SET_TILE_INFO(gfx_code,code,color,0)
}

static void get_tile_info_bg(int tile_index)
{
	get_tile_info(tile_index, bg_videoram, 0);
}

static void get_tile_info_fg(int tile_index)
{
	get_tile_info(tile_index, fg_videoram, 1);
}

VIDEO_START( ettrivia )
{
	bg_tilemap = tilemap_create( get_tile_info_bg,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,64,32 );
	fg_tilemap = tilemap_create( get_tile_info_fg,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32 );

	if( !bg_tilemap || !fg_tilemap )
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);

	return 0;
}

VIDEO_UPDATE( ettrivia )
{
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
}

static struct AY8910interface ay8912_interface_2 =
{
	input_port_1_r
};

static struct AY8910interface ay8912_interface_3 =
{
	input_port_0_r
};


static INTERRUPT_GEN( ettrivia_interrupt )
{
	if( readinputport(2) & 0x01 )
	{
		cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	}
	else
	{
		cpunum_set_input_line(0, 0, PULSE_LINE);
	}
}

static MACHINE_DRIVER_START( ettrivia )
	MDRV_CPU_ADD(Z80,12000000/4-48000) //should be ok, it gives the 300 interrupts expected
	MDRV_CPU_PROGRAM_MAP(cpu_map,0)
	MDRV_CPU_IO_MAP(io_map,0)
	MDRV_CPU_VBLANK_INT(ettrivia_interrupt,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_NVRAM_HANDLER(generic_0fill)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 0*8, 28*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)
	MDRV_COLORTABLE_LENGTH(32*4+32*4)

	MDRV_PALETTE_INIT(naughtyb)
	MDRV_VIDEO_START(ettrivia)
	MDRV_VIDEO_UPDATE(ettrivia)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8912_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8912_interface_3)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END

ROM_START( promutrv )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u16.u16",      0x0000, 0x8000, CRC(e37d48be) SHA1(1d700cff0c28e50fa2851e0c46de21aa47a23416) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mt44ic.44",    0x0000, 0x1000, CRC(8d543ea4) SHA1(86ab848a45851540d5d3315e15b92f7b2ac0b77c) )
	ROM_LOAD( "mt46ic.46",    0x1000, 0x1000, CRC(6d6e1f68) SHA1(e8196ecd915a2528122407d31a7078f177be0beb) )

	ROM_REGION( 0x2000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mt48ic.48",    0x0000, 0x1000, CRC(f2efe300) SHA1(419e889b2f4d038ae64e3ccf4e2498add80b4c9f) )
	ROM_LOAD( "mt50ic.50",    0x1000, 0x1000, CRC(ee89d24e) SHA1(e3536df549278040255657201433ab23e0386533) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "ic63.prm",     0x0000, 0x0100, CRC(749da5a8) SHA1(8e30f5b014bc8ff2dc4986ef35a979e525681cb9) ) /* palette low bits */
	ROM_LOAD( "ic64.prm",     0x0100, 0x0100, CRC(1cf9c914) SHA1(4c39b10c1be889d6ef4313b2112f4216d34f7327) ) /* palette high bits */

	ROM_REGION( 0x40000, REGION_USER1, 0 ) /* Question roms */
	ROM_LOAD( "movie-tv.lo0", 0x00000, 0x8000, CRC(dbf03e62) SHA1(0210442ff80cce8fe39ba5e373bca0f47bb389c4) )
	ROM_LOAD( "movie-tv.hi0", 0x08000, 0x8000, CRC(77f09aab) SHA1(007ae0ec1f37b575412fa71c92d1891a62069089) )
	ROM_LOAD( "scifi.lo1",    0x10000, 0x8000, CRC(b5595f81) SHA1(5e7fa334f6541860a5c04e5f345673ea12efafb4) )
	ROM_LOAD( "enter3.hi1",   0x18000, 0x8000, CRC(a8cf603b) SHA1(6efa5753d8d252452b3f5be8635a28364e4d8de1) )
	ROM_LOAD( "sports3.lo2",  0x20000, 0x8000, CRC(bb28fa92) SHA1(a3c4c67be0e31793d68b0b048f3a73e9ce1d5859) )
	ROM_LOAD( "life-sci.hi2", 0x28000, 0x8000, CRC(975d48f4) SHA1(8c702da2178b1429b3c055a33917f44ca46aedb9) )
	ROM_LOAD( "wars.lo3",     0x30000, 0x8000, CRC(c437f9a8) SHA1(c625c46723e279474b52b05c4ec95f1df428505d) )
	ROM_LOAD( "soaps.hi3",    0x38000, 0x8000, CRC(9e20614d) SHA1(02121f2c17768763658e77bf19ccaae38a07e509) )
ROM_END

ROM_START( promutra )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u16.u16",      0x0000, 0x8000, CRC(e37d48be) SHA1(1d700cff0c28e50fa2851e0c46de21aa47a23416) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mt44ic.44",    0x0000, 0x1000, CRC(8d543ea4) SHA1(86ab848a45851540d5d3315e15b92f7b2ac0b77c) )
	ROM_LOAD( "mt46ic.46",    0x1000, 0x1000, CRC(6d6e1f68) SHA1(e8196ecd915a2528122407d31a7078f177be0beb) )

	ROM_REGION( 0x2000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mt48ic.48",    0x0000, 0x1000, CRC(f2efe300) SHA1(419e889b2f4d038ae64e3ccf4e2498add80b4c9f) )
	ROM_LOAD( "mt50ic.50",    0x1000, 0x1000, CRC(ee89d24e) SHA1(e3536df549278040255657201433ab23e0386533) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "ic63.prm",     0x0000, 0x0100, CRC(749da5a8) SHA1(8e30f5b014bc8ff2dc4986ef35a979e525681cb9) ) /* palette low bits */
	ROM_LOAD( "ic64.prm",     0x0100, 0x0100, CRC(1cf9c914) SHA1(4c39b10c1be889d6ef4313b2112f4216d34f7327) ) /* palette high bits */

	ROM_REGION( 0x40000, REGION_USER1, 0 ) /* Question roms */
	ROM_LOAD( "movie-tv.lo0", 0x00000, 0x8000, CRC(dbf03e62) SHA1(0210442ff80cce8fe39ba5e373bca0f47bb389c4) )
	ROM_LOAD( "movie-tv.hi0", 0x08000, 0x8000, CRC(77f09aab) SHA1(007ae0ec1f37b575412fa71c92d1891a62069089) )
	ROM_LOAD( "rock-pop.lo1", 0x10000, 0x8000, CRC(4252bc23) SHA1(d6c5b3c5f227b043f298cea585bcb934538b8880) )
	ROM_LOAD( "rock-pop.hi1", 0x18000, 0x8000, CRC(272aba66) SHA1(305866b07c6bb2d71ee169b0e8c75f95896d4484) )
	ROM_LOAD( "country.lo2",  0x20000, 0x8000, CRC(44673138) SHA1(4e5a3181300bd5f0e9336c2d0ddf900a9b4256d9) )
	ROM_LOAD( "country.hi2",  0x28000, 0x8000, CRC(3d35a612) SHA1(9d17477c8097b1110ed752caa6d280160368eac1) )
	ROM_LOAD( "sex.lo3",      0x30000, 0x8000, CRC(397b9c47) SHA1(bbb05f2ef22be0c099bb21139d21005039c61c31) )
	ROM_LOAD( "enter3.hi3",   0x38000, 0x8000, CRC(a8cf603b) SHA1(6efa5753d8d252452b3f5be8635a28364e4d8de1) )
ROM_END

ROM_START( promutrb )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u16.u16",      0x0000, 0x8000, CRC(e37d48be) SHA1(1d700cff0c28e50fa2851e0c46de21aa47a23416) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mt44ic.44",    0x0000, 0x1000, CRC(8d543ea4) SHA1(86ab848a45851540d5d3315e15b92f7b2ac0b77c) )
	ROM_LOAD( "mt46ic.46",    0x1000, 0x1000, CRC(6d6e1f68) SHA1(e8196ecd915a2528122407d31a7078f177be0beb) )

	ROM_REGION( 0x2000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mt48ic.48",    0x0000, 0x1000, CRC(f2efe300) SHA1(419e889b2f4d038ae64e3ccf4e2498add80b4c9f) )
	ROM_LOAD( "mt50ic.50",    0x1000, 0x1000, CRC(ee89d24e) SHA1(e3536df549278040255657201433ab23e0386533) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "ic63.prm",     0x0000, 0x0100, CRC(749da5a8) SHA1(8e30f5b014bc8ff2dc4986ef35a979e525681cb9) ) /* palette low bits */
	ROM_LOAD( "ic64.prm",     0x0100, 0x0100, CRC(1cf9c914) SHA1(4c39b10c1be889d6ef4313b2112f4216d34f7327) ) /* palette high bits */

	ROM_REGION( 0x40000, REGION_USER1, 0 ) /* Question roms */
	ROM_LOAD( "movie-tv.lo0", 0x00000, 0x8000, CRC(dbf03e62) SHA1(0210442ff80cce8fe39ba5e373bca0f47bb389c4) )
	ROM_LOAD( "movie-tv.hi0", 0x08000, 0x8000, CRC(77f09aab) SHA1(007ae0ec1f37b575412fa71c92d1891a62069089) )
	ROM_LOAD( "rock-pop.lo1", 0x10000, 0x8000, CRC(4252bc23) SHA1(d6c5b3c5f227b043f298cea585bcb934538b8880) )
	ROM_LOAD( "rock-pop.hi1", 0x18000, 0x8000, CRC(272aba66) SHA1(305866b07c6bb2d71ee169b0e8c75f95896d4484) )
	ROM_LOAD( "country.lo2",  0x20000, 0x8000, CRC(44673138) SHA1(4e5a3181300bd5f0e9336c2d0ddf900a9b4256d9) )
	ROM_LOAD( "country.hi2",  0x28000, 0x8000, CRC(3d35a612) SHA1(9d17477c8097b1110ed752caa6d280160368eac1) )
	ROM_LOAD( "enter3.lo3",   0x30000, 0x8000, CRC(a8cf603b) SHA1(6efa5753d8d252452b3f5be8635a28364e4d8de1) )
	ROM_LOAD( "geninfo.hi3",  0x38000, 0x8000, CRC(2747fd74) SHA1(d34ac30349dc965ecd8b05b3f1cb7ee24627f369) )
ROM_END

ROM_START( strvmstr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "stm16.u16",    0x0000, 0x8000, CRC(ae734db9) SHA1(1bacdfdebaa1f250bfbd49053c3910f1396afe11) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "stm44.rom",    0x0000, 0x1000, CRC(e69da710) SHA1(218a9d7600d67858d1f21282a0cebec0ae93e0ff) )
	ROM_LOAD( "stm46.rom",    0x1000, 0x1000, CRC(d927a1f1) SHA1(63a49a61107deaf7a9f28b9653c310c5331f5143) )

	ROM_REGION( 0x2000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "stm48.rom",    0x0000, 0x1000, CRC(51719714) SHA1(fdecbd22ea65eec7b4b5138f89ddc5876b05def6) )
	ROM_LOAD( "stm50.rom",    0x1000, 0x1000, CRC(cfc1a1d1) SHA1(9ef38f12360dd946651e67770742ca72fa6846f1) )

	ROM_REGION( 0x0200, REGION_PROMS, 0 )
	ROM_LOAD( "stm63.prm",    0x0000, 0x0100, BAD_DUMP CRC(305271cf) SHA1(6fd5fe085d79ca7aa57010cffbdb2a85b9c24701) ) /* palette low bits */
	ROM_LOAD( "stm64.prm",    0x0100, 0x0100, BAD_DUMP CRC(69ebc0b8) SHA1(de2b936e3246e3bfc7e2ff9546c1854ec3504cc2) ) /* palette high bits */

	ROM_REGION( 0x40000, REGION_USER1, 0 ) /* Question roms */
	ROM_LOAD( "sex2.lo0",     0x00000, 0x8000, CRC(9c68b277) SHA1(34bc9d7b973fe482abd5e34a058b72eb5ec8db64) )
	ROM_LOAD( "sports.hi0",   0x08000, 0x8000, CRC(3678fb79) SHA1(4e40cc20707195c0e88e595f752a2982b531b57e) )
	ROM_LOAD( "movies.lo1",   0x10000, 0x8000, CRC(16cba1b7) SHA1(8aa3eff72d1ec8dac906f2e803a88578a9fe763c) )
	ROM_LOAD( "rock-pop.hi1", 0x18000, 0x8000, CRC(e2954db6) SHA1(d545236a844b63c85937ee8fb8e65bcd74b1bf43) )
	ROM_LOAD( "sci-fi.lo2",   0x20000, 0x8000, CRC(b5595f81) SHA1(5e7fa334f6541860a5c04e5f345673ea12efafb4) )
	ROM_LOAD( "cars.hi2",     0x28000, 0x8000, CRC(50310557) SHA1(7559c603625e4df442b440b8b08e6efef06e2781) )
	ROM_LOAD( "potprri.lo3",  0x30000, 0x8000, CRC(427eada9) SHA1(bac29ec637a17db95507c68fd73a8ce52744bf8e) )
	ROM_LOAD( "entrtn.hi3",   0x38000, 0x8000, CRC(a8cf603b) SHA1(6efa5753d8d252452b3f5be8635a28364e4d8de1) )
ROM_END

GAME( 1985, promutrv, 0,        ettrivia, ettrivia, 0, ROT270, "Enerdyne Technologies Inc.", "Progressive Music Trivia (Question set 1)", 0 ,0)
GAME( 1985, promutra, promutrv, ettrivia, ettrivia, 0, ROT270, "Enerdyne Technologies Inc.", "Progressive Music Trivia (Question set 2)", 0 ,0)
GAME( 1985, promutrb, promutrv, ettrivia, ettrivia, 0, ROT270, "Enerdyne Technologies Inc.", "Progressive Music Trivia (Question set 3)", 0 ,0)
GAME( 1986, strvmstr, 0,        ettrivia, ettrivia, 0, ROT270, "Enerdyne Technologies Inc.", "Super Trivia Master", GAME_WRONG_COLORS ,0)
