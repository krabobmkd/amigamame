#include "amiga106_video_os3.h"
#include "amiga_video_tracers_clut16.h"
#include "amiga_video_remap.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

extern "C" {
    // from mame
    #include "mame.h"
    #include "video.h"
    #include "mamecore.h"
    #include "osdepend.h"
    #include "palette.h"
}
//#include <stdio.h>
//#include <stdlib.h>

//void waitsecs(int s)
//{
//    for(int i=0;i<50*s;i++)
//    {
//        WaitTOF();
//    }
//}

template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }



Drawable_OS3::Drawable_OS3(IntuitionDrawable &drawable)
 : _drawable(drawable),_pRemap(NULL),_useIntuitionPalette(false)
, _colorsIndexLength(0),_video_attributes(0)
{
   // wpa8temprastport _trp;
    memset(&_trp,0,sizeof(_trp));
}
Drawable_OS3::~Drawable_OS3()
{

}
void Drawable_OS3::draw_WPA8(_mame_display *display)
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

    // align on 16
    int wwal = (ww+15)&0xfffffff0;
    const int bmsize = wwal*(hh+2); // +2 because this looks very messy
    if(bmsize != _wpatempbm.size())
    {
        _wpatempbm.resize(bmsize);
    }
    checkWpa8TmpRp(pRPort,wwal);
    directDrawScreen ddscreen={
        _wpatempbm.data(),
        wwal, // bpr
        0,0,ww,hh // clip rect
    };

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };
    //     if(_PixelFmt == PIXFMT_LUT8) _flags|= DISPFLAG_INTUITIONPALETTE;

    directDrawParams p{&ddscreen,&ddsource,0,0,ww,hh};
    if(_pRemap) _pRemap->directDraw(&p);

    //WritePixelArray8(rp,xstart,ystart,xstop,ystop,array,temprp)

  if(_trp._rp.BitMap)
  {
        WritePixelArray8(pRPort,cenx,ceny,cenx+ww-1,ceny+hh-1,
        _wpatempbm.data(), &_trp._rp
        );
  }

}
void Drawable_OS3::checkWpa8TmpRp(RastPort *rp,int linewidth)
{
    // sizex = the width (in pixels) desired for the bitmap data.
    if(linewidth==0) return;
    int pixelwidth = ((linewidth+15)&0xfffffff0); // align to 16
    if(pixelwidth ==  _trp._checkw && _trp._rp.BitMap != NULL) return;
    if(_trp._rp.BitMap) FreeBitMap(_trp._rp.BitMap);

    memcpy(&_trp,rp,sizeof(RastPort) );
    _trp._rp.Layer = NULL;
    _trp._rp.BitMap =AllocBitMap(
                pixelwidth,2, // it's one but...
            /*rp->BitMap->Depth*/8,BMF_CLEAR,/*rp->BitMap*/ NULL);
    if(!_trp._rp.BitMap) return;
    _trp._checkw=pixelwidth;

}
void Drawable_OS3::draw_WriteChunkyPixels(_mame_display *display)
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

    // - - get a 8bit bitmap from the 16b one, we know there's only 256 color max. - -
    const int bmsize = ww*(hh);
    if(bmsize != _wpatempbm.size()) _wpatempbm.resize(bmsize);

    directDrawScreen ddscreen={
        _wpatempbm.data(),
        ww, // bpr
        0,0,ww,hh // clip rect
    };

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };

    directDrawParams p{&ddscreen,&ddsource,0,0,ww,hh};
    if(_pRemap) _pRemap->directDraw(&p);

//	WriteChunkyPixels(rp,xstart,ystart,xstop,ystop,array,bytesperrow)
//	                  A0 D0     D1     D2    D3    A2     D4
    WriteChunkyPixels(pRPort,cenx,ceny,cenx+ww-1,ceny+hh-1,_wpatempbm.data(),ww );

}


void Drawable_OS3::initRemapTable()
{
    if(_useIntuitionPalette) // cases where we set the screen's palette with LOADRGB32.
    {
        // 8bits screen colors will be managed with LoadRGB32 and direct pixel copy (no clut).
        // if(_colorsIndexLength<=258)
        //     _pRemap = new Paletted_Screen8(_drawable.screen());
        // 8Bits screens will have a fixed 256c palette and  16b index color remap to this.        
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
            if(_colorsIndexLength<=258)
                     _pRemap = new Paletted_Screen8(_drawable.screen());
            // force fixed palette and manage large index to this index at palette change.
            else _pRemap = new Paletted_Screen8ForcePalette(_drawable.screen());

        }
    } else // case where we reuse an existing intuition screen indexed palette. (like on a 8Bit WB)
    {
        // windows on Workbench 8Bit will remap 8&16bits to the palette given by workbench.
        if(_video_attributes & VIDEO_RGB_DIRECT)
        {
            if(_video_attributes & VIDEO_NEEDS_6BITS_PER_GUN)
            {
                _pRemap = new Paletted_Pens8_src15b(_drawable.screen());
            } else
            {
                _pRemap = new Paletted_Pens8_src32b(_drawable.screen());
            }
        } else
        if(_colorsIndexLength>0 && _colorsIndexLength<32768)
        {
            _pRemap = new Paletted_Pens8(_drawable.screen()); // same case for <258 colors or more.
        }
    }
}
void Drawable_OS3::close()
{
    if(_pRemap) delete _pRemap;
    _pRemap = NULL;
    _wpatempbm.clear();
     if(_trp._rp.BitMap) FreeBitMap(_trp._rp.BitMap);
     _trp._rp.BitMap=NULL;
     _trp._checkw=0;
}

