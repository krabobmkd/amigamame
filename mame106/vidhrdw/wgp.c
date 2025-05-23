#include "driver.h"
#include "vidhrdw/taitoic.h"

#define TC0100SCN_GFX_NUM 1

static tilemap *wgp_piv_tilemap[3];

UINT16 *wgp_spritemap;
size_t wgp_spritemap_size;
UINT16 *wgp_pivram;
UINT16 *wgp_piv_ctrlram;

static UINT16 wgp_piv_ctrl_reg;
static UINT16 wgp_piv_zoom[3],wgp_piv_scrollx[3],wgp_piv_scrolly[3];
UINT16 wgp_rotate_ctrl[8];
int wgp_piv_xoffs,wgp_piv_yoffs;



/*******************************************************************/

static void common_get_piv_tile_info(int num,int tile_index)
{
	UINT16 tilenum  = wgp_pivram[tile_index + num*0x1000];	/* 3 blocks of $2000 */
	UINT16 attr = wgp_pivram[tile_index + num*0x1000 + 0x8000];	/* 3 blocks of $2000 */

	SET_TILE_INFO(
			2,
			tilenum & 0x3fff,
			(attr & 0x3f),	/* attr &0x1 ?? */
			TILE_FLIPYX( (attr & 0xc0) >> 6))
}

static void get_piv0_tile_info(int tile_index)
{
	common_get_piv_tile_info(0,tile_index);
}

static void get_piv1_tile_info(int tile_index)
{
	common_get_piv_tile_info(1,tile_index);
}

static void get_piv2_tile_info(int tile_index)
{
	common_get_piv_tile_info(2,tile_index);
}

static void dirty_piv_tilemaps(void)
{
	tilemap_mark_all_tiles_dirty(wgp_piv_tilemap[0]);
	tilemap_mark_all_tiles_dirty(wgp_piv_tilemap[1]);
	tilemap_mark_all_tiles_dirty(wgp_piv_tilemap[2]);
}


int wgp_core_vh_start (int x_offs,int y_offs,int piv_xoffs,int piv_yoffs)
{
	wgp_piv_tilemap[0] = tilemap_create(get_piv0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,64);
	wgp_piv_tilemap[1] = tilemap_create(get_piv1_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,64);
	wgp_piv_tilemap[2] = tilemap_create(get_piv2_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,64);

	if (!wgp_piv_tilemap[0] || !wgp_piv_tilemap[1] || !wgp_piv_tilemap[2] )
		return 1;

	if (TC0100SCN_vh_start(1,TC0100SCN_GFX_NUM,x_offs,y_offs,0,0,0,0,0))
		return 1;

	if (has_TC0110PCR())
		if (TC0110PCR_vh_start())
			return 1;

	wgp_piv_xoffs = piv_xoffs;
	wgp_piv_yoffs = piv_yoffs;

	tilemap_set_transparent_pen( wgp_piv_tilemap[0],0 );
	tilemap_set_transparent_pen( wgp_piv_tilemap[1],0 );
	tilemap_set_transparent_pen( wgp_piv_tilemap[2],0 );

	/* flipscreen n/a */
	tilemap_set_scrolldx( wgp_piv_tilemap[0],-piv_xoffs,0 );
	tilemap_set_scrolldx( wgp_piv_tilemap[1],-piv_xoffs,0 );
	tilemap_set_scrolldx( wgp_piv_tilemap[2],-piv_xoffs,0 );
	tilemap_set_scrolldy( wgp_piv_tilemap[0],-piv_yoffs,0 );
	tilemap_set_scrolldy( wgp_piv_tilemap[1],-piv_yoffs,0 );
	tilemap_set_scrolldy( wgp_piv_tilemap[2],-piv_yoffs,0 );

	/* We don't need tilemap_set_scroll_rows, as the custom draw routine
       applies rowscroll manually */

	TC0100SCN_set_colbanks(0x80,0xc0,0x40);

	/* colors from saved states are often screwy (and this doesn't help...) */
	state_save_register_func_postload(dirty_piv_tilemaps);

	return 0;
}

VIDEO_START( wgp )
{
	return (wgp_core_vh_start(0,0,32,16));
}

