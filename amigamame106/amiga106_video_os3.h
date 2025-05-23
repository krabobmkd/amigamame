#ifndef AMIGA_VIDEO_OS3_H
#define AMIGA_VIDEO_OS3_H

#include "amiga106_video_intuition.h"

extern "C"
{
    #include <graphics/rastport.h>
}
 // gather draw calls here to share.
class Drawable_OS3 {
public:
  Drawable_OS3(IntuitionDrawable &drawable);
  virtual ~Drawable_OS3();
  void draw_WPA8(_mame_display *display);
  void draw_WriteChunkyPixels(_mame_display *display);
#ifdef ACTIVATE_OWN_C2P
// removed because slower than "blazewcp"+writechunkypixels()
  void draw_c2p(_mame_display *display);
#endif
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
    // from mame params
  int _colorsIndexLength;
  int _video_attributes;

  // used for drawRastPortWPA8
    std::vector<UBYTE> _wpatempbm;
    struct wpa8temprastport {
        int _checkw;
        RastPort _rp;
    };
    wpa8temprastport _trp;
    void checkWpa8TmpRp(RastPort *,int linewidth);
};


class Intuition_Screen_OS3 : public Intuition_Screen, public Drawable_OS3
{
public:
    Intuition_Screen_OS3(const AbstractDisplay::params &params);
    ~Intuition_Screen_OS3();

    bool open() override;
    void close() override;
    void draw(_mame_display *display) override;
protected:
};
class Intuition_Window_OS3 : public Intuition_Window, public Drawable_OS3
{
public:
    Intuition_Window_OS3(const AbstractDisplay::params &params);
    ~Intuition_Window_OS3();

    bool open() override;
    void close() override;
    void draw(_mame_display *display) override;

protected:

};
#endif
