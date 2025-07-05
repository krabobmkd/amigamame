// vector.cpp
extern "C" {

#include <math.h>
#include "osinline.h"
#include "driver.h"
#include "vector.h"
#include "artwork.h"
#include <stdio.h>

#define LIMIT5(x) ((x < 0x1f)? x : 0x1f)
#define LIMIT8(x) ((x < 0xff)? x : 0xff)
#define Tcosin(x)   pTcosin[(x)]          /* adjust line width */

void vector_krb_hglow(void);
void vector_krb_fullglow(void);
 
extern int alloc_glowtemps_later;
extern int vector_xmin, vector_ymin, vector_xmax, vector_ymax; /* clipping area */
extern int vector_xminfp, vector_yminfp, vector_xmaxfp, vector_ymaxfp; /* clipping area */
extern float vector_scale_x;              /* scaling to screen */
extern float vector_scale_y;              /* scaling to screen */
extern int antialias;
extern mame_bitmap *vecbitmap;

extern UINT16 vecbeamconst_high;
extern UINT16 vecbeamconst_low;
extern UINT32* pTcosin;            /* adjust line width */

}
static UINT32 *glowtemp,* glowtempv;

static int prev_x1=0, prev_yy1=0, clipbits1=0;

void allocGlowTemp()
{
	UINT32 nbpixglowtemp = (vector_xmax-vector_xmin) * ((vector_ymax-vector_ymin)+1);
	glowtemp = (UINT32 *)auto_malloc(nbpixglowtemp * sizeof(UINT32));
	glowtempv = (UINT32 *)auto_malloc(nbpixglowtemp * sizeof(UINT32));

	alloc_glowtemps_later = 0;
}

// something for RGB  byte access
#ifdef LSB_FIRST
    #define RCDX 2
    #define GCDX 1
    #define BCDX 0
#else
    #define ACDX 0
    #define RCDX 1
    #define GCDX 2
    #define BCDX 3
#endif

