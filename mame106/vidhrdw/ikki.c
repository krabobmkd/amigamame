/******************************************************************************

Ikki (c) 1985 Sun Electronics

Video hardware driver by Uki

    20/Jun/2001 -

******************************************************************************/

#include "driver.h"

static UINT8 ikki_flipscreen, ikki_scroll[2];

PALETTE_INIT( ikki )
{
	int i;

	for (i = 0; i<256; i++)
	{
		int r,g,b;

		r = color_prom[0]*0x11;
		g = color_prom[256]*0x11;
		b = color_prom[2*256]*0x11;

		palette_set_color(i,r,g,b);

		color_prom++;
	}

	/* 256th color is not drawn on screen */
	/* this is used for special transparent function */
	palette_set_color(256,0,0,1);

	color_prom += 2*256;

	/* color_prom now points to the beginning of the lookup table */

	/* sprites lookup table */
	for (i=0; i<512; i++)
	{
		int d = 255-*(color_prom++);
		if ( ((i % 8) == 7) && (d == 0) )
			*(colortable++) = 256; /* special transparent */
		else
			*(colortable++) = d; /* normal color */
	}

	/* bg lookup table */
	for (i=0; i<512; i++)
		*(colortable++) = *(color_prom++);

}

WRITE8_HANDLER( ikki_scroll_w )
{
	ikki_scroll[offset] = data;
}

WRITE8_HANDLER( ikki_scrn_ctrl_w )
{
	ikki_flipscreen = (data >> 2) & 1;
}

VIDEO_UPDATE( ikki )
{

	int offs,chr,col,px,py,f,bank,d;
	UINT8 *VIDEOATTR = memory_region( REGION_USER1 );

	f = ikki_flipscreen;

	/* draw bg layer */

	
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
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs=0; offs<(videoram_size/2); offs++)
	{
		int sx,sy;

		sx = offs / 32;
		sy = offs % 32;

		py = sy*8;
		px = sx*8;

		d = VIDEOATTR[ sx ];

		switch (d)
		{
			case 0x02: /* scroll area */
				px = sx*8 - ikki_scroll[1];
				if (px<0)
					px=px+8*22;
				py = (sy*8 + ~ikki_scroll[0]) & 0xff;
				break;

			case 0x03: /* non-scroll area */
				break;

			case 0x00: /* sprite disable? */
				break;

			case 0x0d: /* sprite disable? */
				break;

			case 0x0b: /* non-scroll area (?) */
				break;

			case 0x0e: /* unknown */
				break;
		}

		if (f != 0)
		{
			px = 248-px;
			py = 248-py;
		}

		col = videoram[offs*2];
		bank = (col & 0xe0) << 3;
		col = ((col & 0x1f)<<0) | ((col & 0x80) >> 2);

		
		dgp0.code = videoram[offs*2+1] + bank;
		dgp0.color = col;
		dgp0.flipx = f;
		dgp0.flipy = f;
		dgp0.sx = px;
		dgp0.sy = py;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph


/* draw sprites */

	fillbitmap(tmpbitmap, Machine->pens[256], 0);

	/* c060 - c0ff */
	
	{ 
	struct drawgfxParams dgp1={
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
	for (offs=0x00; offs<0x800; offs +=4)
	{
		chr = spriteram[offs+1] >> 1 ;
		col = spriteram[offs+2];

		px = spriteram[offs+3];
		py = spriteram[offs+0];

		chr += (col & 0x80);
		col = (col & 0x3f) >> 0 ;

		if (f==0)
			py = 224-py;
		else
			px = 240-px;

		px = px & 0xff;
		py = py & 0xff;

		if (px>248)
			px = px-256;
		if (py>240)
			py = py-256;

		
		dgp1.code = chr;
		dgp1.color = col;
		dgp1.flipx = f;
		dgp1.flipy = f;
		dgp1.sx = px;
		dgp1.sy = py;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph


	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_COLOR,256);


	/* mask sprites */

	
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
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs=0; offs<(videoram_size/2); offs++)
	{
		int sx,sy;

		sx = offs / 32;
		sy = offs % 32;

		d = VIDEOATTR[ sx ];

		if ( (d == 0) || (d == 0x0d) )
		{
			py = sy*8;
			px = sx*8;

			if (f != 0)
			{
				px = 248-px;
				py = 248-py;
			}

			col = videoram[offs*2];
			bank = (col & 0xe0) << 3;
			col = ((col & 0x1f)<<0) | ((col & 0x80) >> 2);

			
			dgp2.code = videoram[offs*2+1] + bank;
			dgp2.color = col;
			dgp2.flipx = f;
			dgp2.flipy = f;
			dgp2.sx = px;
			dgp2.sy = py;
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph


}
