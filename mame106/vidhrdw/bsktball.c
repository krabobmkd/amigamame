/***************************************************************************

    Atari Basketball hardware

***************************************************************************/

#include "driver.h"
#include "bsktball.h"

unsigned char *bsktball_motion;

static tilemap *bg_tilemap;

WRITE8_HANDLER( bsktball_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

static void get_bg_tile_info(int tile_index)
{
	int attr = videoram[tile_index];
	int code = ((attr & 0x0f) << 2) | ((attr & 0x30) >> 4);
	int color = (attr & 0x40) >> 6;
	int flags = (attr & 0x80) ? TILE_FLIPX : 0;

	SET_TILE_INFO(0, code, color, flags)
}

VIDEO_START( bsktball )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	if ( !bg_tilemap )
		return 1;

	return 0;
}

static void bsktball_draw_sprites( mame_bitmap *bitmap )
{
	int motion;

	
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
	for (motion=0;motion<16;motion++)
	{
		int pic = bsktball_motion[motion*4];
		int sy = 28*8 - bsktball_motion[motion*4 + 1];
		int sx = bsktball_motion[motion*4 + 2];
		int color = bsktball_motion[motion*4 + 3];
		int flipx = (pic & 0x80) >> 7;

		pic = (pic & 0x3F);
        color = (color & 0x3F);

        
        dgp0.code = pic;
        dgp0.color = color;
        dgp0.flipx = flipx;
        dgp0.sx = sx;
        dgp0.sy = sy;
        drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( bsktball )
{
	tilemap_draw(bitmap, &Machine->visible_area, bg_tilemap, 0, 0);
	bsktball_draw_sprites(bitmap);
}
