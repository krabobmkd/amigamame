#include "drawCtrl.h"


#include "fileio.h"
extern "C" {
#include "mame.h"
#include "driver.h"
#include <memory.h>
#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
}
extern pen_t black_pen;
extern pen_t black_pen;
extern pen_t white_pen,grey_pen,dblue_pen,blue_pen,yellow_pen,orange_pen,mar1_pen,mar2_pen,red_pen;
struct CommonControlsValues commonControlsValues={0};

static void closePng( png_info *png)
{
    if(!png) return;
    if(png->palette) free(png->palette);
    png->palette = NULL;
    if(png->image) free(png->image);
    png->image = NULL;
    if(png->trans) free(png->trans);
    png->trans = NULL;
}

static int open_and_read_png( const char *filename, png_info *png)
{
	int result;
	mame_file *file;

// printf("open %s\n",filename);

	/* open the file */
	file = mame_fopen(NULL, filename, FILETYPE_SKIN_IMAGE, 0);
	if (!file)
	{
       // printf("file fail\n");
		return 0;
	}

// printf("file ok\n");
	/* read the PNG data */
	result = png_read_file(file, png);
	mame_fclose(file);
	if (!result)
		return 0;

	/* verify we can handle this PNG */
	if (png->bit_depth > 8)
	{
		loginfo(2,"Unsupported bit depth %d (8 bit max)\n", png->bit_depth);
		closePng(png);
		return 0;
	}
	if (png->interlace_method != 0)
	{
		loginfo(2,"Interlace unsupported\n");
		closePng(png);
		return 0;
	}
// printf("before color type test\n");

	if (png->color_type != 0 && png->color_type != 3 && png->color_type != 2 && png->color_type != 6)
	{
		loginfo(2,"Unsupported color type %d\n", png->color_type);
		closePng(png);
		return 0;
	}

	/* if less than 8 bits, upsample */
	png_expand_buffer_8bit(png);

	if(png->num_palette>16)
	{
		loginfo(2,"support 16 colors maximum for ingame skins\n");
		closePng(png);
		return 0;
	}

// printf("ok, width:%d height:%d nbc:%d\n",png->width,png->height,png->num_palette);

	return 1;
}

struct drawableExtra_Img *drawextra_createLogo(const char *pfilename)
{
    struct drawableExtra_Img *p =
    (struct drawableExtra_Img *) auto_malloc(sizeof(struct drawableExtra_Img));
    if(!p) return NULL; // should have jmp throwed anyway.
    int r = open_and_read_png(pfilename,&p->_img._png );
    return p;
}
void drawextra_deleteImg(struct drawableExtra_Img *p)
{
    if(!p) return;
    closePng(&p->_img._png);
}

struct drawableExtra_lever *drawextra_createLever()
{
    struct drawableExtra_lever *p =
    (struct drawableExtra_lever *) auto_malloc(sizeof(struct drawableExtra_lever));
    if(!p) return NULL; // should have jmp throwed anyway.
    int r = open_and_read_png("leverl.png",&p->_img_l._png );
    r = open_and_read_png("leverh.png",&p->_img_h._png );

    return p;
}
void drawextra_deleteLever(struct drawableExtra_lever *p)
{
//printf("drawextra_deleteLever\n");
    if(!p) return;

    closePng(&p->_img_l._png);
    closePng(&p->_img_h._png);
//printf("drawextra_deleteLever end\n");
}

struct drawableExtra_steeringWheel *drawextra_createSteeringWheel(int imageindex)
{
    struct drawableExtra_steeringWheel *p =
    (struct drawableExtra_steeringWheel *) auto_malloc(sizeof(struct drawableExtra_steeringWheel));
    if(!p) return NULL; // should have jmp throwed anyway.

    if(imageindex>2) imageindex=2;
    if(imageindex<0) imageindex=0;
    const char *imgs[]={
        "swheel.png","handlebars.png","hammer.png"
    };
    const char *imagename = imgs[imageindex];
    int r = open_and_read_png(imagename,&p->_img._png );
    if(p->_img._png.image &&
      (p->_img._png.width != 32 || p->_img._png.height != 32 ))
      {
        closePng(&p->_img._png);
        fatalerror("wheel image must be 32x32 pixels, 16 or 8 colors, png format.");
        return p;
      }

