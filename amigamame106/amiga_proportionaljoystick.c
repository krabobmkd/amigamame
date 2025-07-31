/*
  more or less adapted from:
  aminet "AnalogMouse" asm source
  and some DevelopperCD2.1 reference.

*/

#include "amiga_proportionaljoystick.h"
//

#include <proto/exec.h>
#define ALIB_HARDWARE_CIA
#include <proto/alib.h>
#include <proto/misc.h>
#include <proto/potgo.h>
#include    <exec/types.h>
#include    <libraries/dos.h>
// for atexit()
#include    <stdlib.h>
#include <stdio.h>


// this one has custom.pot1dat
#include <hardware/custom.h>
#include <hardware/intbits.h>

#include <devices/gameport.h>
#include <resources/potgo.h>



// declare to C we are managing opening/closing potgo ourselves.
// note PotgoBase open is done here is needed, but closed in atexit is explicitly opened in main()
// extern struct Library *PotgoBase;
struct Library    *PotgoBase=NULL;


static void initPotgoIfNotDone()
{

    if(PotgoBase == NULL)
    {
        printf("open potgo resource\n");
        PotgoBase = OpenResource(POTGONAME);
        printf("potgo resource:%08x\n",(int)PotgoBase);
        // there is no "CloseResource" there is AddResource/RemResource, but it's OS matter.
    }
}
// this is public, to check if we are hardware capable.
int hasProportionalStickResource()
{
    initPotgoIfNotDone();
    return(PotgoBase != NULL);
}

// apparently from alib:
extern struct Custom custom;

static void interuptfunc( register struct PPSticksInteruptData *ppi __asm("a1") )
{
    // read value and relaunch potgo as soon as possible.

    // watch out ! values are linear to "current accumulated voltage since last Potgo start"
    // also if pin not allocated could not work (other app ?).
    ppi->_last_potxdat[1] = custom.pot1dat; // could be  = ((UWORD *)0x00dff014); ((UWORD *)0x00dff012);
    ppi->_last_potxdat[0] = custom.pot0dat;

    WritePotgo(1,1); // value, mask of what is to write.

    if(ppi->_nbcalls<2) ppi->_nbcalls++; // valid if ==2

}

struct ProportionalSticks *createProportionalSticks(ULONG flags, ULONG *preturncode)
{
printf("createProportionalSticks:%08x\n",flags);
    if((flags &3)==0) return NULL; // need port1 or/and port2

printf("create PJSTCK\n");
    initPotgoIfNotDone();

    if(PotgoBase == NULL) {
        // no need for all this if no hardware
        if(preturncode) *preturncode = PROPJOYRET_NOHARDWARE;
        return NULL;
    }

    struct ProportionalSticks *pprops = AllocVec(sizeof(struct ProportionalSticks),MEMF_CLEAR);
    if(!pprops)
    {
        if(preturncode) *preturncode = PROPJOYRET_ALLOC;
        return NULL;
    }

    if(preturncode) *preturncode = PROPJOYRET_OK; // default for now on.


    pprops->_ports[0]._deviceresult = 1; // ports not ok by default.
    pprops->_ports[1]._deviceresult = 1;
 //   pprops->_signr = -1; // default error state for this.

    pprops->_Task = FindTask(NULL);
    pprops->_flags = flags;
    pprops->_gameportPort.mp_SigTask = pprops->_Task; //object to be signaled

    AddPort(&(pprops->_gameportPort));
    pprops->_portAdded = 1;

