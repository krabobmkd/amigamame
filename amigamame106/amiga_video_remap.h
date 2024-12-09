#ifndef AMIGA_VIDEO_REMAP_H
#define AMIGA_VIDEO_REMAP_H

#include <vector>
extern "C"
{
    #include <exec/types.h>
}
#include "amiga106_video.h"
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
/* When target is a 8Bit screen and game 8bit, use LoadRGB32() and no clut.  */
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



/* Whe target screen pixels is 8bit but palette is imposed by OS.
* then Use CLUT
*/
// beta experimental
class Paletted_Pens8 : public Paletted
{
 public:
    Paletted_Pens8(struct Screen *pScreen);
    ~Paletted_Pens8();
    void updatePaletteRemap(_mame_display *display) override;
    void directDraw(directDrawParams *p) override;
protected:
    struct Screen *_pScreen;
    std::vector<UBYTE> _rgb4cube;
    virtual void initRemapCube();
};
class Paletted_Pens8_15b : public Paletted_Pens8
{
public:
    Paletted_Pens8_15b(struct Screen *pScreen);
    ~Paletted_Pens8_15b();
    void updatePaletteRemap(_mame_display *display) override;
    void directDraw(directDrawParams *p) override;
protected:
    struct Screen *_pScreen;
    std::vector<UBYTE> _rgb4cube;
    virtual void initRemapCube();
};

class Palette8 {
public:
    void loadIlbmClut(const UBYTE *pbin,ULONG bsize);
protected:
    std::vector<UBYTE> _p;
};

/* When target is a 8Bit screen and game 16bit, use fixed palette and pens remap */
class Paletted_Screen8ForcePalette : public Paletted_Pens8
{
public:
    Paletted_Screen8ForcePalette(struct Screen *pScreen);
   // int loadIlbmClut(const UBYTE *pbin,ULONG bsize);
protected:
    void initRemapCube() override;
    void initFixedPalette(const UBYTE *prgb,ULONG nbc);

};
class Paletted_Screen8ForcePalette_15b : public Paletted_Screen8ForcePalette
{
public:
    Paletted_Screen8ForcePalette_15b(struct Screen *pScreen);
protected:

};
class Paletted_Screen8ForcePalette_32b : public Paletted_Screen8ForcePalette_15b
{
public:
    Paletted_Screen8ForcePalette_32b(struct Screen *pScreen);
    void directDraw(directDrawParams *p) override;
protected:

};
#endif
