/*
 * amiga_inputs_midiimpl.cpp
 * Purpose: MIDI input implementation and MAME integration layer
 *
 * ╔════════════════════════════════════════════════════════════════════════╗
 * ║              🎹 MIDI INPUT IMPLEMENTATION 🎼                          ║
 * ║  ┌──────────────────────────────────────────────────────────────┐    ║
 * ║  │                                                               │    ║
 * ║  │   MIDI Keyboard/Controller                                    │    ║
 * ║  │          │                                                    │    ║
 * ║  │          ▼                                                    │    ║
 * ║  │   [CAMD Messages]                                             │    ║
 * ║  │          │                                                    │    ║
 * ║  │          ├──► Note Events ──► Buttons                         │    ║
 * ║  │          ├──► Pitch Bend  ──► Analog Axis                     │    ║
 * ║  │          └──► Controllers ──► Analog Sliders                  │    ║
 * ║  │                                                               │    ║
 * ║  └──────────────────────────────────────────────────────────────┘    ║
 * ║        Turn your MIDI gear into game controllers!                     ║
 * ╚════════════════════════════════════════════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
static vector<string> _midinamesAnalog;


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
        _midinames.resize(128*AI_MIDI_NBCHANMAP);
        for(int j=0;j<AI_MIDI_NBCHANMAP;j++)
        for(int i=0;i<128;i++)
        {
            char temp[16];
            snprintf(temp,15,"MidiC%d_%02x",j,i);
            _midinames[(128*j)+i] = temp;
        }
    }
    for(int j=0;j<AI_MIDI_NBCHANMAP;j++)
    for(int i=0;i<128;i++)
    {
        os_code_info osci={ _midinames[(128*j)+i].c_str(),i+(j*128),CODE_OTHER_DIGITAL};
        addOsCode(registerer,&osci,1);
    }
    // - - - then this :

    #define START_CODE_ANALOG 512
  //   printf("p->_mapmode:%d\n",p->_mapmode);
    if( p->_mapmode == 2)
    {
        int iplayer = configControls._serialPort_Player -1; // known >0.
        int analogshift = (MOUSECODE_2_ANALOG_X-MOUSECODE_1_ANALOG_X)*iplayer;
        // note: mapping to MOUSECODE_XX_ANALOG_X
        // makes it automatic to be mapped on most analog games.
        {
            os_code_info osci={"MidiAnlgNotes",START_CODE_ANALOG,MOUSECODE_1_ANALOG_X+(analogshift)};
            addOsCode(registerer,&osci,1);
        }
        {
            os_code_info osci={"MidiAnlgVel",START_CODE_ANALOG+1,MOUSECODE_1_ANALOG_Z+(analogshift)};
            addOsCode(registerer,&osci,1);
        }
        {
            os_code_info osci={"MidiAnlgVol",START_CODE_ANALOG+2,MOUSECODE_1_ANALOG_Y+(analogshift)};
            addOsCode(registerer,&osci,1);
        }
    }
    _midinamesAnalog.resize(AI_MIDI_NBANALOG-3);
    for(int i=0;i<AI_MIDI_NBANALOG-3;i++)
    {
        char temp[16];
        snprintf(temp,15,"MidiAnlg%d",(i+1));
        _midinamesAnalog[i] = temp;
         os_code_info osci={_midinamesAnalog[i].c_str(),START_CODE_ANALOG+3+i,CODE_OTHER_ANALOG_ABSOLUTE};
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
    if(oscode<START_CODE_ANALOG)
        return (int)((struct sMidiController *)o)->_keys[oscode&0x7f][oscode>>7];
    oscode -=START_CODE_ANALOG;
    return ((struct sMidiController *)o)->_analog[oscode];
}
static void midiin_Close(void *o)
{
    MidiControls_close((struct sMidiController *)o);
}
static void midiin_Post(void *o)
{
    // could help if midi device monkeyying...
    MidiControls_FlushMessages((struct sMidiController *)o);
}


struct sMameInputsInterface g_ipt_MidiIn=
{
    midiin_Create,
    midiin_FrameUpdate,
    midiin_GetCode,
    midiin_Close,
    midiin_Post //    PostInputPortInitCheck
};

/*
 * MIDI magic! Turn music into motion! 🎹✨
 *      __
 *     /  \
 *    | ◉◉ |  <- This panda plays a mean game of MIDI Pac-Man!
 *    |  > |
 *     \__/
 */
