/******************************************************************************
 * amiga_video_cgx.h
 *
 *      ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
 *      ▓  CyberGraphX Power!  ▓
 *      ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
 *       ╔═══════════════════╗
 *       ║ █▀▀ █▀▀▀ █   █    ║
 *       ║ █   █ ▀█ ▄▀▄ █    ║
 *       ║ ▀▀▀ ▀▀▀▀ ▀   ▀    ║
 *       ╚═══════════════════╝
 *      Native Graphics Bliss
 *
 * Author: krb
 * Copyright (C) 2025
 * Licensed under GPL v2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 *****************************************************************************/

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

#ifndef REGUFT

#ifdef __GNUC__
#define REGUFT(r) __asm(#r)
//#define REGUFT(r)
#else

#endif
#endif

#include "amiga_video_tracers.h"

// what one frame draws: the same for every rectangle it is drawn in.
struct cgxDirectFrame {
    directDrawSource source;
    int cenx,ceny,ww,hh;
};

class Drawable_CGX {
public:
  Drawable_CGX(IntuitionDrawable &drawable);
  virtual ~Drawable_CGX();
  // draw the frame in the whole bitmap
  void drawCGX_Direct(BitMap *pBitmap,_mame_display *display);
  // or in steps: prepare it once, then draw it in a locked bitmap, shifted,
  // and clipped to a rectangle (all bitmap if NULL).
  void prepareDirectFrame(_mame_display *display, cgxDirectFrame &frame);
  void drawDirectFrame(BitMap *pBitmap, cgxDirectFrame &frame,
                       int shiftx, int shifty, const Rectangle *clip);

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
    static bool useForThisMode(ULONG modeID REGUFT(d0), int flags REGUFT(d1));
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
    bool _screenBitmapDirect; // screen bitmap has the pixel format of _sWbWinSBitmap

};


#endif

/* CGX forever! Direct to hardware, baby!
 *       |\___/|
 *      =) ^Y^ (=  <-- Cool fox loves fast graphics
 *       \  ^  /
 *        )=*=(
 *       /     \
 */
