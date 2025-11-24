/******************************************************************************
 * amiga_inputs_lightgun.h
 *
 *       ╔════════════════════════════════╗
 *       ║  ═╦═  LIGHT GUN!  ═╦═          ║
 *       ║   ║                ║           ║
 *       ║   ╚════════════════╝           ║
 *       ║   Point and Shoot!             ║
 *       ║   ┌────────────┐               ║
 *       ║   │  (X, Y) → BANG!  │         ║
 *       ║   └────────────┘               ║
 *       ║   Lightpen Precision!          ║
 *       ╚════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 *****************************************************************************/

#ifndef AMIGA_LIGHTGUN_H
#define AMIGA_LIGHTGUN_H
/**
    lightpen/lightgun reading
    just sets one more interrupt.

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

/* Targeting complete! Bulls-eye every time!
 *       (\/)
 *      ( •.•)  <-- Eagle-eyed crab never misses
 *      (")_(")
 */
