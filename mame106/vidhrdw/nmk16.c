#include "driver.h"

UINT16 *nmk_bgvideoram,*nmk_fgvideoram,*nmk_txvideoram;
UINT16 *gunnail_scrollram;
static UINT16 gunnail_scrolly;
UINT16 tharrier_scroll;

static int redraw_bitmap;

static UINT16 *spriteram_old,*spriteram_old2;
static int bgbank;
static int videoshift;
static int bioship_background_bank;
static UINT8 bioship_scroll[4];

static tilemap *bg_tilemap,*fg_tilemap,*tx_tilemap;
static mame_bitmap *background_bitmap;

/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

//not 100% right yet (check attract mode in raphero)
static UINT32 bg_scan(UINT32 col,UINT32 row,UINT32 num_cols,UINT32 num_rows)
{
	/* logical (col,row) -> memory offset */

	col = (col & 0xff) | ((col & 0x300)<<1);

	return (row & 0x0f) + ((col & 0x6ff) << 4) + ((row & 0x70) << 8);
}

static UINT32 bg_scan_td2(UINT32 col,UINT32 row,UINT32 num_cols,UINT32 num_rows)
{
	/* logical (col,row) -> memory offset */
	return (row & 0x0f) + ((col & 0x3ff) << 4) + ((row & 0x70) << 10);
}


static void macross_get_bg_tile_info(int tile_index)
{
	int code = nmk_bgvideoram[tile_index];
	SET_TILE_INFO(
			1,
			(code & 0xfff) + (bgbank << 12),
			code >> 12,
			0)
}

static void strahl_get_fg_tile_info(int tile_index)
{
	int code = nmk_fgvideoram[tile_index];
	SET_TILE_INFO(
			3,
			(code & 0xfff),
			code >> 12,
			0)
}

static void macross_get_tx_tile_info(int tile_index)
{
	int code = nmk_txvideoram[tile_index];
	SET_TILE_INFO(
			0,
			code & 0xfff,
			code >> 12,
			0)
}

