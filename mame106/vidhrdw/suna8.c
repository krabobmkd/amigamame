/***************************************************************************

                            -=  SunA 8 Bit Games =-

                    driver by   Luca Elia (l.elia@tin.it)

    These games have only sprites, of a peculiar type:

    there is a region of memory where 4 pages of 32x32 tile codes can
    be written like a tilemap made of 4 pages of 256x256 pixels. Each
    tile uses 2 bytes. Later games may use more pages through RAM
    banking.

    Sprites are rectangular regions of *tiles* fetched from there and
    sent to the screen. Each sprite uses 4 bytes, held within the last
    page of tiles.

    * Note: later games use a more complex format than the following,
            which is yet to be completely understood.

                            [ Sprites Format ]


    Offset:         Bits:               Value:

        0.b                             Y (Bottom up)

        1.b         7--- ----           Sprite Size (1 = 2x32 tiles; 0 = 2x2)

                    2x2 Sprites:
                    -65- ----           Tiles Row (height = 8 tiles)
                    ---4 ----           Page

                    2x32 Sprites:
                    -6-- ----           Ignore X (Multisprite)
                    --54 ----           Page

                    ---- 3210           Tiles Column (width = 2 tiles)

        2.b                             X

        3.b         7--- ----
                    -6-- ----           X (Sign Bit)
                    --54 3---
                    ---- -210           Tiles Bank


                        [ Sprite's Tiles Format ]


    Offset:         Bits:                   Value:

        0.b                             Code (Low Bits)

        1.b         7--- ----           Flip Y
                    -6-- ----           Flip X
                    --54 32--           Color
                    ---- --10           Code (High Bits)



    Set TILEMAPS to 1 to debug.
    Press Z (you see the "tilemaps" in RAM) or
    Press X (you see the "tilemaps" in ROM) then

    - use Q&W to cycle through the pages.
    - Use E&R to cycle through the tiles banks.
    - Use A&S to cycle through the ROM banks (only with X pressed, of course).

***************************************************************************/

#include "driver.h"

#define TILEMAPS 0

int suna8_text_dim; /* specifies format of text layer */

UINT8 suna8_rombank, suna8_spritebank, suna8_palettebank;
UINT8 suna8_unknown;

/* Functions defined in vidhrdw: */

WRITE8_HANDLER( suna8_spriteram_w );			// for debug
WRITE8_HANDLER( suna8_banked_spriteram_w );	// for debug

VIDEO_START( suna8 );
VIDEO_UPDATE( suna8 );


/***************************************************************************
    For Debug: there's no tilemap, just sprites.
***************************************************************************/
#if TILEMAPS
static tilemap *bg_tilemap;
static int tiles, rombank, page;

static void get_tile_info(int tile_index)
{
	UINT8 code, attr;
	if (code_pressed(KEYCODE_X))
	{	UINT8 *rom = memory_region(REGION_CPU1) + 0x10000 + 0x4000*rombank;
		code = rom[ 2 * tile_index + 0 ];
		attr = rom[ 2 * tile_index + 1 ];	}
	else
	{	code = spriteram[ 2 * tile_index + 0 ];
		attr = spriteram[ 2 * tile_index + 1 ];	}
	SET_TILE_INFO(
			0,
			( (attr & 0x03) << 8 ) + code + tiles*0x400,
			(attr >> 2) & 0xf,
			TILE_FLIPYX( (attr >> 6) & 3 ))
}
#endif


READ8_HANDLER( suna8_banked_paletteram_r )
{
	offset += suna8_palettebank * 0x200;
	return paletteram[offset];
}

READ8_HANDLER( suna8_banked_spriteram_r )
{
	offset += suna8_spritebank * 0x2000;
	return spriteram[offset];
}

WRITE8_HANDLER( suna8_spriteram_w )
{
	if (spriteram[offset] != data)
	{
		spriteram[offset] = data;
#if TILEMAPS
		tilemap_mark_tile_dirty(bg_tilemap,offset/2);
#endif
	}
}

