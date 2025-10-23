#include "amiga_inputs_midi.h"
#include "amiga_inputs_interface.h"

// for atexit:
#include <stdlib.h>
#include <stdio.h>
#include <proto/exec.h>

#include <midi/camd.h>
#include <midi/mididefs.h>

#include <clib/camd_protos.h>
#include <inline/camd.h>

#include <dos/dos.h>

struct Library      *CamdBase = NULL;


struct sMidiControllerPrivate
{
    struct sMidiController _s;

    struct MidiNode			*_midi;
    struct MidiLink			*_link;

    // struct Interrupt* _interupt;
    // struct LPInteruptData* _pintdata;
    // int _interuptok;

    // UWORD _miny;
    // UWORD _maxy;
};
static void midiexit()
{
    if(CamdBase) CloseLibrary(CamdBase);
    CamdBase = NULL;
}
int MidiControls_hasMidi()
{
    if(CamdBase==NULL)
    {
        CamdBase = OpenLibrary("camd.library",1);
        if(CamdBase) atexit(&midiexit);
    }
	return (CamdBase != NULL);
}
static const char	*inlinkname = "in.0";
struct sMidiController* MidiControls_create()
{
    if(!MidiControls_hasMidi()) return NULL;

    struct sMidiControllerPrivate* p = (struct sMidiControllerPrivate*)AllocVec(sizeof(struct sMidiControllerPrivate), MEMF_CLEAR);
    if (!p) return NULL;

    ULONG tags[]={
		MIDI_Name,(ULONG)"MAMEIN",
		MIDI_RecvSignal, SIGBREAKB_CTRL_E,
		MIDI_MsgQueue,   100,
		MIDI_ErrFilter, CMEF_All,
		TAG_DONE};
      //  printf("Go CreateMidiA()\n");
    p->_midi = CreateMidiA((struct TagItem *)&tags[0]);

  //  printf("p->_midi: %08x\n",(int)p->_midi);

    ULONG tagsl[]={
		MLINK_Name,(ULONG)"MAMELNK",
		MLINK_Location,(ULONG)inlinkname,
		MLINK_EventMask, /*CMF_Note*/ CMF_All,
		MLINK_Comment,(ULONG)"MAME[Input]",
		TAG_DONE};

     //   printf("Go AddMidiLinkA()\n");

    p->_link = AddMidiLinkA(p->_midi, MLTYPE_Receiver,(struct TagItem *)&tagsl[0]);
  // printf("p->_link: %08x\n",(int)p->_link);

    for(int i=0;i<16;i++) p->_s._chanMap[i]=255;
    p->_s._min_note = 255;
    p->_s._max_note = 0;

