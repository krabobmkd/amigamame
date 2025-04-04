/***************************************************************************

Syusse Oozumou
(c) 1984 Technos Japan (Licensed by Data East)

Driver by Takahiro Nogi (nogi@kt.rim.or.jp) 1999/10/04

***************************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "cpu/m6809/m6809.h"
#include "sound/ay8910.h"
#include "sound/dac.h"

extern UINT8 *ssozumo_videoram2;
extern UINT8 *ssozumo_colorram2;

extern WRITE8_HANDLER( ssozumo_videoram_w );
extern WRITE8_HANDLER( ssozumo_colorram_w );
extern WRITE8_HANDLER( ssozumo_videoram2_w );
extern WRITE8_HANDLER( ssozumo_colorram2_w );
extern WRITE8_HANDLER( ssozumo_paletteram_w );
extern WRITE8_HANDLER( ssozumo_scroll_w );
extern WRITE8_HANDLER( ssozumo_flipscreen_w );

extern PALETTE_INIT( ssozumo );
extern VIDEO_START( ssozumo );
extern VIDEO_UPDATE( ssozumo );


static INTERRUPT_GEN( ssozumo_interrupt )
{
	static int coin;

	if ((readinputport(0) & 0xc0) != 0xc0)
	{
		if (coin == 0)
		{
			coin = 1;
			nmi_line_pulse();
			return;
		}
	}
	else coin = 0;

	irq0_line_hold();
}


WRITE8_HANDLER( ssozumo_sh_command_w )
{
	soundlatch_w(offset, data);
	cpunum_set_input_line(1, M6502_IRQ_LINE, HOLD_LINE);
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x077f) AM_READ(MRA8_RAM)

	AM_RANGE(0x2000, 0x27ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x31ff) AM_READ(MRA8_RAM)

	AM_RANGE(0x4000, 0x4000) AM_READ(input_port_0_r)
	AM_RANGE(0x4010, 0x4010) AM_READ(input_port_1_r)
	AM_RANGE(0x4020, 0x4020) AM_READ(input_port_2_r)
	AM_RANGE(0x4030, 0x4030) AM_READ(input_port_3_r)

	AM_RANGE(0x6000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x077f) AM_WRITE(MWA8_RAM)

	AM_RANGE(0x0780, 0x07ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x2000, 0x23ff) AM_WRITE(ssozumo_videoram2_w) AM_BASE(&ssozumo_videoram2)
	AM_RANGE(0x2400, 0x27ff) AM_WRITE(ssozumo_colorram2_w) AM_BASE(&ssozumo_colorram2)
	AM_RANGE(0x3000, 0x31ff) AM_WRITE(ssozumo_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x3200, 0x33ff) AM_WRITE(ssozumo_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x3400, 0x35ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x3600, 0x37ff) AM_WRITE(MWA8_RAM)

	AM_RANGE(0x4000, 0x4000) AM_WRITE(ssozumo_flipscreen_w)
	AM_RANGE(0x4010, 0x4010) AM_WRITE(ssozumo_sh_command_w)
	AM_RANGE(0x4020, 0x4020) AM_WRITE(ssozumo_scroll_w)
//  AM_RANGE(0x4030, 0x4030) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x4050, 0x407f) AM_WRITE(ssozumo_paletteram_w) AM_BASE(&paletteram)

	AM_RANGE(0x6000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x01ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2007, 0x2007) AM_READ(soundlatch_r)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x01ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x2001, 0x2001) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x2002, 0x2002) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x2003, 0x2003) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x2004, 0x2004) AM_WRITE(DAC_0_signed_data_w)
	AM_RANGE(0x2005, 0x2005) AM_WRITE(interrupt_enable_w)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END


INPUT_PORTS_START( ssozumo )
	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START      /* DSW2 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START      /* DSW1 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Controls ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Dual ) )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,		/* 8*8 characters */
	1024,		/* 1024 characters */
	3,		/* 3 bits per pixel */
	{ 2*1024*8*8, 1024*8*8, 0 },	/* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8		/* every char takes 8 consecutive bytes */
};


