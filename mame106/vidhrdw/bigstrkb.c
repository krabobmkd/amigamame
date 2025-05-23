/* Big Striker (bootleg) Video Hardware */

#include "driver.h"

static tilemap *bsb_tilemap, *bsb_tilemap2, *bsb_tilemap3;

extern UINT16 *bsb_videoram, *bsb_videoram2, *bsb_videoram3;
extern UINT16 *bsb_vidreg1, *bsb_vidreg2;
extern UINT16 *bigstrkb_spriteram;

/* Sprites */

static void bigstrkb_drawsprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	/*- SPR RAM Format -**

     16 bytes per sprite

      nnnn nnnn  nnnn nnnn  aaaa aaaa  aaaa aaaa  xxxx xxxx  xxxx xxxx  yyyy yyyy  yyyy yyyy
        ( rest unused )
    **- End of Comments -*/

	const gfx_element *gfx = Machine->gfx[2];
	UINT16 *source = bigstrkb_spriteram;
	UINT16 *finish = source + 0x800/2;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		gfx, 	// gfx
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
	while( source<finish )
	{
		int xpos, ypos, num, attr;

		int flipx, col;

		xpos = source[2];
		ypos = source[3];
		num = source[0];
		attr = source[1];

		ypos = 0xffff - ypos;


		xpos -= 126;
		ypos -= 16;

		flipx = attr & 0x0100;
		col = attr & 0x000f;

		
		dgp0.code = num;
		dgp0.color = col;
		dgp0.flipx = flipx;
		dgp0.sx = xpos;
		dgp0.sy = ypos;
		drawgfx(&dgp0);
		source+=8;
	}
	} // end of patch paragraph

}

/* Tilemaps */

static UINT32 bsb_bg_scan(UINT32 col,UINT32 row,UINT32 num_cols,UINT32 num_rows)
{
	int offset;

	offset = ((col&0xf)*16) + (row&0xf);
	offset += (col >> 4) * 0x100;
	offset += (row >> 4) * 0x800;

	return offset;
}

static void get_bsb_tile_info(int tile_index)
{
	int tileno,col;

	tileno = bsb_videoram[tile_index] & 0x0fff;
	col= 	bsb_videoram[tile_index] & 0xf000;

	SET_TILE_INFO(0,tileno,col>>12,0)
}

WRITE16_HANDLER( bsb_videoram_w )
{
	if (bsb_videoram[offset] != data)
	{
		bsb_videoram[offset] = data;
		tilemap_mark_tile_dirty(bsb_tilemap,offset);
	}
}

static void get_bsb_tile2_info(int tile_index)
{
	int tileno,col;

	tileno = bsb_videoram2[tile_index] & 0x0fff;
	col= 	bsb_videoram2[tile_index] & 0xf000;

	SET_TILE_INFO(1,tileno,col>>12,0)
}

WRITE16_HANDLER( bsb_videoram2_w )
{
	if (bsb_videoram2[offset] != data)
	{
		bsb_videoram2[offset] = data;
		tilemap_mark_tile_dirty(bsb_tilemap2,offset);
	}
}


static void get_bsb_tile3_info(int tile_index)
{
	int tileno,col;

	tileno = bsb_videoram3[tile_index] & 0x0fff;
	col= 	bsb_videoram3[tile_index] & 0xf000;

	SET_TILE_INFO(1,tileno+0x2000,(col>>12)+(0x100/16),0)
}

WRITE16_HANDLER( bsb_videoram3_w )
{
	if (bsb_videoram3[offset] != data)
	{
		bsb_videoram3[offset] = data;
		tilemap_mark_tile_dirty(bsb_tilemap3,offset);
	}
}

/* Video Start / Update */

VIDEO_START(bigstrkb)
{
	bsb_tilemap = tilemap_create(get_bsb_tile_info,tilemap_scan_cols,TILEMAP_TRANSPARENT, 8, 8,64,32);
	bsb_tilemap2 = tilemap_create(get_bsb_tile2_info,bsb_bg_scan,TILEMAP_OPAQUE, 16, 16,128,64);
	bsb_tilemap3 = tilemap_create(get_bsb_tile3_info,bsb_bg_scan,TILEMAP_TRANSPARENT, 16, 16,128,64);

	if (!bsb_tilemap || !bsb_tilemap2 || !bsb_tilemap3)
		return 1;

	tilemap_set_transparent_pen(bsb_tilemap,15);
//  tilemap_set_transparent_pen(bsb_tilemap2,15);
	tilemap_set_transparent_pen(bsb_tilemap3,15);

 	return 0;
}

VIDEO_UPDATE(bigstrkb)
{
//  fillbitmap(bitmap,get_black_pen(),cliprect);

	tilemap_set_scrollx(bsb_tilemap2,0, bsb_vidreg1[0]+(256-14));
	tilemap_set_scrolly(bsb_tilemap2,0, bsb_vidreg2[0]);

	tilemap_set_scrollx(bsb_tilemap3,0, bsb_vidreg1[1]+(256-14));
	tilemap_set_scrolly(bsb_tilemap3,0, bsb_vidreg2[1]);

	tilemap_draw(bitmap,cliprect,bsb_tilemap2,0,0);
	tilemap_draw(bitmap,cliprect,bsb_tilemap3,0,0);

	bigstrkb_drawsprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,bsb_tilemap,0,0);

//  ui_popup ("Regs %08x %08x %08x %08x",bsb_vidreg2[0],bsb_vidreg2[1],bsb_vidreg2[2],bsb_vidreg2[3]);
}
