#ifndef AMIGA_PARALLELPADS_H
#define AMIGA_PARALLELPADS_H
/**
        Things to read p

*/
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>

struct Task;
struct Interrupt;

struct ParPadsInteruptData {
    UWORD   _last_cia;
    UWORD   _last_checked;
    UWORD   _last_checked_changes;
    UWORD _d;
    // to signal out
    struct Task *_Task;
    ULONG _Signal;

};


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




struct ParallelPads *createParallelPads();
void checkParallelPads();
//void readParallelPads(struct ParallelPads *parpads);
void closeParallelPads(struct ParallelPads *parpads);

#ifdef __cplusplus
}
#endif

#endif
