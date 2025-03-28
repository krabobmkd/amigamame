/******************************************************************

Mr. F. Lea
(C) 1983 PACIFIC NOVELTY MFG. INC.

******************************************************************/

#include "driver.h"

static int mrflea_gfx_bank;

WRITE8_HANDLER( mrflea_gfx_bank_w ){
	mrflea_gfx_bank = data;
	if( data & ~0x14 ){
		logerror( "unknown gfx bank: 0x%02x\n", data );
	}
}

WRITE8_HANDLER( mrflea_videoram_w ){
	int bank = offset/0x400;
	offset &= 0x3ff;
	videoram[offset] = data;
	videoram[offset+0x400] = bank;
	/*  the address range that tile data is written to sets one bit of
    **  the bank select.  The remaining bits are from a video register.
    */
}

WRITE8_HANDLER( mrflea_spriteram_w ){
	if( offset&2 ){ /* tile_number */
		spriteram[offset|1] = offset&1;
		offset &= ~1;
	}
	spriteram[offset] = data;
}

static void draw_sprites( mame_bitmap *bitmap ){
	const gfx_element *gfx = Machine->gfx[0];
	const UINT8 *source = spriteram;
	const UINT8 *finish = source+0x100;
	rectangle clip = Machine->visible_area;
	clip.max_x -= 24;
	clip.min_x += 16;
	
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
		&clip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	while( source<finish ){
		int xpos = source[1]-3;
		int ypos = source[0]-16+3;
		int tile_number = source[2]+source[3]*0x100;

		
		dgp0.code = tile_number;
		dgp0.flipx = /* color */			0;
		dgp0.sx = /* no flip */			xpos;
		dgp0.sy = ypos;
		drawgfx(&dgp0);
		
		dgp0.code = tile_number;
		dgp0.flipx = /* color */			0;
		dgp0.sx = /* no flip */			xpos;
		dgp0.sy = 256+ypos;
		drawgfx(&dgp0);
		source+=4;
	}
	} // end of patch paragraph

}

static void draw_background( mame_bitmap *bitmap ){
	const UINT8 *source = videoram;
	const gfx_element *gfx = Machine->gfx[1];
	int sx,sy;
	int base = 0;
	if( mrflea_gfx_bank&0x04 ) base |= 0x400;
	if( mrflea_gfx_bank&0x10 ) base |= 0x200;
	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		gfx, 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		0, 	// clip
		/* no clip */				TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for( sy=0; sy<256; sy+=8 ){
		for( sx=0; sx<256; sx+=8 ){
			int tile_number = base+source[0]+source[0x400]*0x100;
			source++;
			
			dgp2.code = tile_number;
			dgp2.flipx = /* color */				0;
			dgp2.sx = /* no flip */				sx;
			dgp2.sy = sy;
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph

}

VIDEO_START( mrflea ){
	return 0;
}

VIDEO_UPDATE( mrflea )
{
	draw_background( bitmap );
	draw_sprites( bitmap );
}