VIDEO_START( wgp2 )
{
	return (wgp_core_vh_start(4,2,32,16));
}


/******************************************************************
                 PIV TILEMAP READ AND WRITE HANDLERS

Piv Tilemaps
------------

(The unused gaps look as though Taito considered making their
custom chip capable of four rather than three tilemaps.)

500000 - 501fff : unknown/unused
502000 - 507fff : piv tilemaps 0-2 [tile numbers only]

508000 - 50ffff : this area relates to pixel rows in each piv tilemap.
    Includes rowscroll for the piv tilemaps, 1-2 of which act as a
    simple road. To curve, it has rowscroll applied to each row.

508000 - 5087ff unknown/unused

508800  piv0 row color bank (low byte = row horizontal zoom)
509000  piv1 row color bank (low byte = row horizontal zoom)
509800  piv2 row color bank (low byte = row horizontal zoom)

    Usual low byte is 0x7f, the default row horizontal zoom.

    The high byte is the color offset per pixel row. Controlling
    color bank per scanline is rare in Taito games. Top Speed may
    have a similar system to make its road 'move'.

    In-game the high bytes are set to various values (seen 0 - 0x2b).

50a000  piv0 rowscroll [sky]  (not used, but the code supports this)
50c000  piv1 rowscroll [road] (values 0xfd00 - 0x400)
50e000  piv2 rowscroll [road or scenery] (values 0xfd00 - 0x403)

    [It seems strange that unnecessarily large space allocations were
    made for rowscroll. Perhaps the raster color/zoom effects were an
    afterthought, and 508000-9fff was originally slated as rowscroll
    for 'missing' 4th piv layer. Certainly the layout is illogical.]

510000 - 511fff : unknown/unused
512000 - 517fff : piv tilemaps 0-2 [just tile colors ??]

*******************************************************************/

READ16_HANDLER( wgp_pivram_word_r )
{
	return wgp_pivram[offset];
}

WRITE16_HANDLER( wgp_pivram_word_w )
{
	UINT16 oldword = wgp_pivram[offset];
	COMBINE_DATA(&wgp_pivram[offset]);

	if (offset<0x3000)
	{
		if (oldword != wgp_pivram[offset])
			tilemap_mark_tile_dirty(wgp_piv_tilemap[(offset / 0x1000)], (offset % 0x1000) );
	}
	else if ((offset >=0x3400) && (offset<0x4000))
	{
		/* do nothing, custom draw routine takes care of raster effects */
	}
	else if ((offset >=0x8000) && (offset<0xb000))
	{
		if (oldword != wgp_pivram[offset])
			tilemap_mark_tile_dirty(wgp_piv_tilemap[((offset - 0x8000)/ 0x1000)], (offset % 0x1000) );
	}
}

READ16_HANDLER( wgp_piv_ctrl_word_r )
{
	return wgp_piv_ctrlram[offset];
}

WRITE16_HANDLER( wgp_piv_ctrl_word_w )
{
	UINT16 a,b;

	COMBINE_DATA(&wgp_piv_ctrlram[offset]);
	data = wgp_piv_ctrlram[offset];

	switch (offset)
	{
		case 0x00:
			a = -data;
			b = (a &0xffe0) >> 1;	/* kill bit 4 */
			wgp_piv_scrollx[0] = (a &0xf) | b;
			break;

		case 0x01:
			a = -data;
			b = (a &0xffe0) >> 1;
			wgp_piv_scrollx[1] = (a &0xf) | b;
			break;

		case 0x02:
			a = -data;
			b = (a &0xffe0) >> 1;
			wgp_piv_scrollx[2] = (a &0xf) | b;
			break;

		case 0x03:
			wgp_piv_scrolly[0] = data;
			break;

		case 0x04:
			wgp_piv_scrolly[1] = data;
			break;

		case 0x05:
			wgp_piv_scrolly[2] = data;
			break;

		case 0x06:
			/* Overall control reg (?)
               0x39  %00111001   normal
               0x2d  %00101101   piv2 layer goes under piv1
                     seen on Wgp stages 4,5,7 in which piv 2 used
                     for cloud or scenery wandering up screen */

			wgp_piv_ctrl_reg = data;
			break;

		case 0x08:
			/* piv 0 y zoom (0x7f = normal, not seen others) */
			wgp_piv_zoom[0] = data;
			break;

		case 0x09:
			/* piv 1 y zoom (0x7f = normal, values 0 &
                  0xff7f-ffbc in Wgp2) */
			wgp_piv_zoom[1] = data;
			break;

		case 0x0a:
			/* piv 2 y zoom (0x7f = normal, values 0 &
                  0xff7f-ffbc in Wgp2, 0-0x98 in Wgp round 4/5) */
			wgp_piv_zoom[2] = data;
			break;
	}
}




