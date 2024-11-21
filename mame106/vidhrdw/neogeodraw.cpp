#include "neogeodraw.h"
extern "C" {
    #include "drawgfx.h"
    extern UINT16 *neogeo_vidram16;
    extern int high_tile;
    extern int vhigh_tile;
    extern int vvhigh_tile;
    extern unsigned int neogeo_frame_counter;
    extern UINT8 *neogeo_memory_region_gfx3;
    extern UINT8 *neogeo_memory_region_gfx4;
    extern char zoomx_draw_tables[16][16];
}

#if defined(__GNUC__) && defined(__AMIGA__)
#define REGNG(r) __asm(#r)
    //#define REGM(r)
#else
#define REGNG(r)
#endif


//    int tutute= 0;
// note: could be inverted in tile decoding.
#ifdef LSB_FIRST
        // little endian: intel arm
#define WR_ORD0  0
#define WR_ORD1  1
#define WR_ORD2  2
#define WR_ORD3  3
#define WR_ORD4  4
#define WR_ORD5  5
#define WR_ORD6  6
#define WR_ORD7  7

#else
        // ppc, 68k:
        // 10325476
#define WR_ORD0  6
#define WR_ORD1  7
#define WR_ORD2  4
#define WR_ORD3  5
#define WR_ORD4  2
#define WR_ORD5  3
#define WR_ORD6  0
#define WR_ORD7  1
#endif

extern "C"
    {
    int dbg_nbt=0;
}

static int no_of_tiles=0;
static UINT32 tileOffsetFilter=0;


struct neoPixelWriter_Opaque
{
    static bool isOpaque() { return true; }
    static void setpixel(UINT32 palette,UINT32 col,UINT16 *bm)
    {
        *bm = palette+col;
    }
};

struct neoPixelWriter_Transparent0
{
    static bool isOpaque() { return false; }
    static void setpixel(UINT32 palette,UINT32 col,UINT16 *bm)
    {
        if(col) *bm = palette+col;
    }
};


typedef struct _NeoDrawGfxParams {
    UINT8 *fspr;
    UINT16 **line;
    INT32 dy;
    INT32 sy;
    INT32 ey;
    INT32 sx;
    UINT16 zx;
    UINT16 ipalette;
    int flipx;
} sNeoDrawGfxParams;








// when Y zoom is applied, we have to draw line by lines.
// this would write a 16 pixel sprite line, applying X zoom and flipx
// template are used to avoid x-zoom-related tests-per-pixels.
// everything is inlined by compiler.
template<class PixelWriter,bool flipx,
         char j0,char j1,char j2,char j3,char j4,char j5,char j6,char j7,
         char j8,char j9,char j10,char j11,char j12,char j13,char j14,char j15>
void NeoDrawGfx16line(
    UINT16 *bm REGNG(a0),
    UINT8 *fspr REGNG(a1),
    UINT16 ipalette REGNG(d0))
{
    if(!flipx)
    {
        UINT32 c = *fspr++;

        if (j0) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j1) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fspr++;
        if (j2) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j3) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fspr++;
        if (j4) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j5) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fspr++;
        if (j6) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j7) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }

        c = *fspr++;
        if (j8) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j9) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fspr++;
        if (j10) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j11) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fspr++;
        if (j12) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j13) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fspr;
        if (j14) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j15) { PixelWriter::setpixel(ipalette,c>>4,bm); }
    } else
    {
        fspr +=7;
        UINT32 c = *fspr--;

        if (j0) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j1) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fspr--;
        if (j2) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j3) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fspr--;
        if (j4) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j5) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fspr--;
        if (j6) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j7) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }

        c = *fspr--;
        if (j8) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j9) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fspr--;
        if (j10) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j11) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fspr--;
        if (j12) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j13) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fspr;
        if (j14) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j15) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm);  }


    }
}

