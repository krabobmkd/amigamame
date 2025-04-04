// remaining gfx glitches

// layer priority register not fully understood

#include "driver.h"

extern UINT16 *drgnmst_vidregs;

extern UINT16 *drgnmst_fg_videoram;
static tilemap *drgnmst_fg_tilemap;
extern UINT16 *drgnmst_bg_videoram;
static tilemap *drgnmst_bg_tilemap;
extern UINT16 *drgnmst_md_videoram;
static tilemap *drgnmst_md_tilemap;

extern UINT16 *drgnmst_rowscrollram;
extern UINT16 *drgnmst_vidregs2;


static void get_drgnmst_fg_tile_info(int tile_index)
{
	int tileno,colour, flipyx;
	tileno = drgnmst_fg_videoram[tile_index*2] & 0xfff;
	colour = drgnmst_fg_videoram[tile_index*2+1] & 0x1f;
	flipyx = (drgnmst_fg_videoram[tile_index*2+1] & 0x60)>>5;

	SET_TILE_INFO(1,tileno,colour,TILE_FLIPYX(flipyx))
}

WRITE16_HANDLER( drgnmst_fg_videoram_w )
{
	COMBINE_DATA(&drgnmst_fg_videoram[offset]);
	tilemap_mark_tile_dirty(drgnmst_fg_tilemap,offset/2);
}



static void get_drgnmst_bg_tile_info(int tile_index)
{
	int tileno,colour,flipyx;
	tileno = (drgnmst_bg_videoram[tile_index*2]& 0x1fff)+0x800;
	colour = drgnmst_bg_videoram[tile_index*2+1] & 0x1f;
	flipyx = (drgnmst_bg_videoram[tile_index*2+1] & 0x60)>>5;

	SET_TILE_INFO(3,tileno,colour,TILE_FLIPYX(flipyx))
}

WRITE16_HANDLER( drgnmst_bg_videoram_w )
{
	COMBINE_DATA(&drgnmst_bg_videoram[offset]);
	tilemap_mark_tile_dirty(drgnmst_bg_tilemap,offset/2);
}

static void get_drgnmst_md_tile_info(int tile_index)
{
	int tileno,colour,flipyx;
	tileno = (drgnmst_md_videoram[tile_index*2]& 0x7fff)-0x2000;

	colour = drgnmst_md_videoram[tile_index*2+1] & 0x1f;
	flipyx = (drgnmst_md_videoram[tile_index*2+1] & 0x60)>>5;

	SET_TILE_INFO(2,tileno,colour,TILE_FLIPYX(flipyx))
}

WRITE16_HANDLER( drgnmst_md_videoram_w )
{
	COMBINE_DATA(&drgnmst_md_videoram[offset]);
	tilemap_mark_tile_dirty(drgnmst_md_tilemap,offset/2);
}

static void drgnmst_draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	const gfx_element *gfx = Machine->gfx[0];
	UINT16 *source = spriteram16;
	UINT16 *finish = source + 0x800/2;

	
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
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	while( source<finish )
	{
		int xpos, ypos, number, flipx,flipy,wide,high;
		int x,y;
		int incx,incy;
		int colr;

		number = source[2];
		xpos = source[0];
		ypos = source[1];
		flipx = source[3] & 0x0020;
		flipy = source[3] & 0x0040;
		wide = (source[3] & 0x0f00)>>8;
		high = (source[3] & 0xf000)>>12;
		colr = (source[3] & 0x001f);

		if ((source[3] & 0xff00) == 0xff00) break;


		if (!flipx) { incx = 16;} else { incx = -16; xpos += 16*wide; }
		if (!flipy) { incy = 16;} else { incy = -16; ypos += 16*high; }


		for (y=0;y<=high;y++) {
			for (x=0;x<=wide;x++) {

				int realx, realy, realnumber;

				realx = xpos+incx*x;
				realy = ypos+incy*y;
				realnumber = number+x+y*16;

				
				dgp0.code = realnumber;
				dgp0.color = colr;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = realx;
				dgp0.sy = realy;
				drawgfx(&dgp0);

			}
		}

		source+=4;
	}
	} // end of patch paragraph

}


UINT32 drgnmst_fg_tilemap_scan_cols( UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows )
{
	return (col*32)+(row&0x1f)+((row&0xe0)>>5)*2048;
}

UINT32 drgnmst_md_tilemap_scan_cols( UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows )
{
	return (col*16)+(row&0x0f)+((row&0xf0)>>4)*1024;
}

UINT32 drgnmst_bg_tilemap_scan_cols( UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows )
{
	return (col*8)+(row&0x07)+((row&0xf8)>>3)*512;
}

