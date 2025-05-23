/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"


static int video_control;
static int colour_control;

extern unsigned char spacefb_sound_latch;


/***************************************************************************

  Convert the color PROMs into a more useable format.

  Space FB has one 32 bytes palette PROM, connected to the RGB output this
  way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/
PALETTE_INIT( spacefb )
{
	int i;

	for (i = 0;i < 32;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = 0;
		bit1 = (color_prom[i] >> 6) & 0x01;
		bit2 = (color_prom[i] >> 7) & 0x01;
		if (bit1 | bit2)
			bit0 = 1;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		palette_set_color(i,r,g,b);
	}

	for (i = 0;i < 4 * 8;i++)
	{
		if (i & 3) colortable[i] = i;
		else colortable[i] = 0;
	}
	// RGB background colour flash.. Only red background is actually used
	palette_set_color(32,0x47,0,0);			/* Red */
	palette_set_color(33,0,0,0x47);			/* Blue */
	palette_set_color(34,0x47,0,0x47);		/* Red+Blue */
	palette_set_color(35,0x47,0x47,0x47);	/* White */

}


WRITE8_HANDLER( spacefb_video_control_w )
{
	flip_screen_set(data & 0x01);

	video_control = data;
}


WRITE8_HANDLER( spacefb_port_2_w )
{
logerror("Port #2 = %02d\n",data);
	colour_control = data;
}


/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

VIDEO_UPDATE( spacefb )
{
	int offs;
	int spriteno, col_bit2;
	int background_colour;

	/*
        Space Firebird only uses the red background flashing, but since we are documenting the hardware...

    */

	/* Clear the bitmap */
	background_colour=0;
	switch(colour_control & 0x18)
	{
		case 0x08 : /* ALRD */
			background_colour = 32;		/* RED */
			break;
		case 0x10 : /* ALBU */
			background_colour = 33;		/* BLUE */
			break;
		case 0x18 : /* ALRD+ALBU */
			background_colour = 34;		/* RED+BLUE */
			break;
		default :
			background_colour = 0;		/* BLACK */
			break;
	}
	if (colour_control & 0x80)		/* ALBA - All white?? Stars on/off?? */
	{
		background_colour = 35;			/* WHITE */
	}

	fillbitmap(bitmap,Machine->pens[background_colour],&Machine->visible_area);

	/* Draw the sprite/chars */
	spriteno = (video_control & 0x20) ? 0x80 : 0x00;

	/* A4 of the color PROM, depending on a jumper setting, can either come
       from the CREF line or from sprite memory. CREF is the default
       according to the schematics */
	col_bit2 = (video_control & 0x40) ? 0x04 : 0x00;

	
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
	struct drawgfxParams dgp1={
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
	for (offs = 0; offs < 128; offs++, spriteno++)
	{
		int sx,sy,code,cnt,col;


		sx = 255 - videoram[spriteno];
		sy = videoram[spriteno+0x100];

		code = videoram[spriteno+0x200];
		cnt  = videoram[spriteno+0x300];

		col = (~cnt & 0x03) | col_bit2;

		if (cnt)
		{
			if (cnt & 0x20)
			{
				/* Draw bullets */

				if (flip_screen)
				{
					sx = 260 - sx;
					sy = 252 - sy;
				}

				
				dgp0.code = code & 0x3f;
				dgp0.color = col;
				dgp0.flipx = flip_screen;
				dgp0.flipy = flip_screen;
				dgp0.sx = sx;
				dgp0.sy = sy;
				drawgfx(&dgp0);

			}
			else if (cnt & 0x40)
			{
				sy -= 5;	/* aligns the spaceship and the bullet */

				if (flip_screen)
				{
					sx = 256 - sx;
					sy = 248 - sy;
				}

				
				dgp1.code = 255 - code;
				dgp1.color = col;
				dgp1.flipx = flip_screen;
				dgp1.flipy = flip_screen;
				dgp1.sx = sx;
				dgp1.sy = sy;
				drawgfx(&dgp1);
			}
		}
	}
	} // end of patch paragraph

#if 0
	{
		int b;
		
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
			128+(7-b)*8, 	// clip
			&Machine->visible_area, 	// transparency
			TRANSPARENCY_NONE, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (b=0;b<8;b++)
		{
			
			dgp2.code = // 255 - b;
			dgp2.color = 255 - ( (colour_control & (1<<(7-b))) ? 6 : 5);
			drawgfx(&dgp2);

		}
		} // end of patch paragraph

		
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
			128+(7-b)*8, 	// clip
			&Machine->visible_area, 	// transparency
			TRANSPARENCY_NONE, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (b=0;b<8;b++)
		{
			
			dgp3.code = // 255 - b;
			dgp3.color = 255 - ( (spacefb_sound_latch & (1<<(7-b))) ? (7-b)+5 : 0);
			dgp3.sy = 32+8;
			drawgfx(&dgp3);

		}
		} // end of patch paragraph

	}
#endif
}
