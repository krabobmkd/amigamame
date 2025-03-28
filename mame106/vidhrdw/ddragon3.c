/***************************************************************************

  Video Hardware for Double Dragon 3

***************************************************************************/

#include "driver.h"

UINT16 *ddragon3_bg_videoram16;
UINT16 *ddragon3_fg_videoram16;
UINT16 ddragon3_vreg;

static UINT16 ddragon3_bg_scrollx;
static UINT16 ddragon3_bg_scrolly;
static UINT16 ddragon3_fg_scrollx;
static UINT16 ddragon3_fg_scrolly;
static UINT16 ddragon3_bg_tilebase;

static tilemap *bg_tilemap, *fg_tilemap;


WRITE16_HANDLER( ddragon3_scroll16_w )
{
	switch (offset)
	{
		case 0: COMBINE_DATA(&ddragon3_fg_scrollx);	break;	// Scroll X, BG1
		case 1: COMBINE_DATA(&ddragon3_fg_scrolly);	break;	// Scroll Y, BG1
		case 2: COMBINE_DATA(&ddragon3_bg_scrollx);	break;	// Scroll X, BG0
		case 3: COMBINE_DATA(&ddragon3_bg_scrolly);	break;	// Scroll Y, BG0
		case 4:										break;	// Unknown write
		case 5: flip_screen_set(data & 0x01);		break;	// Flip Screen
		case 6:
			COMBINE_DATA(&ddragon3_bg_tilebase);			// BG Tile Base
			ddragon3_bg_tilebase &= 0x1ff;
			tilemap_mark_all_tiles_dirty(bg_tilemap);
			break;
	}
}

READ16_HANDLER( ddragon3_scroll16_r )
{
	switch (offset)
	{
		case 0: return ddragon3_fg_scrollx;
		case 1: return ddragon3_fg_scrolly;
		case 2: return ddragon3_bg_scrollx;
		case 3: return ddragon3_bg_scrolly;
		case 5: return flip_screen;
		case 6: return ddragon3_bg_tilebase;
	}

	return 0;
}

WRITE16_HANDLER( ddragon3_bg_videoram16_w )
{
	UINT16 oldword = ddragon3_bg_videoram16[offset];

	COMBINE_DATA(&ddragon3_bg_videoram16[offset]);

	if (oldword != ddragon3_bg_videoram16[offset])
	{
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

WRITE16_HANDLER( ddragon3_fg_videoram16_w )
{
	UINT16 oldword = ddragon3_fg_videoram16[offset];

	COMBINE_DATA(&ddragon3_fg_videoram16[offset]);

	if (oldword != ddragon3_fg_videoram16[offset])
	{
		tilemap_mark_tile_dirty(fg_tilemap, offset / 2);
	}
}

static void get_bg_tile_info(int tile_index)
{
	UINT16 attr = ddragon3_bg_videoram16[tile_index];
	int code = (attr & 0x0fff) | ((ddragon3_bg_tilebase & 0x01) << 12);
	int color = ((attr & 0xf000) >> 12) + 16;

	SET_TILE_INFO(0, code, color, 0)
}

static void get_fg_tile_info(int tile_index)
{
	int offs = tile_index * 2;
	UINT16 attr = ddragon3_fg_videoram16[offs];
	int code = ddragon3_fg_videoram16[offs + 1] & 0x1fff;
	int color = attr & 0xf;
	int flags = (attr & 0x40) ? TILE_FLIPX : 0;

	SET_TILE_INFO(0, code, color, flags);
}

VIDEO_START( ddragon3 )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_TRANSPARENT, 16, 16, 32, 32);

	if ( !bg_tilemap )
		return 1;

	fg_tilemap = tilemap_create(get_fg_tile_info, tilemap_scan_rows,
		TILEMAP_TRANSPARENT, 16, 16, 32, 32);

	if ( !fg_tilemap )
		return 1;

	tilemap_set_transparent_pen(bg_tilemap, 0);
	tilemap_set_transparent_pen(fg_tilemap, 0);

	return 0;
}

/*
 * Sprite Format
 * ----------------------------------
 *
 * Word | Bit(s)           | Use
 * -----+-fedcba9876543210-+----------------
 *   0  | --------xxxxxxxx | ypos (signed)
 * -----+------------------+
 *   1  | --------xxx----- | height
 *   1  | -----------xx--- | yflip, xflip
 *   1  | -------------x-- | msb x
 *   1  | --------------x- | msb y?
 *   1  | ---------------x | enable
 * -----+------------------+
 *   2  | --------xxxxxxxx | tile number
 * -----+------------------+
 *   3  | --------xxxxxxxx | bank
 * -----+------------------+
 *   4  | ------------xxxx |color
 * -----+------------------+
 *   5  | --------xxxxxxxx | xpos
 * -----+------------------+
 *   6,7| unused
 */

