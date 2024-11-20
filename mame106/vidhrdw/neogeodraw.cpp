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
//static char dda_x_skip[16];
// static char dda_y_skip[18];
// static char full_y_skip[16]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

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
//?    char *l_y_skip ;
    INT32 dy;
    INT32 sy;
    INT32 ey;
    INT32 sx;
    UINT16 zx;
    UINT16 ipalette;
    int flipx;
} sNeoDrawGfxParams;









template<class PixelWriter>
void NeoDrawGfx16line(sNeoDrawGfxParams *p REGNG(a0))
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
        if(p->zx==15)
        {
            for (INT16 y = sy;y <= ey;y++)
            {
                UINT16 *bm  = p->line[y]+p->sx;
                UINT32 *fspr4 = (UINT32*)fspr;
                UINT32 v = *fspr4++;
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
        }
        else
        {   // zoomx + flipx
            char *zoomx_draw = zoomx_draw_tables[p->zx];
            for (INT16 y = sy;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y])+sx;
                UINT8 *fsprb = fspr+7;

                UINT8 c = *fsprb--;
                if (zoomx_draw[0]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[1]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[2]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[3]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[4]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[5]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[6]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[7]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }

                c = *fsprb--;
                if (zoomx_draw[8]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[9]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[10]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[11]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[12]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[13]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb;
                if (zoomx_draw[14]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[15]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }

                fspr+=dy;
            }
        }
    }
    else		/* normal */
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
        /*      else
        {
            char *zoomx_draw = zoomx_draw_tables[zx];
            for (INT16 y = sy ;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y]) + sx;
                fspr+=(*l_y_skip++)*dy;
                UINT8 *fsprb = (UINT8 *)fspr;

                UINT8 c = *fsprb++;


                if (zoomx_draw[0]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[1]) { UINT8 col=c>>4;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[2]) { UINT8 col=c&0x0f;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[3]) { UINT8 col=c>>4;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[4]) { UINT8 col=c&0x0f;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[5]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[6]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[7]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }

                c = *fsprb++;
                if (zoomx_draw[8]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[9]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[10]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[11]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[12]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[13]) { UINT8 col=c>>4;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb;
                if (zoomx_draw[14]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[15]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }

            }
        }*/
    } // end no flip x

}






















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
        UINT16 *bm  = ((unsigned short *)line[y]) + sx;
        UINT8 *fsprb = (UINT8 *)fspr;

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
        if (j15) { PixelWriter::setpixel(ipalette,c>>4,bm); bm++; }

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
        if(p->zx==15)
        {
            for (INT16 y = sy;y <= ey;y++)
            {
                UINT16 *bm  = p->line[y]+p->sx;
                UINT32 *fspr4 = (UINT32*)fspr;
                UINT32 v = *fspr4++;
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
        }
        else
        {   // zoomx + flipx
            char *zoomx_draw = zoomx_draw_tables[p->zx];
            for (INT16 y = sy;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y])+sx;
                UINT8 *fsprb = fspr+7;

                UINT8 c = *fsprb--;
                if (zoomx_draw[0]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[1]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[2]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[3]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[4]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[5]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[6]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[7]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }

                c = *fsprb--;
                if (zoomx_draw[8]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[9]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[10]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[11]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb--;
                if (zoomx_draw[12]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[13]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb;
                if (zoomx_draw[14]) { UINT8 col=c>>4;  PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[15]) { UINT8 col=c&0x0f;  PixelWriter::setpixel(ipalette,col,bm); bm++; }

                fspr+=dy;
            }
        }
    }
    else		/* normal */
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
  /*      else
        {
            char *zoomx_draw = zoomx_draw_tables[zx];
            for (INT16 y = sy ;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y]) + sx;
                fspr+=(*l_y_skip++)*dy;
                UINT8 *fsprb = (UINT8 *)fspr;

                UINT8 c = *fsprb++;


                if (zoomx_draw[0]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[1]) { UINT8 col=c>>4;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[2]) { UINT8 col=c&0x0f;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[3]) { UINT8 col=c>>4;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[4]) { UINT8 col=c&0x0f;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[5]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[6]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[7]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }

                c = *fsprb++;
                if (zoomx_draw[8]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[9]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[10]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[11]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb++;
                if (zoomx_draw[12]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[13]) { UINT8 col=c>>4;PixelWriter::setpixel(ipalette,col,bm); bm++; }
                c = *fsprb;
                if (zoomx_draw[14]) { UINT8 col=c&0x0f; PixelWriter::setpixel(ipalette,col,bm); bm++; }
                if (zoomx_draw[15]) { UINT8 col=c>>4; PixelWriter::setpixel(ipalette,col,bm); bm++; }

            }
        }*/
    } // end no flip x

}

