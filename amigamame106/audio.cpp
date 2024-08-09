/**************************************************************************
 *
 * Copyright (C) 1999 Mats Eirik Hansen (mats.hansen@triumph.no)
 *
 * $Id: audio.c,v 1.1 1999/04/28 18:50:48 meh Exp $
 *
 * $Log: audio.c,v $
 * Revision 1.1  1999/04/28 18:50:48  meh
 * Initial revision
 *
 *
 *************************************************************************/




extern "C" {
#include <devices/audio.h>

#define AHI_BASE_NAME audio->AHIBase
//#include <inline/ahi.h>

}
#include <stdlib.h>


#include "audio.h"

#define DEF_BUFFER_SIZE     32768
#define DEF_MIN_FREE_CHIP   (64*1024)

#define MAX_AHI_CHANNEL_TAGS  9
#define MAX_PAULA_FREQUENCY   28867
#define MAX_PAULA_LENGTH    65536

#define MAKE_PERIOD(f) ((f)?(((ULONG)3579547)/f):(65536))

#define INTELULONG(i) (((i)<<24)|((i)>>24)|(((i)<<8)&0x00ff0000)|(((i)>>8)&0x0000ff00))


extern struct Library *UtilityBase;

#ifdef POWERUP
extern struct Library *PPCLibBase;

static inline APTR memAlloc(ULONG size)
{
  return(PPCAllocVec(size, MEMF_PUBLIC|MEMF_CLEAR));
}

static inline void memFree(APTR mem)
{
  PPCFreeVec(mem);
}
#else
static inline APTR memAlloc(ULONG size)
{
  return(AllocVec(size, MEMF_PUBLIC|MEMF_CLEAR));
}

static inline void memFree(APTR mem)
{
  FreeVec(mem);
}
#endif

struct Audio *AllocAudio(Tag tags,...)
{
  struct Audio  *audio;
  struct TagItem  *tag, *taglist;
  LONG      use_ahi;
  LONG      channels;
  LONG      max_sounds;
  LONG      buffer_size;
  BYTE      map_channels[4];
  ULONG     min_free_chip;
  LONG      size;
  LONG      i;

  taglist = (struct TagItem *) &tags;

  use_ahi     = FALSE;
  channels    = 4;
  max_sounds    = 0;
  min_free_chip = DEF_MIN_FREE_CHIP;
  buffer_size   = DEF_BUFFER_SIZE;
  map_channels[0] = 0;
  map_channels[1] = 1;
  map_channels[2] = 2;
  map_channels[3] = 3;

  while((tag = NextTagItem(&taglist)))
  {
    switch(tag->ti_Tag)
    {
      case AA_UseAHI:
        use_ahi = tag->ti_Data;
        break;
      case AA_Channels:
        channels = tag->ti_Data;
        break;
      case AA_MaxSounds:
        max_sounds = tag->ti_Data;
        break;
      case AA_MapChannel0:
        map_channels[0] = tag->ti_Data;
        break;
      case AA_MapChannel1:
        map_channels[1] = tag->ti_Data;
        break;
      case AA_MapChannel2:
        map_channels[2] = tag->ti_Data;
        break;
      case AA_MapChannel3:
        map_channels[3] = tag->ti_Data;
        break;
      case AA_MinFreeChip:
        min_free_chip = tag->ti_Data;
        break;
    }
  }

  size = sizeof(struct Audio);
  
  if(use_ahi)
    size += ((channels * MAX_AHI_CHANNEL_TAGS) + 1) * sizeof(struct TagItem);
  else
    size += 9 * sizeof(struct IOAudio);

  audio = (struct Audio *)AllocVec(size, MEMF_CLEAR|MEMF_PUBLIC);
  
