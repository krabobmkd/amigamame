/*
 * amiga_video_tracers_argb32.cpp
 * Purpose: 32-bit ARGB pixel format tracer implementation with optimizations
 *
 * ╔════════════════════════════════════════════════════════════════════════╗
 * ║             🌈 ARGB32 PIXEL TRACER - TRUE COLOR 🎨                    ║
 * ║  ┌──────────────────────────────────────────────────────────────┐    ║
 * ║  │                                                               │    ║
 * ║  │   MAME ARGB32  ──►  Format Conversion  ──►  Screen Output   │    ║
 * ║  │                                                               │    ║
 * ║  │   ┌──────────┐      ┌────────────────┐      ┌────────────┐  │    ║
 * ║  │   │  32-bit  │  ──► │  RGB15/16      │  ──► │  Display   │  │    ║
 * ║  │   │  ARGB    │      │  RGB24/ARGB32  │      │  Pixels    │  │    ║
 * ║  │   └──────────┘      │  BGR variants  │      └────────────┘  │    ║
 * ║  │                     └────────────────┘                       │    ║
 * ║  │                                                               │    ║
 * ║  │   Optimized with inline assembly for 68k performance!        │    ║
 * ║  └──────────────────────────────────────────────────────────────┘    ║
 * ║      Handles all color depths: 15/16/24/32-bit with swizzling!        ║
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

#include "amiga_video_tracers_argb32.h"
#include "amiga_video_tracers_core.h"
#include <string.h>

// this file is just to implement template calls...
// the loop itself, scaling, rotations and clipping: amiga_video_tracers_core.h

#ifdef __GNUC__
#define ASM
#define REG(r) __asm(#r)
#endif


struct typeRGB15{
    typeRGB15(ULONG argb) : a(0),r((char)(argb>>(16+3))),g((char)(argb>>(8+3))),b((char)argb>>3) {}
    USHORT  a:1, r:5, g:5, b:5;
};

struct typeBGR15{
    typeBGR15(ULONG argb) : a(0),r((char)(argb>>(16+3))),g((char)(argb>>(8+3))),b((char)argb>>3) {}
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
 * which, as I understand, are possibly "remanaged by the OS 68040 library" thus producing horrible code
 * before being remapped by emu68, if I get it. Fine bitswapping is problematic on this hardware.
  Normally R8G8B8A to A8R8G8B8 should be very fine.
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
           : "+d"( argb)  // read and written: same register in and out
           :
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

/*
    krb 2025/02/22
    grrr... P96 vc tells 32b modes are BGRA,
    but it's actually BRGA "PC", so native p96 modes looks more AGRB...

*/
// perfect copy for this type
typedef ULONG typeARGB32;

//ok: write on PIXFMT_BGRA32
struct typeBGRA32{
//    typeBGRA32(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    typeBGRA32(ULONG argb) : a(0) {

    asm volatile(
       "move.b %0,(%1)\n"
       "\tlsr.l #8,%0\n"
       "\tmove.b %0,1(%1)\n"
       "\tlsr.l #8,%0\n"
       "\tmove.b %0,2(%1)\n"
       "\tnop"
       : "+d"(argb) // out
       : "a"(this)  // in
       : "memory" // trashed: writes b,g,r through this
       );
//        b = (char)argb;
//        argb>>=8;
//        g = (char)argb;
//        argb>>=8;
//        r = (char)argb;
    }

    char b,g,r,a;
};

struct typeRGBA32{
    typeRGBA32(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb),a(0) {}
    char r,g,b,a;
};


// - - - - - -

// the pixel types give their bytes as a 16 or 32 bit word for the row writers
template<typename SCREENPIXTYPE>
void directDraw16_ARGB32T(directDrawParams *p)
{
    static_assert(sizeof(SCREENPIXTYPE) == 2, "16 bit pixel type");
    directDrawT<tracerRow16,ULONG>(p, [](ULONG argb) -> ULONG {
        SCREENPIXTYPE t(argb);
        UWORD v;
        memcpy(&v, &t, sizeof(v));
        return v;
    });
}
template<typename SCREENPIXTYPE>
void directDraw32_ARGB32T(directDrawParams *p)
{
    static_assert(sizeof(SCREENPIXTYPE) == 4, "32 bit pixel type");
    directDrawT<tracerRow32,ULONG>(p, [](ULONG argb) -> ULONG {
        SCREENPIXTYPE t(argb);
        ULONG v;
        memcpy(&v, &t, sizeof(v));
        return v;
    });
}

void directDraw_RGB15_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeRGB15>(p);
}
void directDraw_BGR15_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeBGR15>(p);
}
void directDraw_RGB15PC_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeRGB15PC>(p);
}
void directDraw_BGR15PC_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeBGR15PC>(p);
}

void directDraw_RGB16_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeRGB16>(p);
}
void directDraw_BGR16_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeBGR16>(p);
}
void directDraw_RGB16PC_ARGB32(directDrawParams *p)
{

    directDraw16_ARGB32T<typeRGB16PC>(p);
}
void directDraw_BGR16PC_ARGB32(directDrawParams *p)
{
    directDraw16_ARGB32T<typeBGR16PC>(p);
}

void directDraw_type24_ARGB32(directDrawParams *p)
{
    directDrawT<tracerRow24,ULONG>(p, [](ULONG argb) -> ULONG { return argb; });
}


void directDrawARGB32_ARGB32(directDrawParams *p)
{
    directDrawT<tracerRow32,ULONG>(p, [](ULONG argb) -> ULONG { return argb; });
}
// PIXFMT_BGRA32 type 12.
void directDrawBGRA32_ARGB32(directDrawParams *p)
{
    directDraw32_ARGB32T<typeBGRA32>(p);
}
void directDrawRGBA32_ARGB32(directDrawParams *p)
{
    directDraw32_ARGB32T<typeRGBA32>(p);
}

/*
 * True color pixels flowing like a rainbow! 🌈
 *        __
 *     __(  )__
 *    (  \__/  )  <- This parrot displays all 16 million colors!
 *     \      /
 *      ||  ||
 */