template<int fullmode>
static inline void tileYLoopZoomY(
    sNeoDrawGfxParams *p REGNG(a0),
    UINT32 offs REGNG(d0),INT16 sy REGNG(d1),INT32 my REGNG(d2),UINT16 zy REGNG(d3),
    const rectangle *cliprect REGNG(a1)
    )
{
    const gfx_element *gfx=Machine->gfx[2]; /* Save constant struct dereference */
    UINT8 *fspr = (UINT8 *)neogeo_memory_region_gfx3;
    /* get pointer to table in zoom ROM (thanks to Miguel Angel Horna for the info) */
    UINT8 *zoomy_rom = memory_region_gfx4 + (zy << 8);
    INT16 drawn_lines = 0;

    const INT16 miny = (INT16)cliprect->min_y;
    const INT16 maxy = (INT16)cliprect->max_y;
    const sx = p->sx;
    // persistent
    UINT16 tile=16; // can only be 0,15
    UINT16 tileatr;
    /* my holds the number of tiles in each vertical multisprite block */
    while (drawn_lines < my*0x10)
    {
        INT16 yy = (sy + drawn_lines) & 0x1ff;

        if (yy >= miny && yy <= maxy)
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

                    if (zy) // fullmode apply
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

                if (drawn_lines & 0x100)
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

                // yoffs = zoomy_rom[zoom_line] & 0x0f;
                // tile = zoomy_rom[zoom_line] >> 4;

                // // if (invert)
                // // {
                // //     tile ^= 0x1f;
                // //     yoffs ^= 0x0f;
                // // }
                // tile ^= 0x1f*invert;
                // yoffs ^= 0x0f*invert;


            } // end no full mode (99% cases)

            // yoffs = zoomy_rom[zoom_line] & 0x0f;

            // tile = zoomy_rom[zoom_line] >> 4;

            // // if (invert)
            // // {
            // //     tile ^= 0x1f;
            // //     yoffs ^= 0x0f;
            // // }
            if(newtile != tile)
            {
                tile = newtile;
                tileno	= neogeo_vidram16[offs+2*tile];
                tileatr = neogeo_vidram16[offs+2*tile+1];

                tileno += ((UINT32)tileatr & tileOffsetFilter)<<12;

                if (tileatr & 0x08) tileno=(tileno&~7)|(neogeo_frame_counter&7);	/* fixed */
                else if (tileatr & 0x04) tileno=(tileno&~3)|(neogeo_frame_counter&3);	/* fixed */
            }

//todo: needed params would be:
       //      UINT16 *bm = line[yy]+sx
       //      fspr + (tileno*128) + yoffs
       //      tileatr -> give color, flipx

       // //     if (tileatr & 0x02) yoffs ^= 0x0f;	/* flip y */

       //      NeoMVSDrawGfxLine(line[yy]+sx,
       //                     //   gfx,
       //                        tileno,
       //                        tileatr >> 8,
       //                        tileatr & 0x01,	/* flip x */
       //                        sx,yy,zx,yoffs,
       //                        cliprect
       //                        );
        }

        drawn_lines++;
    }  /* for y */
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
    // here sy is [0,511]
    if(sy>240) sy -=512; // does the wrap thing.
    const gfx_element *gfx=Machine->gfx[2]; /* Save constant struct dereference */
    // clip tiles UP completely off, fast.
    if(sy<=cliprect->min_y-16) {
       INT32 d= (cliprect->min_y-sy)>>4;
        offs += d*2;
        my -= d;
        sy += d<<4;
    }


    INT16 syy=sy;
    INT16 ey = sy + 15; 	/* Clip for size of zoomed object */

    if (syy < cliprect->min_y) syy = cliprect->min_y;

    UINT8 *fspr = (UINT8 *)neogeo_memory_region_gfx3;
    /* my holds the number of tiles in each vertical multisprite block */
    for(;my>0;my--) {
        UINT32 tileno	= (UINT32)neogeo_vidram16[offs];
        offs++;
        UINT16 tileatr = neogeo_vidram16[offs];
        offs++;
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

            if (ey > cliprect->max_y) ey = cliprect->max_y;

            if (/*flipy*/tileatr & 0x02)
            {
                p->dy = -8;
//                fspr+=128 - 8 - (syy-sy)*8;
                p->fspr = fspr + (tileno*128) + 128 - 8 - (syy-sy)*8;
            }
            else		/* normal */
            {
                p->dy = 8;
                p->fspr = fspr + (tileno*128) + (syy-sy)*8;
                //fspr+=(syy-sy)*8;
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
        if( sy > cliprect->max_y) break;
        syy=sy;
        ey = sy + 15;
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

    int nbt = 0;
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

            if (my==0) continue; // fast exit when sprite not used.

            if (my > 0x20)
            {
                my = 0x20;

                if (sy < 248)
                    fullmode = 1;
                else
                    /* kludge to avoid a white line in KOF94 Japan stage... */
                    /* probably indication of a subtle bug somewhere else */
                    fullmode = 0;
            }
            else
                fullmode = 0;
           		/* setup x zoom */
        }

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
            } else
            { // no-fullstrip, yzoom
                tileYLoopZoomY(&sParams,offs,sy,my,zy,cliprect);
            //  tileYLoopNozoomY(&sParams,offs,sy,my,cliprect);
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
                 tileYLoopZoomY(&sParams,offs,sy,my,zy,cliprect);
  // tileYLoopNozoomY(&sParams,offs,sy,my,cliprect);
            }

        } // end full mode vstrip

/*

                if(nbt == dbg_nbt)
                {
                    int by = sy& 0x1ff;
                    if(by<cliprect->min_y) by = cliprect->min_y;
                    if(by>cliprect->max_y-5 ) by=cliprect->max_y-5;
                    {
                    // for(int hy=sy;hy<sy+16;hy++)
                    //     for(int hx=sx;hx<sx+16;hx++)
                    //         line[hy+2][hx+2]=4095;
                    for(int hy=by;hy<by+5;hy++)
                    for(int hx=sx;hx<sx+5;hx++)
                            line[hy][hx]=1;
                    }
                }
                 nbt++;
*/

    }  /* for count */




}
