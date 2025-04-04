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
#define MDTF_CANAVOIDPUSHCONTEXT 8
typedef struct _DriverTuning {
	UINT32 _bootframeskip;
	UINT32 _minimumCpuCycles;
	UINT32 _flags;
} sDriverTuning;

sDriverTuning *getDriverTuningByName(const char *drivername);
sDriverTuning *getDriverTuning(const struct _game_driver *);
// when Machine pointer is set.
//sDriverTuning *getDriverTuning();
// when Machine pointer is set.
UINT32 getDriverTuningFlags();
 void applyDriverTuning(const struct _game_driver *);
#ifdef __cplusplus
}
#endif


#endif	/* __DRIVERTUNING_H__ */