  if(audio)
  {
    NewList((struct List *) &audio->Sounds);

    audio->UseAHI   = use_ahi;
    audio->Channels   = channels;
    audio->BufferSize = ((buffer_size + 3) >> 2) << 2;
    audio->MasterVolume = 100;
    audio->NextSound  = 1;
    audio->MinFreeChip  = min_free_chip;

    for(i = 0; i < 4; i++)
    {
      if(map_channels[i] < channels)
        audio->MapChannels[i] = map_channels[i];
      else
        audio->MapChannels[i] = i;
    }

    audio->ChannelArray = AAllocChannelArray(audio, 0);

    if(audio->ChannelArray)
    {
      audio->MsgPort  = CreateMsgPort();
        
      if(audio->MsgPort)
      {
        if(use_ahi)
        {
          audio->AHITags    = (struct TagItem *) &audio[1];
          audio->AHIRequest = (struct AHIRequest *) CreateIORequest(audio->MsgPort, sizeof(struct AHIRequest));
          
          if(audio->AHIRequest)
          {
            audio->AHIRequest->ahir_Version = 4;
          
            if(!OpenDevice(AHINAME, AHI_NO_UNIT, (struct IORequest *) audio->AHIRequest, 0))
            {
              audio->AHIBase  = (struct Library *) audio->AHIRequest->ahir_Std.io_Device;

              audio->AHIAudioCtrl = AHI_AllocAudio( AHIA_Channels,  channels,
                                  AHIA_Sounds,  max_sounds + 1,
                                  TAG_END);
                                  
              if(audio->AHIAudioCtrl)
              {
                audio->AHISampleInfo.ahisi_Type   = AHIST_M8S;
                audio->AHISampleInfo.ahisi_Address  = 0;
                audio->AHISampleInfo.ahisi_Length = 0xffffffff;
              
                if(!AHI_LoadSound(0, AHIST_DYNAMICSAMPLE, &audio->AHISampleInfo, audio->AHIAudioCtrl))
                {
                  AHI_ControlAudio(audio->AHIAudioCtrl, AHIC_Play, TRUE);

                  audio->AHIEffMasterVolume.ahie_Effect = AHIET_MASTERVOLUME;
                  audio->AHIEffMasterVolume.ahiemv_Volume = (audio->Channels >> 1) * 0x10000;
      
                  AHI_SetEffect(&audio->AHIEffMasterVolume, audio->AHIAudioCtrl);

                  return(audio);
                }

                AHI_FreeAudio(audio->AHIAudioCtrl);
              }
              
              CloseDevice((struct IORequest *) audio->AHIRequest);
            }
            
            DeleteIORequest((struct IORequest *) audio->AHIRequest);
          }
        }
        else
        {
          UBYTE ch;

          audio->AudioRequests = (struct IOAudio *) &audio[1];
          
          for(i = 0; i < 9; i++)
          {
            audio->AudioRequests[i].ioa_Request.io_Message.mn_ReplyPort = audio->MsgPort;
            audio->AudioRequests[i].ioa_Request.io_Message.mn_Length  = sizeof(struct IOAudio);
          }
          
          audio->Buffers = (BYTE *)AllocVec(8 * audio->BufferSize, MEMF_PUBLIC|MEMF_CHIP);
          
          if(audio->Buffers)
          {
            ch = 0xf;
          
            audio->AudioRequests[0].ioa_Request.io_Command  = ADCMD_ALLOCATE;
            audio->AudioRequests[0].ioa_Request.io_Flags  = ADIOF_NOWAIT|IOF_QUICK;
            audio->AudioRequests[0].ioa_Data        = &ch;
            audio->AudioRequests[0].ioa_Length        = 1;

            if(!OpenDevice("audio.device", 0, (struct IORequest *) audio->AudioRequests, 0))
            {
              for(i = 1; i < 9; i++)
              {
                audio->AudioRequests[i].ioa_Request.io_Device = audio->AudioRequests->ioa_Request.io_Device;
                audio->AudioRequests[i].ioa_AllocKey      = audio->AudioRequests->ioa_AllocKey;
              }

              return(audio);
            }
            
            FreeVec(audio->Buffers);
          }
        }

        DeleteMsgPort(audio->MsgPort);
      }
      
      AFreeChannelArray(audio->ChannelArray);
    }
    
    FreeVec(audio);
  }
  
  return(NULL);
}

