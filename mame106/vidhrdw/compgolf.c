/****************************************************************************************

 Competition Golf Final Round
 video hardware emulation

****************************************************************************************/

#include "driver.h"

UINT8 *compgolf_bg_ram;
static tilemap *text_tilemap, *background_tilemap;

int compgolf_scrollx_lo = 0, compgolf_scrolly_lo = 0, compgolf_scrollx_hi = 0, compgolf_scrolly_hi = 0;

PALETTE_INIT( compgolf )
{
	int i;

	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		int bit0,bit1,bit2,r,g,b;
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = 0;
		bit1 = (color_prom[i] >> 6) & 0x01;
		bit2 = (color_prom[i] >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(i,r,g,b);
	}
}

WRITE8_HANDLER( compgolf_video_w )
{
	videoram[offset] = data;
	tilemap_mark_tile_dirty(text_tilemap, offset/2);
}

WRITE8_HANDLER( compgolf_back_w )
{
	compgolf_bg_ram[offset] = data;
	tilemap_mark_tile_dirty(background_tilemap, offset/2);
}

static void get_text_info(int cgindex)
{
	cgindex<<=1;
	SET_TILE_INFO(2, videoram[cgindex+1]|(videoram[cgindex]<<8), videoram[cgindex]>>2, 0)
}

static UINT32 back_scan(UINT32 col,UINT32 row,UINT32 num_cols,UINT32 num_rows)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x0f) + ((row & 0x0f) << 4) + ((col & 0x10) << 4) + ((row & 0x10) << 5);
}

static void get_back_info(int cgindex)
{
	int attr = compgolf_bg_ram[cgindex*2];
	int code = compgolf_bg_ram[cgindex*2+1] + ((attr & 1) << 8);
	int color = (attr & 0x3e) >> 1;

	SET_TILE_INFO(1,code,color,0)
}

VIDEO_START( compgolf )
{
	background_tilemap = tilemap_create(get_back_info,back_scan,TILEMAP_OPAQUE, 16, 16, 32, 32);
	text_tilemap = tilemap_create(get_text_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8, 32, 32);

	if (!text_tilemap || !background_tilemap)
		return 1;

	tilemap_set_transparent_pen(text_tilemap,0);

	return 0;
}

/*
preliminary sprite list:
       0        1        2        3
xx------ xxxxxxxx -------- -------- sprite code
---x---- -------- -------- -------- Double Height
----x--- -------- -------- -------- Color,all of it?
-------- -------- xxxxxxxx -------- Y pos
-------- -------- -------- xxxxxxxx X pos
-----x-- -------- -------- -------- Flip X
-------- -------- -------- -------- Flip Y(used?)
*/
static void draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int offs,fx,fy,x,y,color,sprite;

	
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
	for (offs = 0;offs < 0x60;offs += 4)
	{
		sprite = spriteram[offs+1] + (((spriteram[offs] & 0xc0) >> 6)*0x100);
		x = 240 - spriteram[offs+3];
		y = spriteram[offs+2];
		color = (spriteram[offs] & 8)>>3;
		fx = spriteram[offs]&4;
		fy = 0; /* ? */

		
		dgp0.code = sprite;
		dgp0.color = color;
		dgp0.flipx = fx;
		dgp0.flipy = fy;
		dgp0.sx = x;
		dgp0.sy = y;
		drawgfx(&dgp0);

		/* Double Height */
		if(spriteram[offs] & 0x10)
		{
			
			dgp0.code = sprite+1;
			dgp0.color = color;
			dgp0.flipx = fx;
			dgp0.flipy = fy;
			dgp0.sx = x;
			dgp0.sy = y+16;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( compgolf )
{
	int scrollx = compgolf_scrollx_hi + compgolf_scrollx_lo;
	int scrolly = compgolf_scrolly_hi + compgolf_scrolly_lo;

	tilemap_set_scrollx(background_tilemap,0,scrollx);
	tilemap_set_scrolly(background_tilemap,0,scrolly);

	tilemap_draw(bitmap, cliprect, background_tilemap, 0, 0);
	tilemap_draw(bitmap, cliprect, text_tilemap, 0, 0);
	draw_sprites(bitmap,cliprect);
}
