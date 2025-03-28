#include "driver.h"
#include "profiler.h"

UINT16 *taitob_scroll;
UINT16 *TC0180VCU_ram;
UINT16 *taitob_spriteram;
UINT16 *taitob_pixelram;

static tilemap *bg_tilemap, *fg_tilemap, *tx_tilemap;
static UINT16 bg_rambank[2],fg_rambank[2],tx_rambank;

/* framebuffer is a raw bitmap, remapped as a last step */
static mame_bitmap *framebuffer[2],*pixel_bitmap;

static UINT16 pixel_scroll[2];

static UINT8 framebuffer_page;

static int b_bg_color_base = 0;
static int b_fg_color_base = 0;
static int b_sp_color_base = 0;
static int b_tx_color_base = 0;


static UINT8 video_control = 0;
static UINT16 TC0180VCU_ctrl[0x10] = {0};

/* TC0180VCU control registers:
* offset:
* 0 - -----xxx bg ram page 0 (tile codes)
*     -xxx---- bg ram page 1 (attributes)
* 1 - -----xxx fg ram page 0 (tile codes)
*     -xxx---- fg ram page 1 (attributes)
* 2 - xxxxxxxx number of independent foreground scrolling blocks (see below)
* 3 - xxxxxxxx number of independent background scrolling blocks
* 4 - --xxxxxx text tile bank 0
* 5 - --xxxxxx text tile bank 1
* 6 - ----xxxx text ram page
* 7 - xxxxxxxx video control: pixelram page and enable, screen flip, sprite to foreground priority (see below)
* 8 to f - unused (always zero)
*
******************************************************************************************
*
* offset 6 - text video page register:
*            This location controls which page of video text ram to view
* hitice:
*     0x08 (00001000) - show game text: credits XX, player1 score
*     0x09 (00001001) - show FBI logo
* rambo3:
*     0x08 (00001000) - show game text
*     0x09 (00001001) - show taito logo
*     0x0a (00001010) - used in pair with 0x09 to smooth screen transitions (attract mode)
*
* Is bit 3 (0x08) video text enable/disable ?
*
******************************************************************************************
*
* offset 7 - video control register:
*            bit 0 (0x01) 1 = don't erase sprite frame buffer "after the beam"
*            bit 3 (0x08) sprite to foreground priority
*                         1 = bg, fg, obj, tx
*                         0 = bg, obj1, fg, obj0, tx (obj0/obj1 selected by bit 0 of color code)
*            bit 4 (0x10) screen flip (active HI) (this one is for sure)
*            bit 5 (0x20) could be global video enable switch (Hit the Ice clears this
*                         bit, clears videoram portions and sets this bit)
*            bit 6 (0x40) frame buffer page to show when bit 7 is set
*            bit 7 (0x80) don't flip frame buffer every vblank, use the page selected by bit 6
*
*/

static void taitob_video_control (unsigned char data)
{
#if 0
	if (data != video_control)
		ui_popup("video control = %02x",data);
#endif

	video_control = data;

	if (video_control & 0x80)
		framebuffer_page = (~video_control & 0x40) >> 6;

	tilemap_set_flip(ALL_TILEMAPS, (video_control & 0x10) ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0 );
}


READ16_HANDLER( taitob_v_control_r )
{
	return (TC0180VCU_ctrl[offset]);
}

