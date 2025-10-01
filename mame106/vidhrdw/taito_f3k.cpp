
extern "C"
{
    #include "driver.h"
    #include "taito_f3.h"

struct tempsprite
{
	int code,color;
	int flipx,flipy;
	int x,y;
	int zoomx,zoomy;
	int pri;
};

    extern struct tempsprite *tf3_spritelist;
    extern const struct tempsprite *tf3_sprite_end;
    extern UINT8 tf3_sprite_pri_usage;

    extern UINT8 *tf3_tile_opaque_sp;
    extern mame_bitmap *tf3_pri_alp_bitmap;
   // void video_update_taito_f3k( mame_bitmap *bitmap, const rectangle *cliprect);
   void f3_init_alpha_blend_func(void);
   void video_update_taito_f3k_drawsprites(mame_bitmap *bitmap, const rectangle *cliprect);


    void tf3_drawscanlines_k(
		mame_bitmap *bitmap,int xsize,INT16 *draw_line_num,
		const struct f3_playfield_line_inf **line_t,
		const int *sprite,
		UINT32 orient,
		int skip_layer_num);
}



#define PSET_T					\
	c = *source;				\
	if(c)						\
	{							\
		p=*pri;					\
		if(!p || p==0xff)		\
		{						\
			*dest = pal[c];		\
			*pri = pri_dst;		\
		}						\
	}

#define PSET_O					\
	p=*pri;						\
	if(!p || p==0xff)			\
	{							\
		*dest = pal[*source];	\
		*pri = pri_dst;			\
	}

#define NEXT_P					\
	source += dx;				\
	dest++;						\
	pri++;

INLINE void f3_drawgfx( mame_bitmap *dest_bmp,const gfx_element *gfx,
		unsigned int code,
		unsigned int color,
		int flipx,int flipy,
		int sx,int sy,
		const rectangle *clip,
		UINT8 pri_dst)
{
	rectangle myclip;

	pri_dst=1<<pri_dst;

	/* KW 991012 -- Added code to force clip to bitmap boundary */
	if(clip)
	{
		myclip.min_x = clip->min_x;
		myclip.max_x = clip->max_x;
		myclip.min_y = clip->min_y;
		myclip.max_y = clip->max_y;

		if (myclip.min_x < 0) myclip.min_x = 0;
		if (myclip.max_x >= dest_bmp->width) myclip.max_x = dest_bmp->width-1;
		if (myclip.min_y < 0) myclip.min_y = 0;
		if (myclip.max_y >= dest_bmp->height) myclip.max_y = dest_bmp->height-1;

		clip=&myclip;
	}

	if(! gfx || !gfx->colortable ) return;

    const pen_t *pal = &gfx->colortable[gfx->color_granularity * (color % gfx->total_colors)]; /* ASG 980209 */
    int source_base = (code % gfx->total_elements) * 16;


    /* compute sprite increment per screen pixel */
    int dx = 1;
    int dy = 1;

    int ex = sx+16;
    int ey = sy+16;

    int x_index_base;
    int y_index;

    if( flipx )
    {
        x_index_base = 15;
        dx = -1;
    }
    else
    {
        x_index_base = 0;
    }

    if( flipy )
    {
        y_index = 15;
        dy = -1;
    }
    else
    {
        y_index = 0;
    }

    if( clip )
    {
        if( sx < clip->min_x)
        { /* clip left */
            int pixels = clip->min_x-sx;
            sx += pixels;
            x_index_base += pixels*dx;
        }
        if( sy < clip->min_y )
        { /* clip top */
            int pixels = clip->min_y-sy;
            sy += pixels;
            y_index += pixels*dy;
        }
        /* NS 980211 - fixed incorrect clipping */
        if( ex > clip->max_x+1 )
        { /* clip right */
            int pixels = ex-clip->max_x-1;
            ex -= pixels;
        }
        if( ey > clip->max_y+1 )
        { /* clip bottom */
            int pixels = ey-clip->max_y-1;
            ey -= pixels;
        }
    }

    if( ex<=sx || ey<=sy) return;

 /* skip if inner loop doesn't draw anything */
//              if (dest_bmp->depth == 32)

    int y=ey-sy;
    int x=(ex-sx-1)|(tf3_tile_opaque_sp[code % gfx->total_elements]<<4);
    UINT8 *source0 = gfx->gfxdata + (source_base+y_index) * 16 + x_index_base;
    UINT32 *dest0 = (UINT32 *)dest_bmp->line[sy]+sx;
    UINT8 *pri0 = (UINT8 *)tf3_pri_alp_bitmap->line[sy]+sx;
    int yadv = dest_bmp->rowpixels;
    dy=dy*16;
    while(1)
    {
        UINT8 *source = source0;
        UINT32 *dest = dest0;
        UINT8 *pri = pri0;

        switch(x)
        {
            int c;
            UINT8 p;
            case 31: PSET_O NEXT_P
            case 30: PSET_O NEXT_P
            case 29: PSET_O NEXT_P
            case 28: PSET_O NEXT_P
            case 27: PSET_O NEXT_P
            case 26: PSET_O NEXT_P
            case 25: PSET_O NEXT_P
            case 24: PSET_O NEXT_P
            case 23: PSET_O NEXT_P
            case 22: PSET_O NEXT_P
            case 21: PSET_O NEXT_P
            case 20: PSET_O NEXT_P
            case 19: PSET_O NEXT_P
            case 18: PSET_O NEXT_P
            case 17: PSET_O NEXT_P
            case 16: PSET_O break;

            case 15: PSET_T NEXT_P
            case 14: PSET_T NEXT_P
            case 13: PSET_T NEXT_P
            case 12: PSET_T NEXT_P
            case 11: PSET_T NEXT_P
            case 10: PSET_T NEXT_P
            case  9: PSET_T NEXT_P
            case  8: PSET_T NEXT_P
            case  7: PSET_T NEXT_P
            case  6: PSET_T NEXT_P
            case  5: PSET_T NEXT_P
            case  4: PSET_T NEXT_P
            case  3: PSET_T NEXT_P
            case  2: PSET_T NEXT_P
            case  1: PSET_T NEXT_P
            case  0: PSET_T
        }

        if(!(--y)) break;
        source0 += dy;
        dest0+=yadv;
        pri0+=yadv;
    } // y loop

}
#undef PSET_T
#undef PSET_O
#undef NEXT_P


