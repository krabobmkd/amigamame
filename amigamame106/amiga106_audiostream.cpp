/**************************************************************************
 *
 * Copyright (C) 2024 Vic Ferry (http://github.com/krabobmkd)
 *
 *
 *************************************************************************/

// from mame:
extern "C" {
    // contains Machine definition
    #include "mame.h"
    #include "driver.h"
    #include "osdepend.h"
}

// from amiga
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/ahi.h>
#include <proto/utility.h>
extern "C" {
    #include "exec/types.h"
    #include <dos/dos.h>
    #include <dos/dostags.h>
    #include <exec/memory.h>
    #include <dos/dosextens.h>
    #include <dos/dostags.h>
}

//#define COMPILE_AUDIO_AHI 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amiga106_config.h"


// describe a sound buffer to write. Passed to a SoundWriter function to create sound.
// you should only read values from it and write m_pBuffer and maybe m_Volume.
struct sSoundToWrite
{	// write: stereo Writing on leftright buffer (signed short*2)table:
	SHORT 	    *m_pBuffer;
	// write: AHI Volume multiplier. should be 0x00010000; or do not touch.
	ULONG 	    m_Volume;
	// read: the amount of data to write in pBuffer. *2 for stereo.
	ULONG m_nbSampleToFill;
	// read: play frequency (22050,44100,...) should be the one given with AHIS_Init()
	ULONG	m_PlayFrequency;
	// Total Amount of sample played from the begining.
	// seconds should be found with:  m_TotalSampleDone/m_PlayFrequency.
	unsigned long long	m_TotalSampleDone;
};

typedef enum {
	eAHIS_ok=0,
	eAHIS_Init, // special private value
	eAHIS_DeviceError,
	eAHIS_NotEnoughMemory,
	eAHIS_ThreadError,
    eAHIS_StreamEnd,
	eAHIS_NumberOfError // used to extend the error list by other libs.
} eAHIError;


struct Process *mainprocess = NULL;

// Use  AHIS_Create()  AHIS_Delete() functions below to manage it:
// private struct returned as handler !
struct sAHISoundServer
{
	// sample per seconds while playing:
	ULONG 			m_freq;
	// private system terms for AHI:
	struct MsgPort 	*m_AHImp;
	struct AHIRequest  *m_AHIio;
	struct AHIRequest  *m_AHIio2;
	struct AHIRequest  *m_join;
	SHORT *m_pSBuff1, *m_pSBuff2;
	// private thread handles: - we use volatile keyword to avoid memory coherence problem at interuption -
	//volatile struct Process *m_hThread; // NULL if thread doesn't exist.
	//volatile ULONG m_AskThreadDeath; // 0: thread should live. other: should die.
    unsigned int    m_nextSamples;
    unsigned int    m_stereo; // set at init.
    unsigned int    m_leftRightState;
	unsigned long long	m_TotalSampleDone;
	int	m_Error;
};

//  global
sAHISoundServer *pAHIS = NULL;

// used by thread itself
static void AHI_Close( sAHISoundServer *pAHIS )
{
	if(pAHIS->m_join) {

        if (!CheckIO((struct IORequest *)(pAHIS->m_join))) {
               AbortIO((struct IORequest *)(pAHIS->m_join));
           }
		WaitIO((struct IORequest *)(pAHIS->m_join));
	}

	// close ahi
	if(pAHIS->m_pSBuff1){ FreeVec(pAHIS->m_pSBuff1); pAHIS->m_pSBuff1=NULL; }
	if(pAHIS->m_pSBuff2){ FreeVec(pAHIS->m_pSBuff2); pAHIS->m_pSBuff2=NULL; }

	if(pAHIS->m_AHIio){
		CloseDevice((struct IORequest *)(pAHIS->m_AHIio));
		//FreeSysObject(ASOT_IOREQUEST, pAHIS->m_AHIio);
 		DeleteExtIO((struct IORequest *)(pAHIS->m_AHIio));
        pAHIS->m_AHIio= NULL;
		}

	if(pAHIS->m_AHIio2){
        //FreeSysObject(ASOT_IOREQUEST, pAHIS->m_AHIio2);
        DeleteExtIO((struct IORequest *)(pAHIS->m_AHIio2));
 		pAHIS->m_AHIio2= NULL;
		}
	if(pAHIS->m_AHImp){
        DeletePort(pAHIS->m_AHImp);
        //FreeSysObject(ASOT_PORT, pAHIS->m_AHImp);
 		pAHIS->m_AHImp= NULL;
		}

}