WRITE16_HANDLER( taitob_v_control_w )
{
	UINT16 oldword = TC0180VCU_ctrl[offset];

	COMBINE_DATA (&TC0180VCU_ctrl[offset]);

	if (ACCESSING_MSB)
	{
		switch(offset)
		{
		case 0:
			if (oldword != TC0180VCU_ctrl[offset])
			{
				tilemap_mark_all_tiles_dirty(fg_tilemap);
				fg_rambank[0] = (((TC0180VCU_ctrl[offset]>>8)&0x0f)<<12);
				fg_rambank[1] = (((TC0180VCU_ctrl[offset]>>12)&0x0f)<<12);
			}
			break;
		case 1:
			if (oldword != TC0180VCU_ctrl[offset])
			{
				tilemap_mark_all_tiles_dirty(bg_tilemap);
				bg_rambank[0] = (((TC0180VCU_ctrl[offset]>>8)&0x0f)<<12);
				bg_rambank[1] = (((TC0180VCU_ctrl[offset]>>12)&0x0f)<<12);
			}
			break;
		case 4:
		case 5:
			if (oldword != TC0180VCU_ctrl[offset])
				tilemap_mark_all_tiles_dirty(tx_tilemap);
			break;
		case 6:
			if (oldword != TC0180VCU_ctrl[offset])
			{
				tilemap_mark_all_tiles_dirty(tx_tilemap);
				tx_rambank = (((TC0180VCU_ctrl[offset]>>8)&0x0f)<<11);
			}
			break;
		case 7:
			taitob_video_control( (TC0180VCU_ctrl[offset]>>8) & 0xff );
			break;
		default:
			break;
		}
	}
}

WRITE16_HANDLER( hitice_pixelram_w )
{
  int sy = offset >> 9;
  int sx = offset & 0x1ff;

  COMBINE_DATA(&taitob_pixelram[offset]);

  if (ACCESSING_LSB)
  {
    /* bit 15 of pixel_scroll[0] is probably flip screen */

    /* I always draw flipped because the driver doesn't support flip screen yet */
    //plot_pixel(pixel_bitmap,1023-2*sx,  511-sy,Machine->pens[b_fg_color_base * 16 + (data & 0xff)]);
    //plot_pixel(pixel_bitmap,1023-2*sx-1,511-sy,Machine->pens[b_fg_color_base * 16 + (data & 0xff)]);
    plot_pixel(pixel_bitmap,2*sx,  sy,Machine->pens[b_fg_color_base * 16 + (data & 0xff)]);
    plot_pixel(pixel_bitmap,2*sx+1,sy,Machine->pens[b_fg_color_base * 16 + (data & 0xff)]);
  }
}

WRITE16_HANDLER( hitice_pixel_scroll_w )
{
	COMBINE_DATA(&pixel_scroll[offset]);
}

static void hitice_clear_pixel_bitmap(void)
{
	int i;

    for (i = 0;i < 0x40000;i++)
		hitice_pixelram_w(i,0,0);
}

static void get_bg_tile_info(int tile_index)
{
	int tile  = TC0180VCU_ram[tile_index + bg_rambank[0]];
	int color = TC0180VCU_ram[tile_index + bg_rambank[1]];

	SET_TILE_INFO(
		1,
		tile,
		b_bg_color_base + (color&0x3f),
		TILE_FLIPYX((color & 0x00c0)>>6))
}

static void get_fg_tile_info(int tile_index)
{
	int tile  = TC0180VCU_ram[tile_index + fg_rambank[0]];
	int color = TC0180VCU_ram[tile_index + fg_rambank[1]];

	SET_TILE_INFO(
		1,
		tile,
		b_fg_color_base + (color&0x3f),
		TILE_FLIPYX((color & 0x00c0)>>6))
}

static void get_tx_tile_info(int tile_index)
{
	int tile = TC0180VCU_ram[tile_index + tx_rambank];

	SET_TILE_INFO(
		0,
		(tile & 0x07ff) | ((TC0180VCU_ctrl[4 + ((tile & 0x800) >> 11)]>>8) << 11),
		b_tx_color_base + ((tile >> 12) & 0x0f),
		0)
}