template<class PixelWriter,bool flipx>
void NeoDrawGfx16line0( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 >(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line1( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line2( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line3( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0>(bm,fspr,ipalette); }

template<class PixelWriter,bool flipx>
void NeoDrawGfx16line4( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line5( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx, 0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line6( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line7( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(bm,fspr,ipalette); }


template<class PixelWriter,bool flipx>
void NeoDrawGfx16line8( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line9( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line10( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line11( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1>(bm,fspr,ipalette); }


template<class PixelWriter,bool flipx>
void NeoDrawGfx16line12( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line13( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line14( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx, 1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1>(bm,fspr,ipalette); }
template<class PixelWriter,bool flipx>
void NeoDrawGfx16line15( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0))
{ NeoDrawGfx16line<PixelWriter,flipx,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1>(bm,fspr,ipalette); }

typedef void (*neoLineWriter)( UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0));

// - - - this table is indexed like this:
// (p->zx) +   // 0->15   zoomx cases
// ((penusage & 1)<<4) + // 0 or 16,  , 0 means opaque
// ((tileatr & 0x01)<<5) // 0 or 32, flip x

static neoLineWriter lineWriters[64]={

    // - - - -- - - - -- - - - - no flipx, Opaque
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line0<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
   [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line1<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line2<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line3<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line4<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line5<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line6<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line7<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line8<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line9<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line10<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line11<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line12<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line13<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line14<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line15<neoPixelWriter_Opaque,false>(bm,fspr,ipalette);},

    // - - - -- - - - -- - - - - no flipx, Transparent0

    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line0<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
   [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line1<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line2<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line3<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line4<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line5<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line6<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line7<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line8<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line9<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line10<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line11<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line12<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line13<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line14<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line15<neoPixelWriter_Transparent0,false>(bm,fspr,ipalette);},


    // - - - -- - - - -- - - - - flipx, Opaque
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line0<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
   [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line1<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line2<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line3<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line4<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line5<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line6<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line7<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line8<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line9<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line10<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line11<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line12<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line13<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line14<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line15<neoPixelWriter_Opaque,true>(bm,fspr,ipalette);},

    // - - - -- - - - -- - - - - flipx, Transparent0

    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line0<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
   [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line1<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line2<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line3<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line4<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line5<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line6<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line7<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line8<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line9<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line10<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line11<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line12<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line13<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line14<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},
    [](UINT16 *bm REGNG(a0), UINT8 *fspr REGNG(a1),UINT16 ipalette REGNG(d0)){
            NeoDrawGfx16line15<neoPixelWriter_Transparent0,true>(bm,fspr,ipalette);},

};



// This version is used when no Y Zoom, so we can do a classic fast y loop.
// template are used to avoid x-zoom-related tests-per-pixels.
// everything is inlined by compiler.
// there is technically 15*2 assembled version of this.
// PixelWriter tells if opaque or transparent.
// if opaque: ... no test at all per pixels.
template<class PixelWriter,
         char j0,char j1,char j2,char j3,char j4,char j5,char j6,char j7,
         char j8,char j9,char j10,char j11,char j12,char j13,char j14,char j15>
void NeoDrawGfx16zx(sNeoDrawGfxParams *p REGNG(a0))
{
    UINT8 *fspr = p->fspr;
    UINT16 **line = p->line;
    UINT16 ipalette = p->ipalette;
    INT32 dy = p->dy;
    INT32 sy = p->sy;
    INT32 ey = p->ey;
    INT32 sx = p->sx;
    for (INT16 y = sy ;y <= ey;y++)
    {
        UINT16 *bm  = line[y] + sx;
        UINT8 *fsprb = fspr;

        UINT32 c = *fsprb++;

        if (j0) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j1) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fsprb++;
        if (j2) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j3) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fsprb++;
        if (j4) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j5) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fsprb++;
        if (j6) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j7) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }

        c = *fsprb++;
        if (j8) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j9) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fsprb++;
        if (j10) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j11) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fsprb++;
        if (j12) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j13) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }
        c = *fsprb;
        if (j14) { PixelWriter::setpixel(ipalette,c&0x0f,bm); bm++; }
        if (j15) { PixelWriter::setpixel(ipalette,c>>4,bm); }

        fspr+=dy;
    }
}
// used to avoid x-zoom-related tests-per-pixels.
// everything is inlined by compiler.
// there is technically 15*2 assembled version of this.
// PixelWriter tells if opaque or transparent.
// if opaque: ... no test at all per pixels.
 // FLIPX version.
template<class PixelWriter,
         char j0,char j1,char j2,char j3,char j4,char j5,char j6,char j7,
         char j8,char j9,char j10,char j11,char j12,char j13,char j14,char j15>
void NeoDrawGfx16flipxZx(sNeoDrawGfxParams *p REGNG(a0))
{
    UINT8 *fspr = p->fspr + 7; // +7 because flipx start end of x line.
    UINT16 **line = p->line;
    UINT16 ipalette = p->ipalette;
    INT32 dy = p->dy;
    INT32 sy = p->sy;
    INT32 ey = p->ey;
    INT32 sx = p->sx;
    for (INT16 y = sy ;y <= ey;y++)
    {
        UINT16 *bm  = line[y] + sx;
        UINT8 *fsprb = fspr;

        UINT32 c = *fsprb--;

        if (j0) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j1) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fsprb--;
        if (j2) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j3) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fsprb--;
        if (j4) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j5) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fsprb--;
        if (j6) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j7) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }

        c = *fsprb--;
        if (j8) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j9) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fsprb--;
        if (j10) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j11) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fsprb--;
        if (j12) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j13) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        c = *fsprb;
        if (j14) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
        if (j15) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm);  }

        fspr+=dy;
    }
}

