/*

Dr. Tomy    -   (c) 1993 Playmark

A rip-off of Dr. Mario by Playmark, using some code from Gaelco's Big Karnak on
similar hardware.

*/

#include "driver.h"
#include "sound/okim6295.h"

static UINT16 *drtomy_spriteram;
static UINT16 *drtomy_videoram_bg, *drtomy_videoram_fg;
static tilemap *tilemap_bg, *tilemap_fg;

static void get_tile_info_fg(int tile_index)
{
	int code  = drtomy_videoram_fg[tile_index] & 0xfff;
	int color = (drtomy_videoram_fg[tile_index] & 0xf000) >> 12;
	SET_TILE_INFO(2, code, color, 0)
}


static void get_tile_info_bg(int tile_index)
{
	int code  = drtomy_videoram_bg[tile_index] & 0xfff;
	int color = (drtomy_videoram_bg[tile_index] & 0xf000) >> 12;
	SET_TILE_INFO(1, code, color, 0)
}


/*
    Sprite Format (almost like gaelco.c)
    -------------

    Word | Bit(s)            | Description
    -----+-FEDCBA98-76543210-+--------------------------
      0  | -------- xxxxxxxx | y position
      0  | ----x--- -------- | sprite size
      0  | -x------ -------- | flipx
      0  | x------- -------- | flipy
      1  | xxxxxxxx xxxxxxxx | not used
      2  | -------x xxxxxxxx | x position
      2  | ---xxxx- -------- | sprite color
      3  | -------- ------xx | sprite code (8x8 cuadrant)
      3  | xxxxxxxx xxxxxx-- | sprite code
*/

static void drtomy_draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int i, x, y, ex, ey;
	const gfx_element *gfx = Machine->gfx[0];

	static int x_offset[2] = {0x0,0x2};
	static int y_offset[2] = {0x0,0x1};

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		gfx, 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 3; i < 0x1000/2; i+=4){
		int sx = drtomy_spriteram[i+2] & 0x01ff;
		int sy = (240 - (drtomy_spriteram[i] & 0x00ff)) & 0x00ff;
		int number = drtomy_spriteram[i+3];
		int color = (drtomy_spriteram[i+2] & 0x1e00) >> 9;
		int attr = (drtomy_spriteram[i] & 0xfe00) >> 9;

		int xflip = attr & 0x20;
		int yflip = attr & 0x40;
		int spr_size;

		if (attr & 0x04){
			spr_size = 1;
		}
		else{
			spr_size = 2;
			number &= (~3);
		}

		for (y = 0; y < spr_size; y++){
			for (x = 0; x < spr_size; x++){

				ex = xflip ? (spr_size-1-x) : x;
				ey = yflip ? (spr_size-1-y) : y;

				
				dgp0.code = number + x_offset[ex] + y_offset[ey];
				dgp0.color = color;
				dgp0.flipx = xflip;
				dgp0.flipy = yflip;
				dgp0.sx = sx-0x09+x*8;
				dgp0.sy = sy+y*8;
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph

}

VIDEO_START( drtomy )
{
	tilemap_bg = tilemap_create(get_tile_info_bg,tilemap_scan_rows,TILEMAP_OPAQUE,      16,16,32,32);
	tilemap_fg = tilemap_create(get_tile_info_fg,tilemap_scan_rows,TILEMAP_TRANSPARENT, 16,16,32,32);

	if (!tilemap_bg || !tilemap_fg)
		return 1;

	tilemap_set_transparent_pen(tilemap_fg,0);

	return 0;
}

VIDEO_UPDATE( drtomy )
{
	tilemap_draw(bitmap,cliprect,tilemap_bg,0,0);
	tilemap_draw(bitmap,cliprect,tilemap_fg,0,0);
	drtomy_draw_sprites(bitmap,cliprect);
}

WRITE16_HANDLER( drtomy_vram_fg_w )
{
	COMBINE_DATA(&drtomy_videoram_fg[offset]);
	tilemap_mark_tile_dirty(tilemap_fg,offset);
}

WRITE16_HANDLER( drtomy_vram_bg_w )
{
	COMBINE_DATA(&drtomy_videoram_bg[offset]);
	tilemap_mark_tile_dirty(tilemap_bg,offset);
}

WRITE16_HANDLER( drtomy_okibank_w )
{
	static int oki_bank;

	if(oki_bank != (data & 3))
	{
		oki_bank = data & 3;
		OKIM6295_set_bank_base(0, oki_bank * 0x40000);
	}

	/* unknown bit 2 -> (data & 4) */
}

static ADDRESS_MAP_START( drtomy_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM	/* ROM */
	AM_RANGE(0x100000, 0x100fff) AM_RAM AM_WRITE(drtomy_vram_fg_w) AM_BASE(&drtomy_videoram_fg)	/* Video RAM FG */
	AM_RANGE(0x101000, 0x101fff) AM_RAM AM_WRITE(drtomy_vram_bg_w) AM_BASE(&drtomy_videoram_bg) /* Video RAM BG */
	AM_RANGE(0x200000, 0x2007ff) AM_READWRITE(MRA16_RAM, paletteram16_xRRRRRGGGGGBBBBB_word_w) AM_BASE(&paletteram16) /* Palette */
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&drtomy_spriteram) /* Sprite RAM */
	AM_RANGE(0x700000, 0x700001) AM_READ(input_port_0_word_r) /* DIPSW #1 */
	AM_RANGE(0x700002, 0x700003) AM_READ(input_port_1_word_r) /* DIPSW #2 */
	AM_RANGE(0x700004, 0x700005) AM_READ(input_port_2_word_r) /* INPUT #1 */
	AM_RANGE(0x700006, 0x700007) AM_READ(input_port_3_word_r) /* INPUT #2 */
	AM_RANGE(0x70000c, 0x70000d) AM_WRITE(drtomy_okibank_w) /* OKI banking */
	AM_RANGE(0x70000e, 0x70000f) AM_READWRITE(OKIM6295_status_0_lsb_r, OKIM6295_data_0_lsb_w) /* OKI 6295*/
	AM_RANGE(0xffc000, 0xffffff) AM_RAM	/* Work RAM */
ADDRESS_MAP_END

static const gfx_layout tilelayout8=
{
	8,8,									/* 8x8 tiles */
	RGN_FRAC(1,4),							/* number of tiles */
	4,										/* bitplanes */
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) }, /* plane offsets */
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};

