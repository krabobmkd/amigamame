/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"

UINT8 *seicross_row_scroll;

static tilemap *bg_tilemap;

/***************************************************************************

  Convert the color PROMs into a more useable format.

  Seicross has two 32x8 palette PROMs, connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/
PALETTE_INIT( seicross )
{
	int i;


	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = (color_prom[i] >> 6) & 0x01;
		bit1 = (color_prom[i] >> 7) & 0x01;
		b = 0x4f * bit0 + 0xa8 * bit1;

		palette_set_color(i,r,g,b);
	}
}

WRITE8_HANDLER( seicross_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

WRITE8_HANDLER( seicross_colorram_w )
{
	if (colorram[offset] != data)
	{
		/* bit 5 of the address is not used for color memory. There is just */
		/* 512k of memory; every two consecutive rows share the same memory */
		/* region. */
		offset &= 0xffdf;

		colorram[offset] = data;
		colorram[offset + 0x20] = data;

		tilemap_mark_tile_dirty(bg_tilemap, offset);
		tilemap_mark_tile_dirty(bg_tilemap, offset + 0x20);
	}
}

static void get_bg_tile_info(int tile_index)
{
	int code = videoram[tile_index] + ((colorram[tile_index] & 0x10) << 4);
	int color = colorram[tile_index] & 0x0f;
	int flags = ((colorram[tile_index] & 0x40) ? TILE_FLIPX : 0) | ((colorram[tile_index] & 0x80) ? TILE_FLIPY : 0);

	SET_TILE_INFO(0, code, color, flags)
}

VIDEO_START( seicross )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	if ( !bg_tilemap )
		return 1;

	tilemap_set_scroll_cols(bg_tilemap, 32);

	return 0;
}

static void seicross_draw_sprites( mame_bitmap *bitmap )
{
	int offs;

	
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
	for (offs = spriteram_size - 4;offs >= 0;offs -= 4)
	{
		int x = spriteram[offs + 3];
		
		dgp0.code = (spriteram[offs] & 0x3f) + ((spriteram[offs + 1] & 0x10) << 2) + 128;
		dgp0.color = spriteram[offs + 1] & 0x0f;
		dgp0.flipx = spriteram[offs] & 0x40;
		dgp0.flipy = spriteram[offs] & 0x80;
		dgp0.sx = x;
		dgp0.sy = 240-spriteram[offs + 2];
		drawgfx(&dgp0);
		if(x>0xf0)
        {
			dgp0.code = (spriteram[offs] & 0x3f) + ((spriteram[offs + 1] & 0x10) << 2) + 128;
			dgp0.color = spriteram[offs + 1] & 0x0f;
			dgp0.flipx = spriteram[offs] & 0x40;
			dgp0.flipy = spriteram[offs] & 0x80;
			dgp0.sx = x-256;
			dgp0.sy = 240-spriteram[offs + 2];
			drawgfx(&dgp0);
        }
	}
	} // end of patch paragraph


	
	{ 
	struct drawgfxParams dgp2={
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
	for (offs = spriteram_2_size - 4;offs >= 0;offs -= 4)
	{
		int x = spriteram_2[offs + 3];
		
		dgp2.code = (spriteram_2[offs] & 0x3f) + ((spriteram_2[offs + 1] & 0x10) << 2);
		dgp2.color = spriteram_2[offs + 1] & 0x0f;
		dgp2.flipx = spriteram_2[offs] & 0x40;
		dgp2.flipy = spriteram_2[offs] & 0x80;
		dgp2.sx = x;
		dgp2.sy = 240-spriteram_2[offs + 2];
		drawgfx(&dgp2);
		if(x>0xf0)
		{
			dgp2.code = (spriteram_2[offs] & 0x3f) + ((spriteram_2[offs + 1] & 0x10) << 2);
			dgp2.color = spriteram_2[offs + 1] & 0x0f;
			dgp2.flipx = spriteram_2[offs] & 0x40;
			dgp2.flipy = spriteram_2[offs] & 0x80;
			dgp2.sx = x-256;
			dgp2.sy = 240-spriteram_2[offs + 2];
			drawgfx(&dgp2);
        }
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( seicross )
{
	int col;

	for (col = 0; col < 32; col++)
	{
		tilemap_set_scrolly(bg_tilemap, col, seicross_row_scroll[col]);
	}

	tilemap_draw(bitmap, &Machine->visible_area, bg_tilemap, 0, 0);
	seicross_draw_sprites(bitmap);
}
