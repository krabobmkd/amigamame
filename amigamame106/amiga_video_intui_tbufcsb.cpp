#include "amiga_video_intui_tbufcsb.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdlib.h>
#include <stdio.h>
extern "C" {
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
}

TripleBuffer_CSB::TripleBuffer_CSB(Intuition_Screen &screen)
 : TripleBuffer(), _screen(screen),_dbufport(NULL),_isSignalToWait(0)
{
    //    printf(" TripleBuffer_CSB alloc\n");
    memset(&_screenBuffer[0],0,sizeof(_screenBuffer));
}
TripleBuffer_CSB::~TripleBuffer_CSB()
{
    close();
}

int TripleBuffer_CSB::init()
{
    _tripleBufferInitOk = 0;
    if(!_screen.screen()) return 0;

    if(_screenBuffer[0]._pScreenBuffer ||
         _screenBuffer[1]._pScreenBuffer ||
         _screenBuffer[2]._pScreenBuffer)
            close();

    // the AGA/CGX unified os3 way
    _screenBuffer[0]._pScreenBuffer = AllocScreenBuffer(_screen.screen(),NULL,SB_SCREEN_BITMAP);
    if(!_screenBuffer[0]._pScreenBuffer) return 0;

    for(int i=0; i<3 ;i++)
    {
        SBuffer &sb = _screenBuffer[i];
        if(!sb._pScreenBuffer)
            sb._pScreenBuffer = AllocScreenBuffer(_screen.screen(),NULL,SB_COPY_BITMAP );
        if(! sb._pScreenBuffer)
        {
            close();
            return 0;
        }
        // init rastport, needed by some draw functions.
        // " Initialize a RastPort structure to standard values."
        // I feel super 100% reasured by this marvellous function.
        InitRastPort( &sb._rport );
        sb._rport.BitMap = sb._pScreenBuffer->sb_BitMap;
        sb._pScreenBuffer->sb_DBufInfo->dbi_UserData1 = (APTR)i;
    }
    _dbufport = CreateMsgPort();
    if(!_dbufport) {
        close();
        return 0;
    }
    _sigbitmask = 1 << _dbufport->mp_SigBit ;

    _lastIndexDrawn = 0;
    _indexToDraw = 1;
    _tripleBufferInitOk = 1;

    return 1;
}
void TripleBuffer_CSB::close()
{
    if(!_screen.screen()) return;

    if(_isSignalToWait)
    {
        Wait(_sigbitmask);
        struct Message *dbmsg;
        // flush the amiga way. ReplyMsg() hangs for this one.
	    while ( dbmsg = GetMsg( _dbufport ) ) { }
	    _isSignalToWait =0;
    }

    for(int i=2;i>=0;i--)
    {
        SBuffer &sb = _screenBuffer[i];

        if(sb._pScreenBuffer) FreeScreenBuffer(_screen.screen(),sb._pScreenBuffer);
        sb._pScreenBuffer = NULL;
    }
    if ( _dbufport ) DeleteMsgPort( _dbufport );
    _dbufport = NULL;
}
RastPort *TripleBuffer_CSB::rastPort()
{
    if(!_tripleBufferInitOk) return NULL;
    return &_screenBuffer[_indexToDraw]._rport;
}
BitMap *TripleBuffer_CSB::bitmap()
{
    if(!_tripleBufferInitOk) return NULL;
    return _screenBuffer[_indexToDraw]._pScreenBuffer->sb_BitMap;
}

void TripleBuffer_CSB::waitFrame()
{
    if(_isSignalToWait)
    {
        // Wait only if previius screen never displayed.
        // only happens when we goes very fast.
        // theorically does best vertical synch.
        // this would do a perfect 60Hz timer, with Process sleep.        
        Wait(1<<_dbufport->mp_SigBit);

        // flush the Amiga message this way. ReplyMsg() actually hangs in that case.
        struct Message *dbmsg;
	    while ( dbmsg = GetMsg( _dbufport ) ) {}

         // unlink previous buffer to message port .
        _screenBuffer[_lastIndexDrawn]._pScreenBuffer->
            sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = NULL;
        _isSignalToWait =0;
    }
}

void TripleBuffer_CSB::afterBufferDrawn()
{
    // may apply double buffer
    if(!_tripleBufferInitOk) return;

    // may wait previous ChangeScreenBuffer() acknoledge call, which happens.
    // this is sent once the previous screen has been seen one frame.
    //no, done elsewhere
    waitFrame();

    SBuffer &sb = _screenBuffer[_indexToDraw];
    ScreenBuffer *psb = sb._pScreenBuffer;

    psb->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
    psb->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = _dbufport;

    ULONG success =  ChangeScreenBuffer(_screen.screen(), psb);
    if(!success) {
       // hardware locked for any reason,...
       WaitTOF();
       return;
    } else
    {
        // want next call to wait ChangeScreenBuffer() effective.
        _isSignalToWait = 1;
        _lastIndexDrawn = _indexToDraw;
        _indexToDraw++;
        if(_indexToDraw==3) _indexToDraw=0;
    }

}
