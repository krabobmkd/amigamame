#include "amiga106_config.h"

#include <sstream>
#include <algorithm>
#include <proto/exec.h>
#include <proto/dos.h>

// from mame
extern "C" {
    #include "driver.h"
    // use xml from mame
    #include "xmlfile.h"

    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
}

#include "serializer_mamexml.h"

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

MameConfig::MameConfig()
    : _NumDrivers(0)
    , _activeDriver(-1)
    , _listShowState(0)
{
    initDriverIndex();
}
MameConfig::~MameConfig()
{}

void MameConfig::setActiveDriver(int indexInDriverList)
{
    if(indexInDriverList<0 || indexInDriverList>=_NumDrivers)
    {
        _activeDriver = -1;
        return;
    }

    _activeDriver = indexInDriverList;

}
void MameConfig::setDriverListState(int listState)
{
    _listShowState = listState;
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
    printf("MameConfig::save\n");

    xml_data_node *root = xml_file_create();
    xml_data_node *confignode;
    mame_file *file=NULL;
    xml_data_node *display;

    /* if we don't have a root, bail */
    if (!root)
        return 0;

    file = mame_fopen("main", 0, FILETYPE_CONFIG, 1);
    if(!file) {
        xml_file_free(root);
        return 0;
    }
    /* create a config node */
    confignode = xml_add_child(root,pcd_mame, NULL);

    xml_set_attribute_int(confignode, "version", 1);

    // save known rom list
    if(_romsFound.size()>0)
    {
        stringstream ssroms;
        int i=0;
        for(const _game_driver *const*d : _romsFound)
        {
            char sep=' ';
            if(i==8) {i=0; sep='\n';}
            ssroms << string((*d)->name) << sep;
            i++;
        }
        string romslist = ssroms.str();
        xml_add_child(confignode,pcf_roms, romslist.c_str());
    }


    if(_activeDriver !=-1)
    {
        xml_add_child(confignode,pcf_last, drivers[_activeDriver]->name );
    }
    if(_listShowState !=-1)
    {
        xml_data_node *pn = xml_add_child(confignode,pcf_list,NULL);
        if(pn) xml_set_attribute_int(pn,"show",_listShowState);
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

}

int MameConfig::load()
{
    xml_data_node *root=NULL,*confignode,*node; //, *confignode, *systemnode;
	const char *srcfile;
	int version, count;
	mame_file *file=NULL;
    printf("MameConfig::load\n");
    // resolve short name to index after load, like scan does.

    resettodefault();

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
        initRomsFoundReverse();
    }

    node = xml_get_sibling(confignode->child, pcf_last);
    if(node && node->value) _activeDriver = _driverIndex.index(node->value);

    node = xml_get_sibling(confignode->child, pcf_list);

    _listShowState = 0;
    if(node) _listShowState = xml_get_attribute_int(node,"show",0);

    // - - - automatise serialized members, same way as write:
    {
        XmlReader xmlreader(confignode);
        serialize(xmlreader);
    }

    xml_file_free(root);
	mame_fclose(file);
	return 1;
error:
    if(root) xml_file_free(root);
    if(file) mame_fclose(file);
    return 0;
}
void MameConfig::serialize(ASerializer &serializer)
{
    // defines what is loaded/saved/gui edited.
    serializer("Display",   (ASerializable&)_display,0);
    serializer("Audio",     (ASerializable&)_audio,0);
    serializer("Controls",  (ASerializable&)_controls, SERFLAG_GROUP_2COLUMS);
    serializer("Paths",     (ASerializable&)_paths,0);
    serializer("Help",     (ASerializable&)_help,0);
}
void MameConfig::toDefault()
{
    _display._drawEngine = DrawEngine::CgxDirectCpuOrWPA8;
    _display._perScreenMode.clear();

    _audio._mode = AudioMode::AHI;
    _audio._freq = 22050;

    _controls._PlayerPort[0]=ControlPort::Port2llJoy;
        _controls._PlayerPortType[0]=1;
    _controls._PlayerPort[1]=ControlPort::None; _controls._PlayerPortType[1]=0;
    _controls._PlayerPort[2]=ControlPort::None; _controls._PlayerPortType[2]=0;
    _controls._PlayerPort[3]=ControlPort::None; _controls._PlayerPortType[3]=0;

    _paths._romsPath = "PROGDIR:roms";
    _paths._userPath = "PROGDIR:user";

}

