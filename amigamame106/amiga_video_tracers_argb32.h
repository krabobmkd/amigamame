#ifndef AMIGA_VIDEO_TRACERS_ARGB32_H_
#define AMIGA_VIDEO_TRACERS_ARGB32_H_
/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga_video_tracers.h"

extern "C" {

// added for ARGB32 modes (only games RGB highcolor, not using clut.
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
