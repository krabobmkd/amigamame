#include "drawCtrl.h"


#include "fileio.h"
extern "C" {
#include <memory.h>
#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
}


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

 printf("open %s\n",filename);

	/* open the file */
	file = mame_fopen(NULL, filename, FILETYPE_SKIN_IMAGE, 0);
	if (!file)
	{
        printf("file fail\n");
		return 0;
	}

 printf("file ok\n");
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
 printf("before color type test\n");

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

 printf("ok, width:%d height:%d nbc:%d\n",png->width,png->height,png->num_palette);

	return 1;
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
printf("drawextra_deleteLever\n");
    if(!p) return;

    closePng(&p->_img_l._png);
    closePng(&p->_img_h._png);
printf("drawextra_deleteLever end\n");
}

struct drawableExtra_steeringWheel *drawextra_createSteeringWheel()
{
    struct drawableExtra_steeringWheel *p =
    (struct drawableExtra_steeringWheel *) auto_malloc(sizeof(struct drawableExtra_steeringWheel));
    if(!p) return NULL; // should have jmp throwed anyway.
    int r = open_and_read_png("swheel.png",&p->_img._png );

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
printf("drawextra_deleteSteeringWheel\n");
    if(!p) return;
    closePng(&p->_img._png);
printf("drawextra_deleteSteeringWheel end\n");
}

void drawextra_setpos(struct drawableExtra *p, int x,int y)
{
    p->_xpos = x;
    p->_ypos = y;
}
// quick index remap on 32 first and 32 last colors...



extern rgb_t *game_palette;
//extern rgb_t *adjusted_palette;
// struct extraBitmap
// for CLUT16 destination bitmaps :
static void refreshRemapCLU16(mame_bitmap *bitmap,struct extraBitmap &bm, int indexStart)
{
    int nbc = 16;
    if(bm._png.num_palette<nbc) nbc = bm._png.num_palette;

    if(bm._png.palette == NULL)
    {
        memset(&bm._colormap[0],0,sizeof(UINT16)*nbc);
        return;
    }
	UINT8 *palette = bm._png.palette+3; // +3 because start 1
    for(int i=1;i<nbc;i++)
    {
        INT16 cr = (*palette++);
        INT16 cg = (*palette++);
        INT16 cb = (*palette++);
  //  printf("c:%");
        cr>>=2;
        cg>>=2;
        cb>>=2;

        UINT16 bestindex=0;
        INT32 besterror = 0x7fffffff;
        for(int j=indexStart;j<indexStart+32+8;j++)
        {
            rgb_t c = game_palette[j];
            INT16 pb = (c>>2) & 0x3f;
            INT16 pg = (c>>(8+2)) & 0x3f;
            INT16 pr = (c>>(16+2)) & 0x3f;
            INT16 er_r = pr-cr;
            INT16 er_g = pg-cg;
            INT16 er_b = pb-cb;
            INT32 terr =( er_r*er_r )+ (er_g*er_g) + (er_b*er_b);
            if(terr == 0) {
                bestindex = j;
                break;
            }
            if(terr<besterror)
            {
                besterror = terr;
                bestindex = j;
            }
        }
        bm._colormap[i] = bestindex;
    }

}

static void drawextra_simpleDrawCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,int x, int y,struct extraBitmap &bm )
{
    if( ! bm._png.image) return;
    if(y<0) y=0;
    int y1 = cliprect->min_y + y;
    int y2 = y1 + bm._png.height;
    if(y2>=cliprect->max_y) y2 = cliprect->max_y-1;

    for(int y=y1 ; y<y2 ; y++)
    {
        UINT16 *pw = ((UINT16 *)bitmap->line[y]) + x;
        UINT8 *pr = bm._png.image + (bm._png.rowbytes *2 * (y-y1) ); // ->line[y];

        for(int xx=0 ; xx<bm._png.width ; xx++)
        {
            UINT8 c = *pr++;
            if(c) *pw = bm._colormap[c];
            pw++;
        }
    }
}

void drawextra_leverCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,struct drawableExtra_lever *p, int value, int remapIndexStart)
{
    if(!p) return;
    struct extraBitmap &bm = (value)?(p->_img_h):(p->_img_l);

	refreshRemapCLU16(bitmap,bm,remapIndexStart);
    drawextra_simpleDrawCLUT16(bitmap,cliprect,p->_geo._xpos,p->_geo._ypos,bm);

}
void drawextra_wheelCLUT16(mame_bitmap *bitmap, const rectangle *cliprect,struct drawableExtra_steeringWheel *p, int value, int remapIndexStart)
{
    if(!p) return;
    if( ! p->_img._png.image) return;
	// rotozoom !
	//bitmap->
    struct extraBitmap &bm = p->_img;

	refreshRemapCLU16(bitmap,bm,remapIndexStart);
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
            if(c) *pw = bm._colormap[c];
            pw++;
            lx1 += xdx;
            ly1 += ydx;
        }
        lx += xdy;
        ly += ydy;
    }

}
