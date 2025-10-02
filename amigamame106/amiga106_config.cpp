#include "amiga106_config.h"

#include <sstream>
#include <algorithm>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>
#include <fstream>
#include "drivertuning.h"

// from mame
extern "C" {
    #include "driver.h"
    #include "mame.h"
    // use xml from mame
    #include "xmlfile.h"

    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
}

#include "serializer_mamexml.h"

extern "C" {
    int hasParallelPort();
    int hasProportionalStickResource();
}


using namespace std;

inline const std::string trimSlach( std::string s) {
    if(s.length()>0 && s.back()=='/') {
        return s.substr(0,s.length()-1);
    } else return s;
}


MameConfig &getMainConfig()
{
    static MameConfig config;
    return config;
}


MameConfig::MameConfig() : ASerializable()
    , _NumDrivers(0)
    , _activeDriver(-1)
    , _listShowState(0)
    , _sortMode(SortMode::Name)
    , _romsFoundTouched(false)
    , m_listcolstate({0})
{

}
MameConfig::~MameConfig()
{}

void MameConfig::setActiveDriver(int indexInDriverList)
{
    if(indexInDriverList<0 || indexInDriverList>=_NumDrivers)
    {
        _activeDriver = -1;
        _display._perScreenModeS.setActive("");
        _display._perGameS.setActive("");
        return;
    }

    _activeDriver = indexInDriverList;
    _display._perScreenModeS.setActive( _resolutionStrings[indexInDriverList]);

    const game_driver *drv  =drivers[indexInDriverList];
    _display._perGameS.setActive(drv->name);

}
void MameConfig::setDriverListShowMode(int listState)
{
    if(_listShowState == listState) return;
    _listShowState = listState;
    updateRomFilters();
}
void MameConfig::setDriverListFilters(unsigned long long enums,UWORD tagmask)
{
    if(_filter_genre_enums == enums &&
     _ored_genre_tags == tagmask) return;
    _filter_genre_enums = enums;
    _ored_genre_tags = tagmask;
    updateRomFilters();

}

void MameConfig::setColumnOrder(struct UIListState &listcolstate)
{
    m_listcolstate = listcolstate;
}

// xml ids must be all lowercase
static const char *pcd_mame="mame";

static const char *pcf_roms="roms"; // cached list of roms found.
//static const char *pcf_romsdir="romsdir";
//static const char *pcf_userdir="userdir";
static const char *pcf_last="last";
static const char *pcf_list="list";
//static const char *pcf_display="display";
//static const char *pcf_startwindowed="startwindowed";
int MameConfig::save()
{
    // note: got to save rom short name id, not driver index ! index evolve with compilation.
    //printf("MameConfig::save\n");

    xml_data_node *root = xml_file_create();
    xml_data_node *confignode;
    mame_file *file=NULL;
    xml_data_node *display;

    /* if we don't have a root, bail */
    if (!root)
        return 0;

    // note: FILETYPE_CONFIG will use getMainConfig().getUserDir()/file
    file = mame_fopen("main", 0, FILETYPE_CONFIG, 1);
    if(!file) {
        xml_file_free(root);
        return 0;
    }
    /* create a config node */
    confignode = xml_add_child(root,pcd_mame, NULL);

    xml_set_attribute_int(confignode, "version", 1);

    // save known rom list

    if(_romsFound.size()>0 && _romsFoundTouched)
    {

        mame_file *romsfoundfile = mame_fopen("romsfound", 0, FILETYPE_CONFIG, 1);
        // Beta3d: don't use xml for this, it explodes when romset is big.
        if(romsfoundfile)
        {

            int i=0;
            for(const _game_driver *const*d : _romsFound)
            {
                char sep=' ';
                if(i==8) {i=0; sep='\n';}
                mame_fputs(romsfoundfile,(*d)->name);
                mame_fputs(romsfoundfile," ");
                i++;
            }

            mame_fclose(romsfoundfile);
        }

    }


    if(_activeDriver !=-1)
    {
        xml_add_child(confignode,pcf_last, drivers[_activeDriver]->name );
    }
    if(_listShowState !=-1)
    {
        xml_data_node *pn = xml_add_child(confignode,pcf_list,NULL);
        if(pn) xml_set_attribute_int(pn,"show",_listShowState);

        if(m_listcolstate.nbcolumn>0)
        {
            std::stringstream ss;
            for(int i=0;i<m_listcolstate.nbcolumn;i++)
            {
                ss << (int)m_listcolstate.columnOrder[i] << " ";
            }
            string s =ss.str();
            xml_set_attribute(pn,"order",s.c_str());
            std::stringstream ss2;
            xml_set_attribute_int(pn,"sort",m_listcolstate.sortedcolumn);
        }
    }

    // - - - automatise serialized members:
    XmlWriter xmlwriter(confignode);
    serialize(xmlwriter);

    /* flush the file */
    xml_file_write(root, file);

    /* free and get out of here */
    xml_file_free(root);

    if(file) mame_fclose(file);

    return 1;
}
void MameConfig::resettodefault()
{
    toDefault();

    _romsFound.clear();
    _romsFoundReverse.clear();
    _activeDriver =-1;
    // need to be done even if load fail.
    initRomsFoundReverse();
}

