/******************************************************************************
 * amiga_video_remap.h
 *
 *       ╔════════════════════════════════╗
 *       ║  PALETTE REMAPPING ENGINE      ║
 *       ║  ┌──────────────────┐          ║
 *       ║  │ 256 →  8bit      │          ║
 *       ║  │ 32K → 16bit      │          ║
 *       ║  │ 16M → 32bit      │          ║
 *       ║  └──────────────────┘          ║
 *       ║  Color space transformation!   ║
 *       ╚════════════════════════════════╝
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 *****************************************************************************/

#ifndef AMIGA_VIDEO_REMAP_H
#define AMIGA_VIDEO_REMAP_H

#include <vector>
extern "C"
{
    #include <exec/types.h>
}
#include "amiga_video.h"
#include "amiga_video_tracers.h"
class Paletted
{
public:
    Paletted();
    virtual ~Paletted(){}
    int needRemap() const { return _needFirstRemap; }
     virtual void updatePaletteRemap(_mame_display *display) = 0;
    // actual tracer to use with this kind of remap :)
     virtual void directDraw(directDrawParams *p) {};
     // color lookup table to final screen pixel format. Use either table.
    std::vector<UBYTE> _clut8;
    std::vector<USHORT> _clut16;
    std::vector<ULONG> _clut32;
protected:
    int _needFirstRemap;
};

/* if game send color indexed bitmap, manage remap to final bitmap pixel format  */
class Paletted_CGX : public Paletted
{
public:
    Paletted_CGX(int colorsIndexLength, int screenPixFmt, int bytesPerPix);
    ~Paletted_CGX();
    void updatePaletteRemap(_mame_display *display) override;
    void updatePaletteRemap15b();
protected:

    int _pixFmt,_bytesPerPix;
};
/* When target is an 8-bit screen and game is 8-bit, use LoadRGB32() and no CLUT.  */
class Paletted_Screen8 : public Paletted
{
 public:
    Paletted_Screen8(struct Screen *pScreen);
    void updatePaletteRemap(_mame_display *display) override;
    void directDraw(directDrawParams *p) override;
protected:
    struct Screen *_pScreen;
    ULONG _palette[3*32+2]; // LOADRGB32 format, used to load colors per 32.
};



/* When target screen pixels are 8-bit but palette is imposed by OS,
* then use CLUT.
*/
class Paletted_Pens8 : public Paletted
{
 public:
    Paletted_Pens8(struct Screen *pScreen);
    ~Paletted_Pens8();
    void updatePaletteRemap(_mame_display *display) override;
    void directDraw(directDrawParams *p) override;
protected:
    struct Screen *_pScreen;
    int _screenNbc;
    std::vector<UBYTE> _rgbcube; // is either 4x4x4=4096 or 5x5x5=32768
    bool _use15BitPrecision;
    virtual void initRemapCube();
};
// RGB15 to 8bit external palette (WB)
class Paletted_Pens8_src15b : public Paletted_Pens8
{
public:
    Paletted_Pens8_src15b(struct Screen *pScreen);
    ~Paletted_Pens8_src15b();
    void updatePaletteRemap(_mame_display *display) override;
protected:
};
// RGB32 to 8bit external palette  (WB)
class Paletted_Pens8_src32b : public Paletted_Pens8_src15b
{
public:
    Paletted_Pens8_src32b(struct Screen *pScreen);
    ~Paletted_Pens8_src32b();
    void directDraw(directDrawParams *p) override;
protected:
};
class Palette8 {
public:
    void loadIlbmClut(const UBYTE *pbin,ULONG bsize);
protected:
    std::vector<UBYTE> _p;
};

// =========================================================================
/* When target is an 8-bit screen and game is 16-bit, use fixed palette and pens remap */
class Paletted_Screen8ForcePalette : public Paletted_Pens8
{
public:
    Paletted_Screen8ForcePalette(struct Screen *pScreen);
   // int loadIlbmClut(const UBYTE *pbin,ULONG bsize);
protected:
    void initRemapCube() override;
    // do LoadRgb32()
    void initFixedPalette(const UBYTE *prgb,ULONG nbc);

};
// RGB15 to 8Bit, our palette
class Paletted_Screen8ForcePalette_15b : public Paletted_Screen8ForcePalette
{
public:
    Paletted_Screen8ForcePalette_15b(struct Screen *pScreen);
    void initRemapCube() override;
    void directDraw(directDrawParams *p) override;
};
// RGB32 to 8Bit,  our palette
class Paletted_Screen8ForcePalette_32b : public Paletted_Screen8ForcePalette_15b
{
public:
    Paletted_Screen8ForcePalette_32b(struct Screen *pScreen);
    void directDraw(directDrawParams *p) override;
protected:

};
#endif

/* Palettes remapped! Colors transformed flawlessly!
 *       /\___/\
 *      (  o.o  )  <-- Raccoon remaps all the colors
 *       > ^ ^ <
 */
