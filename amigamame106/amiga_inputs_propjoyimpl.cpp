#include <vector>
#include "amiga_inputs_interface.h"
#include "amiga_config.h"
#include "amiga_inputs_propjoy.h"

#include <proto/exec.h>
extern "C" {
    // from MAME
    #include "input.h"
    #include "mamelog.h"
    #include "usrintrf.h"
    #include "inptport.h"
}


using namespace std;

// pjoy/pad , port1/2 , X/Y
static const char * const _AnalogNames[2][2][4]=
{
    {
        {"PropJoy1 X","PropJoy1 Y","PropJoy1 Bt1","PropJoy1 Bt2"},
        {"PropJoy2 X","PropJoy2 Y","PropJoy2 Bt1","PropJoy2 Bt2"}
    },
    {
        {"C64Pad1 X","C64Pad1 Y","C64Pad1 Bt1","C64Pad1 Bt2"},
        {"C64Pad2 X","C64Pad2 Y","C64Pad2 Bt1","C64Pad2 Bt2"}
    }
};

static void *propjoy_Create(void *registerer,fAddOsCode addOsCode)
{
    MameConfig::Controls &configControls = getMainConfig().controls();

    ULONG propJoysticksFlags=0;
    if( configControls._llPort_Type[0] == PORT_TYPE_PROPORTIONALJOYSTICK ||
        configControls._llPort_Type[0] == PORT_TYPE_C64PADDLE)
    {
        propJoysticksFlags |= PROPJOYFLAGS_PORT1 ;
        if(configControls._llPort_Type[0] == PORT_TYPE_C64PADDLE ) propJoysticksFlags |= PROPJOYFLAGS_PORT1_INVERTXY;
    }
    if( configControls._llPort_Type[1] == PORT_TYPE_PROPORTIONALJOYSTICK ||
        configControls._llPort_Type[1] == PORT_TYPE_C64PADDLE)
    {
        propJoysticksFlags |= PROPJOYFLAGS_PORT2 ;
       if(configControls._llPort_Type[1] == PORT_TYPE_C64PADDLE ) propJoysticksFlags |= PROPJOYFLAGS_PORT2_INVERTXY;
    }
    // if not asked, exit.
    if((propJoysticksFlags & (PROPJOYFLAGS_PORT1|PROPJOYFLAGS_PORT2))==0) return NULL;

    if(configControls._PropJoyAxisReverseP2 & 1)  propJoysticksFlags |= PROPJOYFLAGS_PORT2_INVERSEX ;
    if(configControls._PropJoyAxisReverseP2 & 2)  propJoysticksFlags |= PROPJOYFLAGS_PORT2_INVERSEY ;
    if(configControls._PropJoyAxisReverseP1 & 1)  propJoysticksFlags |= PROPJOYFLAGS_PORT1_INVERSEX ;
    if(configControls._PropJoyAxisReverseP1 & 2)  propJoysticksFlags |= PROPJOYFLAGS_PORT1_INVERSEY ;

    ULONG result=PROPJOYRET_OK;
    struct ProportionalSticks *p = createProportionalSticks(propJoysticksFlags,
                    0/*PROPJOYTIMER_LOWLEVEL_ADDTIMER*/, // try cia timer
                    &result,loginfo2); // could fail.

    if(!p || (result != PROPJOYRET_OK))
    {
        logerror(getProportionalStickErrorMessage(result));
        return NULL;
    }

    ui_popup_time(5, "Analog controllers:\nGo far in all directions to calibrate.");

    // Map...
    for(int iport=0;iport<2;iport++)
    {
        int iplayer = configControls._llPort_Player[iport];
        if(iplayer == 0) continue;
        iplayer--;
        int itype = configControls._llPort_Type[iport];

        if((itype == PORT_TYPE_PROPORTIONALJOYSTICK ||
                itype == PORT_TYPE_C64PADDLE) && iport<2) // iport<2 because direct DB9 port1/2 code, not Lowlevel lib.
        {
            int ispad = (itype == PORT_TYPE_C64PADDLE)?1:0;

            int analogmamecodeshift = ((int)JOYCODE_2_ANALOG_X-(int)JOYCODE_1_ANALOG_X)*(iplayer);
            // but yet 2 bt joysticks. we need that.
            int buttonmamecodeshift = ((int)JOYCODE_2_LEFT - (int)JOYCODE_1_LEFT)*(iplayer);
            int ipshft = iport<<8; // still use Lowlevel keycodes for buttons
            // these are real analog potentiometer, with 8b absolute coords.
            // let's declare them to MAME with enums JOYCODE_1_ANALOG_X
            // prop.joy button declared as regular joystick buttons...
                  vector<os_code_info> kbi2={
            {_AnalogNames[ispad][iport][2],(iport*8)+2,JOYCODE_1_BUTTON1+buttonmamecodeshift},
            {_AnalogNames[ispad][iport][3],(iport*8)+3,JOYCODE_1_BUTTON2+buttonmamecodeshift},
            // use same internal slot as mouses XY.
            {_AnalogNames[ispad][iport][0], (iport*8)+0,JOYCODE_1_ANALOG_X+analogmamecodeshift},
            {_AnalogNames[ispad][iport][1], (iport*8)+1,JOYCODE_1_ANALOG_Y+analogmamecodeshift},
                };

                addOsCode(registerer,kbi2.data(),kbi2.size());
        } // end if pots.
    } // loop by player

    return p;
}

// mandatory, used by real time.
static int propjoy_GetCode(void *o, ULONG oscode)
{
    int illport = oscode>>3;
    UINT8 shortcode = ((UINT8)oscode) & 7; // analog X,Y ,bt1,bt2

    struct PPSticksValues &ppsv = ((struct ProportionalSticks*)o)->_values[illport];

    if(!ppsv.valid) return 0; // needs 2 frames or more...

    // order and amplitude validated with outrun wheel drawing.
    if(shortcode ==0) return  (128-ppsv.x)<<9; // already calibrated
    if(shortcode ==1) return  (128-ppsv.y)<<9; // already calibrated
    // buttons
    if(shortcode ==2) return ((ppsv.bt & 1)!=0);
    if(shortcode ==3) return ((ppsv.bt & 2)!=0);

    return 0;
}
static void propjoy_Close(void *o)
{
    closeProportionalSticks(((struct ProportionalSticks *)o));
}

// this is thrown later at init, after machine inputs are inited.
static void propjoy_PostInputPortInitCheck(void *o)
{
    if( !input_machine_has_any_analog() )
    {
        ui_popup_time(6, "Game uses no analog input,\nso analog controllers are useless.");
    }
}

struct sMameInputsInterface g_ipt_PropJoy=
{
    propjoy_Create,
    NULL,
    propjoy_GetCode,
    propjoy_Close,
    propjoy_PostInputPortInitCheck
};