static VIDEO_START( taitob_core )
{
	framebuffer[0] = auto_bitmap_alloc(512,256);
	framebuffer[1] = auto_bitmap_alloc(512,256);
	pixel_bitmap = NULL;  /* only hitice needs this */

	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,     16,16,64,64);
	fg_tilemap = tilemap_create(get_fg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,16,16,64,64);
	tx_tilemap = tilemap_create(get_tx_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,64,32);

	if (!bg_tilemap || !fg_tilemap || !tx_tilemap || !framebuffer[0] || !framebuffer[1])
		return 1;

	tilemap_set_transparent_pen(fg_tilemap,0);
	tilemap_set_transparent_pen(tx_tilemap,0);

	tilemap_set_scrolldx(bg_tilemap,0,24*8);
	tilemap_set_scrolldx(fg_tilemap,0,24*8);
	tilemap_set_scrolldx(tx_tilemap,0,24*8);


	state_save_register_global_array(bg_rambank);
	state_save_register_global_array(fg_rambank);
	state_save_register_global(tx_rambank);

	state_save_register_global_array(pixel_scroll);
	state_save_register_global(framebuffer_page);

	state_save_register_global(video_control);
	state_save_register_global_array(TC0180VCU_ctrl);

	state_save_register_global_bitmap(framebuffer[0]);
	state_save_register_global_bitmap(framebuffer[1]);

	return 0;
}

VIDEO_START( taitob_color_order0 )
{
  /*graphics are shared, only that they use different palette*/
  /*this is the basic layout used in: Nastar, Ashura Blaster, Hit the Ice, Rambo3, Tetris*/

  /*Note that in both this and color order 1
  * pixel_color_base/color_granularity is equal to sprites color base.
  * Pure coincidence ?*/

  b_bg_color_base = 0xc0;		/*background*/
  b_fg_color_base = 0x80;		/*foreground*/
  b_sp_color_base = 0x40*16;	/*sprites   */
  b_tx_color_base = 0x00;		/*text      */

  return video_start_taitob_core();
}

VIDEO_START( taitob_color_order1 )
{
  /*and this is the reversed layout used in: Crime City, Puzzle Bobble*/
  b_bg_color_base = 0x00;
  b_fg_color_base = 0x40;
  b_sp_color_base = 0x80*16;
  b_tx_color_base = 0xc0;

  return video_start_taitob_core();
}

VIDEO_START( taitob_color_order2 )
{
  /*this is used in: rambo3a, masterw, silentd, selfeena, ryujin */
  b_bg_color_base = 0x30;
  b_fg_color_base = 0x20;
  b_sp_color_base = 0x10*16;
  b_tx_color_base = 0x00;

  return video_start_taitob_core();
}


VIDEO_START( hitice )
{
  if (video_start_taitob_color_order0())
    return 1;

  pixel_bitmap = auto_bitmap_alloc(1024,512);

  if (!pixel_bitmap)
    return 1;

  state_save_register_global_bitmap(pixel_bitmap);

  return 0;
}

VIDEO_RESET( hitice )
{
	/* kludge: clear the bitmap on startup */
	hitice_clear_pixel_bitmap();
}


READ16_HANDLER( TC0180VCU_word_r )
{
  return TC0180VCU_ram[offset];
}

WRITE16_HANDLER( TC0180VCU_word_w )
{
  UINT16 oldword = TC0180VCU_ram[offset];
  COMBINE_DATA(&TC0180VCU_ram[offset]);

  if (oldword != TC0180VCU_ram[offset])
  {
    if ((offset & 0x7000) == fg_rambank[0] || (offset & 0x7000) == fg_rambank[1])
      tilemap_mark_tile_dirty(fg_tilemap,offset & 0x0fff);
    if ((offset & 0x7000) == bg_rambank[0] || (offset & 0x7000) == bg_rambank[1])
      tilemap_mark_tile_dirty(bg_tilemap,offset & 0x0fff);
    if ((offset & 0x7800) == tx_rambank)
      tilemap_mark_tile_dirty(tx_tilemap,offset & 0x7ff);
  }
}

READ16_HANDLER( TC0180VCU_framebuffer_word_r )
{
  int sy = offset >> 8;
  int sx = 2*(offset & 0xff);

  return
    (read_pixel(framebuffer[sy >> 8],sx,  sy & 0xff) << 8) |
     read_pixel(framebuffer[sy >> 8],sx+1,sy & 0xff);
}

WRITE16_HANDLER( TC0180VCU_framebuffer_word_w )
{
  int sy = offset >> 8;
  int sx = 2*(offset & 0xff);

  if (ACCESSING_MSB)
    plot_pixel(framebuffer[sy >> 8],sx,  sy & 0xff,data >> 8);
  if (ACCESSING_LSB)
    plot_pixel(framebuffer[sy >> 8],sx+1,sy & 0xff,data & 0xff);
}


