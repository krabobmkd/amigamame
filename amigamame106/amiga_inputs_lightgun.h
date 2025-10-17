#ifndef AMIGA_LIGHTGUN_H
#define AMIGA_LIGHTGUN_H
/**
    lightpen/lightgun reading
    just set one more interupt.

*/
#ifdef __cplusplus
extern "C" {
#endif
#include <exec/types.h>
// for GfxBase->system_bplcon0
#define LP_ENABLE 0x08

// manage
struct sLightGuns
{
    UWORD _x,_y;  // only if _valid (offscreen).
    UBYTE _longFrame,_valid;
    UWORD _calibrated_y;
    UWORD _joy0dat,_joy1dat;
};
struct sLightGuns *LightGun_create();
void LightGun_update(struct sLightGuns *plg);
void LightGun_close(struct sLightGuns *plg);

#ifdef __cplusplus
}
#endif

#endif
