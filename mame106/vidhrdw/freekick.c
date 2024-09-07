/* Free Kick Video Hardware */

#include "driver.h"

tilemap *freek_tilemap;
UINT8 *freek_videoram;


static void get_freek_tile_info(int tile_index)
{
	int tileno,palno;

	tileno = freek_videoram[tile_index]+((freek_videoram[tile_index+0x400]&0xe0)<<3);
	palno=freek_videoram[tile_index+0x400]&0x1f;
	SET_TILE_INFO(0,tileno,palno,0)
}



VIDEO_START(freekick)
{
	freek_tilemap = tilemap_create(get_freek_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE, 8, 8,32,32);
	return 0;
}



WRITE8_HANDLER( freek_videoram_w )
{
	freek_videoram[offset] = data;
	tilemap_mark_tile_dirty(freek_tilemap,offset&0x3ff);
}

static void gigas_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	int offs;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
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
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		int xpos = spriteram[offs + 3];
		int ypos = spriteram[offs + 2];
		int code = spriteram[offs + 0]|( (spriteram[offs + 1]&0x20) <<3 );

		int flipx = 0;
		int flipy = 0;
		int color = spriteram[offs + 1] & 0x1f;

		if (flip_screen_x)
		{
			xpos = 240 - xpos;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			ypos = 256 - ypos;
			flipy = !flipy;
		}

		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = xpos;
		dgp0.sy = 240-ypos;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}


static void pbillrd_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	int offs;

	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
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
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		int xpos = spriteram[offs + 3];
		int ypos = spriteram[offs + 2];
		int code = spriteram[offs + 0];

		int flipx = 0;//spriteram[offs + 0] & 0x80; //?? unused ?
		int flipy = 0;//spriteram[offs + 0] & 0x40;
		int color = spriteram[offs + 1] & 0x0f;

		if (flip_screen_x)
		{
			xpos = 240 - xpos;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			ypos = 256 - ypos;
			flipy = !flipy;
		}

		
		dgp1.code = code;
		dgp1.color = color;
		dgp1.flipx = flipx;
		dgp1.flipy = flipy;
		dgp1.sx = xpos;
		dgp1.sy = 240-ypos;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph

}



static void freekick_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	int offs;

	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
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
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		int xpos = spriteram[offs + 3];
		int ypos = spriteram[offs + 0];
		int code = spriteram[offs + 1]+ ((spriteram[offs + 2] & 0x20) << 3);

		int flipx = spriteram[offs + 2] & 0x80;	//?? unused ?
		int flipy = spriteram[offs + 2] & 0x40;
		int color = spriteram[offs + 2] & 0x1f;

		if (flip_screen_x)
		{
			xpos = 240 - xpos;
			flipx = !flipx;
		}
		if (flip_screen_y)
		{
			ypos = 256 - ypos;
			flipy = !flipy;
		}

		
		dgp2.code = code;
		dgp2.color = color;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = xpos;
		dgp2.sy = 248-ypos;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE(gigas)
{
	tilemap_draw(bitmap,cliprect,freek_tilemap,0,0);
	gigas_draw_sprites(bitmap,cliprect);
}

VIDEO_UPDATE(pbillrd)
{
	tilemap_draw(bitmap,cliprect,freek_tilemap,0,0);
	pbillrd_draw_sprites(bitmap,cliprect);
}

VIDEO_UPDATE(freekick)
{
	tilemap_draw(bitmap,cliprect,freek_tilemap,0,0);
	freekick_draw_sprites(bitmap,cliprect);
}
