/***************************************************************************

                              -= Tetris Plus 2 =-

                    driver by   Luca Elia (l.elia@tin.it)


Note:   if MAME_DEBUG is defined, pressing Z with:

        Q       shows the background
        W       shows the foreground
        A       shows the sprites

        Keys can be used together!


    [ 2 Scrolling Layers ]

    The Background is a 64 x 64 Tilemap with 16 x 16 x 8 tiles (1024 x 1024).
    The Foreground is a 64 x 64 Tilemap with 8 x 8 x 8 tiles (512 x 512).
    Each tile needs 4 bytes.

    [ 1024? Sprites ]

    Sprites are "nearly" tile based: if the data in the ROMs is decoded
    as 8x8 tiles, and each 32 consecutive tiles are drawn like a column
    of 256 pixels, it turns out that every 32 x 32 tiles form a picture
    of 256 x 256 pixels (a "page").

    Sprites are portions of any size from those page's graphics rendered
    to screen.

To Do:

-   There is a 3rd unimplemented layer capable of rotation (not used by
    the game, can be tested in service mode).
-   Priority RAM is not taken into account.

***************************************************************************/

#include "driver.h"


extern UINT16 tetrisp2_systemregs[0x10];

/* Variables needed by driver: */

UINT16 *tetrisp2_vram_bg, *tetrisp2_scroll_bg;
UINT16 *tetrisp2_vram_fg, *tetrisp2_scroll_fg;
UINT16 *tetrisp2_vram_rot, *tetrisp2_rotregs;

UINT16 *tetrisp2_priority;

UINT16 *rocknms_sub_vram_bg, *rocknms_sub_scroll_bg;
UINT16 *rocknms_sub_vram_fg, *rocknms_sub_scroll_fg;
UINT16 *rocknms_sub_vram_rot, *rocknms_sub_rotregs;

UINT16 *rocknms_sub_priority;

static mame_bitmap *rocknms_main_tmpbitmap;


/***************************************************************************


                                    Palette


***************************************************************************/

/* BBBBBGGGGGRRRRRx xxxxxxxxxxxxxxxx */
WRITE16_HANDLER( tetrisp2_palette_w )
{
	data = COMBINE_DATA(&paletteram16[offset]);
	if ((offset & 1) == 0)
	{
		int r = (data >>  1) & 0x1f;
		int g = (data >>  6) & 0x1f;
		int b = (data >> 11) & 0x1f;
		palette_set_color(offset/2,(r << 3) | (r >> 2),(g << 3) | (g >> 2),(b << 3) | (b >> 2));
	}
}

WRITE16_HANDLER( rocknms_sub_palette_w )
{
	data = COMBINE_DATA(&paletteram16_2[offset]);
	if ((offset & 1) == 0)
	{
		int r = (data >>  1) & 0x1f;
		int g = (data >>  6) & 0x1f;
		int b = (data >> 11) & 0x1f;
		palette_set_color((0x8000 + (offset/2)),(r << 3) | (r >> 2),(g << 3) | (g >> 2),(b << 3) | (b >> 2));
	}
}



/***************************************************************************


                                    Priority


***************************************************************************/

READ16_HANDLER( tetrisp2_priority_r )
{
	return tetrisp2_priority[offset];
}

READ16_HANDLER( rocknms_sub_priority_r )
{
	return rocknms_sub_priority[offset];
}


WRITE16_HANDLER( tetrisp2_priority_w )
{
	if (ACCESSING_MSB)
	{
		data |= ((data & 0xff00) >> 8);
		tetrisp2_priority[offset] = data;
	}
}


WRITE16_HANDLER( rockn_priority_w )
{
	if (ACCESSING_MSB)
	{
		tetrisp2_priority[offset] = data;
	}
}

WRITE16_HANDLER( rocknms_sub_priority_w )
{
	if (ACCESSING_MSB)
	{
		rocknms_sub_priority[offset] = data;
	}
}



/***************************************************************************


                                    Tilemaps


    Offset:     Bits:                   Value:

        0.w                             Code
        2.w     fedc ba98 7654 ----
                ---- ---- ---- 3210     Color


***************************************************************************/

