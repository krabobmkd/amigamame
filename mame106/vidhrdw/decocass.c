/***********************************************************************

    DECO Cassette System vidhrdw

 ***********************************************************************/

#include "driver.h"
#include "machine/decocass.h"

static INT32 watchdog_count;
static INT32 watchdog_flip;
static INT32 color_missiles;
static INT32 color_center_bot;
static INT32 mode_set;
static INT32 back_h_shift;
static INT32 back_vl_shift;
static INT32 back_vr_shift;
static INT32 part_h_shift;
static INT32 part_v_shift;
static INT32 center_h_shift_space;
static INT32 center_v_shift;

UINT8 *decocass_charram;
UINT8 *decocass_fgvideoram;
UINT8 *decocass_colorram;
UINT8 *decocass_bgvideoram; /* shares bits D0-3 with tileram! */
UINT8 *decocass_tileram;
UINT8 *decocass_objectram;

size_t decocass_fgvideoram_size;
size_t decocass_colorram_size;
size_t decocass_bgvideoram_size;
size_t decocass_tileram_size;
size_t decocass_objectram_size;

static tilemap *fg_tilemap, *bg_tilemap_l, *bg_tilemap_r;

static char *sprite_dirty;
static char *char_dirty;
static char *tile_dirty;
static char object_dirty;

static rectangle bg_tilemap_l_clip;
static rectangle bg_tilemap_r_clip;

