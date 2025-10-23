/***************************************************************************

    sound.c

    Core sound functions and definitions.

    Copyright (c) 1996-2006, Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/
#ifndef SOUND_KRB_H
#define SOUND_KRB_H

#include "osd_cpu.h"
// KRB added
struct SampleFrame
{
    INT32 *_leftmix, *_rightmix;
    UINT32 _read;
    UINT32 _written;
    UINT16 _writelock;
    UINT16 _readlock; // mandatory grabulon. Else, it would have no sense.
};

extern int currentSampleFrame;
extern struct SampleFrame SampleFrames[];
#endif