int MameConfig::load()
{
    xml_data_node *root=NULL,*confignode,*node; //, *confignode, *systemnode;
	const char *srcfile;
	int version, count;
	mame_file *file=NULL;
    _romsFoundTouched = false;
    // resolve short name to index after load, like scan does.

    resettodefault();

    // note: FILETYPE_CONFIG will use getMainConfig().getUserDir()/file
    file = mame_fopen("main", 0, FILETYPE_CONFIG, 0);
    if(!file)  goto error;


    /* read the file */
	root = xml_file_read(file, NULL);
	if (!root)
		goto error;

	confignode = xml_get_sibling(root->child, pcd_mame);
	if (!confignode)
		goto error;

    {
        _romsFound.clear();
        mame_file *romsfoundfile = mame_fopen("romsfound", 0, FILETYPE_CONFIG, 0);
        if(romsfoundfile)
        {
            mame_fseek(romsfoundfile, 0, SEEK_END);
            ULONG filesize = mame_ftell(romsfoundfile);
            mame_fseek(romsfoundfile, 0, SEEK_SET);
            if(filesize>0)
            {
                char *p = (char *)malloc(filesize+1);
                if(p)
                {
                    UINT32 done = mame_fread(romsfoundfile,p,filesize);
                    p[done]=0;
                    UINT32 i=0;
                    string s;
                    while(i<done)
                    {
                        if((p[i]==' ' || i==(done-1) ) && s.length()>0)
                        {
                            //_romsFound.push_back(&drivers[idriver]);
                            int idriver = _driverIndex.index(s.c_str());
                            if(idriver>=0) _romsFound.push_back(&drivers[idriver]);
                            s.clear();
                        } else s+=p[i];
                        i++;
                    }
                    free(p);
                }
            }
            mame_fclose(romsfoundfile);
        }
    /* old, was ok, but sometimes explode mame xml
        node = xml_get_sibling(confignode->child,pcf_roms);

        if(node && node->value)
        {
            string roms( node->value ); // already start/end stripped.
            size_t i=0;
            while(i != string::npos)
            {
               size_t in = roms.find_first_of(" \t\n",i+1);
               if(i>0) i++;
                string s = roms.substr(i,in-i);
                if(s.size()>0) {
                    //printf("read rom:%s:\n",s.c_str());
                    int idriver = _driverIndex.index(s.c_str());
                    if(idriver>=0) _romsFound.push_back(&drivers[idriver]);
                }
               i = in;
            }
        }
        */
        initRomsFoundReverse();
    }

    node = xml_get_sibling(confignode->child, pcf_last);
    if(node && node->value) _activeDriver = _driverIndex.index(node->value);

    node = xml_get_sibling(confignode->child, pcf_list);

    _listShowState = 0;
    if(node)
    {
        _listShowState = xml_get_attribute_int(node,"show",0);
        const char *porderlist = xml_get_attribute_string(node,"order","");
        if(porderlist && *porderlist != 0) {
            stringstream ss;
            ss << porderlist;
            for(int i=0;i<8;i++) {
                int v;
                ss >> v;
                //printf("v:%d\n",v);
                m_listcolstate.columnOrder[i]=v;
            }
        } else
        {
            for(int i=0;i<8;i++) m_listcolstate.columnOrder[i]=i;
        }
        m_listcolstate.sortedcolumn = xml_get_attribute_int(node,"sort",0);

    }

    // - - - automatise serialized members, same way as write:
    {
        XmlReader xmlreader(confignode);
        serialize(xmlreader);
    }

    xml_file_free(root);
	mame_fclose(file);
	updateRomFilters();
	return 1;
error:
    if(root) xml_file_free(root);
    if(file) mame_fclose(file);
    updateRomFilters();
    return 0;
}
void MameConfig::serialize(ASerializer &serializer)
{
    // defines what is loaded/saved/gui edited.
    serializer("Display",   (ASerializable&)_display,SERFLAG_GROUP_SCROLLER);
    serializer("Audio",     (ASerializable&)_audio,0);
    serializer("Controls",  (ASerializable&)_controls, SERFLAG_GROUP_2COLUMS|SERFLAG_GROUP_HASCOMMENT);

    std::string controlPanelComments =
        "Describe what is plugged (Joystick,Pads,Mouses)\n and to which player it belongs.\n"
        "Keyboard is configured during game with Tab Key menu.";
    if(hasProportionalStickResource()) // unrelated bu tells we are on Amiga classic.
    {   // if classic hardware ports...
        controlPanelComments += "\nAnalog controllers must be plugged when switched off.";
    }
    serializer.setComment("Controls",controlPanelComments);

    serializer("Misc",     (ASerializable&)_misc,0);
    serializer("Help",     (ASerializable&)_help,SERFLAG_GROUP_SCROLLER);
}
void MameConfig::toDefault()
{
    _display._drawEngine = DrawEngine::CgxDirectCpuOrWPA8;
    _display._perScreenMode.clear();
    _display._color_brightness = 1.0f;
    //old_display._color_gamma = 1.0f;
    _display._flags = 0;
    _display._buffering = ScreenBufferMode::Single;

    _display._vector._resolution = VectorResolution::e480x360;
    _display._vector._glow = GlowMode::None;
    _display._vector._remanence = Remanence::Low;
    _display._vector._flags = VDISPLAYFLAGS_ANTIALIAS;
    _display._vector._intensity = 1.0f;
    _display._vector._flags = VDISPLAYFLAGS_ANTIALIAS;

    _audio._mode = AudioMode::AHI;
    _audio._freq = 22050;
    _audio._forceMono = true;


    #define SJA_TYPE_AUTOSENSE 0
    #define SJA_TYPE_GAMECTLR  1
    #define SJA_TYPE_MOUSE	   2
    #define SJA_TYPE_JOYSTK    3
//    _controls._PlayerPort[0]=ControlPortLL::Port2llJoy;
//        _controls._PlayerPortType[0]=1;
//    _controls._PlayerPort[1]=ControlPortLL::None; _controls._PlayerPortType[1]=0;
//    _controls._PlayerPort[2]=ControlPortLL::None; _controls._PlayerPortType[2]=0;
//    _controls._PlayerPort[3]=ControlPortLL::None; _controls._PlayerPortType[3]=0;
    _controls._llPort_Player[0] = 1;
    _controls._llPort_Type[0] = SJA_TYPE_MOUSE; // SJA_TYPE_AUTOSENSE

    _controls._llPort_Player[1] = 1;
    _controls._llPort_Type[1] = SJA_TYPE_GAMECTLR;

    _controls._llPort_Player[2] = 0;
    _controls._llPort_Type[2] = 0;
    _controls._llPort_Player[3] = 0;
    _controls._llPort_Type[3] = 0;

    _controls._parallelPort_Player[0]=0;
    _controls._parallel_type[0]=1; // joy1 by def because the only way up to date
    _controls._parallelPort_Player[1]=0;
    _controls._parallel_type[1]=1; // joy1 by def because the only way up to date

    _misc._romsPath = "PROGDIR:roms";
    _misc._samplesPath = "PROGDIR:samples";
 //NOT THIS ONE !! decide where configs are written:  _misc._userPath = "PROGDIR:user";
    _misc._useCheatCodeFile = false;
    _misc._cheatFilePath = "PROGDIR:cheat.dat";
   // _misc._MiscFlags = 0;
    _misc._Goodies =  GOODIESFLAGS_WHEEL|GOODIESFLAGS_GEAR;
    _misc._Optims = OPTIMFLAGS_DIRECTWGXWIN;

}
MameConfig::Display_PerScreenMode::Display_PerScreenMode() : ASerializable() {
// printf("Display_PerScreenMode\n");
    _modeid._modeId = INVALID_ID;
    _modeid._depth = 8;
}
void MameConfig::Display_PerScreenMode::serialize(ASerializer &serializer)
{
    serializer("Screen Selection",(int &)_ScreenModeChoice,{"Find Best","Select Mode"});
    serializer("Screen mode",_modeid);
    serializer("Full Screen",(int &)_FSscaleMode,{"Center, No Scale (Faster)","Scale To Border","Stretch Scale"});
    serializer("Rotate Screens",(int &)_rotateMode,{"None","+90","180","-90"});

    // want modeid greyed if screen not choosen
    // can't do [=] because real object is a clone
    serializer.listenChange("Screen Selection",[](ASerializer &serializer, void *p)
    {
        ScreenModeChoice *pScreenModeChoice = (ScreenModeChoice *)p;
        // if(*pScreenModeChoice == ScreenModeChoice::Best)
        // {
        //      _modeid._modeId = INVALID_ID;
        //      _modeid._depth = 8;
        //     //serializer.update("Display.Per Screen Mode.Screen mode");
        // }
        serializer.enable("Display.Per Screen Mode.Screen mode",(*pScreenModeChoice == ScreenModeChoice::Choose)?1:0);
        ASerializable *pser = serializer.getObject("Display.Per Screen Mode");
        if(pser) {
            Display_PerScreenMode *pthis = (Display_PerScreenMode *)pser;
            if(pthis && *pScreenModeChoice == ScreenModeChoice::Best)
            {
              pthis->_modeid._modeId = INVALID_ID;
              pthis->_modeid._depth = 8;
            }
            serializer.update("Display.Per Screen Mode.Screen mode");
        }
    });

//         int _window_posx,_window_posy,_window_width,_window_height,_window_validpos;
    serializer("wx",_window_posx,{});
    serializer("wy",_window_posy,{});
    serializer("ww",_window_width,{});
    serializer("wh",_window_height,{});
    serializer("wv",_window_validpos,{});
}
// void MameConfig::Display_PerScreenMode::valueUpdated(std::string upatedValue)
// {

