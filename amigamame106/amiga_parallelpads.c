/*
    By Krb, most codes from examples.

    from:
     https://wiki.amigaos.net/wiki/Exec_Interrupts
    and from amiga developper CD 2.1 read34.asm


https://github.com/niklasekstrom/amiga-par-to-spi-adapter/blob/master/spi-lib/spi.c#L296
*/

#include "amiga_parallelpads.h"
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

// for some gcc ?
// struct Library *miscbase=NULL;
//#include "cia_protos.h"
//#include "misc_protos.h"

#include "proto/cia.h"

// now more like https://github.com/niklasekstrom/amiga-par-to-spi-adapter

//static volatile UBYTE *cia_a_prb = (volatile UBYTE *)0xbfe101;
//static volatile UBYTE *cia_a_ddrb = (volatile UBYTE *)0xbfe301;

//static volatile UBYTE *cia_b_pra = (volatile UBYTE *)0xbfd000;
//static volatile UBYTE *cia_b_ddra = (volatile UBYTE *)0xbfd200;

//#define USE_CIA_INTERUPT 1

#ifdef USE_CIA_INTERUPT
struct Library *ciaabase=NULL;
#endif

static struct Interrupt flag_interrupt;

// - - - - -


#include    <stdlib.h>
#include    <stdio.h>

// apparently from alib:
extern struct Custom custom;

// https://wiki.amigaos.net/wiki/Exec_Interrupts


void closeParallelPads(struct ParallelPads *parpads);

static UBYTE *allocname = "Mame"; // or use task name ?

//D0 - scratch
//D1 - scratch
//A0 - scratch
//A1 - server is_Data pointer (scratch)
//A5 - jump vector register (scratch)
//A6 - scratch
//all other registers must be preserved
static void interuptfunc( register struct ParPadsInteruptData *ppi __asm("a1") )
{

// 	movea.l	_portptr,a1		; a1 now holds the destination
//	move.b	_ciaaprb,(a1)		; move byte from port to dest
//	movea.l	_fireptr,a1		; a1 now holds the destination
//	move.b	_ciabpra,(a1)		; move byte from port to dest
//    UBYTE aprb = ~ciaaprb;
//    UBYTE bpra = ~ciabpra;
    UWORD ciavalues = ~(( ((UWORD)ciaaprb)<<8)| (UWORD)ciabpra);
    if(ciavalues != ppi->_last_cia )
    {
        // here if anything changed.

        // got to know if anything changed between checkings
        ppi->_last_checked_changes |= (ciavalues^ppi->_last_cia); //
        ppi->_last_checked |= ciavalues;

        ppi->_last_cia = ciavalues;
//        ppi->_counter++;
//       if( ppi->_Signal) Signal( ppi->_Task, ppi->_Signal );
    }
}

struct ParallelPads *createParallelPads()
{
    struct Interrupt *rbfint;
    struct ParPadsInteruptData *ppidata;
    BOOL priorenable;
    BYTE signr;

    struct ParallelPads *pparpads = AllocVec(sizeof(struct ParallelPads),MEMF_CLEAR);
    if(!pparpads) return NULL;
    pparpads->_signr = -1; // default error state for this.

    // - - -
//	miscbase = (struct Library *)OpenResource(MISCNAME);
//	if (!miscbase)
//	{
//		success = -1;
//		goto fail_out1;
//	}

//	ciaabase = (struct Library *)OpenResource(CIAANAME);
//	if (!ciaabase)
//	{
//		//success = -2;
//		goto error;
//	}
    printf("OpenResource(CIAANAME) ok\n");

    // - - - - acquire parallel port
    //Disable();
        pparpads->_parallelResOK = (UWORD)(AllocMiscResource(MR_PARALLELPORT,allocname)==NULL);
        if(!pparpads->_parallelResOK) { Enable(); goto error; }

        pparpads->_parallelBitsOK = (UWORD)(AllocMiscResource(MR_PARALLELBITS,allocname)==NULL);
        if(!pparpads->_parallelBitsOK) { Enable(); goto error; }

    printf("Parallel acquired ok\n");

    // - - - - - install interupt
//	flag_interrupt.is_Node.ln_Name = (char *)spi_lib_name;
//	flag_interrupt.is_Node.ln_Type = NT_INTERRUPT;
//	is_Data
//	flag_interrupt.is_Code = &interuptfunc ;

//	Disable();
//	AddIntServer();

//	if (AddICRVector(ciaabase, CIAICRB_FLG, &flag_interrupt))
//	{
//		Enable();
//		goto error;
//	}

//	AbleICR(ciaabase, CIAICRF_FLG);
//	SetICR(ciaabase, CIAICRF_FLG);
//	Enable();

