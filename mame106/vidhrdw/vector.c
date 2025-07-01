/******************************************************************************
 *
 * vector.c
 *
 *
 * Copyright 1997,1998 by the M.A.M.E. Project
 *
 *        anti-alias code by Andrew Caldwell
 *        (still more to add)
 *
 * 040227 Fixed miny clip scaling which was breaking in mhavoc. AREK
 * 010903 added support for direct RGB modes MLR
 * 980611 use translucent vectors. Thanks to Peter Hirschberg
 *        and Neil Bradley for the inspiration. BW
 * 980307 added cleverer dirty handling. BW, ASG
 *        fixed antialias table .ac
 * 980221 rewrote anti-alias line draw routine
 *        added inline assembly multiply fuction for 8086 based machines
 *        beam diameter added to draw routine
 *        beam diameter is accurate in anti-alias line draw (Tcosin)
 *        flicker added .ac
 * 980203 moved LBO's routines for drawing into a buffer of vertices
 *        from avgdvg.c to this location. Scaling is now initialized
 *        by calling vector_init(...). BW
 * 980202 moved out of msdos.c ASG
 * 980124 added anti-alias line draw routine
 *        modified avgdvg.c and sega.c to support new line draw routine
 *        added two new tables Tinten and Tmerge (for 256 color support)
 *        added find_color routine to build above tables .ac
 *
 **************************************************************************** */



#include <math.h>
#include "osinline.h"
#include "driver.h"
#include "vector.h"
#include "artwork.h"
#include <stdio.h>

#define MAX_DIRTY_PIXELS (2*MAX_PIXELS)

unsigned char *vectorram;
size_t vectorram_size;

int antialias;                            /* flag for anti-aliasing */
static int beam;                                 /* size of vector beam    */

UINT16 vecbeamconst_high;
UINT16 vecbeamconst_low;

static int flicker;                              /* beam flicker value     */
int translucency;

//static int beam_diameter_is_one;		  /* flag that beam is one pixel wide */

float vector_scale_x;              /* scaling to screen */
float vector_scale_y;              /* scaling to screen */

static float gamma_correction = 1.2f;
static float flicker_correction = 0.0f;
static float intensity_correction = 1.5f;

static int (*vector_aux_renderer)(point *start, int num_points) = NULL;


int alloc_glowtemps_later;

static point *new_list;
static point *old_list;
static int new_index;
static int old_index;

/* coordinates of pixels are stored here for faster removal */
#if USE_DIRTYPIXXELS
	vector_pixel_t *vector_dirty_list;
	static int dirty_index;
	static vector_pixel_t* pixel;
	static int p_index = 0;
#endif



UINT32 *pTcosin;            /* adjust line width */

#define Tcosin(x)   pTcosin[(x)]          /* adjust line width */

#define ANTIALIAS_GUNBIT  6             /* 6 bits per gun in vga (1-8 valid) */
#define ANTIALIAS_GUNNUM  (1<<ANTIALIAS_GUNBIT)

//static UINT8 Tgamma[256];         /* quick gamma anti-alias table  */
//static UINT8 Tgammar[256];        /* same as above, reversed order */

mame_bitmap *vecbitmap;
static int vecwidth, vecheight;
int vector_xmin, vector_ymin, vector_xmax, vector_ymax; /* clipping area */

static int vector_runs;	/* vector runs per refresh */

static void (*vector_draw_to)(point* curpoint);

void vector_register_aux_renderer(int (*aux_renderer)(point *start, int num_points))
{
	vector_aux_renderer = aux_renderer;
}

/*
 * multiply and divide routines for drawing lines
 * can be be replaced by an assembly routine in osinline.h
 */
#ifndef vec_mult
/*
static inline int vec_mult(int parm1, int parm2)
{
	int temp,result;

	temp     = abs(parm1);
	result   = (temp&0x0000ffff) * (parm2&0x0000ffff);
	result >>= 16;
	result  += (temp&0x0000ffff) * (parm2>>16       );
	result  += (temp>>16       ) * (parm2&0x0000ffff);
	result >>= 16;
	result  += (temp>>16       ) * (parm2>>16       );

	if( parm1 < 0 )
		return(-result);
	else
		return( result);
}
*/
//krb

