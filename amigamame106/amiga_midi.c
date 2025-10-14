#include "amiga_midi.h"

#include <proto/exec.h>

struct sMidiControllerPrivate
{
    struct sMidiController _s;
    struct Interrupt* _interupt;
    struct LPInteruptData* _pintdata;
    int _interuptok;

    UWORD _miny;
    UWORD _maxy;
};

int MidiControls_hasMidi()
{

	return 0;
}

struct sMidiController* MidiControls_create()
{
    struct sMidiControllerPrivate* p = (struct sMidiControllerPrivate*)AllocVec(sizeof(struct sMidiControllerPrivate), MEMF_CLEAR);
    if (!p) return NULL;

    return (sMidiController*)p;
}
void MidiControls_update(struct sMidiController* p)
{
    if (!p) return;
}
void MidiControls_close(struct sMidiController* p)
{
    if (!p) return;
    FreeVec(p);
}