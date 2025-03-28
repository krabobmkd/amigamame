/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"


static int flipscreen;


/***************************************************************************

  Convert the color PROMs into a more useable format.

***************************************************************************/

PALETTE_INIT( wiping )
{
	int i;
	#define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
	#define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + (offs)])


	for (i = 0;i < Machine->drv->total_colors;i++)
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

	/* color_prom now points to the beginning of the lookup table */

	/* chars use colors 0-15 */
	for (i = 0;i < TOTAL_COLORS(0);i++)
		COLOR(0,i ^ 3) = *(color_prom++) & 0x0f;

	/* sprites use colors 16-31 */
	for (i = 0;i < TOTAL_COLORS(1);i++)
		COLOR(1,i ^ 3) = (*(color_prom++) & 0x0f) + 0x10;
}



WRITE8_HANDLER( wiping_flipscreen_w )
{
	if (flipscreen != (data & 1))
	{
		flipscreen = (data & 1);
		memset(dirtybuffer,1,videoram_size);
	}
}



/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( wiping )
{
	int offs;

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = videoram_size - 1; offs > 0; offs--)
	{
		if (dirtybuffer[offs])
		{
			int mx,my,sx,sy;

			dirtybuffer[offs] = 0;

	        mx = offs % 32;
			my = offs / 32;

			if (my < 2)
			{
				sx = my + 34;
				sy = mx - 2;
			}
			else if (my >= 30)
			{
				sx = my - 30;
				sy = mx - 2;
			}
			else
			{
				sx = mx + 2;
				sy = my - 2;
			}

			if (flipscreen)
			{
				sx = 35 - sx;
				sy = 27 - sy;
			}

			
			dgp0.code = videoram[offs];
			dgp0.color = colorram[offs] & 0x3f;
			dgp0.flipx = flipscreen;
			dgp0.flipy = flipscreen;
			dgp0.sx = sx*8;
			dgp0.sy = sy*8;
			drawgfx(&dgp0);
        	}
	}
	} // end of patch paragraph

	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);

	/* Note, we're counting up on purpose ! */
	/* This way the vacuum cleaner is always on top */
	
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
		TRANSPARENCY_COLOR, 	// transparency
		0x1f, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0x0; offs < 128; offs += 2) {
		int sx,sy,flipx,flipy,otherbank;

		sx = spriteram[offs+0x100+1] + ((spriteram[offs+0x81] & 0x01) << 8) - 40;
		sy = 224 - spriteram[offs+0x100];

		otherbank = spriteram[offs+0x80] & 0x01;

		flipy = spriteram[offs] & 0x40;
		flipx = spriteram[offs] & 0x80;

		if (flipscreen)
		{
			sy = 208 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp1.code = (spriteram[offs] & 0x3f) + 64 * otherbank;
		dgp1.color = spriteram[offs+1] & 0x3f;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = sx;
		dgp1.sy = sy;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph


	/* redraw high priority chars */
	
	{ 
	struct drawgfxParams dgp2={
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
	for (offs = videoram_size - 1; offs > 0; offs--)
	{
		if (colorram[offs] & 0x80)
		{
			int mx,my,sx,sy;

	        mx = offs % 32;
			my = offs / 32;

			if (my < 2)
			{
				sx = my + 34;
				sy = mx - 2;
			}
			else if (my >= 30)
			{
				sx = my - 30;
				sy = mx - 2;
			}
			else
			{
				sx = mx + 2;
				sy = my - 2;
			}

			if (flipscreen)
			{
				sx = 35 - sx;
				sy = 27 - sy;
			}

			
			dgp2.code = videoram[offs];
			dgp2.color = colorram[offs] & 0x3f;
			dgp2.flipx = flipscreen;
			dgp2.flipy = flipscreen;
			dgp2.sx = sx*8;
			dgp2.sy = sy*8;
			drawgfx(&dgp2);
        	}
	}
	} // end of patch paragraph



#if 0
{
	int i,j;
	extern unsigned char *wiping_soundregs;

	for (i = 0;i < 8;i++)
	{
		for (j = 0;j < 8;j++)
		{
			char buf[40];
			sprintf(buf,"%01x",wiping_soundregs[i*8+j]&0xf);
			ui_draw_text(buf,j*10,i*8);
		}
	}

	for (i = 0;i < 8;i++)
	{
		for (j = 0;j < 8;j++)
		{
			char buf[40];
			sprintf(buf,"%01x",wiping_soundregs[0x2000+i*8+j]>>4);
			ui_draw_text(buf,j*10,80+i*8);
		}
	}
}
#endif
}
