/*
 * amiga_video_cgx.cpp
 * Purpose: CyberGraphX (RTG) video driver - Main implementation
 *
 * ╔════════════════════════════════════════════════════════════════════════╗
 * ║            🖥️ CYBERGRAPHX RTG VIDEO DRIVER 🎮                         ║
 * ║  ┌──────────────────────────────────────────────────────────────┐    ║
 * ║  │                                                               │    ║
 * ║  │   MAME Bitmap  ──►  Pixel Convert  ──►  RTG Framebuffer     │    ║
 * ║  │                                                               │    ║
 * ║  │   ┌──────────┐      ┌──────────┐      ┌──────────┐          │    ║
 * ║  │   │ Game Src │ ───► │  Tracer  │ ───► │  Screen  │          │    ║
 * ║  │   │ CLUT/RGB │      │ Engine   │      │ RGB15/32 │          │    ║
 * ║  │   └──────────┘      └──────────┘      └──────────┘          │    ║
 * ║  │                                                               │    ║
 * ║  │   Features:                                                  │    ║
 * ║  │   • Direct CPU rendering to locked bitmaps                   │    ║
 * ║  │   • 16-bit and 32-bit optimized paths                        │    ║
 * ║  │   • Automatic screen mode selection (OwnCGXBestModeID)       │    ║
 * ║  │   • Palette remapping for CLUT games                         │    ║
 * ║  │   • Rotation support (90°/180°/270°)                         │    ║
 * ║  │   • Fullscreen and windowed modes                            │    ║
 * ║  │   • Triple buffering support                                 │    ║
 * ║  │   • Works with Picasso96, CyberGraphX, SAGA                  │    ║
 * ║  └──────────────────────────────────────────────────────────────┘    ║
 * ║      Bringing arcade pixels to RTG cards since 2025! 🚀               ║
 * ╚════════════════════════════════════════════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include "amiga_video_cgx.h"
#include "amiga_video_tracers_clut16.h"
#include "amiga_video_tracers_argb32.h"
#include "amiga_video_remap.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>

// Amiga
extern "C" {
    #include <cybergraphx/cybergraphics.h>
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <graphics/modeid.h>
    #include <graphics/displayinfo.h>
    #include <graphics/clip.h>
    #include <utility/hooks.h>
}

extern "C" {
    // from mame
    #include "mame.h"
    #include "mamelog.h"
    #include "video.h"
    #include "mamecore.h"
    #include "osdepend.h"
    #include "palette.h"
}
#include <stdio.h>
#include <stdlib.h>

extern struct Library *CyberGfxBase;

template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }


// because BestCModeIDTagList is sabotaged in recent P96 drivers.
// do some search based on "correct height first" - it seems that is what correct drivers does.
ULONG OwnCGXBestModeID(int w,int h,int depth)
{
    if(depth >8 && depth <16 ) depth=16;
    else if(depth>16 && depth<24) depth=24;
    else if(depth>24) depth=32;

    ULONG minHeightExcluded=0;
    while(1)
    {
        // - -  primarily bother about height
        ULONG tid = NextDisplayInfo(INVALID_ID);
        ULONG bestHeightToTest=0;
        ULONG bestHeightToTestErr=0xffffffff;
        for(;tid != INVALID_ID; tid = NextDisplayInfo(tid))
        {   // here we only want RTG screens.
            if(!IsCyberModeID(tid) || (tid&0x0000ffff) ==0 ) continue; // escape new P96 bugged "ghost modes with RGB inverted."

            ULONG mdepth = GetCyberIDAttr( CYBRIDATTR_DEPTH, tid );
            if(mdepth != depth) continue;

            ULONG hm = GetCyberIDAttr( CYBRIDATTR_HEIGHT, tid );

            if(hm>=h && hm >minHeightExcluded) {
                ULONG err = hm-h;
                if(err<bestHeightToTestErr) {
                    bestHeightToTestErr = err;
                    bestHeightToTest = hm;
                }
            }
        }
        if(bestHeightToTest == 0)
        {
            // this is the return case when no mode found.
            // ultimately, we could just search mode with less depth.
            // there could be old gfx cards that manages just 16b or 8b modes...
            if(depth == 32) return OwnCGXBestModeID(w,h,24);
            if(depth == 24) return OwnCGXBestModeID(w,h,16);
            if(depth == 16) return OwnCGXBestModeID(w,h,8);
            return INVALID_ID;
        }
        // - - - - then only bother for width, amongst modes with selected height...
        int errorWidth= 0xffff;
        ULONG bestmode = INVALID_ID;

        for(tid = NextDisplayInfo(INVALID_ID);tid != INVALID_ID; tid = NextDisplayInfo(tid))
        {
            if(!IsCyberModeID(tid) || (tid&0x0000ffff) ==0 ) continue; // escape new P96 bugged "ghost modes with RGB inverted."

            ULONG mdepth = GetCyberIDAttr( CYBRIDATTR_DEPTH, tid );
            if(mdepth != depth) continue;

            ULONG hm = GetCyberIDAttr( CYBRIDATTR_HEIGHT, tid );
            if(hm != bestHeightToTest) continue; // what we selected in the first loop.

            ULONG wm = GetCyberIDAttr( CYBRIDATTR_WIDTH, tid );
            ULONG pixfmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT, tid );

            int e = wm-w;

            if(e<0) continue;
            // ultimately, if we asked a 32b mode, we would prefer RGBA32 to BGRA32.
            if(depth ==16 && pixfmt != PIXFMT_RGB16) e++; //  would be faster than PIXFMT_RGB16PC
            if(depth ==32 && pixfmt != PIXFMT_RGBA32) e++; // RGBA would be faster than BGRA
            if(e<errorWidth)
            {
                bestmode = tid;
                errorWidth = e;
            }
        }
        if(bestmode != INVALID_ID) return bestmode;
        // if height correct but no width big enough, had to try next bigger height...

        minHeightExcluded = bestHeightToTest;
    } // end while

    return INVALID_ID;
}

bool Intuition_Screen_CGX::useForThisMode(ULONG modeID REGUFT(d0), int flags REGUFT(d1))
{
    ULONG someModeId;

    if(!CyberGfxBase)
    {
        return false;
     }
    if (modeID != INVALID_ID)
    {
        // if screen Id explicit, we know if native or cgx.
        return (bool)IsCyberModeID(modeID);
    }
    // case where some p96 or cgx installed but no monitor activated.
    // check a standard resolution
    if((flags & DISPFLAG_USEOWNCGXBESTMODE)!=0) {
        someModeId = OwnCGXBestModeID(320,240,8);
    } else
    {
        struct TagItem cgxtags[]={
            CYBRBIDTG_NominalHeight,320,
            CYBRBIDTG_NominalWidth,240,
            CYBRBIDTG_Depth,8,
            TAG_DONE,0 };

        someModeId = BestCModeIDTagList(cgxtags);
    }

    if( someModeId == INVALID_ID) return false;
    return (bool)IsCyberModeID(someModeId);
}
bool Intuition_Window_CGX::useForWbWindow()
{
    if(!CyberGfxBase) return false;
    Screen *pWbScreen=NULL;
    if ((pWbScreen = LockPubScreen(NULL)))
    {
        ULONG wbIsCgxManageable = GetCyberMapAttr(pWbScreen->RastPort.BitMap,CYBRMATTR_ISCYBERGFX);
        ULONG wbColorDepth = GetCyberMapAttr(pWbScreen->RastPort.BitMap,CYBRMATTR_DEPTH);
        UnlockPubScreen(NULL,pWbScreen);
        return (wbIsCgxManageable !=0 && wbColorDepth >8); // if cgx 8bit, use OS3 functions.
    }
    return false;
}
// - - - - - - - -shared

Drawable_CGX::Drawable_CGX(IntuitionDrawable &drawable)
 : _drawable(drawable),_pRemap(NULL),_useIntuitionPalette(false)
 , _PixelFmt(0),_PixelBytes(1)
 , directDrawARGB32(NULL)
 , _colorsIndexLength(0)
 , _video_attributes(0)
{
}
Drawable_CGX::~Drawable_CGX()
{
    if(_pRemap) delete _pRemap;
}
void Drawable_CGX::initRemapTable()
{
    if(_useIntuitionPalette) // actually means dest is private screen 8bit
    {

        // 8bits screen colors will be managed with LoadRGB32 and direct pixel copy (no clut).
    //    _pRemap = new Paletted_Screen8(_drawable.screen());
        if(_video_attributes & VIDEO_RGB_DIRECT)
        {
            if(_video_attributes & VIDEO_NEEDS_6BITS_PER_GUN)
            {
                _pRemap = new Paletted_Screen8ForcePalette_32b(_drawable.screen());
            } else
            {
                _pRemap = new Paletted_Screen8ForcePalette_15b(_drawable.screen());
            }
        } else
        {
            // use exact palette index and update if nb color fits.
            Screen *pScreen = _drawable.screen();
            int privScreenNbColors = 1<<(pScreen->RastPort.BitMap->Depth);
            // printf("_colorsIndexLength:%d privScreenNbColors:%d\n",
            //     _colorsIndexLength,privScreenNbColors);
            if((_colorsIndexLength+10)<=(privScreenNbColors))
                     _pRemap = new Paletted_Screen8(pScreen);
            // force fixed palette and manage large index to this index at palette change.
            else
            {
             _pRemap = new Paletted_Screen8ForcePalette(pScreen);
            }

        }
    } else
    {
        if((_video_attributes & VIDEO_RGB_DIRECT)==0 &&
            _colorsIndexLength>0)
        {
            _pRemap = new Paletted_CGX(_colorsIndexLength,_PixelFmt,_PixelBytes);
        } else
        if((_video_attributes & VIDEO_RGB_DIRECT)!=0 /*&& params._driverDepth == 15*/)
        {
            Paletted_CGX *p =  new Paletted_CGX(_colorsIndexLength,_PixelFmt,_PixelBytes);
            _pRemap = p;
            p->updatePaletteRemap15b(); // once for all.
        }
    }
}
void Drawable_CGX::close()
{
    if(_pRemap) delete _pRemap;
    _pRemap = NULL;

}

