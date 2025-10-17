#include "amiga_video_os3.h"
#include "amiga_video_tracers_clut16.h"
#include "amiga_video_remap.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "amiga_inputs_lightgun.h"
extern "C" {
    // from mame
    #include "mame.h"
    #include "video.h"
    #include "mamecore.h"
    #include "osdepend.h"
    #include "palette.h"
}
extern "C" {
    // from amigaos but not proto
    #include "hardware/custom.h"
    #include "graphics/gfxbase.h"
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
    close();
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
    const int bmsize = wwal*(hh+4); // +4 because zoom trick in draswcale.
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
    const int bmsize = ww*(hh+4); // +2 becaus of zoom trick but well.
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
extern "C" {
#ifdef __GNUC__
#define REG(r) __asm(#r)
#else
#define REG(r)
#endif

    void c2p(UBYTE *chunkyscreen REG(a0),
             struct BitMap *bm REG(a1),
            WORD chunkyx REG(d0),
            WORD chunkyy REG(d1),
            WORD offsx REG(d2),
            WORD offsy REG(d3),
            LONG chunkybpr REG(d4)
             );
}
#ifdef ACTIVATE_OWN_C2P
void Drawable_OS3::draw_c2p(_mame_display *display)
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

    // when screen
    int c2pww = ww;
    if(c2pww>_drawable.widthPhys()) c2pww = _drawable.widthPhys();
    // c2p needs 32pixels aligned width
    c2pww =(c2pww+31)& 0xffffffe0 ;

    int c2phh = hh;
     if(c2phh>_drawable.heightPhys()) c2phh = _drawable.heightPhys();
    // - - get a 8bit bitmap for pixel conversion and then c2p - -
    const int bmsize = ww*(c2phh+4); // +2 becaus of zoom trick but well.
    if(bmsize != _wpatempbm.size()) _wpatempbm.resize(bmsize);

    directDrawScreen ddscreen={
        _wpatempbm.data(),
        ww, // bpr
        0,0,ww,c2phh // clip rect
    };

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _drawable.flags()
    };

    directDrawParams p{&ddscreen,&ddsource,0,0,ww,hh};

    if(_pRemap) _pRemap->directDraw(&p);

    c2p(  _wpatempbm.data(), // chunky
            pRPort->BitMap,
           c2pww,// WORD chunkyx REG(d0),
           c2phh,// WORD chunkyy REG(d1),
          (((WORD)cenx)+15) & 0xfff0, //it's in pixels, round it 16 pixels, (should be 32 for AGA).
          (WORD)ceny, // WORD offsy REG(d3)
          ww
             );
}
#endif

void Drawable_OS3::initRemapTable()
{
    if(_useIntuitionPalette) // cases where we set a private screen palette with LOADRGB32.
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
            Screen *pScreen = _drawable.screen();
            int privScreenNbColors = 1<<(pScreen->RastPort.BitMap->Depth);
            if((_colorsIndexLength+10)<=(privScreenNbColors))
            {
                     _pRemap = new Paletted_Screen8(pScreen);
            }
            // force fixed palette and manage large index to this index at palette change.
            else
            {
             _pRemap = new Paletted_Screen8ForcePalette(pScreen);
            }


        }
    } else // case where we re-use an existing intuition screen indexed palette. (like on a 8Bit Workbench)
    {
        //printf("_colorsIndexLength:%d\n",_colorsIndexLength);
        // windows on Workbench 8Bit will remap 8&16bits to the palette given by workbench.
        if(_video_attributes & VIDEO_RGB_DIRECT)
        {            
            if(_video_attributes & VIDEO_NEEDS_6BITS_PER_GUN)
            {
                _pRemap = new Paletted_Pens8_src32b(_drawable.screen());
            } else
            {
                _pRemap = new Paletted_Pens8_src15b(_drawable.screen());
            }
        } else // most games not RGB
        {
            if(_colorsIndexLength>0)
            {
                _pRemap = new Paletted_Pens8(_drawable.screen()); // same case for <258 colors or more.
            }
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
    , _lightpen_inited(0)
{
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
    int width = params._width;
    int height = params._height;
    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);

    _useIntuitionPalette = true;
    _ScreenDepthAsked = params._forcedDepth; // used by OpenSCreen(), AGA max, default.
    if(_ScreenModeId == INVALID_ID)
    {
        // 1.3: test 16b/24b depth then if fail 8b,5b,4b , for native modes AGA/OCS.
        static const char depthsToTest[4]={8,5,4,0}; // OCS has 32b and 16b modes, 0 is termination.
        int idepth = 0;
        while(depthsToTest[idepth] != 0 &&
           _ScreenModeId == INVALID_ID)
        {
            _ScreenModeId = BestModeID(
                    BIDTAG_Depth,(int)depthsToTest[idepth],
                    BIDTAG_NominalWidth,width,
                    BIDTAG_NominalHeight,height,
                    TAG_DONE );
            if(_ScreenModeId != INVALID_ID) _ScreenDepthAsked = (int)depthsToTest[idepth];
            idepth++;
        }
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
            //_screenDepthAsked = (int)dims.MaxDepth; // if mode was
            _fullscreenWidth = (int)(dims.Nominal.MaxX - dims.Nominal.MinX)+1;
            _fullscreenHeight = (int)(dims.Nominal.MaxY - dims.Nominal.MinY)+1;
            // if game screen big, try some oversan conf.
            // if(_fullscreenWidth< width )
            // {
            //     _fullscreenWidth = (int)(dims.MaxOScan.MaxX - dims.MaxOScan.MinX)+1;
            // }
            if(_fullscreenHeight< height )
            {
                _fullscreenHeight = (int)(dims.MaxOScan.MaxY - dims.MaxOScan.MinY)+1;
            }

        // printf("aga mode $%08x w:%d h:%d\n",
        //       (int)_ScreenModeId, _fullscreenWidth,_fullscreenHeight);

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
    _fullscreenWidth = (_fullscreenWidth+31) & 0xffffffe0; // 32pixel align for c2p
    bool ok = Intuition_Screen::open();
    if(!ok) return false;
    // after Screen is open, may create create color remap table for clut.
    initRemapTable();

    if(_flags & DISPFLAG_LIGHTGUN)
    {
    	GfxBase->system_bplcon0 |= LP_ENABLE;
    	RemakeDisplay();
    	_lightpen_inited = 1;
    }

    return ok;
}
void Intuition_Screen_OS3::close()
{
    if(_lightpen_inited)
    {
    	GfxBase->system_bplcon0 &= ~LP_ENABLE;
    	RemakeDisplay();
    	_lightpen_inited = 0;
    }
    Intuition_Screen::close();
    Drawable_OS3::close();
}

void Intuition_Screen_OS3::draw(_mame_display *display)
{
    // WritePixelArrays is OS3.0, We could use WriteChunkyPixels which is OS3.1.
    // note: all (even OS3.2.x) AGA/ECS versions are damn slow, must use patch blazewcp (not new WPA)


#ifdef ACTIVATE_OWN_C2P
    ULONG destflags = GetBitMapAttr(_pScreen->RastPort.BitMap,BMA_FLAGS);
    if( (destflags & BMF_STANDARD) != 0 )
    {   // would mean it's OCS/AGA
        Drawable_OS3::draw_c2p(display);
    } else
#endif
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

   // double buffer that use scroll is patched here:
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
    _colorsIndexLength = params._colorsIndexLength;
    _video_attributes = params._video_attributes;
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
