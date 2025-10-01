
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
    extern int f3_flipscreen;
   // void video_update_taito_f3k( mame_bitmap *bitmap, const rectangle *cliprect);
   void f3_init_alpha_blend_func(void);
   void video_update_taito_f3k_drawsprites(mame_bitmap *bitmap, const rectangle *cliprect);


//    void tf3_drawscanlines_k(
//		mame_bitmap *bitmap,int xsize,INT16 *draw_line_num,
//		const struct f3_playfield_line_inf **line_t,
//		const int *sprite,
//		UINT32 orient,
//		int skip_layer_num);
    void f3_scanline_draw_k(mame_bitmap *bitmap, const rectangle *cliprect);
}

#if defined(__GNUC__) && defined(__AMIGA__)
#define REGPX(r) __asm(#r)
#define REGVAL REGPX(a0)
#else
#define REGPX(r)
#endif


typedef UINT32 Pixt;
class cPix {
public:
    Pixt  d;
    UINT8 p,t;
};


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

// globals inited in c versions, legacy:

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



    // - - - - -
    extern struct f3_playfield_line_inf *pf_line_inf;
    extern struct f3_spritealpha_line_inf *sa_line_inf;
}

int (*f3_dpix_n[8][16])(Pixt s_pix, cPix &val );
int (**f3_dpix_lp[5])(Pixt s_pix, cPix &val );
int (**f3_dpix_sp[9])(Pixt s_pix, cPix &val );

// - - - globals used as locals during tracing...


// aim -> make all locals...
// dval is color of pixel "along the plane mixing pipeline"
//static Pixt dval;
// pval is prio value for pixel "along the plane mixing pipeline"
//static UINT8 pval;
// transparency thing...
//static UINT8 tval;

static UINT8 pdest_2a; //= 0x10;
static UINT8 pdest_2b; //= 0x20;
static UINT8 pdest_3a;// = 0x40;
static UINT8 pdest_3b;// = 0x80;

static int tr_2a;// = 0;
static int tr_2b;// = 1;
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


INLINE void f3_alpha_blend32_s( const UINT8 *alphas, UINT32 s, cPix &val )
{
	UINT8 *sc = (UINT8 *)&s;
	UINT8 *dc = (UINT8 *)&val.d;
	dc[COLOR1] = alphas[sc[COLOR1]];
	dc[COLOR2] = alphas[sc[COLOR2]];
	dc[COLOR3] = alphas[sc[COLOR3]];
}

INLINE void f3_alpha_blend32_d( const UINT8 *alphas, UINT32 s, cPix &val )
{
	UINT8 *sc = (UINT8 *)&s;
	UINT8 *dc = (UINT8 *)&val.d;
	dc[COLOR1] = f3_add_sat[dc[COLOR1]][alphas[sc[COLOR1]]];
	dc[COLOR2] = f3_add_sat[dc[COLOR2]][alphas[sc[COLOR2]]];
	dc[COLOR3] = f3_add_sat[dc[COLOR3]][alphas[sc[COLOR3]]];
}

/*============================================================================*/

INLINE void f3_alpha_blend_1_1( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_1,s,val);}
INLINE void f3_alpha_blend_1_2( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_2,s,val);}
INLINE void f3_alpha_blend_1_4( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_4,s,val);}
INLINE void f3_alpha_blend_1_5( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_5,s,val);}
INLINE void f3_alpha_blend_1_6( UINT32 s, cPix &val){f3_alpha_blend32_d(f3_alpha_s_1_6,s,val);}
INLINE void f3_alpha_blend_1_8( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_8,s,val);}
INLINE void f3_alpha_blend_1_9( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_9,s,val);}
INLINE void f3_alpha_blend_1_a( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_1_a,s,val);}

INLINE void f3_alpha_blend_2a_0( UINT32 s, cPix &val ){f3_alpha_blend32_s(f3_alpha_s_2a_0,s,val);}
INLINE void f3_alpha_blend_2a_4( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_2a_4,s,val);}
INLINE void f3_alpha_blend_2a_8( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_2a_8,s,val);}

INLINE void f3_alpha_blend_2b_0( UINT32 s, cPix &val ){f3_alpha_blend32_s(f3_alpha_s_2b_0,s,val);}
INLINE void f3_alpha_blend_2b_4( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_2b_4,s,val);}
INLINE void f3_alpha_blend_2b_8( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_2b_8,s,val);}

INLINE void f3_alpha_blend_3a_0( UINT32 s, cPix &val ){f3_alpha_blend32_s(f3_alpha_s_3a_0,s,val);}
INLINE void f3_alpha_blend_3a_1( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_3a_1,s,val);}
INLINE void f3_alpha_blend_3a_2( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_3a_2,s,val);}

INLINE void f3_alpha_blend_3b_0( UINT32 s, cPix &val ){f3_alpha_blend32_s(f3_alpha_s_3b_0,s,val);}
INLINE void f3_alpha_blend_3b_1( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_3b_1,s,val);}
INLINE void f3_alpha_blend_3b_2( UINT32 s, cPix &val ){f3_alpha_blend32_d(f3_alpha_s_3b_2,s,val);}


