#ifndef AMIGA_MIDI_H
#define AMIGA_MIDI_H
/**
    read midi inputs as game controllers, it makes lots of buttons...
*/
#ifdef __cplusplus
extern "C" {
#endif
#include <exec/types.h>

#define MIDICONF_AllBUTTONS 1
#define MIDICONF_NotesAreAnalogX 2
#define MIDICONF_AllButtonsAndSliders 3


// manage
struct sMidiController
{
    ULONG _signal;
    UWORD _analog[12]; // 3 + 8
    int _mapmode;
    UBYTE _chans[4];
    UBYTE _chanMap[16];
    int _nbchans; // nbchans ever seen, max 4.
    UBYTE _keys[128][4];

};
int MidiControls_hasMidi();

struct sMidiController *MidiControls_create();
void MidiControls_update(struct sMidiController*p);
void MidiControls_close(struct sMidiController*p);

#ifdef __cplusplus
}
#endif

#endif