void Drawable_CGX::prepareDirectFrame(_mame_display *display, cgxDirectFrame &frame)
{
    // - - update palette if exist and is needed.
    if(!isSourceRGBA32() && _pRemap &&
        ((display->changed_flags & GAME_PALETTE_CHANGED) !=0 || _pRemap->needRemap()))
    {
        _pRemap->updatePaletteRemap(display);
    }

    // applied width height if using scale or not, and centering.
    int sourcewidth,sourceheight;
    _drawable.getGeometry(display,frame.cenx,frame.ceny,frame.ww,frame.hh,sourcewidth,sourceheight);

    mame_bitmap *bitmap = display->game_bitmap;

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };
    frame.source = ddsource;
}

void Drawable_CGX::drawDirectFrame(BitMap *pBitmap, cgxDirectFrame &frame,
                                   int shiftx, int shifty, const Rectangle *clip)
{
    if(!pBitmap) return;

    directDrawScreen ddscreen;
    ULONG bmwidth,bmheight;

    // the base address is only valid inside the lock: one lock per call,
    // and no library call inside.
    APTR hdl = LockBitMapTags(pBitmap,
                              LBMI_WIDTH,(ULONG)&bmwidth,
                              LBMI_HEIGHT,(ULONG)&bmheight,
                              LBMI_BYTESPERROW,(ULONG)&ddscreen._bpr,
                              LBMI_BASEADDRESS,(ULONG)&ddscreen._base,
                              TAG_DONE);
    if(!hdl) return;

    ddscreen._clipX1 = 0;
    ddscreen._clipY1 = 0;
    ddscreen._clipX2 = (WORD)bmwidth;
    ddscreen._clipY2 = (WORD)bmheight;
    if(clip)
    {   // Rectangle max is included, clip max is excluded.
        if(clip->MinX > ddscreen._clipX1) ddscreen._clipX1 = clip->MinX;
        if(clip->MinY > ddscreen._clipY1) ddscreen._clipY1 = clip->MinY;
        if(clip->MaxX+1 < ddscreen._clipX2) ddscreen._clipX2 = clip->MaxX+1;
        if(clip->MaxY+1 < ddscreen._clipY2) ddscreen._clipY2 = clip->MaxY+1;
    }

    directDrawParams p{&ddscreen,&frame.source,
        frame.cenx+shiftx,frame.ceny+shifty,frame.ww,frame.hh};

    if(ddscreen._clipX1 >= ddscreen._clipX2 || ddscreen._clipY1 >= ddscreen._clipY2)
    {
        // nothing visible
    } else
    if(isSourceRGBA32())
    {
        // this function pointer has been choosen for the right _PixelFmt and avoid an ugly switch.
        if(directDrawARGB32) directDrawARGB32(&p);
        else if(_pRemap) _pRemap->directDraw(&p); // 8bit cases
    } else
    if(_pRemap)
    {
        switch(_PixelFmt) {
            case PIXFMT_RGB15:case PIXFMT_BGR15:case PIXFMT_RGB15PC:case PIXFMT_BGR15PC:
            case PIXFMT_RGB16:case PIXFMT_BGR16:case PIXFMT_RGB16PC:case PIXFMT_BGR16PC:
            if(_pRemap->_clut16.size()>0)
            {
                directDrawClut_UWORD_UWORD_UWORD(&p,_pRemap->_clut16.data());
            }
            break;
            case PIXFMT_RGB24:case PIXFMT_BGR24:
            if(_pRemap->_clut32.size()>0)
            {
                directDrawClut_type24_ULONG_UWORD(&p,_pRemap->_clut32.data());
            }
            break;
            case PIXFMT_ARGB32:case PIXFMT_BGRA32:case PIXFMT_RGBA32:
            if(_pRemap->_clut32.size()>0)
            {
                directDrawClut_ULONG_ULONG_UWORD(&p,_pRemap->_clut32.data());
            }
            break;
            case PIXFMT_LUT8:
            {
                _pRemap->directDraw(&p);
            }
            break;
        default:
            // shouldnt happen.
            break;
        }
    }
    //else: Truecolor (todo)

    UnLockBitMap(hdl);
}