static void taitob_draw_sprites (mame_bitmap *bitmap,const rectangle *cliprect)
{
/*  Sprite format: (16 bytes per sprite)
  offs:             bits:
  0000: 0xxxxxxxxxxxxxxx: tile code - 0x0000 to 0x7fff in qzshowby
  0002: 0000000000xxxxxx: color (0x00 - 0x3f)
        x000000000000000: flipy
        0x00000000000000: flipx
        00????????000000: unused ?
  0004: xxxxxx0000000000: doesn't matter - some games (eg nastar) fill this with sign bit, some (eg ashura) do not
        000000xxxxxxxxxx: x-coordinate 10 bits signed (all zero for sprites forming up a big sprite, except for the first one)
  0006: xxxxxx0000000000: doesn't matter - same as x
        000000xxxxxxxxxx: y-coordinate 10 bits signed (same as x)
  0008: xxxxxxxx00000000: sprite x-zoom level
        00000000xxxxxxxx: sprite y-zoom level
      0x00 - non scaled = 100%
      0x80 - scaled to 50%
      0xc0 - scaled to 25%
      0xe0 - scaled to 12.5%
      0xff - scaled to zero pixels size (off)
      Sprite zoom is used in Ashura Blaster just in the beginning
      where you can see a big choplifter and a japanese title.
      This japanese title is a scaled sprite.
      It is used in Crime City also at the end of the third level (in the garage)
      where there are four columns on the sides of the screen
      Heaviest usage is in Rambo 3 - almost every sprite in game is scaled
  000a: xxxxxxxx00000000: x-sprites number (big sprite) decremented by one
        00000000xxxxxxxx: y-sprites number (big sprite) decremented by one
  000c - 000f: unused
*/

  int x,y,xlatch=0,ylatch=0,x_no=0,y_no=0,x_num=0,y_num=0,big_sprite=0;
  int offs,code,color,flipx,flipy;
  unsigned int data, zoomx, zoomy, zx, zy, zoomxlatch=0, zoomylatch=0;

  
  { 
  struct drawgfxParams dgp0={
  	bitmap, 	// dest
  	Machine->gfx[1], 	// gfx
  	0, 	// code
  	0, 	// color
  	0, 	// flipx
  	0, 	// flipy
  	0, 	// sx
  	0, 	// sy
  	cliprect, 	// clip
  	TRANSPARENCY_PEN_RAW, 	// transparency
  	0, 	// transparent_color
  	0, 	// scalex
  	0, 	// scaley
  	NULL, 	// pri_buffer
  	0 	// priority_mask
    };
  
  { 
  struct drawgfxParams dgpz0={
  	bitmap, 	// dest
  	Machine->gfx[1], 	// gfx
  	0, 	// code
  	0, 	// color
  	0, 	// flipx
  	0, 	// flipy
  	0, 	// sx
  	0, 	// sy
  	cliprect, 	// clip
  	TRANSPARENCY_PEN_RAW, 	// transparency
  	0, 	// transparent_color
  	0x00010000, 	// scalex
  	0x00010000, 	// scaley
  	NULL, 	// pri_buffer
  	0 	// priority_mask
    };
  for (offs = (0x1980-16)/2; offs >=0; offs -= 8)
  {
    code = taitob_spriteram[offs];

    color = taitob_spriteram[offs+1];
    flipx = color & 0x4000;
    flipy = color & 0x8000;
#if 0
/*check the unknown bits*/
    if (color & 0x3fc0){
      loginfo(2,"sprite color (taitob)=%4x ofs=%4x\n",color,offs);
      color = rand()&0x3f;
    }
#endif
    color = (color & 0x3f) * 16;

    x = taitob_spriteram[offs+2] & 0x3ff;
    y = taitob_spriteram[offs+3] & 0x3ff;
    if (x >= 0x200)  x -= 0x400;
    if (y >= 0x200)  y -= 0x400;

    data = taitob_spriteram[offs+5];
    if (data)
    {
      if (!big_sprite)
      {
        x_num = (data>>8) & 0xff;
        y_num = (data) & 0xff;
        x_no  = 0;
        y_no  = 0;
        xlatch = x;
        ylatch = y;
        data = taitob_spriteram[offs+4];
        zoomxlatch = (data>>8) & 0xff;
        zoomylatch = (data) & 0xff;
        big_sprite = 1;
      }
    }

    data = taitob_spriteram[offs+4];
    zoomx = (data>>8) & 0xff;
    zoomy = (data) & 0xff;
    zx = (0x100 - zoomx) / 16;
    zy = (0x100 - zoomy) / 16;

    if (big_sprite)
    {
      zoomx = zoomxlatch;
      zoomy = zoomylatch;

      x = xlatch + x_no * (0x100 - zoomx) / 16;
      y = ylatch + y_no * (0x100 - zoomy) / 16;
      zx = xlatch + (x_no+1) * (0x100 - zoomx) / 16 - x;
      zy = ylatch + (y_no+1) * (0x100 - zoomy) / 16 - y;
      y_no++;
      if (y_no>y_num)
      {
        y_no = 0;
        x_no++;
        if (x_no>x_num)
          big_sprite = 0;
      }
    }

    if ( zoomx || zoomy )
    {
      
      dgpz0.code = code;
      dgpz0.color = color;
      dgpz0.flipx = flipx;
      dgpz0.flipy = flipy;
      dgpz0.sx = x;
      dgpz0.sy = y;
      dgpz0.scalex = (zx << 16) / 16;
      dgpz0.scaley = (zy << 16) / 16;
      drawgfxzoom(&dgpz0);
    }
    else
    {
      
      dgp0.code = code;
      dgp0.color = color;
      dgp0.flipx = flipx;
      dgp0.flipy = flipy;
      dgp0.sx = x;
      dgp0.sy = y;
      drawgfx(&dgp0);
    }
  }
  } // end of patch paragraph

  } // end of patch paragraph

}


