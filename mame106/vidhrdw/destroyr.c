/***************************************************************************

Atari Destroyer video emulation

***************************************************************************/

#include "driver.h"

int destroyr_wavemod;
int destroyr_cursor;

UINT8* destroyr_major_obj_ram;
UINT8* destroyr_minor_obj_ram;
UINT8* destroyr_alpha_num_ram;


VIDEO_UPDATE( destroyr )
{
	int i;
	int j;

	fillbitmap(bitmap, Machine->pens[0], &Machine->visible_area);

	/* draw major objects */

	
	{ 
	struct drawgfxParams dgp0={
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
	for (i = 0; i < 16; i++)
	{
		int attr = destroyr_major_obj_ram[2 * i + 0] ^ 0xff;
		int horz = destroyr_major_obj_ram[2 * i + 1];

		int num = attr & 3;
		int scan = attr & 4;
		int flipx = attr & 8;

		if (scan == 0)
		{
			if (horz >= 192)
				horz -= 256;
		}
		else
		{
			if (horz < 192)
				continue;
		}

		
		dgp0.code = num;
		dgp0.flipx = flipx;
		dgp0.sx = horz;
		dgp0.sy = 16 * i;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph


	/* draw alpha numerics */

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 32; j++)
		{
			int num = destroyr_alpha_num_ram[32 * i + j];

			
			dgp1.code = num;
			dgp1.sx = 8 * j;
			dgp1.sy = 8 * i;
			drawgfx(&dgp1);
		}
	}
	} // end of patch paragraph


	/* draw minor objects */

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < 2; i++)
	{
		int horz = 256 - destroyr_minor_obj_ram[i + 2];
		int vert = 256 - destroyr_minor_obj_ram[i + 4];

		
		dgp2.code = destroyr_minor_obj_ram[i + 0];
		dgp2.sx = horz;
		dgp2.sy = vert;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph


	/* draw waves */

	
	{ 
	struct drawgfxParams dgp3={
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
	for (i = 0; i < 4; i++)
	{
		
		dgp3.code = destroyr_wavemod ? 1 : 0;
		dgp3.sx = 64 * i;
		dgp3.sy = 0x4e;
		drawgfx(&dgp3);
	}
	} // end of patch paragraph


	/* draw cursor */

	for (i = 0; i < 256; i++)
	{
		if (i & 4)
			plot_pixel(bitmap, i, destroyr_cursor ^ 0xff, Machine->pens[7]);
	}
}