//     if(_ScreenModeChoice == ScreenModeChoice::Best)
//     {
//         _modeid._modeId = INVALID_ID;
//         _modeid._depth = 8;
//     }
// }
bool MameConfig::Display_PerScreenMode::isDefault()
{   // will not be written if is default.
    return (_ScreenModeChoice == ScreenModeChoice::Best &&
            _rotateMode == RotateMode::Rot0 &&
            _window_validpos == 0
            );
}
// - - - - - - -

MameConfig::Display_PerGame::Display_PerGame() : ASerializable() {
}
void MameConfig::Display_PerGame::serialize(ASerializer &serializer)
{
    serializer("FrameSkip",_frameSkip);
}
bool MameConfig::Display_PerGame::isDefault()
{   // will not be written if is default.
    return (!_frameSkip);
}

// - - - - --
MameConfig::Display_Vector::Display_Vector() : ASerializable() {
}
void MameConfig::Display_Vector::serialize(ASerializer &serializer)
{
    serializer("Resolution",(int&)_resolution,{"320x240","400x300","480x360","640x480" });
    serializer("Glow Mode",(int&)_glow,{"None","Horizontal","Full"});
    serializer("Remanence",(int&)_remanence,{"None","Low","High"});

    serializer(" ",_flags,VDISPLAYFLAGS_ANTIALIAS,{"Antialias","High Color"});
     // min,max,step, default
   serializer("Intensity",_intensity,0.5f,1.5f,0.05f,1.0f);

}
bool MameConfig::Display_Vector::isDefault()
{   // will not be written if is default.
    return (_resolution == VectorResolution::e480x360 &&
            _glow == GlowMode::None &&
            _remanence == Remanence::Low &&
            (_flags == (VDISPLAYFLAGS_ANTIALIAS)) &&
            _intensity == 1.0f);

}

