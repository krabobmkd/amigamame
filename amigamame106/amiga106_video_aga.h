#ifndef AMIGA_VIDEO_AGA_H
#define AMIGA_VIDEO_AGA_H

extern "C"
{
    #include <exec/ports.h>
}
#include "amiga106_video.h"

struct _osd_create_params;
struct _mame_display;
struct Window;
struct Screen;
struct RastPort;
struct BitMap;

/* if game send color indexed bitmap, manage remap to final bitmap pixel format  */
class Paletted_AGA
{
public:
    Paletted_AGA(const _osd_create_params *params);
    ~Paletted_AGA();
    void updatePaletteRemap(_mame_display *display);
    int needRemap() const { return _needFirstRemap; }

    std::vector<UBYTE> _clut8;
//    std::vector<USHORT> _clut16;
//    std::vector<ULONG> _clut32;
protected:
    int _needFirstRemap;
//    int _pixFmt,_bytesPerPix;
};

/** virtual, at this level manage screen and window opening, not rendering */

class IntuitionDrawableAGA {
public:
    IntuitionDrawableAGA();
    virtual ~IntuitionDrawableAGA();
    virtual void open() = 0;
    virtual void close()= 0;
    virtual MsgPort *userPort() = 0;
//    inline ULONG pixelFmt() const { return _PixelFmt; }
//    inline ULONG pixelBytes() const { return _PixelBytes; }

    virtual void drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap);
protected:
//    ULONG _PixelFmt,_PixelBytes;
    int _width,_height;
    int _dx,_dy; // draw delta (for windows borders)
    virtual RastPort *rastPort() = 0;
    virtual BitMap *bitmap() = 0;
};

class Intuition_ScreenAGA : public IntuitionDrawableAGA
{
public:
    Intuition_ScreenAGA(const _osd_create_params *params,ULONG screnModeId=~0);
    ~Intuition_ScreenAGA();
    void open() override;
    void close() override;
    MsgPort *userPort() override;
protected:
    Screen *_pScreen;
    Window *_pScreenWindow;
    ULONG   _ScreenModeId;
    int _fullscreenWidth; // guessed from modeid.
    int _fullscreenHeight;
    void *_pMouseRaster;
    RastPort *rastPort() override;
    BitMap *bitmap() override;

};
class Intuition_WindowAGA : public IntuitionDrawableAGA
{
public:
    Intuition_WindowAGA(const _osd_create_params *params);
    ~Intuition_WindowAGA();
    void open() override;
    void close() override;
    MsgPort *userPort() override;
protected:
    Window *_pWbWindow;
    BitMap *_sWbWinSBitmap;
    int _machineWidth,_machineHeight;
    RastPort *rastPort() override;
    BitMap *bitmap() override;
    void drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap) override;
};


class Display_AGA : public MameDisplay
{
public:
    Display_AGA();
    ~Display_AGA();
    void open(const _osd_create_params *params,int window, ULONG forcedModeID=~0) override;
    void close() override;
    int good() override;
    void draw(_mame_display *pmame_display) override;    
    MsgPort *userPort() override;
protected:
    IntuitionDrawableAGA   *_drawable; // screen or window
    Paletted_AGA        *_remap;    // null if true color app.

};


#endif
