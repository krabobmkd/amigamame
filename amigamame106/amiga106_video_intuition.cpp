#include "amiga106_video_intuition.h"

#include "amiga106_video_intuition_tbufcsb.h"
#include "amiga106_video_os3.h"
#include "amiga106_video_cgx.h"
#include "amiga106_video_cgxscalepixelarray.h"
#include "amiga106_video_drawprogress.h"
#include "amiga106_config.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include "version.h"
#include <string>
// Amiga
extern "C" {
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <graphics/modeid.h>
    #include <graphics/text.h>
    #include <hardware/blit.h>
    // actual dreaded Layer struct
    #include <graphics/clip.h>
}

extern "C" {
    #include "intuiuncollide.h"
    // from mame
    #include "mame.h"
    // just to get game name for title
    #include "driver.h"
    #include "video.h"
    #include "mamecore.h"
    #include "osdepend.h"
    #include "palette.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

TripleBuffer::TripleBuffer()
    : _tripleBufferInitOk(0)
    ,_lastIndexDrawn(0)
    ,_indexToDraw(0)
    ,_d(0)
{

}

IntuitionDrawable::IntuitionDrawable(int flags)
: _widthtarget(0),_heighttarget(0)
, _widthphys(0),_heightphys(0) // target screen may be WB or private
, _screenshiftx(0),_screenshifty(0)
, _useScale(0)
, _flags(flags), _heightBufferSwitch(0),_heightBufferSwitchApplied(0)
, _greyPen(0),_blackpen(1) // default on WB screen
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
    //printf("IntuitionDrawable::getGeometry :%d %d\n",sourcewidth,sourceheight);
    if(_flags & ORIENTATION_SWAP_XY)
    {
      //  printf("swap\n");
        doSwap(sourcewidth,sourceheight);
    }

    if((_widthtarget>sourcewidth || _heighttarget>sourceheight) && _useScale)
    {   // like window mode or perfect screen fit.
        ww = _widthtarget;
        hh = _heighttarget;
        // let's consider scale always take all available screen,
        // which is the case for window mode.

        // when wb zoom
        cenx = _screenshiftx; // 0 all the time except WB screen direct rendering
        ceny = _screenshifty; // 0 ...
    } else
    {   // screen mode: it's better keep pixel per pixel resolution and create adapted picasso modes.
        MameConfig::Display_PerScreenMode &config = getMainConfig().display().getActiveMode();

        if(config._FSscaleMode == MameConfig::FSScaleMode::ScaleToBorder)
        {
            int sourceratio = (sourcewidth<<16)/sourceheight;
            int screenratio = (_widthtarget<<16)/_heighttarget;
            if(sourceratio==screenratio)
            {   // like scaletostretch
                ww = _widthtarget;
                hh = _heighttarget;
                cenx = _screenshiftx;
                ceny = _screenshifty;
            }else if(sourceratio<screenratio)
            {
//                printf("vertical border touch: sourcewidth:%d  sourceheight:%d _width:%d _height:%d\n"
//                       ,sourcewidth,sourceheight,_width,_height);
                // vertical border touch
                ceny = 0;
                hh = _heighttarget;
                ww = sourcewidth*_heighttarget/sourceheight;
                cenx = _widthtarget-ww;
                cenx>>=1;
//                printf("ww:%d  hh:%d cenx:%d ceny:%d\n"
//                       ,ww,hh,cenx,ceny);
            } else
            {
                // horizontal border touch
                ww = _widthtarget;
                cenx = 0;
                hh = sourceheight*_widthtarget/sourcewidth;
                ceny = _heighttarget-hh;
                ceny>>=1;
            }
        } else if(config._FSscaleMode == MameConfig::FSScaleMode::ScaleToStretch)
        {
            ww = _widthtarget;
            hh = _heighttarget;
            cenx = _screenshiftx;
            ceny = _screenshifty;
        } else
            // that, or default.
       // if(config._FSscaleMode == MameConfig::FSScaleMode::CenterWithNoScale)
        {
            ww = sourcewidth;
            hh = sourceheight;

            cenx = _widthtarget-sourcewidth;
            ceny = _heighttarget-sourceheight;
            cenx>>=1;
            ceny>>=1;
            cenx += _screenshiftx;
            ceny += _screenshifty;
        }

    }

    // could happen if screen more little than source.
    if(cenx<0) cenx=0;
    if(ceny<0) ceny=0;
    if(hh+ceny>_heightphys){ hh=_heightphys-ceny; }// fast cheap clipping
    if(ww+cenx>_widthphys) { ww=_widthphys-cenx; }

    if(_heightBufferSwitch) ceny+= _heightBufferSwitchApplied;
}
// void IntuitionDrawable::waitFrame()
// {
//     // default behaviour, 50Hz or less.
//     WaitTOF();
// }

