#include "amiga_video_tracers_argb32.h"

// this files is just to implement template calls...

template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }

#ifdef __GNUC__
#define ASM
#define REG(r) __asm(#r)
#endif


// to manage 24 bits mode pixel copy without any arse,
// assume there is a 3 byte length type than can copy its value from a 4 byte type.
// this is finely used by following templates for 24bits mode.
struct type24{
    type24(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    char r,g,b;
};


struct typeRGB15{
    typeRGB15(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+3))),b((char)argb>>3) {}
    USHORT  a:1, r:5, g:5, b:5;
};

struct typeBGR15{
    typeBGR15(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+3))),b((char)argb>>3) {}
    USHORT  a:1, b:5, g:5, r:5;
};
// validated:
struct typeRGB16{
    typeRGB16(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+2))),b((char)argb>>3) {}
    USHORT  r:5, g:6, b:5;
};

struct typeBGR16{
    typeBGR16(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+2))),b((char)argb>>3) {}
    USHORT  b:5, g:6, r:5;
};
// - - - -

struct typeRGB15PC{
    typeRGB15PC(ULONG argb) : a(argb){
       b = (b<<8) |(b>>8);
    }
    union { typeRGB15 a; USHORT b;};
};

struct typeBGR15PC{
    typeBGR15PC(ULONG argb) : a(argb){
       b = (b<<8) |(b>>8);
    }
    union { typeBGR15 a; USHORT b;};
};
//  NOTE: RGB16PC is the only one pistorm picasso actually use for 16bits pixels.
/* works, version 1, generate 2 bfins + 1 bfextu,
 * which, as I understand, are possibly "remanaged by the OS 68040 library" thus does horrible code
 * before being remapped by emu68, if I get it. fine bitswapping is problemtaic on this hardware.
  normaly R8G8B8A to A8R8G8B8 should be very fine.
*/
//struct typeRGB16PC{
//    typeRGB16PC(ULONG argb) : a(argb){
//        b = (b<<8) |(b>>8);
//    }
//    union { typeRGB16 a; USHORT b;};
//}; // sizeof() ==2 , always.

// - - - IMPORTANT NOTES FOR 68000 INLINE ASM
//  For example, on the 68000, I is defined to stand for the range of values 1 to 8. This is the range permitted as a shift count in the shift instructions.
// =  Means that this operand is written to by this instruction: the previous value is discarded and replaced by new data.
//+  Means that this operand is both read and written by the instruction.
// & earlyclobber before output
// d a floating point
// Motorola 680x0—config/m68k/constraints.md
// a adress register
// d data register
// f 68881 float
// I integer range 1->8
// J 16b signed
// K signed number
// L integer -8 -1

struct typeRGB16PC{
    typeRGB16PC( unsigned int argb)
    {
        // 0000 00000  rrrr r___  GGGg gg__  bbbb b___
        //                        gggb bbbb  rrrr rGGG
        // r >>16
        // b << 5
        // GGG >>13 (ou rol.w 3
        // ggg<<3
        // this was done only to remove bfins and move.b, which does extra slowdown.
       // volatile ULONG t __asm("d1");
        asm volatile(
           "move.l %0,d1\n\t"
           //"swap %0\n\t"
            "lsr.l #8,%0\n\t"  // better than swap for emu ? gcc never uses swap.
            "lsr.w #8,%0\n\t"
           "and.b #0xf8,%0\n\t"   // ________ rrrr r___ ok

           "move.w d1,d2\n\t"
           "rol.w #3,d2\n\t"
           "and.b #0x07,d2\n\t"
           "or.b  d2,%0\n\t"      // rrrr rGGG

            "move.w d1,d2\n\t"
            "lsl.w #3,d2\n\t"
            "and.w #0xe000,d2\n\t"
            "or.w  d2,%0\n\t"      // ggg____ rrrr rGGG

            "lsl.w #5,d1\n\t"
            "and.w #0x1f00,d1\n\t"
            "or.w  d1,%0\n\t"      // ggg____ rrrr rGGG
           "\n\t"
// this syntax is a hell, I hope you like pain .
// it is "asm code" : "spec"(varoutput) : "spec"(varinput), : (extra register used)
//  'd' for d0->d7 'a' for a0->a7 , also can put constants
           : "=d"( argb)
           : "d" ( argb)
           : "d1","d2"
           );
        v = (USHORT)argb;
    } //  "=g" (t)
    USHORT v;
    //USHORT  g2:3,b:5,r:5,g1:3; // just so you know this is R5G6B5 with then bytes swapped.
}; // sizeof() ==2


