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

Paletted_CGX::Paletted_CGX(const AmigaDisplay::params &params, int screenPixFmt, int bytesPerPix)
    : _needFirstRemap(1), _pixFmt(screenPixFmt),_bytesPerPix(bytesPerPix)
{
    switch(bytesPerPix){
        case 1: _clut8.reserve(params._colorsIndexLength); break;
        case 2: _clut16.reserve(params._colorsIndexLength); break;
        case 3: case 4: _clut32.reserve(params._colorsIndexLength); break;
    }
}
Paletted_CGX::~Paletted_CGX(){}
// almost true color, used by neogeo
void Paletted_CGX::updatePaletteRemap15b()
{
    if(!_needFirstRemap) return;  // done once for all.

    printf(" * **** FIRST 15b REMAP **** _bytesPerPix:%d\n",_bytesPerPix);
    const int nbremap = 32768;
    if(_needFirstRemap)
    {
        switch(_bytesPerPix){
            case 1: if(_clut8.size()<nbremap) _clut8.resize(nbremap); break;
            case 2: if(_clut16.size()<nbremap) _clut16.resize(nbremap); break;
            case 3: case 4: if(_clut32.size()<nbremap) _clut32.resize(nbremap);
             break;
        }
    }
    printf(" _clut32 size:%d\n",_clut32.size());
    USHORT *p16a= _clut16.data();

    if(_pixFmt == PIXFMT_RGB15)
    {
        // special case: same format, should be a copy...
        for(UWORD i=0;i<32768;i++) *p16a++ = i;
    }else
    if(_pixFmt == PIXFMT_RGB15PC)
    {
        // special case,almost same format
        for(UWORD i=0;i<32768;i++) *p16a++ = ((i>>8)&0x00ff)|((i<<8)&0xff00);
    }else
    {   // other formats 16b
        if(_bytesPerPix == 2)
        for(UWORD i=0;i<32768;i++)
        {
            UWORD r = i>>10;
            UWORD g= (i>>5)& 0x1f;
            UWORD b= i& 0x1f;

            switch(_pixFmt)
            {
                case PIXFMT_BGR15:
                     *p16a++ =(b<<10)|(g<<5)|(r);
                break;
                case PIXFMT_BGR15PC:
                    {
                        UWORD d=(b<<10)|(g<<5)|(r);
                        *p16a++ =((d>>8)&0x00ff)|((d<<8)&0xff00);
                    }
                break;
                case PIXFMT_RGB16:
                     *p16a++ =(r<<11)|(g<<6)|((g<<1)&0x0020)|(b);
                break;
                case PIXFMT_BGR16:
                     *p16a++ =(b<<11)|(g<<6)|((g<<1)&0x0020)|(r);
                break;
                case PIXFMT_RGB16PC:
                    {
                        UWORD d=(r<<11)|(g<<6)|((g<<1)&0x0020)|(b);
                        *p16a++ =((d>>8)&0x00ff)|((d<<8)&0xff00);
                    }
                break;
                case PIXFMT_BGR16PC:
                    {
                        UWORD d=(b<<11)|(g<<6)|((g<<1)&0x0020)|(r);
                        *p16a++ =((d>>8)&0x00ff)|((d<<8)&0xff00);
                    }
                break;
                case PIXFMT_LUT8: // no sense, should just use RGB32 os palette, do not select Display_CGX_Paletted
                            // TODO for aga lut8
                break;
            }
        } else // end loop
            // 24 and 32 bits
        if(_bytesPerPix == 3 || _bytesPerPix ==4)
        {
            ULONG *p32a= _clut32.data();

            for(ULONG i=0;i<32768;i++)
            {
                // extends component 5 to 8 the right way
                ULONG r = (ULONG)((i>>7)|(i>>12));
                ULONG g = (ULONG)((i>>2)& 0x00f8)|((i>>7)& 0x0007);
                ULONG b = (ULONG)((i<<3)& 0x00f8)|((i>>2)& 0x0007);

                switch(_pixFmt)
                {
                    case PIXFMT_BGR24: //todo but no drawer -> yes now there is.
                        break;
                    // - - -32b cases
                    case PIXFMT_RGB24:
                    case PIXFMT_ARGB32:
                        *p32a++= (r<<16)|(g<<8)|b;
                       break;
                    case PIXFMT_BGRA32:
                        *p32a++= (b<<24)|(g<<16)|(r<<8);
                       break;
                    case PIXFMT_RGBA32:
                         *p32a++= (r<<24)|(g<<16)|(b<<8);
                        break;
                    case PIXFMT_LUT8: // no sense, should just use RGB32 os palette, do not select Display_CGX_Paletted
                                // TODO for aga lut8
                    break;
                } // end switch

            } // end loop all 24 and 32 bits
        }

    } // end else not 16bspecial



    _needFirstRemap =0;
}