void Drawable_CGX::drawCGX_Direct(BitMap *pBitmap,_mame_display *display)
{
    if(!pBitmap) return;
    cgxDirectFrame frame;
    prepareDirectFrame(display,frame);
    drawDirectFrame(pBitmap,frame,0,0,NULL);
}
void Drawable_CGX::initARGB32DrawFunctionFromPixelFormat()
{
    // find function pointer to draw that mode. _PixelFmt
    switch(_PixelFmt) {
        case PIXFMT_RGB15:directDrawARGB32=&directDraw_RGB15_ARGB32; break;
        case PIXFMT_BGR15:directDrawARGB32=&directDraw_BGR15_ARGB32; break;
        case PIXFMT_RGB15PC:directDrawARGB32=&directDraw_RGB15PC_ARGB32; break;
        case PIXFMT_BGR15PC:directDrawARGB32=&directDraw_BGR15PC_ARGB32; break;
        case PIXFMT_RGB16:directDrawARGB32=&directDraw_RGB16_ARGB32; break;
        case PIXFMT_BGR16:directDrawARGB32=&directDraw_BGR16_ARGB32; break;
        case PIXFMT_RGB16PC:directDrawARGB32=&directDraw_RGB16PC_ARGB32; break;
        case PIXFMT_BGR16PC:directDrawARGB32=&directDraw_BGR16PC_ARGB32; break;

        case PIXFMT_RGB24:
            case PIXFMT_BGR24:
        directDrawARGB32=&directDraw_type24_ARGB32;
        break;
        // 11
        case PIXFMT_ARGB32:directDrawARGB32=&directDrawARGB32_ARGB32; break;
        // 12, ok:
        case PIXFMT_BGRA32:directDrawARGB32=&directDrawBGRA32_ARGB32; break;
        case PIXFMT_RGBA32:directDrawARGB32=&directDrawRGBA32_ARGB32; break;
    default:
        directDrawARGB32 = NULL; // case for LUT8
        break;
    }

}


