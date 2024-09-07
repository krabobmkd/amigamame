/***************************************************************************

Atari Boxer (prototype) video emulation

***************************************************************************/

#include "driver.h"

UINT8* boxer_tile_ram;
UINT8* boxer_sprite_ram;


static void draw_boxer(mame_bitmap* bitmap, const rectangle* cliprect)
{
	int n;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[n], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		1, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (n = 0; n < 2; n++)
	{
		const UINT8* p = memory_region(n == 0 ? REGION_USER1 : REGION_USER2);

		int i;
		int j;

		int x = 196 - boxer_sprite_ram[0 + 2 * n];
		int y = 192 - boxer_sprite_ram[1 + 2 * n];

		int l = boxer_sprite_ram[4 + 2 * n] & 15;
		int r = boxer_sprite_ram[5 + 2 * n] & 15;

		for (i = 0; i < 8; i++)
		{
			for (j = 0; j < 4; j++)
			{
				UINT8 code;

				code = p[32 * l + 4 * i + j];

				
				dgp0.code = code;
				dgp0.flipx = code & 0x80;
				dgp0.sx = x + 8 * j;
				dgp0.sy = y + 8 * i;
				drawgfx(&dgp0);

				code = p[32 * r + 4 * i - j + 3];

				
				dgp0.code = code;
				dgp0.flipx = !(code & 0x80);
				dgp0.sx = x + 8 * j + 32;
				dgp0.sy = y + 8 * i;
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( boxer )
{
	int i;
	int j;

	fillbitmap(bitmap, 1, cliprect);

	
	{ 
	struct drawgfxParams dgp2={
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
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 32; j++)
		{
			UINT8 code = boxer_tile_ram[32 * i + j];

			
			dgp2.code = code;
			dgp2.flipx = code & 0x40;
			dgp2.flipy = code & 0x40;
			dgp2.sx = 8 * j + 4;
			dgp2.sy = 8 * (i % 2) + 32 * (i / 2);
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph


	draw_boxer(bitmap, cliprect);
}
