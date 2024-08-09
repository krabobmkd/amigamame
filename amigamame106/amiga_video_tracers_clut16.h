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
// all these use 16b color index or RGB15, used by most games
void directDrawClutT_UWORD_UWORD(directDrawScreen *screen , directDrawSource *source,LONG x1 ,LONG y1 ,LONG w, LONG h,UWORD *lut);
void directDrawClutT_ULONG_ULONG(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,ULONG *lut);
void directDrawClutT_type24_ULONG(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,ULONG *lut);
void directDrawClutT_UBYTE_UBYTE(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,UBYTE *lut);
void directDraw_UBYTE_UBYTE(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);

// added for ARGB32 modes (only games RGB highcolor, not using clut.
// manage all CGX screen pixel modes

void directDraw_RGB15_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDraw_BGR15_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDraw_RGB15PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDraw_BGR15PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);

void directDraw_RGB16_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDraw_BGR16_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDraw_RGB16PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDraw_BGR16PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);

void directDraw_type24_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);

void directDrawARGB32_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDrawBGRA32_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);
void directDrawRGBA32_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h);



}