WRITE8_HANDLER( suna8_banked_spriteram_w )
{
	offset += suna8_spritebank * 0x2000;
	if (spriteram[offset] != data)
	{
		spriteram[offset] = data;
#if TILEMAPS
		tilemap_mark_tile_dirty(bg_tilemap,offset/2);
#endif
	}
}

/*
    Banked Palette RAM. The data is scrambled
*/
WRITE8_HANDLER( brickzn_banked_paletteram_w )
{
	int r,g,b;
	UINT16 rgb;
	offset += suna8_palettebank * 0x200;
	paletteram[offset] = data;
	rgb = (paletteram[offset&~1] << 8) + paletteram[offset|1];
	r	=	(((rgb & (1<<0xc))?1:0)<<0) |
			(((rgb & (1<<0xb))?1:0)<<1) |
			(((rgb & (1<<0xe))?1:0)<<2) |
			(((rgb & (1<<0xf))?1:0)<<3);
	g	=	(((rgb & (1<<0x8))?1:0)<<0) |
			(((rgb & (1<<0x9))?1:0)<<1) |
			(((rgb & (1<<0xa))?1:0)<<2) |
			(((rgb & (1<<0xd))?1:0)<<3);
	b	=	(((rgb & (1<<0x4))?1:0)<<0) |
			(((rgb & (1<<0x3))?1:0)<<1) |
			(((rgb & (1<<0x6))?1:0)<<2) |
			(((rgb & (1<<0x7))?1:0)<<3);

	r = (r << 4) | r;
	g = (g << 4) | g;
	b = (b << 4) | b;
	palette_set_color(offset/2,r,g,b);
}



int suna8_vh_start_common(int dim)
{
	suna8_text_dim = dim;
	if (!(suna8_text_dim > 0))
	{
		paletteram	=	auto_malloc(0x200 * 2);
		spriteram	=	auto_malloc(0x2000 * 2);
		suna8_spritebank  = 0;
		suna8_palettebank = 0;
	}

#if TILEMAPS
	bg_tilemap = tilemap_create(	get_tile_info, tilemap_scan_cols,
								TILEMAP_TRANSPARENT,
								8,8,0x20*((suna8_text_dim > 0)?4:8),0x20);

	if ( bg_tilemap == NULL )	return 1;
	tilemap_set_transparent_pen(bg_tilemap,15);
#endif

	return 0;
}

VIDEO_START( suna8_textdim0 )	{ return suna8_vh_start_common(0);  }
VIDEO_START( suna8_textdim8 )	{ return suna8_vh_start_common(8);  }
VIDEO_START( suna8_textdim12 )	{ return suna8_vh_start_common(12); }

/***************************************************************************


                                Sprites Drawing


***************************************************************************/

