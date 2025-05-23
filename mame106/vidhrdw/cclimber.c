/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "includes/cclimber.h"


unsigned char *cclimber_bsvideoram;
size_t cclimber_bsvideoram_size;
unsigned char *cclimber_bigspriteram;
unsigned char *cclimber_column_scroll;
static int palettebank;
static int sidepanel_enabled;


/***************************************************************************

  Convert the color PROMs into a more useable format.

  Crazy Climber has three 32x8 palette PROMs.
  The palette PROMs are connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/
PALETTE_INIT( cclimber )
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
		bit0 = 0;
		bit1 = (*color_prom >> 6) & 0x01;
		bit2 = (*color_prom >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(i,r,g,b);
		color_prom++;
	}


	/* character and sprite lookup table */
	/* they use colors 0-63 */
	for (i = 0;i < TOTAL_COLORS(0);i++)
	{
		/* pen 0 always uses color 0 (background in River Patrol and Silver Land) */
		if (i % 4 == 0) COLOR(0,i) = 0;
		else COLOR(0,i) = i;
	}

	/* big sprite lookup table */
	/* it uses colors 64-95 */
	for (i = 0;i < TOTAL_COLORS(2);i++)
	{
		if (i % 4 == 0) COLOR(2,i) = 0;
		else COLOR(2,i) = i + 64;
	}
}


/***************************************************************************

  Convert the color PROMs into a more useable format.

  Swimmer has two 256x4 char/sprite palette PROMs and one 32x8 big sprite
  palette PROM.
  The palette PROMs are connected to the RGB output this way:
  (the 500 and 250 ohm resistors are made of 1 kohm resistors in parallel)

  bit 3 -- 250 ohm resistor  -- BLUE
        -- 500 ohm resistor  -- BLUE
        -- 250 ohm resistor  -- GREEN
  bit 0 -- 500 ohm resistor  -- GREEN
  bit 3 -- 1  kohm resistor  -- GREEN
        -- 250 ohm resistor  -- RED
        -- 500 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

  bit 7 -- 250 ohm resistor  -- BLUE
        -- 500 ohm resistor  -- BLUE
        -- 250 ohm resistor  -- GREEN
        -- 500 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 250 ohm resistor  -- RED
        -- 500 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

  Additionally, the background color of the score panel is determined by
  these resistors:

                  /--- tri-state --  470 -- BLUE
  +5V -- 1kohm ------- tri-state --  390 -- GREEN
                  \--- tri-state -- 1000 -- RED

***************************************************************************/

#define BGPEN (256+32)
#define SIDEPEN (256+32+1)

PALETTE_INIT( swimmer )
{
	int i;
	#define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
	#define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + (offs)])


	for (i = 0;i < 256;i++)
	{
		int bit0,bit1,bit2,r,g,b;


		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;
		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i+256] >> 0) & 0x01;
		bit2 = (color_prom[i+256] >> 1) & 0x01;
		g = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (color_prom[i+256] >> 2) & 0x01;
		bit2 = (color_prom[i+256] >> 3) & 0x01;
		b = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;

		palette_set_color(i,r,g,b);

		/* side panel */
		if (i % 8)
		{
			COLOR(0,i) = i;
		    COLOR(0,i+256) = i;
		}
		else
		{
			/* background */
			COLOR(0,i) = BGPEN;
			COLOR(0,i+256) = SIDEPEN;
		}
	}

	color_prom += 2 * 256;

	/* big sprite */
	for (i = 0;i < 32;i++)
	{
		int bit0,bit1,bit2,r,g,b;


		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;
		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (color_prom[i] >> 6) & 0x01;
		bit2 = (color_prom[i] >> 7) & 0x01;
		b = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;

		palette_set_color(i+256,r,g,b);

		if (i % 8 == 0) COLOR(2,i) = BGPEN;  /* enforce transparency */
		else COLOR(2,i) = i+256;
	}

	/* background */
	palette_set_color(BGPEN,0,0,0);
	/* side panel background color */
#if 0
	// values calculated from the resistors don't seem to match the real board
	palette_set_color(SIDEPEN,0x24,0x5d,0x4e);
#endif
	palette_set_color(SIDEPEN,0x20,0x98,0x79);
}