template<class PixelWriter>
void NeoDrawGfx16(sNeoDrawGfxParams *p REGNG(a0))
{
    if (p->flipx)	/* X flip */
    {
        UINT8 *fspr = p->fspr;
        UINT16 **line = p->line;
        UINT16 ipalette = p->ipalette;
        INT32 dy = p->dy;
        INT16 sy = (INT16)p->sy;
        INT16 ey = (INT16)p->ey;
        INT32 sx = p->sx;
        //if(p->zx==15)
        switch(p->zx)
        {
            case 15:
            for (INT16 y = sy;y <= ey;y++)
            {
                UINT16 *bm  = p->line[y]+p->sx;
                UINT32 *fspr4 = (UINT32*)fspr;
                UINT32 v = *fspr4++;
                 // test totally removed by compiler when PixelWriter::isOpaque(). template magic.
                if(PixelWriter::isOpaque() || v)
                {
                    UINT8 col;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD7);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD6);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD5);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD4);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD3);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD2);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD1);
                    v>>=4;
                    col = ((UINT8)v); PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD0);
                }
                v = *fspr4;
                 // test totally removed by compiler when PixelWriter::isOpaque(). template magic.
                if(PixelWriter::isOpaque() || v)
                {
                    UINT8 col;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD7);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD6);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD5);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD4);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD3);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD2);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD1);
                    v>>=4;
                    col = ((UINT8)v); PixelWriter::setpixel(ipalette,col,bm+WR_ORD0);
                }
                fspr+=dy;
            }
            break;
            case 0:NeoDrawGfx16flipxZx<PixelWriter,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0>(p);
                break;
            case 1:NeoDrawGfx16flipxZx<PixelWriter,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0>(p);
                break;
            case 2:NeoDrawGfx16flipxZx<PixelWriter,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0>(p);
                break;
            case 3:NeoDrawGfx16flipxZx<PixelWriter,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0>(p);
                break;
            case 4:NeoDrawGfx16flipxZx<PixelWriter,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0>(p);
                break;
            case 5:NeoDrawGfx16flipxZx<PixelWriter,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0>(p);
                break;
            case 6:NeoDrawGfx16flipxZx<PixelWriter,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(p);
                break;
            case 7:NeoDrawGfx16flipxZx<PixelWriter,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(p);
                break;
            case 8:
                NeoDrawGfx16flipxZx<PixelWriter,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0>(p);
                break;
            case 9:NeoDrawGfx16flipxZx<PixelWriter,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0>(p);
                break;
            case 10:NeoDrawGfx16flipxZx<PixelWriter,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1>(p);
                break;
            case 11:NeoDrawGfx16flipxZx<PixelWriter,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1>(p);
                break;
            case 12:NeoDrawGfx16flipxZx<PixelWriter,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1> (p);
                break;
            case 13:NeoDrawGfx16flipxZx<PixelWriter,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1>(p);
                break;
            case 14:NeoDrawGfx16flipxZx<PixelWriter,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1>(p);
                break;

        } // end flipx zx switch
    }
    else		/* normal no flipx */
    {
        switch(p->zx)
        {
        case 15:
        {
            UINT8 *fspr = p->fspr;
            UINT16 **line = p->line;
            UINT16 ipalette = p->ipalette;
            INT32 dy = p->dy;
            INT32 sy = p->sy;
            INT32 ey = p->ey;
            INT32 sx = p->sx;
            for (INT16 y = sy ;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y]) + sx;

                UINT32 *fspr4 = (UINT32*)fspr;
                UINT32 v = fspr4[0];
                if(PixelWriter::isOpaque() || v )
                {
                    UINT8 col;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD0);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD1);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD2);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD3);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD4);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD5);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+WR_ORD6);
                    v>>=4;
                    col = ((UINT8)v); PixelWriter::setpixel(ipalette,col,bm+WR_ORD7);
                }
                v = fspr4[1];
                if( PixelWriter::isOpaque() || v )
                {
                    UINT8 col;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD0);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD1);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD2);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD3);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD4);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD5);
                    v>>=4;
                    col = ((UINT8)v)&0x0f; PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD6);
                    v>>=4;
                    col = ((UINT8)v); PixelWriter::setpixel(ipalette,col,bm+8+WR_ORD7);
                }
                fspr+=dy;
            }
        } break; //15
            /*
	{ 0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 },
	{ 0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0 },
	{ 0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 },
	{ 0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0 },

	{ 0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0 },
	{ 0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0 },
	{ 0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0 },
	{ 1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0 },

	{ 1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0 },
	{ 1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0 },
	{ 1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1 },
	{ 1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1 },

	{ 1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1 },
	{ 1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1 },
	{ 1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
*/
        case 0:NeoDrawGfx16zx<PixelWriter,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0>(p);
            break;
        case 1:NeoDrawGfx16zx<PixelWriter,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0>(p);
            break;
        case 2:NeoDrawGfx16zx<PixelWriter,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0>(p);
            break;
        case 3:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0>(p);
            break;
        case 4:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0>(p);
            break;
        case 5:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0>(p);
            break;
        case 6:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(p);
            break;
        case 7:NeoDrawGfx16zx<PixelWriter,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(p);
            break;
        case 8:
            NeoDrawGfx16zx<PixelWriter,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0>(p);
            break;
        case 9:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0>(p);
            break;
        case 10:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1>(p);
            break;
        case 11:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1>(p);
            break;
        case 12:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1> (p);
            break;
        case 13:NeoDrawGfx16zx<PixelWriter,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1>(p);
            break;
        case 14:NeoDrawGfx16zx<PixelWriter,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1>(p);
            break;

        }// end zoom switch normal x

    } // end no flip x

}

