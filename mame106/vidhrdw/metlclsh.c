/***************************************************************************

                              -= Metal Clash =-

    Sprites:
        16x16 tiles with 8 pens and 2 color codes,
        each sprite can be 1 or 2 tiles tall

    Background:
        size is 512x256 (scrolling, peculiar memory layout),
        16x16 tiles with 8 pens and no color code,
        1 byte per tile (tiles are banked)

    Foreground:
        size is 256x256 (no scrolling),
        8x8 tiles with 4 pens and 4 color codes,
        2 bytes per tile, low and high per tile priority

***************************************************************************/

#include "driver.h"

/* Local variables: */

static tilemap *bg_tilemap,*fg_tilemap;

static UINT8 metlclsh_write_mask, *metlclsh_otherram;

/* Variables that driver has access to: */

UINT8 *metlclsh_bgram, *metlclsh_fgram, *metlclsh_scrollx;

/* Functions that driver has access to: */

WRITE8_HANDLER( metlclsh_rambank_w )
{
	if (data & 1)
	{
		metlclsh_write_mask = 0;
		memory_set_bankptr(1, metlclsh_bgram);
	}
	else
	{
		metlclsh_write_mask = 1 << (data >> 1);
		memory_set_bankptr(1, metlclsh_otherram);
	}
}

static UINT8 metlclsh_gfxbank;
WRITE8_HANDLER( metlclsh_gfxbank_w )
{
	if (!(data & 4) && (metlclsh_gfxbank != data))
	{
		tilemap_mark_all_tiles_dirty(bg_tilemap);
		metlclsh_gfxbank = data & 3;
	}
}

/***************************************************************************

                            Background tilemap

                memory offset of each tile of the tilemap:

                    00 08  ..  78 100 108  ..  178
                     .  .       .   .   .        .
                     .  .       .   .   .        .
                    07 0f  ..  7f 107 10f  ..  17f
                    80 88  ..  f8 180 188  ..  1f8
                     .  .       .   .   .        .
                     .  .       .   .   .        .
                    87 8f  ..  ff 187 18f  ..  1ff

***************************************************************************/

UINT32 metlclsh_bgtilemap_scan(UINT32 col,UINT32 row,UINT32 num_cols,UINT32 num_rows)
{
	return	(row & 7) + ((row & ~7) << 4) + ((col & 0xf) << 3) + ((col & ~0xf) << 4);
}

static void get_bg_tile_info(int tile_index)
{
	SET_TILE_INFO(1, metlclsh_bgram[tile_index] + (metlclsh_gfxbank << 7),0,0)
}

WRITE8_HANDLER( metlclsh_bgram_w )
{
	/*  This ram is banked: it's either the tilemap (e401 = 1)
        or bit n of another area (e401 = n << 1)? (that I don't understand) */
	if (metlclsh_write_mask)
	{
		/* unknown area - the following is almost surely wrong */
// 405b (e401 = e c a 8 6 4 2 0) writes d400++
// 4085 (e401 = e c a 8 6 4 2 0) writes d400++
// 4085 (e401 = e a 6 2) writes d000++
// 405b (e401 = e a 6 2) writes d000++

//      metlclsh_otherram[offset] |= (data & metlclsh_write_mask);
		metlclsh_otherram[offset] = (metlclsh_otherram[offset] & ~metlclsh_write_mask) | (data & metlclsh_write_mask);
	}
	else
	{
		/* tilemap */
		if (metlclsh_bgram[offset] != data)
		{
			metlclsh_bgram[offset] = data;
			tilemap_mark_tile_dirty(bg_tilemap,offset & 0x1ff);
		}
	}
}

/***************************************************************************

                            Foreground tilemap

    Offset:     Bits:           Value:

        0x000                   Code
        0x400   7--- ----       Priority (0/1 -> over/below sprites and background)
                -65- ----       Color
                ---- --10       Code

***************************************************************************/

static void get_fg_tile_info(int tile_index)
{
	UINT8 code = metlclsh_fgram[tile_index + 0x000];
	UINT8 attr = metlclsh_fgram[tile_index + 0x400];
	SET_TILE_INFO(2, code + ((attr & 0x03) << 8), (attr >> 5) & 3, 0)
	tile_info.priority = ((attr & 0x80) ? 1 : 2);
}

