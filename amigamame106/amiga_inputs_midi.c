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
		MIDI_Name, "MAMEIN",
		MIDI_RecvSignal, SIGBREAKB_CTRL_E,
		MIDI_MsgQueue,   100,
		MIDI_ErrFilter, CMEF_All,
		TAG_DONE};
        printf("Go CreateMidiA()\n");
    p->_midi = CreateMidiA((struct TagItem *)&tags[0]);

    printf("p->_midi: %08x\n",(int)p->_midi);

    ULONG tagsl[]={
		MLINK_Name, "MAMELNK",
		MLINK_Location,	inlinkname,
		MLINK_EventMask, /*CMF_Note*/ CMF_All,
		MLINK_Comment,	"MAME[Input]",
		TAG_DONE};

        printf("Go AddMidiLinkA()\n");

    p->_link = AddMidiLinkA(p->_midi, MLTYPE_Receiver,(struct TagItem *)&tagsl[0]);
    printf("p->_link: %08x\n",(int)p->_link);


    return (struct sMidiController*)p;
}
void MidiControls_update(struct sMidiController* p)
{
    struct sMidiControllerPrivate *sp = (struct sMidiControllerPrivate *)p;
    if (!sp) return;

    MidiMsg		msg;
/*
typedef union
{
    ULONG l[2];
    UBYTE b[4];
} MidiMsg;
*/
    while (GetMidi(sp->_midi,&msg))
    {
        if (noteon(&msg))
// #if BYNOTE
        {
       // msg.mm_Data1
        // int		note = ((msg.mm_Data1-36)/2) % 31;
        //     int		velocity = msg.mm_Data2 * 39 / 127;

        //     velocity = velocity * VELSCALE / 100;
        //     if (velocity > 39) velocity = 39;

        //     if (vu_levels[note] < velocity) vu_levels[note] = velocity;
        }
// #else
        // {	int		channel = msg.mm_Status & 0x0f;
        //     int		velocity = msg.mm_Data2 * 39 / 127;

        //     velocity = velocity * VELSCALE / 100;
        //     if (velocity > 39) velocity = 39;

        //     if (vu_levels[channel] < velocity) vu_levels[channel] = velocity;
        // }
//#endif
    } // end while
}


void MidiControls_close(struct sMidiController* p)
{  
    struct sMidiControllerPrivate *sp = (struct sMidiControllerPrivate *)p;
    if (!sp) return;

    if(sp->_midi)
    {
        printf("Go DeleteMidi()\n");
        DeleteMidi(sp->_midi);
    }
    FreeVec(sp);
}






// #include <intuition/intuition.h>

// #include <exec/interrupts.h>
// #include <hardware/intbits.h>
// #include <midi/camd.h>
// #include <midi/mididefs.h>
// #include <clib/exec_protos.h>
// #include <clib/camd_protos.h>
// #include <clib/intuition_protos.h>
// #include <clib/graphics_protos.h>

// #include <pragmas/camd_pragmas.h>

// #include <dos/dos.h>

// #include <stdio.h>

// #define BYNOTE			1
// #define VELSCALE		150

// #define	LEAVE			{ printf("exit\n"); goto exitit; }
// #define	unless(x)		if ( !(x) )

// struct Library			*IntuitionBase,
// 						*GfxBase,
// 						*CamdBase;
// extern struct Library	*SysBase;

// struct Window			*Window;
// struct Screen			*Screen;
// struct ViewPort			*vp;
// struct RastPort			*rp;

// USHORT colors[32] = {
// 	0x000,0xAAA,0x888,0x555,0x333,0xBB0,0x6B0,0x0F0,
// 	0xF99,0xF89,0xE8A,0xE8A,0xE7A,0xE7B,0xD6B,0xD6C,
// 	0xD5D,0xC5D,0xA5C,0x94C,0x84C,0x74B,0x53B,0x43B,
// 	0x333,0x444,0x555,0x666,0x777,0x888,0xEEE,0xFFF };

// struct NewScreen NewScreen = { 0,200-47,320,47, 5, 0,1, NULL, CUSTOMSCREEN, };

