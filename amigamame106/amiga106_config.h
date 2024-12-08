#ifndef AMIGA_MAME_CONFIG_H
#define AMIGA_MAME_CONFIG_H
extern "C"
{
    #include <exec/types.h>
}
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

#include "serializer.h"

typedef long BPTR;
struct FileInfoBlock;
struct _game_driver;
struct _global_options;
struct _game_driver;
typedef struct _game_driver game_driver;
// driver name list could actually get big, avoid looping it.
class NameDriverMap {
public:
/*old , looks ok but problems with os4 ?
    void insert(const char *n, int mamedriverindex) {
        using namespace std;
        if(!n || *n==0) return;
        _m[*n][string(n)]=mamedriverindex;
    }
    // tell if driver known, and return index on mame driver list.
    int index(const char *n) const {
        using namespace std;
        if(!n || *n==0) return -1;
         unordered_map<char,unordered_map<string,int>>::const_iterator cit = _m.find(*n);
        if(cit==_m.end()) return -1;
        const std::unordered_map<std::string,int> &mm = cit->second;
        unordered_map<string,int>::const_iterator cit2 = mm.find(string(n));
        if(cit2==mm.end()) return -1;
        return cit2->second;
    }

    std::unordered_map<char,std::unordered_map<std::string,int>> _m;
    */
    void insert(const char *n, int mamedriverindex) {
        using namespace std;
        if(!n || *n==0) return;
         string sn(n);
        _m[sn]=mamedriverindex;
    }
    // tell if driver known, and return index on mame driver list.
    int index(const char *n) const {
        using namespace std;
        if(!n || *n==0) return -1;
        string sn(n);
        unordered_map<string,int>::const_iterator cit2 = _m.find(sn);
        if(cit2==_m.end()) return -1;
        return cit2->second;
    }

    std::unordered_map<std::string,int> _m;
};


/** Main configuration.
 *  Mame106 core manage itself default and per driver configuration.
 *  We just manage here:
 *  - rom dir pref , - user file prefs
 *  - scanned rom found list.
 *  Then modeid and per game prefs should be patched in mame per driver conf.
 */
class MameConfig : public ASerializable {
public:
    MameConfig();
    ~MameConfig();

    void init(int argc,char **argv);
    // list all drivers to output.
    void listFull();
    //create new cheat file with just data for known drivers.
    void filterCheatFile(const char *pcheatf);

    void resettodefault();
    void setRomPath(const char *rompath);
    void setUserPath(const char *userpath);
    // set when driver selected,
    void setActiveDriver(int indexInDriverList);
    void setDriverListState(int listState);

    int save();
    int load();

    int activeDriver() const { return _activeDriver; }
    int driverListstate() const {return _listShowState; }

    // - -  update detected rom list - - -
    int scanDrivers();
    //int allDrivers();
    // should be done very soon at start.
    void setUserDir(const char *pPath) { _misc._userPath = (pPath)?pPath:""; }
    // - - path to main dirs --

    const char *getUserDir() const {return _misc._userPath.c_str(); }
    const char *getRomsDir() const {return _misc._romsPath.c_str(); }
    const char *getSamplesDir() const {return _misc._samplesPath.c_str(); }

    const std::vector<const _game_driver *const*> &romsFound() const { return _romsFound; };
    // do not keep that table...
    void buildAllRomsVector(std::vector<const _game_driver *const*> &v);

    const NameDriverMap  &driverIndex() const { return _driverIndex; };

    // to display in bold when found.
    int isDriverFound(const _game_driver *const*drv);

    // apply to mame options
    void applyToMameOptions(_global_options &mameOptions,const game_driver *drv);

    // have an optimized version
    void getDriverScreenModestring(const _game_driver **drv, std::string &screenid,int &video_attribs/*, int &nbp*/);
    static void getDriverScreenModestringP(const _game_driver *drv, std::string &screenid,int &video_attribs/*, int *nbPlayers=NULL*/);

    void serialize(ASerializer &serializer) override;

