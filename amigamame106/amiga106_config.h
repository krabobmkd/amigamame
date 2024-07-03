#ifndef AMIGA_MAME_CONFIG_H
#define AMIGA_MAME_CONFIG_H
extern "C"
{
    #include <exec/types.h>
}
#include <string>
#include <unordered_map>
#include <vector>

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

struct ScreenConf {
     ULONG _modeID;
};

/** Main configuration.
 *  Mame106 core manage itself default and per driver configuration.
 *  We just manage here:
 *  - rom dir pref , - user file prefs
 *  - scanned rom found list.
 *  Then modeid and per game prefs should be patched in mame per driver conf.
 */
class MameConfig {
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
    ULONG audio() const { return _audio; }
    ULONG sampleRate() const { return _sampleRate; }
    // - -  update detected rom list - - -
    int scanDrivers();
    //int allDrivers();
    // - - path to main dirs --
    const char *getUserDir() const {return _userDir.c_str(); }
    const char *getRomsDir() const {return _romsDir.c_str(); }

    const std::vector<const _game_driver *const*> &romsFound() const { return _romsFound; };
    // do not keep that table...
    void buildAllRomsVector(std::vector<const _game_driver *const*> &v);

    const NameDriverMap  &driverIndex() const { return _driverIndex; };

    // to display in bold when found.
    int isDriverFound(const _game_driver *const*drv);

    // apply to mame options
    void applyToMameOptions(_global_options &mameOptions);

    //  - - - -amiga ports related confs (ll is for lowlevel library) ---.
    // playercontrols

    struct Inputs {
        // user defined use of ports. If NotAvail will use ll "autosense"
//#define SJA_TYPE_AUTOSENSE 0
//#define SJA_TYPE_GAMECTLR  1
//#define SJA_TYPE_MOUSE	   2
//#define SJA_TYPE_JOYSTK    3
        int _lowlevelExplicitPortsType[4]; // lowlevel.library is said to manage 4 ports...
        bool        _useParallelPadsForp3p4;
    };
    const Inputs &inputs() const { return _inputsprefs; }

    static void getDriverScreenModestring(const _game_driver *drv, std::string &screenid,int &video_attribs);
protected:
    int _NumDrivers; // in current linker mame driver list. Can be huge.
    // - - - prefs unique for app
    std::string _userDir;
    std::string _romsDir; // finally just use one, but a tested one.

    int   _startWindowed; // else fullscreen.


    int     _activeDriver;
    int     _listShowState;

    int         _audio;
    ULONG       _sampleRate;

    int     _doubleWindow;

    ScreenConf _defaultscreenconf;
    // TODO video prefs, per video config
    // keys are like: "320x224x16"
    std::unordered_map<std::string,ScreenConf> _screenconfs;

    //TODO
    Inputs _inputsprefs;

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

    void sortDrivers( std::vector<const _game_driver *const*> &romsFound);
    void initRomsFoundReverse();
};

// access to singleton
MameConfig &getMainConfig();


#endif