static tilemap *tilemap_bg, *tilemap_fg, *tilemap_rot;
static tilemap *tilemap_sub_bg, *tilemap_sub_fg, *tilemap_sub_rot;

#define NX_0  (0x40)
#define NY_0  (0x40)

static void get_tile_info_bg(int tile_index)
{
	UINT16 code_hi = tetrisp2_vram_bg[ 2 * tile_index + 0];
	UINT16 code_lo = tetrisp2_vram_bg[ 2 * tile_index + 1];
	SET_TILE_INFO(
			1,
			code_hi,
			code_lo & 0xf,
			0)
}

WRITE16_HANDLER( tetrisp2_vram_bg_w )
{
	UINT16 old_data	=	tetrisp2_vram_bg[offset];
	UINT16 new_data	=	COMBINE_DATA(&tetrisp2_vram_bg[offset]);
	if (old_data != new_data)	tilemap_mark_tile_dirty(tilemap_bg,offset/2);
}



#define NX_1  (0x40)
#define NY_1  (0x40)

static void get_tile_info_fg(int tile_index)
{
	UINT16 code_hi = tetrisp2_vram_fg[ 2 * tile_index + 0];
	UINT16 code_lo = tetrisp2_vram_fg[ 2 * tile_index + 1];
	SET_TILE_INFO(
			3,
			code_hi,
			code_lo & 0xf,
			0)
}

WRITE16_HANDLER( tetrisp2_vram_fg_w )
{
	UINT16 old_data	=	tetrisp2_vram_fg[offset];
	UINT16 new_data	=	COMBINE_DATA(&tetrisp2_vram_fg[offset]);
	if (old_data != new_data)	tilemap_mark_tile_dirty(tilemap_fg,offset/2);
}


static void get_tile_info_rot(int tile_index)
{
	UINT16 code_hi = tetrisp2_vram_rot[ 2 * tile_index + 0];
	UINT16 code_lo = tetrisp2_vram_rot[ 2 * tile_index + 1];
	SET_TILE_INFO(
			2,
			code_hi,
			code_lo & 0xf,
			0)
}

WRITE16_HANDLER( tetrisp2_vram_rot_w )
{
	UINT16 old_data	=	tetrisp2_vram_rot[offset];
	UINT16 new_data	=	COMBINE_DATA(&tetrisp2_vram_rot[offset]);
	if (old_data != new_data)	tilemap_mark_tile_dirty(tilemap_rot,offset/2);
}

static void get_tile_info_rocknms_sub_bg(int tile_index)
{
	UINT16 code_hi = rocknms_sub_vram_bg[ 2 * tile_index + 0];
	UINT16 code_lo = rocknms_sub_vram_bg[ 2 * tile_index + 1];
	SET_TILE_INFO(
			5,
			code_hi,
			code_lo & 0xf,
			0)
}

WRITE16_HANDLER( rocknms_sub_vram_bg_w )
{
	UINT16 old_data	=	rocknms_sub_vram_bg[offset];
	UINT16 new_data	=	COMBINE_DATA(&rocknms_sub_vram_bg[offset]);
	if (old_data != new_data)	tilemap_mark_tile_dirty(tilemap_sub_bg,offset/2);
}


static void get_tile_info_rocknms_sub_fg(int tile_index)
{
	UINT16 code_hi = rocknms_sub_vram_fg[ 2 * tile_index + 0];
	UINT16 code_lo = rocknms_sub_vram_fg[ 2 * tile_index + 1];
	SET_TILE_INFO(
			7,
			code_hi,
			code_lo & 0xf,
			0)
}

WRITE16_HANDLER( rocknms_sub_vram_fg_w )
{
	UINT16 old_data	=	rocknms_sub_vram_fg[offset];
	UINT16 new_data	=	COMBINE_DATA(&rocknms_sub_vram_fg[offset]);
	if (old_data != new_data)	tilemap_mark_tile_dirty(tilemap_sub_fg,offset/2);
}


