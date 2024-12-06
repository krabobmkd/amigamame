#ifndef AMIGA_VIDEO_INTUITION_H
#define AMIGA_VIDEO_INTUITION_H

#include <vector>
#include "amiga106_video.h"
#include "amiga_video_remap.h"

extern "C"
{
   // #include <exec/ports.h>
    #include <graphics/rastport.h>
}

struct Window;
struct Screen;
struct RastPort;
struct BitMap;
struct ScreenBuffer;

// - - - - from driver.h
#define VIDEO_RGB_DIRECT	 			0x0004
#define VIDEO_NEEDS_6BITS_PER_GUN		0x0008

// this differenciate Screen and Windows
class IntuitionDrawable {
public:
    IntuitionDrawable(int flags);
    virtual ~IntuitionDrawable();
    virtual bool open() = 0;
    virtual void close() {}
    virtual void draw(_mame_display *display) = 0;
    //virtual MsgPort *userPort() = 0;
    virtual Window *window() = 0;
    virtual RastPort *rastPort() = 0;
    virtual Screen *screen() { return NULL; }
    virtual BitMap *bitmap() { return NULL; }
    virtual void waitFrame();
    //inline ULONG pixelFmt() const { return _PixelFmt; }
    //inline ULONG pixelBytes() const { return _PixelBytes; }
    inline int flags() const { return _flags; }
    void getGeometry(_mame_display *display,int &cenx,int &ceny,int &ww,int &hh, int &sourceWidth,int &sourceHeight);
  //  virtual void drawRastPort_CGX(_mame_display *display,Paletted *pRemap);
  //   void drawRastPortWPA8(_mame_display *display,Paletted *pRemap);

    inline int width() const { return _widthtarget; }
    inline int height() const { return _heighttarget; }

    inline int blackPen() const { return (int)_blackpen; }
    inline int greyPen() const { return (int)_greyPen; }
    inline int whitePen() const { return 2; }
protected:
    int _widthtarget,_heighttarget;
    int _widthphys,_heightphys;
    int _screenshiftx,_screenshifty; // when direct WB bitmap rendering trick.
    int _useScale;
    int _flags;
    int _heightBufferSwitch;
    int _heightBufferSwitchApplied;
    USHORT _greyPen,_blackpen;
};

class TripleBuffer;

class Intuition_Screen : public IntuitionDrawable
{
public:
    Intuition_Screen(const AbstractDisplay::params &params);
    bool open() override;
    void close() override;
    Screen *screen() override;
    Window *window() override;
    RastPort *rastPort() override;
    BitMap *bitmap() override;
    void waitFrame() override;
protected:
    Screen *_pScreen;
    Window *_pScreenWindow;
    ULONG   _ScreenModeId;
    int _fullscreenWidth; // guessed from modeid.
    int _fullscreenHeight;
    int _screenDepthAsked; // 8 or 16, needed for screen opening.
    void *_pMouseRaster;

    TripleBuffer *_pTripleBufferImpl;

};

class TripleBuffer
{
public:
    TripleBuffer();
    virtual ~TripleBuffer(){}
    virtual int init() =0;
    virtual void close() =0;
    virtual RastPort *rastPort() = 0;
    virtual BitMap *bitmap() =0;
    //virtual int beforeBufferDrawn() =0;
    virtual void waitFrame() = 0;
    virtual void afterBufferDrawn() =0;
    char _tripleBufferInitOk;
    char _lastIndexDrawn;
    char _indexToDraw;
    char _d;
};


class Intuition_Window : public IntuitionDrawable
{
public:
    Intuition_Window(const AbstractDisplay::params &params);
    ~Intuition_Window();
    bool open() override;
    void close() override;
    Screen *screen() override;
    Window *window() override;
    RastPort *rastPort() override;
#ifdef USE_DIRECT_WB_RENDERING
    bool isOnTop();
#endif
protected:
    Window *_pWbWindow;

    int _machineWidth,_machineHeight;
    int _maxzoomfactor;


};



class IntuitionDisplay : public AbstractDisplay
{
public:
    IntuitionDisplay();
    ~IntuitionDisplay();
    bool open(const params &pparams) override;
    void init_rgb_components(unsigned int *rgbcomponents) override; // needed for RGB mode.
    void close() override;
    int good() override;
    void draw(_mame_display *pmame_display) override;
    void drawProgress(int per256, int enm) override;
    MsgPort *userPort() override;
    bool switchFullscreen() override;
    void WaitFrame() override;

protected:
    IntuitionDrawable   *_drawable; // this manages screen or window opening
 //   IntuitionDrawer     *_drawer; // this manages drawing API.
    params _params; // last params set


    // - - - - -  added for direct graphics draw for progbar
    enum epens {
        eWhite,
        eBlack,
        eGrey,
        eBlue,
        eBlue2,
        eEnd
    };

//    int _pens[eEnd];
//    void obtainPens();
//    void releasePens();
    struct TextFont *_font;


};


#endif