#endif

/* can be be replaced by an assembly routine in osinline.h */
#ifndef vec_div
static inline int vec_div(int parm1, int parm2)
{
	if( (parm2>>12) )
	{
		parm1 = (parm1<<4) / (parm2>>12);
		if( parm1 > 0x00010000 )
			return( 0x00010000 );
		if( parm1 < -0x00010000 )
			return( -0x00010000 );
		return( parm1 );
	}
	return( 0x00010000 );
}
#endif

/* MLR 990316 new gamma handling added */
//void vector_set_gamma(float _gamma)
//{
//	int i, h;
//
//	gamma_correction = _gamma;
//
//	for (i = 0; i < 256; i++)
//	{
//		h = 255.0*pow(i/255.0, 1.0/gamma_correction);
//		if( h > 255) h = 255;
//		Tgamma[i] = Tgammar[255-i] = h;
//	}
//}

//float vector_get_gamma(void)
//{
//	return gamma_correction;
//}

void vector_set_flicker(float _flicker)
{
	flicker_correction = _flicker;
	flicker = (int)(flicker_correction * 2.55);
}

float vector_get_flicker(void)
{
	return flicker_correction;
}

void vector_set_intensity(float _intensity)
{
	intensity_correction = _intensity;
}

float vector_get_intensity(void)
{
	return intensity_correction;
}

/*
 * Initializes vector game video emulation
 */

VIDEO_START( vector )
{
	int i;

	/* Grab the settings for this session */
	antialias = options.antialias;
	translucency = options.translucency;
	vector_set_flicker(options.vector_flicker);
	vector_set_intensity(options.vector_intensity);
	beam = options.beam;
	if (beam < 0x00012000) beam = 0x00012000; // must be > to 0x00010000 now
	// for function 
	vecbeamconst_high = (UINT16)(((UINT32)beam << 4)>>16);
	vecbeamconst_low = (UINT16)((UINT32)beam << 4);

#if USE_DIRTYPIXXELS
	p_index = 0;
#endif
	new_index = 0;
	old_index = 0;
	vector_runs = 0;

	if (Machine->color_depth == 32)
	{
		if (antialias)
		{
			vector_draw_to = vector_draw_to32aa;
		}
		else
		{
			vector_draw_to = vector_draw_to32;
		}		
	}
	else // 15,16
	{
		if (antialias)
		{
			vector_draw_to = vector_draw_to15aa;
		}
		else
		{
			vector_draw_to = vector_draw_to15;
		}
	}

	/* allocate memory for tables */
	pTcosin = auto_malloc ( (2048+1) * sizeof(pTcosin[0]));   /* yes! 2049 is correct */

#if USE_DIRTYPIXXELS
	pixel = auto_malloc(MAX_PIXELS * sizeof(pixel[0]));
	vector_dirty_list = auto_malloc (MAX_DIRTY_PIXELS * sizeof (vector_dirty_list[0]));
#endif
	old_list = auto_malloc (MAX_POINTS * sizeof (old_list[0]));
	new_list = auto_malloc (MAX_POINTS * sizeof (new_list[0]));


	/* build cosine table for fixing line width in antialias */
	for (i=0; i<=2048; i++)
	{
		Tcosin(i) = (int)((double)(1.0/cos(atan((double)(i)/2048.0)))*0x10000000 + 0.5);
	}

	/* build gamma correction table */
	//vector_set_gamma (gamma_correction);

	/* make sure we reset the list */
#if USE_DIRTYPIXXELS
	vector_dirty_list[0] = VECTOR_PIXEL_END;
#endif
    // - -- - -mini buffer for glow
//	UINT32 nbpixglowtemp = Machine->drv->screen_width;
//    if(Machine->drv->screen_height >nbpixglowtemp) nbpixglowtemp = Machine->drv->screen_height;
//    nbpixglowtemp += 4;

	/* krb note : the given visible pixel resolution given by:
	"MDRV_SCREEN_SIZE(400,300)"  in driver, is what is available here in 
	 Machine->drv->screen_width , Machine->drv->screen_height
	 ... but is it actually overriden at init by options vector_width / vector_height
	 so basically it's also wrong, and only the bitmap passed during game can be trusted for vectors.
	 Basically all 
	 also window size if forced to be 
	*/
	alloc_glowtemps_later = 1;

	return 0;
}