static void get_tile_info_rocknms_sub_rot(int tile_index)
{
	UINT16 code_hi = rocknms_sub_vram_rot[ 2 * tile_index + 0];
	UINT16 code_lo = rocknms_sub_vram_rot[ 2 * tile_index + 1];
	SET_TILE_INFO(
			6,
			code_hi,
			code_lo & 0xf,
			0)
}

WRITE16_HANDLER( rocknms_sub_vram_rot_w )
{
	UINT16 old_data	=	rocknms_sub_vram_rot[offset];
	UINT16 new_data	=	COMBINE_DATA(&rocknms_sub_vram_rot[offset]);
	if (old_data != new_data)	tilemap_mark_tile_dirty(tilemap_sub_rot,offset/2);
}




VIDEO_START( tetrisp2 )
{
	tilemap_bg = tilemap_create(	get_tile_info_bg,tilemap_scan_rows,
								TILEMAP_TRANSPARENT,
								16,16,NX_0,NY_0);

	tilemap_fg = tilemap_create(	get_tile_info_fg,tilemap_scan_rows,
								TILEMAP_TRANSPARENT,
								8,8,NX_1,NY_1);

	tilemap_rot = tilemap_create(	get_tile_info_rot,tilemap_scan_rows,
								TILEMAP_TRANSPARENT,
								16,16,NX_0*2,NY_0*2);

	if (!tilemap_bg || !tilemap_fg || !tilemap_rot)	return 1;

	tilemap_set_transparent_pen(tilemap_bg,0);
	tilemap_set_transparent_pen(tilemap_fg,0);
	tilemap_set_transparent_pen(tilemap_rot,0);

	return 0;
}

VIDEO_START( rockntread )
{
	tilemap_bg = tilemap_create(	get_tile_info_bg,tilemap_scan_rows,
								TILEMAP_TRANSPARENT,
								16, 16, 256, 16);	// rockn ms(main),1,2,3,4

	tilemap_fg = tilemap_create(	get_tile_info_fg,tilemap_scan_rows,
								TILEMAP_TRANSPARENT,
								8, 8, 64, 64);

	tilemap_rot = tilemap_create(	get_tile_info_rot,tilemap_scan_rows,
								TILEMAP_TRANSPARENT,
								16, 16, 128, 128);

	if (!tilemap_bg || !tilemap_fg || !tilemap_rot)	return 1;

	tilemap_set_transparent_pen(tilemap_bg, 0);
	tilemap_set_transparent_pen(tilemap_fg, 0);
	tilemap_set_transparent_pen(tilemap_rot, 0);

	return 0;
}


VIDEO_START( rocknms )
{
	tilemap_bg = tilemap_create(get_tile_info_bg,tilemap_scan_rows,
					TILEMAP_TRANSPARENT,
					16, 16, 256, 16);	// rockn ms(main),1,2,3,4

	tilemap_fg = tilemap_create(get_tile_info_fg,tilemap_scan_rows,
					TILEMAP_TRANSPARENT,
					8, 8, 64, 64);

	tilemap_rot = tilemap_create(get_tile_info_rot,tilemap_scan_rows,
					TILEMAP_TRANSPARENT,
					16, 16, 128, 128);

	tilemap_sub_bg = tilemap_create(get_tile_info_rocknms_sub_bg,tilemap_scan_rows,
					TILEMAP_TRANSPARENT,
					16, 16, 32, 256);	// rockn ms(sub)

	tilemap_sub_fg = tilemap_create(get_tile_info_rocknms_sub_fg,tilemap_scan_rows,
					TILEMAP_TRANSPARENT,
					8, 8, 64, 64);

	tilemap_sub_rot = tilemap_create( get_tile_info_rocknms_sub_rot,tilemap_scan_rows,
					TILEMAP_TRANSPARENT,
					16, 16, 128, 128);

	rocknms_main_tmpbitmap = auto_bitmap_alloc(Machine->drv->screen_width, 224*1);

	if (!tilemap_bg || !tilemap_fg || !tilemap_rot)	return 1;
	if (!tilemap_sub_bg || !tilemap_sub_fg || !tilemap_sub_rot)	return 1;
	if (!rocknms_main_tmpbitmap)	return 1;

	tilemap_set_transparent_pen(tilemap_bg, 0);
	tilemap_set_transparent_pen(tilemap_fg, 0);
	tilemap_set_transparent_pen(tilemap_rot, 0);

	tilemap_set_transparent_pen(tilemap_sub_bg, 0);
	tilemap_set_transparent_pen(tilemap_sub_fg, 0);
	tilemap_set_transparent_pen(tilemap_sub_rot, 0);

	return 0;
}


