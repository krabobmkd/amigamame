#ifndef __DRIVERTUNING_H__
#define __DRIVERTUNING_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "driver.h"

typedef struct _DriverTuning {
	int _bootframeskip; // also no brake
	
} sDriverTuning;

sDriverTuning *getDriverTuningByName(const char *drivername);
sDriverTuning *getDriverTuning(const struct _game_driver *);


 void applyDriverTuning(const struct _game_driver *);
#ifdef __cplusplus
}
#endif


#endif	/* __DRIVERTUNING_H__ */
