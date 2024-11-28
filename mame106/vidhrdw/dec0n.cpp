// dec0n.cpp
extern "C" {
    #include "mame.h"
    #include "driver.h"
    #include "includes/dec0.h"
    #include "tilemap.h"
    void custom_tilemap_draw(mame_bitmap *bitmap,
								const rectangle *cliprect,
								tilemap *tilemap_ptr,
								const UINT16 *rowscroll_ptr,
								const UINT16 *colscroll_ptr,
								const UINT16 *control0,
								const UINT16 *control1,
								int flags);
}

typedef struct {
    mame_bitmap *bitmap;
	const rectangle *cliprect;
    const mame_bitmap *src_bitmap;
    const UINT16 *rowscroll_ptr;
    const UINT16 *colscroll_ptr;
    const UINT16 *control1;
    const UINT16 columnOffsetConstant;
} custileparams;

template<int flipscreen, int ignoretransp,int mapfront,int row_scroll_enabled,int col_scroll_enabled>
void custom_tilemap_drawT( custileparams *params)
{

	int src_y=0;
	UINT16 scrollx=params->control1[0];
	UINT16 scrolly=params->control1[1];
	int width_mask = params->src_bitmap->width - 1;
	int height_mask = params->src_bitmap->height - 1;

	/* Column scroll & row scroll may per applied per pixel, there are
    shift registers for each which control the granularity of the row/col
    offset (down to per line level for row, and per 8 lines for column).

    Nb:  The row & col selectors are _not_ affected by the shape of the
    playfield (ie, 256*1024, 512*512 or 1024*256).  So even if the tilemap
    width is only 256, 'src_x' should not wrap at 256 in the code below (to
    do so would mean the top half of row RAM would never be accessed which
    is incorrect).

    Nb2:  Real hardware exhibits a strange bug with column scroll on 'mode 2'
    (256*1024) - the first column has a strange additional offset, but
    curiously the first 'wrap' (at scroll offset 256) does not have this offset,
    it is displayed as expected.  The bug is confimed to only affect this mode,
    the other two modes work as expected.  This bug is not emulated, as it
    doesn't affect any games.
    */

    int yscrollf = params->control1[3]&0xf;
    int xscrollf = params->control1[2]&0xf;

    UINT16 xscrollfmask =   (0x003f>>xscrollf);
    xscrollf +=3; // because does ((src_x >> 3) >> xscrollf)

    const UINT16 *rowscroll_ptr = params->rowscroll_ptr;
    const UINT16 *colscroll_ptr = params->colscroll_ptr;

    const UINT16 **srclines = (const UINT16 **)params->src_bitmap->line;

    UINT16 **destlines = (UINT16 **)params->bitmap->line;

    INT16 y,maxx= (INT16)params->cliprect->max_x, maxy = (INT16)params->cliprect->max_y;
    INT16 srcbmwidth = params->src_bitmap->width;

	if (flipscreen)
		src_y = (params->src_bitmap->height - 256) - scrolly  ;
	else
		src_y = scrolly;

	for (y=0; y<=maxy; y++) {
    	INT16 src_x;
		if (row_scroll_enabled)
			src_x=scrollx + rowscroll_ptr[(src_y >> yscrollf) & (0x1ff>>yscrollf)];
		else
			src_x=scrollx;

		if (flipscreen)
			src_x=(srcbmwidth - 256) - src_x;

        UINT16 *pline = destlines[y];

        INT16 x=0;
        //int srcy2 = (src_y + params->columnOffsetConstant) & height_mask;
        const UINT16 *line2 = srclines[(src_y + params->columnOffsetConstant) & height_mask];

		for (x=0; x<=maxx; x++ , src_x++ ) {

            UINT16  p;
			if (col_scroll_enabled)
			{
				int column_offset=colscroll_ptr[(src_x >> xscrollf) & xscrollfmask ];
				p = (srclines[(src_y + column_offset)&height_mask])[src_x&width_mask];
            } else
            {
                p = line2[src_x&width_mask];
            }

			if (ignoretransp || (p&0xf))
			{
				if(mapfront)
				{
					/* Top 8 pens of top 8 palettes only */
                    if ((p&0x88)==0x88) pline[x] = p; // Machine->pens[p];
				}
				else
				{

                    pline[x] = p; // Machine->pens[p];
				}
			}
		}
		src_y++;
	}
}


