#ifndef AMIGA_VIDEO_REMAP_H
#define AMIGA_VIDEO_REMAP_H

#include <vector>
extern "C"
{
    #include <exec/types.h>
}
#include "amiga106_video.h"

/* if game send color indexed bitmap, manage remap to final bitmap pixel format  */
class Paletted_CGX
{
public:
    Paletted_CGX(const AmigaDisplay::params &params, int screenPixFmt, int bytesPerPix);
    ~Paletted_CGX();
    void updatePaletteRemap(_mame_display *display);
    void updatePaletteRemap15b();
    int needRemap() const { return _needFirstRemap; }

    std::vector<UBYTE> _clut8;
    std::vector<USHORT> _clut16;
    std::vector<ULONG> _clut32;
protected:
    int _needFirstRemap;
    int _pixFmt,_bytesPerPix;
};

#endif