static void bjtwin_get_bg_tile_info(int tile_index)
{
	int code = nmk_bgvideoram[tile_index];
	int bank = (code & 0x800) ? 1 : 0;
	SET_TILE_INFO(
			bank,
			(code & 0x7ff) + ((bank) ? (bgbank << 11) : 0),
			code >> 12,
			0)
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( bioship )
{
	bg_tilemap = tilemap_create(macross_get_bg_tile_info,bg_scan,TILEMAP_TRANSPARENT,16,16,256,32);
	tx_tilemap = tilemap_create(macross_get_tx_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,32,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);
	background_bitmap = auto_bitmap_alloc(8192,512);

	if (!bg_tilemap || !background_bitmap)
		return 1;

	tilemap_set_transparent_pen(bg_tilemap,15);
	tilemap_set_transparent_pen(tx_tilemap,15);
	bioship_background_bank=0;
	redraw_bitmap = 1;

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift =  0;	/* 256x224 screen, no shift */

	return 0;
}

VIDEO_START( strahl )
{
	bg_tilemap = tilemap_create(macross_get_bg_tile_info,bg_scan,TILEMAP_OPAQUE,16,16,256,32);
	fg_tilemap = tilemap_create(strahl_get_fg_tile_info, bg_scan,TILEMAP_TRANSPARENT,16,16,256,32);
	tx_tilemap = tilemap_create(macross_get_tx_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,32,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);

	if (!bg_tilemap || !fg_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,15);
	tilemap_set_transparent_pen(tx_tilemap,15);

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift =  0;	/* 256x224 screen, no shift */
	background_bitmap = NULL;
	return 0;
}

VIDEO_START( macross )
{
	bg_tilemap = tilemap_create(macross_get_bg_tile_info,bg_scan,TILEMAP_OPAQUE,16,16,256,32);
	tx_tilemap = tilemap_create(macross_get_tx_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,32,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);

	if (!bg_tilemap)
		return 1;

	tilemap_set_transparent_pen(tx_tilemap,15);

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift =  0;	/* 256x224 screen, no shift */
	background_bitmap = NULL;

	return 0;
}

VIDEO_START( gunnail )
{
	bg_tilemap = tilemap_create(macross_get_bg_tile_info,bg_scan,TILEMAP_OPAQUE,16,16,256,32);
	tx_tilemap = tilemap_create(macross_get_tx_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,64,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);

	if (!bg_tilemap)
		return 1;

	tilemap_set_transparent_pen(tx_tilemap,15);
	tilemap_set_scroll_rows(bg_tilemap,512);

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift = 64;	/* 384x224 screen, leftmost 64 pixels have to be retrieved */
						/* from the other side of the tilemap (!) */
	background_bitmap = NULL;

	return 0;
}

VIDEO_START( macross2 )
{
	bg_tilemap = tilemap_create(macross_get_bg_tile_info,bg_scan,TILEMAP_OPAQUE,16,16,1024,128);
	tx_tilemap = tilemap_create(macross_get_tx_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,64,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);

	if (!bg_tilemap)
		return 1;

	tilemap_set_transparent_pen(tx_tilemap,15);

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift = 64;	/* 384x224 screen, leftmost 64 pixels have to be retrieved */
						/* from the other side of the tilemap (!) */
	background_bitmap = NULL;
	return 0;
}

VIDEO_START( tdragon2 )
{
	bg_tilemap = tilemap_create(macross_get_bg_tile_info,bg_scan_td2,TILEMAP_OPAQUE,16,16,1024,32);
	tx_tilemap = tilemap_create(macross_get_tx_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,64,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);

	if (!bg_tilemap)
		return 1;

	tilemap_set_transparent_pen(tx_tilemap,15);

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift = 64;	/* 384x224 screen, leftmost 64 pixels have to be retrieved */
						/* from the other side of the tilemap (!) */
	background_bitmap = NULL;
	return 0;
}

VIDEO_START( bjtwin )
{
	bg_tilemap = tilemap_create(bjtwin_get_bg_tile_info,tilemap_scan_cols,TILEMAP_OPAQUE,8,8,64,32);
	spriteram_old = auto_malloc(spriteram_size);
	spriteram_old2 = auto_malloc(spriteram_size);

	if (!bg_tilemap)
		return 1;

	memset(spriteram_old,0,spriteram_size);
	memset(spriteram_old2,0,spriteram_size);

	videoshift = 64;	/* 384x224 screen, leftmost 64 pixels have to be retrieved */
						/* from the other side of the tilemap (!) */
	background_bitmap = NULL;
	return 0;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

READ16_HANDLER( nmk_bgvideoram_r )
{
	return nmk_bgvideoram[offset];
}

WRITE16_HANDLER( nmk_bgvideoram_w )
{
	int oldword = nmk_bgvideoram[offset];
	int newword = oldword;
	COMBINE_DATA(&newword);

	if (oldword != newword)
	{
		nmk_bgvideoram[offset] = newword;
		tilemap_mark_tile_dirty(bg_tilemap,offset);
	}
}

READ16_HANDLER( nmk_fgvideoram_r )
{
	return nmk_fgvideoram[offset];
}

WRITE16_HANDLER( nmk_fgvideoram_w )
{
	int oldword = nmk_fgvideoram[offset];
	int newword = oldword;
	COMBINE_DATA(&newword);

	if (oldword != newword)
	{
		nmk_fgvideoram[offset] = newword;
		tilemap_mark_tile_dirty(fg_tilemap,offset);
	}
}

READ16_HANDLER( nmk_txvideoram_r )
{
	return nmk_txvideoram[offset];
}

WRITE16_HANDLER( nmk_txvideoram_w )
{
	int oldword = nmk_txvideoram[offset];
	int newword = oldword;
	COMBINE_DATA(&newword);

	if (oldword != newword)
	{
		nmk_txvideoram[offset] = newword;
		tilemap_mark_tile_dirty(tx_tilemap,offset);
	}
}

static int mustang_bg_xscroll;

WRITE16_HANDLER( mustang_scroll_w )
{
//  printf("mustang %04x %04x %04x\n",offset,data,mem_mask);

	switch (data & 0xff00)
	{
		case 0x0000:
			mustang_bg_xscroll = (mustang_bg_xscroll & 0x00ff) | ((data & 0x00ff)<<8);
			break;

		case 0x0100:
			mustang_bg_xscroll = (mustang_bg_xscroll & 0xff00) | (data & 0x00ff);
			break;

		case 0x0200:
			break;

		case 0x0300:
			break;

		default:
			break;
	}

	tilemap_set_scrollx(bg_tilemap,0,mustang_bg_xscroll - videoshift);
}

WRITE16_HANDLER( bioshipbg_scroll_w )
{
	static UINT8 scroll[4];

	if (ACCESSING_MSB)
	{
		scroll[offset] = (data >> 8) & 0xff;

		if (offset & 2)
			tilemap_set_scrolly(bg_tilemap,0,scroll[2] * 256 + scroll[3]);
		else
			tilemap_set_scrollx(bg_tilemap,0,scroll[0] * 256 + scroll[1] - videoshift);
	}
}

WRITE16_HANDLER( nmk_scroll_w )
{
	if (ACCESSING_LSB)
	{
		static UINT8 scroll[4];

		scroll[offset] = data & 0xff;

		if (offset & 2)
			tilemap_set_scrolly(bg_tilemap,0,scroll[2] * 256 + scroll[3]);
		else
			tilemap_set_scrollx(bg_tilemap,0,scroll[0] * 256 + scroll[1] - videoshift);
	}
}

WRITE16_HANDLER( nmk_scroll_2_w )
{
	if (ACCESSING_LSB)
	{
		static UINT8 scroll[4];

		scroll[offset] = data & 0xff;

		if (offset & 2)
			tilemap_set_scrolly(fg_tilemap,0,scroll[2] * 256 + scroll[3]);
		else
			tilemap_set_scrollx(fg_tilemap,0,scroll[0] * 256 + scroll[1] - videoshift);
	}
}

WRITE16_HANDLER( nmk_scroll_3_w )
{
	COMBINE_DATA(&gunnail_scrollram[offset]);

//  ui_popup( "scroll %04x, %04x", gunnail_scrollram[0], gunnail_scrollram[0x100]);

	tilemap_set_scrollx(bg_tilemap,0,gunnail_scrollram[0]-videoshift);
	tilemap_set_scrolly(bg_tilemap,0,gunnail_scrollram[0x100]);
}


WRITE16_HANDLER( vandyke_scroll_w )
{
	static UINT16 scroll[4];

	scroll[offset] = data;

	tilemap_set_scrollx(bg_tilemap,0,scroll[0] * 256 + (scroll[1] >> 8));
	tilemap_set_scrolly(bg_tilemap,0,scroll[2] * 256 + (scroll[3] >> 8));
}

WRITE16_HANDLER( manybloc_scroll_w )
{
	COMBINE_DATA(&gunnail_scrollram[offset]);

	tilemap_set_scrollx(bg_tilemap,0,gunnail_scrollram[0x82/2]-videoshift);
	tilemap_set_scrolly(bg_tilemap,0,gunnail_scrollram[0xc2/2]);
}

WRITE16_HANDLER( nmk_flipscreen_w )
{
	if (ACCESSING_LSB)
		flip_screen_set(data & 0x01);
}

WRITE16_HANDLER( nmk_tilebank_w )
{
	if (ACCESSING_LSB)
	{
		if (bgbank != (data & 0xff))
		{
			bgbank = data & 0xff;
			tilemap_mark_all_tiles_dirty(bg_tilemap);
		}
	}
}

WRITE16_HANDLER( bioship_scroll_w )
{
	if (ACCESSING_MSB)
		bioship_scroll[offset]=data>>8;
}

WRITE16_HANDLER( bioship_bank_w )
{
	if (ACCESSING_LSB)
	{
		if (bioship_background_bank != data)
		{
			bioship_background_bank = data;
			redraw_bitmap=1;
		}
	}
}

WRITE16_HANDLER( gunnail_scrollx_w )
{
	COMBINE_DATA(&gunnail_scrollram[offset]);
}

WRITE16_HANDLER( gunnail_scrolly_w )
{
	COMBINE_DATA(&gunnail_scrolly);
}

/***************************************************************************

  Display refresh

***************************************************************************/

extern int is_blkheart;

// manybloc uses extra flip bits on the sprites, but these break other games

static void draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, int priority)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[2], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		if ((spriteram_old2[offs] & 0x0001) || (spriteram_old2[offs] && is_blkheart))
		{
			int sx = (spriteram_old2[offs+4] & 0x1ff) + videoshift;
			int sy = (spriteram_old2[offs+6] & 0x1ff);
			int code = spriteram_old2[offs+3];
			int color = spriteram_old2[offs+7];
			int w = (spriteram_old2[offs+1] & 0x0f);
			int h = ((spriteram_old2[offs+1] & 0xf0) >> 4);
			int pri = (spriteram_old2[offs] & 0xc0) >> 6;
			int xx,yy,x;
			int delta = 16;

			if(pri != priority)
				continue;

			if (flip_screen)
			{
				sx = 368 - sx;
				sy = 240 - sy;
				delta = -16;
			}

			yy = h;
			do
			{
				x = sx;
				xx = w;
				do
				{
					
					dgp0.code = code;
					dgp0.color = color;
					dgp0.flipx = flip_screen;
					dgp0.flipy = flip_screen;
					dgp0.sx = ((x + 16) & 0x1ff) - 16;
					dgp0.sy = sy & 0x1ff;
					drawgfx(&dgp0);
					code++;
					x += delta;
				} while (--xx >= 0);

				sy += delta;
			} while (--yy >= 0);
		}
	}
	} // end of patch paragraph

}

