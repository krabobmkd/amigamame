/***************************************************************************

    Atari Sprint 2 video emulation

***************************************************************************/

#include "driver.h"
#include "includes/sprint2.h"

UINT8* sprint2_video_ram;

static tilemap* bg_tilemap;
static mame_bitmap* helper;

static int collision[2];


static void get_tile_info(int tile_index)
{
	UINT8 code = sprint2_video_ram[tile_index];

	SET_TILE_INFO(0, code & 0x3f, code >> 7, 0)
}


VIDEO_START( sprint2 )
{
	helper = auto_bitmap_alloc(Machine->drv->screen_width, Machine->drv->screen_height);

	if (helper == NULL)
	{
		return 1;
	}

	bg_tilemap = tilemap_create(get_tile_info, tilemap_scan_rows, TILEMAP_OPAQUE, 16, 8, 32, 32);

	if (bg_tilemap == NULL)
	{
		return 1;
	}

	return 0;
}


READ8_HANDLER( sprint2_collision1_r )
{
	return collision[0];
}
READ8_HANDLER( sprint2_collision2_r )
{
	return collision[1];
}


WRITE8_HANDLER( sprint2_collision_reset1_w )
{
	collision[0] = 0;
}
WRITE8_HANDLER( sprint2_collision_reset2_w )
{
	collision[1] = 0;
}


WRITE8_HANDLER( sprint2_video_ram_w )
{
	if (data != sprint2_video_ram[offset])
	{
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}

	sprint2_video_ram[offset] = data;
}


static UINT8 collision_check(rectangle* rect)
{
	UINT8 data = 0;

	int x;
	int y;

	for (y = rect->min_y; y <= rect->max_y; y++)
	{
		for (x = rect->min_x; x <= rect->max_x; x++)
		{
			pen_t a = read_pixel(helper, x, y);

			if (a == 0)
			{
				data |= 0x40;
			}
			if (a == 3)
			{
				data |= 0x80;
			}
		}
	}

	return data;
}


static int get_sprite_code(int n)
{
	return sprint2_video_ram[0x398 + 2 * n + 1] >> 3;
}
static int get_sprite_x(int n)
{
	return 2 * (248 - sprint2_video_ram[0x390 + 1 * n]);
}
static int get_sprite_y(int n)
{
	return 1 * (248 - sprint2_video_ram[0x398 + 2 * n]);
}


VIDEO_UPDATE( sprint2 )
{
	int i;

	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);

	/* draw the sprites */

	
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
	for (i = 0; i < 4; i++)
	{
		
		dgp0.code = get_sprite_code(i);
		dgp0.color = i;
		dgp0.sx = get_sprite_x(i);
		dgp0.sy = get_sprite_y(i);
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}


VIDEO_EOF( sprint2 )
{
	int i;
	int j;

	/*
     * Collisions are detected for both player cars:
     *
     * D7 => collision w/ white playfield
     * D6 => collision w/ black playfield or another car
     *
     */

	
	{ 
	struct drawgfxParams dgp1={
		helper, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&rect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		1, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	struct drawgfxParams dgp2={
		helper, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&rect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < 2; i++)
	{
		rectangle rect;

		rect.min_x = get_sprite_x(i);
		rect.min_y = get_sprite_y(i);
		rect.max_x = get_sprite_x(i) + Machine->gfx[1]->width - 1;
		rect.max_y = get_sprite_y(i) + Machine->gfx[1]->height - 1;

		if (rect.min_x < Machine->visible_area.min_x)
			rect.min_x = Machine->visible_area.min_x;
		if (rect.min_y < Machine->visible_area.min_y)
			rect.min_y = Machine->visible_area.min_y;
		if (rect.max_x > Machine->visible_area.max_x)
			rect.max_x = Machine->visible_area.max_x;
		if (rect.max_y > Machine->visible_area.max_y)
			rect.max_y = Machine->visible_area.max_y;

		/* check for sprite-tilemap collisions */

		tilemap_draw(helper, &rect, bg_tilemap, 0, 0);

		
		dgp1.code = get_sprite_code(i);
		dgp1.sx = get_sprite_x(i);
		dgp1.sy = get_sprite_y(i);
		drawgfx(&dgp1);

		collision[i] |= collision_check(&rect);

		/* check for sprite-sprite collisions */

		for (j = 0; j < 4; j++)
		{
			if (j != i)
			{
				
				dgp2.code = get_sprite_code(j);
				dgp2.sx = get_sprite_x(j);
				dgp2.sy = get_sprite_y(j);
				drawgfx(&dgp2);
			}
		}

		
		dgp1.code = get_sprite_code(i);
		dgp1.sx = get_sprite_x(i);
		dgp1.sy = get_sprite_y(i);
		drawgfx(&dgp1);

		collision[i] |= collision_check(&rect);
	}
	} // end of patch paragraph

}