static int dpix_1_noalpha(UINT32 s_pix, cPix &val) {val.d = s_pix; return 1;}
static int dpix_ret1(UINT32 s_pix, cPix &val) {return 1;}
static int dpix_ret0(UINT32 s_pix, cPix &val) {return 0;}
static int dpix_1_1(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_1(s_pix,val); return 1;}
static int dpix_1_2(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_2(s_pix,val); return 1;}
static int dpix_1_4(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_4(s_pix,val); return 1;}
static int dpix_1_5(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_5(s_pix,val); return 1;}
static int dpix_1_6(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_6(s_pix,val); return 1;}
static int dpix_1_8(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_8(s_pix,val); return 1;}
static int dpix_1_9(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_9(s_pix,val); return 1;}
static int dpix_1_a(UINT32 s_pix, cPix &val) {if(s_pix) f3_alpha_blend_1_a(s_pix,val); return 1;}

static int dpix_2a_0(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_2a_0(s_pix,val);
	else	  val.d = 0;
	if(pdest_2a) {val.p |= pdest_2a;return 0;}
	return 1;
}
static int dpix_2a_4(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_2a_4(s_pix,val);
	if(pdest_2a) {val.p |= pdest_2a;return 0;}
	return 1;
}
static int dpix_2a_8(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_2a_8(s_pix,val);
	if(pdest_2a) {val.p |= pdest_2a;return 0;}
	return 1;
}

static int dpix_3a_0(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_3a_0(s_pix,val);
	else	  val.d = 0;
	if(pdest_3a) {val.p |= pdest_3a;return 0;}
	return 1;
}
static int dpix_3a_1(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_3a_1(s_pix,val);
	if(pdest_3a) {val.p |= pdest_3a;return 0;}
	return 1;
}
static int dpix_3a_2(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_3a_2(s_pix,val);
	if(pdest_3a) {val.p |= pdest_3a;return 0;}
	return 1;
}

static int dpix_2b_0(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_2b_0(s_pix,val);
	else	  val.d = 0;
	if(pdest_2b) {val.p |= pdest_2b;return 0;}
	return 1;
}
static int dpix_2b_4(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_2b_4(s_pix,val);
	if(pdest_2b) {val.p |= pdest_2b;return 0;}
	return 1;
}
static int dpix_2b_8(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_2b_8(s_pix,val);
	if(pdest_2b) {val.p |= pdest_2b;return 0;}
	return 1;
}

static int dpix_3b_0(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_3b_0(s_pix,val);
	else	  val.d = 0;
	if(pdest_3b) {val.p |= pdest_3b;return 0;}
	return 1;
}
static int dpix_3b_1(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_3b_1(s_pix,val);
	if(pdest_3b) {val.p |= pdest_3b;return 0;}
	return 1;
}
static int dpix_3b_2(UINT32 s_pix, cPix &val)
{
	if(s_pix) f3_alpha_blend_3b_2(s_pix,val);
	if(pdest_3b) {val.p |= pdest_3b;return 0;}
	return 1;
}