// - - - - - - - - -
Intuition_Screen::Intuition_Screen(const AbstractDisplay::params &params)
    : IntuitionDrawable(params._flags)
    , _pScreen(NULL)
    , _pScreenWindow(NULL)

    , _ScreenModeId(params._forcedModeID)
    , _ScreenDepthAsked(params._forcedDepth)
    , _fullscreenWidth(0)
    , _fullscreenHeight(0)
    , _pMouseRaster(NULL)
    , _pTripleBufferImpl(NULL)
{
}

bool Intuition_Screen::open()
{
     if(_pScreenWindow) return true; // already open.

    if(_ScreenModeId == INVALID_ID)
    {
        loginfo(2,"Can't find a screen mode ");
        return false; // set by inherited class.
     }

   // note: all this is regular OS intuition, no CGX
	struct ColorSpec colspec[6]={ // let's do it amiga default like
                0,  0,0,0, //black  inverte black and grey against amiga default.
                                // this will draw screen black by default.
                1,  8,8,8,  // grey
                2,  15,15,15,
                3,  1,8,15, // blue 1
                4,  0,1,8, // blue 2
                // end
                -1,0,0,0};
     _greyPen = 1;
    _blackpen = 0;

    // that stupid OS function (or driver) want SA_Depth,24 for 32bit depth, or fail.
    if(_ScreenDepthAsked == 32 )_ScreenDepthAsked =24;
    //printf("ask depth:%d\n",_ScreenDepthAsked);

    ULONG appliedHeight = _fullscreenHeight;
    if(_flags & DISPFLAG_USEHEIGHTBUFFER)
    {
        _heightBufferSwitch = 1;
        _heightBufferSwitchApplied = _fullscreenHeight;
        appliedHeight*=2;
    }


 	_pScreen = OpenScreenTags( NULL,
			SA_DisplayID,_ScreenModeId,
                        SA_Title, (ULONG)"MAME", // used as ID by promotion tools and else ?
                        SA_Width, _fullscreenWidth,
                        SA_Height,appliedHeight,
                        SA_Depth,_ScreenDepthAsked,
//                        SA_Behind,TRUE,    /* Open behind */
                        SA_Quiet,TRUE,     /* quiet */
			SA_Type,CUSTOMSCREEN,
			SA_Interleaved,FALSE, // test, may make C2P faster
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
                    WA_DragBar,0,                    WA_RptQueue,0, // empeach key repeat messages
                    WA_GimmeZeroZero,FALSE, 
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
    _widthtarget = _widthphys = _fullscreenWidth;
    _heighttarget = _heightphys = _fullscreenHeight;

    if(_flags & DISPFLAG_USETRIPLEBUFFER)
    {

        if(_pTripleBufferImpl) delete _pTripleBufferImpl;
        _pTripleBufferImpl = new TripleBuffer_CSB(*this); // could fail, in which case back to direct rendering
        _pTripleBufferImpl->init();

    }

    return true;
}

void Intuition_Screen::close()
{
    IntuitionDrawable::close();

    if(_pScreenWindow) CloseWindow(_pScreenWindow);
    _pScreenWindow = NULL;

    if(_pTripleBufferImpl){
        delete _pTripleBufferImpl;
        _pTripleBufferImpl = NULL;
    }
    if(_pScreen) CloseScreen(_pScreen);
    _pScreen = NULL;
    if(_pMouseRaster) FreeRaster( (PLANEPTR) _pMouseRaster ,8,8);
    _pMouseRaster = NULL;
}
Window *Intuition_Screen::window()
{
    return _pScreenWindow;
}
RastPort *Intuition_Screen::rastPort()
{
    if(!_pScreen) return NULL;
    // may use triple buffer
    if(_pTripleBufferImpl &&
    _pTripleBufferImpl->_tripleBufferInitOk)
    {
        return _pTripleBufferImpl->rastPort();
    }
   return &_pScreen->RastPort;
}
Screen *Intuition_Screen::screen()
{ return _pScreen;
}
BitMap *Intuition_Screen::bitmap()
{    
    if(_pTripleBufferImpl &&
    _pTripleBufferImpl->_tripleBufferInitOk)  // may use triple buffer
       return _pTripleBufferImpl->bitmap();

    return _pScreen->RastPort.BitMap;
}
// void Intuition_Screen::waitFrame()
// {
//     WaitTOF();
//     // dangerous, can't use triple buffer for synch bevause call too generic ?
//     //
//     // if(!_pTripleBufferImpl) {
//     //     WaitTOF();
//     //     return;
//     // }
//     // _pTripleBufferImpl->waitFrame();
// }
// - - - - - - - - -

Intuition_Window::Intuition_Window(const AbstractDisplay::params &params)
    : IntuitionDrawable(params._flags)
    , _pWbWindow(NULL)
    , _machineWidth(params._width),_machineHeight(params._height)
    , _maxzoomfactor(1)
    , _allowDirectDraw(0)
    , _useLastPosition(0)
{
//   printf(" ***** ** Intuition_Window CREATE:%d %d %08x\n",params._width,params._height,params._flags);
    // "windows can't use SVP double buffer"
    _flags &= ~DISPFLAG_USEHEIGHTBUFFER;

    if(params._flags & ORIENTATION_SWAP_XY)
    {
        doSwap(_machineWidth,_machineHeight);

    }
  // this enable or disbale window sizing:
  _maxzoomfactor = 3;
  _useScale = 1;

    MameConfig::Controls &configControls = getMainConfig().controls();
    MameConfig::Misc &configMisc = getMainConfig().misc();

    _allowDirectDraw = ((configMisc._Optims & OPTIMFLAGS_DIRECTWGXWIN) != 0);
}
Intuition_Window::~Intuition_Window()
{
    close();
}

inline void WindowLimitsGZZ(struct Window *window, LONG widthMin, LONG heightMin, ULONG widthMax, ULONG heightMax)
{
    LONG bordertotalwidth = window->BorderLeft + window->BorderRight;
    LONG bordertotalheight = window->BorderTop + window->BorderBottom;

    WindowLimits(window,
                widthMin+bordertotalwidth,
                heightMin+bordertotalheight,
                widthMax+bordertotalwidth, //max
                heightMax+bordertotalheight
                );
}

bool Intuition_Window::open()
{
    if(_pWbWindow) return true; // already ok

    Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))
            ) return false;
    _widthphys = pWbScreen->Width;
    _heightphys = pWbScreen->Height;

    // window title
    static std::string windowTitle;
    windowTitle = APPNAMEA;
    if(Machine && Machine->gamedrv && Machine->gamedrv->description)
    {
        windowTitle += "  -  ";
        windowTitle += Machine->gamedrv->description;
    }


    // open window in center of workbench
    int xcen = (pWbScreen->Width - _machineWidth);
    int ycen = (pWbScreen->Height - _machineHeight);
    if(xcen<0) xcen=0;
    xcen>>=1;
    if(ycen<0) ycen=0;
    ycen>>=1;

    int iwidth = _machineWidth;
    int iheight = _machineHeight;

    if(_useLastPosition)
    {
        xcen = _lastx;
        ycen = _lasty;
        iwidth = _lastWidth;
        iheight = _lastHeight;
    }
