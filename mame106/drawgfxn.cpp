#include "drawgfxn.h"
#include <stdio.h>
#include <cstdint>

#include <map>
using namespace std;

#ifdef LSB_FIRST
#define WRITEORD0 0
#define WRITEORD1 1
#define WRITEORD2 2
#define WRITEORD3 3


#define SHIFT0 0
#define SHIFT1 8
#define SHIFT2 16
#define SHIFT3 24

#else
#define WRITEORD0 3
#define WRITEORD1 2
#define WRITEORD2 1
#define WRITEORD3 0

#define SHIFT3 0
#define SHIFT2 8
#define SHIFT1 16
#define SHIFT0 24


#endif


template<typename destPix_t>
class DestPixContext {
public:
    DestPixContext(struct drawgfxParams *p DGREG(a0),UINT16 ofsx, UINT16 ofsy) :
        _p( ((destPix_t *)p->dest->line[ofsy]) + ofsx),_rowpixels((UINT16)p->dest->rowpixels) {}
    void incrementx(INT16 n) {
        _p += n;
    }
    void incrementy(INT16 n) {
        _p += _rowpixels*n;
    }
    void setPix(UINT32 n,pen_t color) {
        _p[n] = color;
    }

    destPix_t *_p;
    UINT16 _rowpixels;
};

/*
    #define SETPIXELCOLOR(dest,n)
        {
            if (((1 << (pridata[dest] & 0x1f)) & pmask) == 0)
            {
                if (pridata[dest] & 0x80)
                {
                    dstdata[dest] = palette_shadow_table[n];
                } else
                {
                    dstdata[dest] = (n);
                }
            }
            pridata[dest] = (pridata[dest] & 0x7f) | afterdrawmask;
        }


*/
// #define SETPIXELCOLOR(dest,n)
// { if (((1 << (pridata[dest] & 0x1f)) & pmask) == 0)
//    { if (pridata[dest] & 0x80) { dstdata[dest] = palette_shadow_table[n];} else { dstdata[dest] = (n);} } pridata[dest] = (pridata[dest] & 0x7f) | afterdrawmask; }

// original bitmap prio table goes like: Mask 1f give a bit with 1<<(prio&1f), to be masked by
// p->priority_mask also 1f
static UINT8 sillyPrioTable[]={
    1,2,2,4, // 0
};


 typedef UINT8 pri_t;
// //map<int,int> _k;
// template<typename destPix_t>
// class DestPixPrioContext {
// public:
//     DestPixPrioContext(struct drawgfxParams *p DGREG(a0),
//                        UINT16 ofsx, UINT16 ofsy) :
//         _p( ((destPix_t *)p->dest->line[ofsy]) + ofsx),
//         _prio(((pri_t *)p->pri_buffer->line[ofsy])+ofsx),
//         _rowpixels((UINT16)p->dest->rowpixels),
//         _np(
//             // works with silworm and cps1
//             (((p->priority_mask)>>2) & 4) | // silkw text playfield
//             (((p->priority_mask)>>2) & 2) | // silkw foreg
//             (((p->priority_mask)>>1) & 1)   // silkw backg

//             ){}
//     void incrementx(INT16 n) {
//         _p += n;
//         _prio += n;
//     }
//     void incrementy(INT16 n) {
//         _p += _rowpixels*n;
//         _prio += _rowpixels*n;
//     }
//     void setPix(UINT32 n,pen_t color) {
//         // for silkworm values goes 0->7 because of the 3 playfields.
//         // 0xf0
//         UINT8 priobef = _prio[n]; // 1 lighted if background, 2 is foregroud, 4 is text.

//         if((_np &priobef)==0) // bit lighted in _np means "hidden by"
//         {
//             _prio[n] = priobef | 0x1f; // |0x1f mask all next, means reverse sprite draw order
//             _p[n] = color;
//         } //TODO DOESNT WORK VALIDATE AGAINST SILKWORM/RYGAR/... first. vidhrw/tecmo.c
//     }

//     destPix_t *_p;
//     pri_t   *_prio;
//     UINT16 _rowpixels;
//     const pri_t _np;
// };