static int dpix_2_0(UINT32 s_pix, cPix &val)
{
	UINT8 tr2=val.p&1;
	if(s_pix)
	{
		if(tr2==tr_2b)		{f3_alpha_blend_2b_0(s_pix,val);if(pdest_2b) val.p |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{f3_alpha_blend_2a_0(s_pix,val);if(pdest_2a) val.p |= pdest_2a;else return 1;}
	}
	else
	{
		if(tr2==tr_2b)		{val.d = 0;if(pdest_2b) val.p |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{val.d = 0;if(pdest_2a) val.p |= pdest_2a;else return 1;}
	}
	return 0;
}
static int dpix_2_4(UINT32 s_pix, cPix &val)
{
	UINT8 tr2=val.t&1;
	if(s_pix)
	{
		if(tr2==tr_2b)		{f3_alpha_blend_2b_4(s_pix,val);if(pdest_2b) val.p |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{f3_alpha_blend_2a_4(s_pix,val);if(pdest_2a) val.p |= pdest_2a;else return 1;}
	}
	else
	{
		if(tr2==tr_2b)		{if(pdest_2b) val.p |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{if(pdest_2a) val.p |= pdest_2a;else return 1;}
	}
	return 0;
}
static int dpix_2_8(UINT32 s_pix, cPix &val)
{
	UINT8 tr2=val.t&1;
	if(s_pix)
	{
		if(tr2==tr_2b)		{f3_alpha_blend_2b_8(s_pix,val);if(pdest_2b) val.p |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{f3_alpha_blend_2a_8(s_pix,val);if(pdest_2a) val.p |= pdest_2a;else return 1;}
	}
	else
	{
		if(tr2==tr_2b)		{if(pdest_2b) val.p |= pdest_2b;else return 1;}
		else if(tr2==tr_2a)	{if(pdest_2a) val.p |= pdest_2a;else return 1;}
	}
	return 0;
}

static int dpix_3_0(UINT32 s_pix, cPix &val)
{
	UINT8 tr2=val.t&1;
	if(s_pix)
	{
		if(tr2==tr_3b)		{f3_alpha_blend_3b_0(s_pix,val);if(pdest_3b) val.p |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{f3_alpha_blend_3a_0(s_pix,val);if(pdest_3a) val.p |= pdest_3a;else return 1;}
	}
	else
	{
		if(tr2==tr_3b)		{val.d = 0;if(pdest_3b) val.p |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{val.d = 0;if(pdest_3a) val.p |= pdest_3a;else return 1;}
	}
	return 0;
}
static int dpix_3_1(UINT32 s_pix, cPix &val)
{
	UINT8 tr2=val.t&1;
	if(s_pix)
	{
		if(tr2==tr_3b)		{f3_alpha_blend_3b_1(s_pix,val);if(pdest_3b) val.p |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{f3_alpha_blend_3a_1(s_pix,val);if(pdest_3a) val.p |= pdest_3a;else return 1;}
	}
	else
	{
		if(tr2==tr_3b)		{if(pdest_3b) val.p |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{if(pdest_3a) val.p |= pdest_3a;else return 1;}
	}
	return 0;
}
static int dpix_3_2(UINT32 s_pix, cPix &val)
{
	UINT8 tr2=val.t&1;
	if(s_pix)
	{
		if(tr2==tr_3b)		{f3_alpha_blend_3b_2(s_pix,val);if(pdest_3b) val.p |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{f3_alpha_blend_3a_2(s_pix,val);if(pdest_3a) val.p |= pdest_3a;else return 1;}
	}
	else
	{
		if(tr2==tr_3b)		{if(pdest_3b) val.p |= pdest_3b;else return 1;}
		else if(tr2==tr_3a)	{if(pdest_3a) val.p |= pdest_3a;else return 1;}
	}
	return 0;
}


INLINE void dpix_1_sprite(UINT32 s_pix, cPix &val)
{
	if(s_pix)
	{
		UINT8 p1 = val.p&0xf0;
		if     (p1==0x10)	f3_alpha_blend_1_1(s_pix,val);
		else if(p1==0x20)	f3_alpha_blend_1_2(s_pix,val);
		else if(p1==0x40)	f3_alpha_blend_1_4(s_pix,val);
		else if(p1==0x50)	f3_alpha_blend_1_5(s_pix,val);
		else if(p1==0x60)	f3_alpha_blend_1_6(s_pix,val);
		else if(p1==0x80)	f3_alpha_blend_1_8(s_pix,val);
		else if(p1==0x90)	f3_alpha_blend_1_9(s_pix,val);
		else if(p1==0xa0)	f3_alpha_blend_1_a(s_pix,val);
	}
}

INLINE void dpix_bg(UINT32 bgcolor, cPix &val)
{
	UINT8 p1 = val.p&0xf0;
	if(!p1)			val.d = bgcolor;
	else if(p1==0x10)	f3_alpha_blend_1_1(bgcolor,val);
	else if(p1==0x20)	f3_alpha_blend_1_2(bgcolor,val);
	else if(p1==0x40)	f3_alpha_blend_1_4(bgcolor,val);
	else if(p1==0x50)	f3_alpha_blend_1_5(bgcolor,val);
	else if(p1==0x60)	f3_alpha_blend_1_6(bgcolor,val);
	else if(p1==0x80)	f3_alpha_blend_1_8(bgcolor,val);
	else if(p1==0x90)	f3_alpha_blend_1_9(bgcolor,val);
	else if(p1==0xa0)	f3_alpha_blend_1_a(bgcolor,val);
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

// int nbwr_case1=0;
// int nbwr_case2=0;
// int nbwr_case3=0;
// int nbwr_case4=0;
// int nbwr_case5=0;
// int nbwr_case6=0;

// int nbwr_case7=0;
// int nbwr_case8=0;
// int nbwr_case9=0;
// int nbwr_case10=0;
// int nbwr_case11=0;
// int nbwr_case12=0;

// int nbwr_case13=0;
// int nbwr_case14=0;
// int nbwr_case15=0;
// int nbwr_case16=0;
// int nbwr_case17=0;
// int nbwr_case18=0;

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


// if(shouldp()) printf("noalp %d %d %d %d %d %d\n",sprite_noalp_0>>8,sprite_noalp_1>>8,sprite_noalp_2>>8,sprite_noalp_3>>8,sprite_noalp_4>>8,sprite_noalp_5>>8);

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
        // x loop
        while (1)
        {
            cPix val;
            val.p=*dstp;
            if (val.p!=0xff)
            {
                UINT8 sprite_pri;

//                UINT8 tval;
//                Pixt dval;

                switch(skip_layer_num)
                {
                    case 0: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                                && (sprite_pri=sprite[0]&val.p))
                            {
                                if(sprite_noalp_0) break;
                                if(!f3_dpix_sp[sprite_pri]) break;
                                if(f3_dpix_sp[sprite_pri][val.p>>4](*dsti,val)) {
                               // nbwr_case1++;
                                *dsti=val.d;

                                break;}
                            }
                            if (!useXplaneClip || (cx>=clip_al0 && cx<clip_ar0 && !(cx>=clip_bl0 && cx<clip_br0)))
                            {
                                val.t=*tsrc0;
                                if(val.t&0xf0)
                                    if(f3_dpix_lp[0][val.p>>4](clut[*src0],val)) {
                                        // nbwr_case2++;
                                        *dsti=val.d;break;
                                    }
                            }
                    case 1: if( (!useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs) ))
                                && (sprite_pri=sprite[1]&val.p))
                            {
                                if(sprite_noalp_1) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(val.p&0xf0)) break;
                                    else {dpix_1_sprite(*dsti,val);
                                       //  nbwr_case3++;
                                        *dsti=val.d;break;
                                    }
                                }
                                if(f3_dpix_sp[sprite_pri][val.p>>4](*dsti,val)) {
                                    // nbwr_case4++;
                                    *dsti=val.d;break;
                                    }
                            }
                            if (!useXplaneClip ||(cx>=clip_al1 && cx<clip_ar1 && !(cx>=clip_bl1 && cx<clip_br1)))
                            {
                                val.t=*tsrc1;if(val.t&0xf0) if(f3_dpix_lp[1][val.p>>4](clut[*src1],val))
                                {
                                    // nbwr_case5++;
                                    *dsti=val.d;break;
                                }
                            }
                    case 2: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[2]&val.p))
                            {
                                if(sprite_noalp_2) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(val.p&0xf0)) break;
                                    else {dpix_1_sprite(*dsti,val);
                                       //  nbwr_case6++;
                                        *dsti=val.d;break;
                                    }
                                }
                                if(f3_dpix_sp[sprite_pri][val.p>>4](*dsti,val)) {
                                   //  nbwr_case7++;
                                *dsti=val.d;break;}
                            }
                            if (!useXplaneClip || (cx>=clip_al2 && cx<clip_ar2 && !(cx>=clip_bl2 && cx<clip_br2)))
                            {
                                val.t=*tsrc2;if(val.t&0xf0) if(f3_dpix_lp[2][val.p>>4](clut[*src2],val))
                                {
                                  //   nbwr_case8++;
                                    *dsti=val.d;break;
                                }
                            }
                    case 3: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[3]&val.p))
                            {
                                if(sprite_noalp_3) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(val.p&0xf0)) break;
                                    else {dpix_1_sprite(*dsti,val);
                                   //   nbwr_case9++;
                                    *dsti=val.d;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][val.p>>4](*dsti,val)) {
                              //   nbwr_case10++;
                                *dsti=val.d;break;}
                            }
                            if (!useXplaneClip ||(cx>=clip_al3 && cx<clip_ar3 && !(cx>=clip_bl3 && cx<clip_br3)))
                            {
                                val.t=*tsrc3;
                                if(val.t&0xf0)
                                {
                                    UINT8 pv = val.p>>4;
                                    if(!pv)
                                    {
                                        *dsti=clut[*src3];
                                        break;
                                    }
                                    if(f3_dpix_lp[3][pv](clut[*src3],val))
                                    {
                                      //    nbwr_case11++;
                                        *dsti=val.d;break;
                                    }
                                }
                            }

                    case 4: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[4]&val.p))
                            {
                                if(sprite_noalp_4) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(val.p&0xf0)) break;
                                    else {dpix_1_sprite(*dsti,val);
                                   //  nbwr_case12++;

                                    *dsti=val.d;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][val.p>>4](*dsti,val)) {
                             //    nbwr_case13++;
                                *dsti=val.d;break;}
                            }
                            if (!useXplaneClip ||(cx>=clip_al4 && cx<clip_ar4 && !(cx>=clip_bl4 && cx<clip_br4)))
                             {
                                val.t=*tsrc4;
                                if(val.t&0xf0)
                                {
                                    UINT8 pv=  val.p>>4;
                                    if(!pv) {
                                        *dsti=clut[*src4];
                                        break;
                                    }
                                    if(f3_dpix_lp[4][pv](clut[*src4],val))
                                    {
                                      //   nbwr_case14++;
                                        *dsti=val.d;
                                        break;
                                    }
                                }
                            }

                    case 5: if( ( !useXspriteClip || (cx>=clip_als && cx<clip_ars && !(cx>=clip_bls && cx<clip_brs)))
                            && (sprite_pri=sprite[5]&val.p))
                            {
                                if(sprite_noalp_5) break;
                                if(!f3_dpix_sp[sprite_pri])
                                {
                                    if(!(val.p&0xf0)) break;
                                    else {dpix_1_sprite(*dsti,val);
                            //     nbwr_case15++;
                                    *dsti=val.d;break;}
                                }
                                if(f3_dpix_sp[sprite_pri][val.p>>4](*dsti,val)) {
                             //    nbwr_case16++;
                                *dsti=val.d;break;}
                            }
                            if(!bgcolor) {if(!(val.p&0xf0)) {
                             //    nbwr_case17++;
                            *dsti=0;break;}}
                            else dpix_bg(bgcolor,val);
                             //    nbwr_case18++;
                            *dsti=val.d;
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

