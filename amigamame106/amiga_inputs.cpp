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
#include "amiga_inputs_parallelpads.h"
#include "amiga_inputs_propjoy.h"
#include "amiga_inputs_lightgun.h"
#include "amiga_inputs_midi.h"

#include "amiga_config.h"

//#include <stdio.h>
extern struct sMameInputsInterface g_ipt_ParallelPads;



struct sMameInputsInterface *g_inputInterfaces[] =
{
    &g_ipt_Keyboard,
    &g_ipt_LLMouses,
    &g_ipt_ParallelPads,
    // &g_ipt_PropJoy,
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
    return 0;
}



/* Joystick calibration routines BW 19981216 */
/* Do we need to calibrate the joystick at all? */
int osd_joystick_needs_calibration(void)
{
    return 0;
}
/* Preprocessing for joystick calibration. Returns 0 on success */
void osd_joystick_start_calibration(void)
{

}
/* Prepare the next calibration step. Return a description of this step. */
/* (e.g. "move to upper left") */
const char *osd_joystick_calibrate_next(void)
{
    return NULL;
}
/* Get the actual joystick calibration data for the current position */
void osd_joystick_calibrate(void)
{

}
/* Postprocessing (e.g. saving joystick data to config) */
void osd_joystick_end_calibration(void)
{

}

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


// ------------------------

// - - - - -mame minimix input module api:

static const char * const parpadsbtnames[2][11]={
    {
        "PrPad3 Blue",
        "PrPad3 Red",
        "PrPad3 UP",
        "PrPad3 DOWN",
        "PrPad3 LEFT",
        "PrPad3 RIGHT"
    },
    {
        "PrPad4 Blue",
        "PrPad4 Red",
        "PrPad4 UP",
        "PrPad4 DOWN",
        "PrPad4 LEFT",
        "PrPad4 RIGHT"
    },
};
TODO
static void *parpads_Create(void *registerer,fAddOsCode addOsCode)
{
    MameConfig::Controls &configControls = getMainConfig().controls();
    MameConfig::Misc &configMisc = getMainConfig().misc();

    // - - - - parralel port 3&4 joystick extension: init if needed.
    // loop for parallel port
    int useParallelExtension=0;
    int readj4bt2 = 0;
    for(int ipar=0 ; ipar<2 ;ipar++)
    {
        int iPlayer = configControls._parallelPort_Player[ipar] ;
        if( iPlayer == 0 ) continue;
        int type = configControls._parallel_type[ipar];
        if(type == 0 ) continue;

        useParallelExtension = 1;
        if(ipar == 1) readj4bt2=1;
    }

    if(useParallelExtension ==0) return NULL; // no instance, no init.

    struct ParallelPads *p = createParallelPads(readj4bt2); // could fail.
    if(!p) return NULL;

    // then parallel port joystick extension pr3 pr4
    for(int ipar=0;ipar<2;ipar++) // actually 2
    {
        int iplayer = configControls._parallelPort_Player[ipar];
        if(iplayer == 0) continue;
        iplayer--;
        int itype = configControls._parallel_type[ipar];
        if(itype == 0) continue; //still not inited

        int iport = 2+ipar; // we hack parallel pads as Lowlevel Pads3 and 4 !!!
        int ipshft = iport<<8;
        const int mamecodeshift =
            ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT) *iplayer ;

    // joystick can only manage 1 or 2 bt pads here (2 for sega SMS pads)...
      vector<os_code_info> kbi2={
      // note standard parallel port extensions joysticks manages 1bt each, 2nd button only if hw hack.
        {parpadsbtnames[ipar][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
        {parpadsbtnames[ipar][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
        //
        {parpadsbtnames[ipar][2],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
        {parpadsbtnames[ipar][3],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
        {parpadsbtnames[ipar][4],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
        {parpadsbtnames[ipar][5],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
        };
        //_kbi.insert(_kbi.end(),kbi2.begin(),kbi2.end());
        addOsCode(registerer,kbi2.data(),kbi2.size());
    } // end loop per par

    return p;
}



static void *parpads_Close(void *o)
{
    closeParallelPads((struct ParallelPads *)o);
}

struct sMameInputsInterface g_ipt_ParallelPads=
{
    parpads_Create,
    parpads_FrameUpdate,
    parpads_GetCode,
    parpads_Close,
    NULL //    PostInputPortInitCheck
};

