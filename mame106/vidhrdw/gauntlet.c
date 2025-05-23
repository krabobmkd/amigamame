/***************************************************************************

    Atari Gauntlet hardware

****************************************************************************/

#include "driver.h"
#include "machine/atarigen.h"
#include "gauntlet.h"
#include "palette.h"


/*************************************
 *
 *  Globals we own
 *
 *************************************/

UINT8 vindctr2_screen_refresh;



/*************************************
 *
 *  Statics
 *
 *************************************/

static UINT8 playfield_tile_bank;
static UINT8 playfield_color_bank;


//krb
WRITE16_HANDLER( paletteram16_IIIIRRRRGGGGBBBB_gauntlet_word_w )
{
//    COMBINE_DATA(&paletteram16[offset]);
    paletteram16[offset] = data;

//    set_color_4444(offset, 12, 8, 4, 0, paletteram16[offset]);


    static const UINT8 ztable[16] =
        { 0x0, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11 };
    int i, r, g, b;

    i = ztable[(data >> 12) & 15];
    UINT32 val=((data <<8) & 0x000f0000) |
                 ((data <<4) & 0x00000f00)
                | (data & 0x0f);
    val *= i;

    /*
    r = ((data >> rshift) & 15) * i;
    g = ((data >> gshift) & 15) * i;
    b = ((data >> bshift) & 15) * i;
*/
    //palette_set_color(color, r, g, b);
    setpalettefast_neogeo(offset, val );

}

/*************************************
 *
 *  Tilemap callbacks
 *
 *************************************/

static void get_alpha_tile_info(int tile_index)
{
	UINT16 data = atarigen_alpha[tile_index];
	int code = data & 0x3ff;
	int color = ((data >> 10) & 0x0f) | ((data >> 9) & 0x20);
	int opaque = data & 0x8000;
	SET_TILE_INFO(1, code, color, opaque ? TILE_IGNORE_TRANSPARENCY : 0);
}


static void get_playfield_tile_info(int tile_index)
{
	UINT16 data = atarigen_playfield[tile_index];
	int code = ((playfield_tile_bank * 0x1000) + (data & 0xfff)) ^ 0x800;
	int color = 0x10 + (playfield_color_bank * 8) + ((data >> 12) & 7);
	SET_TILE_INFO(0, code, color, (data >> 15) & 1);
}



/*************************************
 *
 *  Video system start
 *
 *************************************/

VIDEO_START( gauntlet )
{
	static const struct atarimo_desc modesc =
	{
		0,					/* index to which gfx system */
		1,					/* number of motion object banks */
		1,					/* are the entries linked? */
		1,					/* are the entries split? */
		0,					/* render in reverse order? */
		0,					/* render in swapped X/Y order? */
		0,					/* does the neighbor bit affect the next object? */
		8,					/* pixels per SLIP entry (0 for no-slip) */
		1,					/* pixel offset for SLIPs */
		0,					/* maximum number of links to visit/scanline (0=all) */

		0x100,				/* base palette entry */
		0x100,				/* maximum number of colors */
		0,					/* transparent pen index */

		{{ 0,0,0,0x03ff }},	/* mask for the link */
		{{ 0 }},			/* mask for the graphics bank */
		{{ 0x7fff,0,0,0 }},	/* mask for the code index */
		{{ 0 }},			/* mask for the upper code index */
		{{ 0,0x000f,0,0 }},	/* mask for the color */
		{{ 0,0xff80,0,0 }},	/* mask for the X position */
		{{ 0,0,0xff80,0 }},	/* mask for the Y position */
		{{ 0,0,0x0038,0 }},	/* mask for the width, in tiles*/
		{{ 0,0,0x0007,0 }},	/* mask for the height, in tiles */
		{{ 0,0,0x0040,0 }},	/* mask for the horizontal flip */
		{{ 0 }},			/* mask for the vertical flip */
		{{ 0 }},			/* mask for the priority */
		{{ 0 }},			/* mask for the neighbor */
		{{ 0 }},			/* mask for absolute coordinates */

		{{ 0 }},			/* mask for the special value */
		0,					/* resulting value to indicate "special" */
		0					/* callback routine for special entries */
	};

	UINT16 *codelookup;
	int i, size;

	/* initialize the playfield */
	atarigen_playfield_tilemap = tilemap_create(get_playfield_tile_info, tilemap_scan_cols, TILEMAP_OPAQUE, 8,8, 64,64);
	if (!atarigen_playfield_tilemap)
		return 1;

	/* initialize the motion objects */
	if (!atarimo_init(0, &modesc))
		return 1;

	/* initialize the alphanumerics */
	atarigen_alpha_tilemap = tilemap_create(get_alpha_tile_info, tilemap_scan_rows, TILEMAP_TRANSPARENT, 8,8, 64,32);
	if (!atarigen_alpha_tilemap)
		return 1;
	tilemap_set_transparent_pen(atarigen_alpha_tilemap, 0);

	/* modify the motion object code lookup table to account for the code XOR */
	codelookup = atarimo_get_code_lookup(0, &size);
	for (i = 0; i < size; i++)
		codelookup[i] ^= 0x800;

	/* set up the base color for the playfield */
	playfield_color_bank = vindctr2_screen_refresh ? 0 : 1;
	return 0;
}