void FreeAudio(struct Audio *audio)
{
  struct MinNode  *node;
  struct IOAudio  *ioa;
  LONG      i;

  node = audio->Sounds.mlh_Head->mln_Succ;

  while(node)
  {
    if(audio->UseAHI)
      AHI_UnloadSound(((struct ASound *) node->mln_Pred)->Sound, audio->AHIAudioCtrl);
    else if(((struct ASound *) node->mln_Pred)->AHISampleInfo.ahisi_Address != &((struct ASound *) node->mln_Pred)[1])
      FreeVec(((struct ASound *) node->mln_Pred)->AHISampleInfo.ahisi_Address);

    memFree(node->mln_Pred);
    
    node = node->mln_Succ;
  }

  if(audio->UseAHI)
  {
    AHI_UnloadSound(0, audio->AHIAudioCtrl);
    AHI_FreeAudio(audio->AHIAudioCtrl);
    CloseDevice((struct IORequest *) audio->AHIRequest);
    DeleteIORequest((struct IORequest *) audio->AHIRequest);
    DeleteMsgPort(audio->MsgPort);
  }
  else
  {
    for(i = 0; i < 4; i++)
    {
      if(audio->Status[i] == AS_PLAYING1)
        AbortIO((struct IORequest *) &audio->AudioRequests[2*i]);
      else if(audio->Status[i] == AS_PLAYING2)
        AbortIO((struct IORequest *) &audio->AudioRequests[2*i+1]);
    }

    while(audio->Status[0] || audio->Status[1] || audio->Status[2] || audio->Status[3])
    {
      WaitPort(audio->MsgPort);

      ioa = (struct IOAudio *) GetMsg(audio->MsgPort);
      
      if(ioa)
        audio->Status[(((ULONG) ioa) - ((ULONG) audio->AudioRequests)) / (sizeof(struct IOAudio) << 1)] = AS_IDLE;
    }
    
    CloseDevice((struct IORequest *) audio->AudioRequests);
    
    FreeVec(audio->Buffers);
  }

  AFreeChannelArray(audio->ChannelArray);

  FreeVec(audio);
}

struct AChannelArray *AAllocChannelArray(struct Audio *audio, LONG length)
{
  struct AChannelArray  *ca;
  LONG          size;
  LONG          i;
  BYTE          *buffer;
  
  size = sizeof(struct AChannelArray) + (audio->Channels * (sizeof(struct AChannel) + length));
  
  ca = (struct AChannelArray *) memAlloc(size);
  
  if(ca)
  {
    ca->Audio   = audio;
    ca->Size    = size;
    ca->Length    = length;
    ca->Channels  = (struct AChannel *) &ca[1];
    
    buffer = (BYTE *) & ca->Channels[audio->Channels];
    
    for(i = 0; i < audio->Channels; i++)
    {
      ca->Channels[i].Buffer = buffer;
      
      buffer += length;
    }

#ifdef POWERUP
    PPCCacheClearE(ca, ca->Size, CACRF_ClearD);
#endif
  }
  
  return(ca);
}

void AFreeChannelArray(struct AChannelArray *ca)
{
  if(ca)
    memFree(ca);
}