void Paletted_CGX::updatePaletteRemap(_mame_display *display)
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
        // on first force all dirty to have all done once.
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

        switch(_pixFmt)
        {
        // - - - - -15b cases
         case PIXFMT_RGB15:
            for(;i<iend;i++) { ULONG c = *gpal++; *p16++ = ((c>>9)&0x7c00)|((c>>6)&0x03e0)|((c>>3)&0x001f); }
            break;
         case PIXFMT_BGR15:
            for(;i<iend;i++) { ULONG c = *gpal++; *p16++ = ((c<<7)&0x7c00)|((c>>6)&0x03e0)|((c>>19)&0x001f); }
            break;
         case PIXFMT_RGB15PC:
            for(;i<iend;i++) { ULONG c = *gpal++; USHORT d = ((c>>9)&0x7c00)|((c>>6)&0x03e0)|((c>>3)&0x001f);
              //  *pb++ = (UBYTE)d; d>>=8;  *pb++ = (UBYTE)d;
                *p16++ = ((d>>8)&0x00ff)|((d<<8)&0xff00);
            }
            break;
         case PIXFMT_BGR15PC:
            for(;i<iend;i++) { ULONG c = *gpal++; USHORT d = ((c<<7)&0x7c00)|((c>>6)&0x03e0)|((c>>19)&0x001f);
               // *pb++ = (UBYTE)d; d>>=8;  *pb++ = (UBYTE)d;
                *p16++ = ((d>>8)&0x00ff)|((d<<8)&0xff00);
            }
            break;
         // - -- - - - 16b cases
         case PIXFMT_RGB16:
            for(;i<iend;i++) { ULONG c = *gpal++; *p16++ = ((c>>8)&0xf800)|((c>>5)&0x07e0)|((c>>3)&0x001f); }
            break;
         case PIXFMT_BGR16:

            for(;i<iend;i++) { ULONG c = *gpal++; USHORT d = ((c>>8)&0xf800)|(((USHORT)c>>5)&0x07e0)|(((USHORT)c>>3)&0x001f);
                *p16++ = d;
            }
            break;
         case PIXFMT_RGB16PC:
            for(;i<iend;i++)
            {
                ULONG c = *gpal++; USHORT d = (((c>>8))&0xf800)|(((USHORT)c>>5)&0x07e0)|(((USHORT)c>>3)&0x001f);
                *p16++ = ((d>>8)&0x00ff)|((d<<8)&0xff00);
            }
            break;
         case PIXFMT_BGR16PC:
            for(;i<iend;i++) { ULONG c = *gpal++; USHORT d =  ((c<<8)&0xf800)|((c>>5)&0x07e0)|((c>>19)&0x001f);

                *p16++ = ((d>>8)&0x00ff)|((d<<8)&0xff00);
            }
            break;

         case PIXFMT_BGR24:
            for(;i<iend;i++) { ULONG c = *gpal++; ULONG d = ((c>>16)&0x000000ff)|((c)&0x0000ff00)|((c<<16)&0x00ff0000);
                *p32++= d;
                }
             break;
         // - - -32b cases
         case PIXFMT_RGB24:
         case PIXFMT_ARGB32:
            // this is the id one, no need for table, direct palette use.
            for(;i<iend;i++) { *p32++= (*gpal++);}
            break;
         case PIXFMT_BGRA32:
            for(;i<iend;i++) { ULONG c = *gpal++; ULONG d = ((c>>8)&0x0000ff00)|((c<<8)&0x00ff0000)|((c<<24)&0xff000000);            *p32++= d;
            }
            break;
         case PIXFMT_RGBA32:
            for(;i<iend;i++) { *p32++= (*gpal++)<<8;}
            break;
         case PIXFMT_LUT8: // no sense, should just use RGB32 os palette, do not select Display_CGX_Paletted
            // let's consider a 2/3/3 RGB
            // no: let's consider intuition pens.
    //        for(;i<nbc;i++) { ULONG c = *gpal++; ULONG d = ((c>>16)&0x0000ff00)|((c<<8)&0x00ff0000)|((c<<24)&0xff000000);
    //            *p32++= d;
    //        }
        default:
            break;
        }
    }
    _needFirstRemap = 0;
}

IntuitionDrawable::IntuitionDrawable(int flags)
: _PixelFmt(0),_PixelBytes(0),_width(0),_height(0),_dx(0),_dy(0),_useScale(0)
, _flags(flags)
{
}
IntuitionDrawable::~IntuitionDrawable()
{
}

