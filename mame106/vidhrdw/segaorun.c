/***************************************************************************

    Sega Outrun hardware

***************************************************************************/

#include "driver.h"
#include "segaic16.h"
#include "includes/system16.h"

#include "drawCtrl.h"

//krb: draw optionnal control goodies
static struct drawableExtra_steeringWheel *_wheelgoody=NULL;
static struct drawableExtra_lever *_levergoody=NULL;

extern unsigned int GetDisplayGoodiesFlags();
/*************************************
 *
 *  Video startup
 *
 *************************************/

static void extraclean()
{
    if(_levergoody) drawextra_deleteLever(_levergoody);
    _levergoody = NULL;
    if(_wheelgoody) drawextra_deleteSteeringWheel(_wheelgoody);
    _wheelgoody = NULL;
}

void add_exit_callback(void (*callback)(void));

static void initGoodies(int shagon)
{
    //krb
    unsigned int configGoodiesFlags = GetDisplayGoodiesFlags();
    if(configGoodiesFlags & 3)
    {
        if((configGoodiesFlags & 2) && (shagon==0))
        {
            _levergoody = drawextra_createLever();
            if(_levergoody) drawextra_setpos(&_levergoody->_geo,160+44+34,224-39);
        }
        if(configGoodiesFlags & 1)
        {
            _wheelgoody = drawextra_createSteeringWheel(shagon);
            if(_wheelgoody) drawextra_setpos(&_wheelgoody->_geo,160+44,224-44);
        }
        add_exit_callback(extraclean);
    }
}

VIDEO_START( shangon )
{
	/* compute palette info */
	segaic16_palette_init(0x1000);

	/* initialize the tile/text layers */
	if (segaic16_tilemap_init( SEGAIC16_TILEMAP_16B_ALT, 0x000, 0, 2))
		return 1;

	/* initialize the sprites */
	if (segaic16_sprites_init(0, SEGAIC16_SPRITES_16B, 0x400, 0))
		return 1;

	/* initialize the road */
	if (segaic16_road_init(0, SEGAIC16_ROAD_OUTRUN, 0x7f6, 0x7c0, 0x7c0, 0))
		return 1;

    initGoodies(1);
	return 0;
}

VIDEO_START( outrun )
{
	/* compute palette info */
	segaic16_palette_init(0x1000);

	/* initialize the tile/text layers */
	if (segaic16_tilemap_init( SEGAIC16_TILEMAP_16B, 0x000, 0, 2))
		return 1;

	/* initialize the sprites */
	if (segaic16_sprites_init(0, SEGAIC16_SPRITES_OUTRUN, 0x800, 0))
		return 1;

	/* initialize the road */
	if (segaic16_road_init(0, SEGAIC16_ROAD_OUTRUN, 0x400, 0x420, 0x780, 0))
		return 1;

    initGoodies(0);

	return 0;
}



/*************************************
 *
 *  Video update
 *
 *************************************/

VIDEO_UPDATE( shangon )
{
	/* reset priorities */
	fillbitmap(priority_bitmap, 0, cliprect);

	/* draw the low priority road layer */
	segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_BACKGROUND);

	/* draw background */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 0, 0x01);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 1, 0x02);

	/* draw foreground */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 0, 0x02);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 1, 0x04);

	/* draw the high priority road */
	segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_FOREGROUND);

	/* text layer */
	/* note that we inflate the priority of the text layer to prevent sprites */
	/* from drawing over the high scores */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 0, 0x08);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 1, 0x08);

	/* draw the sprites */
	segaic16_sprites_draw(0, bitmap, cliprect);

    int remapIndexStart=32;
    if(_wheelgoody)
        drawextra_wheelCLUT16(bitmap,cliprect,_wheelgoody, commonControlsValues.analogValues[0],remapIndexStart);


}
// segaic16
//extern void segaic16_sprites_outrun_draw2( mame_bitmap *bitmap, const rectangle *cliprect);

VIDEO_UPDATE( outrun )
{
	/* if no drawing is happening, fill with black and get out */
	if (!segaic16_display_enable)
	{
		fillbitmap(bitmap, get_black_pen(), cliprect);
		return;
	}

	/* reset priorities */
	fillbitmap(priority_bitmap, 0, cliprect);

	/* draw the low priority road layer */
	segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_BACKGROUND);

	/* draw background */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 0, 0x01);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_BACKGROUND, 1, 0x02);

	/* draw foreground */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 0, 0x02);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_FOREGROUND, 1, 0x04);

	/* draw the high priority road */
    segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_FOREGROUND);

	/* text layer */
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 0, 0x04);
	segaic16_tilemap_draw( bitmap, cliprect, SEGAIC16_TILEMAP_TEXT, 1, 0x08);

	/* draw the sprites */
	segaic16_sprites_draw(0, bitmap, cliprect);
//    segaic16_sprites_outrun_draw2(bitmap, cliprect); // shorter call

	//krb: draw optionnal control goodies
	// test a static hud pixel on the screen to check if we're into gameplay:
	// very accurate because it's not yet color it's sprite private palette index
	// cliprect->min_y can be not the right value.
	UINT16 pixval = ((UINT16*) bitmap->line[215])[cliprect->min_x+298];
	// 23 in demo mode, 356 music selection screen, 41 play mode .
    if(pixval == 41)
    {
        int remapIndexStart=32;
        if(_levergoody)
            drawextra_leverCLUT16(bitmap,cliprect,_levergoody, commonControlsValues._lever,remapIndexStart);
        if(_wheelgoody)
            drawextra_wheelCLUT16(bitmap,cliprect,_wheelgoody, commonControlsValues.analogValues[0],remapIndexStart);
    }



}
