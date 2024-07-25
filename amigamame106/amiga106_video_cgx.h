#ifndef AMIGA_VIDEO_CGX_H
#define AMIGA_VIDEO_CGX_H

// note: anything exported from amiga_video.cpp is defined in mame/osdepends.h
#include <vector>
extern "C"
{
    #include <exec/ports.h>
}
#include "amiga106_video.h"
extern "C" {
    #include "osdepend.h"
}
#include "amiga_video_remap.h"

struct _osd_create_params;
struct _mame_display;
struct Window;
struct Screen;
struct RastPort;
struct BitMap;

/** virtual, at this level manage screen and window opening, not rendering */

class IntuitionDrawable {
public:
    IntuitionDrawable(int flags);
    virtual ~IntuitionDrawable();
    virtual void open() = 0;
    virtual void close()= 0;
    virtual MsgPort *userPort() = 0;
    virtual RastPort *rastPort() = 0;
    inline ULONG pixelFmt() const { return _PixelFmt; }
    inline ULONG pixelBytes() const { return _PixelBytes; }

    virtual void drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap);
protected:
    ULONG _PixelFmt,_PixelBytes;
    int _width,_height;
    int _useScale;
    int _flags;
    // 8Bits Modes can either have static remaped palette,
    // or _8BitsHasPaletteMode
    int _8BitsHasDynamicPalette;
    virtual BitMap *bitmap() = 0;
};

class Intuition_Screen : public IntuitionDrawable
{
public:
    Intuition_Screen(const AmigaDisplay::params &params);
    ~Intuition_Screen();
    void open() override;
    void close() override;
    MsgPort *userPort() override;
    RastPort *rastPort() override;
protected:
    Screen *_pScreen;
    Window *_pScreenWindow;
    ULONG   _ScreenModeId;
    int _fullscreenWidth; // guessed from modeid.
    int _fullscreenHeight;
    void *_pMouseRaster;

    BitMap *bitmap() override;

};
class Intuition_Window : public IntuitionDrawable
{
public:
    Intuition_Window(const AmigaDisplay::params &params);
    ~Intuition_Window();
    void open() override;
    void close() override;
    MsgPort *userPort() override;
    RastPort *rastPort() override;
protected:
    Window *_pWbWindow;
    BitMap *_sWbWinSBitmap;
    int _machineWidth,_machineHeight;
    int _maxzoomfactor;
    BitMap *bitmap() override;
    void drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap) override;
};
class Intuition_ScaleWindow : public Intuition_Window
{
public:
    Intuition_ScaleWindow(const AmigaDisplay::params &params);
    ~Intuition_ScaleWindow();
protected:
    //void synchBmSize();
    //void drawRastPort_CGX(_mame_display *display,Paletted_CGX *pRemap) override;
};

class Display_CGX : public AmigaDisplay
{
public:
    Display_CGX();
    ~Display_CGX();
    void open(const params &pparams) override;
    void close() override;
    int good() override;
    void draw(_mame_display *pmame_display) override;
    MsgPort *userPort() override;
//    RastPort *rastPort() override;
    int switchFullscreen() override;
    void WaitFrame() override;
protected:
    IntuitionDrawable   *_drawable; // screen or window
    Paletted_CGX        *_remap;    // null if true color app.
    params _params; // last params set
    ULONG _forcedModeId;
 //   void drawRastPort(RastPort *pRPort,_mame_display *pmame_display,int dx,int dy);
};
/*
class Display_CGX_TrueColor : public Display_Intuition
{
public:
    Display_CGX_TrueColor(const _osd_create_params *params, ULONG forcedModeID=~0);
    ~Display_CGX_TrueColor();
    void draw(_mame_display *pmame_display) override;
protected:

};*/
// - - - - - - -
/*
class Display_P96 : public MameDisplay
{
public:
    Display_P96(unsigned int width,
				unsigned int height);
    ~Display_P96();
    void draw(_mame_display *pmame_display) override;
protected:
    Window *_pWindow;
};
*/

#endif