// would draw LUT screens or truecolor, ...
// _width,_height must be set before call.
void IntuitionDrawable::drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap)
{
    if(!CyberGfxBase) return;
    RastPort *pRPort = rastPort();

    BitMap *pBitmap = bitmap();
    if(!pRPort || !pBitmap) return;
    mame_bitmap *bitmap = display->game_bitmap;

    directDrawScreen ddscreen;

    int depth,pixfmt,pixbytes,bpr;

    UWORD *pp = (UWORD *)bitmap->base;
  //  printf("pixels values:%04x %04x %04x\n",(int)*pp,(int)pp[500],(int)pp[64*512+32]);
    ULONG bmwidth,bmheight;

 printf("bitmap->rowbytes:%d\n",(int)bitmap->rowbytes);
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
         {
            if((_width>sourcewidth || _height>sourceheight) && _useScale)
            {
                directDrawScaleClutT<UWORD,UWORD>(&ddscreen,&ddsource,0,0,
                    _width,_height,
                pRemap->_clut16.data(),_flags);
            } else
            {
                directDrawClut16(&ddscreen,&ddsource,cenx+_dx,ceny+_dy,pRemap->_clut16.data(),_flags);
            }
         }
            break;
         case PIXFMT_RGB24:case PIXFMT_BGR24:
        {
            // theorically, ... untested because rare cgx implementations
            // -> now tested by tricking
            directDrawScaleClutT<type24,ULONG>(&ddscreen,&ddsource,0,0,
                _width,_height,
            pRemap->_clut32.data(),_flags);
        }
             break;
         case PIXFMT_ARGB32:case PIXFMT_BGRA32:case PIXFMT_RGBA32:
         if(pRemap->_clut32.size()>0)
         {
            if((_width>sourcewidth || _height>sourceheight) && _useScale)
            {
                directDrawScaleClutT<ULONG,ULONG>(&ddscreen,&ddsource,0,0,
                    _width,_height,
                pRemap->_clut32.data(),_flags);
            } else
            {
                directDrawClut32(&ddscreen,&ddsource,cenx+_dx,ceny+_dy,pRemap->_clut32.data(),_flags);
            }
         }
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
    : IntuitionDrawable(params._flags)
    , _pScreen(NULL)
    , _pScreenWindow(NULL)

    , _ScreenModeId(params._forcedModeID)
    , _fullscreenWidth(0)
    , _fullscreenHeight(0)
    , _pMouseRaster(NULL)
{
    printf(" **** Intuition_Screen SET VIDEO ATTRIBUTES:%08x ****\n",_flags);
    // may get mode from Cybergraphics...
    if(CyberGfxBase)
    {
        int width = params._width;
        int height = params._height;
        if(params._flags & ORIENTATION_SWAP_XY) doSwap(width,height);


        int screenDepth = (params._colorsIndexLength<=256)?8:16; // more would be Display_CGX_TrueColor.

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
    _width = _fullscreenWidth;
    _height = _fullscreenHeight;
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
  if(params._flags & ORIENTATION_SWAP_XY) doSwap(_machineWidth,_machineHeight);
}
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
        WA_MaxHeight, _machineWidth*_maxzoomfactor,
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

void Intuition_Window::drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap)
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
void Display_CGX::open(const AmigaDisplay::params &pparams)
{
   if(!CyberGfxBase) return;
   if(_drawable) return;
   _params=pparams;

    if(_params._flags & DISPFLAG_STARTWITHWINDOW)
    {
        _drawable = new /*Intuition_Window*/Intuition_ScaleWindow(pparams);
    } else
    {

        _drawable = new Intuition_Screen(pparams);
    }

    if(!_drawable) return;
    // must open before computing remap to get correct pixel format.s
    _drawable->open();

    if((pparams._video_attributes & VIDEO_RGB_DIRECT)==0 &&
        pparams._colorsIndexLength>0)
    {
        _remap = new Paletted_CGX(pparams,_drawable->pixelFmt(),_drawable->pixelBytes());
    } else
    if((pparams._video_attributes & VIDEO_RGB_DIRECT)!=0 && pparams._driverDepth == 15)
    {
        _remap = new Paletted_CGX(pparams,_drawable->pixelFmt(),_drawable->pixelBytes());
        _remap->updatePaletteRemap15b(); // once for all.
    }


}
int Display_CGX::switchFullscreen()
{
   if(!CyberGfxBase) return -1;
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

    if(CyberGfxBase) _drawable->drawRastPort_CGX(display,_remap);
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
