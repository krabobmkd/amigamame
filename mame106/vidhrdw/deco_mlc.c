/*
    The MLC graphics hardware is quite complicated - the usual method of having 'object ram' that
    controls sprites is expanded into object ram that controls sprite blocks that may be stored
    in RAM or ROM.  Each tile in a block may be specified explicitly via a display list in ROM or
    calculated as part of a block offset.

    Blocks can be scaled and subpositioned, and are usually 4-6bpp but two 4bpp blocks can be
    combined into 8bpp with a flag.
*/

#include "driver.h"

extern int mlc_raster_table[9][256];
extern UINT32 mlc_clipper[32];
static mame_bitmap *temp_bitmap;
static UINT32 colour_mask, *mlc_buffered_spriteram;
UINT32 *mlc_vram, *mlc_clip_ram;

/******************************************************************************/

VIDEO_START( mlc )
{
	alpha_set_level(0x80);

	if (Machine->gfx[0]->color_granularity==16)
		colour_mask=0x7f;
	else if (Machine->gfx[0]->color_granularity==32)
		colour_mask=0x3f;
	else
		colour_mask=0x1f;

	temp_bitmap = auto_bitmap_alloc_depth( 512, 512, -32 );
	mlc_buffered_spriteram = auto_malloc(0x3000);

	if (!temp_bitmap)
		return 1;

	return 0;
}

static void blitRaster(mame_bitmap *bitmap, int rasterMode)
{
#if 0
	int x,y;
	for (y=0; y<256; y++) //todo
	{
		UINT32* src=(UINT32 *)temp_bitmap->line[y&0x1ff];
		UINT32* dst=(UINT32 *)bitmap->line[y];
		UINT32 xptr=(mlc_raster_table[0][y]<<13);

		if (code_pressed(KEYCODE_X))
			xptr=0;

		for (x=0; x<320; x++)
		{
			if (src[x])
				dst[x]=src[(xptr>>16)&0x1ff];

			//if (code_pressed(KEYCODE_X))
			//  xptr+=0x10000;
			//else if(rasterHackTest[0][y]<0)
				xptr+=0x10000 - ((mlc_raster_table[2][y]&0x3ff)<<5);
			//else
			//  xptr+=0x10000 + (mlc_raster_table[0][y]<<5);
		}
	}
#endif
}

