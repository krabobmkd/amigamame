/***************************************************************************

    Video Hardware for Blood Brothers

***************************************************************************/

#include "driver.h"

UINT16 *bloodbro_txvideoram;
UINT16 *bloodbro_bgvideoram,*bloodbro_fgvideoram;
UINT16 *bloodbro_scroll;

static tilemap *bg_tilemap,*fg_tilemap,*tx_tilemap;


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_bg_tile_info(int tile_index)
{
	int code = bloodbro_bgvideoram[tile_index];
	SET_TILE_INFO(
			1,
			code & 0xfff,
			code >> 12,
			0)
}

static void get_fg_tile_info(int tile_index)
{
	int code = bloodbro_fgvideoram[tile_index];
	SET_TILE_INFO(
			2,
			code & 0xfff,
			code >> 12,
			0)
}

static void get_tx_tile_info(int tile_index)
{
	int code = bloodbro_txvideoram[tile_index];
	SET_TILE_INFO(
			0,
			code & 0xfff,
			code >> 12,
			0)
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( bloodbro )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,     16,16,32,16);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,16);
	tx_tilemap = tilemap_create(get_tx_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,32,32);

	if (!bg_tilemap || !fg_tilemap || !tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,15);
	tilemap_set_transparent_pen(tx_tilemap,15);

	return 0;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE16_HANDLER( bloodbro_bgvideoram_w )
{
	int oldword = bloodbro_bgvideoram[offset];
	COMBINE_DATA(&bloodbro_bgvideoram[offset]);
	if (oldword != bloodbro_bgvideoram[offset])
		tilemap_mark_tile_dirty(bg_tilemap,offset);
}

WRITE16_HANDLER( bloodbro_fgvideoram_w )
{
	int oldword = bloodbro_fgvideoram[offset];
	COMBINE_DATA(&bloodbro_fgvideoram[offset]);
	if (oldword != bloodbro_fgvideoram[offset])
		tilemap_mark_tile_dirty(fg_tilemap,offset);
}

WRITE16_HANDLER( bloodbro_txvideoram_w )
{
	int oldword = bloodbro_txvideoram[offset];
	COMBINE_DATA(&bloodbro_txvideoram[offset]);
	if (oldword != bloodbro_txvideoram[offset])
		tilemap_mark_tile_dirty(tx_tilemap,offset);
}



/***************************************************************************

  Display refresh


    Blood Bros / Skysmash Spriteram
    -------------------------------

    Slightly more sophisticated successor to the Toki sprite chip.

    It has "big sprites" created by setting width or height >0. Tile
    numbers are read consecutively.

      +0   x....... ........  sprite disabled if set
    +0   .x...... ........  Flip y (no evidence for this!!)
    +0   ..x..... ........  Flip x
    +0   ....x... ........  Priority (1=high)
    +0   ......xx x.......  Width: do this many tiles horizontally
    +0   ........ .xxx....  Height: do this many tiles vertically
    +0   ........ ....xxxx  Color bank

    +1   ...xxxxx xxxxxxxx  Tile number
    +2   .......x xxxxxxxx  X coordinate
    +3   .......x xxxxxxxx  Y coordinate


    Weststry Bootleg Spriteram
    --------------------------

    Lacks the "big sprite" feature of the original. Needs some
    tile number remapping for some reason.

    +0   .......x xxxxxxxx  Sprite Y coordinate
    +1   ...xxxxx xxxxxxxx  Sprite tile number
    +2   xxxx.... ........  Sprite color bank
    +2   ......x. ........  Sprite flip x
    +2   ........ x.......  Priority ??
    +3   .......x xxxxxxxx  Sprite X coordinate

***************************************************************************/

/* SPRITE INFO (8 bytes)

   D-F?P?SS SSSSCCCC
   ---TTTTT TTTTTTTT
   -------X XXXXXXXX
   -------- YYYYYYYY */

