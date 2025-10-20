#include <vector>
#include <string>
#include "amiga_inputs_interface.h"
#include "amiga_config.h"
#include "amiga_inputs_midi.h"
#include <proto/exec.h>
extern "C" {
    // from MAME
    #include "input.h"
    #include "mamelog.h"
    #include "usrintrf.h"
}

using namespace std;

static vector<string> _midinames;
static vector<string> _midinamesA;


static void *midiin_Create(void *registerer,fAddOsCode addOsCode)
{
    MameConfig::Controls &configControls = getMainConfig().controls();

    if(configControls._serialPort_Player==0 ||
     configControls._serialPort_Type == 0 ) return NULL;

    int hasMidi = MidiControls_hasMidi();
    if(!hasMidi)
    {
        const char *perr="MIDI controls need camd.library to be installed.";
        ui_popup_time(5,perr);
        loginfo(2,perr);
        return NULL;
    }

    struct sMidiController *p = MidiControls_create();
    if(!p) return NULL;
    p->_mapmode = configControls._serialPort_Type;

    // - - publish all midi buttons as very abstract buttons, not mapped by default.
    if(_midinames.size()==0)
    {
        _midinames.resize(128*4);
        for(int j=0;j<4;j++)
        for(int i=0;i<128;i++)
        {
            char temp[16];
            snprintf(temp,15,"MidiC%d_%02x",j,i);
            _midinames[(128*j)+i] = temp;
        }
    }
    for(int j=0;j<4;j++)
    for(int i=0;i<128;i++)
    {
        os_code_info osci={ _midinames[(128*j)+i] .c_str(),i+(j*128),CODE_OTHER_DIGITAL};
        addOsCode(registerer,&osci,1);
    }
    // - - - then this :
    _midinamesA.resize(12);
    for(int i=0;i<12;i++)
    {
        char temp[16];
        snprintf(temp,15,"MidiAnlg%02x",i);
        _midinamesA[i] = temp;
         os_code_info osci={_midinamesA[i].c_str(),512+i,CODE_OTHER_ANALOG_ABSOLUTE};
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
    if(oscode<512)
        return ((struct sMidiController *)o)->_keys[oscode&0x7f][oscode>>7];
    oscode -=512;
    return ((struct sMidiController *)o)->_analog[oscode];
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