template<int fullmode>
static inline void tileYLoopZoomY(
    sNeoDrawGfxParams *p REGNG(a0),
    UINT32 offs REGNG(d0),INT16 sy REGNG(d1),INT32 my REGNG(d2),UINT16 zy REGNG(d3),
    const rectangle *cliprect REGNG(a1)
    )
{
    // it is assumed that my is max 0x20.
    const gfx_element *gfx=Machine->gfx[2]; /* Save constant struct dereference */
    UINT8 *fspr = (UINT8 *)neogeo_memory_region_gfx3;
    /* get pointer to table in zoom ROM (thanks to Miguel Angel Horna for the info) */
    UINT8 *zoomy_rom = neogeo_memory_region_gfx4 + (zy << 8);
    UINT16 **line = p->line;

    const INT16 miny = (INT16)cliprect->min_y;
    const INT16 maxy = (INT16)cliprect->max_y;
    const INT32 sx = p->sx;

    // - - - - - - - -
    // tile-related data that are persistent if next line is same tile.
    UINT16 tile=16; // can only be 0,15, so it will be changed at first iteration.
    UINT32 penusage; // = gfx->pen_usage[tileno];
    UINT8 *fspr_tile;
    UINT16 ipalette; // =  p->ipalette = (tileatr >> 4 & 0x0ff0); ;
    neoLineWriter linewriter;
    UINT16 yXorFlipper=0;

    INT16 drawn_lines = 0;

    // make things more clear for tests, avoid ymin ymax tests inside yloop.
    // incidently 0x01ff mask will have the same results.
    const INT16 mysize = my<<4;

    // jump first offscreen if there is.
    if(sy<miny) { drawn_lines = (miny-sy); }

    // next stop is either mysize or maxy
     INT16 next_stop = mysize;


     if( (maxy - sy) < next_stop )
     {
         next_stop = (maxy - sy) ;
     }

     // super cool Y loop, only 3 tests per Y (was 6 before)
    /* my holds the number of tiles in each vertical multisprite block */
     // the sickness with this loop and test is that,
     // same strip can start mid screen, then loop down and continue up the screen.
     // thid outer while is meant to perform down then up, 2 pass max.
    while(drawn_lines < mysize)
    {
        while(drawn_lines < next_stop)
        {
            UINT16 newtile;
            UINT16 yoffs;
            int zoom_line;

            zoom_line = drawn_lines & 0xff;

            // this test actually templatized. -> no test
            if (fullmode)	/* fix for joyjoy, trally... */
            {
                UINT16 invert=0;
                if (drawn_lines & 0x100)
                {
                    zoom_line ^= 0xff;
                    invert = 1;
                }

                    if (zy) // fullmode special things applied here
                    {
                        zoom_line %= 2*zy;
                        if (zoom_line >= zy)
                        {
                            zoom_line = 2*zy-1 - zoom_line;
                            invert ^= 1;
                        }
                    }

                yoffs = zoomy_rom[zoom_line] & 0x0f;
                newtile = zoomy_rom[zoom_line] >> 4;

                if (invert)
                {
                    newtile ^= 0x1f;
                    yoffs ^= 0x0f;
                }

            } else
            {  // no fullmode, 99% cases

                if (drawn_lines & 0x100) // if second batch of 16 tiles !
                {
                    zoom_line ^= 0xff;
                    // invert
                    yoffs = (zoomy_rom[zoom_line] & 0x0f)^0x0f;
                    newtile = (zoomy_rom[zoom_line] >> 4)^0x1f;
                } else
                {
                    yoffs = zoomy_rom[zoom_line] & 0x0f;
                    newtile = zoomy_rom[zoom_line] >> 4;
                }

            } // end no full mode (99% cases)

            if(newtile != tile)
            {
                tile = newtile;
                // tile index in base is 16b, then extended, so UINT32.
                UINT32 tileno	= (UINT32)neogeo_vidram16[offs+2*tile];
                UINT16 tileatr = neogeo_vidram16[offs+2*tile+1];

                tileno += ((UINT32)tileatr & tileOffsetFilter)<<12;

                if (tileatr & 0x08) tileno=(tileno&~7)|(neogeo_frame_counter&7);	/* fixed */
                else if (tileatr & 0x04) tileno=(tileno&~3)|(neogeo_frame_counter&3);	/* fixed */

                penusage = gfx->pen_usage[tileno];            // escape whole tile here ?
/*todo
                if(penusage == 1) // only color 0 on whole tile.
                {
                    // escape to line that is next tile...
                    drawn_lines += escapeToNextTile_table[zoom_line];
                    continue;
                }
                */
                yXorFlipper = (tileatr>>1 & 0x01) * 0x0f;

                ipalette = (tileatr >> 4 & 0x0ff0);
                fspr_tile =  fspr + (tileno*128) ;

                // there is a function pointer table with 16 xzoomcase *(2flipx) * (2opaque/tr)
                // = 64 functions , NO TESTS !!
                linewriter = lineWriters[
                                    (p->zx) +   // 0->15
                                    ((penusage & 1)<<4) + // 0 or 16,  , 0 means opaque
                                    ((tileatr & 0x01)<<5) // 0 or 32, flip x
                                    ];

            }
            if(penusage == 1) { drawn_lines++; continue; }

            // todo, this test could be escaped in tile change with a jump table.
            //if(penusage == 1) continue; // only color 0 on whole tile.
            INT16 yy = (sy + drawn_lines) & 0x1ff;
            linewriter( line[yy]+sx , fspr_tile + ((yoffs^yXorFlipper)<<3) , ipalette );

            drawn_lines++;

        }  /* for y */

        if(drawn_lines < mysize)
        {   // means previous stop for ymax, so now
            // jump down to up, and continue up if very long strip.
            int delta = ( 0x200 - (sy+drawn_lines)) + miny; // jump blank, actually.
            drawn_lines += delta;
            next_stop = mysize;
        }

    }
} // end Yzoom version