/* very accurate but slow version 
void vector_krb_fullglow(void)
{
    if( Machine->color_depth != 32) return;

	if (alloc_glowtemps_later) allocGlowTemp();

    UINT32 cl=32; // means stock 64 pixels.
	// 32+32 64 -> div6
#define hgdivser 6

    // apply H & V glow on temp buffer without composition
    // do composition at the end.

    // hpass
    UINT32* glowbuf = glowtemp;
    for (INT32 y = vector_ymin; y < vector_ymax; y++)
    {
        UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
        //UINT32* glowbuf = glowtemp;
        INT32 r_ac=0;
        INT32 g_ac=0;
        INT32 b_ac=0;

        // accum nextread before screen
        for (UINT32 x = 0; x < cl; x++)
        {
            UINT32 cnext = prgb[x];
            r_ac += (cnext>>16);
            g_ac += (cnext>>8) & 0x0ff;
            b_ac += (cnext) & 0x0ff;
        }

        UINT32 x = 0;
        // left case, only accum next
        for (; x < cl; x++)
        {
            UINT32 cnext = prgb[x+cl];
            r_ac += cnext>>16;
            g_ac += (cnext>>8) & 0x0ff;
            b_ac += (cnext) & 0x0ff;

            UINT32 cr = (r_ac>>hgdivser);
            UINT32 cg = (g_ac>>hgdivser) ;
            UINT32 cb = (b_ac>>hgdivser) ;
            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
        }
        // center case,
        UINT32 xend = (vector_xmax-vector_xmin-cl);
        for (; x < xend; x++)
        {
            UINT32 cnext = prgb[x+cl];
            r_ac += cnext>>16;
            g_ac += (cnext>>8) & 0x0ff;
            b_ac += (cnext) & 0x0ff;

            UINT32 cprev = prgb[x-cl];
            r_ac -= cprev>>16;
            g_ac -= (cprev>>8) & 0x0ff;
            b_ac -= (cprev) & 0x0ff;

            UINT32 cr = (r_ac>>hgdivser);
            UINT32 cg = (g_ac>>hgdivser) ;
            UINT32 cb = (b_ac>>hgdivser) ;
            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
        }
        // right case,
        xend = (vector_xmax-vector_xmin);
        for (; x < xend; x++)
        {
            UINT32 cprev = prgb[x-cl];
            r_ac -= cprev>>16;
            g_ac -= (cprev>>8) & 0x0ff;
            b_ac -= (cprev) & 0x0ff;

            UINT32 cr = (r_ac>>hgdivser);
            UINT32 cg = (g_ac>>hgdivser) ;
            UINT32 cb = (b_ac>>hgdivser) ;
            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
        }
    } // end H pass
    // - - - -  - - - - -  -  V pass

	const UINT32 lb = vector_xmax- vector_xmin;
	const UINT32 cly = cl * lb;
#define vgdivser 6

	// source is glowtemp
	// dest is glowtempv , same size
	UINT32* prgb_l = glowtemp; // ((UINT32*)vecbitmap->line[ymin]) + xmin;
	UINT32* pdest_l = glowtempv;
	for (INT32 x = vector_xmin; x < vector_xmax; x++)
	{
		UINT32* prgb = prgb_l;
		UINT32* glowbuf = pdest_l;
		INT32 r_ac = 0;
		INT32 g_ac = 0;
		INT32 b_ac = 0;
		// - - - -
		// accum nextread before screen
		for (UINT32 y = 0; y < cly; y += lb)
		{
			UINT32 cnext = prgb[y];
			r_ac += (cnext >> 16);
			g_ac += (cnext >> 8) & 0x0ff;
			b_ac += (cnext) & 0x0ff;
		}
		UINT32 y = 0;
		// up case, only accum next
		for (; y < cly; y += lb)
		{
			UINT32 cnext = prgb[y + cly];
			r_ac += cnext >> 16;
			g_ac += (cnext >> 8) & 0x0ff;
			b_ac += (cnext) & 0x0ff;

			UINT32 cr = (r_ac >> vgdivser);
			UINT32 cg = (g_ac >> vgdivser);
			UINT32 cb = (b_ac >> vgdivser);
			glowbuf[y] = (cr << 16) | (cg << 8) | cb;
		}
		// center case,
		UINT32 yend = ((vector_ymax - vector_ymin) - cl) * lb;
		for (; y <= yend; y += lb)
		{
			UINT32 cnext = prgb[y + cly];
			r_ac += cnext >> 16;
			g_ac += (cnext >> 8) & 0x0ff;
			b_ac += (cnext) & 0x0ff;

			UINT32 cprev = prgb[y - cly];
			r_ac -= cprev >> 16;
			g_ac -= (cprev >> 8) & 0x0ff;
			b_ac -= (cprev) & 0x0ff;

			UINT32 cr = (r_ac >> vgdivser);
			UINT32 cg = (g_ac >> vgdivser);
			UINT32 cb = (b_ac >> vgdivser);
			glowbuf[y] = (cr << 16) | (cg << 8) | cb;
		}
		// down case,
		yend = (vector_ymax - vector_ymin) * lb;
		for (; y <= yend; y += lb)
		{
			UINT32 cprev = prgb[y - cly];
			r_ac -= cprev >> 16;
			g_ac -= (cprev >> 8) & 0x0ff;
			b_ac -= (cprev) & 0x0ff;

			UINT32 cr = (r_ac >> vgdivser);
			UINT32 cg = (g_ac >> vgdivser);
			UINT32 cb = (b_ac >> vgdivser);
			glowbuf[y] = (cr << 16) | (cg << 8) | cb;
		}
		// - - -
		prgb_l++;
		pdest_l++;
	}

    // - - - - composition
    glowbuf = glowtempv;

    for (INT32 y = vector_ymin; y < vector_ymax; y++)
    {
		if (vecbitmap->line[y] == NULL) continue;
        UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
        for (INT32 x = vector_xmin; x < vector_xmax; x++)
        {
            UINT32 chere = *prgb;
            UINT32 cglow = *glowbuf++;

            UINT32 cr = cglow>>16;
            UINT32 rh = chere>>16;
            if(cr>rh) rh=cr;

            UINT32 gh = (chere>>8) & 0x0ff;
            UINT32 cg = (cglow>>8) & 0x0ff;
            if(cg>gh) gh=cg;

            UINT32 bh = (chere) & 0x0ff;
            UINT32 cb = (cglow) & 0x0ff;
            if(cb>bh) bh=cb;

            *prgb++=(rh<<16)|(gh<<8)|bh;

        }
    }

}
*/
// pixel types for template and cache-like full glow version

