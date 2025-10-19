#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/lowlevel.h>
#include <proto/keymap.h>
#include <proto/alib.h>
#include "intuiuncollide.h"


#include "amiga_config.h"
#include "amiga_video.h"

#include <string>

extern "C" {
    // from MAME
    #include "osdepend.h"
    #include "input.h"
    #include "inptport.h"
    #include "usrintrf.h"
    #include "mamelog.h"
    #include "mame.h"
}
#include "amiga_inputs_kbd_ll.h"
#include "amiga_inputs_interface.h"

extern "C" {
    struct Library *LowLevelBase = NULL;
	extern UINT32 _throttleIsOn; // with shift+f10 key
}

using namespace std;

struct mapkeymap {
    std::string _name;
    unsigned char _rawkeycode;
};
/*moved up to interface because used by mouse & parallel port and anything that sends regular buttons...
class kbdInput {
public:
    int _useAnyMouse=0;
    int         _NbKeysUpStack=0;
    BYTE         _Keys[256*4]={0}; // bools state for actual keyboard rawkeys + lowlevel pads code
    UWORD        _NextKeysUpStack[256]={0}; // delay event between frame to not loose keys.

    vector<mapkeymap> _km; // keep the instance of strings from keymap libs
    string extrakey_name[3];
};

// also here. also in inputs instance list.
static kbdInput *g_pInputs=NULL;

static void mapRawKeyToString(UWORD rawkeycode, std::string &s)
{
    char temp[8];
    struct InputEvent ie={0};
    ie.ie_Class = IECLASS_RAWKEY;
    ie.ie_Code = rawkeycode;

    WORD actual = MapRawKey(&ie, temp, 7, 0);
    temp[actual]=0; //
    if(actual>0)
    {
        // mame wants maj at this level, if not it displays empty
        int i=0;
        while(temp[i]!=0) {
            if(temp[i]>='a' && temp[i]<='z') temp[i]-=32;
        i++;
        }
        s = temp;

    }
}
static inline unsigned int nameToMameKeyEnum(std::string &s)
{
    if(s.length()==1)
    {
        char c = s[0];
        if(c>='a' && c<='z')
        {
            return KEYCODE_A + (unsigned int)(c-'a');
        }
        if(c>='A' && c<='Z')
        {
            return KEYCODE_A + (unsigned int)(c-'A');
        }
        if(c==',') return KEYCODE_COMMA;
        if(c==':') return KEYCODE_COLON;
        if(c=='/') return KEYCODE_SLASH;
        if(c=='\\') return KEYCODE_BACKSLASH;
        if(c=='*') return KEYCODE_ASTERISK; // there is another one on pad (?)
        if(c=='=') return KEYCODE_EQUALS;
        if(c=='-') return KEYCODE_MINUS;
        if(c==145) return KEYCODE_QUOTE;
        if(c==146) return KEYCODE_QUOTE;
    }

    return CODE_OTHER_DIGITAL;
}
// strings kept static, more easy and safe to refer.
static const char * const padsbtnames[4][11]={
    {
        "Pad1 Blue",
        "Pad1 Red",
        "Pad1 Yellow",
        "Pad1 Green",
        "Pad1 Forward",
        "Pad1 Reverse",
        "Pad1 Play",
        "Pad1 UP",
        "Pad1 DOWN",
        "Pad1 LEFT",
        "Pad1 RIGHT"
    },
    {
        "Pad2 Blue",
        "Pad2 Red",
        "Pad2 Yellow",
        "Pad2 Green",
        "Pad2 Forward",
        "Pad2 Reverse",
        "Pad2 Play",
        "Pad2 UP",
        "Pad2 DOWN",
        "Pad2 LEFT",
        "Pad2 RIGHT"
    },
    {
        "Pad3 Blue",
        "Pad3 Red",
        "Pad3 Yellow",
        "Pad3 Green",
        "Pad3 Forward",
        "Pad3 Reverse",
        "Pad3 Play",
        "Pad3 UP",
        "Pad3 DOWN",
        "Pad3 LEFT",
        "Pad3 RIGHT"
    },
    {
        "Pad4 Blue",
        "Pad4 Red",
        "Pad4 Yellow",
        "Pad4 Green",
        "Pad4 Forward",
        "Pad4 Reverse",
        "Pad4 Play",
        "Pad4 UP",
        "Pad4 DOWN",
        "Pad4 LEFT",
        "Pad4 RIGHT"
    },
};

static void *kbd_Create(void *registerer,fAddOsCode addOsCode)
{
    kbdInput *p  =new kbdInput();
    if(!p) return NULL;
    g_pInputs = p;

    // here are all the easy constant ones that match all keyboards:
    vector<os_code_info> kbi = {
        {"KEY UP",0x4C,KEYCODE_UP},
        {"KEY DOWN",0x4D,KEYCODE_DOWN},
        {"KEY LEFT",0x4F,KEYCODE_LEFT},
        {"KEY RIGHT",0x4E,KEYCODE_RIGHT},

        {"SPACE",0x40,KEYCODE_SPACE},

        {"L SHIFT",0x60,KEYCODE_LSHIFT},
        {"R SHIFT",0x61,KEYCODE_RSHIFT},
        {"L ALT",0x64,KEYCODE_LALT},
        {"R ALT",0x65,KEYCODE_RALT},
        {"L AMIGA",0x66,KEYCODE_LWIN},
        {"R AMIGA",0x67,KEYCODE_RWIN},

         {">",0x30,AMIGA_GREATERTHAN},

        {"ENTER",0x44,KEYCODE_ENTER},

        {"TAB",0x42,KEYCODE_TAB},
        {"ESC",0x45,KEYCODE_ESC},
        {"F1",0x50,KEYCODE_F1},
        {"F2",0x51,KEYCODE_F2},
        {"F3",0x52,KEYCODE_F3},
        {"F4",0x53,KEYCODE_F4},
        {"F5",0x54,KEYCODE_F5},
        {"F6",0x55,KEYCODE_F6},
        {"F7",0x56,KEYCODE_F7},
        {"F8",0x57,KEYCODE_F8},
        {"F9",0x58,KEYCODE_F9},
     // used by us to switch screen   {"F10",0x59,KEYCODE_F10},
        // on pc F11 is used for performance display, let's use that
        {"HELP",0x5F,/*KEYCODE_HOME*/KEYCODE_F11 }, // ... dunno.

        {"~",0x00,KEYCODE_TILDE},
        {"1",0x01,KEYCODE_1},
        {"2",0x02,KEYCODE_2},
        {"3",0x03,KEYCODE_3},
        {"4",0x04,KEYCODE_4},
        {"5",0x05,KEYCODE_5},
        {"6",0x06,KEYCODE_6},
        {"7",0x07,KEYCODE_7},
        {"8",0x08,KEYCODE_8},
        {"9",0x09,KEYCODE_9},
        {"0",0x0A,KEYCODE_0},

        {"BACKSPACE",0x41,KEYCODE_BACKSPACE},
        {"DEL",0x46,KEYCODE_DEL},


        {"CTRL",0x63,KEYCODE_LCONTROL},

        // whole amiga pad
        {"[ PAD", 0x5A, KEYCODE_OPENBRACE },
        {"] PAD", 0x5B, KEYCODE_CLOSEBRACE },
        {"/ PAD",0x5C,KEYCODE_SLASH_PAD},
        {"* PAD",0x5D,KEYCODE_ASTERISK},

        {"0PAD",0x0F,KEYCODE_0_PAD},
        {"1PAD",0x1D,KEYCODE_1_PAD},
        {"2PAD DOWN",0x1E,KEYCODE_2_PAD},
        {"3PAD",0x1F,KEYCODE_3_PAD},
        {"4PAD LEFT",0x2D,KEYCODE_4_PAD},
        {"5PAD",0x2E,KEYCODE_5_PAD},
        {"6PAD RIGHT",0x2F,KEYCODE_6_PAD},
        {"7PAD",0x3D,KEYCODE_7_PAD},
        {"8PAD UP",0x3E,KEYCODE_8_PAD},
        {"9PAD",0x3F,KEYCODE_9_PAD},
        {"- PAD",0x4A,KEYCODE_MINUS_PAD},
        {"+ PAD",0x5E,KEYCODE_PLUS_PAD},
        // mame codes is missing keypad '.'
        {". PAD",0x3C,CODE_OTHER_DIGITAL},
        {"ENTER PAD",0x43,KEYCODE_ENTER_PAD},
    };
    addOsCode(registerer,kbi.data(),kbi.size());

    // then add rawkeys which meanings changes by locale settings
    {
        vector<unsigned char> keystodo={0x0b,0x0c,0x0d}; // first of each lines
        {   unsigned char ic=0;
            while(ic<12) {keystodo.push_back(0x10+ic); ic++; }
            ic=0;
            while(ic<12) {keystodo.push_back(0x20+ic); ic++; }
            ic=0;
            while(ic<11) {keystodo.push_back(0x30+ic); ic++; }
        }
        const int nbk = (int)keystodo.size();
        p->_km.reserve(nbk);
        p->_km.resize(nbk);
        for(int i=0;i<nbk;i++)
        {
            mapkeymap &mkm =p->_km[i];
            mkm._rawkeycode = keystodo[i];
            // ask amiga OS about the meaning of that key on this configuration.
            mapRawKeyToString((UWORD)keystodo[i],mkm._name);
          //  printf("keystodo:%d mapped to:%s:\n",keystodo[i],mkm._name.c_str());
            // then look if it correspond to something in mame enums...
            if( keystodo[i]==26) {
                mkm._name = "?";  // MapRawKey doesnt get this one well.
            }
            if(mkm._name.length()>0)
            {
                unsigned int mamekc = nameToMameKeyEnum(mkm._name);
                os_code_info osci={mkm._name.c_str(),keystodo[i],(input_code)mamekc};
                addOsCode(registerer,&osci,1);
            } else {
            // printf("code with no name:%d\n",keystodo[i]);
            }
        }
        // then add keys which need a rawkey mapped to a mame constant, with varying name
        // mame enum was modified for this.
        {
            mapRawKeyToString((UWORD)0x003A,p->extrakey_name[0]);
            os_code_info osci={p->extrakey_name[0].c_str(),0x3A,AMIGA_SPECIAL_RAWKEY_3A};
            addOsCode(registerer,&osci,1);
        }
        {
            mapRawKeyToString((UWORD)0x002A,p->extrakey_name[1]);
            os_code_info osci={p->extrakey_name[1].c_str(),0x2A,AMIGA_SPECIAL_RAWKEY_2A};
            addOsCode(registerer,&osci,1);
        }
        {
            mapRawKeyToString((UWORD)0x002B,p->extrakey_name[2]);
            os_code_info osci={p->extrakey_name[2].c_str(),0x2B,AMIGA_SPECIAL_RAWKEY_2B};
            addOsCode(registerer,&osci,1);
        }

    }

    // - - - - Lowlevel controllers will send buttons as keyboard, so init here:
    MameConfig::Controls &configControls = getMainConfig().controls();
    MameConfig::Misc &configMisc = getMainConfig().misc();
    // 1 - - - - stats what's needed.
    //r1.6 only open lowlevel if some lowlevel type asked, and open/close each time.
    p->_useAnyMouse=0;
    UBYTE useAnyLowLevelControl = 0;

    for(int iLLPort=0;iLLPort<4;iLLPort++) // 2 hardware DB9 port, +the elusive mysterious 3&4 lowlevel ports.
    {
        int iPlayer = configControls._llPort_Player[iLLPort] ;
        if( iPlayer == 0) continue;

        int lowlevelState = configControls._llPort_Type[iLLPort];

        // lowlevel only, code 0 (autosense) is now used as "no controller"
        if(lowlevelState>0 && lowlevelState<=3) // shouldnt -> now we have values >3 for propjoy
        {
            useAnyLowLevelControl++;
            if(lowlevelState == SJA_TYPE_MOUSE) p->_useAnyMouse = 1;
        }
    } // loop by ll port

    // 2 - - - - - open lowlevel.library if needed so far
    if(useAnyLowLevelControl>0)
    {
        LowLevelBase = OpenLibrary("lowlevel.library", 0);
        if(!LowLevelBase)
        {
            loginfo2(1,"can't open lowlevel.library needed by some controller.");
        }
        // r1.6: let's accept the case where LL not there when null.
    }

    // 3 - - - -configure lowlevel if present, so pads are received as keys.
    if(LowLevelBase)
    {
        for(int iLLPort=0;iLLPort<4;iLLPort++) // 2 hardware DB9 port, +the elusive mysterious 3&4 lowlevel ports.
        {
            int iPlayer = configControls._llPort_Player[iLLPort] ;
            if( iPlayer == 0) continue;
            iPlayer--;

            int lowlevelState = configControls._llPort_Type[iLLPort];
            if(lowlevelState<=0 || lowlevelState>3) continue;

            SystemControl( SCON_AddCreateKeys,iLLPort, TAG_END,0);

            // configure port as mouse,jostick or CD32 pads...
            SetJoyPortAttrs(iLLPort,SJA_Type,lowlevelState,TAG_DONE);

            int ipshft = iLLPort<<8;
            const int mamecodeshift =
                ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT) *iPlayer ;
            // here we treat only CD32Pads and 1/2Bt joysticks...
            // do not declare pads if it is mouse !!!
            if(lowlevelState == SJA_TYPE_GAMECTLR)
            {
                vector<os_code_info> kbi2={
                    {padsbtnames[iLLPort][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
                    {padsbtnames[iLLPort][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
                    {padsbtnames[iLLPort][3-1],RAWKEY_PORT0_BUTTON_YELLOW+ipshft,JOYCODE_1_BUTTON3+mamecodeshift},
                    {padsbtnames[iLLPort][4-1],RAWKEY_PORT0_BUTTON_GREEN+ipshft,JOYCODE_1_BUTTON4+mamecodeshift},
                    {padsbtnames[iLLPort][5-1],RAWKEY_PORT0_BUTTON_FORWARD+ipshft,JOYCODE_1_BUTTON6+mamecodeshift},
                    {padsbtnames[iLLPort][6-1],RAWKEY_PORT0_BUTTON_REVERSE+ipshft,JOYCODE_1_BUTTON5+mamecodeshift},
                    {padsbtnames[iLLPort][6],RAWKEY_PORT0_BUTTON_PLAY+ipshft,JOYCODE_1_START+mamecodeshift},
                    {padsbtnames[iLLPort][7],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
                    {padsbtnames[iLLPort][8],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
                    {padsbtnames[iLLPort][9],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
                    {padsbtnames[iLLPort][10],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
                };
                addOsCode(registerer,kbi2.data(),kbi2.size());
            } else
            if(lowlevelState == SJA_TYPE_JOYSTK )
            {
                vector<os_code_info> kbi2={
                    {padsbtnames[iLLPort][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
                    {padsbtnames[iLLPort][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
                    {padsbtnames[iLLPort][7],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
                    {padsbtnames[iLLPort][8],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
                    {padsbtnames[iLLPort][9],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
                    {padsbtnames[iLLPort][10],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
                };
                addOsCode(registerer,kbi2.data(),kbi2.size());
            }


        } // loop by ll port

    } // end of LL configuration

    return p;
}

// called from video
void Inputs_Keyboard_ll_Update(struct MsgPort *pMsgPort)
{
    struct IntuiMessage *im;
    struct MenuItem   *mitem;
    int doSwitchFS=0;
    static UBYTE fcounter =1;
    fcounter++;
    if(fcounter==255) fcounter=1; // we just need it to be different from previous frame.

    if(!pMsgPort || !g_pInputs) return;

    // treat keys up next frame so no keys are missed.
    for(int i=0;i<g_pInputs->_NbKeysUpStack;i++)
    {
        g_pInputs->_Keys[g_pInputs->_NextKeysUpStack[i]] =0;
    }
    g_pInputs->_NbKeysUpStack = 0;

    // - - - -
    while((im = (struct IntuiMessage *) GetMsg(pMsgPort)))
    {
        ULONG imclass = im->Class;
        UWORD imcode  = im->Code;
        UWORD imqual  = im->Qualifier;

        ReplyMsg((struct Message *) im); // the faster the better.

        switch(imclass)
        {
            case IDCMP_RAWKEY:
            if(!(imqual & IEQUALIFIER_REPEAT) )
            {
                // same as amiga rawkey for keyboard, then joypads are remaped.
                // pack that to fit one byte.
                #define IKEY_RAWMASK_CD32PADS 0x037f // rawmask has evolved with CD32 pads
                UWORD finalkeycode = imcode & IKEY_RAWMASK_CD32PADS ; //IKEY_RAWMASK;

               // printf("key:%04x\n",finalkeycode);

                if(imcode & IECODE_UP_PREFIX)
                {                   // if many down/up happens in one frame, we must see it has pressed, then up next frame.
                   //printf("Up:%04x\n",finalkeycode);

                    // if many down/up happens in one frame, we must see it has pressed, then up next frame.
                   if(g_pInputs->_NbKeysUpStack<256)
                   {
                        g_pInputs->_NextKeysUpStack[g_pInputs->_NbKeysUpStack] = finalkeycode;
                        g_pInputs->_NbKeysUpStack++;
                   } else {
                        // shouldnt happen, but does maintain consistency.
                        g_pInputs->_Keys[finalkeycode] = 0; // down
                   }
                   if(finalkeycode == 0x59) _throttleIsOn=0;
                }
                else
                {
                   //printf("Down:%04x\n",finalkeycode);
                    UBYTE prev = g_pInputs->_Keys[finalkeycode];
                    if(prev != 0 && prev == fcounter )
                    {   // means down->up->down for same key in the same frame,
                        // which is common is just 8fps and player is blasting a key...
                        // in that case remove previous delayed down we just put, because
                        // next up could happen next frame.
                        for(int i=0;i<g_pInputs->_NbKeysUpStack;i++) // just a few there
                        {
                            if(g_pInputs->_NextKeysUpStack[i]==finalkeycode)
                                g_pInputs->_NextKeysUpStack[i]= 0x0080; // a rawkey that can't exist and will not be watched.
                        }

                    }
                    g_pInputs->_Keys[finalkeycode] = fcounter;
                    //printf("key:%04x on\n",(int)finalkeycode);
                    // F10 -> switch fullscreen, but next, it will destroy the current port wer'e using.
                    if(finalkeycode == 0x59) { // F10
                        if(imqual &(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
                        {
                            // r1.7, shift F10, throttle mode when pressed
                            _throttleIsOn = 1;
                        } else
                        {
                            doSwitchFS =1;
                        }
                    }
                }
            }
            break;
        case IDCMP_CLOSEWINDOW:
            mame_schedule_exit();
        break;
        case IDCMP_MOUSEBUTTONS:
            break;
        case IDCMP_CHANGEWINDOW:
            // after system bloqued everything for an amount of time (moving windows,...),

            // need to not upset a timer when window moves block app.
            extern void ResetWatchTimer();
            ResetWatchTimer();
            break;
        case IDCMP_NEWSIZE:

            break;
         default:
            // class 8: IDCMP_MOUSEBUTTONS
          //      printf("getmsg() unmanaged: class:%d\n",(int)imclass);
            break;
        }
    }

    if(doSwitchFS) SwitchWindowFullscreen();
}

// used by real time to check keys.
static int kbd_GetCode(void *o, ULONG oscode)
{
    return ((kbdInput *)o)->_Keys[oscode];
}

void kbd_Close(void *o)
{
    kbdInput *p = (kbdInput *)o;

    if(LowLevelBase)
    {
        for(int i=0;i<2;i++)
        {
            SystemControl(
                // stops rawkey codes for the joystick/game
                SCON_RemCreateKeys,i,
                TAG_END,0
                );
        }
        CloseLibrary(LowLevelBase);
    }
    LowLevelBase = NULL;
    delete p;
    g_pInputs = NULL;
}


struct sMameInputsInterface g_ipt_Keyboard=
{
    kbd_Create,
    NULL, // FrameUpdate, done explicitly to manage MsgPort.
    kbd_GetCode,
    kbd_Close,
    NULL //    PostInputPortInitCheck
};

// - - - - - - - -


static const char * _keepMouseNames[]=
{
    "Mouse1 X",
    "Mouse1 Y",
    "Mouse1 Bt1",
    "Mouse1 Bt2",
    "Mouse1 Bt3",
    "","","",

    "Mouse2 X",
    "Mouse2 Y",
    "Mouse2 Bt1",
    "Mouse2 Bt2",
    "Mouse2 Bt3",
    "","","",

    "Mouse3 X",
    "Mouse3 Y",
    "Mouse3 Bt1",
    "Mouse3 Bt2",
    "Mouse3 Bt3",
    "","","",

    "Mouse4 X",
    "Mouse4 Y",
    "Mouse4 Bt1",
    "Mouse4 Bt2",
    "Mouse4 Bt3",
    "","","",
};

struct InputsMouse
{

    // - - mouse states for 4 players, remapped from any ll ports.
    struct LLMouse {
        ULONG _mousestate; // high bytes are sadly clamped, but would work for 2+ mouses.
        //
        UBYTE _lastMouseStateX;
        UBYTE _lastMouseStateY;
        WORD _d;
    };
    LLMouse _mstate[4];
};

static void *mouse_Create(void *registerer,fAddOsCode addOsCode)
{
    if(!g_pInputs ||
        (g_pInputs && g_pInputs->_useAnyMouse == 0)) return NULL;

    struct InputsMouse  *p = (struct InputsMouse *) AllocVec(sizeof(struct InputsMouse), MEMF_CLEAR);
    if(!p) return NULL;

    MameConfig::Controls &configControls = getMainConfig().controls();
    MameConfig::Misc &configMisc = getMainConfig().misc();

    for(int iport=0;iport<4;iport++) // actually 2
    {
        int iplayer = configControls._llPort_Player[iport];
        if(iplayer == 0) continue;
        iplayer--;

        int itype = configControls._llPort_Type[iport];
      //  printf("iport:%d itype:%d\n",iport,itype);
        if(itype == SJA_TYPE_MOUSE) // if LL mouse
        {
            int mameAnlgSizePerPl = ((int)MOUSECODE_2_ANALOG_X-(int)MOUSECODE_1_ANALOG_X);
            int mameMouseBtSizePerPl = ((int)MOUSECODE_2_BUTTON1-(int)MOUSECODE_1_BUTTON1);
            {
                os_code_info osci={_keepMouseNames[(iport*8)+0],
                            (iport*8)+0,MOUSECODE_1_ANALOG_X+(iplayer*mameAnlgSizePerPl)};
                addOsCode(registerer,&osci,1);
            }
            {
                os_code_info osci={_keepMouseNames[(iport*8)+1],
                            (iport*8)+1,MOUSECODE_1_ANALOG_Y+(iplayer*mameAnlgSizePerPl)};
                addOsCode(registerer,&osci,1);
            }
            // mouse buttons
            {
                os_code_info osci={_keepMouseNames[(iport*8)+2],
                            (iport*8)+2,MOUSECODE_1_BUTTON1+(iplayer*mameMouseBtSizePerPl)};
                addOsCode(registerer,&osci,1);
            }
            {
                os_code_info osci={_keepMouseNames[(iport*8)+3],
                            (iport*8)+3,MOUSECODE_1_BUTTON2+(iplayer*mameMouseBtSizePerPl)};
                addOsCode(registerer,&osci,1);
            }
            {
                os_code_info osci={_keepMouseNames[(iport*8)+4],
                            (iport*8)+4,MOUSECODE_1_BUTTON3+(iplayer*mameMouseBtSizePerPl)};
                addOsCode(registerer,&osci,1);
            }
        } // end if LL mouse
    } // loop by player

    return p;
}
static void mouse_FrameUpdate(void *o)
{
    if(!LowLevelBase || !g_pInputs) return;
   MameConfig::Controls &configControls = getMainConfig().controls();

    for(int iLLPort=0;iLLPort<4;iLLPort++) // actually 2, ... but 4.
    {
        int iplayer = configControls._llPort_Player[iLLPort];
        if(iplayer ==0) continue;
        int itype = configControls._llPort_Type[iLLPort];
        //OLD, now manage more. if(itype != SJA_TYPE_MOUSE ) continue;

        ULONG state = ReadJoyPort( iLLPort);
        ULONG portType = state>>28;
        if(portType == SJA_TYPE_MOUSE)
        {
           // printf("g_pInputs->_mousestate %d %08x\n",iLLPort,(int)state);
           ((struct InputsMouse*)o)->_mstate[iLLPort]._mousestate = state;

           // got to redirect mouse buttons in that case, to shot in arkanoid for example.
           // int playershift = (iplayer-1)<<8;
           // // not perfect to do here because could jump a slow frame
           // g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_RED+playershift] = (int)((state & JPF_BUTTON_RED)!=0);
           // g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_BLUE+playershift] = (int)((state & JPF_BUTTON_BLUE)!=0);
           // g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_PLAY+playershift] = (int)((state & JPF_BUTTON_PLAY)!=0);
        }

    } // loop by port
}
// mandatory, used by real time.
static int mouse_GetCode(void *o, ULONG oscode)
{
    ULONG illport =  oscode>>3;
    UINT8 shortcode = ((UINT8)oscode) & 7;

    InputsMouse::LLMouse &llm = ((struct InputsMouse *)o)->_mstate[illport]; // mousestate[illport];
    UINT32 state = llm._mousestate;
    const INT32 minswitch = 128;

    switch(shortcode)
    {
    case 0: // mouse x
    {
        BYTE s = (BYTE)state ; // get 0->255 X in lowlevel meaning
        INT32 delta = ((INT32)s)-((INT32)llm._lastMouseStateX);
        llm._lastMouseStateX = s;
        if(delta<-minswitch) delta +=256;
        else if(delta>minswitch) delta -=256;
        return delta<<10;
    }
    case 1: // mouse y
    {
        BYTE s = (BYTE)(state>>8); // get 0->255 Y in lowlevel meaning
        INT32 delta = ((INT32)s)-((INT32)llm._lastMouseStateY);
        llm._lastMouseStateY = s;
        // delta should be some pixels ...
        if(delta<-minswitch) delta +=256;
        else if(delta>minswitch) delta -=256;
        return delta<<10;
    }
    case 2: // mouse bt 1
        return (state &JPF_BUTTON_RED)!=0 ;
    case 3: // mouse bt 2
        return (state &JPF_BUTTON_BLUE)!=0 ;
    case 4: // mouse bt 3
        return (state &JPF_BUTTON_PLAY)!=0 ;

    }
    return 0;
}

static void mouse_Close(void *o)
{
    if(o) FreeVec(o);
}



struct sMameInputsInterface g_ipt_LLMouses=
{
    mouse_Create,
    mouse_FrameUpdate,
    mouse_GetCode,
    mouse_Close,
    NULL //    PostInputPortInitCheck
};