// still manage x zoom, x flip, y flip.
// we can treat 16pix height tile with a fast y loop.
// no need for "yzoom table"

static inline void tileYLoopNozoomY(
    sNeoDrawGfxParams *p REGNG(a0),
    UINT32 offs REGNG(d0),INT16 sy REGNG(d1),INT32 my REGNG(d2),
    const rectangle *cliprect REGNG(a1)
    )
{
    UINT16 ofsb=0;
    // here sy is [0,511]
    // just does a fast clip if starts off.

    const gfx_element *gfx=Machine->gfx[2]; /* Save constant struct dereference */
    // clip tiles UP completely off, fast.
    if(sy<=cliprect->min_y-16) {
       INT32 d= (cliprect->min_y-sy)>>4;
        ofsb += d*2;
        my -= d;
        sy += d<<4;
    }
    UINT16 *pvidram = &neogeo_vidram16[offs];


    UINT8 *fspr = (UINT8 *)neogeo_memory_region_gfx3;
    /* my holds the number of tiles in each vertical multisprite block */
    for(;my>0;my--) {
        UINT32 tileno	= (UINT32)pvidram[ofsb];
        ofsb++;
        UINT16 tileatr = pvidram[ofsb];
        ofsb++;

        // fast test of tile offsets bits 0x0070.
        tileno += ((UINT32)tileatr & tileOffsetFilter)<<12;

        if (tileatr & 0x08) tileno=(tileno&~7)|(neogeo_frame_counter&7);	/* fixed */
        else if (tileatr & 0x04) tileno=(tileno&~3)|(neogeo_frame_counter&3);	/* fixed */

        // tileno = tileno % no_of_tiles; // safety.
        if(tileno>=no_of_tiles) return; // less divs, souldnt happen anyway.

        UINT32 penusage = gfx->pen_usage[tileno];

        if ((penusage & ~1) != 0)  // if not fully transparent.
        {
            // = = = = prepare y things = = = =
            INT16 syy=sy;
            INT16 ey = sy + 15; 	/* Clip for size of zoomed object */

            if (syy < cliprect->min_y) syy = cliprect->min_y;
            if (ey > cliprect->max_y) ey = cliprect->max_y;

            if (/*flipy*/tileatr & 0x02)
            {
                p->dy = -8;
                p->fspr = fspr + (tileno*128) + 128 - 8 - (syy-sy)*8;
            }
            else		/* normal */
            {
                p->dy = 8;
                p->fspr = fspr + (tileno*128) + (syy-sy)*8;
            }
            p->ipalette = (tileatr >> 4 & 0x0ff0); // (tileatr >> 8) * gfx->color_granularity

             p->sy = syy;
             p->ey = ey;
//             p->sx = sx; done outside.
//             p->zx = zx;
             p->flipx = tileatr & 0x01;


            if((penusage & 1)==0)
            {   // fully opaque
                NeoDrawGfx16<neoPixelWriter_Opaque>(p);

            } else
            {
                NeoDrawGfx16<neoPixelWriter_Transparent0>(p);
            }

        } // end if draw ok.

        sy +=16;        
        if( sy > cliprect->max_y)
        {
            // if my 32, will round to up !
            INT32 d= ((512 -sy)+cliprect->min_y)>>4;

//                ((512-sy)>>4) +1; // +1 because miny at 16 !!

            ofsb += d*2;
            my -= d;
            sy += d<<4;
            sy-=512;
        }

    }  /* for y sprites */
}

