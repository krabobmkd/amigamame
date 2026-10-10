/*
 * amiga_video_tracers_core.h
 * Purpose: the one drawing loop of all tracers: scaling, flips, swaps,
 *          clipping, and the screen rows written as aligned 32 bit words.
 *
 * The graphics card is behind the Zorro bus: each write is one bus write,
 * whatever its size. Every row goes out as aligned 32 bit words:
 *
 * - tracerRow8:  0 to 3 bytes up to a 4 byte boundary, then 4 pixels per
 *                word, then 0 to 3 last bytes.
 * - tracerRow16: 1 pixel as a 16 bit word if the row starts at 2 mod 4, then
 *                2 pixels per word, then 1 last pixel if one is left.
 * - tracerRow24: 0 to 3 pixels as bytes up to a 4 byte boundary (the address
 *                moves by 3), then 4 pixels as 12 bytes, 3 aligned words,
 *                then 0 to 3 last pixels as bytes. Bytes in order r, g, b.
 * - tracerRow32: 1 pixel per word.
 *
 * The "3 lines trick": with a vertical zoom, up to 3 consecutive screen
 * lines show the same source line. They are written together, the source
 * line read and converted once (only when the screen bytes per row is a
 * multiple of 4: aligned words on every line).
 *
 * Flips and swaps only change where the source pixels are read: vxStart,
 * addw (along a screen row) and vh, addh (from row to row), in 16.16 fixed
 * point. So the screen row is always written from left to right, and
 * clipping on any side is the same for all orientations: the clipped pixels
 * are skipped by moving vxStart and vh, the drawn ones are exactly the same
 * as without clipping. This is what allows drawing a window one layers
 * ClipRect at a time.
 *
 * Without scaling the step is exactly 1, and a row reads the source with a
 * moving pointer.
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 */

#ifndef AMIGA_VIDEO_TRACERS_CORE_H_
#define AMIGA_VIDEO_TRACERS_CORE_H_

#include <stddef.h>
#include "amiga_video_tracers.h"

// big endian stores (the Amiga), a host test may replace them
#ifndef TRACER_STORE32
#define TRACER_STORE32(p, v) (*(ULONG *)(p) = (ULONG)(v))
#endif
#ifndef TRACER_STORE16
#define TRACER_STORE16(p, v) (*(UWORD *)(p) = (UWORD)(v))
#endif
#ifndef TRACER_STORE8
#define TRACER_STORE8(p, v) (*(UBYTE *)(p) = (UBYTE)(v))
#endif

// one long, word or byte on the NL lines, bpr bytes apart
template<int NL> static inline void tracerPut32(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER_STORE32(d, v);
    if (NL > 1) TRACER_STORE32(d + bpr, v);
    if (NL > 2) TRACER_STORE32(d + 2 * bpr, v);
}
template<int NL> static inline void tracerPut16(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER_STORE16(d, v);
    if (NL > 1) TRACER_STORE16(d + bpr, v);
    if (NL > 2) TRACER_STORE16(d + 2 * bpr, v);
}
template<int NL> static inline void tracerPut8(UBYTE *d, ULONG bpr, ULONG v)
{
    TRACER_STORE8(d, v);
    if (NL > 1) TRACER_STORE8(d + bpr, v);
    if (NL > 2) TRACER_STORE8(d + 2 * bpr, v);
}

// Row writers: one screen row of w pixels from gen(), called once per pixel
// from left to right, returning the screen pixel in the low bits.

struct tracerRow8 {
    enum { BPP = 1 };
    template<int NL, typename GEN>
    static inline void row(UBYTE *d, LONG w, ULONG bpr, GEN &gen)
    {
        while (w > 0 && ((ULONG)(size_t)d & 3) != 0) {
            tracerPut8<NL>(d, bpr, gen());
            d++;
            w--;
        }
        while (w >= 4) {
            // in this order: gen() reads the source left to right
            ULONG a = gen();
            ULONG b = gen() & 0xff;
            ULONG c = gen() & 0xff;
            ULONG e = gen() & 0xff;
            tracerPut32<NL>(d, bpr, (a << 24) | (b << 16) | (c << 8) | e);
            d += 4;
            w -= 4;
        }
        while (w-- > 0) {
            tracerPut8<NL>(d, bpr, gen());
            d++;
        }
    }
};

struct tracerRow16 {
    enum { BPP = 2 };
    template<int NL, typename GEN>
    static inline void row(UBYTE *d, LONG w, ULONG bpr, GEN &gen)
    {
        if (w > 0 && ((ULONG)(size_t)d & 2) != 0) {
            tracerPut16<NL>(d, bpr, gen());
            d += 2;
            w--;
        }
        while (w >= 2) {
            ULONG a = gen();
            ULONG b = gen() & 0x0000ffff;
            tracerPut32<NL>(d, bpr, (a << 16) | b);
            d += 4;
            w -= 2;
        }
        if (w > 0) {
            tracerPut16<NL>(d, bpr, gen());
        }
    }
};