void suna8_draw_normal_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int i;
	int mx = 0;	// multisprite x counter

	int max_x	=	Machine->drv->screen_width	- 8;
	int max_y	=	Machine->drv->screen_height - 8;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
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
	for (i = 0x1d00; i < 0x2000; i += 4)
	{
		int srcpg, srcx,srcy, dimx,dimy, tx, ty;
		int gfxbank, colorbank = 0, flipx,flipy, multisprite;

		int y		=	spriteram[i + 0];
		int code	=	spriteram[i + 1];
		int x		=	spriteram[i + 2];
		int bank	=	spriteram[i + 3];

		if (suna8_text_dim > 0)
		{
			/* Older, simpler hardware */
			flipx = 0;
			flipy = 0;
			gfxbank = bank & 0x3f;
			switch( code & 0x80 )
			{
			case 0x80:
				dimx = 2;					dimy =	32;
				srcx  = (code & 0xf) * 2;	srcy = 0;
				srcpg = (code >> 4) & 3;
				break;
			case 0x00:
			default:
				dimx = 2;					dimy =	2;
				srcx  = (code & 0xf) * 2;	srcy = ((code >> 5) & 0x3) * 8 + 6;
				srcpg = (code >> 4) & 1;
				break;
			}
			multisprite = ((code & 0x80) && (code & 0x40));
		}
		else
		{
			/* Newer, more complex hardware (not finished yet!) */
			switch( code & 0xc0 )
			{
			case 0xc0:
				dimx = 4;					dimy = 32;
				srcx  = (code & 0xe) * 2;	srcy = 0;
				flipx = (code & 0x1);
				flipy = 0;
				gfxbank = bank & 0x1f;
				srcpg = (code >> 4) & 3;
				break;
			case 0x80:
				dimx = 2;					dimy = 32;
				srcx  = (code & 0xf) * 2;	srcy = 0;
				flipx = 0;
				flipy = 0;
				gfxbank = bank & 0x1f;
				srcpg = (code >> 4) & 3;
				break;
// hardhea2: fire code=52/54 bank=a4; player code=02/04/06 bank=08; arrow:code=16 bank=27
			case 0x40:
				dimx = 4;					dimy = 4;
				srcx  = (code & 0xe) * 2;
				flipx = code & 0x01;
				flipy = bank & 0x10;
				srcy  = (((bank & 0x80)>>4) + (bank & 0x04) + ((~bank >> 4)&2)) * 2;
				srcpg = (code >> 4) & 7;
				gfxbank = (bank & 0x3) + (srcpg & 4);	// brickzn: 06,a6,a2,b2->6. starfigh: 01->01,4->0
				colorbank = (bank & 8) >> 3;
				break;
			case 0x00:
			default:
				dimx = 2;					dimy = 2;
				srcx  = (code & 0xf) * 2;
				flipx = 0;
				flipy = 0;
				gfxbank = bank & 0x03;
				srcy  = (((bank & 0x80)>>4) + (bank & 0x04) + ((~bank >> 4)&3)) * 2;
				srcpg = (code >> 4) & 3;
				break;
			}
			multisprite = ((code & 0x80) && (bank & 0x80));
		}

		x = x - ((bank & 0x40) ? 0x100 : 0);
		y = (0x100 - y - dimy*8 ) & 0xff;

		/* Multi Sprite */
		if ( multisprite )	{	mx += dimx*8;	x = mx;	}
		else					mx = x;

		gfxbank	*= 0x400;

		for (ty = 0; ty < dimy; ty ++)
		{
			for (tx = 0; tx < dimx; tx ++)
			{
				int addr	=	(srcpg * 0x20 * 0x20) +
								((srcx + (flipx?dimx-tx-1:tx)) & 0x1f) * 0x20 +
								((srcy + (flipy?dimy-ty-1:ty)) & 0x1f);

				int tile	=	spriteram[addr*2 + 0];
				int attr	=	spriteram[addr*2 + 1];

				int tile_flipx	=	attr & 0x40;
				int tile_flipy	=	attr & 0x80;

				int sx		=	 x + tx * 8;
				int sy		=	(y + ty * 8) & 0xff;

				if (flipx)	tile_flipx = !tile_flipx;
				if (flipy)	tile_flipy = !tile_flipy;

				if (flip_screen)
				{	sx = max_x - sx;	tile_flipx = !tile_flipx;
					sy = max_y - sy;	tile_flipy = !tile_flipy;	}

				
				dgp0.code = tile + (attr & 0x3)*0x100 + gfxbank;
				dgp0.color = ((attr >> 2) & 0xf) | colorbank;
				dgp0.flipx = // hardhea2 player2							tile_flipx;
				dgp0.flipy = tile_flipy;
				dgp0.sx = sx;
				dgp0.sy = sy;
				drawgfx(&dgp0);
			}
		}

	}
	} // end of patch paragraph

}

