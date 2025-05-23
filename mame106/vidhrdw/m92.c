/*****************************************************************************

    Irem M92 video hardware, Bryan McPhail, mish@tendril.co.uk

    Brief Overview:

        3 scrolling playfields, 512 by 512.
        Each playfield can enable rowscroll, change shape (to 1024 by 512),
        be enabled/disabled and change position in VRAM.

        Tiles can have several priority values:
            0 = standard
            1 = Top 8 pens appear over sprites (split tilemap)
            2 = Whole tile appears over sprites
            3 = ?  Seems to be the whole tile is over sprites (as 2).

        Sprites have 2 priority values:
            0 = standard
            1 = Sprite appears over all tiles, including high priority pf1

        Raster interrupts can be triggered at any line of the screen redraw,
        typically used in games like R-Type Leo to multiplex the top playfield.

*****************************************************************************

    Master Control registers:

        Word 0: Playfield 1 control
            Bit  0x40:  1 = Rowscroll enable, 0 = disable
            Bit  0x10:  0 = Playfield enable, 1 = disable
            Bit  0x04:  0 = 512 x 512 playfield, 1 = 1024 x 512 playfield
            Bits 0x03:  Playfield location in VRAM (0, 0x4000, 0x8000, 0xc000)
        Word 1: Playfield 2 control (as above)
        Word 2: Playfield 3 control (as above)
        Word 3: Raster IRQ position.

    The raster IRQ position is offset by 128+8 from the first visible line,
    suggesting there are 8 lines before the first visible one.

*****************************************************************************/

#include "driver.h"
#include "drawgfxn.h"

static tilemap *pf3_wide_layer,*pf3_layer,*pf2_layer,*pf1_wide_layer,*pf1_layer;
static INT32 pf1_control[8],pf2_control[8],pf3_control[8],pf4_control[8];
static INT32 pf1_vram_ptr,pf2_vram_ptr,pf3_vram_ptr;
static INT32 pf1_enable,pf2_enable,pf3_enable;
static INT32 pf1_rowscroll,pf2_rowscroll,pf3_rowscroll;
static INT32 pf1_shape,pf2_shape,pf3_shape;
static INT32 m92_sprite_list;

int m92_raster_irq_position,m92_raster_enable;
unsigned char *m92_vram_data,*m92_spritecontrol;
int m92_game_kludge;

extern void m92_sprite_interrupt(void);
int m92_sprite_buffer_busy;
static int m92_palette_bank;

/*****************************************************************************/

static void spritebuffer_callback (int dummy)
{
	m92_sprite_buffer_busy=0x80;
	if (m92_game_kludge!=2) /* Major Title 2 doesn't like this interrupt!? */
		m92_sprite_interrupt();
}

WRITE8_HANDLER( m92_spritecontrol_w )
{
	static int sprite_extent;

	m92_spritecontrol[offset]=data;

	/* Sprite list size register - used in spriteroutine */
	if (offset==0)
		sprite_extent=data;

	/* Sprite control - display all sprites, or partial list */
	if (offset==4) {
		if (data==8)
			m92_sprite_list=(((0x100 - sprite_extent)&0xff)*8);
		else
			m92_sprite_list=0x800;

		/* Bit 0 is also significant */
	}

	/* Sprite buffer - the data written doesn't matter (confirmed by several games) */
	if (offset==8) {
		buffer_spriteram_w(0,0);
		m92_sprite_buffer_busy=0;

		/* Pixel clock is 26.6666 MHz, we have 0x800 bytes, or 0x400 words
        to copy from spriteram to the buffer.  It seems safe to assume 1
        word can be copied per clock.  So:

            1 MHz clock would be 1 word every 0.000,001s = 1000ns
            26.6666MHz clock would be 1 word every 0.000,000,037 = 37 ns
            Buffer should copy in about 37888 ns.
        */
		timer_set (TIME_IN_NSEC(37 * 0x400), 0, spritebuffer_callback);
	}
//  loginfo(2,"%04x: m92_spritecontrol_w %08x %08x\n",activecpu_get_pc(),offset,data);
}

