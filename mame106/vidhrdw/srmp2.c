/***************************************************************************

Functions to emulate the video hardware of the machine.

  Video hardware is very similar with "seta" hardware except color PROM.

***************************************************************************/


#include "driver.h"




int srmp2_color_bank;
int srmp3_gfx_bank;
int mjyuugi_gfx_bank;


PALETTE_INIT( srmp2 )
{
	int i;

	for (i = 0; i < Machine->drv->total_colors; i++)
	{
		int col, r, g, b;

		col = (color_prom[i] << 8) + color_prom[i + Machine->drv->total_colors];

		r = (col & 0x7c00) >> 10;
		g = (col & 0x03e0) >> 5;
		b = (col & 0x001f);

		r = (r << 3) | (r >> 2);
		g = (g << 3) | (g >> 2);
		b = (b << 3) | (b >> 2);

		palette_set_color(i,r,g,b);
	}

	for (i = 0; i < Machine->drv->total_colors; i++)
	{
		colortable[i] = i ^ 0x0f;
	}
}


PALETTE_INIT( srmp3 )
{
	int i;

	for (i = 0; i < Machine->drv->total_colors; i++)
	{
		int col, r, g, b;

		col = (color_prom[i] << 8) + color_prom[i + Machine->drv->total_colors];

		r = (col & 0x7c00) >> 10;
		g = (col & 0x03e0) >> 5;
		b = (col & 0x001f);

		r = (r << 3) | (r >> 2);
		g = (g << 3) | (g >> 2);
		b = (b << 3) | (b >> 2);

		palette_set_color(i,r,g,b);
	}
}


static void srmp2_draw_sprites(mame_bitmap *bitmap)
{
/*
    Sprite RAM A:   spriteram16_2
    ==============
     + 0x000 - 0x3ff
      x--- ----  ---- ---- : Flip X
      -x-- ----  ---- ---- : Flip Y
      --xx xxxx  xxxx xxxx : Tile number

     + 0x400 - 0x7ff
      xxxx x---  ---- ---- : Color
      ---- ---x  xxxx xxxx : X coords

    Sprite RAM B:   spriteram16
    ==============
     + 0x000 - 0x3ff
      ---- ----  xxxx xxxx : Y coords

     + 0x600
      ---- ----  -x-- ---- : Flip screen
*/

	int offs;
	int xoffs, yoffs;

	int ctrl	=	spriteram16[ 0x600/2 ];
	int ctrl2	=	spriteram16[ 0x602/2 ];

	int flip	=	ctrl & 0x40;

	/* Sprites Banking and/or Sprites Buffering */
	UINT16 *src = spriteram16_2 + ( ((ctrl2 ^ (~ctrl2<<1)) & 0x40) ? 0x2000/2 : 0 );

	int max_y	=	Machine -> drv -> screen_height;

	xoffs	=	flip ? 0x10 : 0x10;
	yoffs	=	flip ? 0x05 : 0x07;

	
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
	for (offs = (0x400-2)/2; offs >= 0/2; offs -= 2/2)
	{
		int code	=	src[offs + 0x000/2];

		int x		=	src[offs + 0x400/2];
		int y		=	spriteram16[offs + 0x000/2] & 0xff;

		int flipx	=	code & 0x8000;
		int flipy	=	code & 0x4000;

		int color   = (x >> 11) & 0x1f;

		if (flip)
		{
			y = max_y - y;
			flipx = !flipx;
			flipy = !flipy;
		}

		code = code & 0x3fff;

		if (srmp2_color_bank) color |= 0x20;

		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = (x + xoffs) & 0x1ff;
		dgp0.sy = max_y - ((y + yoffs) & 0x0ff);
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}


static void srmp3_draw_sprites_map(mame_bitmap *bitmap)
{
	int offs, col;
	int xoffs, yoffs;

	int ctrl	=	spriteram[ 0x600/2 ];
	int ctrl2	=	spriteram[ 0x602/2 ];

	int flip	=	ctrl & 0x40;
	int numcol	=	ctrl2 & 0x0f;

	int upper	=	( spriteram[ 0x604/2 ] & 0xFF ) +
					( spriteram[ 0x606/2 ] & 0xFF ) * 256;

	int max_y	=	0xf0;

	xoffs	=	flip ? 0x10 : 0x10;
	yoffs	=	flip ? -0x01 : -0x01;

	/* Number of columns to draw - the value 1 seems special, meaning:
       draw every column */
	if (numcol == 1)	numcol = 16;

	/* The first column is the frontmost, see twineagl test mode */
	
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
		\					&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (col = numcol - 1; col >= 0; col--)
	{
		int x	=	spriteram[(col * 0x20 + 0x08 + 0x400)/2] & 0xff;
		int y	=	spriteram[(col * 0x20 + 0x00 + 0x400)/2] & 0xff;

		/* draw this column */
		for (offs = 0; offs < 0x40/2; offs += 2/2)
		{
			int code	=	(((spriteram_3[((col)&0x0f) * 0x40/2 + offs + 0x800/2] & 0xff) << 8) + (spriteram_2[((col)&0xf) * 0x40/2 + offs + 0x800/2] & 0xff));

			int color   =	 ((spriteram_3[((col)&0x0f) * 0x40/2 + offs + 0xc00/2] & 0xf8) >> 3);

			int flipx	=	code & 0x8000;
			int flipy	=	code & 0x4000;

			int sx		=	  x + xoffs  + (offs & 1) * 16;
			int sy		=	-(y + yoffs) + (offs / 2) * 16 -
							(Machine->drv->screen_height-(Machine->visible_area.max_y + 1));

			if (upper & (1 << col))	sx += 256;

			if (flip)
			{
				sy = max_y - 14 - sy - 0x100;
				flipx = !flipx;
				flipy = !flipy;
			}

			code = code & 0x1fff;

#define DRAWTILE(_x_, _y_)  \
			
			dgp1.code = \					code;
			dgp1.color = \					color;
			dgp1.flipx = \					flipx;
			dgp1.flipy = flipy;
			dgp1.sx = \					_x_;
			dgp1.sy = _y_;
			drawgfx(&dgp1);

			DRAWTILE(sx - 0x000, sy + 0x000)
			DRAWTILE(sx - 0x200, sy + 0x000)
			DRAWTILE(sx - 0x000, sy + 0x100)
			DRAWTILE(sx - 0x200, sy + 0x100)

		}
		/* next column */
	}
	} // end of patch paragraph

}


