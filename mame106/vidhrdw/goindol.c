/***************************************************************************
  Goindol

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"

UINT8 *goindol_bg_videoram;
UINT8 *goindol_fg_videoram;
UINT8 *goindol_fg_scrollx;
UINT8 *goindol_fg_scrolly;

size_t goindol_fg_videoram_size;
size_t goindol_bg_videoram_size;
int goindol_char_bank;

static tilemap *bg_tilemap,*fg_tilemap;



/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_fg_tile_info(int tile_index)
{
	int code = goindol_fg_videoram[2*tile_index+1];
	int attr = goindol_fg_videoram[2*tile_index];
	SET_TILE_INFO(
			0,
			code | ((attr & 0x7) << 8) | (goindol_char_bank << 11),
			(attr & 0xf8) >> 3,
			0)
}

static void get_bg_tile_info(int tile_index)
{
	int code = goindol_bg_videoram[2*tile_index+1];
	int attr = goindol_bg_videoram[2*tile_index];
	SET_TILE_INFO(
			1,
			code | ((attr & 0x7) << 8) | (goindol_char_bank << 11),
			(attr & 0xf8) >> 3,
			0)
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( goindol )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_SPLIT,      8,8,32,32);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);

	if (!fg_tilemap || !bg_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);

	return 0;
}


/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( goindol_fg_videoram_w )
{
	if (goindol_fg_videoram[offset] != data)
	{
		goindol_fg_videoram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap,offset / 2);
	}
}

WRITE8_HANDLER( goindol_bg_videoram_w )
{
	if (goindol_bg_videoram[offset] != data)
	{
		goindol_bg_videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset / 2);
	}
}



/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, int gfxbank, unsigned char *sprite_ram)
{
	int offs,sx,sy,tile,palette;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[gfxbank], 	// gfx
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
	for (offs = 0 ;offs < spriteram_size; offs+=4)
	{
		sx = sprite_ram[offs];
		sy = 240-sprite_ram[offs+1];

		if (flip_screen)
		{
			sx = 248 - sx;
			sy = 248 - sy;
		}

		if ((sprite_ram[offs+1] >> 3) && (sx < 248))
		{
			tile	 = ((sprite_ram[offs+3])+((sprite_ram[offs+2] & 7) << 8));
			tile	+= tile;
			palette	 = sprite_ram[offs+2] >> 3;

			
			dgp0.code = tile;
			dgp0.color = palette;
			dgp0.flipx = flip_screen;
			dgp0.flipy = flip_screen;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
			
			dgp0.code = tile+1;
			dgp0.color = palette;
			dgp0.flipx = flip_screen;
			dgp0.flipy = flip_screen;
			dgp0.sx = sx;
			dgp0.sy = sy + (flip_screen ? -8 : 8);
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( goindol )
{
	tilemap_set_scrollx(fg_tilemap,0,*goindol_fg_scrollx);
	tilemap_set_scrolly(fg_tilemap,0,*goindol_fg_scrolly);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
	draw_sprites(bitmap,cliprect,1,spriteram);
	draw_sprites(bitmap,cliprect,0,spriteram_2);
}