WRITE8_HANDLER( m92_videocontrol_w )
{
	/*
        Many games write:
            0x2000
            0x201b in alternate frames.

        Some games write to this both before and after the sprite buffer
        register - perhaps some kind of acknowledge bit is in there?

        Lethal Thunder fails it's RAM test with the upper palette bank
        enabled.  This was one of the earlier games and could actually
        be a different motherboard revision (most games use M92-A-B top
        pcb, a M92-A-A revision could exist...).
    */
	if (offset==0)
	{
		/* Access to upper palette bank */
		if ((data & 0x2) == 0x2 && m92_game_kludge!=3) m92_palette_bank = 1;
		else                     m92_palette_bank = 0;
	}
//  loginfo(2,"%04x: m92_videocontrol_w %d = %02x\n",activecpu_get_pc(),offset,data);
}

READ8_HANDLER( m92_paletteram_r )
{
	return paletteram_r(offset + 0x800*m92_palette_bank);
}

WRITE8_HANDLER( m92_paletteram_w )
{
	paletteram_xBBBBBGGGGGRRRRR_le_w(offset + 0x800*m92_palette_bank,data);
}

/*****************************************************************************/

static void get_pf1_tile_info(int tile_index)
{
	int tile,color,pri;
	tile_index = 4*tile_index+pf1_vram_ptr;

	tile=m92_vram_data[tile_index]+(m92_vram_data[tile_index+1]<<8)+((m92_vram_data[tile_index+3]&0x80)<<9);
	color=m92_vram_data[tile_index+2];
	if (m92_vram_data[tile_index+3]&1) pri = 2;
	else if (color&0x80) pri = 1;
	else pri = 0;

	SET_TILE_INFO(
			0,
			tile,
			color&0x7f,
			TILE_FLIPYX((m92_vram_data[tile_index+3] & 0x6)>>1) | TILE_SPLIT(pri))
}

static void get_pf2_tile_info(int tile_index)
{
	int tile,color,pri;
	tile_index = 4*tile_index + pf2_vram_ptr;

	tile=m92_vram_data[tile_index]+(m92_vram_data[tile_index+1]<<8)+((m92_vram_data[tile_index+3]&0x80)<<9);
	color=m92_vram_data[tile_index+2];
	if (m92_vram_data[tile_index+3]&1) pri = 2;
	else if (color&0x80) pri = 1;
	else pri = 0;

	SET_TILE_INFO(
			0,
			tile,
			color&0x7f,
			TILE_FLIPYX((m92_vram_data[tile_index+3] & 0x6)>>1) | TILE_SPLIT(pri))
}

static void get_pf3_tile_info(int tile_index)
{
	int tile,color,pri;
	tile_index = 4*tile_index + pf3_vram_ptr;
	tile=m92_vram_data[tile_index]+(m92_vram_data[tile_index+1]<<8)+((m92_vram_data[tile_index+3]&0x80)<<9);
	color=m92_vram_data[tile_index+2];

	if (color&0x80) pri = 1;
	else pri = 0;

	SET_TILE_INFO(
			0,
			tile,
			color&0x7f,
			TILE_FLIPYX((m92_vram_data[tile_index+3] & 0x6)>>1) | TILE_SPLIT(pri))
}

static void get_pf1_wide_tile_info(int tile_index)
{
	int tile,color,pri;
	tile_index = 4*tile_index + pf1_vram_ptr;

	tile=m92_vram_data[tile_index]+(m92_vram_data[tile_index+1]<<8)+((m92_vram_data[tile_index+3]&0x80)<<9);
	color=m92_vram_data[tile_index+2];
	if (m92_vram_data[tile_index+3]&1) pri = 2;
	else if (color&0x80) pri = 1;
	else pri = 0;

	SET_TILE_INFO(
			0,
			tile,
			color&0x7f,
			TILE_FLIPYX((m92_vram_data[tile_index+3] & 0x6)>>1) | TILE_SPLIT(pri))
}

static void get_pf3_wide_tile_info(int tile_index)
{
	int tile,color,pri;
	tile_index = 4*tile_index + pf3_vram_ptr;

	tile=m92_vram_data[tile_index]+(m92_vram_data[tile_index+1]<<8)+((m92_vram_data[tile_index+3]&0x80)<<9);
	color=m92_vram_data[tile_index+2];

	if (color&0x80) pri = 1;
	else pri = 0;

	SET_TILE_INFO(
			0,
			tile,
			color&0x7f,
			TILE_FLIPYX((m92_vram_data[tile_index+3] & 0x6)>>1) | TILE_SPLIT(pri))
}

/*****************************************************************************/

