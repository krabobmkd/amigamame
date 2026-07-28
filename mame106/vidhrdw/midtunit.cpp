/*************************************************************************

    midtunit.cpp - Midway T-unit DMA sprite blitter.

    This replaces the old macro-generated C blitter family (DMA_DRAW_FUNC /
    DMA_DRAW_FUNC_BODY / DECLARE_BLITTER_SET, formerly in vidhrdw/midtunit.c)
    with C++ templates. Same idea as tilemap.cpp: plain C functions
    (midtunit_dma_draw) are the only thing the C side (midtunit.c) calls;
    everything else here is internal.

    Why templates instead of the macro wall:
      - one readable function body instead of ~9000 characters of macro,
        much easier to single-step/inspect in a debugger.
      - the per-pixel "is this x column inside the clip window" test that
        the macro version ran on every single pixel is, for the common
        unscaled case, hoisted to a once-per-row decision (see need_clip
        below) instead of a branch inside the innermost loop.
      - the DMA command's bpp is almost always 8 on real T-unit hardware:
        the graphics ROM loader (machine/midtunit.c, init_tunit_generic)
        always deinterleaves the ROM into one full byte per pixel before
        the DMA engine ever touches it, regardless of the source ROM's
        native bit depth. A dedicated bpp==8 template specialization lets
        the compiler fold the per-pixel mask/step math to compile-time
        constants instead of recomputing them from dma_state.bpp on every
        pixel. The fully generic (runtime bpp) path is kept as a fallback
        for correctness on any case that isn't actually bpp==8.

**************************************************************************/

extern "C" {
	#include "driver.h"
	#include "midtunit.h"
}
#include "midtunit_dma.h"


enum PixMode { PM_SKIP = 0, PM_COLOR = 1, PM_COPY = 2 };


/*** fast pixel extractor - matches the portable EXTRACTGEN() fallback used
     on this (big-endian, non-powerc) target; 'mask' selects the width. ***/
static inline int extract_word(const UINT8 *base, UINT32 o, int mask)
{
	unsigned v = (unsigned)base[o >> 3] | ((unsigned)base[(o >> 3) + 1] << 8);
	return (int)((v >> (o & 7)) & (unsigned)mask);
}


template<int ZERO, int NONZERO>
static inline void write_pixel(UINT16 *d, int sx, const UINT8 *base, UINT32 o, int mask, UINT16 pal, UINT16 color)
{
	if (ZERO == NONZERO)
	{
		/* zero and non-zero pixels are handled identically */
		if (ZERO == PM_COLOR)
			d[sx] = color;
		else if (ZERO == PM_COPY)
			d[sx] = (UINT16)(extract_word(base, o, mask) | pal);
	}
	else
	{
		int pixel = extract_word(base, o, mask);
		if (pixel)
		{
			if (NONZERO == PM_COLOR)
				d[sx] = color;
			else if (NONZERO == PM_COPY)
				d[sx] = (UINT16)(pixel | pal);
		}
		else
		{
			if (ZERO == PM_COLOR)
				d[sx] = color;
			else if (ZERO == PM_COPY)
				d[sx] = pal;
		}
	}
}


/*** the core blitter, one template instantiation per (xflip, skip, scale,
     zero mode, nonzero mode, bpp8-fast) combination - direct translation
     of the old DMA_DRAW_FUNC_BODY macro, plus the clip-test hoist. ***/