void suna8_draw_text_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int i;

	int max_x	=	Machine->drv->screen_width	- 8;
	int max_y	=	Machine->drv->screen_height - 8;

	/* Earlier games only */
	if (!(suna8_text_dim > 0))	return;

	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
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
	for (i = 0x1900; i < 0x19ff; i += 4)
	{
		int srcpg, srcx,srcy, dimx,dimy, tx, ty;

		int y		=	spriteram[i + 0];
		int code	=	spriteram[i + 1];
		int x		=	spriteram[i + 2];
		int bank	=	spriteram[i + 3];

		if (~code & 0x80)	continue;

		dimx = 2;					dimy = suna8_text_dim;
		srcx  = (code & 0xf) * 2;	srcy = (y & 0xf0) / 8;
		srcpg = (code >> 4) & 3;

		x = x - ((bank & 0x40) ? 0x100 : 0);
		y = 0;

		bank	=	(bank & 0x3f) * 0x400;

		for (ty = 0; ty < dimy; ty ++)
		{
			for (tx = 0; tx < dimx; tx ++)
			{
				int real_ty	=	(ty < (dimy/2)) ? ty : (ty + 0x20 - dimy);

				int addr	=	(srcpg * 0x20 * 0x20) +
								((srcx + tx) & 0x1f) * 0x20 +
								((srcy + real_ty) & 0x1f);

				int tile	=	spriteram[addr*2 + 0];
				int attr	=	spriteram[addr*2 + 1];

				int flipx	=	attr & 0x40;
				int flipy	=	attr & 0x80;

				int sx		=	 x + tx * 8;
				int sy		=	(y + real_ty * 8) & 0xff;

				if (flip_screen)
				{	sx = max_x - sx;	flipx = !flipx;
					sy = max_y - sy;	flipy = !flipy;	}

				
				dgp1.code = tile + (attr & 0x3)*0x100 + bank;
				dgp1.color = (attr >> 2) & 0xf;
				dgp1.flipx = flipx;
				dgp1.flipy = flipy;
				dgp1.sx = sx;
				dgp1.sy = sy;
				drawgfx(&dgp1);
			}
		}

	}
	} // end of patch paragraph

}

/***************************************************************************


                                Screen Drawing


***************************************************************************/

VIDEO_UPDATE( suna8 )
{
	/* see hardhead, hardhea2 test mode (press button 2 for both players) */
	fillbitmap(bitmap,Machine->pens[0xff],cliprect);

#ifdef MAME_DEBUG
#if TILEMAPS
	if (code_pressed(KEYCODE_Z) || code_pressed(KEYCODE_X))
	{
		int max_tiles = memory_region_length(REGION_GFX1) / (0x400 * 0x20);

		if (code_pressed_memory(KEYCODE_Q))	{ page--;	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);	}
		if (code_pressed_memory(KEYCODE_W))	{ page++;	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);	}
		if (code_pressed_memory(KEYCODE_E))	{ tiles--;	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);	}
		if (code_pressed_memory(KEYCODE_R))	{ tiles++;	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);	}
		if (code_pressed_memory(KEYCODE_A))	{ rombank--;	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);	}
		if (code_pressed_memory(KEYCODE_S))	{ rombank++;	tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);	}

		rombank  &= 0xf;
		page  &= (suna8_text_dim > 0)?3:7;
		tiles %= max_tiles;
		if (tiles < 0)	tiles += max_tiles;

		tilemap_set_scrollx( bg_tilemap, 0, 0x100 * page);
		tilemap_set_scrolly( bg_tilemap, 0, 0);
		tilemap_draw(bitmap,cliprect, bg_tilemap, 0, 0);
#if 1
	ui_popup("%02X %02X %02X %02X - p%2X g%02X r%02X",
						suna8_rombank, suna8_palettebank, suna8_spritebank, suna8_unknown,
						page,tiles,rombank	);
#endif
	}
	else
#endif
#endif
	{
		suna8_draw_normal_sprites(bitmap,cliprect);
		suna8_draw_text_sprites(bitmap,cliprect);
	}
}
