/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"



UINT8 *goldstar_video1, *goldstar_video2, *goldstar_video3;
size_t goldstar_video_size;
UINT8 *goldstar_scroll1, *goldstar_scroll2, *goldstar_scroll3;

static UINT8 *dirtybuffer1, *dirtybuffer2, *dirtybuffer3;
static mame_bitmap *tmpbitmap1, *tmpbitmap2, *tmpbitmap3;
static int bgcolor;



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/
VIDEO_START( goldstar )
{
//        int i;

	if (video_start_generic() != 0)
		return 1;

	dirtybuffer1 = auto_malloc(3 * goldstar_video_size * sizeof(UINT8));
	dirtybuffer2 = dirtybuffer1 + goldstar_video_size;
	dirtybuffer3 = dirtybuffer2 + goldstar_video_size;

	/* the background area is half as high as the screen */
	tmpbitmap1 = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	tmpbitmap2 = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	tmpbitmap3 = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);

	if (!(tmpbitmap1 && tmpbitmap2 && tmpbitmap3))
	{
		return 1;
	}

	/* leave everything at the default, but map all foreground 0 pens as transparent */
//        for (i = 0;i < 16;i++) palette_used_colors[8 * i] = PALETTE_COLOR_TRANSPARENT;

	return 0;
}




WRITE8_HANDLER( goldstar_video1_w )
{
	if (goldstar_video1[offset] != data)
	{
		dirtybuffer1[offset] = 1;
		goldstar_video1[offset] = data;
	}
}
WRITE8_HANDLER( goldstar_video2_w )
{
	if (goldstar_video2[offset] != data)
	{
		dirtybuffer2[offset] = 1;
		goldstar_video2[offset] = data;
	}
}
WRITE8_HANDLER( goldstar_video3_w )
{
	if (goldstar_video3[offset] != data)
	{
		dirtybuffer3[offset] = 1;
		goldstar_video3[offset] = data;
	}
}



WRITE8_HANDLER( goldstar_fa00_w )
{
	/* bit 1 toggles continuously - might be irq enable or watchdog reset */

	/* bit 2 selects background gfx color (I think) */
	if (bgcolor != ((data & 0x04) >> 2))
	{
		bgcolor = (data & 0x04) >> 2;
		memset(dirtybuffer1,1,goldstar_video_size);
		memset(dirtybuffer2,1,goldstar_video_size);
		memset(dirtybuffer3,1,goldstar_video_size);
	}
}



static const rectangle visible1 = { 14*8, (14+48)*8-1,  4*8,  (4+7)*8-1 };
static const rectangle visible2 = { 14*8, (14+48)*8-1, 12*8, (12+7)*8-1 };
static const rectangle visible3 = { 14*8, (14+48)*8-1, 20*8, (20+7)*8-1 };

/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/
VIDEO_UPDATE( goldstar )
{
	int offs;


//        if (palette_recalc())
	{
		memset(dirtybuffer,1,videoram_size);
		memset(dirtybuffer1,1,goldstar_video_size);
		memset(dirtybuffer2,1,goldstar_video_size);
		memset(dirtybuffer3,1,goldstar_video_size);
	}

	for (offs = videoram_size - 1;offs >= 0;offs--)
	{
		if (dirtybuffer[offs])
		
{ 
struct drawgfxParams dgp0={
	tmpbitmap, 	// dest
	Machine->gfx[0], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	0, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			int sx,sy;


			dirtybuffer[offs] = 0;

			sx = offs % 64;
			sy = offs / 64;

			
			dgp0.code = videoram[offs] + ((colorram[offs] & 0xf0) << 4);
			dgp0.color = colorram[offs] & 0x0f;
			dgp0.sx = 8*sx;
			dgp0.sy = 8*sy;
			drawgfx(&dgp0);
		}
} // end of patch paragraph

	}

	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);


	for (offs = goldstar_video_size - 1;offs >= 0;offs--)
	{
		int sx = offs % 64;
		int sy = offs / 64;


		if (dirtybuffer1[offs])
		
{ 
struct drawgfxParams dgp1={
	tmpbitmap1, 	// dest
	Machine->gfx[1], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	0, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			dirtybuffer1[offs] = 0;

			
			dgp1.code = goldstar_video1[offs];
			dgp1.color = bgcolor;
			dgp1.sx = sx*8;
			dgp1.sy = sy*32;
			drawgfx(&dgp1);
		}
} // end of patch paragraph


		if (dirtybuffer2[offs])
		
{ 
struct drawgfxParams dgp2={
	tmpbitmap2, 	// dest
	Machine->gfx[1], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	0, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			dirtybuffer2[offs] = 0;

			
			dgp2.code = goldstar_video2[offs];
			dgp2.color = bgcolor;
			dgp2.sx = sx*8;
			dgp2.sy = sy*32;
			drawgfx(&dgp2);
		}
} // end of patch paragraph


		if (dirtybuffer3[offs])
		
{ 
struct drawgfxParams dgp3={
	tmpbitmap3, 	// dest
	Machine->gfx[1], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	0, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			dirtybuffer3[offs] = 0;

			
			dgp3.code = goldstar_video3[offs];
			dgp3.color = bgcolor;
			dgp3.sx = sx*8;
			dgp3.sy = sy*32;
			drawgfx(&dgp3);
		}
} // end of patch paragraph

	}

	{
		int i,scrolly[64];

		for (i= 0;i < 64;i++)
			scrolly[i] = -goldstar_scroll1[i];

		copyscrollbitmap(bitmap,tmpbitmap1,0,0,64,scrolly,&visible1,TRANSPARENCY_NONE,0);
                copybitmap(bitmap,tmpbitmap,0,0,0,0,&visible1,TRANSPARENCY_PEN,0);

		for (i= 0;i < 64;i++)
			scrolly[i] = -goldstar_scroll2[i];

		copyscrollbitmap(bitmap,tmpbitmap2,0,0,64,scrolly,&visible2,TRANSPARENCY_NONE,0);
                copybitmap(bitmap,tmpbitmap,0,0,0,0,&visible2,TRANSPARENCY_PEN,0);

		for (i= 0;i < 64;i++)
			scrolly[i] = -goldstar_scroll3[i];

		copyscrollbitmap(bitmap,tmpbitmap3,0,0,64,scrolly,&visible3,TRANSPARENCY_NONE,0);
                copybitmap(bitmap,tmpbitmap,0,0,0,0,&visible3,TRANSPARENCY_PEN,0);
	}
}