static UINT32 tile_offset[32*32] = {
	0x078,0x079,0x07a,0x07b,0x07c,0x07d,0x07e,0x07f,0x0ff,0x0fe,0x0fd,0x0fc,0x0fb,0x0fa,0x0f9,0x0f8,0x278,0x279,0x27a,0x27b,0x27c,0x27d,0x27e,0x27f,0x2ff,0x2fe,0x2fd,0x2fc,0x2fb,0x2fa,0x2f9,0x2f8,
	0x070,0x071,0x072,0x073,0x074,0x075,0x076,0x077,0x0f7,0x0f6,0x0f5,0x0f4,0x0f3,0x0f2,0x0f1,0x0f0,0x270,0x271,0x272,0x273,0x274,0x275,0x276,0x277,0x2f7,0x2f6,0x2f5,0x2f4,0x2f3,0x2f2,0x2f1,0x2f0,
	0x068,0x069,0x06a,0x06b,0x06c,0x06d,0x06e,0x06f,0x0ef,0x0ee,0x0ed,0x0ec,0x0eb,0x0ea,0x0e9,0x0e8,0x268,0x269,0x26a,0x26b,0x26c,0x26d,0x26e,0x26f,0x2ef,0x2ee,0x2ed,0x2ec,0x2eb,0x2ea,0x2e9,0x2e8,
	0x060,0x061,0x062,0x063,0x064,0x065,0x066,0x067,0x0e7,0x0e6,0x0e5,0x0e4,0x0e3,0x0e2,0x0e1,0x0e0,0x260,0x261,0x262,0x263,0x264,0x265,0x266,0x267,0x2e7,0x2e6,0x2e5,0x2e4,0x2e3,0x2e2,0x2e1,0x2e0,
	0x058,0x059,0x05a,0x05b,0x05c,0x05d,0x05e,0x05f,0x0df,0x0de,0x0dd,0x0dc,0x0db,0x0da,0x0d9,0x0d8,0x258,0x259,0x25a,0x25b,0x25c,0x25d,0x25e,0x25f,0x2df,0x2de,0x2dd,0x2dc,0x2db,0x2da,0x2d9,0x2d8,
	0x050,0x051,0x052,0x053,0x054,0x055,0x056,0x057,0x0d7,0x0d6,0x0d5,0x0d4,0x0d3,0x0d2,0x0d1,0x0d0,0x250,0x251,0x252,0x253,0x254,0x255,0x256,0x257,0x2d7,0x2d6,0x2d5,0x2d4,0x2d3,0x2d2,0x2d1,0x2d0,
	0x048,0x049,0x04a,0x04b,0x04c,0x04d,0x04e,0x04f,0x0cf,0x0ce,0x0cd,0x0cc,0x0cb,0x0ca,0x0c9,0x0c8,0x248,0x249,0x24a,0x24b,0x24c,0x24d,0x24e,0x24f,0x2cf,0x2ce,0x2cd,0x2cc,0x2cb,0x2ca,0x2c9,0x2c8,
	0x040,0x041,0x042,0x043,0x044,0x045,0x046,0x047,0x0c7,0x0c6,0x0c5,0x0c4,0x0c3,0x0c2,0x0c1,0x0c0,0x240,0x241,0x242,0x243,0x244,0x245,0x246,0x247,0x2c7,0x2c6,0x2c5,0x2c4,0x2c3,0x2c2,0x2c1,0x2c0,
	0x038,0x039,0x03a,0x03b,0x03c,0x03d,0x03e,0x03f,0x0bf,0x0be,0x0bd,0x0bc,0x0bb,0x0ba,0x0b9,0x0b8,0x238,0x239,0x23a,0x23b,0x23c,0x23d,0x23e,0x23f,0x2bf,0x2be,0x2bd,0x2bc,0x2bb,0x2ba,0x2b9,0x2b8,
	0x030,0x031,0x032,0x033,0x034,0x035,0x036,0x037,0x0b7,0x0b6,0x0b5,0x0b4,0x0b3,0x0b2,0x0b1,0x0b0,0x230,0x231,0x232,0x233,0x234,0x235,0x236,0x237,0x2b7,0x2b6,0x2b5,0x2b4,0x2b3,0x2b2,0x2b1,0x2b0,
	0x028,0x029,0x02a,0x02b,0x02c,0x02d,0x02e,0x02f,0x0af,0x0ae,0x0ad,0x0ac,0x0ab,0x0aa,0x0a9,0x0a8,0x228,0x229,0x22a,0x22b,0x22c,0x22d,0x22e,0x22f,0x2af,0x2ae,0x2ad,0x2ac,0x2ab,0x2aa,0x2a9,0x2a8,
	0x020,0x021,0x022,0x023,0x024,0x025,0x026,0x027,0x0a7,0x0a6,0x0a5,0x0a4,0x0a3,0x0a2,0x0a1,0x0a0,0x220,0x221,0x222,0x223,0x224,0x225,0x226,0x227,0x2a7,0x2a6,0x2a5,0x2a4,0x2a3,0x2a2,0x2a1,0x2a0,
	0x018,0x019,0x01a,0x01b,0x01c,0x01d,0x01e,0x01f,0x09f,0x09e,0x09d,0x09c,0x09b,0x09a,0x099,0x098,0x218,0x219,0x21a,0x21b,0x21c,0x21d,0x21e,0x21f,0x29f,0x29e,0x29d,0x29c,0x29b,0x29a,0x299,0x298,
	0x010,0x011,0x012,0x013,0x014,0x015,0x016,0x017,0x097,0x096,0x095,0x094,0x093,0x092,0x091,0x090,0x210,0x211,0x212,0x213,0x214,0x215,0x216,0x217,0x297,0x296,0x295,0x294,0x293,0x292,0x291,0x290,
	0x008,0x009,0x00a,0x00b,0x00c,0x00d,0x00e,0x00f,0x08f,0x08e,0x08d,0x08c,0x08b,0x08a,0x089,0x088,0x208,0x209,0x20a,0x20b,0x20c,0x20d,0x20e,0x20f,0x28f,0x28e,0x28d,0x28c,0x28b,0x28a,0x289,0x288,
	0x000,0x001,0x002,0x003,0x004,0x005,0x006,0x007,0x087,0x086,0x085,0x084,0x083,0x082,0x081,0x080,0x200,0x201,0x202,0x203,0x204,0x205,0x206,0x207,0x287,0x286,0x285,0x284,0x283,0x282,0x281,0x280,
	0x178,0x179,0x17a,0x17b,0x17c,0x17d,0x17e,0x17f,0x1ff,0x1fe,0x1fd,0x1fc,0x1fb,0x1fa,0x1f9,0x1f8,0x378,0x379,0x37a,0x37b,0x37c,0x37d,0x37e,0x37f,0x3ff,0x3fe,0x3fd,0x3fc,0x3fb,0x3fa,0x3f9,0x3f8,
	0x170,0x171,0x172,0x173,0x174,0x175,0x176,0x177,0x1f7,0x1f6,0x1f5,0x1f4,0x1f3,0x1f2,0x1f1,0x1f0,0x370,0x371,0x372,0x373,0x374,0x375,0x376,0x377,0x3f7,0x3f6,0x3f5,0x3f4,0x3f3,0x3f2,0x3f1,0x3f0,
	0x168,0x169,0x16a,0x16b,0x16c,0x16d,0x16e,0x16f,0x1ef,0x1ee,0x1ed,0x1ec,0x1eb,0x1ea,0x1e9,0x1e8,0x368,0x369,0x36a,0x36b,0x36c,0x36d,0x36e,0x36f,0x3ef,0x3ee,0x3ed,0x3ec,0x3eb,0x3ea,0x3e9,0x3e8,
	0x160,0x161,0x162,0x163,0x164,0x165,0x166,0x167,0x1e7,0x1e6,0x1e5,0x1e4,0x1e3,0x1e2,0x1e1,0x1e0,0x360,0x361,0x362,0x363,0x364,0x365,0x366,0x367,0x3e7,0x3e6,0x3e5,0x3e4,0x3e3,0x3e2,0x3e1,0x3e0,
	0x158,0x159,0x15a,0x15b,0x15c,0x15d,0x15e,0x15f,0x1df,0x1de,0x1dd,0x1dc,0x1db,0x1da,0x1d9,0x1d8,0x358,0x359,0x35a,0x35b,0x35c,0x35d,0x35e,0x35f,0x3df,0x3de,0x3dd,0x3dc,0x3db,0x3da,0x3d9,0x3d8,
	0x150,0x151,0x152,0x153,0x154,0x155,0x156,0x157,0x1d7,0x1d6,0x1d5,0x1d4,0x1d3,0x1d2,0x1d1,0x1d0,0x350,0x351,0x352,0x353,0x354,0x355,0x356,0x357,0x3d7,0x3d6,0x3d5,0x3d4,0x3d3,0x3d2,0x3d1,0x3d0,
	0x148,0x149,0x14a,0x14b,0x14c,0x14d,0x14e,0x14f,0x1cf,0x1ce,0x1cd,0x1cc,0x1cb,0x1ca,0x1c9,0x1c8,0x348,0x349,0x34a,0x34b,0x34c,0x34d,0x34e,0x34f,0x3cf,0x3ce,0x3cd,0x3cc,0x3cb,0x3ca,0x3c9,0x3c8,
	0x140,0x141,0x142,0x143,0x144,0x145,0x146,0x147,0x1c7,0x1c6,0x1c5,0x1c4,0x1c3,0x1c2,0x1c1,0x1c0,0x340,0x341,0x342,0x343,0x344,0x345,0x346,0x347,0x3c7,0x3c6,0x3c5,0x3c4,0x3c3,0x3c2,0x3c1,0x3c0,
	0x138,0x139,0x13a,0x13b,0x13c,0x13d,0x13e,0x13f,0x1bf,0x1be,0x1bd,0x1bc,0x1bb,0x1ba,0x1b9,0x1b8,0x338,0x339,0x33a,0x33b,0x33c,0x33d,0x33e,0x33f,0x3bf,0x3be,0x3bd,0x3bc,0x3bb,0x3ba,0x3b9,0x3b8,
	0x130,0x131,0x132,0x133,0x134,0x135,0x136,0x137,0x1b7,0x1b6,0x1b5,0x1b4,0x1b3,0x1b2,0x1b1,0x1b0,0x330,0x331,0x332,0x333,0x334,0x335,0x336,0x337,0x3b7,0x3b6,0x3b5,0x3b4,0x3b3,0x3b2,0x3b1,0x3b0,
	0x128,0x129,0x12a,0x12b,0x12c,0x12d,0x12e,0x12f,0x1af,0x1ae,0x1ad,0x1ac,0x1ab,0x1aa,0x1a9,0x1a8,0x328,0x329,0x32a,0x32b,0x32c,0x32d,0x32e,0x32f,0x3af,0x3ae,0x3ad,0x3ac,0x3ab,0x3aa,0x3a9,0x3a8,
	0x120,0x121,0x122,0x123,0x124,0x125,0x126,0x127,0x1a7,0x1a6,0x1a5,0x1a4,0x1a3,0x1a2,0x1a1,0x1a0,0x320,0x321,0x322,0x323,0x324,0x325,0x326,0x327,0x3a7,0x3a6,0x3a5,0x3a4,0x3a3,0x3a2,0x3a1,0x3a0,
	0x118,0x119,0x11a,0x11b,0x11c,0x11d,0x11e,0x11f,0x19f,0x19e,0x19d,0x19c,0x19b,0x19a,0x199,0x198,0x318,0x319,0x31a,0x31b,0x31c,0x31d,0x31e,0x31f,0x39f,0x39e,0x39d,0x39c,0x39b,0x39a,0x399,0x398,
	0x110,0x111,0x112,0x113,0x114,0x115,0x116,0x117,0x197,0x196,0x195,0x194,0x193,0x192,0x191,0x190,0x310,0x311,0x312,0x313,0x314,0x315,0x316,0x317,0x397,0x396,0x395,0x394,0x393,0x392,0x391,0x390,
	0x108,0x109,0x10a,0x10b,0x10c,0x10d,0x10e,0x10f,0x18f,0x18e,0x18d,0x18c,0x18b,0x18a,0x189,0x188,0x308,0x309,0x30a,0x30b,0x30c,0x30d,0x30e,0x30f,0x38f,0x38e,0x38d,0x38c,0x38b,0x38a,0x389,0x388,
	0x100,0x101,0x102,0x103,0x104,0x105,0x106,0x107,0x187,0x186,0x185,0x184,0x183,0x182,0x181,0x180,0x300,0x301,0x302,0x303,0x304,0x305,0x306,0x307,0x387,0x386,0x385,0x384,0x383,0x382,0x381,0x380
};

