// segaic16n.cpp complements the .c version

extern "C"
{
#include "driver.h"
#include "mame.h"
#include "segaic16.h"
#include "vidhrdw/res_net.h"
#include <stdio.h>
#include <stdlib.h>
}
#ifdef __AMIGA__
#define FINLINE __attribute__((always_inline))
#else
#define FINLINE
#endif



extern "C" {
// publish C++ functions as "C"
void segaic16_sprites_outrun_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect);
void segaic16_sprites_thndrbld_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect);
void segaic16_sprites_sharrier_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect);
void segaic16_sprites_yboard_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect);

extern struct palette_info palette;
extern struct rotate_info segaic16_ybd_rotate[SEGAIC16_MAX_ROTATE];
}

/*******************************************************************************************
 *
 *  Out Run/X-Board-style sprites
 *
 *      Offs  Bits               Usage
 *       +0   e------- --------  Signify end of sprite list
 *       +0   -h-h---- --------  Hide this sprite if either bit is set
 *       +0   ----bbb- --------  Sprite bank
 *       +0   -------t tttttttt  Top scanline of sprite + 256
 *       +2   oooooooo oooooooo  Offset within selected sprite bank
 *       +4   ppppppp- --------  Signed 7-bit pitch value between scanlines
 *       +4   -------x xxxxxxxx  X position of sprite (position $BE is screen position 0)
 *       +6   -s------ --------  Enable shadows
 *       +6   --pp---- --------  Sprite priority, relative to tilemaps
 *       +6   ------vv vvvvvvvv  Vertical zoom factor (0x200 = full size, 0x100 = half size, 0x300 = 2x size)
 *       +8   y------- --------  Render from top-to-bottom (1) or bottom-to-top (0) on screen
 *       +8   -f------ --------  Horizontal flip: read the data backwards if set
 *       +8   --x----- --------  Render from left-to-right (1) or right-to-left (0) on screen
 *       +8   ------hh hhhhhhhh  Horizontal zoom factor (0x200 = full size, 0x100 = half size, 0x300 = 2x size)
 *       +E   dddddddd dddddddd  Scratch space for current address
 *
 *    Out Run only:
 *       +A   hhhhhhhh --------  Height in scanlines - 1
 *       +A   -------- -ccccccc  Sprite color palette
 *
 *    X-Board only:
 *       +A   ----hhhh hhhhhhhh  Height in scanlines - 1
 *       +C   -------- cccccccc  Sprite color palette
 *
 *******************************************************************************************/

//#define outrun_draw_pixel() 												\
//	/* only draw if onscreen, not 0 or 15 */								\
//	if (x >= cliprect->min_x && x <= cliprect->max_x && pix != 0 && pix != 15) \
//	{																		\
//		/* are we high enough priority to be visible? */					\
//		if (sprpri > pri[x])												\
//		{																	\
//			/* shadow/hilight mode? */										\
//			if (shadow && pix == 0xa)										\
//				dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;	\
//																			\
//			/* regular draw */												\
//			else															\
//				dest[x] = pix | color;										\
//		}																	\
//																			\
//		/* always mark priority so no one else draws here */				\
//		pri[x] = 0xff;														\
//	}																		\

template<bool shadow,bool highlight,bool test15,bool testxmin,bool testxmax> FINLINE
 void outrun_draw_pixelT(UINT16 *dest,UINT8 *pri,const int sprpri, int x , const rectangle *cliprect,
    int pix, int color )
{
	if (    pix != 0 &&
            (!testxmin || (x >= cliprect->min_x)) &&
            (!testxmax || (x <= cliprect->max_x)) &&
           (!test15 || pix != 15))
	{
		/* are we high enough priority to be visible? */
		if (sprpri > pri[x])
		{
			/* shadow/hilight mode? */
			if (shadow && pix == 0xa)
			{
                // dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;
    			if(highlight)
    			{
        			dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;
    			} else dest[x] |= palette.entries;
            }
			/* regular draw */
			else
				dest[x] = pix | color;
		}
		/* always mark priority so no one else draws here */
		pri[x] = 0xff;
	}
}

struct yloopprms {
    mame_bitmap *bitmap;
    const rectangle *cliprect;
    const UINT32 *spritedata;
    int top; int ytarget; int ydelta;
    const int hzoom;
    const int vzoom;
    const int pitch;
    int addr;
    int xpos;
    int sprpri;
    int color;

};