void video_update_taito_f3k_drawsprites(mame_bitmap *bitmap, const rectangle *cliprect)
{
	const struct tempsprite *sprite_ptr;
	const gfx_element *sprite_gfx = Machine->gfx[2];

	sprite_ptr = tf3_sprite_end;
	tf3_sprite_pri_usage=0;
	while (sprite_ptr != tf3_spritelist)
	{
		int pri;
		sprite_ptr--;

		pri=sprite_ptr->pri;
		tf3_sprite_pri_usage|=1<<pri;

		if(sprite_ptr->zoomx==16 && sprite_ptr->zoomy==16)
			f3_drawgfx(bitmap,sprite_gfx,
					sprite_ptr->code,
					sprite_ptr->color,
					sprite_ptr->flipx,sprite_ptr->flipy,
					sprite_ptr->x,sprite_ptr->y,
					cliprect,
					pri);
//		else
//			f3_drawgfxzoom(bitmap,sprite_gfx,
//					sprite_ptr->code,
//					sprite_ptr->color,
//					sprite_ptr->flipx,sprite_ptr->flipy,
//					sprite_ptr->x,sprite_ptr->y,
//					cliprect,
//					sprite_ptr->zoomx,sprite_ptr->zoomy,
//					pri);
	}

}

// =============================================================================
// the dreaded plane mixer
extern "C" {
    extern int f3_alpha_level_2as;
    extern int f3_alpha_level_2ad;
    extern int f3_alpha_level_3as;
    extern int f3_alpha_level_3ad;
    extern int f3_alpha_level_2bs;
    extern int f3_alpha_level_2bd;
    extern int f3_alpha_level_3bs;
    extern int f3_alpha_level_3bd;

    extern UINT8 f3_add_sat[256][256];

    extern const UINT8 *f3_alpha_s_1_1;
    extern const UINT8 *f3_alpha_s_1_2;
    extern const UINT8 *f3_alpha_s_1_4;
    extern const UINT8 *f3_alpha_s_1_5;
    extern const UINT8 *f3_alpha_s_1_6;
    extern const UINT8 *f3_alpha_s_1_8;
    extern const UINT8 *f3_alpha_s_1_9;
    extern const UINT8 *f3_alpha_s_1_a;

    extern const UINT8 *f3_alpha_s_2a_0;
    extern const UINT8 *f3_alpha_s_2a_4;
    extern const UINT8 *f3_alpha_s_2a_8;

    extern const UINT8 *f3_alpha_s_2b_0;
    extern const UINT8 *f3_alpha_s_2b_4;
    extern const UINT8 *f3_alpha_s_2b_8;

    extern const UINT8 *f3_alpha_s_3a_0;
    extern const UINT8 *f3_alpha_s_3a_1;
    extern const UINT8 *f3_alpha_s_3a_2;

    extern const UINT8 *f3_alpha_s_3b_0;
    extern const UINT8 *f3_alpha_s_3b_1;
    extern const UINT8 *f3_alpha_s_3b_2;


    extern int (*f3_dpix_n[8][16])(UINT32 s_pix);
    extern int (**f3_dpix_lp[5])(UINT32 s_pix);
    extern int (**f3_dpix_sp[9])(UINT32 s_pix);

    // - - - - -
    extern struct f3_playfield_line_inf *pf_line_inf;
    extern struct f3_spritealpha_line_inf *sa_line_inf;
}
// - - - globals used as locals during tracing...
static UINT32 dval;
static UINT8 pval;
static UINT8 tval;
static UINT8 pdest_2a; //= 0x10;
static UINT8 pdest_2b; //= 0x20;
static int tr_2a;// = 0;
static int tr_2b;// = 1;
static UINT8 pdest_3a;// = 0x40;
static UINT8 pdest_3b;// = 0x80;
static int tr_3a;// = 0;
static int tr_3b;// = 1;

#ifdef LSB_FIRST
#define COLOR1 0
#define COLOR2 1
#define COLOR3 2
#else
#define COLOR1 3
#define COLOR2 2
#define COLOR3 1
#endif


