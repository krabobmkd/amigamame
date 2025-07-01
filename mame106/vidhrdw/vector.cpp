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
extern float vector_scale_x;              /* scaling to screen */
extern float vector_scale_y;              /* scaling to screen */
extern int antialias;
extern mame_bitmap *vecbitmap;

extern UINT16 vecbeamconst_high;
extern UINT16 vecbeamconst_low;
extern UINT32* pTcosin;            /* adjust line width */

}
static UINT32 *glowtemp,* glowtempv;



void allocGlowTemp()
{
	UINT32 nbpixglowtemp = (vector_xmax-vector_xmin) * ((vector_ymax-vector_ymin)+1);
	glowtemp = (UINT32 *)auto_malloc(nbpixglowtemp * sizeof(UINT32));
	glowtempv = (UINT32 *)auto_malloc(nbpixglowtemp * sizeof(UINT32));

	alloc_glowtemps_later = 0;
}

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



//#define GLOWPERBYTE 1
#ifdef GLOWPERBYTE
void vector_krb_hglow(void)
{
    if( Machine->color_depth != 32) return;

	if (alloc_glowtemps_later) allocGlowTemp();

    UINT32 cl=32; // means stock 64 pixels.
	// 32+32 64 -> div6
#define hhgdivser 7

    // apply H & V glow on temp buffer without composition
    // do composition at the end.

    // hpass

    for (UINT32 y = vector_ymin; y < vector_ymax; y++)
    {
        UINT8* prgb = ((UINT8*)vecbitmap->line[y])+vector_xmin;
        INT32 r_ac=0;
        INT32 g_ac=0;
        INT32 b_ac=0;
        UINT8* glowbuf = (UINT8* )glowtemp;
        // accum nextread before screen
        for (UINT32 x = 0; x < cl*4; x+=4)
        {
            r_ac += prgb[x+RCDX];
            g_ac += prgb[x+GCDX];
            b_ac += prgb[x+BCDX];
        }

        UINT32 x = 0;
        // left case, only accum next
        for (; x < cl*4; x+=4)
        {
            r_ac += prgb[x+cl*4+RCDX];
            g_ac += prgb[x+cl*4+GCDX];
            b_ac += prgb[x+cl*4+BCDX];

            *glowbuf++ = (r_ac>>hhgdivser);
            *glowbuf++ = (g_ac>>hhgdivser);
            *glowbuf++ = (b_ac>>hhgdivser);
        }
        // center case,
        UINT32 xend = (vector_xmax-vector_xmin-cl);
        for (; x < xend; x++)
        {
//            UINT32 cnext = prgb[x+cl];
//            r_ac += cnext>>16;
//            g_ac += (cnext>>8) & 0x0ff;
//            b_ac += (cnext) & 0x0ff;

//            UINT32 cprev = prgb[x-cl];
//            r_ac -= cprev>>16;
//            g_ac -= (cprev>>8) & 0x0ff;
//            b_ac -= (cprev) & 0x0ff;

//            UINT32 cr = (r_ac>>hhgdivser);
//            UINT32 cg = (g_ac>>hhgdivser) ;
//            UINT32 cb = (b_ac>>hhgdivser) ;
//            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
        }
        // right case,
        xend = (vector_xmax-vector_xmin);
        for (; x < xend; x++)
        {
            UINT32 cprev = prgb[x-cl];
            r_ac -= cprev>>16;
            g_ac -= (cprev>>8) & 0x0ff;
            b_ac -= (cprev) & 0x0ff;

            UINT32 cr = (r_ac>>hhgdivser);
            UINT32 cg = (g_ac>>hhgdivser) ;
            UINT32 cb = (b_ac>>hhgdivser) ;
            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
        }

        // compose line
        glowbuf = (UINT8 *)glowtemp;
        prgb = (UINT8*)(((UINT32*)vecbitmap->line[y])+vector_xmin);
        for (UINT32 x = vector_xmin; x < vector_xmax; x++)
        {
            UINT32 chere = *prgb;
            UINT32 cglow = *glowbuf++;

            UINT32 cr = cglow;
            UINT32 rh = chere;
            if(cr>rh) rh=cr;
            rh >>=16;

            UINT32 gh = (chere>>8) & 0x0ff;
            UINT32 cg = (cglow>>8) & 0x0ff;
            if(cg>gh) gh=cg;

            UINT32 bh = (chere) & 0x0ff;
            UINT32 cb = (cglow) & 0x0ff;
            if(cb>bh) bh=cb;

            *prgb++=(rh<<16)|(gh<<8)|bh;
        }


    } // y loop
}
#else
void vector_krb_hglow(void)
{
    if( Machine->color_depth != 32) return;

	if (alloc_glowtemps_later) allocGlowTemp();

    UINT32 cl=32; // means stock 64 pixels.
	// 32+32 64 -> div6
#define hhgdivser 7

    // apply H & V glow on temp buffer without composition
    // do composition at the end.

    // hpass

    for (UINT32 y = vector_ymin; y < vector_ymax; y++)
    {
        UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
        //UINT32* glowbuf = glowtemp;
        INT32 r_ac=0;
        INT32 g_ac=0;
        INT32 b_ac=0;
        UINT32* glowbuf = glowtemp;
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

            UINT32 cr = (r_ac>>hhgdivser);
            UINT32 cg = (g_ac>>hhgdivser) ;
            UINT32 cb = (b_ac>>hhgdivser) ;
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

            UINT32 cr = (r_ac>>hhgdivser);
            UINT32 cg = (g_ac>>hhgdivser) ;
            UINT32 cb = (b_ac>>hhgdivser) ;
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

            UINT32 cr = (r_ac>>hhgdivser);
            UINT32 cg = (g_ac>>hhgdivser) ;
            UINT32 cb = (b_ac>>hhgdivser) ;
            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
        }

        // compose line
        glowbuf = glowtemp;
        prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
        for (UINT32 x = vector_xmin; x < vector_xmax; x++)
        {
            UINT32 chere = *prgb;
            UINT32 cglow = *glowbuf++;

            UINT32 cr = cglow;
            UINT32 rh = chere;
            if(cr>rh) rh=cr;
            rh >>=16;

            UINT32 gh = (chere>>8) & 0x0ff;
            UINT32 cg = (cglow>>8) & 0x0ff;
            if(cg>gh) gh=cg;

            UINT32 bh = (chere) & 0x0ff;
            UINT32 cb = (cglow) & 0x0ff;
            if(cb>bh) bh=cb;

            *prgb++=(rh<<16)|(gh<<8)|bh;
        }


    } // y loop
}
#endif
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
    for (UINT32 y = vector_ymin; y < vector_ymax; y++)
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
	for (UINT32 x = vector_xmin; x < vector_xmax; x++)
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

    for (UINT32 y = vector_ymin; y < vector_ymax; y++)
    {
		if (vecbitmap->line[y] == NULL) continue;
        UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
        for (UINT32 x = vector_xmin; x < vector_xmax; x++)
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

// - - -
// omre cache-pleasing approch, by tiles, with border stacking for accumulators.
//void vector_krb_dreadfullglow(void)
//{
//    if( Machine->color_depth != 32) return;

//#define HgSubW 16
//#define HgSubH 8


//	if (alloc_glowtemps_later) allocGlowTemp();

//    UINT32 cl=32; // means stock 64 pixels.
//	// 32+32 64 -> div6
//#define hgdivser 6

//    // apply H & V glow on temp buffer without composition
//    // do composition at the end.

//    // vertical pre-accumulation to
//    for (UINT32 mx = vector_xmin; mx < vector_xmax; mx+=HgSubW)
//    {
//    }

//    // hpass
//    UINT32* glowbuf = glowtemp;
//    for (UINT32 my = vector_ymin; my < vector_ymax; my+=HgSubH)
//    {
//        UINT16 Haccums[HgSubH * 3]; // for 3 for RGB.
//        // pre-accumulation

//        for (UINT32 mx = vector_xmin; mx < vector_xmax; mx+=HgSubW)
//        {


//            // pre accum


//            for (UINT32 y = my; y < vector_ymax; y++)
//            {
//            }
//        } // end main x
//    } //end main y


//        UINT8* prgb = (UINT8 *)(((UINT32*)vecbitmap->line[y])+vector_xmin);
//        //UINT32* glowbuf = glowtemp;
//        INT16 r_ac=0;
//        INT16 g_ac=0;
//        INT16 b_ac=0;

//        // accum nextread before screen
//        for (UINT32 x = 0; x < cl*4; x+=4)
//        {
//            r_ac += prgb[x+RCDX];
//            g_ac += prgb[x+GCDX];
//            b_ac += prgb[x+BCDX];
//        }

//        UINT32 x = 0;
//        // left case, only accum next
//        for (; x < cl*4; x+=4)
//        {
//            r_ac += prgb[x+cl*4+RCDX];
//            g_ac += prgb[x+cl*4+GCDX];
//            b_ac += prgb[x+cl*4+BCDX];

//            UINT32 cr = (r_ac>>hgdivser);
//            UINT32 cg = (g_ac>>hgdivser) ;
//            UINT32 cb = (b_ac>>hgdivser) ;
//            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
//        }
//        // center case,
//        UINT32 xend = (vector_xmax-vector_xmin-cl);
//        for (; x < xend; x++)
//        {
//            UINT32 cnext = prgb[x+cl];
//            r_ac += cnext>>16;
//            g_ac += (cnext>>8) & 0x0ff;
//            b_ac += (cnext) & 0x0ff;

//            UINT32 cprev = prgb[x-cl];
//            r_ac -= cprev>>16;
//            g_ac -= (cprev>>8) & 0x0ff;
//            b_ac -= (cprev) & 0x0ff;

//            UINT32 cr = (r_ac>>hgdivser);
//            UINT32 cg = (g_ac>>hgdivser) ;
//            UINT32 cb = (b_ac>>hgdivser) ;
//            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
//        }
//        // right case,
//        xend = (vector_xmax-vector_xmin);
//        for (; x < xend; x++)
//        {
//            UINT32 cprev = prgb[x-cl];
//            r_ac -= cprev>>16;
//            g_ac -= (cprev>>8) & 0x0ff;
//            b_ac -= (cprev) & 0x0ff;

//            UINT32 cr = (r_ac>>hgdivser);
//            UINT32 cg = (g_ac>>hgdivser) ;
//            UINT32 cb = (b_ac>>hgdivser) ;
//            *glowbuf++ = (cr<<16)|(cg<<8)|cb;
//        }
//    } // end H pass
//    // - - - -  - - - - -  -  V pass

//	const UINT32 lb = vector_xmax- vector_xmin;
//	const UINT32 cly = cl * lb;
//#define vgdivser 6

//	// source is glowtemp
//	// dest is glowtempv , same size
//	UINT32* prgb_l = glowtemp; // ((UINT32*)vecbitmap->line[ymin]) + xmin;
//	UINT32* pdest_l = glowtempv;
//	for (UINT32 x = vector_xmin; x < vector_xmax; x++)
//	{
//		UINT32* prgb = prgb_l;
//		UINT32* glowbuf = pdest_l;
//		INT32 r_ac = 0;
//		INT32 g_ac = 0;
//		INT32 b_ac = 0;
//		// - - - -
//		// accum nextread before screen
//		for (UINT32 y = 0; y < cly; y += lb)
//		{
//			UINT32 cnext = prgb[y];
//			r_ac += (cnext >> 16);
//			g_ac += (cnext >> 8) & 0x0ff;
//			b_ac += (cnext) & 0x0ff;
//		}
//		UINT32 y = 0;
//		// up case, only accum next
//		for (; y < cly; y += lb)
//		{
//			UINT32 cnext = prgb[y + cly];
//			r_ac += cnext >> 16;
//			g_ac += (cnext >> 8) & 0x0ff;
//			b_ac += (cnext) & 0x0ff;

//			UINT32 cr = (r_ac >> vgdivser);
//			UINT32 cg = (g_ac >> vgdivser);
//			UINT32 cb = (b_ac >> vgdivser);
//			glowbuf[y] = (cr << 16) | (cg << 8) | cb;
//		}
//		// center case,
//		UINT32 yend = ((vector_ymax - vector_ymin) - cl) * lb;
//		for (; y <= yend; y += lb)
//		{
//			UINT32 cnext = prgb[y + cly];
//			r_ac += cnext >> 16;
//			g_ac += (cnext >> 8) & 0x0ff;
//			b_ac += (cnext) & 0x0ff;

//			UINT32 cprev = prgb[y - cly];
//			r_ac -= cprev >> 16;
//			g_ac -= (cprev >> 8) & 0x0ff;
//			b_ac -= (cprev) & 0x0ff;

//			UINT32 cr = (r_ac >> vgdivser);
//			UINT32 cg = (g_ac >> vgdivser);
//			UINT32 cb = (b_ac >> vgdivser);
//			glowbuf[y] = (cr << 16) | (cg << 8) | cb;
//		}
//		// down case,
//		yend = (vector_ymax - vector_ymin) * lb;
//		for (; y <= yend; y += lb)
//		{
//			UINT32 cprev = prgb[y - cly];
//			r_ac -= cprev >> 16;
//			g_ac -= (cprev >> 8) & 0x0ff;
//			b_ac -= (cprev) & 0x0ff;

//			UINT32 cr = (r_ac >> vgdivser);
//			UINT32 cg = (g_ac >> vgdivser);
//			UINT32 cb = (b_ac >> vgdivser);
//			glowbuf[y] = (cr << 16) | (cg << 8) | cb;
//		}
//		// - - -
//		prgb_l++;
//		pdest_l++;
//	}

//    // - - - - composition
//    glowbuf = glowtempv;

//    for (UINT32 y = vector_ymin; y < vector_ymax; y++)
//    {
//		if (vecbitmap->line[y] == NULL) continue;
//        UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
//        for (UINT32 x = vector_xmin; x < vector_xmax; x++)
//        {
//            UINT32 chere = *prgb;
//            UINT32 cglow = *glowbuf++;

//            UINT32 cr = cglow>>16;
//            UINT32 rh = chere>>16;
//            if(cr>rh) rh=cr;

//            UINT32 gh = (chere>>8) & 0x0ff;
//            UINT32 cg = (cglow>>8) & 0x0ff;
//            if(cg>gh) gh=cg;

//            UINT32 bh = (chere) & 0x0ff;
//            UINT32 cb = (cglow) & 0x0ff;
//            if(cb>bh) bh=cb;

//            *prgb++=(rh<<16)|(gh<<8)|bh;

//        }
//    }

//}

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
    Pix15(rgb_t col) :
        _r(RGB_RED(col)>>3), _g(RGB_GREEN(col)>>3), _b(RGB_BLUE(col)>>3)
    {}

    UINT16 _r, _g, _b;
    UINT16 _rt, _gt, _bt;
    void tint(UINT8 t) {
        _rt = (_r * t) >> 8;
        _gt = (_g * t) >> 8;
        _bt = (_b * t) >> 8;
    }
    void aa_pixel(int x, int y)
    {
        UINT32 dst;

        if (x < vector_xmin || x >= vector_xmax)
            return;
        if (y < vector_ymin || y >= vector_ymax)
            return;
        UINT16* pdst = ((UINT16*)vecbitmap->line[y]) + x;
        dst = *pdst;
        *pdst = LIMIT5(_b + (dst & 0x1f))
            | (LIMIT5(_g + ((dst >> 5) & 0x1f)) << 5)
            | (LIMIT5(_r + (dst >> 10)) << 10);

    }
    void aa_pixeltint(int x, int y)
    {
        UINT32 dst;

        if (x < vector_xmin || x >= vector_xmax)
            return;
        if (y < vector_ymin || y >= vector_ymax)
            return;
        UINT16* pdst = ((UINT16*)vecbitmap->line[y]) + x;
        dst = *pdst;
        *pdst = LIMIT5(_bt + (dst & 0x1f))
            | (LIMIT5(_gt + ((dst >> 5) & 0x1f)) << 5)
            | (LIMIT5(_rt + (dst >> 10)) << 10);

    }

};
class Pix32 {
public:
    Pix32(rgb_t col) :
    _r(RGB_RED(col)), _g(RGB_GREEN(col)), _b(RGB_BLUE(col))
    {}
    UINT16 _r, _g, _b;
    UINT16 _rt, _gt, _bt;
    void tint(UINT8 t) {
        _rt = (_r * t) >> 8;
        _gt = (_g * t) >> 8;
        _bt = (_b * t) >> 8;
    }
    void aa_pixel(int x, int y)
    {
        vector_pixel_t coords;
        UINT32 dst;
        //TODO wtf clip outside !
        if (x < vector_xmin || x >= vector_xmax)
            return;
        if (y < vector_ymin || y >= vector_ymax)
            return;
        UINT32* pdst = ((UINT32*)vecbitmap->line[y]) + x;
        dst = *pdst;
        *pdst = LIMIT8(_b + (dst & 0xff))
            | (LIMIT8(_g + ((dst >> 8) & 0xff)) << 8)
            | (LIMIT8(_r + (dst >> 16)) << 16);

    }
    void aa_pixeltint(int x, int y)
    {
        vector_pixel_t coords;
        UINT32 dst;
        //TODO wtf clip outside !
        if (x < vector_xmin || x >= vector_xmax)
            return;
        if (y < vector_ymin || y >= vector_ymax)
            return;
        UINT32* pdst = ((UINT32*)vecbitmap->line[y]) + x;
        dst = *pdst;
        *pdst = LIMIT8(_bt + (dst & 0xff))
            | (LIMIT8(_gt + ((dst >> 8) & 0xff)) << 8)
            | (LIMIT8(_rt + (dst >> 16)) << 16);

    }

};