/********************************************
    state saving setup
 ********************************************/
void decocass_video_state_save_init(void)
{
	state_save_register_global(watchdog_count);
	state_save_register_global(watchdog_flip);
	state_save_register_global(color_missiles);
	state_save_register_global(color_center_bot);
	state_save_register_global(mode_set);
	state_save_register_global(back_h_shift);
	state_save_register_global(back_vl_shift);
	state_save_register_global(back_vr_shift);
	state_save_register_global(part_h_shift);
	state_save_register_global(part_v_shift);
	state_save_register_global(center_h_shift_space);
	state_save_register_global(center_v_shift);
}

/********************************************
    tilemap callbacks
 ********************************************/

static UINT32 fgvideoram_scan_cols( UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows )
{
	/* logical (col,row) -> memory offset */
	return (num_cols - 1 - col) * num_rows + row;
}

static UINT32 bgvideoram_scan_cols( UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows )
{
	/* logical (col,row) -> memory offset */
	return tile_offset[col * num_rows + row];
}

static void get_bg_l_tile_info(int tile_index)
{
	int color = (color_center_bot >> 7) & 1;
	SET_TILE_INFO(
			2,
			(0x80 == (tile_index & 0x80)) ? 16 : decocass_bgvideoram[tile_index] >> 4,
			color,
			0)
}

