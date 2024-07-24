/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

// Amiga
extern "C" {
    #include <exec/types.h>
}

#include <stdio.h>


// from driver.h
#ifndef ORIENTATION_MASK
    #define ORIENTATION_MASK        		0x0007
    #define	ORIENTATION_FLIP_X				0x0001	/* mirror everything in the X direction */
    #define	ORIENTATION_FLIP_Y				0x0002	/* mirror everything in the Y direction */
    #define ORIENTATION_SWAP_XY				0x0004	/* mirror along the top-left/bottom-right diagonal */

    #define	ROT0							0
    #define	ROT90							(ORIENTATION_SWAP_XY | ORIENTATION_FLIP_X)	/* rotate clockwise 90 degrees */
    #define	ROT180							(ORIENTATION_FLIP_X | ORIENTATION_FLIP_Y)	/* rotate 180 degrees */
    #define	ROT270							(ORIENTATION_SWAP_XY | ORIENTATION_FLIP_Y)	/* rotate counter-clockwise 90 degrees */
#endif



struct directDrawScreen {
    void *_base;
    ULONG _bpr;
    WORD _clipX1,_clipY1,_clipX2,_clipY2;
};
struct directDrawSource {
    void *_base;
    ULONG _bpr;
    WORD _x1,_y1,_x2,_y2; // to be drawn.
    int _swapFlags;
};


extern "C" {
void directDrawClutT_UWORD_UWORD(directDrawScreen *screen , directDrawSource *source,LONG x1 ,LONG y1 ,LONG w, LONG h,UWORD *lut);
void directDrawClutT_ULONG_ULONG(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,ULONG *lut);
void directDrawClutT_type24_ULONG(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,ULONG *lut);
void directDrawClutT_UBYTE_UBYTE(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,UBYTE *lut);

//void testx(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,UWORD *lut, int swapFlags);
}

//template<typename SCREENPIXTYPE,typename CLUTTYPE>
//void directDrawScaleClutTSwapXY(directDrawScreen *screen ,
//                directDrawSource *source, // mame bitmap, not swapped.
//                LONG x1 , // final dest screen, already swapped.
//                LONG y1 ,
//                LONG w , // final dest screen, already swapped.
//                LONG h ,
//                CLUTTYPE *lut,
//                int swapFlags
//            )
//{
//    UWORD wsobpr = source->_bpr>>1;
//    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

//    UWORD *psourcebm = (UWORD *)source->_base;
//    psourcebm += (source->_y1 * wsobpr) + source->_x1;

//    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
//    pscreenbm += (y1 * wscbpr) + x1;

//    LONG sourceHeight = source->_y2 - source->_y1;
//    if(sourceHeight<=0) return;

//    LONG sourceWidth = source->_x2 - source->_x1;
//    if(sourceWidth<=0) return;
//    LONG addw = (sourceHeight<<16)/w; // swapped source size
//    LONG addh = (sourceWidth<<16)/h; //

//    // version that tries to do many consecutive lines when they are the same.
//    LONG vh = 0;
//    LONG vxStart = 0;
//   if(swapFlags & ORIENTATION_FLIP_Y)
//    {
//        vh = (sourceWidth<<16)-1;
//        addh = -addh;
//    }
//    if(swapFlags & ORIENTATION_FLIP_X)
//    {
//        vxStart = (sourceHeight<<16)-1;
//        addw = -addw;
//    }
//    // dest screen is scanned horizontally, source vertically.
//    for(LONG hh=0;hh<h;)
//    {
//        UWORD *psoline = psourcebm +(vh>>16); /* wsobpr*(vh>>16)*/ ;
//        UWORD *psoline2 = psourcebm + ((vh+addh)>>16);
//        UWORD *psoline3 = psourcebm + ((vh+addh+addh)>>16);
//        if(psoline == psoline3) // means 3 lines are the same.
//        {
//            SCREENPIXTYPE *pscline = pscreenbm;
//            LONG vx = vxStart;
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = lut[psoline[((UWORD)(vx>>16))*wsobpr]];
//                pscline++;
//                vx += addw;
//            }
//            vh += addh*3;
//            pscreenbm += wscbpr*3;
//            hh +=3;
//        } else
//        if(psoline == psoline2) // means 2 lines are the same.
//        {
//            SCREENPIXTYPE *pscline = pscreenbm;
//            LONG vx = vxStart;
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = lut[psoline[((UWORD)(vx>>16))*wsobpr]];
//                pscline++;
//                vx += addw;
//            }
//            vh += addh+addh;
//            pscreenbm += wscbpr+wscbpr;
//            hh +=2;
//        } else
//        {
//            // other do normal one line ...
//            SCREENPIXTYPE *pscline = pscreenbm;
//            LONG vx = vxStart;
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline++ = lut[psoline[((UWORD)(vx>>16))*wsobpr]];
//                vx += addw;
//            }
//            vh += addh;
//            pscreenbm += wscbpr;
//            hh++;
//        }