Intuition_Screen_CGX::Intuition_Screen_CGX(const AbstractDisplay::params &params)
    : Intuition_Screen(params) , Drawable_CGX((IntuitionDrawable&)*this)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
    if(!CyberGfxBase) return; // this is all the point and shouldnt happen.
    int width = params._width;
    int height = params._height;
    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);
    if(params._video_attributes & VIDEO_RGB_DIRECT)
    {
        if(params._video_attributes &VIDEO_NEEDS_6BITS_PER_GUN)
            _ScreenDepthAsked= 32;
        else _ScreenDepthAsked= 16;
    }else
        _ScreenDepthAsked = (params._colorsIndexLength<=256)?8:16; // more would be Display_CGX_TrueColor.

    if(/*_ScreenDepthAsked == 32 &&*/ (_flags & DISPFLAG_FORCEDEPTH16)!=0)
        _ScreenDepthAsked = 16;

    int useOwnBestMethod=((_flags &DISPFLAG_USEOWNCGXBESTMODE)!=0);
    if(_ScreenModeId == INVALID_ID)
    {
   // printf("find best mode, _screenDepthAsked:%d...\n",(int)_ScreenDepthAsked);
        char depthsToTest[5]={_ScreenDepthAsked,8,5,4,0};  // OCS has 32b and 16b modes, 0 is termination
        int idepth = 0;
        while(depthsToTest[idepth] != 0 &&
           _ScreenModeId == INVALID_ID)
        {
            if(useOwnBestMethod) {
                _ScreenModeId = OwnCGXBestModeID(width,height,depthsToTest[idepth]);
            } else
            {
                struct TagItem cgxtags[]={
                    CYBRBIDTG_NominalHeight,height,
                    CYBRBIDTG_NominalWidth,width,
                    CYBRBIDTG_Depth,depthsToTest[idepth],
                    TAG_DONE,0 };
                _ScreenModeId = BestCModeIDTagList(cgxtags);
            }

            if(_ScreenModeId != INVALID_ID) _ScreenDepthAsked = (int)depthsToTest[idepth];
            idepth++;
        }
        if(_ScreenModeId == INVALID_ID)
        {
            loginfo(2,"Can't find cyber screen mode for w%d h%d d%d ",width,height,_ScreenDepthAsked);
            return;
        }

    } // end if no mode decided at first

    // inquire mode
    if( IsCyberModeID(_ScreenModeId) )
    {
        _fullscreenWidth = GetCyberIDAttr( CYBRIDATTR_WIDTH, _ScreenModeId );
        _fullscreenHeight = GetCyberIDAttr( CYBRIDATTR_HEIGHT, _ScreenModeId );
        _PixelFmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT, _ScreenModeId );
        _PixelBytes = GetCyberIDAttr( CYBRIDATTR_BPPIX, _ScreenModeId );

        _ScreenDepthAsked = GetCyberIDAttr( CYBRIDATTR_DEPTH, _ScreenModeId );

       // printf("cgx mode w:%d h:%d pixfmt:%d pixbytes:%d final depth:%d MODE:%08x\n",
       //          _fullscreenWidth,_fullscreenHeight,
       //             (int)_PixelFmt, (int)_PixelBytes,(int)_ScreenDepthAsked,
       //             (int)_ScreenModeId);

    } else
    {

    }
    if(_PixelFmt == PIXFMT_LUT8)
    {
        _useIntuitionPalette = true;
        _flags |= DISPFLAG_INTUITIONPALETTE;
    }

    // if source is direct RGB32  find function pointer to draw that mode _PixelFmt
    if(isSourceRGBA32())
    {
        initARGB32DrawFunctionFromPixelFormat();
    }
}
extern "C" {
void ui_popup_time(int seconds, const char *text,...);

}
bool Intuition_Screen_CGX::open()
{
    if(_flags & DISPFLAG_LIGHTGUN)
    {
       loginfo(1,"Lightguns can't be used with RTG screens!\nThey need AGA/OCS screens with Cathode Ray Tube monitors!");
       ui_popup_time(5, "Lightguns can't be used with RTG screens!\nThey need AGA/OCS screens with Cathode Ray Tube monitors!");
    }
    bool ok = Intuition_Screen::open();
    if(!ok) return false;

    // after Screen is open, may create create color remap table for clut.
    initRemapTable();



    return true;
}
void Intuition_Screen_CGX::close()
{
    Intuition_Screen::close();
    Drawable_CGX::close();
}

