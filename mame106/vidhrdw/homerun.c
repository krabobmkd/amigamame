#include "driver.h"

static int homerun_gfx_ctrl;
static int homerun_gc_up,homerun_gc_down;
int homerun_xpa,homerun_xpb,homerun_xpc;

tilemap *homerun_tilemap;
UINT8 *homerun_videoram;

#define half_screen 116

WRITE8_HANDLER(homerun_banking_w)
{
	if(cpu_getscanline()>half_screen)
		homerun_gc_down=data&3;
	else
		homerun_gc_up=data&3;

  	tilemap_mark_all_tiles_dirty(homerun_tilemap);

	data>>=5;
	if(!data)
		memory_set_bankptr(1, memory_region(REGION_CPU1) );
	else
		memory_set_bankptr(1, memory_region(REGION_CPU1) + 0x10000 + (((data-1)&0x7)*0x4000 ));
}

WRITE8_HANDLER( homerun_videoram_w )
{

	homerun_videoram[offset]=data;
	tilemap_mark_tile_dirty(homerun_tilemap,offset&0xfff);
}

WRITE8_HANDLER(homerun_color_w)
{
	int r,g,b;
	int bit0,bit1,bit2;
	bit0 = (data >> 0) & 0x01;
	bit1 = (data >> 1) & 0x01;
	bit2 = (data >> 2) & 0x01;
	r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
	bit0 = (data >> 3) & 0x01;
	bit1 = (data >> 4) & 0x01;
	bit2 = (data >> 5) & 0x01;
	g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
	bit0 = 0;
	bit1 = (data >> 6) & 0x01;
	bit2 = (data >> 7) & 0x01;
	b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
	palette_set_color(offset,r,g,b);
}

static void get_homerun_tile_info(int tile_index)
{
	int tileno,palno;
	tileno = (homerun_videoram[tile_index])+((homerun_videoram[tile_index+0x1000]&0x38)<<5)+ ((homerun_gfx_ctrl&1)<<11);
	palno=(homerun_videoram[tile_index+0x1000]&0x7);
	SET_TILE_INFO(0,tileno,palno,0)
}



VIDEO_START(homerun)
{
	homerun_tilemap = tilemap_create(get_homerun_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE, 8, 8,64,64);
	return 0;
}

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect )
{
	int offs;
	
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
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = spriteram_size-4; offs >=0; offs -= 4)
	{
		int code,color,sx,sy,flipx,flipy;
		sx = spriteram[offs+3];
		sy = spriteram[offs+0]-16;
		code = (spriteram[offs+1] ) +((spriteram[offs+2]&0x8)<<5 )+(homerun_gfx_ctrl<<9);
		color = (spriteram[offs+2] & 0x7)+8 ;
		flipx=(spriteram[offs+2] & 0x40) ;
		flipy=(spriteram[offs+2] & 0x80) ;
		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipx;
		dgp0.flipy = flipy;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

VIDEO_UPDATE(homerun)
{
	rectangle myclip=*cliprect;

	/* upper part */

	tilemap_set_scrollx(homerun_tilemap, 0, homerun_xpc+((homerun_xpa&2)<<7) );
	tilemap_set_scrolly(homerun_tilemap, 0, homerun_xpb+((homerun_xpa&1)<<8) );
	myclip.max_y/=2;
	homerun_gfx_ctrl=homerun_gc_up;
	tilemap_draw(bitmap,&myclip,homerun_tilemap,0,0);
	draw_sprites(bitmap,&myclip);

	/* lower part */

	myclip.min_y+=myclip.max_y;
	myclip.max_y*=2;
	homerun_gfx_ctrl=homerun_gc_down;
	tilemap_draw(bitmap,&myclip,homerun_tilemap,0,0);
	draw_sprites(bitmap,&myclip);

	homerun_gc_down=homerun_gc_up;
}