// ---------------
/*
  osd_start_audio_stream() is called at the start of the emulation to initialize
  the output stream, then osd_update_audio_stream() is called every frame to
  feed new data. osd_stop_audio_stream() is called when the emulation is stopped.

  The sample rate is fixed at Machine->sample_rate. Samples are 16-bit, signed.
  When the stream is stereo, left and right samples are alternated in the
  stream.

  osd_start_audio_stream() and osd_update_audio_stream() must return the number
  of samples (or couples of samples, when using stereo) required for next frame.
  This will be around Machine->sample_rate / Machine->drv->frames_per_second,
  the code may adjust it by SMALL AMOUNTS to keep timing accurate and to
  maintain audio and video in sync when using vsync. Note that sound emulation,
  especially when DACs are involved, greatly depends on the number of samples
  per frame to be roughly constant, so the returned value must always stay close
  to the reference value of Machine->sample_rate / Machine->drv->frames_per_second.
  Of course that value is not necessarily an integer so at least a +/- 1
  adjustment is necessary to avoid drifting over time.
*/
int osd_start_audio_stream(int stereo)
{
#if defined(COMPILE_AUDIO_AHI)
    if(pAHIS) osd_stop_audio_stream();

    if(!Machine) return 0; // driver and machine are already inited during this call

    pAHIS = (sAHISoundServer *)AllocVec(sizeof(sAHISoundServer),
                              //OS4 MEMF_SHARED|MEMF_CLEAR
                              MEMF_PUBLIC|MEMF_CLEAR
                              );
    if(!pAHIS)
    {

        return 0;
    }
    int freq = Machine->sample_rate;
    int ifps = (int) Machine->drv->frames_per_second;
    if(freq ==0 || ifps == 0) return 0;

    unsigned int updateLength = ((freq / ifps)+1) & 0xfffffffe;

    printf("osd_start_audio_stream: msfreq: %d ifps:%d uplength:%d stereo:%d\n",
           freq,ifps,updateLength,stereo);

    pAHIS->m_freq = freq;
    pAHIS->m_nextSamples = updateLength;
    pAHIS->m_stereo = stereo;
    pAHIS->m_leftRightState = 0;
    unsigned int buffLength = updateLength<<1;
    if(stereo) buffLength<<=1;

    BYTE deviceResult;
	pAHIS->m_AHImp = CreatePort(NULL,0);
            //AllocSysObject(ASOT_PORT, NULL);
	pAHIS->m_AHIio = (struct AHIRequest*)CreateExtIO(pAHIS->m_AHImp ,sizeof(struct AHIRequest));
            /*AllocSysObjectTags(ASOT_IOREQUEST,
		ASOIOR_Size,		sizeof(struct AHIRequest),
		ASOIOR_ReplyPort,	pAHIS->m_AHImp,
		TAG_END);*/

	if (pAHIS->m_AHIio) {
		pAHIS->m_AHIio->ahir_Version = 4;
		deviceResult =
			OpenDevice(AHINAME,0, (struct IORequest *)(pAHIS->m_AHIio), 0);
	}

    if (deviceResult) {
		pAHIS->m_Error = eAHIS_DeviceError;
		AHI_Close(pAHIS);
        printf(" AHI OpenDevice fail\n");
		return 0;
	}
	pAHIS->m_AHIio2 = (struct AHIRequest *)CreateExtIO(pAHIS->m_AHImp ,sizeof(struct AHIRequest)); /*AllocSysObjectTags(ASOT_IOREQUEST,
		ASOIOR_Size,		sizeof(struct AHIRequest),
		ASOIOR_ReplyPort,	pAHIS->m_AHImp,
		TAG_END);*/
    if(!pAHIS->m_AHIio2) {
		pAHIS->m_Error = eAHIS_DeviceError;
		AHI_Close(pAHIS);
        printf(" AHI CreateExtIO fail\n");
		return 0;
    }
    CopyMem(pAHIS->m_AHIio, pAHIS->m_AHIio2, sizeof(struct AHIRequest));

    pAHIS->m_pSBuff1 = (SHORT *) AllocVec(buffLength, MEMF_PUBLIC|MEMF_CLEAR);
    pAHIS->m_pSBuff2 = (SHORT *) AllocVec(buffLength, MEMF_PUBLIC|MEMF_CLEAR);
	if (!pAHIS->m_pSBuff1 || !pAHIS->m_pSBuff2 ) {
		pAHIS->m_Error = (int) eAHIS_NotEnoughMemory;
        printf(" AHI buffer alloc fail\n");
		AHI_Close(pAHIS);
		return 0;
	}
	//ok for ahi init.

	pAHIS->m_Error = eAHIS_ok;

	// prepare struct which is passed to write func:
//	sSoundToWrite soundToWrite;
//	soundToWrite.m_nbSampleToFill = BUFFERSIZE>>2; // always
//	soundToWrite.m_PlayFrequency = pAHIS->m_freq; // always
//	soundToWrite.m_TotalSampleDone = 0ULL; // 64b.

	// loop still something ask to stop.
    pAHIS->m_join = NULL; // retain the last one to tell next request we continue this one.

    printf("osd_start_audio_stream: OK, ask %d\n",pAHIS->m_nextSamples);

    // must return samples to do next.
    return pAHIS->m_nextSamples;
#else
    return 800;
#endif
}

