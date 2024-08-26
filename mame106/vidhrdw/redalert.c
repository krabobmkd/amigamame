/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"

unsigned char *redalert_backram;
unsigned char *redalert_spriteram1;
unsigned char *redalert_spriteram2;
unsigned char *redalert_spriteram3;
unsigned char *redalert_characterram;
unsigned char *redalert_characterram2;

static unsigned char redalert_dirtyback[0x400];
static unsigned char redalert_dirtycharacter[0x100];
static unsigned char redalert_dirtycharacter2[0x100];
static unsigned char redalert_backcolor[0x400];


/* There might be a color PROM that dictates this? */
/* These guesses are based on comparing the color bars on the test
   screen with the picture in the manual */
static unsigned char color_lookup[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3,
	1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

	1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
};

static int backcolor, flip=0;

WRITE8_HANDLER( redalert_c040_w )
{
	/* Only seems to load D0-D3 into a flip-flop. */
	/* D0/D1 seem to head off to unconnected circuits */
	/* D2 connects to a "NL" line, and NOTted to a "NH" line */
	/* D3 connects to a "YI" line */

	/*
        D0 == 1             -> 1 player
        D1 == 1 and D0 == 1 -> 2 players
    */
	flip = !(data & 0x04);
}

WRITE8_HANDLER( redalert_backcolor_w )
{
	/* Only seems to load D0-D2 into a flip-flop. */
	/* Outputs feed into RAM which seems to feed to RGB lines. */
	backcolor = data & 0x07;
}

WRITE8_HANDLER( demoneye_c040_w )
{
	/*
        D0 == 1             -> 1 player
        D1 == 1 and D0 == 1 -> 2 players
    */
	flip = data & 0x04;
}

/***************************************************************************
redalert_backram_w
***************************************************************************/

WRITE8_HANDLER( redalert_backram_w )
{
	int charnum;

	charnum = offset / 8 % 0x400;

	if ((redalert_backram[offset] != data) ||
		(redalert_backcolor[charnum] != backcolor))
	{
		redalert_dirtyback[charnum] = 1;
		dirtybuffer[charnum] = 1;
		redalert_backcolor[charnum] = backcolor;

		redalert_backram[offset] = data;
	}
}

/***************************************************************************
redalert_spriteram1_w
***************************************************************************/

WRITE8_HANDLER( redalert_spriteram1_w )
{
	if (redalert_spriteram1[offset] != data)
	{
		redalert_dirtycharacter[((offset / 8) % 0x80) + 0x80] = 1;

		redalert_spriteram1[offset] = data;
	}
}

/***************************************************************************
redalert_spriteram2_w
***************************************************************************/

WRITE8_HANDLER( redalert_spriteram2_w )
{
	if (redalert_spriteram2[offset] != data)
	{

		redalert_dirtycharacter[((offset / 8) % 0x80) + 0x80] = 1;

		redalert_spriteram2[offset] = data;
	}
}

/***************************************************************************
redalert_characterram_w
***************************************************************************/

WRITE8_HANDLER( redalert_characterram_w )
{
	if (redalert_characterram[offset] != data)
	{
		redalert_dirtycharacter[((offset / 8) % 0x80)] = 1;

		redalert_characterram[offset] = data;
	}
}

WRITE8_HANDLER( redalert_characterram2_w )
{
	if (redalert_characterram2[offset] != data)
	{
		redalert_dirtycharacter[((offset / 8) % 0x80)] = 1;

		redalert_characterram2[offset] = data;
	}
}

WRITE8_HANDLER( redalert_spriteram3_w )
{
	if (redalert_spriteram3[offset] != data)
	{
		redalert_dirtycharacter2[((offset / 8) % 0x80) + 0x80] = 1;

		redalert_spriteram3[offset] = data;
	}

}


