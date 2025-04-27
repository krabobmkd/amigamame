
#include "taitoicn.h"
extern "C"
{
    #include "driver.h"

    // from taitoic.c
    #define TC0100SCN_RAM_SIZE 0x14000	/* enough for double-width tilemaps */
    #define TC0100SCN_TOTAL_CHARS 256
    #define TC0100SCN_MAX_CHIPS 3

    extern UINT16 TC0100SCN_ctrl[TC0100SCN_MAX_CHIPS][8];

    extern UINT16 *TC0100SCN_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_bg_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_fg_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_tx_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_char_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_bgscroll_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_fgscroll_ram[TC0100SCN_MAX_CHIPS],
				*TC0100SCN_colscroll_ram[TC0100SCN_MAX_CHIPS];

    extern  int TC0100SCN_bgscrollx[TC0100SCN_MAX_CHIPS],TC0100SCN_bgscrolly[TC0100SCN_MAX_CHIPS],
		TC0100SCN_fgscrollx[TC0100SCN_MAX_CHIPS],TC0100SCN_fgscrolly[TC0100SCN_MAX_CHIPS];

#define PC080SN_RAM_SIZE 0x10000
#define PC080SN_MAX_CHIPS 2
    extern tilemap *PC080SN_tilemap[PC080SN_MAX_CHIPS][2];
    extern int PC080SN_bg_gfx[PC080SN_MAX_CHIPS];
    extern int PC080SN_yinvert,PC080SN_dblwidth;


    extern int PC080SN_bgscrollx[PC080SN_MAX_CHIPS][2],PC080SN_bgscrolly[PC080SN_MAX_CHIPS][2];
    extern int PC080SN_xoffs,PC080SN_yoffs;

    extern UINT16 *PC080SN_ram[PC080SN_MAX_CHIPS],
				*PC080SN_bg_ram[PC080SN_MAX_CHIPS][2],
				*PC080SN_bgscroll_ram[PC080SN_MAX_CHIPS][2];
}


template<int ignoretr,int hasprio>
void Taitoic_TC0100SCN_tilemap_draw_fgT(mame_bitmap *bitmap,const rectangle *cliprect, int chip, tilemap* tmap ,int flags, UINT32 priority)
{
	const mame_bitmap *src_bitmap = tilemap_get_pixmap(tmap);
	int width_mask, height_mask, x, y;
	int column_offset, src_x=0, src_y=0;
	int scrollx_delta = - tilemap_get_scrolldx( tmap );
	int scrolly_delta = - tilemap_get_scrolldy( tmap );

	width_mask=(src_bitmap->width - 1) &0x3ff;
	height_mask=src_bitmap->height - 1;

	src_y=(TC0100SCN_fgscrolly[chip] + scrolly_delta)&height_mask;
	if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
		src_y=256-src_y;

	//We use cliprect->max_y and cliprect->max_x to support games which use more than 1 screen
    UINT16 *ColumnTable = &(TC0100SCN_colscroll_ram[chip][0]);
	// Row offsets are 'screen space' 0-255 regardless of Y scroll
	for (y=0; y<=cliprect->max_y; y++) {
		src_x=(TC0100SCN_fgscrollx[chip] - TC0100SCN_fgscroll_ram[chip][(y + scrolly_delta)&0x1ff] + scrollx_delta + cliprect->min_x)&width_mask;
		if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
			src_x=256 - 64 - src_x;

        UINT16 *pbm = (UINT16 *)bitmap->line[y];
        pbm += cliprect->min_x;
        UINT8 *pri=NULL;
        if(hasprio)
        {   pri = (UINT8 *) priority_bitmap->line[y];
            pri += cliprect->min_x;
        }
		// Col offsets are 'tilemap' space 0-511, and apply to blocks of 8 pixels at once
		for (x=0; x<=(cliprect->max_x - cliprect->min_x); x++) {

            src_x&= width_mask;
			column_offset=ColumnTable[src_x>>3];

			UINT16 p=((UINT16*)src_bitmap->line[(src_y - column_offset)&height_mask])[src_x];
            //  p is always 16c palette with 0 transparent, + a palette offset.
            // so need &0x000f to test transparency.
			if ((p&0x000f)!=0 || ignoretr)
			{
				//plot_pixel(bitmap, x + cliprect->min_x, y, Machine->pens[p]);
				if (hasprio)
				{
					// pri[x] |= priority;
					pri[x] = priority;
				}
				*pbm = p; // Machine->pens[p];
			}
			pbm++;
			//src_x=(src_x+1)&width_mask;
			src_x++;
		}
		src_y=(src_y+1)&height_mask;
	}
}



