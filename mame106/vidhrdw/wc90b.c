#include "driver.h"


UINT8 *wc90b_fgvideoram,*wc90b_bgvideoram,*wc90b_txvideoram;

UINT8 *wc90b_scroll1x;
UINT8 *wc90b_scroll2x;

UINT8 *wc90b_scroll1y;
UINT8 *wc90b_scroll2y;


static tilemap *tx_tilemap,*fg_tilemap,*bg_tilemap;




/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_bg_tile_info(int tile_index)
{
	int attr = wc90b_bgvideoram[tile_index];
	int tile = wc90b_bgvideoram[tile_index + 0x800];
	SET_TILE_INFO(
			9 + ((attr & 3) + ((attr >> 1) & 4)),
			tile,
			attr >> 4,
			0)
}

static void get_fg_tile_info(int tile_index)
{
	int attr = wc90b_fgvideoram[tile_index];
	int tile = wc90b_fgvideoram[tile_index + 0x800];
	SET_TILE_INFO(
			1 + ((attr & 3) + ((attr >> 1) & 4)),
			tile,
			attr >> 4,
			0)
}

static void get_tx_tile_info(int tile_index)
{
	SET_TILE_INFO(
			0,
			wc90b_txvideoram[tile_index + 0x800] + ((wc90b_txvideoram[tile_index] & 0x07) << 8),
			wc90b_txvideoram[tile_index] >> 4,
			0)
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( wc90b )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,     16,16,64,32);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,32);
	tx_tilemap = tilemap_create(get_tx_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,64,32);

	if (!bg_tilemap || !fg_tilemap || !tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,15);
	tilemap_set_transparent_pen(tx_tilemap,15);

	return 0;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( wc90b_bgvideoram_w )
{
	if (wc90b_bgvideoram[offset] != data)
	{
		wc90b_bgvideoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset & 0x7ff);
	}
}

WRITE8_HANDLER( wc90b_fgvideoram_w )
{
	if (wc90b_fgvideoram[offset] != data)
	{
		wc90b_fgvideoram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap,offset & 0x7ff);
	}
}

WRITE8_HANDLER( wc90b_txvideoram_w )
{
	if (wc90b_txvideoram[offset] != data)
	{
		wc90b_txvideoram[offset] = data;
		tilemap_mark_tile_dirty(tx_tilemap,offset & 0x7ff);
	}
}



/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect, int priority ){
  int offs;

  /* draw all visible sprites of specified priority */
	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[ 17 ], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		/* sy */						cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for ( offs = spriteram_size - 8;offs >= 0;offs -= 8 ){

		if ( ( ~( spriteram[offs+3] >> 6 ) & 3 ) == priority ) {

			if ( spriteram[offs+1] > 16 ) { /* visible */
				int code = ( spriteram[offs+3] & 0x3f ) << 4;
				int bank = spriteram[offs+0];
				int flags = spriteram[offs+4];

				code += ( bank & 0xf0 ) >> 4;
				code <<= 2;
				code += ( bank & 0x0f ) >> 2;

				
				dgp0.code = code;
				dgp0.color = flags >> 4;
				dgp0.flipx = /* color */						bank&1;
				dgp0.flipy = /* flipx */						bank&2;
				dgp0.sx = /* flipy */						spriteram[offs + 2];
				dgp0.sy = /* sx */						240 - spriteram[offs + 1];
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( wc90b )
{
	tilemap_set_scrollx(bg_tilemap,0,8 * wc90b_scroll2x[0] + 256);
	tilemap_set_scrolly(bg_tilemap,0,wc90b_scroll2y[0] + ((wc90b_scroll2y[0] < 0x10 || wc90b_scroll2y[0] == 0xff) ? 256 : 0));
	tilemap_set_scrollx(fg_tilemap,0,8 * wc90b_scroll1x[0] + 256);
	tilemap_set_scrolly(fg_tilemap,0,wc90b_scroll1y[0] + ((wc90b_scroll1y[0] < 0x10 || wc90b_scroll1y[0] == 0xff) ? 256 : 0));

//  draw_sprites( bitmap,cliprect, 3 );
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites( bitmap,cliprect, 2 );
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
	draw_sprites( bitmap,cliprect, 1 );
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
	draw_sprites( bitmap,cliprect, 0 );
}