    // create sinus table
    for(int i=0;i<512;i++)
    {
        p->_sincost[i] = (INT16)(16384.0f * sinf((float)i * (3.1415f*2.0f/512.0f)));
    }
    // create silhouette table
    const int ir = 16;
    const int ir2plus = (ir+1)*(ir+1);

    // UINT8 _sil[32*2];
    UINT8 *pSil = p->_sil;
    for(int y=0;y<32;y++)
    {
        int idy = y-ir;
        int ir2mdy2 = ir2plus - (idy*idy);
        if(ir2mdy2<=0)
        {
            *pSil++ = 32; // jumped
            *pSil++ = 0; // drawn
            continue;
        }
        int distcen = (int) sqrtf((float)ir2mdy2);
        int nbpixdrawn = distcen*2;
        if(nbpixdrawn>32) nbpixdrawn = 32;
        int nbjump = 32 - nbpixdrawn;
        *pSil++ = nbjump>>1; // pixel jumps
        *pSil++ =nbpixdrawn; // actual drawn.
    }



    return p;
}
void drawextra_deleteSteeringWheel(struct drawableExtra_steeringWheel *p)
{
//printf("drawextra_deleteSteeringWheel\n");
    if(!p) return;
    closePng(&p->_img._png);
//printf("drawextra_deleteSteeringWheel end\n");
}

void drawextra_setpos(struct drawableExtra *p, int x,int y)
{
    p->_xpos = x;
    p->_ypos = y;
}
// quick index remap on 32 first and 32 last colors...


extern "C" {
    extern rgb_t *game_palette;
}
//extern rgb_t *adjusted_palette;

// - - - - -
class PixCLUT16
{
public:
    PixCLUT16() : _colorstart(black_pen-1) {  }
    UINT16 v(UINT8 c) { return _colorstart+c; }
    int _colorstart;
};
/*
	uirotfont->colortable[1] = black_pen;
	uirotfont->colortable[2] = white_pen;
	uirotfont->colortable[3] = grey_pen;
	uirotfont->colortable[4] = dblue_pen;
	uirotfont->colortable[5] = blue_pen;
	uirotfont->colortable[6] = yellow_pen;
	uirotfont->colortable[7] = orange_pen;
	uirotfont->colortable[8] = mar1_pen;
	uirotfont->colortable[9] = mar2_pen;
	uirotfont->colortable[10] = red_pen;
*/
class PixRGB15
{
public:
    PixRGB15() {
        rgbpal[0] = black_pen;
        rgbpal[1] = black_pen;
        rgbpal[2] = white_pen;
        rgbpal[3] = grey_pen;
        rgbpal[4] = dblue_pen;
        rgbpal[5] = blue_pen;
        rgbpal[6] = yellow_pen;
        rgbpal[7] = orange_pen;
        rgbpal[8] = mar1_pen;
        rgbpal[9] = mar2_pen;
        rgbpal[10] = red_pen;
    }
    UINT16 v(UINT8 c) { return rgbpal[c]; }
    UINT16 rgbpal[16];
};

class PixRGB32
{
public:
    PixRGB32() {
       rgbpal[0] = black_pen;
        rgbpal[1] = black_pen;
        rgbpal[2] = white_pen;
        rgbpal[3] = grey_pen;
        rgbpal[4] = dblue_pen;
        rgbpal[5] = blue_pen;
        rgbpal[6] = yellow_pen;
        rgbpal[7] = orange_pen;
        rgbpal[8] = mar1_pen;
        rgbpal[9] = mar2_pen;
        rgbpal[10] = red_pen;
    }
    UINT32 v(UINT8 c) { return rgbpal[c]; }
    UINT32 rgbpal[16];
};
//  -- - -
template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }

template<class Pix>
class BmDestCoord {
public:

    BmDestCoord(mame_bitmap *bitmap, const rectangle *visiblerect,UINT32 flags)
    : _bitmap(bitmap)
    , _visiblerect(visiblerect)
    ,x_app_x(1),x_app_y(0),x_offs(0/*visiblerect->min_x*/)
    ,y_app_x(0),y_app_y(1),y_offs(0/*visiblerect->min_y*/)
     {
        if(flags & ORIENTATION_SWAP_XY) swapxy();
        if(flags & ORIENTATION_FLIP_X) flipx();
        if(flags & ORIENTATION_FLIP_Y) flipy();
        // then report offset 0/1 to screen size
        if(x_offs==0) x_offs = visiblerect->min_x;
        else x_offs = visiblerect->max_x;
        if(y_offs==0) y_offs = visiblerect->min_y;
        else y_offs = visiblerect->max_y;
//         switch(flags)
//         {
//            // case 0: break;
//             case ORIENTATION_FLIP_X:
//             printf("case fx\n");
//                 x_app_x = -1;
//                 x_app_y = 0;
//                 x_offs = _visiblerect->max_x ;
//             break;
//             case ORIENTATION_FLIP_Y:
//             printf("case fy\n");
//                 y_app_x = 0;
//                 y_app_y = -1;
//                 y_offs = _visiblerect->max_y ;
//             break;
//             case ORIENTATION_FLIP_X|ORIENTATION_FLIP_Y:
//                 x_app_x = -1;
//                 x_app_y = 0;
//                 x_offs = _visiblerect->max_x ;
//                 y_app_x = 0;
//                 y_app_y = -1;
//                 y_offs = _visiblerect->max_y ;
//             break;
// //---- swap
//            case ORIENTATION_SWAP_XY:
//                 break;
//            case ORIENTATION_FLIP_X|ORIENTATION_SWAP_XY:
//                 // x_app_x = -1;
//                 // x_app_y = 0;
//                 // x_offs = (_visiblerect->max_x-_visiblerect->min_x) ;
//                 // y_app_x = 0;
//                 // y_app_y = 1;
//                 // y_offs = _visiblerect->min_y ;
//             break;
//             case ORIENTATION_FLIP_Y|ORIENTATION_SWAP_XY:
//             //OK arkanoid
//                 x_app_x = 0;
//                 x_app_y = 1;
//                 x_offs = _visiblerect->min_x ;
//                 y_app_x = -1;
//                 y_app_y = 0;
//                 y_offs = _visiblerect->max_y;
//             break;
//             case ORIENTATION_FLIP_X|ORIENTATION_FLIP_Y|ORIENTATION_SWAP_XY:

//             break;

//         }

     }
    mame_bitmap *_bitmap;
    const rectangle *_visiblerect;
    int x_app_x,x_app_y,x_offs;
    int y_app_x,y_app_y,y_offs;

    void swapxy() {
        doSwap(x_app_x,y_app_x);
        doSwap(x_app_y,y_app_y);
        doSwap(x_offs,y_offs);
    }
    void flipx() {
        x_offs = 1 ;
        x_app_x = -x_app_x;
        x_app_y = -x_app_y;
    }
    void flipy() {
        y_offs = 1;
        y_app_x = -y_app_x;
        y_app_y = -y_app_y;
    }
    int x(int x,int y) const { return x_offs + x*x_app_x + y*x_app_y;  }
    int y(int x,int y) const { return y_offs + x*y_app_x + y*y_app_y;  }
    int dx() const { return  x_app_x+(y_app_x*_bitmap->rowpixels);  }
    int dy() const { return  y_app_y;  }

    Pix *pix(int xx,int yy) {
                int px = x(xx,yy);
                int py = y(xx,yy);
                return ((Pix *)_bitmap->line[py]) +  px ;
            }
};

template<class BmDst,class Pix,typename T>
void drawextra_simpleDrawT(BmDst &bmdest,int x, int y,struct extraBitmap &bm )
{
    if( ! bm._png.image) return;
//    if(y<0) y=0;
//    int y1 =pix.y1() + y;
//    int y2 = y1 + bm._png.height;
//    if(y2>=pix.y2()) y2 = pix.y2();

    Pix p;
   // int colorstart = black_pen-1; // works because we know it's clut16 mode and colors are index.

    for(int yy=0 ; yy<bm._png.height ; yy++)
    {
//        int yproj = bmdest.pix(x,y+yy);
        T *pw = bmdest.pix(x,y+yy); //((UINT16 *)bitmap->line[ydest]) + (x * pix.xStride());
        UINT8 *pr = bm._png.image + (bm._png.rowbytes *2 * yy ); // ->line[y];

        for(int xx=0 ; xx<bm._png.width ; xx++)
        {
            UINT8 c = *pr++;
            if(c) *pw = p.v(c);   //colorstart + c;
            pw+=bmdest.dx();  // * stride
        }
    }
}

//static void drawextra_simpleDrawCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,int x, int y,struct extraBitmap &bm )
//{
//    drawextra_simpleDrawT<PixCLUT16>(bitmap,cliprect,x,y,bm);
//}

