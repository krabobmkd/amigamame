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

#ifdef PPJSCODE_TAKEPORT0CONTROL
    #include <devices/input.h>
#endif

#ifdef PPJSCODE_ALLOWLOWLEVELTIMER
    #include <proto/lowlevel.h>
    extern struct Library *LowLevelBase;
#endif

#include <resources/potgo.h>

const char *getProportionalStickErrorMessage(ULONG errcode)
{
    if(errcode == PROPJOYRET_NOHARDWARE) return "Harware is not proportionnal joystick capable.\n";
    if(errcode == PROPJOYRET_ALLOC) return "analog joystick: can't alloc.\n";
    if(errcode == PROPJOYRET_GAMEPORTU0FAIL) return "analog joystick: can't open gameport 1.\n";
    if(errcode == PROPJOYRET_GAMEPORTU1FAIL) return "analog joystick: can't open gameport 2.\n";
    if(errcode == PROPJOYRET_GAMEPORTU0INUSE) return "analog joystick: gameport 1 in use.\n";
    if(errcode == PROPJOYRET_GAMEPORTU1INUSE) return "analog joystick: gameport 2 in use.\n";
    if(errcode == PROPJOYRET_ALLDEVICEUSED) return "analog joystick: gameport device locked.\n";
    if(errcode == PROPJOYRET_NOANALOGPINS) return "analog joystick: can't reserve analog pins.\n";
 //   if(errcode == PROPJOYRET_NOSIGNAL) return "prop. joystick: can't find signal.";
    return "";
}

// declare to C we are managing opening/closing potgo ourselves.
// note PotgoBase open is done here is needed, but closed in atexit is explicitly opened in main()
// extern struct Library *PotgoBase;
struct Library    *PotgoBase=NULL;