static const gfx_layout tilelayout16 =
{
	16,16,									/* 16x16 tiles */
	RGN_FRAC(1,4),							/* number of tiles */
	4,										/* bitplanes */
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) }, /* plane offsets */
	{ 0,1,2,3,4,5,6,7, 16*8+0,16*8+1,16*8+2,16*8+3,16*8+4,16*8+5,16*8+6,16*8+7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8, 8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	32*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tilelayout8,  0x100, 16 }, /* Sprites */
	{ REGION_GFX1, 0, &tilelayout16, 0x000, 16 }, /* BG */
	{ REGION_GFX1, 0, &tilelayout16, 0x200, 16 }, /* FG */
	{ -1 }
};


INPUT_PORTS_START( drtomy )
	PORT_START	/* DSW #1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x00, "5 Coins/4 Credits" )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x00, "5 Coins/4 Credits" )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )

	PORT_START	/* DSW #2 */
	PORT_DIPNAME( 0x01, 0x01, "Time" )
	PORT_DIPSETTING(    0x00, "Less" )
	PORT_DIPSETTING(    0x01, "More" )
	PORT_DIPNAME( 0x02, 0x02, "Number of Virus" )
	PORT_DIPSETTING(    0x02, "Less" )
	PORT_DIPSETTING(    0x00, "More" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x08, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x20, DEF_STR( English ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Italian ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x40, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* 1P INPUTS & COINSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START	/* 2P INPUTS & STARTSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END


static MACHINE_DRIVER_START( drtomy )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,24000000/2)			/* ? MHz */
	MDRV_CPU_PROGRAM_MAP(drtomy_map,0)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*16, 32*16)
	MDRV_VISIBLE_AREA(0, 320-1, 16, 256-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(drtomy)
	MDRV_VIDEO_UPDATE(drtomy)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 26000000/16/165)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.8)
MACHINE_DRIVER_END


ROM_START( drtomy )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "15.u21", 0x00001, 0x20000, CRC(0b8d763b) SHA1(082005985a2de7b941ea227bbf6e761a197132e6) )
	ROM_LOAD16_BYTE( "16.u22", 0x00000, 0x20000, CRC(206f4d65) SHA1(f4a28bc6041981d50a03477e63e90d5ff8ffb765) )

	ROM_REGION( 0x400000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "20.u80",	0x000000, 0x40000, CRC(4d4d86ff) SHA1(60df0bf8ba62fea42ff756cd7c5485b57f597098) )
	ROM_LOAD( "19.u81",	0x100000, 0x40000, CRC(49ecbfe2) SHA1(16889663bdd3b7d0a350d5b18e221480413f6b4f) )
	ROM_LOAD( "18.u82", 0x200000, 0x40000, CRC(8ee5c921) SHA1(6ba43eeb3b633c3db22f7b18b8fe91f250da2242) )
	ROM_LOAD( "17.u83",	0x300000, 0x40000, CRC(42044b1c) SHA1(6fd01911932e0fb800ffefec595a9e7c524faa8f) )

	ROM_REGION( 0x80000, REGION_USER1, 0 ) /* OKIM6295 samples */
	ROM_LOAD( "14.u23",	0x00000, 0x80000, CRC(479614ec) SHA1(b6300b344422f0a64146b853411f5285eaaada28) )

	ROM_REGION( 0x100000, REGION_SOUND1, 0 )
	ROM_COPY( REGION_USER1, 0x00000, 0x00000, 0x20000)
	ROM_COPY( REGION_USER1, 0x00000, 0x20000, 0x20000)
	ROM_COPY( REGION_USER1, 0x00000, 0x40000, 0x20000)
	ROM_COPY( REGION_USER1, 0x20000, 0x60000, 0x20000)
	ROM_COPY( REGION_USER1, 0x00000, 0x80000, 0x20000)
	ROM_COPY( REGION_USER1, 0x40000, 0xa0000, 0x20000)
	ROM_COPY( REGION_USER1, 0x00000, 0xc0000, 0x20000)
	ROM_COPY( REGION_USER1, 0x60000, 0xe0000, 0x20000)
ROM_END


GAME( 1993, drtomy, 0, drtomy, drtomy, 0, ROT0, "Playmark", "Dr. Tomy", 0 ,0)