/*
 * Clear the old bitmap. Delete pixel for pixel, this is faster than memset.
 */
#if USE_DIRTYPIXXELS
static void vector_clear_pixels (void)
{
	vector_pixel_t coords;
	int i;

	if (Machine->color_depth == 32)
	{
		for (i=p_index-1; i>=0; i--)
		{
			coords = pixel[i];
			((UINT32 *)vecbitmap->line[VECTOR_PIXEL_Y(coords)])[VECTOR_PIXEL_X(coords)] = 0;
		}
	}
	else
	{
		for (i=p_index-1; i>=0; i--)
		{
			coords = pixel[i];
			((UINT16 *)vecbitmap->line[VECTOR_PIXEL_Y(coords)])[VECTOR_PIXEL_X(coords)] = 0;
		}
	}
	p_index=0;
}
#endif
void vector_krb_dim(void)
{
	int i;

    if(options.vector_remanence < 1 || Machine->color_depth != 32)
    {
        if(Machine->color_depth == 32)
        for (UINT32 y = vector_ymin; y < vector_ymax; y++)
        {
            UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
            for (UINT32 x = vector_xmin; x < vector_xmax; x++)
            {
                *prgb++ = 0;
            }
        }
        else if(Machine->color_depth == 15 || Machine->color_depth == 16)
        {
           for (UINT32 y = vector_ymin; y < vector_ymax; y++)
            {
                UINT16* prgb = ((UINT16*)vecbitmap->line[y])+vector_xmin;
                for (UINT32 x = vector_xmin; x < vector_xmax; x++)
                {
                    *prgb++ = 0;
                }
            }
        }
    } else if(options.vector_remanence ==1)
    {
        // div 2
        for (UINT32 y = vector_ymin; y < vector_ymax; y++)
        {
            UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
            for (UINT32 x = vector_xmin; x < vector_xmax; x++)
            {
                UINT32 c = *prgb;
                *prgb++ = (c >> 1) & 0x7f7f7f7f;
            }
        }
    }else // options.vector_remanence ==2
    {
        // * 3/4
        for (UINT32 y = vector_ymin; y < vector_ymax; y++)
        {
            UINT32* prgb = ((UINT32*)vecbitmap->line[y])+vector_xmin;
            for (UINT32 x = vector_xmin; x < vector_xmax; x++)
            {
                UINT32 c = *prgb;
                UINT32 c1 = (c >> 1) & 0x7f7f7f7f;
                UINT32 c2 = (c >> 2) & 0x3f3f3f3f;
                *prgb++ = c1+c2;
            }
        }
    }
#if USE_DIRTYPIXXELS
	p_index = 0;
#endif
}

/*
 * draws an anti-aliased pixel (blends pixel with background)
 */
