//#include <stdio.h>

#include "cgxhooks.h"
#include <proto/cybergraphics.h>
#include <proto/exec.h>

#include <cybergraphx/cybergraphics.h>

#include <stdio.h>

extern struct Library       *CyberGfxBase;

int nbh=0;
int nbhs=0;
void  FreeCGXHook(struct CGXHook *hook )
{
    //printf("FreeCGXHook\n");
    if(hook) FreeVec(hook);
}

int rgb16entry(register struct CGXHook *hook __asm("a0") ,
                       register void *object __asm("a2"),
                       register struct CDrawMsg *pDrawMsg  __asm("a1"))
{

    nbh++;
    return 0;
}
int rgb16subentry()
{
    nbhs++;
    return 0;
}

struct CGXHook *  AllocRGB16Hook(struct RastPort *rp )
{
    // test bitmap is cgx
    if( !CyberGfxBase ||
        !rp ||
        !rp->BitMap ||
        !GetCyberMapAttr(rp->BitMap,CYBRMATTR_ISCYBERGFX)) return NULL;

        printf("AllocRGB16Hook bm:%08x\n",(int)rp->BitMap);

    struct CGXHook *h = (struct CGXHook *) AllocVec(sizeof(struct CGXHook), MEMF_CLEAR);
    if(!h) return NULL;
//    printf("AllocRGB16RemapHook\n");

    h->PixFmt = GetCyberMapAttr(rp->BitMap,CYBRMATTR_PIXFMT);
    h->BytesPerPixel = GetCyberMapAttr(rp->BitMap,CYBRMATTR_BPPIX);
        /* struct Hook
    struct MinNode h_MinNode;
    ULONG	   (*h_Entry)();	 assembler entry point
    ULONG	   (*h_SubEntry)();	 often HLL entry point
    APTR	   h_Data;		 owner specific
        */
   h->Hook.h_Entry = rgb16entry;
   h->Hook.h_SubEntry = rgb16subentry;
    /*
    struct Hook			Hook;
	ULONG				Type;
	struct Library		*SysBase;
	struct Library		*GfxBase;
	struct Library		*LayersBase;
	struct Library		*CyberGfxBase;
	ULONG				BytesPerPixel;
	ULONG				PixFmt;
	struct Screen		*Screen;
	UBYTE				*Source;
	ULONG				Data;
	ULONG				SrcMod;
	ULONG				SrcX;
	ULONG				SrcY;
	ULONG				DstX;
	ULONG				DstY;
	ULONG				Remap[256];
*/


    return h;
}

void  DoRGB16Hook(struct CGXHook *hook,
							   UBYTE *chunky, struct RastPort *rp ,
							   LONG srcx, LONG srcy ,
							   LONG dstx , LONG dsty ,
							   LONG width , LONG height ,
							   LONG srcmod )
{

    nbh=0;
    nbhs=0;
    if( !CyberGfxBase || !hook ) return;
   // printf("DoRGB16Hook bm:%08x\n",(int)rp->BitMap);
    APTR hdl = LockBitMapTags(rp->BitMap,TAG_DONE,0);
    if(!hdl) return;

//    printf("would do \n");
    DoCDrawMethodTags((struct Hook *)hook,rp,TAG_DONE,0);

    UnLockBitMap(hdl);
    printf("nbh:%d nbhs:%d\n",nbh,nbhs);

}

