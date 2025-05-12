// complements the .c version
extern "C"
{
#include "driver.h"
#include "mame.h"
#include "segaic16.h"
#include "vidhrdw/res_net.h"
#include <stdio.h>
#include <stdlib.h>
}

struct sprite_info
{
	UINT8			index;							/* index of this structure */
	UINT8			type;							/* type of sprite system (see segaic16.h for details) */
	UINT8			flip;							/* screen flip? */
	UINT8			shadow;							/* shadow or hilight? */
	UINT8			bank[16];						/* banking redirection */
	UINT16			colorbase;						/* base color index */
	INT32			ramsize;						/* size of sprite RAM */
	INT32			xoffs;							/* X scroll offset */
	void			(*draw)(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect);
	UINT16 *		spriteram;						/* pointer to spriteram pointer */
	UINT16 *		buffer;							/* buffered spriteram for those that use it */
};

struct palette_info
{
	INT32			entries;						/* number of entries (not counting shadows) */
	UINT8			normal[32];						/* RGB translations for normal pixels */
	UINT8			shadow[32];						/* RGB translations for shadowed pixels */
	UINT8			hilight[32];					/* RGB translations for hilighted pixels */
};



extern "C" {
void segaic16_sprites_outrun_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect);
extern struct palette_info palette;

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

#define outrun_draw_pixel() 												\
	/* only draw if onscreen, not 0 or 15 */								\
	if (x >= cliprect->min_x && x <= cliprect->max_x && pix != 0 && pix != 15) \
	{																		\
		/* are we high enough priority to be visible? */					\
		if (sprpri > pri[x])												\
		{																	\
			/* shadow/hilight mode? */										\
			if (shadow && pix == 0xa)										\
				dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;	\
																			\
			/* regular draw */												\
			else															\
				dest[x] = pix | color;										\
		}																	\
																			\
		/* always mark priority so no one else draws here */				\
		pri[x] = 0xff;														\
	}																		\

// template<int shadow>
//  void or_drpix(UINT16 *dest,UINT8 *pri,int sprpri, int x,   )
// {
// 	if (x >= cliprect->min_x && x <= cliprect->max_x && pix != 0 && pix != 15)
// 	{
// 		/* are we high enough priority to be visible? */
// 		if (sprpri > pri[x])
// 		{
// 			/* shadow/hilight mode? */
// 			if (shadow && pix == 0xa)
// 				dest[x] += (paletteram16[dest[x]] & 0x8000) ? palette.entries*2 : palette.entries;

// 			/* regular draw */
// 			else
// 				dest[x] = pix | color;
// 		}
// 		/* always mark priority so no one else draws here */
// 		pri[x] = 0xff;
// 	}
// }

void segaic16_sprites_outrun_draw(struct sprite_info *info, mame_bitmap *bitmap, const rectangle *cliprect)
{
	UINT8 numbanks = memory_region_length(REGION_GFX2) / 0x40000;
	const UINT32 *spritebase = (const UINT32 *)memory_region(REGION_GFX2);
	UINT16 *data;

	/* first scan forward to find the end of the list */
	for (data = info->buffer; data < info->buffer + info->ramsize/2; data += 8)
		if (data[0] & 0x8000)
			break;

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
		int x, y, ytarget, yacc = 0;
		const UINT32 *spritedata;

		/* adjust X coordinate */
		/* note: the threshhold below is a guess. If it is too high, rachero will draw garbage */
		/* If it is too low, smgp won't draw the bottom part of the road */
		if (xpos < 0x80 && xdelta < 0)
			xpos += 0x200;
		xpos -= 0xbe;

		/* initialize the end address to the start address */
		data[7] = addr;

		/* if hidden, or top greater than/equal to bottom, or invalid bank, punt */
		if (hide || height == 0)
			continue;

		/* clamp to within the memory region size */
		if (numbanks)
			bank %= numbanks;
		spritedata = spritebase + 0x10000 * bank;

		/* clamp to a maximum of 8x (not 100% confirmed) */
		if (vzoom < 0x40) vzoom = 0x40;
		if (hzoom < 0x40) hzoom = 0x40;

		/* loop from top to bottom */
		ytarget = top + ydelta * height;
		for (y = top; y != ytarget; y += ydelta)
		{
			/* skip drawing if not within the cliprect */
			if (y >= cliprect->min_y && y <= cliprect->max_y)
			{
				UINT16 *dest = (UINT16 *)bitmap->line[y];
				UINT8 *pri = (UINT8 *)priority_bitmap->line[y];
				int xacc = 0;

				/* non-flipped case */
				if (!flip)
				{
					/* start at the word before because we preincrement below */
					data[7] = addr - 1;
					for (x = xpos;
                        (xdelta > 0 && x <= cliprect->max_x) ||
                        (xdelta < 0 && x >= cliprect->min_x); )
					{
						UINT32 pixels = spritedata[++data[7]];

						/* draw four pixels */
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

						/* stop if the second-to-last pixel in the group was 0xf */
						if ((pixels & 0x000000f0) == 0x000000f0)
                            break;
					}
				}

				/* flipped case */
				else
				{
					/* start at the word after because we predecrement below */
					data[7] = addr + 1;
					for (x = xpos; (xdelta > 0 && x <= cliprect->max_x) || (xdelta < 0 && x >= cliprect->min_x); )
					{
						UINT32 pixels = spritedata[--data[7]];
                        int pix;
						/* draw four pixels */
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

						/* stop if the second-to-last pixel in the group was 0xf */
						if ((pixels & 0x0f000000) == 0x0f000000)
						 	break;
					}
				}
			}

			/* accumulate zoom factors; if we carry into the high bit, skip an extra row */
			yacc += vzoom;
			addr += pitch * (yacc >> 9);
			yacc &= 0x1ff;
		}
	}
}
