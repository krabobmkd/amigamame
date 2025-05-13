#include "drivertuning.h"

#include <map>
#include <string>

#define DEFMINCPUC 16

extern "C" {
	#include "mamecore.h"
	UINT32 _bootframeskip=0;
	UINT32 _minimumCpuCycles=DEFMINCPUC;
}

using namespace std;




static map<string,sDriverTuning> _tunings={
    /* {"archivename_or_parent",
            { nbframes_to_throttle_at_boot,
             minimum_cycles_to_execute_per_cpu_slice,
             MDTF_flags or'ed  }}
    */
	{"neogeo",{50,DEFMINCPUC,0 /*MDTF_M68K_SAFE_MOVEMWRITE|MDTF_M68K_SAFE_MOVEMREAD*/}}, //
	{"sgemf",{8*60,DEFMINCPUC,0}},
	{"chasehq",{5*60,DEFMINCPUC,0}},
	{"ghouls",{22*60,DEFMINCPUC,0}},


	{"outrun",{0,384,0}}, // for _minimumCpuCycles
	{"shangon",{0,2,0}}, // for _minimumCpuCycles
	{"sharrier",{0,384,0}}, // for _minimumCpuCycles
//	{"thndrbld",{0,2048,0}},
	{"thndrbld",{0,2,0}}, // thunderblade strange timer issues (test) need minimum slice


	{"bombjack",{12*60,0,0}},

	{"mk",{0,128,0}}, // mortal kombat insane interupt slicing

	{"gtmr",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE|MDTF_M68K_SAFE_MOVEMREAD}},     // 1000 miglia
	{"gtmr2",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE|MDTF_M68K_SAFE_MOVEMREAD}},     // 1000 miglia

// test
//	{"bigrun",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE|MDTF_M68K_SAFE_MOVEMREAD}},

	{"1944",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE|MDTF_M68K_SAFE_MOVEMREAD}},
	{"bublbob2",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE|MDTF_M68K_SAFE_MOVEMREAD}},
	{"armwar",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"truxton",{12*60,DEFMINCPUC,MDTF_LONGBOOT}}, // boot after the end of the world.
	{"hellfire",{12*60,DEFMINCPUC,MDTF_LONGBOOT}},

    {"arkanoid",{11*60,DEFMINCPUC,0}},
    {"arkretrn",{12*60,DEFMINCPUC,0}},
    {"ashura",{3*60,DEFMINCPUC,0}},
    {"slapfigh",{13*60,DEFMINCPUC,0}},

    {"bbakraid",{16*60,DEFMINCPUC,0}},
    {"battleg",{20*60,DEFMINCPUC,0}},
	{"batrider",{18*60,DEFMINCPUC,0}},

    {"wb32",{9*60,DEFMINCPUC,0}},
    {"gauntlet",{9*60,DEFMINCPUC,0}},
    {"gaunt2",{9*60,DEFMINCPUC,0}},

   	{"sgemf",{8*60,DEFMINCPUC,0}},
	{"sfa",{8*60,DEFMINCPUC,0}},
 	{"sfa2",{8*60,DEFMINCPUC,0}},
	{"sfa3",{8*60,DEFMINCPUC,0}},
	{"ssf2",{15*60,DEFMINCPUC,0}},

	{"galaga",{14*60,DEFMINCPUC,0}},

	//toaplan1
	{"demonwld",{0,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},     // NEEDED FOR SURE
	{"outzone",{8*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},   //NEEDED FOR SURE
	// following not tested but same hardware &editor may need MDTF_M68K_SAFE_MOVEMWRITE
	{"samesame",{0*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"rallybik",{0*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"truxton",{0*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"hellfire",{0*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"vimana",{0*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},
	{"zerowing",{0*60,DEFMINCPUC,MDTF_M68K_SAFE_MOVEMWRITE}},

};

sDriverTuning *getDriverTuningByName(const char *drivername)
{
	map<string,sDriverTuning> ::iterator fit = _tunings.find(string(drivername));
	if(fit == _tunings.end()) return 0L;
	return &(fit->second);
}
sDriverTuning *getDriverTuning(const struct _game_driver *pdriver)
{
    if(!pdriver) return NULL;
    sDriverTuning *dt = getDriverTuningByName(pdriver->name);
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
UINT32 getDriverTuningFlags()
{
    sDriverTuning *drvtunings = getDriverTuning(Machine->gamedrv);
    if(!drvtunings) return 0;
    return drvtunings->_flags;
}
void applyDriverTuning(const struct _game_driver *pd)
{
    if(!pd) return;
    _bootframeskip = 0;
    sDriverTuning *ptuning = getDriverTuning(pd);
    if(!ptuning) return;
    _bootframeskip = ptuning->_bootframeskip;
    _minimumCpuCycles = ptuning->_minimumCpuCycles;

    if(ptuning->_flags & MDTF_LONGBOOT)
    {
        ui_popup("Long boot, must wait...");
    }
}
