/* Diver Boy - Video Hardware */

#include "driver.h"

UINT16 *diverboy_spriteram;
size_t diverboy_spriteram_size;


VIDEO_START(diverboy)
{
	return 0;
}

static void diverboy_drawsprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	UINT16 *source = diverboy_spriteram;
	UINT16 *finish = source + (diverboy_spriteram_size/2);

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[bank], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		(source[1] & 0x0008) ? TRANSPARENCY_NONE : TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	while (source < finish)
	{
		INT16 xpos,ypos,number,colr,bank,flash;

		ypos = source[4];
		xpos = source[0];
		colr = (source[1]& 0x00f0) >> 4;
		number = source[3];
		flash = source[1] & 0x1000;

		colr |= ((source[1] & 0x000c) << 2);

		ypos = 0x100 - ypos;

		bank = (source[1]&0x0002) >> 1;

		if (!flash || (cpu_getcurrentframe() & 1))
		{
			
			dgp0.code = number;
			dgp0.color = colr;
			dgp0.sx = xpos;
			dgp0.sy = ypos;
			drawgfx(&dgp0);
		}

		source+=8;
	}
	} // end of patch paragraph

}

VIDEO_UPDATE(diverboy)
{
//  fillbitmap(bitmap,get_black_pen(),cliprect);
	diverboy_drawsprites(bitmap,cliprect);
}
