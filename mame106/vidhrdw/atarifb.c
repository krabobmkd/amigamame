/*************************************************************************

    Atari Football hardware

*************************************************************************/

#include "driver.h"
#include "atarifb.h"

/* local */
size_t atarifb_alphap1_vram_size;
size_t atarifb_alphap2_vram_size;
unsigned char *atarifb_alphap1_vram;
unsigned char *atarifb_alphap2_vram;
unsigned char *atarifb_scroll_register;


static const rectangle bigfield_area = {  4*8, 34*8-1, 0*8, 32*8-1 };
static const rectangle left_area =     {  0*8,  3*8-1, 0*8, 32*8-1 };
static const rectangle right_area =    { 34*8, 38*8-1, 0*8, 32*8-1 };

/***************************************************************************
***************************************************************************/
WRITE8_HANDLER( atarifb_alphap1_vram_w )
{
	atarifb_alphap1_vram[offset] = data;
}

WRITE8_HANDLER( atarifb_alphap2_vram_w )
{
	atarifb_alphap2_vram[offset] = data;
}

/***************************************************************************
***************************************************************************/
WRITE8_HANDLER( atarifb_scroll_w )
{
	if (data - 8 != *atarifb_scroll_register)
	{
		*atarifb_scroll_register = data - 8;
		memset(dirtybuffer,1,videoram_size);
	}
}

/***************************************************************************
***************************************************************************/

VIDEO_START( atarifb )
{
	if (video_start_generic())
		return 1;

	memset(dirtybuffer, 1, videoram_size);

	return 0;
}

/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

