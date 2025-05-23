/*

Mighty Warrior
Elettronica Video-Games S.R.L, 19??

PCB Layout
----------

|----------------------------------------------|      |---------------------------|
|     M6295  OKI0      PAL          CN1        |      | SF2-0    SF2-1     SF4-1  |
|     M6295  OKI1      PAL     |------------|  |      |                           |
|         PAL                  |------------|  |      | DW-0     SF3-1     SF4-0  |
|                         6264                 |      |                           |
|      2018               6264                 |      |          DW-1      SF3-0  |
|J     2018         |-------|    PAL           |      |                           |
|A            PAL   |ACTEL  |                  |      | OBM-12   OBM-17    OBM-15 |
|M     |-------|    |A1020A |45MHz             |      |                           |
|M PAL |ACTEL  |    |PL84C  |    PAL           |      | OBM-6    OBM-11    OBM-9  |
|A     |A1020A |2018|-------|PAL               |      |                           |
|62256 |PL84C  |2018                           |      | OBM-0    OBM-5     OBM-3  |
|62256 |-------|                               |      |                           |
|DSW1  62256   62256                           |      | OBM-13   OBM-14    OBM-16 |
|DSW2  PRG_OD  PRG_EV                          |      |                           |
|  |------------|PAL               CN2         |      | OBM-7    OBM-8     OBM-10 |
|  |MC68000P10  |          2018|------------|  |      |                           |
|  |------------|   2018   2018|------------|  |      | OBM-1    OBM-2     OBM-4  |
|12MHz              2018        PAL            |      |                           |
|----------------------------------------------|      |---------------------------|
Notes:
      68000 clock : 12.000MHz
      M6295 clocks: 0.9375MHz (45/48). Sample Rate = 937500 / 132
      VSync       : 54Hz
      CN1/2       : Connector for plug-in ROM daughterboard. The board
                    contains only 26x 27C4001 EPROMs and 2x 74LS139 logic IC's.


 driver by Pierpaolo Prazzoli

*/

#include "driver.h"
#include "sound/okim6295.h"

static tilemap *bg_tilemap, *mlow_tilemap, *mhigh_tilemap, *tx_tilemap;
static UINT16 *bg_videoram, *mlow_videoram, *mhigh_videoram, *tx_videoram, *sprites_buffer;
static UINT16 *bg_scrollram, *mlow_scrollram, *mhigh_scrollram, *vidattrram;
static UINT16 *mwarr_ram;

static WRITE16_HANDLER( bg_videoram_w )
{
	int oldword = bg_videoram[offset];
	COMBINE_DATA(&bg_videoram[offset]);
	if (oldword != bg_videoram[offset])
		tilemap_mark_tile_dirty(bg_tilemap,offset);
}

static WRITE16_HANDLER( mlow_videoram_w )
{
	int oldword = mlow_videoram[offset];
	COMBINE_DATA(&mlow_videoram[offset]);
	if (oldword != mlow_videoram[offset])
		tilemap_mark_tile_dirty(mlow_tilemap,offset);
}

static WRITE16_HANDLER( mhigh_videoram_w )
{
	int oldword = mhigh_videoram[offset];
	COMBINE_DATA(&mhigh_videoram[offset]);
	if (oldword != mhigh_videoram[offset])
		tilemap_mark_tile_dirty(mhigh_tilemap,offset);
}

static WRITE16_HANDLER( tx_videoram_w )
{
	int oldword = tx_videoram[offset];
	COMBINE_DATA(&tx_videoram[offset]);
	if (oldword != tx_videoram[offset])
		tilemap_mark_tile_dirty(tx_tilemap,offset);
}

static WRITE16_HANDLER( oki1_bank_w )
{
	OKIM6295_set_bank_base(1, 0x40000 * (data & 3));
}

static WRITE16_HANDLER( sprites_commands_w )
{
	static int which = 0;

	if( which )
	{
		int i;

		switch(data)
		{
		case 0:
			/* clear sprites on screen */
			for( i = 0; i < 0x800; i++)
			{
				sprites_buffer[i] = 0;
			}
			which = 0;
			break;

		default:
			logerror("used unknown sprites command %02X\n",data);
		case 0xf:
			/* refresh sprites on screen */
			for( i = 0; i < 0x800; i++)
			{
				sprites_buffer[i] = spriteram16[i];
			}
			break;

		case 0xd:
			/* keep sprites on screen */
			break;
		}
	}

	which ^= 1;
}