// - - -- -
template<typename destPix_t>
class DestPixRealPrioContext {
public:
    DestPixRealPrioContext(struct drawgfxParams *p DGREG(a0),
                       UINT16 ofsx, UINT16 ofsy) :
        _p( ((destPix_t *)p->dest->line[ofsy]) + ofsx),
        _prio(((pri_t *)p->pri_buffer->line[ofsy])+ofsx),
        _rowpixels((UINT16)p->dest->rowpixels),
        _mask(p->priority_mask)
       {}
    void incrementx(INT16 n) {
        _p += n;
        _prio += n;
    }
    void incrementy(INT16 n) {
        _p += _rowpixels*n;
        _prio += _rowpixels*n;
    }
    void setPix(UINT32 n,pen_t color) {
        // for silkworm values goes 0->7 because of the 3 playfields.
        // 0xf0
        if((_mask & (1<<_prio[n]))==0) // bit lighted in _np means "hidden by"
            _p[n] = color;
        _prio[n] = 31;
    }

    destPix_t *_p;
    pri_t   *_prio;
    UINT16 _rowpixels;
    UINT32 _mask;

};


class SourceContext {
public:
    SourceContext(const gfx_element *gfx DGREG(a1), unsigned int code )
        : _p( gfx->gfxdata + code * gfx->char_modulo),_linemodulo(gfx->line_modulo)
         {}
    void incrementy(INT32 n) {
        _p += n* _linemodulo;
    }
    void incrementx(int n) {
            _p += n;
    }
    UINT8 *_p;
    INT16 _linemodulo;
};


template<typename destPix_t>
class ClutColor {
public:
    ClutColor(const pen_t *paldata) : _paldata(paldata) { }
    destPix_t color(UINT16 c) {
        return _paldata[c];
    }
    const pen_t *_paldata;
};

template<typename destPix_t>
class ClutColorDirect {
public:
    ClutColorDirect(UINT32 offset) : _offset(offset) { }
    destPix_t color(UINT16 c) {
        return c + _offset;
    }
    UINT32 _offset;
};

/*
    krb: redo some of drawgfx.c
    the idea is: more template, less useless switches.
*/
//

template<class destContext,typename destPix_t,
         class srcContext,class MapColor>
