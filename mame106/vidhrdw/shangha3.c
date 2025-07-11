/***************************************************************************

Custom blitter GA9201 KA01-0249 (120pin IC)


This is a tile-based blitter that writes to a frame buffer. The buffer is
never cleared, so stuff drawn is left over from frame to frame until it is
overwritten.

Tiles are stored in ROM and have a fixed 16x16 size. The blitter can draw them
as single sprites (composed of one or more tiles in the horizontal direction),
or as larger sprites whose tile codes are picked from a tilemap in RAM.

Sprites can be zoomed, distorted and rotated.

Shadows are supported on pen 14 (shangha3 makes heavy use of them) but it's not
clear how they are turned on and off. heberpop definitely doesn't use them,
pen 14 is supposed to be solid black (outlines).

The chip addresses 0x100000 bytes of memory, containing both the command list,
tilemap, and spare RAM to be used by the CPU.

The command list starts at a variable point in memory, set by
shangha3_gfxlist_addr_w(), and always ends at 0x0fffff.

Large sprites refer to a single tilemap starting at 0x000000, the command list
contains the coordinates of the place in the tilemap to pick data from.

The commands list is processed when shangha3_blitter_go_w() is written to, so
it is not processed automatically every frame.

The commands have a fixed length of 16 words. The format is as follows:

Word | Bit(s)           | Use
-----+-fedcba9876543210-+----------------
  0  | ---------------- | unused?
  1  | xxxx------------ | high bits of tile #; for tilemaps, this is applied to all tiles
  1  | ----xxxxxxxxxxxx | low bits of tile #; probably unused for tilemaps
  2  | ---xxxxxxxxx---- | x coordinate of destination top left corner
  3  | ---xxxxxxxxx---- | y coordinate of destination top left corner
  4  | ------------x--- | 0 = use code as-is  1 = fetch codes from tilemap RAM
  4  | -------------x-- | 1 = draw "compressed" tilemap, as if tiles were 8x8 instead of 16x16
  4  | --------------x- | flip y
  4  | ---------------x | flip x
  5  | --------x------- | unknown (used by blocken)
  5  | ---------xxx---- | high bits of color #; for tilemaps, this is applied to all tiles
  5  | ------------xxxx | low bits of color #; probably unused for tilemaps
  6  | xxxxxxxxxxxxxxxx | width-1 of destination rectangle    \ if *both* of these are 0,
  7  | xxxxxxxxxxxxxxxx | height-1 of destination rectangle   / disable sprite
  8  | xxxxxxxxxxxxxxxx | x coordinate*0x10 of source top left corner (tilemaps only?)
  9  | xxxxxxxxxxxxxxxx | y coordinate*0x10 of source top left corner (tilemaps only?)
  a  | xxxxxxxxxxxxxxxx | x zoom
  b  | xxxxxxxxxxxxxxxx | rotation
  c  | xxxxxxxxxxxxxxxx | rotation
  d  | xxxxxxxxxxxxxxxx | x zoom
  e  | ---------------- | unknown
  f  | ---------------- | unknown

***************************************************************************/

#include "driver.h"
#include "profiler.h"


UINT16 *shangha3_ram;
size_t shangha3_ram_size;

int shangha3_do_shadows;

static UINT16 gfxlist_addr;
static mame_bitmap *rawbitmap;



