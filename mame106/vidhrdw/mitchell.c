/***************************************************************************

 Pang Video Hardware

***************************************************************************/

#include "driver.h"
#include "sound/okim6295.h"


/* Globals */
size_t pang_videoram_size;
unsigned char *pang_videoram;
unsigned char *pang_colorram;

/* Private */
static unsigned char *pang_objram;           /* Sprite RAM */

static tilemap *bg_tilemap;
static int flipscreen;



/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_tile_info(int tile_index)
{
	unsigned char attr = pang_colorram[tile_index];
	int code = pang_videoram[2*tile_index] + (pang_videoram[2*tile_index+1] << 8);
	SET_TILE_INFO(
			0,
			code,
			attr & 0x7f,
			(attr & 0x80) ? TILE_FLIPX : 0)
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( pang )
{
	pang_objram=NULL;
	paletteram=NULL;


	bg_tilemap = tilemap_create(get_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);

	if (!bg_tilemap)
		return 1;

	tilemap_set_transparent_pen(bg_tilemap,15);

	/*
        OBJ RAM
    */
	pang_objram=auto_malloc(pang_videoram_size);
	memset(pang_objram, 0, pang_videoram_size);

	/*
        Palette RAM
    */
	paletteram = auto_malloc(2*Machine->drv->total_colors);
	memset(paletteram, 0, 2*Machine->drv->total_colors);

	return 0;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

/***************************************************************************
  OBJ / CHAR RAM HANDLERS (BANK 0 = CHAR, BANK 1=OBJ)
***************************************************************************/

static int video_bank;

WRITE8_HANDLER( pang_video_bank_w )
{
	/* Bank handler (sets base pointers for video write) (doesn't apply to mgakuen) */
	video_bank = data;
}

WRITE8_HANDLER( mstworld_video_bank_w )
{
	/* Monsters World seems to freak out if more bits are used.. */
	video_bank = data&1;
}


WRITE8_HANDLER( mgakuen_videoram_w )
{
	if (pang_videoram[offset] != data)
	{
		pang_videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset/2);
	}
}

READ8_HANDLER( mgakuen_videoram_r )
{
	return pang_videoram[offset];
}

WRITE8_HANDLER( mgakuen_objram_w )
{
	pang_objram[offset]=data;
}

READ8_HANDLER( mgakuen_objram_r )
{
	return pang_objram[offset];
}

WRITE8_HANDLER( pang_videoram_w )
{
	if (video_bank) mgakuen_objram_w(offset,data);
	else mgakuen_videoram_w(offset,data);
}

READ8_HANDLER( pang_videoram_r )
{
	if (video_bank) return mgakuen_objram_r(offset);
	else return mgakuen_videoram_r(offset);
}

/***************************************************************************
  COLOUR RAM
****************************************************************************/

WRITE8_HANDLER( pang_colorram_w )
{
	if (pang_colorram[offset] != data)
	{
		pang_colorram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset);
	}
}

READ8_HANDLER( pang_colorram_r )
{
	return pang_colorram[offset];
}

/***************************************************************************
  PALETTE HANDLERS (COLOURS: BANK 0 = 0x00-0x3f BANK 1=0x40-0xff)
****************************************************************************/

static int paletteram_bank;

WRITE8_HANDLER( pang_gfxctrl_w )
{
loginfo(2,"PC %04x: pang_gfxctrl_w %02x\n",activecpu_get_pc(),data);
{
	char baf[40];
	sprintf(baf,"%02x",data);
//  ui_popup(baf);
}

	/* bit 0 is unknown (used, maybe back color enable?) */

	/* bit 1 is coin counter */
	coin_counter_w(0,data & 2);

	/* bit 2 is flip screen */
	if (flipscreen != (data & 0x04))
	{
		flipscreen = data & 0x04;
		tilemap_set_flip(ALL_TILEMAPS,flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	}

	/* bit 3 is unknown (used, e.g. marukin pulses it on the title screen) */

	/* bit 4 selects OKI M6295 bank */
	OKIM6295_set_bank_base(0, (data & 0x10) ? 0x40000 : 0x00000);

	/* bit 5 is palette RAM bank selector (doesn't apply to mgakuen) */
	paletteram_bank = data & 0x20;

	/* bits 6 and 7 are unknown, used in several places. At first I thought */
	/* they were bg and sprites enable, but this screws up spang (screen flickers */
	/* every time you pop a bubble). However, not using them as enable bits screws */
	/* up marukin - you can see partially built up screens during attract mode. */
}

WRITE8_HANDLER( mstworld_gfxctrl_w )
{
loginfo(2,"PC %04x: pang_gfxctrl_w %02x\n",activecpu_get_pc(),data);
{
	char baf[40];
	sprintf(baf,"%02x",data);
//  ui_popup(baf);
}

	/* bit 0 is unknown (used, maybe back color enable?) */

	/* bit 1 is coin counter */
	coin_counter_w(0,data & 2);

	/* bit 2 is flip screen */
	if (flipscreen != (data & 0x04))
	{
		flipscreen = data & 0x04;
		tilemap_set_flip(ALL_TILEMAPS,flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	}

	/* bit 3 is unknown (used, e.g. marukin pulses it on the title screen) */

	/* bit 4 selects OKI M6295 bank */
//  OKIM6295_set_bank_base(0, (data & 0x10) ? 0x40000 : 0x00000);
	/* not here.. it has its own z80 + sound banking */

	/* bit 5 is palette RAM bank selector (doesn't apply to mgakuen) */
	paletteram_bank = data & 0x20;

	/* bits 6 and 7 are unknown, used in several places. At first I thought */
	/* they were bg and sprites enable, but this screws up spang (screen flickers */
	/* every time you pop a bubble). However, not using them as enable bits screws */
	/* up marukin - you can see partially built up screens during attract mode. */
}

WRITE8_HANDLER( pang_paletteram_w )
{
	if (paletteram_bank) paletteram_xxxxRRRRGGGGBBBB_le_w(offset + 0x800,data);
	else paletteram_xxxxRRRRGGGGBBBB_le_w(offset,data);
}

READ8_HANDLER( pang_paletteram_r )
{
	if (paletteram_bank) return paletteram_r(offset + 0x800);
	return paletteram_r(offset);
}

WRITE8_HANDLER( mgakuen_paletteram_w )
{
	paletteram_xxxxRRRRGGGGBBBB_le_w(offset,data);
}

READ8_HANDLER( mgakuen_paletteram_r )
{
	return paletteram_r(offset);
}



/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int offs,sx,sy;

	/* the last entry is not a sprite, we skip it otherwise spang shows a bubble */
	/* moving diagonally across the screen */
	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0x1000-0x40;offs >= 0;offs -= 0x20)
	{
		int code = pang_objram[offs];
		int attr = pang_objram[offs+1];
		int color = attr & 0x0f;
		sx = pang_objram[offs+3] + ((attr & 0x10) << 4);
		sy = ((pang_objram[offs+2] + 8) & 0xff) - 8;
		code += (attr & 0xe0) << 3;
		if (flipscreen)
		{
			sx = 496 - sx;
			sy = 240 - sy;
		}
		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipscreen;
		dgp0.flipy = flipscreen;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( pang )
{
	fillbitmap(bitmap,Machine->pens[0],cliprect);
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect);
}
