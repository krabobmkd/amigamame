#include "driver.h"
#include "vidhrdw/taitoic.h"

#define TC0100SCN_GFX_NUM 1

/**********************************************************/

static int warriorb_core_vh_start (int x_offs,int multiscrn_xoffs)
{
	int chips;

	chips = number_of_TC0100SCN();

	if (chips <= 0)	/* we have an erroneous TC0100SCN configuration */
		return 1;

	if (TC0100SCN_vh_start(chips,TC0100SCN_GFX_NUM,x_offs,0,0,0,0,0,multiscrn_xoffs))
		return 1;

	if (has_TC0110PCR())
		if (TC0110PCR_vh_start())
			return 1;

	if (has_second_TC0110PCR())
		if (TC0110PCR_1_vh_start())
			return 1;

	/* Ensure palette from correct TC0110PCR used for each screen */
	TC0100SCN_set_chip_colbanks(0,0x100,0x0);

	return 0;
}

VIDEO_START( darius2d )
{
	return (warriorb_core_vh_start(4,0));
}

VIDEO_START( warriorb )
{
	return (warriorb_core_vh_start(4,1));
}


/************************************************************
            SPRITE DRAW ROUTINE
************************************************************/

static void warriorb_draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect,int y_offs)
{
	int offs, data, data2, tilenum, color, flipx, flipy;
	int x, y, priority, pri_mask;

#ifdef MAME_DEBUG
	int unknown=0;
#endif

	/* pdrawgfx() needs us to draw sprites front to back */
	
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
		priority_bitmap, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < spriteram_size/2;offs += 4)
	{
		data = spriteram16[offs+1];
		tilenum = data & 0x7fff;

		data = spriteram16[offs+0];
		y = (-(data &0x1ff) - 24) & 0x1ff;	/* (inverted y adjusted for vis area) */
		flipy = (data & 0x200) >> 9;

		data2 = spriteram16[offs+2];
		/* 8,4 also seen in msbyte */
		priority = (data2 & 0x0100) >> 8; // 1 = low

		if(priority)
			pri_mask = 0xfffe | (1<<31);
		else
			pri_mask = 0|(1<<31);

		color    = (data2 & 0x7f);

		data = spriteram16[offs+3];
		x = (data & 0x3ff);
		flipx = (data & 0x400) >> 10;


#ifdef MAME_DEBUG
		if (data2 & 0xf280)   unknown |= (data2 &0xf280);
#endif

		y += y_offs;

		/* sprite wrap: coords become negative at high values */
		if (x>0x3c0) x -= 0x400;
		if (y>0x180) y -= 0x200;

		
		dgp0.code = tilenum;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = x;
		dgp0.sy = y;
		dgp0.priority_mask = pri_mask;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph


#ifdef MAME_DEBUG
	if (unknown)
		ui_popup("unknown sprite bits: %04x",unknown);
#endif
}


/**************************************************************
                SCREEN REFRESH
**************************************************************/

VIDEO_UPDATE( warriorb )
{
	UINT8 layer[3], nodraw;

	TC0100SCN_tilemap_update();

	layer[0] = TC0100SCN_bottomlayer(0);
	layer[1] = layer[0]^1;
	layer[2] = 2;

	/* Clear priority bitmap */
	fillbitmap(priority_bitmap, 0, cliprect);

	/* chip 0 does tilemaps on the left, chip 1 does the ones on the right */
	// draw bottom layer
	nodraw  = TC0100SCN_tilemap_draw(bitmap,cliprect,0,layer[0],TILEMAP_IGNORE_TRANSPARENCY,0);	/* left */
	nodraw |= TC0100SCN_tilemap_draw(bitmap,cliprect,1,layer[0],TILEMAP_IGNORE_TRANSPARENCY,0);	/* right */

	/* Ensure screen blanked even when bottom layers not drawn due to disable bit */
	if(nodraw) fillbitmap(bitmap, get_black_pen(), cliprect);

	// draw middle layer
	TC0100SCN_tilemap_draw(bitmap,cliprect,0,layer[1],0,1);
	TC0100SCN_tilemap_draw(bitmap,cliprect,1,layer[1],0,1);

	/* Sprites can be under/over the layer below text layer */
	warriorb_draw_sprites(bitmap,cliprect,8); // draw sprites

	// draw top(text) layer
	TC0100SCN_tilemap_draw(bitmap,cliprect,0,layer[2],0,0);
	TC0100SCN_tilemap_draw(bitmap,cliprect,1,layer[2],0,0);
}
