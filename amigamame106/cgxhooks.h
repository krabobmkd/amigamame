#ifndef CGXHOOKS_RE_H
#define CGXHOOKS_RE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>



#define	CGXHOOK_WRITECLUT8		0
#define	CGXHOOK_COLORIMPOSE		1
#define	CGXHOOK_BITMAPIMPOSE	2

struct CGXHookMsg
{
	struct Layer    *Layer;
	struct Rectangle Bounds;
	LONG             OffsetX;
	LONG             OffsetY;
};

struct CGXHook
{
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
	//ULONG				Remap[256];
};

void  FreeCGXHook(struct CGXHook *hook );

struct CGXHook *  AllocRGB16Hook(struct RastPort *rp);

void  DoRGB16Hook(struct CGXHook *hook,
							   UBYTE *chunky, struct RastPort *rp ,
							   LONG srcx, LONG srcy ,
							   LONG dstx , LONG dsty ,
							   LONG width , LONG height ,
							   LONG srcmod );

//void  CustomDrawPixelRGB16Hook(struct CGXHook *hook );

#ifdef __cplusplus
}
#endif

#endif