static void mlc_drawgfxzoom( mame_bitmap *dest_bmp,const gfx_element *gfx,
		unsigned int code1,unsigned int code2, unsigned int color,int flipx,int flipy,int sx,int sy,
		const rectangle *clip,int transparency,int transparent_color,int use8bpp,
		int scalex, int scaley)
{
	rectangle myclip;

	if (!scalex || !scaley) return;

	/*
    scalex and scaley are 16.16 fixed point numbers
    1<<15 : shrink to 50%
    1<<16 : uniform scale
    1<<17 : double to 200%
    */

	/* KW 991012 -- Added code to force clip to bitmap boundary */
	if(clip)
	{
		myclip.min_x = clip->min_x;
		myclip.max_x = clip->max_x;
		myclip.min_y = clip->min_y;
		myclip.max_y = clip->max_y;

		if (myclip.min_x < 0) myclip.min_x = 0;
		if (myclip.max_x >= dest_bmp->width) myclip.max_x = dest_bmp->width-1;
		if (myclip.min_y < 0) myclip.min_y = 0;
		if (myclip.max_y >= dest_bmp->height) myclip.max_y = dest_bmp->height-1;

		clip=&myclip;
	}

	{
		if( gfx && gfx->colortable )
		{
			const pen_t *pal = &gfx->colortable[gfx->color_granularity * (color % gfx->total_colors)]; /* ASG 980209 */
			int source_base1 = (code1 % gfx->total_elements) * gfx->height;
			int source_base2 = (code2 % gfx->total_elements) * gfx->height;

			int sprite_screen_height = (scaley*gfx->height+(sy&0xffff))>>16;
			int sprite_screen_width = (scalex*gfx->width+(sx&0xffff))>>16;

			sx>>=16;
			sy>>=16;

			if (sprite_screen_width && sprite_screen_height)
			{
				/* compute sprite increment per screen pixel */
				int dx = (gfx->width<<16)/sprite_screen_width;
				int dy = (gfx->height<<16)/sprite_screen_height;

				int ex = sx+sprite_screen_width;
				int ey = sy+sprite_screen_height;

				int x_index_base;
				int y_index;

				if( flipx )
				{
					x_index_base = (sprite_screen_width-1)*dx;
					dx = -dx;
				}
				else
				{
					x_index_base = 0;
				}

				if( flipy )
				{
					y_index = (sprite_screen_height-1)*dy;
					dy = -dy;
				}
				else
				{
					y_index = 0;
				}

				if( clip )
				{
					if( sx < clip->min_x)
					{ /* clip left */
						int pixels = clip->min_x-sx;
						sx += pixels;
						x_index_base += pixels*dx;
					}
					if( sy < clip->min_y )
					{ /* clip top */
						int pixels = clip->min_y-sy;
						sy += pixels;
						y_index += pixels*dy;
					}
					/* NS 980211 - fixed incorrect clipping */
					if( ex > clip->max_x+1 )
					{ /* clip right */
						int pixels = ex-clip->max_x-1;
						ex -= pixels;
					}
					if( ey > clip->max_y+1 )
					{ /* clip bottom */
						int pixels = ey-clip->max_y-1;
						ey -= pixels;
					}
				}

				if( ex>sx )
				{ /* skip if inner loop doesn't draw anything */
					int y;

					/* case 1: TRANSPARENCY_PEN */
					if (transparency == TRANSPARENCY_PEN)
					{
						{
							for( y=sy; y<ey; y++ )
							{
								UINT8 *source1 = gfx->gfxdata + (source_base1+(y_index>>16)) * gfx->line_modulo;
								UINT8 *source2 = gfx->gfxdata + (source_base2+(y_index>>16)) * gfx->line_modulo;
								UINT32 *dest = (UINT32 *)dest_bmp->line[y];

								int x, x_index = x_index_base;

								for( x=sx; x<ex; x++ )
								{
									int c = source1[x_index>>16];
									if (use8bpp)
										c=(c<<4)|source2[x_index>>16];

									if( c != transparent_color ) dest[x] = pal[c];

									x_index += dx;
								}

								y_index += dy;
							}
						}
					}

					/* case 6: TRANSPARENCY_ALPHA */
					if (transparency == TRANSPARENCY_ALPHA)
					{
						{
							for( y=sy; y<ey; y++ )
							{
								UINT8 *source = gfx->gfxdata + (source_base1+(y_index>>16)) * gfx->line_modulo;
								UINT32 *dest = (UINT32 *)dest_bmp->line[y];

								int x, x_index = x_index_base;
								for( x=sx; x<ex; x++ )
								{
									int c = source[x_index>>16];
									if( c != transparent_color ) dest[x] = alpha_blend32(dest[x], 0); //pal[c]);
									x_index += dx;
								}

								y_index += dy;
							}
						}
					}
				}
			}
		}
	}
}

