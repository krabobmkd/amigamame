#ifndef AMIGA_VIDEO_CGX_H
#define AMIGA_VIDEO_CGX_H

#include "amiga106_video_intuition.h"

// note: anything exported from amiga_video.cpp is defined in mame/osdepends.h
#include <vector>
extern "C"
{
    #include <exec/ports.h>
    #include <graphics/rastport.h>
}
//#include "amiga106_video.h"
//extern "C" {
//    #include "osdepend.h"
//}

class Drawable_CGX {
public:
  Drawable_CGX(IntuitionDrawable &drawable);
  virtual ~Drawable_CGX();
  void drawCGX_DirectCPU(_mame_display *display);
protected:
  void initRemapTable();
  void close();
  IntuitionDrawable &_drawable;
  Paletted *_pRemap;
  bool _useIntuitionPalette;
  ULONG _PixelFmt,_PixelBytes; // CGX values, from screen or window bitmap.
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

// - - - - - - -
class Drawable_CGXScalePixelArray {
public:
  Drawable_CGXScalePixelArray(IntuitionDrawable &drawable);
  virtual ~Drawable_CGXScalePixelArray();
  void drawCGX_scale(_mame_display *display);
protected:
  void initRemapTable();
  void close();
  IntuitionDrawable &_drawable;
  Paletted *_pRemap;
  bool _useIntuitionPalette;
    // from mame params
  int _colorsIndexLength;
  int _video_attributes;
    // cpu remap here
  std::vector<ULONG> _bm;
};
class Intuition_Screen_CGXScale : public Intuition_Screen, public Drawable_CGXScalePixelArray
{
public:
    Intuition_Screen_CGXScale(const AbstractDisplay::params &params);
    static bool useForThisMode(ULONG modeID);
    bool open() override;
    void close() override;
    void draw(_mame_display *display) override;
protected:
};

class Intuition_Window_CGXScale : public Intuition_Window, public Drawable_CGXScalePixelArray
{
public:
    Intuition_Window_CGXScale(const AbstractDisplay::params &params);
//    ~Intuition_Window_CGXScale();
//same    static bool useForWbWindow();
    void draw(_mame_display *display) override;
    bool open() override;
    void close() override;
protected:

};

#endif
