#include "driver.h"



unsigned char *lastday_txvideoram;
unsigned char *lastday_bgscroll,*lastday_fgscroll,*bluehawk_fg2scroll;
UINT16 *rshark_scroll1,*rshark_scroll2,*rshark_scroll3,*rshark_scroll4;
UINT16 *popbingo_scroll, *popbingo_scroll2;
static int tx_pri;
static int flytiger_pri;
static int sprites_disabled;


WRITE8_HANDLER( lastday_ctrl_w )
{
	/* bits 0 and 1 are coin counters */
	coin_counter_w(0,data & 0x01);
	coin_counter_w(1,data & 0x02);

	/* bit 3 is used but unknown */

	/* bit 4 disables sprites */
	sprites_disabled = data & 0x10;

	/* bit 6 is flip screen */
	flip_screen_set(data & 0x40);
}

WRITE8_HANDLER( pollux_ctrl_w )
{
	/* bit 0 is flip screen */
	flip_screen_set(data & 0x01);

	/* bits 6 and 7 are coin counters */
	coin_counter_w(0,data & 0x80);
	coin_counter_w(1,data & 0x40);

	/* bit 1 is used but unknown */

	/* bit 2 is continuously toggled (unknown) */

	/* bit 4 is used but unknown */
}

WRITE8_HANDLER( primella_ctrl_w )
{
 	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU1);

	/* bits 0-2 select ROM bank */
	bankaddress = 0x10000 + (data & 0x07) * 0x4000;
	memory_set_bankptr(1,&RAM[bankaddress]);

	/* bit 3 disables tx layer */
	tx_pri = data & 0x08;

	/* bit 4 flips screen */
	flip_screen_set(data & 0x10);

	/* bit 5 used but unknown */

//  logerror("%04x: bankswitch = %02x\n",activecpu_get_pc(),data&0xe0);
}

WRITE8_HANDLER( flytiger_ctrl_w )
{
	/* bit 0 is flip screen */
	flip_screen_set(data & 0x01);

	/* bits 1, 2, 3 used but unknown */

	/* bit 4 changes tilemaps priority */
	flytiger_pri = data & 0x10;
}

WRITE16_HANDLER( rshark_ctrl_w )
{
	if (ACCESSING_LSB)
	{
		/* bit 0 flips screen */
		flip_screen_set(data & 0x01);

		/* bit 4 used but unknown */

		/* bit 5 used but unknown */
	}
}

static void draw_layer(mame_bitmap *bitmap,int gfx,const unsigned char *scroll,
		const unsigned char *tilemap,int transparency)
{
	int offs;
	int scrollx,scrolly;

	scrollx = scroll[0] + (scroll[1] << 8);
	scrolly = scroll[3] + (scroll[4] << 8);

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		transparency, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x100;offs += 2)
	{
		int sx,sy,code,color,attr,flipx,flipy;
		int toffs = offs+((scrollx&~0x1f)>>1);

		attr = tilemap[toffs];
		code = tilemap[toffs+1] | ((attr & 0x01) << 8) | ((attr & 0x80) << 2),
		color = (attr & 0x78) >> 3;
		sx = 32 * ((offs/2) / 8) - (scrollx & 0x1f);
		sy = (32 * ((offs/2) % 8) - scrolly) & 0xff;
		flipx = attr & 0x02;
		flipy = attr & 0x04;
		if (flip_screen)
		{
			sx = 512-32 - sx;
			sy = 256-32 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
		/* wraparound */
		if (scrolly & 0x1f)
		
{ 
struct drawgfxParams dgp1={
	bitmap, 	// dest
	Machine->gfx[gfx], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	transparency, 	// transparency
	15, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp1.code = code;
			dgp1.color = color;
			dgp1.flipx = flipx;
			dgp1.flipy = flipy;
			dgp1.sx = sx;
			dgp1.sy = ((sy + 0x20) & 0xff) - 0x20;
			drawgfx(&dgp1);
		}
} // end of patch paragraph

	}
	} // end of patch paragraph

}

