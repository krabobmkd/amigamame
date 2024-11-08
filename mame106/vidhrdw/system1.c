/*************************************************************************

  System1 / System 2.   By Jarek Parchanski & Mirko Buffoni.

  Many thanks to Roberto Ventura, for precious information about
  System 1 hardware.

*************************************************************************/

#include "system1.h"

unsigned char *system1_scroll_y;
unsigned char *system1_scroll_x;
unsigned char *system1_videoram;
unsigned char *system1_backgroundram;
unsigned char *system1_sprites_collisionram;
unsigned char *system1_background_collisionram;
unsigned char *system1_scrollx_ram;
size_t system1_videoram_size;
size_t system1_backgroundram_size;

static UINT8 *sprite_onscreen_map;
static int background_scrollx=0,background_scrolly=0;
static UINT8 *bg_dirtybuffer;

static int scrollx_row[32];
static mame_bitmap *tmp_bitmap;

//static int system1_pixel_mode = 0
static UINT8 system1_background_memory,system1_video_mode=0;

static const unsigned char *system1_color_prom;

static UINT8 *wbml_paged_videoram;
static UINT8 wbml_videoram_bank=0,wbml_videoram_bank_latch=0;

static int blockgal_kludgeoffset;
static int system1_sprite_xoffset = 0;

/***************************************************************************

  There are two kind of color handling: in the System 1 games, values in the
  palette RAM are directly mapped to colors with the usual BBGGGRRR format;
  in the System 2 ones (Choplifter, WBML, etc.), the value in the palette RAM
  is a lookup offset for three palette PROMs in RRRRGGGGBBBB format.

  It's hard to tell for sure because they use resistor packs, but here's
  what I think the values are from measurment with a volt meter:

  Blue: .250K ohms
  Blue: .495K ohms
  Green:.250K ohms
  Green:.495K ohms
  Green:.995K ohms
  Red:  .495K ohms
  Red:  .250K ohms
  Red:  .995K ohms

  accurate to +/- .003K ohms.

***************************************************************************/
PALETTE_INIT( system1 )
{
	system1_color_prom = color_prom;
}