template<class pixel,bool b_antialias>
void vector_draw_toT(pixel &pix, point* curpoint)
{
    int x2 = curpoint->x;
    int y2 = curpoint->y;
    rgb_t col = curpoint->col;
    int intensity = curpoint->intensity;
    rgb_t(*color_callback)(void) = curpoint->callback;

    unsigned char a1;
    int dx, dy, sx, sy, cx, cy, width;
    static int x1, yy1;
    int xx, yy;

    x2 = (int)(vector_scale_x * x2);
    y2 = (int)(vector_scale_y * y2);

    /* [2] adjust cords if needed */

    if (!b_antialias)
    {
        x2 = (x2 + 0x8000) >> 16;
        y2 = (y2 + 0x8000) >> 16;
    }

    /* [3] handle color and intensity */

    if (intensity == 0) goto end_draw;

    col = Tinten(intensity, col);

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
        dx = abs(x1 - x2);
        dy = abs(yy1 - y2);
        sx = (x1 <= x2) ? 1 : -1;
        sy = (yy1 <= y2) ? 1 : -1;
        cx = dx / 2;
        cy = dy / 2;

        if (dx >= dy)
        {
            for (;;)
            {
               // if (color_callback) col = Tinten(intensity, (*color_callback)());
                pix.aa_pixel(x1, yy1);
                if (x1 == x2) break;
                x1 += sx;
                cx -= dy;
                if (cx < 0)
                {
                    yy1 += sy;
                    cx += dx;
                }
            }
        }
        else
        {
            for (;;)
            {
               // if (color_callback) col = Tinten(intensity, (*color_callback)());
                pix.aa_pixel(x1, yy1);
                if (yy1 == y2) break;
                yy1 += sy;
                cy -= dx;
                if (cy < 0)
                {
                    x1 += sx;
                    cy += dy;
                }
            }
        }
    }

end_draw:

    x1 = x2;
    yy1 = y2;
    

}

void vector_draw_to15(point* curpoint)
{
    Pix15 pix(curpoint->col);
    vector_draw_toT<Pix15,false>(pix,curpoint);
}
void vector_draw_to32(point* curpoint)
{
    Pix32 pix(curpoint->col);
    vector_draw_toT<Pix32, false>(pix, curpoint);    
}
void vector_draw_to15aa(point* curpoint)
{
    Pix15 pix(curpoint->col);
    vector_draw_toT<Pix15, true>(pix, curpoint);
}
void vector_draw_to32aa(point* curpoint)
{
    Pix32 pix(curpoint->col);
    vector_draw_toT<Pix32, true>(pix, curpoint);
}