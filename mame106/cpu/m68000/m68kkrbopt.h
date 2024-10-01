#ifndef M68KKRBOPT_H_
#define M68KKRBOPT_H_

#define OPTIM68K_NOXOR 1
#define OPTIM68K_NOMASK_A 1

//this was silly #define OPTIM68K_USEREGIRMAGIC 1

// batrider no boot, + no sound arkretrn
//#define OPTIM68K_SQUEEZEPPCREG 1

#ifdef __AMIGA__
#define OPTIM68K_USEFAST32INTRF 1
#define OPTIM68K_USEFASTMOVEMREAD 1
#define OPTIM68K_USEFASTMOVEMWRITE 1

// divs problem with batrider
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
#define COREIRREG REG68KCORE(d1)
struct m68k_cpu_instance;
struct m68ki_cpu_core;

#define OPTIM68K_USEINSTANCE 1
#if OPTIM68K_USEINSTANCE

    #if OPTIM68K_USEREGIRMAGIC
        #define OPTIM68K_DOUSEREGIRMAGIC 1
        #define M68KOPT_PARAMS1 struct m68ki_cpu_core *p68k COREREG
        #define M68KOPT_PARAMS struct m68ki_cpu_core *p68k COREREG, uint regir COREIRREG
        #define M68KOPT_PASSPARAMS p68k,regir
    #else
        #define M68KOPT_PARAMS struct m68k_cpu_instance *p68k COREREG
        #define M68KOPT_PASSPARAMS p68k
    #endif
#else
    #define M68KOPT_PARAMS void
    #define M68KOPT_PASSPARAMS
#endif
#endif
