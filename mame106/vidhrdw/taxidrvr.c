#include "driver.h"
#include "taxidrvr.h"


UINT8 *taxidrvr_vram0,*taxidrvr_vram1,*taxidrvr_vram2,*taxidrvr_vram3;
UINT8 *taxidrvr_vram4,*taxidrvr_vram5,*taxidrvr_vram6,*taxidrvr_vram7;
UINT8 *taxidrvr_scroll;
int taxidrvr_bghide;
static int spritectrl[9];


WRITE8_HANDLER( taxidrvr_spritectrl_w )
{
	spritectrl[offset] = data;
}



VIDEO_UPDATE( taxidrvr )
{
	int offs;
	int sx,sy;


	if (taxidrvr_bghide)
	{
		fillbitmap(bitmap,Machine->pens[0],cliprect);


		/* kludge to fix scroll after death */
		taxidrvr_scroll[0] = taxidrvr_scroll[1] = taxidrvr_scroll[2] = taxidrvr_scroll[3] = 0;
		spritectrl[2] = spritectrl[5] = spritectrl[8] = 0;
	}
	else
	{
		
		{ 
		struct drawgfxParams dgp0={
			bitmap, 	// dest
			Machine->gfx[3], 	// gfx
			0, 	// code
			0, 	// color
			0, 	// flipx
			0, 	// flipy
			0, 	// sx
			0, 	// sy
			cliprect, 	// clip
			TRANSPARENCY_NONE, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (offs = 0;offs < 0x400;offs++)
		{
			sx = offs % 32;
			sy = offs / 32;

			
			dgp0.code = taxidrvr_vram3[offs];
			dgp0.sx = (sx*8-taxidrvr_scroll[0])&0xff;
			dgp0.sy = (sy*8-taxidrvr_scroll[1])&0xff;
			drawgfx(&dgp0);
		}
		} // end of patch paragraph


		
		{ 
		struct drawgfxParams dgp1={
			bitmap, 	// dest
			Machine->gfx[2], 	// gfx
			0, 	// code
			0, 	// color
			0, 	// flipx
			0, 	// flipy
			0, 	// sx
			0, 	// sy
			cliprect, 	// clip
			TRANSPARENCY_PEN, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (offs = 0;offs < 0x400;offs++)
		{
			sx = offs % 32;
			sy = offs / 32;

			
			dgp1.code = taxidrvr_vram2[offs]+256*taxidrvr_vram2[offs+0x400];
			dgp1.sx = (sx*8-taxidrvr_scroll[2])&0xff;
			dgp1.sy = (sy*8-taxidrvr_scroll[3])&0xff;
			drawgfx(&dgp1);
		}
		} // end of patch paragraph


		if (spritectrl[2] & 4)
		{
			for (offs = 0;offs < 0x1000;offs++)
			{
				int color;

				sx = ((offs/2) % 64-spritectrl[0]-256*(spritectrl[2]&1))&0x1ff;
				sy = ((offs/2) / 64-spritectrl[1]-128*(spritectrl[2]&2))&0x1ff;

				color = (taxidrvr_vram5[offs/4]>>(2*(offs&3)))&0x03;
				if (color)
				{
					if (sx > 0 && sx < 256 && sy > 0 && sy < 256)
						plot_pixel(bitmap,sx,sy,color);
				}
			}
		}

		if (spritectrl[5] & 4)
		{
			for (offs = 0;offs < 0x1000;offs++)
			{
				int color;

				sx = ((offs/2) % 64-spritectrl[3]-256*(spritectrl[5]&1))&0x1ff;
				sy = ((offs/2) / 64-spritectrl[4]-128*(spritectrl[5]&2))&0x1ff;

				color = (taxidrvr_vram6[offs/4]>>(2*(offs&3)))&0x03;
				if (color)
				{
					if (sx > 0 && sx < 256 && sy > 0 && sy < 256)
						plot_pixel(bitmap,sx,sy,color);
				}
			}
		}

		if (spritectrl[8] & 4)
		{
			for (offs = 0;offs < 0x1000;offs++)
			{
				int color;

				sx = ((offs/2) % 64-spritectrl[6]-256*(spritectrl[8]&1))&0x1ff;
				sy = ((offs/2) / 64-spritectrl[7]-128*(spritectrl[8]&2))&0x1ff;

				color = (taxidrvr_vram7[offs/4]>>(2*(offs&3)))&0x03;
				if (color)
				{
					if (sx > 0 && sx < 256 && sy > 0 && sy < 256)
						plot_pixel(bitmap,sx,sy,color);
				}
			}
		}

		
		{ 
		struct drawgfxParams dgp2={
			bitmap, 	// dest
			Machine->gfx[1], 	// gfx
			0, 	// code
			0, 	// color
			0, 	// flipx
			0, 	// flipy
			0, 	// sx
			0, 	// sy
			cliprect, 	// clip
			TRANSPARENCY_PEN, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (offs = 0;offs < 0x400;offs++)
		{
			sx = offs % 32;
			sy = offs / 32;

			
			dgp2.code = taxidrvr_vram1[offs];
			dgp2.sx = sx*8;
			dgp2.sy = sy*8;
			drawgfx(&dgp2);
		}
		} // end of patch paragraph


		for (offs = 0;offs < 0x2000;offs++)
		{
			int color;

			sx = (offs/2) % 64;
			sy = (offs/2) / 64;

			color = (taxidrvr_vram4[offs/4]>>(2*(offs&3)))&0x03;
			if (color)
			{
				plot_pixel(bitmap,sx,sy,2*color);
			}
		}
	}

	
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
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x400;offs++)
	{
		sx = offs % 32;
		sy = offs / 32;

		
		dgp3.code = taxidrvr_vram0[offs];
		dgp3.sx = sx*8;
		dgp3.sy = sy*8;
		drawgfx(&dgp3);
	}
	} // end of patch paragraph

}