static void bloodbro_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs;
	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[3], 	// gfx
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
		priority_bitmap, 	// pri_buffer
		0// pri_mask 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2;offs += 4)
	{
		int sx,sy,x,y,width,height,attributes,tile_number,color,flipx,flipy,pri_mask;

		attributes = spriteram16[offs+0];
		if (attributes & 0x8000) continue;	/* disabled */

		width = ((attributes>>7)&7);
		height = ((attributes>>4)&7);
		pri_mask = (attributes & 0x0800) ? 0x02 : 0;
		tile_number = spriteram16[offs+1]&0x1fff;
		sx = spriteram16[offs+2]&0x1ff;
		sy = spriteram16[offs+3]&0x1ff;
		if (sx >= 256) sx -= 512;
		if (sy >= 256) sy -= 512;

		flipx = attributes & 0x2000;
		flipy = attributes & 0x4000;	/* ?? */
		color = attributes & 0xf;

        dgp0.color = color;
        dgp0.flipx = flipx;
        dgp0.flipy = flipy;
        dgp0.priority_mask = pri_mask | (1<<31);

		for (x = 0;x <= width;x++)
		{
            dgp0.sx = flipx ? (sx + 16*(width-x)) : (sx + 16*x);
			for (y = 0;y <= height;y++)
			{				
				dgp0.code = tile_number++;
				dgp0.sy = flipy ? (sy + 16*(height-y)) : (sy + 16*y);
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph

}

/* SPRITE INFO (8 bytes)

   D------- YYYYYYYY
   ---TTTTT TTTTTTTT
   CCCC--F? -?--????  Priority??
   -------X XXXXXXXX
*/

static void weststry_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs;

	/* TODO: the last two entries are not sprites - control registers? */
	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[3], 	// gfx
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
		priority_bitmap, 	// pri_buffer
		0//pri_mask 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2 - 8;offs += 4)
	{
		int data = spriteram16[offs+2];
		int data0 = spriteram16[offs+0];
		int code = spriteram16[offs+1]&0x1fff;
		int sx = spriteram16[offs+3]&0x1ff;
		int sy = 0xf0-(data0&0xff);
		int flipx = data & 0x200;
		int flipy = data & 0x400;	/* ??? */
		int color = (data&0xf000)>>12;
		int pri_mask = (data & 0x0080) ? 0x02 : 0;

        dgp1.priority_mask = pri_mask | (1<<31);
		if (sx >= 256) sx -= 512;

		if (data0 & 0x8000) continue;	/* disabled */

		/* Remap code 0x800 <-> 0x1000 */
		code = (code&0x7ff) | ((code&0x800)<<1) | ((code&0x1000)>>1);

		
		dgp1.code = code;
		dgp1.color = color;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = sx;
		dgp1.sy = sy;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph

}



VIDEO_UPDATE( bloodbro )
{
	tilemap_set_scrollx(bg_tilemap,0,bloodbro_scroll[0x10]);	/* ? */
	tilemap_set_scrolly(bg_tilemap,0,bloodbro_scroll[0x11]);	/* ? */
	tilemap_set_scrollx(fg_tilemap,0,bloodbro_scroll[0x12]);
	tilemap_set_scrolly(fg_tilemap,0,bloodbro_scroll[0x13]);

	fillbitmap(priority_bitmap,0,cliprect);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,1);
	bloodbro_draw_sprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( weststry )
{
//  tilemap_set_scrollx(bg_tilemap,0,bloodbro_scroll[0x10]);    /* ? */
//  tilemap_set_scrolly(bg_tilemap,0,bloodbro_scroll[0x11]);    /* ? */
//  tilemap_set_scrollx(fg_tilemap,0,bloodbro_scroll[0x12]);
//  tilemap_set_scrolly(fg_tilemap,0,bloodbro_scroll[0x13]);

	fillbitmap(priority_bitmap,0,cliprect);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,1);
	weststry_draw_sprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}


VIDEO_UPDATE( skysmash )
{
	tilemap_set_scrollx(bg_tilemap,0,bloodbro_scroll[0x08]);
	tilemap_set_scrolly(bg_tilemap,0,bloodbro_scroll[0x09]);	/* ? */
	tilemap_set_scrollx(fg_tilemap,0,bloodbro_scroll[0x0a]);
	tilemap_set_scrolly(fg_tilemap,0,bloodbro_scroll[0x0b]);	/* ? */

	fillbitmap(priority_bitmap,0,cliprect);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,1);
	bloodbro_draw_sprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}