/****************************************************************
                     SPRITE DRAW ROUTINES

TODO
====

Implement rotation/zoom properly.

Sprite/piv priority: sprites always over?

Wgp round 1 had some junky brown mud bank sprites in-game.
They are indexed 0xe720-e790. 0x2720*4 => +0x9c80-9e80 in
the spritemap area. They should be 2x2 not 4x4 tiles. We
kludge this. Round 2 +0x9d40-9f40 contains the 2x2 sprites.
What REALLY controls number of tiles in a sprite?

Sprite colors: dust after crash in Wgp2 is odd; some
black/grey barrels on late Wgp circuit also look strange -
possibly the same wrong color.


Memory Map
----------

400000 - 40bfff : Sprite tile mapping area

    Tile numbers (0-0x3fff) alternate with word containing tile
    color/unknown bits. I'm _not_ 100% sure that only Wgp2 uses
    the unknown bits.

    xxxxxxxx x.......  unused ??
    ........ .x......  unknown (Wgp2 only: Taito tyre bridge on default course)
    ........ ..x.....  unknown (Wgp2 only)
    ........ ...x....  unknown (Wgp2 only: Direction signs just before hill # 1)
    ........ ....cccc  color (0-15)

    Tile map for each standard big sprite is 64 bytes (16 tiles).
    (standard big sprite comprises 4x4 16x16 tiles)

    Tile map for each small sprite only uses 16 of the 64 bytes.
      The remaining 48 bytes are garbage and should be ignored.
    (small sprite comprises 2x2 16x16 tiles)

40c000 - 40dbff : Sprite Table

    Every 16 bytes contains one sprite entry. First entry is
    ignored [as 0 in active sprites list means no sprite].

    (0x1c0 [no.of entries] * 0x40 [bytes for big sprite] = 0x6fff
    of sprite tile mapping area can be addressed at any one time.)

    Sprite entry     (preliminary)
    ------------

    +0x00  x pos (signed)
    +0x02  y pos (signed)
    +0x04  index to tile mapping area [2 msbs always set]

           (400000 + (index &0x3fff) << 2) points to relevant part of
           sprite tile mapping area. Index >0x2fff would be invalid.

    +0x06  zoom size (pixels) [typical range 0x1-5f, 0x3f = standard]
           Looked up from a logarithm table in the data rom indexed
           by the z coordinate. Max size prog allows before it blanks
           the sprite is 0x140.

    +0x08  incxx ?? (usually stuck at 0xf800)
    +0x0a  incyy ?? (usually stuck at 0xf800)

    +0x0c  z coordinate i.e. how far away the sprite is from you
           going into the screen. Max distance prog allows before it
           blanks the sprite is 0x3fff. 0x1400 is about the farthest
           away that the code creates sprites. 0x400 = standard
           distance corresponding to 0x3f zoom.  <0x400 = close to

    +0x0e  non-zero only during rotation.

    NB: +0x0c and +0x0e are paired. Equivalent of incyx and incxy ??

    (No longer used entries typically have 0xfff6 in +0x06 and +0x08.)

    Only 2 rotation examples (i) at 0x40c000 when Taito
    logo displayed (Wgp only). (ii) stage 5 (rain).
    Other in-game sprites are simply using +0x06 and +0x0c,

    So the sprite rotation in Wgp screenshots must be a *blanket*
    rotate effect, identical to the one applied to piv layers.
    This explains why sprite/piv positions are basically okay
    despite failure to implement rotation.

40dc00 - 40dfff: Active Sprites list

    Each word is a sprite number, 0x0 thru 0x1bf. If !=0
    a word makes active the 0x10 bytes of sprite data at
    (40c000 + sprite_num * 0x10). (Wgp2 fills this in reverse).

40fff0: Unknown (sprite control word?)

    Wgp alternates 0x8000 and 0. Wgp2 only pokes 0.
    Could this be some frame buffer control that would help to
    reduce the sprite timing glitches in Wgp?

****************************************************************/