static void TC0180VCU_tilemap_draw(mame_bitmap *bitmap,const rectangle *cliprect,tilemap *tmap,int plane)
{
/*plane = 0 fg tilemap*/
/*plane = 1 bg tilemap*/
  rectangle my_clip;
  int i;
  int scrollx, scrolly;
  int lines_per_block;	/* number of lines scrolled by the same amount (per one scroll value) */
  int number_of_blocks;	/* number of such blocks per _screen_ (256 lines) */

  lines_per_block = 256 - (TC0180VCU_ctrl[2 + plane] >> 8);
  number_of_blocks = 256 / lines_per_block;


  my_clip.min_x =  cliprect->min_x;
  my_clip.max_x =  cliprect->max_x;

  for (i = 0;i < number_of_blocks;i++)
  {
    scrollx = taitob_scroll[ plane*0x200 + i*2*lines_per_block   ];
    scrolly = taitob_scroll[ plane*0x200 + i*2*lines_per_block+1 ];

    my_clip.min_y = i*lines_per_block;
    my_clip.max_y = (i+1)*lines_per_block -1;
	if (video_control&0x10)   /*flip screen*/
	{
		my_clip.min_y = bitmap->height - 1 - (i+1)*lines_per_block -1;
		my_clip.max_y = bitmap->height - 1 - i*lines_per_block;
	}
    sect_rect(&my_clip, cliprect);

    if (my_clip.min_y <= my_clip.max_y)
    {
      tilemap_set_scrollx(tmap,0,-scrollx);
      tilemap_set_scrolly(tmap,0,-scrolly);
      tilemap_draw(bitmap,&my_clip,tmap,0,0);
    }
  }
}


