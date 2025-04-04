/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "vidhrdw/konamiic.h"

static tilemap *bg_tilemap[2];
static tilemap *textlayer;
static UINT8 *private_spriteram[2];
static int priority;

static UINT8 *combasc_io_ram;
static int combasc_vreg;

static int combasc_bank_select; /* 0x00..0x1f */
static int combasc_video_circuit; /* 0 or 1 */
static UINT8 *combasc_page[2];
static UINT8 combasc_scrollram0[0x40];
static UINT8 combasc_scrollram1[0x40];
static UINT8 *combasc_scrollram;


PALETTE_INIT( combasc )
{
	int i,pal,clut = 0;
	for( pal=0; pal<8; pal++ )
	{
		switch( pal )
		{
			case 0: /* other sprites */
			case 2: /* other sprites(alt) */
			clut = 1;	/* 0 is wrong for Firing Range III targets */
			break;

			case 4: /* player sprites */
			case 6: /* player sprites(alt) */
			clut = 2;
			break;

			case 1: /* background */
			case 3: /* background(alt) */
			clut = 1;
			break;

			case 5: /* foreground tiles */
			case 7: /* foreground tiles(alt) */
			clut = 3;
			break;
		}

		for( i=0; i<256; i++ )
		{
			if ((pal & 1) == 0)	/* sprites */
			{
				if (color_prom[256 * clut + i] == 0)
					*(colortable++) = 0;
				else
					*(colortable++) = 16 * pal + color_prom[256 * clut + i];
			}
			else	/* chars */
				*(colortable++) = 16 * pal + color_prom[256 * clut + i];
		}
	}
}

PALETTE_INIT( combascb )
{
	int i,pal;
	for( pal=0; pal<8; pal++ )
	{
		for( i=0; i<256; i++ )
		{
			if ((pal & 1) == 0)	/* sprites */
				*(colortable++) = 16 * pal + (color_prom[i] ^ 0x0f);
			else	/* chars */
				*(colortable++) = 16 * pal + (i & 0x0f);	/* no lookup? */
		}
	}
}

/***************************************************************************

    Callbacks for the TileMap code

***************************************************************************/

static void get_tile_info0(int tile_index)
{
	UINT8 attributes = combasc_page[0][tile_index];
	int bank = 4*((combasc_vreg & 0x0f) - 1);
	int number,color;

	if (bank < 0) bank = 0;
	if ((attributes & 0xb0) == 0) bank = 0;	/* text bank */

	if (attributes & 0x80) bank += 1;
	if (attributes & 0x10) bank += 2;
	if (attributes & 0x20) bank += 4;

	color = ((K007121_ctrlram[0][6]&0x10)*2+16) + (attributes & 0x0f);

	number = combasc_page[0][tile_index + 0x400] + 256*bank;

	SET_TILE_INFO(
			0,
			number,
			color,
			0)
	tile_info.priority = (attributes & 0x40) >> 6;
}

static void get_tile_info1(int tile_index)
{
	UINT8 attributes = combasc_page[1][tile_index];
	int bank = 4*((combasc_vreg >> 4) - 1);
	int number, color;

	if (bank < 0) bank = 0;
	if ((attributes & 0xb0) == 0) bank = 0;	/* text bank */

	if (attributes & 0x80) bank += 1;
	if (attributes & 0x10) bank += 2;
	if (attributes & 0x20) bank += 4;

	color = ((K007121_ctrlram[1][6]&0x10)*2+16+4*16) + (attributes & 0x0f);

	number = combasc_page[1][tile_index + 0x400] + 256*bank;

	SET_TILE_INFO(
			1,
			number,
			color,
			0)
	tile_info.priority = (attributes & 0x40) >> 6;
}

static void get_text_info(int tile_index)
{
	UINT8 attributes = combasc_page[0][tile_index + 0x800];
	int number = combasc_page[0][tile_index + 0xc00];
	int color = 16 + (attributes & 0x0f);

	SET_TILE_INFO(
			0,
			number,
			color,
			0)
}


