/*
 * amiga_video_tracers_16.h
 * Purpose: 16 bit target tracer (15 and 16 bit screens) with 32 bit writes
 *
 * The graphics card is behind the Zorro bus: each write is one bus write.
 * Writing 2 pixels as one aligned 32 bit word halves the writes. Here:
 *
 * - every row goes out as aligned 32 bit words: 1 pixel as a 16 bit word if
 *   the row starts at 2 mod 4, then pairs of pixels as aligned 32 bit words,
 *   then 1 last pixel as a 16 bit word if one is left.
 * - the "3 lines trick": with a vertical zoom, up to 3 consecutive screen
 *   lines show the same source line. They are written together, the source
 *   line read and converted once (only when the screen bytes per row is a
 *   multiple of 4: aligned words on every line).
 *
 * Flips and swaps only change where the source pixels are read: the screen
 * row is always written from left to right, so both apply to every
 * orientation. Same setup, scaling and clipping as directDrawScaleClutTT(),
 * also used without scaling (the step is then exactly 1).
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 */

#ifndef AMIGA_VIDEO_TRACERS_16_H_
#define AMIGA_VIDEO_TRACERS_16_H_

#include <stddef.h>
#include "amiga_video_tracers.h"

// big endian stores (the Amiga), a host test may replace them
#ifndef TRACER16_STORE32
#define TRACER16_STORE32(p, v) (*(ULONG *)(p) = (ULONG)(v))
#endif
#ifndef TRACER16_STORE16
#define TRACER16_STORE16(p, v) (*(UWORD *)(p) = (UWORD)(v))
#endif

// one long or word on the NL lines, bpr bytes apart
template<int NL> static inline void tracer16Put32(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER16_STORE32(d, v);
    if (NL > 1) TRACER16_STORE32(d + bpr, v);
    if (NL > 2) TRACER16_STORE32(d + 2 * bpr, v);
}
template<int NL> static inline void tracer16Put16(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER16_STORE16(d, v);
    if (NL > 1) TRACER16_STORE16(d + bpr, v);
    if (NL > 2) TRACER16_STORE16(d + 2 * bpr, v);
}

// One screen row of w pixels from gen(), called once per pixel from left to
// right, returning the 16 bit screen pixel (the high word is ignored).
template<int NL, typename GEN>
static inline void tracerRow16(UBYTE *d, LONG w, ULONG bpr, GEN &gen)
{
    if (w > 0 && ((ULONG)(size_t)d & 2) != 0) {
        tracer16Put16<NL>(d, bpr, gen());
        d += 2;
        w--;
    }
    while (w >= 2) {
        // in this order: gen() reads the source left to right
        ULONG a = gen();
        ULONG b = gen() & 0x0000ffff;
        tracer16Put32<NL>(d, bpr, (a << 16) | b);
        d += 4;
        w -= 2;
    }
    if (w > 0) {
        tracer16Put16<NL>(d, bpr, gen());
    }
}

// The whole drawing, from a source of SRCTYPE pixels, getColor(pixel)
// giving the 16 bit screen pixel.
template<typename SRCTYPE, bool C_swapXY, typename GETCOLOR>
void directDraw16TT(directDrawParams *p, GETCOLOR getColor)
{
    directDrawScreen *screen = p->screen;
    directDrawSource *source = p->source;
    LONG x1 = p->x1, y1 = p->y1;
    LONG w = p->w, h = p->h;

    UWORD wsobpr = source->_bpr / sizeof(SRCTYPE);
    ULONG scbpr = screen->_bpr;     // bytes

    SRCTYPE *psourcebm = (SRCTYPE *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    UBYTE *pscreenbm = (UBYTE *)screen->_base;
    pscreenbm += (y1 * scbpr) + x1 * 2;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if (sourceHeight <= 0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if (sourceWidth <= 0) return;

    LONG addw, addh;
    LONG vh;
    LONG vxStart;
    UWORD hmod;
    UWORD vmod;

    if (w <= 0 || h <= 0) return;
    if (C_swapXY) {
        addw = (sourceHeight << 16) / w; // swapped source size
        addh = (sourceWidth << 16) / h;
    } else {
        addw = (sourceWidth << 16) / w;
        addh = (sourceHeight << 16) / h;
    }
    vh = 0;
    vxStart = 0;
    hmod = (C_swapXY) ? 1 : wsobpr;
    vmod = (C_swapXY) ? wsobpr : 1;
    if (source->_swapFlags & ORIENTATION_FLIP_Y) {
        vh = (C_swapXY) ? (sourceWidth << 16) - 1 : (sourceHeight << 16) - 1;
        addh = -addh;
    }
    if (source->_swapFlags & ORIENTATION_FLIP_X) {
        vxStart = (C_swapXY) ? (sourceHeight << 16) - 1 : (sourceWidth << 16) - 1;
        addw = -addw;
    }

    // clipping, as directDrawScaleClutTT()
    if (y1 + h > screen->_clipY2) h = (screen->_clipY2) - y1;
    if (x1 + w > screen->_clipX2) w = (screen->_clipX2) - x1;

    if (x1 < screen->_clipX1) { w -= (screen->_clipX1 - x1); x1 = screen->_clipX1; }
    if (y1 < screen->_clipY1) { h -= (screen->_clipY1 - y1); y1 = screen->_clipY1; }

    // lines written together: aligned words on each of them
    const bool multi = (scbpr & 3) == 0;

    LONG hh = 0;
    while (hh < h) {
        SRCTYPE *psoline = psourcebm + hmod * (vh >> 16);
        int nl = 1;

        if (multi && hh + 1 < h && psourcebm + hmod * ((vh + addh) >> 16) == psoline) {
            nl = 2;
            if (hh + 2 < h && psourcebm + hmod * ((vh + addh + addh) >> 16) == psoline) {
                nl = 3;
            }
        }
        LONG vx = vxStart;
        auto gen = [&]() -> ULONG {
            ULONG c = getColor(psoline[((UWORD)(vx >> 16)) * vmod]);
            vx += addw;
            return c;
        };
        if (nl == 3) {
            tracerRow16<3>(pscreenbm, w, scbpr, gen);
        } else if (nl == 2) {
            tracerRow16<2>(pscreenbm, w, scbpr, gen);
        } else {
            tracerRow16<1>(pscreenbm, w, scbpr, gen);
        }
        vh += addh * nl;
        pscreenbm += scbpr * nl;
        hh += nl;
    }
}

template<typename SRCTYPE, typename GETCOLOR>
void directDraw16T(directDrawParams *p, GETCOLOR getColor)
{
    if (p->source->_swapFlags & ORIENTATION_SWAP_XY) {
        directDraw16TT<SRCTYPE, true>(p, getColor);
    } else {
        directDraw16TT<SRCTYPE, false>(p, getColor);
    }
}

#endif
