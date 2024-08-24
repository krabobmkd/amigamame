/***************************************************************************

    Jaleco fcombat

***************************************************************************/

#include "driver.h"

int fcombat_sh;
int fcombat_sv;


#define BACKGROUND_X_START		32
#define BACKGROUND_X_START_FLIP	72

#define VISIBLE_X_MIN			(12*8)
#define VISIBLE_X_MAX			(52*8)
#define VISIBLE_Y_MIN			(2*8)
#define VISIBLE_Y_MAX			(30*8)


static tilemap *bgmap;

static void get_bg_tile_info(int tile_index)
{
	int tileno,palno;//32*16 x 32

	//palno=(tile_index-(tile_index/32*16)*32*16)/32;

	tileno = memory_region(REGION_USER1)[tile_index];
	palno=0x18;//memory_region(REGION_USER2)[tile_index]>>3;
	SET_TILE_INFO(2,tileno,palno,0)
}

UINT8 fcombat_cocktail_flip;

static UINT8 char_palette, sprite_palette;
static UINT8 char_bank;



/***************************************************************************

  Convert the color PROMs into a more useable format.

  The palette PROM is connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/

PALETTE_INIT( fcombat )
{
	int i;

	for (i = 0; i < Machine->drv->total_colors; i++)
	{
		int bit0, bit1, bit2, r, g, b;

		/* red component */
		bit0 = (*color_prom >> 0) & 0x01;
		bit1 = (*color_prom >> 1) & 0x01;
		bit2 = (*color_prom >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (*color_prom >> 3) & 0x01;
		bit1 = (*color_prom >> 4) & 0x01;
		bit2 = (*color_prom >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (*color_prom >> 6) & 0x01;
		bit2 = (*color_prom >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(i,r,g,b);
		color_prom++;
	}

	/* color_prom now points to the beginning of the char lookup table */

	/* fg chars */
	for (i = 0; i < 256; i++)
		colortable[i + 0x000] = 16 + (color_prom[(i & 0xc0) | ((i & 3) << 4) | ((i >> 2) & 15)] & 15);
	color_prom += 256;

	/* color_prom now points to the beginning of the sprite lookup table */

	/* sprites */
	for (i = 0; i < 256; i++)
		colortable[i + 0x100] = 16 + (color_prom[(i & 0xc0) | ((i & 3) << 4) | ((i >> 2) & 15)] & 15);
	color_prom += 256;

	/* bg chars (this is not the full story... there are four layers mixed */
	/* using another PROM */
	for (i = 0; i < 256; i++)
		//colortable[i + 0x200] = 16 + (color_prom[(i & 0xc0) | ((i & 3) << 4) | ((i >> 2) & 15)] & 15);
	colortable[i + 0x200] = *color_prom++ & 15;
}



/*************************************
 *
 *  Video system startup
 *
 *************************************/

VIDEO_START( fcombat )
{
	bgmap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE, 16, 16,32*8*2,32);
	return video_start_generic();
}



/*************************************
 *
 *  Video register I/O
 *
 *************************************/

WRITE8_HANDLER( fcombat_videoreg_w )
{
	/* bit 0 = flip screen and joystick input multiplexor */
	fcombat_cocktail_flip = data & 1;

	/* bits 1-2 char lookup table bank */
	char_palette = (data & 0x06) >> 1;

	/* bits 3 char bank */
	char_bank = (data & 0x08) >> 3;

	/* bits 4-5 unused */

	/* bits 6-7 sprite lookup table bank */
	sprite_palette = 0;//(data & 0xc0) >> 6;
	//ui_popup("%08x",data);
}



VIDEO_UPDATE( fcombat )
{
	int sx, sy, offs, i;

	/* draw background */
	tilemap_set_scrolly( bgmap,0, fcombat_sh);
	tilemap_set_scrollx( bgmap,0, fcombat_sv-24);

tilemap_mark_all_tiles_dirty(bgmap);
	tilemap_draw(bitmap,cliprect,bgmap,0,0);
	//draw_background(bitmap, cliprect);

	/* draw sprites */
	
	{ 
	struct drawgfxParams dgp4={
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
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0; i < spriteram_size; i += 4)
	{
		int flags = spriteram[i + 0];
		int y = spriteram[i + 1] ^ 255;
		int code = spriteram[i + 2]+((flags&0x20)<<3);
		int x = spriteram[i + 3] * 2 + 72;

		int xflip = flags & 0x80;
		int yflip = flags & 0x40;
		int doubled =0;// flags & 0x10;
		int wide = flags & 0x08;
		int code2 = code;

		int color = ((flags >> 1) & 0x03) | ((code >> 5) & 0x04) | (code & 0x08) | (sprite_palette * 16);
				const gfx_element *gfx =  Machine->gfx[1];

		if (fcombat_cocktail_flip)
		{
			x = 64*8 - gfx->width - x;
			y = 32*8 - gfx->height - y;
			if (wide) y -= gfx->height;
			xflip = !xflip;
			yflip = !yflip;
		}

		if (wide)
		
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
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			if (yflip)
				code |= 0x10, code2 &= ~0x10;
			else
				code &= ~0x10, code2 |= 0x10;

			
			dgp0.code = code2;
			dgp0.color = color;
			dgp0.flipx = xflip;
			dgp0.flipy = yflip;
			dgp0.sx = x;
			dgp0.sy = y + gfx->height;
			drawgfx(&dgp0);
		}
} // end of patch paragraph


		if(flags&0x10)
		
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
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{



			
			dgp1.code = code2+16;
			dgp1.color = color;
			dgp1.flipx = xflip;
			dgp1.flipy = yflip;
			dgp1.sx = x;
			dgp1.sy = y + gfx->height;
			drawgfx(&dgp1);
			
			dgp1.code = code2+16*2;
			dgp1.color = color;
			dgp1.flipx = xflip;
			dgp1.flipy = yflip;
			dgp1.sx = x;
			dgp1.sy = y + 2*gfx->height;
			drawgfx(&dgp1);
			
			dgp1.code = code2+16*3;
			dgp1.color = color;
			dgp1.flipx = xflip;
			dgp1.flipy = yflip;
			dgp1.sx = x;
			dgp1.sy = y + 3*gfx->height;
			drawgfx(&dgp1);

		}
} // end of patch paragraph




		
		dgp4.code = code;
		dgp4.color = color;
		dgp4.flipx = xflip;
		dgp4.flipy = yflip;
		dgp4.sx = x;
		dgp4.sy = y;
		drawgfx(&dgp4);

		if (doubled) i += 4;
	}
	} // end of patch paragraph


	/* draw the visible text layer */
	for (sy = VISIBLE_Y_MIN/8; sy < VISIBLE_Y_MAX/8; sy++)
		
		{ 
		struct drawgfxParams dgp5={
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
		for (sx = VISIBLE_X_MIN/8; sx < VISIBLE_X_MAX/8; sx++)
		{
			int x = fcombat_cocktail_flip ? (63*8 - 8*sx) : 8*sx;
			int y = fcombat_cocktail_flip ? (31*8 - 8*sy) : 8*sy;

			offs = sx + sy * 64;
			
			dgp5.code = videoram[offs] + 256 * char_bank;
			dgp5.color = ((videoram[offs] & 0xf0) >> 4) + char_palette * 16;
			dgp5.flipx = fcombat_cocktail_flip;
			dgp5.flipy = fcombat_cocktail_flip;
			dgp5.sx = x;
			dgp5.sy = y;
			drawgfx(&dgp5);
		}
		} // end of patch paragraph

}
