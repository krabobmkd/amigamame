/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga106_video_cgx.h"
#include "amiga_video_tracers_clut16.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
//#include <proto/layers.h>
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

// - - - - from driver.h
/* is the video hardware raser or vector base? */
#define	VIDEO_TYPE_RASTER				0x0000
#define	VIDEO_TYPE_VECTOR				0x0001

/* should VIDEO_UPDATE by called at the start of VBLANK or at the end? */
#define	VIDEO_UPDATE_BEFORE_VBLANK		0x0000
#define	VIDEO_UPDATE_AFTER_VBLANK		0x0002

/* set this to use a direct RGB bitmap rather than a palettized bitmap */
#define VIDEO_RGB_DIRECT	 			0x0004


IntuitionDrawable::IntuitionDrawable(int flags)
: _PixelFmt(0),_PixelBytes(0),_width(0),_height(0),_useScale(0)
, _flags(flags)
{
  _trp._checkw=0;
  _trp._rp.BitMap = NULL;
}
IntuitionDrawable::~IntuitionDrawable()
{
    if(_trp._rp.BitMap) FreeBitMap(_trp._rp.BitMap);
}
void IntuitionDrawable::getGeometry(_mame_display *display,int &cenx,int &ceny,int &ww,int &hh)
{

    // +1 because goes 0,319
    int sourcewidth = (display->game_visible_area.max_x - display->game_visible_area.min_x)+1;
    int sourceheight =( display->game_visible_area.max_y - display->game_visible_area.min_y)+1;
    if(_flags & ORIENTATION_SWAP_XY)
    {
        doSwap(sourcewidth,sourceheight);
    }

    if((_width>sourcewidth || _height>sourceheight) && _useScale)
    {
        ww = _width;
        hh = _height;
        // let's consider scale always take all available screen,
        // which is the case for window mode.
        cenx = 0;
        ceny = 0;
    } else
    {
        ww = sourcewidth;
        hh = sourceheight;

        cenx = _width-sourcewidth;
        ceny = _height-sourceheight;
        if(cenx<0) cenx = 0;
        if(ceny<0) ceny = 0;
        cenx>>=1;
        ceny>>=1;
        if(hh>_height) hh=_height; // fast cheap clipping
        if(ww>_width) ww=_width;
    }

}
void IntuitionDrawable::drawRastPortWPA8(_mame_display *display,Paletted *pRemap)
{
    RastPort *pRPort = rastPort();
    if(!pRPort) return;
   // BitMap *pBitmap = bitmap();
    mame_bitmap *bitmap = display->game_bitmap;

    int cenx,ceny,ww,hh;
    getGeometry(display,cenx,ceny,ww,hh);
    // align on 16
    int wwal = (ww+15)&0xfffffff0;

    const int bmsize = wwal*hh;
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
        _flags
    };
    //     if(_PixelFmt == PIXFMT_LUT8) _flags|= DISPFLAG_INTUITIONPALETTE;
    if(_flags & DISPFLAG_INTUITIONPALETTE)
    {   // no clut table and use intuition LoadRGB32 to change screen colors
        // actually does WORD to BYTE conversion with no lut.
        directDraw_UBYTE_UBYTE(&ddscreen,&ddsource,0,0,ww,hh);
    } else
    {   // remap to shared screen colors, need a clut.
        if(pRemap && pRemap->_clut8.size()>0)
        {
            directDrawClutT_UBYTE_UBYTE(&ddscreen,&ddsource,0,0,ww,hh,pRemap->_clut8.data());
        }
    }
    //WritePixelArray8(rp,xstart,ystart,xstop,ystop,array,temprp)
    WritePixelArray8(pRPort,cenx,ceny,cenx+ww-1,ceny+hh-1,
        _wpatempbm.data(), &_trp._rp
        );
}
void IntuitionDrawable::checkWpa8TmpRp(RastPort *rp,int linewidth)
{
    int bpr = ((linewidth+15)>>4)<<4;
    if(bpr ==  _trp._checkw) return;
    if(_trp._rp.BitMap) FreeBitMap(_trp._rp.BitMap);

    memcpy(&_trp,rp,sizeof(RastPort) );
    _trp._rp.Layer = NULL;
    _trp._rp.BitMap =AllocBitMap(bpr*8,1,
            rp->BitMap->Depth,BMF_CLEAR,NULL);
    if(!_trp._rp.BitMap) return;
    _trp._checkw=bpr;

}
// would draw LUT screens or truecolor, ...
// _width,_height must be set before call.
void IntuitionDrawable::drawRastPort_CGX(_mame_display *display,Paletted *pRemap)
{
    RastPort *pRPort = rastPort();

    BitMap *pBitmap = bitmap();
    if(!pRPort || !pBitmap) return;

    bool useCgxDirect = (CyberGfxBase && GetCyberMapAttr(pBitmap,CYBRMATTR_ISCYBERGFX));
    if(!useCgxDirect)
    {
        if(_PixelFmt == PIXFMT_LUT8) drawRastPortWPA8(display,pRemap);
        return;
    }


    mame_bitmap *bitmap = display->game_bitmap;

    directDrawScreen ddscreen;

    int depth,pixfmt,pixbytes,bpr;

    UWORD *pp = (UWORD *)bitmap->base;
    ULONG bmwidth,bmheight;

int debugval=0;
    APTR hdl = LockBitMapTags(pBitmap,
                              LBMI_WIDTH,(ULONG)&bmwidth,
                              LBMI_HEIGHT,(ULONG)&bmheight,
                              LBMI_DEPTH,(ULONG)&depth,
                              LBMI_PIXFMT,(ULONG)&pixfmt,
                              LBMI_BYTESPERPIX,(ULONG)&pixfmt,
                              LBMI_BYTESPERROW,(ULONG)&ddscreen._bpr,
                              LBMI_BASEADDRESS,(ULONG)&ddscreen._base,
                              TAG_DONE);
    if(!hdl) return;

    ddscreen._clipX1 = 0;//10;
    ddscreen._clipY1 = 0; //10;

    ddscreen._clipX2 = (WORD)bmwidth; //-10;
    ddscreen._clipY2 = (WORD)bmheight; //-10;

    // applied width height if using scale or not, and centering.
    int cenx,ceny,ww,hh;
    getGeometry(display,cenx,ceny,ww,hh);

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1,
        _flags
    };
    if(pRemap)
    {
        switch(_PixelFmt) {
            case PIXFMT_RGB15:case PIXFMT_BGR15:case PIXFMT_RGB15PC:case PIXFMT_BGR15PC:
            case PIXFMT_RGB16:case PIXFMT_BGR16:case PIXFMT_RGB16PC:case PIXFMT_BGR16PC:
            if(pRemap->_clut16.size()>0)
            {
                directDrawClutT_UWORD_UWORD(&ddscreen,&ddsource,cenx,ceny,ww,hh,pRemap->_clut16.data());
            }
            break;
            case PIXFMT_RGB24:case PIXFMT_BGR24:
            if(pRemap->_clut32.size()>0)
            {
                directDrawClutT_type24_ULONG(&ddscreen,&ddsource,cenx,ceny,ww,hh,pRemap->_clut32.data());
            }
            break;
            case PIXFMT_ARGB32:case PIXFMT_BGRA32:case PIXFMT_RGBA32:
            if(pRemap->_clut32.size()>0)
            {
                directDrawClutT_ULONG_ULONG(&ddscreen,&ddsource,cenx,ceny,ww,hh,pRemap->_clut32.data());
            }
            break;
            case PIXFMT_LUT8:
            if(_flags & DISPFLAG_INTUITIONPALETTE)
            {               
                //8Bit using fullscreen with dynamic palette change, should just copy pixels.
                directDraw_UBYTE_UBYTE(&ddscreen,&ddsource,cenx,ceny,ww,hh);
            } else {
                if(pRemap->_clut8.size()>0)
                {   // 8bit using remap and static palette (like on workbench 8bit)
                    directDrawClutT_UBYTE_UBYTE(&ddscreen,&ddsource,cenx,ceny,ww,hh,pRemap->_clut8.data());
                }
            }
            break;
        default:
            debugval =2;
            // shouldnt happen.
            break;
        }
    } else
    {
        debugval=3;
        //Truecolor (todo)
    }

    UnLockBitMap(hdl);
     if(debugval ==2) printf("unknown pixfmt\n");
    else if(debugval ==3) printf("Truecolor todo\n");
