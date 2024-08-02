#include "amiga106_video_intuition.h"

#include "amiga106_video_os3.h"
#include "amiga106_video_cgx.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

// Amiga
extern "C" {
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <graphics/modeid.h>
   // #include <graphics/displayinfo.h>
}
//#include "intuiuncollide.h"

extern "C" {
    // from mame
    #include "mame.h"
    #include "video.h"
    #include "mamecore.h"
    #include "osdepend.h"
    #include "palette.h"
//    #include "driver.h"
}

// from driver.h
#ifndef ORIENTATION_MASK
    #define ORIENTATION_MASK        		0x0007
    #define	ORIENTATION_FLIP_X				0x0001	/* mirror everything in the X direction */
    #define	ORIENTATION_FLIP_Y				0x0002	/* mirror everything in the Y direction */
    #define ORIENTATION_SWAP_XY				0x0004	/* mirror along the top-left/bottom-right diagonal */

    #define	ROT0							0
    #define	ROT90							(ORIENTATION_SWAP_XY | ORIENTATION_FLIP_X)	/* rotate clockwise 90 degrees */
    #define	ROT180							(ORIENTATION_FLIP_X | ORIENTATION_FLIP_Y)	/* rotate 180 degrees */
    #define	ROT270							(ORIENTATION_SWAP_XY | ORIENTATION_FLIP_Y)	/* rotate counter-clockwise 90 degrees */
#endif

template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }


//void Intuition_Window::drawRastPort_CGX(_mame_display *display,Paletted *pRemap)
//{
//    if(_pWbWindow && _sWbWinSBitmap)
//    {
//        _width = (int)(_pWbWindow->GZZWidth);
//        _height = (int)(_pWbWindow->GZZHeight);
//        IntuitionDrawable::drawRastPort_CGX(display,pRemap);

////        BltBitMapRastPort( _sWbWinSBitmap,//CONST struct BitMap *srcBitMap,
////                           0,0, //LONG xSrc, LONG ySrc,
////                           _pWbWindow->RPort,//struct RastPort *destRP,
////                           0,0,//LONG xDest, LONG yDest,
////                           _width, _height,
////                           0x00c0//ULONG minterm  -> copy minterm.
////                           );
//    }
//}



IntuitionDrawable::IntuitionDrawable(int flags)
: _width(0),_height(0),_useScale(0)
, _flags(flags)
{
//  _trp._checkw=0;
//  _trp._rp.BitMap = NULL;
}
IntuitionDrawable::~IntuitionDrawable()
{
    close();
}
//void IntuitionDrawable::close()
//{
////    if(_trp._rp.BitMap) FreeBitMap(_trp._rp.BitMap);
////    _trp._checkw=0;
////    _trp._rp.BitMap = NULL;
//}
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
// - - - - - - - - -
Intuition_Screen::Intuition_Screen(const AbstractDisplay::params &params)
    : IntuitionDrawable((params._flags&15))
    , _pScreen(NULL)
    , _pScreenWindow(NULL)

    , _ScreenModeId(params._forcedModeID)
    , _fullscreenWidth(0)
    , _fullscreenHeight(0)
    , _screenDepthAsked(8) // default.
    , _pMouseRaster(NULL)
{
//    int width = params._width;
//    int height = params._height;
//    if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);
//    int screenDepth = (params._colorsIndexLength<=256)?8:16; // more would be Display_CGX_TrueColor.

//    if(_ScreenModeId == INVALID_ID)
//    {
//        if(CyberGfxBase)
//        {
//             struct TagItem cgxtags[]={
//                    CYBRBIDTG_NominalWidth,width,
//                    CYBRBIDTG_NominalHeight,height,
//                    CYBRBIDTG_Depth,screenDepth,
//                    TAG_DONE,0 };
//            _ScreenModeId = BestCModeIDTagList(cgxtags);
//            if(_ScreenModeId == INVALID_ID)
//            {
//                logerror("Can't find cyber screen mode for w%d h%d d%d ",width,height,screenDepth);
//                return;
//            }

//        } else
//        {
//            // using OS3 to find mode.
//            if(screenDepth>8)
//            {
//                logerror("Can't find 16b mode without CGX ");
//                return;
//            }
//            _ScreenModeId = BestModeID(
//                    BIDTAG_Depth,8,
//                    BIDTAG_NominalWidth,width,
//                    BIDTAG_NominalHeight,height,
//                    TAG_DONE );
//            if(_ScreenModeId == INVALID_ID)
//            {
//                logerror("Can't find screen mode for w%d h%d d%d ",width,height,screenDepth);
//                return;
//            }
//        }
//    } // end if no mode decided at first

//    // inquire mode
//    if(CyberGfxBase && IsCyberModeID(_ScreenModeId) )
//    {
//        _fullscreenWidth = GetCyberIDAttr( CYBRIDATTR_WIDTH, _ScreenModeId );
//        _fullscreenHeight = GetCyberIDAttr( CYBRIDATTR_HEIGHT, _ScreenModeId );
//        _PixelFmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT, _ScreenModeId );
//        _PixelBytes = GetCyberIDAttr( CYBRIDATTR_BPPIX, _ScreenModeId );
//    } else
//    {
//        LONG v;
//        struct DimensionInfo dims;
//        v = GetDisplayInfoData(NULL, (UBYTE *) &dims, sizeof(struct DimensionInfo),
//                     DTAG_DIMS, _ScreenModeId);
//        if(v>0)
//        {
//            _fullscreenWidth = (int)(dims.Nominal.MaxX - dims.Nominal.MinX)+1;
//            _fullscreenHeight = (int)(dims.Nominal.MaxY - dims.Nominal.MinY)+1;
//            // if game screen big, try some oversan conf.
//            if(_fullscreenWidth< width )
//            {
//                _fullscreenWidth = (int)(dims.MaxOScan.MaxX - dims.MaxOScan.MinX)+1;
//            }
//            if(_fullscreenHeight< height )
//            {
//                _fullscreenHeight = (int)(dims.MaxOScan.MaxY - dims.MaxOScan.MinY)+1;
//            }
//        } else
//        {   // shouldnt happen, fallback
//            _fullscreenWidth = width;
//            _fullscreenHeight = height;
//        }
//        // TODO for ECS we can't do depth 8 should be checked
//        _PixelFmt = PIXFMT_LUT8; // will be treated with WritePixelArray8 anyway.
//        _PixelBytes = 1;
//    }
//    // 8bits screen colors will be managed with LoadRGB32 and direct pixel copy (no clut).
//    if(_PixelFmt == PIXFMT_LUT8) _flags|= DISPFLAG_INTUITIONPALETTE;

}