#define LIMIT5(x) ((x < 0x1f)? x : 0x1f)
#define LIMIT8(x) ((x < 0xff)? x : 0xff)
#if USE_DIRTYPIXXELS
void vector_draw_aa_pixel_15 (int x, int y, rgb_t col, int dirty)
#else
void vector_draw_aa_pixel_15(int x, int y, rgb_t col)
#endif
{
	vector_pixel_t coords;
	UINT32 dst;

	if (x < vector_xmin || x >= vector_xmax)
		return;
	if (y < vector_ymin || y >= vector_ymax)
		return;

	dst = ((UINT16 *)vecbitmap->line[y])[x];
	((UINT16 *)vecbitmap->line[y])[x] = LIMIT5((RGB_BLUE(col) >> 3) + (dst & 0x1f))
		| (LIMIT5((RGB_GREEN(col) >> 3) + ((dst >> 5) & 0x1f)) << 5)
		| (LIMIT5((RGB_RED(col) >> 3) + (dst >> 10)) << 10);



#if USE_DIRTYPIXXELS
	coords = VECTOR_PIXEL(x, y);
	if (p_index < MAX_PIXELS)
		pixel[p_index++] = coords;
	/* Mark this pixel as dirty */
	if (dirty_index<MAX_DIRTY_PIXELS)
		vector_dirty_list[dirty_index++] = coords;
#endif
}
#if USE_DIRTYPIXXELS
static void vector_draw_aa_pixel_32(int x, int y, rgb_t col, int dirty)
#else
static void vector_draw_aa_pixel_32(int x, int y, rgb_t col)
#endif

{
	vector_pixel_t coords;
	UINT32 dst;

	if (x < vector_xmin || x >= vector_xmax)
		return;
	if (y < vector_ymin || y >= vector_ymax)
		return;

	dst = ((UINT32 *)vecbitmap->line[y])[x];
	((UINT32 *)vecbitmap->line[y])[x] = LIMIT8(RGB_BLUE(col) + (dst & 0xff))
		| (LIMIT8(RGB_GREEN(col) + ((dst >> 8) & 0xff)) << 8)
		| (LIMIT8(RGB_RED(col) + (dst >> 16)) << 16);

#if USE_DIRTYPIXXELS
	coords = VECTOR_PIXEL(x, y);
	if (p_index < MAX_PIXELS)
		pixel[p_index++] = coords;
	/* Mark this pixel as dirty */
	if (dirty_index<MAX_DIRTY_PIXELS)
		vector_dirty_list[dirty_index++] = coords;
#endif
}


/*
 * draws a line
 *
 * input:   x2  16.16 fixed point
 *          y2  16.16 fixed point
 *         col  0-255 indexed color (8 bit)
 *   intensity  0-255 intensity
 *       dirty  bool  mark the pixels as dirty while plotting them
 *
 * written by Andrew Caldwell
 */

int vector_logging = 0;

/*
 * Adds a line end point to the vertices list. The vector processor emulation
 * needs to call this.
 */
void vector_add_point (int x, int y, rgb_t color, int intensity)
{
	point *newpoint;

	intensity *= intensity_correction;
	if (intensity > 0xff)
		intensity = 0xff;

	if (flicker && (intensity > 0))
	{
		intensity += (intensity * (0x80-(rand()&0xff)) * flicker)>>16;
		if (intensity < 0)
			intensity = 0;
		if (intensity > 0xff)
			intensity = 0xff;
	}
	newpoint = &new_list[new_index];
	newpoint->x = x;
	newpoint->y = y;
	newpoint->col = color;
	newpoint->intensity = intensity;
	newpoint->callback = 0;
	newpoint->status = VDIRTY; /* mark identical lines as clean later */

	new_index++;
	if (new_index >= MAX_POINTS)
	{
		new_index--;
		loginfo(2,"*** Warning! Vector list overflow!\n");
	}
}

void vector_add_point_callback (int x, int y, rgb_t (*color_callback)(void), int intensity)
{
	point *newpoint;

	intensity *= intensity_correction;
	if (intensity > 0xff)
		intensity = 0xff;

	if (flicker && (intensity > 0))
	{
		intensity += (intensity * (0x80-(rand()&0xff)) * flicker)>>16;
		if (intensity < 0)
			intensity = 0;
		if (intensity > 0xff)
			intensity = 0xff;
	}
	newpoint = &new_list[new_index];
	newpoint->x = x;
	newpoint->y = y;
	newpoint->col = 1;
	newpoint->intensity = intensity;
	newpoint->callback = color_callback; //krb: uniquely used for sparkle_callback
	newpoint->status = VDIRTY; /* mark identical lines as clean later */

	new_index++;
	if (new_index >= MAX_POINTS)
	{
		new_index--;
		loginfo(2,"*** Warning! Vector list overflow!\n");
	}
}

