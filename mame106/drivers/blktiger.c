/***************************************************************************

  Black Tiger

  Driver provided by Paul Leaman

  Thanks to Ishmair for providing information about the screen
  layout on level 3.

Notes:
- sprites/tile priority is a guess. I didn't find a PROM that would simply
  translate to the scheme I implemented.

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/2203intf.h"


extern unsigned char *blktiger_txvideoram;

WRITE8_HANDLER( blktiger_screen_layout_w );

READ8_HANDLER( blktiger_bgvideoram_r );
WRITE8_HANDLER( blktiger_bgvideoram_w );
WRITE8_HANDLER( blktiger_txvideoram_w );
WRITE8_HANDLER( blktiger_video_control_w );
WRITE8_HANDLER( blktiger_video_enable_w );
WRITE8_HANDLER( blktiger_bgvideoram_bank_w );
WRITE8_HANDLER( blktiger_scrollx_w );
WRITE8_HANDLER( blktiger_scrolly_w );

VIDEO_START( blktiger );
VIDEO_UPDATE( blktiger );
VIDEO_EOF( blktiger );



/* this is a protection check. The game crashes (thru a jump to 0x8000) */
/* if a read from this address doesn't return the value it expects. */
static READ8_HANDLER( blktiger_protection_r )
{
	int data = activecpu_get_reg(Z80_DE) >> 8;
	loginfo(2,"protection read, PC: %04x Result:%02x\n",activecpu_get_pc(),data);
	return data;
}

static WRITE8_HANDLER( blktiger_bankswitch_w )
{
	memory_set_bank(1, data & 0x0f);
}

static WRITE8_HANDLER( blktiger_coinlockout_w )
{
	coin_lockout_w(0,~data & 0x01);
	coin_lockout_w(1,~data & 0x02);
}



static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1)
	AM_RANGE(0xc000, 0xcfff) AM_READ(blktiger_bgvideoram_r)
	AM_RANGE(0xd000, 0xffff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_WRITE(blktiger_bgvideoram_w)
	AM_RANGE(0xd000, 0xd7ff) AM_WRITE(blktiger_txvideoram_w) AM_BASE(&blktiger_txvideoram)
	AM_RANGE(0xd800, 0xdbff) AM_WRITE(paletteram_xxxxBBBBRRRRGGGG_split1_w) AM_BASE(&paletteram)
	AM_RANGE(0xdc00, 0xdfff) AM_WRITE(paletteram_xxxxBBBBRRRRGGGG_split2_w) AM_BASE(&paletteram_2)
	AM_RANGE(0xe000, 0xfdff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xfe00, 0xffff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(input_port_0_r)
	AM_RANGE(0x01, 0x01) AM_READ(input_port_1_r)
	AM_RANGE(0x02, 0x02) AM_READ(input_port_2_r)
	AM_RANGE(0x03, 0x03) AM_READ(input_port_3_r)
	AM_RANGE(0x04, 0x04) AM_READ(input_port_4_r)
	AM_RANGE(0x05, 0x05) AM_READ(input_port_5_r)
	AM_RANGE(0x07, 0x07) AM_READ(blktiger_protection_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(soundlatch_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(blktiger_bankswitch_w)
	AM_RANGE(0x03, 0x03) AM_WRITE(blktiger_coinlockout_w)
	AM_RANGE(0x04, 0x04) AM_WRITE(blktiger_video_control_w)
	AM_RANGE(0x06, 0x06) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x07, 0x07) AM_WRITE(MWA8_NOP) /* Software protection (7) */
	AM_RANGE(0x08, 0x09) AM_WRITE(blktiger_scrollx_w)
	AM_RANGE(0x0a, 0x0b) AM_WRITE(blktiger_scrolly_w)
	AM_RANGE(0x0c, 0x0c) AM_WRITE(blktiger_video_enable_w)
	AM_RANGE(0x0d, 0x0d) AM_WRITE(blktiger_bgvideoram_bank_w)
	AM_RANGE(0x0e, 0x0e) AM_WRITE(blktiger_screen_layout_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xc800, 0xc800) AM_READ(soundlatch_r)
	AM_RANGE(0xe000, 0xe000) AM_READ(YM2203_status_port_0_r)
	AM_RANGE(0xe001, 0xe001) AM_READ(YM2203_read_port_0_r)
	AM_RANGE(0xe002, 0xe002) AM_READ(YM2203_status_port_1_r)
	AM_RANGE(0xe003, 0xe003) AM_READ(YM2203_read_port_1_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(YM2203_control_port_0_w)
	AM_RANGE(0xe001, 0xe001) AM_WRITE(YM2203_write_port_0_w)
	AM_RANGE(0xe002, 0xe002) AM_WRITE(YM2203_control_port_1_w)
	AM_RANGE(0xe003, 0xe003) AM_WRITE(YM2203_write_port_1_w)
ADDRESS_MAP_END



INPUT_PORTS_START( blktiger )
	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* DSW0 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START      /* DSW1 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "7")
	PORT_DIPNAME( 0x1c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x1c, "1 (Easiest)")
	PORT_DIPSETTING(    0x18, "2" )
	PORT_DIPSETTING(    0x14, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x0c, "5 (Normal)" )
	PORT_DIPSETTING(    0x08, "6" )
	PORT_DIPSETTING(    0x04, "7" )
	PORT_DIPSETTING(    0x00, "8 (Hardest)" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )	/* could be VBLANK */
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 4, 0 },
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+4, RGN_FRAC(1,2)+0, 4, 0 },
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			16*16+0, 16*16+1, 16*16+2, 16*16+3, 16*16+8+0, 16*16+8+1, 16*16+8+2, 16*16+8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	32*16
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0x300, 32 },	/* colors 0x300-0x37f */
	{ REGION_GFX2, 0, &spritelayout, 0x000, 16 },	/* colors 0x000-0x0ff */
	{ REGION_GFX3, 0, &spritelayout, 0x200,  8 },	/* colors 0x200-0x27f */
	{ -1 } /* end of array */
};