VIDEO_START( shangha3 )
{
	if ((rawbitmap = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)
		return 1;

	if (shangha3_do_shadows)
	{
		int i;

		/* Prepare the shadow table */
		/* We draw in a raw bitmap so we don't have to remap pens through Machine->pens */
		for (i = 0;i < 14;i++)
			gfx_drawmode_table[i] = DRAWMODE_SOURCE;
		gfx_drawmode_table[14] = DRAWMODE_SHADOW;
		for (i = 0;i < 128;i++)
			palette_shadow_table[Machine->pens[i]] = Machine->pens[i+128];
	}

	return 0;
}



WRITE16_HANDLER( shangha3_flipscreen_w )
{
	if (ACCESSING_LSB)
	{
		/* bit 7 flips screen, the rest seems to always be set to 0x7e */
		flip_screen_set(data & 0x80);

		if ((data & 0x7f) != 0x7e) ui_popup("flipscreen_w %02x",data);
	}
}

WRITE16_HANDLER( shangha3_gfxlist_addr_w )
{
	COMBINE_DATA(&gfxlist_addr);
}


WRITE16_HANDLER( shangha3_blitter_go_w )
{
	int offs;


	profiler_mark(PROFILER_VIDEO);



    struct drawgfxParams dgp0={
        rawbitmap, 	// dest
        Machine->gfx[0], 	// gfx
        0, 	// code
        0, 	// color
        0, 	// flipx
        0, 	// flipy
        0, 	// sx
        0, 	// sy
        NULL, 	// clip
        TRANSPARENCY_PEN, 	// transparency
        15, 	// transparent_color
        0, 	// scalex
        0, 	// scaley
        NULL, 	// pri_buffer
        0 	// priority_mask
      };
    struct drawgfxParams dgpz0={
        rawbitmap, 	// dest
        Machine->gfx[0], 	// gfx
        0, 	// code
        0, 	// color
        0, 	// flipx
        0, 	// flipy
        0, 	// sx
        0, 	// sy
        NULL, 	// clip
        shangha3_do_shadows ? TRANSPARENCY_PEN_TABLE : TRANSPARENCY_PEN, // , 	// transparency
        15, 	// transparent_color
        0x00010000, 	// scalex
        0x00010000, 	// scaley
        NULL, 	// pri_buffer
        0 	// priority_mask
      };

	for (offs = gfxlist_addr << 3;offs < shangha3_ram_size/2;offs += 16)
	{
		int sx,sy,x,y,code,color,flipx,flipy,sizex,sizey,zoomx,zoomy;


		code = shangha3_ram[offs+1];
		color = shangha3_ram[offs+5] & 0x7f;
		flipx = shangha3_ram[offs+4] & 0x01;
		flipy = shangha3_ram[offs+4] & 0x02;
		sx = (shangha3_ram[offs+2] & 0x1ff0) >> 4;
		if (sx >= 0x180) sx -= 0x200;
		sy = (shangha3_ram[offs+3] & 0x1ff0) >> 4;
		if (sy >= 0x100) sy -= 0x200;
		sizex = shangha3_ram[offs+6];
		sizey = shangha3_ram[offs+7];
		zoomx = shangha3_ram[offs+10];
		zoomy = shangha3_ram[offs+13];

		if (flip_screen)
		{
			sx = 383 - sx - sizex;
			sy = 255 - sy - sizey;
			flipx = !flipx;
			flipy = !flipy;
		}

		if ((sizex || sizey)
				/* avoid garbage on startup */
&& sizex < 512 && sizey < 256 && zoomx < 0x1f0 && zoomy < 0x1f0)
		{
			rectangle myclip;
            dgp0.clip = &myclip;
            dgpz0.clip = &myclip;
//if (shangha3_ram[offs+11] || shangha3_ram[offs+12])
//logerror("offs %04x: sx %04x sy %04x zoom %04x %04x %04x %04x fx %d fy %d\n",offs,sx,sy,zoomx,shangha3_ram[offs+11]),shangha3_ram[offs+12],zoomy,flipx,flipy);

			myclip.min_x = sx;
			myclip.max_x = sx + sizex;
			myclip.min_y = sy;
			myclip.max_y = sy + sizey;

			if (shangha3_ram[offs+4] & 0x08)	/* tilemap */
			{
				int srcx,srcy,dispx,dispy,w,h,condensed;

				condensed = shangha3_ram[offs+4] & 0x04;

				srcx = shangha3_ram[offs+8]/16;
				srcy = shangha3_ram[offs+9]/16;
				dispx = srcx & 0x0f;
				dispy = srcy & 0x0f;

				h = (sizey+15)/16+1;
				w = (sizex+15)/16+1;

				if (condensed)
				{
					h *= 2;
					w *= 2;
					srcx /= 8;
					srcy /= 8;
				}
				else
				{
					srcx /= 16;
					srcy /= 16;
				}


				for (y = 0;y < h;y++)
				{
					for (x = 0;x < w;x++)
					{
						int dx,dy,tile;

						if (condensed)
						{
							int addr = ((y+srcy) & 0x1f) +
										0x20 * ((x+srcx) & 0xff);
							tile = shangha3_ram[addr];
							dx = 8*x*(0x200-zoomx)/0x100 - dispx;
							dy = 8*y*(0x200-zoomy)/0x100 - dispy;
						}
						else
						{
							int addr = ((y+srcy) & 0x0f) +
										0x10 * ((x+srcx) & 0xff) +
										0x100 * ((y+srcy) & 0x10);
							tile = shangha3_ram[addr];
							dx = 16*x*(0x200-zoomx)/0x100 - dispx;
							dy = 16*y*(0x200-zoomy)/0x100 - dispy;
						}

						if (flipx) dx = sx + sizex-15 - dx;
						else dx = sx + dx;
						if (flipy) dy = sy + sizey-15 - dy;
						else dy = sy + dy;

						
						dgp0.code = (tile & 0x0fff) | (code & 0xf000);
						dgp0.color = (tile >> 12) | (color & 0x70);
						dgp0.flipx = flipx;
						dgp0.flipy = flipy;
						dgp0.sx = dx;
						dgp0.sy = dy;
						drawgfx(&dgp0);
					}
				}


			}
			else	/* sprite */
			{
				int w;
                if (zoomx <= 1 && zoomy <= 1)
                {
                    dgpz0.code = code;
                    dgpz0.color = color;
                    dgpz0.flipx = flipx;
                    dgpz0.flipy = flipy;
                    dgpz0.sx = sx;
                    dgpz0.sy = sy;

                    dgpz0.scalex = 0x1000000;
                    dgpz0.scaley = 0x1000000;
                    drawgfxzoom(&dgpz0);
                }
                else
                        {
				w = (sizex+15)/16;

				
				{ 
				struct drawgfxParams dgpz1={
					rawbitmap, 	// dest
					Machine->gfx[0], 	// gfx
					0, 	// code
					0, 	// color
					0, 	// flipx
					0, 	// flipy
					0, 	// sx
					0, 	// sy
					&myclip, 	// clip
					shangha3_do_shadows ? TRANSPARENCY_PEN_TABLE : TRANSPARENCY_PEN, 	// transparency
					15, 	// transparent_color
					0x00010000, 	// scalex
					0x00010000, 	// scaley
					NULL, 	// pri_buffer
					0 	// priority_mask
				  };
				for (x = 0;x < w;x++)
				{
					
					dgpz1.code = code;
					dgpz1.color = color;
					dgpz1.flipx = flipx;
					dgpz1.flipy = flipy;
					dgpz1.sx = sx + 16*x;
					dgpz1.sy = sy;
					dgpz1.scalex = (0x200-zoomx)*0x100;
					dgpz1.scaley = (0x200-zoomy)*0x100;
					drawgfxzoom(&dgpz1);

					if ((code & 0x000f) == 0x0f)
						code = (code + 0x100) & 0xfff0;
					else
						code++;
				}
				} // end of patch paragraph

                }
			}


		}
	}

	profiler_mark(PROFILER_END);
}


VIDEO_UPDATE( shangha3 )
{
	copybitmap(bitmap,rawbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
}
