/***************************************************************************

    Sega System 16A hardware

***************************************************************************/

#include "driver.h"
#include "segaic16.h"
#include "includes/system16.h"



/*************************************
 *
 *  Video startup
 *
 *************************************/

VIDEO_START( system16a )
{
	/* compute palette info */
	segaic16_palette_init(0x800);

	/* initialize the tile/text layers */
	if (segaic16_tilemap_init( SEGAIC16_TILEMAP_16A, 0x000, 0, 1))
		return 1;

	/* initialize the sprites */
	if (segaic16_sprites_init(0, SEGAIC16_SPRITES_16A, 0x400, 0))
		return 1;

	return 0;
}



/*************************************
 *
 *  Video update
 *
 *************************************/

VIDEO_UPDATE( system16a )
{
	/* if no drawing is happening, fill with black and get out */
	if (!segaic16_display_enable)
	{
		fillbitmap(bitmap, get_black_pen(), cliprect);
		return;
	}

	/* reset priorities */
	fillbitmap(priority_bitmap, 0, cliprect);

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

	/* draw the sprites */
	segaic16_sprites_draw(0, bitmap, cliprect);
}