template<bool XFLIP, bool SKIP, bool SCALE, int ZERO, int NONZERO, bool BPP8>
static void dma_draw_tmpl()
{
	int height = dma_state.height << 8;
	const UINT8 *base = midyunit_gfx_rom;
	UINT32 offset = dma_state.offset;
	UINT16 pal = dma_state.palette;
	UINT16 color = (UINT16)(pal | dma_state.color);
	int sy = dma_state.ypos, iy = 0, ty;
	int bpp = BPP8 ? 8 : (int)dma_state.bpp;
	int mask = BPP8 ? 0xff : ((1 << bpp) - 1);
	int xstep = SCALE ? dma_state.xstep : 0x100;
	int leftclip = dma_state.leftclip, rightclip = dma_state.rightclip;

	/* loop over the height */
	while (iy < height)
	{
		int startskip = dma_state.startskip << 8;
		int endskip = dma_state.endskip << 8;
		int width = dma_state.width << 8;
		int sx = dma_state.xpos, ix = 0, tx;
		UINT32 o = offset;
		int pre = 0, post = 0;
		UINT16 *d;

		/* handle skipping */
		if (SKIP)
		{
			UINT8 value = (UINT8)extract_word(base, o, 0xff);
			o += 8;

			/* adjust for preskip */
			pre = (value & 0x0f) << (dma_state.preskip + 8);
			tx = pre / xstep;
			if (XFLIP)
				sx = (sx - tx) & XPOSMASK;
			else
				sx = (sx + tx) & XPOSMASK;
			ix += tx * xstep;

			/* adjust for postskip */
			post = ((value >> 4) & 0x0f) << (dma_state.postskip + 8);
			width -= post;
			endskip -= post;
		}

		/* handle Y clipping (equivalent of "goto clipy" skipping the whole row) */
		if (sy >= dma_state.topclip && sy <= dma_state.botclip)
		{
			/* handle start skip */
			if (ix < startskip)
			{
				tx = ((startskip - ix) / xstep) * xstep;
				ix += tx;
				o += (tx >> 8) * bpp;
			}

			/* handle end skip */
			if ((width >> 8) > dma_state.width - dma_state.endskip)
				width = (dma_state.width - dma_state.endskip) << 8;

			/* determine destination pointer */
			d = &local_videoram[sy * 512];

			/* --- clip test hoist ---
			   for the unscaled case, x advances by exactly +-1 per pixel, so
			   the whole set of columns this row touches is a single known
			   run (barring wraparound past the 1024-entry coordinate space,
			   which we don't bother splitting - falls back to the safe
			   per-pixel test below, same as it always did). Deciding this
			   once per row instead of once per pixel is the actual win;
			   it's still exactly the same test, just hoisted. */
			bool need_clip = true;
			if (!SCALE)
			{
				int n = (width - ix) >> 8;
				if (n <= 0)
					need_clip = false;
				else
				{
					int lo, hi;
					if (!XFLIP) { lo = sx; hi = sx + n - 1; }
					else        { hi = sx; lo = sx - (n - 1); }
					if (lo >= 0 && hi <= XPOSMASK)
						need_clip = !(lo >= leftclip && hi <= rightclip);
				}
			}

			if (!need_clip)
			{
				/* loop until we draw the entire width - no per-pixel clip test */
				while (ix < width)
				{
					write_pixel<ZERO, NONZERO>(d, sx, base, o, mask, pal, color);

					if (XFLIP)
						sx = (sx - 1) & XPOSMASK;
					else
						sx = (sx + 1) & XPOSMASK;

					if (!SCALE)
					{
						ix += 0x100;
						o += bpp;
					}
					else
					{
						tx = ix >> 8;
						ix += xstep;
						tx = (ix >> 8) - tx;
						o += bpp * tx;
					}
				}
			}
			else
			{
				/* loop until we draw the entire width - per-pixel clip test */
				while (ix < width)
				{
					if (sx >= leftclip && sx <= rightclip)
						write_pixel<ZERO, NONZERO>(d, sx, base, o, mask, pal, color);

					if (XFLIP)
						sx = (sx - 1) & XPOSMASK;
					else
						sx = (sx + 1) & XPOSMASK;

					if (!SCALE)
					{
						ix += 0x100;
						o += bpp;
					}
					else
					{
						tx = ix >> 8;
						ix += xstep;
						tx = (ix >> 8) - tx;
						o += bpp * tx;
					}
				}
			}
		}

		/* advance to the next row */
		if (dma_state.yflip)
			sy = (sy - 1) & YPOSMASK;
		else
			sy = (sy + 1) & YPOSMASK;

		if (!SCALE)
		{
			iy += 0x100;
			width = dma_state.width;
			if (SKIP)
			{
				offset += 8;
				width -= (pre + post) >> 8;
				if (width > 0) offset += width * bpp;
			}
			else
				offset += width * bpp;
		}
		else
		{
			ty = iy >> 8;
			iy += dma_state.ystep;
			ty = (iy >> 8) - ty;
			if (!SKIP)
				offset += ty * dma_state.width * bpp;
			else if (ty--)
			{
				o = offset + 8;
				width = dma_state.width - ((pre + post) >> 8);
				if (width > 0) o += width * bpp;
				while (ty--)
				{
					UINT8 value = (UINT8)extract_word(base, o, 0xff);
					o += 8;
					pre = (value & 0x0f) << dma_state.preskip;
					post = ((value >> 4) & 0x0f) << dma_state.postskip;
					width = dma_state.width - pre - post;
					if (width > 0) o += width * bpp;
				}
				offset = o;
			}
		}
	}
}


/*** runtime-to-compile-time dispatch cascade: narrows one bool axis per
     call until everything is a template parameter and the fully
     specialized dma_draw_tmpl<> can be called. 8 (zero,nonzero) pairs x
     2 xflip x 2 skip x 2 scale x 2 bpp8 = 128 instantiations total. ***/
