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
    if(_useIntuitionPalette)
    {   // no clut table and use intuition LoadRGB32 to change screen colors
        // actually does WORD to BYTE conversion with no lut.
        directDraw_UBYTE_UBYTE(&ddscreen,&ddsource,0,0,ww,hh);
    } else
    {   // remap to shared screen colors, need a clut.
    //TODO
//        if(pRemap && pRemap->_clut8.size()>0)
//        {
//           directDrawClutT_UBYTE_UBYTE(&ddscreen,&ddsource,0,0,ww,hh,pRemap->_clut8.data());
//        }
    }
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


void Drawable_OS3::initRemapTable()
{
    if(_useIntuitionPalette)
    {
        // 8bits screen colors will be managed with LoadRGB32 and direct pixel copy (no clut).
        _pRemap = new Paletted_Screen8(_drawable.screen());
    } else
    {

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
            logerror(" **** Can't find screen mode for w%d h%d",width,height);
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
    Drawable_OS3::draw_WPA8(display);
}

// - - -- - - -- - - -
Intuition_Window_OS3::Intuition_Window_OS3(const AbstractDisplay::params &params)
    : Intuition_Window(params), Drawable_OS3((IntuitionDrawable&)*this)
{

}
Intuition_Window_OS3::~Intuition_Window_OS3()
{

}
// open() is  Intuition_Window::open()

void Intuition_Window_OS3::close()
{
    Intuition_Window::close();
    Drawable_OS3::close();
}
void Intuition_Window_OS3::draw(_mame_display *display)
{
    // would draw a OS3 window on workbench possibly AGA.
    //TODO, with the ObtainPen() ans special remap thing.
    // Drawable_OS3::draw_WPA8(display);
}
