#ifndef __DRIVERTUNING_H__
#define __DRIVERTUNING_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "driver.h"

// this is all a krb invention...

// Mame Driver Tuning Flags
#define MDTF_M68K_SAFE_MOVEMWRITE 1
#define MDTF_M68K_SAFE_MOVEMREAD 2
#define MDTF_LONGBOOT 4

typedef struct _DriverTuning {
	UINT32 _bootframeskip;
	UINT32 _flags;
} sDriverTuning;

sDriverTuning *getDriverTuningByName(const char *drivername);
sDriverTuning *getDriverTuning(const struct _game_driver *);


 void applyDriverTuning(const struct _game_driver *);
#ifdef __cplusplus
}
#endif


#endif	/* __DRIVERTUNING_H__ */