bool Intuition_Screen::open()
{
    if(_pScreenWindow) return true; // already open.
    if(_ScreenModeId == INVALID_ID)
    {
        logerror("Can't find a screen mode ");
        return false; // set by inherited class.
     }

   // note: all this is regular OS intuition, no CGX
	struct ColorSpec colspec[2]={0,0,0,0,-1,0,0,0};
 	_pScreen = OpenScreenTags( NULL,
			SA_DisplayID,_ScreenModeId,
                        SA_Width, _fullscreenWidth,
                        SA_Height,_fullscreenHeight,
                        SA_Depth,_screenDepthAsked,
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
    IntuitionDrawable::close();
    if(_pScreenWindow) CloseWindow(_pScreenWindow);
    _pScreenWindow = NULL;
    if(_pScreen) CloseScreen(_pScreen);
    _pScreen = NULL;
    if(_pMouseRaster) FreeRaster( (PLANEPTR) _pMouseRaster ,8,8);
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
Screen *Intuition_Screen::screen()
{ return _pScreen;
}
BitMap *Intuition_Screen::bitmap()
{
    return _pScreen->RastPort.BitMap;
}
// - - - - - - - - -

Intuition_Window::Intuition_Window(const AbstractDisplay::params &params) : IntuitionDrawable(params._flags)
    , _pWbWindow(NULL)
    , _machineWidth(params._width),_machineHeight(params._height)
    , _maxzoomfactor(1)
{
   printf(" ***** ** Intuition_Window CREATE:%d %d %08x\n",params._width,params._height,params._flags);

  if(params._flags & ORIENTATION_SWAP_XY) doSwap(_machineWidth,_machineHeight);

  // this enable or disbale window sizing:
  _maxzoomfactor = 3;
  _useScale = 1;
}
Intuition_Window::~Intuition_Window()
{
    close();
}
bool Intuition_Window::open()
{
    if(_pWbWindow) return true; // already ok

    Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))
            ) return false;

    // open window in center of workbench
    int xcen = (pWbScreen->Width - _machineWidth);
    int ycen = (pWbScreen->Height - _machineHeight);
    if(xcen<0) xcen=0;
    xcen>>=1;
    if(ycen<0) ycen=0;
    ycen>>=1;
//    printf("openWindow:_machineWidth:%d _machineHeight:%d xcen:%d ycen:%d \n",_machineWidth,_machineHeight,xcen,ycen);