/***************************************************************************

  Swimmer can directly set the background color.
  The latch is connected to the RGB output this way:
  (the 500 and 250 ohm resistors are made of 1 kohm resistors in parallel)

  bit 7 -- 250 ohm resistor  -- RED
        -- 500 ohm resistor  -- RED
        -- 250 ohm resistor  -- GREEN
        -- 500 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 250 ohm resistor  -- BLUE
        -- 500 ohm resistor  -- BLUE
  bit 0 -- 1  kohm resistor  -- BLUE

***************************************************************************/
WRITE8_HANDLER( swimmer_bgcolor_w )
{
	int bit0,bit1,bit2;
	int r,g,b;


	/* red component */
	bit0 = 0;
	bit1 = (data >> 6) & 0x01;
	bit2 = (data >> 7) & 0x01;
	r = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;

	/* green component */
	bit0 = (data >> 3) & 0x01;
	bit1 = (data >> 4) & 0x01;
	bit2 = (data >> 5) & 0x01;
	g = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;

	/* blue component */
	bit0 = (data >> 0) & 0x01;
	bit1 = (data >> 1) & 0x01;
	bit2 = (data >> 2) & 0x01;
	b = 0x20 * bit0 + 0x40 * bit1 + 0x80 * bit2;

	palette_set_color(BGPEN,r,g,b);
}



WRITE8_HANDLER( cclimber_colorram_w )
{
	if (colorram[offset] != data)
	{
		/* bit 5 of the address is not used for color memory. There is just */
		/* 512 bytes of memory; every two consecutive rows share the same memory */
		/* region. */
		offset &= 0xffdf;

		dirtybuffer[offset] = 1;
		dirtybuffer[offset + 0x20] = 1;

		colorram[offset] = data;
		colorram[offset + 0x20] = data;
	}
}



WRITE8_HANDLER( cclimber_bigsprite_videoram_w )
{
	cclimber_bsvideoram[offset] = data;
}



WRITE8_HANDLER( swimmer_palettebank_w )
{
	set_vh_global_attribute(&palettebank, data & 1);
}



WRITE8_HANDLER( swimmer_sidepanel_enable_w )
{
	set_vh_global_attribute(&sidepanel_enabled, data );
}



