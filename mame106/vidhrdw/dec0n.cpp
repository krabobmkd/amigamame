// dec0n.cpp
extern "C" {
    #include "mame.h"
    #include "driver.h"
    #include "includes/dec0.h"
    #include "tilemap.h"
    void custom_tilemap_draw(mame_bitmap *bitmap,
								const rectangle *cliprect,
								tilemap *tilemap_ptr,
								const UINT16 *rowscroll_ptr,
								const UINT16 *colscroll_ptr,
								const UINT16 *control0,
								const UINT16 *control1,
								int flags);
}

typedef struct {
    tilemap *tilemap_ptr;
    mame_bitmap *bitmap;
	const rectangle *cliprect;
    const mame_bitmap *src_bitmap;
    const UINT16 *rowscroll_ptr;
    const UINT16 *colscroll_ptr;
    const UINT16 *control1;
    const UINT16 columnOffsetConstant;
    const UINT16 rowOffsetConstant;
} custileparams;

template<int flipscreen, int ignoretransp,int mapfront,int row_scroll_enabled,int col_scroll_enabled>
void custom_tilemap_drawT( custileparams *params)
{

	int src_y=0;
	UINT16 scrollx=params->control1[0];
	UINT16 scrolly=params->control1[1];
	int width_mask = params->src_bitmap->width - 1;
	int height_mask = params->src_bitmap->height - 1;

	/* Column scroll & row scroll may per applied per pixel, there are
    shift registers for each which control the granularity of the row/col
    offset (down to per line level for row, and per 8 lines for column).

    Nb:  The row & col selectors are _not_ affected by the shape of the
    playfield (ie, 256*1024, 512*512 or 1024*256).  So even if the tilemap
    width is only 256, 'src_x' should not wrap at 256 in the code below (to
    do so would mean the top half of row RAM would never be accessed which
    is incorrect).

    Nb2:  Real hardware exhibits a strange bug with column scroll on 'mode 2'
    (256*1024) - the first column has a strange additional offset, but
    curiously the first 'wrap' (at scroll offset 256) does not have this offset,
    it is displayed as expected.  The bug is confimed to only affect this mode,
    the other two modes work as expected.  This bug is not emulated, as it
    doesn't affect any games.
    */

    int yscrollf = params->control1[3]&0xf;
    int xscrollf = params->control1[2]&0xf;

    UINT16 xscrollfmask =   (0x003f>>xscrollf);
    xscrollf +=3; // because does ((src_x >> 3) >> xscrollf)

    const UINT16 *rowscroll_ptr = params->rowscroll_ptr;
    const UINT16 *colscroll_ptr = params->colscroll_ptr;

    const UINT16 **srclines = (const UINT16 **)params->src_bitmap->line;

    UINT16 **destlines = (UINT16 **)params->bitmap->line;

    INT16 y,maxx= (INT16)params->cliprect->max_x, maxy = (INT16)params->cliprect->max_y;
    INT16 srcbmwidth = params->src_bitmap->width;

	if (flipscreen)
		src_y = ((params->src_bitmap->height - 256) - scrolly) + params->columnOffsetConstant ; //krb: not sure this one
	else
		src_y = scrolly+ params->columnOffsetConstant;

	for (y=0; y<=maxy; y++) {
    	INT16 src_x;
		if (row_scroll_enabled)
			src_x=scrollx + rowscroll_ptr[(src_y >> yscrollf) & (0x1ff>>yscrollf)];
		else
			src_x=scrollx;

		if (flipscreen)
			src_x=(srcbmwidth - 256) - src_x;

        UINT16 *pline = destlines[y];

        const UINT16 *line2 = srclines[(src_y ) & height_mask];

		for (INT16 x=0; x<=maxx; x++ , src_x++ ) {

            UINT16  p;
			if (col_scroll_enabled)
			{
				int column_offset=colscroll_ptr[(src_x >> xscrollf) & xscrollfmask ];
				p = (srclines[(src_y + column_offset)&height_mask])[src_x&width_mask];
            } else
            {
                p = line2[src_x&width_mask];
            }

			if (ignoretransp || (p&0xf))
			{
				if(mapfront)
				{
					/* Top 8 pens of top 8 palettes only */
                    if ((p&0x88)==0x88) pline[x] = p; // Machine->pens[p];
				}
				else
				{

                    pline[x] = p; // Machine->pens[p];
				}
			}
		}
		src_y++;
	}
}