//    printf("openWindow:_machineWidth:%d _machineHeight:%d xcen:%d ycen:%d \n",_machineWidth,_machineHeight,xcen,ycen);

    _pWbWindow = (Window *)OpenWindowTags(NULL,
        WA_Left,xcen,
        WA_Top,ycen,
        // just so you know intuition code,
        // when WFLG_GIMMEZEROZERO flag is not set, window size
        // are given in full size including borders. Borders size can change
        // with intuition prefs and font size, so then you basically don't know
        // the inner size.  n this case size is given with this: (...)
        // WA_Width, _machineWidth,
        // WA_Height, _machineHeight,
        // WA_MaxWidth,  _machineWidth*_maxzoomfactor,
        // WA_MaxHeight, _machineHeight*_maxzoomfactor,
        // WA_MinWidth, _machineWidth,
        // WA_MinHeight, _machineHeight,
        // ... When WFLG_GIMMEZEROZERO (GZZ) flag is set,
        // the coords must be given with WA_InnerWidth WA_InnerHeight
        WA_InnerWidth, iwidth, // because WFLG_GIMMEZEROZERO .
        WA_InnerHeight, iheight,
        // ... then with GZZ, Min/Max must be set after window creation with WindowLimits(),
        // and like the window is created, we know the borders size.
        WA_RptQueue,0, // no rawkey repeat messages
        WA_RMBTrap,TRUE, // allow second mouse button for our usage, not menu, when window is focus.
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/
            IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY | IDCMP_CHANGEWINDOW |
            IDCMP_NEWSIZE /*| IDCMP_INTUITICKS*/ | IDCMP_CLOSEWINDOW
            /*| IDCMP_REFRESHWINDOW*/,

        WA_Flags, /*WFLG_SIZEGADGET*/ /*| WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM |

            */ WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE
            //|WFLG_SUPER_BITMAP     // also use another layer cliprect mecanism.
           // | WFLG_NOCAREREFRESH
            // | WFLG_SMART_REFRESH
            | WFLG_SIMPLE_REFRESH // important for direct draw optimisation.
            | WFLG_GIMMEZEROZERO
             | ((_maxzoomfactor>1)?WFLG_SIZEGADGET:0)
            ,
        WA_Title,(ULONG)windowTitle.c_str(), /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,
        TAG_DONE
        );
