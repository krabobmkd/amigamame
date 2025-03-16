
#ifndef __TAITOICN_H__
#define __TAITOICN_H__



#ifdef __cplusplus
extern "C" {
#endif
    #include "tilemap.h"

// krb: replacement for taitoic.c version, using template and optimizations
void TC0100SCN_tilemap_draw_fg(mame_bitmap *bitmap,const rectangle *cliprect, int chip, tilemap* tmap ,int flags, UINT32 priority);

void initTopSpeedRoadColorTable();
void topspeed_custom_draw(mame_bitmap *bitmap,const rectangle *cliprect,int chip,int layer,int flags,
							UINT32 priority,UINT16 *color_ctrl_ram);


#ifdef __cplusplus
}
#endif

#endif	/* __TAITOICN_H__ */