static void get_tile_info0_bootleg(int tile_index)
{
	UINT8 attributes = combasc_page[0][tile_index];
	int bank = 4*((combasc_vreg & 0x0f) - 1);
	int number, pal, color;

	if (bank < 0) bank = 0;
	if ((attributes & 0xb0) == 0) bank = 0;	/* text bank */

	if (attributes & 0x80) bank += 1;
	if (attributes & 0x10) bank += 2;
	if (attributes & 0x20) bank += 4;

	pal = (bank == 0 || bank >= 0x1c || (attributes & 0x40)) ? 1 : 3;
	color = pal*16;// + (attributes & 0x0f);
	number = combasc_page[0][tile_index + 0x400] + 256*bank;

	SET_TILE_INFO(
			0,
			number,
			color,
			0)
}

static void get_tile_info1_bootleg(int tile_index)
{
	UINT8 attributes = combasc_page[1][tile_index];
	int bank = 4*((combasc_vreg >> 4) - 1);
	int number, pal, color;

	if (bank < 0) bank = 0;
	if ((attributes & 0xb0) == 0) bank = 0;	/* text bank */

	if (attributes & 0x80) bank += 1;
	if (attributes & 0x10) bank += 2;
	if (attributes & 0x20) bank += 4;

	pal = (bank == 0 || bank >= 0x1c || (attributes & 0x40)) ? 5 : 7;
	color = pal*16;// + (attributes & 0x0f);
	number = combasc_page[1][tile_index + 0x400] + 256*bank;

	SET_TILE_INFO(
			1,
			number,
			color,
			0)
}

static void get_text_info_bootleg(int tile_index)
{
//  UINT8 attributes = combasc_page[0][tile_index + 0x800];
	int number = combasc_page[0][tile_index + 0xc00];
	int color = 16;// + (attributes & 0x0f);

	SET_TILE_INFO(
			1,
			number,
			color,
			0)
}

/***************************************************************************

    Start the video hardware emulation.

***************************************************************************/

VIDEO_START( combasc )
{
	combasc_vreg = -1;

	bg_tilemap[0] = tilemap_create(get_tile_info0,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);
	bg_tilemap[1] = tilemap_create(get_tile_info1,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);
	textlayer =  tilemap_create(get_text_info, tilemap_scan_rows,TILEMAP_OPAQUE,     8,8,32,32);

	private_spriteram[0] = auto_malloc(0x800);
	private_spriteram[1] = auto_malloc(0x800);
	memset(private_spriteram[0],0,0x800);
	memset(private_spriteram[1],0,0x800);

	if (bg_tilemap[0] && bg_tilemap[1] && textlayer)
	{
		tilemap_set_transparent_pen(bg_tilemap[0],0);
		tilemap_set_transparent_pen(bg_tilemap[1],0);
		tilemap_set_transparent_pen(textlayer,0);

		tilemap_set_scroll_rows(textlayer,32);

		return 0;
	}

	return 1;
}

VIDEO_START( combascb )
{
	combasc_vreg = -1;

	bg_tilemap[0] = tilemap_create(get_tile_info0_bootleg,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);
	bg_tilemap[1] = tilemap_create(get_tile_info1_bootleg,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);
	textlayer =  tilemap_create(get_text_info_bootleg, tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,32,32);

	private_spriteram[0] = auto_malloc(0x800);
	private_spriteram[1] = auto_malloc(0x800);
	memset(private_spriteram[0],0,0x800);
	memset(private_spriteram[1],0,0x800);

	if (bg_tilemap[0] && bg_tilemap[1] && textlayer)
	{
		tilemap_set_transparent_pen(bg_tilemap[0],0);
		tilemap_set_transparent_pen(bg_tilemap[1],0);
		tilemap_set_transparent_pen(textlayer,0);

		tilemap_set_scroll_rows(bg_tilemap[0],32);
		tilemap_set_scroll_rows(bg_tilemap[1],32);

		return 0;
	}

	return 1;
}

/***************************************************************************

    Memory handlers

***************************************************************************/

READ8_HANDLER( combasc_video_r )
{
	return videoram[offset];
}

WRITE8_HANDLER( combasc_video_w )
{
	if( videoram[offset]!=data )
	{
		videoram[offset] = data;
		if( offset<0x800 )
		{
			if (combasc_video_circuit)
				tilemap_mark_tile_dirty(bg_tilemap[1],offset & 0x3ff);
			else
				tilemap_mark_tile_dirty(bg_tilemap[0],offset & 0x3ff);
		}
		else if( offset<0x1000 && combasc_video_circuit==0 )
		{
			tilemap_mark_tile_dirty( textlayer,offset & 0x3ff);
		}
	}
}