template<bool shadow,bool doclipx,int deltax,bool doextra15> FINLINE
void  outrun_sprite_loopy_noflip(
        mame_bitmap *bitmap,
        const rectangle *cliprect,
        const UINT32 *spritedata,
        int top,int ytarget,int ydelta,
        const int hzoom,const int vzoom,const int pitch,int addr,int xpos,int sprpri,int color)
{
    int yacc = 0;

    const bool notc15=false;
     const bool yestc15=true;
     const bool nohlight=false;

    // compute silly xstart with 2 first pixels that are always transparent
    int xjmp = (hzoom>>8); // >>10 <<1 for 2 pixels
    if(deltax>0)
        xpos+= xjmp;
    else  xpos -= xjmp;
    // let xacc to zero, who gives ?
    int xacc_startx = (hzoom*xjmp) & 0x1ff;

    for (int y = top; y != ytarget; y += ydelta)
    {
        // skip drawing if not within the cliprect
        if (y >= cliprect->min_y && y <= cliprect->max_y)
        {
            UINT16 *dest = (UINT16 *)bitmap->line[y];
            UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
            int xacc = 0;

            // start at the word before because we preincrement below
            UINT16 data7 = addr;

            int x = xpos;
// hzoom 0x200=1 ?
         //   if (x >= cliprect->min_x
            UINT32 pixels = spritedata[data7++];
            // first byte is obviously unused.
            // it's always 0,15, the end code for reverse.

            // the following 2 jumps has been replaced by xjmp.
            //while (xacc < 0x200) { x++; xacc += hzoom; } xacc -= 0x200;
            //while (xacc < 0x200) { x++; xacc += hzoom; } xacc -= 0x200;

           //
           xacc = xacc_startx;

            for (;
                (deltax > 0 && x <= cliprect->max_x) || // tests hopefully crunched by template.
                (deltax < 0 && x >= cliprect->min_x)
            ; )
            {
                // draw 8 pixels, start at pixel 3
                int pix;

                pix = (pixels >> 20) & 0xf; while (xacc < 0x200) {
                        outrun_draw_pixelT<shadow,nohlight,doextra15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                         x+=deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 16) & 0xf; while (xacc < 0x200) {
                        outrun_draw_pixelT<shadow,nohlight,doextra15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                          x+=deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 12) & 0xf; while (xacc < 0x200) {
                        outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                          x+=deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >>  8) & 0xf; while (xacc < 0x200) {
                        outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                          x+=deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >>  4) & 0xf;
 if(pix == 0x0f) break; // real stop case.
                while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                      x+=deltax; xacc += hzoom; } xacc -= 0x200;

                pix = (pixels >>  0) & 0xf; while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                     x+=deltax; xacc += hzoom; } xacc -= 0x200;
                // actual end of the 8 pixels slot here, 6 -> 2 of the next if not exited.

                pixels = spritedata[data7++];
                // start of the next
                pix = (pixels >> 28) & 0xf; while (xacc < 0x200) {
                        outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                         x+=deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 24) & 0xf; while (xacc < 0x200) {
                       outrun_draw_pixelT<shadow,nohlight,yestc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                         x+=deltax; xacc += hzoom; } xacc -= 0x200;

            }
        }//if y ok

        // accumulate zoom factors; if we carry into the high bit, skip an extra row
        yacc += vzoom;
        addr += pitch * (yacc >> 9);
        yacc &= 0x1ff;
    }


}