READ8_HANDLER( m92_vram_r )
{
	return m92_vram_data[offset];
}

WRITE8_HANDLER( m92_vram_w )
{
	int a,wide;

	m92_vram_data[offset]=data;

	/* Work out what area to dirty, potentially more than 1 */
	a=offset&0xc000;
	wide=offset&0x7fff;
	offset&=0x3fff;

	if (a==pf1_vram_ptr || (a==pf1_vram_ptr+0x4000)) {
		tilemap_mark_tile_dirty(pf1_layer,offset/4);
		tilemap_mark_tile_dirty(pf1_wide_layer,wide/4);
	}

	if (a==pf2_vram_ptr)
		tilemap_mark_tile_dirty(pf2_layer,offset/4);

	if (a==pf3_vram_ptr || (a==pf3_vram_ptr+0x4000)) {
		tilemap_mark_tile_dirty(pf3_layer,offset/4);
		tilemap_mark_tile_dirty(pf3_wide_layer,wide/4);
	}
}

/*****************************************************************************/

WRITE8_HANDLER( m92_pf1_control_w )
{
	pf1_control[offset]=data;
}

WRITE8_HANDLER( m92_pf2_control_w )
{
	pf2_control[offset]=data;
}

WRITE8_HANDLER( m92_pf3_control_w )
{
	pf3_control[offset]=data;
}

WRITE8_HANDLER( m92_master_control_w )
{
	static int last_pf1_ptr,last_pf2_ptr,last_pf3_ptr;

	pf4_control[offset]=data;

	switch (offset) {
		case 0: /* Playfield 1 (top layer) */
			if ((pf4_control[0]&0x10)==0x10) pf1_enable=0; else pf1_enable=1;
			if ((pf4_control[0]&0x40)==0x40) pf1_rowscroll=1; else pf1_rowscroll=0;
			pf1_vram_ptr=(pf4_control[0]&3)*0x4000;
			pf1_shape=(pf4_control[0]&4)>>2;

			if (pf1_shape) {
				tilemap_set_enable(pf1_layer,0);
				tilemap_set_enable(pf1_wide_layer,pf1_enable);
			}
			else {
				tilemap_set_enable(pf1_layer,pf1_enable);
				tilemap_set_enable(pf1_wide_layer,0);
			}
			/* Have to dirty tilemaps if vram pointer has changed */
			if (m92_game_kludge!=1 && last_pf1_ptr!=pf1_vram_ptr) {
				tilemap_mark_all_tiles_dirty(pf1_layer);
				tilemap_mark_all_tiles_dirty(pf1_wide_layer);
			}
			last_pf1_ptr=pf1_vram_ptr;
			break;
		case 2: /* Playfield 2 (middle layer) */
			if ((pf4_control[2]&0x10)==0x10) pf2_enable=0; else pf2_enable=1;
			if ((pf4_control[2]&0x40)==0x40) pf2_rowscroll=1; else pf2_rowscroll=0;
			tilemap_set_enable(pf2_layer,pf2_enable);
			pf2_vram_ptr=(pf4_control[2]&3)*0x4000;
			pf2_shape=(pf4_control[2]&4)>>2;
			if (last_pf2_ptr!=pf2_vram_ptr)
				tilemap_mark_all_tiles_dirty(pf2_layer);
			last_pf2_ptr=pf2_vram_ptr;
			break;
		case 4: /* Playfield 3 (bottom layer) */
			if ((pf4_control[4]&0x10)==0x10) pf3_enable=0; else pf3_enable=1;
			if ((pf4_control[4]&0x40)==0x40) pf3_rowscroll=1; else pf3_rowscroll=0;
			pf3_shape=(pf4_control[4]&4)>>2;

			if (pf3_shape) {
				tilemap_set_enable(pf3_layer,0);
				tilemap_set_enable(pf3_wide_layer,pf3_enable);
			}
			else {
				tilemap_set_enable(pf3_layer,pf3_enable);
				tilemap_set_enable(pf3_wide_layer,0);
			}
			pf3_vram_ptr=(pf4_control[4]&3)*0x4000;
			if (last_pf3_ptr!=pf3_vram_ptr) {
				tilemap_mark_all_tiles_dirty(pf3_layer);
				tilemap_mark_all_tiles_dirty(pf3_wide_layer);
			}
			last_pf3_ptr=pf3_vram_ptr;
			break;
		case 6:
		case 7:
//          if (flip_screen)
//              m92_raster_irq_position=256-(((pf4_control[7]<<8) | pf4_control[6])-128);
//          else
				m92_raster_irq_position=((pf4_control[7]<<8) | pf4_control[6])-128;
//          if (offset==7)
//              loginfo(2,"%06x: Raster %d %d\n",activecpu_get_pc(),offset, m92_raster_irq_position);
			break;
	}
}

