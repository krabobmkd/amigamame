/***************************************************************************

    Mustache Boy
    (c)1987 March Electronics

***************************************************************************/


#include "driver.h"

static tilemap *bg_tilemap;
static int control_byte;

PALETTE_INIT(mustache)
{
  int i;

  for (i = 0;i < 256;i++)
  {
	int bit0,bit1,bit2,bit3,r,g,b;

	/* red component */
	bit0 = (color_prom[i] >> 0) & 0x01;
	bit1 = (color_prom[i] >> 1) & 0x01;
	bit2 = (color_prom[i] >> 2) & 0x01;
	bit3 = (color_prom[i] >> 3) & 0x01;
	r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

	/* green component */
	bit0 = (color_prom[i + 256] >> 0) & 0x01;
	bit1 = (color_prom[i + 256] >> 1) & 0x01;
	bit2 = (color_prom[i + 256] >> 2) & 0x01;
	bit3 = (color_prom[i + 256] >> 3) & 0x01;
	g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

	/* blue component */
	bit0 = (color_prom[i + 512] >> 0) & 0x01;
	bit1 = (color_prom[i + 512] >> 1) & 0x01;
	bit2 = (color_prom[i + 512] >> 2) & 0x01;
	bit3 = (color_prom[i + 512] >> 3) & 0x01;
	b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

	palette_set_color(i,r,g,b);
  }
}

WRITE8_HANDLER( mustache_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset / 2);
	}
}

WRITE8_HANDLER (mustache_video_control_w)
{
	if (flip_screen != (data & 0x01))
	{
		flip_screen_set(data & 0x01);
		tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);
	}

	/* tile bank */

	if ((control_byte ^ data) & 0x08)
	{
		control_byte = data;
		tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);
	}
}

WRITE8_HANDLER( mustache_scroll_w )
{
	tilemap_set_scrollx(bg_tilemap, 0, 0x100 - data);
	tilemap_set_scrollx(bg_tilemap, 1, 0x100 - data);
	tilemap_set_scrollx(bg_tilemap, 2, 0x100 - data);
	tilemap_set_scrollx(bg_tilemap, 3, 0x100);
}

static void get_bg_tile_info(int tile_index)
{
	int attr = videoram[2 * tile_index + 1];
	int code = videoram[2 * tile_index] + ((attr & 0xe0) << 3) + ((control_byte & 0x08) << 7);
	int color = attr & 0x0f;

	SET_TILE_INFO(0, code, color, 0)
}

VIDEO_START( mustache )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows_flip_x,
		TILEMAP_OPAQUE, 8, 8, 64, 32);

	tilemap_set_scroll_rows(bg_tilemap, 4);

	return 0;
}

static void mustache_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	rectangle clip = *cliprect;
	const gfx_element *gfx = Machine->gfx[1];
	int offs;

	
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
		&clip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		int sy = 240-spriteram[offs];
		int sx = 240-spriteram[offs+3];
		int code = spriteram[offs+2];
		int attr = spriteram[offs+1];
		int color = (attr & 0xe0)>>5;

		if (sy == 240) continue;

		code+=(attr&0x0c)<<6;

		if ((control_byte & 0xa))
			clip.max_y = Machine->visible_area.max_y;
		else
			if (flip_screen)
				clip.min_y = Machine->visible_area.min_y + 56;
			else
				clip.max_y = Machine->visible_area.max_y - 56;

		if (flip_screen)
		{
			sx = 240 - sx;
			sy = 240 - sy;
		}

		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flip_screen;
		dgp0.flipy = flip_screen;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( mustache )
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	mustache_draw_sprites(bitmap, cliprect);
}
