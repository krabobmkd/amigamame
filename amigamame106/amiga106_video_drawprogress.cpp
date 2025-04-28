#include "amiga106_video_drawprogress.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/diskfont.h>

#include <stdio.h>
//
extern "C" {
static const unsigned char tlogomamebm[(128/8)*36]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,0,0,0,32,4,0,0,128,0,4,0,192,0,63,255,254,
    0,0,0,96,12,0,1,128,0,12,1,192,0,127,255,252,0,0,0,224,28,0,3,128,0,28,3,192,0,255,255,248,
    0,0,1,224,60,0,7,128,0,60,7,192,1,255,255,240,0,0,3,224,124,0,15,128,0,124,15,192,3,255,255,224,
    0,0,7,224,252,0,31,128,0,252,31,192,7,255,255,192,0,0,15,225,252,0,63,128,1,252,63,192,15,254,0,0,
    0,0,31,227,252,0,127,128,3,252,127,192,31,252,0,0,0,0,63,231,252,0,255,128,7,252,255,192,63,248,0,0,
    0,0,127,239,252,1,255,128,15,253,255,192,127,255,128,0,0,0,255,255,252,3,255,128,31,255,255,192,255,255,0,0,
    0,1,255,255,252,7,255,128,63,255,255,193,255,254,0,0,0,3,255,255,252,15,255,128,127,255,255,195,255,252,0,0,
    0,7,255,255,252,31,255,128,255,255,255,199,255,248,0,0,0,15,255,255,252,63,255,129,255,255,255,207,255,240,0,0,
    0,31,255,255,252,127,255,131,255,255,255,223,255,224,0,0,0,63,243,255,252,255,255,135,255,127,255,191,248,0,0,0,
    0,127,227,255,249,255,191,143,254,127,223,127,240,0,0,0,0,255,195,253,243,255,63,159,252,127,158,255,224,0,0,0,
    1,255,131,249,231,254,63,191,248,127,29,255,255,255,240,0,3,255,3,241,207,252,63,127,240,126,27,255,255,255,224,0,
    7,254,3,225,159,248,62,255,224,124,23,255,255,255,192,0,15,252,3,193,63,240,61,255,192,120,15,255,255,255,128,0,
    31,248,3,128,127,224,59,255,128,112,31,255,255,255,0,0,63,240,3,0,255,192,55,255,0,96,63,255,255,254,0,0,
    127,224,2,1,255,128,47,254,0,64,127,255,255,252,0,0,0,0,0,3,255,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,7,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,252,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,31,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,240,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,127,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,192,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,255,128,0,0,0,0,0,0,0,0,0,0,0,0,0,3,255,0,0,0,0,0,0,0,0,0,0,0,0
};

}
/*
struct RastPortData
{
   struct RastPort *rp;
   struct Layer_Info *li;
   struct Layer *l;
};

struct RastPortData *CreateRastPort(int width, int height, struct BitMap *friend_bitmap, BOOL displayable)
{
   struct RastPortData *rpd = AllocMem(sizeof(*rpd), MEMF_ANY);

   if (rpd)
   {
      int depth = GetBitMapAttr(friend_bitmap, BMA_DEPTH);
      struct BitMap *bm = AllocBitMap(width, height, depth, BMF_MINPLANES | displayable ? BMF_DISPLAYABLE : 0);

      if (bm)
      {
         rpd->li = NewLayerInfo();

         if (rpd->li)
         {
            rpd->l = CreateUpfrontLayer(rpd->li, bm, 0, 0 width - 1, height - 1, 0, NULL);

            if (rpd->l)
            {
               rpd->rp = rpd->l->rp;
               return rpd;
            }

            DisposeLayerInfo(rpd->li);
         }

         FreeBitMap(bm);
      }

      FreeMem(rpd, sizeof(*rpd);
   }

   return NULL;
}

void DeleteRastPort(struct RastPortData *rpd)
{
   FreeBitMap(rpd->rp->BitMap);
   DeleteLayer (0, rpd->l);
   DisposeLayerInfo(rpd->li);
   FreeMem(rpd, sizeof(*rpd));
}
*/


