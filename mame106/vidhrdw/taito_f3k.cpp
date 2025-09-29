
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
   void video_update_taito_f3k_drawsprites(mame_bitmap *bitmap, const rectangle *cliprect);
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