template<bool shadow,bool doclipx,int deltax,bool doextra15> FINLINE
void outrun_sprite_loopy_flip(
        mame_bitmap *bitmap,
        const rectangle *cliprect,
        const UINT32 *spritedata,
        int top,int ytarget,int ydelta,
        const int hzoom,const int vzoom,const int pitch,int addr,int xpos,int sprpri,int color)
{
    int yacc = 0;

    const bool notc15=false;
    const bool yestc15=true;
     const bool nohlight=false;
    // compute silly xstart with 2 first pixels that are always transparent
    int xjmp = (hzoom>>8); // >>10 <<1 for 2 pixels

    if(deltax>0)
        xpos+= xjmp;
    else  xpos -= xjmp;
    int xacc_startx = (hzoom*xjmp) & 0x1ff;

    for (int y = top; y != ytarget; y += ydelta)
    {
        // skip drawing if not within the cliprect
        if (y >= cliprect->min_y && y <= cliprect->max_y)
        {
            UINT16 *dest = (UINT16 *)bitmap->line[y];
            UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
            int xacc = xacc_startx;

            // start at the word before because we preincrement below
            UINT16 data7 = addr ;

            int x = xpos;
// hzoom 0x200=1 ?
         //   if (x >= cliprect->min_x
            UINT32 pixels = spritedata[data7--];
            // first byte is obviously unused.
            // it's always 0,15, the end code for reverse.

            // the following 2 jumps has been replaced by xjmp.
            //while (xacc < 0x200) { x++; xacc += hzoom; } xacc -= 0x200;
            //while (xacc < 0x200) { x++; xacc += hzoom; } xacc -= 0x200;

            for (;
                    (deltax > 0 && x <= cliprect->max_x) ||
                 ( deltax < 0 && x >= cliprect->min_x); )
            {

                int pix;
                // draw four pixels
//                pix = (pixels >>  0) & 0xf; while (xacc < 0x200) {
//                   outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
//                    x += deltax; xacc += hzoom; } xacc -= 0x200;
//                pix = (pixels >>  4) & 0xf; while (xacc < 0x200) {
//                    outrun_draw_pixelT<shadow,nohlight,yestc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
//                    x += deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >>  8) & 0xf; while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 12) & 0xf; while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 16) & 0xf; while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,doextra15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 20) & 0xf; while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,doextra15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >> 24) & 0xf;
    if(pix == 0x0f) break;  // stop if the second-to-last pixel in the group was 0xf
                while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;

                pix = (pixels >> 28) & 0xf;

                while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;

                pixels = spritedata[data7--];

                pix = (pixels >>  0) & 0xf; while (xacc < 0x200) {
                   outrun_draw_pixelT<shadow,nohlight,notc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;
                pix = (pixels >>  4) & 0xf; while (xacc < 0x200) {
                    outrun_draw_pixelT<shadow,nohlight,yestc15,doclipx,doclipx>(dest,pri,sprpri,x,cliprect,pix,color);
                    x += deltax; xacc += hzoom; } xacc -= 0x200;

            }

        }//if y ok

        // accumulate zoom factors; if we carry into the high bit, skip an extra row
        yacc += vzoom;
        addr += pitch * (yacc >> 9);
        yacc &= 0x1ff;
    }


}
// thunderblade and outrun uses this, but some thbld sprites needs extra more color&5 test,
// that we can avoid in all outrun sprites.
template<bool doExtra15t>
void segaic16_sprites_outrun_drawT(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect)
{
	UINT8 numbanks = memory_region_length(REGION_GFX2) / 0x40000;
	const UINT32 *spritebase = (const UINT32 *)memory_region(REGION_GFX2);
	UINT16 *data;

	/* first scan forward to find the end of the list */
	for (data = info->buffer; data < info->buffer + info->ramsize/2; data += 8)
		if (data[0] & 0x8000)
			break;


 int numBankMask = 1;
  while(numBankMask<numbanks) numBankMask<<=1;
 numBankMask--;

	/* now scan backwards and render the sprites in order */
	for (data -= 8; data >= info->buffer; data -= 8)
	{
		int hide    = (data[0] & 0x5000);
		int bank    = (data[0] >> 9) & 7;
		int top     = (data[0] & 0x1ff) - 0x100;
		UINT16 addr = data[1];
		int pitch   = (INT16)((data[2] >> 1) | ((data[4] & 0x1000) << 3)) >> 8;
		int xpos    = data[2] & 0x1ff;
		int shadow  = (data[3] >> 14) & 1;
		int sprpri  = 1 << ((data[3] >> 12) & 3);
		int vzoom   = data[3] & 0x7ff;
		int ydelta  = (data[4] & 0x8000) ? 1 : -1;

		int flip    = (~data[4] >> 14) & 1;
		int xdelta  = (data[4] & 0x2000) ? 1 : -1;

		int hzoom   = data[4] & 0x7ff;
		int height  = ((info->type == SEGAIC16_SPRITES_OUTRUN) ? (data[5] >> 8) : (data[5] & 0xfff)) + 1;
		int color   = info->colorbase + (((info->type == SEGAIC16_SPRITES_OUTRUN) ? (data[5] & 0x7f) : (data[6] & 0xff)) << 4);
		int x, y, ytarget;
		const UINT32 *spritedata;

		/* adjust X coordinate */
		/* note: the threshhold below is a guess. If it is too high, rachero will draw garbage */
		/* If it is too low, smgp won't draw the bottom part of the road */
		if (xpos < 0x80 && xdelta < 0)
			xpos += 0x200;
		xpos -= 0xbe;

		/* if hidden, or top greater than/equal to bottom, or invalid bank, punt */
		if (hide || height == 0)
			continue;

		/* clamp to within the memory region size */
		if (numbanks)
    		bank &= numBankMask;
			//old: bank %= numbanks;
		spritedata = spritebase + 0x10000 * bank;

		/* clamp to a maximum of 8x (not 100% confirmed) */
		if (vzoom < 0x40) vzoom = 0x40;
		if (hzoom < 0x40) hzoom = 0x40;

		/* loop from top to bottom */
		ytarget = top + ydelta * height;

    // krb silly code to find projected pixel width, to avoid x clip test per pixel.
    int pixwidth=0;
    while((spritedata[addr+pixwidth] & 0x000000f0) != 0x0f0) pixwidth++;
    pixwidth++;
    pixwidth<<=3; // *=8;
    pixwidth -=2; // because 2 lasts are always empty. it's 4 , 6+6, 6+8+6, 6+8+8+6,...
    // 2 first are jumped in yloop.

    if(!shadow)
    {
        if (!flip)
        {
            if (xdelta > 0)
            {
                int xend=xpos + ((pixwidth*hzoom)>>10);
                bool needclipx = ((xpos<cliprect->min_x) || (xend>=cliprect->max_x));
                if(needclipx)
                {
                 outrun_sprite_loopy_noflip<false,true,1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                 outrun_sprite_loopy_noflip<false,false,1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                // end if xdelta > 0
            } else
            {   // delta <0
                int xend=xpos - ((pixwidth*hzoom)>>10);
                bool needclipx = ((xend<cliprect->min_x) || (xpos>=cliprect->max_x));
                if(needclipx)
                {
                 outrun_sprite_loopy_noflip<false,true,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                 outrun_sprite_loopy_noflip<false,false,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }

            } // end if xdelta < 0


            // end if no flipx
        } else
        { // if flip


          if (xdelta > 0)
            {
                int xend=xpos + ((pixwidth*hzoom)>>10);
                bool needclipx = ((xpos<cliprect->min_x) || (xend>=cliprect->max_x));
                if(needclipx)
                {
                 outrun_sprite_loopy_flip<false,true,1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                 outrun_sprite_loopy_flip<false,false,1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                // end if xdelta > 0
            } else
            {   // delta <0
                int xend=xpos - ((pixwidth*hzoom)>>10);
                bool needclipx = ((xend<cliprect->min_x) || (xpos>=cliprect->max_x));
                if(needclipx)
                {
                 outrun_sprite_loopy_flip<false,true,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                 outrun_sprite_loopy_flip<false,false,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }

            } // end if xdelta < 0


        } // end if flipx
        }
        else
        {
            //shadow
       if (!flip)
        {
            if (xdelta > 0)
            {
                int xend=xpos + ((pixwidth*hzoom)>>10);
                bool needclipx = ((xpos<cliprect->min_x) || (xend>=cliprect->max_x));
                if(needclipx)
                {
                    outrun_sprite_loopy_noflip<true,true,1,doExtra15t>(
                        bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                    outrun_sprite_loopy_noflip<true,false,1,doExtra15t>(
                       bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }

                // end if xdelta > 0
            } else
            {   // delta <0

                int xend=xpos - ((pixwidth*hzoom)>>10);
                bool needclipx = ((xend<cliprect->min_x) || (xpos>=cliprect->max_x));
                if(needclipx)
                {
                 outrun_sprite_loopy_noflip<true,true,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                 outrun_sprite_loopy_noflip<true,false,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }

               // outrun_sprite_loopy<0,0>();
            } // end if xdelta < 0
            // end if no flipx
        } else
        { // if flip

            if (xdelta > 0)
            {
                int xend=xpos + ((pixwidth*hzoom)>>10);
                bool needclipx = ((xpos<cliprect->min_x) || (xend>=cliprect->max_x));
                if(needclipx)
                {
                    outrun_sprite_loopy_flip<true,true,1,doExtra15t>(
                        bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                    outrun_sprite_loopy_flip<true,false,1,doExtra15t>(
                       bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }

                // end if xdelta > 0
            } else
            {   // delta <0

                int xend=xpos - ((pixwidth*hzoom)>>10);
                bool needclipx = ((xend<cliprect->min_x) || (xpos>=cliprect->max_x));
                if(needclipx)
                {
                 outrun_sprite_loopy_flip<true,true,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }
                else
                {
                 outrun_sprite_loopy_flip<true,false,-1,doExtra15t>(
                     bitmap,cliprect,spritedata,top,ytarget,ydelta,hzoom,vzoom,pitch,addr,xpos,sprpri,color );
                }

               // outrun_sprite_loopy<0,0>();
            } // end if xdelta < 0




        } // end if flipx


        } // end shadow
/*oldemodel
		for (y = top; y != ytarget; y += ydelta)
		{
			// skip drawing if not within the cliprect
			if (y >= cliprect->min_y && y <= cliprect->max_y)
			{
				UINT16 *dest = (UINT16 *)bitmap->line[y];
				UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
				int xacc = 0;

				// non-flipped case
				if (!flip)
				{
					// start at the word before because we preincrement below
					data[7] = addr - 1;
					for (x = xpos;
                        (xdelta > 0 && x <= cliprect->max_x) ||
                        (xdelta < 0 && x >= cliprect->min_x); )
					{
						UINT32 pixels = spritedata[++data[7]];

						// draw four pixels
						int pix;
						pix = (pixels >> 28) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 24) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 20) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 16) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 12) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >>  8) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >>  4) & 0xf;
// if(pix == 0x0f) break;
						while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;

						pix = (pixels >>  0) & 0xf;
 //if(pix == 0x0f) beak;
						 while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;

						// stop if the second-to-last pixel in the group was 0xf
						if ((pixels & 0x000000f0) == 0x000000f0)
                            break;
					}
				}

				// flipped case
				else
				{
					// start at the word after because we predecrement below
					data[7] = addr + 1;
					for (x = xpos; (xdelta > 0 && x <= cliprect->max_x) || (xdelta < 0 && x >= cliprect->min_x); )
					{
						UINT32 pixels = spritedata[--data[7]];
                        int pix;
						// draw four pixels
						pix = (pixels >>  0) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >>  4) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >>  8) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 12) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 16) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 20) & 0xf; while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;
						pix = (pixels >> 24) & 0xf;
           // if(pix == 0x0f) break;
						while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;

						pix = (pixels >> 28) & 0xf;
           // if(pix == 0x0f) break;
						while (xacc < 0x200) { outrun_draw_pixel(); x += xdelta; xacc += hzoom; } xacc -= 0x200;

						// stop if the second-to-last pixel in the group was 0xf
						if ((pixels & 0x0f000000) == 0x0f000000)
						 	break;
					}
				}
			}

			// accumulate zoom factors; if we carry into the high bit, skip an extra row
			yacc += vzoom;
			addr += pitch * (yacc >> 9);
			yacc &= 0x1ff;
		}
*/
	} // end loop per sprites
}
void segaic16_sprites_outrun_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect)
{
    segaic16_sprites_outrun_drawT<false>(info, bitmap,cliprect);
}
void segaic16_sprites_thndrbld_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect)
{
    segaic16_sprites_outrun_drawT<true>(info, bitmap,cliprect);
}






/*******************************************************************************************
 *
 *  Space Harrier-style sprites
 *
 *      Offs  Bits               Usage
 *       +0   bbbbbbbb --------  Bottom scanline of sprite - 1
 *       +0   -------- tttttttt  Top scanline of sprite - 1
 *       +2   bbbb---- --------  Sprite bank
 *       +2   -------x xxxxxxxx  X position of sprite (position $BD is screen position 0)
 *       +4   s------- --------  Sprite shadow enable (0=enable, 1=disable)
 *       +4   -p------ --------  Sprite priority
 *       +4   --cccccc --------  Sprite color palette
 *       +4   -------- -ppppppp  Signed 7-bit pitch value between scanlines
 *       +6   f------- --------  Horizontal flip: read the data backwards if set
 *       +6   -ooooooo oooooooo  Offset within selected sprite bank
 *       +8   --zzzzzz --------  Horizontal zoom factor
 *       +8   -------- --zzzzzz  Vertical zoom factor
 *       +E   dddddddd dddddddd  Scratch space for current address
 *
 *  Special notes:
 *
 *      There is an interaction between the horizonal flip bit and the offset.
 *      The offset is maintained as a 16-bit value, even though only the lower
 *      15 bits are used for the address. The top bit is used to control flipping.
 *      This means that if the low 15 bits overflow during rendering, the sprite
 *      data will be read backwards after the overflow. This is important to
 *      emulate correctly as many games make use of this feature to render sprites
 *      at the beginning of a bank.
 *
 *******************************************************************************************/

//#define sharrier_draw_pixel()												\
//	/* only draw if onscreen, not 0 or 15 */								\
//	if (pix != 0 && pix != 15)												\
//	{																		\
//		/* are we high enough priority to be visible? */					\
//		if (sprpri > pri[x])												\
//		{																	\
//			/* shadow/hilight mode? */										\
//			if (shadow && pix == 0xa)										\
//				dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;	\
//																			\
//			/* regular draw */												\
//			else															\
//				dest[x] = pix | color;										\
//		}																	\
//																			\
//		/* always mark priority so no one else draws here */				\
//		pri[x] = 0xff;														\
//	}																		\


template<bool shadow,bool highlight,bool test15> FINLINE
 void  sharrier_draw_pixelT(UINT16 *dest,UINT8 *pri,const int sprpri, int x, int pix, int color )
{
	if ( pix != 0 &&  (!test15 || pix != 15))
	{
		/* are we high enough priority to be visible? */
		if (sprpri > pri[x])
		{
			/* shadow/hilight mode? */
			if (shadow && pix == 0xa)
			{
                // dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;
    			if(highlight)
    			{
        			dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;
    			} else dest[x] |= palette.entries;
            }
			/* regular draw */
			else
				dest[x] = pix | color;
		}
		/* always mark priority so no one else draws here */
		pri[x] = 0xff;
	}
}


void segaic16_sprites_sharrier_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect)
{
	UINT8 numbanks = memory_region_length(REGION_GFX2) / 0x20000;
	const UINT32 *spritebase = (const UINT32 *)memory_region(REGION_GFX2);
	const UINT8 *zoom = (const UINT8 *)memory_region(REGION_PROMS);
	UINT16 *data;

    const bool notc15=false;
    const bool yestc15=true;
    const bool nohlight=false;

 int numBankMask = 1;
  while(numBankMask<numbanks) numBankMask<<=1;
 numBankMask--;

	/* first scan forward to find the end of the list */
	for (data = info->spriteram; data < info->spriteram + info->ramsize/2; data += 8)
		if ((data[0] >> 8) > 0xf0)
			break;

	/* now scan backwards and render the sprites in order */
	for (data -= 8; data >= info->spriteram; data -= 8)
	{
		int bottom  = (data[0] >> 8) + 1;
		int top     = (data[0] & 0xff) + 1;
		int bank    = info->bank[(data[1] >> 12) & 0x7];
		int xpos    = (data[1] & 0x1ff) - 0xbd;
		int shadow  = (~data[2] >> 15) & 1;
		int sprpri  = ((data[2] >> 14) & 1) ? (1<<3) : (1<<1);
		int color   = info->colorbase + (((data[2] >> 8) & 0x3f) << 4);
		int pitch   = (INT16)(data[2] << 9) >> 9;
		UINT16 addr = data[3];
		int hzoom   = ((data[4] >> 8) & 0x3f) << 1;
		int vzoom   = (data[4] >> 0) & 0x3f;
		int x, y, zaddr, zmask;
		const UINT32 *spritedata;

		/* initialize the end address to the start address */
		data[7] = addr;

		/* if hidden, or top greater than/equal to bottom, or invalid bank, punt */
		if ((top >= bottom) || bank == 255)
			continue;

		/* clamp to within the memory region size */
		if (numbanks)
			//bank %= numbanks;
			bank &= numBankMask;
		spritedata = spritebase + 0x8000 * bank;

		/* determine the starting zoom address and mask */
		zaddr = (vzoom & 0x38) << 5;
		zmask = 1 << (vzoom & 7);

        // compute silly xstart with 2 first pixels that are always transparent
        int xjmp = (hzoom>>7); // >>10 <<1 for 2 pixels
        xpos+= xjmp;
        int xacc_startx = (hzoom*xjmp) & 0x0ff;


        if(!shadow)
        {
		/* loop from top to bottom */
		for (y = top; y < bottom; y++)
		{
			/* advance a row */
			addr += pitch;

			/* if the zoom bit says so, add pitch a second time */
			if (zoom[zaddr++] & zmask)
				addr += pitch;

			/* skip drawing if not within the cliprect */
			if (y >= cliprect->min_y && y <= cliprect->max_y)
			{
				UINT16 *dest = (UINT16 *)bitmap->line[y];
				UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
				int xacc = xacc_startx;

				/* note that the System 16A sprites have a design flaw that allows the address */
				/* to carry into the flip flag, which is the topmost bit -- it is very important */
				/* to emulate this as the games compensate for it */

				/* non-flipped case */
				if (!(addr & 0x8000))
				{
					/* start at the word before because we preincrement below */
					data[7] = addr - 1;
					UINT32 pixels = spritedata[++data[7] & 0x7fff];
					for (x = xpos; x <= cliprect->max_x; )
					{
						/* draw 8 pixels 7 +1 */
						int pix = (pixels >> 24) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,notc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >> 20) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >> 16) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,notc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >> 12) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >>  8) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,notc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >>  4) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >>  0) & 0xf;
                /* stop if the last pixel in the group was 0xf */
                if (pix == 15)
                    break;

						xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,notc15>(dest,pri, sprpri,x, pix, color ); x++; }

                        pixels =  spritedata[++data[7] & 0x7fff];
						pix = (pixels >> 28) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }


					}
				}

				/* flipped case */
				else
				{
					/* start at the word after because we predecrement below */
					data[7] = addr + 1;
                    UINT32 pixels = spritedata[--data[7] & 0x7fff];
					for (x = xpos; x <= cliprect->max_x; )
					{
						/* draw 8 pixels */
						int pix = (pixels >>  4) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >>  8) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 12) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 16) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 20) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 24) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 28) & 0xf;
                        /* stop if the last pixel in the group was 0xf */
                if (pix == 15)
                    break;
                        xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                        if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,notc15>(dest,pri, sprpri,x, pix, color );  x++; }
                        //  -- - -  -
                        pixels = spritedata[--data[7] & 0x7fff];
                        //
						pix = (pixels >>  0) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<false,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }

					}
				}
			}//end if y ok
		} // end y loop

            // end no shadow
        } else
        {   // with shadow

		/* loop from top to bottom */
		for (y = top; y < bottom; y++)
		{
			/* advance a row */
			addr += pitch;

			/* if the zoom bit says so, add pitch a second time */
			if (zoom[zaddr++] & zmask)
				addr += pitch;

			/* skip drawing if not within the cliprect */
			if (y >= cliprect->min_y && y <= cliprect->max_y)
			{
				UINT16 *dest = (UINT16 *)bitmap->line[y];
				UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
				int xacc = xacc_startx;

				/* note that the System 16A sprites have a design flaw that allows the address */
				/* to carry into the flip flag, which is the topmost bit -- it is very important */
				/* to emulate this as the games compensate for it */

				/* non-flipped case */
				if (!(addr & 0x8000))
				{
					/* start at the word before because we preincrement below */
					data[7] = addr - 1;
					UINT32 pixels = spritedata[++data[7] & 0x7fff];
					for (x = xpos; x <= cliprect->max_x; )
					{
					//KRB optim: could clean sprite 15 values when unlikely, could remove tests with yestc15.
						/* draw 8 pixels 7 +1 */
						int pix = (pixels >> 24) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >> 20) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >> 16) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >> 12) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >>  8) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >>  4) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }
						pix = (pixels >>  0) & 0xf;
                /* stop if the last pixel in the group was 0xf */
                if (pix == 15)
                    break;

						xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,notc15>(dest,pri, sprpri,x, pix, color ); x++; }

                        pixels =  spritedata[++data[7] & 0x7fff];
						pix = (pixels >> 28) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x) sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }


					}
				}

				/* flipped case */
				else
				{
					/* start at the word after because we predecrement below */
					data[7] = addr + 1;
                    UINT32 pixels = spritedata[--data[7] & 0x7fff];
					for (x = xpos; x <= cliprect->max_x; )
					{
						/* draw 8 pixels */
						int pix = (pixels >>  4) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >>  8) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 12) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 16) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 20) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 24) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color );  x++; }
						pix = (pixels >> 28) & 0xf;
                        /* stop if the last pixel in the group was 0xf */
                if (pix == 15)
                    break;
                        xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                        if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,notc15>(dest,pri, sprpri,x, pix, color );  x++; }
                        //  -- - -  -
                        pixels = spritedata[--data[7] & 0x7fff];
                        //
						pix = (pixels >>  0) & 0xf; xacc = (xacc & 0xff) + hzoom; if (xacc < 0x100) {
                            if (x >= cliprect->min_x)sharrier_draw_pixelT<true,false,yestc15>(dest,pri, sprpri,x, pix, color ); x++; }

					}
				}
			}//end if y ok
		} // end y loop



            // end with shadow
        }


	} // end loop per sprite
}











