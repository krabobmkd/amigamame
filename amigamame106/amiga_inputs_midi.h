#ifndef AMIGA_MIDI_H
#define AMIGA_MIDI_H
/**
    read midi inputs as game controllers, it makes lots of buttons...
*/
#ifdef __cplusplus
extern "C" {
#endif
#include <exec/types.h>

// manage
struct sMidiController
{
    ULONG _signal;
    UWORD _analog_x,_analog_y;  // notes as _x, volume as _y.
   
};
int MidiControls_hasMidi();

struct sMidiController *MidiControls_create();
void MidiControls_update(struct sMidiController*p);
void MidiControls_close(struct sMidiController*p);

#ifdef __cplusplus
}
#endif

#endif
