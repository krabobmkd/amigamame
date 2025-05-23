/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/
#include "driver.h"
#include <ctype.h>



extern unsigned char *tnzs_objram;
extern unsigned char *tnzs_vdcram;
extern unsigned char *tnzs_scrollram;
extern unsigned char *tnzs_objctrl;

static int tnzs_screenflip;

/***************************************************************************

  The New Zealand Story doesn't have a color PROM. It uses 1024 bytes of RAM
  to dynamically create the palette. Each couple of bytes defines one
  color (15 bits per pixel; the top bit of the second byte is unused).
  Since the graphics use 4 bitplanes, hence 16 colors, this makes for 32
  different color codes.

***************************************************************************/


/***************************************************************************

  Convert the color PROMs into a more useable format.

  Arkanoid has a two 512x8 palette PROMs. The two bytes joined together
  form 512 xRRRRRGGGGGBBBBB color values.

***************************************************************************/
PALETTE_INIT( arknoid2 )
{
	int i,col,r,g,b;

	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		col = (color_prom[i]<<8)+color_prom[i+512];
		r =  (col & 0x7c00)>>7;	/* Red */
		g =  (col & 0x03e0)>>2;	/* Green */
		b =  (col & 0x001f)<<3;	/* Blue */
		palette_set_color(i,r,g,b);
	}
}



/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

void tnzs_vh_draw_background(mame_bitmap *bitmap,unsigned char *m)
{
	int x,y,column,tot;
	int scrollx, scrolly;
	unsigned int upperbits;
	int ctrl2	=	tnzs_objctrl[1];


	if ((ctrl2 ^ (~ctrl2<<1)) & 0x40)
	{
		m += 0x800;
	}


	/* The byte at f200 is the y-scroll value for the first column.
       The byte at f204 is the LSB of x-scroll value for the first column.

       The other columns follow at 16-byte intervals.

       The 9th bit of each x-scroll value is combined into 2 bytes
       at f302-f303 */

	/* f301 controls how many columns are drawn. */
	tot = tnzs_objctrl[1] & 0x1f;
	if (tot == 1) tot = 16;

	upperbits = tnzs_objctrl[2] + tnzs_objctrl[3] * 256;

	
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
		0, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (column = 0;column < tot;column++)
	{
		scrollx = tnzs_scrollram[column*16+4] - ((upperbits & 0x01) * 256);
		if (tnzs_screenflip)
			scrolly = tnzs_scrollram[column*16] + 1 - 256;
		else
			scrolly = -tnzs_scrollram[column*16] + 1;

		for (y=0;y<16;y++)
		{
			for (x=0;x<2;x++)
			{
				int code,color,flipx,flipy,sx,sy;
				int i = 32*(column^8) + 2*y + x;


				code = m[i] + ((m[i + 0x1000] & 0x3f) << 8);
				color = (m[i + 0x1200] & 0xf8) >> 3; /* colours at d600-d7ff */
				sx = x*16;
				sy = y*16;
				flipx = m[i + 0x1000] & 0x80;
				flipy = m[i + 0x1000] & 0x40;
				if (tnzs_screenflip)
				{
					sy = 240 - sy;
					flipx = !flipx;
					flipy = !flipy;
				}

				
				dgp0.code = code;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = sx + scrollx;
				dgp0.sy = (sy + scrolly) & 0xff;
				drawgfx(&dgp0);

				/* wrap around x */
				
				dgp0.code = code;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = sx + 512 + scrollx;
				dgp0.sy = (sy + scrolly) & 0xff;
				drawgfx(&dgp0);
			}
		}


		upperbits >>= 1;
	}
	} // end of patch paragraph

}

void tnzs_vh_draw_foreground(mame_bitmap *bitmap,
							 unsigned char *char_pointer,
							 unsigned char *x_pointer,
							 unsigned char *y_pointer,
							 unsigned char *ctrl_pointer,
							 unsigned char *color_pointer)
{
	int i;
	int ctrl2	=	tnzs_objctrl[1];


	if ((ctrl2 ^ (~ctrl2<<1)) & 0x40)
	{
		char_pointer += 0x800;
		x_pointer += 0x800;
		ctrl_pointer += 0x800;
		color_pointer += 0x800;
	}


	/* Draw all 512 sprites */
	
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
	for (i=0x1ff;i >= 0;i--)
	{
		int code,color,sx,sy,flipx,flipy;

		code = char_pointer[i] + ((ctrl_pointer[i] & 0x3f) << 8);
		color = (color_pointer[i] & 0xf8) >> 3;
		sx = x_pointer[i] - ((color_pointer[i] & 1) << 8);
		sy = 240 - y_pointer[i];
		flipx = ctrl_pointer[i] & 0x80;
		flipy = ctrl_pointer[i] & 0x40;
		if (tnzs_screenflip)
		{
			sy = 240 - sy;
			flipx = !flipx;
			flipy = !flipy;
			/* hack to hide Chuka Taisens grey line, top left corner */
			if ((sy == 0) && (code == 0)) sy += 240;
		}

		
		dgp2.code = code;
		dgp2.color = color;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = sx;
		dgp2.sy = sy+2;
		drawgfx(&dgp2);

		/* wrap around x */
		
		dgp2.code = code;
		dgp2.color = color;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = sx + 512;
		dgp2.sy = sy+2;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( tnzs )
{
	/* If the byte at f300 has bit 6 set, flip the screen
       (I'm not 100% sure about this) */
	tnzs_screenflip = (tnzs_objctrl[0] & 0x40) >> 6;


	/* Blank the background */
	fillbitmap(bitmap, Machine->pens[0], &Machine->visible_area);

	/* Redraw the background tiles (c400-c5ff) */
	tnzs_vh_draw_background(bitmap, tnzs_objram + 0x400);

	/* Draw the sprites on top */
	tnzs_vh_draw_foreground(bitmap,
							tnzs_objram + 0x0000, /*  chars : c000 */
							tnzs_objram + 0x0200, /*      x : c200 */
							tnzs_vdcram + 0x0000, /*      y : f000 */
							tnzs_objram + 0x1000, /*   ctrl : d000 */
							tnzs_objram + 0x1200); /* color : d200 */
}

VIDEO_EOF( tnzs )
{
	int ctrl2	=	tnzs_objctrl[1];
	if (~ctrl2 & 0x20)
	{
		// note I copy sprites only. seta.c also copies the "floating tilemap"
		if (ctrl2 & 0x40)
		{
			memcpy(&tnzs_objram[0x0000],&tnzs_objram[0x0800],0x0400);
			memcpy(&tnzs_objram[0x1000],&tnzs_objram[0x1800],0x0400);
		}
		else
		{
			memcpy(&tnzs_objram[0x0800],&tnzs_objram[0x0000],0x0400);
			memcpy(&tnzs_objram[0x1800],&tnzs_objram[0x1000],0x0400);
		}

		// and I copy the "floating tilemap" BACKWARDS - this fixes kabukiz
		memcpy(&tnzs_objram[0x0400],&tnzs_objram[0x0c00],0x0400);
		memcpy(&tnzs_objram[0x1400],&tnzs_objram[0x1c00],0x0400);
	}
}