/*****************************************************************************/

VIDEO_START( m92 )
{

	pf1_layer = tilemap_create(
		get_pf1_tile_info,tilemap_scan_rows,
		TILEMAP_SPLIT,
		8,8,
		64,64
	);

	pf2_layer = tilemap_create(
		get_pf2_tile_info,tilemap_scan_rows,
		TILEMAP_SPLIT,
		8,8,
		64,64
	);

	pf3_layer = tilemap_create(
		get_pf3_tile_info,tilemap_scan_rows,
		TILEMAP_SPLIT,
		8,8,
		64,64
	);

	pf1_wide_layer = tilemap_create(
		get_pf1_wide_tile_info,tilemap_scan_rows,
		TILEMAP_SPLIT,
		8,8,
		128,64
	);

	pf3_wide_layer = tilemap_create(
		get_pf3_wide_tile_info,tilemap_scan_rows,
		TILEMAP_SPLIT,
		8,8,
		128,64
	);

	paletteram = auto_malloc(0x1000);

	if (!pf1_layer || !pf2_layer || !pf3_layer || !pf1_wide_layer || !pf3_wide_layer)
		return 1;

	/* split type 0 - totally transparent in front half */
	tilemap_set_transmask(pf1_layer,0,0xffff,0x0001);
	tilemap_set_transmask(pf2_layer,0,0xffff,0x0001);
	tilemap_set_transmask(pf3_layer,0,0xffff,0x0000);
	tilemap_set_transmask(pf1_wide_layer,0,0xffff,0x0001);
	tilemap_set_transmask(pf3_wide_layer,0,0xffff,0x0000);
	/* split type 1 - pens 0-7 transparent in front half */
	tilemap_set_transmask(pf1_layer,1,0x00ff,0xff01);
	tilemap_set_transmask(pf2_layer,1,0x00ff,0xff01);
	tilemap_set_transmask(pf3_layer,1,0x00ff,0xff00);
	tilemap_set_transmask(pf1_wide_layer,1,0x00ff,0xff01);
	tilemap_set_transmask(pf3_wide_layer,1,0x00ff,0xff00);
	/* split type 2 - pen 0 transparent in front half */
	tilemap_set_transmask(pf1_layer,2,0x0001,0xffff);
	tilemap_set_transmask(pf2_layer,2,0x0001,0xffff);
	tilemap_set_transmask(pf3_layer,2,0x0001,0xfffe);
	tilemap_set_transmask(pf1_wide_layer,2,0x0001,0xffff);
	tilemap_set_transmask(pf3_wide_layer,2,0x0001,0xfffe);

	pf1_vram_ptr=pf2_vram_ptr=pf3_vram_ptr=0;
	pf1_enable=pf2_enable=pf3_enable=0;
	pf1_rowscroll=pf2_rowscroll=pf3_rowscroll=0;
	pf1_shape=pf2_shape=pf3_shape=0;

	memset(spriteram,0,0x800);
	memset(buffered_spriteram,0,0x800);

	state_save_register_global(pf1_vram_ptr);
	state_save_register_global(pf1_shape);
	state_save_register_global(pf1_enable);
	state_save_register_global(pf1_rowscroll);
	state_save_register_global_array(pf1_control);

	state_save_register_global(pf2_vram_ptr);
	state_save_register_global(pf2_shape);
	state_save_register_global(pf2_enable);
	state_save_register_global(pf2_rowscroll);
	state_save_register_global_array(pf2_control);

	state_save_register_global(pf3_vram_ptr);
	state_save_register_global(pf3_shape);
	state_save_register_global(pf3_enable);
	state_save_register_global(pf3_rowscroll);
	state_save_register_global_array(pf3_control);

	state_save_register_global_array(pf4_control);

	state_save_register_global(m92_sprite_list);
	state_save_register_global(m92_raster_irq_position);
	state_save_register_global(m92_sprite_buffer_busy);
	state_save_register_global(m92_palette_bank);

	state_save_register_global_pointer(paletteram, 0x1000);

	return 0;
}