Intuition_Screen_OS3::Intuition_Screen_OS3(const AbstractDisplay::params &params)
    : Intuition_Screen(params), Drawable_OS3((IntuitionDrawable&)*this)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
    int width = params._width;
    int height = params._height;
    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);
    _screenDepthAsked = 8; // used by OpenSCreen(), AGA max.

    _useIntuitionPalette = true;

    if(_ScreenModeId == INVALID_ID)
    {
        _ScreenModeId = BestModeID(
                BIDTAG_Depth,8,
                BIDTAG_NominalWidth,width,
                BIDTAG_NominalHeight,height,
                TAG_DONE );
        if(_ScreenModeId == INVALID_ID)
        {
            loginfo(2," **** Can't find screen mode for w%d h%d",width,height);
            return;
        }
    } // end if no mode decided at first

    // inquire mode metrics
    {
        LONG v;
        struct DimensionInfo dims;
        v = GetDisplayInfoData(NULL, (UBYTE *) &dims, sizeof(struct DimensionInfo),
                     DTAG_DIMS, _ScreenModeId);
        if(v>0)
        {
            _fullscreenWidth = (int)(dims.Nominal.MaxX - dims.Nominal.MinX)+1;
            _fullscreenHeight = (int)(dims.Nominal.MaxY - dims.Nominal.MinY)+1;
            // if game screen big, try some oversan conf.
            if(_fullscreenWidth< width )
            {
                _fullscreenWidth = (int)(dims.MaxOScan.MaxX - dims.MaxOScan.MinX)+1;
            }
            if(_fullscreenHeight< height )
            {
                _fullscreenHeight = (int)(dims.MaxOScan.MaxY - dims.MaxOScan.MinY)+1;
            }

//        printf("aga mode $%08x w:%d h:%d\n",
//              (int)_ScreenModeId, _fullscreenWidth,_fullscreenHeight);

        } else
        {   // shouldnt happen, fallback
            _fullscreenWidth = width;
            _fullscreenHeight = height;
        }
    }
}
Intuition_Screen_OS3::~Intuition_Screen_OS3()
{

}

bool Intuition_Screen_OS3::open()
{
    //printf("Intuition_Screen_OS3::open\n");
    bool ok = Intuition_Screen::open();
    if(!ok) return false;
    // after Screen is open, may create create color remap table for clut.
    initRemapTable();
    return ok;
}
void Intuition_Screen_OS3::close()
{
    Intuition_Screen::close();
    Drawable_OS3::close();
}
void Intuition_Screen_OS3::draw(_mame_display *display)
{
    // WritePixelArrays is OS3.0, We could use WriteChunkyPixels which is OS3.1.
    if(GfxBase->LibNode.lib_Version>=40)
    {
        Drawable_OS3::draw_WriteChunkyPixels(display);
    } else
    if(GfxBase->LibNode.lib_Version>=36)
    {
        Drawable_OS3::draw_WPA8(display);
    } else {
        return;
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

// - - -- - - -- - - -
Intuition_Window_OS3::Intuition_Window_OS3(const AbstractDisplay::params &params)
    : Intuition_Window(params), Drawable_OS3((IntuitionDrawable&)*this)
{
    printf("Intuition_Window_OS3\n");
}
Intuition_Window_OS3::~Intuition_Window_OS3()
{

}
// open() is  Intuition_Window::open()
bool Intuition_Window_OS3::open()
{
    if(_pWbWindow) return true; // already ok
    bool ok = Intuition_Window::open();
    if(!ok) return false;

    // if any clut or RGB15 only:
    initRemapTable();

    return true;
}

void Intuition_Window_OS3::close()
{
    Intuition_Window::close();
    Drawable_OS3::close();
}
void Intuition_Window_OS3::draw(_mame_display *display)
{
   // printf("Intuition_Window_OS3::draw:\n");
     if(!_pWbWindow) return;
    // would draw a OS3 window on workbench possibly AGA or CGX 8Bit.
    // will draw to the current size.
    _widthtarget = (int)(_pWbWindow->GZZWidth);
    _heighttarget = (int)(_pWbWindow->GZZHeight);

    _screenshiftx = 0; // because rastport is inside window
    _screenshifty = 0;
    // will draw on friend bitmap _sWbWinSBitmap.
    //printf("GfxBase->LibNode.lib_Version %d\n",GfxBase->LibNode.lib_Version);
    if(GfxBase->LibNode.lib_Version>=40)
    {
        Drawable_OS3::draw_WriteChunkyPixels(display);
    } else
    if(GfxBase->LibNode.lib_Version>=36)
    {
        Drawable_OS3::draw_WPA8(display);
    } else {
        return;
    }

}