IntuiBufferedDrawing::IntuiBufferedDrawing()
: _clearedscreenw(0),_clearedscreenh(0)
, _BgBm(NULL),_bgrp(NULL),_layerinfo(NULL),_layer(NULL)
, _bmx1(0),_bmy1(0),_bmw(0),_bmh(0)
{

}
IntuiBufferedDrawing::~IntuiBufferedDrawing()
{
    closeTempBm();
}
void IntuiBufferedDrawing::clearBm(RastPort *rp, int pen,int w,int h)
{
    // should test pen also
    if( w != _clearedscreenw || h != _clearedscreenh)
    {
        SetAPen(rp,pen);
        RectFill(rp,0,0,w,h);
        _clearedscreenw=w; _clearedscreenh=h;
    }
}
void IntuiBufferedDrawing::setUpdatableArea(RastPort *rp,int x1,int y1,int w,int h)
{
    if ( !_BgBm ||
       (_bmx1 != x1 || _bmy1 != y1 || _bmw != w || _bmh != h )
       )
   {
      // printf("setUpdatableArea %d %d\n",w,h);
       // the way to create RastPort that actually clips drawing.
         if(_BgBm) closeTempBm();
         _BgBm = AllocBitMap(w,h,rp->BitMap->Depth,/*BMF_CLEAR*/0,rp->BitMap);
         if(!_BgBm) return;

         _layerinfo = NewLayerInfo();
         if(!_layerinfo) {  closeTempBm(); return; }

         _layer = CreateUpfrontLayer(_layerinfo, _BgBm, 0, 0, w - 1, h - 1, 0, NULL);

         if(!_layer) {  closeTempBm(); return; }
         _bgrp = _layer->rp;

        // the following will not manage draw func clipping:
        //InitRastPort(&BgRp);
        //BgRp.BitMap = _BgBm;

        _bmx1 = x1; _bmy1 = y1;
        _bmw = w;
        _bmh = h;
   }

}
void IntuiBufferedDrawing::closeTempBm()
{
    if(_layer) DeleteLayer (0,_layer);
    _layer = NULL;
    if(_layerinfo) DisposeLayerInfo(_layerinfo);
    _layerinfo = NULL;
    _bgrp = NULL;
    if(_BgBm) FreeBitMap(_BgBm);
    _BgBm = NULL;
}
void IntuiBufferedDrawing::SetAPenTmp(int pen)
{
    if(!_bgrp) return;
    SetAPen(_bgrp,pen);
}

void IntuiBufferedDrawing::RectFillTmp(int x1,int y1,int x2,int y2)
{
    if(!_bgrp) return;
    // consider last excluded  unlike graphics fillrect.
    x1 -= _bmx1;
    x2 -= _bmx1+1;
    y1 -= _bmy1;
    y2 -= _bmy1+1;
    if(x2>_bmw-1) x2=_bmw-1;
    if(y2>_bmh-1) y2=_bmh-1;
    if(x1<0) x1=0;
    if(y1<0) y1=0;
    RectFill(_bgrp,x1,y1,x2,y2);
}
void IntuiBufferedDrawing::drawTextTmp(TextFont *font,int x1,int y1,int pen,const char *ptext)
{
    if(!_bgrp) return;
    SetFont(_bgrp,font);

    int l = strlen(ptext);
    SetDrMd(_bgrp,  JAM1  );

    SetAPen(_bgrp,pen);
    Move(_bgrp,x1-_bmx1,y1-_bmy1);
    Text(_bgrp,ptext,l);
}
void IntuiBufferedDrawing::maskBitmapTmp(BitMap *pbm,int x1,int y1,int w,int h,UWORD minterm, PLANEPTR ptrmask)
{
    if(!_bgrp) return;
   BltMaskBitMapRastPort(pbm,0,0,
        _bgrp,x1-_bmx1,y1-_bmy1,128,36,
        minterm,
         ptrmask
         );
}

void IntuiBufferedDrawing::finalBlit(RastPort *rp)
{
    if(!_BgBm || _bmw==0 || _bmh ==0) return;

    BltBitMapRastPort( _BgBm,//CONST struct BitMap *srcBitMap,
           0,0, //LONG xSrc, LONG ySrc,
           rp,//struct RastPort *destRP,
           _bmx1,_bmy1,//LONG xDest, LONG yDest,
           _bmw, _bmh,
           0x00c0//ULONG minterm  -> copy minterm.
           );
}