VIDEO_START(drgnmst)
{
	drgnmst_fg_tilemap = tilemap_create(get_drgnmst_fg_tile_info,drgnmst_fg_tilemap_scan_cols,TILEMAP_TRANSPARENT,      8, 8, 64,64);
	tilemap_set_transparent_pen(drgnmst_fg_tilemap,15);

	drgnmst_md_tilemap = tilemap_create(get_drgnmst_md_tile_info,drgnmst_md_tilemap_scan_cols,TILEMAP_TRANSPARENT,      16, 16, 64,64);
	tilemap_set_transparent_pen(drgnmst_md_tilemap,15);

	drgnmst_bg_tilemap = tilemap_create(get_drgnmst_bg_tile_info,drgnmst_bg_tilemap_scan_cols,TILEMAP_TRANSPARENT,      32, 32, 64,64);
	tilemap_set_transparent_pen(drgnmst_bg_tilemap,15);

	// do the other tilemaps have rowscroll too? probably not ..
	tilemap_set_scroll_rows(drgnmst_md_tilemap,1024);

	return 0;
}

VIDEO_UPDATE(drgnmst)
{
	int y, rowscroll_bank;

	tilemap_set_scrollx(drgnmst_bg_tilemap,0, drgnmst_vidregs[10]-18); // verify
	tilemap_set_scrolly(drgnmst_bg_tilemap,0, drgnmst_vidregs[11]); // verify

//  tilemap_set_scrollx(drgnmst_md_tilemap,0, drgnmst_vidregs[8]-16); // rowscrolled
	tilemap_set_scrolly(drgnmst_md_tilemap,0, drgnmst_vidregs[9]); // verify

	tilemap_set_scrollx(drgnmst_fg_tilemap,0, drgnmst_vidregs[6]-18); // verify (test mode colour test needs it)
	tilemap_set_scrolly(drgnmst_fg_tilemap,0, drgnmst_vidregs[7]); // verify

	rowscroll_bank = (drgnmst_vidregs[4] & 0x30) >> 4;

	for (y = 0; y < 1024; y++)
		tilemap_set_scrollx(drgnmst_md_tilemap,y, drgnmst_vidregs[8]-16+drgnmst_rowscrollram[y+0x800*rowscroll_bank]);

	// todo: figure out which bits relate to the order
	switch (drgnmst_vidregs2[0])
	{
		case 0x2451: // fg unsure
		case 0x2d9a: // fg unsure
		case 0x2440: // all ok
		case 0x245a: // fg unsure, title screen
			tilemap_draw(bitmap,cliprect,drgnmst_fg_tilemap,TILEMAP_IGNORE_TRANSPARENCY,0);
			tilemap_draw(bitmap,cliprect,drgnmst_md_tilemap,0,0);
			tilemap_draw(bitmap,cliprect,drgnmst_bg_tilemap,0,0);
			break;
		case 0x23c0: // all ok
			tilemap_draw(bitmap,cliprect,drgnmst_bg_tilemap,TILEMAP_IGNORE_TRANSPARENCY,0);
			tilemap_draw(bitmap,cliprect,drgnmst_fg_tilemap,0,0);
			tilemap_draw(bitmap,cliprect,drgnmst_md_tilemap,0,0);
			break;
		case 0x38da: // fg unsure
		case 0x215a: // fg unsure
		case 0x2140: // all ok
			tilemap_draw(bitmap,cliprect,drgnmst_fg_tilemap,TILEMAP_IGNORE_TRANSPARENCY,0);
			tilemap_draw(bitmap,cliprect,drgnmst_bg_tilemap,0,0);
			tilemap_draw(bitmap,cliprect,drgnmst_md_tilemap,0,0);
			break;
		case 0x2d80: // all ok
			tilemap_draw(bitmap,cliprect,drgnmst_md_tilemap,TILEMAP_IGNORE_TRANSPARENCY,0);
			tilemap_draw(bitmap,cliprect,drgnmst_bg_tilemap,0,0);
			tilemap_draw(bitmap,cliprect,drgnmst_fg_tilemap,0,0);
			break;
		default:
			tilemap_draw(bitmap,cliprect,drgnmst_bg_tilemap,TILEMAP_IGNORE_TRANSPARENCY,0);
			tilemap_draw(bitmap,cliprect,drgnmst_fg_tilemap,0,0);
			tilemap_draw(bitmap,cliprect,drgnmst_md_tilemap,0,0);
			logerror ("unknown video priority regs %04x\n", drgnmst_vidregs2[0]);

	}

	drgnmst_draw_sprites(bitmap,cliprect);

//  ui_popup ("x %04x x %04x x %04x x %04x x %04x", drgnmst_vidregs2[0], drgnmst_vidregs[12], drgnmst_vidregs[13], drgnmst_vidregs[14], drgnmst_vidregs[15]);
//  ui_popup ("x %04x x %04x y %04x y %04x z %04x z %04x",drgnmst_vidregs[0],drgnmst_vidregs[1],drgnmst_vidregs[2],drgnmst_vidregs[3],drgnmst_vidregs[4],drgnmst_vidregs[5]);

}