WRITE8_HANDLER( combasc_vreg_w )
{
	if (data != combasc_vreg)
	{
		tilemap_mark_all_tiles_dirty( textlayer );
		if ((data & 0x0f) != (combasc_vreg & 0x0f))
			tilemap_mark_all_tiles_dirty( bg_tilemap[0] );
		if ((data >> 4) != (combasc_vreg >> 4))
			tilemap_mark_all_tiles_dirty( bg_tilemap[1] );
		combasc_vreg = data;
	}
}

WRITE8_HANDLER( combascb_sh_irqtrigger_w )
{
	soundlatch_w(offset,data);
	cpunum_set_input_line_and_vector(1,0,HOLD_LINE,0xff);
}

READ8_HANDLER( combascb_io_r )
{
	return readinputport(offset);
}

WRITE8_HANDLER( combascb_priority_w )
{
	priority = data & 0x20;
}

WRITE8_HANDLER( combasc_bankselect_w )
{
	UINT8 *page = memory_region(REGION_CPU1) + 0x10000;

	if (data & 0x40)
	{
		combasc_video_circuit = 1;
		videoram = combasc_page[1];
		combasc_scrollram = combasc_scrollram1;
	}
	else
	{
		combasc_video_circuit = 0;
		videoram = combasc_page[0];
		combasc_scrollram = combasc_scrollram0;
	}

	priority = data & 0x20;

	if (data & 0x10)
	{
		memory_set_bankptr(1,page + 0x4000 * ((data & 0x0e) >> 1));
	}
	else
	{
		memory_set_bankptr(1,page + 0x20000 + 0x4000 * (data & 1));
	}
}

WRITE8_HANDLER( combascb_bankselect_w )
{
	if (data & 0x40)
	{
		combasc_video_circuit = 1;
		videoram = combasc_page[1];
	}
	else
	{
		combasc_video_circuit = 0;
		videoram = combasc_page[0];
	}

	data = data & 0x1f;
	if( data != combasc_bank_select )
	{
		UINT8 *page = memory_region(REGION_CPU1) + 0x10000;
		combasc_bank_select = data;

		if (data & 0x10)
		{
			memory_set_bankptr(1,page + 0x4000 * ((data & 0x0e) >> 1));
		}
		else
		{
			memory_set_bankptr(1,page + 0x20000 + 0x4000 * (data & 1));
		}

		if (data == 0x1f)
		{
			memory_set_bankptr(1,page + 0x20000 + 0x4000 * (data & 1));
			memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4400, 0x4403, 0, 0, combascb_io_r);/* IO RAM & Video Registers */
			memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4400, 0x4400, 0, 0, combascb_priority_w);
			memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4800, 0x4800, 0, 0, combascb_sh_irqtrigger_w);
			memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4c00, 0x4c00, 0, 0, combasc_vreg_w);
		}
		else
		{
			memory_install_read8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4000, 0x7fff, 0, 0, MRA8_BANK1);	/* banked ROM */
			memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0x4000, 0x7fff, 0, 0, MWA8_ROM);
		}
	}
}

MACHINE_RESET( combasc )
{
	UINT8 *MEM = memory_region(REGION_CPU1) + 0x38000;


	combasc_io_ram  = MEM + 0x0000;
	combasc_page[0] = MEM + 0x4000;
	combasc_page[1] = MEM + 0x6000;

	memset( combasc_io_ram,  0x00, 0x4000 );
	memset( combasc_page[0], 0x00, 0x2000 );
	memset( combasc_page[1], 0x00, 0x2000 );

	combasc_bank_select = -1;
	combasc_bankselect_w( 0,0 );
}

WRITE8_HANDLER( combasc_pf_control_w )
{
	K007121_ctrl_w(combasc_video_circuit,offset,data);

	if (offset == 7)
		tilemap_set_flip(bg_tilemap[combasc_video_circuit],(data & 0x08) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);

	if (offset == 3)
	{
		if (data & 0x08)
			memcpy(private_spriteram[combasc_video_circuit],combasc_page[combasc_video_circuit]+0x1000,0x800);
		else
			memcpy(private_spriteram[combasc_video_circuit],combasc_page[combasc_video_circuit]+0x1800,0x800);
	}
}

