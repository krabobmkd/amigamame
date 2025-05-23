/***************************************************************************

    vidhrdw.c

    Functions to emulate the video hardware of the machine.

    There are only a few differences between the video hardware of Mysterious
    Stones and Mat Mania. The tile bank select bit is different and the sprite
    selection seems to be different as well. Additionally, the palette is stored
    differently. I'm also not sure that the 2nd tile page is really used in
    Mysterious Stones.

***************************************************************************/

#include "driver.h"



unsigned char *matmania_videoram2,*matmania_colorram2;
size_t matmania_videoram2_size;
unsigned char *matmania_videoram3,*matmania_colorram3;
size_t matmania_videoram3_size;
unsigned char *matmania_scroll;
static mame_bitmap *tmpbitmap2;
static unsigned char *dirtybuffer2;

unsigned char *matmania_pageselect;

/***************************************************************************

  Convert the color PROMs into a more useable format.

  Mat Mania is unusual in that it has both PROMs and RAM to control the
  palette. PROMs are used for characters and background tiles, RAM for
  sprites.
  I don't know for sure how the PROMs are connected to the RGB output,
  but it's probably the usual:

  bit 7 -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 2.2kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
        -- 1  kohm resistor  -- RED
  bit 0 -- 2.2kohm resistor  -- RED

  bit 3 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 1  kohm resistor  -- BLUE
  bit 0 -- 2.2kohm resistor  -- BLUE

***************************************************************************/
PALETTE_INIT( matmania )
{
	int i;
	#define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
	#define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs])


	for (i = 0;i < 64;i++)
	{
		int bit0,bit1,bit2,bit3,r,g,b;


		bit0 = (color_prom[0] >> 0) & 0x01;
		bit1 = (color_prom[0] >> 1) & 0x01;
		bit2 = (color_prom[0] >> 2) & 0x01;
		bit3 = (color_prom[0] >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		bit0 = (color_prom[0] >> 4) & 0x01;
		bit1 = (color_prom[0] >> 5) & 0x01;
		bit2 = (color_prom[0] >> 6) & 0x01;
		bit3 = (color_prom[0] >> 7) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		bit0 = (color_prom[64] >> 0) & 0x01;
		bit1 = (color_prom[64] >> 1) & 0x01;
		bit2 = (color_prom[64] >> 2) & 0x01;
		bit3 = (color_prom[64] >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		palette_set_color(i,r,g,b);
		color_prom++;
	}
}



WRITE8_HANDLER( matmania_paletteram_w )
{
	int bit0,bit1,bit2,bit3,val;
	int r,g,b;
	int offs2;


	paletteram[offset] = data;
	offs2 = offset & 0x0f;

	val = paletteram[offs2];
	bit0 = (val >> 0) & 0x01;
	bit1 = (val >> 1) & 0x01;
	bit2 = (val >> 2) & 0x01;
	bit3 = (val >> 3) & 0x01;
	r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

	val = paletteram[offs2 | 0x10];
	bit0 = (val >> 0) & 0x01;
	bit1 = (val >> 1) & 0x01;
	bit2 = (val >> 2) & 0x01;
	bit3 = (val >> 3) & 0x01;
	g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

	val = paletteram[offs2 | 0x20];
	bit0 = (val >> 0) & 0x01;
	bit1 = (val >> 1) & 0x01;
	bit2 = (val >> 2) & 0x01;
	bit3 = (val >> 3) & 0x01;
	b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

	palette_set_color(offs2 + 64,r,g,b);
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/
VIDEO_START( matmania )
{
	dirtybuffer = auto_malloc(videoram_size);
	memset(dirtybuffer,1,videoram_size);

	dirtybuffer2 = auto_malloc(matmania_videoram3_size);
	memset(dirtybuffer2,1,matmania_videoram3_size);

	/* Mat Mania has a virtual screen twice as large as the visible screen */
	if ((tmpbitmap = auto_bitmap_alloc(Machine->drv->screen_width,2* Machine->drv->screen_height)) == 0)
		return 1;

	/* Mat Mania has a virtual screen twice as large as the visible screen */
	if ((tmpbitmap2 = auto_bitmap_alloc(Machine->drv->screen_width,2 * Machine->drv->screen_height)) == 0)
		return 1;

	return 0;
}



WRITE8_HANDLER( matmania_videoram3_w )
{
	if (matmania_videoram3[offset] != data)
	{
		dirtybuffer2[offset] = 1;

		matmania_videoram3[offset] = data;
	}
}



WRITE8_HANDLER( matmania_colorram3_w )
{
	if (matmania_colorram3[offset] != data)
	{
		dirtybuffer2[offset] = 1;

		matmania_colorram3[offset] = data;
	}
}


VIDEO_UPDATE( matmania )
{
	int offs;


	/* Update the tiles in the left tile ram bank */
	
	{ 
	struct drawgfxParams dgp0={
		tmpbitmap, 	// dest
		Machine->gfx[1], 	// gfx
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

			sx = 15 - offs / 32;
			sy = offs % 32;

			
			dgp0.code = videoram[offs] + ((colorram[offs] & 0x08) << 5);
			dgp0.color = (colorram[offs] & 0x30) >> 4;
			dgp0.flipy = sy >= 16;
			dgp0.sx = /* flip horizontally tiles on the right half of the bitmap */					16*sx;
			dgp0.sy = 16*sy;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph


	/* Update the tiles in the right tile ram bank */
	
	{ 
	struct drawgfxParams dgp1={
		tmpbitmap2, 	// dest
		Machine->gfx[1], 	// gfx
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
	for (offs = matmania_videoram3_size - 1;offs >= 0;offs--)
	{
		if (dirtybuffer2[offs])
		{
			int sx,sy;


			dirtybuffer2[offs] = 0;

			sx = 15 - offs / 32;
			sy = offs % 32;

			
			dgp1.code = matmania_videoram3[offs] + ((matmania_colorram3[offs] & 0x08) << 5);
			dgp1.color = (matmania_colorram3[offs] & 0x30) >> 4;
			dgp1.flipy = sy >= 16;
			dgp1.sx = /* flip horizontally tiles on the right half of the bitmap */					16*sx;
			dgp1.sy = 16*sy;
			drawgfx(&dgp1);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scrolly;


		scrolly = -*matmania_scroll;
		if (*matmania_pageselect)
			copyscrollbitmap(bitmap,tmpbitmap2,0,0,1,&scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
		else
			copyscrollbitmap(bitmap,tmpbitmap,0,0,1,&scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}


	/* Draw the sprites */
	
	{ 
	struct drawgfxParams dgp2={
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
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		if (spriteram[offs] & 0x01)
		{
			
			dgp2.code = spriteram[offs+1] + ((spriteram[offs] & 0xf0) << 4);
			dgp2.color = (spriteram[offs] & 0x08) >> 3;
			dgp2.flipx = spriteram[offs] & 0x04;
			dgp2.flipy = spriteram[offs] & 0x02;
			dgp2.sx = 239 - spriteram[offs+3];
			dgp2.sy = (240 - spriteram[offs+2]) & 0xff;
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph



	/* draw the frontmost playfield. They are characters, but draw them as sprites */
	
	{ 
	struct drawgfxParams dgp3={
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
	for (offs = matmania_videoram2_size - 1;offs >= 0;offs--)
	{
		int sx,sy;


		sx = 31 - offs / 32;
		sy = offs % 32;

		
		dgp3.code = matmania_videoram2[offs] + 256 * (matmania_colorram2[offs] & 0x07);
		dgp3.color = (matmania_colorram2[offs] & 0x30) >> 4;
		dgp3.sx = 8*sx;
		dgp3.sy = 8*sy;
		drawgfx(&dgp3);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( maniach )
{
	int offs;


	/* Update the tiles in the left tile ram bank */
	
	{ 
	struct drawgfxParams dgp4={
		tmpbitmap, 	// dest
		Machine->gfx[1], 	// gfx
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

			sx = 15 - offs / 32;
			sy = offs % 32;

			
			dgp4.code = videoram[offs] + ((colorram[offs] & 0x03) << 8);
			dgp4.color = (colorram[offs] & 0x30) >> 4;
			dgp4.flipy = sy >= 16;
			dgp4.sx = /* flip horizontally tiles on the right half of the bitmap */					16*sx;
			dgp4.sy = 16*sy;
			drawgfx(&dgp4);
		}
	}
	} // end of patch paragraph


	/* Update the tiles in the right tile ram bank */
	
	{ 
	struct drawgfxParams dgp5={
		tmpbitmap2, 	// dest
		Machine->gfx[1], 	// gfx
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
	for (offs = matmania_videoram3_size - 1;offs >= 0;offs--)
	{
		if (dirtybuffer2[offs])
		{
			int sx,sy;


			dirtybuffer2[offs] = 0;

			sx = 15 - offs / 32;
			sy = offs % 32;

			
			dgp5.code = matmania_videoram3[offs] + ((matmania_colorram3[offs] & 0x03) << 8);
			dgp5.color = (matmania_colorram3[offs] & 0x30) >> 4;
			dgp5.flipy = sy >= 16;
			dgp5.sx = /* flip horizontally tiles on the right half of the bitmap */					16*sx;
			dgp5.sy = 16*sy;
			drawgfx(&dgp5);
		}
	}
	} // end of patch paragraph



	/* copy the temporary bitmap to the screen */
	{
		int scrolly;


		scrolly = -*matmania_scroll;
		if (*matmania_pageselect)
			copyscrollbitmap(bitmap,tmpbitmap2,0,0,1,&scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
		else
			copyscrollbitmap(bitmap,tmpbitmap,0,0,1,&scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}


	/* Draw the sprites */
	
	{ 
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
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		if (spriteram[offs] & 0x01)
		{
			
			dgp6.code = spriteram[offs+1] + ((spriteram[offs] & 0xf0) << 4);
			dgp6.color = (spriteram[offs] & 0x08) >> 3;
			dgp6.flipx = spriteram[offs] & 0x04;
			dgp6.flipy = spriteram[offs] & 0x02;
			dgp6.sx = 239 - spriteram[offs+3];
			dgp6.sy = (240 - spriteram[offs+2]) & 0xff;
			drawgfx(&dgp6);
		}
	}
	} // end of patch paragraph



	/* draw the frontmost playfield. They are characters, but draw them as sprites */
	
	{ 
	struct drawgfxParams dgp7={
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
	for (offs = matmania_videoram2_size - 1;offs >= 0;offs--)
	{
		int sx,sy;


		sx = 31 - offs / 32;
		sy = offs % 32;

		
		dgp7.code = matmania_videoram2[offs] + 256 * (matmania_colorram2[offs] & 0x07);
		dgp7.color = (matmania_colorram2[offs] & 0x30) >> 4;
		dgp7.sx = 8*sx;
		dgp7.sy = 8*sy;
		drawgfx(&dgp7);
	}
	} // end of patch paragraph

}
