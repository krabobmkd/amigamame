/******************************************************************************
 * amiga_video_tracers_argb32.h
 *
 *       ╔════════════════════════════════╗
 *       ║  █▓▒░ ARGB32 TRACERS ░▒▓█      ║
 *       ║  True Color Graphics!          ║
 *       ║  ┌──────────────────┐          ║
 *       ║  │ R  G  B  Alpha   │          ║
 *       ║  │ 8  8  8    8     │          ║
 *       ║  │ 16777216 colors! │          ║
 *       ║  └──────────────────┘          ║
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

#ifndef AMIGA_VIDEO_TRACERS_ARGB32_H_
#define AMIGA_VIDEO_TRACERS_ARGB32_H_

#include "amiga_video_tracers.h"

extern "C" {

// added for ARGB32 modes (only for RGB highcolor games, not using CLUT).
// manage all CGX screen pixel modes

void directDraw_RGB15_ARGB32(directDrawParams *p);
void directDraw_BGR15_ARGB32(directDrawParams *p);
void directDraw_RGB15PC_ARGB32(directDrawParams *p);
void directDraw_BGR15PC_ARGB32(directDrawParams *p);

void directDraw_RGB16_ARGB32(directDrawParams *p);
void directDraw_BGR16_ARGB32(directDrawParams *p);
void directDraw_RGB16PC_ARGB32(directDrawParams *p);
void directDraw_BGR16PC_ARGB32(directDrawParams *p);

void directDraw_type24_ARGB32(directDrawParams *p);

void directDrawARGB32_ARGB32(directDrawParams *p);
void directDrawBGRA32_ARGB32(directDrawParams *p);
void directDrawRGBA32_ARGB32(directDrawParams *p);


}
#endif

/* 32-bit color depth mastered! RGB perfection!
 *       __
 *      {00}  <-- Gopher digs deep colors
 *      \__/
 *       ||
 */