READ8_HANDLER( combasc_scrollram_r )
{
	return combasc_scrollram[offset];
}

WRITE8_HANDLER( combasc_scrollram_w )
{
	combasc_scrollram[offset] = data;
}



/***************************************************************************

    Display Refresh

***************************************************************************/

static void draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect, const UINT8 *source,int circuit,UINT32 pri_mask)
{
	int base_color = (circuit*4)*16+(K007121_ctrlram[circuit][6]&0x10)*2;

	K007121_sprites_draw(circuit,bitmap,cliprect,source,base_color,0,0,pri_mask);
}


VIDEO_UPDATE( combasc )
{
	int i;


	if (K007121_ctrlram[0][0x01] & 0x02)
	{
		tilemap_set_scroll_rows(bg_tilemap[0],32);
		for (i = 0;i < 32;i++)
		{
			tilemap_set_scrollx(bg_tilemap[0],i,combasc_scrollram0[i]);
		}
	}
	else
	{
		tilemap_set_scroll_rows(bg_tilemap[0],1);
		tilemap_set_scrollx(bg_tilemap[0],0,K007121_ctrlram[0][0x00] | ((K007121_ctrlram[0][0x01] & 0x01) << 8));
	}

	if (K007121_ctrlram[1][0x01] & 0x02)
	{
		tilemap_set_scroll_rows(bg_tilemap[1],32);
		for (i = 0;i < 32;i++)
		{
			tilemap_set_scrollx(bg_tilemap[1],i,combasc_scrollram1[i]);
		}
	}
	else
	{
		tilemap_set_scroll_rows(bg_tilemap[1],1);
		tilemap_set_scrollx(bg_tilemap[1],0,K007121_ctrlram[1][0x00] | ((K007121_ctrlram[1][0x01] & 0x01) << 8));
	}

	tilemap_set_scrolly(bg_tilemap[0],0,K007121_ctrlram[0][0x02]);
	tilemap_set_scrolly(bg_tilemap[1],0,K007121_ctrlram[1][0x02]);

	fillbitmap(priority_bitmap,0,cliprect);

	if (priority == 0)
	{
		tilemap_draw(bitmap,cliprect,bg_tilemap[1],TILEMAP_IGNORE_TRANSPARENCY|0,4);
		tilemap_draw(bitmap,cliprect,bg_tilemap[1],TILEMAP_IGNORE_TRANSPARENCY|1,8);
		tilemap_draw(bitmap,cliprect,bg_tilemap[0],0,1);
		tilemap_draw(bitmap,cliprect,bg_tilemap[0],1,2);

		/* we use the priority buffer so sprites are drawn front to back */
		draw_sprites(bitmap,cliprect,private_spriteram[1],1,0x0f00);
		draw_sprites(bitmap,cliprect,private_spriteram[0],0,0x4444);
	}
	else
	{
		tilemap_draw(bitmap,cliprect,bg_tilemap[0],TILEMAP_IGNORE_TRANSPARENCY|0,1);
		tilemap_draw(bitmap,cliprect,bg_tilemap[0],TILEMAP_IGNORE_TRANSPARENCY|1,2);
		tilemap_draw(bitmap,cliprect,bg_tilemap[1],1,4);
		tilemap_draw(bitmap,cliprect,bg_tilemap[1],0,8);

		/* we use the priority buffer so sprites are drawn front to back */
		draw_sprites(bitmap,cliprect,private_spriteram[1],1,0x0f00);
		draw_sprites(bitmap,cliprect,private_spriteram[0],0,0x4444);
	}

	if (K007121_ctrlram[0][0x01] & 0x08)
	{
		for (i = 0;i < 32;i++)
		{
			tilemap_set_scrollx(textlayer,i,combasc_scrollram0[0x20+i] ? 0 : TILE_LINE_DISABLED);
			tilemap_draw(bitmap,cliprect,textlayer,0,0);
		}
	}

	/* chop the extreme columns if necessary */
	if (K007121_ctrlram[0][0x03] & 0x40)
	{
		rectangle clip;

		clip = *cliprect;
		clip.max_x = clip.min_x + 7;
		fillbitmap(bitmap,Machine->pens[0],&clip);

		clip = *cliprect;
		clip.min_x = clip.max_x - 7;
		fillbitmap(bitmap,Machine->pens[0],&clip);
	}
}








