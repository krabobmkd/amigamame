#ifndef AMIGA_VIDEO_CGX_H
#define AMIGA_VIDEO_CGX_H

#include "amiga_video_intuition.h"

// note: anything exported from amiga_video.cpp is defined in mame/osdepends.h
#include <vector>
extern "C"
{
    #include <exec/ports.h>
    #include <graphics/rastport.h>
}

struct directDrawParams;

class Drawable_CGX {
public:
  Drawable_CGX(IntuitionDrawable &drawable);
  virtual ~Drawable_CGX();
  void drawCGX_DirectCPU16(  RastPort *pRPort, BitMap *pBitmap,_mame_display *display);
  void drawCGX_DirectCPU32( BitMap *pBitmap,_mame_display *display);

  inline bool isSourceRGBA32() {
      const ULONG fritata = (VIDEO_RGB_DIRECT|VIDEO_NEEDS_6BITS_PER_GUN);
      return ((_video_attributes&fritata)==fritata);
  }
protected:
  void initRemapTable();
  void close();
  IntuitionDrawable &_drawable;
  Paletted *_pRemap;
  bool _useIntuitionPalette;
  ULONG _PixelFmt,_PixelBytes; // CGX values, from screen or window bitmap.

  // function pointer to draw32 drawCGX_DirectCPU32
  void (*directDrawARGB32)(directDrawParams *p);

  void initARGB32DrawFunctionFromPixelFormat();

    // from mame params
  int _colorsIndexLength;
  int _video_attributes;
};

class Intuition_Screen_CGX : public Intuition_Screen, public Drawable_CGX
{
public:
    Intuition_Screen_CGX(const AbstractDisplay::params &params);
    static bool useForThisMode(ULONG modeID);
    bool open() override;
    void close() override;
    void draw(_mame_display *display) override;
protected:

};
class Intuition_Window_CGX : public Intuition_Window, public Drawable_CGX
{
public:
    Intuition_Window_CGX(const AbstractDisplay::params &params);
   // ~Intuition_Window_CGX();
    static bool useForWbWindow();
    void draw(_mame_display *display) override;
    bool open() override;
    void close() override;
    BitMap *bitmap() override;
protected:
    BitMap *_sWbWinSBitmap;

};


#endif