// color accumulator
struct ColAcc {
    ColAcc(INT16 startval) {
        r=g=b=startval;
    }
    INT16 r,g,b;
    inline void add(UINT16 argb) {
        r += (argb >> 10);
        g += (argb >> 5) & 0x01f;
        b += (argb) & 0x01f;
    }
    inline void sub(UINT16 argb) {
        r -= (argb >> 10);
        g -= (argb >> 5) & 0x01f;
        b -= (argb) & 0x01f;
    }
    inline void add(UINT32 argb) {
        r += (argb >> 16)/*& 0x0ff*/;
        g += (argb >> 8) & 0x0ff;
        b += (argb) & 0x0ff;
    }
    inline void sub(UINT32 argb) {
        r -= (argb >> 16)/*& 0x0ff*/;
        g -= (argb >> 8) & 0x0ff;
        b -= (argb) & 0x0ff;
    }
    inline void divtorgb(UINT32 &o, const int shift) {
        UINT32 cr = (r >> shift);
        UINT32 cg = (g >> shift);
        UINT32 cb = (b >> shift);
        o = (cr << 16) | (cg << 8) | cb;
    }
    inline void divtorgb(UINT16& o, const int shift) {
        UINT32 cr = ((r) >> shift);
        UINT32 cg = ((g) >> shift);
        UINT32 cb = ((b) >> shift);
        o = (UINT16)((cr << 10) | (cg << 5) | cb);
    }
};
class GlowPix32 {
public:
    // get highest RGB
    void compose(UINT32 cglow)
    {
        UINT32 cr = cglow >> 16;
        UINT32 rh = _p >> 16;
        if (cr > rh) rh = cr;

        UINT32 gh = (_p >> 8) & 0x0ff;
        UINT32 cg = (cglow >> 8) & 0x0ff;
        if (cg > gh) gh = cg;

        UINT32 bh = (_p) & 0x0ff;
        UINT32 cb = (cglow) & 0x0ff;
        if (cb > bh) bh = cb;

        _p = (rh << 16) | (gh << 8) | bh;
    }

    UINT32 _p;
};
class GlowPix15 {
public:
    // get highest RGB
    void compose(UINT16 cglow)
    {
        UINT16 cr = cglow >> 10;
        UINT16 rh = _p >> 10;
        if (cr > rh) rh = cr;

        UINT16 gh = (_p >> 5) & 0x01f;
        UINT16 cg = (cglow >> 5) & 0x01f;
        if (cg > gh) gh = cg;

        UINT16 bh = (_p) & 0x01f;
        UINT16 cb = (cglow) & 0x01f;
        if (cb > bh) bh = cb;

        _p = (rh << 10) | (gh << 5) | bh;
    }