//    }


//}

//template<typename SCREENPIXTYPE,typename CLUTTYPE>
//void directDrawScaleClutTNoSwap(directDrawScreen *screen ,
//                directDrawSource *source,
//                LONG x1 ,
//                LONG y1 ,
//                LONG w ,
//                LONG h ,
//                CLUTTYPE *lut,
//                int swapFlags
//            )
//{
//    UWORD wsobpr = source->_bpr>>1;
//    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

//    UWORD *psourcebm = (UWORD *)source->_base;
//    psourcebm += (source->_y1 * wsobpr) + source->_x1;

//    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
//    pscreenbm += (y1 * wscbpr) + x1;

//    LONG sourceHeight = source->_y2 - source->_y1;
//    if(sourceHeight<=0) return;

//    LONG sourceWidth = source->_x2 - source->_x1;
//    if(sourceWidth<=0) return;
//    LONG addw = (sourceWidth<<16)/w;
//    LONG addh = (sourceHeight<<16)/h;

//    // version that tries to do many consecutive lines when they are the same.
//    LONG vh = 0;
//    LONG vxStart = 0;
//   if(swapFlags & ORIENTATION_FLIP_Y)
//    {
//        vh = (sourceHeight<<16)-1;
//        addh = -addh;
//    }
//    if(swapFlags & ORIENTATION_FLIP_X)
//    {
//        vxStart = (sourceWidth<<16)-1;
//        addw = -addw;
//    }
//    for(LONG hh=0;hh<h;)
//    {
//        // test if the 3 consecutives lines are the same
//        UWORD *psoline = psourcebm + wsobpr*(vh>>16);
//        UWORD *psoline2 = psourcebm + wsobpr*((vh+addh)>>16);
//        UWORD *psoline3 = psourcebm + wsobpr*((vh+addh+addh)>>16);
//        if(psoline == psoline3) // means 3 lines are the same.
//        {
//            SCREENPIXTYPE *pscline = pscreenbm;
//            LONG vx = vxStart;
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = lut[psoline[(WORD)(vx>>16)]];
//                pscline++;
//                vx += addw;
//            }
//            vh += addh*3;
//            pscreenbm += wscbpr*3;
//            hh +=3;
//        } else if(psoline == psoline2) // means 2 lines are the same.
//        {
//            SCREENPIXTYPE *pscline = pscreenbm;
//            LONG vx = vxStart;
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline = pscline[wscbpr] = lut[psoline[(WORD)(vx>>16)]];
//                pscline++;
//                vx += addw;
//            }
//            vh += addh<<1;
//            pscreenbm += wscbpr<<1;
//            hh+=2;
//        } else
//        {
//            // other do normal one line ...
//            SCREENPIXTYPE *pscline = pscreenbm;
//            LONG vx = vxStart;
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline++ = lut[psoline[(WORD)(vx>>16)]];
//                vx += addw;
//            }
//            vh += addh;
//            pscreenbm += wscbpr;
//            hh++;
//        }

//    }

//}

//template<typename SCREENPIXTYPE,typename CLUTTYPE>
//void directDrawClutTSwapXY(directDrawScreen *screen ,
//                directDrawSource *source, // mame bitmap, not swapped.
//                LONG x1 ,
//                LONG y1 ,
//                CLUTTYPE *lut,
//                int swapFlags
//            )
//{

//    int w = source->_y2 - source->_y1;
//    int h = source->_x2 - source->_x1;

//    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
//    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

//    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
//    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }


//    UWORD wsobpr = source->_bpr>>1;
//    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

//    UWORD *psourcebm = (UWORD *)source->_base;
//    psourcebm += (source->_y1 * wsobpr) + source->_x1;

//    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
//    pscreenbm += (y1 * wscbpr) + x1;

//    LONG sourceHeight = source->_y2 - source->_y1;
//    if(sourceHeight<=0) return;

//    LONG sourceWidth = source->_x2 - source->_x1;
//    if(sourceWidth<=0) return;
//    LONG addw = wsobpr; //(sourceHeight<<16)/w; // swapped source size
//    LONG addh = 1; //(sourceWidth<<16)/h; //

