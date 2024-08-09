#ifndef AMIGA_VIDEO_H
#define AMIGA_VIDEO_H

// note: anything exported from amiga_video.cpp is defined in mame/osdepends.h
#include <vector>
extern "C"
{
    #include <exec/ports.h>
}

struct _osd_create_params;
struct _mame_display;
struct Window;
struct Screen;
struct RastPort;
struct BitMap;

/** full virtual */
class AbstractDisplay
{
public:
    AbstractDisplay();
    virtual ~AbstractDisplay();
    // flags are orientations bits + our prefs
    #define DISPFLAG_STARTWITHWINDOW 8
    #define DISPFLAG_INTUITIONPALETTE  16
    #define DISPFLAG_USESCALEPIXARRAY  32
    struct params {
        ULONG _flags;
        ULONG _forcedModeID;
        int _width,_height;
        int _colorsIndexLength;
        int _video_attributes;
        int _driverDepth;
    };
    virtual bool open(const params &params) = 0;
    virtual void init_rgb_components(unsigned int *rgbcomponents) = 0; // needed for RGB mode.
    virtual void close()= 0;
    virtual void draw(_mame_display *pmame_display) = 0;
    virtual int good() = 0;
    virtual bool switchFullscreen() = 0;
    virtual MsgPort *userPort() = 0;
//    virtual RastPort *rastPort() = 0;
    virtual void WaitFrame() = 0; // either WaitTTOF or Wait beam
protected:
};
//// experimental trick for leds -> not done
//class ExtraBitmap
//{
//public:
//    int _width;
//    int _height;
//    std::vector<UWORD> _bm; // use the game palette ?
//};


bool SwitchWindowFullscreen();
void ResetWatchTimer();

#endif
