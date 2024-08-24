/***************************************************************************

Atari Canyon Bomber video emulation

***************************************************************************/

#include "driver.h"
#include "includes/canyon.h"

static tilemap *bg_tilemap;

UINT8* canyon_videoram;


WRITE8_HANDLER( canyon_videoram_w )
{
	if (canyon_videoram[offset] != data)
	{
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}

	canyon_videoram[offset] = data;
}


static void get_bg_tile_info(int tile_index)
{
	UINT8 code = canyon_videoram[tile_index];

	SET_TILE_INFO(0, code & 0x3f, code >> 7, 0)
}


VIDEO_START( canyon )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	return (bg_tilemap == NULL) ? 1 : 0;
}


static void canyon_draw_sprites(mame_bitmap *bitmap, const rectangle* cliprect)
{
	int i;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
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
	for (i = 0; i < 2; i++)
	{
		int x = canyon_videoram[0x3d0 + 2 * i + 0x1];
		int y = canyon_videoram[0x3d0 + 2 * i + 0x8];
		int c = canyon_videoram[0x3d0 + 2 * i + 0x9];

		
		dgp0.code = c >> 3;
		dgp0.color = i;
		dgp0.flipx = !(c & 0x80);
		dgp0.sx = 224 - x;
		dgp0.sy = 240 - y;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}


static void canyon_draw_bombs(mame_bitmap *bitmap, const rectangle* cliprect)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		int sx = 254 - canyon_videoram[0x3d0 + 2 * i + 0x5];
		int sy = 246 - canyon_videoram[0x3d0 + 2 * i + 0xc];

		rectangle rect;

		rect.min_x = sx;
		rect.min_y = sy;
		rect.max_x = sx + 1;
		rect.max_y = sy + 1;

		if (rect.min_x < cliprect->min_x) rect.min_x = cliprect->min_x;
		if (rect.min_y < cliprect->min_y) rect.min_y = cliprect->min_y;
		if (rect.max_x > cliprect->max_x) rect.max_x = cliprect->max_x;
		if (rect.max_y > cliprect->max_y) rect.max_y = cliprect->max_y;

		fillbitmap(bitmap, i, &rect);
	}
}


VIDEO_UPDATE( canyon )
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);

	canyon_draw_sprites(bitmap, cliprect);

	canyon_draw_bombs(bitmap, cliprect);

	/* watchdog is disabled during service mode */
		watchdog_enable(!(readinputport(2) & 0x10));
}
