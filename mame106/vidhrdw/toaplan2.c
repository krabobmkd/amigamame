/***************************************************************************

  Functions to emulate the video hardware of some Toaplan games,
  which use one or more Toaplan L7A0498 GP9001 graphic controllers.

  The simpler hardware of these games use one GP9001 controller.
  Next we have games that use two GP9001 controllers, whose priority
  schemes between the two controllers is unknown at this time, and
  may be game dependant.
  Finally we have games using one GP9001 controller and an additional
  text tile layer, which has highest priority. This text tile layer
  appears to have line-scroll support. Some of these games copy the
  text tile gfx data to RAM from the main CPU ROM, which easily allows
  for effects to be added to the tiles, by manipulating the text tile
  gfx data. The tiles are then dynamically decoded from RAM before
  displaying them.


 To Do / Unknowns
    -  Hack is needed to reset sound CPU and sound chip when machine
        is 'tilted' in Pipi & Bibis. Otherwise sound CPU interferes
        with the main CPU test of shared RAM. You get a 'Sub CPU RAM Error'
    -  What do Scroll registers 0Eh and 0Fh really do ????
    -  Snow Bros 2 sets bit 6 of the sprite X info word during weather
        world map, and bits 4, 5 and 6 of the sprite X info word during
        the Rabbit boss screen - reasons are unknown.
    -  Fourth set of scroll registers have been used for Sprite scroll
        though it may not be correct. For most parts this looks right
        except for Snow Bros 2 when in the rabbit boss screen (all sprites
        jump when big green nasty (which is the foreground layer) comes
        in from the left)
    -  Teki Paki tests video RAM from address 0 past SpriteRAM to $37ff.
        This seems to be a bug in Teki Paki's vram test routine !
    -  Batsugun, relationship between the two video controllers (priority
        wise) is wrong and unknown.


 GP9001 Video RAM address layout:

    Bank          data size of video layer
    -----------------------------------------
    $0000-07FF    800h words for background layer
    $0800-0FFF    800h words for foreground layer
    $1000-17FF    800h words for top (text) layer
    $1800-1BFF    400h words for sprites (100 possible sprites)



 GP9001 Tile RAM format (each tile takes up 32 bits)

  0         1         2         3
  ---- ---- ---- ---- xxxx xxxx xxxx xxxx = Tile number (0 - FFFFh)
  ---- ---- -xxx xxxx ---- ---- ---- ---- = Color (0 - 7Fh)
  ---- ---- ?--- ---- ---- ---- ---- ---- = unknown / unused
  ---- xxxx ---- ---- ---- ---- ---- ---- = Priority (0 - Fh)
  ???? ---- ---- ---- ---- ---- ---- ---- = unknown / unused / possible flips

Sprites are of varying sizes between 8x8 and 128x128 with any variation
in between, in multiples of 8 either way.

Here we draw the first 8x8 part of the sprite, then by using the sprite
dimensions, we draw the rest of the 8x8 parts to produce the complete
sprite.

There seems to be sprite buffering - double buffering actually.

 GP9001 Sprite RAM format (data for each sprite takes up 4 words)

  0
  ---- ----  ---- --xx = top 2 bits of Sprite number
  ---- ----  xxxx xx-- = Color (0 - 3Fh)
  ---- xxxx  ---- ---- = Priority (0 - Fh)
  ---x ----  ---- ---- = Flip X
  --x- ----  ---- ---- = Flip Y
  -?-- ----  ---- ---- = unknown / unused
  x--- ----  ---- ---- = Show sprite ?

  1
  xxxx xxxx  xxxx xxxx = Sprite number (top two bits in word 0)

  2
  ---- ----  ---- xxxx = Sprite X size (add 1, then multiply by 8)
  ---- ----  -??? ---- = unknown - used in Snow Bros. 2
  xxxx xxxx  x--- ---- = X position

  3
  ---- ----  ---- xxxx = Sprite Y size (add 1, then multiply by 8)
  ---- ----  -??? ---- = unknown / unused
  xxxx xxxx  x--- ---- = Y position


 Extra-text RAM format

 Truxton 2, Fixeight and Raizing games have an extra-text layer.

  Text RAM format      $0000-1FFF (actually its probably $0000-0FFF)
  ---- --xx xxxx xxxx = Tile number
  xxxx xx-- ---- ---- = Color (0 - 3Fh) + 40h

  Text flip / ???      $0000-01EF (some games go to $01FF (excess?))
  ---x xxxx xxxx xxxx = ??? line something (line to draw ?) ???
  x--- ---- ---- ---- = flip for the Text tile

  Text X line-scroll ? $0000-01EF (some games go to $01FF (excess?))
  ---- ---x xxxx xxxx = X-Scroll for each line



 GP9001 Scroll Registers (hex) :

    00      Background scroll X (X flip off)
    01      Background scroll Y (Y flip off)
    02      Foreground scroll X (X flip off)
    03      Foreground scroll Y (Y flip off)
    04      Top (text) scroll X (X flip off)
    05      Top (text) scroll Y (Y flip off)
    06      Sprites    scroll X (X flip off) ???
    07      Sprites    scroll Y (Y flip off) ???
    0E      ??? Initialise Video controller at startup ???
    0F      Scroll update complete ??? (Not used in Ghox and V-Five)

    80      Background scroll X (X flip on)
    81      Background scroll Y (Y flip on)
    82      Foreground scroll X (X flip on)
    83      Foreground scroll Y (Y flip on)
    84      Top (text) scroll X (X flip on)
    85      Top (text) scroll Y (Y flip on)
    86      Sprites    scroll X (X flip on) ???
    87      Sprites    scroll Y (Y flip on) ???
    8F      Same as 0Fh except flip bit is active


Scroll Register 0E writes (Video controller inits ?) from different games:

Teki-Paki        | Ghox             | Knuckle Bash     | Truxton 2        |
0003, 0002, 4000 | ????, ????, ???? | 0202, 0203, 4200 | 0003, 0002, 4000 |

Dogyuun          | Batsugun         |
0202, 0203, 4200 | 0202, 0203, 4200 |
1202, 1203, 5200 | 1202, 1203, 5200 | <--- Second video controller

Pipi & Bibis     | Fix Eight        | V-Five           | Snow Bros. 2     |
0003, 0002, 4000 | 0202, 0203, 4200 | 0202, 0203, 4200 | 0202, 0203, 4200 |

***************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "drawgfxn.h"


#define TOAPLAN2_BG_VRAM_SIZE   0x1000	/* Background RAM size */
#define TOAPLAN2_FG_VRAM_SIZE   0x1000	/* Foreground RAM size */
#define TOAPLAN2_TOP_VRAM_SIZE  0x1000	/* Top Layer  RAM size */
#define TOAPLAN2_SPRITERAM_SIZE 0x800	/* Sprite     RAM size */
#define RAIZING_TX_GFXRAM_SIZE  0x8000	/* GFX data decode RAM size */
extern  size_t toaplan2_tx_vram_size;		 /* 0x2000 Text layer RAM size */
extern  size_t toaplan2_tx_offs_vram_size;	 /* 0x200 Text layer tile flip and positon ? */
extern  size_t toaplan2_tx_scroll_vram_size; /* 0x200 Text layer scroll ? */
extern  size_t batrider_paletteram16_size;



#define TOAPLAN2_SPRITE_FLIPX 0x1000	/* Sprite flip flags */
#define TOAPLAN2_SPRITE_FLIPY 0x2000

#define CPU_2_NONE		0x00
#define CPU_2_Z80		0x5a
#define CPU_2_HD647180	0xa5
#define CPU_2_V25		0xff