WRITE8_HANDLER( metlclsh_fgram_w )
{
	if (metlclsh_fgram[offset] != data)
	{
		metlclsh_fgram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap,offset & 0x3ff);
	}
}


/***************************************************************************

                            Video Hardware Init

***************************************************************************/

VIDEO_START( metlclsh )
{
	metlclsh_otherram = auto_malloc(0x800);	// banked ram

	bg_tilemap = tilemap_create(get_bg_tile_info,metlclsh_bgtilemap_scan,TILEMAP_TRANSPARENT,16,16,32,16);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);

	if (!bg_tilemap || !fg_tilemap)
		return 1;

	tilemap_set_transparent_pen( bg_tilemap, 0 );
	tilemap_set_transparent_pen( fg_tilemap, 0 );

	return 0;
}


/***************************************************************************

                                Sprites Drawing

    Offset:     Bits:           Value:

        0       7--- ----       0
                -65- ----       Code (high bits)
                ---4 ----       Double height (2 tiles)
                ---- 3---       Color
                ---- -2--       Flip X
                ---- --1-       Flip Y
                ---- ---0       Enable

        1                       Code (low bits)
        2                       Y (bottom -> top)
        3                       X (right -> left)

***************************************************************************/

static void metlclsh_draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	gfx_element *gfx = Machine->gfx[0];
	int offs;

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size; offs += 4)
	{
		int attr,code,color,sx,sy,flipx,flipy,wrapy,sizey;

		attr	=	spriteram[offs];
		if (!(attr & 0x01))	continue;	// enable

		flipy	=	(attr & 0x02);
		flipx	=	(attr & 0x04);
		color	=	(attr & 0x08) >> 3;
		sizey	=	(attr & 0x10);	// double height
		code	=	((attr & 0x60) << 3) + spriteram[offs+1];

		sx	=	240 - spriteram[offs+3];
		if (sx < -7) sx += 256;
		sy	=	240 - spriteram[offs+2];

		if (flip_screen)
		{
			sx = 240 - sx;	flipx = !flipx;
			sy = 240 - sy;	flipy = !flipy;		if (sizey)	sy+=16;
			if (sy > 240)	sy -= 256;
		}

		/* Draw twice, at sy and sy + 256 (wrap around) */
		for ( wrapy = 0; wrapy <= 256; wrapy += 256 )
		{
			if (sizey)
			{
				
				dgp0.code = code & ~1;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = sx;
				dgp0.sy = sy + (flipy ? 0 : -16) + wrapy;
				drawgfx(&dgp0);

				
				dgp0.code = code |  1;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = sx;
				dgp0.sy = sy + (flipy ? -16 : 0) + wrapy;
				drawgfx(&dgp0);
			}
			else
			{
				
				dgp0.code = code;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = sx;
				dgp0.sy = sy + wrapy;
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph

}

/***************************************************************************

                                Screen Drawing

    Video register e402 (metlclsh seems to only use the values 0,8,9,b):

        7654 ----       0
        ---- 3---       Background enable
        ---- -2--       0
        ---- --1-       Background scroll x high bit
        ---- ---0       ? (not bg flipx!)

***************************************************************************/

VIDEO_UPDATE( metlclsh )
{
	fillbitmap(bitmap,Machine->pens[0x10],cliprect);

	tilemap_draw(bitmap,cliprect,fg_tilemap,1,0);	// low priority tiles of foreground
	if (metlclsh_scrollx[0] & 0x08)					// background (if enabled)
	{
		/* The background seems to be always flipped along x */
		tilemap_set_flip(bg_tilemap, (flip_screen ? (TILEMAP_FLIPX|TILEMAP_FLIPY) : 0) ^ TILEMAP_FLIPX);
		tilemap_set_scrollx(bg_tilemap, 0,metlclsh_scrollx[1] + ((metlclsh_scrollx[0]&0x02)<<7) );
		tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	}
	metlclsh_draw_sprites(bitmap,cliprect);			// sprites
	tilemap_draw(bitmap,cliprect,fg_tilemap,2,0);	// high priority tiles of foreground

//  ui_popup("%02X",metlclsh_scrollx[0]);
}