void drawgfxT(struct drawgfxParams *p DGREG(a0), int isOpaque)
{
    // - - - - from drawgfx()
  //  mame_bitmap *dest = p->dest;
    const gfx_element *gfx DGREG(a1) = p->gfx;
    UINT8 transpen = (UINT8)p->transparent_color;
    unsigned int code = p->code;

    // test if opaque after all.
    if(!isOpaque)
    {
        UINT32 transmask = 1 << transpen;
        UINT32 pen_usage = p->gfx->pen_usage[code];
		if ((pen_usage & ~transmask) == 0)
			return;
		else if ((pen_usage & transmask) == 0)
			/* character is totally opaque, can disable transparency */
			isOpaque = 1;
    }

    int flipx = p->flipx;
    int flipy = p->flipy;
    INT16 sx = p->sx;
    INT16 sy = p->sy;
//    UINT32 pri_mask = p->priority_mask;

    const rectangle *clip = p->clip;

	INT16 ox;
	INT16 oy;
	INT16 ex;
	INT16 ey;

	/* check bounds */
	ox = sx;
	oy = sy;

	ex = sx + gfx->width; // consider end excluded, immense lots of useless -1 removed.
	if (sx < clip->min_x) sx = clip->min_x;

	if (ex > clip->max_x+1) ex = clip->max_x+1;
    int dstwidth = ex-sx; /* dest width */
    if(dstwidth<=0) return;

	ey = sy + gfx->height;
	if ( sy < clip->min_y) sy = clip->min_y;

	if ( ey > clip->max_y+1) ey = clip->max_y+1;
    int dstheight = ey-sy;	/* dest height */
    if(dstheight<=0) return;

        srcContext srcdata(gfx,code);

		int srcwidth = gfx->width;									/* source width */
		int srcheight = gfx->height;									/* source height */

		int leftskip = sx-ox;											/* left skip */
		int topskip = sy-oy;											/* top skip */
        destContext dstdata(p,sx,sy);

//        MapColor pal(&gfx->colortable[gfx->color_granularity * p->color]);

        MapColor pal(gfx->color_granularity * p->color);

//		UINT8 *pribuf = (p->pri_buffer) ? ((UINT8 *)p->pri_buffer->line[sy]) + sx : NULL;

 // INCREMENT_DST {dstdata+=(n);pridata += (n);}
    // - - - - -BLOCKMOVEXXXX
    /*
#define COMMON_ARGS
		const UINT8 *srcdata,int srcwidth,int srcheight,int srcmodulo,
		int leftskip,int topskip,int flipx,int flipy,
		DATA_TYPE *dstdata,int dstwidth,int dstheight,int dstmodulo
BLOCKMOVELU(4toN_opaque,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata));
   */
    // - - - - - - ADJUST
    INT16 ydir;
	if (flipy)
	{
    	dstdata.incrementy(dstheight-1);
    	srcdata.incrementy(srcheight - dstheight - topskip);
		ydir = -1;
	}
	else
	{
    	srcdata.incrementy(topskip);
		ydir = 1;
	}
    /* adjust x moved in other flipx test, one test less.
    if (flipx)
	{
    	dstdata.incrementx(dstwidth-1);
    	srcdata.incrementx(srcwidth - dstwidth - leftskip);
	}
	else
	{
        srcdata.incrementx(leftskip);
	}*/
    INT16 srcmodulo = gfx->width; // - leftskip;

    if(isOpaque)
    {
        if (flipx)
        {
            // adjust
            dstdata.incrementx(dstwidth /*aparently not, against topspeed sprite tiling - 1*/);
            srcdata.incrementx(srcwidth - dstwidth - leftskip );

            INT16 destmodulo = ydir*dstdata._rowpixels + dstwidth;
            while (dstheight)
            {

                destPix_t *destend = dstdata._p - dstwidth;
                UINT8 *scrdatap = srcdata._p;
                while (((uintptr_t)scrdatap & 3) && dstdata._p > destend)
                {
                    dstdata.setPix(0,pal.color( *scrdatap++ ));
                    dstdata.incrementx(-1);
                }
                UINT32 *sd4 = (UINT32 *)scrdatap;
                while (dstdata._p >= (destend + 4))
                {
                    UINT32 col4 = *(sd4++);
                    dstdata.incrementx(-4);
                    dstdata.setPix(WRITEORD3,pal.color((UINT8)col4));
                    col4>>=8;
                    dstdata.setPix(WRITEORD2,pal.color((UINT8)col4));
                    col4>>=8;
                    dstdata.setPix(WRITEORD1,pal.color((UINT8)col4));
                    col4>>=8;
                    dstdata.setPix(WRITEORD0,pal.color((UINT8)col4));
                }
                scrdatap = (UINT8 *)sd4;
                while (dstdata._p > destend)
                {
                    dstdata.setPix(0,pal.color( *scrdatap++ ));
                    dstdata.incrementx(-1);
                }

                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                dstheight--;
            }

        }
        else
        {
            // adjust
            srcdata.incrementx(leftskip);

            INT16 destmodulo = ydir*dstdata._rowpixels - dstwidth;
            while (dstheight)
            {
                destPix_t * destend = dstdata._p + dstwidth;
                UINT8 *scrdatap = srcdata._p;
                while (((uintptr_t)scrdatap & 3) && dstdata._p < destend)	/* longword align */
                {
                    dstdata.setPix(0,pal.color( *scrdatap++ ));
                    dstdata.incrementx(1);
                }
                UINT32 *sd4 = (UINT32 *)scrdatap;
                while (dstdata._p <= (destend - 4))
                {
                    UINT32 col4= *(sd4++);
                    dstdata.setPix(WRITEORD0,pal.color((UINT8)col4));
                    col4>>=8;
                    dstdata.setPix(WRITEORD1,pal.color((UINT8)col4));
                    col4>>=8;
                    dstdata.setPix(WRITEORD2,pal.color((UINT8)col4));
                    col4>>=8;
                    dstdata.setPix(WRITEORD3,pal.color((UINT8)col4));
                    dstdata.incrementx(4);
                }
                scrdatap = (UINT8 *)sd4;
                while (dstdata._p < destend)
                {
                    dstdata.setPix(0,pal.color( *scrdatap++ ));
                    dstdata.incrementx(1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                // INCREMENT_DST(ydir*VMODULO - dstwidth*HMODULO)
                dstheight--;
            } // end while h
        } // end if not flipx

    } else // end if opaque
    {   // start if transparent
        UINT32 trans4 = transpen * 0x01010101UL;

        if (flipx)
        {
            // adjust
            dstdata.incrementx(dstwidth  /*aparently not, against topspeed sprite tiling - 1*/);
            srcdata.incrementx(srcwidth - dstwidth - leftskip);

            INT16 destmodulo = ydir*dstdata._rowpixels + dstwidth;
            while (dstheight)
            {
                destPix_t *destend = dstdata._p - dstwidth;
                UINT8 *scrdatap = srcdata._p;
                while (((uintptr_t)scrdatap & 3) && dstdata._p > destend)	/* longword align */
                {
                    UINT8 col = *scrdatap++;
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(-1);
                }
                UINT32 *sd4 = (UINT32 *)scrdatap;
                while (dstdata._p >= (destend + 4))
                {
                    UINT32 col4;
                    dstdata.incrementx(-4);
                    if ((col4 = *(sd4++)) != trans4)
                    {
                        UINT8 col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD3,pal.color(col));
                        col4>>=8;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD2,pal.color(col));
                        col4>>=8;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD1,pal.color(col));
                        col4>>=8;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD0,pal.color(col));
                    }
                }
                scrdatap = (UINT8 *)sd4;
                while (dstdata._p > destend)
                {
                    UINT8 col = *scrdatap++;
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(-1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                dstheight--;
            }
        }
        else
        {
            // adjust
            srcdata.incrementx(leftskip);

            INT16 destmodulo = ydir*dstdata._rowpixels - dstwidth;
            while (dstheight)
            {
                destPix_t * destend = dstdata._p + dstwidth;
                UINT8 *scrdatap = srcdata._p;
                while (((uintptr_t)scrdatap & 3) && dstdata._p < destend)	/* longword align */
                {
                    UINT8 col = *scrdatap++;
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(1);
                }
                UINT32 *sd4 = (UINT32 *)scrdatap;
                while (dstdata._p <= (destend - 4))
                {
                    UINT32 col4;
                    if ((col4 = *(sd4++)) != trans4)
                    {
                        UINT8 col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD0,pal.color(col));
                        col4>>=8;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD1,pal.color(col));
                        col4>>=8;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD2,pal.color(col));
                        col4>>=8;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix(WRITEORD3,pal.color(col));
                    }
                    dstdata.incrementx(4);
                }
                scrdatap = (UINT8 *)sd4;
                while (dstdata._p < destend)
                {
                    UINT8 col = *(scrdatap++);
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                // INCREMENT_DST(ydir*VMODULO - dstwidth*HMODULO)
                dstheight--;
            } // end while h
        } // end if not flipx

    } // end if transparent

}
// manage access to 2 pixels of 16 colors encoded on one byte 0xf0 0x0f
// this is used as template param class and is completely asm-inlined.
class SourceContextP4 {
public:
    SourceContextP4(const gfx_element *gfx DGREG(a1), unsigned int code )
        : _p(gfx->gfxdata + code * gfx->char_modulo), _linemodulo(gfx->line_modulo),_bleft(0)
         {}
    // return value and increment, rare use for clipping.
    UINT8 getp() {
        UINT8 v = ((*_p) >>(_bleft<<2)) & 0x0f;
         _p += _bleft;
        _bleft ^=1;
        return v;
    }
    void incrementy(INT32 n) {
        _p += n* _linemodulo;
        _bleft = 0;
    }

