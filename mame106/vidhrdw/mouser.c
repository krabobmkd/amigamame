/*******************************************************************************

     Mouser - Video Hardware:

     Character map with scrollable rows, 1024 possible characters.
        - index = byte from videoram + 2 bits from colorram)
        - (if row is scrolled, videoram is offset, colorram is not)
        - 16 4-color combinations for each char, from colorram

     15 Sprites controlled by 4-byte records
        - 16 4-color combinations
        - 2 banks of 64 sprite characters each

*******************************************************************************/

#include "driver.h"

PALETTE_INIT( mouser )
{
	int i;

	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = BIT(*color_prom,0);
		bit1 = BIT(*color_prom,1);
		bit2 = BIT(*color_prom,2);
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = BIT(*color_prom,3);
		bit1 = BIT(*color_prom,4);
		bit2 = BIT(*color_prom,5);
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = BIT(*color_prom,6);
		bit1 = BIT(*color_prom,7);
		b = 0x4f * bit0 + 0xa8 * bit1;

		palette_set_color(i,r,g,b);
		color_prom++;
	}
}

WRITE8_HANDLER( mouser_flip_screen_x_w )
{
	flip_screen_x_set(~data & 1);
}

WRITE8_HANDLER( mouser_flip_screen_y_w )
{
	flip_screen_y_set(~data & 1);
}

WRITE8_HANDLER( mouser_spriteram_w )
{
	/* Mark the entire row as dirty if row scrollram is written */
	/* Only used by the MOUSER logo */

	int i;

	if (offset < 32)
	{
		for(i=0;i<32;i++)
		dirtybuffer[offset+i*32] = 1;
	}
	spriteram_w(offset, data);
}

WRITE8_HANDLER( mouser_colorram_w )
{
	dirtybuffer[offset] = 1;
	colorram_w(offset, data);
}

VIDEO_UPDATE( mouser )
{
	int offs;
	int sx,sy;
	int flipx,flipy;

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		int scrolled_y_position;
		int color_offs;

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
	0, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			dirtybuffer[offs] = 0;

			sx = offs % 32;
			sy = offs / 32;

			if (flip_screen_x)
			{
				sx = 31 - sx;
			}

			if (flip_screen_y)
			{
				sy = 31 - sy;
			}

			/* This bit of spriteram appears to be for row scrolling */
			/* Note: this is dependant on flipping in y */
			scrolled_y_position = (256 + 8*sy - spriteram[offs%32])%256;
			/* I think we still need to fetch the colorram bits to from the ram underneath, which is not scrolled */
			/* Ideally we would merge these on a pixel-by-pixel basis, but it's ok to do this char-by-char, */
			/* Since it's only for the MOUSER logo and it looks fine */
			/* Note: this is _not_ dependant on flipping */
			color_offs = offs%32 + ((256 + 8*(offs/32) - spriteram[offs%32])%256)/8*32;

			
			dgp0.code = videoram[offs] | (colorram[color_offs]>>5)*256 | ((colorram[color_offs]>>4)&1)*512;
			dgp0.color = colorram[color_offs]%16;
			dgp0.flipx = flip_screen_x;
			dgp0.flipy = flip_screen_y;
			dgp0.sx = 8*sx;
			dgp0.sy = scrolled_y_position;
			drawgfx(&dgp0);
		}
} // end of patch paragraph

	}

	copyscrollbitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);

	/* There seem to be two sets of sprites, each decoded identically */

	/* This is the first set of 7 sprites */
	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[1+((spriteram[offs+1]&0x20)>>5)], 	// gfx
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
	for(offs = 0x0084; offs < 0x00A0; offs += 4)
	{
		sx = spriteram[offs+3];
		sy = 0xef-spriteram[offs+2];

		flipx = (spriteram[offs]&0x40)>>6;
		flipy = (spriteram[offs]&0x80)>>7;

		if (flip_screen_x)
		{
			flipx = !flipx;
			sx = 240 - sx;
		}

		if (flip_screen_y)
		{
			flipy = !flipy;
			sy = 238 - sy;
		}

		if ((spriteram[offs+1]&0x10)>>4)
			
			dgp1.code = spriteram[offs]&0x3f;
			dgp1.color = spriteram[offs+1]%16;
			dgp1.flipx = flipx;
			dgp1.flipy = flipy;
			dgp1.sx = sx;
			dgp1.sy = sy;
			drawgfx(&dgp1);
	}
	} // end of patch paragraph


	/* This is the second set of 8 sprites */
	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		Machine->gfx[1+((spriteram[offs+1]&0x20)>>5)], 	// gfx
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
	for(offs = 0x00C4; offs < 0x00E4; offs += 4)
	{
		sx = spriteram[offs+3];
		sy = 0xef-spriteram[offs+2];

		flipx = (spriteram[offs]&0x40)>>6;
		flipy = (spriteram[offs]&0x80)>>7;

		if (flip_screen_x)
		{
			flipx = !flipx;
			sx = 240 - sx;
		}

		if (flip_screen_y)
		{
			flipy = !flipy;
			sy = 238 - sy;
		}

		if ((spriteram[offs+1]&0x10)>>4)
			
			dgp2.code = spriteram[offs]&0x3f;
			dgp2.color = spriteram[offs+1]%16;
			dgp2.flipx = flipx;
			dgp2.flipy = flipy;
			dgp2.sx = sx;
			dgp2.sy = sy;
			drawgfx(&dgp2);
	}
	} // end of patch paragraph


}