static void get_bg_r_tile_info(int tile_index)
{
	int color = (color_center_bot >> 7) & 1;
	SET_TILE_INFO(
			2,
			(0x00 == (tile_index & 0x80)) ? 16 : decocass_bgvideoram[tile_index] >> 4,
			color,
			TILE_FLIPY)
}

static void get_fg_tile_info(int tile_index)
{
	UINT8 code = decocass_fgvideoram[tile_index];
	UINT8 attr = decocass_colorram[tile_index];
	SET_TILE_INFO(
			0,
			256 * (attr & 3) + code,
			color_center_bot & 1,
			0)
}

/********************************************
    big object
 ********************************************/

static void draw_object(mame_bitmap *bitmap, const rectangle *cliprect)

{ 
struct drawgfxParams dgp0={
	bitmap, 	// dest
	Machine->gfx[3], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	cliprect, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
	int sx, sy, color;

	if (0 == (mode_set & 0x80))  /* part_h_enable off? */
		return;

	color = (color_center_bot >> 4) & 15;

	sy = 192 - (part_v_shift & 0x7f);

	if (part_h_shift & 0x80)
		sx = (part_h_shift & 0x7f) + 1;
	else
		sx = 91 - (part_h_shift & 0x7f);

	
	dgp0.color = color;
	dgp0.sx = sx + 64;
	dgp0.sy = sy;
	drawgfx(&dgp0);
	
	dgp0.color = color;
	dgp0.sx = sx;
	dgp0.sy = sy;
	drawgfx(&dgp0);
	
	dgp0.color = color;
	dgp0.sx = sx + 64;
	dgp0.sy = sy - 64;
	drawgfx(&dgp0);
	
	dgp0.color = color;
	dgp0.sx = sx;
	dgp0.sy = sy - 64;
	drawgfx(&dgp0);
}
} // end of patch paragraph


