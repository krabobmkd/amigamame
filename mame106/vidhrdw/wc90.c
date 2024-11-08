#include "driver.h"


UINT8 *wc90_fgvideoram,*wc90_bgvideoram,*wc90_txvideoram;


UINT8 *wc90_scroll0xlo, *wc90_scroll0xhi;
UINT8 *wc90_scroll1xlo, *wc90_scroll1xhi;
UINT8 *wc90_scroll2xlo, *wc90_scroll2xhi;

UINT8 *wc90_scroll0ylo, *wc90_scroll0yhi;
UINT8 *wc90_scroll1ylo, *wc90_scroll1yhi;
UINT8 *wc90_scroll2ylo, *wc90_scroll2yhi;


static tilemap *tx_tilemap,*fg_tilemap,*bg_tilemap;



/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_bg_tile_info(int tile_index)
{
	int attr = wc90_bgvideoram[tile_index];
	int tile = wc90_bgvideoram[tile_index + 0x800] +
					256 * ((attr & 3) + ((attr >> 1) & 4));
	SET_TILE_INFO(
			2,
			tile,
			attr >> 4,
			0)
}

static void get_fg_tile_info(int tile_index)
{
	int attr = wc90_fgvideoram[tile_index];
	int tile = wc90_fgvideoram[tile_index + 0x800] +
					256 * ((attr & 3) + ((attr >> 1) & 4));
	SET_TILE_INFO(
			1,
			tile,
			attr >> 4,
			0)
}

static void get_tx_tile_info(int tile_index)
{
	SET_TILE_INFO(
			0,
			wc90_txvideoram[tile_index + 0x800] + ((wc90_txvideoram[tile_index] & 0x07) << 8),
			wc90_txvideoram[tile_index] >> 4,
			0)
}

static void track_get_bg_tile_info(int tile_index)
{
	int attr = wc90_bgvideoram[tile_index];
	int tile = wc90_bgvideoram[tile_index + 0x800] +
					256 * (attr & 7);
	SET_TILE_INFO(
			2,
			tile,
			attr >> 4,
			0)
}

static void track_get_fg_tile_info(int tile_index)
{
	int attr = wc90_fgvideoram[tile_index];
	int tile = wc90_fgvideoram[tile_index + 0x800] +
					256 * (attr & 7);
	SET_TILE_INFO(
			1,
			tile,
			attr >> 4,
			0)
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( wc90 )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,     16,16,64,32);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,32);
	tx_tilemap = tilemap_create(get_tx_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,64,32);

	if (!bg_tilemap || !fg_tilemap || !tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);
	tilemap_set_transparent_pen(tx_tilemap,0);

	return 0;
}

VIDEO_START( wc90t )
{
	bg_tilemap = tilemap_create(track_get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,     16,16,64,32);
	fg_tilemap = tilemap_create(track_get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,32);
	tx_tilemap = tilemap_create(get_tx_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,64,32);

	if (!bg_tilemap || !fg_tilemap || !tx_tilemap)
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);
	tilemap_set_transparent_pen(tx_tilemap,0);

	return 0;
}


/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( wc90_bgvideoram_w )
{
	if (wc90_bgvideoram[offset] != data)
	{
		wc90_bgvideoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset & 0x7ff);
	}
}

WRITE8_HANDLER( wc90_fgvideoram_w )
{
	if (wc90_fgvideoram[offset] != data)
	{
		wc90_fgvideoram[offset] = data;
		tilemap_mark_tile_dirty(fg_tilemap,offset & 0x7ff);
	}
}

WRITE8_HANDLER( wc90_txvideoram_w )
{
	if (wc90_txvideoram[offset] != data)
	{
		wc90_txvideoram[offset] = data;
		tilemap_mark_tile_dirty(tx_tilemap,offset & 0x7ff);
	}
}



/***************************************************************************

  Display refresh

***************************************************************************/


