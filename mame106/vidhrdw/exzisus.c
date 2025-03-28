/***************************************************************************

Functions to emulate the video hardware of the machine.

 Video hardware of this hardware is almost similar with "mexico86". So,
 most routines are derived from mexico86 driver.

***************************************************************************/


#include "driver.h"


UINT8 *exzisus_videoram0;
UINT8 *exzisus_videoram1;
UINT8 *exzisus_objectram0;
UINT8 *exzisus_objectram1;
size_t  exzisus_objectram_size0;
size_t  exzisus_objectram_size1;



/***************************************************************************
  Memory handlers
***************************************************************************/

READ8_HANDLER ( exzisus_videoram_0_r )
{
	return exzisus_videoram0[offset];
}


READ8_HANDLER ( exzisus_videoram_1_r )
{
	return exzisus_videoram1[offset];
}


READ8_HANDLER ( exzisus_objectram_0_r )
{
	return exzisus_objectram0[offset];
}


READ8_HANDLER ( exzisus_objectram_1_r )
{
	return exzisus_objectram1[offset];
}


WRITE8_HANDLER( exzisus_videoram_0_w )
{
	exzisus_videoram0[offset] = data;
}


WRITE8_HANDLER( exzisus_videoram_1_w )
{
	exzisus_videoram1[offset] = data;
}


WRITE8_HANDLER( exzisus_objectram_0_w )
{
	exzisus_objectram0[offset] = data;
}


WRITE8_HANDLER( exzisus_objectram_1_w )
{
	exzisus_objectram1[offset] = data;
}


/***************************************************************************
  Screen refresh
***************************************************************************/

VIDEO_UPDATE( exzisus )
{
	int offs;
	int sx, sy, xc, yc;
	int gfx_num, gfx_attr, gfx_offs;

	/* Is this correct ? */
	fillbitmap(bitmap, Machine->pens[1023], &Machine->visible_area);

	/* ---------- 1st TC0010VCU ---------- */
	sx = 0;
	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0 ; offs < exzisus_objectram_size0 ; offs += 4)
    {
		int height;

		/* Skip empty sprites. */
		if ( !(*(UINT32 *)(&exzisus_objectram0[offs])) )
		{
			continue;
		}

		gfx_num = exzisus_objectram0[offs + 1];
		gfx_attr = exzisus_objectram0[offs + 3];

		if ((gfx_num & 0x80) == 0)	/* 16x16 sprites */
		{
			gfx_offs = ((gfx_num & 0x7f) << 3);
			height = 2;

			sx = exzisus_objectram0[offs + 2];
			sx |= (gfx_attr & 0x40) << 2;
		}
		else	/* tilemaps (each sprite is a 16x256 column) */
		{
			gfx_offs = ((gfx_num & 0x3f) << 7) + 0x0400;
			height = 32;

			if (gfx_num & 0x40)			/* Next column */
			{
				sx += 16;
			}
			else
			{
				sx = exzisus_objectram0[offs + 2];
				sx |= (gfx_attr & 0x40) << 2;
			}
		}

		sy = 256 - (height << 3) - (exzisus_objectram0[offs]);

		for (xc = 0 ; xc < 2 ; xc++)
		{
			int goffs = gfx_offs;
			for (yc = 0 ; yc < height ; yc++)
			{
				int code, color, x, y;

				code  = (exzisus_videoram0[goffs + 1] << 8) | exzisus_videoram0[goffs];
				color = (exzisus_videoram0[goffs + 1] >> 6) | (gfx_attr & 0x0f);
				x = (sx + (xc << 3)) & 0xff;
				y = (sy + (yc << 3)) & 0xff;

				if (flip_screen)
				{
					x = 248 - x;
					y = 248 - y;
				}

				
				dgp0.code = code & 0x3fff;
				dgp0.color = color;
				dgp0.flipx = flip_screen;
				dgp0.flipy = flip_screen;
				dgp0.sx = x;
				dgp0.sy = y;
				drawgfx(&dgp0);
				goffs += 2;
			}
			gfx_offs += height << 1;
		}
	}
	} // end of patch paragraph


	/* ---------- 2nd TC0010VCU ---------- */
	sx = 0;
	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0 ; offs < exzisus_objectram_size1 ; offs += 4)
    {
		int height;

		/* Skip empty sprites. */
		if ( !(*(UINT32 *)(&exzisus_objectram1[offs])) )
		{
			continue;
		}

		gfx_num = exzisus_objectram1[offs + 1];
		gfx_attr = exzisus_objectram1[offs + 3];

		if ((gfx_num & 0x80) == 0)	/* 16x16 sprites */
		{
			gfx_offs = ((gfx_num & 0x7f) << 3);
			height = 2;

			sx = exzisus_objectram1[offs + 2];
			sx |= (gfx_attr & 0x40) << 2;
		}
		else	/* tilemaps (each sprite is a 16x256 column) */
		{
			gfx_offs = ((gfx_num & 0x3f) << 7) + 0x0400;	///
			height = 32;

			if (gfx_num & 0x40)			/* Next column */
			{
				sx += 16;
			}
			else
			{
				sx = exzisus_objectram1[offs + 2];
				sx |= (gfx_attr & 0x40) << 2;
			}
		}
		sy = 256 - (height << 3) - (exzisus_objectram1[offs]);

		for (xc = 0 ; xc < 2 ; xc++)
		{
			int goffs = gfx_offs;
			for (yc = 0 ; yc < height ; yc++)
			{
				int code, color, x, y;

				code  = (exzisus_videoram1[goffs + 1] << 8) | exzisus_videoram1[goffs];
				color = (exzisus_videoram1[goffs + 1] >> 6) | (gfx_attr & 0x0f);
				x = (sx + (xc << 3)) & 0xff;
				y = (sy + (yc << 3)) & 0xff;

				if (flip_screen)
				{
					x = 248 - x;
					y = 248 - y;
				}

				
				dgp1.code = code & 0x3fff;
				dgp1.color = color;
				dgp1.flipx = flip_screen;
				dgp1.flipy = flip_screen;
				dgp1.sx = x;
				dgp1.sy = y;
				drawgfx(&dgp1);
				goffs += 2;
			}
			gfx_offs += height << 1;
		}
	}
	} // end of patch paragraph

}