//    LONG vh = 0;
//    LONG vxStart = 0;
//   if(swapFlags & ORIENTATION_FLIP_Y)
//    {
//        vh = sourceWidth-1;
//        addh = -1;
//    }
//    if(swapFlags & ORIENTATION_FLIP_X)
//    {
//        vxStart = (sourceHeight-1)*wsobpr;
//        addw = -wsobpr;
//    }
//    // dest screen is scanned horizontally, source vertically.
//    for(LONG hh=0;hh<h;)
//    {
//        UWORD *psoline = psourcebm +vh; /* wsobpr*(vh>>16)*/ ;
//        SCREENPIXTYPE *pscline = pscreenbm;
//        LONG vx = vxStart;
//        for(LONG ww=0;ww<w;ww++)
//        {
//            *pscline++ = lut[psoline[vx]];
//            vx += addw;
//        }
//        vh += addh;
//        pscreenbm += wscbpr;
//        hh++;
//    }

//}

//template<typename SCREENPIXTYPE,typename CLUTTYPE>
//void directDrawClutTNoSwap(directDrawScreen *screen ,
//                directDrawSource *source,
//                LONG x1 ,
//                LONG y1 ,

//                CLUTTYPE *lut,
//                int swapFlags
//            )
//{
//    int w = source->_x2 - source->_x1;
//    int h = source->_y2 - source->_y1;

//    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
//    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

//    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
//    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }

//    UWORD wsobpr = source->_bpr>>1;
//    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

//    UWORD *psourcebm = (UWORD *)source->_base;
//    psourcebm += (source->_y1 * wsobpr) + source->_x1;

//    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
//    pscreenbm += (y1 * wscbpr) + x1;

//    LONG sourceHeight = source->_y2 - source->_y1;
//    if(sourceHeight<=0) return;

//    LONG sourceWidth = source->_x2 - source->_x1;
//    if(sourceWidth<=0) return;
//    LONG addw = 1;// (sourceWidth<<16)/w;
//    LONG addh = wsobpr; // (sourceHeight<<16)/h;

//    LONG vh = 0;
//    LONG vxStart = 0;
//   if(swapFlags & ORIENTATION_FLIP_Y)
//    {
//        vh = (sourceHeight-1)*wsobpr;// (sourceHeight<<16)-1;
//        addh = -wsobpr;
//    }
//    if(swapFlags & ORIENTATION_FLIP_X)
//    {
//        vxStart = sourceWidth/*-1*/;
//        addw = -1;
//    }

//    for(LONG hh=0;hh<h;)
//    {
//        UWORD *psoline = psourcebm + vh;

//        SCREENPIXTYPE *pscline = pscreenbm;
//        LONG vx = vxStart;
//        if(addw==1)
//        {
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline++ = lut[psoline[vx++]];
//            }
//        } else
//        {   // swap X
//            for(LONG ww=0;ww<w;ww++)
//            {
//                *pscline++ = lut[psoline[--vx]];
//            }
//        }
//        vh += addh;
//        pscreenbm += wscbpr;
//        hh++;

//    }

//}

//template<typename SCREENPIXTYPE,typename CLUTTYPE>
//void directDrawScaleClutT(directDrawScreen *screen ,
//                directDrawSource *source,
//                LONG x1 ,
//                LONG y1 ,
//                LONG w ,
//                LONG h ,
//                CLUTTYPE *lut,
//                int swapFlags
//            )
//{
//    if(swapFlags & ORIENTATION_SWAP_XY)
//    {
//        directDrawScaleClutTSwapXY<SCREENPIXTYPE,CLUTTYPE>(
//            screen,source,x1,y1,w,h,lut,swapFlags);
//    } else
//    {
//        directDrawScaleClutTNoSwap<SCREENPIXTYPE,CLUTTYPE>(
//            screen,source,x1,y1,w,h,lut,swapFlags);
//    }
//}


//template<typename SCREENPIXTYPE,typename CLUTTYPE>
//void directDrawClutT(directDrawScreen *screen ,
//                directDrawSource *source,
//                LONG x1 ,
//                LONG y1 ,
//                CLUTTYPE *lut,
//                int swapFlags
//            )
//{
//    if(swapFlags & ORIENTATION_SWAP_XY)
//    {
//        directDrawClutTSwapXY<SCREENPIXTYPE,CLUTTYPE>(
//            screen,source,x1,y1,lut,swapFlags);
//    } else
//    {
//        directDrawClutTNoSwap<SCREENPIXTYPE,CLUTTYPE>(
//            screen,source,x1,y1,lut,swapFlags);
//    }
//}
