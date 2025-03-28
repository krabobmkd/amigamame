/*******************************************************************************

XX Mission (c) 1986 UPL

Video hardware driver by Uki

    31/Mar/2001 -

*******************************************************************************/

#include "driver.h"

UINT8 *xxmissio_fgram;
size_t xxmissio_fgram_size;

static UINT8 xxmissio_xscroll,xxmissio_yscroll;
static UINT8 flipscreen;
static UINT8 xxmissio_bg_redraw;


WRITE8_HANDLER( xxmissio_scroll_x_w )
{
	xxmissio_xscroll = data;
}
WRITE8_HANDLER( xxmissio_scroll_y_w )
{
	xxmissio_yscroll = data;
}

WRITE8_HANDLER( xxmissio_flipscreen_w )
{
	if ((data & 0x01) != flipscreen)
	{
		flipscreen = data & 0x01;
		xxmissio_bg_redraw = 1;
	}
}

READ8_HANDLER( xxmissio_fgram_r )
{
	return xxmissio_fgram[offset];
}
WRITE8_HANDLER( xxmissio_fgram_w )
{
	xxmissio_fgram[offset] = data;
}

WRITE8_HANDLER( xxmissio_videoram_w )
{
	int offs = offset & 0x7e0;
	int x = (offset + (xxmissio_xscroll >> 3) ) & 0x1f;
	offs |= x;

	videoram[offs] = data;
	dirtybuffer[offs & 0x3ff] = 1;
}
READ8_HANDLER( xxmissio_videoram_r )
{
	int offs = offset & 0x7e0;
	int x = (offset + (xxmissio_xscroll >> 3) ) & 0x1f;
	offs |= x;

	return videoram[offs];
}

WRITE8_HANDLER( xxmissio_paletteram_w )
{
	if (paletteram[offset] != data)
	{
		paletteram_BBGGRRII_w(offset,data);

		if (offset >= 0x200)
			xxmissio_bg_redraw = 1;
	}
}

/****************************************************************************/

VIDEO_UPDATE( xxmissio )
{
	int offs;
	int chr,col;
	int x,y,px,py,fx,fy,sx,sy;

	int size = videoram_size/2;

	if (xxmissio_bg_redraw==1)
		memset(dirtybuffer,1,size);

/* draw BG layer */

	
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
	for (y=0; y<32; y++)
	{
		for (x=0; x<32; x++)
		{
			offs = y*0x20 + x;

			if (flipscreen!=0)
				offs = (size-1)-offs;

			if (dirtybuffer[offs] != 0)
			{
				dirtybuffer[offs]=0;

				px = x*16;
				py = y*8;

				chr = videoram[ offs ] ;
				col = videoram[ offs + size];
				chr = chr + ((col & 0xc0) << 2 );
				col = col & 0x0f;

				
				dgp0.code = chr;
				dgp0.color = col;
				dgp0.flipx = flipscreen;
				dgp0.flipy = flipscreen;
				dgp0.sx = px;
				dgp0.sy = py;
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph


	if (flipscreen == 0)
	{
		sx = -xxmissio_xscroll*2+12;
		sy = -xxmissio_yscroll;
	}
	else
	{
		sx = xxmissio_xscroll*2+2;
		sy = xxmissio_yscroll;
	}

	copyscrollbitmap(bitmap,tmpbitmap,1,&sx,1,&sy,&Machine->visible_area,TRANSPARENCY_NONE,0);
	xxmissio_bg_redraw = 0;

/* draw sprites */

	
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
	for (offs=0; offs<spriteram_size; offs +=32)
	{
		chr = spriteram[offs];
		col = spriteram[offs+3];

		fx = ((col & 0x10) >> 4) ^ flipscreen;
		fy = ((col & 0x20) >> 5) ^ flipscreen;

		x = spriteram[offs+1]*2;
		y = spriteram[offs+2];

		chr = chr + ((col & 0x40) << 2);
		col = col & 0x07;

		if (flipscreen==0)
		{
			px = x-8;
			py = y;
		}
		else
		{
			px = 480-x-8;
			py = 240-y;
		}

		px &= 0x1ff;

		
		dgp1.code = chr;
		dgp1.color = col;
		dgp1.flipx = fx;
		dgp1.flipy = fy;
		dgp1.sx = px;
		dgp1.sy = py;
		drawgfx(&dgp1);
		if (px>0x1e0)
        {
			dgp1.code = chr;
			dgp1.color = col;
			dgp1.flipx = fx;
			dgp1.flipy = fy;
			dgp1.sx = px-0x200;
			dgp1.sy = py;
			drawgfx(&dgp1);
        }

	}
	} // end of patch paragraph



/* draw FG layer */

	
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
	for (y=4; y<28; y++)
	{
		for (x=0; x<32; x++)
		{
			offs = y*32+x;
			chr = xxmissio_fgram[offs];
			col = xxmissio_fgram[offs + 0x400] & 0x07;

			if (flipscreen==0)
			{
				px = 16*x;
				py = 8*y;
			}
			else
			{
				px = 496-16*x;
				py = 248-8*y;
			}

			
			dgp3.code = chr;
			dgp3.color = col;
			dgp3.flipx = flipscreen;
			dgp3.flipy = flipscreen;
			dgp3.sx = px;
			dgp3.sy = py;
			drawgfx(&dgp3);
		}
	}
	} // end of patch paragraph


}