WRITE8_HANDLER( system1_paletteram_w )
{
	int val,r,g,b;


	paletteram[offset] = data;

	if (system1_color_prom)
	{
		int bit0,bit1,bit2,bit3;

		val = system1_color_prom[data+0*256];
		bit0 = (val >> 0) & 0x01;
		bit1 = (val >> 1) & 0x01;
		bit2 = (val >> 2) & 0x01;
		bit3 = (val >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		val = system1_color_prom[data+1*256];
		bit0 = (val >> 0) & 0x01;
		bit1 = (val >> 1) & 0x01;
		bit2 = (val >> 2) & 0x01;
		bit3 = (val >> 3) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		val = system1_color_prom[data+2*256];
		bit0 = (val >> 0) & 0x01;
		bit1 = (val >> 1) & 0x01;
		bit2 = (val >> 2) & 0x01;
		bit3 = (val >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
	}
	else
	{
		val = (data >> 0) & 0x07;
		r = (val << 5) | (val << 2) | (val >> 1);

		val = (data >> 3) & 0x07;
		g = (val << 5) | (val << 2) | (val >> 1);

		val = (data >> 5) & 0x06;
		if (val) val++;
		b = (val << 5) | (val << 2) | (val >> 1);
	}

	palette_set_color(offset,r,g,b);
}

void system1_postload(void)
{
	memset(sprite_onscreen_map, 255, 256*256);
	memset(bg_dirtybuffer, 1, 1024);
}

VIDEO_START( system1 )
{
	sprite_onscreen_map = auto_malloc(256*256);
	memset(sprite_onscreen_map,255,256*256);

	bg_dirtybuffer = auto_malloc(1024);
	memset(bg_dirtybuffer,1,1024);

	if ((tmp_bitmap = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)
		return 1;

	state_save_register_func_postload(system1_postload);
	state_save_register_global(system1_background_memory);
	state_save_register_global(system1_video_mode);

	system1_sprite_xoffset = 1;

	return 0;
}

VIDEO_START( wbml )
{
	sprite_onscreen_map = auto_malloc(256*256);
	memset(sprite_onscreen_map,255,256*256);

	bg_dirtybuffer = auto_malloc(1024);
	memset(bg_dirtybuffer,1,1024);
	wbml_paged_videoram = auto_malloc(0x4000);	/* Allocate 16k for background banked ram */
	memset(wbml_paged_videoram,0,0x4000);

	if ((tmp_bitmap = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)
		return 1;

	system1_sprite_xoffset = 1+7*2;

	state_save_register_func_postload(system1_postload);
	state_save_register_global(system1_background_memory);
	state_save_register_global(system1_video_mode);

	state_save_register_global_pointer(wbml_paged_videoram, 0x4000);
	state_save_register_global(wbml_videoram_bank);
	state_save_register_global(wbml_videoram_bank_latch);

	return 0;
}

WRITE8_HANDLER( system1_videomode_w )
{
if (data & 0x6e) loginfo(2,"videomode = %02x\n",data);

	/* bit 0 is coin counter */
	coin_counter_w(0, data & 1);

	/* bit 4 is screen blank */
	system1_video_mode = data;

	/* bit 7 is flip screen */
	flip_screen_set(data & 0x80);
}

READ8_HANDLER( system1_videomode_r )
{
	return system1_video_mode;
}

void system1_define_background_memory(int mode)
{
	system1_background_memory = mode;
}


INLINE int get_sprite_bottom_y(int spr_number)
{
	return  spriteram[0x10 * spr_number + SPR_Y_BOTTOM];
}

INLINE void draw_pixel(mame_bitmap *bitmap,
				  	   int x,int y,int x_flipped,int y_flipped,
				  	   int spr_number,int color)
{
	int xr,yr;
	int sprite_onscreen;


	if (x < 0 || x >= Machine->drv->screen_width ||
		y < 0 || y >= Machine->drv->screen_height)
		return;

	if (sprite_onscreen_map[256*y+x] != 255)
	{
		sprite_onscreen = sprite_onscreen_map[256*y+x];
		system1_sprites_collisionram[sprite_onscreen + 32 * spr_number] = 0xff;
	}

	sprite_onscreen_map[256*y+x] = spr_number;

	if (x_flipped >= Machine->visible_area.min_x ||
		x_flipped <= Machine->visible_area.max_x ||
		y_flipped >= Machine->visible_area.min_y ||
		y_flipped <= Machine->visible_area.max_y)
	{
		plot_pixel(bitmap, x_flipped, y_flipped, color);
	}

	xr = ((x - background_scrollx) & 0xff) / 8;
	yr = ((y - background_scrolly) & 0xff) / 8;

	/* TODO: bits 5 and 6 of backgroundram are also used (e.g. Pitfall2, Mr. Viking) */
	/* what's the difference? Bit 7 is used in Choplifter/WBML for extra char bank */
	/* selection, but it is also set in Pitfall2 */

	if (system1_background_memory == system1_BACKGROUND_MEMORY_SINGLE)
	{
		if (system1_backgroundram[2 * (32 * yr + xr) + 1] & 0x10)
			system1_background_collisionram[0x20 + spr_number] = 0xff;
	}
	else
	{
		/* TODO: I should handle the paged background memory here. */
		/* maybe collision detection is not used by the paged games */
		/* (wbml and tokisens), though tokisens doesn't play very well */
		/* (you can't seem to fit in gaps where you should fit) */
	}

	/* TODO: collision should probably be checked with the foreground as well */
	/* (TeddyBoy Blues, head of the tiger in girl bonus round) */
}

WRITE8_HANDLER( system1_background_collisionram_w )
{
	/* to do the RAM check, Mister Viking writes 0xff and immediately */
	/* reads it back, expecting bit 0 to be NOT set. */
	system1_background_collisionram[offset] = 0x7e;
}

WRITE8_HANDLER( system1_sprites_collisionram_w )
{
	/* to do the RAM check, Mister Viking write 0xff and immediately */
	/* reads it back, expecting bit 0 to be NOT set. */
	/* Up'n Down expects to find 0x7e at f800 before doing the whole */
	/* collision test */
	system1_sprites_collisionram[offset] = 0x7e;
}

static void draw_sprite(mame_bitmap *bitmap,int spr_number)
{
	int sy,row,height,src,bank;
	unsigned char *sprite_base;
	pen_t *sprite_palette;
	INT16 skip;	/* bytes to skip before drawing each row (can be negative) */
	unsigned char *gfx;


	sprite_base	= spriteram + 0x10 * spr_number;

	src = sprite_base[SPR_GFXOFS_LO] + (sprite_base[SPR_GFXOFS_HI] << 8);
	bank = 0x8000 * (((sprite_base[SPR_X_HI] & 0x80) >> 7) + ((sprite_base[SPR_X_HI] & 0x40) >> 5));
	bank &= (memory_region_length(REGION_GFX2)-1);	/* limit to the range of available ROMs */
	skip = sprite_base[SPR_SKIP_LO] + (sprite_base[SPR_SKIP_HI] << 8);

	height = sprite_base[SPR_Y_BOTTOM] - sprite_base[SPR_Y_TOP];
	sprite_palette = Machine->remapped_colortable + 0x10 * spr_number;

	sy = sprite_base[SPR_Y_TOP] + 1;

	/* graphics region #2 contains the packed sprite data */
	gfx = &memory_region(REGION_GFX2)[bank];

	for (row = 0;row < height;row++)
	{
		int x,x_flipped;
		int y,y_flipped;
		int src2;

		src = src2 = src + skip;

		x = sprite_base[SPR_X_LO] + ((sprite_base[SPR_X_HI] & 0x01) << 8) + system1_sprite_xoffset;
		x_flipped = x;
		y = y_flipped = sy+row;

		if (flip_screen)
		{
			y_flipped = 258 - sy - height + row;
			x_flipped = (252*2) - x;
		}

		x /= 2;	/* the hardware has sub-pixel placement, it seems */
		x_flipped /= 2;

		while (1)
		{
			int color1,color2,data;

			data = gfx[src2 & 0x7fff];

			if (src & 0x8000)
			{
				src2--;

				color1 = data & 0x0f;
				color2 = data >> 4;
			}
			else
			{
				src2++;

				color1 = data >> 4;
				color2 = data & 0x0f;
			}

			if (color1 == 15) break;
			if (color1)
				draw_pixel(bitmap,x,y,x_flipped,y_flipped,spr_number,sprite_palette[color1]);
			x++;
			x_flipped += flip_screen ? -1 : 1;

			if (color2 == 15) break;
			if (color2)
				draw_pixel(bitmap,x,y,x_flipped,y_flipped,spr_number,sprite_palette[color2]);
			x++;
			x_flipped += flip_screen ? -1 : 1;
		}
	}
}


static void draw_sprites(mame_bitmap *bitmap)
{
	int spr_number,sprite_bottom_y,sprite_top_y;
	unsigned char *sprite_base;


	memset(sprite_onscreen_map,255,256*256);

	for (spr_number = 0;spr_number < 32;spr_number++)
	{
		sprite_base = spriteram + 0x10 * spr_number;
		sprite_top_y = sprite_base[SPR_Y_TOP];
		sprite_bottom_y = sprite_base[SPR_Y_BOTTOM];
		if (sprite_bottom_y && (sprite_bottom_y-sprite_top_y > 0))
			draw_sprite(bitmap,spr_number);
	}
}



WRITE8_HANDLER( system1_backgroundram_w )
{
	system1_backgroundram[offset] = data;
	bg_dirtybuffer[offset>>1] = 1;
}


static int system1_draw_fg(mame_bitmap *bitmap,int priority)
{
	int sx,sy,offs;
	int drawn = 0;


	priority <<= 3;

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < system1_videoram_size;offs += 2)
	{
		if ((system1_videoram[offs+1] & 0x08) == priority)
		{
			int code,color;


			code = (system1_videoram[offs] | (system1_videoram[offs+1] << 8));
			code = ((code >> 4) & 0x800) | (code & 0x7ff);	/* Heavy Metal only */
			color = ((code >> 5) & 0x3f);
			sx = (offs/2) % 32;
			sy = (offs/2) / 32;

			if (flip_screen)
			{
				sx = 31 - sx;
				sy = 31 - sy;
			}

			code %= Machine->gfx[0]->total_elements;
			if (Machine->gfx[0]->pen_usage[code] & ~1)
			{
				drawn = 1;

				
				dgp0.code = code;
				dgp0.color = color;
				dgp0.flipx = flip_screen;
				dgp0.flipy = flip_screen;
				dgp0.sx = 8*sx + blockgal_kludgeoffset;
				dgp0.sy = 8*sy;
				drawgfx(&dgp0);
			}
		}
	}
	} // end of patch paragraph


	return drawn;
}

static void system1_draw_bg(mame_bitmap *bitmap,int priority)
{
	int sx,sy,offs;
	int background_scrollx_flip, background_scrolly_flip;


	background_scrollx = ((system1_scroll_x[0] >> 1) + ((system1_scroll_x[1] & 1) << 7) + 14 + 2*blockgal_kludgeoffset) & 0xff;
	background_scrolly = (-*system1_scroll_y) & 0xff;

	background_scrollx_flip = (275 - background_scrollx) & 0xff;
	background_scrolly_flip = (256 - background_scrolly) & 0xff;

	if (priority == -1)
	{
		/* optimized far background */

		/* for every character in the background video RAM, check if it has
         * been modified since last time and update it accordingly.
         */

		
		{ 
		struct drawgfxParams dgp1={
			tmp_bitmap, 	// dest
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
		for (offs = 0;offs < system1_backgroundram_size;offs += 2)
		{
			if (bg_dirtybuffer[offs / 2])
			{
				int code,color;


				bg_dirtybuffer[offs / 2] = 0;

				code = (system1_backgroundram[offs] | (system1_backgroundram[offs+1] << 8));
				code = ((code >> 4) & 0x800) | (code & 0x7ff);	/* Heavy Metal only */
				color = ((code >> 5) & 0x3f) + 0x40;
				sx = (offs/2) % 32;
				sy = (offs/2) / 32;

				if (flip_screen)
				{
					sx = 31 - sx;
					sy = 31 - sy;
				}

				
				dgp1.code = code;
				dgp1.color = color;
				dgp1.flipx = flip_screen;
				dgp1.flipy = flip_screen;
				dgp1.sx = 8*sx;
				dgp1.sy = 8*sy;
				drawgfx(&dgp1);
			}
		}
		} // end of patch paragraph


		/* copy the temporary bitmap to the screen */
		if (flip_screen)
			copyscrollbitmap(bitmap,tmp_bitmap,1,&background_scrollx_flip,1,&background_scrolly_flip,&Machine->visible_area,TRANSPARENCY_NONE,0);
		else
			copyscrollbitmap(bitmap,tmp_bitmap,1,&background_scrollx,1,&background_scrolly,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}
	else
	{
		priority <<= 3;

		
		{ 
		struct drawgfxParams dgp2={
			bitmap, 	// dest
			Machine->gfx[0], 	// gfx
			0, 	// code
			0, 	// color
			0, 	// flipx
			0, 	// flipy
			0, 	// sx
			0, 	// sy
			&Machine->visible_area, 	// clip
			TRANSPARENCY_PEN, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (offs = 0;offs < system1_backgroundram_size;offs += 2)
		{
			if ((system1_backgroundram[offs+1] & 0x08) == priority)
			{
				int code,color;


				code = (system1_backgroundram[offs] | (system1_backgroundram[offs+1] << 8));
				code = ((code >> 4) & 0x800) | (code & 0x7ff);	/* Heavy Metal only */
				color = ((code >> 5) & 0x3f) + 0x40;
				sx = (offs/2) % 32;
				sy = (offs/2) / 32;

				if (flip_screen)
				{
					sx = 8*(31-sx) + background_scrollx_flip;
					sy = 8*(31-sy) + background_scrolly_flip;
				}
				else
				{
					sx = 8*sx + background_scrollx;
					sy = 8*sy + background_scrolly;
				}

				/* draw it 4 times because of possible wrap around */
				
				dgp2.code = code;
				dgp2.color = color;
				dgp2.flipx = flip_screen;
				dgp2.flipy = flip_screen;
				dgp2.sx = sx;
				dgp2.sy = sy;
				drawgfx(&dgp2);
				
				dgp2.code = code;
				dgp2.color = color;
				dgp2.flipx = flip_screen;
				dgp2.flipy = flip_screen;
				dgp2.sx = sx-256;
				dgp2.sy = sy;
				drawgfx(&dgp2);
				
				dgp2.code = code;
				dgp2.color = color;
				dgp2.flipx = flip_screen;
				dgp2.flipy = flip_screen;
				dgp2.sx = sx;
				dgp2.sy = sy-256;
				drawgfx(&dgp2);
				
				dgp2.code = code;
				dgp2.color = color;
				dgp2.flipx = flip_screen;
				dgp2.flipy = flip_screen;
				dgp2.sx = sx-256;
				dgp2.sy = sy-256;
				drawgfx(&dgp2);
			}
		}
		} // end of patch paragraph

	}
}

VIDEO_UPDATE( system1 )
{
	int drawn;


	system1_draw_bg(bitmap,-1);
	drawn = system1_draw_fg(bitmap,0);
	/* redraw low priority bg tiles if necessary */
	if (drawn) system1_draw_bg(bitmap,0);
	draw_sprites(bitmap);
	system1_draw_bg(bitmap,1);
	system1_draw_fg(bitmap,1);

	/* even if screen is off, sprites must still be drawn to update the collision table */
	if (system1_video_mode & 0x10)  /* screen off */
		fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);
}









WRITE8_HANDLER( choplifter_scroll_x_w )
{
	system1_scrollx_ram[offset] = data;

	scrollx_row[offset/2] = (system1_scrollx_ram[offset & ~1] >> 1) + ((system1_scrollx_ram[offset | 1] & 1) << 7);
}

static void chplft_draw_bg(mame_bitmap *bitmap, int priority)
{
	int sx,sy,offs;
	int choplifter_scroll_x_on = (system1_scrollx_ram[0] == 0xe5 && system1_scrollx_ram[1] == 0xff) ? 0 : 1;


	if (priority == -1)
	{
		/* optimized far background */

		/* for every character in the background video RAM, check if it has
         * been modified since last time and update it accordingly.
         */

		
		{ 
		struct drawgfxParams dgp6={
			tmp_bitmap, 	// dest
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
		for (offs = 0;offs < system1_backgroundram_size;offs += 2)
		{
			if (bg_dirtybuffer[offs / 2])
			{
				int code,color;


				bg_dirtybuffer[offs / 2] = 0;

				code = (system1_backgroundram[offs] | (system1_backgroundram[offs+1] << 8));
				code = ((code >> 4) & 0x800) | (code & 0x7ff);	/* Heavy Metal only */
				color = ((code >> 5) & 0x3f) + 0x40;
				sx = (offs/2) % 32;
				sy = (offs/2) / 32;

				if (flip_screen)
				{
					sx = 31 - sx;
					sy = 31 - sy;
				}

				
				dgp6.code = code;
				dgp6.color = color;
				dgp6.flipx = flip_screen;
				dgp6.flipy = flip_screen;
				dgp6.sx = 8*sx;
				dgp6.sy = 8*sy;
				drawgfx(&dgp6);
			}
		}
		} // end of patch paragraph


		/* copy the temporary bitmap to the screen */
		if (choplifter_scroll_x_on)
		{
			if (flip_screen)
			{
				int scrollx_row_flip[32],i;

				for (i = 0; i < 32; i++)
					scrollx_row_flip[31-i] = (256-scrollx_row[i]) & 0xff;

				copyscrollbitmap(bitmap,tmp_bitmap,32,scrollx_row_flip,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
			}
			else
				copyscrollbitmap(bitmap,tmp_bitmap,32,scrollx_row,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
		}
		else
			copybitmap(bitmap,tmp_bitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
	}
	else
	{
		priority <<= 3;

		
		{ 
		struct drawgfxParams dgp7={
			bitmap, 	// dest
			Machine->gfx[0], 	// gfx
			0, 	// code
			0, 	// color
			0, 	// flipx
			0, 	// flipy
			0, 	// sx
			0, 	// sy
			&Machine->visible_area, 	// clip
			TRANSPARENCY_PEN, 	// transparency
			0, 	// transparent_color
			0, 	// scalex
			0, 	// scaley
			NULL, 	// pri_buffer
			0 	// priority_mask
		  };
		for (offs = 0;offs < system1_backgroundram_size;offs += 2)
		{
			if ((system1_backgroundram[offs+1] & 0x08) == priority)
			{
				int code,color;


				code = (system1_backgroundram[offs] | (system1_backgroundram[offs+1] << 8));
				code = ((code >> 4) & 0x800) | (code & 0x7ff);	/* Heavy Metal only */
				color = ((code >> 5) & 0x3f) + 0x40;
				sx = (offs/2) % 32;
				sy = (offs/2) / 32;

				if (flip_screen)
				{
					sx = 8*(31-sx);

					if (choplifter_scroll_x_on)
						sx = (sx - scrollx_row[sy]) & 0xff;

					sy = 31 - sy;
				}
				else
				{
					sx = 8*sx;

					if (choplifter_scroll_x_on)
						sx = (sx + scrollx_row[sy]) & 0xff;
				}

				
				dgp7.code = code;
				dgp7.color = color;
				dgp7.flipx = flip_screen;
				dgp7.flipy = flip_screen;
				dgp7.sx = sx;
				dgp7.sy = 8*sy;
				drawgfx(&dgp7);
			}
		}
		} // end of patch paragraph

	}
}

VIDEO_UPDATE( choplifter )
{
	int drawn;


	chplft_draw_bg(bitmap,-1);
	drawn = system1_draw_fg(bitmap,0);
	/* redraw low priority bg tiles if necessary */
	if (drawn) chplft_draw_bg(bitmap,0);
	draw_sprites(bitmap);
	chplft_draw_bg(bitmap,1);
	system1_draw_fg(bitmap,1);

	/* even if screen is off, sprites must still be drawn to update the collision table */
	if (system1_video_mode & 0x10)  /* screen off */
		fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);


#ifdef MAME_DEBUG
	if (code_pressed(KEYCODE_SPACE))		// goto next level
	{
		memory_region(REGION_CPU1)[0xC085]=33;
	}
#endif
}



READ8_HANDLER( wbml_videoram_bank_latch_r )
{
	return wbml_videoram_bank_latch;
}

WRITE8_HANDLER( wbml_videoram_bank_latch_w )
{
	wbml_videoram_bank_latch = data;
	wbml_videoram_bank = (data >> 1) & 0x03;	/* Select 4 banks of 4k, bit 2,1 */
}

READ8_HANDLER( wbml_paged_videoram_r )
{
	return wbml_paged_videoram[0x1000*wbml_videoram_bank + offset];
}

WRITE8_HANDLER( wbml_paged_videoram_w )
{
	wbml_paged_videoram[0x1000*wbml_videoram_bank + offset] = data;
}

static void wbml_draw_bg(mame_bitmap *bitmap, int trasp)
{
	int page;

	int xscroll = (wbml_paged_videoram[0x7c0] >> 1) + ((wbml_paged_videoram[0x7c1] & 1) << 7) - 256 + 5;
	int yscroll = -wbml_paged_videoram[0x7ba];

	
	{ 
	struct drawgfxParams dgp8={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	struct drawgfxParams dgp9={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (page=0; page < 4; page++)
	{
		const unsigned char *source = wbml_paged_videoram + (wbml_paged_videoram[0x0740 + page*2] & 0x07)*0x800;
		int startx = (page&1)*256+xscroll;
		int starty = (page>>1)*256+yscroll;
		int row,col;


		for( row=0; row<32*8; row+=8 )
		{
			for( col=0; col<32*8; col+=8 )
			{
				int code,priority;
				int x = (startx+col) & 0x1ff;
				int y = (starty+row) & 0x1ff;
				if (x > 256) x -= 512;
				if (y > 224) y -= 512;

				if (flip_screen)
				{
					x = 248 - x;
					y = 248 - y;
				}

				code = source[0] + (source[1] << 8);
				priority = code & 0x800;
				code = ((code >> 4) & 0x800) | (code & 0x7ff);

				if (!trasp)
                {
					dgp8.code = code;
					dgp8.color = ((code >> 5) & 0x3f) + 64;
					dgp8.flipx = flip_screen;
					dgp8.flipy = flip_screen;
					dgp8.sx = x;
					dgp8.sy = y;
					drawgfx(&dgp8);
                }
				else if (priority)
				{
					dgp9.code = code;
					dgp9.color = ((code >> 5) & 0x3f) + 64;
					dgp9.flipx = flip_screen;
					dgp9.flipy = flip_screen;
					dgp9.sx = x;
					dgp9.sy = y;
					drawgfx(&dgp9);
                }
				source+=2;
			}
		}
	}
	} // end of patch paragraph
 /* next page */
}

static void wbml_draw_fg(mame_bitmap *bitmap)
{
	int offs;


	
	{ 
	struct drawgfxParams dgp10={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < 0x700;offs += 2)
	{
		int sx,sy,code;


		sx = (offs/2) % 32;
		sy = (offs/2) / 32;
		code = wbml_paged_videoram[offs] | (wbml_paged_videoram[offs+1] << 8);
		code = ((code >> 4) & 0x800) | (code & 0x7ff);

		if (flip_screen)
		{
			sx = 31 - sx;
			sy = 31 - sy;
		}

		
		dgp10.code = code;
		dgp10.color = (code >> 5) & 0x3f;
		dgp10.flipx = flip_screen;
		dgp10.flipy = flip_screen;
		dgp10.sx = 8*sx;
		dgp10.sy = 8*sy;
		drawgfx(&dgp10);
	}
	} // end of patch paragraph

}


VIDEO_UPDATE( wbml )
{
	wbml_draw_bg(bitmap,0);
	draw_sprites(bitmap);
	wbml_draw_bg(bitmap,1);
	wbml_draw_fg(bitmap);

	/* even if screen is off, sprites must still be drawn to update the collision table */
	if (system1_video_mode & 0x10)  /* screen off */
		fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);
}

/* same as wbml but with rows scroll */
static void ufosensi_draw_bg(mame_bitmap *bitmap, int trasp)
{
	int page;

	int yscroll = -wbml_paged_videoram[0x7ba];

	
	{ 
	struct drawgfxParams dgp11={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	struct drawgfxParams dgp12={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (page=0; page < 4; page++)
	{
		const unsigned char *source = wbml_paged_videoram + (wbml_paged_videoram[0x0740 + page*2] & 0x07)*0x800;
		int starty = (page>>1)*256+yscroll;
		int row,col;


		for( row=0; row<32*8; row+=8 )
		{
			for( col=0; col<32*8; col+=8 )
			{
				int code,priority;
				int xscroll = (wbml_paged_videoram[0x7c0 + row/4] >> 1) + ((wbml_paged_videoram[0x7c1 + row/4] & 1) << 7) - 256 + 5;
				int startx = (page&1)*256+xscroll;
				int x = (startx+col) & 0x1ff;
				int y = (starty+row) & 0x1ff;
				if (x > 256) x -= 512;
				if (y > 224) y -= 512;

				if (flip_screen)
				{
					x = 248 - x;
					y = 248 - y;
				}

				code = source[0] + (source[1] << 8);
				priority = code & 0x800;
				code = ((code >> 4) & 0x800) | (code & 0x7ff);

				if (!trasp)
				{
					dgp11.code = code;
					dgp11.color = ((code >> 5) & 0x3f) + 64;
					dgp11.flipx = flip_screen;
					dgp11.flipy = flip_screen;
					dgp11.sx = x;
					dgp11.sy = y;
					drawgfx(&dgp11);
                }
				else if (priority)
				{
					dgp12.code = code;
					dgp12.color = ((code >> 5) & 0x3f) + 64;
					dgp12.flipx = flip_screen;
					dgp12.flipy = flip_screen;
					dgp12.sx = x;
					dgp12.sy = y;
					drawgfx(&dgp12);
                }

				source+=2;
			}
		}
	}
	} // end of patch paragraph
 /* next page */
}

VIDEO_UPDATE( ufosensi )
{
	ufosensi_draw_bg(bitmap,0);
	draw_sprites(bitmap);
	ufosensi_draw_bg(bitmap,1);
	wbml_draw_fg(bitmap);

	/* even if screen is off, sprites must still be drawn to update the collision table */
	if (system1_video_mode & 0x10)  /* screen off */
		fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);
}

VIDEO_UPDATE( blockgal )
{
	int drawn;


	blockgal_kludgeoffset = -8;

	system1_draw_bg(bitmap,-1);
	drawn = system1_draw_fg(bitmap,0);
	/* redraw low priority bg tiles if necessary */
	if (drawn) system1_draw_bg(bitmap,0);
	draw_sprites(bitmap);
	system1_draw_bg(bitmap,1);
	system1_draw_fg(bitmap,1);

	/* even if screen is off, sprites must still be drawn to update the collision table */
	if (system1_video_mode & 0x10)  /* screen off */
		fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);

	blockgal_kludgeoffset = 0;
}
