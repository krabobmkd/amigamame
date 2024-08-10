#ifndef AMIGA_VIDEO_TRACERS_CLUT16_H_
#define AMIGA_VIDEO_TRACERS_CLUT16_H_
/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga_video_tracers.h"

extern "C" {
// all these use 16b color index or RGB15, used by most games
void directDrawClutT_UWORD_UWORD(directDrawParams *p,UWORD *lut);
void directDrawClutT_ULONG_ULONG(directDrawParams *p,ULONG *lut);
void directDrawClutT_type24_ULONG(directDrawParams *p,ULONG *lut);
void directDrawClutT_UBYTE_UBYTE(directDrawParams *p,UBYTE *lut);
void directDraw_UBYTE_UBYTE(directDrawParams *p);

}
#endif