static void bluehawk_draw_layer(mame_bitmap *bitmap,int gfx,const unsigned char *scroll,
		const unsigned char *tilemap,int transparency)
{
	int offs;
	int scrollx,scrolly;

	scrollx = scroll[0] + (scroll[1] << 8);
	scrolly = scroll[3] + (scroll[4] << 8);

	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		transparency, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x100;offs += 2)
	{
		int sx,sy,code,color,attr,flipx,flipy;
		int toffs = offs+((scrollx&~0x1f)>>1);

		attr = tilemap[toffs];
		code = tilemap[toffs+1] | ((attr & 0x03) << 8);
		color = (attr & 0x3c) >> 2;
		sx = 32 * ((offs/2) / 8) - (scrollx & 0x1f);
		sy = (32 * ((offs/2) % 8) - scrolly) & 0xff;
		flipx = attr & 0x40;
		flipy = attr & 0x80;
		if (flip_screen)
		{
			sx = 512-32 - sx;
			sy = 256-32 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp2.code = code;
		dgp2.color = color;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = sx;
		dgp2.sy = sy;
		drawgfx(&dgp2);
		/* wraparound */
		if (scrolly & 0x1f)
		
{ 
struct drawgfxParams dgp3={
	bitmap, 	// dest
	Machine->gfx[gfx], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	transparency, 	// transparency
	15, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp3.code = code;
			dgp3.color = color;
			dgp3.flipx = flipx;
			dgp3.flipy = flipy;
			dgp3.sx = sx;
			dgp3.sy = ((sy + 0x20) & 0xff) - 0x20;
			drawgfx(&dgp3);
		}
} // end of patch paragraph

	}
	} // end of patch paragraph

}