/*******************************************************************************************
 *
 *  Y-Board-style sprites
 *
 *      Offs  Bits               Usage
 *       +0   e------- --------  Signify end of sprite list
 *       +0   -----iii iiiiiiii  Address of indirection table (/16)
 *       +2   bbbb---- --------  Upper 4 bits of bank index
 *       +2   ----xxxx xxxxxxxx  X position of sprite (position $600 is screen position 0)
 *       +4   bbbb---- --------  Lower 4 bits of bank index
 *       +4   ----yyyy yyyyyyyy  Y position of sprite (position $600 is screen position 0)
 *       +6   oooooooo oooooooo  Offset within selected sprite bank
 *       +8   hhhhhhhh hhhhhhhh  Height of sprite
 *       +A   -y------ --------  Render from top-to-bottom (1) or bottom-to-top (0) on screen
 *       +A   --f----- --------  Horizontal flip: read the data backwards if set
 *       +A   ---x---- --------  Render from left-to-right (1) or right-to-left (0) on screen
 *       +A   -----zzz zzzzzzzz  Zoom factor
 *       +C   -ccc---- --------  Sprite color
 *       +C   ----rrrr --------  Sprite priority
 *       +C   -------- pppppppp  Signed 8-bit pitch value between scanlines
 *       +E   ----nnnn nnnnnnnn  Index of next sprite
 *
 *  In addition to these parameters, the sprite area is clipped using scanline extents
 *  stored for every pair of scanlines in the rotation RAM. It's a bit of a cheat for us
 *  to poke our nose into the rotation structure, but there are no known cases of Y-board
 *  sprites without rotation RAM.
 *
 *******************************************************************************************/