extern "C" {
extern int tf3_anyPlaneClipX;
};

static inline void tf3_drawscanlines_k(
		mame_bitmap *bitmap,int xsize,INT16 *draw_line_num,
		const struct f3_playfield_line_inf **line_t,
		const int *sprite,
		UINT32 orient,
		int skip_layer_num)
{
//    skip_layer_num
    // const bool useSprClipxTrue = true;
    // const bool useSprClipxFalse = false;
    // const bool usePlaneClipxTrue = true;
    // const bool usePlaneClipxFalse = false;

    skip_layer_num |= (tf3_anyPlaneClipX<<3);

    switch(skip_layer_num)
    {
        // useClipXsprite, useClipXPlane
        case 0: drawscanlinesT<0,false,false>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 1: drawscanlinesT<1,false,false>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 2: drawscanlinesT<2,false,false>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 3: drawscanlinesT<3,false,false>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 4: drawscanlinesT<4,false,false>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 5: drawscanlinesT<5,false,false>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;

        // case 0: drawscanlinesT<0,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        // case 1: drawscanlinesT<1,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        // case 2: drawscanlinesT<2,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        // case 3: drawscanlinesT<3,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        // case 4: drawscanlinesT<4,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        // case 5: drawscanlinesT<5,useSprClipxTrue,usePlaneClipxTrue>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;

        case 0+8: drawscanlinesT<0,false,true>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 1+8: drawscanlinesT<1,false,true>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 2+8: drawscanlinesT<2,false,true>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 3+8: drawscanlinesT<3,false,true>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 4+8: drawscanlinesT<4,false,true>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;
        case 5+8: drawscanlinesT<5,false,true>(bitmap, xsize,draw_line_num,line_t,sprite,orient); break;


    }

}