    void incrementx(int n) {
        n += _bleft;
        _p += n>>1;
        _bleft  = n & 1;
    }
    UINT8 *_p;
    UINT16 _linemodulo;
    UINT16 _bleft; // 1 or 0, index below byte.
};


template<class destContext,typename destPix_t,
         class srcContext,class MapColor>
void drawgfxPack4T(struct drawgfxParams *p DGREG(a0), int isOpaque)
{
    // - - - - from drawgfx()
  //  mame_bitmap *dest = p->dest;
    const gfx_element *gfx DGREG(a1) = p->gfx;
    UINT8 transpen = (UINT8)p->transparent_color;
    unsigned int code = p->code;

    // test if opaque after all.
    if(!isOpaque)
    {
        UINT32 transmask = 1 << transpen;
        UINT32 pen_usage = p->gfx->pen_usage[code];
		if ((pen_usage & ~transmask) == 0)
			return;
		else if ((pen_usage & transmask) == 0)
			/* character is totally opaque, can disable transparency */
			isOpaque = 1;
    }

    int flipx = p->flipx;
    int flipy = p->flipy;
    INT16 sx = p->sx;
    INT16 sy = p->sy;
//    UINT32 pri_mask = p->priority_mask;

    const rectangle *clip = p->clip;

	INT16 ox;
	INT16 oy;
	INT16 ex;
	INT16 ey;

	/* check bounds */
	ox = sx;
	oy = sy;

	ex = sx + gfx->width; // consider end excluded, immense lots of useless -1 removed.
	if (sx < clip->min_x) sx = clip->min_x;
	if (ex > clip->max_x) ex = clip->max_x;
    int dstwidth = ex-sx; /* dest width */
    if(dstwidth<=0) return;

	ey = sy + gfx->height;
	if ( sy < clip->min_y) sy = clip->min_y;
	if ( ey > clip->max_y) ey = clip->max_y;
    int dstheight = ey-sy;	/* dest height */
    if(dstheight<=0) return;

        srcContext srcdata(gfx,code);

		int srcwidth = gfx->width;									/* source width */
		int srcheight = gfx->height;									/* source height */

		int leftskip = sx-ox;											/* left skip */
		int topskip = sy-oy;											/* top skip */
        destContext dstdata(p,sx,sy);

       // MapColor pal(&gfx->colortable[gfx->color_granularity * p->color]);
        MapColor pal(gfx->color_granularity * p->color);

    // - - - - - - ADJUST

    INT16 ydir;
	if (flipy)
	{
    	dstdata.incrementy(dstheight-1);
    	srcdata.incrementy(srcheight - dstheight - topskip);
		ydir = -1;
	}
	else
	{
    	srcdata.incrementy(topskip);
		ydir = 1;
	}
	/*moved to avoid a test
	if (flipx)
	{
    	dstdata.incrementx(dstwidth-1);
    	srcdata.incrementx(srcwidth - dstwidth - leftskip);
	}
	else
	{
        srcdata.incrementx(leftskip);
	}*/

    INT16 srcmodulo =  gfx->width - dstwidth; // gfx->width; // - leftskip;

    if(isOpaque)
    {
        if (flipx)
        {
            // adjust x
            dstdata.incrementx(dstwidth - 1);
            srcdata.incrementx(srcwidth - dstwidth - leftskip);
            INT16 destmodulo = ydir*dstdata._rowpixels + dstwidth;
            while (dstheight)
            {
                destPix_t *destend = dstdata._p - dstwidth;
                //UINT8 *scrdatap = srcdata._p;
                while (((uintptr_t)srcdata._p & 3) && dstdata._p > destend)
                {
                    dstdata.setPix(0,pal.color(srcdata.getp()));
                    dstdata.incrementx(-1);
                }
                UINT32 *sd4 = (UINT32 *)srcdata._p;
                while (dstdata._p >= (destend + 8))
                {
                    UINT32 col4 = *(sd4++);
                    dstdata.incrementx(-8);

                    dstdata.setPix((WRITEORD3<<1)+1, pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD3<<1), pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD2<<1)+1, pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD2<<1), pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD1<<1)+1, pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD1<<1), pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD0<<1)+1, pal.color((UINT8)col4 & 0x0f));
                    col4 >>= 4;
                    dstdata.setPix((WRITEORD0<<1), pal.color((UINT8)col4));

                }
                srcdata._p = (UINT8 *)sd4;
                while (dstdata._p > destend)
                {
                    dstdata.setPix(0,pal.color( srcdata.getp() ));
                    dstdata.incrementx(-1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                dstheight--;
            }
        }
        else
        {
            // adjust x
            srcdata.incrementx(leftskip);

            INT16 destmodulo = ydir*dstdata._rowpixels - dstwidth;
            while (dstheight)
            {
                destPix_t * destend = dstdata._p + dstwidth;
               // UINT8 *scrdatap = srcdata._p;
                while (((uintptr_t)srcdata._p & 3) && dstdata._p < destend)	/* longword align, 8 Pixels */
                {
                    dstdata.setPix(0,pal.color( srcdata.getp() ));
                    dstdata.incrementx(1);
                }
                UINT32 *sd4 = (UINT32 *)srcdata._p;
                while (dstdata._p <= (destend - 8))
                {
                    UINT32 col4= *(sd4++);

                    dstdata.setPix((WRITEORD0<<1),pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD0<<1)+1,pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD1<<1),pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD1<<1)+1,pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD2<<1),pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD2<<1)+1,pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD3<<1),pal.color((UINT8)col4 & 0x0f));
                    col4>>=4;
                    dstdata.setPix((WRITEORD3<<1)+1,pal.color((UINT8)col4));
                    dstdata.incrementx(8);
                }
                srcdata._p = (UINT8 *)sd4;
                while (dstdata._p < destend)
                {
                    dstdata.setPix(0,pal.color( srcdata.getp() ));
                    dstdata.incrementx(1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);

                dstheight--;
            } // end while h
        } // end if not flipx

    } else // end if opaque
    {   // start if transparent
        UINT32 trans8 = transpen * 0x11111111UL;

        if (flipx)
        {
            // adjust x
            dstdata.incrementx(dstwidth - 1);
            srcdata.incrementx(srcwidth - dstwidth - leftskip);

            INT16 destmodulo = ydir*dstdata._rowpixels + dstwidth;
            while (dstheight)
            {
                destPix_t *destend = dstdata._p - dstwidth;

                while (((uintptr_t)srcdata._p & 3) && dstdata._p > destend)	// longword align
                {
                    UINT8 col = srcdata.getp();
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(-1);
                }
                UINT32 *sd4 = (UINT32 *)srcdata._p;
                while (dstdata._p >= (destend + 8))
                {
                    UINT32 col4;
                    dstdata.incrementx(-8);
                    if ((col4 = *(sd4++)) != trans8)
                    {
                        UINT8 col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD3<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4& 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD3<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD2<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD2<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4& 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD1<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4& 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD1<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD0<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4;
                        if(col != transpen) dstdata.setPix((WRITEORD0<<1),pal.color(col));

                    }
                }
                srcdata._p = (UINT8 *)sd4;
                while (dstdata._p > destend)
                {
                    UINT8 col =  srcdata.getp();
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(-1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                dstheight--;
            }

        }
        else
        {
            // adjust x
            srcdata.incrementx(leftskip);

            INT16 destmodulo = ydir*dstdata._rowpixels - dstwidth;
            while (dstheight)
            {
                destPix_t * destend = dstdata._p + dstwidth;
                while (((uintptr_t)srcdata._p & 3) && dstdata._p < destend)	// longword align 8 pix
                {
                    UINT8 col =srcdata.getp();
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(1);
                }
                UINT32 *sd4 = (UINT32 *)srcdata._p;
                while (dstdata._p <= (destend - 8))
                {
                    UINT32 col4;
                    if ((col4 = *(sd4++)) != trans8)
                    {
                        UINT8 col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD0<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD0<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD1<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD1<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD2<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD2<<1)+1,pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD3<<1),pal.color(col));
                        col4>>=4;
                        col = (UINT8)col4 & 0x0f;
                        if(col != transpen) dstdata.setPix((WRITEORD3<<1)+1,pal.color(col));
                    }
                    dstdata.incrementx(8);
                }
                srcdata._p = (UINT8 *)sd4;
                while (dstdata._p < destend)
                {
                    UINT8 col =srcdata.getp();
                    if (col != transpen) dstdata.setPix(0,pal.color(col));
                    dstdata.incrementx(1);
                }
                srcdata.incrementx(srcmodulo);
                dstdata.incrementx(destmodulo);
                dstheight--;
            } // end while h

        } // end if not flipx


    } // end if transparent

}

