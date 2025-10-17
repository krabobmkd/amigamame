#ifndef AMIGA_VIDEO_CGXSPA_H
#define AMIGA_VIDEO_CGXSPA_H

#include "amiga_video_intuition.h"

// note: anything exported from amiga_video.cpp is defined in mame/osdepends.h
#include <vector>
extern "C"
{
    #include <exec/ports.h>
    #include <graphics/rastport.h>
}

struct directDrawParams;

class Drawable_CGXScalePixelArray {
public:
  Drawable_CGXScalePixelArray(IntuitionDrawable &drawable);
  virtual ~Drawable_CGXScalePixelArray();
  void drawCGX_scale(_mame_display *display);

  inline bool isSourceARGB32() {
      const ULONG fritata = (VIDEO_RGB_DIRECT|VIDEO_NEEDS_6BITS_PER_GUN);
      return ((_video_attributes&fritata)==fritata);
  }
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