static void draw_center(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int sx, sy, x, y, color;

	color = 0;
	if (color_center_bot & 0x10)
		color |= 4;
	if (color_center_bot & 0x20)
		color |= 2;
	if (color_center_bot & 0x40)
		color |= 1;
	if (color_center_bot & 0x80)
		color = (color & 4) + ((color << 1) & 2) + ((color >> 1) & 1);

	sy = center_v_shift;
	sx = (center_h_shift_space >> 2) & 0x3c;

	for (y = 0; y < 4; y++)
		if ((sy + y) >= cliprect->min_y && (sy + y) <= cliprect->max_y)
		{
			if (((sy + y) & color_center_bot & 3) == (sy & color_center_bot & 3))
				for (x = 0; x < 256; x++)
					if (0 != (x & 16) || 0 != (center_h_shift_space & 1))
						plot_pixel(bitmap, (sx + x) & 255, sy + y, Machine->pens[color]);
		}
}

/********************************************
    memory handlers
 ********************************************/

WRITE8_HANDLER( decocass_paletteram_w )
{
	/*
     * RGB output is inverted and A4 is inverted too
     * (ME/ input on 1st paletteram, inverter -> ME/ on 2nd)
     */
	offset = (offset & 31) ^ 16;
	paletteram_BBGGGRRR_w( offset, ~data );
}

WRITE8_HANDLER( decocass_charram_w )
{
	if (data == decocass_charram[offset])
		return;
	decocass_charram[offset] = data;
	/* dirty sprite */
	sprite_dirty[(offset >> 5) & 255] = 1;
	/* dirty char */
	char_dirty[(offset >> 3) & 1023] = 1;
}


WRITE8_HANDLER( decocass_fgvideoram_w )
{
	if (data == decocass_fgvideoram[offset])
		return;
	decocass_fgvideoram[offset] = data;
	tilemap_mark_tile_dirty( fg_tilemap, offset );
}

WRITE8_HANDLER( decocass_colorram_w )
{
	if (data == decocass_colorram[offset])
		return;
	decocass_colorram[offset] = data;
	tilemap_mark_tile_dirty( fg_tilemap, offset );
}

static void mark_bg_tile_dirty(offs_t offset)
{
	if (offset & 0x80)
		tilemap_mark_tile_dirty( bg_tilemap_r, offset );
	else
		tilemap_mark_tile_dirty( bg_tilemap_l, offset );
}

WRITE8_HANDLER( decocass_tileram_w )
{
	if (data == decocass_tileram[offset])
		return;
	decocass_tileram[offset] = data;
	/* dirty tile (64 bytes per tile) */
	tile_dirty[(offset / 64) & 15] = 1;
	/* first 1KB of tile RAM is shared with tilemap RAM */
	if (offset < decocass_bgvideoram_size)
		mark_bg_tile_dirty( offset );
}

WRITE8_HANDLER( decocass_objectram_w )
{
	if (data == decocass_objectram[offset])
		return;
	decocass_objectram[offset] = data;
	/* dirty the object */
	object_dirty = 1;
}

WRITE8_HANDLER( decocass_bgvideoram_w )
{
	if (data == decocass_bgvideoram[offset])
		return;
	decocass_bgvideoram[offset] = data;
	mark_bg_tile_dirty( offset );
}

/* The watchdog is a 4bit counter counting down every frame */
WRITE8_HANDLER( decocass_watchdog_count_w )
{
	LOG(1,("decocass_watchdog_count_w: $%02x\n", data));
	watchdog_count = data & 0x0f;
}

WRITE8_HANDLER( decocass_watchdog_flip_w )
{
	LOG(1,("decocass_watchdog_flip_w: $%02x\n", data));
	watchdog_flip = data;
}

WRITE8_HANDLER( decocass_color_missiles_w )
{
	LOG(1,("decocass_color_missiles_w: $%02x\n", data));
	/* only bits D0-D2 and D4-D6 are connected to
     * the color RAM demux:
     * D0-D2 to the IC0 inputs
     * D4-D6 to the IC1 inputs
     */
	color_missiles = data & 0x77;
}

/*
 * D0 - ??
 * D1 - ??
 * D2 - ptn 1/2
 * D3 - BKG ena
 * D4 - center L on
 * D5 - cross on
 * D6 - tunnel
 * D7 - part h enable
 */
WRITE8_HANDLER( decocass_mode_set_w )
{
	if (data == mode_set)
		return;
	LOG(1,("decocass_mode_set_w: $%02x (%s%s%s%s%s%s%s%s)\n", data,
		(data & 0x01) ? "D0?" : "",
		(data & 0x02) ? " D1?" : "",
		(data & 0x04) ? " ptn1/2" : "",
		(data & 0x08) ? " bkg_ena" : "",
		(data & 0x10) ? " center_l_on" : "",
		(data & 0x20) ? " cross_on" : "",
		(data & 0x40) ? " tunnel" : "",
		(data & 0x80) ? " part_h_enable" : ""));

	set_vh_global_attribute( &mode_set, data );
}