/* Sprite tilemapping area doesn't have a straightforward
   structure for each big sprite: the hardware is probably
   constructing each 4x4 sprite from 4 2x2 sprites... */

static UINT8 xlookup[16] =
	{ 0, 1, 0, 1,
	  2, 3, 2, 3,
	  0, 1, 0, 1,
	  2, 3, 2, 3 };

static UINT8 ylookup[16] =
	{ 0, 0, 1, 1,
	  0, 0, 1, 1,
	  2, 2, 3, 3,
	  2, 2, 3, 3 };

static void wgp_draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect,int y_offs)
{
	int offs,i,j,k;
	int x,y,curx,cury;
	int zx,zy,zoomx,zoomy,priority=0;
	UINT8 small_sprite,col,flipx,flipy;
	UINT16 code,bigsprite,map_index;
	UINT16 rotate=0;
	UINT16 tile_mask = (Machine->gfx[0]->total_elements) - 1;
	static const int primasks[2] = {0x0, 0xfffc};	/* fff0 => under rhs of road only */

	for (offs = 0x1ff;offs >= 0;offs--)
	{
		code = (spriteram16[0xe00+offs]);

		if (code)	/* do we have an active sprite ? */
		{
			i = (code << 3) &0xfff;	/* yes, so we look up its sprite entry */

			x = spriteram16[i];
			y = spriteram16[i + 1];
			bigsprite = spriteram16[i + 2] &0x3fff;

			/* The last five words [i + 3 thru 7] must be zoom/rotation
               control: for time being we kludge zoom using 1 word.
               Timing problems are causing many glitches. */

if ((spriteram16[i + 4]==0xfff6) && (spriteram16[i + 5]==0))
	continue;

if (((spriteram16[i + 4]!=0xf800) && (spriteram16[i + 4]!=0xfff6))
	|| ((spriteram16[i + 5]!=0xf800) && (spriteram16[i + 5]!=0))
	|| spriteram16[i + 7]!=0)

	rotate = i << 1;

	/***** Begin zoom kludge ******/

			zoomx = (spriteram16[i + 3] &0x1ff) + 1;
			zoomy = (spriteram16[i + 3] &0x1ff) + 1;

			y -=4;
	// distant sprites were some 16 pixels too far down //
			y -=((0x40-zoomy)/4);

	/****** end zoom kludge *******/

			/* Treat coords as signed */
			if (x & 0x8000)  x -= 0x10000;
			if (y & 0x8000)  y -= 0x10000;

			map_index = bigsprite << 1;	/* now we access sprite tilemap */

			/* don't know what selects 2x2 sprites: we use a nasty kludge
               which seems to work */

			i = wgp_spritemap[map_index + 0xa];
			j = wgp_spritemap[map_index + 0xc];
			small_sprite = ((i > 0) & (i <=8) & (j > 0) & (j <=8));

			if (small_sprite)
			{
				
				{ 
				struct drawgfxParams dgpz0={
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
					0x00010000, 	// scalex
					0x00010000, 	// scaley
					priority_bitmap, 	// pri_buffer
					primasks[((priority >> 1) &1)] 	// priority_mask
				  };
				for (i=0;i<4;i++)
				{
					code = wgp_spritemap[(map_index + (i << 1))] &tile_mask;
					col  = wgp_spritemap[(map_index + (i << 1) + 1)] &0xf;

					/* not known what controls priority */
					priority = (wgp_spritemap[(map_index + (i << 1) + 1)] &0x70) >> 4;

					flipx=0;	// no flip xy?
					flipy=0;

					k = xlookup[i];	// assumes no xflip
					j = ylookup[i];	// assumes no yflip

					curx = x + ((k*zoomx)/2);
					cury = y + ((j*zoomy)/2);

					zx = x + (((k+1)*zoomx)/2) - curx;
					zy = y + (((j+1)*zoomy)/2) - cury;

					
					dgpz0.code = code;
					dgpz0.color = col;
					dgpz0.flipx = flipx;
					dgpz0.flipy = flipy;
					dgpz0.sx = curx;
					dgpz0.sy = cury;
					dgpz0.scalex = zx << 12;
					dgpz0.scaley = zy << 12;
					drawgfxzoom(&dgpz0);	/* maybe >> 2 or 0...? */
				}
				} // end of patch paragraph

			}
			else
			{
				
				{ 
				struct drawgfxParams dgpz1={
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
					0x00010000, 	// scalex
					0x00010000, 	// scaley
					priority_bitmap, 	// pri_buffer
					primasks[((priority >> 1) &1)] 	// priority_mask
				  };
				for (i=0;i<16;i++)
				{
					code = wgp_spritemap[(map_index + (i << 1))] &tile_mask;
					col  = wgp_spritemap[(map_index + (i << 1) + 1)] &0xf;

					/* not known what controls priority */
					priority = (wgp_spritemap[(map_index + (i << 1) + 1)] &0x70) >> 4;

					flipx=0;	// no flip xy?
					flipy=0;

					k = xlookup[i];	// assumes no xflip
					j = ylookup[i];	// assumes no yflip

					curx = x + ((k*zoomx)/4);
					cury = y + ((j*zoomy)/4);

					zx = x + (((k+1)*zoomx)/4) - curx;
					zy = y + (((j+1)*zoomy)/4) - cury;

					
					dgpz1.code = code;
					dgpz1.color = col;
					dgpz1.flipx = flipx;
					dgpz1.flipy = flipy;
					dgpz1.sx = curx;
					dgpz1.sy = cury;
					dgpz1.scalex = zx << 12;
					dgpz1.scaley = zy << 12;
					drawgfxzoom(&dgpz1);	/* maybe >> 2 or 0...? */
				}
				} // end of patch paragraph

			}
		}

	}
#if 0
	if (rotate)
	{
		char buf[80];
		sprintf(buf,"sprite rotate offs %04x ?",rotate);
		ui_popup(buf);
	}
#endif
}


