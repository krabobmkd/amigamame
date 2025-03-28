/*************************************************************************
 Universal Cheeky Mouse Driver
 (c)Lee Taylor May 1998, All rights reserved.

 For use only in offical Mame releases.
 Not to be distrabuted as part of any commerical work.
***************************************************************************
Functions to emulate the video hardware of the machine.
***************************************************************************/

#include "driver.h"
#include "sound/dac.h"


static int redraw_man = 0;
static int man_scroll = -1;
static UINT8 sprites[0x20];
static int char_palette = 0;


PALETTE_INIT( cheekyms )
{
	int i,j,bit,r,g,b;

	for (i = 0; i < 3; i++)
	{
		const unsigned char* color_prom_save = color_prom;

		/* lower nibble */
		for (j = 0;j < Machine->drv->total_colors/6;j++)
		{
			/* red component */
			bit = (color_prom[0] >> 0) & 0x01;
			r = 0xff * bit;
			/* green component */
			bit = (color_prom[0] >> 1) & 0x01;
			g = 0xff * bit;
			/* blue component */
			bit = (color_prom[0] >> 2) & 0x01;
			b = 0xff * bit;

			palette_set_color(((i*2)*Machine->drv->total_colors/6)+j,r,g,b);
			color_prom++;
		}

		color_prom = color_prom_save;

		/* upper nibble */
		for (j = 0;j < Machine->drv->total_colors/6;j++)
		{
			/* red component */
			bit = (color_prom[0] >> 4) & 0x01;
			r = 0xff * bit;
			/* green component */
			bit = (color_prom[0] >> 5) & 0x01;
			g = 0xff * bit;
			/* blue component */
			bit = (color_prom[0] >> 6) & 0x01;
			b = 0xff * bit;

			palette_set_color(((i*2+1)*Machine->drv->total_colors/6)+j,r,g,b);
			color_prom++;
		}
	}
}


WRITE8_HANDLER( cheekyms_sprite_w )
{
	sprites[offset] = data;
}


WRITE8_HANDLER( cheekyms_port_40_w )
{
	static int last_dac = -1;

	/* The lower bits probably trigger sound samples */

	if (last_dac != (data & 0x80))
	{
		last_dac = data & 0x80;

		DAC_data_w(0, last_dac ? 0x80 : 0);
	}
}


WRITE8_HANDLER( cheekyms_port_80_w )
{
	int new_man_scroll;

	/* Bits 0-1 Sound enables, not sure which bit is which */

	/* Bit 2 is interrupt enable */
	interrupt_enable_w(offset, data & 0x04);

	/* Bit 3-5 Man scroll amount */
    new_man_scroll = (data >> 3) & 0x07;
	if (man_scroll != new_man_scroll)
	{
		man_scroll = new_man_scroll;
		redraw_man = 1;
	}

	/* Bit 6 is palette select (Selects either 0 = PROM M8, 1 = PROM M9) */
	set_vh_global_attribute(&char_palette, (data >> 2) & 0x10);

	/* Bit 7 is screen flip */
	flip_screen_set(data & 0x80);
}



/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( cheekyms )
{
	int offs;


	if (get_vh_global_attribute_changed())
	{
		memset(dirtybuffer, 1, videoram_size);
	}


	fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);

	/* Draw the sprites first, because they're supposed to appear below
       the characters */
	
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
	for (offs = 0; offs < sizeof(sprites)/sizeof(sprites[0]); offs += 4)
	{
		int v1, sx, sy, col, code;

		v1  = sprites[offs + 0];
		sy  = sprites[offs + 1];
		sx  = 256 - sprites[offs + 2];
		col = (sprites[offs + 3] & 0x07);

		if (!(sprites[offs + 3] & 0x08)) continue;

		code = (~v1 << 1) & 0x1f;

		if (v1 & 0x80)
		{
			if (!flip_screen)
			{
				code++;
			}

			
			dgp0.code = code;
			dgp0.color = col;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}
		else
		{
			
			dgp0.code = code + 0x20;
			dgp0.color = col;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);

			
			dgp0.code = code + 0x21;
			dgp0.color = col;
			dgp0.sx = sx + 8*(v1 & 2);
			dgp0.sy = sy + 8*(~v1 & 2);
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph


	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp3={
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
		int sx,sy,man_area;

		sx = offs % 32;
		sy = offs / 32;

		if (flip_screen)
		{
			man_area = ((sy >=  5) && (sy <= 25) && (sx >=  8) && (sx <= 12));
		}
		else
		{
			man_area = ((sy >=  6) && (sy <= 26) && (sx >=  8) && (sx <= 12));
		}

		if (dirtybuffer[offs] ||
			(redraw_man && man_area))
		{
			dirtybuffer[offs] = 0;

			if (flip_screen)
			{
				sx = 31 - sx;
				sy = 31 - sy;
			}

			
			dgp3.code = videoram[offs];
			dgp3.color = 0 + char_palette;
			dgp3.flipx = flip_screen;
			dgp3.flipy = flip_screen;
			dgp3.sx = 8*sx;
			dgp3.sy = 8*sy - (man_area ? man_scroll : 0);
			drawgfx(&dgp3);
		}
	}
	} // end of patch paragraph


	redraw_man = 0;

	/* copy the temporary bitmap to the screen over the sprites */
	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_PEN,Machine->pens[4*char_palette]);
}
