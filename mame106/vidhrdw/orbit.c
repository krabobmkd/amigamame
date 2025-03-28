/***************************************************************************

Atari Orbit video emulation

***************************************************************************/

#include "driver.h"
#include "includes/orbit.h"

UINT8* orbit_playfield_ram;
UINT8* orbit_sprite_ram;

static tilemap* bg_tilemap;

static int orbit_flip_screen;


WRITE8_HANDLER( orbit_playfield_w )
{
	tilemap_mark_tile_dirty(bg_tilemap, offset);

	orbit_playfield_ram[offset] = data;
}


WRITE8_HANDLER( orbit_sprite_w )
{
	orbit_sprite_ram[offset] = data;
}


static UINT32 get_memory_offset(UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows)
{
	return num_cols * row + col;
}


static void get_tile_info(int tile_index)
{
	UINT8 code = orbit_playfield_ram[tile_index];

	int flags = 0;

	if (code & 0x40)
	{
		flags |= TILE_FLIPX;
	}
	if (orbit_flip_screen)
	{
		flags |= TILE_FLIPY;
	}

	SET_TILE_INFO(3, code & 0x3f, 0, flags)
}


VIDEO_START( orbit )
{
	bg_tilemap = tilemap_create(get_tile_info, get_memory_offset, 0, 16, 16, 32, 30);

	if (bg_tilemap == NULL)
		return 1;

	return 0;
}


static void orbit_draw_sprites(mame_bitmap* bitmap, const rectangle* cliprect)
{
	const UINT8* p = orbit_sprite_ram;

	int i;

	
	{ 
	struct drawgfxParams dgpz0={
		bitmap, 	// dest
		Machine->gfx[layout], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0x00010000, 	// scalex
		0x00010000, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < 16; i++)
	{
		int code = *p++;
		int vpos = *p++;
		int hpos = *p++;
		int flag = *p++;

		int layout =
			((flag & 0xc0) == 0x80) ? 1 :
			((flag & 0xc0) == 0xc0) ? 2 : 0;

		int flip_x = code & 0x40;
		int flip_y = code & 0x80;

		int zoom_x = 0x10000;
		int zoom_y = 0x10000;

		code &= 0x3f;

		if (flag & 1)
		{
			code |= 0x40;
		}
		if (flag & 2)
		{
			zoom_x *= 2;
		}

		vpos = 240 - vpos;

		hpos <<= 1;
		vpos <<= 1;

		
		dgpz0.code = code;
		dgpz0.flipx = flip_x;
		dgpz0.flipy = flip_y;
		dgpz0.sx = hpos;
		dgpz0.sy = vpos;
		dgpz0.scalex = zoom_x;
		dgpz0.scaley = zoom_y;
		drawgfxzoom(&dgpz0);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( orbit )
{
	orbit_flip_screen = readinputport(3) & 8;

	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);

	orbit_draw_sprites(bitmap, cliprect);
}
