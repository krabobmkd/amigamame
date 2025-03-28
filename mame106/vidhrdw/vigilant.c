/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

  TODO:

  - Get a dump of the PAL16L8 (IC38 - 4M).  This controls transparency.
    It takes as input 4 bits of sprite color, and 6 bits of background tile
    color.  It outputs a sprite enable line, a background enable line, and
    a background select (which layer of background to draw).
  - Add cocktail flipping.

***************************************************************************/

#include "driver.h"


static rectangle bottomvisiblearea =
{
	16*8, 48*8-1,
	6*8, 32*8-1
};

unsigned char *vigilant_paletteram;
unsigned char *vigilant_sprite_paletteram;

static int horiz_scroll_low=0;
static int horiz_scroll_high=0;
static int rear_horiz_scroll_low=0;
static int rear_horiz_scroll_high=0;
static int rear_color=0;
static int rear_disable=1;

static int rear_refresh=1;

static mame_bitmap *bg_bitmap;


VIDEO_START( vigilant )
{
	video_start_generic();

	if ((bg_bitmap = auto_bitmap_alloc(512*4,256)) == 0)
		return 1;

	return 0;
}



/***************************************************************************
 update_background

 There are three background ROMs, each one contains a 512x256 picture.
 Redraw them if the palette changes.
 **************************************************************************/
static void update_background( void )
{
	int row,col,page;
	int charcode;


	charcode=0;

	/* There are only three background ROMs (4 on bunccaneers!) */
	
	{ 
	struct drawgfxParams dgp0={
		bg_bitmap, 	// dest
		Machine->gfx[2], 	// gfx
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
	for (page=0; page<4; page++)
	{
		for( row=0; row<256; row++ )
		{
			for( col=0; col<512; col+=32 )
			{
				
				dgp0.code = charcode;
				dgp0.color = row < 128 ? 0 : 1;
				dgp0.sx = 512*page + col;
				dgp0.sy = row;
				drawgfx(&dgp0);
				charcode++;
			}
		}
	}
	} // end of patch paragraph

}

/***************************************************************************
 vigilant_paletteram_w

 There are two palette chips, each one is labelled "KNA91H014".  One is
 used for the sprites, one is used for the two background layers.

 The chip has three enables (!CS, !E0, !E1), R/W pins, A0-A7 as input,
 'L' and 'H' inputs, and D0-D4 as input.  'L' and 'H' are used to bank
 into Red, Green, and Blue memory.  There are only 5 bits of memory for
 each byte, and a total of 256*3 bytes memory per chip.

 There are additionally two sets of D0-D7 inputs per chip labelled 'A'
 and 'B'.  There is also an 'S' pin to select between the two input sets.
 These are used to index a color triplet of RGB.  The triplet is read
 from RAM, and output to R0-R4, G0-G4, and B0-B4.
 **************************************************************************/
WRITE8_HANDLER( vigilant_paletteram_w )
{
	int bank,r,g,b;


	paletteram[offset] = data;

	bank = offset & 0x400;
	offset &= 0xff;

	r = (paletteram[bank + offset + 0x000] << 3) & 0xFF;
	g = (paletteram[bank + offset + 0x100] << 3) & 0xFF;
	b = (paletteram[bank + offset + 0x200] << 3) & 0xFF;

	palette_set_color((bank >> 2) + offset,r,g,b);
}



/***************************************************************************
 vigilant_horiz_scroll_w

 horiz_scroll_low  = HSPL, an 8-bit register
 horiz_scroll_high = HSPH, a 1-bit register
 **************************************************************************/
WRITE8_HANDLER( vigilant_horiz_scroll_w )
{
	if (offset==0)
		horiz_scroll_low = data;
	else
		horiz_scroll_high = (data & 0x01) * 256;
}

/***************************************************************************
 vigilant_rear_horiz_scroll_w

 rear_horiz_scroll_low  = RHSPL, an 8-bit register
 rear_horiz_scroll_high = RHSPH, an 8-bit register but only 3 bits are saved
***************************************************************************/
WRITE8_HANDLER( vigilant_rear_horiz_scroll_w )
{
	if (offset==0)
		rear_horiz_scroll_low = data;
	else
		rear_horiz_scroll_high = (data & 0x07) * 256;
}

/***************************************************************************
 vigilant_rear_color_w

 This is an 8-bit register labelled RCOD.
 D6 is hooked to !ROME (rear_disable)
 D3 = RCC2 (rear color bit 2)
 D2 = RCC1 (rear color bit 1)
 D0 = RCC0 (rear color bit 0)

 I know it looks odd, but D1, D4, D5, and D7 are empty.

 What makes this extremely odd is that RCC is supposed to hook up to the
 palette.  However, the top four bits of the palette inputs are labelled:
 "RCC3", "RCC2", "V256E", "RCC0".  Methinks there's a typo.
 **************************************************************************/
WRITE8_HANDLER( vigilant_rear_color_w )
{
	rear_disable = data & 0x40;
	rear_color = (data & 0x0d);
}

/***************************************************************************
 draw_foreground

 ???
 **************************************************************************/

static void draw_foreground( mame_bitmap *bitmap, int priority, int opaque )
{
	int offs;
	int scroll = -(horiz_scroll_low + horiz_scroll_high);


	for (offs = 0; offs<videoram_size; offs+=2 )
	{
		int sy = 8 * ((offs/2) / 64);
		int sx = 8 * ((offs/2) % 64);
		int attributes = videoram[offs+1];
		int color = attributes & 0x0F;
		int tile_number = videoram[offs] | ((attributes & 0xF0) << 4);

		if (priority)	 /* foreground */
		{
			if ((color & 0x0c) == 0x0c)	/* mask sprites */
			{
				
				{ 
				struct drawgfxParams dgp1={
					bitmap, 	// dest
					Machine->gfx[0], 	// gfx
					0, 	// code
					0, 	// color
					0, 	// flipx
					0, 	// flipy
					0, 	// sx
					0, 	// sy
					&bottomvisiblearea, 	// clip
					TRANSPARENCY_PENS, 	// transparency
					0x00ff, 	// transparent_color
					0, 	// scalex
					0, 	// scaley
					NULL, 	// pri_buffer
					0 	// priority_mask
				  };
				if (sy >= 48)
				{
					sx = (sx + scroll) & 0x1ff;

					
					dgp1.code = tile_number;
					dgp1.color = color;
					dgp1.sx = sx;
					dgp1.sy = sy;
					drawgfx(&dgp1);
				}
				} // end of patch paragraph

			}
		}
		else	 /* background */
		
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
	(opaque || color >= 4) ? TRANSPARENCY_NONE : TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			if (sy >= 48)
				sx = (sx + scroll) & 0x1ff;

			
			dgp2.code = tile_number;
			dgp2.color = color;
			dgp2.sx = sx;
			dgp2.sy = sy;
			drawgfx(&dgp2);
		}
} // end of patch paragraph

	}
}



