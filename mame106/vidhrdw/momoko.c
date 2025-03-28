/*******************************************************************************

Momoko 120% (c) 1986 Jaleco

Video hardware driver by Uki

    02/Mar/2001 -

*******************************************************************************/

#include "driver.h"

UINT8 *momoko_bg_scrollx;
UINT8 *momoko_bg_scrolly;
UINT8 momoko_fg_scrollx;
UINT8 momoko_fg_scrolly;
UINT8 momoko_fg_select;
UINT8 momoko_text_scrolly;
UINT8 momoko_text_mode;
UINT8 momoko_bg_select;
UINT8 momoko_bg_priority;
UINT8 momoko_bg_mask;
UINT8 momoko_fg_mask;
UINT8 momoko_flipscreen;

/****************************************************************************/

WRITE8_HANDLER ( momoko_fg_scrollx_w )
{
	momoko_fg_scrollx = data;
}
WRITE8_HANDLER ( momoko_fg_scrolly_w )
{
	momoko_fg_scrolly = data;
}
WRITE8_HANDLER ( momoko_fg_select_w )
{
	momoko_fg_select = data & 0x0f;
	momoko_fg_mask = data & 0x10;
}
WRITE8_HANDLER ( momoko_text_scrolly_w )
{
	momoko_text_scrolly = data;
}
WRITE8_HANDLER ( momoko_text_mode_w )
{
	momoko_text_mode = data;
}
WRITE8_HANDLER ( momoko_bg_scrollx_w )
{
	momoko_bg_scrollx[ offset ] = data;
}
WRITE8_HANDLER ( momoko_bg_scrolly_w )
{
	momoko_bg_scrolly[ offset ] = data;
}
WRITE8_HANDLER( momoko_bg_select_w )
{
	momoko_bg_select = data & 0x0f;
	momoko_bg_mask = data & 0x10;
}
WRITE8_HANDLER( momoko_bg_priority_w )
{
	momoko_bg_priority = data & 0x01;
}
WRITE8_HANDLER( momoko_flipscreen_w )
{
	momoko_flipscreen = data & 0x01;
}
/****************************************************************************/

void momoko_draw_bg_pri(mame_bitmap *bitmap, int chr, int col, int flipx, int flipy, int x,int y, int pri)
{
	int xx,sx,sy,px,py,dot;
	UINT32 gfxadr;
	UINT8 d0, d1;
	UINT8 *BG_GFX = memory_region( REGION_GFX2 );
	for (sy=0; sy<8; sy++)
	{
		gfxadr = chr*16 + sy*2;
		for (xx=0; xx<2; xx++)
		{
			d0 = BG_GFX[gfxadr + xx*4096];
			d1 = BG_GFX[gfxadr + xx*4096+1];
			for (sx=0; sx<4; sx++)
			{
				dot = (d0 & 0x08) | ((d0 & 0x80) >>5) | ((d1 & 0x08) >>2) | ((d1 & 0x80) >>7);
				if (flipx==0) px=sx+xx*4 + x;
					else  px=7-sx-xx*4 + x;
				if (flipy==0) py=sy + y;
					else  py=7-sy + y;

				if (dot>=pri)
					plot_pixel(bitmap, px, py, Machine->pens[col*16+dot+256]);
				d0 = d0 << 1;
				d1 = d1 << 1;
			}
		}
	}
}

/****************************************************************************/

VIDEO_UPDATE( momoko )
{
	int x, y, dx, dy, rx, ry, radr, chr, sy, fx, fy, px, py, offs, col, pri, flip ;

	UINT8 *BG_MAP     = memory_region( REGION_USER1 );
	UINT8 *BG_COL_MAP = memory_region( REGION_USER2 );
	UINT8 *FG_MAP     = memory_region( REGION_USER3 );
	UINT8 *TEXT_COLOR = memory_region( REGION_PROMS );


	flip = momoko_flipscreen ^ (readinputport(4) & 0x01);

/* draw BG layer */

	dx = ( 7 - momoko_bg_scrollx[0] ) & 7 ;
	dy = ( 7 - momoko_bg_scrolly[0] ) & 7 ;
	rx = (momoko_bg_scrollx[0] + momoko_bg_scrollx[1]*256) >> 3 ;
	ry = (momoko_bg_scrolly[0] + momoko_bg_scrolly[1]*256) >> 3 ;

	if (momoko_bg_mask == 0)
	{
		
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
			TRANSPARENCY_NONE, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (y=0; y<29; y++)
		{
			for (x=0; x<32; x++)
			{
				radr = ( (ry + y +2 ) & 0x3ff ) * 128 + ( (rx + x) & 0x7f ) ;
				chr = BG_MAP[ radr ] ;
				col = BG_COL_MAP[ chr+momoko_bg_select*512+momoko_bg_priority*256] & 0x0f;
				chr = chr + momoko_bg_select * 512;

				if (flip==0)
				{
					px = 8*x+dx-6;
					py = 8*y+dy+9;
				}
				else
				{
					px = 248-(8*x+dx-8);
					py = 248-(8*y+dy+9);
				}

				
				dgp0.code = chr;
				dgp0.color = col;
				dgp0.flipx = flip;
				dgp0.flipy = flip;
				dgp0.sx = px;
				dgp0.sy = py;
				drawgfx(&dgp0);
			}
		}
		} // end of patch paragraph

	}
	else
	fillbitmap(bitmap, Machine->pens[256], 0);


