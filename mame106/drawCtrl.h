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
    int _leverCount;
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
	struct extraBitmap _img1,_img2;
};

struct drawableExtra_steeringWheel {
	struct drawableExtra _geo;
	struct extraBitmap _img;
};

// use auto_malloc, so deleted at game exit.
struct drawableExtra_lever *drawextra_createLever();
struct drawableExtra_steeringWheel *drawextra_createSteeringWheel();
void drawextra_setpos(struct drawableExtra *, int x,int y);
void drawextra_leverCLUT16(mame_bitmap *bitmap,struct drawableExtra *, int value);
void drawextra_wheelCLUT16(mame_bitmap *bitmap,struct drawableExtra *, int value);


#ifdef __cplusplus
}
#endif

#endif