//    } // end if sbm ok
    UnlockPubScreen(NULL,pWbScreen);
    if(_pWbWindow == NULL) return false;

    // have to set the min/max size after creation
    // thanks people at https://developer.amigaos3.net/forum .
    WindowLimitsGZZ(_pWbWindow,
                _machineWidth,//widthmin
                _machineHeight,//heightmin
                _machineWidth*_maxzoomfactor, //max
                _machineHeight*_maxzoomfactor
                );

    return true;

}
void Intuition_Window::close()
{
    IntuitionDrawable::close();

    if(_pWbWindow) CloseWindow(_pWbWindow);
    _pWbWindow = NULL;

}
#ifdef USE_DIRECT_WB_RENDERING
//doesnt work at all! how to check that ? Layer.library ?
bool Intuition_Window::isOnTop()
{
    if(!_pWbWindow || !_pWbWindow->WScreen) return false;

    Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))
        ) return false;
    bool isontop =  (pWbScreen->FirstWindow == _pWbWindow);
    UnlockPubScreen(NULL,pWbScreen);

    return isontop;
}
#endif
Screen *Intuition_Window::screen()
{
    if(!_pWbWindow) return NULL;
    return _pWbWindow->WScreen;
}
Window *Intuition_Window::window()
{
    return _pWbWindow;
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
, _pProgressDrawer(NULL)
, _params({0})
{

}
IntuitionDisplay::~IntuitionDisplay()
{
    close();
    if(_pProgressDrawer) delete _pProgressDrawer;
}
bool IntuitionDisplay::open(const AbstractDisplay::params &pparams)
{
   if(_drawable) return true; // already ok
   _params=pparams;

    if(_params._flags & DISPFLAG_STARTWITHWINDOW)
    {
        // type of window depends of the WB nature.
        bool useCgxForWB = Intuition_Window_CGX::useForWbWindow();
        Intuition_Window *piwin=NULL;
        if(useCgxForWB)        
        {
            if(_params._flags & DISPFLAG_USESCALEPIXARRAY)
                piwin =(Intuition_Window *)new Intuition_Window_CGXScale(pparams);
            else
                piwin =(Intuition_Window *)new Intuition_Window_CGX(pparams);
        } else
        {
            piwin =(Intuition_Window *)new Intuition_Window_OS3(pparams);
        }          
        _drawable = (IntuitionDrawable *)piwin;
        if(!_drawable) return false;
        // to avoid a little glicth when repositioning, restart window with known position.
        // before it is opened.
        if(_params._flags & DISPFLAG_STARTWITHWINDOW && _params._wingeo._valid) // if set
        {
            WORD wwidth = _params._wingeo._window_width;
            WORD wheight = _params._wingeo._window_height;
            if(_params._flags & ORIENTATION_SWAP_XY) doSwap(wwidth,wheight);

            piwin->presetBox(_params._wingeo._window_posx,
                            _params._wingeo._window_posy,
                            wwidth,wheight);
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
//void IntuitionDisplay::obtainPens()
//{
//    // then try alloc pens...
//    struct pencolor{ UBYTE r,g,b;};
//    static struct pencolor pencolors[]={
//        {255,255,255},
//        {0,0,0},
//        {128,128,128},
//        {0,192,255},
//        {0,24,240},
//    };

//    Screen *screen = _drawable->screen();
// printf("colormap:%08x\n",(int) screen->ViewPort.ColorMap);
//    if(screen && screen->ViewPort.ColorMap)
//    {
//        struct ColorMap *pColorMap = screen->ViewPort.ColorMap;
//        for(int i=0;i<(int)sizeof(pencolors)/sizeof(struct pencolor);i++)
//        {

//printf("go i:%d\n",(int) i);

//            const struct pencolor &c = pencolors[i];
//            _pens[i] = ObtainBestPen( pColorMap, ((ULONG)c.r)<<24, ((ULONG)c.g)<<24,
//                ((ULONG)c.b)<<24, TAG_END);
//        }
//    }
//     printf("obtainPens end\n");
//}
//void IntuitionDisplay::releasePens()
//{
//    Screen *screen = _drawable->screen();

//    if(screen && screen->ViewPort.ColorMap)
//    {
//        struct ColorMap *pColorMap = screen->ViewPort.ColorMap;
//        for(int i=0;i<(int)sizeof(_pens)/sizeof(int);i++)
//        {
//            if(_pens[i] != -1) ReleasePen( pColorMap, _pens[i]);
//        }
//    }
//}
void IntuitionDisplay::syncWindowGeo()
{
    // if current context is a floating window...
    if(_drawable && (_params._flags & DISPFLAG_STARTWITHWINDOW) && _drawable->window())
    {
        Window *pwin = _drawable->window();
        _params._wingeo._window_posx = pwin->LeftEdge;
        _params._wingeo._window_posy = pwin->TopEdge;
        _params._wingeo._window_width = pwin->Width -(pwin->BorderLeft+pwin->BorderRight);
        _params._wingeo._window_height = pwin->Height -(pwin->BorderTop+pwin->BorderBottom);
        if(_params._flags & ORIENTATION_SWAP_XY)
        {
            doSwap(_params._wingeo._window_width,_params._wingeo._window_height);
        }

        _params._wingeo._valid = 1;
      //  printf("getWindowGeo:%d\n",(int)_params._wingeo._window_posx);
    }
}
bool IntuitionDisplay::switchFullscreen()
{
    syncWindowGeo();
    close();
    _params._flags ^= DISPFLAG_STARTWITHWINDOW; // switch bit.
    return open(_params);
}
void IntuitionDisplay::close()
{
    syncWindowGeo();

    if(_drawable)
    {
        //releasePens();
        _drawable->close();
        delete _drawable;
        _drawable = NULL;
    }
}
int IntuitionDisplay::good()
{
    if(_drawable && _drawable->window()) return 1;
    return 0;
}
void IntuitionDisplay::draw(_mame_display *display)
{
    if(!_drawable) return;
    _drawable->draw(display);

}
// draw a progress bar using only graphics.library
// so this would work on any type of screen.
void IntuitionDisplay::drawProgress(int per256, int enm)
{
    if(!_drawable) return;

    static int lastprog = -1;
    if(lastprog == per256)
    {   // value already drawn.
        return;
    }
    lastprog = per256;

    if(!_pProgressDrawer) _pProgressDrawer = new IntuiProgressBar(_drawable);
    if(_pProgressDrawer)
    {
        _pProgressDrawer->drawProgress( per256, enm);
    }

    if(per256 >=256)
    {
        if(_pProgressDrawer) delete _pProgressDrawer;
        _pProgressDrawer = NULL;
        return; // ok, last used to clear.
    }
}
MsgPort *IntuitionDisplay::userPort()
{
    if(!_drawable) return NULL;
    Window *w = _drawable->window();
    if(!w) return NULL;
    return w->UserPort;
}
void IntuitionDisplay::WaitFrame()
{
    if(!_drawable) return
    // _drawable->waitFrame(); // no, can't be apired with triple buffr wait finally.
    WaitTOF();
}

WindowGeo IntuitionDisplay::getWindowGeometry()  // to save/reload
{
    syncWindowGeo();

    return _params._wingeo;
}


/* tool: test if RastPort currently need layer.library clippings, or not.
 we can use direct pixel copy on workbench bitmap in the simple case.
  **** This is considered dangerous code. ****
*/
bool isRastPortComplete(RastPort *rp,WORD w, WORD h)
{
    Layer *lr = rp->Layer;
    if(!lr) return false; // actually happens a lot when moving/resizing/recreating and could mean windows is being reworked by OS.
    bool isComplete=false;
  // ObtainSemaphore(&(lr->Lock)); // must be done by caller, because following direct draw must also keep lock.
    ClipRect *cr = lr->ClipRect;
    if(cr)
    {
        // to be complete, there must be a single rect. of the whole size.
        // else it is obscured or cliped.
         if( (cr->Next == NULL) || (cr->Next == cr))
         {
            WORD crw = (cr->bounds.MaxX - cr->bounds.MinX)+1;
            WORD crh = (cr->bounds.MaxY - cr->bounds.MinY)+1;
            if(w == crw && h == crh) isComplete = true;
         } // end if just one cliprect
    } /*else -> our window is simple_refresh, no superbitmap use.
    if(lr->SuperClipRect)
    {
        ClipRect *cr = lr->SuperClipRect;
        // to be complete, has
         //  if just one
         if( (cr->Next == NULL) || (cr->Next == cr))
         {
            WORD crw = (cr->bounds.MaxX - cr->bounds.MinX)+1;
            WORD crh = (cr->bounds.MaxY - cr->bounds.MinY)+1;
            if(w == crw && h == crh) isComplete = true;
         } // end if just one cliprect
    }*/

//    ReleaseSemaphore(&(lr->Lock));
    return isComplete;
}
