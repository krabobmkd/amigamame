/*****************************************************************************

Kikiippatsu Mayumi-chan (c) 1988 Victory L.L.C.

    Driver by Uki

*****************************************************************************/

#include "driver.h"
#include "sound/2203intf.h"

#define MCLK 10000000

VIDEO_START( mayumi );
VIDEO_UPDATE( mayumi );

WRITE8_HANDLER( mayumi_videoram_w );
READ8_HANDLER( mayumi_videoram_r );

static int int_enable;
static int input_sel;

/****************************************************************************/

static INTERRUPT_GEN( mayumi_interrupt )
{
	if (int_enable)
		 cpunum_set_input_line(0, 0, HOLD_LINE);
}

static WRITE8_HANDLER( bank_sel_w )
{
	UINT8 *BANKROM = memory_region(REGION_CPU1);
	int bank = ((data & 0x80)) >> 7 | ((data & 0x40) >> 5);
	memory_set_bankptr(1, &BANKROM[0x10000+bank*0x4000]);

	int_enable = data & 1;

	flip_screen_set(data & 2);
}

static MACHINE_RESET( mayumi )
{
	bank_sel_w(0,0);
}

static WRITE8_HANDLER( input_sel_w )
{
	input_sel = data;
}

static READ8_HANDLER( key_matrix_r )
{
	int p,i,ret;

	ret = 0xff;

	p = ~input_sel & 0x1f;

	for (i=0; i<5; i++)
	{
		if (p & (1 << i))
			ret &= readinputport(i+7-offset*5);
	}

	return ret;
}

/****************************************************************************/

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1)
	AM_RANGE(0xc000, 0xdfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xf7ff) AM_READ(mayumi_videoram_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xdfff) AM_WRITE(MWA8_RAM) AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xe000, 0xf7ff) AM_WRITE(mayumi_videoram_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x30, 0x30) AM_READ(input_port_12_r)
	AM_RANGE(0xc1, 0xc2) AM_READ(key_matrix_r)	// 0xc0-c3 8255ppi
	AM_RANGE(0xd1, 0xd1) AM_READ(YM2203_read_port_0_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x30, 0x30) AM_WRITE(bank_sel_w)
	AM_RANGE(0xc0, 0xc0) AM_WRITE(input_sel_w)
	AM_RANGE(0xc3, 0xc3) AM_WRITE(MWA8_NOP)		// 0xc0-c3 8255ppi
	AM_RANGE(0xd0, 0xd0) AM_WRITE(YM2203_control_port_0_w)
	AM_RANGE(0xd1, 0xd1) AM_WRITE(YM2203_write_port_0_w)
ADDRESS_MAP_END

/****************************************************************************/

INPUT_PORTS_START( mayumi )

    PORT_START  /* dsw1 */
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Unknown 1-2" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Unknown 1-3" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x04, 0x04, "Service Mode (DSW 1-6)" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Unknown 1-7" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Unknown 1-8" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START  /* dsw2 */
	PORT_DIPNAME( 0x80, 0x80, "Unknown 2-1" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Unknown 2-2" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Unknown 2-3" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Unknown 2-4" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Unknown 2-5" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "Unknown 2-6" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Unknown 2-7" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Unknown 2-8" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* P1 IN0 (2) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_A )
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_E )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_I )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_M )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_KAN )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P1 IN1 (3) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_B )
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_F )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_J )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_N )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P1 IN2 (4) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_C )
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_G )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_K )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_RON )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P1 IN3 (5) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_D )
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_H )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_PON )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P1 IN4 (6) */
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P2 IN0 (7) */ \
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_A ) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_E ) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_I ) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_M ) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_KAN ) PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P2 IN1 (8) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_B ) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_F ) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_J ) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_N ) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH ) PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P2 IN2 (9) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_C ) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_G ) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_K ) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI ) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_RON ) PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P2 IN3 (10) */
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_MAHJONG_D ) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_MAHJONG_H ) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L ) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_PON ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* P2 IN4 (11) */
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START // 12
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
    PORT_BIT( 0x40, IP_ACTIVE_LOW , IPT_COIN1 )
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_SERVICE( 0x10, IP_ACTIVE_HIGH )
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) // analyzer
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE4 ) // memory reset

INPUT_PORTS_END

/****************************************************************************/

static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	8192,   /* 8192 characters */
	3,      /* 3 bits per pixel */
	{0x20000*8,0x10000*8,0},
	{STEP8(0,1)},
	{STEP8(0,8)},
	8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x00000, &charlayout, 0, 32 },
	{ -1 } /* end of array */
};

static struct YM2203interface ym2203_interface =
{
	input_port_0_r,
	input_port_1_r
};

static MACHINE_DRIVER_START( mayumi )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, MCLK/2) /* 5.000 MHz ? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_CPU_VBLANK_INT(mayumi_interrupt,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_MACHINE_RESET( mayumi )

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_PIXEL_ASPECT_RATIO_1_2)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(2*8, 62*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(mayumi)
	MDRV_VIDEO_UPDATE(mayumi)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, MCLK/4)
	MDRV_SOUND_CONFIG(ym2203_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.15)
	MDRV_SOUND_ROUTE(1, "mono", 0.15)
	MDRV_SOUND_ROUTE(2, "mono", 0.15)
	MDRV_SOUND_ROUTE(3, "mono", 0.40)

	MDRV_NVRAM_HANDLER(generic_0fill)

MACHINE_DRIVER_END

/****************************************************************************/

ROM_START( mayumi )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* CPU */
	ROM_LOAD( "my00.bin",  0x00000, 0x08000, CRC(33189e37) SHA1(cbf75f56360ef7da5b7b1207b58cd0d72bcaf207) )
	ROM_LOAD( "my01.bin",  0x10000, 0x10000, CRC(5280fb39) SHA1(cee7653f4353031701ec1608881b37073b178d9f) ) // Banked
	ROM_COPY( REGION_CPU1, 0x10000, 0x08000, 0x4000 )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE ) /* gfx */
	ROM_LOAD( "my10.bin", 0x00000, 0x10000, CRC(3b4f4f97) SHA1(50bda1484e965f15630bd2e05861d74ddeb0d88e) )
	ROM_LOAD( "my20.bin", 0x10000, 0x10000, CRC(18544029) SHA1(74bd8bb422db33bd7af08afbf9b801bd31a3f199) )
	ROM_LOAD( "my30.bin", 0x20000, 0x10000, CRC(7f22d53f) SHA1(f8e5874ba0fa003ba0d6a504b2169acdf1491484) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 ) /* color PROMs */
	ROM_LOAD( "my-9m.bin", 0x0000,  0x0100, CRC(b18fd669) SHA1(e2b1477c1bc49994b0b652d63a2205363aab9a74) ) // R
	ROM_LOAD( "my-9l.bin", 0x0100,  0x0100, CRC(f3fef561) SHA1(247f579fe91ad7e516c93a873b2ecca780bf6da0) ) // G
	ROM_LOAD( "my-9k.bin", 0x0200,  0x0100, CRC(3e7a8012) SHA1(24129586a1c39f68dad274b5afbdd6c027ab0901) ) // B
ROM_END

GAME( 1988, mayumi, 0, mayumi, mayumi, 0, ROT0, "[Sanritsu] Victory L.L.C.",  "Kikiippatsu Mayumi-chan (Japan)", 0 ,1)