void ASetChannelFrame(struct AChannelArray *ca)
{
  struct Audio    *audio;
  struct AChannel   *c;
  struct AChannel   *ac;
  struct ASound   *sound;
  struct TagItem    *tags;
  struct IOAudio    *aio;
  LONG        i, j, k, l, o;
  BYTE        stop_status[4];
  BYTE        start_status[4];
  BYTE        *sample;
  BYTE        *buffer;
  ULONG       length;
  ULONG       len;
  ULONG       freq;
  ULONG       vol;
  ULONG       step;

  audio = ca->Audio;
  c   = ca->Channels;
  ac    = audio->ChannelArray->Channels;
  
  if(audio->UseAHI)
  {
    tags  = audio->AHITags;
    sound = NULL;

    for(i = 0, j = 0; i < audio->Channels; i++)
    {
      if(c[i].Flags)
      {
        if(c[i].Flags & ACF_Stop)
          AHI_SetSound(i, AHI_NOSOUND, 0, 0, audio->AHIAudioCtrl, AHISF_IMM);
        else if(c[i].Flags & ACF_Restart)
        {
          if(ac[i].Flags)
          {
            tags[j].ti_Tag    = AHIP_BeginChannel;
            tags[j++].ti_Data = i;

            if(ac[i].Flags & ACF_SetSound)
            {
              k = ac[i].Sound;

              tags[j].ti_Tag    = AHIP_Sound;
              tags[j++].ti_Data = k;
              tags[j].ti_Tag    = AHIP_Pan;
              if(i & 1)
                tags[j++].ti_Data = 0;
              else
                tags[j++].ti_Data = 0x10000;
          
              sound = (struct ASound *) audio->Sounds.mlh_Head;

              while(--k)
                sound = (struct ASound *) sound->Node.mln_Succ;

              tags[j].ti_Tag    = AHIP_Offset;
              tags[j++].ti_Data = 0;
              tags[j].ti_Tag    = AHIP_Length;
              tags[j++].ti_Data = sound->AHISampleInfo.ahisi_Length;
            }
            else if(ac[i].Flags & ACF_SetSample)
            {
              tags[j].ti_Tag    = AHIP_Sound;
              tags[j++].ti_Data = 0;
              tags[j].ti_Tag    = AHIP_Pan;
              if(i & 1)
                tags[j++].ti_Data = 0;
              else
                tags[j++].ti_Data = 0x10000;
              tags[j].ti_Tag    = AHIP_Offset;
              tags[j++].ti_Data = ac[i].Sound;
            }
            
            tags[j].ti_Tag    = AHIP_Freq;
            tags[j++].ti_Data = ac[i].Frequency;
            tags[j].ti_Tag    = AHIP_Vol;
            tags[j++].ti_Data = ac[i].Volume * 0x10000 / 100;
            tags[j].ti_Tag    = AHIP_Length;
            tags[j++].ti_Data = ac[i].Length;

            if(!(ac[i].Flags & ACF_Loop))
            {
              tags[j].ti_Tag    = AHIP_LoopSound;
              tags[j++].ti_Data = AHI_NOSOUND;
            }

            tags[j].ti_Tag    = AHIP_EndChannel;
            tags[j++].ti_Data = NULL;
          }
        }
        else
        {   
          tags[j].ti_Tag    = AHIP_BeginChannel;
          tags[j++].ti_Data = i;

          if(c[i].Flags & ACF_SetSound)
          {
            k = c[i].Sound;

            tags[j].ti_Tag    = AHIP_Sound;
            tags[j++].ti_Data = k;
            tags[j].ti_Tag    = AHIP_Pan;
            if(i & 1)
              tags[j++].ti_Data = 0;
            else
              tags[j++].ti_Data = 0x10000;
          
            sound = (struct ASound *) audio->Sounds.mlh_Head;

            while(--k)
              sound = (struct ASound *) sound->Node.mln_Succ;

            tags[j].ti_Tag    = AHIP_Offset;
            tags[j++].ti_Data = 0;
            tags[j].ti_Tag    = AHIP_Length;
            tags[j++].ti_Data = sound->AHISampleInfo.ahisi_Length;

            ac[i].Flags = ACF_SetSound;
            ac[i].Sound = c[i].Sound;

            if(c[i].Flags & ACF_Loop)
              ac[i].Flags |= ACF_Loop;
          }
          else if(c[i].Flags & ACF_SetSample)
          {
            tags[j].ti_Tag    = AHIP_Sound;
            tags[j++].ti_Data = 0;
            tags[j].ti_Tag    = AHIP_Pan;
            if(i & 1)
              tags[j++].ti_Data = 0;
            else
              tags[j++].ti_Data = 0x10000;
            tags[j].ti_Tag    = AHIP_Offset;
            tags[j++].ti_Data = c[i].Sound;

            ac[i].Flags = ACF_SetSample;
            ac[i].Sound = c[i].Sound;

            if(c[i].Flags & ACF_Loop)
              ac[i].Flags |= ACF_Loop;
          }

          if(c[i].Flags & ACF_SetFrequency)
          {
            tags[j].ti_Tag    = AHIP_Freq;
            tags[j++].ti_Data = c[i].Frequency;
            
            ac[i].Frequency = c[i].Frequency;
          }
          else if(c[i].Flags & ACF_SetSound)
          {
            tags[j].ti_Tag    = AHIP_Freq;
            tags[j++].ti_Data = sound->Frequency;

            ac[i].Frequency = sound->Frequency;
          }

          if(c[i].Flags & ACF_SetVolume)
          {
            tags[j].ti_Tag    = AHIP_Vol;
            tags[j++].ti_Data = c[i].Volume * 0x10000 / 100;

            ac[i].Volume  = c[i].Volume;
          }
          else if(c[i].Flags & ACF_SetSound)
          {
            tags[j].ti_Tag    = AHIP_Vol;
            tags[j++].ti_Data = sound->Volume * 0x10000 / 100;

            ac[i].Volume  = sound->Volume;
          }

          if(c[i].Flags & ACF_SetLength)
          {
            tags[j].ti_Tag    = AHIP_Length;
            tags[j++].ti_Data = c[i].Length;

            ac[i].Length  = c[i].Length;
          }

          if(!(c[i].Flags & ACF_Loop)
          && ((c[i].Flags & ACF_SetSound) || (c[i].Flags & ACF_SetSample)))
          {
            tags[j].ti_Tag    = AHIP_LoopSound;
            tags[j++].ti_Data = AHI_NOSOUND;
          }

          tags[j].ti_Tag    = AHIP_EndChannel;
          tags[j++].ti_Data = NULL;
        }
        
        c[i].Flags = 0;
      }

    }

    if((ca->Flags & ACF_SetVolume) && (ca->MasterVolume != audio->MasterVolume))
    {
      audio->AHIEffMasterVolume.ahiemv_Volume = (audio->Channels >> 1) * 0x10000 / 100 * ca->MasterVolume;

      AHI_SetEffect(&audio->AHIEffMasterVolume, audio->AHIAudioCtrl);
      
      audio->MasterVolume = ca->MasterVolume;
    }

    ca->Flags = 0;

    if(j)
    {
      tags[j].ti_Tag = TAG_END;
      
      AHI_PlayA(audio->AHIAudioCtrl, audio->AHITags);
    }
  }
  else
  {
    for(i = 0; i < 4; i++)
    {
      start_status[i] = AS_IDLE;
      stop_status[i]  = AS_IDLE;
    }

    if(ca->Flags & ACF_SetVolume)
      vol = ca->MasterVolume;
    else
      vol = audio->MasterVolume;

    for(i = 0; i < audio->Channels; i++)
    {
      if(c[i].Flags)
      {
        for(j = 0; j < 4; j++)
        {
          if(audio->MapChannels[j] == i)
          {
            if((c[i].Flags & ACF_Stop) && audio->Status[j])
            {
              if(audio->Status[j] == AS_PLAYING1)
                AbortIO((struct IORequest *) &audio->AudioRequests[2*j]);
              else if(audio->Status[j] == AS_PLAYING2)
                AbortIO((struct IORequest *) &audio->AudioRequests[2*j+1]);

              stop_status[j]    = audio->Status[j];
              audio->Status[j]  = AS_IDLE;
            }
            else if(c[i].Flags & ACF_Restart)
            {
            }
            else
            {
              if(c[i].Flags)
              {
                if((c[i].Flags & ACF_SetSound) || (c[i].Flags & ACF_SetSample))
                {               
                  if(c[i].Flags & ACF_SetSound)
                  {
                    k = c[i].Sound;

                    sound = (struct ASound *) audio->Sounds.mlh_Head;

                    while(--k)
                      sound = (struct ASound *) sound->Node.mln_Succ;
                  
                    sample  = (BYTE*)sound->AHISampleInfo.ahisi_Address;
                    length  = sound->AHISampleInfo.ahisi_Length;
                  }
                  else
                  {
                    sample  = (BYTE *) c[i].Sound;
                    length  = c[i].Length;
                  }
                  
                  freq = c[i].Frequency;
                  
                  if(audio->Status[j] == AS_PLAYING1)
                  {
                    o = 1;
                    start_status[j] = AS_PLAYING2;
                    stop_status[j]  = AS_PLAYING1;
                  }
                  else
                  {
                    o = 0;
                    start_status[j] = AS_PLAYING1;
                    if(audio->Status[j] == AS_PLAYING2)
                      stop_status[j]  = AS_PLAYING2;
                  }

                  if((freq > MAX_PAULA_FREQUENCY) || (length > audio->BufferSize)
                  || !(TypeOfMem(sample) & MEMF_CHIP))
                  {
                    len   = length;
                    buffer  = audio->Buffers + (audio->BufferSize * ((j << 1) + o));
                    step  = AResample(&freq, MAX_PAULA_FREQUENCY, &length, audio->BufferSize);
                    
                    if(step == 0x100)
                    {
                      len = (len + 3) >> 2;
                      for(k = 0; k < len; k++)
                        ((ULONG *) buffer)[k] = ((ULONG *) sample)[k];
                    }
                    else
                    {
                      for(k = 0, l = 0; k < length; k++, l += step)
                        buffer[k] = sample[l >> 8];
                    }
                    
                    sample = buffer;
                  }

                  length &= 0xfffffffe;

                  audio->AudioRequests[2*j+o].ioa_Request.io_Unit   = (struct Unit *) (1<<j);
                  audio->AudioRequests[2*j+o].ioa_Request.io_Command  = CMD_WRITE;
                  audio->AudioRequests[2*j+o].ioa_Request.io_Flags  = ADIOF_PERVOL;
                  audio->AudioRequests[2*j+o].ioa_Data        = (UBYTE*)sample;
                  audio->AudioRequests[2*j+o].ioa_Length        = length;
                  audio->AudioRequests[2*j+o].ioa_Period        = MAKE_PERIOD(freq);
                  audio->AudioRequests[2*j+o].ioa_Volume        = (64 * vol * c[i].Volume) / 10000;
                  audio->AudioRequests[2*j+o].ioa_Cycles        = (c[i].Flags & ACF_Loop) ? 0 : 1;
                  
                  ac[i].Frequency = freq;
                  ac[i].Volume  = c[i].Volume;
                  ac[i].Length  = length;
                  ac[i].Sound   = (LONG) sample;
                  
                  audio->FrequencyModifier[j] = (freq << 12) / c[i].Frequency;
                }
                else if(audio->Status[j])
                {
                  if(audio->Status[j] == AS_PLAYING1)
                  {
                    o = 1;
                    start_status[j] = AS_PLAYING2;
                    stop_status[j]  = AS_PLAYING2;
                  }
                  else
                  {
                    o = 0;
                    start_status[j] = AS_PLAYING1;
                    stop_status[j]  = AS_PLAYING1;
                  }

                  if(c[i].Flags & ACF_SetFrequency)
                    ac[i].Frequency = (audio->FrequencyModifier[j] * c[i].Frequency) >> 12;

                  if(c[i].Flags & ACF_SetVolume)
                    ac[i].Volume = c[i].Volume;
                  
                  audio->AudioRequests[2*j+o].ioa_Request.io_Unit   = (struct Unit *) (1<<j);
                  audio->AudioRequests[2*j+o].ioa_Request.io_Command  = ADCMD_PERVOL;
                  audio->AudioRequests[2*j+o].ioa_Request.io_Flags  = 0;
                  audio->AudioRequests[2*j+o].ioa_Period        = MAKE_PERIOD(ac[i].Frequency);
                  audio->AudioRequests[2*j+o].ioa_Volume        = (64 * vol * ac[i].Volume) / 10000;
                }
              }
            }
          }
        }
      }
    }

    for(j = 0; j < 4; j++)
    {
      if(start_status[j] == AS_PLAYING1)
      {
        BeginIO((struct IORequest *) &audio->AudioRequests[2*j]);

        if(audio->Status[j] == AS_PLAYING2)
          AbortIO((struct IORequest *) &audio->AudioRequests[2*j+1]);
      }
      else if(start_status[j] == AS_PLAYING2)
      {
        BeginIO((struct IORequest *) &audio->AudioRequests[2*j+1]);

        if(audio->Status[j] == AS_PLAYING1)
          AbortIO((struct IORequest *) &audio->AudioRequests[2*j]);
      }
    }

    while(stop_status[0] || stop_status[1] || stop_status[2] || stop_status[3])
    {
      WaitPort(audio->MsgPort);
      
      while((aio = (struct IOAudio *) GetMsg(audio->MsgPort)))
      {
        i = (((ULONG) aio) - ((ULONG) audio->AudioRequests)) / sizeof(struct IOAudio);

        if(i&1)
        {
          if(stop_status[i>>1] == AS_PLAYING2)
          {
            audio->Status[i>>1] = AS_IDLE;
            stop_status[i>>1] = AS_IDLE;
          }
          else
            start_status[i>>1] = AS_IDLE;
        }
        else
        {
          if(stop_status[i>>1] == AS_PLAYING1)
          {
            audio->Status[i>>1] = AS_IDLE;
            stop_status[i>>1] = AS_IDLE;
          }
          else
            start_status[i>>1] = AS_IDLE;
        }
      }
    }

    if(vol != audio->MasterVolume)
    {
      audio->MasterVolume = vol;
      
      for(j = 0; j < 4; j++)
      {
        if(audio->Status[j] && !start_status[j])
        {
          if(audio->Status[j] == AS_PLAYING1)
            o = 1;
          else
            o = 0;
                  
          audio->AudioRequests[2*j+o].ioa_Request.io_Unit   = (struct Unit *) (1<<j);
          audio->AudioRequests[2*j+o].ioa_Request.io_Command  = ADCMD_PERVOL;
          audio->AudioRequests[2*j+o].ioa_Request.io_Flags  = 0;
          audio->AudioRequests[2*j+o].ioa_Period        = MAKE_PERIOD(ac[i].Frequency);
          audio->AudioRequests[2*j+o].ioa_Volume        = (64 * vol * ac[i].Volume) / 10000;

          DoIO((struct IORequest *) &audio->AudioRequests[2*j+o]);
        }
      }
    }   

    for(j = 0; j < 4; j++)
    {
      if(start_status[j])
        audio->Status[j] = start_status[j];
    }
  }
}