// - - - - - - -
MameConfig::Display::Display() : ASerializable()
    ,_perScreenModeS(_perScreenMode)
    ,_perGameS(_perGame)
{
}
void MameConfig::Display::serialize(ASerializer &serializer)
{
    serializer("Draw Engine",(int &)_drawEngine,{"CGX Direct CPU Or WPA8",
                                                 "CPU Remap+ScalePixelArray"});
    //
    serializer("Screen Buffer",(int&)_buffering,{"Single","Triple Buffer CSB (slow if nasty driver)","Double Buffer SVP (Also slow if...)"});
    serializer(" ",_flags,0,{
               "On Workbench",
               "Force Depth 16"
               });

    // min,max,step, default
   //nomore serializer("Brightness",_color_brightness,0.25f,1.5f,0.125f,1.0f);
   _color_brightness = 1.0f;

    serializer("Per Screen Mode",_perScreenModeS);
    serializer("Per Game",_perGameS);

    serializer("Vector Screen",_vector,SERFLAG_GROUP_SUB);
    serializer.setEnableIfSelected("Vector Screen","vector");
    serializer.enable("Display.Vector Screen",FALSE);
}
MameConfig::Display_PerScreenMode &MameConfig::Display::getActiveMode()
{
    return _perScreenModeS.getActive();
}
MameConfig::Display_PerGame &MameConfig::Display::getActiveGameConf()
{
    return _perGameS.getActive();
}

MameConfig::Audio::Audio() : ASerializable()
{
}

void MameConfig::Audio::serialize(ASerializer &serializer)
{
    serializer("Mode",(int &)_mode,{"  None  ","   AHI   "});
    serializer("Frequency",_freq,11025,22050,22050); // not more low hz than 11025 it does too little buffers for AHI.
  //hide this for the moment  serializer("Force Mono",_forceMono);

   //finaly not serializer("Flags",_Flags,1,{"Use Samples"});
}

MameConfig::Controls::Controls() : ASerializable() {
}

void MameConfig::Controls::serialize(ASerializer &serializer)
{
    static const vector<string> strPlayers={
        "None",
        "Player 1",
        "Player 2",
        "Player 3",
        "Player 4"
    };
    // these are the bare lowlevel managed enum types of controllers
    static const vector<string> strLLTypes={
        "None", //"Auto Sense",  now ask explicit config.
        "CD32 7bt Pad",
        "Mouse",
        "Joystick(2bt)"
    };
    // this is the same, plus proportional analog controllers not managed by LL.
    // also Amiga Prop.Stick and C64/Atari 8bits paddles are same, but with potentiometer X/Y inverted.
    static const vector<string> strLLTypesPlusProp={
        "None",
        "CD32 7bt Pad",
        "Mouse",
        "Joystick(2bt)",
        "Analog Joystick(2bt)",
        "C64/Atari Paddles(2bt)",
        "Lightgun (2bt)" // only available for one port.
    };


    int hasPots = hasProportionalStickResource(); // some not-classic hardware will not.

    serializer("Mouse Port 1", (int&)_llPort_Player[0],strPlayers);
   //ok, but: serializer("Types P1", (int&)_llPort_Type[0],(hasPots)?strLLTypesPlusProp:strLLTypes);
    serializer("Types P1", (int&)_llPort_Type[0],strLLTypes); // do not enable pots on mouse port for the moment.

    serializer("Joy Port 2", (int&)_llPort_Player[1],strPlayers);
    serializer("Types P2", (int&)_llPort_Type[1],(hasPots)?strLLTypesPlusProp:strLLTypes);

    serializer("Lowlevel Port 3", (int&)_llPort_Player[2],strPlayers);
    serializer("Types P3", (int&)_llPort_Type[2],strLLTypes);

    serializer("Lowlevel Port 4", (int&)_llPort_Player[3],strPlayers);
    serializer("Types P4", (int&)_llPort_Type[3],strLLTypes);

    // - - - -

    if(hasParallelPort())  // some not-classic hardware will not.
    {
        static const vector<string> strPrlTypes={
            "None",
            "Joystick(1or2 bt)",
        };

        serializer("Parallel Port 3", (int&)_parallelPort_Player[0],strPlayers);
        serializer("Types Pr3", (int&)_parallel_type[0],strPrlTypes);
        serializer("Parallel Port 4", (int&)_parallelPort_Player[1],strPlayers);
        serializer("Types Pr4", (int&)_parallel_type[1],strPrlTypes);
    }

    if(hasPots)
    {
        // special options for potentiometers
        serializer("Inverse Axis",_PropJoyAxisReverse,0,{"Joy2X","Joy2Y"});
        serializer.listenChange("Types P2",[](ASerializer &serializer, void *p)
        {
            if(!p) return;
            int *pPort2Type = (int *)p;

            serializer.enable("Controls.Inverse Axis",
                        (*pPort2Type == PORT_TYPE_PROPORTIONALJOYSTICK ||
                         *pPort2Type == PORT_TYPE_C64PADDLE    )?1:0);
        });

        // special options for lightgun
        serializer("Lightgun as",_LightgunPublish,{"Lightgun(no interpolations)","Generic Analog(fit more games)"});
        serializer.listenChange("Types P2",[](ASerializer &serializer, void *p)
        {
            if(!p) return;
            int *pPort2Type = (int *)p;

            serializer.enable("Controls.Lightgun as",
                        (*pPort2Type == PORT_TYPE_LIGHTGUN)?1:0);
        });
    }


}
#ifdef LINK_NEOGEO
 extern const bios_entry *system_bios_neogeo_first;
