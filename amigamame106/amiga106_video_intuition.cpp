#include "amiga106_video_intuition.h"

#include "amiga106_video_intuition_tbufcsb.h"
#include "amiga106_video_os3.h"
#include "amiga106_video_cgx.h"
#include "amiga106_video_cgxscalepixelarray.h"
#include "amiga106_config.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/diskfont.h>
#include "version.h"
// Amiga
extern "C" {
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <graphics/modeid.h>
    #include <graphics/text.h>
    #include <hardware/blit.h>
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



//
extern "C" {
static const unsigned char tlogomamebm[(128/8)*36]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,0,0,0,32,4,0,0,128,0,4,0,192,0,63,255,254,
    0,0,0,96,12,0,1,128,0,12,1,192,0,127,255,252,0,0,0,224,28,0,3,128,0,28,3,192,0,255,255,248,
    0,0,1,224,60,0,7,128,0,60,7,192,1,255,255,240,0,0,3,224,124,0,15,128,0,124,15,192,3,255,255,224,
    0,0,7,224,252,0,31,128,0,252,31,192,7,255,255,192,0,0,15,225,252,0,63,128,1,252,63,192,15,254,0,0,
    0,0,31,227,252,0,127,128,3,252,127,192,31,252,0,0,0,0,63,231,252,0,255,128,7,252,255,192,63,248,0,0,
    0,0,127,239,252,1,255,128,15,253,255,192,127,255,128,0,0,0,255,255,252,3,255,128,31,255,255,192,255,255,0,0,
    0,1,255,255,252,7,255,128,63,255,255,193,255,254,0,0,0,3,255,255,252,15,255,128,127,255,255,195,255,252,0,0,
    0,7,255,255,252,31,255,128,255,255,255,199,255,248,0,0,0,15,255,255,252,63,255,129,255,255,255,207,255,240,0,0,
    0,31,255,255,252,127,255,131,255,255,255,223,255,224,0,0,0,63,243,255,252,255,255,135,255,127,255,191,248,0,0,0,
    0,127,227,255,249,255,191,143,254,127,223,127,240,0,0,0,0,255,195,253,243,255,63,159,252,127,158,255,224,0,0,0,
    1,255,131,249,231,254,63,191,248,127,29,255,255,255,240,0,3,255,3,241,207,252,63,127,240,126,27,255,255,255,224,0,
    7,254,3,225,159,248,62,255,224,124,23,255,255,255,192,0,15,252,3,193,63,240,61,255,192,120,15,255,255,255,128,0,
    31,248,3,128,127,224,59,255,128,112,31,255,255,255,0,0,63,240,3,0,255,192,55,255,0,96,63,255,255,254,0,0,
    127,224,2,1,255,128,47,254,0,64,127,255,255,252,0,0,0,0,0,3,255,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,7,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,252,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,31,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,240,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,127,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,192,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,255,128,0,0,0,0,0,0,0,0,0,0,0,0,0,3,255,0,0,0,0,0,0,0,0,0,0,0,0
};
BitMap *g_res_BitmapLogo=NULL;

}

TripleBuffer::TripleBuffer()
    : _tripleBufferInitOk(0)
    ,_lastIndexDrawn(0)
    ,_indexToDraw(0)
    ,_d(0)
{

}




IntuitionDrawable::IntuitionDrawable(int flags)
: _width(0),_height(0),_useScale(0)
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

    if(_heightBufferSwitch) ceny+= _heightBufferSwitchApplied;
}
void IntuitionDrawable::waitFrame()
{
    // default behaviour, 50Hz or less.
    WaitTOF();
}

