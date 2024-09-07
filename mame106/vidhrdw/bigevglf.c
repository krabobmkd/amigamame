/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/
#include "driver.h"


UINT8 *beg_spriteram1;
UINT8 *beg_spriteram2;


static UINT32 vidram_bank = 0;
static UINT32 plane_selected = 0;
static UINT32 plane_visible = 0;
static UINT8 *vidram;


static mame_bitmap *tmp_bitmap[4];

WRITE8_HANDLER(beg_palette_w)
{
	int color;

	paletteram[offset] = data;
	color = paletteram[offset&0x3ff] | (paletteram[0x400+(offset&0x3ff)] << 8);
	palette_set_color(offset&0x3ff, color&0xf0, (color&0xf)<<4, (color&0xf00)>>4);
}

WRITE8_HANDLER( beg_gfxcontrol_w )
{
/* bits used: 0,1,2,3
 0 and 2 select plane,
 1 and 3 select visible plane,
*/
	plane_selected=((data & 4)>>1) | (data&1);
	plane_visible =((data & 8)>>2) | ((data&2)>>1);
}

WRITE8_HANDLER( bigevglf_vidram_addr_w )
{
	vidram_bank = (data & 0xff) * 0x100;
}

WRITE8_HANDLER( bigevglf_vidram_w )
{
	UINT32 x,y,o;
	o = vidram_bank + offset;
	vidram[ o+0x10000*plane_selected ] = data;
	y = o >>8;
	x = (o & 255);
	plot_pixel(tmp_bitmap[plane_selected],x,y,data );
}

READ8_HANDLER( bigevglf_vidram_r )
{
	return vidram[ 0x10000 * plane_selected + vidram_bank + offset];
}

VIDEO_START( bigevglf )
{
	tmp_bitmap[0] = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	tmp_bitmap[1] = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	tmp_bitmap[2] = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	tmp_bitmap[3] = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	vidram = auto_malloc(0x100*0x100 * 4);
	return 0;
}

void beg_draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int i,j;
	
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
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0xc0-4; i >= 0; i-=4)
	{
		int code,sx,sy;
		code = beg_spriteram2[i+1];
		sx = beg_spriteram2[i+3];
		sy = 200-beg_spriteram2[i];
		for(j=0;j<16;j++)
			
			dgp0.code = beg_spriteram1[(code<<4)+j]+((beg_spriteram1[0x400+(code<<4)+j]&0xf)<<8);
			dgp0.color = beg_spriteram2[i+2] & 0xf;
			dgp0.sx = sx+((j&1)<<3);
			dgp0.sy = sy+((j>>1)<<3);
			drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( bigevglf )
{
	copybitmap(bitmap,tmp_bitmap[ plane_visible ],0,0,0,0,cliprect,TRANSPARENCY_NONE, 0);
	beg_draw_sprites(bitmap,cliprect);
}