void Intuition_Screen_CGX::draw(_mame_display *display)
{

    drawCGX_Direct(bitmap(),display);


   if(_pTripleBufferImpl) _pTripleBufferImpl->afterBufferDrawn();
   if(_flags & DISPFLAG_USEHEIGHTBUFFER) {
        if(_pScreen)
        {
            _pScreen->ViewPort.DyOffset = ((_heightBufferSwitch)?_heightBufferSwitchApplied:0);
            ScrollVPort(&(_pScreen->ViewPort));
         }

        _heightBufferSwitch^=1;
   }

}
// - - -- -  - - --
Intuition_Window_CGX::Intuition_Window_CGX(const AbstractDisplay::params &params)
    :Intuition_Window(params), Drawable_CGX((IntuitionDrawable&)*this),_sWbWinSBitmap(NULL)
    ,_screenBitmapDirect(false)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
}
//Intuition_Window_CGX::~Intuition_Window_CGX()
//{}
// BackFillMessage, not in the OS3 includes: what DoHookClipRects() gives.
struct cgxBackFillMsg {
    struct Layer     *bf_Layer;
    struct Rectangle  bf_Bounds;    // screen coordinates, max included
    LONG              bf_OffsetX;
    LONG              bf_OffsetY;
};
struct cgxClipHook {
    struct Hook hook;   // first: the Hook is a0 on entry
    Intuition_Window_CGX *window;
    cgxDirectFrame *frame;
    BitMap *screenBitmap;
};
// called by DoHookClipRects() for each visible rectangle of the window,
// with the layer locked.
static void cgxClipHookFunc(struct Hook *h REGUFT(a0), struct RastPort *rp REGUFT(a2),
                            cgxBackFillMsg *msg REGUFT(a1))
{
    cgxClipHook *ch = (cgxClipHook *)h;
    // simple refresh: only rectangles on screen, never off-screen bitmaps.
    if(rp->BitMap != ch->screenBitmap) return;
    struct Layer *layer = msg->bf_Layer;
    // the window inner area in screen coordinates
    int shiftx = layer ? layer->bounds.MinX : 0;
    int shifty = layer ? layer->bounds.MinY : 0;
    ch->window->drawDirectFrame(rp->BitMap,*ch->frame,shiftx,shifty,&msg->bf_Bounds);
}

