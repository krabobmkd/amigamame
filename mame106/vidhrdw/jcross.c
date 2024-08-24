#include "driver.h"
#include "cpu/z80/z80.h"

static int flipscreen;
static tilemap *bg_tilemap,  *tx_tilemap;
static UINT8 fg_color, old_fg_color;

extern UINT8 *jcr_textram;


int jcross_vregs[5];

WRITE8_HANDLER( jcross_palettebank_w )
{
	fg_color = data&0xf;
}


static void stuff_palette( int source_index, int dest_index, int num_colors )
{
	UINT8 *color_prom = memory_region(REGION_PROMS) + source_index;
	int i;
	for( i=0; i<num_colors; i++ )
	{
		int bit0=0,bit1,bit2,bit3;
		int red, green, blue;

		bit0 = (color_prom[0x800] >> 2) & 0x01; // ?
		bit1 = (color_prom[0x000] >> 1) & 0x01;
		bit2 = (color_prom[0x000] >> 2) & 0x01;
		bit3 = (color_prom[0x000] >> 3) & 0x01;
		red = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		bit0 = (color_prom[0x800] >> 1) & 0x01; // ?
		bit1 = (color_prom[0x400] >> 2) & 0x01;
		bit2 = (color_prom[0x400] >> 3) & 0x01;
		bit3 = (color_prom[0x000] >> 0) & 0x01;
		green = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		bit0 = (color_prom[0x800] >> 0) & 0x01; // ?
		bit1 = (color_prom[0x800] >> 3) & 0x01; // ?
		bit2 = (color_prom[0x400] >> 0) & 0x01;
		bit3 = (color_prom[0x400] >> 1) & 0x01;
		blue = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		palette_set_color( dest_index++, red, green, blue );
		color_prom++;
	}
}

static void update_palette( int type )
{
	if( fg_color!=old_fg_color )
	{
		stuff_palette( 128+16*(fg_color&0x7), (0x10+type)*16, 16 );
		old_fg_color = fg_color;
	}
}

WRITE8_HANDLER( jcross_background_ram_w )
{
	videoram[offset]=data;
	tilemap_mark_tile_dirty(bg_tilemap,offset);
}

WRITE8_HANDLER( jcross_text_ram_w )
{
	jcr_textram[offset]=data;
	tilemap_mark_tile_dirty(tx_tilemap,offset);
}


static void get_bg_tilemap_info(int tile_index)
{
	SET_TILE_INFO(
			1,
			videoram[tile_index],
			0,
			0)
}

static void get_tx_tilemap_info(int tile_index)
{
	int tile_number = jcr_textram[tile_index];
	SET_TILE_INFO(
			0,
			tile_number,
			0,
			0)
}

VIDEO_START( jcross )
{
	flipscreen = -1;  old_fg_color = -1;

	stuff_palette( 0, 0, 16*8 );
	stuff_palette( 16*8*3, 16*8, 16*8 );

	bg_tilemap = tilemap_create(get_bg_tilemap_info,tilemap_scan_cols,TILEMAP_OPAQUE,8,8,64,64);
	tx_tilemap = tilemap_create(get_tx_tilemap_info,tilemap_scan_cols,TILEMAP_TRANSPARENT,8,8,32,32);

	if (!bg_tilemap ||  !tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(bg_tilemap,0x0f);
	tilemap_set_transparent_pen(tx_tilemap,0xf);
	tilemap_set_scrolldx( bg_tilemap,   16, 22 );
	return 0;

}

/***************************************************************************
**
**  Screen Refresh
**
***************************************************************************/

static void draw_status( mame_bitmap *bitmap, const rectangle *cliprect )
{
	const UINT8 *base =  jcr_textram + 0x400;
	const gfx_element *gfx = Machine->gfx[0];
	int row;
	
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
		TRANSPARENCY_NONE, 	// transparency
		0xf, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for( row=0; row<4; row++ )
	{
		int sy,sx = (row&1)*8;
		const UINT8 *source = base + (row&1)*32;
		if( row>1 )
			sx+=256+16;
		else
			source+=30*32;

		for( sy=0; sy<256; sy+=8 )
		{
			int tile_number = *source++;
			
			dgp0.code = tile_number;
			dgp0.color = tile_number>>5;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect, int scrollx, int scrolly )
{
	const gfx_element *gfx = Machine->gfx[3];
	const UINT8 *source, *finish;
	source = spriteram;
	finish = spriteram + 0x64;

	
	{ 
	struct drawgfxParams dgp1={
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
		7, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	while( source<finish )
	{
		int attributes = source[3];
		int tile_number = source[1];
		int sy = source[0] + ((attributes&0x10)?256:0);
		int sx = source[2] + ((attributes&0x80)?256:0);
		int color = attributes&0xf;
		int flipy = (attributes&0x20);
		int flipx = 0;

		sy=(sy-scrolly)&0x1ff;
		sx=(sx-scrollx)&0x1ff;

		
		dgp1.code = tile_number;
		dgp1.color = color;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = (256-sx)&0x1ff;
		dgp1.sy = sy-16;
		drawgfx(&dgp1);

		source+=4;
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( jcross )
{
	int scroll_attributes = jcross_vregs[0];
	int sprite_scrolly = jcross_vregs[1];
	int sprite_scrollx = jcross_vregs[2];
	int bg_scrolly = jcross_vregs[3];
	int bg_scrollx = jcross_vregs[4];

	if( scroll_attributes & 1 ) sprite_scrollx += 256;
	if( scroll_attributes & 2 ) bg_scrollx += 256;

	if( scroll_attributes & 8 ) sprite_scrolly += 256;
	if( scroll_attributes & 0x10 ) bg_scrolly += 256;
	update_palette(1);


	tilemap_set_scrollx( bg_tilemap, 0, bg_scrollx );
	tilemap_set_scrolly( bg_tilemap, 0, bg_scrolly );
	tilemap_draw( bitmap,cliprect,bg_tilemap,0 ,0);
	draw_sprites( bitmap,cliprect, sprite_scrollx+23, sprite_scrolly+1 );
	tilemap_draw( bitmap,cliprect,tx_tilemap,0 ,0);
	draw_status( bitmap,cliprect );
}
