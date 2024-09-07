#include "driver.h"

#define COLORTABLE_START(gfxn,color)	Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + \
					color * Machine->gfx[gfxn]->color_granularity
#define GFX_COLOR_CODES(gfxn) 		Machine->gfx[gfxn]->total_colors
#define GFX_ELEM_COLORS(gfxn) 		Machine->gfx[gfxn]->color_granularity

unsigned char 	*ninjakd2_scrolly_ram;
unsigned char 	*ninjakd2_scrollx_ram;
unsigned char 	*ninjakd2_bgenable_ram;
unsigned char 	*ninjakd2_spoverdraw_ram;
unsigned char 	*ninjakd2_background_videoram;
size_t ninjakd2_backgroundram_size;
unsigned char 	*ninjakd2_foreground_videoram;
size_t ninjakd2_foregroundram_size;

static mame_bitmap *bitmap_bg;
static mame_bitmap *bitmap_sp;

static unsigned char 	 *bg_dirtybuffer;
static int 		 bg_enable = 1;
static int 		 sp_overdraw = 0;

VIDEO_START( ninjakd2 )
{
	bg_dirtybuffer = auto_malloc(1024);

	if ((bitmap_bg = auto_bitmap_alloc(Machine->drv->screen_width*2,Machine->drv->screen_height*2)) == 0)
		return 1;

	if ((bitmap_sp = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)
		return 1;

	memset(bg_dirtybuffer,1,1024);

	return 0;
}

WRITE8_HANDLER( ninjakd2_bgvideoram_w )
{
	if (ninjakd2_background_videoram[offset] != data)
	{
		bg_dirtybuffer[offset >> 1] = 1;
		ninjakd2_background_videoram[offset] = data;
	}
}

WRITE8_HANDLER( ninjakd2_fgvideoram_w )
{
	if (ninjakd2_foreground_videoram[offset] != data)
		ninjakd2_foreground_videoram[offset] = data;
}

WRITE8_HANDLER( ninjakd2_background_enable_w )
{
	if (bg_enable!=data)
	{
		ninjakd2_bgenable_ram[offset] = data;
		bg_enable = data;
		if (bg_enable)
		 memset(bg_dirtybuffer, 1, ninjakd2_backgroundram_size / 2);
		else
		 fillbitmap(bitmap_bg, Machine->pens[0],0);
	}
}

WRITE8_HANDLER( ninjakd2_sprite_overdraw_w )
{
	if (sp_overdraw!=data)
	{
		ninjakd2_spoverdraw_ram[offset] = data;
		fillbitmap(bitmap_sp,15,&Machine->visible_area);
		sp_overdraw = data;
	}
}

void ninjakd2_draw_foreground(mame_bitmap *bitmap)
{
	int offs;

	/* Draw the foreground text */

	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0 ;offs < ninjakd2_foregroundram_size / 2; offs++)
	{
		int sx,sy,tile,palette,flipx,flipy,lo,hi;

		if (ninjakd2_foreground_videoram[offs*2] | ninjakd2_foreground_videoram[offs*2+1])
		{
			sx = (offs % 32) << 3;
			sy = (offs >> 5) << 3;

			lo = ninjakd2_foreground_videoram[offs*2];
			hi = ninjakd2_foreground_videoram[offs*2+1];
			tile = ((hi & 0xc0) << 2) | lo;
			flipx = hi & 0x10;
			flipy = hi & 0x20;
			palette = hi & 0x0f;

			
			dgp0.code = tile;
			dgp0.color = palette;
			dgp0.flipx = flipx;
			dgp0.flipy = flipy;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}

	}
	} // end of patch paragraph

}


void ninjakd2_draw_background(mame_bitmap *bitmap)
{
	int offs;

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */

	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		0, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0 ;offs < ninjakd2_backgroundram_size / 2; offs++)
	{
		int sx,sy,tile,palette,flipx,flipy,lo,hi;

		if (bg_dirtybuffer[offs])
		{
			sx = (offs % 32) << 4;
			sy = (offs >> 5) << 4;

			bg_dirtybuffer[offs] = 0;

			lo = ninjakd2_background_videoram[offs*2];
			hi = ninjakd2_background_videoram[offs*2+1];
			tile = ((hi & 0xc0) << 2) | lo;
			flipx = hi & 0x10;
			flipy = hi & 0x20;
			palette = hi & 0x0f;
			
			dgp1.code = tile;
			dgp1.color = palette;
			dgp1.flipx = flipx;
			dgp1.flipy = flipy;
			dgp1.sx = sx;
			dgp1.sy = sy;
			drawgfx(&dgp1);
		}

	}
	} // end of patch paragraph

}

void ninjakd2_draw_sprites(mame_bitmap *bitmap)
{
	int offs;

	/* Draw the sprites */

	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 11 ;offs < spriteram_size; offs+=16)
	{
		int sx,sy,tile,palette,flipx,flipy;

		if (spriteram[offs+2] & 2)
		{
			sx = spriteram[offs+1];
			sy = spriteram[offs];
			if (spriteram[offs+2] & 1) sx-=256;
			tile = spriteram[offs+3]+((spriteram[offs+2] & 0xc0)<<2);
			flipx = spriteram[offs+2] & 0x10;
			flipy = spriteram[offs+2] & 0x20;
			palette = spriteram[offs+4] & 0x0f;
			
			dgp2.code = tile;
			dgp2.color = palette;
			dgp2.flipx = flipx;
			dgp2.flipy = flipy;
			dgp2.sx = sx;
			dgp2.sy = sy;
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph

}


/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( ninjakd2 )
{
	int scrollx,scrolly;

	if (bg_enable)
		ninjakd2_draw_background(bitmap_bg);

	scrollx = -((ninjakd2_scrollx_ram[0]+ninjakd2_scrollx_ram[1]*256) & 0x1FF);
	scrolly = -((ninjakd2_scrolly_ram[0]+ninjakd2_scrolly_ram[1]*256) & 0x1FF);

	if (sp_overdraw)	/* overdraw sprite mode */
	{
		ninjakd2_draw_sprites(bitmap_sp);
		ninjakd2_draw_foreground(bitmap_sp);
		copyscrollbitmap(bitmap,bitmap_bg,1,&scrollx,1,&scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
		copybitmap(bitmap,bitmap_sp,0,0,0,0,&Machine->visible_area,TRANSPARENCY_PEN, 15);
	}
	else 			/* normal sprite mode */
	{
		copyscrollbitmap(bitmap,bitmap_bg,1,&scrollx,1,&scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
		ninjakd2_draw_sprites(bitmap);
		ninjakd2_draw_foreground(bitmap);
	}

}
