/**************************************************************************
 *
 * Copyright (C) 2024 Vic Ferry (http://github.com/krabobmkd)
 * forked from 1999 Mats Eirik Hansen (mats.hansen at triumph.no)
 *
 * $Id: amiga.c,v 1.1 1999/04/28 18:50:15 meh Exp meh $
 *
 * $Log: amiga.c,v $
 * Revision 1.1  1999/04/28 18:50:15  meh
 * Initial revision
 *
 *
 *************************************************************************/
// from amiga
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/lowlevel.h>
#include <proto/keymap.h>
#include <proto/alib.h>

#include "intuiuncollide.h"

extern "C" {
    #include <libraries/lowlevel.h>
    #include <devices/keyboard.h>
    #include <hardware/intbits.h>
}

#include <vector>
#include <sstream>
#include <string>
// from mame:
extern "C" {
    #include "osdepend.h"
    #include "input.h"
    // for schedule_exit()
    #include "mame.h"
}

#include "amiga106_inputs.h"
#include "amiga106_config.h"
#include "amiga106_video.h"
#include "amiga_parallelpads.h"


using namespace std;

#include <stdio.h>

#include <stdlib.h>

// extend amiga os code given to mame after "lowlevel rawkey codes".
#define ANALOG_CODESTART 1024

#ifdef USE_DIRECT_KEYBOARD_DEVICE
struct KeyboardMatrix {
    union {
        char _k[16];
        ULONG _kl[4];
    };
    bool operator==(const KeyboardMatrix &o)
    {
        for(int i=0;i<3;i++)
            if(_kl[i] != o._kl[i]) return false;
        return true;
    }
};
#endif

// we don't even need to publish it:
struct MameInputs
{
    struct MsgPort *_pMsgPort;
    int         _NbKeysUpStack;
    BYTE         _Keys[256*4]; // bools state for actual keyboard rawkeys + lowlevel pads code
    UWORD        _NextKeysUpStack[256]; // delay event between frame to not loose keys.
    // - - mouse states for 4 players, remapped from any ll ports.   
    struct LLMouse {
        ULONG _mousestate; // high bytes are sadly clamped, but would work for 2+ mouses.
        //
        UBYTE _lastMouseStateX;
        UBYTE _lastMouseStateY;
        WORD _d;
    };
    LLMouse _mstate[4];

#ifdef USE_DIRECT_KEYBOARD_DEVICE
    // - -  when using direct keyboard device, not intuition idcmp
    // must be allocated in MEMF_PUBLIC
    struct IOStdReq *_KeyIO;
    struct MsgPort  *_KeyMP;

    struct Interrupt *_keyboard_interupt;
    struct KeyboardMatrix *_keyboard_data;
    struct KeyboardMatrix _mainthreadkeys;
    int                 _doUseDirectKeyboard;
#endif

};



using namespace std;

struct mapkeymap {
    std::string _name;
    unsigned char _rawkeycode;
};
class RawKeyMap
{
    public:
    void clear() {
        _kbi.clear();
        _km.clear();
        _keymap_inited  =false;
    }
    vector<os_code_info> _kbi;
    vector<mapkeymap> _km; // keep the instance of strings from keymap libs
    bool _keymap_inited=false;

    void init();
};
static std::vector<std::string> _keepMouseNames=
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

RawKeyMap   rawkeymap;


MameInputs *g_pInputs=NULL;
ParallelPads *g_pParallelPads=NULL; // createParallelPads();
int     llPortsTypes[4]={0,0,0,0};

extern "C" {
    struct Library *LowLevelBase = NULL;
#ifdef USE_DIRECT_KEYBOARD_DEVICE
static void mamekbdInteruptfunc( register struct KeyboardMatrix  *pkm  __asm("a1") )
{
    struct KeyboardMatrix km;
    // hello this is executed at 50Hz or 60Hz, whatever happens . It's a vblank interupt.
    // and it must be all MEMF_PUBLIC.
    if(g_pInputs)
    {

    g_pInputs->_KeyIO->io_Command=KBD_READMATRIX;
    g_pInputs->_KeyIO->io_Data= (APTR)&km._kl[0] ;
    g_pInputs->_KeyIO->io_Length= 16 ;
    DoIO((struct IORequest *)g_pInputs->_KeyIO);

    pkm->_kl[0] |= km._kl[0]; // there is one bit for a key in this, rawkey code order.
    pkm->_kl[1] |= km._kl[1];
    pkm->_kl[2] |= km._kl[2];
    pkm->_kl[3] |= km._kl[3]; // not sure if useful.
    }

}
#endif

} // end extern c

