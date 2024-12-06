/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga_video_remap.h"
#include <stdio.h>

#include <proto/graphics.h>

extern "C" {
    // for pixel formats
    #include <cybergraphx/cybergraphics.h>
    #include <intuition/screens.h>
}

extern "C" {
    // for pixel formats
    #include "mamecore.h"
    #include "video.h"
}

Paletted::Paletted() : _needFirstRemap(1)
{

}

Paletted_CGX::Paletted_CGX(int colorsIndexLength, int screenPixFmt, int bytesPerPix)
    : Paletted(), _pixFmt(screenPixFmt),_bytesPerPix(bytesPerPix)
{
    switch(bytesPerPix){
        case 1: _clut8.reserve(colorsIndexLength); break;
        case 2: _clut16.reserve(colorsIndexLength); break;
        case 3: case 4: _clut32.reserve(colorsIndexLength); break;
    }
}
Paletted_CGX::~Paletted_CGX(){}
// almost true color, used by neogeo
void Paletted_CGX::updatePaletteRemap15b()
{
    if(!_needFirstRemap) return;  // done once for all.

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
    _needFirstRemap =0;
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
// - - - - -

Paletted_Screen8::Paletted_Screen8(struct Screen *pScreen)
    : Paletted(), _pScreen(pScreen) {
}
void Paletted_Screen8::updatePaletteRemap(_mame_display *display)
{
    if(!_pScreen) return;
    //    printf("Paletted_Screen8::updatePaletteRemap\n");
    const rgb_t *gpal1 = display->game_palette;
    USHORT nbc = (USHORT)display->game_palette_entries;
    UINT32 *pdirtrybf =	display->game_palette_dirty;
    bool ignominiousColorTrick = false;
    if(nbc==258)
    {
        // ignomous trick to add 2 colors for interface :( (slap fight, ...)
        ignominiousColorTrick = true;
        rgb_t *g  =const_cast<rgb_t *>(gpal1);
        g[1] = 0x00ffffff;
        nbc = 256;
    }

    if(_needFirstRemap)
    {
        //printf("Paletted_Screen8::updatePaletteRemap _needFirstRemap\n");
        // on first force all dirty to have all done once.
        int nbdirstybf = (nbc+31)>>5;
        for(int i=0;i<nbdirstybf;i++) pdirtrybf[i]=~0;
    }
     _needFirstRemap = 0;

     int nbc1 = nbc;
     if(nbc1>256) nbc1=256;
     // the 256 first colors are used as intuition color palette.
     int j=0;
     for(;j<nbc1;j+=32)
     {
        UINT32 dirtybf = *pdirtrybf++;
        if(!dirtybf) continue; // superfast escape.

        USHORT iend = 32;
        if(iend>(nbc1-j)) iend=(nbc1-j);
        const rgb_t *gpal = gpal1+j;
        ULONG *pc = &_palette[0];
        *pc++ = ((ULONG)iend)<<16 | j;

        for(USHORT i=0;i<iend;i++) {
            ULONG c = (*gpal++);
            *pc++= (c<<8) & 0xff000000;
            *pc++= (c<<16) & 0xff000000;
            *pc++= (c<<24) & 0xff000000;
        }
        *pc = 0; // term.
            //    printf("LoadRGB32\n");
        LoadRGB32(&(_pScreen->ViewPort),(ULONG *) &_palette[0]);
     }
     // todo: if more color, remap to first
//     for(;j<nbc;j+=32)
//     {
//         UINT32 dirtybf = *pdirtrybf++;
//         if(!dirtybf) continue; // superfast escape.
//         USHORT iend = 32;
//         if(iend>(nbc-j)) iend=(nbc1-j);
//     }
}
// - - - - - - --
Paletted_Pens8::Paletted_Pens8(struct Screen *pScreen)
    : Paletted(), _pScreen(pScreen)
{
//    printf("Paletted_Pens8:%08x\n",pScreen);
}
Paletted_Pens8::~Paletted_Pens8()
{
//    printf("~Paletted_Pens8\n");
//    if(_pScreen)
//    {
//        struct ColorMap *pColorMap = _pScreen->ViewPort.ColorMap;
//        if(pColorMap)
//        // basically clean like that.
//        for(ULONG i=0;i<256 ;i++ )
//        {
//            ReleasePen(pColorMap,i);
//        }
//    }
}
void Paletted_Pens8::updatePaletteRemap(_mame_display *display)
{
//    printf("updatePaletteRemap:\n");
    if(!_pScreen) return;
    struct	ColorMap *pColorMap = _pScreen->ViewPort.ColorMap;
    if(!pColorMap) return;
//    printf("updatePaletteRemap: go\n");
    const rgb_t *gpal1 = display->game_palette;
    USHORT nbc = (USHORT)display->game_palette_entries;
    UINT32 *pdirtrybf =	display->game_palette_dirty;

    if(_needFirstRemap)
    {
        // on first force all dirty to have all done once.
        int nbdirstybf = (nbc+31)>>5;
        for(int i=0;i<nbdirstybf;i++) pdirtrybf[i]=~0;
        // also that,
        initRemapCube();
    }
    if(_clut8.size()<nbc) _clut8.resize(nbc,0);
    UBYTE *pclut = _clut8.data();

    _needFirstRemap = 0;
    for(int j=0;j<nbc;j+=32)
    {
        UINT32 dirtybf = *pdirtrybf++;
        if(!dirtybf) continue; // superfast escape.

        USHORT iend = j+32;
        if(iend>nbc) iend=nbc;
        const rgb_t *gpal = gpal1+j;
        for(USHORT i=j;i<iend;i++) {

            if(dirtybf&1)
            {
                ULONG c = *gpal;
                UWORD rgb4 = ((c>>(20-8)) & 0x0f00) |
                              ((c>>(12-4)) & 0x00f0) |
                              ((c>>4) & 0x000f) ;
                pclut[i] =_rgb4cube[rgb4];
            } // end if dirty
            dirtybf>>=1;
            gpal++;
        } // end loop per 32
     }

}
void Paletted_Pens8::initRemapCube()
{
    // should lock -> no, the window locks the WB.
    struct	ColorMap *pColorMap = _pScreen->ViewPort.ColorMap;
    if(!pColorMap) return;

    // 16*16*16
    const UBYTE excluded = 255;
    _rgb4cube.resize(4096,excluded); // don't allow 255

    for(LONG i=0;i<pColorMap->Count ; i++)
    {
       ULONG rgb4 = GetRGB4(pColorMap,i) & 0x0fff;
       _rgb4cube[rgb4] = (UBYTE)i;
    }
    for(UWORD rgbi=0;rgbi<4096;rgbi++)
    {
        //UWORD rgbi = rgi | b;
        if(_rgb4cube[rgbi] ==excluded)
        {
            UBYTE isbest=0;
            ULONG isbesterr=0x0fffffff;
            BYTE r = rgbi>>8;
            BYTE g = (rgbi>>4) & 0x0f;
            BYTE b = rgbi & 0x0f;

            for(UWORD is=0;is<4096;is++)
            {
                if(_rgb4cube[is] == excluded) continue;
                BYTE rs = rgbi>>8;
                BYTE gs = (rgbi>>4) & 0x0f;
                BYTE bs = rgbi & 0x0f;
                ULONG err = (rs-r)*(rs-r) + (gs-g)*(gs-g) + (bs-b)*(bs-b);
                if(err<isbesterr) {
                    isbest = _rgb4cube[is];
                    isbesterr = err;
                    if(isbesterr ==0) break;
                }
            }
            _rgb4cube[rgbi] = isbest;
        } // end if excluded
    }
}

