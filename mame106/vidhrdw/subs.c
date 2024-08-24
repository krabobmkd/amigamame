/***************************************************************************

    Atari Subs hardware

***************************************************************************/

#include "driver.h"
#include "subs.h"
#include "sound/discrete.h"

WRITE8_HANDLER( subs_invert1_w )
{
	if ((offset & 0x01) == 1)
	{
		palette_set_color(0, 0x00, 0x00, 0x00);
		palette_set_color(1, 0xFF, 0xFF, 0xFF);
	}
	else
	{
		palette_set_color(1, 0x00, 0x00, 0x00);
		palette_set_color(0, 0xFF, 0xFF, 0xFF);
	}
}

WRITE8_HANDLER( subs_invert2_w )
{
	if ((offset & 0x01) == 1)
	{
		palette_set_color(2, 0x00, 0x00, 0x00);
		palette_set_color(3, 0xFF, 0xFF, 0xFF);
	}
	else
	{
		palette_set_color(3, 0x00, 0x00, 0x00);
		palette_set_color(2, 0xFF, 0xFF, 0xFF);
	}
}


/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

VIDEO_UPDATE( subs )
{
	int updateall = get_vh_global_attribute_changed();
	int offs;

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		if (updateall || dirtybuffer[offs])
		{
			int charcode;
			int sx,sy;
			int left_enable,right_enable;
			int left_sonar_window,right_sonar_window;

			left_sonar_window = 0;
			right_sonar_window = 0;

			dirtybuffer[offs]=0;

			charcode = videoram[offs];

			/* Which monitor is this for? */
			right_enable = charcode & 0x40;
			left_enable = charcode & 0x80;

			sx = 8 * (offs % 32);
			sy = 8 * (offs / 32);

			/* Special hardware logic for sonar windows */
			if ((sy >= (128+64)) && (sx < 32))
				left_sonar_window = 1;
			else if ((sy >= (128+64)) && (sx >= (128+64+32)))
				right_sonar_window = 1;
			else
				charcode = charcode & 0x3F;

			/* Draw the left screen */
			if ((left_enable || left_sonar_window) && (!right_sonar_window))
			
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
{
				
				dgp0.code = charcode;
				dgp0.sx = sx;
				dgp0.sy = sy;
				drawgfx(&dgp0);
			}
} // end of patch paragraph

			
{ 
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
else
			{
				
				dgp1.sx = sx;
				dgp1.sy = sy;
				drawgfx(&dgp1);
			}
} // end of patch paragraph


			/* Draw the right screen */
			if ((right_enable || right_sonar_window) && (!left_sonar_window))
			
{ 
struct drawgfxParams dgp2={
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
{
				
				dgp2.code = charcode;
				dgp2.sx = sx+256;
				dgp2.sy = sy;
				drawgfx(&dgp2);
			}
} // end of patch paragraph

			
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
else
			{
				
				dgp3.sx = sx+256;
				dgp3.sy = sy;
				drawgfx(&dgp3);
			}
} // end of patch paragraph

		}
	}

	/* copy the character mapped graphics */
	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);

	/* draw the motion objects */
	for (offs = 0; offs < 4; offs++)
	{
		int sx,sy;
		int charcode;
		int prom_set;
		int sub_enable;

		sx = spriteram[0x00 + (offs * 2)] - 16;
		sy = spriteram[0x08 + (offs * 2)] - 16;
		charcode = spriteram[0x09 + (offs * 2)];
		if (offs < 2)
			sub_enable = spriteram[0x01 + (offs * 2)] & 0x80;
		else
			sub_enable = 1;

		prom_set = charcode & 0x01;
		charcode = (charcode >> 3) & 0x1F;

		/* Left screen - special check for drawing right screen's sub */
		if ((offs!=0) || (sub_enable))
		
{ 
struct drawgfxParams dgp4={
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
{
			
			dgp4.code = charcode + 32 * prom_set;
			dgp4.sx = sx;
			dgp4.sy = sy;
			drawgfx(&dgp4);
		}
} // end of patch paragraph


		/* Right screen - special check for drawing left screen's sub */
		if ((offs!=1) || (sub_enable))
		
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
{
			
			dgp5.code = charcode + 32 * prom_set;
			dgp5.sx = sx + 256;
			dgp5.sy = sy;
			drawgfx(&dgp5);
		}
} // end of patch paragraph

	}

	/* Update sound */
	discrete_sound_w(SUBS_LAUNCH_DATA, spriteram[5] & 0x0f);	// Launch data
	discrete_sound_w(SUBS_CRASH_DATA, spriteram[5] >> 4);		// Crash/explode data
}