void InitLowLevelLib()
{
    if(!LowLevelBase)
    {
        LowLevelBase = OpenLibrary("lowlevel.library", 0);
    }
}
// when effective;y asked, to further close.
static USHORT askedPadsRawKey = 0;
static USHORT useAnyMouse = 0;
#ifdef RJP_OPTION
static USHORT useReadJoyPortForPads = 0; // else rawkeys, added for NewLowlevel patch for A2000.
#endif
void ConfigureLowLevelLib()
{
//printf(" ***** ConfigureLowLevelLib\n");
    if(!LowLevelBase) return;

    /*
#define JP_TYPE_NOTAVAIL  (00<<28)	   port data unavailable
#define JP_TYPE_GAMECTLR  (01<<28)	   port has game controller
#define JP_TYPE_MOUSE	  (02<<28)	   port has mouse
#define JP_TYPE_JOYSTK	  (03<<28)	   port has joystick
#define JP_TYPE_UNKNOWN   (04<<28)	   port has unknown device
#define JP_TYPE_MASK	  (15<<28)	   controller type
*/
//#define SJA_TYPE_AUTOSENSE 0
//#define SJA_TYPE_GAMECTLR  1
//#define SJA_TYPE_MOUSE	   2
//#define SJA_TYPE_JOYSTK    3

//    printf("configure lowlevel\n");

    MameConfig::Controls &configControls = getMainConfig().controls();
    MameConfig::Misc &configMisc = getMainConfig().misc();
#ifdef RJP_OPTION
    useReadJoyPortForPads = ((configMisc._MiscFlags & MISCFLAG_USEREADJOYPORT) != 0);
#endif
    // for correct autosense,
    for(int itest=0;itest<2;itest++)
    {
        for(int iport=0;iport<4;iport++) ReadJoyPort(iport);
        WaitTOF();
        WaitTOF();
    }

    useAnyMouse=0;
    for(int iLLPort=0;iLLPort<4;iLLPort++) // actually 2
    {
        int iPlayer = configControls._llPort_Player[iLLPort] ;
        if( iPlayer == 0) continue;

        int lowlevelState = configControls._llPort_Type[iLLPort];

        if(lowlevelState<0 || lowlevelState>3) continue; // shouldnt


        if(lowlevelState != SJA_TYPE_AUTOSENSE)
        {
            if(lowlevelState == SJA_TYPE_MOUSE) useAnyMouse = 1;
            SetJoyPortAttrs(iLLPort,SJA_Type,lowlevelState,TAG_DONE);
        }
    } // loop by ll port

    bool useParallelExtension=false;
    // loop for parallel port
    for(int ipar=0 ; ipar<2 ;ipar++)
    {
        int iPlayer = configControls._parallelPort_Player[ipar] ;
        if( iPlayer == 0 ) continue;
        int type = configControls._parallel_type[ipar];
        if(type == 0 ) continue;

        useParallelExtension = true;
    }


    if(askedPadsRawKey==0
#ifdef RJP_OPTION
     && useReadJoyPortForPads==0
#endif
     )
    {
        SystemControl(
            SCON_AddCreateKeys,0,
            SCON_AddCreateKeys,1,
            TAG_END,0);
        askedPadsRawKey = 1;
    }
    // note: if needed
    //printf("input init: useParallelExtension:%d\n",(int)useParallelExtension);
    if(!g_pParallelPads && useParallelExtension)
    {
        g_pParallelPads = createParallelPads(); // could fail.
    }


}
void CloseLowLevelLib()
{
    if(LowLevelBase == NULL) return;

    if(askedPadsRawKey)
    {
        SystemControl(
            // stops rawkey codes for the joystick/game
            SCON_RemCreateKeys,0,
            SCON_RemCreateKeys,1,
            TAG_END,0
            );
        askedPadsRawKey = 0;
    }
    if(LowLevelBase) CloseLibrary(LowLevelBase);
    LowLevelBase = NULL;

}



