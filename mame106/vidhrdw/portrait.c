/***************************************************************************

  Portraits
  video hardware emulation

***************************************************************************/

#include "driver.h"

int portrait_scroll;
UINT8 *portrait_bgvideoram, *portrait_fgvideoram;
static tilemap *foreground, *background;

WRITE8_HANDLER( portrait_bgvideo_write )
{
	if (portrait_bgvideoram[offset] != data)
	{
		tilemap_mark_tile_dirty(background,offset/2);
		portrait_bgvideoram[offset] = data;
	}
}

WRITE8_HANDLER( portrait_fgvideo_write )
{
	if (portrait_fgvideoram[offset] != data)
	{
		tilemap_mark_tile_dirty(foreground,offset/2);
		portrait_fgvideoram[offset] = data;
	}
}

static void get_tile_info( const UINT8 *source, int tile_index )
{
	int attr    = source[tile_index*2+0];
	int tilenum = source[tile_index*2+1];
	int flags   = 0;
	int color   = 0;

	/* or 0x10 ? */
	if( attr & 0x20 ) flags = TILE_FLIPY;

	switch( attr & 7 )
	{
		case 1:
			tilenum += 0x200;
			break;
		case 3:
			tilenum += 0x300;
			break;
		case 5:
			tilenum += 0x100;
			break;
	}

	SET_TILE_INFO( 0, tilenum, color, flags )
}

static void get_bg_tile_info(int tile_index)
{
	get_tile_info( portrait_bgvideoram, tile_index );
}

static void get_fg_tile_info(int tile_index)
{
	get_tile_info( portrait_fgvideoram, tile_index );
}

VIDEO_START( portrait )
{
	background = tilemap_create( get_bg_tile_info, tilemap_scan_rows, TILEMAP_OPAQUE,      16, 16, 32, 32 );
	foreground = tilemap_create( get_fg_tile_info, tilemap_scan_rows, TILEMAP_TRANSPARENT, 16, 16, 32, 32 );

	if( background && foreground )
	{
		tilemap_set_transparent_pen( foreground, 0 );
		return 0;
	}

	return 1;
}

/* probably not right */
PALETTE_INIT( portrait )
{
	int i,bit1,bit2,r,g,b;

	for (i = 0;i < 0x800;i++)
	{
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 0) & 0x01;
		r = 0x47 * bit1 + 0x97 * bit2;
		bit1 = (color_prom[i] >> 3) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		g = 0x47 * bit1 + 0x97 * bit2;
		bit1 = (color_prom[i] >> 5) & 0x01;
		bit2 = (color_prom[i] >> 4) & 0x01;
		b = 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(i,r,g,b);
	}
}

static void draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect)
{
	UINT8 *source = spriteram;
	UINT8 *finish = source + 0x200;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
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
	while( source < finish )
	{
		int sy      = source[0];
		int sx      = source[1];
		int attr    = source[2];
			/* xx-x---- ?
             * --x----- flipy
             * ----x--- msb source[0]
             * -----x-- msb source[1]
             */
		int tilenum = source[3];
		int color = 0;
		int fy = attr & 0x20;

		if(attr & 0x04) sx |= 0x100;

		if(attr & 0x08) sy |= 0x100;

		sx += (source - spriteram) - 8;
		sx &= 0x1ff;

		sy = (512 - 64) - sy;

		/* wrong! */
		switch( attr & 0xc0 )
		{
		case 0:
			break;

		case 0x40:
			sy -= portrait_scroll;
			break;

		case 0x80:
			sy -= portrait_scroll;
			break;

		case 0xc0:
			break;

		}

		
		dgp0.code = tilenum;
		dgp0.color = color;
		dgp0.flipy = fy;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);

		source += 0x10;
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( portrait )
{
	rectangle cliprect_scroll, cliprect_no_scroll;

	cliprect_scroll = cliprect_no_scroll = *cliprect;

	cliprect_no_scroll.min_x = cliprect_no_scroll.max_x - 111;
	cliprect_scroll.max_x    = cliprect_scroll.min_x    + 319;

	tilemap_set_scrolly(background, 0, 0);
	tilemap_set_scrolly(foreground, 0, 0);
	tilemap_draw(bitmap, &cliprect_no_scroll, background, 0, 0);
	tilemap_draw(bitmap, &cliprect_no_scroll, foreground, 0, 0);

	tilemap_set_scrolly(background, 0, portrait_scroll);
	tilemap_set_scrolly(foreground, 0, portrait_scroll);
	tilemap_draw(bitmap, &cliprect_scroll, background, 0, 0);
	tilemap_draw(bitmap, &cliprect_scroll, foreground, 0, 0);

	draw_sprites(bitmap,cliprect);
}
