/***************************************************************************

    Prehistoric Isle in 1930 (World)        (c) 1989 SNK
    Prehistoric Isle in 1930 (USA)          (c) 1989 SNK
    Genshi-Tou 1930's (Japan)               (c) 1989 SNK

    Emulation by Bryan McPhail, mish@tendril.co.uk

***************************************************************************/

#include "driver.h"
#include "sound/upd7759.h"
#include "sound/3812intf.h"

extern WRITE16_HANDLER( prehisle_bg_videoram16_w );
extern WRITE16_HANDLER( prehisle_fg_videoram16_w );
extern WRITE16_HANDLER( prehisle_control16_w );
extern READ16_HANDLER( prehisle_control16_r );

extern VIDEO_START( prehisle );
extern VIDEO_UPDATE( prehisle );

extern UINT16 *prehisle_bg_videoram16;

static UINT16 *prehisle_ram16;

/******************************************************************************/

static WRITE16_HANDLER( prehisle_sound16_w )
{
	soundlatch_w(0, data & 0xff);
	cpunum_set_input_line(1, INPUT_LINE_NMI, PULSE_LINE);
}

/*******************************************************************************/

static ADDRESS_MAP_START( prehisle_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x070000, 0x073fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x090000, 0x0907ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0a0000, 0x0a07ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0b0000, 0x0b3fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0d0000, 0x0d07ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0e0000, 0x0e00ff) AM_READ(prehisle_control16_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( prehisle_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x070000, 0x073fff) AM_WRITE(MWA16_RAM) AM_BASE(&prehisle_ram16)
	AM_RANGE(0x090000, 0x0907ff) AM_WRITE(prehisle_fg_videoram16_w) AM_BASE(&videoram16)
	AM_RANGE(0x0a0000, 0x0a07ff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16)
	AM_RANGE(0x0b0000, 0x0b3fff) AM_WRITE(prehisle_bg_videoram16_w) AM_BASE(&prehisle_bg_videoram16)
	AM_RANGE(0x0d0000, 0x0d07ff) AM_WRITE(paletteram16_RRRRGGGGBBBBxxxx_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x0f0070, 0x0ff071) AM_WRITE(prehisle_sound16_w)
	AM_RANGE(0x0f0000, 0x0ff0ff) AM_WRITE(prehisle_control16_w)
ADDRESS_MAP_END

/******************************************************************************/

static WRITE8_HANDLER( D7759_write_port_0_w )
{
	upd7759_port_w(offset,data);
	upd7759_start_w (0,0);
	upd7759_start_w (0,1);
}

static ADDRESS_MAP_START( prehisle_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xefff) AM_READ(MRA8_ROM)
	AM_RANGE(0xf000, 0xf7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xf800, 0xf800) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( prehisle_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xefff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xf000, 0xf7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xf800, 0xf800) AM_WRITE(MWA8_NOP)	// ???
ADDRESS_MAP_END

static ADDRESS_MAP_START( prehisle_sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(YM3812_status_port_0_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( prehisle_sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(YM3812_control_port_0_w)
	AM_RANGE(0x20, 0x20) AM_WRITE(YM3812_write_port_0_w)
	AM_RANGE(0x40, 0x40) AM_WRITE(D7759_write_port_0_w)
	AM_RANGE(0x80, 0x80) AM_WRITE(upd7759_0_reset_w)
ADDRESS_MAP_END

/******************************************************************************/

INPUT_PORTS_START( prehisle )
	PORT_START	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2  )

	PORT_START	/* coin */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE_NO_TOGGLE( 0x08, IP_ACTIVE_LOW )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* Dip switches */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Level_Select ) )
	PORT_DIPSETTING(	0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(	0x04, "Only Twice" )
	PORT_DIPSETTING(	0x00, "Always" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coinage ) )
	PORT_DIPSETTING(	0x00, "A 4C/1C B 1C/4C" )
	PORT_DIPSETTING(	0x10, "A 3C/1C B 1C/3C" )
	PORT_DIPSETTING(	0x20, "A 2C/1C B 1C/2C" )
	PORT_DIPSETTING(	0x30, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(	0x80, "2" )
	PORT_DIPSETTING(	0xc0, "3" )
	PORT_DIPSETTING(	0x40, "4" )
	PORT_DIPSETTING(	0x00, "5" )

	PORT_START
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	0x02, DEF_STR( Easy ) )
	PORT_DIPSETTING(	0x03, DEF_STR( Standard ) )
	PORT_DIPSETTING(	0x01, "Middle" )
	PORT_DIPSETTING(	0x00, "Difficult" )
	PORT_DIPNAME( 0x0c, 0x0c, "Game Mode" )
	PORT_DIPSETTING(	0x08, "Demo Sounds Off" )
	PORT_DIPSETTING(	0x0c, "Demo Sounds On" )
	PORT_DIPSETTING(	0x00, "Freeze" )
	PORT_DIPSETTING(	0x04, "Infinite Lives" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(	0x30, "100K 200K" )
	PORT_DIPSETTING(	0x20, "150K 300K" )
	PORT_DIPSETTING(	0x10, "300K 500K" )
	PORT_DIPSETTING(	0x00, DEF_STR( None ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )
	PORT_DIPSETTING(	0x40, DEF_STR( Yes ) )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	1024,
	4,		/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0, 4, 8, 12, 16, 20, 24, 28},
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8	/* every char takes 32 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	16,16,	/* 16*16 sprites */
	0x800,
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0,4,8,12,16,20,24,28,
		0+64*8,4+64*8,8+64*8,12+64*8,16+64*8,20+64*8,24+64*8,28+64*8 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
		8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	128*8	/* every sprite takes 64 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,	/* 16*16 sprites */
	5120,
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0,4,8,12,16,20,24,28,
		0+64*8,4+64*8,8+64*8,12+64*8,16+64*8,20+64*8,24+64*8,28+64*8 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
		8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	128*8	/* every sprite takes 64 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,	 0, 16 },
	{ REGION_GFX2, 0, &tilelayout, 768, 16 },
	{ REGION_GFX3, 0, &tilelayout, 512, 16 },
	{ REGION_GFX4, 0, &spritelayout, 256, 16 },
	{ -1 } /* end of array */
};

/******************************************************************************/

static void irqhandler(int irq)
{
	cpunum_set_input_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM3812interface ym3812_interface =
{
	irqhandler
};

static struct upd7759_interface upd7759_interface =
{
	REGION_SOUND1			/* memory region */
};

/******************************************************************************/

static MACHINE_DRIVER_START( prehisle )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 12000000)
	MDRV_CPU_PROGRAM_MAP(prehisle_readmem,prehisle_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(prehisle_sound_readmem,prehisle_sound_writemem)
	MDRV_CPU_IO_MAP(prehisle_sound_readport,prehisle_sound_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(prehisle)
	MDRV_VIDEO_UPDATE(prehisle)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3812, 4000000)
	MDRV_SOUND_CONFIG(ym3812_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(UPD7759, UPD7759_STANDARD_CLOCK)
	MDRV_SOUND_CONFIG(upd7759_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.90)
MACHINE_DRIVER_END

/******************************************************************************/

ROM_START( prehisle )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "gt.2", 0x00000, 0x20000, CRC(7083245a) SHA1(c4f72440e3fb130c8c44224c958bf70c61e8c34e) )
	ROM_LOAD16_BYTE( "gt.3", 0x00001, 0x20000, CRC(6d8cdf58) SHA1(0078e54db899132d2b1244aed0b974173717f82e) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gt.1",  0x000000, 0x10000, CRC(80a4c093) SHA1(abe59e43259eb80b504bd5541f58cd0e5eb998ab) )

	ROM_REGION( 0x008000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "gt15.b15",   0x000000, 0x08000, CRC(ac652412) SHA1(916c04c3a8a7bfb961313ab73c0a27d7f5e48de1) )

	ROM_REGION( 0x040000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8914.b14", 0x000000, 0x40000, CRC(207d6187) SHA1(505dfd1424b894e7b898f91b89f021ddde433c48) )

	ROM_REGION( 0x040000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8916.h16", 0x000000, 0x40000, CRC(7cffe0f6) SHA1(aba08617964fc425418b098be5167021768bd47c) )

	ROM_REGION( 0x0a0000, REGION_GFX4, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8910.k14", 0x000000, 0x80000, CRC(5a101b0b) SHA1(9645ab1f8d058cf2c6c42ccb4ce92a9b5db10c51) )
	ROM_LOAD( "gt.5",       0x080000, 0x20000, CRC(3d3ab273) SHA1(b5706ada9eb2c22fcc0ac8ede2d2ee02ee853191) )

	ROM_REGION( 0x10000, REGION_GFX5, 0 )	/* background tilemaps */
	ROM_LOAD( "gt.11",  0x000000, 0x10000, CRC(b4f0fcf0) SHA1(b81cc0b6e3e6f5616789bb3e77807dc0ef718a38) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "gt.4",  0x000000, 0x20000, CRC(85dfb9ec) SHA1(78c865e7ccffddb71dcddccab358fa945f521f25) )
ROM_END

ROM_START( prehislu )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "gt-u2.2h", 0x00000, 0x20000, CRC(a14f49bb) SHA1(6b39a894c3d3862be349a58c748d2d763d5a269c) )
	ROM_LOAD16_BYTE( "gt-u3.3h", 0x00001, 0x20000, CRC(f165757e) SHA1(26cf369fed1713deec182852d76fe014ed46d6ac) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gt.1",  0x000000, 0x10000, CRC(80a4c093) SHA1(abe59e43259eb80b504bd5541f58cd0e5eb998ab) )

	ROM_REGION( 0x008000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "gt15.b15",   0x000000, 0x08000, CRC(ac652412) SHA1(916c04c3a8a7bfb961313ab73c0a27d7f5e48de1) )

	ROM_REGION( 0x040000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8914.b14", 0x000000, 0x40000, CRC(207d6187) SHA1(505dfd1424b894e7b898f91b89f021ddde433c48) )

	ROM_REGION( 0x040000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8916.h16", 0x000000, 0x40000, CRC(7cffe0f6) SHA1(aba08617964fc425418b098be5167021768bd47c) )

	ROM_REGION( 0x0a0000, REGION_GFX4, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8910.k14", 0x000000, 0x80000, CRC(5a101b0b) SHA1(9645ab1f8d058cf2c6c42ccb4ce92a9b5db10c51) )
	ROM_LOAD( "gt.5",       0x080000, 0x20000, CRC(3d3ab273) SHA1(b5706ada9eb2c22fcc0ac8ede2d2ee02ee853191) )

	ROM_REGION( 0x10000, REGION_GFX5, 0 )	/* background tilemaps */
	ROM_LOAD( "gt.11",  0x000000, 0x10000, CRC(b4f0fcf0) SHA1(b81cc0b6e3e6f5616789bb3e77807dc0ef718a38) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "gt.4",  0x000000, 0x20000, CRC(85dfb9ec) SHA1(78c865e7ccffddb71dcddccab358fa945f521f25) )
ROM_END

ROM_START( gensitou )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "gt2j.bin", 0x00000, 0x20000, CRC(a2da0b6b) SHA1(d102118f83b96094fd4ea4b3468713c4946c949d) )
	ROM_LOAD16_BYTE( "gt3j.bin", 0x00001, 0x20000, CRC(c1a0ae8e) SHA1(2c9643abfd71edf8612e63d69cea4fbc19aad19d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gt.1",  0x000000, 0x10000, CRC(80a4c093) SHA1(abe59e43259eb80b504bd5541f58cd0e5eb998ab) )

	ROM_REGION( 0x008000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "gt15.b15",   0x000000, 0x08000, CRC(ac652412) SHA1(916c04c3a8a7bfb961313ab73c0a27d7f5e48de1) )

	ROM_REGION( 0x040000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8914.b14", 0x000000, 0x40000, CRC(207d6187) SHA1(505dfd1424b894e7b898f91b89f021ddde433c48) )

	ROM_REGION( 0x040000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8916.h16", 0x000000, 0x40000, CRC(7cffe0f6) SHA1(aba08617964fc425418b098be5167021768bd47c) )

	ROM_REGION( 0x0a0000, REGION_GFX4, ROMREGION_DISPOSE )
	ROM_LOAD( "pi8910.k14", 0x000000, 0x80000, CRC(5a101b0b) SHA1(9645ab1f8d058cf2c6c42ccb4ce92a9b5db10c51) )
	ROM_LOAD( "gt.5",       0x080000, 0x20000, CRC(3d3ab273) SHA1(b5706ada9eb2c22fcc0ac8ede2d2ee02ee853191) )

	ROM_REGION( 0x10000, REGION_GFX5, 0 )	/* background tilemaps */
	ROM_LOAD( "gt.11",  0x000000, 0x10000, CRC(b4f0fcf0) SHA1(b81cc0b6e3e6f5616789bb3e77807dc0ef718a38) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "gt.4",  0x000000, 0x20000, CRC(85dfb9ec) SHA1(78c865e7ccffddb71dcddccab358fa945f521f25) )
ROM_END

/******************************************************************************/

static READ16_HANDLER( world_cycle_r )
{
	int pc=activecpu_get_pc();
	int ret=prehisle_ram16[0x12];

	if ((ret&0x8000) && (pc==0x260c || pc==0x268a || pc==0x2b0a || pc==0x34a8 || pc==0x6ae4 || pc==0x83ac || pc==0x25ce || pc==0x29c4)) {
		cpu_spinuntil_int();
		return ret&0x7fff;
	}
	return ret;
}

static DRIVER_INIT( prehisle )
{
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x70024, 0x70025, 0, 0, world_cycle_r);
}

static READ16_HANDLER( usa_cycle_r )
{
	int pc=activecpu_get_pc();
	int ret=prehisle_ram16[0x12];

	if ((ret&0x8000) && (pc==0x281e || pc==0x28a6 || pc==0x295a || pc==0x2868 || pc==0x8f98 || pc==0x3b1e)) {
		cpu_spinuntil_int();
		return ret&0x7fff;
	}
	return ret;
}

static DRIVER_INIT( prehislu )
{
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x70024, 0x70025, 0, 0, usa_cycle_r);
}

static READ16_HANDLER( jap_cycle_r )
{
	int pc=activecpu_get_pc();
	int ret=prehisle_ram16[0x12];

	if ((ret&0x8000) && (pc==0x34b6 /* Todo! */ )) {
		cpu_spinuntil_int();
		return ret&0x7fff;
	}
	return ret;
}

static DRIVER_INIT( gensitou )
{
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x70024, 0x70025, 0, 0, jap_cycle_r);
}

/******************************************************************************/

GAME( 1989, prehisle, 0,		prehisle, prehisle, prehisle, ROT0, "SNK", "Prehistoric Isle in 1930 (World)", 0 ,0)
GAME( 1989, prehislu, prehisle, prehisle, prehisle, prehislu, ROT0, "SNK of America", "Prehistoric Isle in 1930 (US)", 0 ,0)
GAME( 1989, gensitou, prehisle, prehisle, prehisle, gensitou, ROT0, "SNK", "Genshi-Tou 1930's", 0 ,0)
