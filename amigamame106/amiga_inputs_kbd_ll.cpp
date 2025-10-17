#include "amiga_inputs_kbd_ll.h"

#include "amiga_config.h"

#include <string>


extern "C" {
    // from MAME
    #include "osdepend.h"
    #include "input.h"
//    #include "inptport.h"
//    #include "usrintrf.h"
}


struct kbdInput {
    int         _NbKeysUpStack;
    BYTE         _Keys[256*4]; // bools state for actual keyboard rawkeys + lowlevel pads code
    UWORD        _NextKeysUpStack[256]; // delay event between frame to not loose keys.
};

static void *kbd_Create()
{
    struct kbdInput  *p = (struct kbdInput *) AllocVec(sizeof(struct kbdInput), MEMF_CLEAR);
    if(!p) return NULL;

    return p;
}
static void mapRawKeyToString(UWORD rawkeycode, std::string &s)
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

static void kbd_InitOsCodeMap(struct kbdInput *o,void *m,fAddOsCode addOsCode)
{
   // according to Amiga keyboard locale settings.
    // here are all the easy constant ones that match all keyboards:

    // note: some code calling this just uglily test all values all along,
    // so a good optimisation is stupidly to have the most comon key used first.

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
    // then add keys which need a rawkey mapped to a mame constant, with varying name
    // mame enum was modified for this.
    string key3a_name;
    mapRawKeyToString((UWORD)0x003A,key3a_name);
    _kbi.push_back({key3a_name.c_str(),0x3A,AMIGA_SPECIAL_RAWKEY_3A});

    static string key2a_name;
    mapRawKeyToString((UWORD)0x002A,key2a_name);
    _kbi.push_back({key2a_name.c_str(),0x2A,AMIGA_SPECIAL_RAWKEY_2A});

    static string key2b_name;
    mapRawKeyToString((UWORD)0x002B,key2b_name);
    _kbi.push_back({key2b_name.c_str(),0x2B,AMIGA_SPECIAL_RAWKEY_2B});


} // end kbd_InitOsCodeMap

static void FrameUpdate(struct kbdInput *o)
{


}
// mandatory, used by real time.
static int GetCode(struct kbdInput *o, ULONG oscode)
{

    return 0;
}

void kbd_Close(void *o)
{
    if(o) FreeVec(o);
}


struct sMameInputsInterface g_ipt_Keyboard=
{
    kbd_Create,
    kbd_InitOsCodeMap,
    kbd_FrameUpdate,
    kbd_GetCode,
    kbd_Close
}

// - - - - - - - -

struct mouseInput {
    int         _NbKeysUpStack;
    BYTE         _Keys[256*4]; // bools state for actual keyboard rawkeys + lowlevel pads code
    UWORD        _NextKeysUpStack[256]; // delay event between frame to not loose keys.
};

void *mouse_Create()
{
    struct mouseInput  *p = (struct mouseInput *) AllocVec(sizeof(struct mouseInput), MEMF_CLEAR);
    if(!p) return NULL;

    return p;
}



void mouse_InitOsCodeMap(void *o,void *m,fAddOsCode addOsCode)
{
   // we map RAWKEY codes, their meaning can change

}

void FrameUpdate(void *o)
{


}
// mandatory, used by real time.
int GetCode(void *o, ULONG oscode)
{

    return 0;
}

void mouse_Close(void *o)
{

}



struct sMameInputsInterface g_ipt_LLMouses=
{
    mouse_Create,
    mouse_InitOsCodeMap,
    mouse_FrameUpdate,
    mouse_GetCode,
    mouse_Close
}