    UINT16 _p;
};
// ColT screen pixel type UINT32 UINT16 
template<typename ColT,typename TU,int shiftH,int shiftV,int startacc>
void vector_krb_fullglow2T(void)
{
	if (alloc_glowtemps_later) allocGlowTemp();

    UINT32 cl=32; // means stock 64 pixels.

    // pass1: H , pass 2:  V +composition in cache order

    // hpass
    TU* glowbuf = (TU*)glowtemp;
    for (INT32 y = vector_ymin; y < vector_ymax; y+=2)
    {
        TU* prgbh = ((TU*)vecbitmap->line[y])+vector_xmin;
        TU* prgbl = ((TU*)vecbitmap->line[y+1])+vector_xmin;
        //UINT32* glowbuf = glowtemp;
        ColAcc acc(startacc);

        // accum nextread before screen
        for (UINT32 x = 0; x < cl; x+=2)
        {
            acc.add(prgbh[x]);
            acc.add(prgbh[x + 1]);
            acc.add(prgbl[x]);
            acc.add(prgbl[x + 1]);
        }

        UINT32 x = 0;
        // left case, only accum next
        for (; x < cl; x+=2)
        {
            acc.add(prgbh[x+cl]);
            acc.add(prgbh[x+cl + 1]);
            acc.add(prgbl[x+cl]);
            acc.add(prgbl[x+cl + 1]);

            acc.divtorgb(*glowbuf, shiftH);
            glowbuf++;
        }
        // center case,
        UINT32 xend = (vector_xmax-vector_xmin-cl);
        for (; x < xend; x+=2)
        {
            acc.add(prgbh[x + cl]);
            acc.add(prgbh[x + cl + 1]);
            acc.add(prgbl[x + cl]);
            acc.add(prgbl[x + cl + 1]);

            acc.sub(prgbh[x - cl]);
            acc.sub(prgbh[x - cl + 1]);
            acc.sub(prgbl[x - cl]);
            acc.sub(prgbl[x - cl + 1]);

            acc.divtorgb(*glowbuf, shiftH);
            glowbuf++;
        }
        // right case,
        xend = (vector_xmax-vector_xmin);
        for (; x < xend; x+=2)
        {
            acc.sub(prgbh[x - cl]);
            acc.sub(prgbh[x - cl + 1]);
            acc.sub(prgbl[x - cl]);
            acc.sub(prgbl[x - cl + 1]);

            acc.divtorgb(*glowbuf, shiftH);
            glowbuf++;
        }
    } // end H pass
    // - - - -  - - - - -  -  V pass & composition
 
	const INT32 clh = cl>>1;

#define gsubw 8
#define gsubh 16

#define GLOWONLYH 0
    // - - - A: preaccum
    ColAcc*pVacc = (ColAcc*)glowtempv;
    UINT32 yl= (vector_ymax - vector_ymin);

    TU* prgb_hg = (TU *)glowtemp; 
    INT32 glowmod = (vector_xmax - vector_xmin) >> 1;
    //init preacc start line
    for (INT32 x = 0; x < glowmod; x++)
    {
        ColAcc acc(startacc);
        int idx = x;
        for (INT32 y = 0; y < clh; y++)
        {
            acc.add(prgb_hg[idx]);
            idx += glowmod;
        } // end preaccum V
        *pVacc++ = acc;
    }
    // - - - B: up pass
    const int clm = clh * glowmod;
    const int vbmmod = vecbitmap->rowpixels;
    // up pass, just add again
    INT32 by;
    for (by = 0; by < (clh<<1)/*vector_ymax*/; by += gsubh)
    {
        ColAcc* pVacc = (ColAcc*)glowtempv;

        // final screen to compose
        ColT* pvecbm_c = ((ColT*)vecbitmap->line[by]) + vector_xmin;
        for (INT32 bx = vector_xmin; bx < glowmod; bx++)
        {
            ColAcc acc= *pVacc;
            ColT* pvecbm = pvecbm_c;

            int addidx = ((by>>1) * glowmod)+clm + bx;
            for (INT32 y = 0; y < gsubh; y += 2)
            {            
#if GLOWONLYH
                TU argb = prgb_hg[addidx- clm];
                addidx += glowmod;
#else
                acc.add(prgb_hg[addidx]);
                addidx += glowmod;

                TU argb;
                acc.divtorgb(argb, shiftV);
#endif
                pvecbm[0].compose(argb);
                pvecbm[1].compose(argb);
                pvecbm[vbmmod].compose(argb);
                pvecbm[vbmmod+1].compose(argb);

                pvecbm += vbmmod*2;

            }
            *pVacc++ = acc;
            pvecbm_c+=2;
        } // end bigx
        
    } // end bigy

    // - - - C: center pass
    for (; by < (vector_ymax-clh); by += gsubh)
    {
        ColAcc* pVacc = (ColAcc*)glowtempv;        
        ColT* pvecbm_c = ((ColT*)vecbitmap->line[by]) + vector_xmin; // final screen to compose
        for (INT32 bx = 0; bx < glowmod; bx++)
        {
            ColAcc acc= *pVacc;
            ColT* pvecbm = pvecbm_c;
            int subidx = ((by>>1)* glowmod) -clm + bx;
            int addidx = ((by>>1) * glowmod)+clm + bx;
            for (INT32 y = 0; y < gsubh; y += 2)
            {
#if GLOWONLYH
                TU argb = prgb_hg[addidx - clm];
                addidx += glowmod;
#else
                acc.sub(prgb_hg[subidx]);
                subidx += glowmod;

                acc.add(prgb_hg[addidx]);
                addidx += glowmod;
                TU argb;
                acc.divtorgb(argb, shiftV);
#endif


                pvecbm[0].compose(argb);
                pvecbm[1].compose(argb);
                pvecbm[vbmmod].compose(argb);
                pvecbm[vbmmod+1].compose(argb);

                pvecbm += vbmmod*2;
            }
            *pVacc++ = acc;
            pvecbm_c+=2;
        } // end bigx
    } // end bigy
    // - - - D: down pass
    for (; by < vector_ymax ; by += gsubh)
    {
        ColAcc* pVacc = (ColAcc*)glowtempv;
        ColT* pvecbm_c = ((ColT*)vecbitmap->line[by]) + vector_xmin;        // final screen to compose
        for (INT32 bx = 0; bx < glowmod; bx++)
        {
            ColAcc acc = *pVacc;
            ColT* pvecbm = pvecbm_c;
            int subidx = ((by >> 1) * glowmod) - clm + bx;
            for (INT32 y = 0; y < gsubh; y += 2)
            {
#if GLOWONLYH
                TU argb = prgb_hg[subidx + clm];
                subidx += glowmod;
#else
                acc.sub(prgb_hg[subidx]);
                subidx += glowmod;

                TU argb;
                acc.divtorgb(argb, shiftV);
#endif
                pvecbm[0].compose(argb);
                pvecbm[1].compose(argb);
                pvecbm[vbmmod].compose(argb);
                pvecbm[vbmmod + 1].compose(argb);

                pvecbm += vbmmod * 2;
            }
            *pVacc++ = acc;
            pvecbm_c += 2;
        } // end bigx
    } // end bigy


}

