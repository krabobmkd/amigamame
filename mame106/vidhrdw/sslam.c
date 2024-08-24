/* Super Slam - Video Hardware */

#include "driver.h"

static tilemap *sslam_bg_tilemap, *sslam_tx_tilemap, *sslam_md_tilemap;

extern UINT16 *sslam_bg_tileram, *sslam_tx_tileram, *sslam_md_tileram;
extern UINT16 *sslam_spriteram, *sslam_regs;

static int sprites_x_offset;

static void sslam_drawsprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	const gfx_element *gfx = Machine->gfx[0];
	UINT16 *source = sslam_spriteram;
	UINT16 *finish = source + 0x1000/2;

	source += 3; // strange

	while( source<finish )
	{
		int xpos, ypos, number, flipx, colr, eightbyeight;

		if (source[0] & 0x2000) break;

		xpos = source[2] & 0x1ff;
		ypos = source[0] & 0x01ff;
		colr = (source[2] & 0xf000) >> 12;
		eightbyeight = source[0] & 0x1000;
		flipx = source[0] & 0x4000;
		number = source[3];

		xpos -=16; xpos -=7; xpos += sprites_x_offset;
		ypos = 0xff - ypos;
		ypos -=16; ypos -=7;

		if(ypos < 0)
			ypos += 256;

		if(ypos >= 249)
			ypos -= 256;

		if (!eightbyeight)
		{
			if (flipx)
			
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
				
				dgp0.code = number;
				dgp0.color = colr;
				dgp0.sx = xpos+8;
				dgp0.sy = ypos;
				drawgfx(&dgp0);
				
				dgp0.code = number+1;
				dgp0.color = colr;
				dgp0.sx = xpos+8;
				dgp0.sy = ypos+8;
				drawgfx(&dgp0);
				
				dgp0.code = number+2;
				dgp0.color = colr;
				dgp0.sx = xpos;
				dgp0.sy = ypos;
				drawgfx(&dgp0);
				
				dgp0.code = number+3;
				dgp0.color = colr;
				dgp0.sx = xpos;
				dgp0.sy = ypos+8;
				drawgfx(&dgp0);
			}
} // end of patch paragraph

			
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
else
			{
				
				dgp4.code = number;
				dgp4.color = colr;
				dgp4.sx = xpos;
				dgp4.sy = ypos;
				drawgfx(&dgp4);
				
				dgp4.code = number+1;
				dgp4.color = colr;
				dgp4.sx = xpos;
				dgp4.sy = ypos+8;
				drawgfx(&dgp4);
				
				dgp4.code = number+2;
				dgp4.color = colr;
				dgp4.sx = xpos+8;
				dgp4.sy = ypos;
				drawgfx(&dgp4);
				
				dgp4.code = number+3;
				dgp4.color = colr;
				dgp4.sx = xpos+8;
				dgp4.sy = ypos+8;
				drawgfx(&dgp4);
			}
} // end of patch paragraph

		}
		else
		{
			if (flipx)
			
{ 
struct drawgfxParams dgp8={
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
				
				dgp8.code = number ^ 2;
				dgp8.color = colr;
				dgp8.sx = xpos;
				dgp8.sy = ypos;
				drawgfx(&dgp8);
			}
} // end of patch paragraph

			
{ 
struct drawgfxParams dgp9={
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
else
			{
				
				dgp9.code = number;
				dgp9.color = colr;
				dgp9.sx = xpos;
				dgp9.sy = ypos;
				drawgfx(&dgp9);
			}
} // end of patch paragraph

		}

		source += 4;
	}

}


/* Text Layer */

static void get_sslam_tx_tile_info(int tile_index)
{
	int code = sslam_tx_tileram[tile_index] & 0x0fff;
	int colr = sslam_tx_tileram[tile_index] & 0xf000;

	SET_TILE_INFO(3,code+0xc000 ,colr >> 12,0)
}

WRITE16_HANDLER( sslam_tx_tileram_w )
{
	COMBINE_DATA(&sslam_tx_tileram[offset]);
	tilemap_mark_tile_dirty(sslam_tx_tilemap,offset);
}

/* Middle Layer */

static void get_sslam_md_tile_info(int tile_index)
{
	int code = sslam_md_tileram[tile_index] & 0x0fff;
	int colr = sslam_md_tileram[tile_index] & 0xf000;

	SET_TILE_INFO(2,code+0x2000 ,colr >> 12,0)
}

