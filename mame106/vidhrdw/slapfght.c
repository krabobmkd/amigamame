/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of early Toaplan hardware.

***************************************************************************/

#include "driver.h"
#include "drawgfxn.h"
unsigned char *slapfight_videoram;
unsigned char *slapfight_colorram;
size_t slapfight_videoram_size;
unsigned char *slapfight_scrollx_lo,*slapfight_scrollx_hi,*slapfight_scrolly;
static int flipscreen;

static tilemap *pf1_tilemap,*fix_tilemap;




/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_pf_tile_info(int tile_index)	/* For Performan only */
{
	int tile,color;

	tile=videoram[tile_index] + ((colorram[tile_index] & 0x03) << 8);
	color=(colorram[tile_index] >> 3) & 0x0f;
	SET_TILE_INFO(
			0,
			tile,
			color,
			0)
}

static void get_pf1_tile_info(int tile_index)
{
	int tile,color;

	tile=videoram[tile_index] + ((colorram[tile_index] & 0x0f) << 8);
	color=(colorram[tile_index] & 0xf0) >> 4;

	SET_TILE_INFO(
			1,
			tile,
			color,
			0)
}

static void get_fix_tile_info(int tile_index)
{
	int tile,color;

	tile=slapfight_videoram[tile_index] + ((slapfight_colorram[tile_index] & 0x03) << 8);
	color=(slapfight_colorram[tile_index] & 0xfc) >> 2;

	SET_TILE_INFO(
			0,
			tile,
			color,
			0)
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( perfrman )
{
	pf1_tilemap = tilemap_create(get_pf_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);

	if (!pf1_tilemap)
		return 1;

	tilemap_set_transparent_pen(pf1_tilemap,0);

	return 0;
}

VIDEO_START( slapfight )
{
	pf1_tilemap = tilemap_create(get_pf1_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,64,32);
	fix_tilemap = tilemap_create(get_fix_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);

	if (!pf1_tilemap || !fix_tilemap)
		return 1;

	tilemap_set_transparent_pen(fix_tilemap,0);

	return 0;
}


/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( slapfight_videoram_w )
{
	videoram[offset]=data;
	tilemap_mark_tile_dirty(pf1_tilemap,offset);
}

WRITE8_HANDLER( slapfight_colorram_w )
{
	colorram[offset]=data;
	tilemap_mark_tile_dirty(pf1_tilemap,offset);
}

WRITE8_HANDLER( slapfight_fixram_w )
{
	slapfight_videoram[offset]=data;
	tilemap_mark_tile_dirty(fix_tilemap,offset);
}

WRITE8_HANDLER( slapfight_fixcol_w )
{
	slapfight_colorram[offset]=data;
	tilemap_mark_tile_dirty(fix_tilemap,offset);
}

WRITE8_HANDLER( slapfight_flipscreen_w )
{
	loginfo(2,"Writing %02x to flipscreen\n",offset);
	if (offset==0) flipscreen=1; /* Port 0x2 is flipscreen */
	else flipscreen=0; /* Port 0x3 is normal */
}

#ifdef MAME_DEBUG
void slapfght_log_vram(void)
{
	if ( code_pressed_memory(KEYCODE_B) )
	{
		int i;
		for (i=0; i<0x800; i++)
		{
			loginfo(2,"Offset:%03x   TileRAM:%02x   AttribRAM:%02x   SpriteRAM:%02x\n",i, videoram[i],colorram[i],spriteram[i]);
		}
	}
}
#endif

/***************************************************************************

  Render the Sprites

***************************************************************************/
static void perfrman_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect, int priority_to_display )
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
		int sx, sy;

		if ((buffered_spriteram[offs+2] & 0x80) == priority_to_display)
		{
			if (flipscreen)
			{
				sx = 265 - buffered_spriteram[offs+1];
				sy = 239 - buffered_spriteram[offs+3];
				sy &= 0xff;
			}
			else
			{
				sx = buffered_spriteram[offs+1] + 3;
				sy = buffered_spriteram[offs+3] - 1;
			}
			
			dgp0.code = buffered_spriteram[offs];
			dgp0.color = ((buffered_spriteram[offs+2] >> 1) & 3)+ ((buffered_spriteram[offs+2] << 2) & 4);
			dgp0.flipx = flipscreen;
			dgp0.flipy = flipscreen;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}

/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

VIDEO_UPDATE( perfrman )
{
	tilemap_set_flip( pf1_tilemap, flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	tilemap_set_scrolly( pf1_tilemap ,0 , 0 );
	if (flipscreen) {
		tilemap_set_scrollx( pf1_tilemap ,0 , 264 );
	}
	else {
		tilemap_set_scrollx( pf1_tilemap ,0 , -16 );
	}

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	perfrman_draw_sprites(bitmap,cliprect,0);
	tilemap_draw(bitmap,cliprect,pf1_tilemap,0,0);
	perfrman_draw_sprites(bitmap,cliprect,0x80);

#ifdef MAME_DEBUG
	slapfght_log_vram();
#endif
}


VIDEO_UPDATE( slapfight )
{
	int offs;

	tilemap_set_flip(ALL_TILEMAPS,flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	if (flipscreen) {
		tilemap_set_scrollx( fix_tilemap,0,296);
		tilemap_set_scrollx( pf1_tilemap,0,(*slapfight_scrollx_lo + 256 * *slapfight_scrollx_hi)+296 );
		tilemap_set_scrolly( pf1_tilemap,0, (*slapfight_scrolly)+15 );
		tilemap_set_scrolly( fix_tilemap,0, -1 ); /* Glitch in Tiger Heli otherwise */
	}
	else {
		tilemap_set_scrollx( fix_tilemap,0,0);
		tilemap_set_scrollx( pf1_tilemap,0,(*slapfight_scrollx_lo + 256 * *slapfight_scrollx_hi) );
		tilemap_set_scrolly( pf1_tilemap,0, (*slapfight_scrolly)-1 );
		tilemap_set_scrolly( fix_tilemap,0, -1 ); /* Glitch in Tiger Heli otherwise */
	}

	tilemap_draw(bitmap,cliprect,pf1_tilemap,0,0);

	/* Draw the sprites */
	
	{
    rectangle lcliprect = *cliprect;
	lcliprect.max_x++;
	lcliprect.max_y++;

	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[2], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&lcliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size;offs += 4)
	{
        dgp1.code = buffered_spriteram[offs] + ((buffered_spriteram[offs+2] & 0xc0) << 2);
        dgp1.color = (buffered_spriteram[offs+2] & 0x1e) >> 1;
		if (flipscreen)
		{
			dgp1.sx = 288-(buffered_spriteram[offs+1] + ((buffered_spriteram[offs+2] & 0x01) << 8)) +18;
			dgp1.sy = 240-buffered_spriteram[offs+3];
        }
		else
		{
			dgp1.sx = (buffered_spriteram[offs+1] + ((buffered_spriteram[offs+2] & 0x01) << 8)) - 13;
			dgp1.sy = buffered_spriteram[offs+3];			
        }
        //drawgfx(&dgp1);
        drawgfx_clut16_Src8(&dgp1);
	}
	} // end of patch paragraph


	tilemap_draw(bitmap,cliprect,fix_tilemap,0,0);

#ifdef MAME_DEBUG
	slapfght_log_vram();
#endif
}