static void draw_framebuffer(mame_bitmap *bitmap,const rectangle *cliprect,int priority)
{
  rectangle myclip = *cliprect;
  int x,y;

profiler_mark(PROFILER_USER1);

  priority <<= 4;


	if (video_control & 0x08)
	{
		if (priority) return;

		if (video_control & 0x10)   /*flip screen*/
		{
			/*ui_popup("1. X[%3i;%3i] Y[%3i;%3i]", myclip.min_x, myclip.max_x, myclip.min_y, myclip.max_y);*/
			for (y = myclip.min_y;y <= myclip.max_y;y++)
			{
				UINT16 *src = ((UINT16 *)framebuffer[framebuffer_page]->line[y]) + myclip.min_x;
				UINT16 *dst;

				dst = ((UINT16 *)bitmap->line[bitmap->height-1-y]) + myclip.max_x;

				for (x = myclip.min_x;x <= myclip.max_x;x++)
				{
					UINT16 c = *src++;

					if (c != 0)
						*dst = Machine->pens[b_sp_color_base + c];

					dst--;
				}
			}
		}
		else
		{
			for (y = myclip.min_y;y <= myclip.max_y;y++)
			{
				UINT16 *src = ((UINT16 *)framebuffer[framebuffer_page]->line[y]) + myclip.min_x;
				UINT16 *dst = ((UINT16 *)bitmap->line[y]) + myclip.min_x;

				for (x = myclip.min_x;x <= myclip.max_x;x++)
				{
					UINT16 c = *src++;

					if (c != 0)
						*dst = Machine->pens[b_sp_color_base + c];

					dst++;
				}
			}
		}
	}
	else
	{
		if (video_control & 0x10)   /*flip screen*/
		{
			/*ui_popup("3. X[%3i;%3i] Y[%3i;%3i]", myclip.min_x, myclip.max_x, myclip.min_y, myclip.max_y);*/
			for (y = myclip.min_y;y <= myclip.max_y;y++)
			{
				UINT16 *src = ((UINT16 *)framebuffer[framebuffer_page]->line[y]) + myclip.min_x;
				UINT16 *dst;

				dst = ((UINT16 *)bitmap->line[bitmap->height-1-y]) + myclip.max_x;

				for (x = myclip.min_x;x <= myclip.max_x;x++)
				{
					UINT16 c = *src++;

					if (c != 0 && (c & 0x10) == priority)
						*dst = Machine->pens[b_sp_color_base + c];

					dst--;
				}
			}
		}
    	else
	    {
	        for (y = myclip.min_y;y <= myclip.max_y;y++)
			{
				UINT16 *src = ((UINT16 *)framebuffer[framebuffer_page]->line[y]) + myclip.min_x;
				UINT16 *dst = ((UINT16 *)bitmap->line[y]) + myclip.min_x;

				for (x = myclip.min_x;x <= myclip.max_x;x++)
				{
					UINT16 c = *src++;

					if (c != 0 && (c & 0x10) == priority)
						*dst = Machine->pens[b_sp_color_base + c];

					dst++;
				}
			}
		}
	}
profiler_mark(PROFILER_END);
}

VIDEO_UPDATE( taitob )
{
  if ((video_control & 0x20) == 0)
  {
    fillbitmap(bitmap,Machine->pens[0],cliprect);
    return;
  }

  /* Draw playfields */
  TC0180VCU_tilemap_draw(bitmap,cliprect,bg_tilemap,1);

  draw_framebuffer(bitmap,cliprect,1);

  TC0180VCU_tilemap_draw(bitmap,cliprect,fg_tilemap,0);

  if (pixel_bitmap)  /* hitice only */
  {
    int scrollx = -2*pixel_scroll[0]; //+320;
    int scrolly = -pixel_scroll[1]; //+240;
    /* bit 15 of pixel_scroll[0] is probably flip screen */

    copyscrollbitmap(bitmap,pixel_bitmap,1,&scrollx,1,&scrolly,cliprect,TRANSPARENCY_COLOR,b_fg_color_base * 16);
  }

  draw_framebuffer(bitmap,cliprect,0);

  tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
}



VIDEO_EOF( taitob )
{
  if (~video_control & 0x01)
    fillbitmap(framebuffer[framebuffer_page],0,&Machine->visible_area);

  if (~video_control & 0x80)
    framebuffer_page ^= 1;

  taitob_draw_sprites(framebuffer[framebuffer_page],&Machine->visible_area);
}

