#ifndef AMIGA_MAME_CONFIG_H
#define AMIGA_MAME_CONFIG_H
extern "C"
{
    #include <exec/types.h>
}
#include <string>
#include <unordered_map>
#include <vector>

#include "serializer.h"

typedef long BPTR;
struct FileInfoBlock;
struct _game_driver;
struct _global_options;

// driver name list could actually get big, avoid looping it.
class NameDriverMap {
public:
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
};
/*
struct ScreenConf {
     ULONG _modeID;
};
*/

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
    // - - path to main dirs --
    const char *getUserDir() const {return _paths._userPath.c_str(); }
    const char *getRomsDir() const {return _paths._romsPath.c_str(); }

    const std::vector<const _game_driver *const*> &romsFound() const { return _romsFound; };
    // do not keep that table...
    void buildAllRomsVector(std::vector<const _game_driver *const*> &v);

    const NameDriverMap  &driverIndex() const { return _driverIndex; };

    // to display in bold when found.
    int isDriverFound(const _game_driver *const*drv);

    // apply to mame options
    void applyToMameOptions(_global_options &mameOptions);

    // have an optimized version
    void getDriverScreenModestring(const _game_driver **drv, std::string &screenid,int &video_attribs);
    static void getDriverScreenModestringP(const _game_driver *drv, std::string &screenid,int &video_attribs);

    void serialize(ASerializer &serializer) override;

    enum class DrawEngine :  int
    {
        CgxDirectCpuOrWPA8,
        MoreToCome
       // CgxScalePixelArray,
       // WritePixelArray8,
       // GLShader
        // "CGX Direct CPU","CGX ScalePixelArray","WritePixelArray8","Some GL Shader Would be great"});
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
    struct Display_PerScreenMode : public ASerializable
    {
        void serialize(ASerializer &serializer) override;
        void valueUpdated(std::string upatedValue) override;
        bool isDefault() override;
        RotateMode _rotateMode = RotateMode::Rot0;
        ScreenModeChoice _ScreenModeChoice=ScreenModeChoice::Best;
        FSScaleMode _FSscaleMode = FSScaleMode::CenterWithNoScale;
        ULONG_SCREENMODEID _modeid=~0;
    };
    struct Display : public ASerializable
    {
        Display();
        void serialize(ASerializer &serializer) override;
        DrawEngine _drawEngine = DrawEngine::CgxDirectCpuOrWPA8;
        std::map<std::string,Display_PerScreenMode> _perScreenMode;
        ASerializer::StringMap<Display_PerScreenMode> _perScreenModeS;
    };
    Display &display() { return _display; }



    enum class AudioMode :  int
    {
        None,
        AHI
    };
    struct Audio : public ASerializable
    {
        void serialize(ASerializer &serializer) override;
        AudioMode _mode = AudioMode::AHI;
        int _freq=0;
    };
    Audio &audio() { return _audio; }

    enum class ControlPort :  int
    {
        None,
        Port1llMouse,
        Port2llJoy,
        Port3ll,
        Port4ll,
        Para3,
        Para4,
        Para3Bt4
    };

    struct Controls : public ASerializable
    {
        void serialize(ASerializer &serializer) override;
        ControlPort _PlayerPort[4];
        int _PlayerPortType[4];
    };
    Controls &controls() { return _controls; }

    struct Paths : public ASerializable
    {
        void serialize(ASerializer &serializer) override;        
        std::string _romsPath,_userPath;
    };
    Paths &paths() { return _paths; }

    // just to have a help panel
    struct Help : public ASerializable
    {
        void serialize(ASerializer &serializer) override;
        strText _[10];
    };

    void toDefault();

protected:
    int _NumDrivers; // in current linker mame driver list. Can be huge.

    // - - - configuration as serialized paragraphs.
    Display     _display;
    Audio       _audio;
    Controls    _controls;
    Paths       _paths;
    Help        _help;

    int     _activeDriver;
    int     _listShowState;

//    ScreenConf _defaultscreenconf;
//    // TODO video prefs, per video config
//    // keys are like: "320x224x16"
//    std::unordered_map<std::string,ScreenConf> _screenconfs;

    // name to current mame index to fasten dir search
    NameDriverMap _driverIndex;


    // - - - - - scanned roms zip or dir for UI.
    //std::vector<MameRomFound> _romsFound; // what to save
    // mui like a ptr to ptr list, to insert in one blow.
    // this is meant to be sorted a way or another    
    std::vector<const _game_driver *const*> _romsFound;
    std::vector<UBYTE> _romsFoundReverse;
    int initDriverIndex();
    int scanDriversRecurse(BPTR lock, FileInfoBlock*fib);
    std::vector<std::string> _resolutionStrings;
    std::vector<int> _videoAttribs;
    void sortDrivers( std::vector<const _game_driver *const*> &romsFound);
    void initRomsFoundReverse();
};

// access to singleton
MameConfig &getMainConfig();


#endif