// from mame main thread engine.
int osd_update_audio_stream(INT16 *buffer)
{
#if defined(COMPILE_AUDIO_AHI)
    if(!pAHIS) return 0;

    printf("osd_update_audio_stream %08x\n", (int)pAHIS);
  //  printf("osd_update_audio_stream\n");
    // buffer length is pAHIS->m_nextSamples.

    SHORT *p1 = pAHIS->m_pSBuff1;
    struct AHIRequest  *AHIio = pAHIS->m_AHIio;

    if(pAHIS->m_stereo)
    {
        // stereo would receive 2 calls for left/right...
        if(pAHIS->m_leftRightState==0)
        {
            // left
           for(unsigned int i=0;i<pAHIS->m_nextSamples;i++)
           {
               *p1 = buffer[i];
                p1 +=2;
           }
           pAHIS->m_leftRightState++;
           return pAHIS->m_nextSamples;
        } else
        {
            // right
            SHORT *p1b = p1+1;
            for(unsigned int i=0;i<pAHIS->m_nextSamples;i++) {
                *p1b = buffer[i];
                 p1b +=2;
            }
            // then push
            AHIio->ahir_Std.io_Length = pAHIS->m_nextSamples<<2; // need byte length.
            AHIio->ahir_Std.io_Data = p1;
            AHIio->ahir_Type = AHIST_S16S;

            pAHIS->m_leftRightState=0;
        }
        // end if stereo
    } else
    {
        // mono
        AHIio->ahir_Std.io_Length = pAHIS->m_nextSamples<<1; // need byte length.
        AHIio->ahir_Std.io_Data = buffer;
        AHIio->ahir_Type = AHIST_M16S;
    }


    AHIio->ahir_Std.io_Message.mn_Node.ln_Pri = 128; //64 //127?
    AHIio->ahir_Std.io_Command = CMD_WRITE;
    AHIio->ahir_Std.io_Offset = 0;
    AHIio->ahir_Version = 4;
    AHIio->ahir_Frequency = pAHIS->m_freq;

    //Workout mode to set
    AHIio->ahir_Volume = 0x010000;
    AHIio->ahir_Position = 0x8000;
    AHIio->ahir_Link = pAHIS->m_join;

    SendIO((struct IORequest *)AHIio);

    if (pAHIS->m_join) {
        WaitIO((struct IORequest *)(pAHIS->m_join));
    }

    pAHIS->m_join = AHIio;

    // switch double buffer:
    pAHIS->m_AHIio = pAHIS->m_AHIio2;
    pAHIS->m_AHIio2 = AHIio;

    pAHIS->m_pSBuff1 = pAHIS->m_pSBuff2;
    pAHIS->m_pSBuff2 = p1;

    // must return length of next sample

    return pAHIS->m_nextSamples;
#else
    return 800;
#endif
}
extern "C" {
void osd_stop_audio_stream(void)
{
    printf("osd_stop_audio_stream\n");
    if(pAHIS)
    {
            printf(" DO cLOSE ??\n");
        AHI_Close(pAHIS);
            printf(" DO cLOSE2 ??\n");
    	FreeVec(pAHIS);
            printf(" DO cLOSE3 ??\n");
        pAHIS = NULL;
    }

}
}
/*
  control master volume. attenuation is the attenuation in dB (a negative
  number). To convert from dB to a linear volume scale do the following:
    volume = MAX_VOLUME;
    while (attenuation++ < 0)
        volume /= 1.122018454;      //  = (10 ^ (1/20)) = 1dB
*/

void osd_set_mastervolume(int attenuation)
{

}

int osd_get_mastervolume(void)
{
    return 0;
}

void osd_sound_enable(int enable)
{

}