//    extern int tracer_debug,tracer_debug2;
//    printf("dbg1:%d dbg2:%d\n",tracer_debug,tracer_debug2);

//    if(pRemap->_clut32.size()>16384)
//    {
//        printf("_clut32 values:%08x %08x %08x\n",pRemap->_clut32[14000],
//                pRemap->_clut32[15000],pRemap->_clut32[12000]);
//    }
//    if(pRemap->_clut16.size()>16384)
//    {
//        printf("_clut16 values:%04x %04x %04x\n",pRemap->_clut16[14000],
//                pRemap->_clut16[15000],pRemap->_clut16[12000]);
//    }
   //         printf("_width:%d sourcewidth:%d \n",_width,sourcewidth);
//    printf("isit?\n");
//      printf("ddsource x1:%d  y1:%d \n",ddsource._x1,ddsource._y1);
//    printf("or?\n");
}
// =========================== new impl


Intuition_Screen::Intuition_Screen(const AmigaDisplay::params &params)
    : IntuitionDrawable((params._flags&15))
    , _pScreen(NULL)
    , _pScreenWindow(NULL)

    , _ScreenModeId(params._forcedModeID)
    , _fullscreenWidth(0)
    , _fullscreenHeight(0)
    , _pMouseRaster(NULL)
{
    int width = params._width;
    int height = params._height;
    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);
    int screenDepth = (params._colorsIndexLength<=256)?8:16; // more would be Display_CGX_TrueColor.

    if(_ScreenModeId == INVALID_ID)
    {
        if(CyberGfxBase)
        {
             struct TagItem cgxtags[]={
                    CYBRBIDTG_NominalWidth,width,
                    CYBRBIDTG_NominalHeight,height,
                    CYBRBIDTG_Depth,screenDepth,
                    TAG_DONE,0 };
            _ScreenModeId = BestCModeIDTagList(cgxtags);
            if(_ScreenModeId == INVALID_ID)
            {
                logerror("Can't find cyber screen mode for w%d h%d d%d ",width,height,screenDepth);
                return;
            }

        } else
        {
            // using OS3 to find mode.
            if(screenDepth>8)
            {
                logerror("Can't find 16b mode without CGX ");
                return;
            }
            _ScreenModeId = BestModeID(
                    BIDTAG_Depth,8,
                    BIDTAG_NominalWidth,width,
                    BIDTAG_NominalHeight,height,
                    TAG_DONE );
            if(_ScreenModeId == INVALID_ID)
            {
                logerror("Can't find screen mode for w%d h%d d%d ",width,height,screenDepth);
                return;
            }
        }
    } // end if no mode decided at first

    // inquire mode
    if(CyberGfxBase && IsCyberModeID(_ScreenModeId) )
    {
        _fullscreenWidth = GetCyberIDAttr( CYBRIDATTR_WIDTH, _ScreenModeId );
        _fullscreenHeight = GetCyberIDAttr( CYBRIDATTR_HEIGHT, _ScreenModeId );
        _PixelFmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT, _ScreenModeId );
        _PixelBytes = GetCyberIDAttr( CYBRIDATTR_BPPIX, _ScreenModeId );
    } else
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
        // TODO for ECS we can't do depth 8 should be checked
        _PixelFmt = PIXFMT_LUT8; // will be treated with WritePixelArray8 anyway.
        _PixelBytes = 1;
    }
    // 8bits screen colors will be managed with LoadRGB32 and direct pixel copy (no clut).
    if(_PixelFmt == PIXFMT_LUT8) _flags|= DISPFLAG_INTUITIONPALETTE;

}
Intuition_Screen::~Intuition_Screen()
{
    close();
}
bool Intuition_Screen::open()
{
    if(_pScreenWindow) return true; // already open.
    if(_ScreenModeId == INVALID_ID)
    {
        logerror("Can't find a screen mode ");
        return false; // set by inherited class.
     }

    int depth;
    if(_PixelFmt == PIXFMT_LUT8) depth=8;
    else depth=16;

    // note: all this is regular OS intuition, no CGX
	struct ColorSpec colspec[2]={0,0,0,0,-1,0,0,0};
 	_pScreen = OpenScreenTags( NULL,
			SA_DisplayID,_ScreenModeId,
                        SA_Width, _fullscreenWidth,
                        SA_Height,_fullscreenHeight,
                        SA_Depth,depth,
//                        SA_Behind,TRUE,    /* Open behind */
                        SA_Quiet,TRUE,     /* quiet */
			SA_Type,CUSTOMSCREEN,
			SA_Colors,(ULONG)&colspec[0],
                        0 );

	if( _pScreen == NULL ) return false;

	// --------- open intuition fullscreen window for this screen:

    _pScreenWindow = OpenWindowTags(/*&screenwin*/NULL,
        WA_CustomScreen,(ULONG)_pScreen,
                    WA_Backdrop,FALSE,
                    WA_Borderless,TRUE,
                    WA_Activate,TRUE,
                    WA_RMBTrap,TRUE,
                    WA_ReportMouse,0,
                    WA_SizeGadget,0,
                    WA_DepthGadget,0,
                    WA_CloseGadget,0,
                    WA_DragBar,0,
          WA_GimmeZeroZero,FALSE, // test
                    WA_IDCMP,IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY ,
                    0 );

	if( _pScreenWindow ==  NULL )
	{
        close();
        return false;
	}
	// ------- set invisible mouse pointer:
	_pMouseRaster =  AllocRaster(8 ,8) ;
	if(_pMouseRaster)
	{
        SetPointer( _pScreenWindow ,(UWORD *) _pMouseRaster, 0,1,0,0);
    }
    _width = _fullscreenWidth;
    _height = _fullscreenHeight;

    return true;
}
void Intuition_Screen::close()
{
    if(_pScreenWindow) CloseWindow(_pScreenWindow);
    if(_pScreen) CloseScreen(_pScreen);
    if(_pMouseRaster) FreeRaster( (PLANEPTR) _pMouseRaster ,8,8);

    _pScreenWindow = NULL;
    _pScreen = NULL;
    _pMouseRaster = NULL;
}
MsgPort *Intuition_Screen::userPort()
{
    if(!_pScreenWindow) return NULL;
    return _pScreenWindow->UserPort;
}
RastPort *Intuition_Screen::rastPort()
{
    if(!_pScreen) return NULL;
    return &_pScreen->RastPort;
}
BitMap *Intuition_Screen::bitmap()
{
    if(!_pScreen) return NULL;
    return _pScreen->RastPort.BitMap;
}