/* sprites have flipping and are not delayed 2 frames */
static void manybloc_draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, int priority)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[2], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		if ((spriteram16[offs] & 0x0001) || (spriteram16[offs] && is_blkheart))
		{
			int sx = (spriteram16[offs+4] & 0x1ff) + videoshift;
			int sy = (spriteram16[offs+6] & 0x1ff);
			int code = spriteram16[offs+3];
			int color = spriteram16[offs+7];
			int w = (spriteram16[offs+1] & 0x0f);
			int h = ((spriteram16[offs+1] & 0xf0) >> 4);
			int pri = (spriteram16[offs] & 0xc0) >> 6;
			/* these would break some of the nmk games ... */
			int flipy= ((spriteram16[offs+1] & 0x0200) >> 9);
			int flipx = ((spriteram16[offs+1] & 0x0100) >> 8);

			int xx,yy,x;
			int delta = 16;

			if(pri != priority)
				continue;

			flipx ^= flip_screen;
			flipy ^= flip_screen;

			if (flip_screen)
			{
				sx = 368 - sx;
				sy = 240 - sy;
				delta = -16;
			}

			yy = h;
			do
			{
				x = sx;
				xx = w;
				do
				{
					
					dgp1.code = code;
					dgp1.color = color;
					dgp1.flipx = flipx;
					dgp1.flipy = flipy;
					dgp1.sx = ((x + 16) & 0x1ff) - 16;
					dgp1.sy = sy & 0x1ff;
					drawgfx(&dgp1);

					code++;
					x += delta;
				} while (--xx >= 0);

				sy += delta;
			} while (--yy >= 0);
		}
	}
	} // end of patch paragraph

}