template<int ZERO, int NONZERO, bool XFLIP, bool SKIP, bool SCALE>
static inline void dispatch_bpp(bool bpp8)
{
	if (bpp8)
		dma_draw_tmpl<XFLIP, SKIP, SCALE, ZERO, NONZERO, true>();
	else
		dma_draw_tmpl<XFLIP, SKIP, SCALE, ZERO, NONZERO, false>();
}

template<int ZERO, int NONZERO, bool XFLIP, bool SKIP>
static inline void dispatch_scale(bool scale, bool bpp8)
{
	if (scale)
		dispatch_bpp<ZERO, NONZERO, XFLIP, SKIP, true>(bpp8);
	else
		dispatch_bpp<ZERO, NONZERO, XFLIP, SKIP, false>(bpp8);
}

template<int ZERO, int NONZERO, bool XFLIP>
static inline void dispatch_skip(bool skip, bool scale, bool bpp8)
{
	if (skip)
		dispatch_scale<ZERO, NONZERO, XFLIP, true>(scale, bpp8);
	else
		dispatch_scale<ZERO, NONZERO, XFLIP, false>(scale, bpp8);
}

template<int ZERO, int NONZERO>
static inline void dispatch_xflip(bool xflip, bool skip, bool scale, bool bpp8)
{
	if (xflip)
		dispatch_skip<ZERO, NONZERO, true>(skip, scale, bpp8);
	else
		dispatch_skip<ZERO, NONZERO, false>(skip, scale, bpp8);
}


/*** C-callable entry point, replaces the old dma_draw_xxx[32] table lookup.
     'command' is the raw DMA_COMMAND register value; use_skip/use_scale are
     the two mode selectors midtunit_dma_w() already computes. Decodes the
     same bit layout the old 32-entry dispatch tables encoded:
       bit0 = blit zero pixels (copy)      bit1 = blit non-zero pixels (copy)
       bit2 = blit zero pixels as color    bit3 = blit non-zero pixels as color
       bit4 = xflip
     matching the tables' 4 row groups (no color / color-0 / color-nonzero /
     fill) and the xflip/non-xflip halves exactly. ***/
extern "C" void midtunit_dma_draw(int command, int use_skip, int use_scale)
{
	bool B0 = (command & 0x01) != 0;
	bool B1 = (command & 0x02) != 0;
	bool colorZero = (command & 0x04) != 0;
	bool colorNonzero = (command & 0x08) != 0;
	bool xflip = (command & 0x10) != 0;
	bool skip = use_skip != 0;
	bool scale = use_scale != 0;
	bool bpp8 = (dma_state.bpp == 8);

	int zero_mode    = colorZero    ? PM_COLOR : (B0 ? PM_COPY : PM_SKIP);
	int nonzero_mode = colorNonzero ? PM_COLOR : (B1 ? PM_COPY : PM_SKIP);

	/* matches dma_draw_none: a true no-op, don't touch any state */
	if (zero_mode == PM_SKIP && nonzero_mode == PM_SKIP)
		return;

	if (zero_mode == PM_COPY && nonzero_mode == PM_SKIP)
		dispatch_xflip<PM_COPY, PM_SKIP>(xflip, skip, scale, bpp8);
	else if (zero_mode == PM_SKIP && nonzero_mode == PM_COPY)
		dispatch_xflip<PM_SKIP, PM_COPY>(xflip, skip, scale, bpp8);
	else if (zero_mode == PM_COLOR && nonzero_mode == PM_SKIP)
		dispatch_xflip<PM_COLOR, PM_SKIP>(xflip, skip, scale, bpp8);
	else if (zero_mode == PM_SKIP && nonzero_mode == PM_COLOR)
		dispatch_xflip<PM_SKIP, PM_COLOR>(xflip, skip, scale, bpp8);
	else if (zero_mode == PM_COPY && nonzero_mode == PM_COPY)
		dispatch_xflip<PM_COPY, PM_COPY>(xflip, skip, scale, bpp8);
	else if (zero_mode == PM_COLOR && nonzero_mode == PM_COLOR)
		dispatch_xflip<PM_COLOR, PM_COLOR>(xflip, skip, scale, bpp8);
	else if (zero_mode == PM_COLOR && nonzero_mode == PM_COPY)
		dispatch_xflip<PM_COLOR, PM_COPY>(xflip, skip, scale, bpp8);
	else /* zero_mode == PM_COPY && nonzero_mode == PM_COLOR */
		dispatch_xflip<PM_COPY, PM_COLOR>(xflip, skip, scale, bpp8);
}
