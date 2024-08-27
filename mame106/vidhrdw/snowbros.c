/***************************************************************************

  Snow Brothers by ToaPlan - GFX driver.
  GFX processor - PX79C480FP-3 (KANEKO, Pandora-Chip)

***************************************************************************/

#include "driver.h"
#include "drawgfx.h"

WRITE16_HANDLER( snowbros_flipscreen_w )
{
	if (ACCESSING_MSB)
		flip_screen_set(~data & 0x8000);
}


VIDEO_UPDATE( snowbros )
{
	int sx=0, sy=0, x=0, y=0, offs;


	/*
     * Sprite Tile Format
     * ------------------
     *
     * Byte | Bit(s)   | Use
     * -----+-76543210-+----------------
     *  0-5 | -------- | ?
     *  6   | -------- | ?
     *  7   | xxxx.... | Palette Bank
     *  7   | .......x | XPos - Sign Bit
     *  9   | xxxxxxxx | XPos
     *  7   | ......x. | YPos - Sign Bit
     *  B   | xxxxxxxx | YPos
     *  7   | .....x.. | Use Relative offsets
     *  C   | -------- | ?
     *  D   | xxxxxxxx | Sprite Number (low 8 bits)
     *  E   | -------- | ?
     *  F   | ....xxxx | Sprite Number (high 4 bits)
     *  F   | x....... | Flip Sprite Y-Axis
     *  F   | .x...... | Flip Sprite X-Axis
     */

	/* This clears & redraws the entire screen each pass */

	fillbitmap(bitmap,0xf0,&Machine->visible_area);

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		UINT16 dx = spriteram16[offs+4] & 0xff;
		UINT16 dy = spriteram16[offs+5] & 0xff;
		UINT16 tilecolour = spriteram16[offs+3];
		UINT16 attr = spriteram16[offs+7];
		int flipx =   (attr & 0x80)!=0;
		int flipy =  (attr & 0x40)!=0;
		int tile  = ((attr & 0x3f) << 8) + (spriteram16[offs+6] & 0xff);

		if (tilecolour & 1) dx = -1 - (dx ^ 0xff);
		if (tilecolour & 2) dy = -1 - (dy ^ 0xff);
		if (tilecolour & 4)
		{
			x += dx;
			y += dy;
		}
		else
		{
			x = dx;
			y = dy;
		}

		if (x > 511) x &= 0x1ff;
		if (y > 511) y &= 0x1ff;

		if (flip_screen)
		{
			sx = 240 - x;
			sy = 240 - y;
			flipx = !flipx;
			flipy = !flipy;
		}
		else
		{
			sx = x;
			sy = y;
		}

		
		dgp0.code = tile;
		dgp0.color = (tilecolour & 0xf0) >> 4;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = sx;
		dgp0.sy = sy;

		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( honeydol )
{
	int sx=0, sy=0, x=0, y=0, offs;

	/* not standard snowbros video */

	fillbitmap(bitmap,0xf0,&Machine->visible_area);

	
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
	for (offs = 0x0000/2;offs < 0x2000/2;offs += 8)
	{
		int dx = (spriteram16[offs+4]>>8) & 0xff;
		int dy = (spriteram16[offs+5]>>8) & 0xff;
		int tilecolour = (spriteram16[offs+3]>>8)&3;
		int attr = spriteram16[offs+7]>>8;
		int flipx =   attr & 0x80;
		int flipy =  (attr & 0x40) << 1;
		int tile  = ((attr & 0x3f)<<8) + ((spriteram16[offs+6]>>8) & 0xff);

		x = dx;
		y = dy;

		if (x > 511) x &= 0x1ff;
		if (y > 511) y &= 0x1ff;

		if (flip_screen)
		{
			sx = 240 - x;
			sy = 240 - y;
			flipx = !flipx;
			flipy = !flipy;
		}
		else
		{
			sx = x;
			sy = y;
		}

		
		dgp1.code = tile;
		dgp1.color = tilecolour;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = sx;
		dgp1.sy = sy;
		drawgfx(&dgp1);

		/* second list interleaved with first ??? */
		dx = spriteram16[offs+4] & 0xff;
		dy = spriteram16[offs+5] & 0xff;
		tilecolour = spriteram16[offs+3];
		attr = spriteram16[offs+7];
		flipx =   attr & 0x80;
		flipy =  (attr & 0x40) << 1;
		tile  = ((attr & 0x3f) << 8) + (spriteram16[offs+6] & 0xff);

		x = dx;
		y = dy;

		if (x > 511) x &= 0x1ff;
		if (y > 511) y &= 0x1ff;

		if (flip_screen)
		{
			sx = 240 - x;
			sy = 240 - y;
			flipx = !flipx;
			flipy = !flipy;
		}
		else
		{
			sx = x;
			sy = y;
		}

		
		dgp2.code = tile;
		dgp2.color = (tilecolour & 0xf0) >> 4;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = sx;
		dgp2.sy = sy;
		drawgfx(&dgp2);

	}
	} // end of patch paragraph

}