/*********************************************************
                       CUSTOM DRAW
*********************************************************/

/* These scanline drawing routines lifted from Taito F3: optimise / merge ? */

#undef ADJUST_FOR_ORIENTATION
#define ADJUST_FOR_ORIENTATION(type, orientation, bitmapi, bitmapp, x, y)	\
	type *dsti = &((type *)bitmapi->line[y])[x];							\
	UINT8 *dstp = &((UINT8 *)bitmapp->line[y])[x];							\
	int xadv = 1;															\
	if (orientation)														\
	{																		\
		int dy = (type *)bitmap->line[1] - (type *)bitmap->line[0];			\
		int tx = x, ty = y;													\
		if ((orientation) & ORIENTATION_FLIP_X)								\
			tx = bitmap->width - 1 - tx;									\
		if ((orientation) & ORIENTATION_FLIP_Y)								\
			ty = bitmap->height - 1 - ty;									\
		/* can't lookup line because it may be negative! */					\
		dsti = (type *)((type *)bitmapi->line[0] + dy * ty) + tx;			\
		dstp = (UINT8 *)((UINT8 *)bitmapp->line[0] + dy * ty / sizeof(type)) + tx;	\
	}

INLINE void bryan2_drawscanline(
		mame_bitmap *bitmap,int x,int y,int length,
		const UINT16 *src,int transparent,UINT32 orient,int pri)
{
	ADJUST_FOR_ORIENTATION(UINT16, orient, bitmap, priority_bitmap, x, y);
	if (transparent) {
		while (length--) {
			UINT32 spixel = *src++;
			if (spixel<0x7fff) {
				*dsti = spixel;
				*dstp = pri;
			}
			dsti += xadv;
			dstp += xadv;
		}
	} else { /* Not transparent case */
		while (length--) {
			*dsti = *src++;
			*dstp = pri;
			dsti += xadv;
			dstp += xadv;
		}
	}
}
#undef ADJUST_FOR_ORIENTATION



