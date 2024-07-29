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
#include <proto/graphics.h>
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
	WORD 	    *m_pBuffer;
	// write: AHI Volume multiplier. should be 0x00010000; or do not touch.
	ULONG 	    m_Volume;
	// read: the amount of data to write in pBuffer. *2 for stereo.
	ULONG m_nbSampleToFill;
	// read: play frequency (22050,44100,...) should be the one given with AHIS_Init()
	ULONG	m_PlayFrequency;
	ULONG         m_stereo;
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
    struct Process *m_hThread;
    ULONG m_AskThreadDeath;

	// sample per seconds while playing:
	ULONG 			m_freq;
	// private system terms for AHI:
	struct MsgPort 	*m_AHImp;
	struct AHIRequest  *m_AHIio;
	struct AHIRequest  *m_AHIio2;
	struct AHIRequest  *m_join;
	SHORT *m_pSBuff1, *m_pSBuff2;

    unsigned int    m_nextSamples;
    unsigned int    m_stereo; // set at init from machine.
	unsigned long long	m_TotalSampleDone;
	int	m_Error;
};

//  global
struct Library *AHIBase=NULL;

sAHISoundServer *pAHIS = NULL;
int m_ahi_error=0;

void AHIS_Delete()
{
    printf(" AHIS_Delete:%08x\n",(int)pAHIS);
    if(!pAHIS) return;
    if(	pAHIS->m_Error == eAHIS_ok ) // if thread running
    {
    printf(" set signal\n");
        SetSignal(0L, SIGF_SINGLE);        /* Use SIGF_SINGLE only after */
        pAHIS->m_AskThreadDeath = 1;                                   /* clearing it.               */
    printf(" wait\n");
        Wait(SIGF_SINGLE);     /* Only use SIGF_SINGLE for Wait()ing and */
    }
	FreeVec(pAHIS);
    pAHIS = NULL;
}

// used by thread itself
static void AHISStaticThread_Close( sAHISoundServer *pAHIS )
{
    printf("AHISStaticThread_Close\n");
    int k;
    	// close ahi
	if(pAHIS->m_join) {

        if (!CheckIO((struct IORequest *)(pAHIS->m_join))) {
               AbortIO((struct IORequest *)(pAHIS->m_join));
           }
		WaitIO((struct IORequest *)(pAHIS->m_join));
	}
    printf("AHISStaticThread_Close 2\n");
	if(pAHIS->m_AHIio){
		CloseDevice((struct IORequest *)(pAHIS->m_AHIio));
 		DeleteExtIO((struct IORequest *)(pAHIS->m_AHIio));
        pAHIS->m_AHIio= NULL;
		}
    printf("AHISStaticThread_Close 3\n");
	if(pAHIS->m_AHIio2){
        DeleteExtIO((struct IORequest *)(pAHIS->m_AHIio2));
 		pAHIS->m_AHIio2= NULL;
		}
	if(pAHIS->m_AHImp){
        DeletePort(pAHIS->m_AHImp);
 		pAHIS->m_AHImp= NULL;
		}
    printf("AHISStaticThread_Close 4\n");
    // then close buffers
	if(pAHIS->m_pSBuff1){ FreeVec(pAHIS->m_pSBuff1); pAHIS->m_pSBuff1=NULL; pAHIS->m_pSBuff2=NULL; }
    printf("AHISStaticThread_Close 5\n");
    pAHIS->m_AskThreadDeath = 0;
	pAHIS->m_hThread = 0; // really has to be last.
}

// return how much done.
ULONG soundMixOnThread( sSoundToWrite *pSoundToWrite);
int audiodbg=0;

