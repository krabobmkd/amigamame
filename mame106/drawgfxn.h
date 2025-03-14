#ifndef DRAWGFXN_H_
#define DRAWGFXN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "drawgfx.h"

// src8: 1 pixel, 256 color per byte.
// src4: 2 pixels of 16 colors encoded per byte: 0xf0 0x0f
// prio: pixel tested against previously drawn tilemap, cheap version.

// each can manage transparent/opaque and the 4 flip cases.

void drawgfx_clut16_Src8(struct drawgfxParams *p DGREG(a0));
void drawgfx_clut16_Src4(struct drawgfxParams *p DGREG(a0));

// tecmo.c silkworm, rygar, taitoic.c rastan opwolf,...
void drawgfx_clut16_Src8_prio(struct drawgfxParams *p DGREG(a0));
// cps1.c
void drawgfx_clut16_Src4_prio(struct drawgfxParams *p DGREG(a0));

// chasehq, experimental
// - does zoom or redirected.
// - dest pixel format: clut16
// - source pixel format is 1b per pix but 16 colors (+palette slot shift).
// - transparen color is zero, hardcoded.
// - does priority plane test.
void drawgfxzoom_clut16_Src8_tr0_prio(struct drawgfxParams *p DGREG(a0));

#ifdef __cplusplus
}
#endif

#endif
