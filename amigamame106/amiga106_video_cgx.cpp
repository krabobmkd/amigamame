/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga106_video_cgx.h"
#include "amiga_video_tracers_clut16.h"
#include "amiga_video_tracers_argb32.h"
#include "amiga_video_remap.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>

// Amiga
extern "C" {
    #include <cybergraphx/cybergraphics.h>
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <graphics/modeid.h>
    #include <graphics/displayinfo.h>
}

extern "C" {
    // from mame
    #include "mame.h"
    #include "video.h"
    #include "mamecore.h"
    #include "osdepend.h"
    #include "palette.h"
}
#include <stdio.h>
#include <stdlib.h>

extern struct Library *CyberGfxBase;

template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }


bool Intuition_Screen_CGX::useForThisMode(ULONG modeID)
{
    if(!CyberGfxBase) return false;
    if (modeID != INVALID_ID)
    {   // if screen Id explicit, we know if native or cgx.
        return (bool)IsCyberModeID(modeID);
    }
    // case where some p96 or cgx installed but no monitor activated.
    // check a standard resolution
    struct TagItem cgxtags[]={
            CYBRBIDTG_NominalWidth,320,
            CYBRBIDTG_NominalHeight,240,
            CYBRBIDTG_Depth,8,
            TAG_DONE,0 };
    ULONG someModeId = BestCModeIDTagList(cgxtags);

    if( someModeId== INVALID_ID) return false;
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
            if(_colorsIndexLength<=(privScreenNbColors+6))
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

void Drawable_CGX::drawCGX_DirectCPU16(
        RastPort *pRPort,
        BitMap *pBitmap,
        _mame_display *display)
{
//    RastPort *pRPort = _drawable.rastPort();
//    BitMap *pBitmap = _drawable.bitmap();
    if(!pRPort || !pBitmap) return;

    // - - update palette if exist and is needed.
    if(_pRemap && ((display->changed_flags & GAME_PALETTE_CHANGED) !=0 || _pRemap->needRemap()))
    {
        _pRemap->updatePaletteRemap(display);
    }

    // applied width height if using scale or not, and centering.
    int sourcewidth,sourceheight;
    int cenx,ceny,ww,hh;
    _drawable.getGeometry(display,cenx,ceny,ww,hh,sourcewidth,sourceheight);

    mame_bitmap *bitmap = display->game_bitmap;

    directDrawScreen ddscreen;

 //   int depth,pixfmt,pixbytes;
// int dbg=0;
    ULONG bmwidth,bmheight;

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };

    APTR hdl = LockBitMapTags(pBitmap,
                              LBMI_WIDTH,(ULONG)&bmwidth,
                              LBMI_HEIGHT,(ULONG)&bmheight,
                           //   LBMI_DEPTH,(ULONG)&depth,
                           //   LBMI_PIXFMT,(ULONG)&pixfmt,
                           //   LBMI_BYTESPERPIX,(ULONG)&pixbytes,
                              LBMI_BYTESPERROW,(ULONG)&ddscreen._bpr,
                              LBMI_BASEADDRESS,(ULONG)&ddscreen._base,
                              TAG_DONE);
    if(!hdl) return;

    ddscreen._clipX1 = 0;
    ddscreen._clipY1 = 0;

    ddscreen._clipX2 = (WORD)bmwidth;
    ddscreen._clipY2 = (WORD)bmheight;

    if(_pRemap)
    {
        directDrawParams p{&ddscreen,&ddsource,cenx,ceny,ww,hh};
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
            //debugval =2;
            // shouldnt happen.
            break;
        }
    } else
    {
        //debugval=3;
        //Truecolor (todo)
    }

    UnLockBitMap(hdl);
//    printf("case:%d\n", dbg );
//     if(debugval ==2) printf("unknown pixfmt\n");
//    else if(debugval ==3) printf("Truecolor todo\n");
//      else if(debugval !=0) printf("TROUBLE:%d\n",debugval);

}


