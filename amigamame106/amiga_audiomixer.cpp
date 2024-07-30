#include "amiga106_audiostream.h"
#include "sound_krb.h"
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
            INT32 samp = leftmix[i]+rightmix[i];
            if (samp < -32768)
                samp = -32768;
            else if (samp > 32767)
                samp = 32767;
            *ps++ = 0; // test (WORD)samp;
        }
    }
    pFrame->_read = ntodo;
    pFrame->_readlock = 0;

    return pSoundToWrite->m_nbSampleToFill;
}