/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( redalert )
{
	int offs,i;

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	
	{ 
	struct drawgfxParams dgp0={
		tmpbitmap, 	// dest
		Machine->gfx[2], 	// gfx
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
	struct drawgfxParams dgp1={
		tmpbitmap, 	// dest
		Machine->gfx[3], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_COLOR, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
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
		0, //HAND stat_transparent, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	struct drawgfxParams dgp3={
		tmpbitmap, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_COLOR, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	struct drawgfxParams dgp4={
		tmpbitmap, 	// dest
		Machine->gfx[2], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_COLOR, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		int charcode;
		int stat_transparent;


		charcode = videoram[offs];

		if (dirtybuffer[offs] || redalert_dirtycharacter[charcode] || redalert_dirtycharacter2[charcode])
		{
			int sx,sy,color;


			/* decode modified background */
			if (redalert_dirtyback[offs] == 1)
			{
				decodechar(Machine->gfx[0],offs,redalert_backram,
							Machine->drv->gfxdecodeinfo[0].gfxlayout);
				redalert_dirtyback[offs] = 2;
			}

			/* decode modified characters */
			if (redalert_dirtycharacter[charcode] == 1)
			{
				if (charcode < 0x80)
					decodechar(Machine->gfx[1],charcode,redalert_characterram,
								Machine->drv->gfxdecodeinfo[1].gfxlayout);
				else
					decodechar(Machine->gfx[2],charcode-0x80,redalert_spriteram1,
								Machine->drv->gfxdecodeinfo[2].gfxlayout);
				redalert_dirtycharacter[charcode] = 2;
			}

			if (redalert_dirtycharacter2[charcode] == 1)
			{
				decodechar(Machine->gfx[3],charcode-0x80,redalert_spriteram3,
							Machine->drv->gfxdecodeinfo[3].gfxlayout);
				redalert_dirtycharacter2[charcode] = 2;
			}

			dirtybuffer[offs] = 0;

			sx = 31 - offs / 32;
			sy = offs % 32;

			stat_transparent = TRANSPARENCY_NONE;

			/* First layer of color */
			if (charcode >= 0xC0)
			{
				stat_transparent = TRANSPARENCY_COLOR;

				color = color_lookup[charcode];

				
				dgp0.code = charcode-0x80;
				dgp0.color = color;
				dgp0.sx = 8*sx;
				dgp0.sy = 8*sy;
				drawgfx(&dgp0);

				if( redalert_dirtycharacter2[charcode] != 0 )
				{
					dgp1.code = charcode-0x80;
					dgp1.color = color;
					dgp1.sx = 8*sx;
					dgp1.sy = 8*sy;
					drawgfx(&dgp1);
                }

			}

			/* Second layer - background */
			color = redalert_backcolor[offs];
			dgp2.transparency = stat_transparent;
			dgp2.code = offs;
			dgp2.color = color;
			dgp2.sx = 8*sx;
			dgp2.sy = 8*sy;
			drawgfx(&dgp2);

			/* Third layer - alphanumerics & sprites */
			if (charcode < 0x80)
			{
				color = color_lookup[charcode];
				
				dgp3.code = charcode;
				dgp3.color = color;
				dgp3.sx = 8*sx;
				dgp3.sy = 8*sy;
				drawgfx(&dgp3);
			}
			else if (charcode < 0xC0)
			{
				color = color_lookup[charcode];
				
				dgp4.code = charcode-0x80;
				dgp4.color = color;
				dgp4.sx = 8*sx;
				dgp4.sy = 8*sy;
				drawgfx(&dgp4);

				if( redalert_dirtycharacter2[charcode] != 0 )
				{
					dgp1.code = charcode-0x80;
					dgp1.color = color;
					dgp1.sx = 8*sx;
					dgp1.sy = 8*sy;
					drawgfx(&dgp1);
                }

			}

		}
	}
	} // end of patch paragraph


	for (i = 0;i < 256;i++)
	{
		if (redalert_dirtycharacter[i] == 2)
			redalert_dirtycharacter[i] = 0;

		if (redalert_dirtycharacter2[i] == 2)
			redalert_dirtycharacter2[i] = 0;
	}

	for (i = 0;i < 0x400;i++)
	{
		if (redalert_dirtyback[i] == 2)
			redalert_dirtyback[i] = 0;
	}

	/* copy the character mapped graphics */
	copybitmap(bitmap,tmpbitmap,flip,flip,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);

}
