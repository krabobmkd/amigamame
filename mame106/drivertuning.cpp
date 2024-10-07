#include "drivertuning.h"

#include <map>
#include <string>

extern "C" {
	#include <mamecore.h>	
	unsigned int _bootframeskip=0;
}

using namespace std;



static map<string,sDriverTuning> _tunings={
	{"neogeo",{4*60}},
	{"batrider",{4*60}}
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
    _bootframeskip = 30; // default ?
    sDriverTuning *ptuning = getDriverTuning(pd);
    if(!ptuning) return;
    _bootframeskip = ptuning->_bootframeskip;

}