static inline void yboard_draw_pixel(int x,int minx, int maxx, int ind, int colorpri,UINT16 *dest )
{
    if (x >= minx && x <= maxx && ind < 0x01fe)
        dest[x] = ind | colorpri;
}
static inline void yboard_draw_pixelnt(int x, int ind, int colorpri,UINT16 *dest )
{
    if ( ind < 0x01fe) dest[x] = ind | colorpri;
}

static inline void yboard_draw_pixelnt2(int x, int ind, int colorpri,UINT16 *dest )
{
    dest[x] = ind | colorpri;
}

//    asm volatile(
//       "nop"
//       :
//       : // in
//       : // trashed
//       );
template<bool doclipx,int xdelta> FINLINE
void ybd_sprite_loopy_noflip(
        mame_bitmap *bitmap,
        const rectangle *cliprect,
        const UINT16 *rotatebase,
        const UINT64 *spritedata,
        const UINT16 *indirect,
        int top,int ytarget,int ydelta,
        const int zoom,const int pitch,int addr,int xpos,int startxacc, int colorpri)
{
    int yacc = 0;

    for (int y = top; y != ytarget; y += ydelta)
    {
        /* skip drawing if not within the cliprect */
        if (y >= cliprect->min_y && y <= cliprect->max_y)
        {
            UINT16 *dest = (UINT16 *)bitmap->line[y];
            int minx = rotatebase[y & ~1];
            int maxx = rotatebase[y |  1];


            /* bit 0x8000 from rotate RAM means that Y is above the top of the screen */
            if ((minx & 0x8000) && ydelta < 0)
                break;

            /* bit 0x4000 from rotate RAM means that Y is below the bottom of the screen */
            if ((minx & 0x4000) && ydelta > 0)
                break;

            /* if either bit is set, skip the rest for this scanline */
            if (!(minx & 0xc000))
            {
                /* clamp min/max to the cliprect */
                minx -= 0x600;
                maxx -= 0x600;
                if (minx < cliprect->min_x)
                    minx = cliprect->min_x;
                if (maxx > cliprect->max_x)
                    maxx = cliprect->max_x;

                /* non-flipped case */
                // if (!flip)
                // {
                    /* start at the word before because we preincrement below */
                    UINT16 offs = addr - 1;

                    UINT64 pixels = spritedata[++offs];
                    int xacc =startxacc;

                    for (int x = xpos;
                        (xdelta > 0 && x <= maxx) || // tests hopefully crunched by template.
                        (xdelta < 0 && x >= minx);
                        )
                    {
                        int ind,pix;

                    /* draw four pixels */
                    //pix = (pixels >> 60) & 0xf; ind = indirect[pix]; while (xacc < 0x200) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; } xacc -= 0x200;
                    pix = (pixels >> 56) & 0xf; ind = indirect[pix]; while (xacc < 0x400) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 52) & 0xf; ind = indirect[pix]; while (xacc < 0x600) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 48) & 0xf; ind = indirect[pix]; while (xacc < 0x800) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 44) & 0xf; ind = indirect[pix]; while (xacc < 0xa00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 40) & 0xf; ind = indirect[pix]; while (xacc < 0xc00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 36) & 0xf; ind = indirect[pix]; while (xacc < 0xe00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 32) & 0xf; ind = indirect[pix]; while (xacc < 0x1000) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }

                    pix = (pixels >> 28) & 0xf; ind = indirect[pix]; while (xacc < 0x1200) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 24) & 0xf; ind = indirect[pix]; while (xacc < 0x1400) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 20) & 0xf; ind = indirect[pix]; while (xacc < 0x1600) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 16) & 0xf; ind = indirect[pix]; while (xacc < 0x1800) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 12) & 0xf; ind = indirect[pix]; while (xacc < 0x1a00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >>  8) & 0xf; ind = indirect[pix]; while (xacc < 0x1c00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >>  4) & 0xf; ind = indirect[pix]; while (xacc < 0x1e00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >>  0) & 0xf;

                    /* stop if the last pixel in the group was 0xf */
                    if (pix == 0x0f)
                        break;
                    ind = indirect[pix]; while (xacc < 0x2000) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pixels = spritedata[++offs];
              xacc &= 0x01ff;
                    pix = (pixels >> 60) & 0xf; ind = indirect[pix]; while (xacc < 0x200) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }

                }

            }
        }

        /* accumulate zoom factors; if we carry into the high bit, skip an extra row */
        yacc += zoom;
        addr += pitch * (yacc >> 9);
        yacc &= 0x1ff;
    }
}



