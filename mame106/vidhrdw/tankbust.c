/*
*   Video Driver for Tank Busters
*/

#include "driver.h"

/*
*   variables
*/

static tilemap *bg_tilemap;
static tilemap *txt_tilemap;
UINT8 * txt_ram;

/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

/* background tilemap :

0xc000-0xc7ff:  xxxx xxxx tile code: 8 lsb bits

0xc800-0xcfff:  .... .xxx tile code: 3 msb bits
                .... x... tile priority ON TOP of sprites (roofs and part of the rails)
                .xxx .... tile color code
                x... .... ?? set on *all* roofs (a different bg/sprite priority ?)

note:
 - seems that the only way to get color test right is to swap bit 1 and bit 0 of color code

*/

static void get_bg_tile_info(int tile_index)
{
	int code = videoram[tile_index];
	int attr = colorram[tile_index];

	int color = ((attr>>4) & 0x07);

	code |= (attr&0x07) * 256;


#if 0
	if (attr&0x08)	//priority bg/sprites (1 = this bg tile on top of sprites)
	{
		color = ((int)rand()) & 0x0f;
	}
	if (attr&0x80)  //al the roofs of all buildings have this bit set. What's this ???
	{
		color = ((int)rand()) & 0x0f;
	}
#endif

	/* priority bg/sprites (1 = this bg tile on top of sprites) */
	tile_info.priority = (attr & 0x08) >> 3;

	SET_TILE_INFO(	1,
			code,
			(color&4) | ((color&2)>>1) | ((color&1)<<1),
			0)
}

static void get_txt_tile_info(int tile_index)
{
	int code = txt_ram[tile_index];
	int color = ((code>>6) & 0x03);

	SET_TILE_INFO(	2,
			code & 0x3f,
			((color&2)>>1) | ((color&1)<<1),
			0)
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( tankbust )
{
	/* not scrollable */
	txt_tilemap = tilemap_create(get_txt_tile_info, tilemap_scan_rows, TILEMAP_TRANSPARENT, 8, 8, 64, 32);

	/* scrollable */
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows, TILEMAP_OPAQUE, 8, 8, 64, 32);


	if (!bg_tilemap || !txt_tilemap)
		return 1;

	tilemap_set_transparent_pen(txt_tilemap, 0);

	return 0;
}


/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( tankbust_background_videoram_w )
{
	if( videoram[offset]!=data )
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}
READ8_HANDLER( tankbust_background_videoram_r )
{
	return videoram[offset];
}

WRITE8_HANDLER( tankbust_background_colorram_w )
{
	if( colorram[offset]!=data )
	{
		colorram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}
READ8_HANDLER( tankbust_background_colorram_r )
{
	return colorram[offset];
}

WRITE8_HANDLER( tankbust_txtram_w )
{
	if( txt_ram[offset]!=data )
	{
		txt_ram[offset] = data;
		tilemap_mark_tile_dirty(txt_tilemap, offset);
	}
}
READ8_HANDLER( tankbust_txtram_r )
{
	return txt_ram[offset];
}


static UINT8 xscroll[2];

WRITE8_HANDLER( tankbust_xscroll_w )
{
	if( xscroll[offset] != data )
	{
		int x;

		xscroll[offset] = data;

		x = xscroll[0] + 256 * (xscroll[1]&1);
		if (x>=0x100) x-=0x200;
		tilemap_set_scrollx(bg_tilemap, 0, x );
	}
//ui_popup("x=%02x %02x", xscroll[0], xscroll[1]);
}

static UINT8 yscroll[2];

WRITE8_HANDLER( tankbust_yscroll_w )
{
	if( yscroll[offset] != data )
	{
		int y;

		yscroll[offset] = data;
		y = yscroll[0];
		if (y>=0x80) y-=0x100;
		tilemap_set_scrolly(bg_tilemap, 0, y );
	}
//ui_popup("y=%02x %02x", yscroll[0], yscroll[1]);
}

/***************************************************************************

  Display refresh

***************************************************************************/
/*
spriteram format (4 bytes per sprite):

    offset  0   x.......    flip X
    offset  0   .x......    flip Y
    offset  0   ..xxxxxx    gfx code (6 bits)

    offset  1   xxxxxxxx    y position

    offset  2   ??????..    not used ?
    offset  2   ......?.    used but unknown ??? (color code ? or x ?)
    offset  2   .......x    x position (1 MSB bit)

    offset  3   xxxxxxxx    x position (8 LSB bits)
*/

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs;

	for (offs = 0; offs < spriteram_size; offs += 4)
	{
		int code,color,sx,sy,flipx,flipy;

		code  = spriteram[offs+0] & 0x3f;
		flipy = spriteram[offs+0] & 0x40;
		flipx = spriteram[offs+0] & 0x80;

		sy = (240- spriteram[offs+1]) - 14;
		sx = (spriteram[offs+2] & 0x01) * 256 + spriteram[offs+3] - 7;

		color = 0;

		//0x02 - dont know (most of the time this bit is set in tank sprite and others but not all and not always)
		//0x04 - not used
		//0x08 - not used
		//0x10 - not used
		//0x20 - not used
		//0x40 - not used
		//0x80 - not used
#if 0
		if ((spriteram[offs+2] & 0x02))
		{
			code = ((int)rand()) & 63;
		}
#endif

		if ((spriteram[offs+1]!=4))	//otherwise - ghost sprites
		
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
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{

			
			dgp0.code = code;
			dgp0.color = color;
			dgp0.flipx = flipx;
			dgp0.flipy = flipy;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}
} // end of patch paragraph

	}
}


VIDEO_UPDATE( tankbust )
{
#if 0
	int i;

	for (i=0; i<0x800; i++)
	{
		int tile_attrib = colorram[i];

		if ( (tile_attrib&8) || (tile_attrib&0x80) )
		{
			tilemap_mark_tile_dirty(bg_tilemap, i);
		}
	}
#endif

	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	draw_sprites(bitmap, cliprect);
	tilemap_draw(bitmap, cliprect, bg_tilemap, 1, 0);

	tilemap_draw(bitmap, cliprect, txt_tilemap, 0,0);
}
