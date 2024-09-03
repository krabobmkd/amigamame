#include "drawgfxn.h"

/*
    krb: redo some of drawgfx.c
    the idea is: more template, less useless switches.
*/

template<typename DESTPIX,bool isopaque,bool isPack4, bool prio>
void drawgfxT(struct drawgfxParams *p DGREG(a0))
{
    // - - - - from drawgfx()
    mame_bitmap *dest = p->dest;
    const gfx_element *gfx = p->gfx;
    int flipx = p->flipx;
    int flipy = p->flipy;
    int sx = p->sx;
    int sy = p->sy;
    UINT32 pri_mask = p->priority_mask;
    int transparent_color = p->transparent_color;

    const rectangle *clip = p->clip;

	int ox;
	int oy;
	int ex;
	int ey;

	/* check bounds */
	ox = p->sx;
	oy = p->sy;

	ex = sx + p->gfx->width-1;
	if (sx < 0) sx = 0;
	if (clip && sx < clip->min_x) sx = clip->min_x;
	if (ex >= dest->width) ex = dest->width-1;
	if (clip && ex > clip->max_x) ex = clip->max_x;
	if (sx > ex) return;

	ey = sy + gfx->height-1;
	if (sy < 0) sy = 0;
	if (clip && sy < clip->min_y) sy = clip->min_y;
	if (ey >= dest->height) ey = dest->height-1;
	if (clip && ey > clip->max_y) ey = clip->max_y;
	if (sy > ey) return;


        const unsigned int code = p->code;
        unsigned int color = p->color;

		UINT8 *srcdata = gfx->gfxdata + code * gfx->char_modulo;		/* source data */
		int srcwidth = gfx->width;									/* source width */
		int srcheight = gfx->height;									/* source height */
		int srcmodulo = gfx->line_modulo;								/* source modulo */
		int leftskip = sx-ox;											/* left skip */
		int topskip = sy-oy;											/* top skip */
		DESTPIX *dstdata = ((DESTPIX *)dest->line[sy]) + sx;		/* dest data */
		int dstwidth = ex-sx+1;										/* dest width */
		int dstheight = ey-sy+1;										/* dest height */
		int dstmodulo = ((DESTPIX *)dest->line[1])-((DESTPIX *)dest->line[0]);	/* dest modulo */
		const pen_t *paldata = &gfx->colortable[gfx->color_granularity * color];
		UINT8 *pribuf = (p->pri_buffer) ? ((UINT8 *)p->pri_buffer->line[sy]) + sx : NULL;

        int transparency = p->transparency;

    // - - - - -BLOCKMOVEXXXX
    /*
#define COMMON_ARGS
		const UINT8 *srcdata,int srcwidth,int srcheight,int srcmodulo,
		int leftskip,int topskip,int flipx,int flipy,
		DATA_TYPE *dstdata,int dstwidth,int dstheight,int dstmodulo
BLOCKMOVELU(4toN_opaque,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata));
   */
    // ADJUST_8
	int ydir;
	if (flipy)
	{
		INCREMENT_DST(VMODULO * (dstheight-1))
		srcdata += (srcheight - dstheight - topskip) * srcmodulo;
		ydir = -1;
	}
	else
	{
		srcdata += topskip * srcmodulo;
		ydir = 1;
	}
	if (flipx)
	{
		INCREMENT_DST(HMODULO * (dstwidth-1))
		srcdata += (srcwidth - dstwidth - leftskip);
	}
	else
	{
		srcdata += leftskip;
	}

	srcmodulo -= dstwidth;



//    switch (transparency)
//    {
//        case TRANSPARENCY_NONE:
//            if (gfx->flags & GFX_PACKED)
//            {
//                if (pribuf)
//                    BLOCKMOVEPRI(4toN_opaque,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata,pribuf,pri_mask));
//                else
//                    BLOCKMOVELU(4toN_opaque,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata));
//            }
//            else
//            {
//                if (pribuf)
//                    BLOCKMOVEPRI(8toN_opaque,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata,pribuf,pri_mask));
//                else
//                    BLOCKMOVELU(8toN_opaque,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata));
//            }
//            break;

//        case TRANSPARENCY_PEN:
//            if (gfx->flags & GFX_PACKED)
//            {
//                if (pribuf)
//                    BLOCKMOVEPRI(4toN_transpen,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata,pribuf,pri_mask,transparent_color));
//                else
//                    BLOCKMOVELU(4toN_transpen,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata,transparent_color));
//            }
//            else
//            {
//                if (pribuf)
//                    BLOCKMOVEPRI(8toN_transpen,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata,pribuf,pri_mask,transparent_color));
//                else
//                    BLOCKMOVELU(8toN_transpen,(sd,sw,sh,sm,ls,ts,flipx,flipy,dd,dw,dh,dm,paldata,transparent_color));
//            }
//            break;

//    }



}
// parametrization: transp/opaque ,  PIXPACKED4/PIX8 , prio/noprio


void drawgfx_clut16_T(struct drawgfxParams *p DGREG(a0))
{
    drawgfxT<UINT16,false,true,false>(p);
}	
void drawgfx_clut16_O(struct drawgfxParams *p DGREG(a0))
{
    drawgfxT<UINT16,true,true,false>(p);
}