WRITE8_HANDLER( decocass_color_center_bot_w )
{
	if (data == color_center_bot)
		return;
	LOG(1,("decocass_color_center_bot_w: $%02x (color:%d, center_bot:%d)\n", data, data&3, data>>4));
	/*
     * D7   CL3/4 (swap 2+4)
     * D6   CL1
     * D5   CL2
     * D4   CL4
     * D3   nc
     * D2   nc
     * D1   CLD4
     * D0   CLD3
     */
	set_vh_global_attribute( &color_center_bot, data);
}

WRITE8_HANDLER( decocass_back_h_shift_w )
{
	if (data == back_h_shift)
		return;
	LOG(1,("decocass_back_h_shift_w: $%02x\n", data));
	back_h_shift = data;
}

WRITE8_HANDLER( decocass_back_vl_shift_w )
{
	if (data == back_vl_shift)
		return;
	LOG(1,("decocass_back_vl_shift_w: $%02x\n", data));
	back_vl_shift = data;
}

WRITE8_HANDLER( decocass_back_vr_shift_w )
{
	if (data == back_vr_shift)
		return;
	LOG(1,("decocass_back_vr_shift_w: $%02x\n", data));
	back_vr_shift = data;
}

WRITE8_HANDLER( decocass_part_h_shift_w )
{
	if (data == part_v_shift )
		return;
	LOG(1,("decocass_part_h_shift_w: $%02x\n", data));
	part_h_shift = data;
}

WRITE8_HANDLER( decocass_part_v_shift_w )
{
	if (data == part_v_shift )
		return;
	LOG(1,("decocass_part_v_shift_w: $%02x\n", data));
	part_v_shift = data;
}

WRITE8_HANDLER( decocass_center_h_shift_space_w )
{
	if (data == center_h_shift_space)
		return;
	LOG(1,("decocass_center_h_shift_space_w: $%02x\n", data));
	center_h_shift_space = data;
}

WRITE8_HANDLER( decocass_center_v_shift_w )
{
	LOG(1,("decocass_center_v_shift_w: $%02x\n", data));
	center_v_shift = data;
}

/********************************************
    memory handlers
 ********************************************/

static void draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, int color,
						int sprite_y_adjust, int sprite_y_adjust_flip_screen,
						UINT8 *sprite_ram, int interleave)
{
	int i,offs;

	/* Draw the sprites */
	
	{ 
	struct drawgfxParams dgp4={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0, offs = 0;i < 8; i++, offs += 4*interleave)
	{
		int sx,sy,flipx,flipy;

		if (!(sprite_ram[offs + 0] & 0x01))
			continue;

		sx = 240 - sprite_ram[offs + 3*interleave];
		sy = 240 - sprite_ram[offs + 2*interleave];

		flipx = sprite_ram[offs + 0] & 0x04;
		flipy = sprite_ram[offs + 0] & 0x02;

		if (flip_screen)
		{
			sx = 240 - sx;
			sy = 240 - sy + sprite_y_adjust_flip_screen;

			flipx = !flipx;
			flipy = !flipy;
		}

		sy -= sprite_y_adjust;

		
		dgp4.code = sprite_ram[offs + interleave];
		dgp4.color = color;
		dgp4.flipx = flipx;
		dgp4.flipy = flipy;
		dgp4.sx = sx;
		dgp4.sy = sy;
		drawgfx(&dgp4);

		sy += (flip_screen ? -256 : 256);

		// Wrap around
		
		dgp4.code = sprite_ram[offs + interleave];
		dgp4.color = color;
		dgp4.flipx = flipx;
		dgp4.flipy = flipy;
		dgp4.sx = sx;
		dgp4.sy = sy;
		drawgfx(&dgp4);
	}
	} // end of patch paragraph

}