static const gfx_layout tilelayout =
{
	16,16,	/* 16*16 tiles */
	256,	/* 256 tiles */
	3,		/* 3 bits per pixel */
	{ 2*256*16*16, 256*16*16, 0 },	/* the bitplanes are separated */
	{ 16*8 + 0, 16*8 + 1, 16*8 + 2, 16*8 + 3, 16*8 + 4, 16*8 + 5, 16*8 + 6, 16*8 + 7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8		/* every tile takes 16 consecutive bytes */
};


static const gfx_layout spritelayout =
{
	16,16,		/* 16*16 sprites */
	1280,		/* 1280 sprites */
	3,		/* 3 bits per pixel */
	{ 2*1280*16*16, 1280*16*16, 0 },	/* the bitplanes are separated */
	{ 16*8 + 0, 16*8 + 1, 16*8 + 2, 16*8 + 3, 16*8 + 4, 16*8 + 5, 16*8 + 6, 16*8 + 7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8		/* every sprite takes 16 consecutive bytes */
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,     0, 4 },
	{ REGION_GFX2, 0, &tilelayout,   4*8, 4 },
	{ REGION_GFX3, 0, &spritelayout, 8*8, 2 },
	{ -1 }		/* end of array */
};


static MACHINE_DRIVER_START( ssozumo )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1200000)	/* 1.2 MHz ???? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(ssozumo_interrupt,1)

	MDRV_CPU_ADD(M6502, 975000)
	/* audio CPU */ 		/* 975 kHz ?? */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,16)	/* IRQs are triggered by the main CPU */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8 - 1, 1*8, 31*8 - 1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64 + 16)
	MDRV_COLORTABLE_LENGTH(64 + 16)

	MDRV_PALETTE_INIT(ssozumo)
	MDRV_VIDEO_START(ssozumo)
	MDRV_VIDEO_UPDATE(ssozumo)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_DRIVER_END



