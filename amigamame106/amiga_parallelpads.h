#ifndef AMIGA_PARALLELPADS_H
#define AMIGA_PARALLELPADS_H
/**
        Things to read p

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <hardware/cia.h>

struct Task;
struct Interrupt;

struct ParPadsInteruptData {
    // values written by interuption and read by main thread
    UWORD   _last_cia; // contains actual parrallel port joystick bits, last value read.
    UWORD   _last_checked; // contains actual parrallel port joystick bits.
    UWORD   _last_checked_changes; // xor from last check, 0 if no change.
    UWORD _d;
    // to signal out
    struct Task *_Task;
    ULONG _Signal;

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
     // must be allocated in MEMF_PUBLIC
    struct Interrupt *_rbfint;
    // must be allocated in MEMF_PUBLIC
    struct ParPadsInteruptData *_ppidata;
};

// some hardware has no parallel port at all (everything not amiga classic).
int hasParallelPort();

struct ParallelPads *createParallelPads();
void checkParallelPads();
//void readParallelPads(struct ParallelPads *parpads);
void closeParallelPads(struct ParallelPads *parpads);
/* ciabpra parallel control bits....
 * nooo these are serial port.
#define CIAF_COMDTR	(1L<<7)     Data Terminal Ready
#define CIAF_COMRTS	(1L<<6)     Request To send
#define CIAF_COMCD	(1L<<5)     Carrier Detect
#define CIAF_COMCTS	(1L<<4)     Clear To send
#define CIAF_COMDSR	(1L<<3)     DataSetReady

#define CIAF_PRTRSEL	(1L<<2)  Select ---> Commodore officialy defined as Joy4 Fire Bt1
#define CIAF_PRTRPOUT	(1L<<1)  Printer Out --> let's say it's Joy3 Bt2. winwin.
#define CIAF_PRTRBUSY	(1L<<0) Busy        ---> Commodore officialy defined as Joy3 Fire Bt1
*/

#ifdef __cplusplus
}
#endif

#endif