/***************************************************************************
 draw_background

 ???
 **************************************************************************/
static void draw_background( mame_bitmap *bitmap )
{
	int scrollx = 0x17a + 16*8 - (rear_horiz_scroll_low + rear_horiz_scroll_high);


	if (rear_refresh)
	{
		update_background( );
		rear_refresh=0;
	}

	copyscrollbitmap(bitmap,bg_bitmap,1,&scrollx,0,0,&bottomvisiblearea,TRANSPARENCY_NONE,0);
}

/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap,const rectangle *clip)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp3={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		clip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size;offs += 8)
	{
		int code,color,sx,sy,flipx,flipy,h,y;

		code = spriteram[offs+4] | ((spriteram[offs+5] & 0x0f) << 8);
		color = spriteram[offs+0] & 0x0f;
		sx = (spriteram[offs+6] | ((spriteram[offs+7] & 0x01) << 8));
		sy = 256+128 - (spriteram[offs+2] | ((spriteram[offs+3] & 0x01) << 8));
		flipx = spriteram[offs+5] & 0x40;
		flipy = spriteram[offs+5] & 0x80;
		h = 1 << ((spriteram[offs+5] & 0x30) >> 4);
		sy -= 16 * h;

		code &= ~(h - 1);

		for (y = 0;y < h;y++)
		{
			int c = code;

			if (flipy) c += h-1-y;
			else c += y;

			
			dgp3.code = c;
			dgp3.color = color;
			dgp3.flipx = flipx;
			dgp3.flipy = flipy;
			dgp3.sx = sx;
			dgp3.sy = sy + 16*y;
			drawgfx(&dgp3);
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( vigilant )
{
	int i;

	/* copy the background palette */
	for (i = 0;i < 16;i++)
	{
		int r,g,b;


		r = (paletteram[0x400 + 16 * rear_color + i] << 3) & 0xFF;
		g = (paletteram[0x500 + 16 * rear_color + i] << 3) & 0xFF;
		b = (paletteram[0x600 + 16 * rear_color + i] << 3) & 0xFF;

		palette_set_color(512 + i,r,g,b);

		r = (paletteram[0x400 + 16 * rear_color + 32 + i] << 3) & 0xFF;
		g = (paletteram[0x500 + 16 * rear_color + 32 + i] << 3) & 0xFF;
		b = (paletteram[0x600 + 16 * rear_color + 32 + i] << 3) & 0xFF;

		palette_set_color(512 + 16 + i,r,g,b);
	}

	if (rear_disable)	 /* opaque foreground */
	{
		draw_foreground(bitmap,0,1);
		draw_sprites(bitmap,&bottomvisiblearea);
		draw_foreground(bitmap,1,0);
	}
	else
	{
		draw_background(bitmap);
		draw_foreground(bitmap,0,0);
		draw_sprites(bitmap,&bottomvisiblearea);
		draw_foreground(bitmap,1,0); // priority tiles
	}
}

VIDEO_UPDATE( kikcubic )
{
	int offs;


	
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
	for (offs = 0; offs<videoram_size; offs+=2 )
	{
		int sy = 8 * ((offs/2) / 64);
		int sx = 8 * ((offs/2) % 64);
		int attributes = videoram[offs+1];
		int color = (attributes & 0xF0) >> 4;
		int tile_number = videoram[offs] | ((attributes & 0x0F) << 8);

		if (dirtybuffer[offs] || dirtybuffer[offs+1])
		{
			dirtybuffer[offs] = dirtybuffer[offs+1] = 0;

			
			dgp4.code = tile_number;
			dgp4.color = color;
			dgp4.sx = sx;
			dgp4.sy = sy;
			drawgfx(&dgp4);
		}
	}
	} // end of patch paragraph


	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);

	draw_sprites(bitmap,&Machine->visible_area);
}
