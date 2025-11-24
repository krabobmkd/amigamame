/******************************************************************************
 * amiga_video_tracers_clut16.h
 *
 *       ╔════════════════════════════════╗
 *       ║  ▓▒░ CLUT16 TRACERS ░▒▓        ║
 *       ║  Color Lookup Table Magic!     ║
 *       ║  ┌──────────────────┐          ║
 *       ║  │ INDEX → COLOR    │          ║
 *       ║  │   0   → #FF0000  │          ║
 *       ║  │   1   → #00FF00  │          ║
 *       ║  │   ...            │          ║
 *       ║  └──────────────────┘          ║
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

#ifndef AMIGA_VIDEO_TRACERS_CLUT16_H_
#define AMIGA_VIDEO_TRACERS_CLUT16_H_

#include "amiga_video_tracers.h"

extern "C" {
// for 15b/16b target screens:

// name pattern: directDrawClut_SCREENPIXTYPE_CLUTTYPE_BMSOURCETYPE
// all these use 16b color index or RGB15, used by most games
void directDrawClut_UWORD_UWORD_UWORD(directDrawParams *p,UWORD *lut);
void directDrawClut_ULONG_ULONG_UWORD(directDrawParams *p,ULONG *lut);
void directDrawClut_type24_ULONG_UWORD(directDrawParams *p,ULONG *lut);
void directDrawClut_UBYTE_UBYTE_UWORD(directDrawParams *p,UBYTE *lut);
void directDraw_UBYTE_UBYTE_UWORD(directDrawParams *p);

void directDrawClut_UBYTE_UBYTE_UWORD(directDrawParams *p,UBYTE *lut);

// Special case: target screen is 8-bit, MAME screen is ARGB32, and uses 15-bit long CLUT.
void directDrawClut_UBYTE_UBYTE_ARGB32(directDrawParams *p,UBYTE *lut);

}
#endif

/* CLUT indexed perfectly! Palette perfection!
 *        /\
 *       /  \
 *      / || \  <-- Deer leaps through palettes
 *     |  ||  |
 */
