/***************************************************************************
  Functions to emulate similar video hardware on these Taito games:

  - rastan
  - operation wolf
  - rainbow islands
  - jumping (bootleg)

***************************************************************************/

#include "driver.h"
#include "vidhrdw/taitoic.h"


static UINT16 sprite_ctrl = 0;
static UINT16 sprites_flipscreen = 0;

/***************************************************************************/

VIDEO_START( rastan )
{
	/* (chips, gfxnum, x_offs, y_offs, y_invert, opaque, dblwidth) */
	if (PC080SN_vh_start(1,0,0,0,0,0,0))
		return 1;

	if (PC090OJ_vh_start(1,0,0,0))
		return 1;

	return 0;
}

VIDEO_START( opwolf )
{
	if (PC080SN_vh_start(1,1,0,0,0,0,0))
		return 1;

	if (PC090OJ_vh_start(0,0,0,0))
		return 1;

	return 0;
}

VIDEO_START( rainbow )
{
	/* (chips, gfxnum, x_offs, y_offs, y_invert, opaque, dblwidth) */
	if (PC080SN_vh_start(1,1,0,0,0,0,0))
		return 1;

	if (PC090OJ_vh_start(0,0,0,0))
		return 1;

	return 0;
}

VIDEO_START( jumping )
{
	if (PC080SN_vh_start(1,1,0,0,1,0,0))
		return 1;

	PC080SN_set_trans_pen(0,1,15);

	/* not 100% sure Jumping needs to save both... */
	state_save_register_global(sprite_ctrl);
	state_save_register_global(sprites_flipscreen);
	return 0;
}


WRITE16_HANDLER( rastan_spritectrl_w )
{
	/* bits 5-7 are the sprite palette bank */
	PC090OJ_sprite_ctrl = (data & 0xe0) >> 5;

	/* bit 4 unused */

	/* bits 0 and 1 are coin lockout */
	coin_lockout_w(1,~data & 0x01);
	coin_lockout_w(0,~data & 0x02);

	/* bits 2 and 3 are the coin counters */
	coin_counter_w(1,data & 0x04);
	coin_counter_w(0,data & 0x08);
}

WRITE16_HANDLER( rainbow_spritectrl_w )
{
	if (offset == 0)
	{
		/* bits 0 and 1 always set */
		/* bits 5-7 are the sprite palette bank */
		/* other bits unknown */

		PC090OJ_sprite_ctrl = (data & 0xe0) >> 5;
	}
}

WRITE16_HANDLER( jumping_spritectrl_w )
{
	if (offset == 0)
	{
		/* bits 0 and 1 are set after 15 seconds */
		/* bits 5-7 are the sprite palette bank */
		/* other bits unknown */

		sprite_ctrl = data;
	}
}



/***************************************************************************/

VIDEO_UPDATE( rastan )
{
	int layer[2];

	PC080SN_tilemap_update();

	layer[0] = 0;
	layer[1] = 1;

	//fillbitmap(priority_bitmap,0,cliprect);

 	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[0],TILEMAP_IGNORE_TRANSPARENCY,1);
	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[1],0,2);

	PC090OJ_draw_sprites(bitmap,cliprect,0);
}

/***************************************************************************/

VIDEO_UPDATE( opwolf )
{
	int layer[2];

	PC080SN_tilemap_update();

	layer[0] = 0;
	layer[1] = 1;

	//krb  fillbitmap(priority_bitmap,0,cliprect);

 	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[0],TILEMAP_IGNORE_TRANSPARENCY,1);
	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[1],0,2);

	PC090OJ_draw_sprites(bitmap,cliprect,1);

//  if (input_port_5_word_r(0,0xffff))

//  ui_popup("%d %d",input_port_5_word_r(0,0xffff),input_port_6_word_r(0,0xffff));

	/* See if we should draw artificial gun targets */
	if (1) //input_port_4_word_r(0,0) &0x1) /* Fake DSW */
	{
		/* Draw an aiming crosshair */
		draw_crosshair(bitmap,(input_port_4_word_r(0,0xffff)*320)/256,input_port_5_word_r(0,0xffff),cliprect,0);
	}
}

/***************************************************************************/

VIDEO_UPDATE( rainbow )
{
	int layer[2];

	PC080SN_tilemap_update();

	layer[0] = 0;
	layer[1] = 1;

	fillbitmap(priority_bitmap,0,cliprect);

	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[0],TILEMAP_IGNORE_TRANSPARENCY,1);
	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[1],0,2);

	PC090OJ_draw_sprites(bitmap,cliprect,1);
}

/***************************************************************************

Jumping uses different sprite controller
than rainbow island. - values are remapped
at address 0x2EA in the code. Apart from
physical layout, the main change is that
the Y settings are active low.

*/

VIDEO_UPDATE( jumping )
{
	int offs,layer[2];
	int sprite_colbank = (sprite_ctrl & 0xe0) >> 1;

	PC080SN_tilemap_update();

	/* Override values, or foreground layer is in wrong position */
	PC080SN_set_scroll(0,1,16,0);

	layer[0] = 0;
	layer[1] = 1;

	fillbitmap(priority_bitmap,0,cliprect);

 	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[0],TILEMAP_IGNORE_TRANSPARENCY,0);

	/* Draw the sprites. 128 sprites in total */
	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size/2-8; offs >= 0; offs -= 8)
	{
		int tile = spriteram16[offs];
		if (tile < Machine->gfx[1]->total_elements)
		{
			int sx,sy,color,data1;

			sy = ((spriteram16[offs+1] - 0xfff1) ^ 0xffff) & 0x1ff;
  			if (sy > 400) sy = sy - 512;
			sx = (spriteram16[offs+2] - 0x38) & 0x1ff;
			if (sx > 400) sx = sx - 512;

			data1 = spriteram16[offs+3];
			color = (spriteram16[offs+4] & 0x0f) | sprite_colbank;

			
			dgp0.code = tile;
			dgp0.color = color;
			dgp0.flipx = data1 & 0x40;
			dgp0.flipy = data1 & 0x80;
			dgp0.sx = sx;
			dgp0.sy = sy+1;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph


 	PC080SN_tilemap_draw(bitmap,cliprect,0,layer[1],0,0);

#if 0
	{
		char buf[80];
		sprintf(buf,"sprite_ctrl: %04x",sprite_ctrl);
		ui_popup(buf);
	}
#endif
}

