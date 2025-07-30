/*
    By Krb, most codes from examples.

    from:
     https://wiki.amigaos.net/wiki/Exec_Interrupts
    and from amiga developper CD 2.1 read34.asm


https://github.com/niklasekstrom/amiga-par-to-spi-adapter/blob/master/spi-lib/spi.c#L296
*/

#include "amiga_proportionaljoystick.h"
//

#include <proto/exec.h>
#define ALIB_HARDWARE_CIA
#include <proto/alib.h>
#include <proto/misc.h>

#include    <exec/types.h>
#include    <libraries/dos.h>

#include <hardware/cia.h>
#include <resources/cia.h>
#include <resources/misc.h>

#include <hardware/custom.h>
#include <hardware/intbits.h>

// -> now is inited or not in main.cpp
// struct Library *miscbase=NULL;
//#include "cia_protos.h"
//#include "misc_protos.h"


//#ifdef USE_CIA_INTERUPT
//#include "proto/cia.h"
//struct Library *ciaabase=NULL;
//#endif


int hasProportionalStickResource()
{
    return 0;
}

struct ProportionalSticks *createProportionalSticks()
{
    struct ProportionalSticks *pprops = AllocVec(sizeof(struct ProportionalSticks),MEMF_CLEAR);
    if(!pprops) return NULL;

    return pprops;
}

void closeProportionalSticks(struct ProportionalSticks *pprops)
{
    if(!pprops) return;
    FreeVec(pprops);
}
