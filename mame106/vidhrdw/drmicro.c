/*******************************************************************************

Dr. Micro (c) 1983 Sanritsu

Video hardware
        driver by Uki

*******************************************************************************/

#include "driver.h"

static int flipscreen;

static UINT8 *drmicro_videoram;
static tilemap *drmicro_bg1;
static tilemap *drmicro_bg2;

/****************************************************************************/

void drmicro_flip_w( int flip )
{
	flipscreen = flip ? 1 : 0;
	flip_screen_set(flip);
}

WRITE8_HANDLER( drmicro_videoram_w )
{
	drmicro_videoram[offset] = data;

	if (offset<0x800)
		tilemap_mark_tile_dirty(drmicro_bg2,(offset & 0x3ff));
	else
		tilemap_mark_tile_dirty(drmicro_bg1,(offset & 0x3ff));
}

READ8_HANDLER( drmicro_videoram_r )
{
	return drmicro_videoram[offset];
}

/****************************************************************************/

static void get_bg1_tile_info(int tile_index)
{
	int code,col,flags;

	code = drmicro_videoram[tile_index + 0x0800];
	col  = drmicro_videoram[tile_index + 0x0c00];

	code += (col & 0xc0) << 2;
	flags = ((col & 0x20) ? TILEMAP_FLIPY : 0) | ((col & 0x10) ? TILEMAP_FLIPX : 0);
	col &= 0x0f;

	SET_TILE_INFO( 0, code, col, flags)
}

static void get_bg2_tile_info(int tile_index)
{
	int code,col,flags;

	code = drmicro_videoram[tile_index + 0x0000];
	col  = drmicro_videoram[tile_index + 0x0400];

	code += (col & 0xc0) << 2;
	flags = ((col & 0x20) ? TILEMAP_FLIPY : 0) | ((col & 0x10) ? TILEMAP_FLIPX : 0);
	col &= 0x0f;

	SET_TILE_INFO( 1, code, col, flags)
}

/****************************************************************************/

PALETTE_INIT( drmicro )
{
	int i;

	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		bit0 = (*color_prom >> 0) & 0x01;
		bit1 = (*color_prom >> 1) & 0x01;
		bit2 = (*color_prom >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = (*color_prom >> 3) & 0x01;
		bit1 = (*color_prom >> 4) & 0x01;
		bit2 = (*color_prom >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = 0;
		bit1 = (*color_prom >> 6) & 0x01;
		bit2 = (*color_prom >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(i,r,g,b);
		color_prom++;
	}

	for (i=0; i<Machine->drv->color_table_len; i++)
		colortable[i] = color_prom[i] & 0x0f;
}

VIDEO_START( drmicro)
{
	drmicro_videoram = auto_malloc(0x1000);

	drmicro_bg1 = tilemap_create(get_bg1_tile_info, tilemap_scan_rows,TILEMAP_OPAQUE, 8,8,32,32);
	drmicro_bg2 = tilemap_create(get_bg2_tile_info, tilemap_scan_rows,TILEMAP_TRANSPARENT, 8,8,32,32);

	if (!drmicro_bg1 || !drmicro_bg2)
		return 1;

	tilemap_set_transparent_pen(drmicro_bg2,0);

	return 0;
}

VIDEO_UPDATE( drmicro )
{
	int offs,adr,g;
	int chr,col,attr;
	int x,y,fx,fy;

	tilemap_draw(bitmap, cliprect, drmicro_bg1, 0, 0);
	tilemap_draw(bitmap, cliprect, drmicro_bg2, 0, 0);

	/* draw sprites */

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[3-g], 	// gfx
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
	for (g=0;g<2;g++)
	{
		adr = 0x800*g;

		for (offs=0x00; offs<0x20; offs +=4)
		{
			x =    drmicro_videoram[offs + adr + 3];
			y =    drmicro_videoram[offs + adr + 0];
			attr = drmicro_videoram[offs + adr + 2];
			chr =  drmicro_videoram[offs + adr + 1];

			fx = (chr & 0x01) ^ flipscreen;
			fy = ((chr & 0x02) >> 1) ^ flipscreen;

			chr =  (chr >> 2) | (attr & 0xc0);

			col = (attr & 0x0f) + 0x00;

			if (!flipscreen)
				y = (240-y) & 0xff;
			else
				x = (240-x) & 0xff;

			
			dgp0.code = chr;
			dgp0.color = col;
			dgp0.flipx = fx;
			dgp0.flipy = fy;
			dgp0.sx = x;
			dgp0.sy = y;
			drawgfx(&dgp0);

			if (x>240)
			
{ 
struct drawgfxParams dgp1={
	bitmap, 	// dest
	Machine->gfx[3-g], 	// gfx
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
{
				
				dgp1.code = chr;
				dgp1.color = col;
				dgp1.flipx = fx;
				dgp1.flipy = fy;
				dgp1.sx = x-256;
				dgp1.sy = y;
				drawgfx(&dgp1);
			}
} // end of patch paragraph

		}
	}
	} // end of patch paragraph

}

