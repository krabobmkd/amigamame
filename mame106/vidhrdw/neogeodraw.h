#ifndef _NEOGEO_DRAW_H_
#define _NEOGEO_DRAW_H_

#ifdef __cplusplus
 extern "C" {
#endif
    #include "mamecore.h"
    #include "mame.h"

    void neogeo_drawTilesSprites( mame_bitmap *bitmap, const rectangle *cliprect);

#ifdef __cplusplus
}
#endif

#endif