void vector_krb_fullglow2(void)
{
    if (Machine->color_depth == 32) vector_krb_fullglow2T<GlowPix32,UINT32,6+2,4,3>();
    else if (Machine->color_depth == 15) vector_krb_fullglow2T<GlowPix15, UINT16, 6 + 2, 4,13>();

}

// just horizontal, much simpler
template<typename ColT,typename TU,int startacc>
void vector_krb_hglowT(void)
{
	if (alloc_glowtemps_later) allocGlowTemp();

    const UINT32 cl=32; // means stock 64 pixels.
	// 32+32 64 -> div6
#define hhgdivser 7

    // hpass

    for (INT32 y = vector_ymin; y < vector_ymax; y++)
    {
        TU* prgb = ((TU*)vecbitmap->line[y])+vector_xmin;
        ColAcc acc(startacc);

        TU* glowbuf = (TU *)glowtemp;
        // accum nextread before screen
        for (UINT32 x = 0; x < cl; x++)
        {
            TU cnext = prgb[x];
            acc.add(cnext);
        }

        UINT32 x = 0;
        // left case, only accum next
        for (; x < cl; x++)
        {
            acc.add(prgb[x+cl]);
            acc.divtorgb(*glowbuf,hhgdivser);
            glowbuf++;
        }
        // center case,
        UINT32 xend = (vector_xmax-vector_xmin-cl);
        for (; x < xend; x++)
        {
            acc.add(prgb[x+cl]);
            acc.sub(prgb[x-cl]);
            acc.divtorgb(*glowbuf,hhgdivser);
            glowbuf++;

        }
        // right case,
        xend = (vector_xmax-vector_xmin);
        for (; x < xend; x++)
        {
            acc.sub(prgb[x-cl]);
            acc.divtorgb(*glowbuf,hhgdivser);
            glowbuf++;
        }

        // compose line
        glowbuf = (TU *)glowtemp;
        ColT *pscreen = ((ColT*)vecbitmap->line[y])+vector_xmin;
        for (INT32 x = vector_xmin; x < vector_xmax; x++)
        {
            // TU chere = *prgb;
            pscreen->compose(*glowbuf++);
            pscreen++;
        }


    } // y loop
}
void vector_krb_hglow(void)
{
    if (Machine->color_depth == 32) vector_krb_hglowT<GlowPix32,UINT32,1>();
    else if (Machine->color_depth == 15) vector_krb_hglowT<GlowPix15, UINT16,13>();

}
static inline int vec_multbeam(UINT32 parm2)
{
    int result;
    /*
    result = vecbeamconst_low * ((UINT16)parm2);
    result >>= 16;
    result += vecbeamconst_low * (parm2 >> 16);
    result += vecbeamconst_high * ((UINT16)parm2);
    result >>= 16;
    result += vecbeamconst_high * (parm2 >> 16);
    */
    result = vecbeamconst_high * ((UINT16)parm2);
    result >>= 16;
    result += vecbeamconst_high * (parm2 >> 16);
    return(result);
}

