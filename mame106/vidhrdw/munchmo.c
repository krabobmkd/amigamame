#include "driver.h"


UINT8 *mnchmobl_vreg;
UINT8 *mnchmobl_status_vram;
UINT8 *mnchmobl_sprite_xpos;
UINT8 *mnchmobl_sprite_attr;
UINT8 *mnchmobl_sprite_tile;

static int mnchmobl_palette_bank;
static int flipscreen;

PALETTE_INIT( mnchmobl )
{
	int i;

	for (i = 0;i < Machine->drv->total_colors;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = (color_prom[i] >> 6) & 0x01;
		bit1 = (color_prom[i] >> 7) & 0x01;
		b = 0x4f * bit0 + 0xa8 * bit1;

		palette_set_color(i,r,g,b);
	}
}

WRITE8_HANDLER( mnchmobl_palette_bank_w )
{
	if( mnchmobl_palette_bank!=(data&0x3) )
	{
		memset( dirtybuffer, 1, 0x100 );
		mnchmobl_palette_bank = data&0x3;
	}
}

WRITE8_HANDLER( mnchmobl_flipscreen_w )
{
	if( flipscreen!=data )
	{
		memset( dirtybuffer, 1, 0x100 );
		flipscreen = data;
	}
}


READ8_HANDLER( mnchmobl_sprite_xpos_r ){ return mnchmobl_sprite_xpos[offset]; }
WRITE8_HANDLER( mnchmobl_sprite_xpos_w ){ mnchmobl_sprite_xpos[offset] = data; }

READ8_HANDLER( mnchmobl_sprite_attr_r ){ return mnchmobl_sprite_attr[offset]; }
WRITE8_HANDLER( mnchmobl_sprite_attr_w ){ mnchmobl_sprite_attr[offset] = data; }

READ8_HANDLER( mnchmobl_sprite_tile_r ){ return mnchmobl_sprite_tile[offset]; }
WRITE8_HANDLER( mnchmobl_sprite_tile_w ){ mnchmobl_sprite_tile[offset] = data; }

VIDEO_START( mnchmobl )
{
	dirtybuffer = auto_malloc(0x100);
	memset( dirtybuffer, 1, 0x100 );
	tmpbitmap = auto_bitmap_alloc(512,512);
	if( !tmpbitmap )
		return 1;
	return 0;
}

READ8_HANDLER( mnchmobl_videoram_r )
{
	return videoram[offset];
}

WRITE8_HANDLER( mnchmobl_videoram_w )
{
	offset = offset&0xff; /* mirror the two banks? */
	if( videoram[offset]!=data )
	{
		videoram[offset] = data;
		dirtybuffer[offset] = 1;
	}
}

static void draw_status( mame_bitmap *bitmap )
{
	rectangle clip = Machine->visible_area;
	const gfx_element *gfx = Machine->gfx[0];
	int row;

	
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
		&clip, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for( row=0; row<4; row++ )
	{
		int sy,sx = (row&1)*8;
		const unsigned char *source = mnchmobl_status_vram + (~row&1)*32;
		if( row<=1 )
		{
			source+=2*32;
			sx+=256+32+16;
		}
		for( sy=0; sy<256; sy+=8 )
		{
			
			dgp0.code = *source++;
			dgp0.flipx = /* color */				0;
			dgp0.sx = /* no flip */				sx;
			dgp0.sy = sy;
			drawgfx(&dgp0);
		}
	}
	} // end of patch paragraph

}

static void draw_background( mame_bitmap *bitmap )
{
/*
    ROM B1.2C contains 256 tilemaps defining 4x4 configurations of
    the tiles in ROM B2.2B
*/
	unsigned char *tile_data = memory_region(REGION_GFX2);
	const gfx_element *gfx = Machine->gfx[1];
	int offs;

	
	{ 
	struct drawgfxParams dgp1={
		tmpbitmap, 	// dest
		gfx, 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		0, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for( offs=0; offs<0x100; offs++ )
	{
		if( dirtybuffer[offs] )
		{
			int sy = (offs%16)*32;
			int sx = (offs/16)*32;
			int tile_number = videoram[offs];
			int row,col;
			dirtybuffer[offs] = 0;
			for( row=0; row<4; row++ )
			{
				for( col=0; col<4; col++ )
				{
					
					dgp1.code = tile_data[col+tile_number*4+row*0x400];
					dgp1.color = mnchmobl_palette_bank;
					dgp1.sx = /* flip */						sx+col*8;
					dgp1.sy = sy+row*8;
					drawgfx(&dgp1);
				}
			}
		}
	}
	} // end of patch paragraph


	{
		int scrollx = -(mnchmobl_vreg[6]*2+(mnchmobl_vreg[7]>>7))-64-128-16;
		int scrolly = 0;

		copyscrollbitmap(bitmap,tmpbitmap,
			1,&scrollx,1,&scrolly,
			&Machine->visible_area,TRANSPARENCY_NONE,0);
	}
}

static void draw_sprites( mame_bitmap *bitmap )
{
	const rectangle *clip = &Machine->visible_area;
	int scroll = mnchmobl_vreg[6];
	int flags = mnchmobl_vreg[7];					/*   XB?????? */
	int xadjust = - 128-16 - ((flags&0x80)?1:0);
	int bank = (flags&0x40)?1:0;
	const gfx_element *gfx = Machine->gfx[2+bank];
	int color_base = mnchmobl_palette_bank*4+3;
	int i;
	
	{ 
	struct drawgfxParams dgp2={
		bitmap, 	// dest
		gfx, 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		clip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		7, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for( i=0; i<0x200; i++ )
	{
		int tile_number = mnchmobl_sprite_tile[i];	/*   ETTTTTTT */
		int attributes = mnchmobl_sprite_attr[i];	/*   XYYYYYCC */
		int sx = mnchmobl_sprite_xpos[i];			/*   XXXXXXX? */
		int sy = (i/0x40)*0x20;						/* Y YY------ */
		sy += (attributes>>2)&0x1f;
		if( tile_number != 0xff && (attributes&0x80) )
		{
			sx = (sx>>1) | (tile_number&0x80);
			sx = 2*((-32-scroll - sx)&0xff)+xadjust;
			
			dgp2.code = 0x7f - (tile_number&0x7f);
			dgp2.color = color_base-(attributes&0x03);
			dgp2.sx = /* no flip */				sx;
			dgp2.sy = sy;
			drawgfx(&dgp2);
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( mnchmobl )
{
	draw_background( bitmap );
	draw_sprites( bitmap );
	draw_status( bitmap );
}