static void bluehawk_draw_layer2(mame_bitmap *bitmap,int gfx,const unsigned char *scroll,
		const unsigned char *tilemap,int transparency)
{
	int offs;
	int scrollx,scrolly;

	scrollx = scroll[0] + (scroll[1] << 8);
	scrolly = scroll[3] + (scroll[4] << 8);

	
	{ 
	struct drawgfxParams dgp4={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		transparency, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x100;offs += 2)
	{
		int sx,sy,code,color,attr,flipx,flipy;
		int toffs = offs+((scrollx&~0x1f)>>1);

		attr = tilemap[toffs];
		code = tilemap[toffs+1] | ((attr & 0x01) << 8);
		color = (attr & 0x78) >> 3;
		sx = 32 * ((offs/2) / 8) - (scrollx & 0x1f);
		sy = (32 * ((offs/2) % 8) - scrolly) & 0xff;
		flipx = 0;
		flipy = attr & 0x04;
		if (flip_screen)
		{
			sx = 512-32 - sx;
			sy = 256-32 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp4.code = code;
		dgp4.color = color;
		dgp4.flipx = flipx;
		dgp4.flipy = flipy;
		dgp4.sx = sx;
		dgp4.sy = sy;
		drawgfx(&dgp4);
		/* wraparound */
		if (scrolly & 0x1f)
		
{ 
struct drawgfxParams dgp5={
	bitmap, 	// dest
	Machine->gfx[gfx], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	transparency, 	// transparency
	15, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp5.code = code;
			dgp5.color = color;
			dgp5.flipx = flipx;
			dgp5.flipy = flipy;
			dgp5.sx = sx;
			dgp5.sy = ((sy + 0x20) & 0xff) - 0x20;
			drawgfx(&dgp5);
		}
} // end of patch paragraph

	}
	} // end of patch paragraph

}

static void rshark_draw_layer(mame_bitmap *bitmap,int gfx,UINT16 *scroll,
		const unsigned char *tilemap,const unsigned char *tilemap2,int transparency)
{
	int offs;
	int scrollx,scrolly;

	scrollx = (scroll[0]&0xff) + ((scroll[1]&0xff) << 8);
	scrolly = (scroll[3]&0xff) + ((scroll[4]&0xff) << 8);

	
	{ 
	struct drawgfxParams dgp6={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		transparency, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x800;offs += 2)
	{
		int sx,sy,code,color,attr,attr2,flipx,flipy;
		int toffs = offs+((scrollx&~0x0f)<<2);

		attr = tilemap[toffs];
		attr2 = tilemap2[toffs/2];
		code = tilemap[toffs+1] | ((attr & 0x1f) << 8);
		color = attr2 & 0x0f;
		sx = 16 * ((offs/2) / 32) - (scrollx & 0x0f);
		sy = (16 * ((offs/2) % 32) - scrolly) & 0x1ff;
		if (sy > 256) sy -= 512;
		flipx = attr & 0x40;
		flipy = attr & 0x80;
		if (flip_screen)
		{
			sx = 512-16 - sx;
			sy = 256-16 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp6.code = code;
		dgp6.color = color;
		dgp6.flipx = flipx;
		dgp6.flipy = flipy;
		dgp6.sx = sx;
		dgp6.sy = sy;
		drawgfx(&dgp6);
	}
	} // end of patch paragraph

}

static void popbingo_draw_layer(mame_bitmap *bitmap,int gfx,UINT16 *scroll,
		const unsigned char *tilemap,int transparency)
{

	int offs;
	int scrollx,scrolly;

	scrollx = scroll[0] + (scroll[1] << 8);
	scrolly = scroll[3] + (scroll[4] << 8);

	
	{ 
	struct drawgfxParams dgp7={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		transparency, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x100;offs += 2)
	{
		int sx,sy,code,color,attr,flipx,flipy;
		int toffs = offs+((scrollx&~0x1f)>>1);

		attr = tilemap[toffs];
		code = tilemap[toffs+1] | ((attr & 0x07) << 8);
		color = 0;
		sx = 32 * ((offs/2) / 8) - (scrollx & 0x1f);
		sy = (32 * ((offs/2) % 8) - scrolly) & 0xff;
		flipx = attr & 0x40;
		flipy = attr & 0x80;
		if (flip_screen)
		{
			sx = 512-32 - sx;
			sy = 256-32 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp7.code = code;
		dgp7.color = color;
		dgp7.flipx = flipx;
		dgp7.flipy = flipy;
		dgp7.sx = sx;
		dgp7.sy = sy;
		drawgfx(&dgp7);
		/* wraparound */
		if (scrolly & 0x1f)
		
{ 
struct drawgfxParams dgp8={
	bitmap, 	// dest
	Machine->gfx[gfx], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	transparency, 	// transparency
	15, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp8.code = code;
			dgp8.color = color;
			dgp8.flipx = flipx;
			dgp8.flipy = flipy;
			dgp8.sx = sx;
			dgp8.sy = ((sy + 0x20) & 0xff) - 0x20;
			drawgfx(&dgp8);
		}
} // end of patch paragraph

	}
	} // end of patch paragraph


}

// it's the same as draw_layer function for now...
static void flytiger_draw_layer2(mame_bitmap *bitmap,int gfx,const unsigned char *scroll,
		const unsigned char *tilemap,int transparency)
{
	int offs;
	int scrollx,scrolly;

	scrollx = scroll[0] + (scroll[1] << 8);
	scrolly = scroll[3] + (scroll[4] << 8);

	
	{ 
	struct drawgfxParams dgp9={
		bitmap, 	// dest
		Machine->gfx[gfx], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		transparency, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x100;offs += 2)
	{
		int sx,sy,code,color,attr,flipx,flipy;
		int toffs = offs+((scrollx&~0x1f)>>1);

		attr = tilemap[toffs];
		code = tilemap[toffs+1] | ((attr & 0x01) << 8) | ((attr & 0x80) << 2),
		color = (attr & 0x78) >> 3; //TODO: missing 4th bit or palette bank
		sx = 32 * ((offs/2) / 8) - (scrollx & 0x1f);
		sy = (32 * ((offs/2) % 8) - scrolly) & 0xff;

		flipx = attr & 0x02;
		flipy = attr & 0x04;
		if (flip_screen)
		{
			sx = 512-32 - sx;
			sy = 256-32 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		
		dgp9.code = code;
		dgp9.color = color;
		dgp9.flipx = flipx;
		dgp9.flipy = flipy;
		dgp9.sx = sx;
		dgp9.sy = sy;
		drawgfx(&dgp9);
		/* wraparound */
		if (scrolly & 0x1f)
		
{ 
struct drawgfxParams dgp10={
	bitmap, 	// dest
	Machine->gfx[gfx], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	transparency, 	// transparency
	15, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			
			dgp10.code = code;
			dgp10.color = color;
			dgp10.flipx = flipx;
			dgp10.flipy = flipy;
			dgp10.sx = sx;
			dgp10.sy = ((sy + 0x20) & 0xff) - 0x20;
			drawgfx(&dgp10);
		}
} // end of patch paragraph

	}
	} // end of patch paragraph

}

static void draw_tx(mame_bitmap *bitmap,int yoffset)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp11={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x800;offs++)
	{
		int sx,sy,attr;

		attr = lastday_txvideoram[offs+0x800];
		sx = offs / 32;
		sy = offs % 32;
		if (flip_screen)
		{
			sx = 63 - sx;
			sy = 31 - sy;
		}

		
		dgp11.code = lastday_txvideoram[offs] | ((attr & 0x0f) << 8);
		dgp11.color = (attr & 0xf0) >> 4;
		dgp11.flipx = flip_screen;
		dgp11.flipy = flip_screen;
		dgp11.sx = 8*sx;
		dgp11.sy = 8*(sy + yoffset);
		drawgfx(&dgp11);
	}
	} // end of patch paragraph

}

static void bluehawk_draw_tx(mame_bitmap *bitmap)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp12={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x1000;offs += 2)
	{
		int sx,sy,attr;

		attr = lastday_txvideoram[offs+1];
		sx = (offs/2) / 32;
		sy = (offs/2) % 32;
		if (flip_screen)
		{
			sx = 63 - sx;
			sy = 31 - sy;
		}

		
		dgp12.code = lastday_txvideoram[offs] | ((attr & 0x0f) << 8);
		dgp12.color = (attr & 0xf0) >> 4;
		dgp12.flipx = flip_screen;
		dgp12.flipy = flip_screen;
		dgp12.sx = 8*sx;
		dgp12.sy = 8*sy;
		drawgfx(&dgp12);
	}
	} // end of patch paragraph

}