/* can be be replaced by an assembly routine in osinline.h */
#ifndef vec_div
static inline int vec_div(int parm1, int parm2)
{
    if ((parm2 >> 12))
    {
        parm1 = (parm1 << 4) / (parm2 >> 12);
        if (parm1 > 0x00010000)
            return(0x00010000);
        if (parm1 < -0x00010000)
            return(-0x00010000);
        return(parm1);
    }
    return(0x00010000);
}
#endif


class Pix15 {
public:
    UINT16 _r, _g, _b;
    UINT16 _rt, _gt, _bt;
    void setcol(rgb_t col) {
        _r = ((col >>(16+3)));
        _g = ((col >>(8+3)) & 0x1f);
        _b = ((col >>(3)) & 0x1f);
    }
    void tint(UINT8 t) {
        _rt = (_r * t) >> 8;
        _gt = (_g * t) >> 8;
        _bt = (_b * t) >> 8;
    }
    void aa_pixel(int x, int y)
    {
        UINT16* pdst = ((UINT16*)vecbitmap->line[y]) + x;
        UINT32 dst = *pdst;
        *pdst = LIMIT5(_b + (dst & 0x1f))
            | (LIMIT5(_g + ((dst >> 5) & 0x1f)) << 5)
            | (LIMIT5(_r + (dst >> 10)) << 10);

    }
    void aa_pixeltint(int x, int y)
    {
        UINT16* pdst = ((UINT16*)vecbitmap->line[y]) + x;
        UINT32 dst = *pdst;
        *pdst = LIMIT5(_bt + (dst & 0x1f))
            | (LIMIT5(_gt + ((dst >> 5) & 0x1f)) << 5)
            | (LIMIT5(_rt + (dst >> 10)) << 10);

    }

};
class Pix32 {
public:
    UINT16 _r, _g, _b;
    UINT16 _rt, _gt, _bt;
    void setcol(rgb_t col) {
        _r = RGB_RED(col);
        _g = RGB_GREEN(col);
        _b = RGB_BLUE(col);
    }
    void tint(UINT8 t) {
        _rt = (_r * t) >> 8;
        _gt = (_g * t) >> 8;
        _bt = (_b * t) >> 8;
    }
    void aa_pixel(int x, int y)
    {
        UINT32* pdst = ((UINT32*)vecbitmap->line[y]) + x;
        UINT32 dst = *pdst;
        *pdst = LIMIT8(_b + (dst & 0xff))
            | (LIMIT8(_g + ((dst >> 8) & 0xff)) << 8)
            | (LIMIT8(_r + (dst >> 16)) << 16);

    }
    void aa_pixeltint(int x, int y)
    {
        UINT32* pdst = ((UINT32*)vecbitmap->line[y]) + x;
        UINT32 dst = *pdst;
        *pdst = LIMIT8(_bt + (dst & 0xff))
            | (LIMIT8(_gt + ((dst >> 8) & 0xff)) << 8)
            | (LIMIT8(_rt + (dst >> 16)) << 16);

    }

};

