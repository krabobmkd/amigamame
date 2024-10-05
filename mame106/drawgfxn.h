#ifndef DRAWGFXN_H_
#define DRAWGFXN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "drawgfx.h"

void drawgfx_clut16_Src8(struct drawgfxParams *p DGREG(a0));
void drawgfx_clut16_Src4(struct drawgfxParams *p DGREG(a0));
void drawgfx_clut16_Src8_prio(struct drawgfxParams *p DGREG(a0));
void drawgfx_clut16_Src4_prio(struct drawgfxParams *p DGREG(a0));

#ifdef __cplusplus
}
#endif

#endif
