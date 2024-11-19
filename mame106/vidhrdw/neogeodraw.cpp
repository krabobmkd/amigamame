#include "neogeodraw.h"
extern "C" {
    #include "drawgfx.h"
    extern UINT16 *neogeo_vidram16;
    extern int high_tile;
    extern int vhigh_tile;
    extern int vvhigh_tile;
    extern unsigned int neogeo_frame_counter;
    extern UINT8 *neogeo_memory_region_gfx3;
    extern char zoomx_draw_tables[16][16];
}

#if defined(__GNUC__) && defined(__AMIGA__)
#define REGNG(r) __asm(#r)
    //#define REGM(r)
#else
#define REGNG(r)
#endif


    int tutute= 0;

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
static char dda_y_skip[18];
static char full_y_skip[16]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

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

template<class PixelWriter,
         char j0,char j1,char j2,char j3,char j4,char j5,char j6,char j7,
         char j8,char j9,char j10,char j11,char j12,char j13,char j14,char j15>
void NeoDrawGfx16zx(INT16 dy,INT16 sy,INT16 ey,INT16 sx,UINT16 ipalette,
                  UINT8 *fspr,
                  UINT16 **line,
                  char *l_y_skip)
{
    for (INT16 y = sy ;y <= ey;y++)
    {
        UINT16 *bm  = ((unsigned short *)line[y]) + sx;
        fspr+=(*l_y_skip++)*dy;
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

    }
}

typedef struct _NeoDrawGfxParams {
    UINT8 *fspr;
    UINT16 **line;
    char *l_y_skip ;
    INT32 dy;
    INT32 sy;
    INT32 ey;
    INT32 sx;
    UINT16 zx;
    UINT16 ipalette;
    int flipx;
} sNeoDrawGfxParams;