// only to be used when: 1 is in, 2 is out.
static inline void clipxmin(int &x1,int &y1,int & clipbits1, 
                            int& x2, int& y2, int& clipbits2 )
{
    int c = (x1 - x2) >> 16;
    if (c < 1) c = 1;
    y2 = y1 + (((y2 - y1) >> 8) * ((x1 -vector_xminfp ) >> 8)) / c;

    x2 = vector_xminfp;

}
static inline void clipxmax(int& x1, int& y1, int& clipbits1,
    int& x2, int& y2, int& clipbits2)
{
    int xmax = vector_xmaxfp - (1 << 16);
     
    int c = (x2 - x1) >> 16;
    if (c < 1) c = 1;
    y2 = y1 + (((y2 - y1) >> 8) * (( xmax -x1) >> 8)) / c;

    x2 = xmax;

}
static inline void clipymin(int& x1, int& y1, int& clipbits1,
    int& x2, int& y2, int& clipbits2)
{
    int c = (y1 - y2) >> 16;
    if (c <1) c = 1;
    x2 -=  (((x2-x1)>>8)*((vector_yminfp -y2)>>8)) / c;
    y2 = vector_yminfp;
    clipbits2 &= ~(4|1|2); // 4 remove ymin + recheck both x
    // y done before xclip, recheck x clipbits
    clipbits2 |= (int)(x2 < vector_xminfp)
        | ((x2 >= vector_xmaxfp) ? 2 : 0);
}
static inline void clipymax(int& x1, int& y1, int& clipbits1,
    int& x2, int& y2, int& clipbits2)
{
    int ymax = vector_ymaxfp - (1 << 16);

    int c = (y2 - y1) >> 16;
    if (c < 1) c = 1;
    x2 -= (((x2 - x1) >> 8) * ((y2 -ymax) >> 8)) / c;

    y2 = ymax;

    clipbits2 &= ~(8|1|2);

    // y done before xclip, recheck x clipbits
    clipbits2 |= (int)(x2 < vector_xminfp)
        | ((x2 >= vector_xmaxfp) ? 2 : 0);
}
template<class pixel,bool b_antialias>
void vector_draw_toT(pixel &pix, point* curpoint)
{
    int x2 = curpoint->x;
    int y2 = curpoint->y;
    int intensity = curpoint->intensity;
    rgb_t(*color_callback)(void) = curpoint->callback;

    unsigned char a1;
    int dx, dy, sx, sy, cx, cy, width;
    int x1= prev_x1, yy1= prev_yy1;
    int xx, yy;
     int oredcb;

    x2 = (int)(vector_scale_x * x2);
    y2 = (int)(vector_scale_y * y2);

    if (!b_antialias)
    {    /* [2] adjust cords if needed */
        x2 = (x2 + 0x8000);
        y2 = (y2 + 0x8000);
    }
   // note max is excluded
    int clipbits2 =
        (int)(x2 < vector_xminfp)
        | ((x2 >= vector_xmaxfp) ? 2 : 0)
        | ((y2 < vector_yminfp) ? 4 : 0)
        | ((y2 >= vector_ymaxfp) ? 8 : 0)
        ;

    int x2_preclip = x2;
    int y2_preclip = y2;
    int cb2_preclip = clipbits2;
    /* [3] handle color and intensity */

    if (intensity == 0) goto end_draw;
    // fast clip 
    if ((clipbits2 & clipbits1) != 0) goto end_draw; // means 2 points outside of one of the border.

    oredcb = (clipbits2 | clipbits1);
    if ( oredcb != 0 )
    {
        // means one of the 2 points outside.
        if (clipbits1 & 4) clipymin(x2, y2, clipbits2, x1, yy1, clipbits1);
        else if (clipbits2 & 4) clipymin(prev_x1, prev_yy1, clipbits1, x2, y2, clipbits2);

        if (clipbits1 & 8) clipymax(x2, y2, clipbits2, x1, yy1, clipbits1);
        else if (clipbits2 & 8) clipymax(prev_x1, prev_yy1, clipbits1, x2, y2, clipbits2);
        // can happens at that level
        if ((clipbits2 & clipbits1) != 0) goto end_draw; // means 2 points outside of one of the border.

        if (clipbits1 & 1) clipxmin(x2, y2, clipbits2, x1, yy1, clipbits1);
        else if (clipbits2 & 1) clipxmin(prev_x1, prev_yy1, clipbits1, x2, y2, clipbits2);

        if (clipbits1 & 2) clipxmax(x2, y2, clipbits2, x1, yy1, clipbits1);
        else if (clipbits2 & 2) clipxmax(prev_x1, prev_yy1, clipbits1, x2, y2, clipbits2);

    }

    pix.setcol(Tinten(intensity, curpoint->col)); 

    /* [4] draw line */

    if (b_antialias)
    {
        /* draw an anti-aliased line */
        dx = abs(x1 - x2);
        dy = abs(yy1 - y2);

        if (dx >= dy)
        {
            sx = ((x1 <= x2) ? 1 : -1);
            sy = vec_div(y2 - yy1, dx);
            if (sy < 0)
                dy--;
            x1 >>= 16;
            xx = x2 >> 16;
            width = vec_multbeam( Tcosin(abs(sy) >> 5));
            //if (!beam_diameter_is_one)
            yy1 -= width >> 1; /* start back half the diameter */
            for (;;)
            {
               // if (color_callback) col = Tinten(intensity, (*color_callback)());
                dx = width;    /* init diameter of beam */
                dy = yy1 >> 16;
                pix.tint(0xff - ((UINT8)(yy1 >> 8)));
                pix.aa_pixeltint(x1, dy++);
                dx -= 0x10000 - (0xffff & yy1); /* take off amount plotted */
                a1 = (dx >> 8);   /* calc remainder pixel */
                dx >>= 16;                   /* adjust to pixel (solid) count */
                while (dx--)                 /* plot rest of pixels */
                    pix.aa_pixel(x1, dy++);
                pix.tint(a1);
                pix.aa_pixeltint(x1, dy);
                if (x1 == xx) break;
                x1 += sx;
                yy1 += sy;
            }
        }
        else
        {
            sy = ((yy1 <= y2) ? 1 : -1);
            sx = vec_div(x2 - x1, dy);
            if (sx < 0)
                dx--;
            yy1 >>= 16;
            yy = y2 >> 16;
            width = vec_multbeam( Tcosin(abs(sx) >> 5));
            //if (!beam_diameter_is_one)
            x1 -= width >> 1; /* start back half the width */
            for (;;)
            {
              //  if (color_callback) col = Tinten(intensity, (*color_callback)());
                dy = width;    /* calc diameter of beam */
                dx = x1 >> 16;
                pix.tint(0x00ff - ((UINT8)(x1 >> 8)));
                pix.aa_pixeltint(dx++, yy1);
                dy -= 0x10000 - (0xffff & x1); /* take off amount plotted */
                a1 = (dy >> 8);   /* remainder pixel */
                dy >>= 16;                   /* adjust to pixel (solid) count */
                while (dy--)                 /* plot rest of pixels */
                {
                    pix.aa_pixel(dx++, yy1);
                }
                pix.tint(a1);
                pix.aa_pixeltint(dx, yy1);
                if (yy1 == yy) break;
                yy1 += sy;
                x1 += sx;
            }
        }
    }
    else /* use good old Bresenham for non-antialiasing 980317 BW */
    {
        // now we consider x2 x1 are <<16 even without aa.
        int bx2 = x2>>16;
        int by2 = y2>>16;
        int bx1 = x1 >> 16;
        int by1 = yy1 >> 16;

        dx = abs(bx1 - bx2);
        dy = abs(by1 - by2);
        sx = (bx1 <= bx2) ? 1 : -1;
        sy = (by1 <= by2) ? 1 : -1;
        cx = dx / 2;
        cy = dy / 2;

        if (dx >= dy)
        {
            for (;;)
            {
               // if (color_callback) col = Tinten(intensity, (*color_callback)());
                pix.aa_pixel(bx1, by1);
                if (bx1 == bx2) break;
                bx1 += sx;
                cx -= dy;
                if (cx < 0)
                {
                    by1 += sy;
                    cx += dx;
                }
            }
        }
        else
        {
            for (;;)
            {
               // if (color_callback) col = Tinten(intensity, (*color_callback)());
                pix.aa_pixel(bx1, by1);
                if (by1 == by2) break;
                by1 += sy;
                cy -= dx;
                if (cy < 0)
                {
                    bx1 += sx;
                    cy += dy;
                }
            }
        }
    }

end_draw:

    prev_x1 = x2_preclip;
    prev_yy1 = y2_preclip;
    clipbits1 = cb2_preclip;
    return;

    
}

void vector_draw_to15(point* curpoint)
{
    Pix15 pix;
    vector_draw_toT<Pix15,false>(pix,curpoint);
}
void vector_draw_to32(point* curpoint)
{
    Pix32 pix;
    vector_draw_toT<Pix32, false>(pix, curpoint);    
}
void vector_draw_to15aa(point* curpoint)
{
    Pix15 pix;
    vector_draw_toT<Pix15, true>(pix, curpoint);
}
void vector_draw_to32aa(point* curpoint)
{
    Pix32 pix;
    vector_draw_toT<Pix32, true>(pix, curpoint);
}