// used before each game starts.
void AllocInputs()
{
    if(g_pInputs) return;
    g_pInputs = (MameInputs *)calloc(1,sizeof(MameInputs));
    if(!g_pInputs) return;

  // now done from get key list call

    rawkeymap.clear(); // will make the rawkey table again using conf.

    // - - - -init direct keyboard
#ifdef USE_DIRECT_KEYBOARD_DEVICE
    MameConfig::Controls &configControls = getMainConfig().controls();
    if(/*configControls._useDirectKeyboard*/true)
    {
        //

        struct Interrupt *pi;
        g_pInputs->_keyboard_interupt = pi = (struct Interrupt *) AllocVec(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
        if(pi)
        {
            g_pInputs->_keyboard_data = (struct KeyboardMatrix *) AllocVec(sizeof(struct KeyboardMatrix), MEMF_PUBLIC|MEMF_CLEAR);
            if(g_pInputs->_keyboard_data)
            {
                if (g_pInputs->_KeyMP=CreatePort(NULL,NULL))
                {
                    if (g_pInputs->_KeyIO=(struct IOStdReq *)
                            CreateExtIO(g_pInputs->_KeyMP,sizeof(struct IOStdReq)))
                    {
                        if (!OpenDevice("keyboard.device",NULL,(struct IORequest *)g_pInputs->_KeyIO,NULL))
                        {
                            pi->is_Node.ln_Name = (char *)"mamekbd";
                            pi->is_Data = (APTR)g_pInputs->_keyboard_data;
                            pi->is_Code = (VOID    (*)()) &mamekbdInteruptfunc;
                            AddIntServer(INTB_VERTB,pi);
                            g_pInputs->_doUseDirectKeyboard = 1;

                        } // end if opendevice ok
                    } // end if  CreateExtIO ok
                } // end if createport ok

            } // end if data alloc ok
        } // end if int alloc ok
    } // end if use direct kbd
#endif
}

void FreeInputs()
{
    if(LowLevelBase)
    {   // back to mouse ?
        SetJoyPortAttrs(0,SJA_Type,SJA_TYPE_MOUSE,TAG_DONE);
    }

    if(g_pParallelPads)
    {
        closeParallelPads(g_pParallelPads);
        g_pParallelPads = NULL;
    }


    if(g_pInputs)
    {
#ifdef USE_DIRECT_KEYBOARD_DEVICE
        if(g_pInputs->_keyboard_interupt)
        {
            RemIntServer(INTB_VERTB,g_pInputs->_keyboard_interupt);

            if(g_pInputs->_KeyIO)
            {
                CloseDevice((struct IORequest *)g_pInputs->_KeyIO);
                DeleteExtIO((struct IORequest *)g_pInputs->_KeyIO);
            }
            if(g_pInputs->_KeyMP)
            {
                DeletePort(g_pInputs->_KeyMP);
            }
            if(g_pInputs->_keyboard_data) FreeVec(g_pInputs->_keyboard_data);
            FreeVec(g_pInputs->_keyboard_interupt);
         }
#endif
        free(g_pInputs);
     }
    g_pInputs = NULL;
}
// called from video
void UpdateInputs(struct MsgPort *pMsgPort)
{
  struct IntuiMessage *im;
  struct MenuItem   *mitem;
 int doSwitchFS=0;
    static UBYTE fcounter =1;
    fcounter++;
    if(fcounter==255) fcounter=1; // we just need it to be different from previous frame.

 //printf("UpdateInputs: %08x\n",(int)g_pInputs);
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
                        doSwitchFS =1;
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


//        case IDCMP_MENUPICK:
//        if(inputs->MenuHook)
//        {
//          while(imcode != MENUNULL)
//          {
//            CallHook(inputs->MenuHook, NULL, ITEMNUM(imcode));

//            mitem = ItemAddress(inputs->Window->MenuStrip, imcode);
//            imcode  = mitem->NextSelect;
//          }
//        }
//        break;

//        case IDCMP_REFRESHWINDOW:
//        BeginRefresh(inputs->Window);
//        if(inputs->RefreshHook)
//          CallHookPkt(inputs->RefreshHook, NULL, NULL);
//        EndRefresh(inputs->Window, TRUE);
//        break;

//        case IDCMP_ACTIVEWINDOW:
//        IEnable(inputs);
//        break;

//        case IDCMP_INACTIVEWINDOW:
//        IDisable(inputs);
//        break;

//        default:
//        if(inputs->IDCMPHook)
//          CallHook(inputs->IDCMPHook, NULL, imclass);
        }
    }
    // if any mouse (or anything that needs direct joyport ?)
    // no rawkey for this
    if(useAnyMouse
#ifdef RJP_OPTION
    || useReadJoyPortForPads
#endif
    )
    {
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
           //validated ok
               // printf("g_pInputs->_mousestate %d %08x\n",iLLPort,(int)state);
               g_pInputs->_mstate[iLLPort]._mousestate = state;

//#define MOUSEBTMASK (JPF_BUTTON_RED|JPF_BUTTON_BLUE|JPF_BUTTON_PLAY)

               // got to redirect mouse buttons in that case, to shot in arkanoid for example.
               int playershift = (iplayer-1)<<8;
               // not perfect to do here because could jump a slow frame
               g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_RED+playershift] = (int)((state & JPF_BUTTON_RED)!=0);
               g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_BLUE+playershift] = (int)((state & JPF_BUTTON_BLUE)!=0);
               g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_PLAY+playershift] = (int)((state & JPF_BUTTON_PLAY)!=0);

                //#define JP_MHORZ_MASK	(255<<0)	/* horzizontal position */
                //#define JP_MVERT_MASK	(255<<8)	/* vertical position	*/
                //#define JP_MOUSE_MASK	(JP_MHORZ_MASK|JP_MVERT_MASK)
            }
