/*
 * amiga_video_tracers_24.h
 * Purpose: 24 bit target tracer (PIXFMT_RGB24 / PIXFMT_BGR24 screens)
 *
 * The graphics card is behind the Zorro bus: each byte written is one bus
 * write. 24 bit pixels written as 3 bytes cost 3 writes per pixel, much
 * slower than the 16 and 32 bit tracers. Here:
 *
 * - every row goes out as aligned 32 bit words: 0 to 3 pixels as bytes up to
 *   a 4 byte boundary (the address moves by 3), then 4 pixels are 12 bytes,
 *   3 aligned words, then 0 to 3 last pixels as bytes.
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
 * Bytes in the order of the former type24: r, g, b.
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 */

#ifndef AMIGA_VIDEO_TRACERS_24_H_
#define AMIGA_VIDEO_TRACERS_24_H_

#include <stddef.h>
#include "amiga_video_tracers.h"

// big endian stores (the Amiga), a host test may replace them
#ifndef TRACER24_STORE32
#define TRACER24_STORE32(p, v) (*(ULONG *)(p) = (ULONG)(v))
#endif
#define TRACER24_STORE8(p, v) (*(UBYTE *)(p) = (UBYTE)(v))

// one word or byte on the NL lines, bpr bytes apart
template<int NL> static inline void tracer24Put32(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER24_STORE32(d, v);
    if (NL > 1) TRACER24_STORE32(d + bpr, v);
    if (NL > 2) TRACER24_STORE32(d + 2 * bpr, v);
}
template<int NL> static inline void tracer24Put8(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER24_STORE8(d, v);
    if (NL > 1) TRACER24_STORE8(d + bpr, v);
    if (NL > 2) TRACER24_STORE8(d + 2 * bpr, v);
}
// one pixel as 3 bytes r g b, from 0x??RRGGBB
template<int NL> static inline void tracer24PutPixel(UBYTE *d, ULONG bpr, ULONG c)
{
    tracer24Put8<NL>(d, bpr, c >> 16);
    tracer24Put8<NL>(d + 1, bpr, c >> 8);
    tracer24Put8<NL>(d + 2, bpr, c);
}

// One screen row of w pixels from gen(), called once per pixel from left to
// right, returning 0x??RRGGBB (the high byte is ignored).
template<int NL, typename GEN>
static inline void tracerRow24(UBYTE *d, LONG w, ULONG bpr, GEN &gen)
{
    while (w > 0 && ((ULONG)(size_t)d & 3) != 0) {
        tracer24PutPixel<NL>(d, bpr, gen());
        d += 3;
        w--;
    }
    while (w >= 4) {
        // in this order: gen() reads the source left to right
        ULONG a = gen() & 0x00ffffff;
        ULONG b = gen() & 0x00ffffff;
        ULONG c = gen() & 0x00ffffff;
        ULONG e = gen() & 0x00ffffff;

        // a0 a1 a2 b0 | b1 b2 c0 c1 | c2 e0 e1 e2
        tracer24Put32<NL>(d, bpr, (a << 8) | (b >> 16));
        tracer24Put32<NL>(d + 4, bpr, (b << 16) | (c >> 8));
        tracer24Put32<NL>(d + 8, bpr, (c << 24) | e);
        d += 12;
        w -= 4;
    }
    while (w-- > 0) {
        tracer24PutPixel<NL>(d, bpr, gen());
        d += 3;
    }
}

// The whole drawing, from a source of SRCTYPE pixels, getColor(pixel)
// giving 0x??RRGGBB.
template<typename SRCTYPE, bool C_swapXY, typename GETCOLOR>
void directDraw24TT(directDrawParams *p, GETCOLOR getColor)
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
    pscreenbm += (y1 * scbpr) + x1 * 3;

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
            tracerRow24<3>(pscreenbm, w, scbpr, gen);
        } else if (nl == 2) {
            tracerRow24<2>(pscreenbm, w, scbpr, gen);
        } else {
            tracerRow24<1>(pscreenbm, w, scbpr, gen);
        }
        vh += addh * nl;
        pscreenbm += scbpr * nl;
        hh += nl;
    }
}

template<typename SRCTYPE, typename GETCOLOR>
void directDraw24T(directDrawParams *p, GETCOLOR getColor)
{
    if (p->source->_swapFlags & ORIENTATION_SWAP_XY) {
        directDraw24TT<SRCTYPE, true>(p, getColor);
    } else {
        directDraw24TT<SRCTYPE, false>(p, getColor);
    }
}

#endif
