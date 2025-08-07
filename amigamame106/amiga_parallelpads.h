#ifndef AMIGA_PARALLELPADS_H
#define AMIGA_PARALLELPADS_H
/**
    Parallel pad reading
    Alloc resource and use vblank interupt to to not miss pressings.

    Values are to be read in UWORD (16b)
    ->_ppidata->_last_checked
    bits meaning down there

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <exec/interrupts.h>
#include <hardware/cia.h>

// this is experimental unofficial code to gain Bt2 on parallel joystick "4", as "ACK" signal,
#define PARALLELJOYEXTENSION_USEPORT4BT2INTERUPT 1

struct Task;

struct ParPadsInteruptData {
    // values written by interuption and read by main thread
    UWORD   _last_cia; // contains actual parrallel port joystick bits, last value read.
    UWORD   _last_checked; // contains actual parrallel port joystick bits.
    UWORD   _last_checked_changes; // xor from last check, 0 if no change.
    UWORD _d;
    // to signal out
    struct Task *_Task;
    ULONG _Signal;
    // privatest.
    int _lastAckFlag;
};

// keep what's to open and close.
struct ParallelPads // : public AParallelPads
{
    UWORD _parallelResOK;
    UWORD _parallelBitsOK;
    BYTE _signr;
    BYTE _d;
    WORD _dd;

    // - - - -    
    struct Interrupt _rbfint,_ciaint;
    UWORD _vertbintOk,_ciaintOk;
    // must be allocated in MEMF_PUBLIC
    struct ParPadsInteruptData *_ppidata;


};

/**
 * @brief hasParallelPort
 * tell if there is a parallel port on this machine.
 * some hardware has no parallel port at all (everything not amiga classic).
 * don't even use createParallelPads() if return 0.
 * @return
 */
int hasParallelPort();

/**
 * @brief createParallelPads
 * @param readJ4Bt2WithInterupt
 *      will install cia interupt to get 4rth joystick second button. if 0, will not.
 * @return null if error, else read ->_ppidata->_last_checked to get values.
 */
struct ParallelPads *createParallelPads(int readJ4Bt2WithInterupt);

/**
 * @brief closeParallelPads
 * @param parpads
 */
void closeParallelPads(struct ParallelPads *parpads);


// bits in ->_ppidata->_last_checked
// (directions are the 8bit parallel "data pins")
#define APARJOY_J4_RIGHT 0x8000
#define APARJOY_J4_LEFT 0x4000
#define APARJOY_J4_DOWN 0x2000
#define APARJOY_J4_UP 0x1000

#define APARJOY_J3_RIGHT 0x0800
#define APARJOY_J3_LEFT 0x0400
#define APARJOY_J3_DOWN 0x0200
#define APARJOY_J3_UP 0x0100
// 2 official fire buttons are from control in input mode:
#define APARJOY_J3_FIRE1 0x0004 // aka CIAF_PRTRSEL
#define APARJOY_J4_FIRE1 0x0001 // CIAF_PRTRBUSY

// = = == = winning Sega SMS pads "Button2" on the parallel port Joystick extensions:
// this is an unofficial way, but anyway there's only 2 input pins left on parallel port:
// software supporting this is... anything using this source !
// CIAF_PRTRPOUT,  available and easy to add, and I just tested it works.
// solder one more wire for joy3 bt2:
// SMS pads DB9 J3 "pin 9" ->  to parallel port DB25 "PE" or "POUT" port (pin 12)
#define APARJOY_J3_FIRE2 0x0002
// WARNING, the following is experimental hack, not really read value,
// pin is "Acknowlege" and throw interuptions, not value.
// (there is only 3 readable control input bits on amiga par port).
// solder one more wire for joy4 bt2:
// SMS pads DB9 J4 pin 9 ->  to parallel port DB25 "ACK" (pin 10)
// then this value is faked on and off so you can read it easily:
#define APARJOY_J4_FIRE2 0x0008


/*  note: on parallel port looking at the amiga back
 the pin line up starts with:
  13   12    11    10   9 -> 2
  SEL  POUT  BUSY  ACK  Data
used as:
 J3Bt1  ---  J4Bt1 ---  (used as the 8 directions) -> the official commodre way.
We add:
      J3Bt2       J4Bt2
*/
#ifdef __cplusplus
}
#endif

#endif
