#ifndef AUDIO_H
#define AUDIO_H
/**************************************************************************
 *
 * Copyright (C) 1999 Mats Eirik Hansen (mats.hansen@triumph.no)
 *
 * $Id: audio.h,v 1.1 1999/04/28 18:50:56 meh Exp $
 *
 * $Log: audio.h,v $
 * Revision 1.1  1999/04/28 18:50:56  meh
 * Initial revision
 *
 *
 *************************************************************************/

extern "C" {
#include <devices/ahi.h>
}
#ifndef BPTR
//from dos/dos.h
typedef long  BPTR;
#endif


struct MameSample
{
    ULONG ID;     /* 'MAME'. */
    ULONG Length;   /* NB! little endian. */
    ULONG Frequency;  /* NB! little endian. */
    UBYTE Resolution; /* 8/16 bit. */
    UBYTE Volume;
    UWORD dummy;
    UBYTE Data[0];
};

#define AA_UseAHI   (TAG_USER)
#define AA_Channels   (TAG_USER+1)
#define AA_MaxSounds  (TAG_USER+2)
#define AA_MapChannel0  (TAG_USER+3)
#define AA_MapChannel1  (TAG_USER+4)
#define AA_MapChannel2  (TAG_USER+5)
#define AA_MapChannel3  (TAG_USER+6)
#define AA_MinFreeChip  (TAG_USER+7)

struct AChannel
{
  LONG  Flags;
  LONG  Frequency;
  LONG  Volume;
  LONG  Length;
  LONG  Sound;
  BYTE  *Buffer;
};

#define ACF_Loop      0x00000001
#define ACF_SetFrequency  0x00000002
#define ACF_SetVolume   0x00000004
#define ACF_SetLength   0x00000008
#define ACF_SetSample   0x00000010
#define ACF_SetSound    0x00000020
#define ACF_Stop      0x00000040
#define ACF_Restart     0x00000080

struct AChannelArray
{
  struct Audio    *Audio;
  LONG            Size;
  LONG            Length;
  LONG            Flags;
  LONG            MasterVolume;
  struct AChannel *Channels;
};

struct Audio
{
  LONG                      UseAHI;
  struct MsgPort            *MsgPort;
  struct AHIRequest         *AHIRequest;
  struct Library            *AHIBase;
  struct TagItem            *AHITags;
  struct AHIAudioCtrl       *AHIAudioCtrl;
  struct AHISampleInfo      AHISampleInfo;
  struct AHIEffMasterVolume AHIEffMasterVolume;
  struct IOAudio            *AudioRequests;
  ULONG                     MinFreeChip;
  LONG                      MasterVolume;
  LONG                      Channels;
  LONG                      NextSound;
  struct MinList            Sounds;
  struct AChannelArray      *ChannelArray;
  LONG                      BufferSize;
  BYTE                      *Buffers;
  BYTE                      Status[4];
  BYTE                      MapChannels[4];
  ULONG                     FrequencyModifier[4];
};

#define AS_IDLE     0
#define AS_PLAYING1 1
#define AS_PLAYING2 2

struct ASound
{
  struct MinNode       Node;
  LONG                 Sound;
  LONG                 Frequency;
  LONG                 Volume;
  struct AHISampleInfo AHISampleInfo;
};

void AllocAudio(Tag,...);
void FreeAudio();
struct AChannelArray *AAllocChannelArray( LONG length);
void AFreeChannelArray(struct AChannelArray *channelarray);
void ASetChannelFrame(struct AChannelArray *channelarray);
struct ASound *ALoadSound( UBYTE *sample, LONG res, LONG len, LONG freq, LONG vol);
struct ASound *AReadSound(struct Audio *audio, BPTR file);
ULONG AResample(ULONG *freq, ULONG maxfreq, ULONG *length, ULONG maxlength);

static inline void APlaySample(struct AChannelArray *ca, LONG channel, LONG frequency,
                               LONG volume, LONG length, UBYTE *sample, BOOL loop)
{
  ca->Channels[channel].Frequency = frequency;
  ca->Channels[channel].Volume    = volume;
  ca->Channels[channel].Length    = length;
  ca->Channels[channel].Sound     = (LONG) sample;
  ca->Channels[channel].Flags     = ACF_SetFrequency|ACF_SetVolume|ACF_SetLength|
                                    ACF_SetSample|((loop)?ACF_Loop:0);
}

static inline void APlaySound(struct AChannelArray *ca, LONG channel, LONG frequency,
                              LONG volume, LONG sound, BOOL loop)
{
  ca->Channels[channel].Frequency = frequency;
  ca->Channels[channel].Volume    = volume;
  ca->Channels[channel].Sound     = sound;
  ca->Channels[channel].Flags     = ACF_SetFrequency|ACF_SetVolume|
                                    ACF_SetSound|((loop)?ACF_Loop:0);
}

static inline void ASetMasterVolume(struct AChannelArray *ca, LONG volume)
{
  ca->MasterVolume = volume;
  ca->Flags        = ACF_SetVolume;
}

static inline void ASetFrequency(struct AChannelArray *ca, LONG channel, LONG frequency)
{
  ca->Channels[channel].Frequency =  frequency;
  ca->Channels[channel].Flags     |= ACF_SetFrequency;
}

static inline void ASetVolume(struct AChannelArray *ca, LONG channel, LONG volume)
{
  ca->Channels[channel].Volume =  volume;
  ca->Channels[channel].Flags  |= ACF_SetVolume;
}

static inline void AStopChannel(struct AChannelArray *ca, LONG channel)
{
  ca->Channels[channel].Flags = ACF_Stop;
}

static inline void ARestartChannel(struct AChannelArray *ca, LONG channel)
{
  ca->Channels[channel].Flags = ACF_Restart;
}

#endif