void MameConfig::Display_PerScreenMode::serialize(ASerializer &serializer)
{
    serializer("Screen Selection",(int &)_ScreenModeChoice,{"Find Best","Select Mode"});
    serializer("Screen mode",_modeid);
    serializer("Full Screen",(int &)_FSscaleMode,{"Center, No Scale","Scale To Border","Stretch Scale"});
    serializer("Rotate Screens",(int &)_rotateMode,{"None","+90","180","-90"});

    // want modeid greyed if screen not choosen
    serializer.listenChange("Screen Selection",[](ASerializer &serializer, void *p)
    {
        ScreenModeChoice *pScreenModeChoice = (ScreenModeChoice *)p;
        serializer.enable("Display.Per Screen Mode.Screen mode",(*pScreenModeChoice == ScreenModeChoice::Choose)?1:0);
    });
}
void MameConfig::Display_PerScreenMode::valueUpdated(std::string upatedValue)
{
    if(_ScreenModeChoice == ScreenModeChoice::Best) _modeid = INVALID_ID;

}
bool MameConfig::Display_PerScreenMode::isDefault()
{   // will not be written if is default.
    return (_ScreenModeChoice == ScreenModeChoice::Best &&
            _rotateMode == RotateMode::Rot0);
}

MameConfig::Display::Display() : ASerializable() ,_perScreenModeS(_perScreenMode)
{}
void MameConfig::Display::serialize(ASerializer &serializer)
{
    serializer("Draw Engine",(int &)_drawEngine,{"CGX Direct CPU Or WPA8","( More To Come )"});

    serializer("Per Screen Mode",_perScreenModeS);

}
void MameConfig::Audio::serialize(ASerializer &serializer)
{
    serializer("Mode",(int &)_mode,{"None","AHI"});
    serializer("Frequency",_freq,8000,22050);

}
void MameConfig::Controls::serialize(ASerializer &serializer)
{


    vector<string> ports={
        "None",
        "Port 1(Mouse)",
        "Port 2(Joy)",
        "Port 3 Lowlevel.lib",
        "Port 4 Lowlevel.lib",
        "Parallel Port3(1bt)",
        "Parallel Port4(1bt)",
        "Parallel Port3(2bt)"
    };
//#define SJA_TYPE_AUTOSENSE 0
//#define SJA_TYPE_GAMECTLR  1
//#define SJA_TYPE_MOUSE	   2
//#define SJA_TYPE_JOYSTK    3
    // lowlevel
    vector<string> controlerTypesLL={
        "Auto Sense",
        "CD32 7bt Pad",
        "Mouse",
        "Joystick(2bt)",
    };
    serializer("Player1", (int&)_PlayerPort[0],ports);
    serializer("Type1", _PlayerPortType[0],controlerTypesLL);
    serializer("Player2",  (int&)_PlayerPort[1],ports);
    serializer("Type2", _PlayerPortType[1],controlerTypesLL);
    serializer("Player3", (int&) _PlayerPort[2],ports);
    serializer("Type3", _PlayerPortType[2],controlerTypesLL);
    serializer("Player4", (int&) _PlayerPort[3],ports);
    serializer("Type4", _PlayerPortType[3],controlerTypesLL);

//    strcomment c(
//"Port 1->4 are managed by lowlevel.library\n"
//"SMS 2 buttons pads & CD32 7bt can be used\n"
//"on Amiga port 1&2,\n"
//"Parallel port3&4 extension only have 1 bt\n"
//"Poseidon USB can patch 4 USB pads on lowlevel\n"
//                );
//    serializer((const char *)NULL,c);

}
void MameConfig::Paths::serialize(ASerializer &serializer)
{
    serializer("Roms",_romsPath,SERFLAG_STRING_ISPATH);
}


void MameConfig::Help::serialize(ASerializer &serializer)
{
    // just use the item capabilities of the gui serializer,
    // to display a short notice.
    // actually nothing to load/save.
_[0] ="       Ingame Keys";
    serializer(" ",_[0]);
_[1] ="Player Start 1/2/3/4";
    serializer("1-4 : ",_[1]);
_[2] ="Player Coins 1/2/3/4";
    serializer("5-8 : ",_[2]);
_[3] ="Pause";
    serializer("P : ",_[3]);
_[4] ="Show/Hide Settings";
    serializer("Tab : ",_[4]);
_[5] ="Switch Window/Fullscreen";
    serializer("F10 : ",_[5]);
_[6] ="Show/Hide Statistics";
    serializer("Help : ",_[6]);
_[7] ="Save State +Shift: Load";
    serializer("F7 : ",_[7]);
_[8] ="Reset";
        serializer("F3 : ",_[8]);
_[9] ="Escape Game";
    serializer("Esc : ",_[9]);
}


