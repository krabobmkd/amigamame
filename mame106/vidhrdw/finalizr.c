/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"



unsigned char *finalizr_scroll;
unsigned char *finalizr_videoram2,*finalizr_colorram2;
static int spriterambank,charbank;



PALETTE_INIT( finalizr )
{
	int i;
	#define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
	#define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs])


	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		int bit0,bit1,bit2,bit3,r,g,b;


		/* red component */
		bit0 = (color_prom[0] >> 0) & 0x01;
		bit1 = (color_prom[0] >> 1) & 0x01;
		bit2 = (color_prom[0] >> 2) & 0x01;
		bit3 = (color_prom[0] >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		/* green component */
		bit0 = (color_prom[0] >> 4) & 0x01;
		bit1 = (color_prom[0] >> 5) & 0x01;
		bit2 = (color_prom[0] >> 6) & 0x01;
		bit3 = (color_prom[0] >> 7) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		/* blue component */
		bit0 = (color_prom[Machine->drv->total_colors] >> 0) & 0x01;
		bit1 = (color_prom[Machine->drv->total_colors] >> 1) & 0x01;
		bit2 = (color_prom[Machine->drv->total_colors] >> 2) & 0x01;
		bit3 = (color_prom[Machine->drv->total_colors] >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		palette_set_color(i,r,g,b);
		color_prom++;
	}

	color_prom += Machine->drv->total_colors;
	/* color_prom now points to the beginning of the lookup tables */

	for (i = 0;i < TOTAL_COLORS(1);i++)
	{
		if (*color_prom & 0x0f) COLOR(1,i) = *color_prom & 0x0f;
		else COLOR(1,i) = 0;
		color_prom++;
	}
	for (i = 0;i < TOTAL_COLORS(0);i++)
	{
		COLOR(0,i) = (*(color_prom++) & 0x0f) + 0x10;
	}
}

VIDEO_START( finalizr )
{
	dirtybuffer = 0;
	tmpbitmap = 0;

	dirtybuffer = auto_malloc(videoram_size);
	memset(dirtybuffer,1,videoram_size);

	if ((tmpbitmap = auto_bitmap_alloc(256,256)) == 0)
		return 1;

	return 0;
}



WRITE8_HANDLER( finalizr_videoctrl_w )
{
	if (charbank != (data & 3))
	{
		charbank = data & 3;
		memset(dirtybuffer,1,videoram_size);
	}

	spriterambank = data & 8;

	/* other bits unknown */
}



/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( finalizr )
{
	int offs;


	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp0={
		tmpbitmap, 	// dest
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
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		if (dirtybuffer[offs])
		{
			int sx,sy;


			dirtybuffer[offs] = 0;

			sx = offs % 32;
			sy = offs / 32;

			
			dgp0.code = videoram[offs] + ((colorram[offs] & 0xc0) << 2) + (charbank<<10);
			dgp0.color = (colorram[offs] & 0x0f);
			dgp0.flipx = colorram[offs] & 0x10;
			dgp0.flipy = colorram[offs] & 0x20;
			dgp0.sx = 8*sx;
			dgp0.sy = 8*sy;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scroll;


		scroll = -*finalizr_scroll + 16;

		copyscrollbitmap(bitmap,tmpbitmap,1,&scroll,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}


	/* Draw the sprites. */
	{
		unsigned char *sr;


		if (spriterambank != 0)
			sr = spriteram_2;
		else sr = spriteram;

		for (offs = 0;offs < spriteram_size;offs += 5)
		{
			int sx,sy,flipx,flipy,code,color;


			sx = 16 + sr[offs+3] - ((sr[offs+4] & 0x01) << 8);
			sy = sr[offs+2];
			flipx = sr[offs+4] & 0x20;
			flipy = sr[offs+4] & 0x40;
			code = sr[offs] + ((sr[offs+1] & 0x0f) << 8);
			color = ((sr[offs+1] & 0xf0)>>4);

//          (sr[offs+4] & 0x02) is used, meaning unknown

			switch (sr[offs+4] & 0x1c)
			
{ 
struct drawgfxParams dgp1={
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
struct drawgfxParams dgp6={
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
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
				case 0x10:	/* 32x32? */
				case 0x14:	/* ? */
				case 0x18:	/* ? */
				case 0x1c:	/* ? */
					
					dgp1.code = code;
					dgp1.color = color;
					dgp1.flipx = flipx;
					dgp1.flipy = flipy;
					dgp1.sx = flipx?sx+16:sx;
					dgp1.sy = flipy?sy+16:sy;
					drawgfx(&dgp1);
					
					dgp1.code = code + 1;
					dgp1.color = color;
					dgp1.flipx = flipx;
					dgp1.flipy = flipy;
					dgp1.sx = flipx?sx:sx+16;
					dgp1.sy = flipy?sy+16:sy;
					drawgfx(&dgp1);
					
					dgp1.code = code + 2;
					dgp1.color = color;
					dgp1.flipx = flipx;
					dgp1.flipy = flipy;
					dgp1.sx = flipx?sx+16:sx;
					dgp1.sy = flipy?sy:sy+16;
					drawgfx(&dgp1);
					
					dgp1.code = code + 3;
					dgp1.color = color;
					dgp1.flipx = flipx;
					dgp1.flipy = flipy;
					dgp1.sx = flipx?sx:sx+16;
					dgp1.sy = flipy?sy:sy+16;
					drawgfx(&dgp1);
					break;

				case 0x00:	/* 16x16 */
					
					dgp1.code = code;
					dgp1.color = color;
					dgp1.flipx = flipx;
					dgp1.flipy = flipy;
					dgp1.sx = sx;
					dgp1.sy = sy;
					drawgfx(&dgp1);
					break;

				case 0x04:	/* 16x8 */
					code = ((code & 0x3ff) << 2) | ((code & 0xc00) >> 10);
					
					dgp6.code = code & ~1;
					dgp6.color = color;
					dgp6.flipx = flipx;
					dgp6.flipy = flipy;
					dgp6.sx = flipx?sx+8:sx;
					dgp6.sy = sy;
					drawgfx(&dgp6);
					
					dgp6.code = code | 1;
					dgp6.color = color;
					dgp6.flipx = flipx;
					dgp6.flipy = flipy;
					dgp6.sx = flipx?sx:sx+8;
					dgp6.sy = sy;
					drawgfx(&dgp6);
					break;

				case 0x08:	/* 8x16 */
					code = ((code & 0x3ff) << 2) | ((code & 0xc00) >> 10);
					
					dgp6.code = code & ~2;
					dgp6.color = color;
					dgp6.flipx = flipx;
					dgp6.flipy = flipy;
					dgp6.sx = sx;
					dgp6.sy = flipy?sy+8:sy;
					drawgfx(&dgp6);
					
					dgp6.code = code | 2;
					dgp6.color = color;
					dgp6.flipx = flipx;
					dgp6.flipy = flipy;
					dgp6.sx = sx;
					dgp6.sy = flipy?sy:sy+8;
					drawgfx(&dgp6);
					break;

				case 0x0c:	/* 8x8 */
					code = ((code & 0x3ff) << 2) | ((code & 0xc00) >> 10);
					
					dgp6.code = code;
					dgp6.color = color;
					dgp6.flipx = flipx;
					dgp6.flipy = flipy;
					dgp6.sx = sx;
					dgp6.sy = sy;
					drawgfx(&dgp6);
					break;
			}
} // end of patch paragraph

		}
	}

	
	{ 
	struct drawgfxParams dgp11={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		int sx,sy;


		sx = offs % 32;
		if (sx < 6)
		{
			if (sx >= 3) sx += 30;
			sy = offs / 32;

			
			dgp11.code = finalizr_videoram2[offs] + ((finalizr_colorram2[offs] & 0xc0) << 2);
			dgp11.color = (finalizr_colorram2[offs] & 0x0f);
			dgp11.flipx = finalizr_colorram2[offs] & 0x10;
			dgp11.flipy = finalizr_colorram2[offs] & 0x20;
			dgp11.sx = 8*sx;
			dgp11.sy = 8*sy;
			drawgfx(&dgp11);
		}
	}
	} // end of patch paragraph

}