static void draw_missiles(mame_bitmap *bitmap, const rectangle *cliprect,
						int missile_y_adjust, int missile_y_adjust_flip_screen,
						UINT8 *missile_ram, int interleave)
{
	int i,offs;

	/* Draw the missiles (16 of them) seemingly with alternating colors
     * from the E302 latch (color_missiles) */
	
	{ 
	struct drawgfxParams dgp6={
		bitmap, 	// dest
		Machine->gfx[4], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (i = 0, offs = 0; i < 8; i++, offs += 4*interleave)
	{
		int sx,sy;

		sy = 255 - missile_ram[offs + 0*interleave];
		sx = 255 - missile_ram[offs + 2*interleave];
		if (flip_screen)
		{
			sx = 240 - sx;
			sy = 240 - sy + missile_y_adjust_flip_screen;
		}
		sy -= missile_y_adjust;
		
		dgp6.color = 32 + ((color_missiles >> 4) & 7);
		dgp6.sx = sx;
		dgp6.sy = sy;
		drawgfx(&dgp6);

		sy = 255 - missile_ram[offs + 1*interleave];
		sx = 255 - missile_ram[offs + 3*interleave];
		if (flip_screen)
		{
			sx = 240 - sx;
			sy = 240 - sy + missile_y_adjust_flip_screen;
		}
		sy -= missile_y_adjust;
		
		dgp6.color = 32 + (color_missiles & 7);
		dgp6.sx = sx;
		dgp6.sy = sy;
		drawgfx(&dgp6);
	}
	} // end of patch paragraph

}


static void decode_modified(UINT8 *sprite_ram, int interleave)
{
	int i,offs;

	/* decode dirty characters */
	for (offs = decocass_fgvideoram_size - 1;offs >= 0;offs--)
	{
		int code;

		code = decocass_fgvideoram[offs] + 256 * (decocass_colorram[offs] & 3);

		switch (char_dirty[code])
		{
		case 1:
			decodechar(Machine->gfx[0],code,decocass_charram,Machine->drv->gfxdecodeinfo[0].gfxlayout);
			char_dirty[code] = 2;
			/* fall through */
		case 2:
			tilemap_mark_tile_dirty(fg_tilemap, offs);
			break;
		default:
			break;
		}
	}

	for (i = 0; i < 1024; i++)
	{
		if (char_dirty[i] == 2)
			char_dirty[i] = 0;
	}

	/* decode dirty sprites */
	for (i = 0, offs = 0;i < 8; i++, offs += 4*interleave)
	{
		int code;

		code  = sprite_ram[offs + interleave];
		if (sprite_dirty[code])
		{
			sprite_dirty[code] = 0;

			decodechar(Machine->gfx[1],code,decocass_charram,Machine->drv->gfxdecodeinfo[1].gfxlayout);
		}
	}

	/* decode dirty tiles */
	for (offs = 0; offs < decocass_bgvideoram_size; offs++)
	{
		int code = (decocass_tileram[offs] >> 4) & 15;

		if (tile_dirty[code])
		{
			tile_dirty[code] = 0;

			decodechar(Machine->gfx[2],code,decocass_tileram,Machine->drv->gfxdecodeinfo[2].gfxlayout);

			/* mark all visible tiles dirty */
			for (i = offs; i < decocass_bgvideoram_size; i++)
				if (code == ((decocass_tileram[i] >> 4) & 15))
					mark_bg_tile_dirty(i);
		}
	}

	/* decode object if it is dirty */
	if (object_dirty)
	{
		decodechar(Machine->gfx[3], 0, decocass_objectram, Machine->drv->gfxdecodeinfo[3].gfxlayout);
		decodechar(Machine->gfx[3], 1, decocass_objectram, Machine->drv->gfxdecodeinfo[3].gfxlayout);
		object_dirty = 0;
	}
}

VIDEO_START( decocass )
{
	sprite_dirty = auto_malloc(256);
	char_dirty = auto_malloc(1024);
	tile_dirty = auto_malloc(16);

	bg_tilemap_l = tilemap_create( get_bg_l_tile_info, bgvideoram_scan_cols, TILEMAP_TRANSPARENT, 16, 16, 32, 32 );
	bg_tilemap_r = tilemap_create( get_bg_r_tile_info, bgvideoram_scan_cols, TILEMAP_TRANSPARENT, 16, 16, 32, 32 );
	fg_tilemap = tilemap_create( get_fg_tile_info, fgvideoram_scan_cols, TILEMAP_TRANSPARENT,  8,  8, 32, 32 );

	if (!bg_tilemap_l || !bg_tilemap_r || !fg_tilemap)
		return 1;

	tilemap_set_transparent_pen( bg_tilemap_l, 0 );
	tilemap_set_transparent_pen( bg_tilemap_r, 0 );
	tilemap_set_transparent_pen( fg_tilemap, 0 );

	bg_tilemap_l_clip = Machine->visible_area;
	bg_tilemap_l_clip.max_y = Machine->drv->screen_height / 2;

	bg_tilemap_r_clip = Machine->visible_area;
	bg_tilemap_r_clip.min_y = Machine->drv->screen_height / 2;

	/* background videroam bits D0-D3 are shared with the tileram */
	decocass_bgvideoram = decocass_tileram;
	decocass_bgvideoram_size = 0x0400;	/* d000-d3ff */

	return 0;
}

VIDEO_UPDATE( decocass )
{
	int scrollx, scrolly_l, scrolly_r;
	rectangle clip;

	if (0xc0 != (input_port_2_r(0) & 0xc0))  /* coin slots assert an NMI */
		cpunum_set_input_line(0, INPUT_LINE_NMI, ASSERT_LINE);

	if (0 == (watchdog_flip & 0x04))
		watchdog_reset_w (0,0);
	else if (watchdog_count-- > 0)
		watchdog_reset_w (0,0);

#if TAPE_UI_DISPLAY
	if (tape_timer)
	{
		double tape_time = tape_time0 + tape_dir * timer_timeelapsed(tape_timer);
		if (tape_time < 0.0)
			tape_time = 0.0;
		else if (tape_time > 999.9)
			tape_time = 999.9;
		ui_popup("%c%c [%05.1fs] %c%c",
			(tape_dir < 0 && tape_speed) ? '<' : ' ',
			(tape_dir < 0) ? '<' : ' ',
			tape_time,
			(tape_dir > 0) ? '>' : ' ',
			(tape_dir > 0 && tape_speed) ? '>' : ' ');
	}
#endif
#ifdef MAME_DEBUG
	{
		static int showmsg;
		if (code_pressed_memory(KEYCODE_I))
			showmsg ^= 1;
		if (showmsg)
			ui_popup_time(1, "mode:$%02x cm:$%02x ccb:$%02x h:$%02x vl:$%02x vr:$%02x ph:$%02x pv:$%02x ch:$%02x cv:$%02x",
				mode_set,
				color_missiles,
				color_center_bot,
				back_h_shift,
				back_vl_shift,
				back_vr_shift,
				part_h_shift,
				part_v_shift,
				center_h_shift_space,
				center_v_shift);
	}
#endif

	fillbitmap( bitmap, Machine->pens[0], cliprect );

	decode_modified( decocass_fgvideoram, 0x20 );

	scrolly_l = back_vl_shift;
	scrolly_r = 256 - back_vr_shift;

	scrollx = 256 - back_h_shift;
	if (0 == (mode_set & 0x02))
		scrollx += 256;

#if 0
/* this is wrong */
	if (0 != back_h_shift && 0 == ((mode_set ^ (mode_set >> 1)) & 1))
		scrollx += 256;
#endif

	if (0 == (mode_set & 0x04))
		scrolly_r += 256;
	else
		scrolly_l += 256;

	tilemap_set_scrollx( bg_tilemap_l, 0, scrollx );
	tilemap_set_scrolly( bg_tilemap_l, 0, scrolly_l );

	tilemap_set_scrollx( bg_tilemap_r, 0, scrollx );
	tilemap_set_scrolly( bg_tilemap_r, 0, scrolly_r );

	if (mode_set & 0x20)
	{
		if (mode_set & 0x08)	/* bkg_ena on ? */
		{
			clip = bg_tilemap_l_clip;
			sect_rect(&clip,cliprect);
			tilemap_draw(bitmap,&clip, bg_tilemap_l, 0, 0);

			clip = bg_tilemap_r_clip;
			sect_rect(&clip,cliprect);
			tilemap_draw(bitmap,&clip, bg_tilemap_r, 0, 0);
		}
		draw_object(bitmap,cliprect);
		draw_center(bitmap,cliprect);
	}
	else
	{
		draw_object(bitmap,cliprect);
		draw_center(bitmap,cliprect);
		if (mode_set & 0x08)	/* bkg_ena on ? */
		{
			clip = bg_tilemap_l_clip;
			sect_rect(&clip,cliprect);
			tilemap_draw(bitmap,&clip, bg_tilemap_l, 0, 0);

			clip = bg_tilemap_r_clip;
			sect_rect(&clip,cliprect);
			tilemap_draw(bitmap,&clip, bg_tilemap_r, 0, 0);
		}
	}
	tilemap_draw(bitmap,cliprect, fg_tilemap, 0, 0);
	draw_sprites(bitmap,cliprect, (color_center_bot >> 1) & 1, 0, 0, decocass_fgvideoram, 0x20);
	draw_missiles(bitmap,cliprect, 1, 0, decocass_colorram, 0x20);
}