INLINE void f3_alpha_blend32_s( const UINT8 *alphas, UINT32 s )
{
	UINT8 *sc = (UINT8 *)&s;
	UINT8 *dc = (UINT8 *)&dval;
	dc[COLOR1] = alphas[sc[COLOR1]];
	dc[COLOR2] = alphas[sc[COLOR2]];
	dc[COLOR3] = alphas[sc[COLOR3]];
}

INLINE void f3_alpha_blend32_d( const UINT8 *alphas, UINT32 s )
{
	UINT8 *sc = (UINT8 *)&s;
	UINT8 *dc = (UINT8 *)&dval;
	dc[COLOR1] = f3_add_sat[dc[COLOR1]][alphas[sc[COLOR1]]];
	dc[COLOR2] = f3_add_sat[dc[COLOR2]][alphas[sc[COLOR2]]];
	dc[COLOR3] = f3_add_sat[dc[COLOR3]][alphas[sc[COLOR3]]];
}

/*============================================================================*/

INLINE void f3_alpha_blend_1_1( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_1,s);}
INLINE void f3_alpha_blend_1_2( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_2,s);}
INLINE void f3_alpha_blend_1_4( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_4,s);}
INLINE void f3_alpha_blend_1_5( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_5,s);}
INLINE void f3_alpha_blend_1_6( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_6,s);}
INLINE void f3_alpha_blend_1_8( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_8,s);}
INLINE void f3_alpha_blend_1_9( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_9,s);}
INLINE void f3_alpha_blend_1_a( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_1_a,s);}

INLINE void f3_alpha_blend_2a_0( UINT32 s ){f3_alpha_blend32_s(f3_alpha_s_2a_0,s);}
INLINE void f3_alpha_blend_2a_4( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_2a_4,s);}
INLINE void f3_alpha_blend_2a_8( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_2a_8,s);}

INLINE void f3_alpha_blend_2b_0( UINT32 s ){f3_alpha_blend32_s(f3_alpha_s_2b_0,s);}
INLINE void f3_alpha_blend_2b_4( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_2b_4,s);}
INLINE void f3_alpha_blend_2b_8( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_2b_8,s);}

INLINE void f3_alpha_blend_3a_0( UINT32 s ){f3_alpha_blend32_s(f3_alpha_s_3a_0,s);}
INLINE void f3_alpha_blend_3a_1( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_3a_1,s);}
INLINE void f3_alpha_blend_3a_2( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_3a_2,s);}

INLINE void f3_alpha_blend_3b_0( UINT32 s ){f3_alpha_blend32_s(f3_alpha_s_3b_0,s);}
INLINE void f3_alpha_blend_3b_1( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_3b_1,s);}
INLINE void f3_alpha_blend_3b_2( UINT32 s ){f3_alpha_blend32_d(f3_alpha_s_3b_2,s);}


static int dpix_1_noalpha(UINT32 s_pix) {dval = s_pix; return 1;}
static int dpix_ret1(UINT32 s_pix) {return 1;}
static int dpix_ret0(UINT32 s_pix) {return 0;}
static int dpix_1_1(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_1(s_pix); return 1;}
static int dpix_1_2(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_2(s_pix); return 1;}
static int dpix_1_4(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_4(s_pix); return 1;}
static int dpix_1_5(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_5(s_pix); return 1;}
static int dpix_1_6(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_6(s_pix); return 1;}
static int dpix_1_8(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_8(s_pix); return 1;}
static int dpix_1_9(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_9(s_pix); return 1;}
static int dpix_1_a(UINT32 s_pix) {if(s_pix) f3_alpha_blend_1_a(s_pix); return 1;}

static int dpix_2a_0(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_2a_0(s_pix);
	else	  dval = 0;
	if(pdest_2a) {pval |= pdest_2a;return 0;}
	return 1;
}
static int dpix_2a_4(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_2a_4(s_pix);
	if(pdest_2a) {pval |= pdest_2a;return 0;}
	return 1;
}
static int dpix_2a_8(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_2a_8(s_pix);
	if(pdest_2a) {pval |= pdest_2a;return 0;}
	return 1;
}

static int dpix_3a_0(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_3a_0(s_pix);
	else	  dval = 0;
	if(pdest_3a) {pval |= pdest_3a;return 0;}
	return 1;
}
static int dpix_3a_1(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_3a_1(s_pix);
	if(pdest_3a) {pval |= pdest_3a;return 0;}
	return 1;
}
static int dpix_3a_2(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_3a_2(s_pix);
	if(pdest_3a) {pval |= pdest_3a;return 0;}
	return 1;
}

static int dpix_2b_0(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_2b_0(s_pix);
	else	  dval = 0;
	if(pdest_2b) {pval |= pdest_2b;return 0;}
	return 1;
}
static int dpix_2b_4(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_2b_4(s_pix);
	if(pdest_2b) {pval |= pdest_2b;return 0;}
	return 1;
}
static int dpix_2b_8(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_2b_8(s_pix);
	if(pdest_2b) {pval |= pdest_2b;return 0;}
	return 1;
}

static int dpix_3b_0(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_3b_0(s_pix);
	else	  dval = 0;
	if(pdest_3b) {pval |= pdest_3b;return 0;}
	return 1;
}
static int dpix_3b_1(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_3b_1(s_pix);
	if(pdest_3b) {pval |= pdest_3b;return 0;}
	return 1;
}
static int dpix_3b_2(UINT32 s_pix)
{
	if(s_pix) f3_alpha_blend_3b_2(s_pix);
	if(pdest_3b) {pval |= pdest_3b;return 0;}
	return 1;
}