    int portmask = PROPJOYFLAGS_PORT1;
    for(int iportunit=0 ;iportunit<2 ; iportunit++)
    {
        if((flags & portmask)!=0)
        {
            // - - -- - - - -allocjoyport - - - - -
            struct PStickDevice *ppsd = &(pprops->_ports[iportunit]);
               // OpenDevice( CONST_STRPTR devName, ULONG unit, struct IORequest *ioRequest, ULONG flags );

            BYTE res = OpenDevice("gameport.device",
                            iportunit,
                            &(ppsd->_gameportIOReq),0);
            ppsd->_deviceresult = res;
            if(res !=0)
            {   // only some port could fail...
                //closeProportionalSticks(pprops);
                if(preturncode) *preturncode = PROPJOYRET_DEVICEFAIL;
                return NULL;
            }
            Forbid();

                ppsd->_gameportIOReq.io_Command = GPD_ASKCTYPE;
                ppsd->_gameportIOReq.io_Data = &(ppsd->_portType);
                ppsd->_gameportIOReq.io_Length = 1;

                /*BYTE resdoio =*/ DoIO(&(ppsd->_gameportIOReq));
                if(ppsd->_portType != GPCT_NOCONTROLLER) // GPCT_NOCONTROLLER 0
                {
                    Permit();
                    ppsd->_deviceresult = 1;
                    continue;
//                    closeProportionalSticks(pprops);
//                    if(preturncode) *preturncode = PROPJOYRET_DEVICEUSED;
                   // return NULL;
                }
                // port free, allocating port
                ppsd->_gameportIOReq.io_Command = GPD_SETCTYPE;
                ppsd->_gameportIOReq.io_Data = &(ppsd->_portType);
                ppsd->_gameportIOReq.io_Length = 1;
                ppsd->_portType = GPCT_ALLOCATED;

                /*BYTE resdoio =*/ DoIO(&(ppsd->_gameportIOReq));
            Permit();
            ppsd->_deviceresult = 0; // means OK.
            // - - --  -- end allocjoyport - - - - --

        } // end if open one device unit
        portmask<<=1; // -> PROPJOYFLAGS_PORT2
    }
    // here if both ports are !=0 means total fail.
    if(pprops->_ports[0]._deviceresult != 0 &&
        pprops->_ports[1]._deviceresult != 0)
    {
        closeProportionalSticks(pprops);
        if(preturncode) *preturncode = PROPJOYRET_DEVICEUSED;
        return NULL;
    }

    // - - - - - -allocpotgo - - - - - -- -
//apparently missing from NDK includes !
// values from NDK AllocPotBits () documentation
#define PGBIT_START (1<<0)

#define PGBIT_DATLX (1<<8)
#define PGBIT_OUTLX (1<<9)
#define PGBIT_DATLY (1<<10)
#define PGBIT_OUTLY (1<<11)

#define PGBIT_DATRX (1<<12)
#define PGBIT_OUTRX (1<<13)
#define PGBIT_DATRY (1<<14)
#define PGBIT_OUTRY (1<<15)
    UWORD potsBitsToAlloc = PGBIT_START ;
    // we only need to alloc start, then for each pots,DAT, not OUT bits (If I get it well) !!
    if(pprops->_ports[0]._deviceresult == 0)
    {
     printf("gameport 0 device unit opened\n");
        potsBitsToAlloc |= (PGBIT_DATLX|PGBIT_DATLY);
    }
    if(pprops->_ports[1]._deviceresult == 0)
    {
     printf("gameport 1 device unit opened\n");
        potsBitsToAlloc |= (PGBIT_DATRX|PGBIT_DATRY);
    }

