/***************************************************************************

  Video Hardware for some Technos games:
    Double Dragon, Double Dragon bootleg, Double Dragon II and China Gate

  Two Tile layers.
    Background layer is 512x512 , tiles are 16x16
    Top        layer is 256x256 , tiles are 8x8

  Sprites are 16x16, 16x32, 32x16, or 32x32 (attribute bits set dimension)


BG Tile Layout
  0          1
  ---- -xxx  xxxx xxxx  = Tile number
  --xx x---  ---- ----  = Color
  -x-- ----  ---- ----  = X Flip
  x--- ----  ---- ----  = Y Flip


Top Tile layout.
  0          1
  ---- xxxx  xxxx xxxx  = Tile number
  xxxx ----  ---- ----  = Color (China Gate)
  xxx- ----  ---- ----  = Color (Double Dragon)


Sprite layout.
  0          1          2          3          4
  ---- ----  ---- ----  ---- xxxx  xxxx xxxx  ---- ----  = Sprite number
  ---- ----  ---- ----  -xxx ----  ---- ----  ---- ----  = Color
  xxxx xxxx  ---- ----  ---- ----  ---- ----  ---- ----  = Y position
  ---- ----  ---- ---x  ---- ----  ---- ----  ---- ----  = Y MSb position ???
  ---- ----  ---- ----  ---- ----  ---- ----  xxxx xxxx  = X position
  ---- ----  ---- --x-  ---- ----  ---- ----  ---- ----  = X MSb position ???
  ---- ----  ---- -x--  ---- ----  ---- ----  ---- ----  = Y Flip
  ---- ----  ---- x---  ---- ----  ---- ----  ---- ----  = X Flip
  ---- ----  --xx ----  ---- ----  ---- ----  ---- ----  = Sprite Dimension
  ---- ----  x--- ----  ---- ----  ---- ----  ---- ----  = Visible

***************************************************************************/

#include "driver.h"
#include "drawgfxn.h"
#include <stdio.h>

unsigned char *ddragon_bgvideoram,*ddragon_fgvideoram;
int ddragon_scrollx_hi, ddragon_scrolly_hi;
unsigned char *ddragon_scrollx_lo;
unsigned char *ddragon_scrolly_lo;
unsigned char *ddragon_spriteram;
int technos_video_hw;

static tilemap *fg_tilemap,*bg_tilemap;



/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static UINT32 background_scan(UINT32 col,UINT32 row,UINT32 num_cols,UINT32 num_rows)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x0f) + ((row & 0x0f) << 4) + ((col & 0x10) << 4) + ((row & 0x10) << 5);
}

static void get_bg_tile_info(int tile_index)
{
	unsigned char attr = ddragon_bgvideoram[2*tile_index];
	SET_TILE_INFO(
			2,
			ddragon_bgvideoram[2*tile_index+1] + ((attr & 0x07) << 8),
			(attr >> 3) & 0x07,
			TILE_FLIPYX((attr & 0xc0) >> 6))
}

static void get_fg_tile_info(int tile_index)
{
	unsigned char attr = ddragon_fgvideoram[2*tile_index];
	SET_TILE_INFO(
			0,
			ddragon_fgvideoram[2*tile_index+1] + ((attr & 0x07) << 8),
			attr >> 5,
			0)
}

static void get_fg_16color_tile_info(int tile_index)
{
	unsigned char attr = ddragon_fgvideoram[2*tile_index];
	SET_TILE_INFO(
			0,
			ddragon_fgvideoram[2*tile_index+1] + ((attr & 0x0f) << 8),
			attr >> 4,
			0)
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( ddragon )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,background_scan,  TILEMAP_OPAQUE,     16,16,32,32);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,32,32);

	if (!bg_tilemap || !fg_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);

	return 0;
}

VIDEO_START( chinagat )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,background_scan,  TILEMAP_OPAQUE,     16,16,32,32);
	fg_tilemap = tilemap_create(get_fg_16color_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,32,32);

	if (!bg_tilemap || !fg_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);

	return 0;
}