static UINT16 *bgvideoram16[2];
static UINT16 *fgvideoram16[2];
static UINT16 *topvideoram16[2];
static UINT16 *spriteram16_now[2];	/* Sprites to draw this frame */
static UINT16 *spriteram16_new[2];	/* Sprites to add to next frame */
static UINT16 *spriteram16_n[2];
UINT16 *toaplan2_txvideoram16;		/* Video ram for extra text layer */
UINT16 *toaplan2_txvideoram16_offs;	/* Text layer tile flip and positon ? */
UINT16 *toaplan2_txscrollram16;		/* Text layer scroll ? */
UINT16 *toaplan2_tx_gfxram16;			/* Text Layer RAM based tiles */
UINT16 *raizing_tx_gfxram16;			/* Text Layer RAM based tiles (Batrider) */
static int toaplan2_overflow_vram;		/* Teki Paki VRAM test is bugged. It goes out of range */

static int toaplan2_scroll_reg[2];
static int toaplan2_voffs[2];
static int bg_scrollx[2];
static int bg_scrolly[2];
static int fg_scrollx[2];
static int fg_scrolly[2];
static int top_scrollx[2];
static int top_scrolly[2];
static int sprite_scrollx[2];
static int sprite_scrolly[2];
static int objectbank_dirty = 0;		/* dirty flag of object bank (for Batrider) */
static int batrider_object_bank[8];		/* Batrider object bank */

#ifdef MAME_DEBUG
static int display_bg[2]  = { 1, 1 };
static int display_fg[2]  = { 1, 1 };
static int display_top[2] = { 1, 1 };
static int displog = 0;
static int display_tx = 1;
#endif
static int display_sp[2] = { 1, 1 };

static int sprite_priority[2][16];
static int bg_flip[2] = { 0, 0 };
static int fg_flip[2] = { 0, 0 };
static int top_flip[2] = { 0, 0 };
static int sprite_flip[2] = { 0, 0 };
static int tx_flip = 0;

extern int toaplan2_sub_cpu;

static UINT8 top_tile_priority[2][16];
static UINT8 fg_tile_priority[2][16];
static UINT8 bg_tile_priority[2][16];


static tilemap *top_tilemap[2], *fg_tilemap[2], *bg_tilemap[2];
static tilemap *tx_tilemap;	/* Tilemap for extra-text-layer */

static int xoffset[4];
static int yoffset[4];

static void defaultOffsets(void)
{
		xoffset[0]=0;
		xoffset[1]=0;
		xoffset[2]=0;
		xoffset[3]=0;

		yoffset[0]=0;
		yoffset[1]=0;
		yoffset[2]=0;
		yoffset[3]=0;
}