void neogeo_drawTilesSprites( mame_bitmap *bitmap, const rectangle *cliprect)
{
    /* Draw sprites */
    int sx =0,my =0,zx = 15,zy=0x00ff;
    INT16 sy; // [0,511] , wrap around Y
    int offs,i,count,y,x;

    int t2;

    char fullmode=0;
    int ddax=0,dday=0,yskip=0;
    UINT16 **line=(UINT16 **)bitmap->line;
    unsigned int *pen_usage;
    const gfx_element *gfx=Machine->gfx[2]; /* Save constant struct dereference */

    // - - - - get tile rom length configuration...
    // should be done per game launch .
    no_of_tiles=gfx->total_elements;
    UINT32 ltileOffsetFilter = 0;
    if (no_of_tiles>0x40000)  ltileOffsetFilter = 0x70;
	else if (no_of_tiles>0x20000)  ltileOffsetFilter = 0x30;
	else if (no_of_tiles>0x10000) ltileOffsetFilter = 0x10;
    tileOffsetFilter = ltileOffsetFilter;



    sNeoDrawGfxParams sParams;
    sParams.line = line;

   // int nbt = 0;
    // 384 zoomable 16x16 sprites
    for (INT16 count = 0; count < 0x300 >> 1; count++)
    {
        UINT16 t1 = neogeo_vidram16[(0x10400 >> 1) + count];
        UINT16 t3 = neogeo_vidram16[(0x10000 >> 1) + count];

        // watch out, a sprite can draw nothing but just be its geometry used for shifting next sprtes.
        /* If this bit is set this new column is placed next to last one */
        if (t1 & 0x40) {
            sx += zx+1;
            zx = (t3 >> 8) & 0x0f;
            if (my==0) continue; // fast exit when sprite not used.
        } else {	/* nope it is a new block */
            /* Number of tiles in this strip */
            my = t1 & 0x3f;

            zx = (t3 >> 8) & 0x0f;
            zy = t3 & 0xff;
            t2 = neogeo_vidram16[(0x10800 >> 1) + count];
            sx = (t2 >> 7);
            sy = 0x200 - (t1 >> 7);
            // krb: make Y space much clearer with negative values.
            if( sy > (INT16)cliprect->max_y ) sy -=512; // sy [-300,   0, miny , maxy ]

            //if (my==0) continue; // fast exit when sprite not used.

            if (my > 0x20)
            {
                my = 0x20;
                fullmode = 1;
            }
            else
                fullmode = 0;
           		/* setup x zoom */
        }
        if (my==0) continue; // fast exit when sprite not used.

        if ( sx >= 0x1F0 )
            sx -= 0x200;

        if(sx>=320 || sx<=-16) continue;

        offs = count<<6;

        sParams.sx = sx;
        sParams.zx = zx;

        if(!fullmode) // 99% of the cases.
        {
            if(zy == 0x00ff)
            { // no-fullstrip, no-yzoom
                tileYLoopNozoomY(&sParams,offs,sy,my,cliprect);
            //    tileYLoopZoomY<0>(&sParams,offs,sy,my,zy,cliprect);
            } else
            { // no-fullstrip, yzoom
                tileYLoopZoomY<0>(&sParams,offs,sy,my,zy,cliprect);
            }
        } // end non-full mode vstrip
        else
        {  // fullmode vstrip (RARE)
             if(zy == 0x00ff)
            { //fullstrip, no-yzoom
                tileYLoopNozoomY(&sParams,offs,sy,my,cliprect);
              //
             //   tileYLoopNozoomY(&sParams,offs,sy,my,cliprect);
            } else
            { // fullstrip, yzoom
                tileYLoopZoomY<1>(&sParams,offs,sy,my,zy,cliprect);
  // tileYLoopNozoomY(&sParams,offs,sy,my,cliprect);
            }

        } // end full mode vstrip



                // if(nbt == dbg_nbt)
                // {
                //     int by = sy& 0x1ff;
                //     if(by<cliprect->min_y) by = cliprect->min_y;
                //     if(by>cliprect->max_y-5 ) by=cliprect->max_y-5;
                //     {
                //     // for(int hy=sy;hy<sy+16;hy++)
                //     //     for(int hx=sx;hx<sx+16;hx++)
                //     //         line[hy+2][hx+2]=4095;
                //     for(int hy=by;hy<by+5;hy++)
                //     for(int hx=sx;hx<sx+5;hx++)
                //             line[hy][hx]=1;
                //     }
                // }
                //  nbt++;


    }  /* for count */




}