/* draw sprites (momoko) */

	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[3], 	// gfx
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
	for (offs=0; offs<9*4; offs +=4)
	{
		chr = spriteram[offs+1] | ((spriteram[offs+2]&0x60)<<3);
		chr = ((chr & 0x380) << 1) | (chr & 0x7f);
		col = spriteram[offs+2] & 0x07;
		fx = ((spriteram[offs+2] & 0x10) >> 4) ^ flip;
		fy = ((spriteram[offs+2] & 0x08) >> 3) ^ flip; /* ??? */
		x = spriteram[offs+3];
		y = spriteram[offs+0];
		if (flip==0)
		{
			px = x;
			py = 239-y;
		}
		else
		{
			px = 248-x;
			py = y+1;
		}

		
		dgp1.code = chr;
		dgp1.color = col;
		dgp1.flipx = !fx;
		dgp1.flipy = fy;
		dgp1.sx = px;
		dgp1.sy = py;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph



/* draw BG layer */

	if (momoko_bg_mask ==0)
	{
		for (y=0; y<29; y++)
		{
			for (x=0; x<32; x++)
			{
				radr = ( (ry + y +2 ) & 0x3ff ) * 128 + ( (rx + x) & 0x7f ) ;
				chr = BG_MAP[ radr ] ;
				col = BG_COL_MAP[ chr+momoko_bg_select*512+momoko_bg_priority*256 ];
				pri = (col & 0x10) >> 1;

				if (flip==0)
				{
					px = 8*x+dx-6;
					py = 8*y+dy+9;
				}
				else
				{
					px = 248-(8*x+dx-8);
					py = 248-(8*y+dy+9);
				}
				if (pri != 0)
				{
					col = col & 0x0f;
					chr = chr + momoko_bg_select * 512;
					momoko_draw_bg_pri(bitmap,chr,col,flip,flip,px,py,pri);
				}
			}
		}
	}


/* draw sprites (others) */

	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		Machine->gfx[3], 	// gfx
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
	for (offs=9*4; offs<spriteram_size; offs +=4)
	{
		chr = spriteram[offs+1] | ((spriteram[offs+2]&0x60)<<3);
		chr = ((chr & 0x380) << 1) | (chr & 0x7f);
		col = spriteram[offs+2] & 0x07;
		fx = ((spriteram[offs+2] & 0x10) >> 4) ^ flip;
		fy = ((spriteram[offs+2] & 0x08) >> 3) ^ flip; /* ??? */
		x = spriteram[offs+3];
		y = spriteram[offs+0];
		if (flip==0)
		{
			px = x;
			py = 239-y;
		}
		else
		{
			px = 248-x;
			py = y+1;
		}
		
		dgp2.code = chr;
		dgp2.color = col;
		dgp2.flipx = !fx;
		dgp2.flipy = fy;
		dgp2.sx = px;
		dgp2.sy = py;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph



/* draw text layer */

	
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
	for (y=16; y<240; y++)
	{
		for (x=0; x<32; x++)
		{
			sy = y;
			if (momoko_text_mode == 0)
				col = TEXT_COLOR[ (sy >> 3) + 0x100 ] & 0x0f;
			else
			{
				if (TEXT_COLOR[ y ] < 0x08)
					sy += momoko_text_scrolly;
				col = (TEXT_COLOR[ y ] & 0x07) + 0x10;
			}
			dy=sy & 7;
			if (flip==0)
			{
				px = x*8;
				py = y;
			}
			else
			{
				px = 248-x*8;
				py = 255-y;
			}
			
			dgp3.code = videoram[(sy>>3)*32+x]*8+dy;
			dgp3.color = col;
			dgp3.flipx = flip;
			dgp3.sx = px;
			dgp3.sy = py;
			drawgfx(&dgp3);
		}
	}
	} // end of patch paragraph



/* draw FG layer */

	if (momoko_fg_mask==0)
	{
		dx = ( 7 - momoko_fg_scrollx ) & 7 ;
		dy = ( 7 - momoko_fg_scrolly ) & 7 ;
		rx = momoko_fg_scrollx >> 3 ;
		ry = momoko_fg_scrolly >> 3 ;

		
		{ 
		struct drawgfxParams dgp4={
			bitmap, 	// dest
			Machine->gfx[2], 	// gfx
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
		for (y=0; y<29; y++)
		{
			for (x=0; x<32; x++)
			{
				radr = ((ry + y +34) & 0x3f) *0x20 + ((rx + x) & 0x1f) + (momoko_fg_select & 3) *0x800;
 				chr = FG_MAP[ radr ] ;
				if (flip==0)
				{
					px = 8*x+dx-6;
					py = 8*y+dy+9;
				}
				else
				{
					px = 248-(8*x+dx-8);
					py = 248-(8*y+dy+9);
				}
				
				dgp4.code = chr;
				dgp4.flipx = /* color */					flip;
				dgp4.flipy = flip;
				dgp4.sx = /* flip */					px;
				dgp4.sy = py;
				drawgfx(&dgp4);
			}
		}
		} // end of patch paragraph

	}
}