/* handler called by the 2203 emulator when the internal timers cause an IRQ */
static void irqhandler(int irq)
{
	cpunum_set_input_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2203interface ym2203_interface =
{
	0,
	0,
	0,
	0,
	irqhandler
};

static MACHINE_START( blktiger )
{
	/* configure bankswitching */
	memory_configure_bank(1, 0, 16, memory_region(REGION_CPU1) + 0x10000, 0x4000);
	return 0;
}

static MACHINE_DRIVER_START( blktiger )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 4000000)	/* 4 MHz (?) */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 3000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_MACHINE_START(blktiger)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(blktiger)
	MDRV_VIDEO_EOF(blktiger)
	MDRV_VIDEO_UPDATE(blktiger)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, 3579545)
	MDRV_SOUND_CONFIG(ym2203_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.15)

	MDRV_SOUND_ADD(YM2203, 3579545)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.15)
MACHINE_DRIVER_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( blktiger )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )	/* 64k for code + banked ROMs images */
	ROM_LOAD( "bdu-01a.5e",  0x00000, 0x08000, CRC(a8f98f22) SHA1(f77c0d0ebf3e52a21d2c0c5004350a408b8e6d24) )	/* CODE */
	ROM_LOAD( "bdu-02a.6e",  0x10000, 0x10000, CRC(7bef96e8) SHA1(6d05a73d8400dead78c561b904bf6ef8311e7b91) )	/* 0+1 */
	ROM_LOAD( "bdu-03a.8e",  0x20000, 0x10000, CRC(4089e157) SHA1(7972b1c745057802d4fd66d88b0101eb3c03e701) )	/* 2+3 */
	ROM_LOAD( "bd-04.9e",    0x30000, 0x10000, CRC(ed6af6ec) SHA1(bed303c51bcddf233ad0701306d557a60ce9f5a5) )	/* 4+5 */
	ROM_LOAD( "bd-05.10e",   0x40000, 0x10000, CRC(ae59b72e) SHA1(6e72214b71f2f337af236c8be891a18570cb6fbb) )	/* 6+7 */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "bd-06.1l",  0x0000, 0x8000, CRC(2cf54274) SHA1(87df100c65999ba1e9d358ffd0fe4bba23ae0efb) )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-15.2n",  0x00000, 0x08000, CRC(70175d78) SHA1(2f02be2785d1824002145ea20db79821d0393929) )	/* characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-12.5b",  0x00000, 0x10000, CRC(c4524993) SHA1(9aa6c58004ca1117e5ac44ba8fc51e9128b921b8) )	/* tiles */
	ROM_LOAD( "bd-11.4b",  0x10000, 0x10000, CRC(7932c86f) SHA1(b3b1bc1e2b0db5c2eb8772f8a2c35129cc80d511) )
	ROM_LOAD( "bd-14.9b",  0x20000, 0x10000, CRC(dc49593a) SHA1(e4ef42ba9f238fd43c8217657c92896f31d3912c) )
	ROM_LOAD( "bd-13.8b",  0x30000, 0x10000, CRC(7ed7a122) SHA1(3acc6d4c9731db0609c2e26e3bd255847149ca33) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-08.5a",   0x00000, 0x10000, CRC(e2f17438) SHA1(3e5fdae07d40febedc59c7c7c4d9c6f0d72b58b5) )	/* sprites */
	ROM_LOAD( "bd-07.4a",   0x10000, 0x10000, CRC(5fccbd27) SHA1(33c55aa9c12b3121ca5c3b4c39a9b152b6946461) )
	ROM_LOAD( "bd-10.9a",   0x20000, 0x10000, CRC(fc33ccc6) SHA1(d492626a88565c2626f98ecb1d74535f1ad68e4c) )
	ROM_LOAD( "bd-09.8a",   0x30000, 0x10000, CRC(f449de01) SHA1(f6b40e9eb2471b89c42ab84f4214295d284db0c3) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )	/* PROMs (function unknown) */
	ROM_LOAD( "bd01.8j",   0x0000, 0x0100, CRC(29b459e5) SHA1(0034734a533df3dea16b7b48e072485d7f26f850) )
	ROM_LOAD( "bd02.9j",   0x0100, 0x0100, CRC(8b741e66) SHA1(6c1fda59936a7217b05949f5c54b1f91f4b49dbe) )
	ROM_LOAD( "bd03.11k",  0x0200, 0x0100, CRC(27201c75) SHA1(c54d87f06bfe0b0908389c005014d97156e272c2) )
	ROM_LOAD( "bd04.11l",  0x0300, 0x0100, CRC(e5490b68) SHA1(40f9f92efe7dd97b49144aec02eb509834056915) )
ROM_END

ROM_START( bktigerb )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )	/* 64k for code + banked ROMs images */
	ROM_LOAD( "btiger1.f6",   0x00000, 0x08000, CRC(9d8464e8) SHA1(c847ee9a22b8b636e85427214747e6bd779023e8) )	/* CODE */
	ROM_LOAD( "bdu-02a.6e",   0x10000, 0x10000, CRC(7bef96e8) SHA1(6d05a73d8400dead78c561b904bf6ef8311e7b91) )	/* 0+1 */
	ROM_LOAD( "btiger3.j6",   0x20000, 0x10000, CRC(52c56ed1) SHA1(b6ea61869dcfcedb8cfc14c613440e3f4649866f) )	/* 2+3 */
	ROM_LOAD( "db-04.9e",     0x30000, 0x10000, CRC(ed6af6ec) SHA1(bed303c51bcddf233ad0701306d557a60ce9f5a5) )	/* 4+5 */
	ROM_LOAD( "db-05.10e",    0x40000, 0x10000, CRC(ae59b72e) SHA1(6e72214b71f2f337af236c8be891a18570cb6fbb) )	/* 6+7 */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "bd-06.1l",  0x0000, 0x8000, CRC(2cf54274) SHA1(87df100c65999ba1e9d358ffd0fe4bba23ae0efb) )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-15.2n",  0x00000, 0x08000, CRC(70175d78) SHA1(2f02be2785d1824002145ea20db79821d0393929) )	/* characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-12.5b",  0x00000, 0x10000, CRC(c4524993) SHA1(9aa6c58004ca1117e5ac44ba8fc51e9128b921b8) )	/* tiles */
	ROM_LOAD( "bd-11.4b",  0x10000, 0x10000, CRC(7932c86f) SHA1(b3b1bc1e2b0db5c2eb8772f8a2c35129cc80d511) )
	ROM_LOAD( "bd-14.9b",  0x20000, 0x10000, CRC(dc49593a) SHA1(e4ef42ba9f238fd43c8217657c92896f31d3912c) )
	ROM_LOAD( "bd-13.8b",  0x30000, 0x10000, CRC(7ed7a122) SHA1(3acc6d4c9731db0609c2e26e3bd255847149ca33) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-08.5a",  0x00000, 0x10000, CRC(e2f17438) SHA1(3e5fdae07d40febedc59c7c7c4d9c6f0d72b58b5) )	/* sprites */
	ROM_LOAD( "bd-07.4a",  0x10000, 0x10000, CRC(5fccbd27) SHA1(33c55aa9c12b3121ca5c3b4c39a9b152b6946461) )
	ROM_LOAD( "bd-10.9a",  0x20000, 0x10000, CRC(fc33ccc6) SHA1(d492626a88565c2626f98ecb1d74535f1ad68e4c) )
	ROM_LOAD( "bd-09.8a",  0x30000, 0x10000, CRC(f449de01) SHA1(f6b40e9eb2471b89c42ab84f4214295d284db0c3) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )	/* PROMs (function unknown) */
	ROM_LOAD( "bd01.8j",   0x0000, 0x0100, CRC(29b459e5) SHA1(0034734a533df3dea16b7b48e072485d7f26f850) )
	ROM_LOAD( "bd02.9j",   0x0100, 0x0100, CRC(8b741e66) SHA1(6c1fda59936a7217b05949f5c54b1f91f4b49dbe) )
	ROM_LOAD( "bd03.11k",  0x0200, 0x0100, CRC(27201c75) SHA1(c54d87f06bfe0b0908389c005014d97156e272c2) )
	ROM_LOAD( "bd04.11l",  0x0300, 0x0100, CRC(e5490b68) SHA1(40f9f92efe7dd97b49144aec02eb509834056915) )
ROM_END

ROM_START( blkdrgon )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )	/* 64k for code + banked ROMs images */
	ROM_LOAD( "blkdrgon.5e",  0x00000, 0x08000, CRC(27ccdfbc) SHA1(3caafe00735ba9b24d870ee61ad2cae541551024) )	/* CODE */
	ROM_LOAD( "blkdrgon.6e",  0x10000, 0x10000, CRC(7d39c26f) SHA1(562a3f578e109ae020f65e341c876ad7e510a311) )	/* 0+1 */
	ROM_LOAD( "blkdrgon.8e",  0x20000, 0x10000, CRC(d1bf3757) SHA1(b19f8b986406bde65ac7f0d55d54f87b37f5e42f) )	/* 2+3 */
	ROM_LOAD( "blkdrgon.9e",  0x30000, 0x10000, CRC(4d1d6680) SHA1(e137624c59392de6aaffeded99b024938360bd25) )	/* 4+5 */
	ROM_LOAD( "blkdrgon.10e", 0x40000, 0x10000, CRC(c8d0c45e) SHA1(66c2e5a74c5875a2c8e28740fe944bd943246ce5) )	/* 6+7 */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "bd-06.1l",  0x0000, 0x8000, CRC(2cf54274) SHA1(87df100c65999ba1e9d358ffd0fe4bba23ae0efb) )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "blkdrgon.2n",  0x00000, 0x08000, CRC(3821ab29) SHA1(576f1839f63b0cad6b851d6e6a3e9dec21ac811d) )	/* characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "blkdrgon.5b",  0x00000, 0x10000, CRC(22d0a4b0) SHA1(f9402ea9ffedcb280497a63c5eb352de9d4ca3fd) )	/* tiles */
	ROM_LOAD( "blkdrgon.4b",  0x10000, 0x10000, CRC(c8b5fc52) SHA1(621e899285ce6302e5b25d133d9cd52c09b7b202) )
	ROM_LOAD( "blkdrgon.9b",  0x20000, 0x10000, CRC(9498c378) SHA1(841934ddef724faf04162c4be4aea1684d8d8e0f) )
	ROM_LOAD( "blkdrgon.8b",  0x30000, 0x10000, CRC(5b0df8ce) SHA1(57d10b48bd61b0224ce21b36bde8d2479e8e5df4) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-08.5a",  0x00000, 0x10000, CRC(e2f17438) SHA1(3e5fdae07d40febedc59c7c7c4d9c6f0d72b58b5) )	/* sprites */
	ROM_LOAD( "bd-07.4a",  0x10000, 0x10000, CRC(5fccbd27) SHA1(33c55aa9c12b3121ca5c3b4c39a9b152b6946461) )
	ROM_LOAD( "bd-10.9a",  0x20000, 0x10000, CRC(fc33ccc6) SHA1(d492626a88565c2626f98ecb1d74535f1ad68e4c) )
	ROM_LOAD( "bd-09.8a",  0x30000, 0x10000, CRC(f449de01) SHA1(f6b40e9eb2471b89c42ab84f4214295d284db0c3) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )	/* PROMs (function unknown) */
	ROM_LOAD( "bd01.8j",   0x0000, 0x0100, CRC(29b459e5) SHA1(0034734a533df3dea16b7b48e072485d7f26f850) )
	ROM_LOAD( "bd02.9j",   0x0100, 0x0100, CRC(8b741e66) SHA1(6c1fda59936a7217b05949f5c54b1f91f4b49dbe) )
	ROM_LOAD( "bd03.11k",  0x0200, 0x0100, CRC(27201c75) SHA1(c54d87f06bfe0b0908389c005014d97156e272c2) )
	ROM_LOAD( "bd04.11l",  0x0300, 0x0100, CRC(e5490b68) SHA1(40f9f92efe7dd97b49144aec02eb509834056915) )
ROM_END

ROM_START( blkdrgnb )
	ROM_REGION( 0x50000, REGION_CPU1, 0 )	/* 64k for code + banked ROMs images */
	ROM_LOAD( "a1",           0x00000, 0x08000, CRC(7caf2ba0) SHA1(57b17caff67d36b24075f5865d433bfc8bcc9bc2) )	/* CODE */
	ROM_LOAD( "blkdrgon.6e",  0x10000, 0x10000, CRC(7d39c26f) SHA1(562a3f578e109ae020f65e341c876ad7e510a311) )	/* 0+1 */
	ROM_LOAD( "a3",           0x20000, 0x10000, CRC(f4cd0f39) SHA1(9efc5161c861c7ec8ae72509e71c6d7b71b22fc6) )	/* 2+3 */
	ROM_LOAD( "blkdrgon.9e",  0x30000, 0x10000, CRC(4d1d6680) SHA1(e137624c59392de6aaffeded99b024938360bd25) )	/* 4+5 */
	ROM_LOAD( "blkdrgon.10e", 0x40000, 0x10000, CRC(c8d0c45e) SHA1(66c2e5a74c5875a2c8e28740fe944bd943246ce5) )	/* 6+7 */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "bd-06.1l",  0x0000, 0x8000, CRC(2cf54274) SHA1(87df100c65999ba1e9d358ffd0fe4bba23ae0efb) )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "b5",           0x00000, 0x08000, CRC(852ad2b7) SHA1(9f30c0d7e1127589b03d8f45ea50e0f907181a4b) )	/* characters */

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "blkdrgon.5b",  0x00000, 0x10000, CRC(22d0a4b0) SHA1(f9402ea9ffedcb280497a63c5eb352de9d4ca3fd) )	/* tiles */
	ROM_LOAD( "b1",           0x10000, 0x10000, CRC(053ab15c) SHA1(f0ddc71009ab5dd69ae463c3636ec2332c0556f8) )
	ROM_LOAD( "blkdrgon.9b",  0x20000, 0x10000, CRC(9498c378) SHA1(841934ddef724faf04162c4be4aea1684d8d8e0f) )
	ROM_LOAD( "b3",           0x30000, 0x10000, CRC(9dc6e943) SHA1(0818c1fb2cc8ff403a479457b268bba6ec0730bc) )

	ROM_REGION( 0x40000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "bd-08.5a",  0x00000, 0x10000, CRC(e2f17438) SHA1(3e5fdae07d40febedc59c7c7c4d9c6f0d72b58b5) )	/* sprites */
	ROM_LOAD( "bd-07.4a",  0x10000, 0x10000, CRC(5fccbd27) SHA1(33c55aa9c12b3121ca5c3b4c39a9b152b6946461) )
	ROM_LOAD( "bd-10.9a",  0x20000, 0x10000, CRC(fc33ccc6) SHA1(d492626a88565c2626f98ecb1d74535f1ad68e4c) )
	ROM_LOAD( "bd-09.8a",  0x30000, 0x10000, CRC(f449de01) SHA1(f6b40e9eb2471b89c42ab84f4214295d284db0c3) )

	ROM_REGION( 0x0400, REGION_PROMS, 0 )	/* PROMs (function unknown) */
	ROM_LOAD( "bd01.8j",   0x0000, 0x0100, CRC(29b459e5) SHA1(0034734a533df3dea16b7b48e072485d7f26f850) )
	ROM_LOAD( "bd02.9j",   0x0100, 0x0100, CRC(8b741e66) SHA1(6c1fda59936a7217b05949f5c54b1f91f4b49dbe) )
	ROM_LOAD( "bd03.11k",  0x0200, 0x0100, CRC(27201c75) SHA1(c54d87f06bfe0b0908389c005014d97156e272c2) )
	ROM_LOAD( "bd04.11l",  0x0300, 0x0100, CRC(e5490b68) SHA1(40f9f92efe7dd97b49144aec02eb509834056915) )
ROM_END



GAME( 1987, blktiger, 0,        blktiger, blktiger, 0, ROT0, "Capcom", "Black Tiger", GAME_SUPPORTS_SAVE ,2)
GAME( 1987, bktigerb, blktiger, blktiger, blktiger, 0, ROT0, "bootleg", "Black Tiger (bootleg)", GAME_SUPPORTS_SAVE ,2)
GAME( 1987, blkdrgon, blktiger, blktiger, blktiger, 0, ROT0, "Capcom", "Black Dragon", GAME_SUPPORTS_SAVE ,2)
GAME( 1987, blkdrgnb, blktiger, blktiger, blktiger, 0, ROT0, "bootleg", "Black Dragon (bootleg)", GAME_SUPPORTS_SAVE ,2)
