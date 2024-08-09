#include "amiga106_audiostream.h"
#include "sound_krb.h"
#include "streams.h"
#include <stdio.h>
// return how much done.
ULONG soundMixOnThread( sSoundToWrite *pSoundToWrite)
{
    UWORD ntodo = (UWORD) pSoundToWrite->m_nbSampleToFill;
    WORD *ps = pSoundToWrite->m_pBuffer;

    // note: mame frames and AHI buffers are meant to have the exact same size.

    SampleFrame *pFrame = &SampleFrames[(currentSampleFrame-1)&3]; // test if missed previous
    if(pFrame->_writelock || pFrame->_read>=pFrame->_written)
    {
        pFrame = &SampleFrames[currentSampleFrame];
    }

    if(pFrame->_writelock || pFrame->_read >=pFrame->_written )
    {
        //pFrame = &SampleFrames[(currentSampleFrame-1)&3];
        //if(pFrame->_written ==0) return pSoundToWrite->m_nbSampleToFill;

        // just mirror alternate buffer if late.
        UWORD lh = ((UWORD) pSoundToWrite->m_nbSampleToFill)>>1;
        // point end of prev buffer

        if(pSoundToWrite->m_stereo)
        {
            WORD *pr = pSoundToWrite->m_pPrevBuffer+(pSoundToWrite->m_nbSampleToFill<<1);
            LONG *psl = (LONG *)ps; // copy 4 bytes
            LONG *prl = (LONG *)pr;
            for(UWORD i=0; i<lh ; i++ )
            {
                *psl++ = *--prl;
            }
            for(UWORD i=0; i<lh ; i++ )
            {
                *psl++ = *prl++;
            }
        }
        {
            WORD *pr = pSoundToWrite->m_pPrevBuffer+(pSoundToWrite->m_nbSampleToFill);
            // mono
            for(UWORD i=0; i<lh ; i++ )
            {
                *ps++ = *--pr;
            }
            for(UWORD i=0; i<lh ; i++ )
            {
                *ps++ = *pr++;
            }
        }
        return pSoundToWrite->m_nbSampleToFill; // this will reuse
    }
    pFrame->_readlock = 1;
   // UWORD sampleleft = (UWORD)(pFrame->_written-pFrame->_read);
   // if(sampleleft<minl) minl=sampleleft;
    INT32 *leftmix = pFrame->_leftmix;
    INT32 *rightmix = pFrame->_rightmix;

    if(pSoundToWrite->m_stereo)
    {
        for(UWORD i=0; i<ntodo ; i++ )
        {
            /* clamp the left side */
            INT32 samp = leftmix[i];
            if (samp < -32768)
                samp = -32768;
            else if (samp > 32767)
                samp = 32767;
            *ps++ = (WORD)samp;

            /* clamp the right side */
            samp = rightmix[i];
            if (samp < -32768)
                samp = -32768;
            else if (samp > 32767)
                samp = 32767;
            *ps++ = (WORD)samp;

        }

    } else
    {
        for(UWORD i=0; i<ntodo ; i++ )
        {
            INT32 samp = leftmix[i]; //on mono now, all sounds forced to left. +rightmix[i];
            if (samp < -32768)
                samp = -32768;
            else if (samp > 32767)
                samp = 32767;
            *ps++ = (WORD)samp;
        }
    }
    pFrame->_read = ntodo;
    pFrame->_readlock = 0;

    return pSoundToWrite->m_nbSampleToFill;
}

struct stream_input
{
	sound_stream *stream;						/* pointer to the input stream */
	struct stream_output *source;				/* pointer to the sound_output for this source */
	stream_sample_t *resample;					/* buffer for resampling to the stream's sample rate */
	UINT32			source_frac;				/* fractional position within the source buffer */
	UINT32			step_frac;					/* source stepping rate */
	UINT32			resample_in_pos;			/* resample index where next sample will be written */
	UINT32			resample_out_pos;			/* resample index where next sample will be read */
	INT16			gain;						/* gain to apply to this input */
};

struct stream_output
{
	stream_sample_t *buffer;					/* output buffer */
	UINT32			cur_in_pos;					/* sample index where next sample will be written */
	UINT32			cur_out_pos;				/* sample index where next sample will be read */
	int				dependents;					/* number of dependents */
	INT16			gain;						/* gain to apply to the output */
};


extern "C" {
    void resample_input_stream(struct stream_input *input, int samples);
}

#define FRAC_BITS						14
#define FRAC_ONE						(1 << FRAC_BITS)
#define FRAC_MASK						(FRAC_ONE - 1)