static WRITE16_HANDLER( mwarr_brightness_w )
{
	int i;
	double brightness;

	COMBINE_DATA(&mwarr_ram[0x14/2]);

	brightness = (double)(data & 0xff);
	for (i=0;i<0x800;i++)
	{
		palette_set_brightness(i, brightness/255);
	}
}


static ADDRESS_MAP_START( mwarr_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_ROM
	AM_RANGE(0x100000, 0x1007ff) AM_RAM AM_WRITE(bg_videoram_w) AM_BASE(&bg_videoram)
	AM_RANGE(0x100800, 0x100fff) AM_RAM AM_WRITE(mlow_videoram_w) AM_BASE(&mlow_videoram)
	AM_RANGE(0x101000, 0x1017ff) AM_RAM AM_WRITE(mhigh_videoram_w) AM_BASE(&mhigh_videoram)
	AM_RANGE(0x101800, 0x1027ff) AM_RAM AM_WRITE(tx_videoram_w) AM_BASE(&tx_videoram)
	AM_RANGE(0x103000, 0x1033ff) AM_RAM AM_BASE(&bg_scrollram)
	AM_RANGE(0x103400, 0x1037ff) AM_RAM AM_BASE(&mlow_scrollram)
	AM_RANGE(0x103800, 0x103bff) AM_RAM AM_BASE(&mhigh_scrollram)
	AM_RANGE(0x103c00, 0x103fff) AM_RAM AM_BASE(&vidattrram)
	AM_RANGE(0x104000, 0x104fff) AM_RAM AM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x108000, 0x108fff) AM_RAM AM_BASE(&spriteram16)
	AM_RANGE(0x110000, 0x110001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x110002, 0x110003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x110004, 0x110005) AM_READ(input_port_2_word_r)
	AM_RANGE(0x110010, 0x110011) AM_WRITE(oki1_bank_w)
	AM_RANGE(0x110014, 0x110015) AM_WRITE(mwarr_brightness_w)
	AM_RANGE(0x110016, 0x110017) AM_WRITE(sprites_commands_w)
	AM_RANGE(0x110000, 0x11ffff) AM_RAM AM_BASE(&mwarr_ram)
	AM_RANGE(0x180000, 0x180001) AM_READWRITE(OKIM6295_status_0_lsb_r, OKIM6295_data_0_lsb_w)
	AM_RANGE(0x190000, 0x190001) AM_READWRITE(OKIM6295_status_1_lsb_r, OKIM6295_data_1_lsb_w)
ADDRESS_MAP_END

INPUT_PORTS_START( mwarr )
	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_SPECIAL ) // otherwise it doesn't boot
	PORT_BIT( 0xfff0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	  0x0003, DEF_STR( Very_Easy ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x0004, 0x0000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0000, "Mutant" )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, "Freeze" )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0700, 0x0700, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0500, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x3800, 0x3800, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x3800, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x3000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x2800, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x4000, 0x0000, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Yes ) )
	PORT_SERVICE_NO_TOGGLE( 0x8000, IP_ACTIVE_LOW )
INPUT_PORTS_END

static const gfx_layout mwarr_tile8_layout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ 0,1,2,3 },
	{ 4, 0, RGN_FRAC(1,2)+4, RGN_FRAC(1,2)+0, 12, 8, RGN_FRAC(1,2)+12, RGN_FRAC(1,2)+8 },
	{ 0*16,1*16,2*16,3*16,4*16,5*16,6*16,7*16 },
	8*16
};

static const gfx_layout mwarr_tile16_layout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ 0,1,2,3 },
	{ 4, 0,	RGN_FRAC(1,2)+4, RGN_FRAC(1,2)+0, 12, 8, RGN_FRAC(1,2)+12, RGN_FRAC(1,2)+8,
	  256+4, 256+0, 256+RGN_FRAC(1,2)+4, 256+RGN_FRAC(1,2)+0, 256+12, 256+8, 256+RGN_FRAC(1,2)+12, 256+RGN_FRAC(1,2)+8 },
	{ 0*16,1*16,2*16,3*16,4*16,5*16,6*16,7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16
};

static const gfx_layout mwarr_6bpp_sprites =
{
	16,16,
	RGN_FRAC(1,6),
	6,
	{ RGN_FRAC(5,6), RGN_FRAC(4,6), RGN_FRAC(3,6), RGN_FRAC(2,6), RGN_FRAC(1,6), RGN_FRAC(0,6) },
	{ 135,134,133,132,131,130,129,128,7,6,5,4,3,2,1,0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	32*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &mwarr_6bpp_sprites,  1024, 16 },
	{ REGION_GFX2, 0, &mwarr_tile8_layout,	 384,  8 },
	{ REGION_GFX3, 0, &mwarr_tile16_layout,  256,  8 },
	{ REGION_GFX4, 0, &mwarr_tile16_layout,  128,  8 },
	{ REGION_GFX5, 0, &mwarr_tile16_layout,    0,  8 },
	{ -1 } /* end of array */
};