   printf("interupt installed ok\n");

// from sd adapter:
//	*cia_b_pra = (*cia_b_pra & ~ACT_MASK) | (REQ_MASK | CLK_MASK);
//	*cia_b_ddra = (*cia_b_ddra & ~ACT_MASK) | (REQ_MASK | CLK_MASK);

//	*cia_a_prb = 0xff;
//	*cia_a_ddrb = 0xff;


        // from read34.s:
        // use hard address using amiga.lib:
 //  move.b	#0,_ciaaddrb		; all lines read
 //      andi.b	#$FF,_ciabddra		; busy, pout, and sel. to read

        ciaaddrb = 0; // all lines read
        ciabddra &= 0xFF; // busy, pout, and sel. to read

        // Well, we made it this far, so we've got exclusive access to
        // the parallel port, and all the lines we want to use are
        // set up.
    //Enable();
    // - - - - -
    pparpads->_signr = signr = AllocSignal(-1);
    if(signr == -1) goto error;

    pparpads->_rbfint = rbfint = AllocVec(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
    if(!rbfint) goto error;

    pparpads->_ppidata = ppidata = AllocVec(sizeof(struct ParPadsInteruptData), MEMF_PUBLIC|MEMF_CLEAR);
    if(!ppidata) goto error;

    ppidata->_Task = FindTask(NULL);
    ppidata->_Signal = 1L << signr;

//        rbfint->is_Node.ln_Type = NT_INTERRUPT;      /* Init interrupt node. */
//        strcpy(rbfdata->rd_Name, allocname);
//        rbfint->is_Node.ln_Name = rbfdata->rd_Name;
//        rbfint->is_Data = (APTR)rbfdata;
//        rbfint->is_Code = RBFHandler;

       // rbfint->is_Node.ln_Name = ppidata->rd_Name;
        rbfint->is_Node.ln_Name = (char *)"parpads";
        rbfint->is_Data = (APTR)ppidata;
        rbfint->is_Code = &interuptfunc;

        AddIntServer(INTB_VERTB,rbfint);

//    pparpads->_prior_enable = (BYTE)((custom.intenar & INTF_RBF)!=0) ; /* interrupt */
//    custom.intena = INTF_RBF;                             /* disable it. */
//    pparpads->_prior_interupt = SetIntVector(INTB_RBF, rbfint);

//    rbfint->is_Node.ln_Type = NT_INTERRUPT;      /* Init interrupt node. */
//    strcpy(rbfdata->rd_Name, allocname);
//    rbfint->is_Node.ln_Name = rbfdata->rd_Name;
//    rbfint->is_Data = (APTR)rbfdata;
//    rbfint->is_Code = RBFHandler;
//                                                /* Save state of RBF and */
//    priorenable = custom.intenar & INTF_RBF ? TRUE : FALSE; /* interrupt */
//    custom.intena = INTF_RBF;                             /* disable it. */
//    priorint = SetIntVector(INTB_RBF, rbfint);

    // went OK
//    pparpads->_public._signalBit = (1<<(pparpads->_signr));

    return pparpads;
error:
    closeParallelPads((struct AParallelPads *)pparpads);
    return NULL;
}

//void readParallelPads(struct AParallelPads *parpads)
//{
//    if(!parpads) return;
//    parpads->_aprb = ciaaprb;
//    parpads->_bpra = ciabpra;
//}

void closeParallelPads(struct ParallelPads *pparpads)
{
    if(!pparpads) return;

//	AbleICR(ciaabase, CIAICRF_FLG);

//	*cia_b_ddra &= ~(ACT_MASK | REQ_MASK | CLK_MASK);
//	*cia_a_ddrb = 0;

//	RemICRVector(ciaabase, CIAICRB_FLG, &flag_interrupt);

    if(pparpads->_rbfint)
    {
        RemIntServer(INTB_VERTB,pparpads->_rbfint);
    }
    if(pparpads->_ppidata) FreeVec(pparpads->_ppidata);
    if(pparpads->_rbfint) FreeVec(pparpads->_rbfint);
    if(pparpads->_signr != -1) FreeSignal(pparpads->_signr);

    if(pparpads->_parallelBitsOK) FreeMiscResource(MR_PARALLELBITS);
    if(pparpads->_parallelResOK) FreeMiscResource(MR_PARALLELPORT);

    FreeVec(pparpads);

}
