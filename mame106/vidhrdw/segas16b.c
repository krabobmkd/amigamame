/***************************************************************************

    Sega System 16B hardware

***************************************************************************/

#include "driver.h"
#include "segaic16.h"
#include "includes/system16.h"



/*************************************
 *
 *  Video startup
 *
 *************************************/

static int video_start_common(int type)
{
	/* compute palette info */
	segaic16_palette_init(0x800);

	/* initialize the tile/text layers */
	if (segaic16_tilemap_init( type, 0x000, 0, 2))
		return 1;

	/* initialize the sprites */
	if (segaic16_sprites_init(0, SEGAIC16_SPRITES_16B, 0x400, 0))
		return 1;

	return 0;
}


VIDEO_START( system16b )
{
    if(strcmp(Machine->gamedrv->name,"wb3")==0)
    {
        ui_popup("Hello It's your friend Krabob:\nYou are using archive wb3, prefer archive wb32\n which is way faster\n(because it has 8751 CPU less and does the same.).");
    }
    if(strcmp(Machine->gamedrv->name,"goldnaxe")==0)
    {
        ui_popup("Hello It's your friend Krabob:\nYou are using archive goldnaxe, prefer archive goldnax3\n which is way faster\n(because it has 8751 CPU less and does the same.).");
    }
	return video_start_common(SEGAIC16_TILEMAP_16B);
}


VIDEO_START( timscanr )
{
	return video_start_common(SEGAIC16_TILEMAP_16B_ALT);
}



/*************************************
 *
 *  Video update
 *
 *************************************/

int dbg_plane=0;

VIDEO_UPDATE( system16b )
{
	/* if no drawing is happening, fill with black and get out */
	if (!segaic16_display_enable)
	{
		fillbitmap(bitmap, get_black_pen(), cliprect);
		return;
	}
/*
  from segaic16.c :
    System16b:
    4-layer tilemap hardware in two pairs, with selection between each
    members on the pairs on a 8-lines basis.  Slightly better sprites.

    krb:
    flags 0 -> 1 is "tile priority"

*/

	/* reset priorities */    
	fillbitmap(priority_bitmap, 0, cliprect);

    segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 0 | TILEMAP_IGNORE_TRANSPARENCY, 0x01);
    // second background flag1 never seen in: wb3, gnaxe, shinobi, altbeast.

    segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 0, 0x02);
    segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 1, 0x04);

    segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 0, 0x04);

    // second text used in: altbeast. -> why prio 8 ?
//dbg_plane=1;
    segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 1, 0x08);
//dbg_plane=0;
     // original calls
#if 0
	/* draw background opaquely first, not setting any priorities */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 0 | TILEMAP_IGNORE_TRANSPARENCY, 0x00);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 1 | TILEMAP_IGNORE_TRANSPARENCY, 0x00);

	/* draw background again, just to set the priorities on non-transparent pixels */
	segaic16_tilemap_draw( NULL, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 0, 0x01);
	segaic16_tilemap_draw( NULL, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 1, 0x02);

	/* draw foreground */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 0, 0x02);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 1, 0x04);

	/* text layer */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 0, 0x04);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 1, 0x08);
#endif
	/* draw the sprites */
	segaic16_sprites_draw(0, bitmap, cliprect);
}
