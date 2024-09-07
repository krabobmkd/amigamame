#ifndef DRAWGFXN_H_
#define DRAWGFXN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "drawgfx.h"

void drawgfx_clut16_T(struct drawgfxParams *p DGREG(a0));
void drawgfx_clut16_O(struct drawgfxParams *p DGREG(a0));

#ifdef __cplusplus
}
#endif

#endif