#ifdef RJP_OPTION
             else if(useReadJoyPortForPads &&
                ( portType == SJA_TYPE_JOYSTK || portType == SJA_TYPE_GAMECTLR))
            {
                int playershift = (iplayer-1)<<8;
                // NOT NEEDED WHEN RAWKEY MODE
                g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_RED+playershift] = (int)((state & JPF_BUTTON_RED)!=0);
                g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_BLUE+playershift] = (int)((state & JPF_BUTTON_BLUE)!=0);

                g_pInputs->_Keys[RAWKEY_PORT0_JOY_UP+playershift] = (int)((state & JPF_JOY_UP)!=0);
                g_pInputs->_Keys[RAWKEY_PORT0_JOY_DOWN+playershift] = (int)((state & JPF_JOY_DOWN)!=0);
                g_pInputs->_Keys[RAWKEY_PORT0_JOY_LEFT+playershift] = (int)((state & JPF_JOY_LEFT)!=0);
                g_pInputs->_Keys[RAWKEY_PORT0_JOY_RIGHT+playershift] = (int)((state & JPF_JOY_RIGHT)!=0);
                if(portType == SJA_TYPE_GAMECTLR)
                {
                    g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_YELLOW+playershift] = (int)((state & JPF_BUTTON_YELLOW)!=0);
                    g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_GREEN+playershift] = (int)((state & JPF_BUTTON_GREEN)!=0);
                    g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_FORWARD+playershift] = (int)((state & JPF_BUTTON_FORWARD)!=0);
                    g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_REVERSE+playershift] = (int)((state & JPF_BUTTON_REVERSE)!=0);
                    g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_PLAY+playershift] = (int)((state & JPF_BUTTON_PLAY)!=0);
                }
            }
#endif
        } // loop by port
    }
#ifdef USE_DIRECT_KEYBOARD_DEVICE
    if(g_pInputs->_doUseDirectKeyboard)
    {   // this is ored on interuption
//        if(!(*g_pInputs->_keyboard_data == g_pInputs->_mainthreadkeys) )
//        {
//            printf("ks: %08x %08x %08x %08x\n",
//                g_pInputs->_keyboard_data->_kl[0],
//                g_pInputs->_keyboard_data->_kl[1],
//                g_pInputs->_keyboard_data->_kl[2],
//                g_pInputs->_keyboard_data->_kl[3]
//                   );
//        }
        g_pInputs->_mainthreadkeys = *g_pInputs->_keyboard_data;

         g_pInputs->_keyboard_data->_kl[0]=
         g_pInputs->_keyboard_data->_kl[1]=
         g_pInputs->_keyboard_data->_kl[2]=
         g_pInputs->_keyboard_data->_kl[3]= 0;

    }
