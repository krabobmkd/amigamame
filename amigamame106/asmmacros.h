#ifndef MACROS_H
#define MACROS_H

#ifdef __GNUC__
//#define ASM __saveds
#define ASM
#define REG(r) __asm(#r)
#define MOVETO(v,r) asm("movel %0,"#r : "=m" (v))
#endif

#endif