void drawextra_simpleDraw(mame_bitmap *bitmap,int x, int y,struct extraBitmap *bm )
{
  const rectangle *cliprect = &Machine->visible_area;
    UINT32 video_attribs = Machine->drv->video_attributes;
    UINT32 flags = Machine->ui_orientation;
    if(video_attribs & VIDEO_RGB_DIRECT)
    {
        if(video_attribs & VIDEO_NEEDS_6BITS_PER_GUN)
        {
            BmDestCoord<UINT32> bmdc(bitmap,cliprect,flags);
            drawextra_simpleDrawT<BmDestCoord<UINT32>,PixRGB32,UINT32>(bmdc,x,y,*bm);
        } else
        {
            BmDestCoord<UINT16> bmdc(bitmap,cliprect,flags);
            drawextra_simpleDrawT<BmDestCoord<UINT16>,PixRGB15,UINT16>(bmdc,x,y,*bm);
        }
    } else
    {
        BmDestCoord<UINT16> bmdc(bitmap,cliprect,flags);
        drawextra_simpleDrawT<BmDestCoord<UINT16>,PixCLUT16,UINT16>(bmdc,x,y,*bm);
    }
}

void drawextra_leverCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,struct drawableExtra_lever *p, int value)
{
    if(!p) return;
    struct extraBitmap &bm = (value)?(p->_img_h):(p->_img_l);

//	refreshRemapCLU16(bitmap,bm,remapIndexStart,64);
//    drawextra_simpleDrawCLUT16(bitmap,cliprect,p->_geo._xpos,p->_geo._ypos,bm);

    drawextra_simpleDraw(bitmap,p->_geo._xpos,p->_geo._ypos,&bm);
}
void drawextra_wheelCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,struct drawableExtra_steeringWheel *p, int value)
{
    if(!p) return;
    if( ! p->_img._png.image) return;
	// rotozoom !
	//bitmap->
    struct extraBitmap &bm = p->_img;

// works because we know it's clut16 mode and colors are index.
    int colorstart = black_pen-1;
// printf("colorstart:%d\n",colorstart);

//	refreshRemapCLU16(bitmap,bm,remapIndexStart,64);
   // drawextra_simpleDrawCLUT16(bitmap,cliprect,p->_geo._xpos,p->_geo._ypos,bm);
   //drawextra_drawRotate(bitmap,cliprect,p->_geo._xpos,p->_geo._ypos,bm,value,p->_sincost);
    int x = p->_geo._xpos;
    int y = p->_geo._ypos;

    if(y<0) y=0;
    int y1 = cliprect->min_y + y;
    int y2 = y1 + bm._png.height;
    if(y2>=cliprect->max_y) y2 = cliprect->max_y-1;

    int bmcx = bm._png.width>>1;
    int bmcy = bm._png.height>>1;

//127 = center =
    int angle = 0 - (( (value-127)*14)>>4  );

 // static int itest=0;
 // itest++;
 //  angle = itest;

    INT16 sina =  p->_sincost[angle & 511]; // <<14
    INT16 cosa =  p->_sincost[(angle+128) & 511]; // <<14

    int bprsrc = bm._png.rowbytes *2;

    // projected point topleft, rotate inverted , <<16.
    int lx = (bmcx<<16) + (((-bmcx) * cosa - (-bmcy) * sina )<<2); // * -1,  2 times
    int ly = (bmcy<<16) + (((-bmcx) * sina + (-bmcy) * cosa )<<2);

    // y deriv
    int xdy = (( -sina )<<2);
    int ydy = (( cosa )<<2);

    // x deriv
    int xdx = (( cosa )<<2);
    int ydx = (( sina )<<2);

    UINT8 *psil = p->_sil;
    for(int yy=y1 ; yy<y2 ; yy++)
    {
        UINT16 *pw = ((UINT16 *)bitmap->line[yy]) + x;

        int lx1 = lx;
        int ly1 = ly;
        // - -- follow silhouette
        int pixeljumps = *psil++;
        int pixeldrawns = *psil++;
        lx1 += pixeljumps*xdx;
        ly1 += pixeljumps*ydx;
        pw += pixeljumps;

        for(int xx=0 ; xx<pixeldrawns ; xx++)
        {
            UINT8 c = bm._png.image[bprsrc * ((ly1>>16) & 0x01f) + ((lx1>>16) & 0x01f)];
            if(c) *pw = colorstart + c;
            pw++;
            lx1 += xdx;
            ly1 += ydx;
        }
        lx += xdy;
        ly += ydy;
    }

}