    /*
    The AllocPotBits routine allocates bits in the hardware potgo
    register that the application wishes to manipulate via
    WritePotgo.  The request may be for more than one bit.  A
    user trying to allocate bits may find that they are
    unavailable because they are already allocated, or because
    the start bit itself (bit 0) has been allocated, or if
    requesting the start bit, because input bits have been
    allocated.  A user can block itself from allocation: i.e.
    it should FreePotgoBits the bits it has and re-AllocPotBits if
    it is trying to change an allocation involving the start bit.
    */
// #%0101000000000001,d0
printf("try allocate bits:%08x\n",(int)potsBitsToAlloc);
    pprops->_allocatedBits = AllocPotBits(potsBitsToAlloc); // #0b0101 0000 00000001

printf("allcoateed bits:%08x\n",(int)pprops->_allocatedBits);
    if(pprops->_allocatedBits != potsBitsToAlloc)
    {
        //error
        closeProportionalSticks(pprops);
        if(preturncode) *preturncode = PROPJOYRET_NOANALOGPINS;
        return NULL;
    }
    // note: it's to allow further calls to WritePotgo()

/*
UWORD  __stdargs AllocPotBits( ULONG bits );
VOID  __stdargs FreePotBits( ULONG bits );
VOID  __stdargs WritePotgo( ULONG word, ULONG mask );


#define AllocPotBits(___bits) \
      LP1(0x6, UWORD, AllocPotBits , UWORD, ___bits, d0,\
      , POTGO_BASE_NAME)

#define FreePotBits(___bits) \
      LP1NR(0xc, FreePotBits , UWORD, ___bits, d0,\
      , POTGO_BASE_NAME)

#define WritePotgo(___word, ___mask) \
      LP2NR(0x12, WritePotgo , UWORD, ___word, d0, UWORD, ___mask, d1,\
      , POTGO_BASE_NAME)
*/
    //timer ?

    //install some native vblank interupt, 50 or 60Hz
//    BYTE signr;
//    pprops->_signr = signr = AllocSignal(-1);
//    // can't happen...
//    if(signr == -1) {
//        closeProportionalSticks(pprops);
//        if(preturncode) *preturncode = PROPJOYRET_NOSIGNAL;
//        return NULL;
//    }

    struct PPSticksInteruptData *pintdata;
    pprops->_pintdata = pintdata = AllocVec(sizeof(struct PPSticksInteruptData), MEMF_PUBLIC|MEMF_CLEAR);
    if(!pintdata)
    {
        closeProportionalSticks(pprops);
        if(preturncode) *preturncode = PROPJOYRET_ALLOC;
        return NULL;
    }

    struct Interrupt *rbfint;
    pprops->_rbfint = rbfint = AllocVec(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
    if(!rbfint)
    {
        closeProportionalSticks(pprops);
        if(preturncode) *preturncode = PROPJOYRET_ALLOC;
        return NULL;
    }

//    pintdata->_Task = pprops->_Task;
//    pintdata->_Signal = 1L << signr;

    rbfint->is_Node.ln_Name = (char *)"mamepotgo";
    rbfint->is_Data = (APTR)pintdata;
    rbfint->is_Code = &interuptfunc;

    AddIntServer(INTB_VERTB,rbfint);


    return pprops;
}

void closeProportionalSticks(struct ProportionalSticks *pprops)
{
    if(!pprops) return;

    // close interupt
    if(pprops->_rbfint)
    {
        RemIntServer(INTB_VERTB,pprops->_rbfint);
        FreeVec(pprops->_rbfint);
    }
    if( pprops->_pintdata) FreeVec(pprops->_pintdata);
    //if(pprops->_signr != -1) FreeSignal(pprops->_signr);

    //closetimer?

    //freepotgo
    if(pprops->_allocatedBits != 0)
    {
        FreePotBits(pprops->_allocatedBits);
    }
    //freejoyport devices
    for(int iportunit=0 ;iportunit<2 ; iportunit++)
    {
        struct PStickDevice *ppsd = &(pprops->_ports[iportunit]);
        if(ppsd->_deviceresult != 0 ) continue;

        ppsd->_gameportIOReq.io_Command = GPD_SETCTYPE;
        ppsd->_gameportIOReq.io_Data = &(ppsd->_portType);
        ppsd->_gameportIOReq.io_Length = 1;
        ppsd->_portType = GPCT_NOCONTROLLER;

        /*BYTE resdoio =*/ DoIO(&(ppsd->_gameportIOReq));
        CloseDevice(&(ppsd->_gameportIOReq));
    }
    // remove gameport messagerie
    if( pprops->_portAdded)
    {
        RemPort(&(pprops->_gameportPort));
       // pprops->_portAdded = 0;
    }

    FreeVec(pprops);
}