#endif
// MameConfig::MissingROM::MissingROM() : ASerializable()
//  , _allow(false)
// {
//// _missingRomAction("wget -P %ROMDIR% http://thisisjustanexampletoshowyou.org/%ROM%.zip")
////  wget -P /mnt/iso http://example.com/file.iso
// }
// void MameConfig::MissingROM::serialize(ASerializer &serializer)
// {
//     serializer("Allow",_allow,0);
//     serializer("Action",_missingRomAction,0);
// }


MameConfig::Misc::Misc() : ASerializable()
     , _userPath("PROGDIR:user")
// this can be chanegd at init by args, not by load/save/todefault.
 {
// #define SYSTEM_BIOS_START(name)						static const bios_entry system_bios_##name[] = {
#ifdef LINK_NEOGEO
    const bios_entry *p = system_bios_neogeo_first;
    while(p->_name)
    {
        _neogeoBiosList.push_back(p->_description);
        p++;
    }
#endif

//
}

void MameConfig::Misc::serialize(ASerializer &serializer)
{
    serializer("Roms",_romsPath,SERFLAG_STRING_ISPATH);
    serializer("Samples",_samplesPath,SERFLAG_STRING_ISPATH);
    serializer("Use Cheat Code File",_useCheatCodeFile);
    serializer("Cheat Code File",_cheatFilePath,SERFLAG_STRING_ISFILE);

    // min max step default
    serializer("Speed Limit %",_speedlimit,85.0f,125.0f,5.0f,100.0f);

    serializer("Skip",_skipflags,0,{"Disclaimer","Game Info"});
    serializer("NeoGeo Bios",_neogeo_bios,_neogeoBiosList);

    // serializer("Also...",_MiscFlags,
    //                     0 | SERFLAG_GROUP_FLAGINT2COLUMS // this field both used for default values and UI preference .
    //                 ,{
    //     "CD32 Pads uses AMEGA32 Adapter:\nSwitch 6 buttons to fit SF2."
    //     });

    serializer("Display Controls",_Goodies,
          GOODIESFLAGS_WHEEL|GOODIESFLAGS_GEAR | SERFLAG_GROUP_FLAGINT2COLUMS // this field both used for default values and UI preference .
    ,{
        "Steering Wheel"
        ,"Gear Shift"
      });

    serializer("Optims",_Optims,
          OPTIMFLAGS_DIRECTWGXWIN | SERFLAG_GROUP_FLAGINT2COLUMS //def.
    ,{
        "Direct draw for RTG Windows",
        "Use native BestCModeID()"
      });

    //serializer("Missing ROMs",(ASerializable &)_missingROM);

}
MameConfig::Help::Help() : ASerializable() {
}


void MameConfig::Help::serialize(ASerializer &serializer)
{
    // just use the item capabilities of the gui serializer,
    // to display a short notice.
    // actually nothing to load/save.
_[0] ="       Ingame Keys\n";
    serializer(" ",_[0]);
_[1] ="Player Start 1/2/3/4";
    serializer("1-4 (upper row): ",_[1]);
_[2] ="Player Coins 1/2/3/4";
    serializer("5-8 (upper row): ",_[2]);
_[3] ="Pause";
    serializer("P : ",_[3]);
_[4] ="Show / Hide Mame Settings";
    serializer("Tab : ",_[4]);
_[5] ="Switch Window / Fullscreen";
    serializer("F10 : ",_[5]);

_[6] ="Throttle when kept pressed";
    serializer("Shit+F10 : ",_[6]);

_[7] ="Show / Hide Statistics";
    serializer("Help : ",_[7]);
_[8] ="Save State  ...  F7+Shift: Load State";
    serializer("F7 : ",_[8]);
_[9] ="Reset";
        serializer("F3 : ",_[9]);
_[10] ="Escape Game";
    serializer("Esc : ",_[10]);
// - - - - --
_[11] ="\n- Copy some Mame106 zip archive in 'roms' dir and press scan.";
            serializer(" ",_[11]);
_[12] ="- Don't try to run games tagged 'not working'";
        serializer(" ",_[12]);
_[13] ="- Games with parent archive need their parent,\n   non working games may have a working parent.";
        serializer(" ",_[13]);

}