/*
 * Add new clipping info to the list
 */
void vector_add_clip (int x1, int yy1, int x2, int y2)
{
	point *newpoint;

	newpoint = &new_list[new_index];
	newpoint->x = x1;
	newpoint->y = yy1;

	newpoint->arg1 = x2;
	newpoint->arg2 = y2;
	newpoint->status = VCLIP;

	new_index++;
	if (new_index >= MAX_POINTS)
	{
		new_index--;
		loginfo(2,"*** Warning! Vector list overflow!\n");
	}
}


/*
 * Set the clipping area
 */
static inline void vector_set_clip (point* curpoint)
{
	int x1 = curpoint->x;
	int yy1 = curpoint->y;
	int x2 = curpoint->arg1;
	int y2 = curpoint->arg2;

	/* failsafe */
	if ((x1 >= x2) || (yy1 >= y2))
	{
		loginfo(2,"Error in clipping parameters.\n");
		vector_xmin = 0;
		vector_ymin = 0;
		vector_xmax = vecwidth;
		vector_ymax = vecheight;
		return;
	}

	/* scale coordinates to display */
	x1 = (int)(vector_scale_x*x1);
	yy1= (int)(vector_scale_y*yy1);
	x2 = (int)(vector_scale_x*x2);
	y2 = (int)(vector_scale_y*y2);

	vector_xmin = (x1 + 0x8000) >> 16;
	vector_ymin = (yy1 + 0x8000) >> 16;
	vector_xmax = (x2 + 0x8000) >> 16;
	vector_ymax = (y2 + 0x8000) >> 16;

	/* Make it foolproof by trapping rounding errors */
	if (vector_xmin < 0) vector_xmin = 0;
	if (vector_ymin < 0) vector_ymin = 0;
	if (vector_xmax > vecwidth) vector_xmax = vecwidth;
	if (vector_ymax > vecheight) vector_ymax = vecheight;
}


/*
 * The vector CPU creates a new display list. We save the old display list,
 * but only once per refresh.
 */
void vector_clear_list (void)
{
	point *tmp;

	if (vector_runs == 0)
	{
		old_index = new_index;
		tmp = old_list; old_list = new_list; new_list = tmp;
	}

	new_index = 0;
	vector_runs++;
}


/*
 * By comparing with the last drawn list, we can prevent that identical
 * vectors are marked dirty which appeared at the same list index in the
 * previous frame. BW 19980307
 */
#if USE_DIRTYPIXXELS
static void clever_mark_dirty (void)
{
	int i, min_index, last_match = 0;
	point *new, *old;
	point newclip, oldclip;
	int clips_match = 1;

	if (old_index < new_index)
		min_index = old_index;
	else
		min_index = new_index;

	/* Reset the active clips to invalid values */
	memset (&newclip, 0, sizeof (newclip));
	memset (&oldclip, 0, sizeof (oldclip));

	/* Mark vectors which are not the same in both lists as dirty */
	new = new_list;
	old = old_list;

	for (i = min_index; i > 0; i--, old++, new++)
	{
		/* If this is a clip, we need to determine if the clip regions still match */
		if (old->status == VCLIP || new->status == VCLIP)
		{
			if (old->status == VCLIP)
				oldclip = *old;
			if (new->status == VCLIP)
				newclip = *new;
			clips_match = (newclip.x == oldclip.x) && (newclip.y == oldclip.y) && (newclip.arg1 == oldclip.arg1) && (newclip.arg2 == oldclip.arg2);
			if (!clips_match)
				last_match = 0;

			/* fall through to erase the old line if this is not a clip */
			if (old->status == VCLIP)
				continue;
		}

		/* If the clips match and the vectors match, update */
		else if (clips_match && (new->x == old->x) && (new->y == old->y) &&
			(new->col == old->col) && (new->intensity == old->intensity) &&
			(!new->callback && !old->callback))
		{
			if (last_match)
			{
				new->status = VCLEAN;
				continue;
			}
			last_match = 1;
		}

		/* If nothing matches, remember it */
		else
			last_match = 0;

		/* mark the pixels of the old vector dirty */
		if (dirty_index + old->arg2 - old->arg1 < MAX_DIRTY_PIXELS)
		{
			memcpy(&vector_dirty_list[dirty_index], &pixel[old->arg1], (old->arg2 - old->arg1) * sizeof(pixel[0]));
			dirty_index += old->arg2 - old->arg1;
		}
	}

	/* all old vector with index greater new_index are dirty */
	/* old = &old_list[min_index] here! */
	for (i = (old_index-min_index); i > 0; i--, old++)
	{
		/* skip old clips */
		if (old->status == VCLIP)
			continue;

		/* mark the pixels of the old vector dirty */
		if (dirty_index + old->arg2 - old->arg1 < MAX_DIRTY_PIXELS)
		{
			memcpy(&vector_dirty_list[dirty_index], &pixel[old->arg1], (old->arg2 - old->arg1) * sizeof(pixel[0]));
			dirty_index += old->arg2 - old->arg1;
		}
	}
}
#endif

