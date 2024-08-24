#include "driver.h"

unsigned char *lsasquad_scrollram;



static void draw_layer(mame_bitmap *bitmap,unsigned char *scrollram)
{
	int offs,scrollx,scrolly;


	scrollx = scrollram[3];
	scrolly = -scrollram[0];

	
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x080;offs += 4)
	{
		int base,y,sx,sy,code,color;

		base = 64 * scrollram[offs+1];
		sx = 8*(offs/4) + scrollx;
		if (flip_screen) sx = 248 - sx;
		sx &= 0xff;

		for (y = 0;y < 32;y++)
		{
			int attr;

			sy = 8*y + scrolly;
			if (flip_screen) sy = 248 - sy;
			sy &= 0xff;

			attr = videoram[base + 2*y + 1];
			code = videoram[base + 2*y] + ((attr & 0x0f) << 8);
			color = attr >> 4;

			
			dgp0.code = code;
			dgp0.color = color;
			dgp0.flipx = flip_screen;
			dgp0.flipy = flip_screen;
			dgp0.sx = sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
			if (sx > 248)	/* wraparound */
				
				dgp0.code = code;
				dgp0.color = color;
				dgp0.flipx = flip_screen;
				dgp0.flipy = flip_screen;
				dgp0.sx = sx-256;
				dgp0.sy = sy;
				drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}

static void draw_sprites(mame_bitmap *bitmap)
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
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size-4;offs >= 0;offs -= 4)
	{
		int sx,sy,attr,code,color,flipx,flipy;

		sx = spriteram[offs+3];
		sy = 240 - spriteram[offs];
		attr = spriteram[offs+1];
		code = spriteram[offs+2] + ((attr & 0x30) << 4);
		color = attr & 0x0f;
		flipx = attr & 0x40;
		flipy = attr & 0x80;

		if (flip_screen)
		{
			sx = 240 - sx;
			sy = 240 - sy;
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
		
		dgp2.code = code;
		dgp2.color = color;
		dgp2.flipx = flipx;
		dgp2.flipy = flipy;
		dgp2.sx = sx-256;
		dgp2.sy = sy;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( lsasquad )
{
	fillbitmap(bitmap,Machine->pens[511],&Machine->visible_area);

	draw_layer(bitmap,lsasquad_scrollram + 0x000);
	draw_layer(bitmap,lsasquad_scrollram + 0x080);
	draw_sprites(bitmap);
	draw_layer(bitmap,lsasquad_scrollram + 0x100);
}