void MameConfig::init(int argc,char **argv)
{
    try {
        initDriverIndex();
    } catch(const exception &e)
    {
        loginfo(2,"initDriverIndex exception:%s\n",e.what());
    }
    _filter_genre_enums = 0xffffffffffffffffULL;

}
// from some .h
//struct _input_port_init_params
//{
//    input_port_entry *	ports;		/* base of the port array */
//    int					max_ports;	/* maximum number of ports we can support */
//    int					current_port;/* current port index */
//};
extern "C"
{
    int driverGetNbPlayers(const _game_driver *drv);
}
// extern const game_driver * const drivers[];
void MameConfig::getDriverScreenModestringP(const _game_driver *drv, std::string &screenid,int &video_attribs/*,int *nbPlayers*/)
{
    struct _machine_config machine;
    memset(&machine,0,sizeof(machine));
    drv->drv(&machine);
    video_attribs = machine.video_attributes;

    if(video_attribs & VIDEO_TYPE_VECTOR)
    {
        screenid = "vector"; // resolution is computed from vector config.
        if(drv->flags & ORIENTATION_SWAP_XY) screenid +="(V)"; // so we have different configuration for vertical screens.
        return;
    }

    int width = machine.default_visible_area.max_x - machine.default_visible_area.min_x + 1;
    int height = machine.default_visible_area.max_y - machine.default_visible_area.min_y + 1;

    if(drv->flags & ORIENTATION_SWAP_XY) {
        std::swap(width,height);
    }
    std::stringstream ss;
    ss <<width<<"x"<<height<<" ";
    if(machine.video_attributes &VIDEO_RGB_DIRECT) ss<<"15b";
    else if((machine.total_colors)<=256) ss<<"8b"; // +10 for UI colors
    else  ss<<"16b";

    screenid = ss.str();
}

void MameConfig::initDriverIndex()
{
   // printf("initDriverIndex() 1\n");
    // to be done once.
  int NumDrivers;
  loginfo(0,"initDriverIndex()\n");
  for(NumDrivers = 0; drivers[NumDrivers]; NumDrivers++)
  {
    const game_driver *drv  =drivers[NumDrivers];
    if(drv->flags & (/*GAME_NOT_WORKING|*/NOT_A_DRIVER)) continue;
     _driverIndex.insert(drv->name,NumDrivers);
  }
  //  printf("initDriverIndex() 3 . NumDrivers:%d\n",NumDrivers);
  _NumDrivers =NumDrivers;

    // also get its screen id:
    _resolutionStrings.reserve(_NumDrivers);
    _resolutionStrings.resize(_NumDrivers);
    _videoAttribs.reserve(_NumDrivers);
    _videoAttribs.resize(_NumDrivers);
//    _players.reserve(_NumDrivers);
//    _players.resize(_NumDrivers);
  loginfo(0,"initDriverIndex() ...\n");
    for(NumDrivers = 0; drivers[NumDrivers]; NumDrivers++)
    {
        const game_driver *drv  =drivers[NumDrivers];
        if(drv->flags & (/*GAME_NOT_WORKING|*/NOT_A_DRIVER)) continue;
       // int nbp;
        getDriverScreenModestringP(drv,_resolutionStrings[NumDrivers],_videoAttribs[NumDrivers]/*,&nbp*/);
      //  _players[NumDrivers] = (UBYTE)nbp;

    }
  loginfo(0,"initDriverIndex() ok\n");
}
// provide this C tool:
//const game_driver *getDriverByName(const char *pName)
//{
//    int idriver = getMainConfig().driverIndex().index(pName);
//    if(idriver<0) return NULL;
//    return drivers[idriver];
//}

// list all drivers to output.
void MameConfig::listFull()
{
    printf("Archive   Parent    Year Genre Description\n");

    //     std::map<std::string,int> _m;
    unordered_map<std::string,int>::iterator cit =  _driverIndex._m.begin();
    while(cit != _driverIndex._m.end())
    {
        int idrv = cit->second ; *cit++;
        const game_driver *drv  =drivers[idrv];
        if(drv && drv->name && drv->description && drv->year)
        {
            char temp[12]={0};
            int l = strlen(drv->name);
            strncpy(temp,drv->name,8);
            while(l<8){ temp[l]=' '; l++;}
            temp[8]=0;

            char tparent[12]={0};
            int hasparent = (drv->parent && !(drv->parent[0]=='0' && drv->parent[1]==0) );
            l = (hasparent)?strlen(drv->parent):0;
            if(hasparent) strncpy(tparent,drv->parent,8);
            while(l<8){ tparent[l]=' '; l++;}
            tparent[8]=0;
            printf("%s  %s  %s \"%s\"\n",temp,tparent, drv->year, drv->description);
        }
    }


}

//create new cheat file with just data for known drivers.
void MameConfig::filterCheatFile(const char *pcheatf)
{
    ifstream ifs(pcheatf);
    if(!ifs.good()) return;

    ofstream ofs(string(pcheatf)+".new");

    string l;
    while(getline(ifs,l))
    {
        if(l.size()==0) continue;
        char temp[1024];
        size_t i = l.find(":");
        if(i == string::npos) continue;
        size_t j = l.find(":",i+1);
        if(j == string::npos) continue;
        string n=l.substr(i+1,j-i-1);
        if(_driverIndex.index(n.c_str()) != -1)
        {
            ofs << l << "\n";
        }

    }
    ofs << endl;

}
void MameConfig::getDriverScreenModestring(const _game_driver **drv, std::string &screenid,int &video_attribs/*, int &nbp*/)
{
    int idriver = ((int)drv-(int)&drivers[0])/sizeof(const _game_driver *);
    if(idriver<0 || idriver>=_NumDrivers)
    {
        screenid.clear();
        video_attribs = 0;
        return;
    }
    screenid = _resolutionStrings[idriver];
    video_attribs = _videoAttribs[idriver];
//    nbp = (int)_players[idriver];
}
int MameConfig::DriverCompareScreenMode(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
   int idriver1 = ((int)drv1-(int)&drivers[0])/sizeof(const _game_driver *);
   int idriver2 = ((int)drv2-(int)&drivers[0])/sizeof(const _game_driver *);
    if(idriver1<0 || idriver1>=_NumDrivers ||
        idriver2<0 || idriver2>=_NumDrivers
    )return 0;
    string &s1 = _resolutionStrings[idriver1];
    string &s2 = _resolutionStrings[idriver2];
    /* works but would take 1 and 1024 close.
    return strcmp(s1.c_str(),s2.c_str());
    */
    // let's sort with: vectors are last
    if(s1[0]=='v' || s2[0]=='v') {
      return (int)s2[0] - (int)s1[0];
    }
    // then shape is 640x480 nb
    int width1 = atoi(s1.c_str());
    int width2 = atoi(s2.c_str());
    int i = width2-width1;
    if(i!=0) return i;
    // else keep alphabetic
    return(stricmp((*drv1)->description, (*drv2)->description));
}


