#ifndef AMIGA_VIDEO_INTUITION_TBUFCSB_H
#define AMIGA_VIDEO_INTUITION_TBUFCSB_H

#include "amiga106_video_intuition.h"

/**
    \class TripleBuffer_CSB
     Triple buffer using OS3 Intuition's ChangeScreenBuffer
     which is the "official way" for fast triple buffering.
     ... But it seems the pistorm Picasso Drivers has problem with it.
    it occurs enormous slow down and bugs (when message ports are not used ?).

    Let's use it with the message port and wait signals.

    In a perfect world it's the same code for CGX/Picasso/AGA.

    Highly based on:
    Amiga Developer CD v2.1
        /Contributions/Haage_&_Partner/Storm-Projects/NDKExamples1
        /intuition/doublebuffer.c

*/
class TripleBuffer_CSB : public TripleBuffer
{
public:
    TripleBuffer_CSB(Intuition_Screen &screen);
    ~TripleBuffer_CSB();
    // - - -triple buffer management
    struct SBuffer {
         ScreenBuffer *_pScreenBuffer;
         struct RastPort _rport;  // may need this per screen buffer for special draw func.
    };

    SBuffer _screenBuffer[3];
    struct MsgPort *_dbufport;
    int _sigbitmask;
    int _isSignalToWait;

    int init() override;
    void close() override;
    RastPort *rastPort() override;
    BitMap *bitmap() override;
    //int beforeBufferDrawn() override;
    void waitFrame();
    void afterBufferDrawn() override;
    Intuition_Screen &_screen;
};



#endif