/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static void get_top0_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = topvideoram16[0][2*tile_index];
	tile_number = topvideoram16[0][2*tile_index+1];
	color = attrib & 0x7f;
	SET_TILE_INFO(
			0,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void get_fg0_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = fgvideoram16[0][2*tile_index];
	tile_number = fgvideoram16[0][2*tile_index+1];
	color = attrib & 0x7f;
	SET_TILE_INFO(
			0,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void get_bg0_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = bgvideoram16[0][2*tile_index];
	tile_number = bgvideoram16[0][2*tile_index+1];
	color = attrib & 0x7f;
	SET_TILE_INFO(
			0,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
/// if ((attrib & 0x0f00) == 0) tile_info.flags |= TILE_IGNORE_TRANSPARENCY;
}

static void get_top1_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = topvideoram16[1][2*tile_index];
	tile_number = topvideoram16[1][2*tile_index+1];
	color = attrib & 0x7f;
	SET_TILE_INFO(
			2,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void get_fg1_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = fgvideoram16[1][2*tile_index];
	tile_number = fgvideoram16[1][2*tile_index+1];
	color = attrib & 0x7f;
	SET_TILE_INFO(
			2,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void get_bg1_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = bgvideoram16[1][2*tile_index];
	tile_number = bgvideoram16[1][2*tile_index+1];
	color = attrib & 0x7f;
	SET_TILE_INFO(
			2,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void batrider_get_top0_tile_info(int tile_index)
{
	int color, tile_number, attrib, tile;

	attrib = topvideoram16[0][2*tile_index];
	tile = topvideoram16[0][2*tile_index+1];
	tile_number = ( batrider_object_bank[(tile >> 13) & 7] << 13 ) | ( tile & 0x1fff );
	color = attrib & 0x7f;
	SET_TILE_INFO(
			0,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void batrider_get_fg0_tile_info(int tile_index)
{
	int color, tile_number, attrib, tile;

	attrib = fgvideoram16[0][2*tile_index];
	tile = fgvideoram16[0][2*tile_index+1];
	tile_number = ( batrider_object_bank[(tile >> 13) & 7] << 13 ) | ( tile & 0x1fff );
	color = attrib & 0x7f;
	SET_TILE_INFO(
			0,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void batrider_get_bg0_tile_info(int tile_index)
{
	int color, tile_number, attrib, tile;

	attrib = bgvideoram16[0][2*tile_index];
	tile = bgvideoram16[0][2*tile_index+1];
	tile_number = ( batrider_object_bank[(tile >> 13) & 7] << 13 ) | ( tile & 0x1fff );
	color = attrib & 0x7f;
	SET_TILE_INFO(
			0,
			tile_number,
			color,
			0)
	tile_info.priority = (attrib & 0x0f00) >> 8;
}

static void get_text_tile_info(int tile_index)
{
	int color, tile_number, attrib;

	attrib = toaplan2_txvideoram16[tile_index];
	tile_number = attrib & 0x3ff;
	color = ((attrib >> 10) | 0x40) & 0x7f;
	SET_TILE_INFO(
			2,
			tile_number,
			color,
			0)
	tile_info.priority = 0;
}

/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/
static int create_tilemaps_0(void)
{
	top_tilemap[0] = tilemap_create(get_top0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	fg_tilemap[0] = tilemap_create(get_fg0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	bg_tilemap[0] = tilemap_create(get_bg0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);

	if (!top_tilemap[0] || !fg_tilemap[0] || !bg_tilemap[0])
		return 1;

	tilemap_set_transparent_pen(top_tilemap[0],0);
	tilemap_set_transparent_pen(fg_tilemap[0],0);
	tilemap_set_transparent_pen(bg_tilemap[0],0);

	return 0;
}

static int create_tilemaps_1(void)
{
	top_tilemap[1] = tilemap_create(get_top1_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	fg_tilemap[1] = tilemap_create(get_fg1_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	bg_tilemap[1] = tilemap_create(get_bg1_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);

	if (!top_tilemap[1] || !fg_tilemap[1] || !bg_tilemap[1])
		return 1;

	tilemap_set_transparent_pen(top_tilemap[1],0);
	tilemap_set_transparent_pen(fg_tilemap[1],0);
	tilemap_set_transparent_pen(bg_tilemap[1],0);

	return 0;
}

static int truxton2_create_tilemaps_0(void)
{
	tx_tilemap = tilemap_create(get_text_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);
	top_tilemap[0] = tilemap_create(get_top0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	fg_tilemap[0] = tilemap_create(get_fg0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	bg_tilemap[0] = tilemap_create(get_bg0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);

	if (!tx_tilemap || !top_tilemap[0] || !fg_tilemap[0] || !bg_tilemap[0])
		return 1;

	tilemap_set_scroll_rows(tx_tilemap,8*32);	/* line scrolling */
	tilemap_set_scroll_cols(tx_tilemap,1);

	tilemap_set_transparent_pen(tx_tilemap,0);
	tilemap_set_transparent_pen(top_tilemap[0],0);
	tilemap_set_transparent_pen(fg_tilemap[0],0);
	tilemap_set_transparent_pen(bg_tilemap[0],0);

	return 0;
}

static int batrider_create_tilemaps_0(void)
{
	tx_tilemap = tilemap_create(get_text_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);
	top_tilemap[0] = tilemap_create(batrider_get_top0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	fg_tilemap[0] = tilemap_create(batrider_get_fg0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);
	bg_tilemap[0] = tilemap_create(batrider_get_bg0_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,32,32);

	if (!tx_tilemap || !top_tilemap[0] || !fg_tilemap[0] || !bg_tilemap[0])
		return 1;

	tilemap_set_scroll_rows(tx_tilemap,8*32);	/* line scrolling */
	tilemap_set_scroll_cols(tx_tilemap,1);

	tilemap_set_transparent_pen(tx_tilemap,0);
	tilemap_set_transparent_pen(top_tilemap[0],0);
	tilemap_set_transparent_pen(fg_tilemap[0],0);
	tilemap_set_transparent_pen(bg_tilemap[0],0);

	return 0;
}


static int toaplan2_vram_alloc(int controller)
{
	spriteram16_new[controller] = auto_malloc(TOAPLAN2_SPRITERAM_SIZE);
	memset(spriteram16_new[controller],0,TOAPLAN2_SPRITERAM_SIZE);

	spriteram16_now[controller] = auto_malloc(TOAPLAN2_SPRITERAM_SIZE);
	memset(spriteram16_now[controller],0,TOAPLAN2_SPRITERAM_SIZE);

	topvideoram16[controller] = auto_malloc(TOAPLAN2_TOP_VRAM_SIZE);
	memset(topvideoram16[controller],0,TOAPLAN2_TOP_VRAM_SIZE);

	fgvideoram16[controller] = auto_malloc(TOAPLAN2_FG_VRAM_SIZE);
	memset(fgvideoram16[controller],0,TOAPLAN2_FG_VRAM_SIZE);

	bgvideoram16[controller] = auto_malloc(TOAPLAN2_BG_VRAM_SIZE);
	memset(bgvideoram16[controller],0,TOAPLAN2_BG_VRAM_SIZE);

	spriteram16_n[controller] = spriteram16_now[controller];

	return 0;
}

static int toaplan2_vh_start(int controller)
{
	static int error_level = 0;

	if (toaplan2_vram_alloc(controller))
		return 1;

	if (controller == 0)
	{
		error_level |= create_tilemaps_0();
	}
	if (controller == 1)
	{
		error_level |= create_tilemaps_1();
	}
	return error_level;
}

VIDEO_START( toaplan2_0 )
{
	defaultOffsets();
	return toaplan2_vh_start(0);
}

VIDEO_START( toaplan2_1 )
{
	int error_level = 0;
	error_level |= toaplan2_vh_start(0);
	error_level |= toaplan2_vh_start(1);
	defaultOffsets();
	return error_level;
}

VIDEO_START( truxton2_0 )
{
	if (toaplan2_vram_alloc(0))
	{
		return 1;
	}
	if (truxton2_create_tilemaps_0())
	{
		return 1;
	}
	if(!strcmp(Machine->gamedrv->name,"fixeighb"))
	{
		xoffset[0]=-26;
		xoffset[1]=-22;
		xoffset[2]=-18;
		xoffset[3]=8;

		yoffset[0]=-15;
		yoffset[1]=-15;
		yoffset[2]=-15;
		yoffset[3]=8;
		tilemap_set_scrolldx(tx_tilemap, 0, 0);
	}
	else
	{
		defaultOffsets();
		tilemap_set_scrolldx(tx_tilemap, 0x1d4 +1, 0x2a);
	}
	return 0;
}

VIDEO_START( battleg_0 )
{
	if (toaplan2_vram_alloc(0))
	{
		return 1;
	}
	if (truxton2_create_tilemaps_0())
	{
		return 1;
	}
	tilemap_set_scrolldx(tx_tilemap, 0x1d4, 0x2a);
	defaultOffsets();
	return 0;
}

VIDEO_START( batrider_0 )
{
	raizing_tx_gfxram16 = auto_malloc(RAIZING_TX_GFXRAM_SIZE);
	memset(raizing_tx_gfxram16,0,RAIZING_TX_GFXRAM_SIZE);

	if (toaplan2_vram_alloc(0))
		return 1;
	spriteram16_n[0] = spriteram16_new[0];

	if (batrider_create_tilemaps_0())
		return 1;

	tilemap_set_scrolldx(tx_tilemap, 0x1d4, 0x2a);
	defaultOffsets();
	return 0;
}


/***************************************************************************

  Video I/O port hardware.

***************************************************************************/

void toaplan2_voffs_w(offs_t offset, UINT16 data, UINT32 mem_mask, int controller)
{
	// if (data >= 0x1c00)
	// 	loginfo(2,"Hmmm, unknown video controller %01x layer being selected (%08x)\n",controller,data);
	COMBINE_DATA(&toaplan2_voffs[controller]);
}

WRITE16_HANDLER( toaplan2_0_voffs_w )
{
	toaplan2_voffs_w(offset, data, mem_mask, 0);
}

WRITE16_HANDLER( toaplan2_1_voffs_w )
{
	toaplan2_voffs_w(offset, data, mem_mask, 1);
}

READ16_HANDLER( toaplan2_txvideoram16_r )
{
	return toaplan2_txvideoram16[offset];
}

WRITE16_HANDLER( toaplan2_txvideoram16_w )
{
	UINT16 oldword = toaplan2_txvideoram16[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&toaplan2_txvideoram16[offset]);
		if (offset < (toaplan2_tx_vram_size/4))
			tilemap_mark_tile_dirty(tx_tilemap,offset);
	}
}

READ16_HANDLER( toaplan2_txvideoram16_offs_r )
{
	return toaplan2_txvideoram16_offs[offset];
}
WRITE16_HANDLER( toaplan2_txvideoram16_offs_w )
{
	/* Besides containing flip, function of this RAM is still unknown */
	/* This is however line related as per line-scroll RAM below */
	/* Maybe specifies which line to draw text info (line number data is */
	/*   opposite when flip bits are on) */

	UINT16 oldword = toaplan2_txvideoram16_offs[offset];

	if (oldword != data)
	{
		if (offset == 0)			/* Wrong ! */
		{
			if (data & 0x8000)		/* Flip off */
			{
				tx_flip = 0;
				tilemap_set_flip(tx_tilemap, tx_flip);
				tilemap_set_scrolly(tx_tilemap, 0, 0);
			}
			else					/* Flip on */
			{
				tx_flip = (TILEMAP_FLIPY | TILEMAP_FLIPX);
				tilemap_set_flip(tx_tilemap, tx_flip);
				tilemap_set_scrolly(tx_tilemap, 0, -16);
			}
		}
		COMBINE_DATA(&toaplan2_txvideoram16_offs[offset]);
	}
//  loginfo(2,"Writing %04x to text offs RAM offset %04x\n",data,offset);
}

READ16_HANDLER( toaplan2_txscrollram16_r )
{
	return toaplan2_txscrollram16[offset];
}
WRITE16_HANDLER( toaplan2_txscrollram16_w )
{
	/*** Line-Scroll RAM for Text Layer ***/

	int data_tx = data;

	tilemap_set_scrollx(tx_tilemap, offset, data_tx);

//  loginfo(2,"Writing %04x to text scroll RAM offset %04x\n",data,offset);
	COMBINE_DATA(&toaplan2_txscrollram16[offset]);
}

READ16_HANDLER( toaplan2_tx_gfxram16_r )
{
	return toaplan2_tx_gfxram16[offset];
}

WRITE16_HANDLER( toaplan2_tx_gfxram16_w )
{
	/*** Dynamic GFX decoding for Truxton 2 / FixEight ***/

	UINT16 oldword = toaplan2_tx_gfxram16[offset];
	UINT8 *toaplan2_tx_gfxram = (UINT8 *)(toaplan2_tx_gfxram16);

	if (oldword != data)
	{
		int code = offset/32;
		COMBINE_DATA(&toaplan2_tx_gfxram16[offset]);
		decodechar(Machine->gfx[2], code, toaplan2_tx_gfxram,
					Machine->drv->gfxdecodeinfo[2].gfxlayout);

		tilemap_mark_all_tiles_dirty(tx_tilemap);
	}
}

READ16_HANDLER( raizing_tx_gfxram16_r )
{
	offset += 0x3400/2;
	return raizing_tx_gfxram16[offset];
}
WRITE16_HANDLER( raizing_tx_gfxram16_w )
{
	/*** Dynamic Text GFX decoding for Batrider ***/

	UINT16 oldword = raizing_tx_gfxram16[offset + (0x3400 / 2)];

	if (oldword != data)
	{
		offset += 0x3400/2;
		COMBINE_DATA(&raizing_tx_gfxram16[offset]);
	}
}

WRITE16_HANDLER( batrider_textdata_decode )
{
	/*** Dynamic Text GFX decoding for Batrider ***/
	/*** Only done once during start-up ***/

	int code;
	UINT8 *raizing_tx_gfxram = (UINT8 *)raizing_tx_gfxram16;
	UINT16 *dest = (UINT16 *)raizing_tx_gfxram16;

	memcpy(dest, toaplan2_txvideoram16, toaplan2_tx_vram_size);
	dest += (toaplan2_tx_vram_size/2);
	memcpy(dest, paletteram16, batrider_paletteram16_size);
	dest += (batrider_paletteram16_size/2);
	memcpy(dest, toaplan2_txvideoram16_offs, toaplan2_tx_offs_vram_size);
	dest += (toaplan2_tx_offs_vram_size/2);
	memcpy(dest, toaplan2_txscrollram16, toaplan2_tx_scroll_vram_size);

	/* Decode text characters */
	for (code = 0; code < 1024; code++)
		decodechar (Machine->gfx[2], code, raizing_tx_gfxram, Machine->drv->gfxdecodeinfo[2].gfxlayout);
	tilemap_mark_all_tiles_dirty(tx_tilemap);
}

WRITE16_HANDLER( batrider_objectbank_w )
{
	if (ACCESSING_LSB)
	{
		data &= 0xf;
		if (batrider_object_bank[offset] != data)
		{
			batrider_object_bank[offset] = data;
			objectbank_dirty = 1;
		}
	}
}



int toaplan2_videoram16_r(offs_t offset, int controller)
{
	static UINT16 video_data = 0;
	static offs_t vram_offset;


	switch (toaplan2_voffs[controller] & 0xfc00)
	{
		case 0x0400:
		case 0x0000:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_BG_VRAM_SIZE/2)-1);
				video_data = bgvideoram16[controller][vram_offset];
				break;
		case 0x0c00:
		case 0x0800:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_FG_VRAM_SIZE/2)-1);
				video_data = fgvideoram16[controller][vram_offset];
				break;
		case 0x1400:
		case 0x1000:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_TOP_VRAM_SIZE/2)-1);
				video_data = topvideoram16[controller][vram_offset];
				break;
		case 0x1800:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_SPRITERAM_SIZE/2)-1);
				video_data = spriteram16_new[controller][vram_offset];
				break;
		default:
				video_data = toaplan2_overflow_vram;
				loginfo(2,"Hmmm, reading %04x from unknown VC:%01x layer address %06x  Offset:%01x !!!\n",video_data,controller,toaplan2_voffs[controller],offset);
				break;
	}
	toaplan2_voffs[controller]++;
	return video_data;
}

READ16_HANDLER( toaplan2_0_videoram16_r )
{
	return toaplan2_videoram16_r(offset, 0);
}

READ16_HANDLER( toaplan2_1_videoram16_r )
{
	return toaplan2_videoram16_r(offset, 1);
}

void toaplan2_videoram16_w(offs_t offset, UINT16 data, UINT32 mem_mask, int controller)
{
	UINT16 oldword = 0;
	offs_t vram_offset;

	switch (toaplan2_voffs[controller] & 0xfc00)
	{
		case 0x0400:
		case 0x0000:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_BG_VRAM_SIZE/2)-1);
				oldword = bgvideoram16[controller][vram_offset];
				if (data != oldword)
				{
					COMBINE_DATA(&bgvideoram16[controller][vram_offset]);
					tilemap_mark_tile_dirty(bg_tilemap[controller],vram_offset/2);
				}
				break;
		case 0x0c00:
		case 0x0800:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_FG_VRAM_SIZE/2)-1);
				oldword = fgvideoram16[controller][vram_offset];
				if (data != oldword)
				{
					COMBINE_DATA(&fgvideoram16[controller][vram_offset]);
					tilemap_mark_tile_dirty(fg_tilemap[controller],vram_offset/2);
				}
				break;
		case 0x1400:
		case 0x1000:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_TOP_VRAM_SIZE/2)-1);
				oldword = topvideoram16[controller][vram_offset];
				if (data != oldword)
				{
					COMBINE_DATA(&topvideoram16[controller][vram_offset]);
					tilemap_mark_tile_dirty(top_tilemap[controller],vram_offset/2);
				}
				break;
		case 0x1800:
				vram_offset = toaplan2_voffs[controller] & ((TOAPLAN2_SPRITERAM_SIZE/2)-1);
				COMBINE_DATA(&spriteram16_new[controller][vram_offset]);
				break;
		default:
				toaplan2_overflow_vram = data;
				//loginfo(2,"Hmmm, writing %04x to unknown VC:%01x layer address %06x  Offset:%01x\n",data,controller,toaplan2_voffs[controller],offset);
				break;
	}
	toaplan2_voffs[controller]++;
}

WRITE16_HANDLER( toaplan2_0_videoram16_w )
{
	toaplan2_videoram16_w(offset, data, mem_mask, 0);
}

WRITE16_HANDLER( toaplan2_1_videoram16_w )
{
	toaplan2_videoram16_w(offset, data, mem_mask, 1);
}


void toaplan2_scroll_reg_select_w(offs_t offset, UINT16 data, UINT32 mem_mask, int controller)
{
	if (ACCESSING_LSB)
	{
		toaplan2_scroll_reg[controller] = data & 0x8f;
		if (data & 0x70)
			loginfo(2,"Hmmm, selecting unknown LSB video control register (%04x)  Video controller %01x  \n",toaplan2_scroll_reg[controller],controller);
	}
	else
	{
		loginfo(2,"Hmmm, selecting unknown MSB video control register (%04x)  Video controller %01x  \n",toaplan2_scroll_reg[controller],controller);
	}
}

WRITE16_HANDLER( toaplan2_0_scroll_reg_select_w )
{
	toaplan2_scroll_reg_select_w(offset, data, mem_mask, 0);
}

WRITE16_HANDLER( toaplan2_1_scroll_reg_select_w )
{
	toaplan2_scroll_reg_select_w(offset, data, mem_mask, 1);
}


void toaplan2_scroll_reg_data_w(offs_t offset, UINT16 data, UINT32 mem_mask, int controller)
{
	/************************************************************************/
	/***** layer X and Y flips can be set independantly, so emulate it ******/
	/************************************************************************/

#ifdef MAME_DEBUG
	int vid_controllers = 1;
#endif

	switch(toaplan2_scroll_reg[controller])
	{
		case 0x00:	data -= 0x1d6;			/* 1D6h */
					COMBINE_DATA(&bg_scrollx[controller]);
					bg_flip[controller] &= (~TILEMAP_FLIPX);
					tilemap_set_flip(bg_tilemap[controller],bg_flip[controller]);
					tilemap_set_scrollx(bg_tilemap[controller],0,bg_scrollx[controller]+xoffset[0]);
					break;
		case 0x01:	data -= 0x1ef;			/* 1EFh */
					COMBINE_DATA(&bg_scrolly[controller]);
					bg_flip[controller] &= (~TILEMAP_FLIPY);
					tilemap_set_flip(bg_tilemap[controller],bg_flip[controller]);
					tilemap_set_scrolly(bg_tilemap[controller],0,bg_scrolly[controller]+yoffset[0]);
					break;
		case 0x02:	data -= 0x1d8;			/* 1D0h */
					COMBINE_DATA(&fg_scrollx[controller]);
					fg_flip[controller] &= (~TILEMAP_FLIPX);
					tilemap_set_flip(fg_tilemap[controller],fg_flip[controller]);
					tilemap_set_scrollx(fg_tilemap[controller],0,fg_scrollx[controller]+xoffset[1]);
					break;
		case 0x03:  data -= 0x1ef;			/* 1EFh */
					COMBINE_DATA(&fg_scrolly[controller]);
					fg_flip[controller] &= (~TILEMAP_FLIPY);
					tilemap_set_flip(fg_tilemap[controller],fg_flip[controller]);
					tilemap_set_scrolly(fg_tilemap[controller],0,fg_scrolly[controller]+yoffset[1]);
					break;
		case 0x04:	data -= 0x1da;			/* 1DAh */
					COMBINE_DATA(&top_scrollx[controller]);
					top_flip[controller] &= (~TILEMAP_FLIPX);
					tilemap_set_flip(top_tilemap[controller],top_flip[controller]);
					tilemap_set_scrollx(top_tilemap[controller],0,top_scrollx[controller]+xoffset[2]);
					break;
		case 0x05:	data -= 0x1ef;			/* 1EFh */
					COMBINE_DATA(&top_scrolly[controller]);
					top_flip[controller] &= (~TILEMAP_FLIPY);
					tilemap_set_flip(top_tilemap[controller],top_flip[controller]);
					tilemap_set_scrolly(top_tilemap[controller],0,top_scrolly[controller]+yoffset[2]);
					break;
		case 0x06:  data -= 0x1cc;			/* 1D4h */
					COMBINE_DATA(&sprite_scrollx[controller]);
					if (sprite_scrollx[controller] & 0x8000) sprite_scrollx[controller] |= 0xfffffe00;
					else sprite_scrollx[controller] &= 0x1ff;
					sprite_flip[controller] &= (~TOAPLAN2_SPRITE_FLIPX);
					break;
		case 0x07:	data -= 0x1ef;      /* 1F7h */
					COMBINE_DATA(&sprite_scrolly[controller]);
					if (sprite_scrolly[controller] & 0x8000) sprite_scrolly[controller] |= 0xfffffe00;
					else sprite_scrolly[controller] &= 0x1ff;
					sprite_flip[controller] &= (~TOAPLAN2_SPRITE_FLIPY);
					break;
		case 0x0f:	break;
		case 0x80:  data -= 0x229;			/* 169h */
					COMBINE_DATA(&bg_scrollx[controller]);
					bg_flip[controller] |= TILEMAP_FLIPX;
					tilemap_set_flip(bg_tilemap[controller],bg_flip[controller]);
					tilemap_set_scrollx(bg_tilemap[controller],0,bg_scrollx[controller]+xoffset[0]);
					break;
		case 0x81:	data -= 0x210;			/* 100h */
					COMBINE_DATA(&bg_scrolly[controller]);
					bg_flip[controller] |= TILEMAP_FLIPY;
					tilemap_set_flip(bg_tilemap[controller],bg_flip[controller]);
					tilemap_set_scrolly(bg_tilemap[controller],0,bg_scrolly[controller]+yoffset[0]);
					break;
		case 0x82:	data -= 0x227;			/* 15Fh */
					COMBINE_DATA(&fg_scrollx[controller]);
					fg_flip[controller] |= TILEMAP_FLIPX;
					tilemap_set_flip(fg_tilemap[controller],fg_flip[controller]);
					tilemap_set_scrollx(fg_tilemap[controller],0,fg_scrollx[controller]+xoffset[1]);
					break;
		case 0x83:	data -= 0x210;			/* 100h */
					COMBINE_DATA(&fg_scrolly[controller]);
					fg_flip[controller] |= TILEMAP_FLIPY;
					tilemap_set_flip(fg_tilemap[controller],fg_flip[controller]);
					tilemap_set_scrolly(fg_tilemap[controller],0,fg_scrolly[controller]+yoffset[1]);
					break;
		case 0x84:	data -= 0x225;			/* 165h */
					COMBINE_DATA(&top_scrollx[controller]);
					top_flip[controller] |= TILEMAP_FLIPX;
					tilemap_set_flip(top_tilemap[controller],top_flip[controller]);
					tilemap_set_scrollx(top_tilemap[controller],0,top_scrollx[controller]+xoffset[2]);
					break;
		case 0x85:	data -= 0x210;			/* 100h */
					COMBINE_DATA(&top_scrolly[controller]);
					top_flip[controller] |= TILEMAP_FLIPY;
					tilemap_set_flip(top_tilemap[controller],top_flip[controller]);
					tilemap_set_scrolly(top_tilemap[controller],0,top_scrolly[controller]+yoffset[2]);
					break;
		case 0x86:	data -= 0x17b;			/* 17Bh */
					COMBINE_DATA(&sprite_scrollx[controller]);
					if (sprite_scrollx[controller] & 0x8000) sprite_scrollx[controller] |= 0xfffffe00;
					else sprite_scrollx[controller] &= 0x1ff;
					sprite_flip[controller] |= TOAPLAN2_SPRITE_FLIPX;
					break;
		case 0x87:	data -= 0x108;			/* 108h */
					COMBINE_DATA(&sprite_scrolly[controller]);
					if (sprite_scrolly[controller] & 0x8000) sprite_scrolly[controller] |= 0xfffffe00;
					else sprite_scrolly[controller] &= 0x1ff;
					sprite_flip[controller] |= TOAPLAN2_SPRITE_FLIPY;
					break;
		case 0x8f:	break;

		case 0x0e:	/******* Initialise video controller register ? *******/
					if ((toaplan2_sub_cpu == CPU_2_Z80) && (data == 3))
					{
						/* HACK! When tilted, sound CPU needs to be reset. */
						if (Machine->drv->sound[0].sound_type == SOUND_YM3812)
						{
							cpunum_set_input_line(1, INPUT_LINE_RESET, PULSE_LINE);
							sndti_reset(SOUND_YM3812, 0);
						}
					}

		default:	//loginfo(2,"Hmmm, writing %08x to unknown video control register (%08x)  Video controller %01x  !!!\n",data ,toaplan2_scroll_reg[controller],controller);
					break;
	}

#ifdef MAME_DEBUG

	if (spriteram16_now[1] && spriteram16_new[1]
		&& top_tilemap[1] && fg_tilemap[1] && bg_tilemap[1])
	{
		vid_controllers = 2;
	}

	if ( code_pressed_memory(KEYCODE_W) )
	{
		display_tx += 1;
		display_tx &= 1;
		if (toaplan2_txvideoram16 != 0)
			tilemap_set_enable(tx_tilemap, display_tx);
	}
	if ( code_pressed_memory(KEYCODE_L) )
	{
		display_sp[0] += 1;
		display_sp[0] &= 1;
	}
	if ( code_pressed_memory(KEYCODE_K) )
	{
		display_top[0] += 1;
		display_top[0] &= 1;
		tilemap_set_enable(top_tilemap[0], display_top[0]);
	}
	if ( code_pressed_memory(KEYCODE_J) )
	{
		display_fg[0] += 1;
		display_fg[0] &= 1;
		tilemap_set_enable(fg_tilemap[0], display_fg[0]);
	}
	if ( code_pressed_memory(KEYCODE_H) )
	{
		display_bg[0] += 1;
		display_bg[0] &= 1;
		tilemap_set_enable(bg_tilemap[0], display_bg[0]);
	}
	if (vid_controllers == 2)
	{
		if ( code_pressed_memory(KEYCODE_O) )
		{
			display_sp[1] += 1;
			display_sp[1] &= 1;
		}
		if ( code_pressed_memory(KEYCODE_I) )
		{
			display_top[1] += 1;
			display_top[1] &= 1;
			tilemap_set_enable(top_tilemap[1], display_top[1]);
		}
		if ( code_pressed_memory(KEYCODE_U) )
		{
			display_fg[1] += 1;
			display_fg[1] &= 1;
			tilemap_set_enable(fg_tilemap[1], display_fg[1]);
		}
		if ( code_pressed_memory(KEYCODE_Y) )
		{
			display_bg[1] += 1;
			display_bg[1] &= 1;
			tilemap_set_enable(bg_tilemap[1], display_bg[1]);
		}
	}
#endif
}

WRITE16_HANDLER( toaplan2_0_scroll_reg_data_w )
{
	toaplan2_scroll_reg_data_w(offset, data, mem_mask, 0);
}

WRITE16_HANDLER( toaplan2_1_scroll_reg_data_w )
{
	toaplan2_scroll_reg_data_w(offset, data, mem_mask, 1);
}


/***************************************************************************/
/**************** PIPIBIBI interface into this video driver ****************/

WRITE16_HANDLER( pipibibi_scroll_w )
{
	if (ACCESSING_MSB && ACCESSING_LSB)
	{
		switch(offset)
		{
			case 0x00:	data -= 0x01f; break;
			case 0x01:	data += 0x1ef; break;
			case 0x02:	data -= 0x01d; break;
			case 0x03:	data += 0x1ef; break;
			case 0x04:	data -= 0x01b; break;
			case 0x05:	data += 0x1ef; break;
			case 0x06:	data += 0x1d4; break;
			case 0x07:	data += 0x1f7; break;
			default:	//loginfo(2,"PIPIBIBI writing %04x to unknown scroll register %04x",data, offset);
                break;
		}

		toaplan2_scroll_reg[0] = offset;
		toaplan2_scroll_reg_data_w(offset, data, mem_mask, 0);
	}
}

READ16_HANDLER( pipibibi_videoram16_r )
{
	toaplan2_voffs_w(0, offset, 0, 0);
	return toaplan2_videoram16_r(0, 0);
}

WRITE16_HANDLER( pipibibi_videoram16_w)
{
	toaplan2_voffs_w(0, offset, 0, 0);
	toaplan2_videoram16_w(0, data, mem_mask, 0);
}

READ16_HANDLER( pipibibi_spriteram16_r )
{
	toaplan2_voffs_w(0, (0x1800 + offset), 0, 0);
	return toaplan2_videoram16_r(0, 0);
}

WRITE16_HANDLER( pipibibi_spriteram16_w )
{
	toaplan2_voffs_w(0, (0x1800 + offset), mem_mask, 0);
	toaplan2_videoram16_w(0, data, mem_mask, 0);
}



#ifdef MAME_DEBUG
void toaplan2_log_vram(void)
{
	offs_t sprite_voffs, tile_voffs;
	int vid_controllers = 1;

	if (spriteram16_now[1] && spriteram16_new[1]
		&& top_tilemap[1] && fg_tilemap[1] && bg_tilemap[1])
	{
		vid_controllers = 2;
	}

	if ( code_pressed_memory(KEYCODE_M) )
	{
		UINT16 *source_now0  = (UINT16 *)(spriteram16_now[0]);
		UINT16 *source_new0  = (UINT16 *)(spriteram16_new[0]);
		UINT16 *source_now1  = (UINT16 *)(spriteram16_now[0]);
		UINT16 *source_new1  = (UINT16 *)(spriteram16_new[0]);

		int schar[2],sattr[2],sxpos[2],sypos[2];

		if (vid_controllers == 2)
		{
			source_now1  = (UINT16 *)(spriteram16_now[1]);
			source_new1  = (UINT16 *)(spriteram16_new[1]);
		}

		loginfo(2,"Scrolls   BG-X  BG-Y   FG-X  FG-Y   TOP-X  TOP-Y   Sprite-X  Sprite-Y\n");
		loginfo(2,"---0-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[0],bg_scrolly[0],fg_scrollx[0],fg_scrolly[0],top_scrollx[0],top_scrolly[0],sprite_scrollx[0], sprite_scrolly[0]);
		if (vid_controllers == 2)
		{
			loginfo(2,"---1-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[1],bg_scrolly[1],fg_scrollx[1],fg_scrolly[1],top_scrollx[1],top_scrolly[1],sprite_scrollx[1], sprite_scrolly[1]);
		}
		for ( sprite_voffs = 0; sprite_voffs < (TOAPLAN2_SPRITERAM_SIZE/2); sprite_voffs += 4 )
		{
			sattr[0] = source_now0[sprite_voffs];
			schar[0] = source_now0[sprite_voffs + 1];
			sxpos[0] = source_now0[sprite_voffs + 2];
			sypos[0] = source_now0[sprite_voffs + 3];
			sattr[1] = source_new0[sprite_voffs];
			schar[1] = source_new0[sprite_voffs + 1];
			sxpos[1] = source_new0[sprite_voffs + 2];
			sypos[1] = source_new0[sprite_voffs + 3];
			loginfo(2,"SPoffs    Sprt Attr Xpos Ypos     Sprt Attr Xpos Ypos\n");
			loginfo(2,"0:%03x now:%04x %04x %04x %04x new:%04x %04x %04x %04x\n",sprite_voffs,
						 						schar[0], sattr[0],sxpos[0], sypos[0],
						 						schar[1], sattr[1],sxpos[1], sypos[1]);
			if (vid_controllers == 2)
			{
				sattr[0] = source_now1[sprite_voffs];
				schar[0] = source_now1[sprite_voffs + 1];
				sxpos[0] = source_now1[sprite_voffs + 2];
				sypos[0] = source_now1[sprite_voffs + 3];
				sattr[1] = source_new1[sprite_voffs];
				schar[1] = source_new1[sprite_voffs + 1];
				sxpos[1] = source_new1[sprite_voffs + 2];
				sypos[1] = source_new1[sprite_voffs + 3];
				loginfo(2,"1:%03x now:%04x %04x %04x %04x new:%04x %04x %04x %04x\n",sprite_voffs,
							 					schar[0], sattr[0],sxpos[0], sypos[0],
							 					schar[1], sattr[1],sxpos[1], sypos[1]);
			}
		}
	}
	if ( code_pressed_memory(KEYCODE_N) )
	{
		int tchar[2], tattr[2];
		loginfo(2,"Scrolls   BG-X  BG-Y   FG-X  FG-Y   TOP-X  TOP-Y   Sprite-X  Sprite-Y\n");
		loginfo(2,"---0-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[0],bg_scrolly[0],fg_scrollx[0],fg_scrolly[0],top_scrollx[0],top_scrolly[0],sprite_scrollx[0], sprite_scrolly[0]);
		if (vid_controllers == 2)
		{
			loginfo(2,"---1-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[1],bg_scrolly[1],fg_scrollx[1],fg_scrolly[1],top_scrollx[1],top_scrolly[1],sprite_scrollx[1], sprite_scrolly[1]);
		}
		for ( tile_voffs = 0; tile_voffs < (TOAPLAN2_TOP_VRAM_SIZE/2); tile_voffs += 2 )
		{
			tchar[0] = topvideoram16[0][tile_voffs + 1];
			tattr[0] = topvideoram16[0][tile_voffs];
			if (vid_controllers == 2)
			{
				tchar[1] = topvideoram16[1][tile_voffs + 1];
				tattr[1] = topvideoram16[1][tile_voffs];
				loginfo(2,"TOPoffs:%04x   Tile0:%04x  Attr0:%04x    Tile1:%04x  Attr1:%04x\n", tile_voffs, tchar[0], tattr[0], tchar[1], tattr[1]);
			}
			else
			{
				loginfo(2,"TOPoffs:%04x   Tile0:%04x  Attr0:%04x\n", tile_voffs, tchar[0], tattr[0]);
			}
		}
	}
	if ( code_pressed_memory(KEYCODE_B) )
	{
		int tchar[2], tattr[2];
		loginfo(2,"Scrolls   BG-X  BG-Y   FG-X  FG-Y   TOP-X  TOP-Y   Sprite-X  Sprite-Y\n");
		loginfo(2,"---0-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[0],bg_scrolly[0],fg_scrollx[0],fg_scrolly[0],top_scrollx[0],top_scrolly[0],sprite_scrollx[0], sprite_scrolly[0]);
		if (vid_controllers == 2)
		{
			loginfo(2,"---1-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[1],bg_scrolly[1],fg_scrollx[1],fg_scrolly[1],top_scrollx[1],top_scrolly[1],sprite_scrollx[1], sprite_scrolly[1]);
		}
		for ( tile_voffs = 0; tile_voffs < (TOAPLAN2_FG_VRAM_SIZE/2); tile_voffs += 2 )
		{
			tchar[0] = fgvideoram16[0][tile_voffs + 1];
			tattr[0] = fgvideoram16[0][tile_voffs];
		if (vid_controllers == 2)
			{
				tchar[1] = fgvideoram16[1][tile_voffs + 1];
				tattr[1] = fgvideoram16[1][tile_voffs];
				loginfo(2,"FGoffs:%04x   Tile0:%04x  Attr0:%04x    Tile1:%04x  Attr1:%04x\n", tile_voffs, tchar[0], tattr[0], tchar[1], tattr[1]);
			}
			else
			{
				loginfo(2,"FGoffs:%04x   Tile0:%04x  Attr0:%04x\n", tile_voffs, tchar[0], tattr[0]);
			}
		}
	}
	if ( code_pressed_memory(KEYCODE_V) )
	{
		int tchar[2], tattr[2];
		loginfo(2,"Scrolls   BG-X  BG-Y   FG-X  FG-Y   TOP-X  TOP-Y   Sprite-X  Sprite-Y\n");
		loginfo(2,"---0-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[0],bg_scrolly[0],fg_scrollx[0],fg_scrolly[0],top_scrollx[0],top_scrolly[0],sprite_scrollx[0], sprite_scrolly[0]);
		if (vid_controllers == 2)
		{
			loginfo(2,"---1-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[1],bg_scrolly[1],fg_scrollx[1],fg_scrolly[1],top_scrollx[1],top_scrolly[1],sprite_scrollx[1], sprite_scrolly[1]);
		}
		for ( tile_voffs = 0; tile_voffs < (TOAPLAN2_BG_VRAM_SIZE/2); tile_voffs += 2 )
		{
			tchar[0] = bgvideoram16[0][tile_voffs + 1];
			tattr[0] = bgvideoram16[0][tile_voffs];
			if (vid_controllers == 2)
			{
				tchar[1] = bgvideoram16[1][tile_voffs + 1];
				tattr[1] = bgvideoram16[1][tile_voffs];
				loginfo(2,"BGoffs:%04x   Tile0:%04x  Attr0:%04x    Tile1:%04x  Attr1:%04x\n", tile_voffs, tchar[0], tattr[0], tchar[1], tattr[1]);
			}
			else
			{
				loginfo(2,"BGoffs:%04x   Tile0:%04x  Attr0:%04x\n", tile_voffs, tchar[0], tattr[0]);
			}
		}
	}

	if ( code_pressed_memory(KEYCODE_C) )
		loginfo(2,"Mark here\n");

	if ( code_pressed_memory(KEYCODE_E) )
	{
		displog += 1;
		displog &= 1;
	}
	if (displog)
	{
		loginfo(2,"Scrolls   BG-X  BG-Y   FG-X  FG-Y   TOP-X  TOP-Y   Sprite-X  Sprite-Y\n");
		loginfo(2,"---0-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[0],bg_scrolly[0],fg_scrollx[0],fg_scrolly[0],top_scrollx[0],top_scrolly[0],sprite_scrollx[0], sprite_scrolly[0]);
		if (vid_controllers == 2)
		{
			loginfo(2,"---1-->   %04x  %04x   %04x  %04x    %04x  %04x       %04x    %04x\n", bg_scrollx[1],bg_scrolly[1],fg_scrollx[1],fg_scrolly[1],top_scrollx[1],top_scrolly[1],sprite_scrollx[1], sprite_scrolly[1]);
		}
	}
}
#endif



/***************************************************************************
    Sprite Handlers
***************************************************************************/

static void draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect, int controller, int priority_to_display, int bank_sel )
{
	const gfx_element *gfx = Machine->gfx[ ((controller*2)+1) ];

	int offs;

	UINT16 *source = (UINT16 *)(spriteram16_n[controller]);


	priority_to_display <<= 8;

	
	{
    rectangle lcliprect = *cliprect;
	lcliprect.max_x++;
	lcliprect.max_y++;

	struct drawgfxParams dgp0={
		bitmap, 	// dest
		gfx, 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&lcliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0; offs < (TOAPLAN2_SPRITERAM_SIZE/2); offs += 4)
	{
		int attrib, sprite, color, priority, flipx, flipy, sx, sy;
		int sprite_sizex, sprite_sizey, dim_x, dim_y, sx_base, sy_base;
		int bank, sprite_num;

		attrib = source[offs];
		priority = (attrib & 0x0f00);

		if ((priority == priority_to_display) && (attrib & 0x8000))
		{
			if (!bank_sel)	/* No Sprite select bank switching needed */
			{
				sprite = ((attrib & 3) << 16) | source[offs + 1];	/* 18 bit */
			}
			else		/* Batrider Sprite select bank switching required */
			{
				sprite_num = source[offs + 1] & 0x7fff;
				bank = ((attrib & 3) << 1) | (source[offs + 1] >> 15);
				sprite = (batrider_object_bank[bank] << 15 ) | sprite_num;
			}
			color = (attrib >> 2) & 0x3f;

			/***** find out sprite size *****/
			sprite_sizex = ((source[offs + 2] & 0x0f) + 1) * 8;
			sprite_sizey = ((source[offs + 3] & 0x0f) + 1) * 8;

			/***** find position to display sprite *****/
			sx_base = ((source[offs + 2] >> 7) - (sprite_scrollx[controller]+xoffset[3])) & 0x1ff;
			sy_base = ((source[offs + 3] >> 7) - (sprite_scrolly[controller]+yoffset[3])) & 0x1ff;

			flipx = attrib & TOAPLAN2_SPRITE_FLIPX;
			flipy = attrib & TOAPLAN2_SPRITE_FLIPY;

			if (flipx)
			{
				/***** Wrap sprite position around *****/
				sx_base -= 7;
				if (sx_base >= 0x1c0) sx_base -= 0x200;
			}
			else
			{
				if (sx_base >= 0x180) sx_base -= 0x200;
			}

			if (flipy)
			{
				sy_base -= 7;
				if (sy_base >= 0x1c0) sy_base -= 0x200;
			}
			else
			{
				if (sy_base >= 0x180) sy_base -= 0x200;
			}

			/***** Flip the sprite layer in any active X or Y flip *****/
			if (sprite_flip[controller])
			{
				if (sprite_flip[controller] & TOAPLAN2_SPRITE_FLIPX)
					sx_base = 320 - sx_base;
				if (sprite_flip[controller] & TOAPLAN2_SPRITE_FLIPY)
					sy_base = 240 - sy_base;
			}

			/***** Cancel flip, if it, and sprite layer flip are active *****/
			flipx = (flipx ^ (sprite_flip[controller] & TOAPLAN2_SPRITE_FLIPX));
			flipy = (flipy ^ (sprite_flip[controller] & TOAPLAN2_SPRITE_FLIPY));

			/***** Draw the complete sprites using the dimension info *****/
			for (dim_y = 0; dim_y < sprite_sizey; dim_y += 8)
			{
				if (flipy) sy = sy_base - dim_y;
				else       sy = sy_base + dim_y;
				for (dim_x = 0; dim_x < sprite_sizex; dim_x += 8)
				{
					if (flipx) sx = sx_base - dim_x;
					else       sx = sx_base + dim_x;

					
					dgp0.code = sprite;
					dgp0.color = color;
					dgp0.flipx = flipx;
					dgp0.flipy = flipy;
					dgp0.sx = sx;
					dgp0.sy = sy;
			//old		drawgfx(&dgp0);
                    drawgfx_clut16_Src8(&dgp0);

					sprite++ ;
				}
			}
		}
	}
	} // end of patch paragraph

}


/***************************************************************************
    Mark the sprite priority used list.
***************************************************************************/
static void mark_sprite_priority(int controller)
{
	int priority, offs;

	UINT16 *source = (UINT16 *)(spriteram16_n[controller]);


	for (priority = 0; priority < 16; priority++)
		sprite_priority[controller][priority] = 0;		/* Clear priorities used list */

	for (offs = 0; offs < (TOAPLAN2_SPRITERAM_SIZE/2); offs += 4)
	{
		priority = (source[offs] & 0x0f00) >> 8;
		sprite_priority[controller][priority] = display_sp[controller];
	}
}

/***************************************************************************
    Mark the tile priority used list.
***************************************************************************/
static void mark_tile_priority(int controller)
{
	int priority, offs;

	for (priority = 0; priority < 16; priority++)
	{
		top_tile_priority[controller][priority] = 0;
		fg_tile_priority[controller][priority] = 0;
		bg_tile_priority[controller][priority] = 0;
	}

	for (offs = 0; offs < (TOAPLAN2_BG_VRAM_SIZE/2); offs += 2)
	{
		top_tile_priority[controller][(topvideoram16[controller][offs] & 0x0f00) >> 8] = 1;
		fg_tile_priority[controller][(fgvideoram16[controller][offs] & 0x0f00) >> 8] = 1;
		bg_tile_priority[controller][(bgvideoram16[controller][offs] & 0x0f00) >> 8] = 1;
	}

#if 0
	{
		static int bg[2]={0,0},fg[2]={0,0},top[2]={0,0};
		bg[controller]=0;fg[controller]=0;top[controller]=0;
		for (priority = 0; priority < 16; priority++)
		{
			if (bg_tile_priority[controller][priority]) bg[controller]++;
			if (fg_tile_priority[controller][priority]) fg[controller]++;
			if (top_tile_priority[controller][priority]) top[controller]++;
		}
		ui_popup("b=%2d:%2d f=%2d:%2d t=%2d:%2d",bg[0],bg[1],fg[0],fg[1],top[0],top[1]);
	}
#endif
}

/***************************************************************************
    Draw the game screen in the given mame_bitmap.
***************************************************************************/

VIDEO_UPDATE( toaplan2_0 )
{
	int priority;


#ifdef MAME_DEBUG
	toaplan2_log_vram();
#endif

	mark_sprite_priority(0);
	mark_tile_priority(0);

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	for (priority = 0; priority < 16; priority++)
	{
		if (bg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[0],priority,0);
		if (fg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[0],priority,0);
		if (top_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[0],priority,0);
		if (sprite_priority[0][priority])
			draw_sprites(bitmap,cliprect,0,priority,0);
	}
}

VIDEO_UPDATE( dogyuun_1 )
{
	int priority;


#ifdef MAME_DEBUG
	toaplan2_log_vram();
#endif

	mark_sprite_priority(0);
	mark_sprite_priority(1);
	mark_tile_priority(0);
	mark_tile_priority(1);

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	for (priority = 0; priority < 16; priority++)
	{
		if (bg_tile_priority[1][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[1],priority,0);
		if (fg_tile_priority[1][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[1],priority,0);
		if (top_tile_priority[1][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[1],priority,0);
		if (sprite_priority[1][priority])
			draw_sprites(bitmap,cliprect,1,priority,0);
	}
	for (priority = 0; priority < 16; priority++)
	{
		if (bg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[0],priority,0);
		if (fg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[0],priority,0);
		if (top_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[0],priority,0);
		if (sprite_priority[0][priority])
			draw_sprites(bitmap,cliprect,0,priority,0);
	}
}

VIDEO_UPDATE( batsugun_1 )
{
	int priority;


#ifdef MAME_DEBUG
	toaplan2_log_vram();
#endif

	mark_sprite_priority(0);
	mark_sprite_priority(1);
	mark_tile_priority(0);
	mark_tile_priority(1);

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	for (priority = 0; priority < 16; priority++)
	{
		if (bg_tile_priority[1][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[1],priority,0);/* 2 */
		if (bg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[0],priority,0);
		if (fg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[0],priority,0);
		if (fg_tile_priority[1][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[1],priority,0);
		if (top_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[0],priority,0);
		if (sprite_priority[0][priority])
			draw_sprites(bitmap,cliprect,0,priority,0);
	}

	for (priority = 0; priority < 16; priority++)
	{
		if (top_tile_priority[1][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[1],priority,0);
		if (sprite_priority[1][priority])
			draw_sprites(bitmap,cliprect,1,priority,0);
	}

}

VIDEO_UPDATE( truxton2_0 )
{
	video_update_toaplan2_0(screen,bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}

VIDEO_UPDATE( batrider_0 )
{
	int priority;

	int line;
	rectangle clip;

#ifdef MAME_DEBUG
	toaplan2_log_vram();
#endif

	mark_sprite_priority(0);
	mark_tile_priority(0);

	/* If object bank is changed, all tile must be redrawn to blow off glitches. */
	/* This causes serious slow down. Is there better algorithm ?                */
	if (objectbank_dirty)
	{
		tilemap_mark_all_tiles_dirty(bg_tilemap[0]);
		tilemap_mark_all_tiles_dirty(fg_tilemap[0]);
		objectbank_dirty = 0;
	}

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	for (priority = 0; priority < 16; priority++)
	{
		if (bg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[0],priority,0);
		if (fg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[0],priority,0);
		if (top_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[0],priority,0);
		if (sprite_priority[0][priority])
			draw_sprites(bitmap,cliprect,0,priority,1);	/* consider bank select */
	}

	clip.min_x = Machine->visible_area.min_x;
	clip.max_x = Machine->visible_area.max_x;
	clip.min_y = Machine->visible_area.min_y;
	clip.max_y = Machine->visible_area.max_y;

	/* used for 'for use in' and '8ing' screen on bbakraid, raizing on batrider */
	for (line = 0; line < 256;line++)
	{
		clip.min_y = clip.max_y = line;
		tilemap_set_scrolly(tx_tilemap,0,toaplan2_txvideoram16_offs[line&0xff]-line);
		tilemap_draw(bitmap,&clip,tx_tilemap,0,0);
	}
}


VIDEO_UPDATE( mahoudai_0 )
{
	int priority;


#ifdef MAME_DEBUG
	toaplan2_log_vram();
#endif

	mark_sprite_priority(0);
	mark_tile_priority(0);

	fillbitmap(bitmap,Machine->pens[0],cliprect);

	if (bg_tile_priority[0][0]) tilemap_draw(bitmap,cliprect,bg_tilemap[0],0,0);
	if (fg_tile_priority[0][0]) tilemap_draw(bitmap,cliprect,fg_tilemap[0],0,0);
	if (top_tile_priority[0][0]) tilemap_draw(bitmap,cliprect,top_tilemap[0],0,0);
	for (priority = 1; priority < 16; priority++)
	{
		if (bg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,bg_tilemap[0],priority,0);
		if (fg_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,fg_tilemap[0],priority,0);
		if (top_tile_priority[0][priority]) tilemap_draw(bitmap,cliprect,top_tilemap[0],priority,0);
		if (sprite_priority[0][priority-1])
			draw_sprites(bitmap,cliprect,0,priority-1,0);
	}
	if (sprite_priority[0][15])
		draw_sprites(bitmap,cliprect,0,15,0);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}


VIDEO_EOF( toaplan2_0 )
{
	/** Shift sprite RAM buffers  ***  Used to fix sprite lag **/
	memcpy(spriteram16_now[0],spriteram16_new[0],TOAPLAN2_SPRITERAM_SIZE);
}

VIDEO_EOF( toaplan2_1 )
{
	/** Shift sprite RAM buffers  ***  Used to fix sprite lag **/
	memcpy(spriteram16_now[0],spriteram16_new[0],TOAPLAN2_SPRITERAM_SIZE);
	memcpy(spriteram16_now[1],spriteram16_new[1],TOAPLAN2_SPRITERAM_SIZE);
}

VIDEO_EOF( batrider_0 )
{
	cpunum_set_input_line(0, MC68000_IRQ_4, HOLD_LINE);  /* Frame done */
}