struct ASound *ALoadSound(struct Audio *audio, UBYTE *sample, LONG res, LONG length, LONG freq, LONG vol)
{
  struct ASound *sound;
  UBYTE     *buffer;
  LONG      step;
  LONG      i, j;

  if(audio->UseAHI)
  {
    sound = (struct ASound *)memAlloc(sizeof(struct ASound) + length);
        
    if(sound)
    {
      sound->Sound    = audio->NextSound;
      sound->Frequency  = freq;
      sound->Volume   = vol;
          
      memcpy(&sound[1], sample, length);
          
      if(res == 8)
        sound->AHISampleInfo.ahisi_Type = AHIST_M8S;
      else
        sound->AHISampleInfo.ahisi_Type = AHIST_M16S;
          
      sound->AHISampleInfo.ahisi_Address  = &sound[1];
      sound->AHISampleInfo.ahisi_Length = length;
            
      if(!AHI_LoadSound(sound->Sound, AHIST_SAMPLE, &sound->AHISampleInfo, audio->AHIAudioCtrl))
      {
        audio->NextSound++;
        AddTail((struct List *) &audio->Sounds, (struct Node *) sound);
#ifdef POWERUP
        PPCCacheClearE(sound, sizeof(struct ASound), CACRF_ClearD);
#endif
        return(sound);
      }
      
      memFree(sound);
    }
  }
  else
  {
    if(res != 8)
      return(NULL);
    step = (LONG)AResample((ULONG *)&freq, MAX_PAULA_FREQUENCY,(ULONG *) &length, MAX_PAULA_LENGTH);
    
    if(AvailMem(MEMF_CHIP|MEMF_PUBLIC) >= (audio->MinFreeChip + length))
      buffer = (UBYTE *)AllocVec(length, MEMF_CHIP|MEMF_PUBLIC);
    else
      buffer = NULL;

    if(buffer)
      sound = (struct ASound*)memAlloc(sizeof(struct ASound));
    else
    {
      sound = (struct ASound*)memAlloc(sizeof(struct ASound) + length);
      
      if(sound)
        buffer = (UBYTE *) &sound[1];
    }
    
    if(sound)
    {
      sound->Sound            = audio->NextSound;
      sound->Frequency          = freq;
      sound->Volume           = vol;
      sound->AHISampleInfo.ahisi_Address  = buffer;
      sound->AHISampleInfo.ahisi_Length = length;
        
      if(step == 0x100)
        memcpy(buffer, sample, length);
      else
      {
        for(i = 0, j = 0; i < length; i++, j += step)
          buffer[i] = sample[j >> 8];
      }

      audio->NextSound++;
      AddTail((struct List *) &audio->Sounds, (struct Node *) sound);
#ifdef POWERUP
      PPCCacheClearE(sound, sizeof(struct ASound), CACRF_ClearD);
#endif
      return(sound);
    }
    else if(buffer)
      FreeVec(buffer);
  }

