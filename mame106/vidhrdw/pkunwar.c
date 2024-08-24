/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"


static int flipscreen[2];


WRITE8_HANDLER( pkunwar_flipscreen_w )
{
	if (flipscreen[0] != (data & 1))
	{
		flipscreen[0] = data & 1;
		memset(dirtybuffer,1,videoram_size);
	}
	if (flipscreen[1] != (data & 2))
	{
		flipscreen[1] = data & 2;
		memset(dirtybuffer,1,videoram_size);
	}
}



VIDEO_UPDATE( pkunwar )
{
	int offs;


	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		if (dirtybuffer[offs])
		
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
			int sx,sy;


			dirtybuffer[offs] = 0;

			sx = offs % 32;
			sy = offs / 32;
			if (flipscreen[0]) sx = 31 - sx;
			if (flipscreen[1]) sy = 31 - sy;

			
			dgp0.code = videoram[offs] + ((colorram[offs] & 0x07) << 8);
			dgp0.color = (colorram[offs] & 0xf0) >> 4;
			dgp0.flipx = flipscreen[0];
			dgp0.flipy = flipscreen[1];
			dgp0.sx = 8*sx;
			dgp0.sy = 8*sy;
			drawgfx(&dgp0);
		}
} // end of patch paragraph

	}

	/* copy the character mapped graphics */
	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);


	/* Draw the sprites. */
	
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
	for (offs = 0;offs < spriteram_size;offs += 32)
	{
		int sx,sy,flipx,flipy;


		sx = ((spriteram[offs + 1] + 8) & 0xff) - 8;
		sy = spriteram[offs + 2];
		flipx = spriteram[offs] & 0x01;
		flipy = spriteram[offs] & 0x02;
		if (flipscreen[0])
		{
			sx = 240 - sx;
			flipx = !flipx;
		}
		if (flipscreen[1])
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		
		dgp1.code = ((spriteram[offs] & 0xfc) >> 2) + ((spriteram[offs + 3] & 0x07) << 6);
		dgp1.color = (spriteram[offs + 3] & 0xf0) >> 4;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = sx;
		dgp1.sy = sy;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph



	/* redraw characters which have priority over sprites */
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		if (colorram[offs] & 0x08)
		
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
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			int sx,sy;


			sx = offs % 32;
			sy = offs / 32;
			if (flipscreen[0]) sx = 31 - sx;
			if (flipscreen[1]) sy = 31 - sy;

			
			dgp2.code = videoram[offs] + ((colorram[offs] & 0x07) << 8);
			dgp2.color = (colorram[offs] & 0xf0) >> 4;
			dgp2.flipx = flipscreen[0];
			dgp2.flipy = flipscreen[1];
			dgp2.sx = 8*sx;
			dgp2.sy = 8*sy;
			drawgfx(&dgp2);
		}
} // end of patch paragraph

	}
}
