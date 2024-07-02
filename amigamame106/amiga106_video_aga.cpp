/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga106_video_aga.h"

#include <proto/exec.h>
#include <proto/graphics.h>
//#include <proto/layers.h>
#include <proto/intuition.h>

// Amiga
extern "C" {
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <graphics/modeid.h>
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

struct directDrawScreen {
    void *_base;
    ULONG _bpr;
    WORD _clipX1,_clipY1,_clipX2,_clipY2;
};
struct directDrawSource {
    void *_base;
    ULONG _bpr;
    WORD _x1,_y1,_x2,_y2; // to be drawn.
};
extern "C" {
    //extern int asmval,asmval2;
    // for any of the 8x 16b target mode.
    void directDrawClut8(register directDrawScreen *screen __asm("a0"),
                    register directDrawSource *source __asm("a1"),
                    register LONG x1 __asm("d0"),
                    register LONG y1 __asm("d1"),
                    register USHORT *lut __asm("a2") // actually UWORD* or anywhat.
                );
}


Paletted_AGA::Paletted_AGA(const _osd_create_params *params)
    : _needFirstRemap(1), _pixFmt(screenPixFmt),_bytesPerPix(bytesPerPix)
{
    if(params->colors>256) return;
    _clut8.reserve(params->colors);
}
Paletted_AGA::~Paletted_AGA(){}

void Paletted_AGA::updatePaletteRemap(_mame_display *display)
{
    const rgb_t *gpal1 = display->game_palette;
    const int nbc = display->game_palette_entries;
    UINT32 *pdirtrybf =	display->game_palette_dirty;

    if(_needFirstRemap)
    {
        switch(_bytesPerPix){
            case 1: if(_clut8.size()<nbc) _clut8.resize(nbc); break;
            case 2: if(_clut16.size()<nbc) _clut16.resize(nbc); break;
            case 3: case 4: if(_clut32.size()<nbc) _clut32.resize(nbc);
             break;
        }
        int nbdirstybf = (nbc+31)>>5;
        for(int i=0;i<nbdirstybf;i++) pdirtrybf[i]=~0;
    }

    USHORT *p16a= _clut16.data();
    ULONG *p32a= _clut32.data();
  //  UBYTE *pb=(UBYTE *)_clut16.data();

    for(int j=0;j<nbc;j+=32)
    {
        UINT32 dirtybf = *pdirtrybf++;
        if(!dirtybf) continue; // superfast escape.

        int i=0;
        int iend = 32;
        if(iend>(nbc-j)) iend=(nbc-j);

        const rgb_t *gpal = gpal1+j;
        USHORT *p16= p16a+j;
        ULONG *p32= p32a+j;

//         case PIXFMT_RGB15:
//            for(;i<iend;i++) { ULONG c = *gpal++; *p16++ = ((c>>9)&0x7c00)|((c>>6)&0x03e0)|((c>>3)&0x001f); }
//            break;

    }
    _needFirstRemap = 0;
}



IntuitionDrawable::IntuitionDrawable()

: _PixelFmt(0),_PixelBytes(0),_width(0),_height(0),_dx(0),_dy(0)
{
}
IntuitionDrawable::~IntuitionDrawable()
{
}

// would draw LUT screens or truecolor, ...

void IntuitionDrawable::drawRastPort_AGA(_mame_display *display,Paletted_AGA *pRemap)
{
    if(!CyberGfxBase) return;
    RastPort *pRPort = rastPort();

    BitMap *pBitmap = bitmap();
    if(!pRPort || !pBitmap) return;
    mame_bitmap *bitmap = display->game_bitmap;

    directDrawScreen ddscreen;

    int depth,pixfmt,pixbytes,bpr;


    APTR hdl = LockBitMapTags(pBitmap,
                              LBMI_WIDTH,(ULONG)&_width,
                              LBMI_HEIGHT,(ULONG)&_height,
                              LBMI_DEPTH,(ULONG)&depth,
                              LBMI_PIXFMT,(ULONG)&pixfmt,
                              LBMI_BYTESPERPIX,(ULONG)&pixfmt,
                              LBMI_BYTESPERROW,(ULONG)&ddscreen._bpr,
                              LBMI_BASEADDRESS,(ULONG)&ddscreen._base,
                              TAG_DONE);
    if(!hdl) return;

    ddscreen._clipX1 = 0;//10;
    ddscreen._clipY1 = 0; //10;

    ddscreen._clipX2 = (WORD)_width; //-10;
    ddscreen._clipY2 = (WORD)_height; //-10;

    // +1 because goes 0,319
    int sourcewidth = (display->game_visible_area.max_x - display->game_visible_area.min_x)+1;
    int sourceheight =( display->game_visible_area.max_y - display->game_visible_area.min_y)+1;


    int cenx = _width-sourcewidth;
    int ceny = _height-sourceheight;
    if(cenx<0) cenx = 0;
    if(ceny<0) ceny = 0;
    cenx>>=1;
    ceny>>=1;

    directDrawSource ddsource={bitmap->base,bitmap->rowbytes,
        display->game_visible_area.min_x,display->game_visible_area.min_y,
        display->game_visible_area.max_x+1,display->game_visible_area.max_y+1
    };
    if(pRemap)
    {
        switch(_PixelFmt) {
         case PIXFMT_RGB15:case PIXFMT_BGR15:case PIXFMT_RGB15PC:case PIXFMT_BGR15PC:
         case PIXFMT_RGB16:case PIXFMT_BGR16:case PIXFMT_RGB16PC:case PIXFMT_BGR16PC:
         if(pRemap->_clut16.size()>0)
            directDrawClut16(&ddscreen,&ddsource,cenx+_dx,ceny+_dy,pRemap->_clut16.data());
            break;
         case PIXFMT_RGB24:case PIXFMT_BGR24:
            //TODO
             break;
         case PIXFMT_ARGB32:case PIXFMT_BGRA32:case PIXFMT_RGBA32:
         if(pRemap->_clut32.size()>0)
            directDrawClut32(&ddscreen,&ddsource,cenx+_dx,ceny+_dy,pRemap->_clut32.data());
            break;
        default:
            //LUT8, aga:todo
            break;
        }
    } else
    {
        //Truecolor (todo)
    }

    UnLockBitMap(hdl);
//    printf("isit?\n");
//      printf("ddsource x1:%d  y1:%d \n",ddsource._x1,ddsource._y1);
//    printf("or?\n");
}
// =========================== new impl


Intuition_Screen::Intuition_Screen(const _osd_create_params *params,ULONG forcedModeId) : IntuitionDrawable()
    , _pScreen(NULL)
    , _pScreenWindow(NULL)

    , _ScreenModeId(forcedModeId)
    , _fullscreenWidth(0)
    , _fullscreenHeight(0)
    , _pMouseRaster(NULL)
{
    // may get mode from Cybergraphics...
    if(CyberGfxBase)
    {
        int width = params->width;
        int height = params->height;

        int screenDepth = (params->colors<=256)?8:16; // more would be Display_AGA_TrueColor.

        if(_ScreenModeId == INVALID_ID)
        {
             struct TagItem cgxtags[]={
                    CYBRBIDTG_NominalWidth,width,
                    CYBRBIDTG_NominalHeight,height,
                    CYBRBIDTG_Depth,screenDepth,
                    TAG_DONE,0 };
                 printf("bef BestCModeIDTagList()\n");

            _ScreenModeId = BestCModeIDTagList(cgxtags);
               printf("aft BestCModeIDTagList()\n");
                   fflush(stdout);
        }
        if(_ScreenModeId == INVALID_ID)
        {
            logerror("Can't find cyber screen mode for w%d h%d d%d ",width,height,screenDepth);
            return;
        }
        _fullscreenWidth = GetCyberIDAttr( CYBRIDATTR_WIDTH, _ScreenModeId );
        _fullscreenHeight = GetCyberIDAttr( CYBRIDATTR_HEIGHT, _ScreenModeId );
        _PixelFmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT, _ScreenModeId );
        _PixelBytes = GetCyberIDAttr( CYBRIDATTR_BPPIX, _ScreenModeId );

    } // end if CGX available

}
Intuition_Screen::~Intuition_Screen()
{
    close();
}
void Intuition_Screen::open()
{

    if(_pScreenWindow) return; // already open.
    if(_ScreenModeId == INVALID_ID) return; // set by inherited class.

    // note: all this is regular OS intuition, no CGX
	struct ColorSpec colspec[2]={0,0,0,0,-1,0,0,0};
 	_pScreen = OpenScreenTags( NULL,
			SA_DisplayID,_ScreenModeId,
                        SA_Width, _fullscreenWidth,
                        SA_Height,_fullscreenHeight,
//                        SA_Behind,TRUE,    /* Open behind */
                        SA_Quiet,TRUE,     /* quiet */
			SA_Type,CUSTOMSCREEN,
			SA_Colors,(ULONG)&colspec[0],
                        0 );

	if( _pScreen == NULL ) return;

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
        return ;
	}
	// ------- set invisible mouse pointer:
	_pMouseRaster =  AllocRaster(8 ,8) ;
	if(_pMouseRaster)
	{
        SetPointer( _pScreenWindow ,(UWORD *) _pMouseRaster, 0,1,0,0);
    }

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

