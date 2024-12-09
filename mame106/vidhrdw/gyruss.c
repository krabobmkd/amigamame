/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"


static int flipscreen;

/*
sprites are multiplexed, so we have to buffer the spriteram
scanline by scanline.
*/
static unsigned char *sprite_mux_buffer;
static int scanline;


/***************************************************************************

  Convert the color PROMs into a more useable format.

  Gyruss has one 32x8 palette PROM and two 256x4 lookup table PROMs
  (one for characters, one for sprites).
  The palette PROM is connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/
PALETTE_INIT( gyruss )
{
	int i;
	#define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
	#define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs])


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

	/* color_prom now points to the beginning of the sprite lookup table */

	/* sprites */
	for (i = 0;i < TOTAL_COLORS(1);i++)
		COLOR(1,i) = *(color_prom++) & 0x0f;

	/* characters */
	for (i = 0;i < TOTAL_COLORS(0);i++)
		COLOR(0,i) = (*(color_prom++) & 0x0f) + 0x10;
}



VIDEO_START( gyruss )
{
	sprite_mux_buffer = auto_malloc(256 * spriteram_size);

	return video_start_generic();
}



WRITE8_HANDLER( gyruss_flipscreen_w )
{
	if (flipscreen != (data & 1))
	{
		flipscreen = data & 1;
		memset(dirtybuffer,1,videoram_size);
	}
}



/* Return the current video scan line */
READ8_HANDLER( gyruss_scanline_r )
{
	return scanline;
}



/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap)
{
	rectangle clip = Machine->visible_area;
	int offs;
	int line;

	
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		NULL, // gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&clip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };

	for (line = 0;line < 256;line++)
	{
		if (line >= Machine->visible_area.min_y && line <= Machine->visible_area.max_y)
		{
			unsigned char *sr;

			sr = sprite_mux_buffer + line * spriteram_size;
			clip.min_y = clip.max_y = line;

			for (offs = spriteram_size - 4;offs >= 0;offs -= 4)
			{
				int sx,sy;

				sx = sr[offs];
				sy = 241 - sr[offs + 3];
				if (sy > line-16 && sy <= line)
				{
					dgp0.gfx = Machine->gfx[1 + (sr[offs + 1] & 1)];
					dgp0.code = sr[offs + 1]/2 + 4*(sr[offs + 2] & 0x20);
					dgp0.color = sr[offs + 2] & 0x0f;
					dgp0.flipx = !(sr[offs + 2] & 0x40);
					dgp0.flipy = sr[offs + 2] & 0x80;
					dgp0.sx = sx;
					dgp0.sy = sy;
					drawgfx(&dgp0);
				}
			}
		}
	}
}


VIDEO_UPDATE( gyruss )
{
	int offs;


	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	struct drawgfxParams dgp1={
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
			if (flipscreen)
			{
				sx = 31 - sx;
				sy = 31 - sy;
				flipx = !flipx;
				flipy = !flipy;
			}

			dgp1.code = videoram[offs] + 8 * (colorram[offs] & 0x20);
			dgp1.color = colorram[offs] & 0x0f;
			dgp1.flipx = flipx;
			dgp1.flipy = flipy;
			dgp1.sx = 8*sx;
			dgp1.sy = 8*sy;
			drawgfx(&dgp1);
		}
	}




	/* copy the character mapped graphics */
	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);


	draw_sprites(bitmap);


	/* redraw the characters which have priority over sprites */
	
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
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		int sx,sy,flipx,flipy;


		sx = offs % 32;
		sy = offs / 32;
		flipx = colorram[offs] & 0x40;
		flipy = colorram[offs] & 0x80;
		if (flipscreen)
		{
			sx = 31 - sx;
			sy = 31 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		if ((colorram[offs] & 0x10) != 0)
		{
			dgp2.code = videoram[offs] + 8 * (colorram[offs] & 0x20);
			dgp2.color = colorram[offs] & 0x0f;
			dgp2.flipx = flipx;
			dgp2.flipy = flipy;
			dgp2.sx = 8*sx;
			dgp2.sy = 8*sy;
			drawgfx(&dgp2);
        }
	}


}


INTERRUPT_GEN( gyruss_6809_interrupt )
{
	scanline = 255 - cpu_getiloops();

	memcpy(sprite_mux_buffer + scanline * spriteram_size,spriteram,spriteram_size);

	if (scanline == 255)
		irq0_line_hold();
}