// extern struct Image		im1,
// 						im2,
// 						im3;

// struct Gadget off_gadget = {
// 	NULL,	2,2,13,21,GFLG_GADGHIMAGE|GFLG_GADGIMAGE,GACT_RELVERIFY,GTYP_BOOLGADGET,(APTR)&im1,(APTR)&im2
// };

// struct NewWindow back_ground =
// {	0,0,320,47, 0,1,
// 	VANILLAKEY | MOUSEBUTTONS | GADGETUP,
// 	NOCAREREFRESH | ACTIVATE | BORDERLESS | RMBTRAP,
// 	&off_gadget,NULL,NULL,NULL,NULL,320,200,320,200,CUSTOMSCREEN,
// };

// #if BYNOTE
// #define VU_BARCOUNT		32
// #define BARS_PER_COLOR	2
// #else
// #define VU_BARCOUNT		16
// #define BARS_PER_COLOR	1
// #endif

// WORD					vu_levels[VU_BARCOUNT],
// 						prev_levels[VU_BARCOUNT];

// struct Task				*myTask;

// void draw_vu_levels(void);

// int __interrupt __saveds VBlankInterface( void );

// struct Interrupt VertBlank =
// {	NULL,NULL,NT_INTERRUPT,-60,"VU VBlank",		/* node, pri = -60 */
// 	NULL,										/* data ptr, same as inputevent */
// 	(void *)VBlankInterface						/* code ptr */
// };

// WORD					vblank_ok=NULL;			/* TRUE if vblank int installed	*/

// struct MidiNode			*midi;
// struct MidiLink			*link;

// struct MidiNode *CreateMidi(Tag tag, ...)
// {	return CreateMidiA((struct TagItem *)&tag );
// }

// BOOL SetMidiAttrs(struct MidiNode *mi, Tag tag, ...)
// {	return SetMidiAttrsA(mi, (struct TagItem *)&tag );
// }

// struct MidiLink *AddMidiLink(struct MidiNode *mi, LONG type, Tag tag, ...)
// {	return AddMidiLinkA(mi, type, (struct TagItem *)&tag );
// }

// BOOL SetMidiLinkAttrs(struct MidiLink *mi, Tag tag, ...)
// {	return SetMidiLinkAttrsA(mi, (struct TagItem *)&tag );
// }

// void main(int argc,char *argv[])
// {	char	*inlinkname = "in.0";

// 	if (argc > 1) inlinkname = argv[1];

// 	myTask = FindTask(0);

// 	unless (IntuitionBase = OpenLibrary("intuition.library",0))

// 	{ printf("exit intui\n"); goto exitit; }

//     unless (GfxBase = OpenLibrary("graphics.library",0))
//  { printf("exit gfx\n"); goto exitit; }

// 	unless (CamdBase = OpenLibrary("camd.library",0))
//     { printf("exit camd\n"); goto exitit; }

// 	if ((Screen = OpenScreen(&NewScreen)) == NULL)
//     { printf("exit newscreen\n"); goto exitit; }

// 	vp = &(Screen->ViewPort);
// 	LoadRGB4(vp,colors,32);

// 	back_ground.Screen = Screen;
// 	if ((Window = OpenWindow(&back_ground)) == NULL) LEAVE;
// 	rp = Window->RPort;

// 	DrawImage(rp,&im3,309,0);
// 	SetAPen(rp, 1); Move(rp,19, 0); Draw(rp,308, 0);
// 	SetAPen(rp,24); Move(rp,19, 1); Draw(rp,308, 1);
// 	SetAPen(rp, 3); Move(rp,19, 2); Draw(rp,308, 2);

// 	SetAPen(rp, 1); Move(rp,19,44); Draw(rp,308,44);
// 	SetAPen(rp,24); Move(rp,19,45); Draw(rp,308,45);
// 	SetAPen(rp, 3); Move(rp,19,46); Draw(rp,308,46);

// 	unless (midi = CreateMidi(
// 		MIDI_Name, "VU Meters",
// 		MIDI_RecvSignal, SIGBREAKB_CTRL_E,
// 		MIDI_MsgQueue,   100,
// 		MIDI_ErrFilter, CMEF_All,
// 		TAG_DONE))
// 			LEAVE;

