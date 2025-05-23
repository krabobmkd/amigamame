/***************************************************************************

 Wild West C.O.W.boys of Moo Mesa
 Bucky O'Hare
 (c) 1992 Konami

 Video hardware emulation.

***************************************************************************/

#include "driver.h"
#include "vidhrdw/konamiic.h"

static int sprite_colorbase;
static int layer_colorbase[4], layerpri[3];
static int alpha_enabled;

void moo_set_alpha(int on)
{
	alpha_enabled = on;
}

static void moo_sprite_callback(int *code, int *color, int *priority_mask)
{
	int pri = (*color & 0x03e0) >> 4;

	if      (pri <= layerpri[2])	*priority_mask = 0;
	else if (pri <= layerpri[1])	*priority_mask = 0xf0;
	else if (pri <= layerpri[0])	*priority_mask = 0xf0|0xcc;
	else 							*priority_mask = 0xf0|0xcc|0xaa;

	*color = sprite_colorbase | (*color & 0x001f);
}

static void moo_tile_callback(int layer, int *code, int *color)
{
	*color = layer_colorbase[layer] | (*color>>2 & 0x0f);
}

VIDEO_START(moo)
{
	int offsx, offsy;

	if (Machine->color_depth != 32) return 1; // ensure correct bpp to avoid crashing in-game

	alpha_enabled = 0;

	K053251_vh_start();
	K054338_vh_start();

	if (K056832_vh_start(REGION_GFX1, K056832_BPP_4, 1, NULL, moo_tile_callback, 0)) return 1;

	if (!strcmp(Machine->gamedrv->name, "bucky") || !strcmp(Machine->gamedrv->name, "buckyua"))
	{
		// Bucky doesn't chain tilemaps
		K056832_set_LayerAssociation(0);

		K056832_set_LayerOffset(0, -2, 0);
		K056832_set_LayerOffset(1,  2, 0);
		K056832_set_LayerOffset(2,  4, 0);
		K056832_set_LayerOffset(3,  6, 0);

		offsx = -48;
		offsy =  23;
	}
	else
	{
		// other than the intro showing one blank line alignment is good through the game
		K056832_set_LayerOffset(0, -2+1, 0);
		K056832_set_LayerOffset(1,  2+1, 0);
		K056832_set_LayerOffset(2,  4+1, 0);
		K056832_set_LayerOffset(3,  6+1, 0);

		offsx = -48+1;
		offsy =  23;
	}

	if (K053247_vh_start(REGION_GFX2, offsx, offsy, NORMAL_PLANE_ORDER, moo_sprite_callback)) return 1;

	K054338_invert_alpha(0);

	return 0;
}

/* useful function to sort the three tile layers by priority order */
static void sortlayers(int *layer,int *pri)
{
#define SWAP(a,b) \
	if (pri[a] < pri[b]) \
	{ \
		int t; \
		t = pri[a]; pri[a] = pri[b]; pri[b] = t; \
		t = layer[a]; layer[a] = layer[b]; layer[b] = t; \
	}

	SWAP(0,1)
	SWAP(0,2)
	SWAP(1,2)
}

VIDEO_UPDATE(moo)
{
	static const int K053251_CI[4] = { K053251_CI1, K053251_CI2, K053251_CI3, K053251_CI4 };
	int layers[3];
	int bg_colorbase, new_colorbase, plane, dirty, alpha;

	bg_colorbase       = K053251_get_palette_index(K053251_CI1);
	sprite_colorbase   = K053251_get_palette_index(K053251_CI0);
	layer_colorbase[0] = 0x70;

	if (K056832_get_LayerAssociation())
	{
		for (plane=1; plane<4; plane++)
		{
			new_colorbase = K053251_get_palette_index(K053251_CI[plane]);
			if (layer_colorbase[plane] != new_colorbase)
			{
				layer_colorbase[plane] = new_colorbase;
				K056832_mark_plane_dirty(plane);
			}
		}
	}
	else
	{
		for (dirty=0, plane=1; plane<4; plane++)
		{
			new_colorbase = K053251_get_palette_index(K053251_CI[plane]);
			if (layer_colorbase[plane] != new_colorbase)
			{
				layer_colorbase[plane] = new_colorbase;
				dirty = 1;
			}
		}
		if (dirty) K056832_MarkAllTilemapsDirty();
	}

	layers[0] = 1;
	layerpri[0] = K053251_get_priority(K053251_CI2);
	layers[1] = 2;
	layerpri[1] = K053251_get_priority(K053251_CI3);
	layers[2] = 3;
	layerpri[2] = K053251_get_priority(K053251_CI4);

	sortlayers(layers, layerpri);

	K054338_update_all_shadows();
	K054338_fill_backcolor(bitmap, 0);

	fillbitmap(priority_bitmap,0,cliprect);

	if (layerpri[0] < K053251_get_priority(K053251_CI1))	/* bucky hides back layer behind background */
		K056832_tilemap_draw(bitmap, cliprect, layers[0], 0, 1);

	K056832_tilemap_draw(bitmap, cliprect, layers[1], 0, 2);

	// Enabling alpha improves fog and fading in Moo but causes other things to disappear.
	// There is probably a control bit somewhere to turn off alpha blending.
	alpha_enabled = K054338_read_register(K338_REG_CONTROL) & K338_CTL_MIXPRI; // DUMMY

	alpha = (alpha_enabled) ? K054338_set_alpha_level(1) : 255;

	if (alpha > 0)
		K056832_tilemap_draw(bitmap, cliprect, layers[2], (alpha >= 255) ? 0 : TILEMAP_ALPHA, 4);

	K053247_sprites_draw(bitmap,cliprect);

	K056832_tilemap_draw(bitmap, cliprect, 0, 0, 0);
}
