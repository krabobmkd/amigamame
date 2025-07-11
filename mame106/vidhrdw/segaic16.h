/***************************************************************************

    Sega 16-bit common hardware

***************************************************************************/
#ifndef _SEGAIC16_H_
#define _SEGAIC16_H_
/* globals */
extern UINT8 segaic16_display_enable;
extern UINT16 *segaic16_tileram_0;
extern UINT16 *segaic16_textram_0;
extern UINT16 *segaic16_spriteram_0;
extern UINT16 *segaic16_spriteram_1;
extern UINT16 *segaic16_roadram_0;
extern UINT16 *segaic16_rotateram_0;

/* misc functions */
void segaic16_set_display_enable(int enable);

/* palette handling */
void segaic16_palette_init(int entries);
WRITE16_HANDLER( segaic16_paletteram_w );

/* tilemap systems */
#define SEGAIC16_MAX_TILEMAPS		1

#define SEGAIC16_TILEMAP_HANGON		0
#define SEGAIC16_TILEMAP_16A		1
#define SEGAIC16_TILEMAP_16B		2
#define SEGAIC16_TILEMAP_16B_ALT	3

#define SEGAIC16_TILEMAP_FOREGROUND	0
#define SEGAIC16_TILEMAP_BACKGROUND	1
#define SEGAIC16_TILEMAP_TEXT		2

int segaic16_tilemap_init( int type, int colorbase, int xoffs, int numbanks);
void segaic16_tilemap_reset();
void segaic16_tilemap_draw( mame_bitmap *bitmap, const rectangle *cliprect, int map, int priority, int priority_mark);
void segaic16_tilemap_set_bank( int banknum, int offset);
void segaic16_tilemap_set_flip( int flip);
void segaic16_tilemap_set_rowscroll( int enable);
void segaic16_tilemap_set_colscroll( int enable);

WRITE16_HANDLER( segaic16_tileram_0_w );
WRITE16_HANDLER( segaic16_textram_0_w );
WRITE16_HANDLER( segaic16_paletteram_fast_w );
/* sprite systems */
#define SEGAIC16_MAX_SPRITES		2

#define SEGAIC16_SPRITES_HANGON		0
#define SEGAIC16_SPRITES_16A		1
#define SEGAIC16_SPRITES_16B		2
#define SEGAIC16_SPRITES_SHARRIER	3
#define SEGAIC16_SPRITES_OUTRUN		4
#define SEGAIC16_SPRITES_XBOARD		5
#define SEGAIC16_SPRITES_YBOARD		6
#define SEGAIC16_SPRITES_YBOARD_16B	7

int segaic16_sprites_init(int which, int type, int colorbase, int xoffs);
void segaic16_sprites_draw(int which, mame_bitmap *bitmap, const rectangle *cliprect);
void segaic16_sprites_set_bank(int which, int banknum, int offset);
void segaic16_sprites_set_flip(int which, int flip);
void segaic16_sprites_set_shadow(int which, int shadow);
WRITE16_HANDLER( segaic16_sprites_draw_0_w );
WRITE16_HANDLER( segaic16_sprites_draw_1_w );

/* road systems */
#define SEGAIC16_MAX_ROADS			1

#define SEGAIC16_ROAD_HANGON		0
#define SEGAIC16_ROAD_SHARRIER		1
#define SEGAIC16_ROAD_OUTRUN		2
#define SEGAIC16_ROAD_XBOARD		3

#define SEGAIC16_ROAD_BACKGROUND	0
#define SEGAIC16_ROAD_FOREGROUND	1

int segaic16_road_init(int which, int type, int colorbase1, int colorbase2, int colorbase3, int xoffs);
void segaic16_road_draw(int which, mame_bitmap *bitmap, const rectangle *cliprect, int priority);
READ16_HANDLER( segaic16_road_control_0_r );
WRITE16_HANDLER( segaic16_road_control_0_w );

/* rotation systems */
#define SEGAIC16_MAX_ROTATE			1

#define SEGAIC16_ROTATE_YBOARD		0

int segaic16_rotate_init(int which, int type, int colorbase);
void segaic16_rotate_draw(int which, mame_bitmap *bitmap, const rectangle *cliprect, mame_bitmap *srcbitmap);
READ16_HANDLER( segaic16_rotate_control_0_r );


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


struct rotate_info
{
	UINT8			index;							/* index of this structure */
	UINT8			type;							/* type of rotate system (see segaic16.h for details) */
	UINT16			colorbase;						/* base color index */
	INT32			ramsize;						/* size of rotate RAM */
	UINT16 *		rotateram;						/* pointer to rotateram pointer */
	UINT16 *		buffer;							/* buffered data */
};



#endif