template<bool doclipx,int xdelta> FINLINE
void ybd_sprite_loopy_flip(
        mame_bitmap *bitmap,
        const rectangle *cliprect,
        const UINT16 *rotatebase,
        const UINT64 *spritedata,
        const UINT16 *indirect,
        int top,int ytarget,int ydelta,
        const int zoom,const int pitch,int addr,int xpos,int startxacc, int colorpri)
{
    int yacc = 0;

    for (int y = top; y != ytarget; y += ydelta)
    {
        /* skip drawing if not within the cliprect */
        if (y >= cliprect->min_y && y <= cliprect->max_y)
        {
            UINT16 *dest = (UINT16 *)bitmap->line[y];
            int minx = rotatebase[y & ~1];
            int maxx = rotatebase[y |  1];


            /* bit 0x8000 from rotate RAM means that Y is above the top of the screen */
            if ((minx & 0x8000) && ydelta < 0)
                break;

            /* bit 0x4000 from rotate RAM means that Y is below the bottom of the screen */
            if ((minx & 0x4000) && ydelta > 0)
                break;

            /* if either bit is set, skip the rest for this scanline */
            if (!(minx & 0xc000))
            {
                /* clamp min/max to the cliprect */
                minx -= 0x600;
                maxx -= 0x600;
                if (minx < cliprect->min_x)
                    minx = cliprect->min_x;
                if (maxx > cliprect->max_x)
                    maxx = cliprect->max_x;

                /* start at the word after because we predecrement below */
                UINT16 offs = addr + 1;
                UINT64 pixels = spritedata[--offs];
                int xacc = startxacc;

                for (int x = xpos;
                    ((xdelta > 0 && x <= maxx) || // tests hopefully crunched by template.
                    (xdelta < 0 && x >= minx));
                    )
                {
                    int ind,pix;

                    pix = (pixels >>  4) & 0xf; ind = indirect[pix]; while (xacc < 0x400) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >>  8) & 0xf; ind = indirect[pix]; while (xacc < 0x600) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 12) & 0xf; ind = indirect[pix]; while (xacc < 0x800) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 16) & 0xf; ind = indirect[pix]; while (xacc < 0xa00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 20) & 0xf; ind = indirect[pix]; while (xacc < 0xc00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 24) & 0xf; ind = indirect[pix]; while (xacc < 0xe00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 28) & 0xf; ind = indirect[pix]; while (xacc < 0x1000) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 32) & 0xf; ind = indirect[pix]; while (xacc < 0x1200) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 36) & 0xf; ind = indirect[pix]; while (xacc < 0x1400) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 40) & 0xf; ind = indirect[pix]; while (xacc < 0x1600) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 44) & 0xf; ind = indirect[pix]; while (xacc < 0x1800) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 48) & 0xf; ind = indirect[pix]; while (xacc < 0x1a00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 52) & 0xf; ind = indirect[pix]; while (xacc < 0x1c00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 56) & 0xf; ind = indirect[pix]; while (xacc < 0x1e00) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pix = (pixels >> 60) & 0xf;

                    /* stop if the last pixel in the group was 0xf */
                    if (pix == 0x0f)
                        break;
                    ind = indirect[pix]; while (xacc < 0x2000) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }
                    pixels = spritedata[--offs];
                    xacc &= 0x01ff;
                    pix = (pixels >>  0) & 0xf; ind = indirect[pix]; while (xacc < 0x200) { yboard_draw_pixel(x,minx,maxx,ind,colorpri,dest); x += xdelta; xacc += zoom; }

                } // end loop x

            } // end if line ok

        } // end if y in clip
        /* accumulate zoom factors; if we carry into the high bit, skip an extra row */
        yacc += zoom;
        addr += pitch * (yacc >> 9);
        yacc &= 0x1ff;
    }
}



