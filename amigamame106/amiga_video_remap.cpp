/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga_video_remap.h"
#include <stdio.h>

extern "C" {
    // for pixel formats
    #include <cybergraphx/cybergraphics.h>
}

extern "C" {
    // for pixel formats
    #include "mamecore.h"
    #include "video.h"
}

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

    //printf(" * **** FIRST 15b REMAP **** _bytesPerPix:%d\n",_bytesPerPix);
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