WRITE16_HANDLER( sslam_md_tileram_w )
{
	COMBINE_DATA(&sslam_md_tileram[offset]);
	tilemap_mark_tile_dirty(sslam_md_tilemap,offset);
}

/* Background Layer */

static void get_sslam_bg_tile_info(int tile_index)
{
	int code = sslam_bg_tileram[tile_index] & 0x1fff;
	int colr = sslam_bg_tileram[tile_index] & 0xe000;

	SET_TILE_INFO(1,code ,colr >> 13,0)
}

WRITE16_HANDLER( sslam_bg_tileram_w )
{
	COMBINE_DATA(&sslam_bg_tileram[offset]);
	tilemap_mark_tile_dirty(sslam_bg_tilemap,offset);
}

static void get_powerbls_bg_tile_info(int tile_index)
{
	int code = sslam_bg_tileram[tile_index*2+1] & 0x0fff;
	int colr = (sslam_bg_tileram[tile_index*2+1] & 0xf000) >> 12;
	code |= (sslam_bg_tileram[tile_index*2] & 0x0f00) << 4;

	//(sslam_bg_tileram[tile_index*2] & 0x0f00) == 0xf000 ???

	SET_TILE_INFO(1,code,colr,0)
}

WRITE16_HANDLER( powerbls_bg_tileram_w )
{
	COMBINE_DATA(&sslam_bg_tileram[offset]);
	tilemap_mark_tile_dirty(sslam_bg_tilemap,offset>>1);
}

VIDEO_START(sslam)
{
	sslam_bg_tilemap = tilemap_create(get_sslam_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,16,16,32,32);
	sslam_md_tilemap = tilemap_create(get_sslam_md_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	sslam_tx_tilemap = tilemap_create(get_sslam_tx_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,64);

	if (!sslam_bg_tilemap || !sslam_md_tilemap || !sslam_tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(sslam_md_tilemap,0);
	tilemap_set_transparent_pen(sslam_tx_tilemap,0);

	sprites_x_offset = 0;

	return 0;
}

VIDEO_START(powerbls)
{
	sslam_bg_tilemap = tilemap_create(get_powerbls_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,64,64);

	if (!sslam_bg_tilemap)
		return 1;

	sprites_x_offset = -21;

	return 0;
}

VIDEO_UPDATE(sslam)
{
	if(!(sslam_regs[6] & 1))
	{
		fillbitmap(bitmap,get_black_pen(),cliprect);
		return;
	}

	tilemap_set_scrollx(sslam_tx_tilemap,0, sslam_regs[0]+2);
	tilemap_set_scrolly(sslam_tx_tilemap,0, (sslam_regs[1] & 0xff)+8);
	tilemap_set_scrollx(sslam_md_tilemap,0, sslam_regs[2]+2);
	tilemap_set_scrolly(sslam_md_tilemap,0, sslam_regs[3]+8);
	tilemap_set_scrollx(sslam_bg_tilemap,0, sslam_regs[4]+4);
	tilemap_set_scrolly(sslam_bg_tilemap,0, sslam_regs[5]+8);

	tilemap_draw(bitmap,cliprect,sslam_bg_tilemap,0,0);

	/* remove wraparound from the tilemap (used on title screen) */
	if(sslam_regs[2]+2 > 0x8c8)
	{
		rectangle md_clip;
		md_clip.min_x = cliprect->min_x;
		md_clip.max_x = cliprect->max_x - (sslam_regs[2]+2 - 0x8c8);
		md_clip.min_y = cliprect->min_y;
		md_clip.max_y = cliprect->max_y;

		tilemap_draw(bitmap,&md_clip,sslam_md_tilemap,0,0);
	}
	else
	{
		tilemap_draw(bitmap,cliprect,sslam_md_tilemap,0,0);
	}

	sslam_drawsprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,sslam_tx_tilemap,0,0);
}

VIDEO_UPDATE(powerbls)
{
	if(!(sslam_regs[6] & 1))
	{
		fillbitmap(bitmap,get_black_pen(),cliprect);
		return;
	}

	tilemap_set_scrollx(sslam_bg_tilemap,0, sslam_regs[0]+21);
	tilemap_set_scrolly(sslam_bg_tilemap,0, sslam_regs[1]-240);

	tilemap_draw(bitmap,cliprect,sslam_bg_tilemap,0,0);
	sslam_drawsprites(bitmap,cliprect);
}
