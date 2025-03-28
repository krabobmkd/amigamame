/***************************************************************************

Taito Super Speed Race video emulation

***************************************************************************/

#include "driver.h"

static int toggle;

static unsigned driver_horz;
static unsigned driver_vert;
static unsigned driver_pic;

static unsigned drones_horz;
static unsigned drones_vert[3];
static unsigned drones_mask;

static unsigned track_horz;
static unsigned track_vert[2];
static unsigned track_ice;


WRITE8_HANDLER( sspeedr_driver_horz_w )
{
	driver_horz = (driver_horz & 0x100) | data;
}


WRITE8_HANDLER( sspeedr_driver_horz_2_w )
{
	driver_horz = (driver_horz & 0xff) | ((data & 1) << 8);
}


WRITE8_HANDLER( sspeedr_driver_vert_w )
{
	driver_vert = data;
}


WRITE8_HANDLER( sspeedr_driver_pic_w )
{
	driver_pic = data & 0x1f;
}


WRITE8_HANDLER( sspeedr_drones_horz_w )
{
	drones_horz = (drones_horz & 0x100) | data;
}


WRITE8_HANDLER( sspeedr_drones_horz_2_w )
{
	drones_horz = (drones_horz & 0xff) | ((data & 1) << 8);
}


WRITE8_HANDLER( sspeedr_drones_mask_w )
{
	drones_mask = data & 0x3f;
}


WRITE8_HANDLER( sspeedr_drones_vert_w )
{
	drones_vert[offset] = data;
}


WRITE8_HANDLER( sspeedr_track_horz_w )
{
	track_horz = (track_horz & 0x100) | data;
}


WRITE8_HANDLER( sspeedr_track_horz_2_w )
{
	track_horz = (track_horz & 0xff) | ((data & 1) << 8);
}


WRITE8_HANDLER( sspeedr_track_vert_w )
{
	track_vert[offset] = data & 0x7f;
}


WRITE8_HANDLER( sspeedr_track_ice_w )
{
	track_ice = data & 0x07;
}


static void draw_track(mame_bitmap* bitmap)
{
	const UINT8* p = memory_region(REGION_GFX3);

	int x;
	int y;

	for (x = 0; x < 376; x++)
	{
		unsigned counter_x = x + track_horz + 0x50;

		int flag = 0;

		if (track_ice & 2)
		{
			flag = 1;
		}
		else if (track_ice & 4)
		{
			if (track_ice & 1)
			{
				flag = (counter_x <= 0x1ff);
			}
			else
			{
				flag = (counter_x >= 0x200);
			}
		}

		if (counter_x >= 0x200)
		{
			counter_x -= 0x1c8;
		}

		y = 0;

		/* upper landscape */

		for (; y < track_vert[0]; y++)
		{
			unsigned counter_y = y - track_vert[0];

			int offset =
				((counter_y & 0x1f) << 3) |
				((counter_x & 0x1c) >> 2) |
				((counter_x & 0xe0) << 3);

			if (counter_x & 2)
			{
				plot_pixel(bitmap, x, y, p[offset] / 16);
			}
			else
			{
				plot_pixel(bitmap, x, y, p[offset] % 16);
			}
		}

		/* street */

		for (; y < 128 + track_vert[1]; y++)
		{
			plot_pixel(bitmap, x, y, flag ? 15 : 0);
		}

		/* lower landscape */

		for (; y < 248; y++)
		{
			unsigned counter_y = y - track_vert[1];

			int offset =
				((counter_y & 0x1f) << 3) |
				((counter_x & 0x1c) >> 2) |
				((counter_x & 0xe0) << 3);

			if (counter_x & 2)
			{
				plot_pixel(bitmap, x, y, p[offset] / 16);
			}
			else
			{
				plot_pixel(bitmap, x, y, p[offset] % 16);
			}
		}
	}
}


static void draw_drones(mame_bitmap* bitmap, const rectangle* cliprect)
{
	static const UINT8 code[6] =
	{
		0xf, 0x4, 0x3, 0x9, 0x7, 0xc
	};

	int i;

	
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
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < 6; i++)
	{
		int x;
		int y;

		if ((drones_mask >> i) & 1)
		{
			continue;
		}

		x = (code[i] << 5) - drones_horz - 0x50;

		if (x <= -32)
		{
			x += 0x1c8;
		}

		y = 0xf0 - drones_vert[i >> 1];

		
		dgp0.code = code[i] ^ toggle;
		dgp0.sx = x;
		dgp0.sy = y;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}


static void draw_driver(mame_bitmap* bitmap, const rectangle* cliprect)

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
	cliprect, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
	int x;
	int y;

	if (!(driver_pic & 0x10))
	{
		return;
	}

	x = 0x1e0 - driver_horz - 0x50;

	if (x <= -32)
	{
		x += 0x1c8;
	}

	y = 0xf0 - driver_vert;

	
	dgp1.code = driver_pic;
	dgp1.sx = x;
	dgp1.sy = y;
	drawgfx(&dgp1);
}
} // end of patch paragraph



VIDEO_START( sspeedr )
{
	toggle = 0;

	return 0;
}


VIDEO_UPDATE( sspeedr )
{
	draw_track(bitmap);

	draw_drones(bitmap, cliprect);

	draw_driver(bitmap, cliprect);
}


VIDEO_EOF( sspeedr )
{
	toggle ^= 1;
}