/***************************************************************************


                                Sprites Drawing

    Offset:     Bits:                   Meaning:

    0.w         fedc ba98 ---- ----
                ---- ---- 7654 ----     Priority
                ---- ---- ---- 3---
                ---- ---- ---- -2--     Draw this sprite
                ---- ---- ---- --1-     Flip Y
                ---- ---- ---- ---0     Flip X

    2.w         fedc ba98 ---- ----     Tile's Y position in the tile page (*)
                ---- ---- 7654 3210     Tile's X position in the tile page (*)

    4.w         fedc ---- ---- ----     Color
                ---- ba98 7--- ----
                ---- ---- -654 3210     Tile Page (32x32 tiles = 256x256 pixels each)

    6.w         fedc ba98 ---- ----
                ---- ---- 7654 ----     Y Size - 1 (*)
                ---- ---- ---- 3210     X Size - 1 (*)

    8.w         fedc ba-- ---- ----
                ---- --98 7654 3210     Y (Signed)

    A.w         fedc b--- ---- ----
                ---- -a98 7654 3210     X (Signed)

    C.w         fedc ba98 7654 3210     ZOOM X (unused)

    E.w         fedc ba98 7654 3210     ZOOM Y (unused)

(*) 1 pixel granularity

***************************************************************************/

static void tetrisp2_draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, UINT16 *sprram_top, size_t sprram_size, int gfxnum)
{
	int x, y, tx, ty, sx, sy, flipx, flipy;
	int xsize, ysize, xnum, ynum;
	int xstart, ystart, xend, yend, xinc, yinc;
	int code, attr, color, size;
	int flipscreen;
	UINT32 primask;
	UINT16 *priority_ram;

	int min_x = cliprect->min_x;
	int max_x = cliprect->max_x;
	int min_y = cliprect->min_y;
	int max_y = cliprect->max_y;

	UINT16		*source	=	sprram_top;
	const UINT16	*finish	=	sprram_top + (sprram_size - 0x10) / 2;

	priority_ram = tetrisp2_priority;

	flipscreen = (tetrisp2_systemregs[0x00] & 0x02);

	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[gfxnum], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		NULL,//&clip, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		priority_bitmap, 	// pri_buffer
		0//primask 	// priority_mask
	  };
	for (; source <= finish; source += 0x10/2 )
	{
		rectangle clip;

		attr	=	source[ 0 ];

		if ((attr & 0x0004) == 0)			continue;

		flipx	=	attr & 1;
		flipy	=	attr & 2;

		code	=	source[ 1 ];
		color	=	source[ 2 ];

		tx		=	(code >> 0) & 0xff;
		ty		=	(code >> 8) & 0xff;

		code	=	(tx / 8) +
					(ty / 8) * (0x100/8) +
					(color & 0x7f) * (0x100/8) * (0x100/8);

		color	=	(color >> 12) & 0xf;

		size	=	source[ 3 ];

		xsize	=	((size >> 0) & 0xff) + 1;
		ysize	=	((size >> 8) & 0xff) + 1;

		xnum	=	( ((tx + xsize) & ~7) + (((tx + xsize) & 7) ? 8 : 0) - (tx & ~7) ) / 8;
		ynum	=	( ((ty + ysize) & ~7) + (((ty + ysize) & 7) ? 8 : 0) - (ty & ~7) ) / 8;

		sy		=	source[ 4 ];
		sx		=	source[ 5 ];

		sx		=	(sx & 0x3ff) - (sx & 0x400);
		sy		=	(sy & 0x1ff) - (sy & 0x200);

		/* Flip Screen */
		if (flipscreen)
		{
			sx = max_x + 1 - sx - xsize;	flipx = !flipx;
			sy = max_y + 1 - sy - ysize;	flipy = !flipy;
		}

		/* Clip the sprite if its width or height is not an integer
           multiple of 8 pixels (1 tile) */

		clip.min_x	=	sx;
		clip.max_x	=	sx + xsize - 1;
		clip.min_y	=	sy;
		clip.max_y	=	sy + ysize - 1;

		if (clip.min_x > max_x)	continue;
		if (clip.max_x < min_x)	continue;

		if (clip.min_y > max_y)	continue;
		if (clip.max_y < min_y)	continue;

		if (clip.min_x < min_x)	clip.min_x = min_x;
		if (clip.max_x > max_x)	clip.max_x = max_x;

		if (clip.min_y < min_y)	clip.min_y = min_y;
		if (clip.max_y > max_y)	clip.max_y = max_y;

		if (flipx)	{ xstart = xnum-1;  xend = -1;    xinc = -1;  sx -= xnum*8 - xsize - (tx & 7); }
		else		{ xstart = 0;       xend = xnum;  xinc = +1;  sx -= tx & 7; }

		if (flipy)	{ ystart = ynum-1;  yend = -1;    yinc = -1;  sy -= ynum*8 - ysize - (ty & 7); }
		else		{ ystart = 0;       yend = ynum;  yinc = +1;  sy -= ty & 7; }

		primask = 0;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x1500) / 2] & 0x38) primask |= 1 << 0;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x1400) / 2] & 0x38) primask |= 1 << 1;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x1100) / 2] & 0x38) primask |= 1 << 2;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x1000) / 2] & 0x38) primask |= 1 << 3;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x0500) / 2] & 0x38) primask |= 1 << 4;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x0400) / 2] & 0x38) primask |= 1 << 5;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x0100) / 2] & 0x38) primask |= 1 << 6;
		if (priority_ram[((attr & 0x00f0) | 0x0a00 | 0x0000) / 2] & 0x38) primask |= 1 << 7;

		for (y = ystart; y != yend; y += yinc)
		{
			for (x = xstart; x != xend; x += xinc)
			{
				
				dgp0.code = code++;
				dgp0.color = color;
				dgp0.flipx = flipx;
				dgp0.flipy = flipy;
                dgp0.clip = &clip;
				dgp0.sx = sx + x * 8;
				dgp0.sy = sy + y * 8;
                dgp0.priority_mask = primask | (1<<31);
				drawgfx(&dgp0);
			}
			code	+=	(0x100/8) - xnum;
		}
	}
	} // end of patch paragraph
	/* end sprite loop */
}