static int dpix_2_0(UINT32 s_pix)
{
	UINT8 tr2=tval&1;
	if(s_pix)
	{
		if(tr2==tr_2b)		{f3_alpha_blend_2b_0(s_pix);if(pdest_2b) pval |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{f3_alpha_blend_2a_0(s_pix);if(pdest_2a) pval |= pdest_2a;else return 1;}
	}
	else
	{
		if(tr2==tr_2b)		{dval = 0;if(pdest_2b) pval |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{dval = 0;if(pdest_2a) pval |= pdest_2a;else return 1;}
	}
	return 0;
}
static int dpix_2_4(UINT32 s_pix)
{
	UINT8 tr2=tval&1;
	if(s_pix)
	{
		if(tr2==tr_2b)		{f3_alpha_blend_2b_4(s_pix);if(pdest_2b) pval |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{f3_alpha_blend_2a_4(s_pix);if(pdest_2a) pval |= pdest_2a;else return 1;}
	}
	else
	{
		if(tr2==tr_2b)		{if(pdest_2b) pval |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{if(pdest_2a) pval |= pdest_2a;else return 1;}
	}
	return 0;
}
static int dpix_2_8(UINT32 s_pix)
{
	UINT8 tr2=tval&1;
	if(s_pix)
	{
		if(tr2==tr_2b)		{f3_alpha_blend_2b_8(s_pix);if(pdest_2b) pval |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{f3_alpha_blend_2a_8(s_pix);if(pdest_2a) pval |= pdest_2a;else return 1;}
	}
	else
	{
		if(tr2==tr_2b)		{if(pdest_2b) pval |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{if(pdest_2a) pval |= pdest_2a;else return 1;}
	}
	return 0;
}

static int dpix_3_0(UINT32 s_pix)
{
	UINT8 tr2=tval&1;
	if(s_pix)
	{
		if(tr2==tr_3b)		{f3_alpha_blend_3b_0(s_pix);if(pdest_3b) pval |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{f3_alpha_blend_3a_0(s_pix);if(pdest_3a) pval |= pdest_3a;else return 1;}
	}
	else
	{
		if(tr2==tr_3b)		{dval = 0;if(pdest_3b) pval |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{dval = 0;if(pdest_3a) pval |= pdest_3a;else return 1;}
	}
	return 0;
}
static int dpix_3_1(UINT32 s_pix)
{
	UINT8 tr2=tval&1;
	if(s_pix)
	{
		if(tr2==tr_3b)		{f3_alpha_blend_3b_1(s_pix);if(pdest_3b) pval |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{f3_alpha_blend_3a_1(s_pix);if(pdest_3a) pval |= pdest_3a;else return 1;}
	}
	else
	{
		if(tr2==tr_3b)		{if(pdest_3b) pval |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{if(pdest_3a) pval |= pdest_3a;else return 1;}
	}
	return 0;
}
static int dpix_3_2(UINT32 s_pix)
{
	UINT8 tr2=tval&1;
	if(s_pix)
	{
		if(tr2==tr_3b)		{f3_alpha_blend_3b_2(s_pix);if(pdest_3b) pval |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{f3_alpha_blend_3a_2(s_pix);if(pdest_3a) pval |= pdest_3a;else return 1;}
	}
	else
	{
		if(tr2==tr_3b)		{if(pdest_3b) pval |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{if(pdest_3a) pval |= pdest_3a;else return 1;}
	}
	return 0;
}


INLINE void dpix_1_sprite(UINT32 s_pix)
{
	if(s_pix)
	{
		UINT8 p1 = pval&0xf0;
		if     (p1==0x10)	f3_alpha_blend_1_1(s_pix);
		else if(p1==0x20)	f3_alpha_blend_1_2(s_pix);
		else if(p1==0x40)	f3_alpha_blend_1_4(s_pix);
		else if(p1==0x50)	f3_alpha_blend_1_5(s_pix);
		else if(p1==0x60)	f3_alpha_blend_1_6(s_pix);
		else if(p1==0x80)	f3_alpha_blend_1_8(s_pix);
		else if(p1==0x90)	f3_alpha_blend_1_9(s_pix);
		else if(p1==0xa0)	f3_alpha_blend_1_a(s_pix);
	}
}

INLINE void dpix_bg(UINT32 bgcolor)
{
	UINT8 p1 = pval&0xf0;
	if(!p1)			dval = bgcolor;
	else if(p1==0x10)	f3_alpha_blend_1_1(bgcolor);
	else if(p1==0x20)	f3_alpha_blend_1_2(bgcolor);
	else if(p1==0x40)	f3_alpha_blend_1_4(bgcolor);
	else if(p1==0x50)	f3_alpha_blend_1_5(bgcolor);
	else if(p1==0x60)	f3_alpha_blend_1_6(bgcolor);
	else if(p1==0x80)	f3_alpha_blend_1_8(bgcolor);
	else if(p1==0x90)	f3_alpha_blend_1_9(bgcolor);
	else if(p1==0xa0)	f3_alpha_blend_1_a(bgcolor);
}

void f3_init_alpha_blend_func(void)
{
	int i,j;

	f3_dpix_n[0][0x0]=dpix_1_noalpha;
	f3_dpix_n[0][0x1]=dpix_1_noalpha;
	f3_dpix_n[0][0x2]=dpix_1_noalpha;
	f3_dpix_n[0][0x3]=dpix_1_noalpha;
	f3_dpix_n[0][0x4]=dpix_1_noalpha;
	f3_dpix_n[0][0x5]=dpix_1_noalpha;
	f3_dpix_n[0][0x6]=dpix_1_noalpha;
	f3_dpix_n[0][0x7]=dpix_1_noalpha;
	f3_dpix_n[0][0x8]=dpix_1_noalpha;
	f3_dpix_n[0][0x9]=dpix_1_noalpha;
	f3_dpix_n[0][0xa]=dpix_1_noalpha;
	f3_dpix_n[0][0xb]=dpix_1_noalpha;
	f3_dpix_n[0][0xc]=dpix_1_noalpha;
	f3_dpix_n[0][0xd]=dpix_1_noalpha;
	f3_dpix_n[0][0xe]=dpix_1_noalpha;
	f3_dpix_n[0][0xf]=dpix_1_noalpha;

	f3_dpix_n[1][0x0]=dpix_1_noalpha;
	f3_dpix_n[1][0x1]=dpix_1_1;
	f3_dpix_n[1][0x2]=dpix_1_2;
	f3_dpix_n[1][0x3]=dpix_ret1;
	f3_dpix_n[1][0x4]=dpix_1_4;
	f3_dpix_n[1][0x5]=dpix_1_5;
	f3_dpix_n[1][0x6]=dpix_1_6;
	f3_dpix_n[1][0x7]=dpix_ret1;
	f3_dpix_n[1][0x8]=dpix_1_8;
	f3_dpix_n[1][0x9]=dpix_1_9;
	f3_dpix_n[1][0xa]=dpix_1_a;
	f3_dpix_n[1][0xb]=dpix_ret1;
	f3_dpix_n[1][0xc]=dpix_ret1;
	f3_dpix_n[1][0xd]=dpix_ret1;
	f3_dpix_n[1][0xe]=dpix_ret1;
	f3_dpix_n[1][0xf]=dpix_ret1;

	f3_dpix_n[2][0x0]=dpix_2a_0;
	f3_dpix_n[2][0x1]=dpix_ret0;
	f3_dpix_n[2][0x2]=dpix_ret0;
	f3_dpix_n[2][0x3]=dpix_ret0;
	f3_dpix_n[2][0x4]=dpix_2a_4;
	f3_dpix_n[2][0x5]=dpix_ret0;
	f3_dpix_n[2][0x6]=dpix_ret0;
	f3_dpix_n[2][0x7]=dpix_ret0;
	f3_dpix_n[2][0x8]=dpix_2a_8;
	f3_dpix_n[2][0x9]=dpix_ret0;
	f3_dpix_n[2][0xa]=dpix_ret0;
	f3_dpix_n[2][0xb]=dpix_ret0;
	f3_dpix_n[2][0xc]=dpix_ret0;
	f3_dpix_n[2][0xd]=dpix_ret0;
	f3_dpix_n[2][0xe]=dpix_ret0;
	f3_dpix_n[2][0xf]=dpix_ret0;

	f3_dpix_n[3][0x0]=dpix_3a_0;
	f3_dpix_n[3][0x1]=dpix_3a_1;
	f3_dpix_n[3][0x2]=dpix_3a_2;
	f3_dpix_n[3][0x3]=dpix_ret0;
	f3_dpix_n[3][0x4]=dpix_ret0;
	f3_dpix_n[3][0x5]=dpix_ret0;
	f3_dpix_n[3][0x6]=dpix_ret0;
	f3_dpix_n[3][0x7]=dpix_ret0;
	f3_dpix_n[3][0x8]=dpix_ret0;
	f3_dpix_n[3][0x9]=dpix_ret0;
	f3_dpix_n[3][0xa]=dpix_ret0;
	f3_dpix_n[3][0xb]=dpix_ret0;
	f3_dpix_n[3][0xc]=dpix_ret0;
	f3_dpix_n[3][0xd]=dpix_ret0;
	f3_dpix_n[3][0xe]=dpix_ret0;
	f3_dpix_n[3][0xf]=dpix_ret0;

	f3_dpix_n[4][0x0]=dpix_2b_0;
	f3_dpix_n[4][0x1]=dpix_ret0;
	f3_dpix_n[4][0x2]=dpix_ret0;
	f3_dpix_n[4][0x3]=dpix_ret0;
	f3_dpix_n[4][0x4]=dpix_2b_4;
	f3_dpix_n[4][0x5]=dpix_ret0;
	f3_dpix_n[4][0x6]=dpix_ret0;
	f3_dpix_n[4][0x7]=dpix_ret0;
	f3_dpix_n[4][0x8]=dpix_2b_8;
	f3_dpix_n[4][0x9]=dpix_ret0;
	f3_dpix_n[4][0xa]=dpix_ret0;
	f3_dpix_n[4][0xb]=dpix_ret0;
	f3_dpix_n[4][0xc]=dpix_ret0;
	f3_dpix_n[4][0xd]=dpix_ret0;
	f3_dpix_n[4][0xe]=dpix_ret0;
	f3_dpix_n[4][0xf]=dpix_ret0;

	f3_dpix_n[5][0x0]=dpix_3b_0;
	f3_dpix_n[5][0x1]=dpix_3b_1;
	f3_dpix_n[5][0x2]=dpix_3b_2;
	f3_dpix_n[5][0x3]=dpix_ret0;
	f3_dpix_n[5][0x4]=dpix_ret0;
	f3_dpix_n[5][0x5]=dpix_ret0;
	f3_dpix_n[5][0x6]=dpix_ret0;
	f3_dpix_n[5][0x7]=dpix_ret0;
	f3_dpix_n[5][0x8]=dpix_ret0;
	f3_dpix_n[5][0x9]=dpix_ret0;
	f3_dpix_n[5][0xa]=dpix_ret0;
	f3_dpix_n[5][0xb]=dpix_ret0;
	f3_dpix_n[5][0xc]=dpix_ret0;
	f3_dpix_n[5][0xd]=dpix_ret0;
	f3_dpix_n[5][0xe]=dpix_ret0;
	f3_dpix_n[5][0xf]=dpix_ret0;

	f3_dpix_n[6][0x0]=dpix_2_0;
	f3_dpix_n[6][0x1]=dpix_ret0;
	f3_dpix_n[6][0x2]=dpix_ret0;
	f3_dpix_n[6][0x3]=dpix_ret0;
	f3_dpix_n[6][0x4]=dpix_2_4;
	f3_dpix_n[6][0x5]=dpix_ret0;
	f3_dpix_n[6][0x6]=dpix_ret0;
	f3_dpix_n[6][0x7]=dpix_ret0;
	f3_dpix_n[6][0x8]=dpix_2_8;
	f3_dpix_n[6][0x9]=dpix_ret0;
	f3_dpix_n[6][0xa]=dpix_ret0;
	f3_dpix_n[6][0xb]=dpix_ret0;
	f3_dpix_n[6][0xc]=dpix_ret0;
	f3_dpix_n[6][0xd]=dpix_ret0;
	f3_dpix_n[6][0xe]=dpix_ret0;
	f3_dpix_n[6][0xf]=dpix_ret0;

	f3_dpix_n[7][0x0]=dpix_3_0;
	f3_dpix_n[7][0x1]=dpix_3_1;
	f3_dpix_n[7][0x2]=dpix_3_2;
	f3_dpix_n[7][0x3]=dpix_ret0;
	f3_dpix_n[7][0x4]=dpix_ret0;
	f3_dpix_n[7][0x5]=dpix_ret0;
	f3_dpix_n[7][0x6]=dpix_ret0;
	f3_dpix_n[7][0x7]=dpix_ret0;
	f3_dpix_n[7][0x8]=dpix_ret0;
	f3_dpix_n[7][0x9]=dpix_ret0;
	f3_dpix_n[7][0xa]=dpix_ret0;
	f3_dpix_n[7][0xb]=dpix_ret0;
	f3_dpix_n[7][0xc]=dpix_ret0;
	f3_dpix_n[7][0xd]=dpix_ret0;
	f3_dpix_n[7][0xe]=dpix_ret0;
	f3_dpix_n[7][0xf]=dpix_ret0;

	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			f3_add_sat[i][j] = (i + j < 256) ? i + j : 255;
}


#define GET_PIXMAP_POINTER(pf_num) \
{ \
	const struct f3_playfield_line_inf *line_tmp=line_t[pf_num]; \
	src##pf_num=line_tmp->src[y]; \
	src_s##pf_num=line_tmp->src_s[y]; \
	src_e##pf_num=line_tmp->src_e[y]; \
	tsrc##pf_num=line_tmp->tsrc[y]; \
	tsrc_s##pf_num=line_tmp->tsrc_s[y]; \
	x_count##pf_num=line_tmp->x_count[y]; \
	x_zoom##pf_num=line_tmp->x_zoom[y]; \
	clip_al##pf_num=line_tmp->clip0[y]&0xffff; \
	clip_ar##pf_num=line_tmp->clip0[y]>>16; \
	clip_bl##pf_num=line_tmp->clip1[y]&0xffff; \
	clip_br##pf_num=line_tmp->clip1[y]>>16; \
}

#define CULC_PIXMAP_POINTER(pf_num) \
{ \
	x_count##pf_num += x_zoom##pf_num; \
	if(x_count##pf_num>>16) \
	{ \
		x_count##pf_num &= 0xffff; \
		src##pf_num++; \
		tsrc##pf_num++; \
		if(src##pf_num==src_e##pf_num) {src##pf_num=src_s##pf_num; tsrc##pf_num=tsrc_s##pf_num;} \
	} \
}

/*============================================================================*/
static int nbf=0;
INLINE int shouldp() { return ((nbf & 127)==1); }

template<int skip_layer_num,bool useXspriteClip,bool useXplaneClip>
void drawscanlinesT(
		mame_bitmap *bitmap,int xsize,INT16 *draw_line_num,
		const struct f3_playfield_line_inf **line_t,
		const int *sprite,
		UINT32 orient)
{
	pen_t *clut = &Machine->remapped_colortable[0];
	UINT32 bgcolor=clut[0];
	int length;

	const int x=46;

	UINT32 sprite_noalp_0=sprite[0]&0x100;
	UINT32 sprite_noalp_1=sprite[1]&0x100;
	UINT32 sprite_noalp_2=sprite[2]&0x100;
	UINT32 sprite_noalp_3=sprite[3]&0x100;
	UINT32 sprite_noalp_4=sprite[4]&0x100;
	UINT32 sprite_noalp_5=sprite[5]&0x100;

// bublbob2:
// noalp 0 0 0 0 1 1
//noalp 0 0 0 1 1 1
//noalp 0 0 0 0 1 1
//noalp 0 1 0 0 0 0
//noalp 0 0 1 0 0 0


// printf("noalp %d %d %d %d %d %d\n",sprite_noalp_0>>8,sprite_noalp_1>>8,sprite_noalp_2>>8,sprite_noalp_3>>8,sprite_noalp_4>>8,sprite_noalp_5>>8);

	static UINT16 *src0=0,*src_s0=0,*src_e0=0,clip_al0=0,clip_ar0=0,clip_bl0=0,clip_br0=0;
	static UINT8 *tsrc0=0,*tsrc_s0=0;
	static UINT32 x_count0=0,x_zoom0=0;

	static UINT16 *src1=0,*src_s1=0,*src_e1=0,clip_al1=0,clip_ar1=0,clip_bl1=0,clip_br1=0;
	static UINT8 *tsrc1=0,*tsrc_s1=0;
	static UINT32 x_count1=0,x_zoom1=0;

	static UINT16 *src2=0,*src_s2=0,*src_e2=0,clip_al2=0,clip_ar2=0,clip_bl2=0,clip_br2=0;
	static UINT8 *tsrc2=0,*tsrc_s2=0;
	static UINT32 x_count2=0,x_zoom2=0;

	static UINT16 *src3=0,*src_s3=0,*src_e3=0,clip_al3=0,clip_ar3=0,clip_bl3=0,clip_br3=0;
	static UINT8 *tsrc3=0,*tsrc_s3=0;
	static UINT32 x_count3=0,x_zoom3=0;

	static UINT16 *src4=0,*src_s4=0,*src_e4=0,clip_al4=0,clip_ar4=0,clip_bl4=0,clip_br4=0;
	static UINT8 *tsrc4=0,*tsrc_s4=0;
	static UINT32 x_count4=0,x_zoom4=0;

	UINT16 clip_als=0, clip_ars=0, clip_bls=0, clip_brs=0;

	UINT8 *dstp0,*dstp;

	int yadv = bitmap->rowpixels;
	int i=0,y=draw_line_num[0];
	int ty = y;

	if (orient & ORIENTATION_FLIP_Y)
	{
		ty = bitmap->height - 1 - ty;
		yadv = -yadv;
	}

	dstp0 = (UINT8 *)tf3_pri_alp_bitmap->line[ty] + x;

	pdest_2a = f3_alpha_level_2ad ? 0x10 : 0;
	pdest_2b = f3_alpha_level_2bd ? 0x20 : 0;
	tr_2a =(f3_alpha_level_2as==0 && f3_alpha_level_2ad==255) ? -1 : 0;
	tr_2b =(f3_alpha_level_2bs==0 && f3_alpha_level_2bd==255) ? -1 : 1;
	pdest_3a = f3_alpha_level_3ad ? 0x40 : 0;
	pdest_3b = f3_alpha_level_3bd ? 0x80 : 0;
	tr_3a =(f3_alpha_level_3as==0 && f3_alpha_level_3ad==255) ? -1 : 0;
	tr_3b =(f3_alpha_level_3bs==0 && f3_alpha_level_3bd==255) ? -1 : 1;

    // - - - - - -  -
    UINT32 *dsti0,*dsti;
    dsti0 = (UINT32 *)bitmap->line[ty] + x;
    while(1)
    {
        int cx=0;
        if(useXspriteClip)
        {
            clip_als=sa_line_inf->sprite_clip0[y]&0xffff;
            clip_ars=sa_line_inf->sprite_clip0[y]>>16;
            clip_bls=sa_line_inf->sprite_clip1[y]&0xffff;
            clip_brs=sa_line_inf->sprite_clip1[y]>>16;
        }

        length=xsize;
        dsti = dsti0;
        dstp = dstp0;

        switch(skip_layer_num)
        {
            case 0: GET_PIXMAP_POINTER(0)
            case 1: GET_PIXMAP_POINTER(1)
            case 2: GET_PIXMAP_POINTER(2)
            case 3: GET_PIXMAP_POINTER(3)
            case 4: GET_PIXMAP_POINTER(4)
        }
        // krb: x loop
        while (1)
        {
            pval=*dstp;
            if (pval!=0xff)
            {
                UINT8 sprite_pri;
                switch(skip_layer_num)
                {
                    case 0: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                                && (sprite_pri=sprite[0]&pval))
                            {
                                if(sprite_noalp_0) break;
                                if(!f3_dpix_sp[sprite_pri]) break;
                                if(f3_dpix_sp[sprite_pri][pval>>4](*dsti)) {*dsti=dval;break;}
                            }
                            if (!useXplaneClip || (cx>=clip_al0 && cx<clip_ar0 && !(cx>=clip_bl0 && cx<clip_br0)))
                            {
                                tval=*tsrc0;
                                if(tval&0xf0)
                                    if(f3_dpix_lp[0][pval>>4](clut[*src0])) {*dsti=dval;break;}
                            }
                    case 1: if( (!useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs) ))
                                && (sprite_pri=sprite[1]&pval))
                            {
                                if(sprite_noalp_1) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(pval&0xf0)) break;
                                    else {dpix_1_sprite(*dsti);*dsti=dval;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][pval>>4](*dsti)) {*dsti=dval;break;}
                            }
                            if (!useXplaneClip ||(cx>=clip_al1 && cx<clip_ar1 && !(cx>=clip_bl1 && cx<clip_br1)))
                            {
                                tval=*tsrc1;if(tval&0xf0) if(f3_dpix_lp[1][pval>>4](clut[*src1])) {*dsti=dval;break;}
                            }
                    case 2: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[2]&pval))
                            {
                                if(sprite_noalp_2) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(pval&0xf0)) break;
                                    else {dpix_1_sprite(*dsti);*dsti=dval;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][pval>>4](*dsti)) {*dsti=dval;break;}
                            }
                            if (!useXplaneClip || (cx>=clip_al2 && cx<clip_ar2 && !(cx>=clip_bl2 && cx<clip_br2)))
                            {
                                tval=*tsrc2;if(tval&0xf0) if(f3_dpix_lp[2][pval>>4](clut[*src2])) {*dsti=dval;break;}
                            }
                    case 3: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[3]&pval))
                            {
                                if(sprite_noalp_3) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(pval&0xf0)) break;
                                    else {dpix_1_sprite(*dsti);*dsti=dval;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][pval>>4](*dsti)) {*dsti=dval;break;}
                            }
                            if (!useXplaneClip ||(cx>=clip_al3 && cx<clip_ar3 && !(cx>=clip_bl3 && cx<clip_br3)))
                            {
                                tval=*tsrc3;if(tval&0xf0) if(f3_dpix_lp[3][pval>>4](clut[*src3])) {*dsti=dval;break;}
                            }

                    case 4: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[4]&pval))
                            {
                                if(sprite_noalp_4) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(pval&0xf0)) break;
                                    else {dpix_1_sprite(*dsti);*dsti=dval;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][pval>>4](*dsti)) {*dsti=dval;break;}
                            }
                            if (!useXplaneClip ||(cx>=clip_al4 && cx<clip_ar4 && !(cx>=clip_bl4 && cx<clip_br4)))
                             {
                                tval=*tsrc4;
                                if(tval&0xf0)
                                {
                                    if(f3_dpix_lp[4][pval>>4](clut[*src4]))
                                    {
                                        *dsti=dval;
                                        break;
                                    }
                                }
                            }

                    case 5: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[5]&pval))
                            {
                                if(sprite_noalp_5) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(pval&0xf0)) break;
                                    else {dpix_1_sprite(*dsti);*dsti=dval;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][pval>>4](*dsti)) {*dsti=dval;break;}
                            }
                            if(!bgcolor) {if(!(pval&0xf0)) {*dsti=0;break;}}
                            else dpix_bg(bgcolor);
                            *dsti=dval;
                }
            }

            if(!(--length)) break;
            dsti++;
            dstp++;
            cx++;

            switch(skip_layer_num)
            {
                case 0: CULC_PIXMAP_POINTER(0)
                case 1: CULC_PIXMAP_POINTER(1)
                case 2: CULC_PIXMAP_POINTER(2)
                case 3: CULC_PIXMAP_POINTER(3)
                case 4: CULC_PIXMAP_POINTER(4)
            }
        } // end x loop

        i++;
 //  if(shouldp()) printf("line:%d %d\n",i,draw_line_num[i]);

        if(draw_line_num[i]<0) break;
        if(draw_line_num[i]==y+1)
        {
            dsti0 += yadv;
            dstp0 += yadv;
            y++;
            continue;
        }
        else
        {
            int dy=(draw_line_num[i]-y)*yadv;
            dsti0 += dy;
            dstp0 += dy;
            y=draw_line_num[i];
        }
    }

}
#undef GET_PIXMAP_POINTER
#undef CULC_PIXMAP_POINTER

void tf3_drawscanlines_k(
		mame_bitmap *bitmap,int xsize,INT16 *draw_line_num,
		const struct f3_playfield_line_inf **line_t,
		const int *sprite,
		UINT32 orient,
		int skip_layer_num)
{
//    skip_layer_num
    const bool useSprClipxTrue = true;
    const bool useSprClipxFalse = false;
    const bool usePlaneClipxTrue = true;
    const bool usePlaneClipxFalse = false;

    switch(skip_layer_num)
    {
        case 0: drawscanlinesT<0,useSprClipxFalse,usePlaneClipxFalse>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 1: drawscanlinesT<1,useSprClipxFalse,usePlaneClipxFalse>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 2: drawscanlinesT<2,useSprClipxFalse,usePlaneClipxFalse>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 3: drawscanlinesT<3,useSprClipxFalse,usePlaneClipxFalse>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 4: drawscanlinesT<4,useSprClipxFalse,usePlaneClipxFalse>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;

        case 0+8: drawscanlinesT<0,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 1+8: drawscanlinesT<1,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 2+8: drawscanlinesT<2,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 3+8: drawscanlinesT<3,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 4+8: drawscanlinesT<4,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
    }

}