template<class PixelWriter>
void NeoDrawGfx16(sNeoDrawGfxParams *p REGNG(a0))
{
    UINT8 *fspr = p->fspr;
    UINT16 **line = p->line;
    char *l_y_skip = p->l_y_skip ;
    UINT16 ipalette = p->ipalette;
    INT32 dy = p->dy;
    INT32 sy = p->sy;
    INT32 ey = p->ey;
    INT32 sx = p->sx;

    if (p->flipx)	/* X flip */
    {
        if(p->zx==15)
        {
            for (INT16 y = p->sy;y <= p->ey;y++)
            {
                UINT16 *bm  = p->line[y]+p->sx;
                fspr+=(*l_y_skip++)*dy;

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
            }
        }
        else
        {   // zoomx + flipx
            char *zoomx_draw = zoomx_draw_tables[p->zx];
            for (INT16 y = sy;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y])+sx;
                fspr+=(*l_y_skip++)*dy;
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

            }
        }
    }
    else		/* normal */
    {
        switch(p->zx)
        {
        case 15:
            for (INT16 y = sy ;y <= ey;y++)
            {
                UINT16 *bm  = ((unsigned short *)line[y]) + sx;
                fspr+=(*l_y_skip++)*dy;

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
        case 0:NeoDrawGfx16zx<PixelWriter,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 1:NeoDrawGfx16zx<PixelWriter,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 2:NeoDrawGfx16zx<PixelWriter,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 3:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 4:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 5:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 6:NeoDrawGfx16zx<PixelWriter,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 7:NeoDrawGfx16zx<PixelWriter,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 8:
        {
            tutute++;
        }
            NeoDrawGfx16zx<PixelWriter,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 9:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 10:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 11:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 12:NeoDrawGfx16zx<PixelWriter,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 13:NeoDrawGfx16zx<PixelWriter,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
            break;
        case 14:NeoDrawGfx16zx<PixelWriter,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1>
                (dy,sy,ey,sx,ipalette,fspr,line,l_y_skip);
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


void neogeo_drawTilesSprites( mame_bitmap *bitmap, const rectangle *cliprect)
{
    /* Draw sprites */
    int sx =0,sy =0,oy =0,my =0,zx = 15, rzy = 1;
    int offs,i,count,y,x;

    int t1,t2,t3;

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

        t1 = neogeo_vidram16[(0x10400 >> 1) + count];
        t3 = neogeo_vidram16[(0x10000 >> 1) + count];
        // if(count == 90)
        // {
        //     static int yyy=0;
        //     yyy++;
        // }
        /* If this bit is set this new column is placed next to last one */
        if (t1 & 0x40) {
            sx += zx+1;
            if ( sx >= 0x1F0 )
                sx -= 0x200;

            /* Get new zoom for this column */
            zx = (t3 >> 8) & 0x0f;
            sy = oy;
        } else {	/* nope it is a new block */
            t2 = neogeo_vidram16[(0x10800 >> 1) + count];

            /* Sprite scaling */
            zx = (t3 >> 8) & 0x0f;
            rzy = t3 & 0xff;

            sx = (t2 >> 7);
            if ( sx >= 0x1F0 )
                sx -= 0x200;

            /* Number of tiles in this strip */

            my = t1 & 0x3f;
            // was v37
            // if (my == 0x20) fullmode = 1;
            // else if (my >= 0x21) fullmode = 2;	/* most games use 0x21, but */
            // /* Alpha Mission II uses 0x3f */
            // else fullmode = 0;

            sy = 0x200 - (t1 >> 7);
            if (cliprect->max_y - cliprect->min_y > 8 ||	/* kludge to improve the ssideki games */
                cliprect->min_y == Machine->visible_area.min_y)
            {
                if (sy > 0x110) sy -= 0x200;
                if (fullmode == 2 || (fullmode == 1 && rzy == 0xff))
                {
                    while (sy < 0) sy += 2 * (rzy + 1);
                }
            }
            oy = sy;

            // if (rzy < 0xff && my < 0x10 && my)
            // {
            //     my = (my*256)/(rzy+1);
            //     if (my > 0x10) my = 0x10;
            // }
            // if (my > 0x20) my=0x20;

            //-------------------------
             // try v106 things...
                if (my > 0x20)
                {
                    my = 0x20;
                    fullmode = 1;
                }
                else
                    fullmode = 0;

                if (rzy < 0xff && my < 0x10 && my)
                {
                    my = ((my*256)/(rzy+1));
                    if (my > 0x10) my = 0x10;
                }

            ddax=0;	/* =16; NS990110 neodrift fix */		/* setup x zoom */
        }

        /* No point doing anything if tile strip is 0 */
        if (my==0) continue;

        if(sx>=320 ) continue;

        /* Setup y zoom */
        if(rzy==255)
            yskip=16;
        else
            dday=0;	/* =256; NS990105 mslug fix */

        offs = count<<6;

        /* my holds the number of tiles in each vertical multisprite block */
        for (y=0; y < my ;y++) {
            UINT32 tileno	= neogeo_vidram16[offs];
            offs++;
            UINT16 tileatr = neogeo_vidram16[offs];
            offs++;

            /*old
            if (high_tile && tileatr&0x10) tileno+=0x10000;
            if (vhigh_tile && tileatr&0x20) tileno+=0x20000;
            if (vvhigh_tile && tileatr&0x40) tileno+=0x40000;
            */
            // fast test of tile offsets bits 0x0070.
            tileno += ((UINT32)tileatr & tileOffsetFilter)<<12;

            // v037
            //if (tileatr&0x8) tileno=(tileno&~7)+((tileno+neogeo_frame_counter)&7);
            //else if (tileatr&0x4) tileno=(tileno&~3)+((tileno+neogeo_frame_counter)&3);

            // v106
            if (tileatr & 0x08) tileno=(tileno&~7)|(neogeo_frame_counter&7);	/* fixed */
            else if (tileatr & 0x04) tileno=(tileno&~3)|(neogeo_frame_counter&3);	/* fixed */

            tileno = tileno % no_of_tiles; // safety.
            //if(tileno>=no_of_tiles) continue; // less divs

            if (fullmode == 2 || (fullmode == 1 && rzy == 0xff))
            {
                if (sy >= 248) sy -= 2 * (rzy + 1);
            }
            else if (fullmode == 1)
            {
                if (y == 0x10) sy -= 2 * (rzy + 1);
            }
            else if (sy > 0x110) sy -= 0x200;	/* NS990105 mslug2 fix */

            if(rzy!=255)
            {
                yskip=0;
                dda_y_skip[0]=0;
                for(i=0;i<16;i++)
                {
                    dda_y_skip[i+1]=0;
                    dday-=rzy+1;
                    if(dday<=0)
                    {
                        dday+=256;
                        yskip++;
                        dda_y_skip[yskip]++;
                    }
                    else dda_y_skip[yskip]++;
                }
            }
            UINT32 penusage = gfx->pen_usage[tileno];

            if ( /*((gfx->pen_usage[tileno] & ~1) != 0) &&*/ // if not fully transparent.
                (sy+15 >= cliprect->min_y && sy <= cliprect->max_y)
                )
            {
                INT32 zy = yskip;
                /*UINT16 **line,
                    const gfx_element *gfx,
                     UINT32 code,UINT32 color,int flipx,int flipy,INT16 sx,INT16 sy,
                     INT16 zx,INT16 zy,const rectangle *clip*/

                /*line,
                      gfx,
                      tileno,
                      tileatr >> 8,  color
                      tileatr & 0x01 flipx
                      tileatr & 0x02, flipy
                      sx,sy,zx,yskip,
                      cliprect*/
                // = = = = prepare y things = = = =
                INT32 syy = sy; // clipped
                INT32 ooy = sy;
                INT32 ey = syy + zy -1; 	/* Clip for size of zoomed object */

                if (syy < cliprect->min_y) syy = cliprect->min_y;
                if (ey >= cliprect->max_y) ey = cliprect->max_y;

                if(zy==16)
                    sParams.l_y_skip=full_y_skip;
                else
                    sParams.l_y_skip=dda_y_skip;

                UINT8 *fspr = (UINT8 *)neogeo_memory_region_gfx3;

                fspr += tileno*(128);

                INT16 dy;
                if (/*flipy*/tileatr & 0x02)	/* Y flip */
                {
                    dy = -8;
                    fspr+=128 - 8 - (syy-ooy)*8;
                }
                else		/* normal */
                {
                    dy = 8;
                    fspr+=(syy-ooy)*8;
                }


                sParams.dy = dy;
                sParams.fspr = fspr;

                sParams.ipalette = (tileatr >> 4 & 0x0ff0); // (tileatr >> 8) * gfx->color_granularity

                sParams.sy = syy;
                sParams.ey = ey;
                sParams.sx = sx;
                sParams.zx = zx;
                sParams.flipx = tileatr & 0x01;
                /*typedef struct _NeoDrawGfxParams {
                    UINT8 *fspr;
                    UINT16 **line;
                    char *l_y_skip ;
                    INT16 dy;
                    INT16 sy;
                    INT16 ey;
                    INT16 sx;
                    UINT16 zx;
                    UINT16 ipalette;
                    int flipx;
                } sNeoDrawGfxParams;  */

                if(nbt == 370)
                {
                    static int yyy=0;
                    yyy++;
                }
                if(nbt == dbg_nbt)
                {


                    // for(int hy=sy;hy<sy+16;hy++)
                    //     for(int hx=sx;hx<sx+16;hx++)
                    //         line[hy+2][hx+2]=4095;
                    for(int hy=sy;hy<sy+5;hy++)
                    for(int hx=sx;hx<sx+5;hx++)
                            line[hy][hx]=15;
                    nbt++;
                    sy +=yskip;
                    continue;
                }
                nbt++;
                if(sx>-16)
                {
                    if((penusage & 1)==0)
                    {   // fully opaque
                        NeoDrawGfx16<neoPixelWriter_Opaque>(&sParams
                            //dy,sy,ey,sx,zx,ipalette,tileatr & 0x01/*flipx*/,fspr,line,l_y_skip
                            );

                    } else
                    {
                        NeoDrawGfx16<neoPixelWriter_Transparent0>(&sParams
                            //dy,sy,ey,sx,zx,ipalette,tileatr & 0x01/*flipx*/,fspr,line,l_y_skip
                            );
                    }
                }


            } // end if draw ok.

            sy +=yskip;
        }  /* for y sprites */
    }  /* for count */




}
