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

class Paletted_Screen8 : public Paletted
{
 public:
    Paletted_Screen8(struct Screen *pScreen);
    void updatePaletteRemap(_mame_display *display) override;
protected:
    struct Screen *_pScreen;
    ULONG _palette[3*32+2]; // LOADRGB32 format.
};

class Paletted_Pens8 : public Paletted
{
 public:
    Paletted_Pens8(struct Screen *pScreen);
    ~Paletted_Pens8();
    void updatePaletteRemap(_mame_display *display) override;
protected:
    struct Screen *_pScreen;
};

#endif