// when simple x/y scroll offsets are applied with same values everywhere,
// we can go further in tile optimisation.
// note this is the case of 100% rendering cases for baddudes.
template<int flipscreen, int ignoretransp,int mapfront>
void custom_tilemap_draw_noscrambleT( custileparams *params)
{
	int src_y=0;
	UINT16 scrollx=params->control1[0];
	UINT16 scrolly=params->control1[1];
	int width_mask = params->src_bitmap->width - 1;
	int height_mask = params->src_bitmap->height - 1;

    const UINT16 **srclines = (const UINT16 **)params->src_bitmap->line;

    UINT16 **destlines = (UINT16 **)params->bitmap->line;

    INT16 maxx= (INT16)params->cliprect->max_x, maxy = (INT16)params->cliprect->max_y +1;
    INT16 srcbmwidth = params->src_bitmap->width;

	if (flipscreen)
		src_y = ((params->src_bitmap->height - 256) - scrolly) + params->columnOffsetConstant ; //krb: not sure this one
	else
		src_y = scrolly+ params->columnOffsetConstant;


    INT16 src_x=scrollx;

    if (flipscreen)
        src_x=(srcbmwidth - 256) - src_x;


    UINT16 tilew,nbtilesw,nbtilesh;
    tilemap_get_tile_size(params->tilemap_ptr,&tilew,NULL,&nbtilesw,&nbtilesh);

    // come on, it's either 8x8 for pf1 or 16x16 for pf2 pf3.
    const UINT8 tiledivbits = (tilew == 16)?4:3; // it's 8 or 16.
    const UINT8 tilemask = tilew-1; // it's 8 or 16.
    const UINT16 tilelxmask = (nbtilesw-1)<<tiledivbits; // 31 or 63.
    const UINT16 tilelymask = (nbtilesh-1)<<tiledivbits; // 31 or 63.

    // tiles in the tilemap index
    UINT16 ytile =( src_y & tilelymask);
    UINT16 ylasttile =( (src_y+maxy) & tilelymask);

    src_x &=width_mask;

    UINT16 xfirsttile = src_x /*& tilelxmask*/ ;
    UINT16 xlasttile = (src_x+256) & tilelxmask ;

    INT16 yscreen = -(src_y & tilemask);
   // INT16 xscreenstartx1 = -(src_x & tilemask); always 0 after clip
    const INT16 xscreenstartx2 = (-(src_x & tilemask) + tilew) ;// [1,16]

    const INT16 destmodulo = params->bitmap->rowpixels;
    const INT16 srcmodulo = params->src_bitmap->rowpixels;

    mame_bitmap *ptransbm = tilemap_get_transparency_bitmap( params->tilemap_ptr );

    UINT8 *ptranspdata =  tilemap_get_transparency_data(  params->tilemap_ptr  );

    // 1024 + 32
    // cached_indx = row*tmap->num_cached_cols + c1;
    // if( (tmap->transparency_data[cached_indx]&mask)!=0 )

     // - - - -  y  loop
    while(1) // different because <> wont work with rounding.
    {
        // general Y clip for this row
        INT16 ys1=yscreen;
        INT16 ys2=ys1+tilew;
        INT16 yt1=ytile;
        INT16 yt2=ytile+tilew;
        if(ys1<0) {
            yt1 += -ys1;
            ys1=0;
        }
        if(ys2>maxy) {
            yt2 -= ys2-maxy;
            ys2=maxy;
        }

        // - - - -  x row loop
        UINT16 *plinedest_t = destlines[ys1];

        INT16 inx1 = 0;
        INT16 inx2 = xscreenstartx2;

        INT16 xtile  = xfirsttile;

        UINT32 nbdec = (((yt1 & height_mask)>>tiledivbits)*nbtilesw);
//                           (((src_x+inx1) & width_mask)>>tiledivbits);

        UINT8 *ptranspdata_row = ptranspdata +nbdec;

        // let's say 0 fully trans
        // 1 transp , 2 opaque
        //UINT8 transprev=0,transcur; // strange ways of tilemap.c
        while(1)
        {          
            UINT16 *plinedest_tt = plinedest_t + inx1;
            const UINT16 *plinesrc_tt = srclines[yt1 & height_mask]+ ((src_x+inx1) & width_mask);
            INT16 x2 = inx2;
            if(x2>256) x2=256;
            // - - - - - y tile loop
            // todo test if tile transparent, opaque , or per pixel
            UINT8 transdata = ptranspdata_row[
                        (((src_x+inx1) & width_mask)>>tiledivbits)
                        ] & 16; // 0 or 16, low bits are prio tile mask, sometimes.
            if(!transdata )
            {
                // actually means either wholly trans or wholly opaque
                // horribly, that information is in the transparency bitmap :(
                // or is it because it allows to switch render type by line ? ->no.
                // this is meat to be an tilemap.c internal problem.
                // may have been a choice over what bit left to use.
                const UINT8 *ptrbm = ((const UINT8 *)
                        ptransbm->line[yt1 & height_mask])+ ((src_x+inx1) & width_mask);
                transdata = (*ptrbm & 16)<<1; // would do 0,16,32
            }
            if(ignoretransp || transdata ==32)
            {
                // fully opaque case ->copy
                for(INT16 y=ys1;y<ys2;y++)
                {
                    UINT16 *plinedest_ttt = plinedest_tt ;
                    const UINT16 *plinesrc_ttt = plinesrc_tt;
                    INT16 x1 = inx1;
                    while(x1<x2)
                    {
                        *plinedest_ttt++ = *plinesrc_ttt++;
                        x1++;
                    } // end loop inside tile x

                    plinesrc_tt += srcmodulo; // NO
                    plinedest_tt += destmodulo;

                } // end loop inside tile y
            } else if(transdata)
            {
                // some pixels -> tests
                for(INT16 y=ys1;y<ys2;y++)
                {
                    UINT16 *plinedest_ttt = plinedest_tt ;
                    const UINT16 *plinesrc_ttt = plinesrc_tt;
                    INT16 x1 = inx1;
                    while(x1<x2)
                    {
                        UINT16 p = *plinesrc_ttt++;
                        if(mapfront) // template escaped test
                        {
                            /* Top 8 pens of top 8 palettes only */
                            if ((p&0x88)==0x88) *plinedest_ttt = p; // Machine->pens[p];
                        }
                        else
                        {
                            if(p & 0x0f)
                                *plinedest_ttt = p;
                        }
                        plinedest_ttt++;
                        x1++;
                    } // end loop inside tile x

                    plinesrc_tt += srcmodulo; // NO
                    plinedest_tt += destmodulo;
                } // end loop inside tile y
            }

            if(xtile==xlasttile) break;
            // - - - -  x row loop adds
            xtile +=tilew;
            xtile &= tilelxmask;

            inx1 = inx2;
            inx2 += tilew;
        }// end xloop by tile

        if(ytile==ylasttile) break;
        yscreen += tilew;

        ytile += tilew;
        ytile &= tilelymask;

    } // end yloop by tile

}