static void wgp_piv_layer_draw(mame_bitmap *bitmap,const rectangle *cliprect,int layer,int flags,UINT32 priority)
{
	mame_bitmap *srcbitmap = tilemap_get_pixmap(wgp_piv_tilemap[layer]);
	mame_bitmap *transbitmap = tilemap_get_transparency_bitmap(wgp_piv_tilemap[layer]);

	UINT16 *dst16,*src16;
	UINT8 *tsrc;
	int i,y,y_index,src_y_index,row_index,row_zoom;

	/* I have a fairly strong feeling these should be UINT32's, x_index is
       falling through from max +ve to max -ve quite a lot in this routine */
	int sx,x_index,x_step,x_max;

	UINT32 zoomx,zoomy;
	UINT16 scanline[512];
	UINT16 row_colbank,row_scroll;
	int flipscreen = 0;	/* n/a */
	int machine_flip = 0;	/* for  ROT 180 ? */

	UINT16 screen_width = cliprect->max_x -
							cliprect->min_x + 1;
	UINT16 min_y = cliprect->min_y;
	UINT16 max_y = cliprect->max_y;

	int width_mask=0x3ff;

	zoomx = 0x10000;	/* No overall X zoom, unlike TC0480SCP */

	/* Y-axis zoom offers expansion/compression: 0x7f = no zoom, 0xff = max ???
       In WGP see:  stage 4 (big spectator stand)
                    stage 5 (cloud layer)
                    stage 7 (two bits of background scenery)
                    stage 8 (unknown - surely something should be appearing here...)
       In WGP2 see: road at big hill (default course) */

	/* This calculation may be wrong, the y_index one too */
	zoomy = 0x10000 - (((wgp_piv_ctrlram[0x08 + layer] &0xff) - 0x7f) * 512);

	if (!flipscreen)
	{
		sx = ((wgp_piv_scrollx[layer]) << 16);
		sx += (wgp_piv_xoffs) * zoomx;		/* may be imperfect */

		y_index = (wgp_piv_scrolly[layer] << 16);
		y_index += (wgp_piv_yoffs + min_y) * zoomy;		/* may be imperfect */
	}
	else	/* piv tiles flipscreen n/a */
	{
		sx = 0;
		y_index = 0;
	}

	if (!machine_flip) y=min_y; else y=max_y;

	do
	{
		int a;

		src_y_index = (y_index>>16) &0x3ff;
		row_index = src_y_index;

		row_zoom = wgp_pivram[row_index + layer*0x400 + 0x3400] &0xff;

		row_colbank = wgp_pivram[row_index + layer*0x400 + 0x3400] >> 8;
		a = (row_colbank &0xe0);	/* kill bit 4 */
		row_colbank = (((row_colbank &0xf) << 1) | a) << 4;

		row_scroll = wgp_pivram[row_index + layer*0x1000 + 0x4000];
		a = (row_scroll &0xffe0) >> 1;	/* kill bit 4 */
		row_scroll = ((row_scroll &0xf) | a) &width_mask;

		x_index = sx - (row_scroll << 16);

		x_step = zoomx;
		if (row_zoom > 0x7f)	/* zoom in: reduce x_step */
		{
			x_step -= (((row_zoom - 0x7f) << 8) &0xffff);
		}
		else if (row_zoom < 0x7f)	/* zoom out: increase x_step */
		{
			x_step += (((0x7f - row_zoom) << 8) &0xffff);
		}

		x_max = x_index + screen_width * x_step;
		src16 = (UINT16 *)srcbitmap->line[src_y_index];
		tsrc  = (UINT8 *)transbitmap->line[src_y_index];
		dst16 = scanline;

		if (flags & TILEMAP_IGNORE_TRANSPARENCY)
		{
			for (i=0; i<screen_width; i++)
			{
				*dst16++ = src16[(x_index >> 16) &width_mask] + row_colbank;
				x_index += x_step;
			}
		}
		else
		{
			for (i=0; i<screen_width; i++)
			{
				if (tsrc[(x_index >> 16) &width_mask])
					*dst16++ = src16[(x_index >> 16) &width_mask] + row_colbank;
				else
					*dst16++ = 0x8000;
				x_index += x_step;
			}
		}

		if (flags & TILEMAP_IGNORE_TRANSPARENCY)
			bryan2_drawscanline(bitmap,0,y,screen_width,scanline,0,ROT0,priority);
		else
			bryan2_drawscanline(bitmap,0,y,screen_width,scanline,1,ROT0,priority);

		y_index += zoomy;
		if (!machine_flip) y++; else y--;
	}
	while ( (!machine_flip && y<=max_y) || (machine_flip && y>=min_y) );

}



