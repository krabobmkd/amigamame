/******************************************************************************
 * amiga_audiostream.h
 *
 *       ╔════════════════════════════════╗
 *       ║  ♫♪  AUDIO STREAM  ♪♫          ║
 *       ║  ┌─────────────────┐           ║
 *       ║  │  ▂▃▅▆█ SOUND █▆▅▃▂│           ║
 *       ║  │ Stereo Mixing!  │           ║
 *       ║  └─────────────────┘           ║
 *       ║   AHI Sound System             ║
 *       ║   Crystal Clear Audio!         ║
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

#ifndef AMIGA_AUDIOSTREAM_H_
#define AMIGA_AUDIOSTREAM_H_

extern "C"
{
    #include <exec/types.h>

extern int soundMixerIsOn();
}

// Describes a sound buffer to write
// used by the mixer between mame and AHI.
struct sSoundToWrite
{	// write: stereo Writing on leftright buffer (signed short*2)table:
	WORD 	    *m_pBuffer;
	WORD 	    *m_pPrevBuffer; // previous buffer, for tricks.
	// write: AHI Volume multiplier. should be 0x00010000; or do not touch.
	//ULONG 	    m_Volume;
	// read: The amount of data to write in pBuffer. *2 for stereo.
	ULONG m_nbSampleToFill;
	// read: play frequency (22050,44100,...) should be the one given with AHIS_Init()
	ULONG	m_PlayFrequency;
	ULONG         m_stereo;
	// Total Amount of sample played from the beginning.
	// Seconds can be found with:  m_TotalSampleDone/m_PlayFrequency.
	//unsigned long long	m_TotalSampleDone;
};


// return how much done.
ULONG soundMixOnThread( sSoundToWrite *pSoundToWrite);




#endif

/* Sound streaming perfectly! Hear that quality!
 *        ()_()
 *       (='.'=)  <-- Musical bunny approves
 *       (")_(")
 */