// 	unless (link = AddMidiLink(midi, MLTYPE_Receiver,
// 		MLINK_Name, "VU Meter Link",
// 		MLINK_Location,	inlinkname,
// 		MLINK_EventMask, CMF_Note,
// 		MLINK_Comment,	"VU Meters [Input]",
// 		TAG_DONE))
// 			LEAVE;

// 	AddIntServer(INTB_VERTB, &VertBlank);
// 	vblank_ok = TRUE;

// 	while (TRUE)
// 	{	struct IntuiMessage *message;
// 		LONG			signals;

// 		signals = Wait((1 << Window->UserPort->mp_SigBit) | SIGBREAKF_CTRL_F | SIGBREAKF_CTRL_E);

// 		if (signals & SIGBREAKF_CTRL_F)
// 		{	int i;

// 			for (i=0; i<VU_BARCOUNT; i++)
// 			{	if (vu_levels[i] > 0)
// 					vu_levels[i]--;
// 			}
// 			draw_vu_levels();
// 		}

// 		if (signals & SIGBREAKF_CTRL_E)
// 		{
// 			MidiMsg		msg;

// 			while (GetMidi(midi,&msg))
// 			{
// 				if (noteon(&msg))
// #if BYNOTE
// 				{	int		note = ((msg.mm_Data1-36)/2) % 31;
// 					int		velocity = msg.mm_Data2 * 39 / 127;

// 					velocity = velocity * VELSCALE / 100;
// 					if (velocity > 39) velocity = 39;

// 					if (vu_levels[note] < velocity) vu_levels[note] = velocity;
// 				}
// #else
// 				{	int		channel = msg.mm_Status & 0x0f;
// 					int		velocity = msg.mm_Data2 * 39 / 127;

// 					velocity = velocity * VELSCALE / 100;
// 					if (velocity > 39) velocity = 39;

// 					if (vu_levels[channel] < velocity) vu_levels[channel] = velocity;
// 				}
// #endif
// 			}
// 		}

// 		while (message = (struct IntuiMessage *)GetMsg(Window->UserPort))
// 		{	ULONG		class = message->Class;

// 			ReplyMsg(&message->ExecMessage);
// 			if (class == GADGETUP) LEAVE;
// 		}
// 	}
// exitit:
// 	if (vblank_ok) RemIntServer(INTB_VERTB, &VertBlank);
// 	if (midi) DeleteMidi(midi);
// 	if (Window){ ClearMenuStrip(Window); CloseWindow(Window); }
// 	if (Screen) CloseScreen(Screen);
// 	if (CamdBase) CloseLibrary(CamdBase);
// 	if (GfxBase) CloseLibrary(GfxBase);
// 	if (IntuitionBase) CloseLibrary(IntuitionBase);
// }

// void draw_vu_levels(void)
// {	int					i;

// 	int					ymin, ymax, y1, y2;
// 	int					xmin, xmax;

// 	for (i=0; i<VU_BARCOUNT; i++)
// 	{
// 		if (vu_levels[i] > prev_levels[i])
// 		{
// 			SetAPen(rp,i/BARS_PER_COLOR+8);

// 			ymin = prev_levels[i];
// 			ymax = vu_levels[i];
// 		}
// 		else
// 		{	SetAPen(rp,0);
// 			ymax = prev_levels[i];
// 			ymin = vu_levels[i];
// 		}
// #if BYNOTE
// 		xmin = 21 + i * 9;
// 		xmax = xmin + 7;
// #else
// 		xmin = 21 + i * 18;
// 		xmax = xmin + 15;
// #endif

// 		y1 = 43 - ymax;
// 		y2 = 43 - ymin - 1;

// 		if (y1 <= y2) RectFill(rp,xmin,y1,xmax,y2);

// 		prev_levels[i] = vu_levels[i];
// 	}
// }

// int __interrupt __saveds VBlankInterface( void )
// {	Signal(myTask,SIGBREAKF_CTRL_F);
// 	return 0;									/* server chain continues		*/
// }
