/***************************************************************************

Atari Sky Raider video emulation

***************************************************************************/

#include "driver.h"

int skyraid_scroll;

UINT8* skyraid_alpha_num_ram;
UINT8* skyraid_pos_ram;
UINT8* skyraid_obj_ram;

static mame_bitmap *helper;


VIDEO_START( skyraid )
{
	if ((helper = auto_bitmap_alloc(128, 240)) == NULL)
		return 1;

	return 0;
}


static void draw_text(mame_bitmap* bitmap, const rectangle* cliprect)
{
	const UINT8* p = skyraid_alpha_num_ram;

	int i;

	
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
	for (i = 0; i < 4; i++)
	{
		int x;
		int y;

		y = 136 + 16 * (i ^ 1);

		for (x = 0; x < bitmap->width; x += 16)
		{
			
			dgp0.code = *p++;
			dgp0.sx = x;
			dgp0.sy = y;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}


static void draw_terrain(mame_bitmap* bitmap)
{
	const UINT8* p = memory_region(REGION_USER1);

	int x;
	int y;

	for (y = 0; y < bitmap->height; y++)
	{
		int offset = (16 * skyraid_scroll + 16 * ((y + 1) / 2)) & 0x7FF;

		x = 0;

		while (x < bitmap->width)
		{
			UINT8 val = p[offset++];

			int color = val / 32;
			int count = val % 32;

			rectangle r;

			r.min_y = y;
			r.min_x = x;
			r.max_y = y + 1;
			r.max_x = x + 31 - count;

			fillbitmap(bitmap, Machine->pens[color], &r);

			x += 32 - count;
		}
	}
}


static void draw_sprites(mame_bitmap* bitmap, const rectangle* cliprect)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		int code = skyraid_obj_ram[8 + 2 * i + 0] & 15;
		int flag = skyraid_obj_ram[8 + 2 * i + 1] & 15;
		int vert = skyraid_pos_ram[8 + 2 * i + 0];
		int horz = skyraid_pos_ram[8 + 2 * i + 1];

		vert -= 31;

		if (flag & 1)
		
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
	cliprect, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	2, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp1.code = code ^ 15;
			dgp1.color = code >> 3;
			dgp1.sx = horz / 2;
			dgp1.sy = vert;
			drawgfx(&dgp1);
		}
} // end of patch paragraph

	}
}


static void draw_missiles(mame_bitmap* bitmap, const rectangle* cliprect)
{
	int i;

	/* hardware is restricted to one sprite per scanline */

	
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
	for (i = 0; i < 4; i++)
	{
		int code = skyraid_obj_ram[2 * i + 0] & 15;
		int vert = skyraid_pos_ram[2 * i + 0];
		int horz = skyraid_pos_ram[2 * i + 1];

		vert -= 15;
		horz -= 31;

		
		dgp2.code = code ^ 15;
		dgp2.sx = horz / 2;
		dgp2.sy = vert;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph

}


static void draw_trapezoid(mame_bitmap* dst, mame_bitmap* src)
{
	const UINT8* p = memory_region(REGION_USER2);

	int x;
	int y;

	for (y = 0; y < dst->height; y++)
	{
		UINT16* pSrc = src->line[y];
		UINT16* pDst = dst->line[y];

		int x1 = 0x000 + p[(y & ~1) + 0];
		int x2 = 0x100 + p[(y & ~1) + 1];

		for (x = x1; x < x2; x++)
		{
			pDst[x] = pSrc[128 * (x - x1) / (x2 - x1)];
		}
	}
}


VIDEO_UPDATE( skyraid )
{
	fillbitmap(bitmap, Machine->pens[0], cliprect);

	draw_terrain(helper);
	draw_sprites(helper, cliprect);
	draw_missiles(helper, cliprect);
	draw_trapezoid(bitmap, helper);
	draw_text(bitmap, cliprect);
}
