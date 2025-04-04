/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"


unsigned char *marineb_column_scroll;
int marineb_active_low_flipscreen;
static int palbank;


WRITE8_HANDLER( marineb_palbank0_w )
{
	int new_palbank = (palbank & ~1) | (data & 1);
	set_vh_global_attribute(&palbank, new_palbank);
}

WRITE8_HANDLER( marineb_palbank1_w )
{
	int new_palbank = (palbank & ~2) | ((data << 1) & 2);
	set_vh_global_attribute(&palbank, new_palbank);
}

WRITE8_HANDLER( marineb_flipscreen_x_w )
{
	flip_screen_x_set(data ^ marineb_active_low_flipscreen);
}

WRITE8_HANDLER( marineb_flipscreen_y_w )
{
	flip_screen_y_set(data ^ marineb_active_low_flipscreen);
}


/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
static void draw_chars(mame_bitmap *_tmpbitmap, mame_bitmap *bitmap,
                       int scroll_cols)
{
	int offs;


	if (get_vh_global_attribute_changed())
	{
		memset(dirtybuffer,1,videoram_size);
	}


	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp0={
		_tmpbitmap, 	// dest
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
			int sx,sy,flipx,flipy;


			dirtybuffer[offs] = 0;

			sx = offs % 32;
			sy = offs / 32;

			flipx = colorram[offs] & 0x20;
			flipy = colorram[offs] & 0x10;

			if (flip_screen_y)
			{
				sy = 31 - sy;
				flipy = !flipy;
			}

			if (flip_screen_x)
			{
				sx = 31 - sx;
				flipx = !flipx;
			}

			
			dgp0.code = videoram[offs] | ((colorram[offs] & 0xc0) << 2);
			dgp0.color = (colorram[offs] & 0x0f) + 16 * palbank;
			dgp0.flipx = flipx;
			dgp0.flipy = flipy;
			dgp0.sx = 8*sx;
			dgp0.sy = 8*sy;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scroll[32];


		if (flip_screen_y)
		{
			for (offs = 0;offs < 32 - scroll_cols;offs++)
				scroll[offs] = 0;

			for (;offs < 32;offs++)
				scroll[offs] = marineb_column_scroll[0];
		}
		else
		{
			for (offs = 0;offs < scroll_cols;offs++)
				scroll[offs] = -marineb_column_scroll[0];

			for (;offs < 32;offs++)
				scroll[offs] = 0;
		}
		copyscrollbitmap(bitmap,tmpbitmap,0,0,32,scroll,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}
}


VIDEO_UPDATE( marineb )
{
	int offs;


	draw_chars(tmpbitmap, bitmap, 24);


	/* draw the sprites */
	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
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
	for (offs = 0x0f; offs >= 0; offs--)
	{
		int gfx,sx,sy,code,col,flipx,flipy,offs2;


		if ((offs == 0) || (offs == 2))  continue;  /* no sprites here */


		if (offs < 8)
		{
			offs2 = 0x0018 + offs;
		}
		else
		{
			offs2 = 0x03d8 - 8 + offs;
		}


		code  = videoram[offs2];
		sx    = videoram[offs2 + 0x20];
		sy    = colorram[offs2];
		col   = (colorram[offs2 + 0x20] & 0x0f) + 16 * palbank;
		flipx =   code & 0x02;
		flipy = !(code & 0x01);

		if (offs < 4)
		{
			/* big sprite */
			gfx = 2;
			code = (code >> 4) | ((code & 0x0c) << 2);
		}
		else
		{
			/* small sprite */
			gfx = 1;
			code >>= 2;
		}

		if (!flip_screen_y)
		{
			sy = 256 - Machine->gfx[gfx]->width - sy;
			flipy = !flipy;
		}

		if (flip_screen_x)
		{
			sx++;
		}

		
		dgp1.code = code;
		dgp1.color = col;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = sx;
		dgp1.sy = sy;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( changes )

{ 
struct drawgfxParams dgp3={
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
	int offs,sx,sy,code,col,flipx,flipy;


	draw_chars(tmpbitmap, bitmap, 26);


	/* draw the small sprites */
	
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
	for (offs = 0x05; offs >= 0; offs--)
	{
		int offs2;


		offs2 = 0x001a + offs;

		code  = videoram[offs2];
		sx    = videoram[offs2 + 0x20];
		sy    = colorram[offs2];
		col   = (colorram[offs2 + 0x20] & 0x0f) + 16 * palbank;
		flipx =   code & 0x02;
		flipy = !(code & 0x01);

		if (!flip_screen_y)
		{
			sy = 256 - Machine->gfx[1]->width - sy;
			flipy = !flipy;
		}

		if (flip_screen_x)
		{
			sx++;
		}

		
		dgp2.code = code >> 2;
		dgp2.color = col;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = sx;
		dgp2.sy = sy;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph


	/* draw the big sprite */

	code  = videoram[0x3df];
	sx    = videoram[0x3ff];
	sy    = colorram[0x3df];
	col   = colorram[0x3ff];
	flipx =   code & 0x02;
	flipy = !(code & 0x01);

	if (!flip_screen_y)
	{
		sy = 256 - Machine->gfx[2]->width - sy;
		flipy = !flipy;
	}

	if (flip_screen_x)
	{
		sx++;
	}

	code >>= 4;

	
	dgp3.code = code;
	dgp3.color = col;
	dgp3.flipx = flipx;
	dgp3.flipy = flipy;
	dgp3.sx = sx;
	dgp3.sy = sy;
	drawgfx(&dgp3);

	/* draw again for wrap around */

	
	dgp3.code = code;
	dgp3.color = col;
	dgp3.flipx = flipx;
	dgp3.flipy = flipy;
	dgp3.sx = sx-256;
	dgp3.sy = sy;
	drawgfx(&dgp3);
}
} // end of patch paragraph



VIDEO_UPDATE( springer )
{
	int offs;


	draw_chars(tmpbitmap, bitmap, 0);


	/* draw the sprites */
	
	{ 
	struct drawgfxParams dgp5={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
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
	for (offs = 0x0f; offs >= 0; offs--)
	{
		int gfx,sx,sy,code,col,flipx,flipy,offs2;


		if ((offs == 0) || (offs == 2))  continue;  /* no sprites here */


		offs2 = 0x0010 + offs;


		code  = videoram[offs2];
		sx    = 240 - videoram[offs2 + 0x20];
		sy    = colorram[offs2];
		col   = (colorram[offs2 + 0x20] & 0x0f) + 16 * palbank;
		flipx = !(code & 0x02);
		flipy = !(code & 0x01);

		if (offs < 4)
		{
			/* big sprite */
			sx -= 0x10;
			gfx = 2;
			code = (code >> 4) | ((code & 0x0c) << 2);
		}
		else
		{
			/* small sprite */
			gfx = 1;
			code >>= 2;
		}

		if (!flip_screen_y)
		{
			sy = 256 - Machine->gfx[gfx]->width - sy;
			flipy = !flipy;
		}

		if (!flip_screen_x)
		{
			sx--;
		}

		
		dgp5.code = code;
		dgp5.color = col;
		dgp5.flipx = flipx;
		dgp5.flipy = flipy;
		dgp5.sx = sx;
		dgp5.sy = sy;
		drawgfx(&dgp5);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( hoccer )
{
	int offs;


	draw_chars(tmpbitmap, bitmap, 0);


	/* draw the sprites */
	
	{ 
	struct drawgfxParams dgp6={
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
	for (offs = 0x07; offs >= 0; offs--)
	{
		int sx,sy,code,col,flipx,flipy,offs2;


		offs2 = 0x0018 + offs;


		code  = spriteram[offs2];
		sx    = spriteram[offs2 + 0x20];
		sy    = colorram[offs2];
		col   = colorram[offs2 + 0x20];
		flipx =   code & 0x02;
		flipy = !(code & 0x01);

		if (!flip_screen_y)
		{
			sy = 256 - Machine->gfx[1]->width - sy;
			flipy = !flipy;
		}

		if (flip_screen_x)
		{
			sx = 256 - Machine->gfx[1]->width - sx;
			flipx = !flipx;
		}

		
		dgp6.code = code >> 2;
		dgp6.color = col;
		dgp6.flipx = flipx;
		dgp6.flipy = flipy;
		dgp6.sx = sx;
		dgp6.sy = sy;
		drawgfx(&dgp6);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( hopprobo )
{
	int offs;


	draw_chars(tmpbitmap, bitmap, 0);


	/* draw the sprites */
	
	{ 
	struct drawgfxParams dgp7={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
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
	for (offs = 0x0f; offs >= 0; offs--)
	{
		int gfx,sx,sy,code,col,flipx,flipy,offs2;


		if ((offs == 0) || (offs == 2))  continue;  /* no sprites here */


		offs2 = 0x0010 + offs;


		code  = videoram[offs2];
		sx    = videoram[offs2 + 0x20];
		sy    = colorram[offs2];
		col   = (colorram[offs2 + 0x20] & 0x0f) + 16 * palbank;
		flipx =   code & 0x02;
		flipy = !(code & 0x01);

		if (offs < 4)
		{
			/* big sprite */
			gfx = 2;
			code = (code >> 4) | ((code & 0x0c) << 2);
		}
		else
		{
			/* small sprite */
			gfx = 1;
			code >>= 2;
		}

		if (!flip_screen_y)
		{
			sy = 256 - Machine->gfx[gfx]->width - sy;
			flipy = !flipy;
		}

		if (!flip_screen_x)
		{
			sx--;
		}

		
		dgp7.code = code;
		dgp7.color = col;
		dgp7.flipx = flipx;
		dgp7.flipy = flipy;
		dgp7.sx = sx;
		dgp7.sy = sy;
		drawgfx(&dgp7);
	}
	} // end of patch paragraph

}
