/* Speed Spin Vidhrdw, see driver file for notes */

#include "driver.h"

UINT8 *speedspn_attram;

static tilemap *speedspn_tilemap;
static UINT8 speedspn_display_disable = 0;
static int speedspn_bank_vidram = 0;
static UINT8* speedspn_vidram;


static void get_speedspn_tile_info(int tile_index)
{
	int code = speedspn_vidram[tile_index*2+1] | (speedspn_vidram[tile_index*2] << 8);
	int attr = speedspn_attram[tile_index^0x400];

	SET_TILE_INFO(0,code,attr & 0x3f,(attr & 0x80) ? TILE_FLIPX : 0)
}

VIDEO_START(speedspn)
{
	speedspn_vidram = auto_malloc(0x1000 * 2);
	speedspn_tilemap = tilemap_create(get_speedspn_tile_info,tilemap_scan_cols,TILEMAP_OPAQUE, 8, 8,64,32);
	return 0;
}

WRITE8_HANDLER( speedspn_vidram_w )
{
	speedspn_vidram[offset + speedspn_bank_vidram] = data;

	if (speedspn_bank_vidram == 0)
		tilemap_mark_tile_dirty(speedspn_tilemap,offset/2);
}

WRITE8_HANDLER( speedspn_attram_w )
{
	speedspn_attram[offset] = data;

	tilemap_mark_tile_dirty(speedspn_tilemap,offset^0x400);
}

READ8_HANDLER( speedspn_vidram_r )
{
	return speedspn_vidram[offset + speedspn_bank_vidram];
}

WRITE8_HANDLER(speedspn_banked_vidram_change)
{
//  logerror("VidRam Bank: %04x\n", data);
	speedspn_bank_vidram = data & 1;
	speedspn_bank_vidram *= 0x1000;
}

WRITE8_HANDLER(speedspn_global_display_w)
{
//  logerror("Global display: %u\n", data);
	speedspn_display_disable = data & 1;
}


static void speedspn_drawsprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	const gfx_element *gfx = Machine->gfx[1];
	UINT8 *source = speedspn_vidram+ 0x1000;
	UINT8 *finish = source + 0x1000;

	
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
		int xpos = source[0];
		int tileno = source[1];
		int attr = source[2];
		int ypos = source[3];
		int color;

		if (attr&0x10) xpos +=0x100;

		xpos = 0x1f8-xpos;
		tileno += ((attr & 0xe0) >> 5) * 0x100;
		color = attr & 0x0f;

		
		dgp0.code = tileno;
		dgp0.color = color;
		dgp0.sx = xpos;
		dgp0.sy = ypos;
		drawgfx(&dgp0);

		source +=4;
	}
	} // end of patch paragraph

}


VIDEO_UPDATE(speedspn)
{
	if (speedspn_display_disable)
	{
		fillbitmap(bitmap,get_black_pen(),cliprect);
		return;
	}

#if 0
	{
		FILE* f;
		f = fopen("vidram.bin","wb");
		fwrite(speedspn_vidram, 1, 0x1000 * 2, f);
		fclose(f);
	}
#endif
	tilemap_set_scrollx(speedspn_tilemap,0, 0x100); // verify
	tilemap_draw(bitmap,cliprect,speedspn_tilemap,0,0);
	speedspn_drawsprites(bitmap,cliprect);
}
