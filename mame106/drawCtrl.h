#ifndef _DRAWCTRL_H_
#define _DRAWCTRL_H_
/*krb Minimix special
  draw special controls on emulated screen sharing colors...
  originaly for outrun controls
*/
#ifdef __cplusplus
extern "C" {
#endif
    #include "mamecore.h"
    #include "mame.h"
    // orriginaly for "artwork":
    #include "png.h"

struct CommonControlsValues {
    int _lever;
   // int _leverCount;
    INT16 analogValues[8];
    UINT16 analogValuesReadCount[8];
};

extern struct CommonControlsValues commonControlsValues;

struct extraBitmap{
    png_info _png;
    UINT16 _colormap[16]; // for clut.
};

struct drawableExtra {
	int _xpos,_ypos;
	int _w,_h;	
};

struct drawableExtra_lever {
	struct drawableExtra _geo;
	struct extraBitmap _img_l,_img_h;
};

struct drawableExtra_steeringWheel {
	struct drawableExtra _geo;
	struct extraBitmap _img;
	// sinus table fixed <<14
	INT16 _sincost[512];
	//silhouette mask for a disk
	UINT8 _sil[32*2];
};

// use auto_malloc, so deleted at game exit.
struct drawableExtra_lever *drawextra_createLever();
void drawextra_deleteLever(struct drawableExtra_lever *);

struct drawableExtra_steeringWheel *drawextra_createSteeringWheel(int isbike);
void drawextra_deleteSteeringWheel(struct drawableExtra_steeringWheel *);

void drawextra_setpos(struct drawableExtra *, int x,int y);
void drawextra_leverCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,struct drawableExtra_lever *, int value, int remapIndexStart);
void drawextra_wheelCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,struct drawableExtra_steeringWheel *, int value, int remapIndexStart);


#ifdef __cplusplus
}
#endif

#endif