/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( ddragon_bgvideoram_w )
{
	if (ddragon_bgvideoram[offset] != data)
	{
		ddragon_bgvideoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset/2);
	}
}

WRITE8_HANDLER( ddragon_fgvideoram_w )
{
	if (ddragon_fgvideoram[offset] != data)
	{
		ddragon_fgvideoram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap,offset/2);
	}
}


/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{

    rectangle lcliprect = *cliprect;
	lcliprect.max_x++;
	lcliprect.max_y++;
	//const gfx_element *gfx = Machine->gfx[1];
    struct drawgfxParams dgp={
            bitmap, Machine->gfx[1],
            0,//code,
            0,//color,
            0,0,//flipx, flipy,
            0,0,// //sx, sy,
            &lcliprect,
            TRANSPARENCY_PEN, 0,
            // - - optionals
            0,0,NULL,0
    };
	UINT8 *src;
	int i;

	if ( technos_video_hw == 1 ) {		/* China Gate Sprite RAM */
		src = (UINT8 *) (spriteram);
	} else {
		src = (UINT8 *) (&( ddragon_spriteram[0x800] ));
	}

	for( i = 0; i < ( 64 * 5 ); i += 5 ) {
		int attr = src[i+1];
		if ( attr & 0x80 ) { /* visible */
			int sx = 240 - src[i+4] + ( ( attr & 2 ) << 7 );
			int sy = 240 - src[i+0] + ( ( attr & 1 ) << 8 );
			int size = ( attr & 0x30 ) >> 4;
			int flipx = ( attr & 8 );
			int flipy = ( attr & 4 );
			int dx = -16,dy = -16;

			int which;
			int color;

			if ( technos_video_hw == 2 )		/* Double Dragon 2 */
			{
				color = ( src[i+2] >> 5 );
				which = src[i+3] + ( ( src[i+2] & 0x1f ) << 8 );
			}
			else
			{
				if ( technos_video_hw == 1 )		/* China Gate */
				{
					if ((sx < -7) && (sx > -16)) sx += 256; /* fix sprite clip */
					if ((sy < -7) && (sy > -16)) sy += 256; /* fix sprite clip */
				}
				color = ( src[i+2] >> 4 ) & 0x07;
				which = src[i+3] + ( ( src[i+2] & 0x0f ) << 8 );
			}

			if (flip_screen)
			{
				sx = 240 - sx;
				sy = 240 - sy;
				flipx = !flipx;
				flipy = !flipy;
				dx = -dx;
				dy = -dy;
			}

			which &= ~size;

            dgp.code = (which+size);
            dgp.color = color;
            dgp.flipx = flipx;
            dgp.flipy = flipy;

            dgp.sx = sx;
            dgp.sy = sy;
            drawgfx_clut16_Src8_pal(&dgp);

			switch ( size ) {
				case 1: /* double y */
                    dgp.code = which;
                    dgp.sy = sy+dy;
                    drawgfx_clut16_Src8_pal(&dgp);
				break;
				case 2: /* double x */
                    dgp.code = which;
                    dgp.sx = sx+dx;
                    drawgfx_clut16_Src8_pal(&dgp);
				break;

				case 3:
                    dgp.code = which;
                    dgp.sx = sx+dx;
                    dgp.sy = sy+dy;
                    drawgfx_clut16_Src8_pal(&dgp);

                    dgp.code = which+1;
                    dgp.sx = sx+dx;
                    dgp.sy = sy;
                    drawgfx_clut16_Src8_pal(&dgp);

                    dgp.code = which+2;
                    dgp.sx = sx;
                    dgp.sy = sy+dy;
                    drawgfx_clut16_Src8_pal(&dgp);
				break;
			}
		}
	}
}

VIDEO_UPDATE( ddragon )
{
	int scrollx = ddragon_scrollx_hi + *ddragon_scrollx_lo;
	int scrolly = ddragon_scrolly_hi + *ddragon_scrolly_lo;

	tilemap_set_scrollx(bg_tilemap,0,scrollx);
	tilemap_set_scrolly(bg_tilemap,0,scrolly);

	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
}