    return (struct sMidiController*)p;
}
void MidiControls_FlushMessages(struct sMidiController*p)
{
    struct sMidiControllerPrivate *sp = (struct sMidiControllerPrivate *)p;
    if (!sp) return;
    MidiMsg		msg;
    while (GetMidi(sp->_midi,&msg)) {}
}
void MidiControls_update(struct sMidiController*p)
{
    struct sMidiControllerPrivate *sp = (struct sMidiControllerPrivate *)p;
    if (!sp) return;
    MidiMsg		msg;
    int minimalChanAsButton=0;
    if(sp->_s._mapmode == MIDICONF_NotesAreAnalogX)
    {
        minimalChanAsButton = 3; // do not grab note keyboards as buttons. (most likely chan 0-1-2...)
    }


   while (GetMidi(sp->_midi,&msg))
    {
        UBYTE highmess = msg.b[0]&0xf0;
        UBYTE channel = msg.b[0]&0x0f; // usually "chan".
        UBYTE isNoteOn=0;
        UBYTE isNoteOff=0;

// printf("high:%02x channel:%02x msg.b[2]:%d\n",highmess,channel,(int)msg.b[2]);
        if(highmess ==MS_NoteOn)
        {
            if(msg.b[2]>0) isNoteOn=1;
            else isNoteOff=1;
        } else if(highmess == MS_NoteOff) isNoteOff=1;

        if(isNoteOn)
        {
            if(channel>=minimalChanAsButton)
            {
                UBYTE rchan=255;
                if(sp->_s._chanMap[channel]==255)
                {
                    if(sp->_s._nbchans<AI_MIDI_NBCHANMAP)
                    {
                        rchan = sp->_s._nbchans;
                        sp->_s._chanMap[channel]=rchan;
                        sp->_s._chans[ sp->_s._nbchans]=channel;
                        //printf("set chan:%d to slot:%d\n",channel,sp->_s._nbchans);
                        sp->_s._nbchans++;

                    }
                }else rchan= sp->_s._chanMap[channel];
                if(rchan != 255)
                {
                   // printf("noteon:%02x slot:%d\n",(int)(msg.b[1]&0x7f),rchan);
                    sp->_s._keys[msg.b[1]&0x7f][rchan] = 1;
                }
            } // end if min chan
            else
            {
               // if(sp->_s._mapmode == MIDICONF_NotesAreAnalogX)
                {
                    // per note:
                    UBYTE inote = msg.b[1]&0x7f;
                    if(inote>sp->_s._max_note) sp->_s._max_note=inote;
                    if(inote<sp->_s._min_note) sp->_s._min_note=inote;
                    int l = (int)sp->_s._max_note - (int)sp->_s._min_note;
                    //printf("l:%d\n",(int)l);
                    if( l == 0)
                    {
                        sp->_s._analog[0] = 0 ; //center
                    } else
                    {
                       // int = ((((int)inote - (int)sp->_s._min_note)<<(9+8))/l)-(128<<9);

                         sp->_s._analog[0] = ((((LONG)inote-sp->_s._min_note)<<(9+8))/l)-(128<<9);
                       //  printf("nlg notes:%d\n", sp->_s._analog[0]);
                    }

                    sp->_s._analog[1] =
                    ( (msg.b[2]&0x7f)<<(9+1))-(128<<9) ; // volume...

                }

            } // end if chan not used for buttons
        } // end if noteon

        else if(isNoteOff)
        {
            if(channel>=minimalChanAsButton)
            {
                UBYTE rchan= sp->_s._chanMap[channel];
                if(rchan != 255)
                {
                    sp->_s._keys[msg.b[1]&0x7f][rchan] = 0;
                }
            } // end if min chan
        } // end if noteoff


        if(highmess == MS_Ctrl)
        {
            // those "slider controls" are alway 0->127
            UBYTE inlg=255;
            if(msg.b[1]==2) // volume knob
            {
                inlg = 2;
            }
            if(msg.b[1]>=0x0e && msg.b[1]<=0x15) //8
            {
                inlg = 3+msg.b[1]-0x0e;
            }
            if(inlg != 255 && inlg>=2 && inlg<AI_MIDI_NBANALOG)
            {
           // printf("inlg:%d\n",inlg);
            // ( (msg.b[2]&0x7f)<<(9+1))-(128<<9) ;
                sp->_s._analog[inlg]=( (msg.b[2]&0x7f)<<(9+1))-(128<<9) ;
                // (128-msg.b[2])<<9;
            }
            // vol b0 02 7f  volume knob
             // f1  b0 0e (0->7f)  00
             // f2  b0 0f (0->7f)  00
             // f3  b0 10 (0->7f)  00
             // f8  b0 15 (0->7f)  00
        }



    } // end while
}


void MidiControls_close(struct sMidiController* p)
{  
    struct sMidiControllerPrivate *sp = (struct sMidiControllerPrivate *)p;
    if (!sp) return;

    if(sp->_midi)
    {
       // printf("Go DeleteMidi()\n");
        DeleteMidi(sp->_midi);
    }
    FreeVec(sp);
}

/*


clavier rouge:

 note plus basse: 48
 plus haute:84   LOW:1  -> son chan est 1

 buttons spé:
 36 37 38 39   low:9
 40 41 42 43

 MOLETTE (VOLUME) ?
 B0 07 (VOL0->7F) 00

 f1  b0 0e (0->7f)  00
 f2  b0 0f (0->7f)  00
 f3  b0 10 (0->7f)  00
 f8  b0 15 (0->7f)  00

 molette modulation roue:
  e0 00  (val 0->$40->$7f) 00

  touchpad:
  b0 01   (0->7f) 00
 ---------------------------------- clavier noir:
 le noteoff est: on, + velocity à 0
 note la plus basse: 36
 plus haute:96
notes:
 92 -> low: 2 -> son chan est 2

 b2 comme b0 sur l'autre
 b2 07 (0->7f) 00

 roue molette pitch: e2 00 (val 0->$40->$7f) 00
 roue molette modulation: b2 01 (val 0->$40->$7f) 00
 ----------------------
 sonuus G2M + theremin
  e0 (0->78) (0,3->7f)
  ausi
  b0
  ça fait:

  90 noteon
  b0 02 7f  <- après tout les noteon ya: b0 02 7f
  e0
  e0
  80 noteoff
  b0 02 7f  <- après tout les noteon ya: b0 00
 après tout les note
----------------------
Control Change (0xB0) controller number, controller value.
Pitch Bend (0xE0)    7 least-significant bits of the value,MM  7 most-significant bits of the value (14b)
        center is  (0x2000).
*/