/***************************************************************************

    bootleg Combat School sprites. Each sprite has 5 bytes:

byte #0:    sprite number
byte #1:    y position
byte #2:    x position
byte #3:
    bit 0:      x position (bit 0)
    bits 1..3:  ???
    bit 4:      flip x
    bit 5:      unused?
    bit 6:      sprite bank # (bit 2)
    bit 7:      ???
byte #4:
    bits 0,1:   sprite bank # (bits 0 & 1)
    bits 2,3:   unused?
    bits 4..7:  sprite color

***************************************************************************/

static void bootleg_draw_sprites( mame_bitmap *bitmap, const rectangle *cliprect, const UINT8 *source, int circuit )
{
	const gfx_element *gfx = Machine->gfx[circuit+2];

	int limit = ( circuit) ? (program_read_byte(0xc2)*256 + program_read_byte(0xc3)) : (program_read_byte(0xc0)*256 + program_read_byte(0xc1));
	const UINT8 *finish;

	source+=0x1000;
	finish = source;
	source+=0x400;
	limit = (0x3400-limit)/8;
	if( limit>=0 ) finish = source-limit*8;
	source-=8;

	while( source>finish )
	{
		UINT8 attributes = source[3]; /* PBxF ?xxX */
		
{ 
struct drawgfxParams dgp0={
	bitmap, 	// dest
	gfx, 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	cliprect, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	15, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
			int number = source[0];
			int x = source[2] - 71 + (attributes & 0x01)*256;
			int y = 242 - source[1];
			UINT8 color = source[4]; /* CCCC xxBB */

			int bank = (color & 0x03) | ((attributes & 0x40) >> 4);

			number = ((number & 0x02) << 1) | ((number & 0x04) >> 1) | (number & (~6));
			number += 256*bank;

			color = (circuit*4)*16 + (color >> 4);

			/*  hacks to select alternate palettes */
//          if(combasc_vreg == 0x40 && (attributes & 0x40)) color += 1*16;
//          if(combasc_vreg == 0x23 && (attributes & 0x02)) color += 1*16;
//          if(combasc_vreg == 0x66 ) color += 2*16;

			
			dgp0.code = number;
			dgp0.color = color;
			dgp0.flipx = attributes & 0x10;
			dgp0.sx = /* flip */				x;
			dgp0.sy = y;
			drawgfx(&dgp0);
		}
} // end of patch paragraph

		source -= 8;
	}
}

VIDEO_UPDATE( combascb )
{
	int i;

	for( i=0; i<32; i++ )
	{
		tilemap_set_scrollx( bg_tilemap[0],i, combasc_io_ram[0x040+i]+5 );
		tilemap_set_scrollx( bg_tilemap[1],i, combasc_io_ram[0x060+i]+3 );
	}
	tilemap_set_scrolly( bg_tilemap[0],0, combasc_io_ram[0x000] );
	tilemap_set_scrolly( bg_tilemap[1],0, combasc_io_ram[0x020] );

	if (priority == 0)
	{
		tilemap_draw( bitmap,cliprect,bg_tilemap[1],TILEMAP_IGNORE_TRANSPARENCY,0);
		bootleg_draw_sprites( bitmap,cliprect, combasc_page[0], 0 );
		tilemap_draw( bitmap,cliprect,bg_tilemap[0],0 ,0);
		bootleg_draw_sprites( bitmap,cliprect, combasc_page[1], 1 );
	}
	else
	{
		tilemap_draw( bitmap,cliprect,bg_tilemap[0],TILEMAP_IGNORE_TRANSPARENCY,0);
		bootleg_draw_sprites( bitmap,cliprect, combasc_page[0], 0 );
		tilemap_draw( bitmap,cliprect,bg_tilemap[1],0 ,0);
		bootleg_draw_sprites( bitmap,cliprect, combasc_page[1], 1 );
	}

	tilemap_draw( bitmap,cliprect,textlayer,0,0);
}