VIDEO_UPDATE( wintbob )
{
	int offs;

	fillbitmap(bitmap,get_black_pen(),&Machine->visible_area);

	
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
	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		int xpos  = spriteram16[offs] & 0xff;
		int ypos  = spriteram16[offs+4] & 0xff;
/*      int unk1  = spriteram16[offs+1] & 0x01;*/  /* Unknown .. Set for the Bottom Left part of Sprites */
		int disbl = spriteram16[offs+1] & 0x02;
/*      int unk2  = spriteram16[offs+1] & 0x04;*/  /* Unknown .. Set for most things */
		int wrapr = spriteram16[offs+1] & 0x08;
		int colr  = (spriteram16[offs+1] & 0xf0) >> 4;
		int tilen = (spriteram16[offs+2] << 8) + (spriteram16[offs+3] & 0xff);
		int flipx = spriteram16[offs+2] & 0x80;
		int flipy = (spriteram16[offs+2] & 0x40) << 1;

		if (wrapr == 8) xpos -= 256;

		if (flip_screen)
		{
			xpos = 240 - xpos;
			ypos = 240 - ypos;
			flipx = !flipx;
			flipy = !flipy;
		}

		if ((xpos > -16) && (ypos > 0) && (xpos < 256) && (ypos < 240) && (disbl !=2))
		{
			
			dgp3.code = tilen;
			dgp3.color = colr;
			dgp3.flipx = flipx;
			dgp3.flipy = flipy;
			dgp3.sx = xpos;
			dgp3.sy = ypos;
			drawgfx(&dgp3);
		}
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( snowbro3 )
{

	int sx=0, sy=0, x=0, y=0, offs;

	/*
     * Sprite Tile Format
     * ------------------
     *
     * Byte | Bit(s)   | Use
     * -----+-76543210-+----------------
     *  0-5 | -------- | ?
     *  6   | -------- | ?
     *  7   | xxxx.... | Palette Bank
     *  7   | .......x | XPos - Sign Bit
     *  9   | xxxxxxxx | XPos
     *  7   | ......x. | YPos - Sign Bit
     *  B   | xxxxxxxx | YPos
     *  7   | .....x.. | Use Relative offsets
     *  C   | -------- | ?
     *  D   | xxxxxxxx | Sprite Number (low 8 bits)
     *  E   | -------- | ?
     *  F   | ....xxxx | Sprite Number (high 4 bits)
     *  F   | x....... | Flip Sprite Y-Axis
     *  F   | .x...... | Flip Sprite X-Axis
     */

	/* This clears & redraws the entire screen each pass */

	fillbitmap(bitmap,get_black_pen(),&Machine->visible_area);

	
	{ 
	struct drawgfxParams dgp4={
		bitmap, 	// dest
		NULL, // hand done gfx, 	// gfx
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
	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		gfx_element *gfx = Machine->gfx[0];
		UINT16 dx = spriteram16[offs+4] & 0xff;
		UINT16 dy = spriteram16[offs+5] & 0xff;
		UINT16 tilecolour = spriteram16[offs+3];
		UINT16 attr = spriteram16[offs+7];
		int flipx =   attr & 0x80;
		int flipy =  (attr & 0x40) << 1;
		int tile  = ((attr & 0xff) << 8) + (spriteram16[offs+6] & 0xff);

		if (tilecolour & 1) dx = -1 - (dx ^ 0xff);
		if (tilecolour & 2) dy = -1 - (dy ^ 0xff);
		if (tilecolour & 4)
		{
			x += dx;
			y += dy;
		}
		else
		{
			x = dx;
			y = dy;
		}

		if (x > 511) x &= 0x1ff;
		if (y > 511) y &= 0x1ff;

		if (flip_screen)
		{
			sx = 240 - x;
			sy = 240 - y;
			flipx = !flipx;
			flipy = !flipy;
		}
		else
		{
			sx = x;
			sy = y;
		}

		if (offs < 0x800) /* i guess this is the right way */
		{
			gfx = Machine->gfx[1];
			tilecolour = 0x10;
		}

        dgp4.gfx = gfx;
        dgp4.code = tile;
        dgp4.color = (tilecolour & 0xf0) >> 4;
        dgp4.flipx = flipx;
        dgp4.flipy = flipy;
        dgp4.sx = sx;
        dgp4.sy = sy;
        drawgfx(&dgp4);
	}
	} // end of patch paragraph

}