static char pos32x32[] = { 0, 1, 2, 3 };
static char pos32x32x[] = { 1, 0, 3, 2 };
static char pos32x32y[] = { 2, 3, 0, 1 };
static char pos32x32xy[] = { 3, 2, 1, 0 };

static char pos32x64[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
static char pos32x64x[] = { 5, 4, 7, 6, 1, 0, 3, 2 };
static char pos32x64y[] = { 2, 3, 0, 1,	6, 7, 4, 5 };
static char pos32x64xy[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

static char pos64x32[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
static char pos64x32x[] = { 1, 0, 3, 2, 5, 4, 7, 6 };
static char pos64x32y[] = { 6, 7, 4, 5, 2, 3, 0, 1 };
static char pos64x32xy[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

static char pos64x64[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static char pos64x64x[] = { 5, 4, 7, 6, 1, 0, 3, 2, 13, 12, 15, 14, 9, 8, 11, 10 };
static char pos64x64y[] = { 10, 11, 8, 9, 14, 15, 12, 13, 2, 3, 0, 1, 6, 7,	4, 5 };
static char pos64x64xy[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

static char* p32x32[4] = {
	pos32x32,
	pos32x32x,
	pos32x32y,
	pos32x32xy
};

static char* p32x64[4] = {
	pos32x64,
	pos32x64x,
	pos32x64y,
	pos32x64xy
};

static char* p64x32[4] = {
	pos64x32,
	pos64x32x,
	pos64x32y,
	pos64x32xy
};

static char* p64x64[4] = {
	pos64x64,
	pos64x64x,
	pos64x64y,
	pos64x64xy
};

static void drawsprite_16x16(struct drawgfxParams *pdgp,int code,int sx,int sy,int bank)
{
    pdgp->code = code;
    pdgp->sx = sx;
    pdgp->sy = sy;
    drawgfx(pdgp);
}

static void drawsprite_16x32( struct drawgfxParams *pdgp,int code,int sx,int sy,int bank) {
	
	if ( pdgp->flipy ) {
        
        pdgp->code = code+1;
        pdgp->sx = sx;
        pdgp->sy = sy+16;
        drawgfx(pdgp);
        
        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);
	}
    else {
        
        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code+1;
        pdgp->sx = sx;
        pdgp->sy = sy+16;
        drawgfx(pdgp);
	}


}

static void drawsprite_16x64(struct drawgfxParams *pdgp,int code,int sx,int sy,int bank ) {
	
	if (pdgp->flipy ) {

        pdgp->code = code+3;
        pdgp->sx = sx;
        pdgp->sy = sy+48;
        drawgfx(pdgp);

        pdgp->code = code+2;
        pdgp->sx = sx;
        pdgp->sy = sy+32;
        drawgfx(pdgp);

        pdgp->code = code+1;
        pdgp->sx = sx;
        pdgp->sy = sy+16;
        drawgfx(pdgp);

        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);
	} else {

        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code+1;
        pdgp->sx = sx;
        pdgp->sy = sy+16;
        drawgfx(pdgp);

        pdgp->code = code+2;
        pdgp->sx = sx;
        pdgp->sy = sy+32;
        drawgfx(pdgp);

        pdgp->code = code+3;
        pdgp->sx = sx;
        pdgp->sy = sy+48;
        drawgfx(pdgp);
	}


}

static void drawsprite_32x16( struct drawgfxParams *pdgp,int code,int sx,int sy,int bank ) {

	if ( pdgp->flipx ) {

        pdgp->code = code+1;
        pdgp->sx = sx+16;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);
	} else {

        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code+1;
        pdgp->sx = sx+16;
        pdgp->sy = sy;
        drawgfx(pdgp);
	}

}

static void drawsprite_32x32( struct drawgfxParams *pdgp,int code,int sx,int sy,int bank )
{ 

	char *p = p32x32[ bank&3 ];

    pdgp->code = code+p[0];
    pdgp->sx = sx;
    pdgp->sy = sy;
    drawgfx(pdgp);
    
    pdgp->code = code+p[1];
    pdgp->sx = sx+16;
    pdgp->sy = sy;
    drawgfx(pdgp);

    pdgp->code = code+p[2];
    pdgp->sx = sx;
    pdgp->sy = sy+16;
    drawgfx(pdgp);

    pdgp->code = code+p[3];
    pdgp->sx = sx+16;
    pdgp->sy = sy+16;
    drawgfx(pdgp);
}

static void drawsprite_32x64( struct drawgfxParams *pdgp,int code,int sx,int sy ,int bank)
{
	char *p = p32x64[ bank&3 ];

    pdgp->code = code+p[0];
    pdgp->sx = sx;
    pdgp->sy = sy;
    drawgfx(pdgp);

    pdgp->code = code+p[1];
    pdgp->sx = sx+16;
    pdgp->sy = sy;
    drawgfx(pdgp);

    pdgp->code = code+p[2];
    pdgp->sx = sx;
    pdgp->sy = sy+16;
    drawgfx(pdgp);

    pdgp->code = code+p[3];
    pdgp->sx = sx+16;
    pdgp->sy = sy+16;
    drawgfx(pdgp);

    pdgp->code = code+p[4];
    pdgp->sx = sx;
    pdgp->sy = sy+32;
    drawgfx(pdgp);

    pdgp->code = code+p[5];
    pdgp->sx = sx+16;
    pdgp->sy = sy+32;
    drawgfx(pdgp);

    pdgp->code = code+p[6];
    pdgp->sx = sx;
    pdgp->sy = sy+48;
    drawgfx(pdgp);

    pdgp->code = code+p[7];
    pdgp->sx = sx+16;
    pdgp->sy = sy+48;
    drawgfx(pdgp);
}



static void drawsprite_64x16( struct drawgfxParams *pdgp,int code,int sx,int sy,int bank ) {
	
	if ( pdgp->flipx ) {

        pdgp->code = code+3;
        pdgp->sx = sx+48;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code+2;
        pdgp->sx = sx+32;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code+1;
        pdgp->sx = sx+16;
        pdgp->sy = sy;
        drawgfx(pdgp);

        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);
	} else {
        
        pdgp->code = code;
        pdgp->sx = sx;
        pdgp->sy = sy;
        drawgfx(pdgp);
        
        pdgp->code = code+1;
        pdgp->sx = sx+16;
        pdgp->sy = sy;
        drawgfx(pdgp);
        
        pdgp->code = code+2;
        pdgp->sx = sx+32;
        pdgp->sy = sy;
        drawgfx(pdgp);
        
        pdgp->code = code+3;
        pdgp->sx = sx+48;
        pdgp->sy = sy;
        drawgfx(pdgp);
	}
}

static void drawsprite_64x32( struct drawgfxParams *pdgp,int code,int sx,int sy,int bank ) {

	char *p = p64x32[ bank&3 ];

    static const int dxy[]={
        0,0,
        16,0,
        0,16,
        16,16,
        32,0,
        48,0,
        32,16,
        48,16
    };
    const int *pdxy = dxy;

    for(UINT16 i=0;i<8;i++)
    {
        int dx = *pdxy++;
        int dy = *pdxy++;
        
        pdgp->code = code+p[i];
        pdgp->sx = sx+dx;
        pdgp->sy = sy+dy;
        drawgfx(pdgp);
    }

}

static void drawsprite_64x64(struct drawgfxParams *pdgp,int code,int sx,int sy,int bank ) {

	char *p = p64x64[ bank&3 ];

    static const int dxy[]={
        0,0,
        16,0,
        0,16,
        16,16,
        32,0,
        48,0,
        32,16,
        48,16,
        0,32,
        16,32,
        0,48,
        16,48,
        32,32,
        48,32,
        32,48,
        48,48
    };
    const int *pdxy = dxy;

    for(UINT16 i=0;i<16;i++)
    {
        int dx = *pdxy++;
        int dy = *pdxy++;

        pdgp->code = code+p[i];
        pdgp->sx = sx+dx;
        pdgp->sy = sy+dy;
        drawgfx(pdgp);
    }
}

static void drawsprite_invalid(struct drawgfxParams *pdgp,int code,int sx,int sy,int bank ) {
	loginfo(2,"8 pixel sprite size not supported\n" );
}
//  mame_bitmap *, const rectangle *, int, int, int, int, int )
typedef void (*drawsprites_procdef)(struct drawgfxParams *pdgp,int code,int sx,int sy,int bank);

static drawsprites_procdef drawsprites_proc[16] = {
	drawsprite_invalid,		/* 0000 = 08x08 */
	drawsprite_invalid,		/* 0001 = 16x08 */
	drawsprite_invalid,		/* 0010 = 32x08 */
	drawsprite_invalid,		/* 0011 = 64x08 */
	drawsprite_invalid,		/* 0100 = 08x16 */
	drawsprite_16x16,		/* 0101 = 16x16 */
	drawsprite_32x16,		/* 0110 = 32x16 */
	drawsprite_64x16,		/* 0111 = 64x16 */
	drawsprite_invalid,		/* 1000 = 08x32 */
	drawsprite_16x32,		/* 1001 = 16x32 */
	drawsprite_32x32,		/* 1010 = 32x32 */
	drawsprite_64x32,		/* 1011 = 64x32 */
	drawsprite_invalid,		/* 1100 = 08x64 */
	drawsprite_16x64,		/* 1101 = 16x64 */
	drawsprite_32x64,		/* 1110 = 32x64 */
	drawsprite_64x64		/* 1111 = 64x64 */
};

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect, int priority )
{
	int offs, sx,sy, flags, which;

    struct drawgfxParams dgp={
		bitmap, 	// dest
		Machine->gfx[3], 	// gfx
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


	/* draw all visible sprites of specified priority */
	for (offs = 0;offs < spriteram_size;offs += 16){
		int bank = spriteram[offs+0];

		if ( ( bank >> 4 ) == priority ) {

			if ( bank & 4 ) { /* visible */
				which = ( spriteram[offs+2] >> 2 ) + ( spriteram[offs+3] << 6 );

				sx = spriteram[offs + 8] + ( (spriteram[offs + 9] & 1 ) << 8 );
				sy = spriteram[offs + 6] + ( (spriteram[offs + 7] & 1 ) << 8 );

				flags = spriteram[offs+4];

                dgp.color = flags >> 4;
                dgp.flipx = bank&1; 	// flipx
                dgp.flipy = bank&2; 	// flipy

				( *( drawsprites_proc[ flags & 0x0f ] ) )( &dgp,which,sx,sy,bank );
			}
		}
	}
}


VIDEO_UPDATE( wc90 )
{
	tilemap_set_scrollx(bg_tilemap,0,wc90_scroll2xlo[0] + 256 * wc90_scroll2xhi[0]);
	tilemap_set_scrolly(bg_tilemap,0,wc90_scroll2ylo[0] + 256 * wc90_scroll2yhi[0]);
	tilemap_set_scrollx(fg_tilemap,0,wc90_scroll1xlo[0] + 256 * wc90_scroll1xhi[0]);
	tilemap_set_scrolly(fg_tilemap,0,wc90_scroll1ylo[0] + 256 * wc90_scroll1yhi[0]);
	tilemap_set_scrollx(tx_tilemap,0,wc90_scroll0xlo[0] + 256 * wc90_scroll0xhi[0]);
	tilemap_set_scrolly(tx_tilemap,0,wc90_scroll0ylo[0] + 256 * wc90_scroll0yhi[0]);

//  draw_sprites( bitmap,cliprect, 3 );
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites( bitmap,cliprect, 2 );
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
	draw_sprites( bitmap,cliprect, 1 );
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
	draw_sprites( bitmap,cliprect, 0 );
}