#endif
    // apply change from parallel pads to player 3 & 4
    // g_pParallelPads exists if any player selected for either pr port.
    if(g_pParallelPads && g_pParallelPads->_ppidata->_last_checked_changes )
    {
        MameConfig::Controls &configControls = getMainConfig().controls();
        int prport3_player = configControls._parallelPort_Player[0];
        int prport4_player = configControls._parallelPort_Player[1];

        // bit are hardware regs ((ciaaprb<<8) | ciabpra)
        // ciaaprb: parallel port data, used as (joy3 RightLeftDownUp, joy4 LeftRightUpDown)
        // ciabpra:
        UWORD changed = g_pParallelPads->_ppidata->_last_checked_changes;
        const UWORD state = g_pParallelPads->_ppidata->_last_checked;

        // rk means rawkey, spud.
        static const UWORD rk[]={
            // order of the bits in parrallel registers for loop test.
            RAWKEY_PORT0_JOY_RIGHT,RAWKEY_PORT0_JOY_LEFT,
            RAWKEY_PORT0_JOY_DOWN,RAWKEY_PORT0_JOY_UP,
        };

        static const UWORD prportDirectionsBits[]={0x0800,0x8000};
        // ciabpra is parralel portcontrol bits, used for joystick buttons.
        // commodore official SDK defines
//#define CIAF_PRTRSEL	(1L<<2)
//#define CIAF_PRTRPOUT	(1L<<1)  -> should work ok
//#define CIAF_PRTRBUSY	(1L<<0)
        static const UWORD prportFireBits[]={0x0004,0x0001};
        static const UWORD prportFire2Bits[]={0x0002,0x0000};

        for(int iparallelportJoystick=0 ; iparallelportJoystick<2 ; iparallelportJoystick++)
        {
            int iplayer = configControls._parallelPort_Player[iparallelportJoystick];
            if(iplayer>0)
            {
                //NO ! int iPlayer_rkshift = ((iplayer-1) & 3)<<8;
                // we always declare prp3/prp4 as LL3&4.
                // the redirection "to either player1,2,3,4 controller" is done
                // up there in osd_get_code_list()->rawkeymap.init() at game init.
                int iPlayer_rkshift = (iparallelportJoystick+2)<<8;
                //  test directions
                UWORD testbit=prportDirectionsBits[iparallelportJoystick];
                for(int i=0;i<4;i++) {
                    g_pInputs->_Keys[rk[i]+iPlayer_rkshift] = (BYTE)((testbit & state)!=0); // down
                    testbit>>=1;
                }
                // fire bt
                g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_RED+iPlayer_rkshift] =
                    (BYTE)((prportFireBits[iparallelportJoystick] & state)!=0); // down
                // r1.6: may have second button on parallel port jstick
                g_pInputs->_Keys[RAWKEY_PORT0_BUTTON_BLUE+iPlayer_rkshift] =
                    (BYTE)((prportFireBits[iparallelportJoystick] & state)!=0); // down
            }
        }

        g_pParallelPads->_ppidata->_last_checked = 0;
        g_pParallelPads->_ppidata->_last_checked_changes = 0;
    }

    if(doSwitchFS) SwitchWindowFullscreen();
}

/******************************************************************************

  Keyboard

******************************************************************************/



extern struct Library *KeymapBase;



