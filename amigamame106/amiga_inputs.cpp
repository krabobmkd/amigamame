/**************************************************************************
 *
 * Copyright (C) 2025 Vic Ferry (http://github.com/krabobmkd)
 * License is LGPL
 *
 *************************************************************************/
#include "amiga_inputs.h"

#include <vector>
#include <string>
// from mame:
extern "C" {
    #include "inptport.h"
}


//  - - - -
#include "amiga_inputs_interface.h"
#include "amiga_inputs_kbd_ll.h"
#include "amiga_inputs_parpadsimpl.h"
#include "amiga_inputs_propjoyimpl.h"
//#include "amiga_inputs_lightgun.h"
#include "amiga_inputs_midi.h"

struct sMameInputsInterface *g_inputInterfaces[] =
{
    &g_ipt_Keyboard,
    &g_ipt_LLMouses,
    &g_ipt_ParallelPads,
    &g_ipt_PropJoy,
    // &g_ipt_MidiIn,
    NULL,
};

using namespace std;

/** Manage agregation of oscode->mame input code list
 *  inited by all input modules.
*/
class CodeList
{
    public:
    vector<os_code_info> _codes;
    void AddOsCode(os_code_info *pFirst, int nbcodes)
    {
        while(nbcodes>0 )
        {
             _codes.push_back({pFirst->name,pFirst->oscode+_currentInitCode,pFirst->inputcode});
            pFirst++;
            nbcodes--;
        }
    }
    void endList() {
        _codes.push_back({NULL,0,0});
    }
    void clear() {
        _codes.clear();
    }
    int _currentInitCode;

};
static CodeList g_codeList;

static void AddOsCode(void *m,os_code_info *pFirst, int nbcodes)
{
   ((CodeList *)m)->AddOsCode(pFirst,nbcodes);
}

// - - - - - -


static vector<void *>_instances;


void Inputs_Free()
{
    struct sMameInputsInterface **pii = &g_inputInterfaces[0];
    int i=0;
    while(*pii && i<_instances.size())
    {
        void *pinstance = _instances[i];
        if(pinstance && (*pii)->Close) (*pii)->Close(pinstance);
        pii++;
        i++;
    }
    _instances.clear();
}

/*
  return a list of all available keys (see input.h)
*/
const os_code_info *osd_get_code_list(void)
{
    Inputs_Free();
    // create instances for each input module,each will do according to config.controls, will open libs...
    struct sMameInputsInterface **pii = &g_inputInterfaces[0];
    g_codeList._currentInitCode = 0;
    while(*pii)
    {
        void *pinstance = (*pii)->Create(&g_codeList,&AddOsCode);
        _instances.push_back(pinstance); // can be null, but keep order.
        g_codeList._currentInitCode += (1<<10);
        pii++;
    }
    g_codeList.endList();
    return g_codeList._codes.data();

}

void Inputs_FrameUpdate()
{
    struct sMameInputsInterface **pii = &g_inputInterfaces[0];
    int i=0;
    while(*pii && i<_instances.size())
    {
        void *pinstance = _instances[i];
        if(pinstance && (*pii)->FrameUpdate) (*pii)->FrameUpdate(pinstance);
        pii++;
        i++;
    }

}


/*
  tell whether the specified key is pressed or not. keycode is the OS dependant
  code specified in the list returned by osd_get_key_list().
*/
INT32 osd_get_code_value(os_code oscode)
{
    int im = oscode>>10;
    return g_inputInterfaces[im]->GetCode(_instances[im],oscode & 0x3ff);
}

/*
  Return the Unicode value of the most recently pressed key. This
  function is used only by text-entry routines in the user interface and should
  not be used by drivers. The value returned is in the range of the first 256
  bytes of Unicode, e.g. ISO-8859-1. A return value of 0 indicates no key down.

  Set flush to 1 to clear the buffer before entering text. This will avoid
  having prior UI and game keys leak into the text entry.
*/
int osd_readkey_unicode(int flush)
{
   // printf("osd_readkey_unicode\n");
   // not used in standard MAME !
    return 0;
}

 // we calibrate our own way !

/* Joystick calibration routines BW 19981216 */
/* Do we need to calibrate the joystick at all? */
int osd_joystick_needs_calibration(void){ return 0;}
/* Preprocessing for joystick calibration. Returns 0 on success */
void osd_joystick_start_calibration(void){}
/* Prepare the next calibration step. Return a description of this step. */
/* (e.g. "move to upper left") */
const char *osd_joystick_calibrate_next(void){ return NULL;}
/* Get the actual joystick calibration data for the current position */
void osd_joystick_calibrate(void){}
/* Postprocessing (e.g. saving joystick data to config) */
void osd_joystick_end_calibration(void){}

/*
  inptport.c defines some general purpose defaults for key and joystick bindings.
  They may be further adjusted by the OS dependent code to better match the
  available keyboard, e.g. one could map pause to the Pause key instead of P, or
  snapshot to PrtScr instead of F12. Of course the user can further change the
  settings to anything he/she likes.
  This function is called on startup, before reading the configuration from disk.
  Scan the list, and change the keys/joysticks you want.
*/
void osd_customize_inputport_list(input_port_default_entry *defaults)
{
}

// added on osd api by krb, thrown late during inits, allow to check controllers are usable by current game.
void osd_post_input_port_init_check()
{
    // at this level both osd_code_list and input_port_init are inited.
    struct sMameInputsInterface **pii = &g_inputInterfaces[0];
    int i=0;
    while(*pii && i<_instances.size())
    {
        void *pinstance = _instances[i];
        if(pinstance && (*pii)->PostInputPortInitCheck) // optional func.
            (*pii)->PostInputPortInitCheck(pinstance);
        pii++;
        i++;
    }
}


