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

// final, public, interpolated data after calibration, per player, per frame.
// just do
struct PPSticksValues {
    WORD valid; // if !=0 , valid.
    WORD x,y; // coord
    WORD bt; // 1 & 2
};


// internal use, just the data exchanged between interupt and process.
struct PPSticksInteruptData {

    UWORD _last_potxdat[2];
    UWORD _last_joyxdat[2];
    UWORD _allocatedbits;
    UWORD _nbcalls; // should only be valid after 2.
    // to signal out - not needed imo.
    //struct Task *_Task;
    //ULONG _Signal;

};

// private, for calibration, let's just use min/max of the known valid movements.
struct PStickCalib
{
    WORD min,max/*,_center*/;
};
struct PStickCalibXY
{
    struct PStickCalib x,y;
};

// struct PropStickCalibration
// {
//     WORD _xmin,_xmax,_xcenter;
//     WORD _ymin,_ymax,_ycenter;
// };

// internal use
struct PStickDevice {
    struct IOStdReq _gameportIOReq;
    UBYTE _deviceresult;
    UBYTE _portType;
    WORD _d;

};

// keep what's to open and close. internal use
struct ProportionalSticks
{
    // final values for each hardware ports... each may be inited or not.
    struct PPSticksValues _values[2];

    struct MsgPort _gameportPort;
    // to signal out
    struct Task *_Task;
    ULONG _flags; // as passed to init.

    UBYTE _portAdded;
    UBYTE _dummy;

    UWORD _allocatedBits;

    struct PStickDevice _ports[2];
    struct PStickCalibXY _calibration[2];
    APTR _potgobase;


    // - - - - messaging, no use atm.
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

typedef void (*PPStickInitLogFunc)(int elevel,const char *pMessage);

// public. use that.  PROPJOYFLAGS_PORT1|PROPJOYFLAGS_PORT2 for both ports.
// preturncode and logFunc can be null.
struct ProportionalSticks *createProportionalSticks(ULONG flag, ULONG *preturncode, PPStickInitLogFunc logFunc);

void closeProportionalSticks(struct ProportionalSticks *prop);

//olde ULONG getProportionalStickValues(struct ProportionalSticks *prop,ULONG iport);
// call once per frame or so, then read prop->_values and check if valid.
void ProportionalSticksUpdate(struct ProportionalSticks *prop);

// if any after createProportionalSticks()
const char *getProportionalStickErrorMessage(ULONG errcode);

#ifdef __cplusplus
}
#endif

#endif