Intuition_Window::Intuition_Window(const AmigaDisplay::params &params) : IntuitionDrawable(params._flags)
    , _pWbWindow(NULL)
    , _sWbWinSBitmap(NULL)
    , _machineWidth(params._width),_machineHeight(params._height)
    , _maxzoomfactor(1)
{
   printf(" ***** ** Intuition_Window CREATE:%d %d %08x\n",params._width,params._height,params._flags);

  if(params._flags & ORIENTATION_SWAP_XY) doSwap(_machineWidth,_machineHeight);
}
Intuition_Window::~Intuition_Window()
{
    close();
}
bool Intuition_Window::open()
{
    if(_pWbWindow) return true; // already ok

    Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))) return false;

    int xcen = (pWbScreen->Width - _machineWidth);
    int ycen = (pWbScreen->Height - _machineHeight);
    if(xcen<0) xcen=0;
    xcen>>=1;
    if(ycen<0) ycen=0;
    ycen>>=1;
//    printf("openWindow:_machineWidth:%d _machineHeight:%d xcen:%d ycen:%d \n",_machineWidth,_machineHeight,xcen,ycen);

// struct BitMap * __stdargs AllocBitMap( ULONG sizex, ULONG sizey, ULONG depth, ULONG flags, CONST struct BitMap *friend_bitmap );

    _sWbWinSBitmap = AllocBitMap(_machineWidth*_maxzoomfactor,_machineHeight*_maxzoomfactor,
            pWbScreen->RastPort.BitMap->Depth,BMF_CLEAR|BMF_DISPLAYABLE,pWbScreen->RastPort.BitMap);
    if(_sWbWinSBitmap) {

        _pWbWindow = (Window *)OpenWindowTags(NULL,
        WA_Left,xcen,
        WA_Top,ycen,
     //   WA_Width, _machineWidth,
     //   WA_Height, _machineHeight,
        WA_InnerWidth, _machineWidth,
        WA_InnerHeight, _machineHeight,
        WA_MaxWidth,  _machineWidth*_maxzoomfactor,
        WA_MaxHeight, _machineHeight*_maxzoomfactor,
        WA_MinWidth, _machineWidth,
        WA_MinHeight, _machineHeight,
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY /*|
            IDCMP_NEWSIZE*/ /*| IDCMP_INTUITICKS*/ | IDCMP_CLOSEWINDOW,

        WA_Flags, /*WFLG_SIZEGADGET*/ /*| WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM |

            */ WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE /*|
            WFLG_SUPER_BITMAP*/
             | WFLG_GIMMEZEROZERO
           // | WFLG_NOCAREREFRESH
             | WFLG_SMART_REFRESH
            //| WFLG_SIMPLE_REFRESH
             | ((_maxzoomfactor>1)?WFLG_SIZEGADGET:0)
            ,
     //   WA_Gadgets, &(SideGad),
        WA_Title,(ULONG) "Mame 0.106 Krb ", /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,

   //     WA_SuperBitMap, (ULONG)_sWbWinSBitmap,
        TAG_DONE
        );
    } // end if sbm ok
    UnlockPubScreen(NULL,pWbScreen);

    if( _pWbWindow == NULL ) return false;

    if(CyberGfxBase && _sWbWinSBitmap &&  GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_ISCYBERGFX))
    {
        _PixelFmt = GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_PIXFMT);
        _PixelBytes = GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_BPPIX);
    } else
    {
        _PixelFmt = PIXFMT_LUT8;
        _PixelBytes = 1;
    }
    return true;

}
void Intuition_Window::close()
{
    if(_pWbWindow) CloseWindow(_pWbWindow);
    if(_sWbWinSBitmap) FreeBitMap(_sWbWinSBitmap);
    _pWbWindow = NULL;
    _sWbWinSBitmap = NULL;
}
MsgPort *Intuition_Window::userPort()
{
    if(!_pWbWindow) return NULL;
    return _pWbWindow->UserPort;
}
RastPort *Intuition_Window::rastPort()
{
    if(!_pWbWindow) return NULL;
    return _pWbWindow->RPort;
}
BitMap *Intuition_Window::bitmap()
{
    if(!_pWbWindow) return NULL;
    return _sWbWinSBitmap;
}

