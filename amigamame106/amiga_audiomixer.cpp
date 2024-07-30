#include "amiga106_audiostream.h"
#include "sound_krb.h"

// return how much done.
ULONG soundMixOnThread( sSoundToWrite *pSoundToWrite)
{
    UWORD ntodo = pSoundToWrite->m_nbSampleToFill;
    WORD *ps = pSoundToWrite->m_pBuffer;

    // note: mame frames and AHI buffers are meant to have the exact same size.

    SampleFrame *pFrame = &SampleFrames[currentSampleFrame];
    if(pFrame->_locked || pFrame->_read ==pFrame->_written )
    {
//        pFrame = &SampleFrames[(currentSampleFrame-1)&3];
//        if(pFrame->_written ==0) return pSoundToWrite->m_nbSampleToFill;
//        pFrame->_read=0;
        // just mirror alternate buffer if late.
        UWORD lh = ((UWORD) pSoundToWrite->m_nbSampleToFill)>>1;
        WORD *pr = pSoundToWrite->m_pPrevBuffer;
        if(pSoundToWrite->m_stereo)
        {
            LONG *psl = (LONG *)ps;
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
    UWORD minl = ntodo;
    UWORD sampleleft = (UWORD)(pFrame->_written-pFrame->_read);
    if(sampleleft<minl) minl=sampleleft;
    INT32 *leftmix = pFrame->_leftmix;
    INT32 *rightmix = pFrame->_rightmix;

    if(pSoundToWrite->m_stereo)
    {
        for(UWORD i=0; i<minl ; i++ )
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
        for(UWORD i=0; i<minl ; i++ )
        {
            INT32 samp = leftmix[i]+rightmix[i];
            if (samp < -32768)
                samp = -32768;
            else if (samp > 32767)
                samp = 32767;
            *ps++ = 0; // test (WORD)samp;
        }
    }
    pFrame->_read += minl;


    return pSoundToWrite->m_nbSampleToFill;
}
