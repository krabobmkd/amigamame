/***************************************************************************

    Venture Line Super Rider driver

***************************************************************************/

#include "driver.h"
#include "suprridr.h"


UINT8 *suprridr_bgram;
UINT8 *suprridr_fgram;

static tilemap *fg_tilemap;
static tilemap *bg_tilemap;
static tilemap *bg_tilemap_noscroll;
static UINT8 flipx, flipy;



/*************************************
 *
 *  Tilemap callbacks
 *
 *************************************/

static void get_tile_info(int tile_index)
{
	UINT8 code = suprridr_bgram[tile_index];
	SET_TILE_INFO(0, code, 0, 0);
}


static void get_tile_info2(int tile_index)
{
	UINT8 code = suprridr_fgram[tile_index];
	SET_TILE_INFO(1, code, 0, 0);
}



/*************************************
 *
 *  Video startup
 *
 *************************************/

VIDEO_START( suprridr )
{
	fg_tilemap          = tilemap_create(get_tile_info2, tilemap_scan_rows, TILEMAP_TRANSPARENT, 8,8, 32,32);
	bg_tilemap          = tilemap_create(get_tile_info,  tilemap_scan_rows, TILEMAP_OPAQUE,      8,8, 32,32);
	bg_tilemap_noscroll = tilemap_create(get_tile_info,  tilemap_scan_rows, TILEMAP_OPAQUE,      8,8, 32,32);
	if (!fg_tilemap || !bg_tilemap || !bg_tilemap_noscroll)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap, 0);
	return 0;
}



/*************************************
 *
 *  Color PROM decoding
 *
 *************************************/

PALETTE_INIT( suprridr )
{
	int i;

	for (i = 0; i < 96; i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = (*color_prom >> 0) & 0x01;
		bit1 = (*color_prom >> 1) & 0x01;
		bit2 = (*color_prom >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (*color_prom >> 3) & 0x01;
		bit1 = (*color_prom >> 4) & 0x01;
		bit2 = (*color_prom >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = (*color_prom >> 6) & 0x01;
		bit1 = (*color_prom >> 7) & 0x01;
		b = 0x4f * bit0 + 0xa8 * bit1;

		palette_set_color(i,r,g,b);
		color_prom++;
	}
}



/*************************************
 *
 *  Screen flip/scroll registers
 *
 *************************************/

WRITE8_HANDLER( suprridr_flipx_w )
{
	flipx = data & 1;
	tilemap_set_flip(ALL_TILEMAPS, (flipx ? TILEMAP_FLIPX : 0) | (flipy ? TILEMAP_FLIPY : 0));
}


WRITE8_HANDLER( suprridr_flipy_w )
{
	flipy = data & 1;
	tilemap_set_flip(ALL_TILEMAPS, (flipx ? TILEMAP_FLIPX : 0) | (flipy ? TILEMAP_FLIPY : 0));
}


WRITE8_HANDLER( suprridr_fgdisable_w )
{
	tilemap_set_enable(fg_tilemap, ~data & 1);
}


WRITE8_HANDLER( suprridr_fgscrolly_w )
{
	tilemap_set_scrolly(fg_tilemap, 0, data);
}


WRITE8_HANDLER( suprridr_bgscrolly_w )
{
	tilemap_set_scrolly(bg_tilemap, 0, data);
}



/*************************************
 *
 *  Video RAM writes
 *
 *************************************/

WRITE8_HANDLER( suprridr_bgram_w )
{
	suprridr_bgram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset);
	tilemap_mark_tile_dirty(bg_tilemap_noscroll, offset);
}


WRITE8_HANDLER( suprridr_fgram_w )
{
	suprridr_fgram[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap, offset);
}



/*************************************
 *
 *  Video refresh
 *
 *************************************/

VIDEO_UPDATE( suprridr )
{
	rectangle subclip;
	int i;

	/* render left 4 columns with no scroll */
	subclip = Machine->visible_area;
	subclip.max_x = subclip.min_x + (flipx ? 1*8 : 4*8) - 1;
	sect_rect(&subclip, cliprect);
	tilemap_draw(bitmap, &subclip, bg_tilemap_noscroll, 0, 0);

	/* render right 1 column with no scroll */
	subclip = Machine->visible_area;
	subclip.min_x = subclip.max_x - (flipx ? 4*8 : 1*8) + 1;
	sect_rect(&subclip, cliprect);
	tilemap_draw(bitmap, &subclip, bg_tilemap_noscroll, 0, 0);

	/* render the middle columns normally */
	subclip = Machine->visible_area;
	subclip.min_x += flipx ? 1*8 : 4*8;
	subclip.max_x -= flipx ? 4*8 : 1*8;
	sect_rect(&subclip, cliprect);
	tilemap_draw(bitmap, &subclip, bg_tilemap, 0, 0);

	/* render the top layer */
	tilemap_draw(bitmap, cliprect, fg_tilemap, 0, 0);

	/* draw the sprites */
	
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
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < 48; i++)
	{
		int code = (spriteram[i*4+1] & 0x3f) | ((spriteram[i*4+2] >> 1) & 0x40);
		int color = spriteram[i*4+2] & 0x7f;
		int fx = spriteram[i*4+1] & 0x40;
		int fy = spriteram[i*4+1] & 0x80;
		int x = spriteram[i*4+3];
		int y = 240 - spriteram[i*4+0];

		if (flipx)
		{
			fx = !fx;
			x = 240 - x;
		}
		if (flipy)
		{
			fy = !fy;
			y = 240 - y;
		}
		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = fx;
		dgp0.flipy = fy;
		dgp0.sx = x;
		dgp0.sy = y;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}
