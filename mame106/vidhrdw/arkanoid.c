/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"

static UINT8 gfxbank, palettebank;
extern UINT8 arkanoid_paddle_select;

static tilemap *bg_tilemap;

WRITE8_HANDLER( arkanoid_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset / 2);
	}
}
extern int last_partial_scanline;
//krb - try to allow sprite multiplexing for abyss demo.
WRITE8_HANDLER( arkcrsn5_spriteram_w )
{
	if (spriteram[offset] != data)
	{
		spriteram[offset] = data;
		if((offset & 3) == 1) // if write Y pos of sprite...
		{
    		int scanline = cpu_getscanline(); // heavy seriously optimize that.
    		//if(scanline>12)
    		scanline += 24; //ok 24; // test
            if (scanline > last_partial_scanline) //optim
            {
                force_partial_update(scanline);
            }
		}
	}
}

WRITE8_HANDLER( arkanoid_d008_w )
{
	int bank;

	/* bits 0 and 1 flip X and Y, I don't know which is which */
	if (flip_screen_x != (data & 0x01)) {
		flip_screen_x_set(data & 0x01);
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}

	if (flip_screen_y != (data & 0x02)) {
		flip_screen_y_set(data & 0x02);
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}

	/* bit 2 selects the input paddle */
    arkanoid_paddle_select = data & 0x04;

	/* bit 3 is coin lockout (but not the service coin) */
	coin_lockout_w(0, !(data & 0x08));
	coin_lockout_w(1, !(data & 0x08));

	/* bit 4 is unknown */

	/* bits 5 and 6 control gfx bank and palette bank. They are used together */
	/* so I don't know which is which. */
	bank = (data & 0x20) >> 5;

	if (gfxbank != bank) {
		gfxbank = bank;
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}

	bank = (data & 0x40) >> 6;

	if (palettebank != bank) {
		palettebank = bank;
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}

	/* bit 7 is unknown */
}

static void get_bg_tile_info(int tile_index)
{
	int offs = tile_index * 2;
	int code = videoram[offs + 1] + ((videoram[offs] & 0x07) << 8) + 2048 * gfxbank;
	int color = ((videoram[offs] & 0xf8) >> 3) + 32 * palettebank;

	SET_TILE_INFO(0, code, color, 0)
}

VIDEO_START( arkanoid )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	state_save_register_global(gfxbank);
	state_save_register_global(palettebank);

	if ( !bg_tilemap )
		return 1;

	return 0;
}

static void arkanoid_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	int offs;

    struct drawgfxParams dgp0={
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

	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		int sx,sy,code;

		sx = spriteram[offs];
		sy = 248 - spriteram[offs + 1];
		if (flip_screen_x) sx = 248 - sx;
		if (flip_screen_y) sy = 248 - sy;

		code = spriteram[offs + 3] + ((spriteram[offs + 2] & 0x03) << 8) + 1024 * gfxbank;

        dgp0.code = 2 * code;
		dgp0.color = ((spriteram[offs + 2] & 0xf8) >> 3) + 32 * palettebank;
		dgp0.flipx = flip_screen_x;
		dgp0.flipy = flip_screen_y;
		dgp0.sx = sx;
		dgp0.sy = sy + (flip_screen_y ? 8 : -8);
		drawgfx(&dgp0);

        dgp0.code = 2 * code + 1;
		dgp0.color = ((spriteram[offs + 2] & 0xf8) >> 3) + 32 * palettebank;
		dgp0.flipx = flip_screen_x;
		dgp0.flipy = flip_screen_y;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
}

/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( arkanoid )
{
//printf("arkanoid update\n");
	tilemap_draw(bitmap, /*&Machine->visible_area*/ cliprect, bg_tilemap, 0, 0);
	arkanoid_draw_sprites(bitmap, cliprect);
}