static void get_bg_tile_info(int tile_index)
{
	int tileno,colour;

	tileno = bg_videoram[tile_index] & 0x1fff;
	colour = (bg_videoram[tile_index] & 0xe000) >> 13;

	SET_TILE_INFO(4,tileno,colour,0)
}

static void get_mlow_tile_info(int tile_index)
{
	int tileno,colour;

	tileno = mlow_videoram[tile_index] & 0x1fff;
	colour = (mlow_videoram[tile_index] & 0xe000) >> 13;

	SET_TILE_INFO(3,tileno,colour,0)
}

static void get_mhigh_tile_info(int tile_index)
{
	int tileno,colour;

	tileno = mhigh_videoram[tile_index] & 0x1fff;
	colour = (mhigh_videoram[tile_index] & 0xe000) >> 13;

	SET_TILE_INFO(2,tileno,colour,0)
}

static void get_tx_tile_info(int tile_index)
{
	int tileno,colour;

	tileno = tx_videoram[tile_index] & 0x1fff;
	colour = (tx_videoram[tile_index] & 0xe000) >> 13;

	SET_TILE_INFO(1,tileno,colour,0)
}

VIDEO_START( mwarr )
{
	bg_tilemap    = tilemap_create(get_bg_tile_info,   tilemap_scan_cols,TILEMAP_OPAQUE,      16, 16,64,16);
	mlow_tilemap  = tilemap_create(get_mlow_tile_info, tilemap_scan_cols,TILEMAP_TRANSPARENT, 16, 16,64,16);
	mhigh_tilemap = tilemap_create(get_mhigh_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT, 16, 16,64,16);
	tx_tilemap    = tilemap_create(get_tx_tile_info,   tilemap_scan_rows,TILEMAP_TRANSPARENT,  8,  8,64,32);

	sprites_buffer = auto_malloc(sizeof(UINT16) * 0x800);

	if(!bg_tilemap || !mlow_tilemap || !tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(mlow_tilemap,0);
	tilemap_set_transparent_pen(mhigh_tilemap,0);
	tilemap_set_transparent_pen(tx_tilemap,0);

	tilemap_set_scroll_rows(bg_tilemap, 256);
	tilemap_set_scroll_rows(mlow_tilemap, 256);
	tilemap_set_scroll_rows(mhigh_tilemap, 256);

	return 0;
}

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	const UINT16 *source = sprites_buffer+0x800-4;
	const UINT16 *finish = sprites_buffer;
	const gfx_element *gfx = Machine->gfx[0];
	int x, y, color, flipx, dy, pri, pri_mask, i;

	
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
		priority_bitmap, 	// pri_buffer
		pri_mask 	// priority_mask
	  };
	while( source>=finish )
	{
		/* draw sprite */
		if( source[0] & 0x0800 )
		{
			y = 512 - (source[0] & 0x01ff);
			x = (source[3] & 0x3ff) - 9;

			color = source[1] & 0x000f;
			flipx = source[1] & 0x0200;

			dy = (source[0] & 0xf000)>>12;

			pri		 =	((source[1] & 0x3c00)>>10);	// Priority (1 = Low)
			pri_mask =	~((1 << (pri+1)) - 1);		// Above the first "pri" levels

			for(i=0;i<=dy;i++)
			{
				
				dgp0.code = source[2]+i;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.sx = x;
				dgp0.sy = y+i*16;
				drawgfx(&dgp0);

				/* wrap around x */
				
				dgp0.code = source[2]+i;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.sx = x-1024;
				dgp0.sy = y+i*16;
				drawgfx(&dgp0);

				/* wrap around y */
				
				dgp0.code = source[2]+i;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.sx = x;
				dgp0.sy = y-512+i*16;
				drawgfx(&dgp0);

				/* wrap around x & y */
				
				dgp0.code = source[2]+i;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.sx = x-1024;
				dgp0.sy = y-512+i*16;
				drawgfx(&dgp0);
			}
		}

		source -= 0x4;
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( mwarr )
{
	int i;

	fillbitmap(priority_bitmap,0,cliprect);

	if(vidattrram[6] & 1)
	{
		for(i=0;i<256;i++)
			tilemap_set_scrollx(bg_tilemap, i, bg_scrollram[i]+20);
	}
	else
	{
		for(i=0;i<256;i++)
			tilemap_set_scrollx(bg_tilemap, i, bg_scrollram[0]+19);
	}

	if(vidattrram[6] & 4)
	{
		for(i=0;i<256;i++)
			tilemap_set_scrollx(mlow_tilemap, i, mlow_scrollram[i]+19);
	}
	else
	{
		for(i=0;i<256;i++)
			tilemap_set_scrollx(mlow_tilemap, i, mlow_scrollram[0]+19);
	}

	if(vidattrram[6] & 0x10)
	{
		for(i=0;i<256;i++)
			tilemap_set_scrollx(mhigh_tilemap, i, mhigh_scrollram[i]+19);
	}
	else
	{
		for(i=0;i<256;i++)
			tilemap_set_scrollx(mhigh_tilemap, i, mhigh_scrollram[0]+19);
	}

	tilemap_set_scrolly(bg_tilemap,    0, vidattrram[1]+1);
	tilemap_set_scrolly(mlow_tilemap,  0, vidattrram[2]+1);
	tilemap_set_scrolly(mhigh_tilemap, 0, vidattrram[3]+1);

	tilemap_set_scrollx(tx_tilemap, 0, vidattrram[0]+16);
	tilemap_set_scrolly(tx_tilemap, 0, vidattrram[4]+1);

	tilemap_draw(bitmap,cliprect,bg_tilemap,   0,0x01);
	tilemap_draw(bitmap,cliprect,mlow_tilemap, 0,0x02);
	tilemap_draw(bitmap,cliprect,mhigh_tilemap,0,0x04);
	tilemap_draw(bitmap,cliprect,tx_tilemap,   0,0x10);
	draw_sprites(bitmap,cliprect);
}

static MACHINE_DRIVER_START( mwarr )
	MDRV_CPU_ADD(M68000, 12000000)
	MDRV_CPU_PROGRAM_MAP(mwarr_map,0)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_FRAMES_PER_SECOND(54)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8+1, 48*8-1-8-1, 0, 30*8-1)
	MDRV_PALETTE_LENGTH(0x800)

	MDRV_VIDEO_START(mwarr)
	MDRV_VIDEO_UPDATE(mwarr)

	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 937500 / 132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(OKIM6295, 937500 / 132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


ROM_START( mwarr )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "prg_ev", 0x00000, 0x80000, CRC(d1d5e0a6) SHA1(f47955459d41c904b96de000b32cae156ee3bcba) )
	ROM_LOAD16_BYTE( "prg_od", 0x00001, 0x80000, CRC(e5217d91) SHA1(6a5d282e8e5b98628f98530e3c47b9b398e9334e) )

	ROM_REGION( 0x900000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "obm-0",  0x000000, 0x80000, CRC(b4707ba1) SHA1(35330a31e9837e5f848a21fa6f589412b35a04a0) )
	ROM_LOAD( "obm-6",  0x080000, 0x80000, CRC(f9675acc) SHA1(06e0c0c0928ace331ebd08cfeeaa2c8b5603457f) )
	ROM_LOAD( "obm-12", 0x100000, 0x80000, CRC(6239c4dd) SHA1(128040e9517151faf15c75dc1f2d79c5a66b9e1c) )
	ROM_LOAD( "obm-1",  0x180000, 0x80000, CRC(817dcead) SHA1(697b4b3e18e022e0635a3b02cbce1e4d2959a732) )
	ROM_LOAD( "obm-7",  0x200000, 0x80000, CRC(3a93c499) SHA1(9ecd72c5ef4f0edbdc19946bd33aa4e74690756d) )
	ROM_LOAD( "obm-13", 0x280000, 0x80000, CRC(bac42f06) SHA1(6998e605db732e6be9d8213e96bfb04a258eae8f) )
	ROM_LOAD( "obm-2",  0x300000, 0x80000, CRC(68cd29b0) SHA1(02f7bf463cd15eaf4713d33494f19c4fcd199e87) )
	ROM_LOAD( "obm-8",  0x380000, 0x80000, CRC(f9482638) SHA1(ea6256136362a12a40d6b168157c28a14236fcc1) )
	ROM_LOAD( "obm-14", 0x400000, 0x80000, CRC(79ed46b8) SHA1(93b503b58a316be312a74f2da7df3dbcd275884b) )
	ROM_LOAD( "obm-3",  0x480000, 0x80000, CRC(6e924cb8) SHA1(3c56dfcd042108b1cd16395bcdda0fd92a6ab0f7) )
	ROM_LOAD( "obm-9",  0x500000, 0x80000, CRC(be1fb64e) SHA1(4141b6b78fa9830cf5dc4f4f0b29e87e57f70ccb) )
	ROM_LOAD( "obm-15", 0x580000, 0x80000, CRC(5e0efb71) SHA1(d556ed9307a9a9f59f5235981b4b091a88399c98) )
	ROM_LOAD( "obm-4",  0x600000, 0x80000, CRC(f34b67bd) SHA1(91d6553144e45ea1b96bf59403b3e26224b79a7d) )
	ROM_LOAD( "obm-10", 0x680000, 0x80000, CRC(00c68a23) SHA1(a4984932ac3fae368700f77ad16b35b0138dbc21) )
	ROM_LOAD( "obm-16", 0x700000, 0x80000, CRC(e9516379) SHA1(8d9aaa2ee1331dd3eb8951a1008811703dd9f41d) )
	ROM_LOAD( "obm-5",  0x780000, 0x80000, CRC(b2b976f3) SHA1(77dee06ac1187c8a1c4188951bd7b0a62ec84350) )
	ROM_LOAD( "obm-11", 0x800000, 0x80000, CRC(7bf1e4da) SHA1(4aeef3b7c23303580a851dc793e9671a2a0f421f) )
	ROM_LOAD( "obm-17", 0x880000, 0x80000, CRC(47bd56e8) SHA1(e10569e89083165a7efe29f84167a1c15171ccaf) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "sf4-0",  0x000000, 0x80000, CRC(25938b2d) SHA1(6336e41eee58cab9a524b9bca08965786cc133d3) )
	ROM_LOAD( "sf4-1",  0x080000, 0x80000, CRC(2269ce5c) SHA1(4c6169acf17bba94dc5684f5db60d5bcf73ad068) )

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "sf3-0",  0x000000, 0x80000, CRC(86cd162c) SHA1(95d5f300e3671ebe29b2331325f4d80b96988619) )
	ROM_LOAD( "sf3-1",  0x080000, 0x80000, CRC(2e755e54) SHA1(74b1e099358a07848f7c22c71fbe2661e1ebb417) )

	ROM_REGION( 0x100000, REGION_GFX4, ROMREGION_DISPOSE )
	ROM_LOAD( "sf2-0",  0x000000, 0x80000, CRC(622a1816) SHA1(b7b88a90ff69e8f2e291e1f9299708ec97ef9b77) )
	ROM_LOAD( "sf2-1",  0x080000, 0x80000, CRC(545f89e9) SHA1(e7d52dc2da3770d7310698af47da9ff7ec32388c) )

	ROM_REGION( 0x100000, REGION_GFX5, ROMREGION_DISPOSE )
	ROM_LOAD( "dw-0",   0x000000, 0x80000, CRC(b9b18d00) SHA1(4f38502c75eae88916bc58bfd5d255bac59d0813) )
	ROM_LOAD( "dw-1",   0x080000, 0x80000, CRC(7aea0b12) SHA1(07cbcd6ddcd9ead068b0f5763829e8474b699085) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 ) /* Samples */
	ROM_LOAD( "oki0",   0x000000, 0x40000, CRC(005811ce) SHA1(9149bc8e9cc16ce3db4e22f8cb7ea8a57a66980e) )

	ROM_REGION( 0x080000, REGION_USER1, 0 ) /* Samples */
	ROM_LOAD( "oki1",   0x000000, 0x80000, CRC(bcde2330) SHA1(452d871360fa907d2e4ebad93c3fba9a3fa32fa7) )

	/* $00000-$20000 stays the same in all sound banks, */
	/* the second half of the bank is what gets switched */
	ROM_REGION( 0x100000, REGION_SOUND2, 0 ) /* Samples */
	ROM_COPY( REGION_USER1, 0x000000, 0x000000, 0x020000)
	ROM_COPY( REGION_USER1, 0x000000, 0x020000, 0x020000)
	ROM_COPY( REGION_USER1, 0x000000, 0x040000, 0x020000)
	ROM_COPY( REGION_USER1, 0x020000, 0x060000, 0x020000)
	ROM_COPY( REGION_USER1, 0x000000, 0x080000, 0x020000)
	ROM_COPY( REGION_USER1, 0x040000, 0x0a0000, 0x020000)
	ROM_COPY( REGION_USER1, 0x000000, 0x0c0000, 0x020000)
	ROM_COPY( REGION_USER1, 0x060000, 0x0e0000, 0x020000)
ROM_END

GAME( 199?, mwarr, 0, mwarr, mwarr, 0, ROT0,  "Elettronica Video-Games S.R.L.", "Mighty Warriors", 0 ,0)