/*************************************
 *
 *  Horizontal scroll register
 *
 *************************************/

WRITE16_HANDLER( gauntlet_xscroll_w )
{
	UINT16 oldxscroll = *atarigen_xscroll;
	COMBINE_DATA(atarigen_xscroll);

	/* if something changed, force a partial update */
	if (*atarigen_xscroll != oldxscroll)
	{
		force_partial_update(cpu_getscanline());

		/* adjust the scrolls */
		tilemap_set_scrollx(atarigen_playfield_tilemap, 0, *atarigen_xscroll);
		atarimo_set_xscroll(0, *atarigen_xscroll & 0x1ff);
	}
}



/*************************************
 *
 *  Vertical scroll/PF bank register
 *
 *************************************/

WRITE16_HANDLER( gauntlet_yscroll_w )
{
	UINT16 oldyscroll = *atarigen_yscroll;
	COMBINE_DATA(atarigen_yscroll);

	/* if something changed, force a partial update */
	if (*atarigen_yscroll != oldyscroll)
	{
		force_partial_update(cpu_getscanline());

		/* if the bank changed, mark all tiles dirty */
		if (playfield_tile_bank != (*atarigen_yscroll & 3))
		{
			playfield_tile_bank = *atarigen_yscroll & 3;
			tilemap_mark_all_tiles_dirty(atarigen_playfield_tilemap);
		}

		/* adjust the scrolls */
		tilemap_set_scrolly(atarigen_playfield_tilemap, 0, *atarigen_yscroll >> 7);
		atarimo_set_yscroll(0, (*atarigen_yscroll >> 7) & 0x1ff);
	}
}



/*************************************
 *
 *  Main refresh
 *
 *************************************/

VIDEO_UPDATE( gauntlet )
{
	struct atarimo_rect_list rectlist;
	mame_bitmap *mobitmap;
	int x, y, r;

	/* draw the playfield */
	tilemap_draw(bitmap, cliprect, atarigen_playfield_tilemap, 0, 0);

	/* draw and merge the MO */
	mobitmap = atarimo_render(0, cliprect, &rectlist);
	for (r = 0; r < rectlist.numrects; r++, rectlist.rect++)
		for (y = rectlist.rect->min_y; y <= rectlist.rect->max_y; y++)
		{
			UINT16 *mo = (UINT16 *)mobitmap->base + mobitmap->rowpixels * y;
			UINT16 *pf = (UINT16 *)bitmap->base + bitmap->rowpixels * y;
			for (x = rectlist.rect->min_x; x <= rectlist.rect->max_x; x++)
				if (mo[x])
				{
					/* verified via schematics:

                        MO pen 1 clears PF color bit 0x80
                    */
					if ((mo[x] & 0x0f) == 1)
					{
						/* Vindicators Part II has extra logic here for the bases */
						if (!vindctr2_screen_refresh || (mo[x] & 0xf0) != 0)
							pf[x] ^= 0x80;
					}
					else
						pf[x] = mo[x];

					/* erase behind ourselves */
					mo[x] = 0;
				}
		}

	/* add the alpha on top */
	tilemap_draw(bitmap, cliprect, atarigen_alpha_tilemap, 0, 0);
}
