#include "amiga106_video_intuition.h"

#include "amiga106_video_os3.h"
#include "amiga106_video_cgx.h"
#include "amiga106_video_cgxscalepixelarray.h"
#include "amiga106_config.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "version.h"
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

#include <stdio.h>
#include <stdlib.h>
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

void IntuitionDrawable::getGeometry(_mame_display *display,int &cenx,int &ceny,int &ww,int &hh, int &sourcewidth, int &sourceheight)
{

    // +1 because goes 0,319
    sourcewidth = (display->game_visible_area.max_x - display->game_visible_area.min_x)+1;
    sourceheight =( display->game_visible_area.max_y - display->game_visible_area.min_y)+1;
    if(_flags & ORIENTATION_SWAP_XY)
    {
        doSwap(sourcewidth,sourceheight);
    }

    if((_width>sourcewidth || _height>sourceheight) && _useScale)
    {   // like window mode or perfect screen fit.
        ww = _width;
        hh = _height;
        // let's consider scale always take all available screen,
        // which is the case for window mode.
        cenx = 0;
        ceny = 0;
    } else
    {   // screen mode: it's better keep pixel per pixel resolution and create adapted picasso modes.
        MameConfig::Display_PerScreenMode &config = getMainConfig().display().getActiveMode();

        if(config._FSscaleMode == MameConfig::FSScaleMode::ScaleToBorder)
        {
            int sourceratio = (sourcewidth<<16)/sourceheight;
            int screenratio = (_width<<16)/_height;
            if(sourceratio==screenratio)
            {   // like scaletostretch
                ww = _width;
                hh = _height;
                cenx = 0;
                ceny = 0;
            }else if(sourceratio<screenratio)
            {
//                printf("vertical border touch: sourcewidth:%d  sourceheight:%d _width:%d _height:%d\n"
//                       ,sourcewidth,sourceheight,_width,_height);
                // vertical border touch
                ceny = 0;
                hh = _height;
                ww = sourcewidth*_height/sourceheight;
                cenx = _width-ww;
                cenx>>=1;
//                printf("ww:%d  hh:%d cenx:%d ceny:%d\n"
//                       ,ww,hh,cenx,ceny);
            } else
            {
                // horizontal border touch
                ww = _width;
                cenx = 0;
                hh = sourceheight*_width/sourcewidth;
                ceny = _height-hh;
                ceny>>=1;
            }
        } else if(config._FSscaleMode == MameConfig::FSScaleMode::ScaleToStretch)
        {
            ww = _width;
            hh = _height;
            cenx = 0;
            ceny = 0;
        } else
            // that, or default.
       // if(config._FSscaleMode == MameConfig::FSScaleMode::CenterWithNoScale)
        {
            ww = sourcewidth;
            hh = sourceheight;

            cenx = _width-sourcewidth;
            ceny = _height-sourceheight;
            cenx>>=1;
            ceny>>=1;
        }

    }
    // could happen if screen more little than source.
    if(cenx<0) cenx=0;
    if(ceny<0) ceny=0;
    if(hh+ceny>_height){ hh=_height; ceny=0; }// fast cheap clipping
    if(ww+cenx>_width) { ww=_width; cenx=0; }
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
    // that stupid OS function (or driver) want SA_Depth,24 for 32bit depth, or fail.
    if(_screenDepthAsked == 32 )_screenDepthAsked =24;

 	_pScreen = OpenScreenTags( NULL,
			SA_DisplayID,_ScreenModeId,
                        SA_Title, (ULONG)"MAME", // used as ID by promotion tools and else ?
                        SA_Width, _fullscreenWidth,
                        SA_Height,_fullscreenHeight,
                        SA_Depth,_screenDepthAsked,
//                        SA_Behind,TRUE,    /* Open behind */
                        SA_Quiet,TRUE,     /* quiet */
			SA_Type,CUSTOMSCREEN,
			SA_Colors,(ULONG)&colspec[0],
                        0 );
//    printf("screen:%08x\n",(int)_pScreen);
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
//   printf(" ***** ** Intuition_Window CREATE:%d %d %08x\n",params._width,params._height,params._flags);

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
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/
            IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY | IDCMP_CHANGEWINDOW |
            IDCMP_NEWSIZE /*| IDCMP_INTUITICKS*/ | IDCMP_CLOSEWINDOW,

        WA_Flags, /*WFLG_SIZEGADGET*/ /*| WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM |

            */ WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE /*|
            WFLG_SUPER_BITMAP*/
             | WFLG_GIMMEZEROZERO
           // | WFLG_NOCAREREFRESH
             | WFLG_SMART_REFRESH
            //| WFLG_SIMPLE_REFRESH
             | ((_maxzoomfactor>1)?WFLG_SIZEGADGET:0)
            ,
        WA_Title,(ULONG)APPNAMEA, /* take title from version string */
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
        if(useCgxForWB)
        {
            if(_params._flags & DISPFLAG_USESCALEPIXARRAY)
                _drawable =(IntuitionDrawable *)new Intuition_Window_CGXScale(pparams);
            else
                _drawable =(IntuitionDrawable *)new Intuition_Window_CGX(pparams);
        } else
        {
            _drawable =(IntuitionDrawable *)new Intuition_Window_OS3(pparams);
        }

    } else
    {   // open full screen
        // if cgx present
        bool useCgx = Intuition_Screen_CGX::useForThisMode(pparams._forcedModeID);

        if(useCgx)
        {
            if(_params._flags & DISPFLAG_USESCALEPIXARRAY)
                _drawable =(IntuitionDrawable *) new Intuition_Screen_CGXScale(pparams);
            else
                _drawable =(IntuitionDrawable *) new Intuition_Screen_CGX(pparams);
        } else
        {
            _drawable =(IntuitionDrawable *)new Intuition_Screen_OS3(pparams);
        }
    }

    if(!_drawable) return false;

    if(!_drawable->open())
    {
        close();
        return false;
    }

    return true;
}
// needed for RGB mode.
void IntuitionDisplay::init_rgb_components(unsigned int *rgbcomponents)
{
    const ULONG fritata = (VIDEO_RGB_DIRECT|VIDEO_NEEDS_6BITS_PER_GUN );
    if(  (_params._video_attributes & fritata) == fritata)
    {
        rgbcomponents[0] = 0x00ff0000;
        rgbcomponents[1] = 0x0000ff00;
        rgbcomponents[2] = 0x000000ff;
    } else
    {
        rgbcomponents[0] = 0x00007c00;
        rgbcomponents[1] = 0x000003e0;
        rgbcomponents[2] = 0x0000001f;
    }
}
bool IntuitionDisplay::switchFullscreen()
{
   close();
   _params._flags ^= DISPFLAG_STARTWITHWINDOW; // switch bit.
   return open(_params);
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