/**************************************************************
                        SCREEN REFRESH
**************************************************************/

VIDEO_UPDATE( wgp )
{
	int i;
	UINT8 layer[3];

#ifdef MAME_DEBUG
	static UINT8 dislayer[4];
#endif

#ifdef MAME_DEBUG
	if (code_pressed_memory (KEYCODE_V))
	{
		dislayer[0] ^= 1;
		ui_popup("piv0: %01x",dislayer[0]);
	}

	if (code_pressed_memory (KEYCODE_B))
	{
		dislayer[1] ^= 1;
		ui_popup("piv1: %01x",dislayer[1]);
	}

	if (code_pressed_memory (KEYCODE_N))
	{
		dislayer[2] ^= 1;
		ui_popup("piv2: %01x",dislayer[2]);
	}

	if (code_pressed_memory (KEYCODE_M))
	{
		dislayer[3] ^= 1;
		ui_popup("TC0100SCN top bg layer: %01x",dislayer[3]);
	}
#endif

	for (i=0;i<3;i++)
	{
		tilemap_set_scrollx(wgp_piv_tilemap[i], 0, wgp_piv_scrollx[i]);
		tilemap_set_scrolly(wgp_piv_tilemap[i], 0, wgp_piv_scrolly[i]);
	}

	TC0100SCN_tilemap_update();

	fillbitmap(bitmap, Machine->pens[0], cliprect);

	layer[0] = 0;
	layer[1] = 1;
	layer[2] = 2;

	if (wgp_piv_ctrl_reg == 0x2d)
	{
		layer[1] = 2;
		layer[2] = 1;
	}

/* We should draw the following on a 1024x1024 bitmap... */

#ifdef MAME_DEBUG
	if (dislayer[layer[0]]==0)
#endif
	wgp_piv_layer_draw(bitmap,cliprect,layer[0],TILEMAP_IGNORE_TRANSPARENCY,1);

#ifdef MAME_DEBUG
	if (dislayer[layer[1]]==0)
#endif
	wgp_piv_layer_draw(bitmap,cliprect,layer[1],0,2);

#ifdef MAME_DEBUG
	if (dislayer[layer[2]]==0)
#endif
	wgp_piv_layer_draw(bitmap,cliprect,layer[2],0,4);

	wgp_draw_sprites(bitmap,cliprect,16);

/* ... then here we should apply rotation from wgp_sate_ctrl[] to
   the bitmap before we draw the TC0100SCN layers on it */

	layer[0] = TC0100SCN_bottomlayer(0);
	layer[1] = layer[0]^1;
	layer[2] = 2;

	TC0100SCN_tilemap_draw(bitmap,cliprect,0,layer[0],0,0);

#ifdef MAME_DEBUG
	if (dislayer[3]==0)
#endif
	TC0100SCN_tilemap_draw(bitmap,cliprect,0,layer[1],0,0);
	TC0100SCN_tilemap_draw(bitmap,cliprect,0,layer[2],0,0);

#if 0
	{
		char buf[80];
		sprintf(buf,"wgp_piv_ctrl_reg: %04x y zoom: %04x %04x %04x",wgp_piv_ctrl_reg,
						wgp_piv_zoom[0],wgp_piv_zoom[1],wgp_piv_zoom[2]);
		ui_popup(buf);
	}
#endif

/* Enable this to watch the rotation control words */
#if 0
	{
		char buf[80];
		int i;

		for (i = 0; i < 8; i += 1)
		{
			sprintf (buf, "%02x: %04x", i, wgp_rotate_ctrl[i]);
			ui_draw_text (buf, 0, i*8);
		}
	}
#endif
}