static void srmp3_draw_sprites(mame_bitmap *bitmap)
{
/*
    Sprite RAM A:   spriteram_2
    ==============
     + 0x000 - 0x1ff
      xxxx xxxx : Tile number (low)

     + 0x200 - 0x3ff
      ---- ---- : Color
      xxxx xxxx : X coords (low)


    Sprite RAM B:   spriteram_3
    ==============
     + 0x000 - 0x1ff
      x--- ---- : Flip X ?
      -x-- ---- : Flip Y ?
      --x- ---- : Use GFX bank flag
      ---x xxxx : Tile number (high)

     + 0x200 - 0x3ff
      xxxx x--- : Color
      ---- ---x : X coords (high)


    Sprite RAM C:   spriteram
    ==============
     + 0x000 - 0x1ff
      xxxx xxxx : Y coords (low)

     + 0x300
      -x-- ---- : Flip screen
*/

	int offs;
	int xoffs, yoffs;

	int max_y	=	Machine -> drv -> screen_height;

	int ctrl	=	spriteram[ 0x600/2 ];
//  int ctrl2   =   spriteram[ 0x602/2 ];

	int flip	=	ctrl & 0x40;

	srmp3_draw_sprites_map(bitmap);

	xoffs	=	flip ? 0x10 : 0x10;
	yoffs	=	flip ? 0x06 : 0x06;

	
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
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0x200 - 1; offs >= 0; offs--)
	{
		int code	=	(((spriteram_3[offs + 0x000] & 0xff) << 8) + (spriteram_2[offs + 0x000] & 0xff));
		int gfxbank	=	  (spriteram_3[offs + 0x000] & 0x20);

		int color	=	((spriteram_3[offs + 0x200] & 0xf8) >> 3);

		int x		=	(((spriteram_3[offs + 0x200] & 0x01) << 8) + (spriteram_2[offs + 0x200] & 0xff));
		int y		=	  (spriteram[offs + 0x000] & 0xff);

		int flipx	=	code & 0x8000;
		int flipy	=	code & 0x4000;

		code = (code & 0x1fff);
		if (gfxbank) code += ((srmp3_gfx_bank + 1) * 0x2000);

		if (flip)
		{
			y = max_y - y;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp2.code = code;
		dgp2.color = color;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = (x + xoffs) & 0x1ff;
		dgp2.sy = max_y - ((y + yoffs) & 0x0ff);
		drawgfx(&dgp2);
	}
	} // end of patch paragraph

}