IntuiProgressBar::IntuiProgressBar(IntuitionDrawable *drawable)
    : IDrawProgress(),IntuiBufferedDrawing()
    , _drawable(drawable)
    , _pChipLogoAlloc(NULL)
    , _font(NULL)
{

}
IntuiProgressBar::~IntuiProgressBar()
{
    if(_font)
    {
      CloseFont(_font);
      _font=NULL;
    }
    if(_pChipLogoAlloc) FreeVec(_pChipLogoAlloc);
}
void IntuiProgressBar::drawProgress(int per256, int enm)
{

    Window *win = _drawable->window();
    if(!win) return;

    RastPort *rp = win->RPort;
    if(!rp) return;

    // - -  get geometry
    int w = (int)(win->GZZWidth); // this way manage window border size
    int h = (int)(win->GZZHeight & 0x0000ffff);
//    printf("drawProgress1 w:%d h:%d\n",w,h);
    if(_drawable->flags() & DISPFLAG_USEHEIGHTBUFFER)
    {
//     printf("make div\n");
       h>>=1; // double height screen for double buffer trick...
    }
//    printf("drawProgress w:%d h:%d\n",w,h);

    int x1 =  (16*w)>>8;
    int x2 = ((256-16)*w)>>8;
    int y1 =  (198*h)>>8;
    int y2 =  (208*h)>>8;
    int wless = w;
    if(h<wless) wless=h;
    int border =  (1*wless)>>8;
    if(border<1) border=1;


    int blackpen = _drawable->blackPen(); // _pens[eBlack];
    int whitepen = _drawable->whitePen(); // _pens[eWhite];
    int greypen= _drawable->greyPen();

//    if(blackpen == -1) blackpen = GetAPen(rp);
//    if(whitepetopn == -1) whitepen = GetBPen(rp);

    if(per256 >=256)
    {
        // means final pass before game screen , must clean again and quit.
        SetAPen(rp,blackpen); // that time 0, not blackpen, for the case of galaga 0 white
        RectFill(rp,0,0,w,h);
        return;
    }
    // - - - - our quick solution to avoid draw glitch
    // is to consider one big clean per resize
    // and then just draw temp / reblit a portion of screen.
    // which is managed by IntuiBufferedDrawing.
    clearBm(rp,blackpen,w,h); // atually clean whole dest. screen or not.

    // this set or reuse the temp bitmap
    int dry1 = y1-37+10; // top of logo
    int dry2 = y2+8; // down of text
    setUpdatableArea(rp,0,dry1,w,dry2-dry1);

    // clean at temp buffer level
    SetAPenTmp(blackpen);
    RectFillTmp(0,dry1,w,dry2);

    // // draw external bars
    SetAPenTmp(greypen);
    RectFillTmp(x1,y1,x2,y1+border);
    RectFillTmp(x1,y2-border,x2,y2);
    RectFillTmp(x1,y1+border,x1+border+border,y2-border);
    RectFillTmp(x2-border-border,y1+border,x2,y2-border);

    // inside bar

    int xb1 = x1+3*border;
    int xb2 = x2-3*border;
    int yb1 = y1+2*border;
    int yb2 = y2-2*border;
    RectFillTmp(xb1,yb1,xb1 + ((xb2-xb1)*per256)/256,yb2);

    const char *phases[]={
        "Temporal convector flux init.",
        "Initialize input ports.",
        "Rom load...",
        "Memory and CPU inits...",
        "Hi Score load.",
        "Rebuild machine...",
        "Decode graphics...",
        "Load cheats...",
        ""
    };

    if(!_font)
    {
        struct TextAttr fntatrb={
            (STRPTR)"topaz.font",7,0,0 // trick: crush one line of 8 to 7 gives nasty cool style.
        };
        _font = OpenDiskFont(&fntatrb );
    }

    if(_font && enm<sizeof(phases)/sizeof(const char *))
    {
/*
        ebStart=0,
        ebInput,
        ebRomLoad,
        ebMemoryAndCpu,
        ebHighScoreSaveLoad,
        eDriver, //  decryption is done and memory maps altered
        eSoundVideo,
        eCheat,
        eProgressEnd
*/
        int xt= x1+8;
        const char *p = phases[enm];

        drawTextTmp(_font,xt,yb2+6,blackpen,p);
        drawTextTmp(_font,xt+1,yb2+6,blackpen,p);
        drawTextTmp(_font,xt+1,yb2+7,whitepen,p);

    }

     // - - - logo !

    ULONG destflags =GetBitMapAttr(rp->BitMap,BMA_FLAGS);

    PLANEPTR bmptr = (PLANEPTR)&tlogomamebm[0];
    if(((destflags & BMF_STANDARD)!=0) && _pChipLogoAlloc == NULL)
    {
        // means screen will have native blitter use,
        // so source bitmap needs to be in chipram
        // alloc once.
        _pChipLogoAlloc = (UWORD *)AllocVec(sizeof(tlogomamebm),MEMF_CHIP);
        if(_pChipLogoAlloc)
        {
            memcpy(_pChipLogoAlloc,bmptr,sizeof(tlogomamebm));
        }
    }
    if(_pChipLogoAlloc) bmptr = (PLANEPTR)_pChipLogoAlloc;

    BitMap bm;
    InitBitMap(&bm, 2, 128, 36 );
    bm.Planes[0] = bmptr;
    bm.Planes[1] = bmptr;

    // mask erase to make shadow, will work both on native planar and gfx cards !
    if(blackpen==0)
    {
        maskBitmapTmp(&bm,w-128-16-2  ,y1-37+11,128,36,
            0x0022, /*  blitter minterms 0x00c0*/   // d8 e4 ?
            bmptr);
    }

    // mask write logo
    maskBitmapTmp(&bm,w-128-16,y1-37+10,128,36,
        0x00e4, /*  blitter minterms 0x00c0*/   // d8 e4 ?
         bmptr);

    // means final pass before game screen , must clean again and quit.
    SetAPen(rp,blackpen);
    RectFill(rp,0,0,w,dry1-1);

    finalBlit(rp);

    SetAPen(rp,blackpen);
    RectFill(rp,0,dry2,w,h);
/* note to find correct blitter "minterms" values for masking.
 * A source, B mask, C dest
A B C BitPos in minterm
0 0 0 0
0 0 1 1
0 1 0 2
0 1 1 3

1 0 0 4
1 0 1 5
1 1 0 6
1 1 1 7
*/
}