/*****************************************************************************/

static void m92_drawsprites(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs=0;

	
	{ 
	rectangle clipex = *cliprect;
    clipex.max_x++;
    clipex.max_y++;

	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[1], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&clipex, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		priority_bitmap, 	// pri_buffer
		0  // hand	// priority_mask
	  };
	while (offs<m92_sprite_list) {
		int x,y,sprite,colour,fx,fy,x_multi,y_multi,i,j,s_ptr,pri;

		y=(buffered_spriteram[offs+0] | (buffered_spriteram[offs+1]<<8))&0x1ff;
		x=(buffered_spriteram[offs+6] | (buffered_spriteram[offs+7]<<8))&0x1ff;

		if ((buffered_spriteram[offs+4]&0x80)==0x80) pri=(0|(1<<31)); else pri=(2|(1<<31));


		x = x - 16;
		y = 512 - 16 - y;

	    sprite=(buffered_spriteram[offs+2] | (buffered_spriteram[offs+3]<<8));
		colour=buffered_spriteram[offs+4]&0x7f;

		fx=buffered_spriteram[offs+5]&1;
		fy=(buffered_spriteram[offs+5]&2)>>1;
		y_multi=(buffered_spriteram[offs+1]>>1)&0x3;
		x_multi=(buffered_spriteram[offs+1]>>3)&0x3;

		y_multi=1 << y_multi; /* 1, 2, 4 or 8 */
		x_multi=1 << x_multi; /* 1, 2, 4 or 8 */

		if (fx) x+=16 * (x_multi - 1);

		for (j=0; j<x_multi; j++)
		{
			s_ptr=8 * j;
			if (!fy) s_ptr+=y_multi-1;

			for (i=0; i<y_multi; i++)
			{
                dgp0.code = sprite + s_ptr;
                dgp0.priority_mask = pri;
				dgp0.color = colour;
				if (flip_screen) {
					int ffx=fx,ffy=fy;
					if (ffx) ffx=0; else ffx=1;
					if (ffy) ffy=0; else ffy=1;
					
					dgp0.flipx = ffx;
					dgp0.flipy = ffy;
					dgp0.sx = 496-x;
					dgp0.sy = 496-(y-i*16);
				} else {

					dgp0.flipx = fx;
					dgp0.flipy = fy;
					dgp0.sx = x;
					dgp0.sy = y-i*16;
				}
				drawgfx_clut16_Src8_prio(&dgp0);
				if (fy) s_ptr++; else s_ptr--;
			}
			if (fx) x-=16; else x+=16;
			offs+=8;
		}
	}
	} // end of patch paragraph

}

/*****************************************************************************/

