#include "driver.h"
#include "deco16ic.h"

UINT16* pcktgaldb_fgram;
UINT16* pcktgaldb_sprites;


static void pktgaldx_drawsprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int offs;
	int flipscreen=!flip_screen;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[2], 	// gfx
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
	for (offs = 0;offs < 0x400;offs += 4)
	{
		int x,y,sprite,colour,multi,fx,fy,inc,flash,mult;

		sprite = spriteram16[offs+1];
		if (!sprite) continue;

		y = spriteram16[offs];
		flash=y&0x1000;
		if (flash && (cpu_getcurrentframe() & 1)) continue;

		x = spriteram16[offs+2];
		colour = (x >>9) & 0x1f;

		fx = y & 0x2000;
		fy = y & 0x4000;
		multi = (1 << ((y & 0x0600) >> 9)) - 1;	/* 1x, 2x, 4x, 8x height */

		x = x & 0x01ff;
		y = y & 0x01ff;
		if (x >= 320) x -= 512;
		if (y >= 256) y -= 512;
		y = 240 - y;
        x = 304 - x;

		if (x>320) continue;

		sprite &= ~multi;
		if (fy)
			inc = -1;
		else
		{
			sprite += multi;
			inc = 1;
		}

		if (flipscreen)
		{
			y=240-y;
			x=304-x;
			if (fx) fx=0; else fx=1;
			if (fy) fy=0; else fy=1;
			mult=16;
		}
		else mult=-16;

		while (multi >= 0)
		{
			
			dgp0.code = sprite - multi * inc;
			dgp0.color = colour;
			dgp0.flipx = fx;
			dgp0.flipy = fy;
			dgp0.sx = x;
			dgp0.sy = y + mult * multi;
			drawgfx(&dgp0);

			multi--;
		}
	}
	} // end of patch paragraph

}

static int pktgaldx_bank_callback(const int bank)
{
	return ((bank>>4) & 0x7) * 0x1000;
}

/* Video on the orginal */
VIDEO_START(pktgaldx)
{
	if (deco16_1_video_init())
		return 1;

	deco16_set_tilemap_bank_callback(1,pktgaldx_bank_callback);

	return 0;
}

VIDEO_UPDATE(pktgaldx)
{
	flip_screen_set( deco16_pf12_control[0]&0x80 );
	deco16_pf12_update(deco16_pf1_rowscroll,deco16_pf2_rowscroll);

	fillbitmap(bitmap,Machine->pens[0x0],cliprect); /* not Confirmed */
	fillbitmap(priority_bitmap,0,NULL);

	deco16_tilemap_2_draw(bitmap,cliprect,0,0);
	pktgaldx_drawsprites(bitmap,cliprect);
	deco16_tilemap_1_draw(bitmap,cliprect,0,0);
}

/* Video for the bootleg */
VIDEO_START(pktgaldb)
{
	return 0;
}

VIDEO_UPDATE(pktgaldb)
{
	int x,y;
	int offset=0;
	int tileno;
	int colour;

	fillbitmap(bitmap, get_black_pen(), cliprect);

	/* the bootleg seems to treat the tilemaps as sprites */
	
	{ 
	struct drawgfxParams dgp1={
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
	for (offset = 0;offset<0x1600/2;offset+=8)
	{
		tileno =  pcktgaldb_sprites[offset+3] | (pcktgaldb_sprites[offset+2]<<16);
		colour =  pcktgaldb_sprites[offset+1]>>1;
		x = pcktgaldb_sprites[offset+0];
		y = pcktgaldb_sprites[offset+4];

		x-=0xc2;
		y&=0x1ff;
		y-=8;

		
		dgp1.code = tileno^0x1000;
		dgp1.color = colour;
		dgp1.sx = x;
		dgp1.sy = y;
		drawgfx(&dgp1);
	}
	} // end of patch paragraph


	
	{ 
	struct drawgfxParams dgp2={
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
	for (offset = 0x1600/2;offset<0x2000/2;offset+=8)
	{
		tileno =  pcktgaldb_sprites[offset+3] | (pcktgaldb_sprites[offset+2]<<16);
		colour =  pcktgaldb_sprites[offset+1]>>1;
		x = pcktgaldb_sprites[offset+0]&0x1ff;
		y = pcktgaldb_sprites[offset+4]&0x0ff;

		x-=0xc2;
		y&=0x1ff;
		y-=8;

		
		dgp2.code = tileno^0x4000;
		dgp2.color = colour;
		dgp2.sx = x;
		dgp2.sy = y;
		drawgfx(&dgp2);
	}
	} // end of patch paragraph


	
	{ 
	struct drawgfxParams dgp3={
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
	for (offset = 0x2000/2;offset<0x4000/2;offset+=8)
	{
		tileno =  pcktgaldb_sprites[offset+3] | (pcktgaldb_sprites[offset+2]<<16);
		colour =  pcktgaldb_sprites[offset+1]>>1;
		x = pcktgaldb_sprites[offset+0]&0x1ff;
		y = pcktgaldb_sprites[offset+4]&0x0ff;

		x-=0xc2;
		y&=0x1ff;
		y-=8;

		
		dgp3.code = tileno^0x3000;
		dgp3.color = colour;
		dgp3.sx = x;
		dgp3.sy = y;
		drawgfx(&dgp3);
	}
	} // end of patch paragraph


}
