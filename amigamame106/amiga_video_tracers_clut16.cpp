/*
 * amiga_video_tracers_clut16.cpp
 * Purpose: 16-bit CLUT (Color Look-Up Table) pixel format tracer implementation
 *
 * ╔════════════════════════════════════════════════════════════════════════╗
 * ║              🎨 CLUT16 PIXEL TRACER - 16-BIT COLORS 🖌️                ║
 * ║  ┌──────────────────────────────────────────────────────────────┐    ║
 * ║  │                                                               │    ║
 * ║  │   MAME Pixels  ──►  CLUT Conversion  ──►  16-bit Screen     │    ║
 * ║  │                                                               │    ║
 * ║  │   ┌────────────┐       ┌────────────┐       ┌────────────┐  │    ║
 * ║  │   │  UWORD/    │  ───► │   Lookup   │  ───► │  RGB15/16  │  │    ║
 * ║  │   │  ARGB32    │       │   Table    │       │   Output   │  │    ║
 * ║  │   └────────────┘       └────────────┘       └────────────┘  │    ║
 * ║  │                                                               │    ║
 * ║  │   Handles scaling, rotation (90°/180°/270°), and clipping   │    ║
 * ║  └──────────────────────────────────────────────────────────────┘    ║
 * ║      High-performance templated pixel conversion for Amiga!           ║
 * ╚════════════════════════════════════════════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "amiga_video_tracers_clut16.h"
#include "amiga_video_tracers_core.h"

// this file is just to implement template calls...
// the loop itself, scaling, rotations and clipping: amiga_video_tracers_core.h

struct typeARGB32ToRGB15 {
    static inline UWORD conv(ULONG argb) {
        return ((argb>>(16+3-10))&0x7c00) |
               ((argb>>(8+3-5))&0x03e0) |
               ((argb>>3)&0x001f);
    }
};

void directDrawClut_UWORD_UWORD_UWORD(directDrawParams *p,UWORD *lut)
{
    directDrawT<tracerRow16,UWORD>(p, [lut](UWORD i) -> ULONG { return lut[i]; });
}
void directDrawClut_ULONG_ULONG_UWORD(directDrawParams *p,ULONG *lut)
{
    directDrawT<tracerRow32,UWORD>(p, [lut](UWORD i) -> ULONG { return lut[i]; });
}
void directDrawClut_type24_ULONG_UWORD(directDrawParams *p,ULONG *lut)
{
    directDrawT<tracerRow24,UWORD>(p, [lut](UWORD i) -> ULONG { return lut[i]; });
}
void directDrawClut_UBYTE_UBYTE_UWORD(directDrawParams *p,UBYTE *lut)
{
    directDrawT<tracerRow8,UWORD>(p, [lut](UWORD i) -> ULONG { return lut[i]; });
}

// - - --
void directDraw_UBYTE_UBYTE_UWORD(directDrawParams *p)
{
    directDrawT<tracerRow8,UWORD>(p, [](UWORD i) -> ULONG { return i; });
}

// - - - -
// Special case: target screen is 8-bit, MAME screen is ARGB32, and uses long CLUT.
void directDrawClut_UBYTE_UBYTE_ARGB32(directDrawParams *p,UBYTE *lut)
{
    directDrawT<tracerRow8,ULONG>(p, [lut](ULONG argb) -> ULONG {
        return lut[typeARGB32ToRGB15::conv(argb)];
    });
}

/*
 * All pixels traced and converted perfectly! 🎨
 *      /\___/\
 *     ( =^.^= )  <- This chameleon changes colors with every pixel!
 *      (") (")_/
 *        U U
 */