/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
static void drawbigsprite(mame_bitmap *bitmap)
{
	int offs;
	int ox,oy,sx,sy,flipx,flipy;
	int color;


	ox = 136 - cclimber_bigspriteram[3];
	oy = 128 - cclimber_bigspriteram[2];
	flipx = cclimber_bigspriteram[1] & 0x10;
	flipy = cclimber_bigspriteram[1] & 0x20;
	if (flip_screen_y)      /* only the Y direction has to be flipped */
	{
		oy = 128 - oy;
		flipy = !flipy;
	}
	color = cclimber_bigspriteram[1] & 0x07;	/* cclimber */
//  color = cclimber_bigspriteram[1] & 0x03;    /* swimmer */

	
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
		(oy+8*sy) & 0xff, 	// clip
		0, 	// transparency
		TRANSPARENCY_PEN, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = cclimber_bsvideoram_size - 1;offs >= 0;offs--)
	{
		sx = offs % 16;
		sy = offs / 16;
		if (flipx) sx = 15 - sx;
		if (flipy) sy = 15 - sy;

		
		dgp0.code = //              cclimber_bsvideoram[offs];
		dgp0.color = /* cclimber */				cclimber_bsvideoram[offs] + ((cclimber_bigspriteram[1] & 0x08) << 5);
		dgp0.flipx = /* swimmer */				color;
		dgp0.flipy = flipx;
		dgp0.sx = flipy;
		dgp0.sy = (ox+8*sx) & 0xff;
		drawgfx(&dgp0);

		/* wraparound */
		
		dgp0.code = //              cclimber_bsvideoram[offs];
		dgp0.color = /* cclimber */				cclimber_bsvideoram[offs] + ((cclimber_bigspriteram[1] & 0x08) << 5);
		dgp0.flipx = /* swimmer */				color;
		dgp0.flipy = flipx;
		dgp0.sx = flipy;
		dgp0.sy = ((ox+8*sx) & 0xff) - 256;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( cclimber )
{
	int offs;


	if (get_vh_global_attribute_changed())
	{
		memset(dirtybuffer,1,videoram_size);
	}

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp2={
		tmpbitmap, 	// dest
		Machine->gfx[(colorram[offs] & 0x10) ? 1 : 0], 	// gfx
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
			flipx = colorram[offs] & 0x40;
			flipy = colorram[offs] & 0x80;
			/* vertical flipping flips two adjacent characters */
			if (flipy) sy ^= 1;

			if (flip_screen_x)
			{
				sx = 31 - sx;
				flipx = !flipx;
			}
			if (flip_screen_y)
			{
				sy = 31 - sy;
				flipy = !flipy;
			}

			
			dgp2.code = videoram[offs] + 8 * (colorram[offs] & 0x20);
			dgp2.color = colorram[offs] & 0x0f;
			dgp2.flipx = flipx;
			dgp2.flipy = flipy;
			dgp2.sx = 8*sx;
			dgp2.sy = 8*sy;
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scroll[32];


		if (flip_screen_x)
		{
			for (offs = 0;offs < 32;offs++)
			{
				scroll[offs] = -cclimber_column_scroll[31 - offs];
				if (flip_screen_y) scroll[offs] = -scroll[offs];
			}
		}
		else
		{
			for (offs = 0;offs < 32;offs++)
			{
				scroll[offs] = -cclimber_column_scroll[offs];
				if (flip_screen_y) scroll[offs] = -scroll[offs];
			}
		}

		copyscrollbitmap(bitmap,tmpbitmap,0,0,32,scroll,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}


	if (cclimber_bigspriteram[0] & 1)
		/* draw the "big sprite" below sprites */
		drawbigsprite(bitmap);


	/* Draw the sprites. Note that it is important to draw them exactly in this */
	/* order, to have the correct priorities. */
	
	{ 
	struct drawgfxParams dgp3={
		bitmap, 	// dest
		Machine->gfx[spriteram[offs + 1] & 0x10 ? 4 : 3], 	// gfx
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
		int sx,sy,flipx,flipy;


		sx = spriteram[offs + 3];
		sy = 240 - spriteram[offs + 2];
		flipx = spriteram[offs] & 0x40;
		flipy = spriteram[offs] & 0x80;
		if (flip_screen_x)
		{
			sx = 240 - sx;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		
		dgp3.code = (spriteram[offs] & 0x3f) + 2 * (spriteram[offs + 1] & 0x20);
		dgp3.color = spriteram[offs + 1] & 0x0f;
		dgp3.flipx = flipx;
		dgp3.flipy = flipy;
		dgp3.sx = sx;
		dgp3.sy = sy;
		drawgfx(&dgp3);
	}
	} // end of patch paragraph



	if ((cclimber_bigspriteram[0] & 1) == 0)
		/* draw the "big sprite" over sprites */
		drawbigsprite(bitmap);
}


VIDEO_UPDATE( swimmer )
{
	int offs;

	if (get_vh_global_attribute_changed())
	{
		memset(dirtybuffer,1,videoram_size);
	}

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp4={
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
			int sx,sy,flipx,flipy,color;


			dirtybuffer[offs] = 0;

			sx = offs % 32;
			sy = offs / 32;
			flipx = colorram[offs] & 0x40;
			flipy = colorram[offs] & 0x80;
			/* vertical flipping flips two adjacent characters */
			if (flipy) sy ^= 1;

			color = (colorram[offs] & 0x0f) + 0x10 * palettebank;
			if (sx >= 24 && sidepanel_enabled)
			{
			    color += 32;
			}

			if (flip_screen_x)
			{
				sx = 31 - sx;
				flipx = !flipx;
			}
			if (flip_screen_y)
			{
				sy = 31 - sy;
				flipy = !flipy;
			}

			
			dgp4.code = videoram[offs] + ((colorram[offs] & 0x10) << 4);
			dgp4.color = color;
			dgp4.flipx = flipx;
			dgp4.flipy = flipy;
			dgp4.sx = 8*sx;
			dgp4.sy = 8*sy;
			drawgfx(&dgp4);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scroll[32];


		if (flip_screen_y)
		{
			for (offs = 0;offs < 32;offs++)
				scroll[offs] = cclimber_column_scroll[31 - offs];
		}
		else
		{
			for (offs = 0;offs < 32;offs++)
				scroll[offs] = -cclimber_column_scroll[offs];
		}

		copyscrollbitmap(bitmap,tmpbitmap,0,0,32,scroll,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}


	if (cclimber_bigspriteram[0] & 1)
		/* draw the "big sprite" below sprites */
		drawbigsprite(bitmap);


	/* Draw the sprites. Note that it is important to draw them exactly in this */
	/* order, to have the correct priorities. */
	
	{ 
	struct drawgfxParams dgp5={
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
		int sx,sy,flipx,flipy;


		sx = spriteram[offs + 3];
		sy = 240 - spriteram[offs + 2];
		flipx = spriteram[offs] & 0x40;
		flipy = spriteram[offs] & 0x80;
		if (flip_screen_x)
		{
			sx = 240 - sx;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		
		dgp5.code = (spriteram[offs] & 0x3f) | (spriteram[offs + 1] & 0x10) << 2;
		dgp5.color = (spriteram[offs + 1] & 0x0f) + 0x10 * palettebank;
		dgp5.flipx = flipx;
		dgp5.flipy = flipy;
		dgp5.sx = sx;
		dgp5.sy = sy;
		drawgfx(&dgp5);
	}
	} // end of patch paragraph



	if ((cclimber_bigspriteram[0] & 1) == 0)
		/* draw the "big sprite" over sprites */
		drawbigsprite(bitmap);
}

VIDEO_UPDATE( yamato )
{
	int offs;
	int i,j;

	/* bg gradient */
	fillbitmap(bitmap, 0, cliprect);

	for(i=0;i<256;i++)
	{
		for(j=0;j<256;j++)
		{
			plot_pixel(bitmap, i-8, j, 16*4+8*4 + memory_region(REGION_USER1)[(flip_screen_x?0x1280:0x1200)+(i>>1)]);
		}
	}

	if (get_vh_global_attribute_changed())
	{
		memset(dirtybuffer,1,videoram_size);
	}

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp6={
		tmpbitmap, 	// dest
		Machine->gfx[(colorram[offs] & 0x10) ? 1 : 0], 	// gfx
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
			flipx = colorram[offs] & 0x40;
			flipy = colorram[offs] & 0x80;
			/* vertical flipping flips two adjacent characters */
			if (flipy) sy ^= 1;

			if (flip_screen_x)
			{
				sx = 31 - sx;
				flipx = !flipx;
			}
			if (flip_screen_y)
			{
				sy = 31 - sy;
				flipy = !flipy;
			}

			
			dgp6.code = videoram[offs] + 8 * (colorram[offs] & 0x20);
			dgp6.color = colorram[offs] & 0x0f;
			dgp6.flipx = flipx;
			dgp6.flipy = flipy;
			dgp6.sx = 8*sx;
			dgp6.sy = 8*sy;
			drawgfx(&dgp6);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scroll[32];


		if (flip_screen_x)
		{
			for (offs = 0;offs < 32;offs++)
			{
				scroll[offs] = -cclimber_column_scroll[31 - offs];
				if (flip_screen_y) scroll[offs] = -scroll[offs];
			}
		}
		else
		{
			for (offs = 0;offs < 32;offs++)
			{
				scroll[offs] = -cclimber_column_scroll[offs];
				if (flip_screen_y) scroll[offs] = -scroll[offs];
			}
		}

		copyscrollbitmap(bitmap,tmpbitmap,0,0,32,scroll,&Machine->visible_area,TRANSPARENCY_PEN,0);
	}


	if (cclimber_bigspriteram[0] & 1)
		/* draw the "big sprite" below sprites */
		drawbigsprite(bitmap);


	/* Draw the sprites. Note that it is important to draw them exactly in this */
	/* order, to have the correct priorities. */
	
	{ 
	struct drawgfxParams dgp7={
		bitmap, 	// dest
		Machine->gfx[spriteram[offs + 1] & 0x10 ? 4 : 3], 	// gfx
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
		int sx,sy,flipx,flipy;


		sx = spriteram[offs + 3];
		sy = 240 - spriteram[offs + 2];
		flipx = spriteram[offs] & 0x40;
		flipy = spriteram[offs] & 0x80;
		if (flip_screen_x)
		{
			sx = 240 - sx;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		
		dgp7.code = (spriteram[offs] & 0x3f) + 2 * (spriteram[offs + 1] & 0x20);
		dgp7.color = spriteram[offs + 1] & 0x0f;
		dgp7.flipx = flipx;
		dgp7.flipy = flipy;
		dgp7.sx = sx;
		dgp7.sy = sy;
		drawgfx(&dgp7);
	}
	} // end of patch paragraph



	if ((cclimber_bigspriteram[0] & 1) == 0)
		/* draw the "big sprite" over sprites */
		drawbigsprite(bitmap);
}


/* Top Roller */

#define PRIORITY_OVER  	0x20
#define PRIORITY_UNDER 	0x00
#define PRIORITY_MASK 	0x20

static tilemap *bg_tilemap;

UINT8 *toprollr_videoram2;
UINT8 *toprollr_videoram3;
UINT8 *toprollr_videoram4;


static void get_tile_info_bg(int tile_index)
{
	int code = toprollr_videoram3[tile_index];
	int attr = toprollr_videoram4[tile_index];
	int flipx,flipy,bank,palette;

	bank=(attr&0x40)<<2;





	flipx=1;
	flipy=0;

	code+=bank;

	palette=(attr&0xf)+0x18 ;

	SET_TILE_INFO(1, code, palette, (flipx?TILE_FLIPX:0)|(flipy?TILE_FLIPY:0))
}

VIDEO_START( toprollr )
{
	bg_tilemap = tilemap_create(get_tile_info_bg,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,32,32);
	return 0;
}

static void trdrawbigsprite(mame_bitmap *bitmap,const rectangle *cliprect,int priority)
{
		if((cclimber_bigspriteram[1]&PRIORITY_MASK)==priority)
		{
			int code,xs,ys,palette,bank,x,y;
			int flipx=0;
			int flipy=0;

			xs=136-cclimber_bigspriteram[3];
			ys=128-cclimber_bigspriteram[2];


			if(xs==0)
			{
				return;
			}

			if (flip_screen_x)
			{
				flipx^=1;
			}

			palette=cclimber_bigspriteram[1]&0x7;

			bank=(cclimber_bigspriteram[1]>>3)&3;


			for(y=0;y<16;y++)
				
				{ 
				struct drawgfxParams dgp8={
					bitmap, 	// dest
					Machine->gfx[3], 	// gfx
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
				for(x=0;x<16;x++)
				{
					int sx=x;
					int sy=y;
					if (flipx) sx = 15 - x;
					if (flipy) sy = 15 - y;

					code=cclimber_bsvideoram[y*16+x]+bank*256;

					
					dgp8.code = code;
					dgp8.color = palette;
					dgp8.flipx = flipx;
					dgp8.flipy = flipy;
					dgp8.sx = (sx*8+xs)&0xff;
					dgp8.sy = (sy*8+ys)&0xff;
					drawgfx(&dgp8);
					
					dgp8.code = code;
					dgp8.color = palette;
					dgp8.flipx = flipx;
					dgp8.flipy = flipy;
					dgp8.sx = ((sx*8+xs)&0xff)-256;
					dgp8.sy = ((sy*8+ys)&0xff)-256;
					drawgfx(&dgp8);
				}
				} // end of patch paragraph

		}
}

VIDEO_UPDATE( toprollr )
{

	UINT32 x,y;
	int offs;
	rectangle myclip=*cliprect;
	myclip.min_x=4*8;
	myclip.max_x=29*8-1;

	fillbitmap(bitmap, 0, cliprect);

	tilemap_set_scrollx(bg_tilemap,0,toprollr_videoram3[0]+8);
	tilemap_mark_all_tiles_dirty(bg_tilemap);
	tilemap_draw(bitmap, &myclip,bg_tilemap,0,0);

	trdrawbigsprite(bitmap, &myclip, PRIORITY_UNDER);

	
	{ 
	struct drawgfxParams dgp10={
		bitmap, 	// dest
		Machine->gfx[2], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&myclip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size - 4;offs >= 0;offs -= 4)
	{
		int sx,sy,flipx,flipy,palette;

		sx = spriteram[offs + 3]-8;
		sy = 240 - spriteram[offs + 2];
		flipx = spriteram[offs] & 0x40;
		flipy = spriteram[offs] & 0x80;
		if (flip_screen_x)
		{
			sx = 240 - sx;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		palette=0x08+(spriteram[offs + 1] & 0x0f);

		
		dgp10.code = (spriteram[offs] & 0x3f) + 2 * (spriteram[offs + 1] & 0x20)+8*(spriteram[offs + 1] & 0x10);
		dgp10.color = palette;
		dgp10.flipx = flipx;
		dgp10.flipy = flipy;
		dgp10.sx = sx;
		dgp10.sy = sy;
		drawgfx(&dgp10);
	}
	} // end of patch paragraph


	trdrawbigsprite(bitmap, &myclip, PRIORITY_OVER);

	for(y=0;y<32;y++)
		
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
			cliprect, 	// clip
			TRANSPARENCY_PEN, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for(x=0;x<32;x++)
		{
			int sx=x*8;
			int sy=y*8;
			int flipx=0;
			int flipy=0;

			int code=videoram[y*32+x];
			int attr=(x>16)?(toprollr_videoram2[(y&0xfe)*32+x]):(toprollr_videoram2[y*32+x]);
			int palette;

			if (flip_screen_x)
			{
				sx = 240 - sx;
				flipx^=1;
			}
			if (flip_screen_y)
			{
				sy = 240 - sy;
				flipy^=1;
			}

			palette=8+(attr&0xf);
			
			dgp11.code = code+((attr&0xf0)<<4);
			dgp11.color = palette;
			dgp11.flipx = flipx;
			dgp11.flipy = flipy;
			dgp11.sx = sx;
			dgp11.sy = sy;
			drawgfx(&dgp11);

		}
		} // end of patch paragraph

}