void resample_input_stream(struct stream_input *input, int samples)
{
	stream_sample_t *dest = input->resample + input->resample_in_pos;
	stream_sample_t *source = input->source->buffer;
	INT16 gain = (input->gain * input->source->gain) >> 8;
	UINT32 pos = input->source_frac;
	UINT32 step = input->step_frac;

	//printf(" gain:%d\n",gain);
	/* perfectly matching */
	if (step == FRAC_ONE)
	{
        UINT32 rpos = pos >> FRAC_BITS;
        pos += samples<<FRAC_BITS;
        if(gain == 0x0100)
        {
            while (samples--) *dest++ = source[rpos++];
        }
        else if(gain == 0x0040) // very common case, just div 4.
        {
            while (samples--) *dest++ = source[rpos++] >> 2;
        }
        else if(gain == 0x0080) // may happen
        {
            while (samples--) *dest++ = source[rpos++] >> 1;
        } else
        {
            while (samples--) *dest++ = (source[rpos++] * gain) >> 8;
        }
	}

	/* input is undersampled: use linear interpolation */
    // KRB: actualy is perfect until *2-1
	else if (step < ((FRAC_ONE<<1)-1))   //was: if (step < FRAC_ONE)
	{
		while (samples--)
		{
			/* compute the sample */
			UINT32 posdiv = (pos >> FRAC_BITS);
			UINT16 posmask = ((UINT16)pos & FRAC_MASK);
			UINT16 invposmask = ((UINT16)FRAC_ONE -posmask);
			INT32 sample  = source[posdiv] * invposmask;
			sample += source[posdiv + 1] * posmask;
			sample >>= FRAC_BITS;
			*dest++ = (sample * gain) >> 8;
			pos += step;
		}
	}

	/* input is oversampled: sum the energy */
	else
	{
	// printf(" OVERSAMPLED ! %08x\n",step);
		/* use 8 bits to allow some extra headroom */
		/*original
		int smallstep = step >> (FRAC_BITS - 8);

		while (samples--)
		{
			int tpos = pos >> FRAC_BITS;
			int remainder = smallstep;
			int scale;

			// compute the sample
			scale = (FRAC_ONE - (pos & FRAC_MASK)) >> (FRAC_BITS - 8);
			sample = source[tpos++] * scale;
			remainder -= scale;
			while (remainder > 0x100)
			{
				sample += source[tpos++] * 0x100;
				remainder -= 0x100;
			}
			sample += source[tpos] * remainder;
			sample /= smallstep;

			*dest++ = (sample * gain) >> 8;
			pos += step;
		}
		*/

#ifdef BIG_INTERPOLATION
        int smallstep = step >> (FRAC_BITS - 8);
        int divider = (1<<14)/smallstep;
        divider = (divider*gain)>>8;

		while (samples--)
		{
			int tpos = pos >> FRAC_BITS;
			int remainder = smallstep;
			int scale;

			// compute the sample
			scale = (FRAC_ONE - (pos & FRAC_MASK)) >> (FRAC_BITS - 8);
			INT32 sample = source[tpos++] * scale;
			remainder -= scale;
			while (remainder > 0x100)
			{
				sample += source[tpos++]<<8; // * 0x100; common...
				remainder -= 0x100;
			}
			sample += source[tpos] * remainder;
			//sample /= smallstep;
			// *dest++ = (sample * gain) >> 8;

			*dest++ = (sample * divider) >> 14;
            pos += step;
		}
#else
        int hstep = (step >> (1+FRAC_BITS));
        if(gain== 0x0100)
        {
            while (samples--)
            {
                UINT32 rpos = pos >> FRAC_BITS;
                *dest++ = (source[rpos]  + source[rpos+hstep])>>1;
                pos += step;
            }
        } else if(gain == 0x0040)
        {
            while (samples--)
            {
                UINT32 rpos = pos >> FRAC_BITS;
                *dest++ = (source[rpos]  + source[rpos+hstep])>>3;
                pos += step;
            }
        } else
        {
            UINT16 gain2=gain>>1; // >>1 to avoid one shift
            while (samples--)
            {
                /* compute the sample */
                UINT32 rpos = pos >> FRAC_BITS;
                INT32 sample  = (source[rpos]  + source[rpos+hstep]);
                *dest++ = (sample * gain2) >> 8;
                pos += step;
            }
        }
#endif


	}

	/* update the input parameters */
	input->resample_in_pos = dest - input->resample;
	input->source_frac = pos;
}



