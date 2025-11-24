/******************************************************************************
 * amiga_video_drawprogress.h
 *
 *       ╔════════════════════════════════╗
 *       ║  PROGRESS BAR RENDERER         ║
 *       ║  ┌──────────────────┐          ║
 *       ║  │ ████████░░░░  60%│          ║
 *       ║  │ Loading...       │          ║
 *       ║  └──────────────────┘          ║
 *       ║                                ║
 *       ║  Smooth loading animation!     ║
 *       ╚════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 *****************************************************************************/

#ifndef AMIGA_VIDEO_INTUITIONDP_H
#define AMIGA_VIDEO_INTUITIONDP_H

#include <vector>
#include "amiga_video_intuition.h"
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

// This has to manage its own resource...
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

/* Loading bars drawn! Progress visualized!
 *       @..@
 *      (----)  <-- Sloth loads at its own pace
 *     ( >__< )
 */
