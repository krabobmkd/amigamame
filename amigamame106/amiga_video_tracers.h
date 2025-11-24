/******************************************************************************
 * amiga_video_tracers.h
 *
 *       ╔════════════════════════════════╗
 *       ║  ▓▒░ PIXEL TRACERS ░▒▓         ║
 *       ║  Fast Bit Blitting Magic!      ║
 *       ║  ┌──────────────────┐          ║
 *       ║  │ ROT0  ROT90  →   │          ║
 *       ║  │ ROT180 ROT270 ↻  │          ║
 *       ║  └──────────────────┘          ║
 *       ║   Every pixel traced!          ║
 *       ╚════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 *****************************************************************************/

#ifndef AMIGA_VIDEO_TRACERS_H_
#define AMIGA_VIDEO_TRACERS_H_

// Amiga
extern "C" {
    #include <exec/types.h>
}


// from mame driver.h
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


// These are used to describe source & destination bitmaps for all tracers
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

// actually optimize the 3 recursive calls to reach drawing routine
struct directDrawParams {
    directDrawScreen *screen;
    directDrawSource *source;
    LONG x1,  y1 , w,  h;
};


#endif

/* Every pixel in its right place! Mission accomplished!
 *       ʕ •ᴥ•ʔ  <-- Bear approved all rotations
 */
