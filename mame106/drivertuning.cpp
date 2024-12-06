#include "drivertuning.h"

#include <map>
#include <string>

extern "C" {
	#include "mamecore.h"
	UINT32 _bootframeskip=0;
}

using namespace std;



static map<string,sDriverTuning> _tunings={
	{"neogeo",{180,0}},
	{"batrider",{4*60,0}},
	{"sgemf",{8*60,0}},
	//toaplan1
	{"demonwld",{0,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"truxton",{12*60,MDTF_LONGBOOT}}, // boot after the end of the world.
	{"hellfire",{12*60,MDTF_LONGBOOT}},

    {"arkretrn",{12*60,0}},
    {"ashura",{3*60,0}},
    {"slapfigh",{9*60,0}},
    {"bbakraid",{16*60,0}},
    {"battleg",{20*60,0}},
    {"wb32",{9*60,0}},
    {"gauntlet",{9*60,0}},
    {"gaunt2",{9*60,0}},

   	{"sgemf",{8*60,0}},
	{"sfa",{8*60,0}},
 	{"sfa2",{8*60,0}},
	{"sfa3",{8*60,0}},
	{"ssf2",{15*60,0}},
};

sDriverTuning *getDriverTuning(const char *drivername)
{
	map<string,sDriverTuning> ::iterator fit = _tunings.find(string(drivername));
	if(fit == _tunings.end()) return 0L;
	return &(fit->second);
}
sDriverTuning *getDriverTuning(const struct _game_driver *pdriver)
{
    if(!pdriver) return NULL;
    sDriverTuning *dt = getDriverTuning(pdriver->name);
    if(dt) return dt;
    if(pdriver->parent)
    {
        int i=0;
        while(drivers[i]) {
           if(strcmp(pdriver->parent, drivers[i]->name) == 0 ) return getDriverTuning(drivers[i]);
            i++;
        }
    }
    return NULL;
}

void applyDriverTuning(const struct _game_driver *pd)
{
    if(!pd) return;
    _bootframeskip = 0;
    sDriverTuning *ptuning = getDriverTuning(pd);
    if(!ptuning) return;
    _bootframeskip = ptuning->_bootframeskip;

    if(ptuning->_flags & MDTF_M68K_SAFE_MOVEMWRITE)
    {
        ui_popup("Long boot, must wait...");
    }
}