void Intuition_Window_CGX::draw(_mame_display *display)
{
     if(!_pWbWindow || !_sWbWinSBitmap) return;

    // will draw to the current size.
    _widthtarget = (int)(_pWbWindow->GZZWidth);
    _heighttarget = (int)(_pWbWindow->GZZHeight);
    _screenshiftx = 0; // window coordinates, the shift is applied per frame.
    _screenshifty = 0;

    cgxDirectFrame frame;
    prepareDirectFrame(display,frame);

    BitMap *pScreenBM = _pWbWindow->WScreen->RastPort.BitMap;
    if(_screenBitmapDirect && _pWbWindow->RPort->Layer)
    {
        // draw directly in the screen bitmap, only in the visible parts of
        // the window: layers.library gives them one by one.
        cgxClipHook ch;
        ch.hook.h_Entry = (ULONG (*)())cgxClipHookFunc;
        ch.hook.h_SubEntry = NULL;
        ch.hook.h_Data = NULL;
        ch.window = this;
        ch.frame = &frame;
        ch.screenBitmap = pScreenBM;
        DoHookClipRects(&ch.hook,_pWbWindow->RPort,NULL);
    } else
    {
        // draw on friend bitmap _sWbWinSBitmap,
        drawDirectFrame(_sWbWinSBitmap,frame,0,0,NULL);

        // then use os copy to window, it manages layers, slow because 2 pass but easy way.
        BltBitMapRastPort( _sWbWinSBitmap,//CONST struct BitMap *srcBitMap,
               0,0, //LONG xSrc, LONG ySrc,
               _pWbWindow->RPort,//struct RastPort *destRP,
               0,0,//LONG xDest, LONG yDest,
               _widthtarget, _heighttarget,
               0x00c0//ULONG minterm  -> copy minterm.
               );
     }
}
bool Intuition_Window_CGX::open()
{
    if(_pWbWindow) return true; // already ok
    bool ok = Intuition_Window::open();
    if(!ok) return false;

    if(!_sWbWinSBitmap && _pWbWindow)
    {
        _sWbWinSBitmap = AllocBitMap(
                        _machineWidth*_maxzoomfactor,_machineHeight*_maxzoomfactor,
                    _pWbWindow->WScreen->RastPort.BitMap->Depth,
                        BMF_CLEAR|BMF_DISPLAYABLE, _pWbWindow->WScreen->RastPort.BitMap);
        if(!_sWbWinSBitmap) {
            close();
            return false;
        }
    }

    if(CyberGfxBase && _sWbWinSBitmap && GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_ISCYBERGFX))
    {   // this should be the case:
        _PixelFmt = GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_PIXFMT);
        _PixelBytes = GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_BPPIX);
    } else
    {
        _PixelFmt = PIXFMT_LUT8;
        _PixelBytes = 1;
    }

    // direct drawing in the screen bitmap needs the same pixel format
    BitMap *pScreenBM = _pWbWindow->WScreen->RastPort.BitMap;
    _screenBitmapDirect = (CyberGfxBase && GetCyberMapAttr(pScreenBM,CYBRMATTR_ISCYBERGFX) &&
        GetCyberMapAttr(pScreenBM,CYBRMATTR_PIXFMT) == _PixelFmt);

    if(isSourceRGBA32())
    {
        initARGB32DrawFunctionFromPixelFormat();
    } else
    {
        // if any clut or RGB15 only:
        initRemapTable();
    }
    return (_sWbWinSBitmap != NULL);
}
void Intuition_Window_CGX::close()
{
    if(_sWbWinSBitmap) FreeBitMap(_sWbWinSBitmap);
    _sWbWinSBitmap = NULL;
    Intuition_Window::close();
    Drawable_CGX::close();

}
BitMap *Intuition_Window_CGX::bitmap()
{
    return _sWbWinSBitmap;
}

/*
 * Pixels rendered at lightning speed on your RTG card! ⚡
 *       ___
 *      /   \___
 *     |  ^  ^  |  <- This falcon sees every pixel perfectly!
 *     |   >    |
 *      \_____/
 *        | |
 *       _| |_
 */