  return(NULL);
}

struct ASound *AReadSound(struct Audio *audio, BPTR file)
{
  struct MameSample mame_sample;
  struct ASound   *sound;
  UBYTE       *buffer;
  UBYTE       *sample;
  LONG        step;
  LONG        i, j;
  LONG        frequency;
  LONG        length;
  
  if(Read(file, &mame_sample, sizeof(struct MameSample)) == sizeof(struct MameSample))
  {
    if((mame_sample.ID == MAKE_ID('M','A','M','E')) && mame_sample.Length)
    {
      frequency = INTELULONG(mame_sample.Frequency);
      length    = INTELULONG(mame_sample.Length);
      
      if(audio->UseAHI)
      {
        sound = (struct ASound *)memAlloc(sizeof(struct ASound) + length);
        
        if(sound)
        {
          sound->Sound    = audio->NextSound;
          sound->Frequency  = frequency;
          sound->Volume   = mame_sample.Volume;
          
          Read(file, &sound[1], length);
          
          if((mame_sample.Resolution == 8) || (mame_sample.Resolution == 16))
          {
            if(mame_sample.Resolution == 8)
              audio->AHISampleInfo.ahisi_Type = AHIST_M8S;
            else
              audio->AHISampleInfo.ahisi_Type = AHIST_M16S;
          
            sound->AHISampleInfo.ahisi_Address  = &sound[1];
            sound->AHISampleInfo.ahisi_Length = length;
            
            if(!AHI_LoadSound(sound->Sound, AHIST_SAMPLE, &sound->AHISampleInfo, audio->AHIAudioCtrl))
            {
              audio->NextSound++;
              AddTail((struct List *) &audio->Sounds, (struct Node *) sound);
#ifdef POWERUP
              PPCCacheClearE(sound, sizeof(struct ASound), CACRF_ClearD);
#endif
              return(sound);
            }
          }
          
          memFree(sound);
        }
      }
      else
      {
        if(mame_sample.Resolution != 8)
          return(NULL);

        step = AResample((ULONG*) &frequency, MAX_PAULA_FREQUENCY,(ULONG*) &length, MAX_PAULA_LENGTH);
    
        if(AvailMem(MEMF_CHIP|MEMF_PUBLIC) >= (audio->MinFreeChip + length))
          buffer = (UBYTE*)AllocVec(length, MEMF_CHIP|MEMF_PUBLIC);
        else
          buffer = NULL;

        if(buffer)
          sound = (struct ASound *)memAlloc(sizeof(struct ASound));
        else
        {
          sound = (struct ASound *)memAlloc(sizeof(struct ASound) + length);
      
          if(sound)
            buffer = (UBYTE *) &sound[1];
        }
    
        if(sound)
        {
          sound->Sound            = audio->NextSound;
          sound->Frequency          = frequency;
          sound->Volume           = mame_sample.Volume;
          sound->AHISampleInfo.ahisi_Address  = buffer;
          sound->AHISampleInfo.ahisi_Length = length;
        
          if(step == 0x100)
            Read(file, buffer, length);
          else
          {
            sample = (UBYTE*)AllocVec(INTELULONG(mame_sample.Length), MEMF_PUBLIC);
            
            if(sample)
            {
              Read(file, sample, INTELULONG(mame_sample.Length));
              
              for(i = 0, j = 0; i < length; i++, j += step)
                buffer[i] = sample[j >> 8];
              
              FreeVec(sample);
            }
            else
            {
              if(sound->AHISampleInfo.ahisi_Address != &sound[1])
                FreeVec(sound->AHISampleInfo.ahisi_Address);
              
              memFree(sound);
              sound = NULL;
            }
          }

          if(sound)
          {
            audio->NextSound++;
            AddTail((struct List *) &audio->Sounds, (struct Node *) sound);
#ifdef POWERUP
            PPCCacheClearE(sound, sizeof(struct ASound), CACRF_ClearD);
#endif
            return(sound);
          }
        }
        else if(buffer)
          FreeVec(buffer);
      }
    }
  }