static void draw_sprites(mame_bitmap *bitmap,int pollux_extensions)
{
	int offs;

	
	{ 
	struct drawgfxParams dgp13={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size-32;offs >= 0;offs -= 32)
	{
		int sx,sy,code,color;
		int flipx=0,flipy=0,height=0,y;

		sx = buffered_spriteram[offs+3] | ((buffered_spriteram[offs+1] & 0x10) << 4);
		sy = buffered_spriteram[offs+2];
		code = buffered_spriteram[offs] | ((buffered_spriteram[offs+1] & 0xe0) << 3);
		color = buffered_spriteram[offs+1] & 0x0f;

		if (pollux_extensions)
		{
			/* gulfstrm, pollux, bluehawk */
			code |= ((buffered_spriteram[offs+0x1c] & 0x01) << 11);

			if (pollux_extensions >= 2)
			{
				/* pollux, bluehawk */
				height = (buffered_spriteram[offs+0x1c] & 0x70) >> 4;
				code &= ~height;

				if (pollux_extensions == 3)
				{
					/* bluehawk */
					sy += 6 - ((~buffered_spriteram[offs+0x1c] & 0x02) << 7);
					flipx = buffered_spriteram[offs+0x1c] & 0x08;
					flipy = buffered_spriteram[offs+0x1c] & 0x04;
				}

				if (pollux_extensions == 4)
				{
					/* flytiger */
					sy -=(buffered_spriteram[offs+0x1c] & 0x02) << 7;
					flipx = buffered_spriteram[offs+0x1c] & 0x08;
					flipy = buffered_spriteram[offs+0x1c] & 0x04;
				}
			}
		}

		if (flip_screen)
		{
			sx = 498 - sx;
			sy = 240-16*height - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		for (y = 0;y <= height;y++)
		{
			
			dgp13.code = code+y;
			dgp13.color = color;
			dgp13.flipx = flipx;
			dgp13.flipy = flipy;
			dgp13.sx = sx;
			dgp13.sy = flipy ? sy + 16*(height-y) : sy + 16*y;
			drawgfx(&dgp13);
		}
	}
	} // end of patch paragraph

}

static void rshark_draw_sprites(mame_bitmap *bitmap)
{
	int offs;

	for (offs = 0;offs < spriteram_size/2;offs += 8)
	{
		if (buffered_spriteram16[offs] & 0x0001)	/* enable */
		{
			int sx,sy,code,color;
			int flipx=0,flipy=0,width,height,x,y;

			sx = buffered_spriteram16[offs+4] & 0x01ff;
			sy = (INT16)buffered_spriteram16[offs+6];
			code = buffered_spriteram16[offs+3];
			color = buffered_spriteram16[offs+7];
			width = buffered_spriteram16[offs+1] & 0x000f;
			height = (buffered_spriteram16[offs+1] & 0x00f0) >> 4;

			if (flip_screen)
			{
				sx = 498-16*width - sx;
				sy = 240-16*height - sy;
				flipx = !flipx;
				flipy = !flipy;
			}

			
			{ 
			struct drawgfxParams dgp14={
				bitmap, 	// dest
				Machine->gfx[0], 	// gfx
				0, 	// code
				0, 	// color
				0, 	// flipx
				0, 	// flipy
				0, 	// sx
				0, 	// sy
				&Machine->visible_area, 	// clip
				TRANSPARENCY_PEN, 	// transparency
				15, 	// transparent_color
				0, 	// scalex
				0, 	// scaley
				NULL, 	// pri_buffer
				0 	// priority_mask
			  };
			for (y = 0;y <= height;y++)
			{
				for (x = 0;x <= width;x++)
				{
					
					dgp14.code = code;
					dgp14.color = color;
					dgp14.flipx = flipx;
					dgp14.flipy = flipy;
					dgp14.sx = flipx ? sx + 16*(width-x) : sx + 16*x;
					dgp14.sy = flipy ? sy + 16*(height-y) : sy + 16*y;
					drawgfx(&dgp14);

					code++;
				}
			}
			} // end of patch paragraph

		}
	}
}


VIDEO_UPDATE( lastday )
{
	fillbitmap(bitmap, get_black_pen(), cliprect);

	if(!(lastday_bgscroll[6] & 0x10))
		draw_layer(bitmap,2,lastday_bgscroll,memory_region(REGION_GFX5),TRANSPARENCY_NONE);

	if(!(lastday_fgscroll[6] & 0x10))
		draw_layer(bitmap,3,lastday_fgscroll,memory_region(REGION_GFX6),TRANSPARENCY_PEN);

	if(!sprites_disabled)
		draw_sprites(bitmap,0);

	draw_tx(bitmap,-1);
}

VIDEO_UPDATE( gulfstrm )
{
	fillbitmap(bitmap, get_black_pen(), cliprect);

	if(!(lastday_bgscroll[6] & 0x10))
		draw_layer(bitmap,2,lastday_bgscroll,memory_region(REGION_GFX5),TRANSPARENCY_NONE);

	if(!(lastday_fgscroll[6] & 0x10))
		draw_layer(bitmap,3,lastday_fgscroll,memory_region(REGION_GFX6),TRANSPARENCY_PEN);

	draw_sprites(bitmap,1);
	draw_tx(bitmap,-1);
}

VIDEO_UPDATE( pollux )
{
	fillbitmap(bitmap, get_black_pen(), cliprect);

	if(!(lastday_bgscroll[6] & 0x10))
		draw_layer(bitmap,2,lastday_bgscroll,memory_region(REGION_GFX5),TRANSPARENCY_NONE);

	if(!(lastday_fgscroll[6] & 0x10))
		draw_layer(bitmap,3,lastday_fgscroll,memory_region(REGION_GFX6),TRANSPARENCY_PEN);

	draw_sprites(bitmap,2);
	draw_tx(bitmap,0);
}

VIDEO_UPDATE( flytiger )
{
	fillbitmap(bitmap, get_black_pen(), cliprect);

	if(flytiger_pri)
	{
		if(!(lastday_fgscroll[6] & 0x10))
			flytiger_draw_layer2(bitmap,3,lastday_fgscroll,memory_region(REGION_GFX4)+0x78000,TRANSPARENCY_NONE);

		if(!(lastday_bgscroll[6] & 0x10))
			draw_layer(bitmap,2,lastday_bgscroll,memory_region(REGION_GFX3)+0x78000,TRANSPARENCY_PEN);
	}
	else
	{
		if(!(lastday_bgscroll[6] & 0x10))
			draw_layer(bitmap,2,lastday_bgscroll,memory_region(REGION_GFX3)+0x78000,TRANSPARENCY_NONE);

		if(!(lastday_fgscroll[6] & 0x10))
			flytiger_draw_layer2(bitmap,3,lastday_fgscroll,memory_region(REGION_GFX4)+0x78000,TRANSPARENCY_PEN);
	}

	draw_sprites(bitmap,4);
	draw_tx(bitmap,0);
}


VIDEO_UPDATE( bluehawk )
{
	bluehawk_draw_layer(bitmap,2,lastday_bgscroll,memory_region(REGION_GFX3)+0x78000,TRANSPARENCY_NONE);
	bluehawk_draw_layer(bitmap,3,lastday_fgscroll,memory_region(REGION_GFX4)+0x78000,TRANSPARENCY_PEN);
	draw_sprites(bitmap,3);
	bluehawk_draw_layer2(bitmap,4,bluehawk_fg2scroll,memory_region(REGION_GFX5)+0x38000,TRANSPARENCY_PEN);
	bluehawk_draw_tx(bitmap);
}

VIDEO_UPDATE( primella )
{
	bluehawk_draw_layer(bitmap,1,lastday_bgscroll,memory_region(REGION_GFX2)+memory_region_length(REGION_GFX2)-0x8000,TRANSPARENCY_NONE);
	if (tx_pri) bluehawk_draw_tx(bitmap);
	bluehawk_draw_layer(bitmap,2,lastday_fgscroll,memory_region(REGION_GFX3)+memory_region_length(REGION_GFX3)-0x8000,TRANSPARENCY_PEN);
	if (!tx_pri) bluehawk_draw_tx(bitmap);
}

VIDEO_UPDATE( rshark )
{
	rshark_draw_layer(bitmap,4,rshark_scroll4,memory_region(REGION_GFX5),memory_region(REGION_GFX6)+0x60000,TRANSPARENCY_NONE);
	rshark_draw_layer(bitmap,3,rshark_scroll3,memory_region(REGION_GFX4),memory_region(REGION_GFX6)+0x40000,TRANSPARENCY_PEN);
	rshark_draw_layer(bitmap,2,rshark_scroll2,memory_region(REGION_GFX3),memory_region(REGION_GFX6)+0x20000,TRANSPARENCY_PEN);
	rshark_draw_layer(bitmap,1,rshark_scroll1,memory_region(REGION_GFX2),memory_region(REGION_GFX6)+0x00000,TRANSPARENCY_PEN);
	rshark_draw_sprites(bitmap);
}

VIDEO_UPDATE( popbingo )
{
	popbingo_draw_layer(bitmap,1,popbingo_scroll,memory_region(REGION_GFX2),TRANSPARENCY_NONE);
	rshark_draw_sprites(bitmap);
}


VIDEO_EOF( dooyong )
{
	buffer_spriteram_w(0,0);
}

VIDEO_EOF( rshark )
{
	buffer_spriteram16_w(0,0,0);
}