void Intuition_Window::drawRastPort_CGX(_mame_display *display,Paletted *pRemap)
{
    if(_pWbWindow && _sWbWinSBitmap)
    {
        _width = (int)(_pWbWindow->GZZWidth);
        _height = (int)(_pWbWindow->GZZHeight);
        IntuitionDrawable::drawRastPort_CGX(display,pRemap);

        BltBitMapRastPort( _sWbWinSBitmap,//CONST struct BitMap *srcBitMap,
                           0,0, //LONG xSrc, LONG ySrc,
                           _pWbWindow->RPort,//struct RastPort *destRP,
                           0,0,//LONG xDest, LONG yDest,
                           _width, _height,
                           0x00c0//ULONG minterm  -> copy minterm.
                           );
    }
}
// ----------------------- scalable
Intuition_ScaleWindow::Intuition_ScaleWindow(const AmigaDisplay::params &params)
    :Intuition_Window(params)
{
    _maxzoomfactor = 3;
    _useScale = 1;
}
Intuition_ScaleWindow::~Intuition_ScaleWindow() {}

// ----------------------- end scalable
Display_CGX::Display_CGX()
: AmigaDisplay()
, _drawable(NULL)
, _remap(NULL)
,_params({0})
,_forcedModeId(~0)
{

}
Display_CGX::~Display_CGX()
{
    if(_drawable) delete _drawable;
    if(_remap) delete _remap;
}
bool Display_CGX::open(const AmigaDisplay::params &pparams)
{
   if(_drawable) return true; // already ok
   _params=pparams;

    if(_params._flags & DISPFLAG_STARTWITHWINDOW)
    {
        _drawable = new Intuition_ScaleWindow(pparams);
    } else
    {
        _drawable = new Intuition_Screen(pparams);
    }

    if(!_drawable) return false;
    // must open before computing remap to get correct pixel format.
    if(!_drawable->open())
    {
        close();
        return false;
    }

    if( (_drawable->flags() & DISPFLAG_INTUITIONPALETTE) && _drawable->screen() )
    {
        // means 8bit screen with palette manageable with intuition.
        _remap = new Paletted_Screen8(_drawable->screen());
    } else
    {
        // if is WB window and WB is <=8bit
        bool remapIsWorkbench=false;
        if(_params._flags & DISPFLAG_STARTWITHWINDOW  )
        {
            Screen *pWbScreen;
            if ((pWbScreen = LockPubScreen(NULL)) &&
                pWbScreen->RastPort.BitMap &&
                pWbScreen->RastPort.BitMap->Depth <=8)
            {
                UnlockPubScreen(NULL,pWbScreen);
                _remap = new Paletted_Pens8(pWbScreen);
            }
        }
        if(!_remap)
        if((pparams._video_attributes & VIDEO_RGB_DIRECT)==0 &&
            pparams._colorsIndexLength>0)
        {
            _remap = new Paletted_CGX(pparams,_drawable->pixelFmt(),_drawable->pixelBytes());
        } else
        if((pparams._video_attributes & VIDEO_RGB_DIRECT)!=0 && pparams._driverDepth == 15)
        {
            Paletted_CGX *p =  new Paletted_CGX(pparams,_drawable->pixelFmt(),_drawable->pixelBytes());
            _remap =p;
            p->updatePaletteRemap15b(); // once for all.
        }
    }

    return true;
}
int Display_CGX::switchFullscreen()
{
   close();
   _params._flags ^= DISPFLAG_STARTWITHWINDOW; // switch bit.
   open(_params);
}
void Display_CGX::close()
{
    if(_drawable)
    {
        _drawable->close();
        delete _drawable;
        _drawable = NULL;
    }
    if(_remap)
    {
        delete _remap;
        _remap = NULL;
    }
}
int Display_CGX::good()
{
    if(_drawable && _drawable->userPort()) return 1;
    return 0;
}
void Display_CGX::draw(_mame_display *display)
{
    if(!_drawable) return;

    // - - update palette if exist and is needed.

    if(_remap && ((display->changed_flags & GAME_PALETTE_CHANGED) !=0 || _remap->needRemap()))
    {
        _remap->updatePaletteRemap(display);
    }
    _drawable->drawRastPort_CGX(display,_remap);
}
MsgPort *Display_CGX::userPort()
{
    if(!_drawable) return NULL;
    return _drawable->userPort();
}

void Display_CGX::WaitFrame()
{
//    RastPort *rp = _drawable->rastPort();
//    if(!CyberGfxBase || !rp || GetCyberMapAttr(rp->BitMap,CYBRMATTR_ISCYBERGFX)==0 )
//    {
         WaitTOF();
         return;
//    }
    // would theorically do 60Hz on 60Hz screens...
 //   WaitBOVP(rp->S);

}