  return(NULL);
}

/****i* audio.c/AResample *************************************************
*
* NAME
*   AResample -- Calculate resample parameters.
*
* SYNOPSIS
*   step = AResample(frequency, maxfrequency, length, maxlength)
*
*   ULONG AResample(ULONG *, ULONG, ULONG *, ULONG);
*
* FUNCTION
*   Calculate resample parameters if the sample is too long or the
*   frequency too high.
*
* INPUTS
*   frequency    - A pointer to the frequency for the sample playback. The
*                  resampled playback frequency will be written back.
*   maxfrequency - Max frequency for a sample. Max frequency for the Amiga
*                  audio hardware is 28867.
*   length       - A pointer to the length of the sample. The resampled
*                  length will be written back.
*   maxlength    - Max length of sample. Max playback length for the
*                  Amiga audio hardware is 131072.
*     
* RESULT
*   step  - Fraction to add to the sample index to get to the next
*           sample that should be included in the resampled version.
*           The fraction is shifted up 8 bits so 0x100 means no
*           resampling needed.
*
* EXAMPLE
*
* NOTES
*
* BUGS
*
* SEE ALSO
*
**************************************************************************/

ULONG AResample(ULONG *freq, ULONG maxfreq, ULONG *length, ULONG maxlength)
{
  ULONG step;

  if(*freq > maxfreq)
  {
    step  = *length;
    *length = (*length * ((maxfreq << 8) / *freq)) >> 8;
    if(*length > maxlength)
      *length = maxlength;
    *freq = (*freq * ((*length<<8) / step))>>8;
    step  = (step<<8) / *length;
  }
  else
  {
    if(*length > maxlength)
    {
      *freq = (*freq * ((maxlength<<8) / *length))>>8;
      step  = (*length<<8) / maxlength;
      *length = maxlength;
    }
    else
      step = 1<<8;
  }
  *length = *length & 0xfffffffc;
  return(step);
}