/***************************************************************************


                                Screen Drawing


***************************************************************************/

VIDEO_UPDATE( tetrisp2 )
{
	static int flipscreen_old = -1;
	int flipscreen;
	int asc_pri;
	int scr_pri;
	int rot_pri;
	int rot_ofsx, rot_ofsy;

	flipscreen = (tetrisp2_systemregs[0x00] & 0x02);

	/* Black background color */
	fillbitmap(bitmap, Machine->pens[0x0000], cliprect);
	fillbitmap(priority_bitmap, 0, NULL);

	/* Flip Screen */
	if (flipscreen != flipscreen_old)
	{
		flipscreen_old = flipscreen;
		tilemap_set_flip(ALL_TILEMAPS, flipscreen ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
	}

	/* Flip Screen */
	if (flipscreen)
	{
		rot_ofsx = 0x053f;
		rot_ofsy = 0x04df;
	}
	else
	{
		rot_ofsx = 0x400;
		rot_ofsy = 0x400;
	}

	tilemap_set_scrollx(tilemap_bg, 0, (((tetrisp2_scroll_bg[ 0 ] + 0x0014) + tetrisp2_scroll_bg[ 2 ] ) & 0xffff));
	tilemap_set_scrolly(tilemap_bg, 0, (((tetrisp2_scroll_bg[ 3 ] + 0x0000) + tetrisp2_scroll_bg[ 5 ] ) & 0xffff));

	tilemap_set_scrollx(tilemap_fg, 0, tetrisp2_scroll_fg[ 2 ]);
	tilemap_set_scrolly(tilemap_fg, 0, tetrisp2_scroll_fg[ 5 ]);

	tilemap_set_scrollx(tilemap_rot, 0, (tetrisp2_rotregs[ 0 ] - rot_ofsx));
	tilemap_set_scrolly(tilemap_rot, 0, (tetrisp2_rotregs[ 2 ] - rot_ofsy));

	asc_pri = scr_pri = rot_pri = 0;

	if((tetrisp2_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((tetrisp2_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((tetrisp2_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		tilemap_draw(bitmap,cliprect, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 0)
		tilemap_draw(bitmap,cliprect, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 0)
		tilemap_draw(bitmap,cliprect, tilemap_fg,  0, 1 << 2);

	if (rot_pri == 1)
		tilemap_draw(bitmap,cliprect, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 1)
		tilemap_draw(bitmap,cliprect, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 1)
		tilemap_draw(bitmap,cliprect, tilemap_fg,  0, 1 << 2);

	if (rot_pri == 2)
		tilemap_draw(bitmap,cliprect, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 2)
		tilemap_draw(bitmap,cliprect, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 2)
		tilemap_draw(bitmap,cliprect, tilemap_fg,  0, 1 << 2);

	tetrisp2_draw_sprites(bitmap,cliprect, spriteram16, spriteram_size, 0);
}

VIDEO_UPDATE( rockntread )
{
	static int flipscreen_old = -1;
	int flipscreen;
	int asc_pri;
	int scr_pri;
	int rot_pri;
	int rot_ofsx, rot_ofsy;

	flipscreen = (tetrisp2_systemregs[0x00] & 0x02);

	/* Black background color */
	fillbitmap(bitmap, Machine->pens[0x0000], cliprect);
	fillbitmap(priority_bitmap, 0, NULL);

	/* Flip Screen */
	if (flipscreen != flipscreen_old)
	{
		flipscreen_old = flipscreen;
		tilemap_set_flip(ALL_TILEMAPS, flipscreen ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
	}

	/* Flip Screen */
	if (flipscreen)
	{
		rot_ofsx = 0x053f;
		rot_ofsy = 0x04df;
	}
	else
	{
		rot_ofsx = 0x400;
		rot_ofsy = 0x400;
	}

	tilemap_set_scrollx(tilemap_bg, 0, (((tetrisp2_scroll_bg[ 0 ] + 0x0014) + tetrisp2_scroll_bg[ 2 ] ) & 0xffff));
	tilemap_set_scrolly(tilemap_bg, 0, (((tetrisp2_scroll_bg[ 3 ] + 0x0000) + tetrisp2_scroll_bg[ 5 ] ) & 0xffff));

	tilemap_set_scrollx(tilemap_fg, 0, tetrisp2_scroll_fg[ 2 ]);
	tilemap_set_scrolly(tilemap_fg, 0, tetrisp2_scroll_fg[ 5 ]);

	tilemap_set_scrollx(tilemap_rot, 0, (tetrisp2_rotregs[ 0 ] - rot_ofsx));
	tilemap_set_scrolly(tilemap_rot, 0, (tetrisp2_rotregs[ 2 ] - rot_ofsy));

	asc_pri = scr_pri = rot_pri = 0;

	if((tetrisp2_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((tetrisp2_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((tetrisp2_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		tilemap_draw(bitmap,cliprect, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 0)
		tilemap_draw(bitmap,cliprect, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 0)
		tilemap_draw(bitmap,cliprect, tilemap_fg,  0, 1 << 2);

	if (rot_pri == 1)
		tilemap_draw(bitmap,cliprect, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 1)
		tilemap_draw(bitmap,cliprect, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 1)
		tilemap_draw(bitmap,cliprect, tilemap_fg,  0, 1 << 2);

	if (rot_pri == 2)
		tilemap_draw(bitmap,cliprect, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 2)
		tilemap_draw(bitmap,cliprect, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 2)
		tilemap_draw(bitmap,cliprect, tilemap_fg,  0, 1 << 2);

	tetrisp2_draw_sprites(bitmap,cliprect, spriteram16, spriteram_size, 0);
}


static void video_update_rocknms_0(mame_bitmap *bitmap, const rectangle *cliprect, int mainscr_x, int mainscr_y)
{
	rectangle rocknms_rect_main, rocknms_rect_sub, myclip;
	int asc_pri;
	int scr_pri;
	int rot_pri;

	myclip.min_x = cliprect->min_x;
	myclip.min_y = cliprect->min_y;
	myclip.max_x = cliprect->max_x;
	myclip.max_y = cliprect->max_y;

	tilemap_set_scrollx(tilemap_bg, 0, tetrisp2_scroll_bg[ 2 ] + 0x000);
	tilemap_set_scrolly(tilemap_bg, 0, tetrisp2_scroll_bg[ 5 ] + 0x000);

	tilemap_set_scrollx(tilemap_fg, 0, tetrisp2_scroll_fg[ 2 ] + 0x000);
	tilemap_set_scrolly(tilemap_fg, 0, tetrisp2_scroll_fg[ 5 ] + 0x000);

	tilemap_set_scrollx(tilemap_rot, 0, tetrisp2_rotregs[ 0 ] + 0x400);
	tilemap_set_scrolly(tilemap_rot, 0, tetrisp2_rotregs[ 2 ] + 0x400);

	tilemap_set_scrollx(tilemap_sub_bg, 0, rocknms_sub_scroll_bg[ 2 ] + 0x000);
	tilemap_set_scrolly(tilemap_sub_bg, 0, rocknms_sub_scroll_bg[ 5 ] + 0x000);

	tilemap_set_scrollx(tilemap_sub_fg, 0, rocknms_sub_scroll_fg[ 2 ] + 0x000);
	tilemap_set_scrolly(tilemap_sub_fg, 0, rocknms_sub_scroll_fg[ 5 ] + 0x000);

	tilemap_set_scrollx(tilemap_sub_rot, 0, rocknms_sub_rotregs[ 0 ] + 0x400);
	tilemap_set_scrolly(tilemap_sub_rot, 0, rocknms_sub_rotregs[ 2 ] + 0x400);


	/* sub screen */

	rocknms_rect_sub.min_x = 0;
	rocknms_rect_sub.min_y = 0;
	rocknms_rect_sub.max_x = 320-1;
	rocknms_rect_sub.max_y = 224-1;
	sect_rect(&rocknms_rect_sub,&myclip);

	/* Black background color */
	fillbitmap(bitmap, Machine->pens[0x0000], &rocknms_rect_sub);
	fillbitmap(priority_bitmap, 0, &rocknms_rect_sub);

	asc_pri = scr_pri = rot_pri = 0;

	if((rocknms_sub_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((rocknms_sub_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((rocknms_sub_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_rot, 0, 1 << 1);
	else if (scr_pri == 0)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_bg,  0, 1 << 0);
	else if (asc_pri == 0)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_fg,  0, 1 << 2);

	if (rot_pri == 1)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_rot, 0, 1 << 1);
	else if (scr_pri == 1)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_bg,  0, 1 << 0);
	else if (asc_pri == 1)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_fg,  0, 1 << 2);

	if (rot_pri == 2)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_rot, 0, 1 << 1);
	else if (scr_pri == 2)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_bg,  0, 1 << 0);
	else if (asc_pri == 2)
		tilemap_draw(bitmap,&rocknms_rect_sub, tilemap_sub_fg,  0, 1 << 2);

	tetrisp2_draw_sprites(bitmap,&rocknms_rect_sub, spriteram16_2, spriteram_2_size, 4);


	/* main screen */

	rocknms_rect_main.min_x = 0;
	rocknms_rect_main.min_y = 0;
	rocknms_rect_main.max_x = 320-1;
	rocknms_rect_main.max_y = 224-1;
	//  sect_rect  &rocknms_rect_sub  &myclip
	if(rocknms_rect_main.min_x < (mainscr_y+320-1) - myclip.max_y) rocknms_rect_main.min_x = (mainscr_y+320-1) - myclip.max_y;
	if(rocknms_rect_main.min_y < myclip.min_x      - mainscr_x   ) rocknms_rect_main.min_y = myclip.min_x - mainscr_x;
	if(rocknms_rect_main.max_x > (mainscr_y+320-1) - myclip.min_y) rocknms_rect_main.max_x = (mainscr_y+320-1) - myclip.min_y;
	if(rocknms_rect_main.max_y > myclip.max_x      - mainscr_x   ) rocknms_rect_main.max_y = myclip.max_x - mainscr_x;

	/* Black background color */
	fillbitmap(rocknms_main_tmpbitmap, Machine->pens[0x0000], &rocknms_rect_main);
	fillbitmap(priority_bitmap, 0, &rocknms_rect_main);

	asc_pri = scr_pri = rot_pri = 0;

	if((tetrisp2_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((tetrisp2_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((tetrisp2_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 0)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 0)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_fg,  0, 1 << 2);

	if (rot_pri == 1)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 1)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 1)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_fg,  0, 1 << 2);

	if (rot_pri == 2)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_rot, 0, 1 << 1);
	else if (scr_pri == 2)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_bg,  0, 1 << 0);
	else if (asc_pri == 2)
		tilemap_draw(rocknms_main_tmpbitmap,&rocknms_rect_main, tilemap_fg,  0, 1 << 2);

	tetrisp2_draw_sprites(rocknms_main_tmpbitmap,&rocknms_rect_main, spriteram16, spriteram_size, 0);


	/* copybitmap_ROT270 */
	rocknms_rect_main.min_x = mainscr_x;
	rocknms_rect_main.min_y = mainscr_y;
	rocknms_rect_main.max_x = mainscr_x + 224 -1;
	rocknms_rect_main.max_y = mainscr_y + 320 -1;
	sect_rect(&rocknms_rect_main,&myclip);
	if(Machine->color_depth != 32)
	{	/* 16bit */
		UINT16 *dest;
		int x,cy;
		int sx = rocknms_rect_main.min_x;
		int ex = rocknms_rect_main.max_x;
		int sy = rocknms_rect_main.min_y;
		int ey = rocknms_rect_main.max_y;
		int startx = 320-1 - (rocknms_rect_main.min_y - mainscr_y);
		int starty = 0     + rocknms_rect_main.min_x - mainscr_x;
		while (sy <= ey)
		{
			x = sx;
			cy = starty;
			dest = ((UINT16*)bitmap->line[sy]) + sx;
			{
				while (x <= ex)
				{
					*dest = ((UINT16*)rocknms_main_tmpbitmap->line[cy])[startx];
					cy++;
					x++;
					dest++;
				}
			}
			startx--;
			sy++;
		}
	}
	else
	{	/* 32bit */
		UINT32 *dest;
		int x,cy;
		int sx = rocknms_rect_main.min_x;
		int ex = rocknms_rect_main.max_x;
		int sy = rocknms_rect_main.min_y;
		int ey = rocknms_rect_main.max_y;
		int startx = 320-1 - (rocknms_rect_main.min_y - mainscr_y);
		int starty = 0     + rocknms_rect_main.min_x - mainscr_x;
		while (sy <= ey)
		{
			x = sx;
			cy = starty;
			dest = ((UINT32*)bitmap->line[sy]) + sx;
			{
				while (x <= ex)
				{
					*dest = ((UINT32*)rocknms_main_tmpbitmap->line[cy])[startx];
					cy++;
					x++;
					dest++;
				}
			}
			startx--;
			sy++;
		}
	}
}

VIDEO_UPDATE( rocknms )
{
	video_update_rocknms_0(bitmap, cliprect, (320-224)/2, 224);
}
