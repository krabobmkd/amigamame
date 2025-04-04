/***************************************************************************

    Atari Night Driver hardware

***************************************************************************/

#include "driver.h"
#include "includes/nitedrvr.h"

UINT8 *nitedrvr_hvc;

static tilemap *bg_tilemap;

WRITE8_HANDLER( nitedrvr_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

WRITE8_HANDLER( nitedrvr_hvc_w )
{
	nitedrvr_hvc[offset & 0x3f] = data;

	if ((offset & 0x30) == 0x30)
		watchdog_reset_w(0, 0);
}

static void get_bg_tile_info(int tile_index)
{
	int code = videoram[tile_index] & 0x3f;

	SET_TILE_INFO(0, code, 0, 0)
}



VIDEO_START( nitedrvr )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	if ( !bg_tilemap )
		return 1;

	return 0;
}

static void nitedrvr_draw_box( mame_bitmap *bitmap, int bx, int by, int ex, int ey )
{
	int x, y;

	for (y = by; y < ey; y++)
	{
		for (x = bx; x < ex; x++)
		{
			if ((y < 256) && (x < 256))
				plot_pixel(bitmap, x, y, Machine->pens[1]);
		}
	}

	return;
}

static void nitedrvr_draw_roadway( mame_bitmap *bitmap )
{
	int roadway;

	for (roadway = 0; roadway < 16; roadway++)
	{
		int bx, by, ex, ey;

		bx = nitedrvr_hvc[roadway];
		by = nitedrvr_hvc[roadway + 16];
		ex = bx + ((nitedrvr_hvc[roadway + 32] & 0xf0) >> 4);
		ey = by + (16 - (nitedrvr_hvc[roadway + 32] & 0x0f));

		nitedrvr_draw_box(bitmap, bx, by, ex, ey);
	}
}

VIDEO_UPDATE( nitedrvr )
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	nitedrvr_draw_roadway(bitmap);
}