#define SET_ALPHA_LEVEL(d,s)			\
{										\
	int level = s;						\
	if(level == 0) level = -1;			\
	d = drawgfx_alpha_cache.alpha[level+1];		\
}

INLINE void f3_alpha_set_level(void)
{
//  SET_ALPHA_LEVEL(f3_alpha_s_1_1, f3_alpha_level_2ad)
	SET_ALPHA_LEVEL(f3_alpha_s_1_1, 255-f3_alpha_level_2as)
//  SET_ALPHA_LEVEL(f3_alpha_s_1_2, f3_alpha_level_2bd)
	SET_ALPHA_LEVEL(f3_alpha_s_1_2, 255-f3_alpha_level_2bs)
	SET_ALPHA_LEVEL(f3_alpha_s_1_4, f3_alpha_level_3ad)
//  SET_ALPHA_LEVEL(f3_alpha_s_1_5, f3_alpha_level_3ad*f3_alpha_level_2ad/255)
	SET_ALPHA_LEVEL(f3_alpha_s_1_5, f3_alpha_level_3ad*(255-f3_alpha_level_2as)/255)
//  SET_ALPHA_LEVEL(f3_alpha_s_1_6, f3_alpha_level_3ad*f3_alpha_level_2bd/255)
	SET_ALPHA_LEVEL(f3_alpha_s_1_6, f3_alpha_level_3ad*(255-f3_alpha_level_2bs)/255)
	SET_ALPHA_LEVEL(f3_alpha_s_1_8, f3_alpha_level_3bd)
//  SET_ALPHA_LEVEL(f3_alpha_s_1_9, f3_alpha_level_3bd*f3_alpha_level_2ad/255)
	SET_ALPHA_LEVEL(f3_alpha_s_1_9, f3_alpha_level_3bd*(255-f3_alpha_level_2as)/255)
//  SET_ALPHA_LEVEL(f3_alpha_s_1_a, f3_alpha_level_3bd*f3_alpha_level_2bd/255)
	SET_ALPHA_LEVEL(f3_alpha_s_1_a, f3_alpha_level_3bd*(255-f3_alpha_level_2bs)/255)

	SET_ALPHA_LEVEL(f3_alpha_s_2a_0, f3_alpha_level_2as)
	SET_ALPHA_LEVEL(f3_alpha_s_2a_4, f3_alpha_level_2as*f3_alpha_level_3ad/255)
	SET_ALPHA_LEVEL(f3_alpha_s_2a_8, f3_alpha_level_2as*f3_alpha_level_3bd/255)

	SET_ALPHA_LEVEL(f3_alpha_s_2b_0, f3_alpha_level_2bs)
	SET_ALPHA_LEVEL(f3_alpha_s_2b_4, f3_alpha_level_2bs*f3_alpha_level_3ad/255)
	SET_ALPHA_LEVEL(f3_alpha_s_2b_8, f3_alpha_level_2bs*f3_alpha_level_3bd/255)

	SET_ALPHA_LEVEL(f3_alpha_s_3a_0, f3_alpha_level_3as)
	SET_ALPHA_LEVEL(f3_alpha_s_3a_1, f3_alpha_level_3as*f3_alpha_level_2ad/255)
	SET_ALPHA_LEVEL(f3_alpha_s_3a_2, f3_alpha_level_3as*f3_alpha_level_2bd/255)

	SET_ALPHA_LEVEL(f3_alpha_s_3b_0, f3_alpha_level_3bs)
	SET_ALPHA_LEVEL(f3_alpha_s_3b_1, f3_alpha_level_3bs*f3_alpha_level_2ad/255)
	SET_ALPHA_LEVEL(f3_alpha_s_3b_2, f3_alpha_level_3bs*f3_alpha_level_2bd/255)
}
#undef SET_ALPHA_LEVEL


