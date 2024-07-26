#ifndef AMIGA_VIDEO_CGX_H
#define AMIGA_VIDEO_CGX_H

// note: anything exported from amiga_video.cpp is defined in mame/osdepends.h
#include <vector>
extern "C"
{
    #include <exec/ports.h>
    #include <graphics/rastport.h>
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
    virtual bool open() = 0;
    virtual void close()= 0;
    virtual MsgPort *userPort() = 0;
    virtual RastPort *rastPort() = 0;
    virtual Screen *screen() { return NULL; }
    inline ULONG pixelFmt() const { return _PixelFmt; }
    inline ULONG pixelBytes() const { return _PixelBytes; }
    inline int flags() const { return _flags; }
    virtual void drawRastPort_CGX(_mame_display *display,Paletted *pRemap);
     void drawRastPortWPA8(_mame_display *display,Paletted *pRemap);

protected:
    void getGeometry(_mame_display *display,int &cenx,int &ceny,int &ww,int &hh);
    ULONG _PixelFmt,_PixelBytes;
    int _width,_height;
    int _useScale;
    int _flags;
    virtual BitMap *bitmap() = 0;
    // used for drawRastPortWPA8
    std::vector<UBYTE> _wpatempbm;
    struct wpa8temprastport {
        int _checkw;
        RastPort _rp;

    };
    wpa8temprastport _trp;
    void checkWpa8TmpRp(RastPort *,int linewidth);

};

class Intuition_Screen : public IntuitionDrawable
{
public:
    Intuition_Screen(const AmigaDisplay::params &params);
    ~Intuition_Screen();
    bool open() override;
    void close() override;
    Screen *screen() override { return _pScreen; }
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
    bool open() override;
    void close() override;
    MsgPort *userPort() override;
    RastPort *rastPort() override;
protected:
    Window *_pWbWindow;
    BitMap *_sWbWinSBitmap;
    int _machineWidth,_machineHeight;
    int _maxzoomfactor;
    BitMap *bitmap() override;
    void drawRastPort_CGX(_mame_display *display,Paletted *pRemap) override;
};
class Intuition_ScaleWindow : public Intuition_Window
{
public:
    Intuition_ScaleWindow(const AmigaDisplay::params &params);
    ~Intuition_ScaleWindow();
protected:

};

class Display_CGX : public AmigaDisplay
{
public:
    Display_CGX();
    ~Display_CGX();
    bool open(const params &pparams) override;
    void close() override;
    int good() override;
    void draw(_mame_display *pmame_display) override;
    MsgPort *userPort() override;
//    RastPort *rastPort() override;
    int switchFullscreen() override;
    void WaitFrame() override;
protected:
    IntuitionDrawable   *_drawable; // this manages screen or window
    Paletted        *_remap;    // this manages pixel mode and color remap.
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

#endif
