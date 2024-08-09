/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga106_video_cgx.h"
#include "amiga_video_tracers_clut16.h"
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
    return ((modeID != INVALID_ID && IsCyberModeID(modeID))
           ||(modeID == INVALID_ID ));
}
bool Intuition_Window_CGX::useForWbWindow()
{
    if(!CyberGfxBase) return false;
    Screen *pWbScreen=NULL;
    if ((pWbScreen = LockPubScreen(NULL)))
    {
        ULONG wbIsCgxManageable = GetCyberMapAttr(pWbScreen->RastPort.BitMap,CYBRMATTR_ISCYBERGFX);
        UnlockPubScreen(NULL,pWbScreen);
        return (wbIsCgxManageable !=0);
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
    if(_useIntuitionPalette)
    {
        // 8bits screen colors will be managed with LoadRGB32 and direct pixel copy (no clut).
        _pRemap = new Paletted_Screen8(_drawable.screen());
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

void Drawable_CGX::drawCGX_DirectCPU16(_mame_display *display)
{
    RastPort *pRPort = _drawable.rastPort();
    BitMap *pBitmap = _drawable.bitmap();
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
        switch(_PixelFmt) {
            case PIXFMT_RGB15:case PIXFMT_BGR15:case PIXFMT_RGB15PC:case PIXFMT_BGR15PC:
            case PIXFMT_RGB16:case PIXFMT_BGR16:case PIXFMT_RGB16PC:case PIXFMT_BGR16PC:
            if(_pRemap->_clut16.size()>0)
            {
                directDrawClutT_UWORD_UWORD(&ddscreen,&ddsource,cenx,ceny,ww,hh,_pRemap->_clut16.data());
            }
            break;
            case PIXFMT_RGB24:case PIXFMT_BGR24:
            if(_pRemap->_clut32.size()>0)
            {
                directDrawClutT_type24_ULONG(&ddscreen,&ddsource,cenx,ceny,ww,hh,_pRemap->_clut32.data());
            }
            break;
            case PIXFMT_ARGB32:case PIXFMT_BGRA32:case PIXFMT_RGBA32:
            if(_pRemap->_clut32.size()>0)
            {
                directDrawClutT_ULONG_ULONG(&ddscreen,&ddsource,cenx,ceny,ww,hh,_pRemap->_clut32.data());
            }
            break;
            case PIXFMT_LUT8:
            if(_drawable.flags() & DISPFLAG_INTUITIONPALETTE)
            {
                //8Bit using fullscreen with dynamic palette change, should just copy pixels.
                directDraw_UBYTE_UBYTE(&ddscreen,&ddsource,cenx,ceny,ww,hh);
            } else {
                if(_pRemap->_clut8.size()>0)
                {   // 8bit using remap and static palette (like on workbench 8bit)
                    directDrawClutT_UBYTE_UBYTE(&ddscreen,&ddsource,cenx,ceny,ww,hh,_pRemap->_clut8.data());
                }
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
//     if(debugval ==2) printf("unknown pixfmt\n");
//    else if(debugval ==3) printf("Truecolor todo\n");
//      else if(debugval !=0) printf("TROUBLE:%d\n",debugval);

}


void Drawable_CGX::drawCGX_DirectCPU32(_mame_display *display)
{
    RastPort *pRPort = _drawable.rastPort();
    BitMap *pBitmap = _drawable.bitmap();
    if(!pRPort || !pBitmap || !directDrawARGB32) return;

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
    directDrawARGB32(&ddscreen,&ddsource,cenx,ceny,ww,hh);

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
        case PIXFMT_ARGB32:directDrawARGB32=&directDrawBGRA32_ARGB32; break;
        case PIXFMT_BGRA32:directDrawARGB32=&directDrawBGRA32_ARGB32; break;
        case PIXFMT_RGBA32:directDrawARGB32=&directDrawRGBA32_ARGB32; break;
    default:
        directDrawARGB32 = NULL;
        break;
    }

}

// =========================== new impl


Intuition_Screen_CGX::Intuition_Screen_CGX(const AbstractDisplay::params &params)
    : Intuition_Screen(params) , Drawable_CGX((IntuitionDrawable&)*this)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
    if(!CyberGfxBase) return; // this is all the point and shouldnt happen.
    int width = params._width;
    int height = params._height;
    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);
    if(params._video_attributes & VIDEO_RGB_DIRECT)_screenDepthAsked= 16;
    else
        _screenDepthAsked = (params._colorsIndexLength<=256)?8:16; // more would be Display_CGX_TrueColor.

    if(_ScreenModeId == INVALID_ID)
    {
        struct TagItem cgxtags[]={
                CYBRBIDTG_NominalWidth,width,
                CYBRBIDTG_NominalHeight,height,
                CYBRBIDTG_Depth,_screenDepthAsked,
                TAG_DONE,0 };
        _ScreenModeId = BestCModeIDTagList(cgxtags);
        if(_ScreenModeId == INVALID_ID)
        {
            logerror("Can't find cyber screen mode for w%d h%d d%d ",width,height,_screenDepthAsked);
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
    }
    if(_PixelFmt == PIXFMT_LUT8)
    {
        _useIntuitionPalette = true;
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
        drawCGX_DirectCPU32(display);
    }
    else
    {
        drawCGX_DirectCPU16(display);
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
void Intuition_Window_CGX::draw(_mame_display *display)
{
     if(!_pWbWindow || !_sWbWinSBitmap) return;

    // will draw on friend bitmap _sWbWinSBitmap to the current size.
    _width = (int)(_pWbWindow->GZZWidth);
    _height = (int)(_pWbWindow->GZZHeight);
    // use cpu direct copy
    if(isSourceRGBA32())
        drawCGX_DirectCPU32(display);
    else
        drawCGX_DirectCPU16(display);

    // then use os copy to window, it manages layers, slow because 2 pass but easy way.
    BltBitMapRastPort( _sWbWinSBitmap,//CONST struct BitMap *srcBitMap,
           0,0, //LONG xSrc, LONG ySrc,
           _pWbWindow->RPort,//struct RastPort *destRP,
           0,0,//LONG xDest, LONG yDest,
           _width, _height,
           0x00c0//ULONG minterm  -> copy minterm.
           );

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
         printf("");
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
    Intuition_Window::close();
    Drawable_CGX::close();
    if(_sWbWinSBitmap) FreeBitMap(_sWbWinSBitmap);
    _sWbWinSBitmap = NULL;
}
BitMap *Intuition_Window_CGX::bitmap()
{
    return _sWbWinSBitmap;
}
// =================================

Drawable_CGXScalePixelArray::Drawable_CGXScalePixelArray(IntuitionDrawable &drawable)
 : _drawable(drawable),_pRemap(NULL),_useIntuitionPalette(false)
 , _colorsIndexLength(0)
 , _video_attributes(0)
{

}
Drawable_CGXScalePixelArray::~Drawable_CGXScalePixelArray()
{
    if(_pRemap) delete _pRemap;
}

void Drawable_CGXScalePixelArray::drawCGX_scale(_mame_display *display)
{
    RastPort *pRPort = _drawable.rastPort();
    if(!pRPort) return;
    mame_bitmap *bitmap = display->game_bitmap;
    // - - update palette if exist and is needed.
    if(_pRemap && ((display->changed_flags & GAME_PALETTE_CHANGED) !=0 || _pRemap->needRemap()))
    {
        _pRemap->updatePaletteRemap(display);
    }
    int sourcewidth,sourceheight;
    int cenx,ceny,ww,hh;
    _drawable.getGeometry(display,cenx,ceny,ww,hh,sourcewidth,sourceheight);

    // whatever happens we render the source size.
    ULONG bmsize = sourcewidth*sourceheight;
    if(bmsize != _bm.size())
    {
        _bm.reserve(bmsize); // I live in the belief that is forces a "exact" alloc in all cases.
        _bm.resize(bmsize);
    }

    directDrawScreen ddscreen={
        _bm.data(),
        sourcewidth*sizeof(ULONG), // bpr
        0,0,sourcewidth,sourceheight // clip rect
    };

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };
    if(isSourceRGBA32())
    {
        // it could be a copy in some case, but we still have to manage rotations
        directDrawARGB32_ARGB32(&ddscreen,&ddsource,0,0,sourcewidth,sourceheight);

    } else
    {   // clut mode, or RGB15 , to RGBA

        // remap in unscaled true color buffer.
        // note should work with 24b, test that.
        // cgx XXXPixelArray only support 24 or 32b format RECTFMT_ARGB
        directDrawClutT_ULONG_ULONG(&ddscreen,&ddsource,0,0,sourcewidth,sourceheight,_pRemap->_clut32.data());
    }

    // let Gfx acceleration do its job (hopefully ;)
    ScalePixelArray(_bm.data(),sourcewidth,sourceheight,ddscreen._bpr,pRPort,
    cenx,ceny,ww,hh,RECTFMT_ARGB ); // also RECTFMT_RGB  RECTFMT_RGBA and RECTFMT_LUT8
}

void Drawable_CGXScalePixelArray::initRemapTable()
{
    if((_video_attributes & VIDEO_RGB_DIRECT)==0 &&
        _colorsIndexLength>0)
    {
        _pRemap = new Paletted_CGX(_colorsIndexLength,PIXFMT_ARGB32,4);
    } else
    if((_video_attributes & VIDEO_RGB_DIRECT)!=0 /*&& params._driverDepth == 15*/)
    {
        Paletted_CGX *p =  new Paletted_CGX(_colorsIndexLength,PIXFMT_ARGB32,4);
        _pRemap = p;
        p->updatePaletteRemap15b(); // once for all.
    }
}
void Drawable_CGXScalePixelArray::close()
{
    if(_pRemap) delete _pRemap;
    _pRemap = NULL;
}
// - - - -

Intuition_Screen_CGXScale::Intuition_Screen_CGXScale(const AbstractDisplay::params &params)
    : Intuition_Screen(params) , Drawable_CGXScalePixelArray((IntuitionDrawable&)*this)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
    if(!CyberGfxBase) return; // this is all the point and shouldnt happen.
    int width = params._width;
    int height = params._height;
    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);
    _screenDepthAsked = 16;  // or 16? -> problems with 32 ??? why ?

//printf("Intuition_Screen_CGXScale %08x\n",_ScreenModeId);

    if(_ScreenModeId == INVALID_ID)
    {
        struct TagItem cgxtags[]={
                CYBRBIDTG_NominalWidth,width,
                CYBRBIDTG_NominalHeight,height,
                CYBRBIDTG_Depth,_screenDepthAsked,
                TAG_DONE,0 };
        _ScreenModeId = BestCModeIDTagList(cgxtags);
        if(_ScreenModeId == INVALID_ID)
        {
            printf("Can't find cyber screen mode for w%d h%d d%d\n",width,height,_screenDepthAsked);
            //logerror("Can't find cyber screen mode for w%d h%d d%d ",width,height,_screenDepthAsked);
            return;
        }

    } // end if no mode decided at first

    // inquire mode
    if( IsCyberModeID(_ScreenModeId) )
    {
        _fullscreenWidth = GetCyberIDAttr( CYBRIDATTR_WIDTH, _ScreenModeId );
        _fullscreenHeight = GetCyberIDAttr( CYBRIDATTR_HEIGHT, _ScreenModeId );
    }
//    printf("dim found: %d %d\n",_fullscreenWidth,_fullscreenHeight);

}

bool Intuition_Screen_CGXScale::open()
{
//    printf("Intuition_Screen_CGXScale::open()\n");
    bool ok = Intuition_Screen::open();
//    printf("Intuition_Screen_CGXScale::open() %d\n",(int)ok);
    if(!ok) return false;

    // after Screen is open, may create create color remap table for clut.
    if(isSourceRGBA32())
    {
        // humm nothing.
    } else {
        // will use clut
        initRemapTable();
    }


    return true;
}
void Intuition_Screen_CGXScale::close()
{
    Intuition_Screen::close();
    Drawable_CGXScalePixelArray::close();
}

void Intuition_Screen_CGXScale::draw(_mame_display *display)
{
    drawCGX_scale(display);
}

// - - -- -  - - --
Intuition_Window_CGXScale::Intuition_Window_CGXScale(const AbstractDisplay::params &params)
 :Intuition_Window(params), Drawable_CGXScalePixelArray((IntuitionDrawable&)*this)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
}

void Intuition_Window_CGXScale::draw(_mame_display *display)
{
     if(!_pWbWindow) return;

    // will draw on friend bitmap _sWbWinSBitmap to the current size.
    _width = (int)(_pWbWindow->GZZWidth);
    _height = (int)(_pWbWindow->GZZHeight);

     drawCGX_scale(display);
}
bool Intuition_Window_CGXScale::open()
{
    if(_pWbWindow) return true; // already ok
    bool ok = Intuition_Window::open();
    if(!ok) return false;

    initRemapTable();

    return true;
}
void Intuition_Window_CGXScale::close()
{
    Intuition_Window::close();
    Drawable_CGXScalePixelArray::close();
}