void custom_tilemap_draw(mame_bitmap *bitmap,
								const rectangle *cliprect,
								tilemap *tilemap_ptr,
								const UINT16 *rowscroll_ptr,
								const UINT16 *colscroll_ptr,
								const UINT16 *control0,
								const UINT16 *control1,
								int flags)
{
    // note: tilemap_get_pixmap() actually DOES the tilemap rendering or update.
	const mame_bitmap *src_bitmap = tilemap_get_pixmap(tilemap_ptr);
	if (!src_bitmap)
		return;

    // test column offset per column, in most case we can use a constant and do less tiling.
    int doColumnOffset = ((colscroll_ptr && (control0[0]&0x8))!=0);
    UINT16 columnOffsetConstant=0;
    if(doColumnOffset)
    {   // we bet just a single y constant would be ok...
        int columnOffsetIsConstant=1;        
        int xscrollf = control1[2]&0xf;
        UINT16 xscrollfmask = (0x003f>>xscrollf);
        columnOffsetConstant=colscroll_ptr[0];
        for(UINT16 x=1;x<xscrollfmask;x++)
        {
            int next_column_offset=colscroll_ptr[x];
            if(next_column_offset != columnOffsetConstant) {columnOffsetIsConstant=0;break;}
        }
        if(columnOffsetIsConstant) doColumnOffset=0; // finnaly, just apply constant
    }

    int doRowOffset = ((rowscroll_ptr && (control0[0]&0x4)) !=0);
    UINT16 rowOffsetConstant=0;
    if(doRowOffset)
    {    // we bet just a single y constant would be ok...
        int rowOffsetIsConstant=1;
        int  yscrollf = control1[3]&0xf;
        UINT16 yscrollfmask = (0x1ff>>yscrollf);
        rowOffsetConstant=rowscroll_ptr[0];
        for(UINT16 y=1;y<yscrollfmask;y++)
        {
            int next_column_offset=rowscroll_ptr[y];
            if(next_column_offset != columnOffsetConstant) { rowOffsetIsConstant=0; break; }
        }
        if(rowOffsetIsConstant) doRowOffset=0; // finnaly, just apply constant
    }


    // 5 booleans will gives 32 possible routines
    int rendertypeid = (flip_screen !=0) |
                    ((flags&TILEMAP_IGNORE_TRANSPARENCY)>>3) |
                     ( (flags&TILEMAP_FRONT)>>4) |
                     ((doRowOffset)<<3) |
                    ((doColumnOffset)<<4);

    custileparams params = {
        tilemap_ptr,
        bitmap,cliprect,src_bitmap,rowscroll_ptr,colscroll_ptr,control1,
        columnOffsetConstant,rowOffsetConstant
    };

    switch(rendertypeid)
    {
        // no vert or horiz parallax means we can apply tile optimisations.
        case 0:  custom_tilemap_draw_noscrambleT<0,0,0>(&params); break;
        case 1:  custom_tilemap_draw_noscrambleT<1,0,0>(&params); break;
        case 2:  custom_tilemap_drawT<0,1,0,0,0>(&params); break; // ignore transp does speedcopy
        case 3:  custom_tilemap_drawT<1,1,0,0,0>(&params); break;
        case 4:  custom_tilemap_draw_noscrambleT<0,0,1>(&params); break;
        case 5:  custom_tilemap_draw_noscrambleT<1,0,1>(&params); break;
        case 6:  custom_tilemap_draw_noscrambleT<0,1,1>(&params); break;
        case 7:  custom_tilemap_draw_noscrambleT<1,1,1>(&params); break;

        case 8:  custom_tilemap_drawT<0,0,0,1,0>(&params); break;
        case 9:  custom_tilemap_drawT<1,0,0,1,0>(&params); break;
        case 10:  custom_tilemap_drawT<0,1,0,1,0>(&params); break;
        case 11:  custom_tilemap_drawT<1,1,0,1,0>(&params); break;
        case 12:  custom_tilemap_drawT<0,0,1,1,0>(&params); break;
        case 13:  custom_tilemap_drawT<1,0,1,1,0>(&params); break;
        case 14:  custom_tilemap_drawT<0,1,1,1,0>(&params); break;
        case 15:  custom_tilemap_drawT<1,1,1,1,0>(&params); break;

        case 16:  custom_tilemap_drawT<0,0,0,0,1>(&params); break;
        case 17:  custom_tilemap_drawT<1,0,0,0,1>(&params); break;
        case 18:  custom_tilemap_drawT<0,1,0,0,1>(&params); break;
        case 19:  custom_tilemap_drawT<1,1,0,0,1>(&params); break;
        case 20:  custom_tilemap_drawT<0,0,1,0,1>(&params); break;
        case 21:  custom_tilemap_drawT<1,0,1,0,1>(&params); break;
        case 22:  custom_tilemap_drawT<0,1,1,0,1>(&params); break;
        case 23:  custom_tilemap_drawT<1,1,1,0,1>(&params); break;

        case 24:  custom_tilemap_drawT<0,0,0,1,1>(&params); break;
        case 25:  custom_tilemap_drawT<1,0,0,1,1>(&params); break;
        case 26:  custom_tilemap_drawT<0,1,0,1,1>(&params); break;
        case 27:  custom_tilemap_drawT<1,1,0,1,1>(&params); break;
        case 28:  custom_tilemap_drawT<0,0,1,1,1>(&params); break;
        case 29:  custom_tilemap_drawT<1,0,1,1,1>(&params); break;
        case 30:  custom_tilemap_drawT<0,1,1,1,1>(&params); break;
        case 31:  custom_tilemap_drawT<1,1,1,1,1>(&params); break;

    }

}
