#ifndef AMIGA_VIDEO_REMAP_H
#define AMIGA_VIDEO_REMAP_H

#include <vector>
extern "C"
{
    #include <exec/types.h>
}
#include "amiga106_video.h"

class Paletted
{
public:
    Paletted();
    virtual ~Paletted(){}
    int needRemap() const { return _needFirstRemap; }
     virtual void updatePaletteRemap(_mame_display *display) = 0;
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
protected:
    struct Screen *_pScreen;
    ULONG _palette[3*32+2]; // LOADRGB32 format.
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
protected:
    struct Screen *_pScreen;
    std::vector<UBYTE> _rgb4cube;
    void initRemapCube();
};

#endif