// struct BitMap * __stdargs AllocBitMap( ULONG sizex, ULONG sizey, ULONG depth, ULONG flags, CONST struct BitMap *friend_bitmap );

//    _sWbWinSBitmap = AllocBitMap(_machineWidth*_maxzoomfactor,_machineHeight*_maxzoomfactor,
//            pWbScreen->RastPort.BitMap->Depth,BMF_CLEAR|BMF_DISPLAYABLE,pWbScreen->RastPort.BitMap);
//    if(_sWbWinSBitmap) {

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
        WA_Title,(ULONG) "Mame 0.106 Krb ", /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,

        TAG_DONE
        );
//    } // end if sbm ok
    UnlockPubScreen(NULL,pWbScreen);

    return ( _pWbWindow != NULL );

}
void Intuition_Window::close()
{
    IntuitionDrawable::close();
    if(_pWbWindow) CloseWindow(_pWbWindow);
    _pWbWindow = NULL;

//    if(_sWbWinSBitmap) FreeBitMap(_sWbWinSBitmap);
//    _sWbWinSBitmap = NULL;
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

// - - - - - --  -
IntuitionDisplay::IntuitionDisplay()
: AbstractDisplay()
, _drawable(NULL)
, _params({0})
{

}
IntuitionDisplay::~IntuitionDisplay()
{
    close();
}
bool IntuitionDisplay::open(const AbstractDisplay::params &pparams)
{
   if(_drawable) return true; // already ok
   _params=pparams;

    if(_params._flags & DISPFLAG_STARTWITHWINDOW)
    {
        // type of window depends of the WB nature.
        bool useCgxForWB = Intuition_Window_CGX::useForWbWindow();
        _drawable =(useCgxForWB)?((IntuitionDrawable *)new Intuition_Window_CGX(pparams) ):
                             ((IntuitionDrawable *)new Intuition_Window_OS3(pparams));
    } else
    {   // open full screen
        // if cgx present
        bool useCgx = Intuition_Screen_CGX::useForThisMode(pparams._forcedModeID);

        _drawable =(useCgx)? ((IntuitionDrawable *)new Intuition_Screen_CGX(pparams)) :
                             ((IntuitionDrawable *)new Intuition_Screen_OS3(pparams));

    }

    if(!_drawable) return false;

    if(!_drawable->open())
    {
        close();
        return false;
    }

//    if( (_drawable->flags() & DISPFLAG_INTUITIONPALETTE) && _drawable->screen() )
//    {
//        // means 8bit screen with palette manageable with intuition.
//        _remap = new Paletted_Screen8(_drawable->screen());
//    } else
//    {
//        // if is WB window and WB is <=8bit
//        bool remapIsWorkbench=false;
//        if(_params._flags & DISPFLAG_STARTWITHWINDOW  )
//        {
//            Screen *pWbScreen;
//            if ((pWbScreen = LockPubScreen(NULL)) &&
//                pWbScreen->RastPort.BitMap &&
//                pWbScreen->RastPort.BitMap->Depth <=8)
//            {
//                UnlockPubScreen(NULL,pWbScreen);
//                _remap = new Paletted_Pens8(pWbScreen);
//            }
//        }
//        if(!_remap)
//        if((pparams._video_attributes & VIDEO_RGB_DIRECT)==0 &&
//            pparams._colorsIndexLength>0)
//        {
//            _remap = new Paletted_CGX(pparams,_drawable->pixelFmt(),_drawable->pixelBytes());
//        } else
//        if((pparams._video_attributes & VIDEO_RGB_DIRECT)!=0 && pparams._driverDepth == 15)
//        {
//            Paletted_CGX *p =  new Paletted_CGX(pparams,_drawable->pixelFmt(),_drawable->pixelBytes());
//            _remap =p;
//            p->updatePaletteRemap15b(); // once for all.
//        }
//    }

    return true;
}
int IntuitionDisplay::switchFullscreen()
{
   close();
   _params._flags ^= DISPFLAG_STARTWITHWINDOW; // switch bit.
   open(_params);
}
void IntuitionDisplay::close()
{
    if(_drawable)
    {
        _drawable->close();
        delete _drawable;
        _drawable = NULL;
    }
}
int IntuitionDisplay::good()
{
    if(_drawable && _drawable->userPort()) return 1;
    return 0;
}
void IntuitionDisplay::draw(_mame_display *display)
{
    if(!_drawable) return;
    _drawable->draw(display);
}
MsgPort *IntuitionDisplay::userPort()
{
    if(!_drawable) return NULL;
    return _drawable->userPort();
}

void IntuitionDisplay::WaitFrame()
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