VIDEO_UPDATE( atarifb )
{
	int offs,obj;
	int sprite_bank;

	if (get_vh_global_attribute_changed())
		memset(dirtybuffer,1,videoram_size);

	/* Soccer uses a different graphics set for sprites */
	if (atarifb_game == 4)
		sprite_bank = 2;
	else
		sprite_bank = 1;

	/* for every character in the Player 1 Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	for (offs = atarifb_alphap1_vram_size - 1;offs >= 0;offs--)
	{
		int charcode;
		int flipbit;
		int disable;
		int sx,sy;

		sx = 8 * (offs / 32) + 35*8;
		sy = 8 * (offs % 32) + 8;

		charcode = atarifb_alphap1_vram[offs] & 0x3f;
		flipbit = (atarifb_alphap1_vram[offs] & 0x40) >> 6;
		disable = (atarifb_alphap1_vram[offs] & 0x80) >> 7;

		if (!disable)
		
{ 
struct drawgfxParams dgp0={
	bitmap, 	// dest
	Machine->gfx[0], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&right_area, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp0.code = charcode;
			dgp0.flipx = flipbit;
			dgp0.flipy = flipbit;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}
} // end of patch paragraph

	}

	/* for every character in the Player 2 Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	for (offs = atarifb_alphap2_vram_size - 1;offs >= 0;offs--)
	{
		int charcode;
		int flipbit;
		int disable;
		int sx,sy;

		sx = 8 * (offs / 32);
		sy = 8 * (offs % 32) + 8;

		charcode = atarifb_alphap2_vram[offs] & 0x3f;
		flipbit = (atarifb_alphap2_vram[offs] & 0x40) >> 6;
		disable = (atarifb_alphap2_vram[offs] & 0x80) >> 7;

		if (!disable)
		
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
	&left_area, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp1.code = charcode;
			dgp1.flipx = flipbit;
			dgp1.flipy = flipbit;
			dgp1.sx = sx;
			dgp1.sy = sy;
			drawgfx(&dgp1);
		}
} // end of patch paragraph

	}

	/* for every character in the Video RAM, check if it has been modified */
	/* since last time and update it accordingly. */
	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		if (dirtybuffer[offs])
		
{ 
struct drawgfxParams dgp2={
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
{
			int charcode;
			int flipx,flipy;
			int sx,sy;

			dirtybuffer[offs]=0;

			charcode = videoram[offs] & 0x3f;
			flipx = (videoram[offs] & 0x40) >> 6;
			flipy = (videoram[offs] & 0x80) >> 7;

			sx = (8 * (offs % 32) - *atarifb_scroll_register);
			sy = 8 * (offs / 32) + 8;

			/* Soccer hack */
			if (atarifb_game == 4)
			{
				sy += 8;
			}

			/* Baseball hack */
			if (atarifb_game == 0x03) sx -= 8;

			if (sx < 0) sx += 256;
			if (sx > 255) sx -= 256;

			
			dgp2.code = charcode;
			dgp2.flipx = flipx;
			dgp2.flipy = flipy;
			dgp2.sx = sx;
			dgp2.sy = sy;
			drawgfx(&dgp2);
		}
} // end of patch paragraph

	}

	/* copy the character mapped graphics */
	copybitmap(bitmap,tmpbitmap,0,0,8*3,0,&bigfield_area,TRANSPARENCY_NONE,0);

	/* Draw our motion objects */
	
	{ 
	struct drawgfxParams dgp4={
		bitmap, 	// dest
		Machine->gfx[sprite_bank], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&bigfield_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (obj=0;obj<16;obj++)
	{
		int charcode;
		int flipx,flipy;
		int sx,sy;
		int shade = 0;

		sy = 255 - spriteram[obj*2 + 1];
		if (sy == 255) continue;

		charcode = spriteram[obj*2] & 0x3f;
		flipx = (spriteram[obj*2] & 0x40);
		flipy = (spriteram[obj*2] & 0x80);
		sx = spriteram[obj*2 + 0x20] + 8*3;

		/* Note on Atari Soccer: */
		/* There are 3 sets of 2 bits each, where the 2 bits represent */
		/* black, dk grey, grey and white. I think the 3 sets determine the */
		/* color of each bit in the sprite, but I haven't implemented it that way. */
		if (atarifb_game == 4)
		
{ 
struct drawgfxParams dgp3={
	bitmap, 	// dest
	Machine->gfx[sprite_bank+1], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&bigfield_area, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			shade = ((spriteram[obj*2+1 + 0x20]) & 0x07);

			
			dgp3.code = charcode;
			dgp3.color = shade;
			dgp3.flipx = flipx;
			dgp3.flipy = flipy;
			dgp3.sx = sx;
			dgp3.sy = sy;
			drawgfx(&dgp3);

			shade = ((spriteram[obj*2+1 + 0x20]) & 0x08) >> 3;
		}
} // end of patch paragraph


		
		dgp4.code = charcode;
		dgp4.color = shade;
		dgp4.flipx = flipx;
		dgp4.flipy = flipy;
		dgp4.sx = sx;
		dgp4.sy = sy;
		drawgfx(&dgp4);

		/* If this isn't soccer, handle the multiplexed sprites */
		if (atarifb_game != 4)
		{
			/* The down markers are multiplexed by altering the y location during */
			/* mid-screen. We'll fake it by essentially doing the same thing here. */
			if ((charcode == 0x11) && (sy == 0x07))
			
{ 
struct drawgfxParams dgp5={
	bitmap, 	// dest
	Machine->gfx[sprite_bank], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&bigfield_area, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
				sy = 0xf1; /* When multiplexed, it's 0x10...why? */
				
				dgp5.code = charcode;
				dgp5.flipx = flipx;
				dgp5.flipy = flipy;
				dgp5.sx = sx;
				dgp5.sy = sy;
				drawgfx(&dgp5);
			}
} // end of patch paragraph

		}
	}
	} // end of patch paragraph


/* If this isn't Soccer, print the plays at the top of the screen */
if (atarifb_game != 4)
{
	char buf1[25], buf2[25];

	switch (atarifb_game)
	{
		case 0x01: /* 2-player football */
			switch (atarifb_lamp1)
			{
				case 0x00:
					sprintf (buf1, "                    ");
					break;
				case 0x01:
					sprintf (buf1, "SWEEP               ");
					break;
				case 0x02:
					sprintf (buf1, "KEEPER              ");
					break;
				case 0x04:
					sprintf (buf1, "BOMB                ");
					break;
				case 0x08:
					sprintf (buf1, "DOWN & OUT          ");
					break;
			}
			switch (atarifb_lamp2)
			{
				case 0x00:
					sprintf (buf2, "                    ");
					break;
				case 0x01:
					sprintf (buf2, "               SWEEP");
					break;
				case 0x02:
					sprintf (buf2, "              KEEPER");
					break;
				case 0x04:
					sprintf (buf2, "                BOMB");
					break;
				case 0x08:
					sprintf (buf2, "          DOWN & OUT");
					break;
			}
			break;
		case 0x02: /* 4-player football */
			switch (atarifb_lamp1 & 0x1f)
			{
				case 0x01:
					sprintf (buf1, "SLANT OUT           ");
					break;
				case 0x02:
					sprintf (buf1, "SLANT IN            ");
					break;
				case 0x04:
					sprintf (buf1, "BOMB                ");
					break;
				case 0x08:
					sprintf (buf1, "DOWN & OUT          ");
					break;
				case 0x10:
					sprintf (buf1, "KICK                ");
					break;
				default:
					sprintf (buf1, "                    ");
					break;
			}
			switch (atarifb_lamp2 & 0x1f)
			{
				case 0x01:
					sprintf (buf2, "           SLANT OUT");
					break;
				case 0x02:
					sprintf (buf2, "            SLANT IN");
					break;
				case 0x04:
					sprintf (buf2, "                BOMB");
					break;
				case 0x08:
					sprintf (buf2, "          DOWN & OUT");
					break;
				case 0x10:
					sprintf (buf2, "                KICK");
					break;
				default:
					sprintf (buf2, "                    ");
					break;
			}
			break;
		case 0x03: /* 2-player baseball */
			switch (atarifb_lamp1 & 0x0f)
			{
				case 0x01:
					sprintf (buf1, "RT SWING/FASTBALL   ");
					break;
				case 0x02:
					sprintf (buf1, "LT SWING/CHANGE-UP  ");
					break;
				case 0x04:
					sprintf (buf1, "RT BUNT/CURVE BALL  ");
					break;
				case 0x08:
					sprintf (buf1, "LT BUNT/KNUCKLE BALL");
					break;
				default:
					sprintf (buf1, "                    ");
					break;
			}
			switch (atarifb_lamp2 & 0x0f)
			{
				case 0x01:
					sprintf (buf2, "   RT SWING/FASTBALL");
					break;
				case 0x02:
					sprintf (buf2, "  LT SWING/CHANGE-UP");
					break;
				case 0x04:
					sprintf (buf2, "  RT BUNT/CURVE BALL");
					break;
				case 0x08:
					sprintf (buf2, "LT BUNT/KNUCKLE BALL");
					break;
				default:
					sprintf (buf2, "                    ");
					break;
			}
			break;
		default:
			sprintf (buf1, "                    ");
			sprintf (buf2, "                    ");
			break;
	}
	ui_draw_text(buf1, 24*8, 0);
	ui_draw_text(buf2, 0, 0);
}
}