static void mjyuugi_draw_sprites_map(mame_bitmap *bitmap)
{
	int offs, col;
	int xoffs, yoffs;

	int total_color_codes	=	Machine->drv->gfxdecodeinfo[0].total_color_codes;

	int ctrl	=	spriteram16[ 0x600/2 ];
	int ctrl2	=	spriteram16[ 0x602/2 ];

	int flip	=	ctrl & 0x40;
	int numcol	=	ctrl2 & 0x000f;

	/* Sprites Banking and/or Sprites Buffering */
	UINT16 *src = spriteram16_2 + ( ((ctrl2 ^ (~ctrl2<<1)) & 0x40) ? 0x2000/2 : 0 );

	int upper	=	( spriteram16[ 0x604/2 ] & 0xFF ) +
					( spriteram16[ 0x606/2 ] & 0xFF ) * 256;

	int max_y	=	0xf0;

	xoffs	=	flip ? 0x10 : 0x10;
	yoffs	=	flip ? 0x09 : 0x07;

	/* Number of columns to draw - the value 1 seems special, meaning:
       draw every column */
	if (numcol == 1)	numcol = 16;

	/* The first column is the frontmost, see twineagl test mode */
	
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
		\					&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (col = numcol - 1; col >= 0; col--)
	{
		int x	=	spriteram16[(col * 0x20 + 0x08 + 0x400)/2] & 0xff;
		int y	=	spriteram16[(col * 0x20 + 0x00 + 0x400)/2] & 0xff;

		/* draw this column */
		for (offs = 0; offs < 0x40/2; offs += 2/2)
		{
			int code	=	src[((col)&0xf) * 0x40/2 + offs + 0x800/2];
			int color	=	src[((col)&0xf) * 0x40/2 + offs + 0xc00/2];

			int gfxbank	=	color & 0x0200;

			int flipx	=	code & 0x8000;
			int flipy	=	code & 0x4000;

			int sx		=	  x + xoffs  + (offs & 1) * 16;
			int sy		=	-(y + yoffs) + (offs / 2) * 16 -
							(Machine->drv->screen_height-(Machine->visible_area.max_y + 1));

			if (upper & (1 << col))	sx += 256;

			if (flip)
			{
				sy = max_y - 16 - sy - 0x100;
				flipx = !flipx;
				flipy = !flipy;
			}

			color	=	((color >> (16-5)) % total_color_codes);
			code	=	(code & 0x3fff) + (gfxbank ? 0x4000 : 0);

#define DRAWTILE(_x_, _y_)  \
			
			dgp3.code = \					code;
			dgp3.color = \					color;
			dgp3.flipx = \					flipx;
			dgp3.flipy = flipy;
			dgp3.sx = \					_x_;
			dgp3.sy = _y_;
			drawgfx(&dgp3);

			DRAWTILE(sx - 0x000, sy + 0x000)
			DRAWTILE(sx - 0x200, sy + 0x000)
			DRAWTILE(sx - 0x000, sy + 0x100)
			DRAWTILE(sx - 0x200, sy + 0x100)

		}
		/* next column */
	}
	} // end of patch paragraph

}


static void mjyuugi_draw_sprites(mame_bitmap *bitmap)
{
/*
    Sprite RAM A:   spriteram16_2
    ==============
     + 0x000 - 0x3ff
      x--- ----  ---- ---- : Flip X
      -x-- ----  ---- ---- : Flip Y
      --x- ----  ---- ---- : Use GFX bank flag
      ---x xxxx  xxxx xxxx : Tile number

     + 0x400 - 0x7ff
      xxxx x---  ---- ---- : Color
      ---- ---x  xxxx xxxx : X coords

    Sprite RAM B:   spriteram16
    ==============
     + 0x000 - 0x3ff
      ---- ----  xxxx xxxx : Y coords

     + 0x600
      ---- ----  -x-- ---- : Flip screen
*/

	int offs;
	int xoffs, yoffs;

	int ctrl	=	spriteram16[ 0x600/2 ];
	int ctrl2	=	spriteram16[ 0x602/2 ];

	int flip	=	ctrl & 0x40;

	/* Sprites Banking and/or Sprites Buffering */
	UINT16 *src = spriteram16_2 + ( ((ctrl2 ^ (~ctrl2<<1)) & 0x40) ? 0x2000/2 : 0 );

	int max_y	=	Machine -> drv -> screen_height;

	mjyuugi_draw_sprites_map(bitmap);

	xoffs	=	flip ? 0x10 : 0x10;
	yoffs	=	flip ? 0x06 : 0x06;

	
	{ 
	struct drawgfxParams dgp4={
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
	for (offs = (0x400 - 6) / 2; offs >= 0 / 2; offs -= 2 / 2)
	{
		int code	=	src[offs + 0x000 / 2];
		int gfxbank	=	code & 0x2000;

		int color	=	((src[offs + 0x400 / 2] >> 11) & 0x1f);

		int x		=	(src[offs + 0x400 / 2] & 0x1ff);
		int y		=	(spriteram16[offs + 0x000 / 2] & 0xff);

		int flipx	=	code & 0x8000;
		int flipy	=	code & 0x4000;

		code = (code & 0x1fff);
		if (gfxbank) code += ((mjyuugi_gfx_bank + 1) * 0x2000);

		if (flip)
		{
			y = max_y - y
				+(Machine->drv->screen_height-(Machine->visible_area.max_y + 1));
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp4.code = code;
		dgp4.color = color;
		dgp4.flipx = flipx;
		dgp4.flipy = flipy;
		dgp4.sx = (x + xoffs) & 0x1ff;
		dgp4.sy = max_y - ((y + yoffs) & 0x0ff);
		drawgfx(&dgp4);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( srmp2 )
{
	fillbitmap(bitmap, Machine->pens[0x1f0], &Machine->visible_area);
	srmp2_draw_sprites(bitmap);
}


VIDEO_UPDATE( srmp3 )
{
	fillbitmap(bitmap, Machine->pens[0x1f0], &Machine->visible_area);
	srmp3_draw_sprites(bitmap);
}


VIDEO_UPDATE( mjyuugi )
{
	fillbitmap(bitmap, Machine->pens[0x1f0], &Machine->visible_area);
	mjyuugi_draw_sprites(bitmap);
}
