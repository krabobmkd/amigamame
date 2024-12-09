#ifndef AMIGA_VIDEO_TRACERS_CLUT16_H_
#define AMIGA_VIDEO_TRACERS_CLUT16_H_
/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga_video_tracers.h"

extern "C" {
// for 15b/16b target screens:

// name pattern: directDrawClut_SCREENPIXTYPE_CLUTTYPE_BMSOURCETYPE
// all these use 16b color index or RGB15, used by most games
void directDrawClut_UWORD_UWORD_UWORD(directDrawParams *p,UWORD *lut);
void directDrawClut_ULONG_ULONG_UWORD(directDrawParams *p,ULONG *lut);
void directDrawClut_type24_ULONG_UWORD(directDrawParams *p,ULONG *lut);
void directDrawClut_UBYTE_UBYTE_UWORD(directDrawParams *p,UBYTE *lut);
void directDraw_UBYTE_UBYTE_UWORD(directDrawParams *p);

void directDrawClut_UBYTE_UBYTE_UWORD(directDrawParams *p,UBYTE *lut);

// special one, target screen is 8b, mame sreen is ARGB32, and use 15b long clut.
void directDrawClut_UBYTE_UBYTE_ARGB32(directDrawParams *p,UBYTE *lut);

}
#endif