static void m92_update_scroll_positions(void)
{
	int i,pf1_off,pf2_off,pf3_off;

	/*  Playfield 3 rowscroll data is 0xdfc00 - 0xdffff
        Playfield 2 rowscroll data is 0xdf800 - 0xdfbff
        Playfield 1 rowscroll data is 0xdf400 - 0xdf7ff

        It appears to be hardwired to those locations.

        In addition, each playfield is staggered 2 pixels horizontally from the
        previous one.  This is most obvious in Hook & Blademaster.

    */

	if (flip_screen) {
		pf1_off=-25;
		pf2_off=-27;
		pf3_off=-29;
	} else {
		pf1_off=0;
		pf2_off=2;
		pf3_off=4;
	}

	if (pf1_rowscroll) {
		tilemap_set_scroll_rows(pf1_layer,512);
		tilemap_set_scroll_rows(pf1_wide_layer,512);
		for (i=0; i<1024; i+=2)
			tilemap_set_scrollx( pf1_layer,i/2, (m92_vram_data[0xf400+i]+(m92_vram_data[0xf401+i]<<8))-pf1_off);
		for (i=0; i<1024; i+=2)
			tilemap_set_scrollx( pf1_wide_layer,i/2, (m92_vram_data[0xf400+i]+(m92_vram_data[0xf401+i]<<8))-pf1_off+256);
	} else {
		tilemap_set_scroll_rows(pf1_layer,1);
		tilemap_set_scroll_rows(pf1_wide_layer,1);
		tilemap_set_scrollx( pf1_layer,0, (pf1_control[5]<<8)+pf1_control[4]-pf1_off );
		tilemap_set_scrollx( pf1_wide_layer,0, (pf1_control[5]<<8)+pf1_control[4]+256-pf1_off );
	}

	if (pf2_rowscroll) {
		tilemap_set_scroll_rows(pf2_layer,512);
		for (i=0; i<1024; i+=2)
			tilemap_set_scrollx( pf2_layer,i/2, (m92_vram_data[0xf800+i]+(m92_vram_data[0xf801+i]<<8))-pf2_off);
	} else {
		tilemap_set_scroll_rows(pf2_layer,1);
		tilemap_set_scrollx( pf2_layer,0, (pf2_control[5]<<8)+pf2_control[4]-pf2_off );
	}

	if (pf3_rowscroll) {
		tilemap_set_scroll_rows(pf3_layer,512);
		for (i=0; i<1024; i+=2)
			tilemap_set_scrollx( pf3_layer,i/2, (m92_vram_data[0xfc00+i]+(m92_vram_data[0xfc01+i]<<8))-pf3_off);
		tilemap_set_scroll_rows(pf3_wide_layer,512);
		for (i=0; i<1024; i+=2)
			tilemap_set_scrollx( pf3_wide_layer,i/2, (m92_vram_data[0xfc00+i]+(m92_vram_data[0xfc01+i]<<8))-pf3_off+256);
	} else {
		tilemap_set_scroll_rows(pf3_layer,1);
		tilemap_set_scrollx( pf3_layer,0, (pf3_control[5]<<8)+pf3_control[4]-pf3_off );
		tilemap_set_scroll_rows(pf3_wide_layer,1);
		tilemap_set_scrollx( pf3_wide_layer,0, (pf3_control[5]<<8)+pf3_control[4]-pf3_off+256 );
	}

	tilemap_set_scrolly( pf1_layer,0, (pf1_control[1]<<8)+pf1_control[0] );
	tilemap_set_scrolly( pf2_layer,0, (pf2_control[1]<<8)+pf2_control[0] );
	tilemap_set_scrolly( pf3_layer,0, (pf3_control[1]<<8)+pf3_control[0] );
	tilemap_set_scrolly( pf1_wide_layer,0, (pf1_control[1]<<8)+pf1_control[0] );
	tilemap_set_scrolly( pf3_wide_layer,0, (pf3_control[1]<<8)+pf3_control[0] );
}

/*****************************************************************************/

static void m92_screenrefresh(mame_bitmap *bitmap,const rectangle *cliprect)
{
	fillbitmap(priority_bitmap,0,cliprect);

	if (pf3_enable) {
		tilemap_draw(bitmap,cliprect,pf3_wide_layer,TILEMAP_BACK,0);
		tilemap_draw(bitmap,cliprect,pf3_layer,		TILEMAP_BACK,0);
	}
	else
		fillbitmap(bitmap,Machine->pens[0],cliprect);

	tilemap_draw(bitmap,cliprect,pf3_wide_layer,TILEMAP_FRONT,1);
	tilemap_draw(bitmap,cliprect,pf3_layer,		TILEMAP_FRONT,1);

	tilemap_draw(bitmap,cliprect,pf2_layer,		TILEMAP_BACK, 0);
	tilemap_draw(bitmap,cliprect,pf2_layer,		TILEMAP_FRONT,1);

	tilemap_draw(bitmap,cliprect,pf1_wide_layer,TILEMAP_BACK, 0);
	tilemap_draw(bitmap,cliprect,pf1_layer,		TILEMAP_BACK, 0);
	tilemap_draw(bitmap,cliprect,pf1_wide_layer,TILEMAP_FRONT,1);
	tilemap_draw(bitmap,cliprect,pf1_layer,		TILEMAP_FRONT,1);

	m92_drawsprites(bitmap,cliprect);
}

VIDEO_UPDATE( m92 )
{
	m92_update_scroll_positions();
	m92_screenrefresh(bitmap,cliprect);

	/* check the keyboard */
	if (code_pressed_memory(KEYCODE_F1)) {
		m92_raster_enable ^= 1;
		if (m92_raster_enable)
			ui_popup("Raster IRQ enabled");
		else
			ui_popup("Raster IRQ disabled");
	}

	/* Flipscreen appears hardwired to the dipswitch - strange */
	if (readinputport(5)&1)
		flip_screen_set(0);
	else
		flip_screen_set(1);
}