void MameConfig::init(int argc,char **argv)
{

}

// extern const game_driver * const drivers[];
void MameConfig::getDriverScreenModestring(const _game_driver *drv, std::string &screenid,int &video_attribs)
{
    struct _machine_config machine;
    drv->drv(&machine);
    video_attribs = machine.video_attributes;

    int width = (machine.default_visible_area.max_x - machine.default_visible_area.min_x)+1;
    int height = (machine.default_visible_area.max_y - machine.default_visible_area.min_y)+1;
    if(drv->flags & ORIENTATION_SWAP_XY) {
        std::swap(width,height);
    }

  std::stringstream ss;
  ss <<width<<"x"<<height<<" ";
  if(machine.video_attributes &VIDEO_RGB_DIRECT) ss<<"15b";
  else if(machine.total_colors<=256) ss<<"8b";
  else  ss<<"16b";

    screenid = ss.str();
}

int MameConfig::initDriverIndex()
{
    // to be done once.
  int NumDrivers;

  for(NumDrivers = 0; drivers[NumDrivers]; NumDrivers++)
  {
    const game_driver *drv  =drivers[NumDrivers];
    if(drv->flags & (/*GAME_NOT_WORKING|*/NOT_A_DRIVER)) continue;
     _driverIndex.insert(drv->name,NumDrivers);
     // also get its screen id:
//    std::string screenmodeId;
//    int vidattribs;
//    getDriverScreenModestring(drv,screenmodeId,vidattribs);

  }
  _NumDrivers =NumDrivers;
}

int MameConfig::scanDrivers()
{
  printf(" *** ScanDrivers: _romsDir:%s\n", _paths._romsPath.c_str());
  _romsFound.clear();
  if(_paths._romsPath.empty()) return 0;
  printf(" *** ScanDrivers 1\n");

    struct FileInfoBlock *fib;
    fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
    if(!fib) return 0;

    BPTR lock = Lock( _paths._romsPath.c_str(), ACCESS_READ);
    if(lock)
    {
        scanDriversRecurse(lock,fib);
        UnLock(lock);
    }

    FreeDosObject(DOS_FIB,fib);

    sortDrivers(_romsFound);
    printf(" *** ScanDrivers end\n");
    initRomsFoundReverse();
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
static int DriverCompareNames(struct _game_driver ***drv1, struct _game_driver ***drv2)
{
  return(stricmp((**drv1)->description, (**drv2)->description));
}


void MameConfig::sortDrivers( std::vector<const _game_driver *const*> &roms)
{
    if(roms.size()==0) return;

    qsort(roms.data(), //&SortedDrivers[DRIVER_OFFSET],
        (int)roms.size() ,//NumDrivers,
         sizeof(struct _game_driver **),
          (int (*)(const void *, const void *)) DriverCompareNames);

}
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
    sortDrivers(v);
}

// apply to mame options
void MameConfig::applyToMameOptions(_global_options &mameOptions)
{
    memset(&mameOptions, 0,sizeof(_global_options));

    options.cheat=1;
    options.gui_host=1;

    options.pause_bright = 0.5f;
    options.brightness = 1.0f;
    options.gamma=0.5f;

    //Re options.samplerate=(audio())?0:sampleRate();
    options.samplerate = 0;
    options.use_samples = 0;

    // ui_orientation

   //  printf("MameConfig::applyToMameOptions applied samplerate:%d\n",options.samplerate);

//todo/old...
    //   options.ror        = (Config[CFG_ROTATION] == CFGR_RIGHT);
    //   options.rol        = (Config[CFG_ROTATION] == CFGR_RIGHT);
    //   options.flipx      = Config[CFG_FLIPX];
    //   options.flipy      = Config[CFG_FLIPY];

//    if(Config[CFG_SOUND] == CFGS_NO)
//      options.samplerate  = 0;
//    else
//      options.samplerate  = 22000;
}

