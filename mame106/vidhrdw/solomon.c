#include "driver.h"

UINT8 *solomon_videoram2;
UINT8 *solomon_colorram2;

static tilemap *bg_tilemap, *fg_tilemap;

WRITE8_HANDLER( solomon_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap, offset);
	}
}

WRITE8_HANDLER( solomon_colorram_w )
{
	if (colorram[offset] != data)
	{
		colorram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap, offset);
	}
}

WRITE8_HANDLER( solomon_videoram2_w )
{
	if (solomon_videoram2[offset] != data)
	{
		solomon_videoram2[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

WRITE8_HANDLER( solomon_colorram2_w )
{
	if (solomon_colorram2[offset] != data)
	{
		solomon_colorram2[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

WRITE8_HANDLER( solomon_flipscreen_w )
{
	if (flip_screen != (data & 0x01))
	{
		flip_screen_set(data & 0x01);
		tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);
	}
}

static void get_bg_tile_info(int tile_index)
{
	int attr = solomon_colorram2[tile_index];
	int code = solomon_videoram2[tile_index] + 256 * (attr & 0x07);
	int color = ((attr & 0x70) >> 4);
	int flags = ((attr & 0x80) ? TILE_FLIPX : 0) | ((attr & 0x08) ? TILE_FLIPY : 0);

	SET_TILE_INFO(1, code, color, flags)
}

static void get_fg_tile_info(int tile_index)
{
	int attr = colorram[tile_index];
	int code = videoram[tile_index] + 256 * (attr & 0x07);
	int color = (attr & 0x70) >> 4;

	SET_TILE_INFO(0, code, color, 0)
}

VIDEO_START( solomon )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	if ( !bg_tilemap )
		return 1;

	fg_tilemap = tilemap_create(get_fg_tile_info, tilemap_scan_rows,
		TILEMAP_TRANSPARENT, 8, 8, 32, 32);

	if ( !fg_tilemap )
		return 1;

	tilemap_set_transparent_pen(fg_tilemap, 0);

	return 0;
}

static void solomon_draw_sprites( mame_bitmap *bitmap )
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size - 4; offs >= 0; offs -= 4)
	{
		int code = spriteram[offs] + 16 * (spriteram[offs + 1] & 0x10);
		int color = (spriteram[offs + 1] & 0x0e) >> 1;
		int flipx = spriteram[offs + 1] & 0x40;
		int flipy =	spriteram[offs + 1] & 0x80;
		int sx = spriteram[offs + 3];
		int sy = 241 - spriteram[offs + 2];

		if (flip_screen)
		{
			sx = 240 - sx;
			sy = 242 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( solomon )
{
	tilemap_draw(bitmap, &Machine->visible_area, bg_tilemap, 0, 0);
	tilemap_draw(bitmap, &Machine->visible_area, fg_tilemap, 0, 0);
	solomon_draw_sprites(bitmap);
}