void segaic16_sprites_yboard_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect)
{
	UINT8 numbanks = (UINT8)(memory_region_length(REGION_GFX1) / 0x80000);
	const UINT64 *spritebase = (const UINT64 *)memory_region(REGION_GFX1);
	const UINT16 *rotatebase = segaic16_ybd_rotate[0].buffer ? segaic16_ybd_rotate[0].buffer : segaic16_ybd_rotate[0].rotateram;
	static UINT8 visited[0x1000];
	UINT16 *data;
	UINT32 next = 0;
	int y;

	/* reset the visited list */
	memset(visited, 0, sizeof(visited));

	/* clear out any scanlines we might be using */
	for (y = cliprect->min_y; y <= cliprect->max_y; y++)
		if (!(rotatebase[y & ~1] & 0xc000))
			memset(&((UINT16 *)bitmap->line[y])[cliprect->min_x], 0xff, (cliprect->max_x - cliprect->min_x + 1) * sizeof(UINT16));

	/* now scan backwards and render the sprites in order */
	for (data = info->spriteram; !(data[0] & 0x8000) && !visited[next]; data = info->spriteram + (next<<3))
	{
		int hide    = (data[0] & 0x5000);
		UINT16 *indirect = info->spriteram + ((data[0] & 0x7ff) << 4);
		int bank    = ((data[1] >> 8) & 0x10) | ((data[2] >> 12) & 0x0f);
		int xpos    = (data[1] & 0x0fff) - 0x600;
		int top     = (data[2] & 0x0fff) - 0x600;
		UINT16 addr = data[3];
		int height  = data[4];
		int ydelta  = (data[5] & 0x4000) ? 1 : -1;
		int flip    = (~data[5] >> 13) & 1;
		int xdelta  = (data[5] & 0x1000) ? 1 : -1;
		int zoom    = data[5] & 0x7ff;
		int colorpri= (data[6] << 1) & 0x0000fe00;
		int pitch   = (INT8)data[6];
		int ytarget, yacc = 0, pix, ind;
		const UINT64 *spritedata;

//        if(dh)
//        {
//            fprintf(dh,"%04x ",(int)next);
//            jj++;
//                if(jj==32) {
//                    fprintf(dh,"\n");
//                    jj=0;
//                }
//         }

		/* note that we've visited this entry and get the offset of the next one */
		visited[next] = 1;
		next = data[7] & 0x0fff;

		/* if hidden, or top greater than/equal to bottom, or invalid bank, punt */
		if (hide || height == 0)
			continue;

		/* clamp to within the memory region size */
		if (numbanks)
			bank %= numbanks;
		spritedata = spritebase + 0x10000 * bank;

		/* clamp to a maximum of 8x (not 100% confirmed) */
		if (zoom == 0) zoom = 1;



        // krb trick of knowing projected pixel with
        // int projectedwidth=0;
        // if(!flip)
        // {
        //     int i=0;
        //     while(spritedata[addr+i] & 0x0f != 0x0f) i++;
        //     i++;
        //     int  sourcewidth = (i<<4)-1;
        //     projectedwidth = ((sourcewidth*zoom)>>9); //0x200 -> 10
        // } //  xpos + xdelta*
       // UINT64 pixels = spritedata[++offs];

        //krb trick of jump first pixel.
		int startxacc =0;
		int startxjmp = 0;
        while (startxacc < 0x200) { startxjmp++; startxacc += zoom; } // next is 0x400, do not clamp.
        xpos += xdelta*startxjmp;

		/* loop from top to bottom */
		ytarget = top + ydelta * height;

        /* non-flipped case */
        if (!flip)
        {

            const bool docliptests=true;
            if(xdelta>0)
                ybd_sprite_loopy_noflip<docliptests,1>(bitmap,cliprect,
                    rotatebase, spritedata, indirect,
                    top,ytarget,ydelta,
                    zoom,pitch,addr,xpos,startxacc,colorpri);
            else
                ybd_sprite_loopy_noflip<docliptests,-1>(bitmap,cliprect,
                    rotatebase,spritedata, indirect,
                    top,ytarget,ydelta,
                    zoom,pitch,addr,xpos,startxacc,colorpri);
            // end with no flipx
        } else
        {   // with flipx
            const bool docliptests=true;
            if(xdelta>0)
                ybd_sprite_loopy_flip<docliptests,1>(bitmap,cliprect,
                    rotatebase, spritedata, indirect,
                    top,ytarget,ydelta,
                    zoom,pitch,addr,xpos,startxacc,colorpri);
            else
                ybd_sprite_loopy_flip<docliptests,-1>(bitmap,cliprect,
                    rotatebase,spritedata, indirect,
                    top,ytarget,ydelta,
                    zoom,pitch,addr,xpos,startxacc,colorpri);

        } // end with flipx

	//	nbi++;
	} // end loop per sprites

}

