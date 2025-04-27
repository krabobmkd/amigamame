#include "driver.h"
#include "vidhrdw/taitoic.h"
#include "drawgfxn.h"

UINT16 *topspeed_spritemap;
UINT16 *topspeed_raster_ctrl;

/****************************************************************************/

VIDEO_START( topspeed )
{
	/* (chips, gfxnum, x_offs, y_offs, y_invert, opaque, dblwidth) */
	if (PC080SN_vh_start(2,1,0,8,0,0,0))
		return 1;

	return 0;
}


/********************************************************************************
                                     SPRITES

    Layout 8 bytes per sprite
    -------------------------

    +0x00   xxxxxxx. ........   Zoom Y
            .......x xxxxxxxx   Y

    +0x02   x....... ........   Flip Y
            ........ .xxxxxxx   Zoom X

    +0x04   x....... ........   Priority
            .x...... ........   Flip X
            ..x..... ........   Unknown
            .......x xxxxxxxx   X

    +0x06   xxxxxxxx ........   Color
            ........ xxxxxxxx   Tile number

********************************************************************************/

void topspeed_draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int offs,map_offset,x,y,curx,cury;
	UINT16 sprite_chunk;
	UINT16 *spritemap = topspeed_spritemap;
	UINT16 data,tilenum,code,color;
	UINT8 flipx,flipy,priority,bad_chunks;
	UINT8 zx,zy,zoomx,zoomy;
	static const int primasks[2] = {
	0xff00 | (1<<31),
	0xfffc | (1<<31)
	};	/* Sprites are over bottom layer or under top layer */

	/* Most of spriteram is not used by the 68000: rest is scratch space for the h/w perhaps ? */

	
	{ 
	struct drawgfxParams dgpz0={
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
		0x00010000, 	// scalex
		0x00010000, 	// scaley
		priority_bitmap, 	// pri_buffer
		0, //primasks[priority] 	// priority_mask
	  };
	for (offs = 0;offs <(0x2c0/2);offs += 4)
	{
		data = spriteram16[offs+2];

		tilenum = spriteram16[offs+3] & 0xff;
		color = (spriteram16[offs+3] & 0xff00) >> 8;
		flipx = (data & 0x4000) >> 14;
		flipy = (spriteram16[offs+1] & 0x8000) >> 15;
		x = data & 0x1ff;
		y = spriteram16[offs] & 0x1ff;
		zoomx = (spriteram16[offs+1]& 0x7f);
		zoomy = (spriteram16[offs] & 0xfe00) >> 9;
		priority = (data & 0x8000) >> 15;
//      unknown = (data & 0x2000) >> 13;

		if (y == 0x180) continue;	/* dead sprite */

		map_offset = tilenum << 7;

		zoomx += 1;
		zoomy += 1;

		y += 3 + (128-zoomy);

		/* treat coords as signed */
		if (x > 0x140) x -= 0x200;
		if (y > 0x140) y -= 0x200;

		bad_chunks = 0;

		dgpz0.color = color;
		dgpz0.priority_mask = primasks[priority];
        dgpz0.flipx = flipx;
        dgpz0.flipy = flipy;

		for (sprite_chunk = 0;sprite_chunk < 128;sprite_chunk++)
		{
            UINT8 j,k,px,py;
			k = sprite_chunk & 0x07; // % 8;   /* 8 sprite chunks per row */
			j = sprite_chunk>>3;  // /8;   /* 16 rows */

			/* pick tiles back to front for x and y flips */
			px = (flipx) ? (7-k) : (k);
			py = (flipy) ? (15-j) : (j);

			code = spritemap[map_offset + (py<<3) + px];

			if (code & 0x8000)
			{
				bad_chunks += 1;
				continue;
			}

			curx = x + ((k*zoomx)/8);
			cury = y + ((j*zoomy)/16);

			zx = x + (((k+1)*zoomx)>>3) - curx;
			zy = y + (((j+1)*zoomy)>>4) - cury;

			dgpz0.code = code;
			dgpz0.sx = curx;
			dgpz0.sy = cury;
			dgpz0.scalex = zx<<12;
			dgpz0.scaley = zy<<13;

			//drawgfxzoom(&dgpz0);
			drawgfxzoom_clut16_Src8_tr0_prio(&dgpz0);
		}

// 		if (bad_chunks)
// loginfo(2,"Sprite number %04x had %02x invalid chunks\n",tilenum,bad_chunks);
	}
	} // end of patch paragraph

}


/***************************************************************************/

VIDEO_UPDATE( topspeed )
{
	UINT8 layer[4];

#ifdef MAME_DEBUG
	static UINT8 dislayer[5];
#endif

#ifdef MAME_DEBUG
	if (code_pressed_memory (KEYCODE_V))
	{
		dislayer[0] ^= 1;
		ui_popup("bg: %01x",dislayer[0]);
	}

	if (code_pressed_memory (KEYCODE_B))
	{
		dislayer[1] ^= 1;
		ui_popup("fg: %01x",dislayer[1]);
	}

	if (code_pressed_memory (KEYCODE_N))
	{
		dislayer[2] ^= 1;
		ui_popup("bg2: %01x",dislayer[2]);
	}

	if (code_pressed_memory (KEYCODE_M))
	{
		dislayer[3] ^= 1;
		ui_popup("fg2: %01x",dislayer[3]);
	}

	if (code_pressed_memory (KEYCODE_C))
	{
		dislayer[4] ^= 1;
		ui_popup("sprites: %01x",dislayer[4]);
	}
#endif

	PC080SN_tilemap_update();

	/* Tilemap layer priority seems hardwired (the order is odd, too) */
	layer[0] = 1;
	layer[1] = 0;
	layer[2] = 1;
	layer[3] = 0;

	fillbitmap(priority_bitmap,0,cliprect);
	fillbitmap(bitmap, Machine->pens[0], cliprect);

    //bg
	//PC080SN_tilemap_draw(bitmap,cliprect,1,layer[0],TILEMAP_IGNORE_TRANSPARENCY,1);
	 PC080SN_tilemap_draw_special(bitmap,cliprect,1,layer[1],0,2,topspeed_raster_ctrl);
 // 	PC080SN_tilemap_draw_special(bitmap,cliprect,0,layer[2],0,4,topspeed_raster_ctrl + 0x100);
	// PC080SN_tilemap_draw(bitmap,cliprect,0,layer[3],0,8);

//	topspeed_draw_sprites(bitmap,cliprect);
}




