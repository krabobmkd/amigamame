#ifndef AMIGA_PROPORTIONALJOYSTICK_H
#define AMIGA_PROPORTIONALJOYSTICK_H
/**
        Things to read

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <hardware/cia.h>

struct Task;
struct Interrupt;

//struct ParPadsInteruptData {
//    // values written by interuption and read by main thread
//    UWORD   _last_cia; // contains actual parrallel port joystick bits, last value read.
//    UWORD   _last_checked; // contains actual parrallel port joystick bits.
//    UWORD   _last_checked_changes; // xor from last check, 0 if no change.
//    UWORD _d;
//    // to signal out
//    struct Task *_Task;
//    ULONG _Signal;

//};

//// keep what's to open and close.
struct ProportionalSticks // : public AParallelPads
{
//    UWORD _parallelResOK;
//    UWORD _parallelBitsOK;
//    BYTE _signr;
//    BYTE _d;
//    WORD _dd;

//    // - - - -
//     // must be allocated in MEMF_PUBLIC
//    struct Interrupt *_rbfint;
//    // must be allocated in MEMF_PUBLIC
//    struct ParPadsInteruptData *_ppidata;
};

// some hardware has no parallel port at all (everything not amiga classic).
// if return 0. don't even try createProportionalSticks().
int hasProportionalStickResource();

struct ProportionalSticks *createProportionalSticks();
//void checkParallelPads();
//void readParallelPads(struct ParallelPads *parpads);
void closeProportionalSticks(struct ProportionalSticks *prop);

#ifdef __cplusplus
}
#endif

#endif