// template parametrization:  PIXPACKED4/PIX8 , prio/noprio
// function params: opaque/transp.

void drawgfx_clut16_Src8(struct drawgfxParams *p DGREG(a0))
{
    drawgfxT<DestPixContext<UINT16>,UINT16,
        SourceContext,ClutColorDirect<UINT16>>(p,(p->transparency !=TRANSPARENCY_PEN)); // 0 transparent, else opaque

}
void drawgfx_clut16_Src4(struct drawgfxParams *p DGREG(a0))
{
    drawgfxPack4T<DestPixContext<UINT16>,UINT16,
        SourceContextP4,ClutColorDirect<UINT16>>(p,(p->transparency !=TRANSPARENCY_PEN)); // 0 transparent, else opaque
}

void drawgfx_clut16_Src8_prio(struct drawgfxParams *p DGREG(a0))
{
    if(p->priority_mask==0)
    {
        drawgfx_clut16_Src8(p);
        return;
    }

    drawgfxT<DestPixRealPrioContext<UINT16>,UINT16,
        SourceContext,ClutColorDirect<UINT16>>(p,(p->transparency !=TRANSPARENCY_PEN)); // 0 transparent, else opaque

}
void drawgfx_clut16_Src4_prio(struct drawgfxParams *p DGREG(a0))
{
    if(p->priority_mask==0)
    {
        drawgfx_clut16_Src4(p);
        return;
    }
    drawgfxPack4T<DestPixRealPrioContext<UINT16>,UINT16,
        SourceContextP4,ClutColorDirect<UINT16>>(p,(p->transparency !=TRANSPARENCY_PEN)); // 0 transparent, else opaque

}