static void tharrier_draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, int priority)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		Machine->gfx[2], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		if ((spriteram16[offs] & 0x0001) || (spriteram16[offs] && is_blkheart))
		{
			int sx = (spriteram16[offs+4] & 0x1ff) + videoshift;
			int sy = (spriteram16[offs+6] & 0x1ff);
			int code = spriteram16[offs+3];
			int color = spriteram16[offs+7];
			int w = (spriteram16[offs+1] & 0x0f);
			int h = ((spriteram16[offs+1] & 0xf0) >> 4);
			int pri = (spriteram16[offs] & 0xc0) >> 6;
			int flipy= ((spriteram16[offs+1] & 0x0200) >> 9);
			int flipx = ((spriteram16[offs+1] & 0x0100) >> 8);

			int xx,yy,x;
			int delta = 16;

			if(pri != priority)
				continue;

			flipx ^= flip_screen;
			flipy ^= flip_screen;

			if (flip_screen)
			{
				sx = 368 - sx;
				sy = 240 - sy;
				delta = -16;
			}

			yy = h;
			sy+=flipy?(delta*h):0;
			do
			{
				x = sx+(flipx?(delta*w):0);


				xx = w;
				do
				{
					
					dgp2.code = code;
					dgp2.color = color;
					dgp2.flipx = flipx;
					dgp2.flipy = flipy;
					dgp2.sx = ((x + 16) & 0x1ff) - 16;
					dgp2.sy = sy & 0x1ff;
					drawgfx(&dgp2);

					code++;
					x +=delta * ( flipx?-1:1 );


				} while (--xx >= 0);
				sy += delta * ( flipy?-1:1);

			} while (--yy >= 0);
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( macross )
{
	tilemap_set_scrollx(tx_tilemap,0,-videoshift);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,3);
	draw_sprites(bitmap,cliprect,2);
	draw_sprites(bitmap,cliprect,1);
	draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

extern UINT16 *mcu_shared_ram;
extern UINT16 *mcu_work_ram;

/*coin setting MCU simulation*/
static void mcu_run(UINT8 dsw_setting)
{
	static UINT8 read_coin;
	static UINT8 old_value;
	static UINT8 coina,coinb;
	UINT8 dsw_a,dsw_b;
	/*needed because of the uncompatibility of the dsw settings.*/
	if(dsw_setting)
	{
		dsw_a = (readinputport(2+dsw_setting) & 0x7);
		dsw_b = (readinputport(2+dsw_setting) & 0x38) >> 3;
	}
	else
	{
		dsw_a = (readinputport(2) & 0x0700) >> 8;
		dsw_b = (readinputport(2) & 0x3800) >> 11;
	}

	read_coin = old_value;
	old_value = readinputport(0);

	if(dsw_a == 0 || dsw_b == 0)
		mcu_work_ram[0x000/2]|=0x4000; //free_play

	if(read_coin != old_value)
	{
		if(!(readinputport(0) & 0x01))//COIN1
		{
			switch(dsw_a & 7)
			{
				case 1: mcu_shared_ram[0xf00/2]+=4; break;
				case 2: mcu_shared_ram[0xf00/2]+=3; break;
				case 3: mcu_shared_ram[0xf00/2]+=2; break;
				case 4:
				coina++;
				if(coina >= 4)
				{
					coina = 0;
					mcu_shared_ram[0xf00/2]++;
				}
				break;
				case 5:
				coina++;
				if(coina >= 3)
				{
					coina = 0;
					mcu_shared_ram[0xf00/2]++;
				}
				break;
				case 6:
				coina++;
				if(coina >= 2)
				{
					coina = 0;
					mcu_shared_ram[0xf00/2]++;
				}
				break;
				case 7: mcu_shared_ram[0xf00/2]++; break;
			}
		}

		if(!(readinputport(0) & 0x02))//COIN2
		{
			switch(dsw_b & 7)
			{
				case 1: mcu_shared_ram[0xf00/2]+=4; break;
				case 2: mcu_shared_ram[0xf00/2]+=3; break;
				case 3: mcu_shared_ram[0xf00/2]+=2; break;
				case 4:
				coinb++;
				if(coinb >= 4)
				{
					coinb = 0;
					mcu_shared_ram[0xf00/2]++;
				}
				break;
				case 5:
				coinb++;
				if(coinb >= 3)
				{
					coinb = 0;
					mcu_shared_ram[0xf00/2]++;
				}
				break;
				case 6:
				coinb++;
				if(coinb >= 2)
				{
					coinb = 0;
					mcu_shared_ram[0xf00/2]++;
				}
				break;
				case 7: mcu_shared_ram[0xf00/2]++; break;
			}
		}

		if(!(readinputport(0) & 0x04))//SERVICE_COIN
			mcu_shared_ram[0xf00/2]++;

		if(mcu_shared_ram[0xf00/2] >= 1 && (mcu_work_ram[0x000/2] & 0x8000))/*enable start button*/
		{
			/*Start a 1-player game,but don't decrement if the player 1 is already playing*/
			if((!(readinputport(0) & 0x08)) /*START1*/
			&& (!(mcu_work_ram[0x000/2] & 0x0200)) /*PLAYER-1 playing*/
			)
				mcu_shared_ram[0xf00/2]--;

			/*Start a 2-players game,but don't decrement if the player 2 is already playing*/
			if((!(readinputport(0) & 0x10))
			&& (!(mcu_work_ram[0x000/2] & 0x0100))
			)
			{
				if(!(mcu_work_ram[0x000/2] & 0x0200) && mcu_shared_ram[0xf00/2] >= 2)
					mcu_shared_ram[0xf00/2]-=2;
				else
					mcu_shared_ram[0xf00/2]--;
			}
		}

		if(mcu_shared_ram[0xf00/2] > 99) mcu_shared_ram[0xf00/2] = 99;
	}
}

VIDEO_UPDATE( tdragon )
{
	mcu_run(1);

	tilemap_set_scrollx(tx_tilemap,0,-videoshift);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,3);
	draw_sprites(bitmap,cliprect,2);
	draw_sprites(bitmap,cliprect,1);
	draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( hachamf )
{
	mcu_run(0);

	tilemap_set_scrollx(tx_tilemap,0,-videoshift);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,3);
	draw_sprites(bitmap,cliprect,2);
	draw_sprites(bitmap,cliprect,1);
	draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( manybloc )
{
	tilemap_set_scrollx(tx_tilemap,0,-videoshift);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	manybloc_draw_sprites(bitmap,cliprect,3);
	manybloc_draw_sprites(bitmap,cliprect,2);
	manybloc_draw_sprites(bitmap,cliprect,1);
	manybloc_draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( tharrier )
{
	tilemap_set_scrollx(tx_tilemap,0,-videoshift);
	tilemap_set_scrollx(bg_tilemap,0,tharrier_scroll);
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tharrier_draw_sprites(bitmap,cliprect,3);
	tharrier_draw_sprites(bitmap,cliprect,2);
	tharrier_draw_sprites(bitmap,cliprect,1);
	tharrier_draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( gunnail )
{
	int i;

	for (i = 0;i < 256;i++)
	{
		tilemap_set_scrollx(bg_tilemap,(i+gunnail_scrolly) & 0x1ff,gunnail_scrollram[0] + gunnail_scrollram[i] - videoshift);
	}
	tilemap_set_scrolly(bg_tilemap,0,gunnail_scrolly);

	video_update_macross(screen,bitmap,cliprect);
}

VIDEO_UPDATE( bioship )
{
	UINT16 *tilerom = (UINT16 *)memory_region(REGION_GFX5);
	int scrollx=-(bioship_scroll[1] + bioship_scroll[0]*256);
	int scrolly=-(bioship_scroll[3] + bioship_scroll[2]*256);

	tilemap_set_scrollx(tx_tilemap,0,-videoshift);

	if (redraw_bitmap)
	{
		int bank = bioship_background_bank * 0x2000;
		int sx=0, sy=0, offs;
		redraw_bitmap=0;

		/* Draw background from tile rom */
		
		{ 
		struct drawgfxParams dgp3={
			background_bitmap, 	// dest
			Machine->gfx[3], 	// gfx
			0, 	// code
			0, 	// color
			0, 	// flipx
			0, 	// flipy
			0, 	// sx
			0, 	// sy
			0, 	// clip
			TRANSPARENCY_NONE, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (offs = 0;offs <0x1000;offs++) {
				UINT16 data = tilerom[offs+bank];
				int numtile = data&0xfff;
				int color = (data&0xf000)>>12;

				
				dgp3.code = numtile;
				dgp3.color = color;
				dgp3.sx = /* no flip */						16*sx;
				dgp3.sy = 16*sy;
				drawgfx(&dgp3);

				data = tilerom[offs+0x1000+bank];
				numtile = data&0xfff;
				color = (data&0xf000)>>12;
				
				dgp3.code = numtile;
				dgp3.color = color;
				dgp3.sx = /* no flip */						16*sx;
				dgp3.sy = (16*sy)+256;
				drawgfx(&dgp3);

				sy++;
				if (sy==16) {sy=0; sx++;}
		}
		} // end of patch paragraph

	}

	copyscrollbitmap(bitmap,background_bitmap,1,&scrollx,1,&scrolly,cliprect,TRANSPARENCY_NONE,0);
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,3);
	draw_sprites(bitmap,cliprect,2);
	draw_sprites(bitmap,cliprect,1);
	draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( strahl )
{
	tilemap_set_scrollx(tx_tilemap,0,-videoshift);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,3);
	draw_sprites(bitmap,cliprect,2);
	draw_sprites(bitmap,cliprect,1);
	draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( bjtwin )
{
	tilemap_set_scrollx(bg_tilemap,0,-videoshift);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,3);
	draw_sprites(bitmap,cliprect,2);
	draw_sprites(bitmap,cliprect,1);
	draw_sprites(bitmap,cliprect,0);
}

VIDEO_EOF( nmk )
{
	/* looks like sprites are *two* frames ahead */
	memcpy(spriteram_old2,spriteram_old,spriteram_size);
	memcpy(spriteram_old,spriteram16,spriteram_size);
}