void mapRawKeyToString(UWORD rawkeycode, std::string &s)
{
    char temp[8];
    struct InputEvent ie={0};
    ie.ie_Class = IECLASS_RAWKEY;
//    ie.ie_SubClass = 0;
    ie.ie_Code = rawkeycode;
//    ie.ie_Qualifier = 0; // im->Qualifier;
//    ie.ie_EventAddress
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
inline unsigned int nameToMameKeyEnum(std::string &s)
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

// - - - keyboard keymap management
void RawKeyMap::init()
{
   _keymap_inited = true;
    // we map RAWKEY codes, their meaning can change
    // according to Amiga keyboard locale settings.
    // here are all the easy constant ones that match all keyboards:

    // note: some code calling this just uglily test all values all along,
    // so a good optimisation is stupidly to have the most comon key used first.

    _kbi = {
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
    MameConfig::Controls &configControls = getMainConfig().controls();
    MameConfig::Misc &configMisc = getMainConfig().misc();
 //   int amega32order = (int)((configMisc._MiscFlags & 1) != 0);

    // then add player to paddle according to conf.    
    // lowlevel send rawkeys for each CD32 pads.
    {
        MameConfig::Controls &configControls = getMainConfig().controls();
        for(int iLLPort=0;iLLPort<4;iLLPort++) // actually 2
        {
            int iplayer = configControls._llPort_Player[iLLPort];
            if(iplayer == 0) continue;
            int itype = configControls._llPort_Type[iLLPort];
            //old bug ! if(itype == 0) continue; //still not inited
            // here we treat only CD32Pads and 1/2Bt joysticks...
            // do not declare pads if it is mouse !!!
            if((itype != SJA_TYPE_GAMECTLR) && (itype != SJA_TYPE_JOYSTK) ) continue;
//            cp controlPort = configControls._PlayerPort[iplayer];
//            int lowlevelState = (int) configControls._PlayerPortType[iLLPort];

                int iport = iLLPort; // 0->3
                int ipshft = iport<<8;
                const int mamecodeshift =
                    ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT) *(iplayer-1) ;
            // if(amega32order == 0)
            // {
              vector<os_code_info> kbi2={
                {padsbtnames[iport][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
                {padsbtnames[iport][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
                {padsbtnames[iport][3-1],RAWKEY_PORT0_BUTTON_YELLOW+ipshft,JOYCODE_1_BUTTON3+mamecodeshift},
                {padsbtnames[iport][4-1],RAWKEY_PORT0_BUTTON_GREEN+ipshft,JOYCODE_1_BUTTON4+mamecodeshift},
                {padsbtnames[iport][5-1],RAWKEY_PORT0_BUTTON_FORWARD+ipshft,JOYCODE_1_BUTTON6+mamecodeshift},
                {padsbtnames[iport][6-1],RAWKEY_PORT0_BUTTON_REVERSE+ipshft,JOYCODE_1_BUTTON5+mamecodeshift},
                {padsbtnames[iport][6],RAWKEY_PORT0_BUTTON_PLAY+ipshft,JOYCODE_1_START+mamecodeshift},
                {padsbtnames[iport][7],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
                {padsbtnames[iport][8],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
                {padsbtnames[iport][9],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
                {padsbtnames[iport][10],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
                };
                _kbi.insert(_kbi.end(),kbi2.begin(),kbi2.end());
           // } else {
           //    vector<os_code_info> kbi2={
           //      {padsbtnames[iport][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
           //      {padsbtnames[iport][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
           //      {padsbtnames[iport][5-1],RAWKEY_PORT0_BUTTON_YELLOW+ipshft,JOYCODE_1_BUTTON5+mamecodeshift},
           //      {padsbtnames[iport][4-1],RAWKEY_PORT0_BUTTON_GREEN+ipshft,JOYCODE_1_BUTTON4+mamecodeshift},
           //      {padsbtnames[iport][3-1],RAWKEY_PORT0_BUTTON_FORWARD+ipshft,JOYCODE_1_BUTTON3+mamecodeshift},
           //      {padsbtnames[iport][6-1],RAWKEY_PORT0_BUTTON_REVERSE+ipshft,JOYCODE_1_BUTTON6+mamecodeshift},
           //      {padsbtnames[iport][6],RAWKEY_PORT0_BUTTON_PLAY+ipshft,JOYCODE_1_START+mamecodeshift},
           //      {padsbtnames[iport][7],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
           //      {padsbtnames[iport][8],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
           //      {padsbtnames[iport][9],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
           //      {padsbtnames[iport][10],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
           //      };
           //      _kbi.insert(_kbi.end(),kbi2.begin(),kbi2.end());
           // }

//            // if parralel port hacks as port 3 and 4.
//           if(controlPort == cp::Para3 ||
//               controlPort == cp::Para4 ||
//               controlPort == cp::Para3Bt4 )
//            {
//                int iport = 2;
//                if(controlPort == cp::Para4) iport=3;
//                int ipshft = iport<<8;
//                const int mamecodeshift =
//                    ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT) *iLLPort ;

//            // joystick are not CD32 pads, can only manage 1 or 2 bt pads here (2 for sega SMS pads)...
//              vector<os_code_info> kbi2={
//                {padsbtnames[iport][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
//                {padsbtnames[iport][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
//                //{padsbtnames[iport][2],RAWKEY_PORT0_BUTTON_YELLOW+ipshft,JOYCODE_1_BUTTON3+mamecodeshift},
//                //{padsbtnames[iport][3],RAWKEY_PORT0_BUTTON_GREEN+ipshft,JOYCODE_1_BUTTON4+mamecodeshift},
//                //{padsbtnames[iport][4],RAWKEY_PORT0_BUTTON_FORWARD+ipshft,JOYCODE_1_BUTTON6+mamecodeshift},
//                //{padsbtnames[iport][5],RAWKEY_PORT0_BUTTON_REVERSE+ipshft,JOYCODE_1_BUTTON5+mamecodeshift},
//                //{padsbtnames[iport][6],RAWKEY_PORT0_BUTTON_PLAY+ipshft,JOYCODE_1_START+mamecodeshift},
//                {padsbtnames[iport][7],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
//                {padsbtnames[iport][8],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
//                {padsbtnames[iport][9],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
//                {padsbtnames[iport][10],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
//                };
//                _kbi.insert(_kbi.end(),kbi2.begin(),kbi2.end());

//            } // end if parallel port hacks concerned.
        } // end loop by port
        for(int ipar=0;ipar<2;ipar++) // actually 2
        {
            int iplayer = configControls._parallelPort_Player[ipar];
            if(iplayer == 0) continue;
            int itype = configControls._parallel_type[ipar];
            if(itype == 0) continue; //still not inited

            int iport = 2+ipar; // we hack parallel pads as Lowlevel Pads3 and 4 !!!
            int ipshft = iport<<8;
            const int mamecodeshift =
                ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT) *(iplayer-1) ;

        // joystick are not CD32 pads, can only manage 1 or 2 bt pads here (2 for sega SMS pads)...
          vector<os_code_info> kbi2={
            {padsbtnames[iport][0],RAWKEY_PORT0_BUTTON_BLUE+ipshft,JOYCODE_1_BUTTON2+mamecodeshift},
            {padsbtnames[iport][1],RAWKEY_PORT0_BUTTON_RED+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
            //{padsbtnames[iport][2],RAWKEY_PORT0_BUTTON_YELLOW+ipshft,JOYCODE_1_BUTTON3+mamecodeshift},
            //{padsbtnames[iport][3],RAWKEY_PORT0_BUTTON_GREEN+ipshft,JOYCODE_1_BUTTON4+mamecodeshift},
            //{padsbtnames[iport][4],RAWKEY_PORT0_BUTTON_FORWARD+ipshft,JOYCODE_1_BUTTON6+mamecodeshift},
            //{padsbtnames[iport][5],RAWKEY_PORT0_BUTTON_REVERSE+ipshft,JOYCODE_1_BUTTON5+mamecodeshift},
            //{padsbtnames[iport][6],RAWKEY_PORT0_BUTTON_PLAY+ipshft,JOYCODE_1_START+mamecodeshift},
            {padsbtnames[iport][7],RAWKEY_PORT0_JOY_UP+ipshft,JOYCODE_1_UP+mamecodeshift},
            {padsbtnames[iport][8],RAWKEY_PORT0_JOY_DOWN+ipshft,JOYCODE_1_DOWN+mamecodeshift},
            {padsbtnames[iport][9],RAWKEY_PORT0_JOY_LEFT+ipshft,JOYCODE_1_LEFT+mamecodeshift},
            {padsbtnames[iport][10],RAWKEY_PORT0_JOY_RIGHT+ipshft,JOYCODE_1_RIGHT+mamecodeshift}
            };
            _kbi.insert(_kbi.end(),kbi2.begin(),kbi2.end());

        } // end loop per par

    }
    // then may add analog controls
    if(useAnyMouse)
    {
        MameConfig::Controls &configControls = getMainConfig().controls();
       // printf("***  USE ANY MOUSE \n");
        for(int iport=0;iport<4;iport++) // actually 2
        {
            int iplayer = configControls._llPort_Player[iport];
            if(iplayer == 0) continue;
            int itype = configControls._llPort_Type[iport];
          //  printf("iport:%d itype:%d\n",iport,itype);
            if(itype != SJA_TYPE_MOUSE) continue; //still not inited

            int mameAnlgSizePerPl = ((int)MOUSECODE_2_ANALOG_X-(int)MOUSECODE_1_ANALOG_X);
            int mameMouseBtSizePerPl = ((int)MOUSECODE_2_BUTTON1-(int)MOUSECODE_1_BUTTON1);
            {
                _kbi.push_back({_keepMouseNames[(iport*8)+0].c_str(),
                                ANALOG_CODESTART+(iport*8)+0,
                                MOUSECODE_1_ANALOG_X+((iplayer-1)*mameAnlgSizePerPl) });
            }
            {
                _kbi.push_back({_keepMouseNames[(iport*8)+1].c_str(),
                                ANALOG_CODESTART+(iport*8)+1,
                                MOUSECODE_1_ANALOG_Y+((iplayer-1)*mameAnlgSizePerPl) });
            }
            // mouse buttons
            {
                _kbi.push_back({_keepMouseNames[(iport*8)+2].c_str(),
                                ANALOG_CODESTART+(iport*8)+2,
                                MOUSECODE_1_BUTTON1+((iplayer-1)*mameMouseBtSizePerPl) });
            }
            {
                _kbi.push_back({_keepMouseNames[(iport*8)+3].c_str(),
                                ANALOG_CODESTART+(iport*8)+3,
                                MOUSECODE_1_BUTTON2+((iplayer-1)*mameMouseBtSizePerPl) });
            }
            {
                // note: I have never seen a working amiga mouse with that elusive 3rd button.
                // lowlevel says it can manage it.
                _kbi.push_back({_keepMouseNames[(iport*8)+4].c_str(),
                                ANALOG_CODESTART+(iport*8)+4,
                                MOUSECODE_1_BUTTON3+((iplayer-1)*mameMouseBtSizePerPl) });
            }
        } // loop by player
    }

    // then add rawkeys which meanings changes by locale settings
    vector<unsigned char> keystodo={0x0b,0x0c,0x0d}; // first of each lines
    {   unsigned char ic=0;
        while(ic<12) {keystodo.push_back(0x10+ic); ic++; }
        ic=0;
        while(ic<12) {keystodo.push_back(0x20+ic); ic++; }
        ic=0;
        while(ic<11) {keystodo.push_back(0x30+ic); ic++; }
    }
    const int nbk = (int)keystodo.size();
    _km.reserve(nbk);
    _km.resize(nbk);
    for(int i=0;i<nbk;i++)
    {
        mapkeymap &mkm =_km[i];
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
            os_code_info oci = {
                mkm._name.c_str(),
                (os_code)keystodo[i],
                (input_code)mamekc
                };
            _kbi.push_back(oci);
        } else {
        // printf("code with no name:%d\n",keystodo[i]);
        }
    }
    // then add those which need a rawkey mapped to a mame constant, with varying name
    // mame enum was modified for this.
    static string key3a_name;
    mapRawKeyToString((UWORD)0x003A,key3a_name);
    _kbi.push_back({key3a_name.c_str(),0x3A,AMIGA_SPECIAL_RAWKEY_3A});

    static string key2a_name;
    mapRawKeyToString((UWORD)0x002A,key2a_name);
    _kbi.push_back({key2a_name.c_str(),0x2A,AMIGA_SPECIAL_RAWKEY_2A});

    static string key2b_name;
    mapRawKeyToString((UWORD)0x002B,key2b_name);
    _kbi.push_back({key2b_name.c_str(),0x2B,AMIGA_SPECIAL_RAWKEY_2B});

    // end
    _kbi.push_back({NULL,0,0});
}



/*
  return a list of all available keys (see input.h)
*/
const os_code_info *osd_get_code_list(void)
{
//    printf(" * * * ** osd_get_key_list  * * * *  *\n");

    ConfigureLowLevelLib();

    if(!rawkeymap._keymap_inited)
    {
        rawkeymap.init(); // this si done once at each gamestart.
    }
    return rawkeymap._kbi.data();

}

/*
  return the value of the specified input. digital inputs return 0 or 1. analog
  inputs should return a value between -65536 and +65536. oscode is the OS dependent
  code specified in the list returned by osd_get_code_list().
*/

/*
  tell whether the specified key is pressed or not. keycode is the OS dependant
  code specified in the list returned by osd_get_key_list().
*/
INT32 osd_get_code_value(os_code oscode)
{
    // now , always rawkey.
    if(!g_pInputs) return 0;
#ifdef USE_DIRECT_KEYBOARD_DEVICE
    if(g_pInputs->_doUseDirectKeyboard)
    {
        if(oscode<0x72)
        {
            return (g_pInputs->_mainthreadkeys._k[oscode>>3] & (1<<(oscode&7)));
        }
    }
#endif
    if(oscode<(256*4)) // 256*4 is lowlevel extended rawkey range.
    {
       // printf("ASKED :%04x\n",(int)oscode);
//        if(g_pInputs->_Keys[oscode])
//        {
//            printf("ASKED AND GOT KEY:%d\n",(int)oscode);
//        }
        return (int)g_pInputs->_Keys[oscode];
    } else
    {
        // does analog like this
        // 1024: analog ll port1
        //
        oscode -= ANALOG_CODESTART;
        int illport = oscode>>3; // /8
        if(illport>=4) return 0;
        UINT8 shortcode = ((UINT8)oscode) & 7;


/* note 21/05/2025
 MAME inptport.c management differenciates those emulated controls:
  - Analog relative inputs: (mouse,trackballs) which ask -+deltas.
  - Analog Absolute inputs: (wheels,lightguns,...) which ask final value.

 it allows to "plug to each of them" Analog relative or Analog Absolute, and adapt values.

Amiga Hardware mouse send relative delta, but:
intuition gives absolute values with pixel coords (reported to a rastport resolution)
and lowlevel mouse/trackball (used here) send absolute values in a 8bit scale
 (really not enough precision, but at least not reported to a resolution)

 If now we imagine plugin a real wheel (USB?) that sends absolute rotation angle,
 We can use the lowlevel mouse API because it already sends absolute.
...  so we can either declare the lowlevel mouse to mame as analog/relative OR analog/absolute,
 which may help plugin a real wheel.

*/
        MameInputs::LLMouse &llm = g_pInputs->_mstate[illport]; // mousestate[illport];
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

        //UBYTE isy = (UBYTE)oscode &1;

        // lowlevel only gives 8 low bits of mouse position.
        // This circus is for re-applying coordinates with highter bits.


//	            If type = JP_TYPE_MOUSE the bit map of portState is:
//	                JPF_BUTTON_BLUE         Right mouse
//	                JPF_BUTTON_RED          Left mouse
//	                JPF_BUTTON_PLAY         Middle mouse
//	                JP_MVERT_MASK           Mask for vertical counter
//	                JP_MHORZ_MASK           Mask for horizontal counter
    }


    return 0;
}

/*
  wait for the user to press a key and return its code. This function is not
  required to do anything, it is here so we can avoid bogging down multitasking
  systems while using the debugger. If you don't want to or can't support this
  function you can just return OSD_KEY_NONE.
*/
//int osd_wait_keypress(void)
//{
//   // printf("osd_wait_keypress\n");

//    return OSD_KEY_NONE;
//}

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