void vector_krb_hglow(void);
void vector_krb_fullglow(void);

VIDEO_UPDATE( vector )
{
	int i;
	point *curpoint;

	int rv = 1;

	/* if there is an auxiliary renderer set, let it run */
	if (vector_aux_renderer)
		rv = vector_aux_renderer(new_list, new_index);

	/* if the aux renderer chooses, it can override the bitmap */

	if (!rv)
	{
		/* This prevents a crash in the artwork system */
#if USE_DIRTYPIXXELS
		vector_dirty_list[0] = VECTOR_PIXEL_END;
#endif
		return;
	}

	/* copy parameters */
	vecbitmap = bitmap;
	vecwidth  = bitmap->width;
	vecheight = bitmap->height;

	/* reset clipping area */
	vector_xmin = 0;
	vector_xmax = vecwidth;
	vector_ymin = 0;
	vector_ymax = vecheight;

	/* setup scaling */
	vector_scale_x = ((float)vecwidth)/(Machine->visible_area.max_x - Machine->visible_area.min_x);
	vector_scale_y = ((float)vecheight)/(Machine->visible_area.max_y - Machine->visible_area.min_y);

	/* next call to vector_clear_list() is allowed to swap the lists */
	vector_runs = 0;

	/* mark pixels which are not idential in newlist and oldlist dirty */
	/* the old pixels which get removed are marked dirty immediately,  */
	/* new pixels are recognized by setting new->dirty */
#if USE_DIRTYPIXXELS
	dirty_index = 0;
	clever_mark_dirty();
#endif
	/* clear ALL pixels in the hidden map */
	//vector_clear_pixels();
	vector_krb_dim();
	/* Draw ALL lines into the hidden map. Mark only those lines with */
	/* new->dirty = 1 as dirty. Remember the pixel start/end indices  */
	curpoint = new_list;

	for (i = 0; i < new_index; i++)
	{
		if (curpoint->status == VCLIP)
		{
			vector_set_clip(curpoint);
		}
		else
		{
#if USE_DIRTYPIXXELS
			curpoint->arg1 = p_index;
#endif
			//printf("i:%d Ti:%d\n",(int)curpoint->intensity,(int) Tgamma[curpoint->intensity]);
			vector_draw_to(curpoint);
			// curpoint->x, curpoint->y, curpoint->col, curpoint->intensity, curpoint->status, curpoint->callback
#if USE_DIRTYPIXXELS
			curpoint->arg2 = p_index;
#endif
		}
		curpoint++;
	} 
#if USE_DIRTYPIXXELS
	vector_dirty_list[dirty_index] = VECTOR_PIXEL_END;
#endif
    if(options.vector_glow==1) vector_krb_hglow();
    else if(options.vector_glow==2) vector_krb_fullglow();
}