static LONG AHISStaticThread(STRPTR args,LONG length,APTR sysbase)
{
audiodbg = 1;
    printf("crac crac process\n");
    // use global var
    if(pAHIS==NULL || !mainprocess) return 1;
	{ // paragraph for continuous double buffer AHI init
        // note: on OS4 the code for this is very different.

    printf("ahi init 1\n");
        BYTE deviceResult;
        pAHIS->m_AHImp = CreatePort(NULL,0);
        pAHIS->m_AHIio = (struct AHIRequest*)CreateExtIO(pAHIS->m_AHImp ,sizeof(struct AHIRequest));
    printf("ahi init 2\n");
        if (pAHIS->m_AHIio) {
            pAHIS->m_AHIio->ahir_Version = 4;
            deviceResult =
                OpenDevice(AHINAME,AHI_NO_UNIT, (struct IORequest *)(pAHIS->m_AHIio), 0);
        }
    printf("ahi init 3\n");
        if (deviceResult) {
            pAHIS->m_Error = eAHIS_DeviceError;
            AHISStaticThread_Close(pAHIS);
            Signal((struct Task *)mainprocess, SIGF_SINGLE);
            return 1;
        }
    printf("ahi init 4\n");
        AHIBase = (struct Library *) pAHIS->m_AHIio->ahir_Std.io_Device;
        pAHIS->m_AHIio2 = (struct AHIRequest *)CreateExtIO(pAHIS->m_AHImp ,sizeof(struct AHIRequest));

        if(!pAHIS->m_AHIio2) {
            pAHIS->m_Error = eAHIS_DeviceError;
            AHISStaticThread_Close(pAHIS);
            Signal((struct Task *)mainprocess, SIGF_SINGLE);
            return 1;
        }
    printf("ahi init 5\n");

        CopyMem(pAHIS->m_AHIio, pAHIS->m_AHIio2, sizeof(struct AHIRequest));
        int stereomult = (pAHIS->m_stereo)?2:1;
        pAHIS->m_pSBuff1 = (SHORT*) AllocVec(pAHIS->m_nextSamples*2*stereomult, MEMF_PUBLIC|MEMF_CLEAR);
        pAHIS->m_pSBuff2 = pAHIS->m_pSBuff1 + pAHIS->m_nextSamples*stereomult;
        if (!pAHIS->m_pSBuff1) {
            pAHIS->m_Error = eAHIS_NotEnoughMemory;
            AHISStaticThread_Close(pAHIS);
            Signal((struct Task *)mainprocess, SIGF_SINGLE);
            return 1;
        }
	//ok for ahi init.
	} // end of paragraph for init

//	audiodbg++;
    printf("send ok signal\n");
	pAHIS->m_Error = eAHIS_ok;
    Signal((struct Task *)mainprocess, SIGF_SINGLE); // signal end of ok init to main thread, will exit start function.

	{ // paragraph for thread loop & data
	// prepare struct which is passed to write func:
	sSoundToWrite soundToWrite;
	soundToWrite.m_nbSampleToFill = pAHIS->m_nextSamples; // always
	soundToWrite.m_PlayFrequency = pAHIS->m_freq; // always
	soundToWrite.m_TotalSampleDone = 0ULL; // 64b.
    soundToWrite.m_stereo =  pAHIS->m_stereo;
	// loop still something ask to stop.
    pAHIS->m_join = NULL; // retain the last one to tell next request we continue this one.

	while(pAHIS->m_AskThreadDeath == 0 )
	{
        audiodbg++;
    	WaitTOF();
//		ULONG numSampleWritten;
//    		SHORT *p1 = pAHIS->m_pSBuff1;

//		soundToWrite.m_pBuffer = p1;
//		soundToWrite.m_Volume = 0x00010000;

//		// write the signal:
//		numSampleWritten = soundMixOnThread( &soundToWrite );

//		// return 0 means no more sound: end of the thread.
//		if(numSampleWritten == 0)
//        {
//            pAHIS->m_Error = eAHIS_StreamEnd;
//            break;
//        }
//		soundToWrite.m_TotalSampleDone +=  (long long int)numSampleWritten;
//		{
//			struct AHIRequest  *AHIio = pAHIS->m_AHIio;
//			AHIio->ahir_Std.io_Message.mn_Node.ln_Pri = 128; //64 //127?
//			AHIio->ahir_Std.io_Command = CMD_WRITE;
//			AHIio->ahir_Std.io_Data = p1;
//			AHIio->ahir_Std.io_Length = (pAHIS->m_stereo)?
//                                    (numSampleWritten<<2) // need byte length.
//                                    :(numSampleWritten<<1);
//			AHIio->ahir_Std.io_Offset = 0;
//            AHIio->ahir_Version = 4;
//			AHIio->ahir_Frequency = pAHIS->m_freq;
//			AHIio->ahir_Type = (pAHIS->m_stereo)?AHIST_S16S:AHIST_M16S;

//// (md_mode&DMODE_STEREO) ? AHIST_S16S : AHIST_M16S;
//			//Workout mode to set
//			AHIio->ahir_Volume = 0x010000;
//			AHIio->ahir_Position = 0x8000; // stereo position to the middle, means 0.5.
//			AHIio->ahir_Link = pAHIS->m_join;
//			SendIO((struct IORequest *)AHIio);

////              PutStr("B");
////               Flush(Output());

//	audiodbg++;
//			if (pAHIS->m_join) {
//				WaitIO((struct IORequest *)(pAHIS->m_join));
//				}
//	audiodbg++;
////              PutStr("C");
////               Flush(Output());

//    			pAHIS->m_join = AHIio;

//			// switch double buffer:
//			pAHIS->m_AHIio = pAHIS->m_AHIio2;
//			pAHIS->m_AHIio2 = AHIio;

//			pAHIS->m_pSBuff1 = pAHIS->m_pSBuff2;
//			pAHIS->m_pSBuff2 = p1;

//		} // end of io paragraph
	} // end of life loop

	}// enmainprocessd of paragraph for thread loop & data
//    //PutStr("  thread die, close AHI\n");
//    //Flush(Output());
//	// assume thread is dead if we reach here:
    printf(" close AHI\n");
	AHISStaticThread_Close(pAHIS);

    printf("send exit signal\n");
    if(mainprocess)
    {
    	Signal((struct Task *)mainprocess, SIGF_SINGLE);
    }
	return 0;

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
 return 800;
 printf("osd_start_audio_stream\n");
    if(pAHIS) osd_stop_audio_stream();

    if(!Machine) return 0; // driver and machine are already inited during this call

    MameConfig::Audio &config = getMainConfig().audio();
    if(config._mode != MameConfig::AudioMode::AHI) return 0;

    int freq =  config._freq; // Machine->sample_rate;
    int machinefreq = Machine->sample_rate;
    if(machinefreq>freq) freq =machinefreq;
    if(freq == 0) return 0;
   // int ifps = (int) Machine->drv->frames_per_second;

 printf("osd_start_audio_stream ok to start thread\n");

   // let's update sound 20 times pr sec...
    ULONG updateLength = ((freq/20)+15)&0xfffffff0;

    mainprocess = (struct Process *)FindTask(NULL);

    pAHIS = (sAHISoundServer *)AllocVec(sizeof(sAHISoundServer),
                              //OS4 MEMF_SHARED|MEMF_CLEAR
                              MEMF_PUBLIC|MEMF_CLEAR
                              );
    if(!pAHIS) return 0;

    printf("freq:%d samplelength:%d\n",freq,updateLength);
	pAHIS->m_Error = eAHIS_Init; // state in which the thread init the AHI device and requests.
    pAHIS->m_freq = freq;
    pAHIS->m_nextSamples = updateLength;
    pAHIS->m_stereo = stereo;

    SetSignal(0L, SIGF_SINGLE);
	{
		struct TagItem threadTags[] = { NP_Entry,(ULONG) &AHISStaticThread,
						//	NP_Child, TRUE, // os4 thread thing
          //Re?              NP_Priority,    60  ,
                        NP_Output,(ULONG) mainprocess->pr_COS,
                        NP_CloseOutput,FALSE,
                        NP_FreeSeglist, FALSE,
		 				TAG_DONE,0  };
        	pAHIS->m_hThread = CreateNewProc( threadTags  );
	}
    if(pAHIS->m_hThread == NULL )
    {
        //printf("createnewproc error\n");
        FreeVec(pAHIS);
        pAHIS = NULL;
        return 0;
	}
 printf("wait for thread init\n");
	// wait for thread to finish AHI init
    Wait(SIGF_SINGLE);
 printf("after wait: err:%d\n",(int)pAHIS->m_Error);
	if(pAHIS->m_Error != eAHIS_ok )
	{
    	m_ahi_error = pAHIS->m_Error;
		AHIS_Delete();
		return 0;
	}
 printf("sound thread ok, samples:%d\n",pAHIS->m_nextSamples);
    // must return samples to do next.
    return pAHIS->m_nextSamples;

}

// from mame main thread engine.
int osd_update_audio_stream(INT16 *buffer)
{
    return (pAHIS)?pAHIS->m_nextSamples:0;
}

void osd_stop_audio_stream(void)
{
    printf(" **** osd_stop_audio_stream: audiodbg:%d\n",audiodbg);
    AHIS_Delete();
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
    printf(" **** osd_sound_enable:%d\n",enable);
}
// return how much done.
ULONG soundMixOnThread( sSoundToWrite *pSoundToWrite)
{
    UWORD n = (UWORD) pSoundToWrite->m_TotalSampleDone ;
    UWORD ntodo = pSoundToWrite->m_nbSampleToFill;
    WORD *ps = pSoundToWrite->m_pBuffer;

    if(pSoundToWrite->m_stereo)
    {
        for(UWORD i=0; i<ntodo ; i++ )
        {
            *ps++ = (n&0x80)?-4000:4000;
            *ps++ = (n&0x80)?-4000:4000;
            n++;
        }
    } else
    {
        for(UWORD i=0; i<ntodo ; i++ )
        {
            *ps++ = (n&0x80)?-4000:4000;
            n++;
        }
    }

    return pSoundToWrite->m_nbSampleToFill;
}
