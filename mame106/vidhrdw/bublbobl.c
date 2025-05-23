/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/
#include "driver.h"
#include <ctype.h>
#include "drawgfxn.h"


unsigned char *bublbobl_objectram;
size_t bublbobl_objectram_size;
int bublbobl_video_enable;


/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( bublbobl )
{
	int offs;
	int sx,sy;
	int gfx_num,gfx_offs;
	const UINT8 *prom_line;


	/* Bubble Bobble doesn't have a real video RAM. All graphics (characters */
	/* and sprites) are stored in the same memory region, and information on */
	/* the background character columns is stored in the area dd00-dd3f */

	/* This clears & redraws the entire screen each pass */
	fillbitmap(bitmap,Machine->pens[255],&Machine->visible_area);

	if (!bublbobl_video_enable) return;

	sx = 0;

    // new drawgfx use excluded max.
	rectangle clipex = Machine->visible_area;
    clipex.max_x++;
    clipex.max_y++;

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
		&clipex,//&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < bublbobl_objectram_size;offs += 4)
    {
		/* skip empty sprites */
		/* this is dword aligned so the UINT32 * cast shouldn't give problems */
		/* on any architecture */
		if (*(UINT32 *)(&bublbobl_objectram[offs]) == 0)
			continue;

		UINT8 gfx_num = bublbobl_objectram[offs + 1];
		UINT8 gfx_attr = bublbobl_objectram[offs + 3];
		prom_line = memory_region(REGION_PROMS) + 0x80 + ((gfx_num & 0xe0) >> 1);

		gfx_offs = ((gfx_num & 0x1f) * 0x80);
		if ((gfx_num & 0xa0) == 0xa0)
			gfx_offs |= 0x1000;

		sy = -bublbobl_objectram[offs + 0];

		for (int yc = 0;yc < 32;yc++)
		{
			if (prom_line[yc/2] & 0x08)	continue;	/* NEXT */

			if (!(prom_line[yc/2] & 0x04))	/* next column */
			{
				sx = bublbobl_objectram[offs + 2];
				if (gfx_attr & 0x40) sx -= 256;
			}
            int goffs = gfx_offs + ((yc & 7) <<1) +
                    ((prom_line[yc/2] & 0x03)<<4);
			for (int xc = 0;xc < 2;xc++)
			{
				int code,color,flipx,flipy,x,y;

				code = videoram[goffs] + ((videoram[goffs + 1] & 0x03)<<8) + 1024 * (gfx_attr & 0x0f);
				color = (videoram[goffs + 1] & 0x3c) >> 2;
				flipx = videoram[goffs + 1] & 0x40;
				flipy = videoram[goffs + 1] & 0x80;
				x = sx + xc * 8;
				y = (sy + yc * 8) & 0xff;

				if (flip_screen)
				{
					x = 248 - x;
					y = 248 - y;
					flipx = !flipx;
					flipy = !flipy;
				}

				dgp0.code = code;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = x;
				dgp0.sy = y;
				//drawgfx(&dgp0); old
                drawgfx_clut16_Src8(&dgp0);
				goffs += 0x40;
			}
		}

		sx += 16;
	}
	} // end of patch paragraph

}