static void initPotgoIfNotDone()
{
    if(PotgoBase == NULL)
    {
        //printf("open potgo resource\n");
        PotgoBase = OpenResource(POTGONAME);
        //printf("potgo resource:%08x\n",(int)PotgoBase);
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

//  - - - -generic
static inline void inline_interuptfunc(register struct PPSticksInteruptData *ppi __asm("a1"))
{
    // read value and relaunch potgo as soon as possible.

    // watch out ! values are linear to "current accumulated voltage since last Potgo start"
    // also if pin not allocated could not work (other app ?).
    ppi->_last_potxdat[0] = custom.pot0dat;
    ppi->_last_potxdat[1] = custom.pot1dat; // could be  = ((UWORD *)0x00dff014); ((UWORD *)0x00dff012);

    // as fast as possible.
    WritePotgo(1,ppi->_allocatedbits); // value, mask of what is to write.

    // yet the 2bt click buttons are there - not same place as classic digital sticks ! it's all inverted.
    ppi->_last_joyxdat[0] = custom.joy0dat; // port1 JOY1DAT $DFF00C
    ppi->_last_joyxdat[1] = custom.joy1dat; // port1
    // for propjoy it's then bit 1 and 9, unlike standard joy.

    if(ppi->_nbcalls<2) ppi->_nbcalls++; // values valid after 2 calls.


}
// - - - - when interupt is installed with addIntServer
static int interuptfunc_addIntServer( register struct PPSticksInteruptData *ppi __asm("a1") )
{
    inline_interuptfunc(ppi);
    return 0; // should set z flag
}
// - - - - when interupt is installed with SetIntVector as override
// NO ! TOO DANGEROUS
//typedef int (setIntVectorFunc*)(
//            register void* jumps __asm("a5"),
//            register void* exec __asm("a6")
//            );
//struct Interrupt *globalstatic_previousVblankInt=NULL;
//static struct PPSticksInteruptData *globalstatic_ppi=NULL;
//static void interuptfunc_setIntVector(
//            register void* jumps __asm("a5"),
//            register void* exec __asm("a6")
//            )
//{
//    // vblank should apply that at once.
//    inline_interuptfunc(globalstatic_ppi);
//    // supercall - ultradangerous
//    if(globalstatic_previousvblankInt)
//    {
//        setIntVectorFunc prev = (setIntVectorFunc)globalstatic_previousVblankInt->is_Code;
//        prev(jumps,exec);
//    }
//}



#ifdef PPJSCODE_TAKEPORT0CONTROL
// if need analog controller on mouse port, need to tell input.device to release it...
static void makeInputDeviceReleaseMousePort(struct ProportionalSticks *pprops)
{
    struct IOStdReq *pioreq = &(pprops->_inputdevIOReq);
    pioreq->io_Message.mn_ReplyPort = &(pprops->_gameportPort);

    pprops->_inputdevres = (int) OpenDevice("input.device",0, (struct IORequest *)pioreq,0);
    if(pprops->_inputdevres !=0) return;

// printf("input device opened ok\n");

    pioreq->io_Command = IND_SETMTYPE;
    pprops->_inputtype = GPCT_NOCONTROLLER;
    pioreq->io_Data    = (APTR)&pprops->_inputtype;
    pioreq->io_Length = 1;
    pioreq->io_Flags = IOB_QUICK;

    DoIO((struct IORequest *)pioreq);   /* Send the command */
// printf("after doio\n");

//IND_SETMTYPE
/*

NAME
    IND_SETMTYPE -- Set the current mouse port controller type

FUNCTION
    This command sets the type of device at the mouse port, so
    the signals at the port may be properly interpreted.

IO REQUEST
    io_Message      mn_ReplyPort set if quick I/O is not possible
    io_Device       preset by the call to OpenDevice
    io_Unit         preset by the call to OpenDevice
    io_Command      IND_SETMTYPE
    io_Flags        IOB_QUICK set if quick I/O is possible
    io_Length       1
    io_Data         the address of the byte variable describing
                    the controller type, as per the equates in
                    the gameport include file
*/

/* input.device ....
IND_SETMPORT 	Set the controller port to which the mouse is connected.
IND_SETMTRIG 	Set conditions that must be met by a mouse before a pending read request will be satisfied.
IND_SETMTYPE 	Set the type of device at the mouse port.
*/


}
static void makeInputDeviceUseMousePortBack(struct ProportionalSticks *pprops)
{
//     printf("makeInputDeviceUseMousePortBack\n");
    if(pprops->_inputdevres == 0)
    {
//     printf("do IND_SETMTYPE GPCT_MOUSE\n");
        //
        struct IOStdReq *pioreq = &(pprops->_inputdevIOReq);
        pioreq->io_Command = IND_SETMTYPE;
        pprops->_inputtype = GPCT_MOUSE;
        pioreq->io_Data    = (APTR)&pprops->_inputtype;
        pioreq->io_Length = 1;
        pioreq->io_Flags = IOB_QUICK;

        DoIO((struct IORequest *)pioreq);   /* Send the command */

//     printf("close device\n");
        if (!(CheckIO((struct IORequest *)&(pprops->_inputdevIOReq))))
        {
            AbortIO((struct IORequest *)&(pprops->_inputdevIOReq));  /* Ask device to abort request, if pending */
        }
        WaitIO((struct IORequest *)&(pprops->_inputdevIOReq));   /* Wait for abort, then clean up */
        CloseDevice((struct IORequest *)&(pprops->_inputdevIOReq));
        pprops->_inputdevres = 1;
    }
}
#endif
struct ProportionalSticks *createProportionalSticks(ULONG flags, ULONG timerMethod, ULONG *preturncode, PPStickInitLogFunc logFunc)
{

    if((flags &3)==0) return NULL; // need port1 or/and port2

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

    pprops->_timerMethod = timerMethod;

#ifdef PPJSCODE_TAKEPORT0CONTROL
    // not ok:
    pprops->_inputdevres = 1;
#endif
    pprops->_ports[0]._deviceresult = 1; // ports not ok by default.
    pprops->_ports[1]._deviceresult = 1;

 //   pprops->_signr = -1; // default error state for this.

    pprops->_Task = FindTask(NULL);
    pprops->_flags = flags;
    pprops->_gameportPort.mp_SigTask = pprops->_Task; //object to be signaled

    AddPort(&(pprops->_gameportPort));
    pprops->_portAdded = 1;

//printf("flag:%d\n",flags); // 8+2
    for(int iportunit=0 ;iportunit<2 ; iportunit++ )
    {
        int portmask = PROPJOYFLAGS_PORT1<<iportunit;
//printf("portmask:%d flag:%d\n",portmask,flags);
        if((flags & portmask)!=0)
        {
            int dosteal = 0;
#ifdef PPJSCODE_TAKEPORT0CONTROL
            if(iportunit==0)
            {
                dosteal = 1;
                makeInputDeviceReleaseMousePort(pprops);
            }
#endif

            // - - -- - - - -allocjoyport - - - - -
            struct PStickDevice *ppsd = &(pprops->_ports[iportunit]);
               // OpenDevice( CONST_STRPTR devName, ULONG unit, struct IORequest *ioRequest, ULONG flags );
//printf("OpenDevice:%d\n",iportunit);
            BYTE res = OpenDevice("gameport.device",
                            iportunit,
                            &(ppsd->_gameportIOReq),0);
            ppsd->_deviceresult = res;
            if(res !=0)
            {   // only some port could fail...
                //closeProportionalSticks(pprops);
                if(logFunc) logFunc(1,getProportionalStickErrorMessage(PROPJOYRET_GAMEPORTU0FAIL+iportunit));
                if(preturncode) *preturncode = PROPJOYRET_GAMEPORTU0FAIL+iportunit;
                continue;
            }

            Forbid();

                ppsd->_gameportIOReq.io_Command = GPD_ASKCTYPE;
                ppsd->_gameportIOReq.io_Data = &(ppsd->_portType);
                ppsd->_gameportIOReq.io_Length = 1;

                /*BYTE resdoio =*/ DoIO(&(ppsd->_gameportIOReq));

               // special case,experimental code , looks like working !
#ifdef PPJSCODE_TAKEPORT0CONTROL
                // if gameport unit 0 not available, ask input.device to free it.
                // if(iportunit==0 && ppsd->_portType != GPCT_NOCONTROLLER)
                // {
                //   //  printf("ppsd->_portType before:%d\n",(int)ppsd->_portType);
                //     //Permit();
                //         makeInputDeviceReleaseMousePort(pprops);
                //     //Forbid();
                //     DoIO(&(ppsd->_gameportIOReq)); // then, ask again if port0 available.
                //     //WORKS !
                //   //  printf("ppsd->_portType after:%d\n",(int)ppsd->_portType);
                // }
#endif

                if(ppsd->_portType != GPCT_NOCONTROLLER) // GPCT_NOCONTROLLER 0
                {
                    Permit();
                 // printf("iportunit:%d device state:%d\n",iportunit,(int)ppsd->_portType);
                    if(logFunc) logFunc(1,getProportionalStickErrorMessage(PROPJOYRET_GAMEPORTU0INUSE+iportunit));
                    if(preturncode) *preturncode = PROPJOYRET_GAMEPORTU0INUSE+iportunit;

                    ppsd->_deviceresult = 1;
                    continue;

                }
                // port free, allocating port
                ppsd->_gameportIOReq.io_Command = GPD_SETCTYPE;
                ppsd->_gameportIOReq.io_Data = &(ppsd->_portType);
                ppsd->_gameportIOReq.io_Length = 1;
                ppsd->_portType = GPCT_ALLOCATED; // GPCT_RELJOYSTICK/GPCT_ABSJOYSTICK  ? -> no both are digital

                /*BYTE resdoio =*/ DoIO(&(ppsd->_gameportIOReq));
            Permit();
            if(dosteal) if(logFunc) logFunc(1,"analog joystick steals mouse port.\n");
            ppsd->_deviceresult = 0; // means port OK.
            // - - --  -- end allocjoyport - - - - --

        } // end if open one device unit

    }
    // here if both ports are !=0 means total fail.
    if(pprops->_ports[0]._deviceresult != 0 &&
        pprops->_ports[1]._deviceresult != 0)
    {
        closeProportionalSticks(pprops);
        if(logFunc) logFunc(2,getProportionalStickErrorMessage(PROPJOYRET_ALLDEVICEUSED));
        if(preturncode) *preturncode = PROPJOYRET_ALLDEVICEUSED;
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
    UWORD potsBitsToAlloc = 0 ;
    // we only need to alloc start, then for each pots,DAT, not OUT bits (If I get it well) !!
    if(pprops->_ports[0]._deviceresult == 0)
    {
//     printf("gameport 0 device unit opened\n");
        potsBitsToAlloc |= (PGBIT_DATLX|PGBIT_DATLY);
    }
    if(pprops->_ports[1]._deviceresult == 0)
    {
//     printf("gameport 1 device unit opened\n");
        potsBitsToAlloc |= (PGBIT_DATRX|PGBIT_DATRY);
    }

    /*
    "The AllocPotBits routine allocates bits in the hardware potgo
    register that the application wishes to manipulate via
    WritePotgo.  The request may be for more than one bit.  A
    user trying to allocate bits may find that they are
    unavailable because they are already allocated, or because
    the start bit itself (bit 0) has been allocated, or if
    requesting the start bit, because input bits have been
    allocated.  A user can block itself from allocation: i.e.
    it should FreePotgoBits the bits it has and re-AllocPotBits if
    it is trying to change an allocation involving the start bit."
    */

    // Krb note: lowlevel block all "DAT" bits as soon as opened.
    // this is in theory OS breaking, but let's just free them, before realloc !
    // sad because gameport device unit are free or locked by ll
    // according to current lowlevel use like it should.
    // In  all cases, configuration would make either DB9 ports managed
    // by lowlevel or by "potgo", but not both.
    //re? -> yes. sometimes something doesnt free them, hard to know why.
    FreePotBits(potsBitsToAlloc);

//printf("try allocate bits:%08x\n",(int)potsBitsToAlloc);
    pprops->_allocatedBits = AllocPotBits(potsBitsToAlloc | PGBIT_START); // #0b0101 0000 00000001

//printf("allocated bits:%08x\n",(int)pprops->_allocatedBits);
    if(pprops->_allocatedBits != (potsBitsToAlloc| PGBIT_START))
    {
        if(logFunc) logFunc(2,getProportionalStickErrorMessage(PROPJOYRET_NOANALOGPINS));
        if(preturncode) *preturncode = PROPJOYRET_NOANALOGPINS;
        closeProportionalSticks(pprops);

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
    // this must be done before launching interupt
    pprops->_calibration[0].x.min = 255;
    pprops->_calibration[0].y.min = 255;
    pprops->_calibration[1].x.min = 255;
    pprops->_calibration[1].y.min = 255;


    struct PPSticksInteruptData *pintdata;
    pprops->_pintdata = pintdata = AllocVec(sizeof(struct PPSticksInteruptData), MEMF_PUBLIC|MEMF_CLEAR);
    if(!pintdata)
    {
        closeProportionalSticks(pprops);
        if(preturncode) *preturncode = PROPJOYRET_ALLOC;
        return NULL;
    }
    pintdata->_allocatedbits = pprops->_allocatedBits;

    int useVblankAsTimer; // = (timerMethod == PROPJOYTIMER_VBLANK_ADDINT);
#ifdef PPJSCODE_ALLOWLOWLEVELTIMER
    if(timerMethod == PROPJOYTIMER_LOWLEVEL_ADDTIMER && (LowLevelBase != NULL))
    {
        pprops->_ll_intHandler = AddTimerInt(&interuptfunc_addIntServer,pintdata);
        if(pprops->_ll_intHandler == NULL)
        {   // no cia timer available, switch back to vblank !
            timerMethod = PROPJOYTIMER_VBLANK_ADDINT;
            useVblankAsTimer = 1;
        }else
        {
            useVblankAsTimer=0;
            // microseconds, max is 90 000 , 20000 -> 50hz
            //  3rd param: 0 one shot, 1, multiple. (what an excellent function)
            StartTimerInt(pprops->_ll_intHandler, 20000, /*continuous*/ 1);
        }
    } else
    {
        useVblankAsTimer = 1;
    }
#else
    timerMethod = PROPJOYTIMER_VBLANK_ADDINT;
    useVblankAsTimer = 1;
#endif
    if(useVblankAsTimer)
    {
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
        rbfint->is_Node.ln_Name = (char *)"MAMEpotgo";
        rbfint->is_Data = (APTR)pintdata;
        rbfint->is_Code = &interuptfunc_addIntServer;

        AddIntServer(INTB_VERTB,rbfint);
        if(logFunc) logFunc(0,"Analog joystick use VBlank interupt.\n");
    }
    if(logFunc) logFunc(0,"Analog joystick inited.\n");
    return pprops;
}

void closeProportionalSticks(struct ProportionalSticks *pprops)
{
    if(!pprops) return;

#ifdef PPJSCODE_ALLOWLOWLEVELTIMER
    // close ll cia timer if needed
    if(pprops->_ll_intHandler && (LowLevelBase != NULL))
    {
        StopTimerInt(pprops->_ll_intHandler);
        RemTimerInt(pprops->_ll_intHandler);
        pprops->_ll_intHandler = NULL;
    }
#endif
    // close vblank interupt, if using it
    if(pprops->_rbfint)
    {
        RemIntServer(INTB_VERTB,pprops->_rbfint);
        FreeVec(pprops->_rbfint);
    }
    if( pprops->_pintdata) FreeVec(pprops->_pintdata);
    //if(pprops->_signr != -1) FreeSignal(pprops->_signr);

    //closetimer? ->no just using vbl at the moment.

    //free potgo
    if(pprops->_allocatedBits != 0)
    {
        FreePotBits(pprops->_allocatedBits);
    }

    // order is important, to bring back mouse port to iput.device,
    // we must free it first.

    //free joyport devices
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
#ifdef PPJSCODE_TAKEPORT0CONTROL
    // order is important
    makeInputDeviceUseMousePortBack(pprops);
#endif

    // remove gameport messagerie
    if( pprops->_portAdded)
    {
        RemPort(&(pprops->_gameportPort));
       // pprops->_portAdded = 0;
    }


    FreeVec(pprops);
}

// should be asked once per frame by ports, at any moment.
// if error or still not ready, return ~0 (invalid). Do WaitTOF() twice after init and retry.
// Bits 15-8   POT0Y value or POT1Y value
// Bits 7-0    POT0X value or POT1X value
// bit 16: bt1, bit 17: bt2.
void ProportionalSticksUpdate(struct ProportionalSticks *prop)
{
    // would work after init + 2 frames.
    if((!prop->_pintdata) || (prop->_pintdata->_nbcalls<2)) return;

    ULONG invertXFlagMask = PROPJOYFLAGS_PORT1_INVERSEX;
    ULONG invertYFlagMask = PROPJOYFLAGS_PORT1_INVERSEY;

    for(int iport=0;iport<2;iport++, invertXFlagMask<<=2, invertYFlagMask<<=2)
    {
        // if port concerned...
        if( prop->_ports[iport]._deviceresult !=0 ) continue;

        UWORD potdat = prop->_pintdata->_last_potxdat[iport];
        UWORD joydat = prop->_pintdata->_last_joyxdat[iport];
        UWORD vx,vy,bt1,bt2;

        vx = (potdat & 0x0ff);
        vy = (potdat>>8);
        bt1 = ((joydat & (1<<1))!=0);
        bt2 = ((joydat & (1<<9))!=0);
        // the sad C64 paddle / Amiga propjoy XY inversion (theorically, to be tested).
        // managed once for all here.
        if(((iport == 0) && (prop->_flags & PROPJOYFLAGS_PORT1_INVERTXY))
         || ((iport == 1) && (prop->_flags & PROPJOYFLAGS_PORT2_INVERTXY))
            )
        {
            UWORD t=vx; vx=vy; vy=t;
            t=bt1; bt1=bt2; bt2=t; // on c64/atari paddles,there's a button on each, they because 2 buttons on joysticks.
        }
        // - - - - -  - let's go for that auto-calibration affair
        if(vx<prop->_calibration[iport].x.min) prop->_calibration[iport].x.min = vx;
        if(vx>prop->_calibration[iport].x.max) prop->_calibration[iport].x.max = vx;
        if(vy<prop->_calibration[iport].y.min) prop->_calibration[iport].y.min = vy;
        if(vy>prop->_calibration[iport].y.max) prop->_calibration[iport].y.max = vy;

        // only one of the direction may be used, so consider valid if any.
        if(prop->_calibration[iport].x.max>prop->_calibration[iport].x.min ||
            prop->_calibration[iport].y.max>prop->_calibration[iport].y.min)
        {
            UWORD dx = prop->_calibration[iport].x.max - prop->_calibration[iport].x.min;

            if(dx>0)
            {   // in my knowledge divu.w would be used, but well: compilers...                        
                prop->_values[iport].x = (WORD)((ULONG)(vx-prop->_calibration[iport].x.min)*255) / dx; // would be 0->255
                if((prop->_flags & invertXFlagMask)!=0)  prop->_values[iport].x = 255 -  prop->_values[iport].x ;
            } else  prop->_values[iport].x = 128; // default center.

            UWORD dy = prop->_calibration[iport].y.max - prop->_calibration[iport].y.min;

            if(dy>0)
            {   // in my knowledge divu.w would be used, but well: compilers...
                prop->_values[iport].y = (WORD)((ULONG)(vy-prop->_calibration[iport].y.min)*255) / dy; // would be 0->255
                if((prop->_flags & invertYFlagMask)!=0)  prop->_values[iport].y = 255 -  prop->_values[iport].y ;
            } else  prop->_values[iport].y = 128; // default center.

            prop->_values[iport].bt = bt1 | (bt2<<1);

            prop->_values[iport].valid = 1;
        } // end if valid

    } // end iport loop

}


