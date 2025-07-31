#ifndef AMIGA_PROPORTIONALJOYSTICK_H
#define AMIGA_PROPORTIONALJOYSTICK_H
/**
    Things to read Proportional Analog Josticks and paddles.
    The "C64/Atari8Bit" way...

    Moving a paddle will make the voltage varies...
    To read a value from a voltage, the hardware flush a capacitor (potgo)
    and then we wait a frame or 2 (well, a constant amount of time.)
    and then we read how much was filled in those caps...

    So we got here:
    software resource accaparation + timer management and interupts + reading direct hardware value.
    Also, need something for calibration of leftmost/rightmost (and center?) values.
    then we just let game engine read the last value cheked.

    - Can Manage Amiga classic DB9 Port1 and Port2, or both.
    - Need calibration because of devices, but also internal timers can change.
    Ask gently gameport device.
    -> test if colision with lowlevel.library josticks/mouse management.
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>

struct Task;
struct Interrupt;

// internal use
struct PPSticksInteruptData {

    UWORD _last_potxdat[2];
    ULONG _nbcalls; // should only be valid after 2.
    // to signal out
    //struct Task *_Task;
    //ULONG _Signal;

};

// this should be public to be configured.
// could be used
//struct ProportionalStickCalibration
//{
//    LONG _xstart,_xcenter,_xend;
//    LONG _ystart,_ycenter,_yend;
//};

// internal use
struct PStickDevice {
    struct IOStdReq _gameportIOReq;
    UBYTE _deviceresult;
    UBYTE _portType;

   // struct ProportionalStickCalibration _calibration;
};

// keep what's to open and close. internal use
struct ProportionalSticks
{
    struct MsgPort _gameportPort;
    // to signal out
    struct Task *_Task;
    ULONG _flags; // as passed to init.

    UBYTE _portAdded;
    UBYTE _dummy;

    UWORD _allocatedBits;

    struct PStickDevice _ports[2];

    APTR _potgobase;

    // - - - -
//    BYTE _signr;
//    BYTE _d;
//    WORD _dd;

     // must be allocated in MEMF_PUBLIC
    struct Interrupt *_rbfint;
     // must be allocated in MEMF_PUBLIC
    struct PPSticksInteruptData *_pintdata;

};

// some hardware has no potentiometer reading port at all (everything not amiga classic).
// if it returns 0, don't even try createProportionalSticks().
int hasProportionalStickResource();

#define PROPJOYFLAGS_PORT1 1
#define PROPJOYFLAGS_PORT2 2
#define PROPJOYFLAGS_PORT1_INVERTXY 4
#define PROPJOYFLAGS_PORT2_INVERTXY 8

#define PROPJOYRET_OK 0
#define PROPJOYRET_NOHARDWARE 1
#define PROPJOYRET_ALLOC 2
#define PROPJOYRET_DEVICEFAIL 3
#define PROPJOYRET_DEVICEUSED 4
#define PROPJOYRET_NOANALOGPINS 5
//#define PROPJOYRET_NOSIGNAL 6

// public. use that.  PROPJOYFLAGS_PORT1|PROPJOYFLAGS_PORT2 for both ports.
struct ProportionalSticks *createProportionalSticks(ULONG flag, ULONG *preturncode);

void closeProportionalSticks(struct ProportionalSticks *prop);

// should be asked once per frame by ports.
// if error or still not ready, return ~0 (invalid). Do WaitTOF() twice after init and retry.
// Bits 15-8   POT0Y value or POT1Y value
// Bits 7-0    POT0X value or POT1X value
static inline ULONG getProportionalStickValues(struct ProportionalSticks *prop,ULONG iport)
{
    if((!prop->_pintdata) || (iport>1) ||
     (!prop->_ports[iport]._deviceresult !=0) ||
       (prop->_pintdata->_nbcalls<2)
    ) return 0xffffffffUL;

    // cast ulong so ~0 is an error.
    UWORD v = (UWORD) prop->_pintdata->_last_potxdat[iport];

    // C64 / Atari Pads has XY inverted ? ... ok.
    if((iport == 0) && (prop->_flags & PROPJOYFLAGS_PORT1_INVERTXY))
    {
        v = ((v>>8)&0x00ff) | (v<<8);
    } else if((iport == 1) && (prop->_flags & PROPJOYFLAGS_PORT2_INVERTXY))
    {
        v = ((v>>8)&0x00ff) | (v<<8);
    }

    return (ULONG)v;
}
static inline const char *getProportionalStickErrorMessage(ULONG errcode)
{
    if(errcode == PROPJOYRET_NOHARDWARE) return "Harware is not proportionnal joystick capable";
    if(errcode == PROPJOYRET_ALLOC) return "prop. joystick: can't alloc";
    if(errcode == PROPJOYRET_DEVICEFAIL) return "prop. joystick: can't open gameport device.";
    if(errcode == PROPJOYRET_DEVICEUSED) return "prop. joystick: gameport device locked.";
    if(errcode == PROPJOYRET_NOANALOGPINS) return "prop. joystick: can't reserve analog pins.";
 //   if(errcode == PROPJOYRET_NOSIGNAL) return "prop. joystick: can't find signal.";
    return "";
}

//
//void startCalibration(int iPort, int iCalibration)

#ifdef __cplusplus
}
#endif

#endif