void Drawable_CGX::drawCGX_DirectCPU32(
     //   RastPort *pRPort,
        BitMap *pBitmap,
    _mame_display *display)
{
    //RastPort *pRPort = _drawable.rastPort();
  //  BitMap *pBitmap = _drawable.bitmap();
    if(/*!pRPort ||*/ !pBitmap) return;

    // applied width height if using scale or not, and centering.
    int sourcewidth,sourceheight;
    int cenx,ceny,ww,hh;
    _drawable.getGeometry(display,cenx,ceny,ww,hh,sourcewidth,sourceheight);

    mame_bitmap *bitmap = display->game_bitmap;

    directDrawScreen ddscreen;

   // int depth,pixfmt,pixbytes;

    ULONG bmwidth,bmheight;

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };

    APTR hdl = LockBitMapTags(pBitmap,
                              LBMI_WIDTH,(ULONG)&bmwidth,
                              LBMI_HEIGHT,(ULONG)&bmheight,
                           //   LBMI_DEPTH,(ULONG)&depth,
                           //   LBMI_PIXFMT,(ULONG)&pixfmt,
                           //   LBMI_BYTESPERPIX,(ULONG)&pixbytes,
                              LBMI_BYTESPERROW,(ULONG)&ddscreen._bpr,
                              LBMI_BASEADDRESS,(ULONG)&ddscreen._base,
                              TAG_DONE);
    if(!hdl) return;

    ddscreen._clipX1 = 0;
    ddscreen._clipY1 = 0;

    ddscreen._clipX2 = (WORD)bmwidth;
    ddscreen._clipY2 = (WORD)bmheight;

    // this function pointer has been choosen for the right _PixelFmt and avoid an ugly switch.
    directDrawParams p{ &ddscreen,&ddsource,cenx,ceny,ww,hh};
    if(directDrawARGB32) directDrawARGB32(&p);
    else if(_pRemap) _pRemap->directDraw(&p); // 8bit cases

    UnLockBitMap(hdl);

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

    if(_ScreenModeId == INVALID_ID)
    {
   // printf("find best mode, _screenDepthAsked:%d...\n",(int)_ScreenDepthAsked);
        char depthsToTest[5]={_ScreenDepthAsked,8,5,4,0};  // OCS has 32b and 16b modes, 0 is termination
        int idepth = 0;
        while(depthsToTest[idepth] != 0 &&
           _ScreenModeId == INVALID_ID)
        {
            struct TagItem cgxtags[]={
                    CYBRBIDTG_NominalWidth,width,
                    CYBRBIDTG_NominalHeight,height,
                    CYBRBIDTG_Depth,depthsToTest[idepth],
                    TAG_DONE,0 };
            _ScreenModeId = BestCModeIDTagList(cgxtags);
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


bool Intuition_Screen_CGX::open()
{
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

    if(isSourceRGBA32() )
    {
        drawCGX_DirectCPU32(bitmap(),display);
    }
    else
    {
        drawCGX_DirectCPU16(rastPort(),bitmap(), display);
    }


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
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
}
//Intuition_Window_CGX::~Intuition_Window_CGX()
//{}
#define USE_DIRECT_WB_RENDERING 1
void Intuition_Window_CGX::draw(_mame_display *display)
{
     if(!_pWbWindow || !_sWbWinSBitmap) return;

    // will draw to the current size.
    _widthtarget = (int)(_pWbWindow->GZZWidth);
    _heighttarget = (int)(_pWbWindow->GZZHeight);

#ifdef USE_DIRECT_WB_RENDERING   
    Layer *lr = _pWbWindow->RPort->Layer;
    if(lr) ObtainSemaphore(&(lr->Lock));
    bool rastPortComplete = isRastPortComplete(_pWbWindow->RPort,(WORD)_widthtarget,(UWORD)_heighttarget) ;

     if(rastPortComplete &&
        _pWbWindow->LeftEdge+_pWbWindow->BorderLeft>0 &&
        _pWbWindow->TopEdge+_pWbWindow->BorderTop > 0 &&
        (_pWbWindow->LeftEdge+_pWbWindow->BorderLeft+_widthtarget)<_widthphys &&
        (_pWbWindow->TopEdge+_pWbWindow->BorderTop+_heighttarget)<_heightphys

        )
     {
         // window is on top, we don't need layer library,
         // and can draw directly to wb rastport.
         // avoid a bitmap copy.

        _screenshiftx = _pWbWindow->LeftEdge+_pWbWindow->BorderLeft; // because rastport is inside window
        _screenshifty = _pWbWindow->TopEdge+_pWbWindow->BorderTop;

        RastPort *pScreenRp = &(_pWbWindow->WScreen->RastPort);
        BitMap *pScreenBM = _pWbWindow->WScreen->RastPort.BitMap;

            // use cpu direct copy
            if(isSourceRGBA32())
                drawCGX_DirectCPU32(pScreenBM,display);
            else
                drawCGX_DirectCPU16(pScreenRp,pScreenBM,display);


        if(lr) ReleaseSemaphore(&(lr->Lock));
     } else
#endif
     {
        if(lr) ReleaseSemaphore(&(lr->Lock));
        _screenshiftx = 0; // because rastport is inside window
        _screenshifty = 0;
        // will draw on friend bitmap _sWbWinSBitmap.
        // use cpu direct copy
        if(isSourceRGBA32())
            drawCGX_DirectCPU32(bitmap(),display);
        else
            drawCGX_DirectCPU16(rastPort(),bitmap(),display);

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