// one pixel as 3 bytes r g b, from 0x??RRGGBB
template<int NL> static inline void tracerPut24(UBYTE *d, ULONG bpr, ULONG c)
{
    tracerPut8<NL>(d, bpr, c >> 16);
    tracerPut8<NL>(d + 1, bpr, c >> 8);
    tracerPut8<NL>(d + 2, bpr, c);
}

struct tracerRow24 {
    enum { BPP = 3 };
    template<int NL, typename GEN>
    static inline void row(UBYTE *d, LONG w, ULONG bpr, GEN &gen)
    {
        while (w > 0 && ((ULONG)(size_t)d & 3) != 0) {
            tracerPut24<NL>(d, bpr, gen());
            d += 3;
            w--;
        }
        while (w >= 4) {
            ULONG a = gen() & 0x00ffffff;
            ULONG b = gen() & 0x00ffffff;
            ULONG c = gen() & 0x00ffffff;
            ULONG e = gen() & 0x00ffffff;

            // a0 a1 a2 b0 | b1 b2 c0 c1 | c2 e0 e1 e2
            tracerPut32<NL>(d, bpr, (a << 8) | (b >> 16));
            tracerPut32<NL>(d + 4, bpr, (b << 16) | (c >> 8));
            tracerPut32<NL>(d + 8, bpr, (c << 24) | e);
            d += 12;
            w -= 4;
        }
        while (w-- > 0) {
            tracerPut24<NL>(d, bpr, gen());
            d += 3;
        }
    }
};

struct tracerRow32 {
    enum { BPP = 4 };
    template<int NL, typename GEN>
    static inline void row(UBYTE *d, LONG w, ULONG bpr, GEN &gen)
    {
        while (w-- > 0) {
            tracerPut32<NL>(d, bpr, gen());
            d += 4;
        }
    }
};

template<typename ROW, typename GEN>
static inline void tracerRows(int nl, UBYTE *d, LONG w, ULONG bpr, GEN &gen)
{
    if (nl == 3) {
        ROW::template row<3>(d, w, bpr, gen);
    } else if (nl == 2) {
        ROW::template row<2>(d, w, bpr, gen);
    } else {
        ROW::template row<1>(d, w, bpr, gen);
    }
}

// The whole drawing, from a source of SRCTYPE pixels, getColor(pixel)
// giving the screen pixel for ROW.
template<typename ROW, typename SRCTYPE, bool C_swapXY, typename GETCOLOR>
void directDrawTT(directDrawParams *p, GETCOLOR getColor)
{
    directDrawScreen *screen = p->screen;
    directDrawSource *source = p->source;
    LONG x1 = p->x1, y1 = p->y1;
    LONG w = p->w, h = p->h;

    UWORD wsobpr = source->_bpr / sizeof(SRCTYPE);
    ULONG scbpr = screen->_bpr;     // bytes

    SRCTYPE *psourcebm = (SRCTYPE *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

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
    // the scale is from the whole drawing, before clipping
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

    // clipping: right and bottom just shorten, left and top also skip the
    // source pixels of the clipped screen pixels.
    if (y1 + h > screen->_clipY2) h = (screen->_clipY2) - y1;
    if (x1 + w > screen->_clipX2) w = (screen->_clipX2) - x1;

    if (x1 < screen->_clipX1) {
        LONG dx = screen->_clipX1 - x1;
        w -= dx;
        vxStart += dx * addw;
        x1 = screen->_clipX1;
    }
    if (y1 < screen->_clipY1) {
        LONG dy = screen->_clipY1 - y1;
        h -= dy;
        vh += dy * addh;
        y1 = screen->_clipY1;
    }
    if (w <= 0 || h <= 0) return;

    UBYTE *pscreenbm = (UBYTE *)screen->_base;
    pscreenbm += (y1 * scbpr) + x1 * ROW::BPP;

    // lines written together: aligned words on each of them
    const bool multi = (scbpr & 3) == 0;
    // no horizontal scaling: read the source row with a moving pointer
    const bool unitX = (addw == 0x10000 || addw == -0x10000);
    const LONG stepX = (addw > 0) ? (LONG)vmod : -(LONG)vmod;

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
        if (unitX) {
            const SRCTYPE *ps = psoline + ((UWORD)(vxStart >> 16)) * vmod;
            auto gen = [&]() -> ULONG {
                ULONG c = getColor(*ps);
                ps += stepX;
                return c;
            };
            tracerRows<ROW>(nl, pscreenbm, w, scbpr, gen);
        } else {
            LONG vx = vxStart;
            auto gen = [&]() -> ULONG {
                ULONG c = getColor(psoline[((UWORD)(vx >> 16)) * vmod]);
                vx += addw;
                return c;
            };
            tracerRows<ROW>(nl, pscreenbm, w, scbpr, gen);
        }
        vh += addh * nl;
        pscreenbm += scbpr * nl;
        hh += nl;
    }
}

template<typename ROW, typename SRCTYPE, typename GETCOLOR>
void directDrawT(directDrawParams *p, GETCOLOR getColor)
{
    if (p->source->_swapFlags & ORIENTATION_SWAP_XY) {
        directDrawTT<ROW, SRCTYPE, true>(p, getColor);
    } else {
        directDrawTT<ROW, SRCTYPE, false>(p, getColor);
    }
}

#endif