    enum class DrawEngine :  int
    {
        CgxDirectCpuOrWPA8,
        CgxScalePixelArray,
       // WritePixelArray8,
       // GLShader :)
    };
    enum class FSScaleMode :  int
    {
        CenterWithNoScale,
        ScaleToBorder,
        ScaleToStretch
    };
    enum class RotateMode :  int
    {
        Rot0,
        Rot90,
        Rot180,
        Rot270
    };
    enum class ScreenModeChoice :  int
    {
        Best,
        Choose
    };
    enum class ScreenBufferMode :  int
    {
        Single,
        TripleBufferCSB,
        DoubleBufferSVP,
    };
    struct Display_PerScreenMode : public ASerializable
    {
        Display_PerScreenMode();
        void serialize(ASerializer &serializer) override;
        void valueUpdated(std::string upatedValue) override;
        bool isDefault() override;
        RotateMode _rotateMode = RotateMode::Rot0;
        ScreenModeChoice _ScreenModeChoice=ScreenModeChoice::Best;
        FSScaleMode _FSscaleMode = FSScaleMode::CenterWithNoScale;
        ULONG_SCREENMODEID _modeid=~0;
        // these one are saved but ot on ui.
        int _window_posx,_window_posy,_window_width,_window_height,_window_validpos=0;
    };
    struct Display : public ASerializable
    {
        Display();
        void serialize(ASerializer &serializer) override;
        DrawEngine _drawEngine = DrawEngine::CgxDirectCpuOrWPA8;        
#define CONFDISPLAYFLAGS_ONWORKBENCH 1
#define CONFDISPLAYFLAGS_FRAMESKIP 2
//#define CONFDISPLAYFLAGS_TRIPLEBUFFER 4
        ULONG_FLAGS _flags = 0;
        ScreenBufferMode  _buffering = ScreenBufferMode::Single;
        //bool    _startOnWorkbench = false;
        Display_PerScreenMode &getActiveMode();
     protected:
        std::map<std::string,Display_PerScreenMode> _perScreenMode;
        ASerializer::StringMap<Display_PerScreenMode> _perScreenModeS;
        float _color_brightness=1.0f;
       // old useless float _color_gamma=1.0f;
        friend class MameConfig;
    };
    Display &display() { return _display; }



    enum class AudioMode :  int
    {
        None,
        AHI
    };
    struct Audio : public ASerializable
    {
        Audio();
        void serialize(ASerializer &serializer) override;
        AudioMode _mode = AudioMode::AHI;
        int _freq=0;
        bool _forceMono=true;
        // ULONG_FLAGS  _Flags = 1; // 1 use samples
    };
    Audio &audio() { return _audio; }


    enum class ControlPortPrl :  int
    {
        None,
        Para3,
        Para4,
        Para3Bt4
    };


    struct Controls : public ASerializable
    {
        Controls();
        void serialize(ASerializer &serializer) override;
        int _llPort_Player[4]; // value 1-4
        int _llPort_Type[4];
        int _parallelPort_Player[2]; // value 1-4
        int _parallel_type[2];

        //bool _useDirectKeyboard = true;

        strText _ll,_pr;

        //ControlPortLL _PlayerPort[4];
        //int _PlayerPortType[4];
    };
    Controls &controls() { return _controls; }

    struct Misc : public ASerializable
    {
        Misc();
        void serialize(ASerializer &serializer) override;        
        std::string _romsPath,_samplesPath,_userPath;
        bool    _useCheatCodeFile = false;
        std::string _cheatFilePath="PROGDIR:cheat.dat";
        float     _speedlimit = 100.0f;
        ULONG_FLAGS  _skipflags = 0;       
        int         _neogeo_bios = 0;

        std::vector<std::string> _neogeoBiosList;
    };
    Misc &misc() { return _misc; }

    // just to have a help panel
    struct Help : public ASerializable
    {
        Help();
        void serialize(ASerializer &serializer) override;
        strText _[10+4];
    };

    void toDefault();

    enum class SortMode :  int
    {
        Name,
        NbPlayers,
        Year,
        Archive,
        Parent
    };

    SortMode     sortMode() { return _sortMode; }

protected:
    int _NumDrivers; // in current linker mame driver list. Can be huge.

    // - - - configuration as serialized paragraphs.
    Display     _display;
    Audio       _audio;
    Controls    _controls;
    Misc       _misc;
    Help        _help;

    int     _activeDriver;

    int     _listShowState;


    SortMode     _sortMode;
//    ScreenConf _defaultscreenconf;
//    // TODO video prefs, per video config
//    // keys are like: "320x224x16"
//    std::unordered_map<std::string,ScreenConf> _screenconfs;

    // name to current mame index to fasten dir search
    NameDriverMap _driverIndex;


    // - - - - - scanned roms zip or dir for UI.
    // mui like a ptr to ptr list, to insert in one blow.
    // this is meant to be sorted a way or another    
    std::vector<const _game_driver *const*> _romsFound;
    std::vector<UBYTE> _romsFoundReverse;
    bool            _romsFoundTouched; // should save or not on exit.
    void initDriverIndex();
    int scanDriversRecurse(BPTR lock, FileInfoBlock*fib);

    std::vector<std::string> _resolutionStrings;
    std::vector<int> _videoAttribs;
    //std::vector<UBYTE> _players;

    //void sortDrivers( std::vector<const _game_driver *const*> &romsFound);
    void initRomsFoundReverse();
};

// access to singleton
MameConfig &getMainConfig();


#endif