int MameConfig::scanDrivers()
{
//  printf(" *** ScanDrivers: _romsDir:%s\n", _paths._romsPath.c_str());
  _romsFound.clear();
  if(_misc._romsPath.empty()) return 0;
//  printf(" *** ScanDrivers 1\n");

    struct FileInfoBlock *fib;
    fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
    if(!fib) return 0;

    BPTR lock = Lock( _misc._romsPath.c_str(), ACCESS_READ);
    if(lock)
    {
        scanDriversRecurse(lock,fib);
        UnLock(lock);
    }

    FreeDosObject(DOS_FIB,fib);

    //sortDrivers(_romsFound);
    _romsFoundTouched = true;
//    printf(" *** ScanDrivers end\n");
    initRomsFoundReverse();

    updateRomFilters();
    return (int)_romsFound.size();
}
int MameConfig::scanDriversRecurse(BPTR lock, FileInfoBlock*fib)
{
    if(!Examine(lock, fib)) return 0;

    if(fib->fib_DirEntryType <= 0) return 0; // if >0, a directory

    while(ExNext(lock, fib))
    {
        // trick: force lowercase at this level
        int i=0;
        char c;
        while((c=fib->fib_FileName[i])!=0) {
            if(c>='A' && c<='Z') c=fib->fib_FileName[i]+= 32;
            i++;
        }
        if(fib->fib_DirEntryType > 0) // if >0, a directory
        { // sub is a dir.
            // could be unzip roms or a subdir
           int idriver = _driverIndex.index(fib->fib_FileName);
           if(idriver >= 0)
           {
                _romsFound.push_back(&drivers[idriver]);
           }
           else
           {    // subdir ?
                //TODO or not.
           }
        } else
        {   // is a file.
            // if end with zip
            // fast, no alloc version
            char *p = fib->fib_FileName;
            int l =strlen(p);
            if(l>4 && p[l-4]=='.' && p[l-3]=='z' && p[l-2]=='i' && p[l-1]=='p')
            {
                p[l-4] = 0;
                int idriver = _driverIndex.index(p);
                if(idriver >= 0)
                {
                    _romsFound.push_back(&drivers[idriver]);
                }
            }
        } // end if is file.

    } // end loop per dir file

}

/* now done by mui list
void MameConfig::sortDrivers( std::vector<const _game_driver *const*> &roms)
{
    if(roms.size()==0) return;

    int (* comparator)(const void *, const void *) =
            (int (*)(const void *, const void *))
            DriverCompareYear;
            //DriverCompareNames;

    switch(_sortMode)
    {
        case SortMode::Name:
        break;
        case SortMode::NbPlayers:
        comparator = (int (*)(const void *, const void *))DriverCompareNbPlayers;
        break;
        case SortMode::Year:
        comparator =(int (*)(const void *, const void *)) DriverCompareYear;
        break;
        case SortMode::Archive:
        comparator = (int (*)(const void *, const void *))DriverCompareArchive;
        break;
        case SortMode::Parent:
        comparator = (int (*)(const void *, const void *))DriverCompareParent;
        break;
    default:break;

    }

    qsort(roms.data(), //&SortedDrivers[DRIVER_OFFSET],
        (int)roms.size() ,//NumDrivers,
         sizeof(struct _game_driver **),
           comparator);

}
*/
void MameConfig::initRomsFoundReverse()
{
    int nbSlots = (_NumDrivers>>3)+1;
    _romsFoundReverse.reserve(nbSlots);
    _romsFoundReverse.resize(nbSlots,0);
    for(const _game_driver *const*drv : _romsFound)
    {
        int idriver = ((int)drv-(int)&drivers[0])/sizeof(const _game_driver *);
        _romsFoundReverse[idriver>>3] |= (1<<(idriver & 7));
    }
}
int MameConfig::isDriverFound(const _game_driver *const*drv)
{
    int idriver = ((int)drv-(int)&drivers[0])/sizeof(const _game_driver *);
    if(idriver<=0 || idriver >=_NumDrivers) return 0;
    return (int)((_romsFoundReverse[idriver>>3] & (1<<(idriver & 7))) !=0);
}