// when simple x/y scroll offsets are applied with same values everywhere,
// we can go further in tile optimisation.
// note this is the case of 100% rendering cases for baddudes.
//template<int flipscreen, int ignoretransp,int mapfront>

// try to adapt heavy dec0 solution:
 template<int ignoretransp,int hasprio>
 void Taitoic_TC0100SCN_tilemap_draw_fgNOCOLUMNOFS_T(mame_bitmap *bitmap,const rectangle *cliprect, int chip, tilemap* tmap ,int flags, UINT32 priority)
//template<int flipscreen, int ignoretransp,int mapfront>
//void custom_tilemap_draw_noscrambleT( custileparams *params)
{
    /*
        Note there is a very particular use of cliprect min_x/min_y
        used to switch multiple screen (ninja warrior 3 screens, darius 2)
        this is in no way what cliprect is meant to do in other drivers.
    */

	const mame_bitmap *src_bitmap = tilemap_get_pixmap(tmap);
	// int width_mask, height_mask, x, y;
	// int column_offset, src_x=0, src_y=0;
    int scrollx =
                TC0100SCN_fgscrollx[chip]
                - tilemap_get_scrolldx( tmap ) /*+ cliprect->min_x*/; //
	int scrolly =
                TC0100SCN_fgscrolly[chip]
             - tilemap_get_scrolldy( tmap ) + cliprect->min_y ;
/*
		src_x=(TC0100SCN_fgscrollx[chip]
		- TC0100SCN_fgscroll_ram[chip][(y + scrolly_delta)&0x1ff]
		+ scrollx_delta + cliprect->min_x)&width_mask;

*/

	if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
		scrolly=256-scrolly;

    if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
        scrollx=256 - 64 - scrollx;

/*
  src_x=(
        TC0100SCN_fgscrollx[chip]
        - TC0100SCN_fgscroll_ram[chip][(y + scrolly_delta)&0x1ff]
        + scrollx_delta + cliprect->min_x
        )&width_mask;

*/
	// width_mask=(src_bitmap->width - 1) &0x3ff;
	// height_mask=src_bitmap->height - 1;

    // - - - - -  -

	int src_y=0;
	// UINT16 scrollx=params->control1[0];
	// UINT16 scrolly=params->control1[1];
	int width_mask = src_bitmap->width - 1;
	int height_mask = src_bitmap->height - 1;

    const UINT16 **srclines = (const UINT16 **)src_bitmap->line;

    UINT16 **destlines = (UINT16 **)bitmap->line;

//    INT16 maxx= (INT16)cliprect->max_x;
    INT16 height = (INT16)(cliprect->max_y+1-cliprect->min_y);
    INT16 width = (INT16)(cliprect->max_x+1-cliprect->min_x);

    INT16 srcbmwidth = src_bitmap->width;

	// if (flipscreen)
	// 	src_y = ((params->src_bitmap->height - 256) - scrolly) + params->columnOffsetConstant ; //krb: not sure this one
	// else
		src_y = scrolly; // + params->columnOffsetConstant;


    INT16 src_x=scrollx;

    // if (flipscreen)
    //     src_x=(srcbmwidth - 256) - src_x;


    UINT16 tilew,nbtilesw,nbtilesh;
    tilemap_get_tile_size(tmap,&tilew,NULL,&nbtilesw,&nbtilesh);

// note here for taitoic use it's always 8.
    // come on, it's either 8x8 for pf1 or 16x16 for pf2 pf3.
    const UINT8 tiledivbits = (tilew == 16)?4:3; // it's 8 or 16.
    const UINT8 tilemask = tilew-1; // it's 8 or 16.
    const UINT16 tilelxmask = (nbtilesw-1)<<tiledivbits; // 31 or 63.
    const UINT16 tilelymask = (nbtilesh-1)<<tiledivbits; // 31 or 63.

    // tiles in the tilemap index
    UINT16 ytile =( src_y & tilelymask);
    UINT16 ylasttile =( (src_y+height) & tilelymask);

    src_x &=width_mask;

    UINT16 xfirsttile = src_x /*& tilelxmask*/ ;
    UINT16 xlasttile = (src_x+width) & tilelxmask ;

    INT16 yscreen = -(src_y & tilemask);
    const INT16 xscreenstartx1 = cliprect->min_x + 0 ; //  always 0 after clip
    const INT16 xscreenstartx2 = cliprect->min_x + (-(src_x & tilemask) + tilew) ;// [1,16]
    const UINT16 maxx = cliprect->max_x+1;

    const INT16 destmodulo = bitmap->rowpixels;
    const INT16 srcmodulo = src_bitmap->rowpixels;

    mame_bitmap *ptransbm = tilemap_get_transparency_bitmap( tmap );

    UINT8 *ptranspdata =  tilemap_get_transparency_data( tmap );

    // 1024 + 32
    // cached_indx = row*tmap->num_cached_cols + c1;
    // if( (tmap->transparency_data[cached_indx]&mask)!=0 )

     // - - - -  y  loop
    while(1)
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
        if(ys2>height) {
            yt2 -= ys2-height;
            ys2=height;
        }

        // - - - -  x row loop
        UINT16 *plinedest_t = destlines[ys1+cliprect->min_y];

        INT16 inx1 = xscreenstartx1;
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
            if(x2>maxx) x2=maxx;
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

                        if(p & 0x0f)
                            *plinedest_ttt = p;

                        plinedest_ttt++;
                        x1++;
                    } // end loop inside tile x

                    plinesrc_tt += srcmodulo;
                    plinedest_tt += destmodulo;
                } // end loop inside tile y
            } else
            {
                // tile saved !
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



// template<int ignoretr,int hasprio>
// void Taitoic_TC0100SCN_tilemap_draw_fgNOCOLUMNOFS_T(mame_bitmap *bitmap,const rectangle *cliprect, int chip, tilemap* tmap ,int flags, UINT32 priority)
// {
// 	const mame_bitmap *src_bitmap = tilemap_get_pixmap(tmap);
// 	int width_mask, height_mask, x, y;
// 	int column_offset, src_x=0, src_y=0;
// 	int scrollx_delta = - tilemap_get_scrolldx( tmap );
// 	int scrolly_delta = - tilemap_get_scrolldy( tmap );

// 	width_mask=(src_bitmap->width - 1) &0x3ff;
// 	height_mask=src_bitmap->height - 1;

// 	src_y=(TC0100SCN_fgscrolly[chip] + scrolly_delta)&height_mask;
// 	if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
// 		src_y=256-src_y;

// 	//We use cliprect->max_y and cliprect->max_x to support games which use more than 1 screen

// 	// Row offsets are 'screen space' 0-255 regardless of Y scroll
// 	for (y=0; y<=cliprect->max_y; y++) {
// 		src_x=(TC0100SCN_fgscrollx[chip] - TC0100SCN_fgscroll_ram[chip][(y + scrolly_delta)&0x1ff] + scrollx_delta + cliprect->min_x)&width_mask;
// 		if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
// 			src_x=256 - 64 - src_x;

//         UINT16 *pbm = (UINT16 *)bitmap->line[y];
//         pbm += cliprect->min_x;
//         UINT8 *pri=NULL;
//         if(hasprio)
//         {   pri = (UINT8 *) priority_bitmap->line[y];
//             pri += cliprect->min_x;
//         }

//         //column_offset=TC0100SCN_colscroll_ram[chip][(src_x)>>3];

//         UINT16 *psrcline = (UINT16*)src_bitmap->line[(src_y /*- column_offset*/)&height_mask];
// 		// Col offsets are 'tilemap' space 0-511, and apply to blocks of 8 pixels at once
// 		for (x=0; x<=(cliprect->max_x - cliprect->min_x); x++) {

//             src_x&= width_mask;
// 			UINT16 p=psrcline[src_x];
//             //  p is always 16c palette with 0 transparent, + a palette offset.
//             // so need &0x000f to test transparency.
// 			if ((p&0x000f)!=0 || ignoretr)
// 			{
// 				//plot_pixel(bitmap, x + cliprect->min_x, y, Machine->pens[p]);
// 				if (hasprio)
// 				{
// 					// pri[x] |= priority; krb: oring bit index has 0 % sense.
// 					*pri = priority;
// 				}
// 				*pbm = p; // Machine->pens[p];
// 			}
// 			pbm++;
// 			pri++;
// 			//src_x=(src_x+1)&width_mask;
// 			src_x++;
// 		}
// 		src_y=(src_y+1)&height_mask;
// 	}
// }

/*
    affects:
    TC0100SCN_tilemap_draw with layer==1
asuka: bonzeadv, asuka,cadash
groundfx
ninjaw   ninja warrior darius2
othunder
taito_f2 ninjak
taito_z  chasehq
undrfire
warriorb
wgp

*/

// replacement for taitoic.c version
void TC0100SCN_tilemap_draw_fg(mame_bitmap *bitmap,const rectangle *cliprect, int chip, tilemap* tmap ,int flags, UINT32 priority)
{
    int useColumnOffset=0;
    // 0x3ff >>3 -> 7f
    for (int x=0; x<0x7f; x++) {
        if( TC0100SCN_colscroll_ram[chip][x] != 0 ) {
        useColumnOffset = 1;
        break;
        }
    }
    if(!useColumnOffset)
    {   // no column offset or all column offset the same...
        if(flags & TILEMAP_IGNORE_TRANSPARENCY)
        {
            if(priority_bitmap)
            {
                Taitoic_TC0100SCN_tilemap_draw_fgT<1,1>(bitmap,cliprect,chip,tmap,flags,priority);
            }
            else
                Taitoic_TC0100SCN_tilemap_draw_fgT<1,0>(bitmap,cliprect,chip,tmap,flags,priority);
        } else
        {   // transparency
            if(priority_bitmap)
                Taitoic_TC0100SCN_tilemap_draw_fgNOCOLUMNOFS_T<0,1>(bitmap,cliprect,chip,tmap,flags,priority);
            else
                Taitoic_TC0100SCN_tilemap_draw_fgNOCOLUMNOFS_T<0,0>(bitmap,cliprect,chip,tmap,flags,priority);
        }

    } else
    {   // column offset applied
        if(flags & TILEMAP_IGNORE_TRANSPARENCY)
        {
            if(priority_bitmap)
            {
                Taitoic_TC0100SCN_tilemap_draw_fgT<1,1>(bitmap,cliprect,chip,tmap,flags,priority);
                fillbitmap(priority_bitmap,priority,cliprect);
            }
            else
                Taitoic_TC0100SCN_tilemap_draw_fgT<1,0>(bitmap,cliprect,chip,tmap,flags,priority);
        } else
        {   // transparency
            if(priority_bitmap)
                Taitoic_TC0100SCN_tilemap_draw_fgT<0,1>(bitmap,cliprect,chip,tmap,flags,priority);
            else
                Taitoic_TC0100SCN_tilemap_draw_fgT<0,0>(bitmap,cliprect,chip,tmap,flags,priority);
        }
    }
}


#if 0
void OTC0100SCN_tilemap_draw_fg(mame_bitmap *bitmap,const rectangle *cliprect, int chip, tilemap* tmap ,int flags, UINT32 priority)
{
	const mame_bitmap *src_bitmap = tilemap_get_pixmap(tmap);
	int width_mask, height_mask, x, y, p;
	int column_offset, src_x=0, src_y=0;
	int scrollx_delta = - tilemap_get_scrolldx( tmap );
	int scrolly_delta = - tilemap_get_scrolldy( tmap );

	width_mask=src_bitmap->width - 1;
	height_mask=src_bitmap->height - 1;

	src_y=(TC0100SCN_fgscrolly[chip] + scrolly_delta)&height_mask;
	if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
		src_y=256-src_y;

	//We use cliprect->max_y and cliprect->max_x to support games which use more than 1 screen

	// Row offsets are 'screen space' 0-255 regardless of Y scroll
	for (y=0; y<=cliprect->max_y; y++) {
		src_x=(TC0100SCN_fgscrollx[chip] - TC0100SCN_fgscroll_ram[chip][(y + scrolly_delta)&0x1ff] + scrollx_delta + cliprect->min_x)&width_mask;
		if (TC0100SCN_ctrl[chip][0x7]&1) // Flipscreen
			src_x=256 - 64 - src_x;

		// Col offsets are 'tilemap' space 0-511, and apply to blocks of 8 pixels at once
		for (x=0; x<=(cliprect->max_x - cliprect->min_x); x++) {
			column_offset=TC0100SCN_colscroll_ram[chip][(src_x&0x3ff) / 8];
			p=(((UINT16*)src_bitmap->line[(src_y - column_offset)&height_mask])[src_x]);

			if ((p&0xf)!=0 || (flags & TILEMAP_IGNORE_TRANSPARENCY))
			{
				plot_pixel(bitmap, x + cliprect->min_x, y, Machine->pens[p]);
				if (priority_bitmap)
				{
					UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
					pri[x + cliprect->min_x]|=priority;
				}
			}
			src_x=(src_x+1)&width_mask;
		}
		src_y=(src_y+1)&height_mask;
	}
}
#endif


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

INLINE void taitoic_drawscanline(
		mame_bitmap *bitmap,int x,int y,
		const UINT16 *src,UINT32 orient,int pri, const rectangle *cliprect)
{
	ADJUST_FOR_ORIENTATION(UINT16, orient, bitmap, priority_bitmap, x, y);
	{
		int length=cliprect->max_x - cliprect->min_x + 1;
		src+=cliprect->min_x;
		dsti+=xadv * cliprect->min_x;
		dstp+=xadv * cliprect->min_x;
        while (length--) {
            *dsti = *src++;
            *dstp = pri;
            dsti += xadv;
            dstp += xadv;
        }
	}
}

INLINE void taitoic_drawscanlineTransp(
		mame_bitmap *bitmap,int x,int y,
		const UINT16 *src,UINT32 orient,int pri, const rectangle *cliprect)
{
	ADJUST_FOR_ORIENTATION(UINT16, orient, bitmap, priority_bitmap, x, y);
	{
		int length=cliprect->max_x - cliprect->min_x + 1;
		src+=cliprect->min_x;
		dsti+=xadv * cliprect->min_x;
		dstp+=xadv * cliprect->min_x;

        while (length--) {
            UINT32 spixel = *src++;
            if (spixel<0x7fff) {
                *dsti = spixel;
                *dstp = pri;
            }
            dsti += xadv;
            dstp += xadv;
        }

	}
}
#undef ADJUST_FOR_ORIENTATION



//krb optim for hellish topspeed_get_road_pixel_color()
INT16 *topspeedRoadColorMapTunel,*topspeedRoadColorMap;
void initTopSpeedRoadColorTable()
{
    topspeedRoadColorMapTunel = (INT16*) auto_malloc(sizeof(INT16)*16*32);
    topspeedRoadColorMap = (INT16*) auto_malloc(sizeof(INT16)*16*32);

    // tunel end 32 colors
    INT16 *pm = topspeedRoadColorMapTunel;
    for(UINT16 color=0;color<32;color++)
    {
        for(INT16 lpixel=0;lpixel<16;lpixel++)
        {
            INT16 pixel = lpixel;
            switch (lpixel) // pixel_type = (pixel  & 0x000f );
            {
                case 0x01:		/* Center lines */
                case 0x02:		/* Road edge (inner) */
                {
                    if (color &0x08)
                    {
                        pixel = 14;
                    } else pixel += 10;	// Tunnel colors
                    break;
                }
                case 0x03:		/* Road edge (outer) */
                {
                    if (color &0x04)
                    {
                        pixel = 14;
                    } else pixel += 10;	// Tunnel colors
                    break;
                }
                case 0x04:		/* Road body */
                {
                    pixel = 14;
                    break;
                }
                case 0x05:		/* Off road */
                {
                    pixel = 15;
                    break;
                }
                default:
                {
                    pixel += 10;
                }
            }
            *pm++ = pixel - lpixel;
        }
    }
    // road colors
    pm = topspeedRoadColorMap;
    for(UINT16 color=0;color<32;color++)
    {
        for(INT16 lpixel=0;lpixel<16;lpixel++)
        {
            INT16 pixel = lpixel;

            INT16 road_body_color,off_road_color;

            /* Color changes based on screenshots from game flyer */

            road_body_color = 4;
            off_road_color =  5;

            if (color &0x10)	road_body_color += 5;
            if (color &0x02)	off_road_color  += 5;

            switch (lpixel) // pixel_type = (pixel  & 0x000f );
            {
                case 0x01:		/* Center lines */
                case 0x02:		/* Road edge (inner) */
                {
                    if (color &0x08)
                    {pixel = road_body_color;
                    }
                    break;
                }
                case 0x03:		/* Road edge (outer) */
                {
                    if (color &0x04)
                    {pixel = road_body_color;
                    }
                    break;
                }
                case 0x04:		/* Road body */
                {
                    pixel = road_body_color;
                    break;
                }
                case 0x05:		/* Off road */
                {
                    pixel = off_road_color;
                    break;
                }
                default: {}
            }
            *pm++ = pixel - lpixel;
        }
    }
}


void topspeed_custom_draw(mame_bitmap *bitmap,const rectangle *cliprect,int chip,int layer,int flags,
							UINT32 priority,UINT16 *color_ctrl_ram)
{
	UINT16 *dst16,*src16;
	UINT8 *tsrc;
	UINT16 scanline[1024];	/* won't be called by a wide-screen game, but just in case... */

	mame_bitmap *srcbitmap = tilemap_get_pixmap(PC080SN_tilemap[chip][layer]);
	mame_bitmap *transbitmap = tilemap_get_transparency_bitmap(PC080SN_tilemap[chip][layer]);

	UINT16 a,color;
	int sx,x_index;
	int i,y,y_index,src_y_index,row_index;

	int flip = 0;
	int machine_flip = 0;	/* for  ROT 180 ? */

	int min_x = cliprect->min_x;
	int max_x = cliprect->max_x;
	int min_y = cliprect->min_y;
	int max_y = cliprect->max_y;
	int screen_width = max_x - min_x + 1;
	int width_mask = 0x1ff;	/* underlying tilemap */

	if (!flip)
	{
		sx =       PC080SN_bgscrollx[chip][layer] + 16 - PC080SN_xoffs;
		y_index =  PC080SN_bgscrolly[chip][layer] + min_y - PC080SN_yoffs;
	}
	else	// never used
	{
		sx = 0;
		y_index = 0;
	}

	if (!machine_flip) y = min_y; else y = max_y;

	do
	{
		src_y_index = y_index &0x1ff;	/* tilemaps are 512 px up/down */
		row_index = (src_y_index - PC080SN_bgscrolly[chip][layer]) &0x1ff;
		color = color_ctrl_ram[(row_index + PC080SN_yoffs - 2) &0xff];

		x_index = sx - (PC080SN_bgscroll_ram[chip][layer][row_index]);

		src16 = (UINT16 *)srcbitmap->line[src_y_index];
		tsrc  = (UINT8 *)transbitmap->line[src_y_index];
		dst16 = scanline;


        INT16 *cmap =((color &0xffe0) == 0xffe0)?
                topspeedRoadColorMapTunel:topspeedRoadColorMap;
        cmap += (color&0x001f)<<4;

		if (flags & TILEMAP_IGNORE_TRANSPARENCY)
		{
            for (i=0; i<screen_width; i++)
            {
                a = src16[x_index & width_mask];
                *dst16++ = a + cmap[a & 0x0f];
                x_index++;
            }
		}
		else
		{
            for (i=0; i<screen_width; i++)
            {
                if (tsrc[x_index & width_mask])
                {
                    a = src16[x_index & width_mask];
                    *dst16++ = a + cmap[a & 0x0f];
                }
                else
                    *dst16++ = 0x8000;
                x_index++;
            }
		}

		if (flags & TILEMAP_IGNORE_TRANSPARENCY)
			taitoic_drawscanline(bitmap,0,y,scanline,ROT0,priority,cliprect);
		else
			taitoic_drawscanlineTransp(bitmap,0,y,scanline,ROT0,priority,cliprect);

		y_index++;
		if (!machine_flip) y++; else y--;
	}
	while ( (!machine_flip && y <= max_y) || (machine_flip && y >= min_y) );
}