static void draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	UINT32 *index_ptr=0;
	int offs,fx=0,fy=0,x,y,color,colorOffset,sprite,indx,h,w,bx,by,fx1,fy1;
	int xmult,ymult,xoffs,yoffs;
	UINT8 *rom = memory_region(REGION_GFX2) + 0x20000, *index_ptr8;
	UINT8 *rawrom = memory_region(REGION_GFX2);
	int blockIsTilemapIndex=0;
	int sprite2=0,indx2=0,use8bppMode=0;
	int yscale,xscale;
	int ybase,yinc;
	int trans;
	int useIndicesInRom=0;
	int hibits=0;
	int tileFormat=0;
	int rasterMode=0;
	int lastRasterMode=0;
	int rasterDirty=0;
	int clipper=0;
	rectangle user_clip;
	UINT32* mlc_spriteram=mlc_buffered_spriteram; // spriteram32

	for (offs = (0x3000/4)-8; offs>=0; offs-=8)
	{
		if ((mlc_spriteram[offs+0]&0x8000)==0)
			continue;
		if ((mlc_spriteram[offs+1]&0x2000) && (cpu_getcurrentframe() & 1))
			continue;

		/*
            Spriteram (1) format (16 bit):

            Word 0: 0x8000 - Sprite enable
                    0x4000 - Use ROM or RAM for spriteram 2 (really top bit of index)
                    0x3fff - Index into spriteram 2
            Word 1: 0x8000 - X flip
                    0x4000 - Y flip
                    0x2000 - Auto-flicker (display sprite only every other frame)
                    0x1000 - If set combine this 4bpp sprite & next one, into 8bpp sprite
                    0x0800 - ?  (Not seen used anywhere)
                    0x0400 - Use raster IRQ lookup table when drawing object
                    0x0300 - Selects clipping window to use
                    0x00ff - Colour/alpha shadow enable
            Word 2: 0x07ff - Y position
            Word 3: 0x07ff - X position
            Word 4: 0x03ff - X scale
            Word 5: 0x03ff - Y scale

            Spriteram (2) format (16 bit):

            Word 0: 0xe000 - ? (Always 0x2000?)
                    0x1000 - X flip
                    0x0f00 - Width in tiles (0==16)
                    0x00ff - X position offset
            Word 1: 0xe000 - ? (Always 0x2000?)
                    0x1000 - Y flip
                    0x0f00 - Height in tiles (0==16)
                    0x00ff - Y position offset
            Word 2: 0xff00 - ? (Always 0?)
                    0x00c0 - If set use tile index as pointer into tile index array, else use as tile index directly
                    0x0080 - If set tile index array format is 12 bit tile, 4 bit colour
                    0x0040 - If set tile index array is 16 bit tile, 0 bit colour
                    0x003c - Hi-bits of tile index after array lookup
                    0x0003 - Hi-bits of tile index, selects ROM or RAM for array
            Word 3: 0xffff - Low-bits of tile index
        */

		y = mlc_spriteram[offs+2]&0x7ff;
		x = mlc_spriteram[offs+3]&0x7ff;

		if (x&0x400) x=-(0x400-(x&0x3ff));
		if (y&0x400) y=-(0x400-(y&0x3ff));

		fx = mlc_spriteram[offs+1]&0x8000;
		fy = mlc_spriteram[offs+1]&0x4000;
		color = mlc_spriteram[offs+1]&0xff;
		rasterMode = (mlc_spriteram[offs+1]>>10)&0x1;
		clipper = (mlc_spriteram[offs+1]>>8)&0x3;
		indx = mlc_spriteram[offs+0]&0x3fff;
		yscale = mlc_spriteram[offs+4]&0x3ff;
		xscale = mlc_spriteram[offs+5]&0x3ff;
		colorOffset = 0;

		/* Clip windows - this mapping seems odd, but is correct for Skull Fang and StadHr96,
        however there are space for 8 clipping windows, where is the high bit? (Or is it ~0x400?) */
		clipper=((clipper&2)>>1)|((clipper&1)<<1); // Swap low two bits

		user_clip.min_y=mlc_clip_ram[(clipper*4)+0];
		user_clip.max_y=mlc_clip_ram[(clipper*4)+1];
		user_clip.min_x=mlc_clip_ram[(clipper*4)+2];
		user_clip.max_x=mlc_clip_ram[(clipper*4)+3];

		if (user_clip.min_y < cliprect->min_y) user_clip.min_y=cliprect->min_y;
		if (user_clip.max_y > cliprect->max_y) user_clip.max_y=cliprect->max_y;
		if (user_clip.min_x < cliprect->min_x) user_clip.min_x=cliprect->min_x;
		if (user_clip.max_x > cliprect->max_x) user_clip.max_x=cliprect->max_x;

		/* Any colours out of range (for the bpp value) trigger 'shadow' mode */
		if (color & (colour_mask+1))
			trans=TRANSPARENCY_ALPHA;
		else
			trans=TRANSPARENCY_PEN;
		color&=colour_mask;

		/* If this bit is set, combine this block with the next one */
		if (mlc_spriteram[offs+1]&0x1000) {
			use8bppMode=1;
			/* In 8bpp the palette base is stored in the next block */
			if (offs-8>=0) {
				color = (mlc_spriteram[offs+1-8]&0x7f);
				indx2 = mlc_spriteram[offs+0-8]&0x3fff;
			}
		} else
			use8bppMode=0;

		/* Lookup tiles/size in sprite index ram OR in the lookup rom */
		if (mlc_spriteram[offs+0]&0x4000) {
			index_ptr8=rom + indx*8; /* Byte ptr */
			h=(index_ptr8[1]>>0)&0xf;
			w=(index_ptr8[3]>>0)&0xf;

			if (!h) h=16;
			if (!w) w=16;

			sprite = (index_ptr8[7]<<8)|index_ptr8[6];
			sprite |= (index_ptr8[4]&3)<<16;

			if (use8bppMode) {
				UINT8* index_ptr28=rom + indx2*8;
				sprite2=(index_ptr28[7]<<8)|index_ptr28[6];
			}
			//unused byte 5
			yoffs=index_ptr8[0]&0xff;
			xoffs=index_ptr8[2]&0xff;

			fy1=(index_ptr8[1]&0xf0)>>4;
			fx1=(index_ptr8[3]&0xf0)>>4;

			tileFormat=index_ptr8[4]&0x80;

			if (index_ptr8[4]&0xc0)
				blockIsTilemapIndex=1;
			else
				blockIsTilemapIndex=0;

			useIndicesInRom=0;

		} else {
			indx&=0x1fff;
			index_ptr=mlc_vram + indx*4;
			h=(index_ptr[0]>>8)&0xf;
			w=(index_ptr[1]>>8)&0xf;

			if (!h) h=16;
			if (!w) w=16;

			if (use8bppMode) {
				UINT32* index_ptr2=mlc_vram + indx2*4;
				sprite2=((index_ptr2[2]&0x3)<<16) | (index_ptr2[3]&0xffff);
			}

			sprite = ((index_ptr[2]&0x3)<<16) | (index_ptr[3]&0xffff);
			if (index_ptr[2]&0xc0)
				blockIsTilemapIndex=1;
			else
				blockIsTilemapIndex=0;

			tileFormat=index_ptr[2]&0x80;

			hibits=(index_ptr[2]&0x3c)<<10;
			useIndicesInRom=index_ptr[2]&3;

			yoffs=index_ptr[0]&0xff;
			xoffs=index_ptr[1]&0xff;

			fy1=(index_ptr[0]&0xf000)>>12;
			fx1=(index_ptr[1]&0xf000)>>12;
		}

		if(fx1&1) fx^=0x8000;
		if(fy1&1) fy^=0x4000;

		if (fx) xmult=-1; else xmult=1;
		if (fy) ymult=-1; else ymult=1;

		ybase=y<<16;
		if (fy)
			ybase+=(yoffs-15) * (yscale<<8);
		else
			ybase-=yoffs * (yscale<<8);

		yinc=(yscale<<8)*16;

		if (fy) yinc=-yinc;

		for (by=0; by<h; by++) {
			int xbase=x<<16;
			int xinc=(xscale<<8)*16;

			if (fx)
				xbase+=(xoffs-15) * (xscale<<8);
			else
				xbase-=xoffs * (xscale<<8);

			if (fx) xinc=-xinc;

			for (bx=0; bx<w; bx++) {
				int tile=sprite;
				int tile2=sprite2;

				if (blockIsTilemapIndex) {
					if (useIndicesInRom)
					{
						const UINT8* ptr=rawrom+(sprite*2);
						tile=(*ptr) + ((*(ptr+1))<<8);

						if (use8bppMode) {
							const UINT8* ptr2=rawrom+(sprite2*2);
							tile2=(*ptr2) + ((*(ptr2+1))<<8);
						}
						else
						{
							tile2=0;
						}

						if (tileFormat)
						{
							colorOffset=(tile&0xf000)>>12;
							tile=(tile&0x0fff)|hibits;
							tile2=(tile2&0x0fff)|hibits;
						}
						else
						{
							colorOffset=0;
							tile=(tile&0xffff)|(hibits<<2);
							tile2=(tile2&0xffff)|(hibits<<2);
						}
					}
					else
					{
						const UINT32* ptr=mlc_vram + ((sprite)&0x7fff);
						tile=(*ptr)&0xffff;

						if (tileFormat)
						{
							colorOffset=(tile&0xf000)>>12;
							tile=(tile&0x0fff)|hibits;
						}
						else
						{
							colorOffset=0;
							tile=(tile&0xffff)|(hibits<<2);
						}

						tile2=0;
					}
				}

				if (rasterMode)
					rasterDirty=1;

				mlc_drawgfxzoom(/*rasterMode ? temp_bitmap : */bitmap,Machine->gfx[0],
								tile,tile2,
								color + colorOffset,fx,fy,xbase,ybase,
								&user_clip,trans,0,
								use8bppMode,(xscale<<8),(yscale<<8));

				sprite++;
				sprite2++;

				xbase+=xinc;
			}
			ybase+=yinc;
		}

		if (lastRasterMode!=0 && rasterDirty)
		{
	//      blitRaster(bitmap, rasterMode);
//          fillbitmap(temp_bitmap,0,cliprect);
			rasterDirty=0;
		}
		lastRasterMode=rasterMode;

		if (use8bppMode)
			offs-=8;
	}
}

VIDEO_EOF( mlc )
{
	/* Spriteram is definitely double buffered, as the vram lookup tables
    are often updated a frame after spriteram is setup to point to a new
    lookup table.  Without buffering incorrect one frame glitches are seen
    in several places, especially in Hoops.
    */
	memcpy(mlc_buffered_spriteram, spriteram32, 0x3000);
}

VIDEO_UPDATE( mlc )
{
//  fillbitmap(temp_bitmap,0,cliprect);
	fillbitmap(bitmap,Machine->pens[0],cliprect); /* Pen 0 fill colour confirmed from Skull Fang level 2 */
	draw_sprites(bitmap,cliprect);
}