void MameConfig::buildAllRomsVector(std::vector<const _game_driver *const*> &v)
{
    v.reserve(_NumDrivers);
    v.resize(_NumDrivers);
    for(int NumDrivers = 0; drivers[NumDrivers]; NumDrivers++)
    {
        v[NumDrivers] = &drivers[NumDrivers];
    }
// now done by mui list    sortDrivers(v);
}
const vector<const _game_driver *const*> &MameConfig::roms() const
{
//    updateRomFilters();
/*
   int     _listShowState;
    UBYTE   _genreEnumSet[60];
    UWORD   _ored_genre_tags;
    UWORD   _usefilter; // optim. If all enums
*/
    const unsigned long long enumsmask = (1ULL<<45)-1;
    int   usefilter=1;
    if((_filter_genre_enums & enumsmask)==enumsmask) usefilter=0;

    if(!usefilter && _listShowState)
    {
        return _romsFound;
    }
    return _romsFiltered;

}
int MameConfig::usesListFilter() const
{
   const unsigned long long enumsmask = (1ULL<<45)-1;
    int   usefilter=1;
    if((_filter_genre_enums & enumsmask)==enumsmask) usefilter=0;
    return usefilter;

}
void MameConfig::updateRomFilters()
{
   _romsFiltered.clear();
//#define CFGS_ALL 0
//#define CFGS_FOUND 1
// extern const game_driver * const drivers[];
    if(_listShowState) {
        // found
        for(int i = 0; i<(int)_romsFound.size() ; i++)
        {
            const game_driver *drv  = *_romsFound[i];
            if(drv->flags & (NOT_A_DRIVER)) continue;
            if(((_filter_genre_enums & (1ULL<<drv->genre))!=0)||
                ((drv->genreflag & _ored_genre_tags)!=0))
                _romsFiltered.push_back(_romsFound[i]);
        }

    } else {
        // all
        for(int i = 0; i<_NumDrivers ; i++)
        {
            const game_driver *drv =drivers[i];
            if(drv->flags & (NOT_A_DRIVER)) continue;  
            if(((_filter_genre_enums & (1ULL<<drv->genre))!=0)||
                ((drv->genreflag & _ored_genre_tags)!=0))
                 _romsFiltered.push_back(&drivers[i]);
        }

    }


/*
        // found
        originlist = _romsFound.data();
        nbOriginDrivers = (int)_romsFound.size();
    } else {
        // all
        originlist = drivers;
        nbOriginDrivers = _NumDrivers;
    }
*/

}

// from cheat.c
extern "C" {
    extern const char	* cheatfile;
}
// apply to mame options
void MameConfig::applyToMameOptions(_global_options &mameOptions,const game_driver *drv)
{
    memset(&mameOptions, 0,sizeof(_global_options));

    options.cheat = (int)( _misc._useCheatCodeFile && _misc._cheatFilePath.length()>0 );
    if(options.cheat)
    {
        cheatfile = _misc._cheatFilePath.c_str();
    } else
    {
       cheatfile = NULL;
    }
    options.gui_host=1;

    options.pause_bright = _display._color_brightness * 0.5f;
    options.brightness =   _display._color_brightness;
  //old  options.gamma= _display._color_gamma;

    options.samplerate=(_audio._mode == AudioMode::None)?0:_audio._freq;
    options.use_samples = 1; // ((_audio._Flags & 1) != 0); // really, ...

    options.skip_disclaimer = (_misc._skipflags & 1) != 0;
    options.skip_gameinfo =
    options.skip_warnings = (_misc._skipflags & 2) != 0;

    // vector things
    Display_Vector &vectorconf = _display._vector;

    // value is pixel line width <<16.
    switch(vectorconf._resolution)
    {
        case VectorResolution::e320x240:
            options.beam = 0x00012000;
            options.vector_width = 320;
            options.vector_height = 240;
            break;
        case VectorResolution::e400x300:
            options.beam = 0x00012000;
            options.vector_width = 400;
            options.vector_height = 300;
            break;
        default:
        case VectorResolution::e480x360:
            options.beam = 0x00014000;
            options.vector_width = 480;
            options.vector_height = 360;
            break;
        case VectorResolution::e640x480:
            options.beam = 0x00020000;
            options.vector_width = 640;
            options.vector_height = 480;
            break;
    }
    if(options.beam<(1<<16)) options.beam = 1<<16; // or crash !

    options.vector_flicker = 0.0f;     /* float vector beam flicker effect control */
    options.vector_intensity = vectorconf._intensity;  /* float vector beam intensity 1.5f defaulty */
    options.translucency = 1;  /* 1 to enable translucency on vectors */
    options.antialias = (int)((vectorconf._flags & VDISPLAYFLAGS_ANTIALIAS)!=0);  /* 1 to enable antialias on vectors */
    options.vector_remanence = (int)vectorconf._remanence;
    options.vector_glow = (int)vectorconf._glow;
    options.vector_force32b = (int)((vectorconf._flags & VDISPLAYFLAGS_FORCE32B)!=0);

#ifdef LINK_NEOGEO
    // if machine points neogeo rom list, then it's neogeo.

    if(drv->bios == system_bios_neogeo_first &&
       _misc._neogeo_bios >0 &&  _misc._neogeo_bios<=12)
    {
        // stupids want a casted name.
        static char t[4];
        snprintf(t,3,"%d",_misc._neogeo_bios);
        options.bios = t;

        //printf("set bios:%s\n",t);
    }
#endif

}

extern "C"
{
unsigned int GetDisplayGoodiesFlags()
{
   MameConfig &c = getMainConfig();
   return (unsigned int )c.misc()._Goodies;
}
}