static void ddragon3_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	UINT16 *source = spriteram16;
	UINT16 *finish = source + 0x800;

    struct drawgfxParams dgp={
            bitmap, Machine->gfx[1],
            0,//code,
            0,//color,
            0,0,//flipx, flipy,
            0,0,// //sx, sy,
            cliprect,
            TRANSPARENCY_PEN, 0,
            // - - optionals
            0,0,NULL,0
    };

	while (source < finish)
	{
		UINT16 attr = source[1];

		if (attr & 0x01)	/* enable */
		{
			int i;
			int bank = source[3] & 0xff;
			int code = (source[2] & 0xff) + (bank * 256);
			dgp.color = source[4] & 0xf;
			int flipx = attr & 0x10;
			int flipy = attr & 0x08;
			int sx = source[5] & 0xff;
			int sy = source[0] & 0xff;
			int height = (attr >> 5) & 0x07;

			if (attr & 0x04) sx |= 0x100;
			if (attr & 0x02) sy = 239 + (0x100 - sy); else sy = 240 - sy;
			if (sx > 0x17f) sx = 0 - (0x200 - sx);

			if (flip_screen)
			{
				sx = 304 - sx;
				sy = 224 - sy;
				flipx = !flipx;
				flipy = !flipy;
			}

            dgp.flipx = flipx;
            dgp.flipy = flipy;
            dgp.sx = sx;

			for (i = 0; i <= height; i++)
			{
    			dgp.code = code + i;
                dgp.sy = sy + (flip_screen ? (i * 16) : (-i * 16));
//				drawgfx(bitmap, Machine->gfx[1], code + i, color, flipx, flipy,
//					sx, sy + (flip_screen ? (i * 16) : (-i * 16)), cliprect,
//					TRANSPARENCY_PEN, 0);
                drawgfx(&dgp);
			}
		}

		source += 8;
	}
}

VIDEO_UPDATE( ddragon3 )
{
	tilemap_set_scrollx(bg_tilemap, 0, ddragon3_bg_scrollx);
	tilemap_set_scrolly(bg_tilemap, 0, ddragon3_bg_scrolly);
	tilemap_set_scrollx(fg_tilemap, 0, ddragon3_fg_scrollx);
	tilemap_set_scrolly(fg_tilemap, 0, ddragon3_fg_scrolly);

	if ((ddragon3_vreg & 0x60) == 0x40)
	{
		tilemap_draw(bitmap, cliprect, bg_tilemap, TILEMAP_IGNORE_TRANSPARENCY, 0);
		tilemap_draw(bitmap, cliprect, fg_tilemap, 0, 0);
		ddragon3_draw_sprites(bitmap, cliprect);
	}
	else if ((ddragon3_vreg & 0x60) == 0x60)
	{
		tilemap_draw(bitmap, cliprect, fg_tilemap, TILEMAP_IGNORE_TRANSPARENCY, 0);
		tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
		ddragon3_draw_sprites(bitmap, cliprect);
	}
	else
	{
		tilemap_draw(bitmap, cliprect, bg_tilemap, TILEMAP_IGNORE_TRANSPARENCY, 0);
		ddragon3_draw_sprites(bitmap, cliprect);
		tilemap_draw(bitmap, cliprect, fg_tilemap, 0, 0);
	}
}

VIDEO_UPDATE( ctribe )
{
	tilemap_set_scrollx( bg_tilemap, 0, ddragon3_bg_scrollx );
	tilemap_set_scrolly( bg_tilemap, 0, ddragon3_bg_scrolly );
	tilemap_set_scrollx( fg_tilemap, 0, ddragon3_fg_scrollx );
	tilemap_set_scrolly( fg_tilemap, 0, ddragon3_fg_scrolly );

	if(ddragon3_vreg & 8)
	{
		tilemap_draw(bitmap, cliprect, fg_tilemap, TILEMAP_IGNORE_TRANSPARENCY, 0);
		ddragon3_draw_sprites(bitmap, cliprect);
		tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	}
	else
	{
		tilemap_draw(bitmap, cliprect, bg_tilemap, TILEMAP_IGNORE_TRANSPARENCY, 0);
		tilemap_draw(bitmap, cliprect, fg_tilemap, 0, 0);
		ddragon3_draw_sprites(bitmap, cliprect);
	}
}