ROM_START( ssozumo )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	/* Main Program ROMs */
	ROM_LOAD( "ic61.g01",	0x06000, 0x2000, CRC(86968f46) SHA1(6acd111b71fbb4ef00ae03be4fb93d305a6564e7) )	// m1
	ROM_LOAD( "ic60.g11",	0x08000, 0x2000, CRC(1a5143dd) SHA1(19e36afcd0827f14f4360b55d952cc1af38327fd) )	// m2
	ROM_LOAD( "ic59.g21",	0x0a000, 0x2000, CRC(d3df04d7) SHA1(a95cff7f67ad2a3dbf7147018889a0de3f9fcbac) )	// m3
	ROM_LOAD( "ic58.g31",	0x0c000, 0x2000, CRC(0ee43a78) SHA1(383a29a2dfdbd600dacf3885039759efab718a45) )	// m4
	ROM_LOAD( "ic57.g41",	0x0e000, 0x2000, CRC(ac77aa4c) SHA1(36ee826327e4433bcdcb8d770fc6176f53d3eed0) )	// m5

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	/* Sound Program & Voice Sample ROMs*/
	ROM_LOAD( "ic47.g50",	0x04000, 0x2000, CRC(b64ec829) SHA1(684f1c37c05fc3812f11e040fb96789c8abb987f) )	// a1
	ROM_LOAD( "ic46.g60",	0x06000, 0x2000, CRC(630d7380) SHA1(aab3f034417a9712c8fa922946eda02751c9e319) )	// a2
	ROM_LOAD( "ic45.g70",	0x08000, 0x2000, CRC(1854b657) SHA1(c4f3c24a2b03bdf4d9fd80d6df944a157f98e617) )	// a3
	ROM_LOAD( "ic44.g80",	0x0a000, 0x2000, CRC(40b9a0da) SHA1(ef51977d23e14fb638b26afcb2617933446d8143) )	// a4
	ROM_LOAD( "ic43.g90",	0x0c000, 0x2000, CRC(20262064) SHA1(2845efa458f4fd873b8559489bcee4b9d8e437c1) )	// a5
	ROM_LOAD( "ic42.ga0",	0x0e000, 0x2000, CRC(98d7e998) SHA1(16bb3315db7d52531a3297e1255478aa1ebc32c2) )	// a6

	ROM_REGION( 0x06000, REGION_GFX1, ROMREGION_DISPOSE )
	/* Character ROMs */
	ROM_LOAD( "ic22.gq0",	0x00000, 0x2000, CRC(b4c7e612) SHA1(2d4f6f79b65aa27e00f173777959ec07e81ff15e) )	// c1
	ROM_LOAD( "ic23.gr0",	0x02000, 0x2000, CRC(90bb9fda) SHA1(9c065a54330133e5afadcb2ae29add5e1005d977) )	// c2
	ROM_LOAD( "ic21.gs0",	0x04000, 0x2000, CRC(d8cd5c78) SHA1(f1567850db649d2b7a029a5f71bbade25bb0393f) )	// c3

	ROM_REGION( 0x06000, REGION_GFX2, ROMREGION_DISPOSE )
	/* tile set ROMs */
	ROM_LOAD( "ic69.gt0",	0x00000, 0x2000, CRC(771116ca) SHA1(2d1c656315f57e1a142725e2d2034543cb3917ea) )	// t1
	ROM_LOAD( "ic59.gu0",	0x02000, 0x2000, CRC(68035bfd) SHA1(da535ff6860f71c1780d4d9dfd1944e355234c5b) )	// t2
	ROM_LOAD( "ic81.gv0",	0x04000, 0x2000, CRC(cdda1f9f) SHA1(d1f1b3e0578fd991c74d4a85313c5d37f08f1eee) )	// t3

	ROM_REGION( 0x1e000, REGION_GFX3, ROMREGION_DISPOSE )
	/* sprites ROMs */
	ROM_LOAD( "ic06.gg0",	0x00000, 0x2000, CRC(d2342c50) SHA1(f502b716d659d9fd3119dbb454296fe9e280fa5d) )	// s1a
	ROM_LOAD( "ic05.gh0",	0x02000, 0x2000, CRC(14a3cb10) SHA1(7b6d63f43ebbe3c3aea7f2e04789cdb78cdd8495) )	// s1b
	ROM_LOAD( "ic04.gi0",	0x04000, 0x2000, CRC(169276c1) SHA1(7f0b54425e0f82f7fcc892d7b8e7719087060d2a) )	// s1c
	ROM_LOAD( "ic03.gj0",	0x06000, 0x2000, CRC(e71b9f28) SHA1(1f4f1a4d44fecb212778bb191e14bbfdc41556a5) )	// s1d
	ROM_LOAD( "ic02.gk0",	0x08000, 0x2000, CRC(6e94773c) SHA1(c3a1b950c1abce7103e6a0c19b5bc47a46612b05) )	// s1e
	ROM_LOAD( "ic29.gl0",	0x0a000, 0x2000, CRC(40f67cc4) SHA1(fb6cfa9c9665c719926fc6ef050682f040852840) )	// s2a
	ROM_LOAD( "ic28.gm0",	0x0c000, 0x2000, CRC(8c97b1a2) SHA1(72ca28959b532f98e0836a9650bb3dd3fdfa755a) )	// s2b
	ROM_LOAD( "ic27.gn0",	0x0e000, 0x2000, CRC(be8bb3dd) SHA1(d032591e73b09e2f076a18298d606edf16998a64) )	// s2c
	ROM_LOAD( "ic26.go0",	0x10000, 0x2000, CRC(9c098a2c) SHA1(d2093f1a4f4b3bf3bbff0adea5bd910993ed4704) )	// s2d
	ROM_LOAD( "ic25.gp0",	0x12000, 0x2000, CRC(f73f8a76) SHA1(13652779d3d30de0b4136eb3f43ee5429861bf35) )	// s2e
	ROM_LOAD( "ic44.gb0",	0x14000, 0x2000, CRC(cdd7f2eb) SHA1(57cf788804f9d2a1283032c25b608ac45064eddb) )	// s3a
	ROM_LOAD( "ic43.gc0",	0x16000, 0x2000, CRC(7b4c632e) SHA1(2acb0f2213928b97fdf239fbabc6d24329cbdd7a) )	// s3b
	ROM_LOAD( "ic42.gd0",	0x18000, 0x2000, CRC(cd1c8fe6) SHA1(ac085a0e8e228ea6bfbe86f209be08221bb066ee) )	// s3c
	ROM_LOAD( "ic41.ge0",	0x1a000, 0x2000, CRC(935578d0) SHA1(e9a9f439e0781627df076c454b16f5796ac991bc) )	// s3d
	ROM_LOAD( "ic40.gf0",	0x1c000, 0x2000, CRC(5a3bf1ba) SHA1(6beebb7ac9c8baa3bbb5b0ebf6a6da768e52d1d3) )	// s3e

	ROM_REGION( 0x0080, REGION_PROMS, 0 )
	ROM_LOAD( "ic33.gz0",	0x00000, 0x0020, CRC(523d29ad) SHA1(48d0ae83a07e4409a1def56772c5156e8d505749) )	/* char palette red and green components */
	ROM_LOAD( "ic30.gz2",	0x00020, 0x0020, CRC(0de202e1) SHA1(ca1aa66c1d3d4724d322ec0346860c37729ddaed) )	/* tile palette red and green components */
	ROM_LOAD( "ic32.gz1",	0x00040, 0x0020, CRC(6fbff4d2) SHA1(b2cd38fa8e9a74539b96d6e8e0375fff2dd77a20) )	/* char palette blue component */
	ROM_LOAD( "ic31.gz3",	0x00060, 0x0020, CRC(18e7fe63) SHA1(b0834b94b22ead765ddac5591ab1dc66ec20f17f) )	/* tile palette blue component */
ROM_END



GAME( 1984, ssozumo, 0, ssozumo, ssozumo, 0, ROT270, "Technos", "Syusse Oozumou (Japan)", 0 ,0)