// - - - - - - - - - - - -

// extern int nbframe;
/*
.L67:
	move.l d0,d4
	asr.l #8,d4
	asr.l #8,d4
	move.b (a2,d4.l),(-47,a5)
	jeq .L47
	clr.l d4
	move.b (a0),d4
	btst d4,d5
	jne .L48
	clr.w d4
	move.b (-47,a5),d4
	add.w (-2,a5),d4
	move.w d4,(a1)
.L48:
	move.b #31,(a0)
.L47:
	add.l d2,d0
	addq.l #2,a1
	addq.l #1,a0
*/
/*
 d5 color
.L67:
	move.l d0,d4
	asr.l #8,d4
	asr.l #8,d4
	move.b (a2,d4.l),d4
	jeq .L47
	clr.l d4
	move.b (a0),d4
	btst d4,d5
	jne .L48
	clr.w d4

	add.w d5,d4
	move.w d4,(a1)
.L48:
	move.b #31,(a0)
.L47:
	add.l d2,d0
	addq.l #2,a1
	addq.l #1,a0

*/
// chasehq, experimental
#define trt_isOpaque 0
#define trt_isTransp 1
// very chasehq thing: invisible sprite to manage road clipping.
#define trt_fulltransp 2

template<int transptype>
void drawgfxzoom_clut16_Src8T(struct drawgfxParams *p DGREG(a0))
{
    int scalex = p->scalex;
    int scaley = p->scaley;

    mame_bitmap *dest_bmp = p->dest;
    const gfx_element *gfx = p->gfx;
    unsigned int code = p->code;

    int flipx = p->flipx,flipy = p->flipy;
    int sx=p->sx,sy=p->sy;

    // - - - -
    mame_bitmap *pri_buffer = p->pri_buffer; // optional
    UINT32 pri_mask = p->priority_mask;

    UINT8 *source_base = gfx->gfxdata + (code /*% gfx->total_elements*/) * gfx->char_modulo;

    int sprite_screen_height = (scaley*gfx->height+0x8000)>>16;
    int sprite_screen_width = (scalex*gfx->width+0x8000)>>16;


    if (!sprite_screen_width || !sprite_screen_height) return;

    /* compute sprite increment per screen pixel */
    int dx = (gfx->width<<16)/sprite_screen_width;
    int dy = (gfx->height<<16)/sprite_screen_height;

    int ex = sx+sprite_screen_width;
    int ey = sy+sprite_screen_height;

    int x_index_base;
    int y_index;

    if( flipx )
    {
        x_index_base = (sprite_screen_width-1)*dx;
        dx = -dx;
    }
    else
    {
        x_index_base = 0;
    }

    if( flipy )
    {
        y_index = (sprite_screen_height-1)*dy;
        dy = -dy;
    }
    else
    {
        y_index = 0;
    }

    {
        const rectangle *clip = p->clip;
        if( clip )
        {
            if( sx < clip->min_x)
            { /* clip left */
                int pixels = clip->min_x-sx;
                sx += pixels;
                x_index_base += pixels*dx;
            }
            if( sy < clip->min_y )
            { /* clip top */
                int pixels = clip->min_y-sy;
                sy += pixels;
                y_index += pixels*dy;
            }
            /* NS 980211 - fixed incorrect clipping */
            if( ex > clip->max_x+1 )
            { /* clip right */
                int pixels = ex-clip->max_x-1;
                ex -= pixels;
            }
            if( ey > clip->max_y+1 )
            { /* clip bottom */
                int pixels = ey-clip->max_y-1;
                ey -= pixels;
            }
        }
    }

    if( ex<=sx ) return;

    {
        // important for inner loop...
        const register UINT16 colorOfs = gfx->color_granularity * p->color;

        for(int y=sy; y<ey; y++ )
        {
            UINT8 *source = source_base + (y_index>>16) * gfx->line_modulo;
            UINT16 *dest = (UINT16 *)dest_bmp->line[y];
            UINT8 *pri = (UINT8 *)pri_buffer->line[y];
            UINT8 *priend = pri+ex;
            pri += sx;
            dest +=sx;
            int x_index = x_index_base;
            //for(int x=sx; x<ex; x++ )
            while(pri<priend)
            {
                 int c = source[x_index>>16];
                // per dest pixel
                if(transptype == trt_isOpaque) // template escaped test
                {
                    UINT8 pr = *pri;
                    if (((1UL <<pr ) & pri_mask) == 0)
                        *dest = colorOfs + c;
                    *pri = 31;
                } else
                {   // transparent
                    if( c /*!= transparent_color*/ )
                    {
                        UINT8 pr = *pri;
                        if (((1UL <<pr ) & pri_mask) == 0)
                            *dest = colorOfs + c;
                        *pri = 31;
                    }
                }
                pri++;
                dest++;
                x_index += dx;
            }
            y_index += dy;
        }
    }

}
void drawgfxzoom_clut16_Src8_tr0_prio(struct drawgfxParams *p DGREG(a0))
{
	if (p->scalex == 0x10000 && p->scaley == 0x10000)
	{
       drawgfx_clut16_Src8_prio(p); // also does opaque test
       return;
	}

    UINT32 transmask = 1 /*<< transpen*/; // always color 0 for the moment.
    UINT32 pen_usage = p->gfx->pen_usage[p->code];

	int isFullTransp = (int)((pen_usage & ~transmask) == 0);
	if(isFullTransp) return;
    int isOpaque = (int)((pen_usage & transmask) == 0);

    if(isOpaque) drawgfxzoom_clut16_Src8T<trt_isOpaque>(p);
   // else if(isFullTransp) drawgfxzoom_clut16_Src8T<trt_fulltransp>(p);
    else drawgfxzoom_clut16_Src8T<trt_isTransp>(p);
}