Intuition_Window::Intuition_Window(const _osd_create_params *params) : IntuitionDrawable()
    , _pWbWindow(NULL)
    , _sWbWinSBitmap(NULL)
    , _machineWidth(params->width),_machineHeight(params->height)
{}
Intuition_Window::~Intuition_Window()
{
    close();
}
void Intuition_Window::open()
{
    if(_pWbWindow) return;

    Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))) return;

    int xcen = (pWbScreen->Width - _machineWidth);
    int ycen = (pWbScreen->Height - _machineHeight);
    if(xcen<0) xcen=0;
    xcen>>=1;
    if(ycen<0) ycen=0;
    ycen>>=1;
    printf("openWindow:_machineWidth:%d _machineHeight:%d xcen:%d ycen:%d \n",_machineWidth,_machineHeight,xcen,ycen);

// struct BitMap * __stdargs AllocBitMap( ULONG sizex, ULONG sizey, ULONG depth, ULONG flags, CONST struct BitMap *friend_bitmap );

    _sWbWinSBitmap = AllocBitMap(_machineWidth,_machineHeight,
            pWbScreen->RastPort.BitMap->Depth,BMF_CLEAR|BMF_DISPLAYABLE,pWbScreen->RastPort.BitMap);
    if(_sWbWinSBitmap) {

        _pWbWindow = (Window *)OpenWindowTags(NULL,
        WA_Left,xcen,
        WA_Top,ycen,
     //   WA_Width, _machineWidth,
     //   WA_Height, _machineHeight,
        WA_InnerWidth, _machineWidth,
        WA_InnerHeight, _machineHeight,
    //    WA_MaxWidth,  WIDTH_SUPER,
    //    WA_MaxHeight, HEIGHT_SUPER,
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY /*|
            IDCMP_NEWSIZE*/ /*| IDCMP_INTUITICKS*/ | IDCMP_CLOSEWINDOW,

        WA_Flags, /*WFLG_SIZEGADGET*/ /*| WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM |

            */ WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE /*|
            WFLG_SUPER_BITMAP*/
             | WFLG_GIMMEZEROZERO
           // | WFLG_NOCAREREFRESH
             | WFLG_SMART_REFRESH
            //| WFLG_SIMPLE_REFRESH
            ,
     //   WA_Gadgets, &(SideGad),
        WA_Title,(ULONG) "Mame 0.106 Krb ", /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,

   //     WA_SuperBitMap, (ULONG)_sWbWinSBitmap,
        TAG_DONE
        );
    } // end if sbm ok
    UnlockPubScreen(NULL,pWbScreen);

    if( _pWbWindow == NULL ) return;


    //_dx = _pWbWindow->BorderLeft;
    //_dy = _pWbWindow->BorderTop;
    // need pixel format at this level

    if(CyberGfxBase)
    {

        if(_sWbWinSBitmap && GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_ISCYBERGFX))
        {
            _PixelFmt = GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_PIXFMT);
            _PixelBytes = GetCyberMapAttr(_sWbWinSBitmap,CYBRMATTR_BPPIX);
        }
    }

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

