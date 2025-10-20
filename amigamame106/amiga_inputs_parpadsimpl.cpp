#include <vector>
#include "amiga_inputs_interface.h"
#include "amiga_config.h"
#include "amiga_inputs_parallelpads.h"
#include <proto/exec.h>
extern "C" {
    // from MAME
    #include "input.h"
}


using namespace std;

struct sParPadsInputModule {
    struct ParallelPads *_p;
    // 2 pads, 6 buttons...
    UBYTE _padsbt[2*8];
};


static const char * const parpadsbtnames[2][11]={
    {
        "PrPad3 RIGHT",
        "PrPad3 LEFT",
        "PrPad3 DOWN",
        "PrPad3 UP",
        "PrPad3 Red",
        "PrPad3 Blue"
    },
    {
        "PrPad4 RIGHT",
        "PrPad4 LEFT",
        "PrPad4 DOWN",
        "PrPad4 UP",
        "PrPad4 Red",
        "PrPad4 Blue"
    }
};

static void *parpads_Create(void *registerer,fAddOsCode addOsCode)
{
    MameConfig::Controls &configControls = getMainConfig().controls();

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

    struct sParPadsInputModule *p = (struct sParPadsInputModule *)AllocVec(sizeof(struct sParPadsInputModule),MEMF_CLEAR);
    if(!p) return NULL;

    p->_p = createParallelPads(readj4bt2); // could fail.
    if(!p->_p)
    {
        FreeVec(p);
        return NULL;
    }

    // then parallel port joystick extension pr3 pr4
    for(int ipar=0;ipar<2;ipar++) // actually 2
    {
        int iplayer = configControls._parallelPort_Player[ipar];
        if(iplayer == 0) continue;
        iplayer--;
        int itype = configControls._parallel_type[ipar];
        if(itype == 0) continue; //still not inited

        int ipshft = ipar<<3;
        const int mamecodeshift =
            ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT) *iplayer ;

    // joystick can only manage 1 or 2 bt pads here (2 for sega SMS pads)...
      vector<os_code_info> kbi2={
        {parpadsbtnames[ipar][0],0+ipshft,JOYCODE_1_RIGHT+mamecodeshift},
        {parpadsbtnames[ipar][1],1+ipshft,JOYCODE_1_LEFT+mamecodeshift},
        {parpadsbtnames[ipar][2],2+ipshft,JOYCODE_1_DOWN+mamecodeshift},
        {parpadsbtnames[ipar][3],3+ipshft,JOYCODE_1_UP+mamecodeshift},
      // note standard parallel port extensions joysticks manages 1bt each, 2nd button only if hw hack.
        {parpadsbtnames[ipar][4],4+ipshft,JOYCODE_1_BUTTON1+mamecodeshift},
        {parpadsbtnames[ipar][5],5+ipshft,JOYCODE_1_BUTTON2+mamecodeshift}
        };
        //_kbi.insert(_kbi.end(),kbi2.begin(),kbi2.end());
        addOsCode(registerer,kbi2.data(),kbi2.size());
    } // end loop per par

    return p;
}
// optional, per frame.
static void parpads_FrameUpdate(void *o)
{
    struct sParPadsInputModule *pm = (struct sParPadsInputModule *)o;
    struct ParallelPads *p =  pm->_p;

    // apply change from parallel pads to player 3 & 4
    // g_pParallelPads exists if any player selected for either pr port.
    if( ! p->_ppidata->_last_checked_changes ) return;

    MameConfig::Controls &configControls = getMainConfig().controls();

    // bit are hardware regs ((ciaaprb<<8) | ciabpra)
    // ciaaprb: parallel port data, used as (joy3 RightLeftDownUp, joy4 LeftRightUpDown)
    // ciabpra:
    UWORD changed = p->_ppidata->_last_checked_changes;
    const UWORD state = p->_ppidata->_last_checked;

    static const UWORD prportDirectionsBits[2]={0x0100,0x1000}; //APARJOY_J3_RIGHT, APARJOY_J4_RIGHT

    static const UWORD prportFire1Bits[2]={APARJOY_J3_FIRE1,APARJOY_J4_FIRE1};
    static const UWORD prportFire2Bits[2]={APARJOY_J3_FIRE2,APARJOY_J4_FIRE2};
    for(int iparallelportJoystick=0 ; iparallelportJoystick<2 ; iparallelportJoystick++)
    {
        int iplayer = configControls._parallelPort_Player[iparallelportJoystick];
        if(iplayer>0)
        {
            //NO ! int iPlayer_rkshift = ((iplayer-1) & 3)<<8;
            // we always declare prp3/prp4 as LL3&4.
            // the redirection "to either player1,2,3,4 controller" is done
            // up there in osd_get_code_list()->rawkeymap.init() at game init.
            int iPlayer_rkshift = iparallelportJoystick<<3;
            //  test directions
            UWORD testbit=prportDirectionsBits[iparallelportJoystick];
            for(int i=0;i<4;i++) {
                pm->_padsbt[i+iPlayer_rkshift] = (BYTE)((testbit & state)!=0); // right
                testbit<<=1;
            }
            // fire bt
            pm->_padsbt[4+iPlayer_rkshift] =
                (BYTE)((prportFire1Bits[iparallelportJoystick] & state)!=0); // red

            // r1.6: we may have second button on parallel port jstick 3 and 4 ,
            //  but it also needs 2 more wires in parallel extension.
            pm->_padsbt[5+iPlayer_rkshift] =
                (BYTE)((prportFire2Bits[iparallelportJoystick] & state)!=0); // blue
        }
    }

    p->_ppidata->_last_checked = 0;
    p->_ppidata->_last_checked_changes = 0;


}
// mandatory, used by real time.
static int parpads_GetCode(void *o, ULONG oscode)
{
    return ((struct sParPadsInputModule *)o)->_padsbt[oscode];
}
static void parpads_Close(void *o)
{
    closeParallelPads(((struct sParPadsInputModule *)o)->_p);
    FreeVec(o);
}

struct sMameInputsInterface g_ipt_ParallelPads=
{
    parpads_Create,
    parpads_FrameUpdate,
    parpads_GetCode,
    parpads_Close,
    NULL //    PostInputPortInitCheck
};
