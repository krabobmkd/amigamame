#ifndef _DRAWCTRL_H_
#define _DRAWCTRL_H_
/*krb Minimix special
  draw special controls on emulated screen sharing colors...
  originaly for outrun controls
*/
#ifdef __cplusplus
extern "C" {
#endif

struct drawableExtra {
	int _xpos,_ypos;
	int _w,_h;	
};

struct drawableExtra_lever {
	struct drawableExtra _geo;
};

struct drawableExtra_wheel {
	struct drawableExtra _geo;
};


void drawextra_setpos(struct drawableExtra *, int x,int y);
void drawextra_lever(struct drawableExtra *, int value);
void drawextra_wheel(struct drawableExtra *, int value);


#ifdef __cplusplus
}
#endif

#endif