void Intuition_Window::drawRastPort_AGA(_mame_display *display,Paletted_AGA *pRemap)
{
    if(_pWbWindow && _sWbWinSBitmap)
    {
        IntuitionDrawable::drawRastPort_AGA(display,pRemap);

        BltBitMapRastPort( _sWbWinSBitmap,//CONST struct BitMap *srcBitMap,
                           0,0, //LONG xSrc, LONG ySrc,
                           _pWbWindow->RPort,//struct RastPort *destRP,
                           0,0,//LONG xDest, LONG yDest,
                           _width, _height,
                           0x00c0//ULONG minterm  -> copy minterm.
                           );
    }
}


Display_AGA::Display_AGA()
: MameDisplay()
, _drawable(NULL)
, _remap(NULL)
{

}
Display_AGA::~Display_AGA()
{
    if(_drawable) delete _drawable;
    if(_remap) delete _remap;
}
void Display_AGA::open(const _osd_create_params *params,int window, ULONG forcedModeID)
{
   if(!CyberGfxBase) return;
   if(_drawable) return;

    if(window)
    {
        _drawable = new Intuition_Window(params);
    } else
    {

        _drawable = new Intuition_Screen(params,forcedModeID);
    }

    if(!_drawable) return;
    _drawable->open();

    if((params->video_attributes & VIDEO_RGB_DIRECT)==0 &&
        params->colors>0)
    {
        _remap = new Paletted_AGA(params,_drawable->pixelFmt(),_drawable->pixelBytes());
    }

}
void Display_AGA::close()
{
    if(!_drawable) return;
    _drawable->close();
}
int Display_AGA::good()
{
    if(_drawable && _drawable->userPort()) return 1;
    return 0;
}
void Display_AGA::draw(_mame_display *display)
{
    if(!_drawable) return;

    // - - update palette if exist and is needed.
    if(_remap && ((display->changed_flags & GAME_PALETTE_CHANGED) !=0 || _remap->needRemap()))
    {

        _remap->updatePaletteRemap(display);
    }


    if(CyberGfxBase) _drawable->drawRastPort_AGA(display,_remap);
}
MsgPort *Display_AGA::userPort()
{
    if(!_drawable) return NULL;
    return _drawable->userPort();
}