void f3_scanline_draw_k(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int i,j,y,ys,ye;
	int y_start,y_end,y_start_next,y_end_next;
	UINT8 draw_line[256];
	INT16 draw_line_num[256];

	UINT32 rot=0;

	if (f3_flipscreen)
	{
		rot=ORIENTATION_FLIP_Y;
		ys=0;
		ye=232;
	}
	else
	{
		ys=24;
		ye=256;
	}

	y_start=ys;
	y_end=ye;
	memset(draw_line,0,256);

nbf++;

 // nbwr_case1=0;
 // nbwr_case2=0;
 // nbwr_case3=0;
 // nbwr_case4=0;
 // nbwr_case5=0;
 // nbwr_case6=0;

 // nbwr_case7=0;
 // nbwr_case8=0;
 // nbwr_case9=0;
 // nbwr_case10=0;
 // nbwr_case11=0;
 // nbwr_case12=0;

 // nbwr_case13=0;
 // nbwr_case14=0;
 // nbwr_case15=0;
 // nbwr_case16=0;
 // nbwr_case17=0;
 // nbwr_case18=0;

// int nbw=0;
	while(1)
	{
  //      if(shouldp()) printf("start main while\n");

		static int alpha_level_last=-1;
		int pos;
		int pri[5],alpha_mode[5],alpha_mode_flag[5],alpha_level;
		UINT16 sprite_alpha;
		UINT8 sprite_alpha_check;
		UINT8 sprite_alpha_all_2a;
		int spri;
		int alpha;
		int layer_tmp[5];

		int count_skip_layer=0;
		int sprite[6]={0,0,0,0,0,0};
		const struct f3_playfield_line_inf *line_t[5];


		/* find same status of scanlines */
		pri[0]=pf_line_inf[0].pri[y_start];
		pri[1]=pf_line_inf[1].pri[y_start];
		pri[2]=pf_line_inf[2].pri[y_start];
		pri[3]=pf_line_inf[3].pri[y_start];
		pri[4]=pf_line_inf[4].pri[y_start];
		alpha_mode[0]=pf_line_inf[0].alpha_mode[y_start];
		alpha_mode[1]=pf_line_inf[1].alpha_mode[y_start];
		alpha_mode[2]=pf_line_inf[2].alpha_mode[y_start];
		alpha_mode[3]=pf_line_inf[3].alpha_mode[y_start];
		alpha_mode[4]=pf_line_inf[4].alpha_mode[y_start];
		alpha_level=sa_line_inf[0].alpha_level[y_start];
		spri=sa_line_inf[0].spri[y_start];
		sprite_alpha=sa_line_inf[0].sprite_alpha[y_start];

		draw_line[y_start]=1;
		draw_line_num[i=0]=y_start;
		y_start_next=-1;
		y_end_next=-1;
		for(y=y_start+1;y<y_end;y++)
		{
			if(!draw_line[y])
			{
				if(pri[0]!=pf_line_inf[0].pri[y]) y_end_next=y+1;
				else if(pri[1]!=pf_line_inf[1].pri[y]) y_end_next=y+1;
				else if(pri[2]!=pf_line_inf[2].pri[y]) y_end_next=y+1;
				else if(pri[3]!=pf_line_inf[3].pri[y]) y_end_next=y+1;
				else if(pri[4]!=pf_line_inf[4].pri[y]) y_end_next=y+1;
				else if(alpha_mode[0]!=pf_line_inf[0].alpha_mode[y]) y_end_next=y+1;
				else if(alpha_mode[1]!=pf_line_inf[1].alpha_mode[y]) y_end_next=y+1;
				else if(alpha_mode[2]!=pf_line_inf[2].alpha_mode[y]) y_end_next=y+1;
				else if(alpha_mode[3]!=pf_line_inf[3].alpha_mode[y]) y_end_next=y+1;
				else if(alpha_mode[4]!=pf_line_inf[4].alpha_mode[y]) y_end_next=y+1;
				else if(alpha_level!=sa_line_inf[0].alpha_level[y]) y_end_next=y+1;
				else if(spri!=sa_line_inf[0].spri[y]) y_end_next=y+1;
				else if(sprite_alpha!=sa_line_inf[0].sprite_alpha[y]) y_end_next=y+1;
				else
				{
					draw_line[y]=1;
					draw_line_num[++i]=y;
					continue;
				}

				if(y_start_next<0) y_start_next=y;
			}
		}

		y_end=y_end_next;
		y_start=y_start_next;
		draw_line_num[++i]=-1;

 //   if(shouldp()) printf("go dsl y_start:%d y_end:%d\n",draw_line_num[0],draw_line_num[i-2]);

		/* alpha blend */
		alpha_mode_flag[0]=alpha_mode[0]&~3;
		alpha_mode_flag[1]=alpha_mode[1]&~3;
		alpha_mode_flag[2]=alpha_mode[2]&~3;
		alpha_mode_flag[3]=alpha_mode[3]&~3;
		alpha_mode_flag[4]=alpha_mode[4]&~3;
		alpha_mode[0]&=3;
		alpha_mode[1]&=3;
		alpha_mode[2]&=3;
		alpha_mode[3]&=3;
		alpha_mode[4]&=3;
		if( alpha_mode[0]>1 ||
			alpha_mode[1]>1 ||
			alpha_mode[2]>1 ||
			alpha_mode[3]>1 ||
			alpha_mode[4]>1 ||
			(sprite_alpha&0xff) != 0xff  )
		{
			/* set alpha level */
			if(alpha_level!=alpha_level_last)
			{
				int al_s,al_d;
				int a=alpha_level;
				int b=(a>>8)&0xf;
				int c=(a>>4)&0xf;
				int d=(a>>0)&0xf;
				a>>=12;

				/* b000 7000 */
				al_s = ( (15-d)*256) / 8;
				al_d = ( (15-b)*256) / 8;
				if(al_s>255) al_s = 255;
				if(al_d>255) al_d = 255;
				f3_alpha_level_3as = al_s;
				f3_alpha_level_3ad = al_d;
				f3_alpha_level_2as = al_d;
				f3_alpha_level_2ad = al_s;

				al_s = ( (15-c)*256) / 8;
				al_d = ( (15-a)*256) / 8;
				if(al_s>255) al_s = 255;
				if(al_d>255) al_d = 255;
				f3_alpha_level_3bs = al_s;
				f3_alpha_level_3bd = al_d;
				f3_alpha_level_2bs = al_d;
				f3_alpha_level_2bd = al_s;

				f3_alpha_set_level();
				alpha_level_last=alpha_level;
			}

			/* set sprite alpha mode */
			sprite_alpha_check=0;
			sprite_alpha_all_2a=1;
			f3_dpix_sp[1]=0;
			f3_dpix_sp[2]=0;
			f3_dpix_sp[4]=0;
			f3_dpix_sp[8]=0;
			for(i=0;i<4;i++)	/* i = sprite priority offset */
			{
				UINT8 sprite_alpha_mode=(sprite_alpha>>(i*2))&3;
				UINT8 sftbit=1<<i;
				if(tf3_sprite_pri_usage&sftbit)
				{
					if(sprite_alpha_mode==1)
					{
						if(f3_alpha_level_2as==0 && f3_alpha_level_2ad==255)
							tf3_sprite_pri_usage&=~sftbit;  // Disable sprite priority block
						else
						{
							f3_dpix_sp[1<<i]=f3_dpix_n[2];
							sprite_alpha_check|=sftbit;
						}
					}
					else if(sprite_alpha_mode==2)
					{
						if(sprite_alpha&0xff00)
						{
							if(f3_alpha_level_3as==0 && f3_alpha_level_3ad==255) tf3_sprite_pri_usage&=~sftbit;
							else
							{
								f3_dpix_sp[1<<i]=f3_dpix_n[3];
								sprite_alpha_check|=sftbit;
								sprite_alpha_all_2a=0;
							}
						}
						else
						{
							if(f3_alpha_level_3bs==0 && f3_alpha_level_3bd==255) tf3_sprite_pri_usage&=~sftbit;
							else
							{
								f3_dpix_sp[1<<i]=f3_dpix_n[5];
								sprite_alpha_check|=sftbit;
								sprite_alpha_all_2a=0;
							}
						}
					}
				}
			}


			/* check alpha level */
			for(i=0;i<5;i++)	/* i = playfield num (pos) */
			{
				int alpha_type = (alpha_mode_flag[i]>>4)&3;

				if(alpha_mode[i]==2)
				{
					if(alpha_type==1)
					{
						if     (f3_alpha_level_2as==0   && f3_alpha_level_2ad==255) alpha_mode[i]=0;
						else if(f3_alpha_level_2as==255 && f3_alpha_level_2ad==0  ) alpha_mode[i]=1;
					}
					else if(alpha_type==2)
					{
						if     (f3_alpha_level_2bs==0   && f3_alpha_level_2bd==255) alpha_mode[i]=0;
						else if(f3_alpha_level_2as==255 && f3_alpha_level_2ad==0 &&
								f3_alpha_level_2bs==255 && f3_alpha_level_2bd==0  ) alpha_mode[i]=1;
					}
					else if(alpha_type==3)
					{
						if     (f3_alpha_level_2as==0   && f3_alpha_level_2ad==255 &&
								f3_alpha_level_2bs==0   && f3_alpha_level_2bd==255) alpha_mode[i]=0;
						else if(f3_alpha_level_2as==255 && f3_alpha_level_2ad==0   &&
								f3_alpha_level_2bs==255 && f3_alpha_level_2bd==0  ) alpha_mode[i]=1;
					}
				}
				else if(alpha_mode[i]==3)
				{
					if(alpha_type==1)
					{
						if     (f3_alpha_level_3as==0   && f3_alpha_level_3ad==255) alpha_mode[i]=0;
						else if(f3_alpha_level_3as==255 && f3_alpha_level_3ad==0  ) alpha_mode[i]=1;
					}
					else if(alpha_type==2)
					{
						if     (f3_alpha_level_3bs==0   && f3_alpha_level_3bd==255) alpha_mode[i]=0;
						else if(f3_alpha_level_3as==255 && f3_alpha_level_3ad==0 &&
								f3_alpha_level_3bs==255 && f3_alpha_level_3bd==0  ) alpha_mode[i]=1;
					}
					else if(alpha_type==3)
					{
						if     (f3_alpha_level_3as==0   && f3_alpha_level_3ad==255 &&
								f3_alpha_level_3bs==0   && f3_alpha_level_3bd==255) alpha_mode[i]=0;
						else if(f3_alpha_level_3as==255 && f3_alpha_level_3ad==0   &&
								f3_alpha_level_3bs==255 && f3_alpha_level_3bd==0  ) alpha_mode[i]=1;
					}
				}
			}

			if (	(alpha_mode[0]==1 || alpha_mode[0]==2 || !alpha_mode[0]) &&
					(alpha_mode[1]==1 || alpha_mode[1]==2 || !alpha_mode[1]) &&
					(alpha_mode[2]==1 || alpha_mode[2]==2 || !alpha_mode[2]) &&
					(alpha_mode[3]==1 || alpha_mode[3]==2 || !alpha_mode[3]) &&
					(alpha_mode[4]==1 || alpha_mode[4]==2 || !alpha_mode[4]) &&
					sprite_alpha_all_2a						)
			{
				int alpha_type = (alpha_mode_flag[0] | alpha_mode_flag[1] | alpha_mode_flag[2] | alpha_mode_flag[3])&0x30;
				if(		(alpha_type==0x10 && f3_alpha_level_2as==255) ||
						(alpha_type==0x20 && f3_alpha_level_2as==255 && f3_alpha_level_2bs==255) ||
						(alpha_type==0x30 && f3_alpha_level_2as==255 && f3_alpha_level_2bs==255)	)
				{
					if(alpha_mode[0]>1) alpha_mode[0]=1;
					if(alpha_mode[1]>1) alpha_mode[1]=1;
					if(alpha_mode[2]>1) alpha_mode[2]=1;
					if(alpha_mode[3]>1) alpha_mode[3]=1;
					if(alpha_mode[4]>1) alpha_mode[4]=1;
					sprite_alpha_check=0;
					f3_dpix_sp[1]=0;
					f3_dpix_sp[2]=0;
					f3_dpix_sp[4]=0;
					f3_dpix_sp[8]=0;
				}
			}
		}
		else
		{
			sprite_alpha_check=0;
			f3_dpix_sp[1]=0;
			f3_dpix_sp[2]=0;
			f3_dpix_sp[4]=0;
			f3_dpix_sp[8]=0;
		}



		/* set scanline priority */
		{
			int pri_max_opa=-1;
			for(i=0;i<5;i++)	/* i = playfield num (pos) */
			{
				int p0=pri[i];
				int pri_sl1=p0&0x0f;

				layer_tmp[i]=i + (pri_sl1<<3);

				if(!alpha_mode[i])
				{
					layer_tmp[i]|=0x80;
					count_skip_layer++;
				}
				else if(alpha_mode[i]==1 && (alpha_mode_flag[i]&0x80))
				{
					if(layer_tmp[i]>pri_max_opa) pri_max_opa=layer_tmp[i];
				}
			}

			if(pri_max_opa!=-1)
			{
				if(pri_max_opa>layer_tmp[0]) {layer_tmp[0]|=0x80;count_skip_layer++;}
				if(pri_max_opa>layer_tmp[1]) {layer_tmp[1]|=0x80;count_skip_layer++;}
				if(pri_max_opa>layer_tmp[2]) {layer_tmp[2]|=0x80;count_skip_layer++;}
				if(pri_max_opa>layer_tmp[3]) {layer_tmp[3]|=0x80;count_skip_layer++;}
				if(pri_max_opa>layer_tmp[4]) {layer_tmp[4]|=0x80;count_skip_layer++;}
			}
		}


		/* sort layer_tmp */
		for(i=0;i<4;i++)
		{
			for(j=i+1;j<5;j++)
			{
				if(layer_tmp[i]<layer_tmp[j])
				{
					int temp = layer_tmp[i];
					layer_tmp[i] = layer_tmp[j];
					layer_tmp[j] = temp;
				}
			}
		}


		/* check sprite & layer priority */
		{
			int l0,l1,l2,l3,l4;
			int pri_sp[5];

			l0=layer_tmp[0]>>3;
			l1=layer_tmp[1]>>3;
			l2=layer_tmp[2]>>3;
			l3=layer_tmp[3]>>3;
			l4=layer_tmp[4]>>3;

			pri_sp[0]=spri&0xf;
			pri_sp[1]=(spri>>4)&0xf;
			pri_sp[2]=(spri>>8)&0xf;
			pri_sp[3]=spri>>12;

			for(i=0;i<4;i++)	/* i = sprite priority offset */
			{
				int sp,sflg=1<<i;
				if(!(tf3_sprite_pri_usage & sflg)) continue;
				sp=pri_sp[i];

				/*
                    sprite priority==playfield priority
                        BUBSYMPH (title)       ---> sprite
                        DARIUSG (ZONE V' BOSS) ---> playfield
                */

				if (f3_game == BUBSYMPH ) sp++;		//BUBSYMPH (title)

					 if(		  sp>l0) sprite[0]|=sflg;
				else if(sp<=l0 && sp>l1) sprite[1]|=sflg;
				else if(sp<=l1 && sp>l2) sprite[2]|=sflg;
				else if(sp<=l2 && sp>l3) sprite[3]|=sflg;
				else if(sp<=l3 && sp>l4) sprite[4]|=sflg;
				else if(sp<=l4		   ) sprite[5]|=sflg;
			}
		}


		/* draw scanlines */
		alpha=0;
		for(i=count_skip_layer;i<5;i++)
		{
			pos=layer_tmp[i]&7;
			line_t[i]=&pf_line_inf[pos];

			if(sprite[i]&sprite_alpha_check) alpha=1;
			else if(!alpha) sprite[i]|=0x100;

			if(alpha_mode[pos]>1)
			{
				int alpha_type=(((alpha_mode_flag[pos]>>4)&3)-1)*2;
				f3_dpix_lp[i]=f3_dpix_n[alpha_mode[pos]+alpha_type];
				alpha=1;
			}
			else
			{
				if(alpha) f3_dpix_lp[i]=f3_dpix_n[1];
				else	  f3_dpix_lp[i]=f3_dpix_n[0];
			}
		}
		if(sprite[5]&sprite_alpha_check) alpha=1;
		else if(!alpha) sprite[5]|=0x100;

 // 		y_end=y_end_next;
//		y_start=y_start_next;
// y_start
//    if(shouldp()) printf("go dsl y_start:%d y_end:%d\n",draw_line_num[0],draw_line_num[i-2]);
	//f3_drawscanlines(bitmap,320,draw_line_num,line_t,sprite,rot,count_skip_layer);
    tf3_drawscanlines_k(bitmap,320,draw_line_num,line_t,sprite,rot,count_skip_layer);

		if(y_start<0) break;
//		nbw++;
		//break;
	}

  // if(shouldp())
  // {
  //       printf("draws: c1:%d \tc2:%d \tc3:%d \tc4:%d\n",nbwr_case1,nbwr_case2,nbwr_case3,nbwr_case4);
  //       printf("draws: c5:%d \tc6:%d \tc7:%d \tc8:%d\n",nbwr_case5,nbwr_case6,nbwr_case7,nbwr_case8);
  //       printf("draws: c9:%d \tc10:%d \tc11:%d \tc12:%d\n",nbwr_case9,nbwr_case10,nbwr_case11,nbwr_case12);
  //       printf("draws: c13:%d \tc14:%d \tc15:%d \tc16:%d\n",nbwr_case13,nbwr_case14,nbwr_case15,nbwr_case16);
  //       printf("draws: c17:%d \tc18:%d\n",nbwr_case17,nbwr_case18);
  // }

//        if(shouldp()) printf("end upd %d\n",nbw);
}
