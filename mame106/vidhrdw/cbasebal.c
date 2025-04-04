#include "driver.h"


static UINT8 *cbasebal_textram,*cbasebal_scrollram;
static tilemap *fg_tilemap,*bg_tilemap;
static int tilebank,spritebank;
static int text_on,bg_on,obj_on;
static int flipscreen;



/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_bg_tile_info(int tile_index)
{
	UINT8 attr = cbasebal_scrollram[2*tile_index+1];
	SET_TILE_INFO(
			1,
			cbasebal_scrollram[2*tile_index] + ((attr & 0x07) << 8) + 0x800 * tilebank,
			(attr & 0xf0) >> 4,
			(attr & 0x08) ? TILE_FLIPX : 0)
}

static void get_fg_tile_info(int tile_index)
{
	UINT8 attr = cbasebal_textram[tile_index+0x800];
	SET_TILE_INFO(
			0,
			cbasebal_textram[tile_index] + ((attr & 0xf0) << 4),
			attr & 0x07,
			(attr & 0x08) ? TILE_FLIPX : 0)
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( cbasebal )
{
	cbasebal_textram = auto_malloc(0x1000);
	cbasebal_scrollram = auto_malloc(0x1000);

	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,   16,16,64,32);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);

	if (!bg_tilemap || !fg_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,3);
	return 0;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( cbasebal_textram_w )
{
	cbasebal_textram[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap,offset & 0x7ff);
}

READ8_HANDLER( cbasebal_textram_r )
{
	return cbasebal_textram[offset];
}

WRITE8_HANDLER( cbasebal_scrollram_w )
{
	cbasebal_scrollram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap,offset/2);
}

READ8_HANDLER( cbasebal_scrollram_r )
{
	return cbasebal_scrollram[offset];
}

WRITE8_HANDLER( cbasebal_gfxctrl_w )
{
	/* bit 0 is unknown - toggles continuously */

	/* bit 1 is flip screen */
	flipscreen = data & 0x02;
	tilemap_set_flip(ALL_TILEMAPS,flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);

	/* bit 2 is unknown - unused? */

	/* bit 3 is tile bank */
	if (tilebank != ((data & 0x08) >> 3))
	{
		tilebank = (data & 0x08) >> 3;
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}

	/* bit 4 is sprite bank */
	spritebank = (data & 0x10) >> 4;

	/* bits 5 is text enable */
	text_on = ~data & 0x20;

	/* bits 6-7 are bg/sprite enable (don't know which is which) */
	bg_on = ~data & 0x40;
	obj_on = ~data & 0x80;

	/* other bits unknown, but used */
}

WRITE8_HANDLER( cbasebal_scrollx_w )
{
	static UINT8 scroll[2];

	scroll[offset] = data;
	tilemap_set_scrollx(bg_tilemap,0,scroll[0] + 256 * scroll[1]);
}

WRITE8_HANDLER( cbasebal_scrolly_w )
{
	static UINT8 scroll[2];

	scroll[offset] = data;
	tilemap_set_scrolly(bg_tilemap,0,scroll[0] + 256 * scroll[1]);
}



/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs,sx,sy;

	/* the last entry is not a sprite, we skip it otherwise spang shows a bubble */
	/* moving diagonally across the screen */
	
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
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size-8;offs >= 0;offs -= 4)
	{
		int code = spriteram[offs];
		int attr = spriteram[offs+1];
		int color = attr & 0x07;
		int flipx = attr & 0x08;
		sx = spriteram[offs+3] + ((attr & 0x10) << 4);
		sy = ((spriteram[offs+2] + 8) & 0xff) - 8;
		code += (attr & 0xe0) << 3;
		code += spritebank * 0x800;

		if (flipscreen)
		{
			sx = 496 - sx;
			sy = 240 - sy;
			flipx = !flipx;
		}

		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipscreen;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( cbasebal )
{
	if (bg_on)
		tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	else
		fillbitmap(bitmap,Machine->pens[768],cliprect);

	if (obj_on)
		draw_sprites(bitmap,cliprect);

	if (text_on)
		tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
}
