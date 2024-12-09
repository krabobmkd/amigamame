/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga106_video_cgxscalepixelarray.h"
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

    int sourcewidth,sourceheight;
    int cenx,ceny,ww,hh;
    _drawable.getGeometry(display,cenx,ceny,ww,hh,sourcewidth,sourceheight);

    if(isSourceARGB32() && (_drawable.flags() & ORIENTATION_MASK)==0)
    {
        // case where wen can use direct mame buffer without temp bitmap
        // let Gfx acceleration do its job (hopefully ;)
        ULONG *pbase = (ULONG *)bitmap->base;// ,bitmap->rowbytes,
        pbase += display->game_visible_area.min_x + ((bitmap->rowbytes>>2) * display->game_visible_area.min_y);
        ScalePixelArray(pbase,sourcewidth,sourceheight,bitmap->rowbytes,pRPort,
            cenx,ceny,ww,hh,RECTFMT_ARGB ); // also RECTFMT_RGB  RECTFMT_RGBA and RECTFMT_LUT8
        return;
    }


    // - - update palette if exist and is needed.
    if(_pRemap && ((display->changed_flags & GAME_PALETTE_CHANGED) !=0 || _pRemap->needRemap()))
    {
        _pRemap->updatePaletteRemap(display);
    }

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
    directDrawParams p{&ddscreen,&ddsource,0,0,sourcewidth,sourceheight};
    if(isSourceARGB32())
    {
        // it could be a copy in some case, but we still have to manage rotations
        // -> now it's done up there
        directDrawARGB32_ARGB32(&p);

    } else
    {   // clut mode, or RGB15 , to RGBA

        // remap in unscaled true color buffer.
        // note should work with 24b, test that.
        // cgx XXXPixelArray only support 24 or 32b format RECTFMT_ARGB
        directDrawClut_ULONG_ULONG_UWORD(&p,_pRemap->_clut32.data());
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

//    if(isSourceRGBA32())
//    {
//            _screenDepthAsked= 32;
//        else _screenDepthAsked= 16;
//    }else

    _screenDepthAsked = 32;  // or 16? -> problems with 32 ??? why ?

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
            //loginfo(2,"Can't find cyber screen mode for w%d h%d d%d ",width,height,_screenDepthAsked);
            return;
        }

    } // end if no mode decided at first

    // inquire mode
    if( IsCyberModeID(_ScreenModeId) )
    {
        _fullscreenWidth = GetCyberIDAttr( CYBRIDATTR_WIDTH, _ScreenModeId );
        _fullscreenHeight = GetCyberIDAttr( CYBRIDATTR_HEIGHT, _ScreenModeId );
        _screenDepthAsked = GetCyberIDAttr( CYBRIDATTR_DEPTH, _ScreenModeId );
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
    if(isSourceARGB32())
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
//    if(_pTripleBufferImpl && !_pTripleBufferImpl->beforeBufferDrawn()) return;
    drawCGX_scale(display);
    if(_pTripleBufferImpl) _pTripleBufferImpl->afterBufferDrawn();
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
    _widthtarget = (int)(_pWbWindow->GZZWidth);
    _heighttarget = (int)(_pWbWindow->GZZHeight);

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