// - - - - - - - - -
Intuition_Screen::Intuition_Screen(const AbstractDisplay::params &params)
    : IntuitionDrawable((params._flags/*&15*/))
    , _pScreen(NULL)
    , _pScreenWindow(NULL)

    , _ScreenModeId(params._forcedModeID)
    , _fullscreenWidth(0)
    , _fullscreenHeight(0)
    , _screenDepthAsked(8) // default.
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
    if(_screenDepthAsked == 32 )_screenDepthAsked =24;


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
    _width = _fullscreenWidth;
    _height = _fullscreenHeight;

    if(_flags & DISPFLAG_USETRIPLEBUFFER)
    {
        if(_pTripleBufferImpl) delete _pTripleBufferImpl;
        _pTripleBufferImpl = new TripleBuffer_CSB(*this); // could fail, in which case back to direct rendering
        _pTripleBufferImpl->init();
     //   printf("TRP IMPL!\n");
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
        return _pTripleBufferImpl->rastPort();

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
void Intuition_Screen::waitFrame()
{
    //
    if(!_pTripleBufferImpl) {
        WaitTOF();
        return;
    }
    _pTripleBufferImpl->waitFrame();
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
        WA_RptQueue,0, // no rawkey repeat messages
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
, _params({0})
//, _pens(0)
, _font(NULL)
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
    //obtainPens();
    if(!_font)
    {
        struct TextAttr fntatrb={
            (STRPTR)"topaz.font",7,0,0
        };
        _font = OpenDiskFont(&fntatrb );
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

bool IntuitionDisplay::switchFullscreen()
{
   close();
   _params._flags ^= DISPFLAG_STARTWITHWINDOW; // switch bit.
   return open(_params);
}
void IntuitionDisplay::close()
{
    if(_font)
    {
      CloseFont(_font);
      _font=NULL;
    }
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
    static int lastprog = -1;
    if(lastprog == per256) return;
    lastprog = per256;

//     printf("drawProgress\n");
    if(!_drawable) return;
    Window *win = _drawable->window();
    if(!win) return;

    RastPort *rp = win->RPort;
    if(!rp) return;

    // - -  draw progress screen
    int w = (int)(win->GZZWidth); // this way manage window border size
    int h = (int)(win->GZZHeight);

    int blackpen = _drawable->blackPen(); // _pens[eBlack];
    int whitepen = _drawable->whitePen(); // _pens[eWhite];
    int greypen= _drawable->greyPen();
//    if(blackpen == -1) blackpen = GetAPen(rp);
//    if(whitepen == -1) whitepen = GetBPen(rp);

//     printf("a:%d b:%d\n",blackpen,whitepen);
// printf("w:%d h:%d\n",w,h);
    SetAPen(rp,blackpen);
    RectFill(rp,0,0,w,h);
    if(per256 >=256) return; // ok, last used to clear.

    int x1 =  (2*w)/256;
    int x2 = (254*w)/256;
    int y1 =  (198*h)/256;
    int y2 =  (208*h)/256;
    int wless = w;
    if(h<wless) wless=h;
    int border =  (1*wless)/256;
    if(border<1) border=1;

    SetAPen(rp,greypen);
    RectFill(rp,x1,y1,x2,y1+border);
    RectFill(rp,x1,y2-border,x2,y2);
    RectFill(rp,x1,y1+border,x1+border,y2-border);
    RectFill(rp,x2-border,y1+border,x2,y2-border);

    // bar
    int xb1 = x1+3*border;
    int xb2 = x2-3*border;
    int yb1 = y1+3*border;
    int yb2 = y2-3*border;
    RectFill(rp,xb1,yb1,xb1 + ((xb2-xb1)*per256)/256,yb2);

    const char *phases[]={
        "Temporal convector flux init.",
        "Initialize input ports.",
        "Rom load...",
        "Memory and CPU inits...",
        "Hi Score load.",
        "Rebuild machine...",
        "Decode graphics...",
        "Load cheats...",
        ""
    };
    if(_font && enm<sizeof(phases)/sizeof(const char *))
    {
        SetFont(rp,_font);

/*
        ebStart=0,
        ebInput,
        ebRomLoad,
        ebMemoryAndCpu,
        ebHighScoreSaveLoad,
        eDriver, //  decryption is done and memory maps altered
        eSoundVideo,
        eCheat,
        eProgressEnd
*/
        int xt= 8;
        const char *p = phases[enm];
        int l = strlen(p);
        SetDrMd(rp,  JAM1  );
        SetAPen(rp,blackpen);
        Move(rp,xt,yb2+6);
        Text(rp,p,l);
        Move(rp,xt+1,yb2+6);
        Text(rp,p,l);

        SetAPen(rp,whitepen);
        Move(rp,xt+1,yb2+7);
        Text(rp,p,l);
    }
    // - - - logo !
    BitMap bm;
    InitBitMap(&bm, 2, 128, 36 );
    bm.Planes[0] = (PLANEPTR)&tlogomamebm[0];
    bm.Planes[1] = (PLANEPTR)&tlogomamebm[0];

    // mask erase shadow
    BltMaskBitMapRastPort
        (&bm,0,0,rp,w-128-16-2  ,y1-37+11,128,36,
         0x0022, /*0x00c0*/
         (PLANEPTR)&tlogomamebm[0]
         );

    // mask write logo
   BltMaskBitMapRastPort
        (&bm,0,0,rp,w-128-16,y1-37+10,128,36,
         0x00e4, /*0x00c0*/   // d8 e4 ?
         (PLANEPTR)&tlogomamebm[0]
         );
    // f0 A -> source
    // cc B -> mask
    // aa C -> dest ! -> nothing change.
/* A source, B mask, C dest
A B C BitPos in minterm
0 0 0 0
0 0 1 1
0 1 0 2
0 1 1 3

1 0 0 4
1 0 1 5
1 1 0 6
1 1 1 7
*/
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
    _drawable->waitFrame();
}


