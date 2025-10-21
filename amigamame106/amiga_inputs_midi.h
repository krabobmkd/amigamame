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

#define AI_MIDI_NBCHANMAP 4
// manage
struct sMidiController
{
    ULONG _signal;
    WORD _analog[12]; //0 is vol
    int _mapmode;
    UBYTE _min_note;
    UBYTE _max_note;
    UWORD _d;
    UBYTE _chans[AI_MIDI_NBCHANMAP]; // maps to real channel that have been seen (vary from devices).
    UBYTE _chanMap[16]; //
    int _nbchans; // nbchans ever seen, max 4.
    UBYTE _keys[128][AI_MIDI_NBCHANMAP]; // button states for number/Channel.

};
int MidiControls_hasMidi();

struct sMidiController *MidiControls_create();
void MidiControls_update(struct sMidiController*p);
void MidiControls_close(struct sMidiController*p);

#ifdef __cplusplus
}
#endif

#endif