struct typeBGR16PC{
    typeBGR16PC(ULONG argb) : a(argb){
       b = (b<<8) |(b>>8);
    }
    union { typeBGR16 a; USHORT b;};
}; // sizeof() ==2

typedef ULONG typeARGB32;

struct typeBGRA32{
    typeBGRA32(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    char b,g,r,a;
};

struct typeRGBA32{
    typeRGBA32(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    char r,g,b,a;
};


template<typename SCREENPIXTYPE,bool C_swapXY>
void directDrawRGB32TT(directDrawParams *p )
{
    directDrawScreen *screen = p->screen;
    directDrawSource *source = p->source;
    LONG x1=p->x1, y1=p->y1;
    LONG w=p->w, h=p->h;
    UWORD wsobpr = source->_bpr>>2;
    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

    ULONG *psourcebm = (ULONG *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
    pscreenbm += (y1 * wscbpr) + x1;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if(sourceHeight<=0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if(sourceWidth<=0) return;

    // clipping
    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }


    LONG addw,addh;
    LONG vh =0;
    LONG vxStart=0 ;


    if(C_swapXY)
    {
        addw = wsobpr;
        addh = 1;
        if(source->_swapFlags & ORIENTATION_FLIP_Y)
        {
            vh = sourceWidth-1;
            addh = -1;
        }
        if(source->_swapFlags & ORIENTATION_FLIP_X)
        {
            vxStart = (sourceHeight-1)*wsobpr;
            addw = -wsobpr;
        }
        for(WORD hh=0;hh<h;)
        {
            ULONG *psoline = psourcebm + vh;

            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline++ = SCREENPIXTYPE(psoline[vx]);
                vx += addw;
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    } else
    {
        addw = 1;
        addh = wsobpr;
        if(source->_swapFlags & ORIENTATION_FLIP_Y)
        {
            vh = (sourceHeight-1)*wsobpr;
            addh = -wsobpr;
        }
        if(source->_swapFlags & ORIENTATION_FLIP_X)
        {
            vxStart = sourceWidth/*-1*/; // ,no -1 because use --vx
            addw = -1;
        }

        for(WORD hh=0;hh<h;)
        {
            ULONG *psoline = psourcebm + vh;

            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            if(addw>0)
            {
                for(WORD ww=0;ww<w;ww++)
                {
                    *pscline++ = SCREENPIXTYPE(psoline[vx++]);
                }
            } else
            {   // swap X
                for(WORD ww=0;ww<w;ww++)
                {
                    *pscline++ = SCREENPIXTYPE(psoline[--vx]);
                }
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    }
}

template<typename SCREENPIXTYPE,bool C_swapXY>
void directDrawRGB32ScaleTT(directDrawParams *p )
{
    directDrawScreen *screen = p->screen;
    directDrawSource *source = p->source;
    LONG x1=p->x1, y1=p->y1;
    LONG w=p->w, h=p->h;

    UWORD wsobpr = source->_bpr>>2;
    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

    ULONG *psourcebm = (ULONG *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
    pscreenbm += (y1 * wscbpr) + x1;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if(sourceHeight<=0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if(sourceWidth<=0) return;

    LONG addw,addh;
    LONG vh ;
    LONG vxStart ;
    UWORD hmod;
    UWORD vmod;


    if(C_swapXY)
    {
        addw = (sourceHeight<<16)/w; // swapped source size
        addh = (sourceWidth<<16)/h; //
    } else
    {
        addw = (sourceWidth<<16)/w;
        addh = (sourceHeight<<16)/h;
    }
    vh = 0;
    vxStart = 0;
    hmod = (C_swapXY)?1:wsobpr;
    vmod = (C_swapXY)?wsobpr:1;
    if(source->_swapFlags & ORIENTATION_FLIP_Y)
    {
        vh = (C_swapXY)?(sourceWidth<<16)-1:(sourceHeight<<16)-1;
        addh = -addh;
    }
    if(source->_swapFlags & ORIENTATION_FLIP_X)
    {
        vxStart = (C_swapXY)?(sourceHeight<<16)-1:(sourceWidth<<16)-1;
        addw = -addw;
    }

    // clipping
    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }

    WORD hh=0;
    while(hh<h-2)
    {
        ULONG *psoline = psourcebm + hmod*(vh>>16);
        ULONG *psoline2 = psourcebm + hmod*((vh+addh)>>16);
        ULONG *psoline3 = psourcebm + hmod*((vh+addh+addh)>>16);
        if(psoline == psoline3) // means 3 lines are the same.
        {
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);
                pscline++;
                vx += addw;
            }
            vh += addh*3;
            pscreenbm += wscbpr*3;
            hh +=3;
        } else
        if(psoline == psoline2) // means 2 lines are the same.
        {
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline = pscline[wscbpr] = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);

                pscline++;
                vx += addw;
            }
            vh += addh+addh;
            pscreenbm += wscbpr+wscbpr;
            hh +=2;
        } else
        {
            // other do normal one line ...
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline++ = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);
                vx += addw;
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    } // end loop h
    // last lines must be one line
    while(hh<h)
    {
        // other do normal one line ...
        ULONG *psoline = psourcebm + hmod*(vh>>16);
        SCREENPIXTYPE *pscline = pscreenbm;
        LONG vx = vxStart;
        for(WORD ww=0;ww<w;ww++)
        {
            *pscline++ = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);
            vx += addw;
        }
        vh += addh;
        pscreenbm += wscbpr;
        hh++;
    }


}

template<typename SCREENPIXTYPE>
void directDrawRGB32T(directDrawParams *p)
{
    directDrawSource *source = p->source;
    LONG w= p->w,h=p->h;
    const bool doSwapXY = source->_swapFlags & ORIENTATION_SWAP_XY;

    WORD sourcewidth = source->_x2-source->_x1;
    WORD sourceheight = source->_y2-source->_y1;
    if(doSwapXY) doSwap(sourcewidth,sourceheight);

    const bool doScale = ((sourcewidth!=w) || (sourceheight!=h));
    if(doSwapXY)
        if(doScale) directDrawRGB32ScaleTT<SCREENPIXTYPE,true>(p);
        else  directDrawRGB32TT<SCREENPIXTYPE,true>(p);
    else
        if(doScale) directDrawRGB32ScaleTT<SCREENPIXTYPE,false>(p);
        else  directDrawRGB32TT<SCREENPIXTYPE,false>(p);

}


// - - - - - -

void directDraw_RGB15_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeRGB15>(p);
}
void directDraw_BGR15_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeBGR15>(p);
}
void directDraw_RGB15PC_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeRGB15PC>(p);
}
void directDraw_BGR15PC_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeBGR15PC>(p);
}

void directDraw_RGB16_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeRGB16>(p);
}
void directDraw_BGR16_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeBGR16>(p);
}
void directDraw_RGB16PC_ARGB32(directDrawParams *p)
{

    directDrawRGB32T<typeRGB16PC>(p);
}
void directDraw_BGR16PC_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeBGR16PC>(p);
}

void directDraw_type24_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<type24>(p);
}


void directDrawARGB32_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeARGB32>(p);
}
void directDrawBGRA32_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeBGRA32>(p);
}
void directDrawRGBA32_ARGB32(directDrawParams *p)
{
    directDrawRGB32T<typeRGBA32>(p);
}






