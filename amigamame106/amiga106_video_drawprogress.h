#ifndef AMIGA_VIDEO_INTUITIONDP_H
#define AMIGA_VIDEO_INTUITIONDP_H

#include <vector>
#include "amiga106_video_intuition.h"
#include <proto/graphics.h>

// all these to not have progressbar glitches...
class IntuiBufferedDrawing
{
public:
    IntuiBufferedDrawing();
    virtual ~IntuiBufferedDrawing();

protected:
   int _clearedscreenw,_clearedscreenh;

    // -- temp raster for less glitch when drawings
    BitMap *_BgBm;
    struct RastPort *_bgrp;
    struct Layer_Info *_layerinfo;
    struct Layer *_layer;

    int _bmx1,_bmy1,_bmw,_bmh;

    void clearBm(RastPort *rp, int pen,int w,int h);
    void finalBlit(RastPort *rp);

    void setUpdatableArea(RastPort *rp,int x1,int y1,int w,int h);
    void closeTempBm();
    // - - -
    void SetAPenTmp(int pen);
    void RectFillTmp(int x1,int y1,int xm,int ym);
    void drawTextTmp(TextFont *font,int x1,int y1,int pen,const char *ptext);
    void maskBitmapTmp(BitMap *pbm,int x1,int y1,int w,int h,UWORD minterm, PLANEPTR ptrmask);

};

// thinhy got to manage own ressource...
class IntuiProgressBar : public IDrawProgress, public IntuiBufferedDrawing
{
public:
    IntuiProgressBar(IntuitionDrawable *drawable);
    ~IntuiProgressBar();
    void drawProgress(int per256, int enm) override;
protected:
    IntuitionDrawable   *_drawable;
    UWORD *_pChipLogoAlloc;

//    int _pens[eEnd];
//    void obtainPens();
//    void releasePens();
    struct TextFont *_font;




};

#endif