void custom_tilemap_draw(mame_bitmap *bitmap,
								const rectangle *cliprect,
								tilemap *tilemap_ptr,
								const UINT16 *rowscroll_ptr,
								const UINT16 *colscroll_ptr,
								const UINT16 *control0,
								const UINT16 *control1,
								int flags)
{
	const mame_bitmap *src_bitmap = tilemap_get_pixmap(tilemap_ptr);
	if (!src_bitmap)
		return;
// <int flip_screen, int ignoretransp,int mapfront,int row_scroll_enabled,int col_scroll_enabled>
//	int row_scroll_enabled = (rowscroll_ptr && (control0[0]&0x4));
//	int col_scroll_enabled = (colscroll_ptr && (control0[0]&0x8));
//	int ignoretransp = (flags&TILEMAP_IGNORE_TRANSPARENCY);
//	int mapfront = (flags&TILEMAP_FRONT);

    // test column offset per column, in most case we can use a constant and do less tiling.
    int doColumnOffset = ((colscroll_ptr && (control0[0]&0x8))!=0);
    int columnOffsetConstant=0;
    if(doColumnOffset)
    {
        int columnOffsetIsConstant=1;
        // we bet just a single y constant would be ok...
        int xscrollf = control1[2]&0xf;
        UINT16 xscrollfmask = (0x003f>>xscrollf);
        columnOffsetConstant=colscroll_ptr[0 ];
        for(UINT16 x=1;x<xscrollfmask;x++)
        {
            int next_column_offset=colscroll_ptr[x];
            if(next_column_offset != columnOffsetConstant)
            {
                columnOffsetIsConstant=0;
                break;
            }

        }
        if(columnOffsetIsConstant) doColumnOffset=0;
    }


    // 5 booleans will gives 32 possible routines
    int rendertypeid = (flip_screen !=0) |
                    ((flags&TILEMAP_IGNORE_TRANSPARENCY)>>3) |
                     ( (flags&TILEMAP_FRONT)>>4) |
                     (((rowscroll_ptr && (control0[0]&0x4)) !=0)<<3) |
                    ((doColumnOffset)<<4);


    custileparams params = {
        bitmap,cliprect,src_bitmap,rowscroll_ptr,colscroll_ptr,control1,columnOffsetConstant
    };

    switch(rendertypeid)
    {
        case 0:  custom_tilemap_drawT<0,0,0,0,0>(&params); break;
        case 1:  custom_tilemap_drawT<1,0,0,0,0>(&params); break;
        case 2:  custom_tilemap_drawT<0,1,0,0,0>(&params); break;
        case 3:  custom_tilemap_drawT<1,1,0,0,0>(&params); break;
        case 4:  custom_tilemap_drawT<0,0,1,0,0>(&params); break;
        case 5:  custom_tilemap_drawT<1,0,1,0,0>(&params); break;
        case 6:  custom_tilemap_drawT<0,1,1,0,0>(&params); break;
        case 7:  custom_tilemap_drawT<1,1,1,0,0>(&params); break;

        case 8:  custom_tilemap_drawT<0,0,0,1,0>(&params); break;
        case 9:  custom_tilemap_drawT<1,0,0,1,0>(&params); break;
        case 10:  custom_tilemap_drawT<0,1,0,1,0>(&params); break;
        case 11:  custom_tilemap_drawT<1,1,0,1,0>(&params); break;
        case 12:  custom_tilemap_drawT<0,0,1,1,0>(&params); break;
        case 13:  custom_tilemap_drawT<1,0,1,1,0>(&params); break;
        case 14:  custom_tilemap_drawT<0,1,1,1,0>(&params); break;
        case 15:  custom_tilemap_drawT<1,1,1,1,0>(&params); break;

        case 16:  custom_tilemap_drawT<0,0,0,0,1>(&params); break;
        case 17:  custom_tilemap_drawT<1,0,0,0,1>(&params); break;
        case 18:  custom_tilemap_drawT<0,1,0,0,1>(&params); break;
        case 19:  custom_tilemap_drawT<1,1,0,0,1>(&params); break;
        case 20:  custom_tilemap_drawT<0,0,1,0,1>(&params); break;
        case 21:  custom_tilemap_drawT<1,0,1,0,1>(&params); break;
        case 22:  custom_tilemap_drawT<0,1,1,0,1>(&params); break;
        case 23:  custom_tilemap_drawT<1,1,1,0,1>(&params); break;

        case 24:  custom_tilemap_drawT<0,0,0,1,1>(&params); break;
        case 25:  custom_tilemap_drawT<1,0,0,1,1>(&params); break;
        case 26:  custom_tilemap_drawT<0,1,0,1,1>(&params); break;
        case 27:  custom_tilemap_drawT<1,1,0,1,1>(&params); break;
        case 28:  custom_tilemap_drawT<0,0,1,1,1>(&params); break;
        case 29:  custom_tilemap_drawT<1,0,1,1,1>(&params); break;
        case 30:  custom_tilemap_drawT<0,1,1,1,1>(&params); break;
        case 31:  custom_tilemap_drawT<1,1,1,1,1>(&params); break;

    }

}
