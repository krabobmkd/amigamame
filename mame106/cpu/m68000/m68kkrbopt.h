#ifndef M68KKRBOPT_H_
#define M68KKRBOPT_H_

#define OPTIM68K_NOMASK_A 1

// do not try to remove  "previous PC register" management,
// batrider no boot, + no sound arkretrn
// it is very needed, notably for branching and pc relative reads.
// a solution would be to put previous reg in a param register
//like we do for regir.
//#define OPTIM68K_SQUEEZEPPCREG 1

#define OPTIM68K_SKIPMOVECCRV 1
#define OPTIM68K_SKIPMOVECCRC 1

// only do one entry search for whole movems.
// now works both on amiga and PC.
#define OPTIM68K_USEFASTMOVEMREAD 1
#define OPTIM68K_USEFASTMOVEMWRITE 1

#ifdef __AMIGA__
     #define OPTIM68K_USEFAST32INTRF 1

    // 68k assembler inline or not:
    #define OPTIM68K_USEDIRECT68KASM_REWRITEMOVES 1
    #define OPTIM68K_USEDIRECT68KASM_MOVEWR_SQUEEZE_NZ 1
     #define OPTIM68K_USEDIRECT68KASM_DIVS 1
     #define OPTIM68K_USEDIRECT68KASM 1
#endif

#if defined(__GNUC__) && defined(__AMIGA__)
#define REG68KCORE(r) __asm(#r)
//#define REG68KCORE(r)
#else
#define REG68KCORE(r)
#endif

#define COREREG REG68KCORE(a2)
#define COREIRREG REG68KCORE(d2)
struct m68k_cpu_instance;
struct m68ki_cpu_core;

// actually can't be removed. Use instances of 68k objects and not those stupid static registers.
// avoid tons of struct copies when "slicing".
// if all CPU where ok with this, we could remove the "push/pull context" pass totally, which would
// speed up gig time. (half of the outrun time)
#define OPTIM68K_USEINSTANCE 1
#if OPTIM68K_USEINSTANCE

    #define M68KOPT_PARAMS struct m68k_cpu_instance *p68k COREREG, unsigned short regir COREIRREG
    #define M68KOPT_PASSPARAMS p68k,regir

#else
    #define M68KOPT_PARAMS void
    #define M68KOPT_PASSPARAMS
#endif
#endif
