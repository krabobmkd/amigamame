/*************************************************************************

    Atari Super Breakout hardware

*************************************************************************/

#include "driver.h"
#include "artwork.h"
#include "includes/sbrkout.h"

UINT8 *sbrkout_horiz_ram;
UINT8 *sbrkout_vert_ram;

static tilemap *bg_tilemap;

WRITE8_HANDLER( sbrkout_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

static void get_bg_tile_info(int tile_index)
{
	int code = (videoram[tile_index] & 0x80) ? videoram[tile_index] : 0;

	SET_TILE_INFO(0, code, 0, 0)
}

VIDEO_START( sbrkout )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	if ( !bg_tilemap )
		return 1;

	return 0;
}

static void sbrkout_draw_balls( mame_bitmap *bitmap )
{
	int ball;

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (ball=2; ball>=0; ball--)
	{
		int code = ((sbrkout_vert_ram[ball * 2 + 1] & 0x80) >> 7);
		int sx = 31 * 8 - sbrkout_horiz_ram[ball * 2];
		int sy = 30 * 8 - sbrkout_vert_ram[ball * 2];

		
		dgp0.code = code;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( sbrkout )
{
	tilemap_draw(bitmap, &Machine->visible_area, bg_tilemap, 0, 0);
	sbrkout_draw_balls(bitmap);
}
