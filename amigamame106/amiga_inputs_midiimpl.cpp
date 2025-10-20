#include <vector>
#include <string>
#include "amiga_inputs_interface.h"
#include "amiga_config.h"
#include "amiga_inputs_midi.h"
#include <proto/exec.h>
extern "C" {
    // from MAME
    #include "input.h"
}

using namespace std;

static vector<string> _midinames;

static void *midiin_Create(void *registerer,fAddOsCode addOsCode)
{
    MameConfig::Controls &configControls = getMainConfig().controls();

    if(configControls._serialPort_Player==0 ||
     configControls._serialPort_Type == 0 ) return NULL;

    struct sMidiController *p = MidiControls_create();
    if(!p) return NULL;

    // - - publish all midi buttons as very abstract buttons, not mapped by default.
    if(_midinames.size()==0)
    {
        _midinames.resize(128);
        for(int i=0;i<128;i++)
        {
            char temp[16];
            snprintf(temp,15,"MidiInC%d",i);
            _midinames[i] = temp;
        }
    }
    for(int i=0;i<128;i++)
    {
        os_code_info osci={_midinames[i].c_str(),i,CODE_OTHER_DIGITAL};
        addOsCode(registerer,&osci,1);
    }
    // - - - then this :
    {
        os_code_info osci={"MidiNoteAsX",128,CODE_OTHER_ANALOG_ABSOLUTE};
        addOsCode(registerer,&osci,1);
    }
    {
        os_code_info osci={"MidiVolAsY",129,CODE_OTHER_ANALOG_ABSOLUTE};
        addOsCode(registerer,&osci,1);
    }

    return p;
}
// optional, per frame.
static void midiin_FrameUpdate(void *o)
{
     MidiControls_update((struct sMidiController *)o);
}
// mandatory, used by real time.
static int midiin_GetCode(void *o, ULONG oscode)
{
    if(oscode<128)
        return ((struct sMidiController *)o)->_keys[oscode];
    if(oscode==128) return ((struct sMidiController *)o)->_analog_x;
    return ((struct sMidiController *)o)->_analog_y;
}
static void midiin_Close(void *o)
{
    MidiControls_close((struct sMidiController *)o);
}

struct sMameInputsInterface g_ipt_MidiIn=
{
    midiin_Create,
    midiin_FrameUpdate,
    midiin_GetCode,
    midiin_Close,
    NULL //    PostInputPortInitCheck
};
