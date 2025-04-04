#include "driver.h"



#define VIDEORAM_SIZE 0x1c00
#define SPRITERAM_START 0x1800
#define SPRITERAM_SIZE (VIDEORAM_SIZE-SPRITERAM_START)

static UINT16 *vram,*buf_spriteram,*buf_spriteram2;

#define VREG_SIZE 18
static UINT16 vreg[VREG_SIZE];

static tilemap *bg_tilemap[3];


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

INLINE void get_tile_info(int tile_index,int plane)
{
	UINT16 attr;

	tile_index = 2*tile_index + 0x800*plane;
	attr = vram[tile_index];
	SET_TILE_INFO(
			1,
			vram[tile_index+1],
			attr & 0x7f,
			0)
	tile_info.priority = (attr & 0x0600) >> 9;
}

static void get_tile_info0(int tile_index)
{
	get_tile_info(tile_index,0);
}

static void get_tile_info1(int tile_index)
{
	get_tile_info(tile_index,1);
}

static void get_tile_info2(int tile_index)
{
	get_tile_info(tile_index,2);
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( othldrby )
{
	bg_tilemap[0] = tilemap_create(get_tile_info0,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	bg_tilemap[1] = tilemap_create(get_tile_info1,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	bg_tilemap[2] = tilemap_create(get_tile_info2,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);

	vram = auto_malloc(VIDEORAM_SIZE * sizeof(vram[0]));
	buf_spriteram = auto_malloc(2*SPRITERAM_SIZE * sizeof(buf_spriteram[0]));

	if (!bg_tilemap[0] || !bg_tilemap[1] || !bg_tilemap[2] )
		return 1;

	buf_spriteram2 = buf_spriteram + SPRITERAM_SIZE;

	tilemap_set_transparent_pen(bg_tilemap[0],0);
	tilemap_set_transparent_pen(bg_tilemap[1],0);
	tilemap_set_transparent_pen(bg_tilemap[2],0);

	return 0;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

static unsigned int vram_addr,vreg_addr;

WRITE16_HANDLER( othldrby_videoram_addr_w )
{
	vram_addr = data;
}

READ16_HANDLER( othldrby_videoram_r )
{
	if (vram_addr < VIDEORAM_SIZE)
		return vram[vram_addr++];
	else
	{
		ui_popup("GFXRAM OUT OF BOUNDS %04x",vram_addr);
		return 0;
	}
}

WRITE16_HANDLER( othldrby_videoram_w )
{
	if (vram_addr < VIDEORAM_SIZE)
	{
		if (vram_addr < SPRITERAM_START)
			tilemap_mark_tile_dirty(bg_tilemap[vram_addr/0x800],(vram_addr&0x7ff)/2);
		vram[vram_addr++] = data;
	}
	else
		ui_popup("GFXRAM OUT OF BOUNDS %04x",vram_addr);
}

WRITE16_HANDLER( othldrby_vreg_addr_w )
{
	vreg_addr = data & 0x7f;	/* bit 7 is set when screen is flipped */
}

WRITE16_HANDLER( othldrby_vreg_w )
{
	if (vreg_addr < VREG_SIZE)
		vreg[vreg_addr++] = data;
	else
		ui_popup("%06x: VREG OUT OF BOUNDS %04x",activecpu_get_pc(),vreg_addr);
}



/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect,int priority)
{
	int offs;

	
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
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < SPRITERAM_SIZE;offs += 4)
	{
		int x,y,color,code,sx,sy,flipx,flipy,sizex,sizey,pri;


		pri = (buf_spriteram[offs] & 0x0600) >> 9;
		if (pri != priority) continue;

		flipx = buf_spriteram[offs] & 0x1000;
		flipy = 0;
		color = (buf_spriteram[offs] & 0x01fc) >> 2;
		code = buf_spriteram[offs+1] | ((buf_spriteram[offs] & 0x0003) << 16);
		sx = (buf_spriteram[offs+2] >> 7);
		sy = (buf_spriteram[offs+3] >> 7);
		sizex = (buf_spriteram[offs+2] & 0x000f) + 1;
		sizey = (buf_spriteram[offs+3] & 0x000f) + 1;

		if (flip_screen)
		{
			flipx = !flipx;
			flipy = !flipy;
			sx = 246 - sx;
			sy = 16 - sy;
		}

		for (y = 0;y < sizey;y++)
		{
			for (x = 0;x < sizex;x++)
			{
				
				dgp0.code = code + x + sizex * y;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
				dgp0.sx = (sx + (flipx ? (-8*(x+1)+1) : 8*x) - vreg[6]+44) & 0x1ff;
				dgp0.sy = (sy + (flipy ? (-8*(y+1)+1) : 8*y) - vreg[7]-9) & 0x1ff;
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph

}

VIDEO_UPDATE( othldrby )
{
	int layer;


	flip_screen_set(vreg[0x0f] & 0x80);

	for (layer = 0;layer < 3;layer++)
	{
		if (flip_screen)
		{
			tilemap_set_scrollx(bg_tilemap[layer],0,vreg[2*layer]+59);
			tilemap_set_scrolly(bg_tilemap[layer],0,vreg[2*layer+1]+248);
		}
		else
		{
			tilemap_set_scrollx(bg_tilemap[layer],0,vreg[2*layer]-58);
			tilemap_set_scrolly(bg_tilemap[layer],0,vreg[2*layer+1]+9);
		}
	}

	fillbitmap(priority_bitmap,0,cliprect);

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	for (layer = 0;layer < 3;layer++)
		tilemap_draw(bitmap,cliprect,bg_tilemap[layer],0,0);
	draw_sprites(bitmap,cliprect,0);
	for (layer = 0;layer < 3;layer++)
		tilemap_draw(bitmap,cliprect,bg_tilemap[layer],1,0);
	draw_sprites(bitmap,cliprect,1);
	for (layer = 0;layer < 3;layer++)
		tilemap_draw(bitmap,cliprect,bg_tilemap[layer],2,0);
	draw_sprites(bitmap,cliprect,2);
	for (layer = 0;layer < 3;layer++)
		tilemap_draw(bitmap,cliprect,bg_tilemap[layer],3,0);
	draw_sprites(bitmap,cliprect,3);
}

VIDEO_EOF( othldrby )
{
	/* sprites need to be delayed two frames */
    memcpy(buf_spriteram,buf_spriteram2,SPRITERAM_SIZE*sizeof(buf_spriteram[0]));
    memcpy(buf_spriteram2,&vram[SPRITERAM_START],SPRITERAM_SIZE*sizeof(buf_spriteram[0]));
}
