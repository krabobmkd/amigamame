/*

STV - VDP1

crude drawing code to support some of the basic vdp1 features
based on what hanagumi columns needs
-- to be expanded

the vdp1 draws to the FRAMEBUFFER which is mapped in memory

Framebuffer todo:
- finish manual erase
- add proper framebuffer erase
- 8 bpp support - now we always draw as 16 bpp, but this is not a problem since
  VDP2 interprets framebuffer as 8 bpp in these cases
*/


#include "driver.h"

int vdp1_sprite_log = 0;

UINT32 *stv_vdp1_vram;
UINT32 *stv_vdp1_regs;
UINT8* stv_vdp1_gfx_decode;

extern UINT32 *stv_scu;
extern int stv_vblank;

UINT16	 *stv_framebuffer[2];
UINT16	 **stv_framebuffer_draw_lines, **stv_framebuffer_display_lines;
int		 stv_framebuffer_width;
int		 stv_framebuffer_height;
int		 stv_framebuffer_mode;
int		 stv_framebuffer_double_interlace;
int		 stv_vdp1_fbcr_accessed;
int		 stv_vdp1_current_display_framebuffer;
int		 stv_vdp1_current_draw_framebuffer;
int		 stv_vdp1_clear_framebuffer_on_next_frame;

int stvvdp1_local_x;
int stvvdp1_local_y;

/*TV Mode Selection Register */
/*
   xxxx xxxx xxxx ---- | UNUSED
   ---- ---- ---- x--- | VBlank Erase/Write (VBE)
   ---- ---- ---- -xxx | TV Mode (TVM)
   TV-Mode:
   This sets the Frame Buffer size,the rotation of the Frame Buffer & the bit width.
   bit 2 HDTV disable(0)/enable(1)
   bit 1 non-rotation/rotation(1)
   bit 0 16(0)/8(1) bits per pixel
   Size of the Frame Buffer:
   7 invalid
   6 invalid
   5 invalid
   4 512x256
   3 512x512
   2 512x256
   1 1024x256
   0 512x256
*/
#define STV_VDP1_TVMR ((stv_vdp1_regs[0x000/4] >> 16)&0x0000ffff)
#define STV_VDP1_VBE  ((STV_VDP1_TVMR & 0x0008) >> 3)
#define STV_VDP1_TVM  ((STV_VDP1_TVMR & 0x0007) >> 0)

/*Frame Buffer Change Mode Register*/
/*
   xxxx xxxx xxx- ---- | UNUSED
   ---- ---- ---x ---- | Even/Odd Coordinate Select Bit (EOS)
   ---- ---- ---- x--- | Double Interlace Mode (DIE)
   ---- ---- ---- -x-- | Double Interlace Draw Line (DIL)
   ---- ---- ---- --x- | Frame Buffer Change Trigger (FCM)
   ---- ---- ---- ---x | Frame Buffer Change Mode (FCT)
*/
#define STV_VDP1_FBCR ((stv_vdp1_regs[0x000/4] >> 0)&0x0000ffff)
#define STV_VDP1_EOS ((STV_VDP1_FBCR & 0x0010) >> 4)
#define STV_VDP1_DIE ((STV_VDP1_FBCR & 0x0008) >> 3)
#define STV_VDP1_DIL ((STV_VDP1_FBCR & 0x0004) >> 2)
#define STV_VDP1_FCM ((STV_VDP1_FBCR & 0x0002) >> 1)
#define STV_VDP1_FCT ((STV_VDP1_FBCR & 0x0001) >> 0)

/*Plot Trigger Register*/
/*
   xxxx xxxx xxxx xx-- | UNUSED
   ---- ---- ---- --xx | Plot Trigger Mode (PTM)

   Plot Trigger Mode:
   3 Invalid
   2 Automatic draw
   1 VDP1 draw by request
   0 VDP1 Idle (no access)
*/
#define STV_VDP1_PTMR ((stv_vdp1_regs[0x004/4] >> 16)&0x0000ffff)
#define STV_VDP1_PTM  ((STV_VDP1_PTMR & 0x0003) >> 0)
#define SET_PTM_FROM_1_TO_0 if(STV_VDP1_PTM & 1)	 stv_vdp1_regs[0x004/4]^=0x00010000
/*Erase/Write Upper-Left register*/
/*
   x--- ---- ---- ---- | UNUSED
   -xxx xxx- ---- ---- | X1 register
   ---- ---x xxxx xxxx | Y1 register

*/
#define STV_VDP1_EWLR ((stv_vdp1_regs[0x008/4] >> 16)&0x0000ffff)
#define STV_VDP1_EWLR_X1 ((STV_VDP1_EWLR & 0x7e00) >> 9)
#define STV_VDP1_EWLR_Y1 ((STV_VDP1_EWLR & 0x01ff) >> 0)
/*Erase/Write Lower-Right register*/
/*
   xxxx xxx- ---- ---- | X3 register
   ---- ---x xxxx xxxx | Y3 register

*/
#define STV_VDP1_EWRR ((stv_vdp1_regs[0x008/4] >> 0)&0x0000ffff)
#define STV_VDP1_EWRR_X3 ((STV_VDP1_EWRR & 0xfe00) >> 9)
#define STV_VDP1_EWRR_Y3 ((STV_VDP1_EWRR & 0x01ff) >> 0)
/*Transfer End Status Register*/
/*
   xxxx xxxx xxxx xx-- | UNUSED
   ---- ---- ---- --x- | CEF
   ---- ---- ---- ---x | BEF

*/
#define STV_VDP1_EDSR ((stv_vdp1_regs[0x010/4] >> 16)&0x0000ffff)
#define STV_VDP1_CEF  (STV_VDP1_EDSR & 2)
#define STV_VDP1_BEF  (STV_VDP1_EDSR & 1)
#define SET_CEF_FROM_1_TO_0 	if(STV_VDP1_CEF)	 stv_vdp1_regs[0x010/4]^=0x00020000
#define SET_CEF_FROM_0_TO_1     if(!(STV_VDP1_CEF))	 stv_vdp1_regs[0x010/4]^=0x00020000
/**/



void stv_vdp1_process_list(mame_bitmap *bitmap, const rectangle *cliprect);

READ32_HANDLER( stv_vdp1_regs_r )
{
//  static int x;

//  x ^= 0x00020000;

	loginfo (2, "cpu #%d (PC=%08X) VDP1: Read from Registers, Offset %04x\n",cpu_getactivecpu(), activecpu_get_pc(), offset);
//  if (offset == 0x04) return x;

	return stv_vdp1_regs[offset];
}

static void stv_clear_framebuffer( int which_framebuffer )
{
	if ( vdp1_sprite_log ) loginfo(2, "Clearing %d framebuffer\n", stv_vdp1_current_draw_framebuffer );
	memset( stv_framebuffer[ which_framebuffer ], 0, 1024 * 256 * sizeof(UINT16) * 2 );
}

int stv_vdp1_start ( void )
{
	stv_vdp1_regs = auto_malloc ( 0x040000 );
	stv_vdp1_vram = auto_malloc ( 0x100000 );
	stv_vdp1_gfx_decode = auto_malloc ( 0x100000 );

	memset(stv_vdp1_regs, 0, 0x040000);
	memset(stv_vdp1_vram, 0, 0x100000);

	stv_framebuffer[0] = auto_malloc( 1024 * 256 * sizeof(UINT16) * 2 ); /* *2 is for double interlace */
	stv_framebuffer[1] = auto_malloc( 1024 * 256 * sizeof(UINT16) * 2 );

	stv_framebuffer_display_lines = auto_malloc( 512 * sizeof(UINT16*) );
	stv_framebuffer_draw_lines = auto_malloc( 512 * sizeof(UINT16*) );

	stv_framebuffer_width = stv_framebuffer_height = 0;
	stv_framebuffer_mode = -1;
	stv_framebuffer_double_interlace = -1;
	stv_vdp1_fbcr_accessed = 0;
	stv_vdp1_current_display_framebuffer = 0;
	stv_vdp1_current_draw_framebuffer = 1;
	stv_clear_framebuffer(stv_vdp1_current_draw_framebuffer);
	stv_vdp1_clear_framebuffer_on_next_frame = 0;
	return 0;
}

static void stv_prepare_framebuffers( void )
{
	int i,rowsize;

	rowsize = stv_framebuffer_width;
	if ( stv_vdp1_current_draw_framebuffer == 0 )
	{
		for ( i = 0; i < stv_framebuffer_height; i++ )
		{
			stv_framebuffer_draw_lines[i] = &stv_framebuffer[0][ i * rowsize ];
			stv_framebuffer_display_lines[i] = &stv_framebuffer[1][ i * rowsize ];
		}
		for ( ; i < 512; i++ )
		{
			stv_framebuffer_draw_lines[i] = &stv_framebuffer[0][0];
			stv_framebuffer_display_lines[i] = &stv_framebuffer[1][0];
		}

	}
	else
	{
		for ( i = 0; i < stv_framebuffer_height; i++ )
		{
			stv_framebuffer_draw_lines[i] = &stv_framebuffer[1][ i * rowsize ];
			stv_framebuffer_display_lines[i] = &stv_framebuffer[0][ i * rowsize ];
		}
		for ( ; i < 512; i++ )
		{
			stv_framebuffer_draw_lines[i] = &stv_framebuffer[1][0];
			stv_framebuffer_display_lines[i] = &stv_framebuffer[0][0];
		}

	}

	for ( ; i < 512; i++ )
	{
		stv_framebuffer_draw_lines[i] = &stv_framebuffer[0][0];
		stv_framebuffer_display_lines[i] = &stv_framebuffer[1][0];
	}

}

static void stv_vdp1_change_framebuffers( void )
{
	stv_vdp1_current_display_framebuffer ^= 1;
	stv_vdp1_current_draw_framebuffer ^= 1;
	if ( vdp1_sprite_log ) loginfo(2, "Changing framebuffers: %d - draw, %d - display\n", stv_vdp1_current_draw_framebuffer, stv_vdp1_current_display_framebuffer );
	stv_prepare_framebuffers();
}

static void stv_set_framebuffer_config( void )
{
	if ( stv_framebuffer_mode == STV_VDP1_TVM &&
		 stv_framebuffer_double_interlace == STV_VDP1_DIE ) return;

	if ( vdp1_sprite_log ) loginfo(2, "Setting framebuffer config\n" );
	stv_framebuffer_mode = STV_VDP1_TVM;
	stv_framebuffer_double_interlace = STV_VDP1_DIE;
	switch( stv_framebuffer_mode )
	{
		case 0: stv_framebuffer_width = 512; stv_framebuffer_height = 256; break;
		case 1: stv_framebuffer_width = 1024; stv_framebuffer_height = 256; break;
		case 2: stv_framebuffer_width = 512; stv_framebuffer_height = 256; break;
		case 3: stv_framebuffer_width = 512; stv_framebuffer_height = 512; break;
		case 4: stv_framebuffer_width = 512; stv_framebuffer_height = 256; break;
		default: loginfo(2, "Invalid framebuffer config %x\n", STV_VDP1_TVM ); stv_framebuffer_width = 512; stv_framebuffer_height = 256; break;
	}
	if ( STV_VDP1_DIE ) stv_framebuffer_height *= 2; /* double interlace */

	stv_vdp1_current_draw_framebuffer = 0;
	stv_vdp1_current_display_framebuffer = 1;
	stv_prepare_framebuffers();
}

WRITE32_HANDLER( stv_vdp1_regs_w )
{
	COMBINE_DATA(&stv_vdp1_regs[offset]);
	if ( offset == 0 )
	{
		stv_set_framebuffer_config();
		if ( ACCESSING_LSW32 )
		{
			if ( vdp1_sprite_log ) loginfo(2, "VDP1: Access to register FBCR = %1X\n", STV_VDP1_FBCR );
			stv_vdp1_fbcr_accessed = 1;
		}
		else
		{
			if ( vdp1_sprite_log ) loginfo(2, "VDP1: Access to register TVMR = %1X\n", STV_VDP1_TVMR );
			if ( STV_VDP1_VBE && stv_vblank )
			{
				stv_vdp1_clear_framebuffer_on_next_frame = 1;
			}

			/* needed by pblbeach, it doesn't clear local coordinates in its sprite list...*/
			if ( !strcmp(Machine->gamedrv->name, "pblbeach") )
			{
				stvvdp1_local_x = stvvdp1_local_y = 0;
			}
		}
	}
	else if ( offset == 1 )
	{
		if ( ACCESSING_MSW32 )
		{
			if ( STV_VDP1_PTMR == 1 )
			{
				if ( vdp1_sprite_log ) loginfo(2, "VDP1: Access to register PTMR = %1X\n", STV_VDP1_PTMR );
				stv_vdp1_process_list( NULL, &Machine->visible_area );

				if(!(stv_scu[40] & 0x2000)) /*Sprite draw end irq*/
				{
					loginfo(2, "Interrupt: Sprite draw end, Vector 0x4d, Level 0x02\n" );
					cpunum_set_input_line_and_vector(0, 2, HOLD_LINE , 0x4d);
				}
			}
		}
		else if ( ACCESSING_LSW32 )
		{
			if ( vdp1_sprite_log ) loginfo(2, "VDP1: Erase data set %08X\n", data );
		}
	}
	else if ( offset == 2 )
	{
		if ( ACCESSING_MSW32 )
		{
			if ( vdp1_sprite_log ) loginfo(2, "VDP1: Erase upper-left coord set: %08X\n", data );
		}
		else if ( ACCESSING_LSW32 )
		{
			if ( vdp1_sprite_log ) loginfo(2, "VDP1: Erase lower-right coord set: %08X\n", data );
		}
	}

}

READ32_HANDLER ( stv_vdp1_vram_r )
{
	return stv_vdp1_vram[offset];
}


WRITE32_HANDLER ( stv_vdp1_vram_w )
{
	UINT8 *vdp1 = stv_vdp1_gfx_decode;

	COMBINE_DATA (&stv_vdp1_vram[offset]);

//  if (((offset * 4) > 0xdf) && ((offset * 4) < 0x140))
//  {
//      loginfo(2,"cpu #%d (PC=%08X): VRAM dword write to %08X = %08X & %08X\n", cpu_getactivecpu(), activecpu_get_pc(), offset*4, data, mem_mask ^ 0xffffffff);
//  }

	data = stv_vdp1_vram[offset];
	/* put in gfx region for easy decoding */
	vdp1[offset*4+0] = (data & 0xff000000) >> 24;
	vdp1[offset*4+1] = (data & 0x00ff0000) >> 16;
	vdp1[offset*4+2] = (data & 0x0000ff00) >> 8;
	vdp1[offset*4+3] = (data & 0x000000ff) >> 0;
}

WRITE32_HANDLER ( stv_vdp1_framebuffer0_w )
{
	//ui_popup ("STV VDP1 Framebuffer 0 WRITE offset %08x data %08x",offset, data);
	if ( STV_VDP1_TVM & 0 )
	{
		/* 8-bit mode */
	}
	else
	{
		/* 16-bit mode */
		if ( ACCESSING_MSW32 )
		{
			stv_framebuffer[stv_vdp1_current_draw_framebuffer][offset*2] = (data >> 16) & 0xffff;
		}
		if ( ACCESSING_LSW32 )
		{
			stv_framebuffer[stv_vdp1_current_draw_framebuffer][offset*2+1] = data & 0xffff;
		}
	}
}

READ32_HANDLER ( stv_vdp1_framebuffer0_r )
{
	UINT32 result = 0;
	//ui_popup ("STV VDP1 Framebuffer 0 READ offset %08x",offset);
	if ( STV_VDP1_TVM & 0 )
	{
		/* 8-bit mode */
	}
	else
	{
		/* 16-bit mode */
		if ( ACCESSING_MSW32 )
		{
			result |= (stv_framebuffer[stv_vdp1_current_draw_framebuffer][offset*2] << 16);
		}
		if ( ACCESSING_LSW32 )
		{
			result |= stv_framebuffer[stv_vdp1_current_draw_framebuffer][offset*2+1];
		}

	}

	return result;
}

WRITE32_HANDLER ( stv_vdp1_framebuffer1_w )
{
	//ui_popup ("STV VDP1 Framebuffer 1 WRITE offset %08x data %08x",offset, data);
}

READ32_HANDLER ( stv_vdp1_framebuffer1_r )
{
	//ui_popup ("STV VDP1 Framebuffer 1 READ offset %08x",offset);
	return 0xffff;
}


/*

there is a command every 0x20 bytes
the first word is the control word
the rest are data used by it

---
00 CMDCTRL
   e--- ---- ---- ---- | end bit (15)
   -jjj ---- ---- ---- | jump select bits (12-14)
   ---- zzzz ---- ---- | zoom point / hotspot (8-11)
   ---- ---- 00-- ---- | UNUSED
   ---- ---- --dd ---- | character read direction (4,5)
   ---- ---- ---- cccc | command bits (0-3)

02 CMDLINK
   llll llll llll ll-- | link
   ---- ---- ---- --00 | UNUSED

04 CMDPMOD
   m--- ---- ---- ---- | MON (looks at MSB and apply shadows etc.)
   -00- ---- ---- ---- | UNUSED
   ---h ---- ---- ---- | HSS (High Speed Shrink)
   ---- p--- ---- ---- | PCLIP (Pre Clipping Disable)
   ---- -c-- ---- ---- | CLIP (Clipping Mode Bit)
   ---- --m- ---- ---- | CMOD (User Clipping Enable Bit)
   ---- ---M ---- ---- | MESH (Mesh Enable Bit)
   ---- ---- e--- ---- | ECD (End Code Disable)
   ---- ---- -S-- ---- | SPD (Transparent Pixel Disable)
   ---- ---- --cc c--- | Colour Mode
   ---- ---- ---- -CCC | Colour Calculation bits

06 CMDCOLR
   mmmm mmmm mmmm mmmm | Colour Bank, Colour Lookup /8

08 CMDSRCA (Character Address)
   aaaa aaaa aaaa aa-- | Character Address
   ---- ---- ---- --00 | UNUSED

0a CMDSIZE (Character Size)
   00-- ---- ---- ---- | UNUSED
   --xx xxxx ---- ---- | Character Size (X)
   ---- ---- yyyy yyyy | Character Size (Y)

0c CMDXA (used for normal sprite)
   eeee ee-- ---- ---- | extension bits
   ---- --xx xxxx xxxx | x position

0e CMDYA (used for normal sprite)
   eeee ee-- ---- ---- | extension bits
   ---- --yy yyyy yyyy | y position

10 CMDXB
12 CMDYB
14 CMDXC
16 CMDYC
18 CMDXD
1a CMDYD
1c CMDGRDA (Gouraud Shading Table)
1e UNUSED
---


*/

static struct stv_vdp2_sprite_list
{

	int CMDCTRL, CMDLINK, CMDPMOD, CMDCOLR, CMDSRCA, CMDSIZE, CMDGRDA;
	int CMDXA, CMDYA;
	int CMDXB, CMDYB;
	int CMDXC, CMDYC;
	int CMDXD, CMDYD;

	int ispoly;
	int isalpha;

} stv2_current_sprite;

/* Gouraud shading */

static struct _stv_gouraud_shading
{
	/* Gouraud shading table */
	UINT16	GA;
	UINT16	GB;
	UINT16	GC;
	UINT16	GD;
	/* Gouraud shading data at drawn pixel */
	UINT32	g_r;
	UINT32	g_g;
	UINT32	g_b;
	/* Gouraud shading delta */
	UINT32	gdelta_r;
	UINT32	gdelta_g;
	UINT32	gdelta_b;
} stv_gouraud_shading;

static void stv_clear_gouraud_shading(void)
{
	memset( &stv_gouraud_shading, 0, sizeof( stv_gouraud_shading ) );
}

static UINT8 stv_read_gouraud_table(void)
{
	int gaddr;

	if ( (stv2_current_sprite.CMDPMOD & 0x7) == 4 )
	{
		gaddr = stv2_current_sprite.CMDGRDA * 8;
		stv_gouraud_shading.GA = (stv_vdp1_vram[gaddr/4] >> 16) & 0xffff;
		stv_gouraud_shading.GB = (stv_vdp1_vram[gaddr/4] >> 0) & 0xffff;
		stv_gouraud_shading.GC = (stv_vdp1_vram[gaddr/4 + 1] >> 16) & 0xffff;
		stv_gouraud_shading.GD = (stv_vdp1_vram[gaddr/4 + 1] >> 0) & 0xffff;
		return 1;
	}
	else
	{
		return 0;
	}
}

static UINT16 stv_compute_shading_at_pixel(int size, int pt, UINT16 g1, UINT16 g2 )
{
	int r,g,b;
	b = ((g2 & 0x7c00) >> 10) - ((g1 & 0x7c00) >> 10);
	g = ((g2 & 0x03e0) >> 5)  - ((g1 & 0x03e0) >> 5);
	r = (g2 & 0x001f) - (g1 & 0x001f);

	b = (((b << 16) / size) * pt) >> 16;
	g = (((g << 16) / size) * pt) >> 16;
	r = (((r << 16) / size) * pt) >> 16;

	b += ((g1 & 0x7c00) >> 10);
	g += ((g1 & 0x03e0) >> 5);
	r += (g1 & 0x001f);

	if ( b < 0 ) b = 0;
	if ( b > 0x1f ) b = 0x1f;
	if ( g < 0 ) g = 0;
	if ( g > 0x1f ) g = 0x1f;
	if ( r < 0 ) r = 0;
	if ( r > 0x1f ) r = 0x1f;

	return ((b & 0x1f) << 10) | ((g & 0x1f) << 5) | (b & 0x1f) | 0x8000;
}

static void stv_setup_shading_for_line(int size, UINT16 g1, UINT16 g2 )
{
	int r,g,b;
	b = ((g2 & 0x7c00) >> 10) - ((g1 & 0x7c00) >> 10);
	g = ((g2 & 0x03e0) >> 5)  - ((g1 & 0x03e0) >> 5);
	r = (g2 & 0x001f) - (g1 & 0x001f);

	stv_gouraud_shading.gdelta_b = (b << 16) / size;
	stv_gouraud_shading.gdelta_g = (g << 16) / size;
	stv_gouraud_shading.gdelta_r = (r << 16) / size;

	stv_gouraud_shading.g_b = ((g1 & 0x7c00) >> 10) << 16;
	stv_gouraud_shading.g_g = ((g1 & 0x03e0) >> 5) << 16;
	stv_gouraud_shading.g_r = (g1 & 0x001f) << 16;
}

static void stv_compute_shading_for_next_point(void)
{
	stv_gouraud_shading.g_b += stv_gouraud_shading.gdelta_b;
	stv_gouraud_shading.g_g += stv_gouraud_shading.gdelta_g;
	stv_gouraud_shading.g_r += stv_gouraud_shading.gdelta_r;
}

static UINT16 stv_apply_gouraud_shading( UINT16 pix )
{
	int r,g,b,sr,sg,sb;
	b = (pix & 0x7c00) >> 10;
	g = (pix & 0x03e0) >> 5;
	r = (pix & 0x001f);
	sb = (stv_gouraud_shading.g_b >> 16) & 0x1f;
	sg = (stv_gouraud_shading.g_g >> 16) & 0x1f;
	sr = (stv_gouraud_shading.g_r >> 16) & 0x1f;

	if ( sb < 0x10 )
		b -= (0x10 - sb);
	else
		b += sb - 0x10;

	if ( b < 0 )    b = 0;
	if ( b > 0x1f ) b = 0x1f;

	if ( sg < 0x10 )
		g -= (0x10 - sg);
	else
		g += sg - 0x10;

	if ( g < 0 )	g = 0;
	if ( g > 0x1f ) g = 0x1f;

	if ( sr < 0x10 )
		r -= (0x10 - sr);
	else
		r += sr - 0x10;

	if ( r < 0 )	r = 0;
	if ( r > 0x1f ) r = 0x1f;

	return 0x8000 | b << 10 | g << 5 | r;
}

/* note that if we're drawing
to the framebuffer we CAN'T frameskip the vdp1 drawing as the hardware can READ the framebuffer
and if we skip the drawing the content could be incorrect when it reads it, although i have no idea
why they would want to */

extern UINT32* stv_vdp2_cram;

INLINE void drawpixel(UINT16 *dest, int patterndata, int offsetcnt)
{
	int pix,mode,transmask,spd = stv2_current_sprite.CMDPMOD & 0x40;
	UINT8* gfxdata = stv_vdp1_gfx_decode;
	int pix2;

	if ( stv2_current_sprite.ispoly )
	{
		pix = stv2_current_sprite.CMDCOLR&0xffff;

		transmask = 0xffff;
		if ( pix & 0x8000 )
		{
			mode = 5;
		}
		else
		{
			mode = 1;
		}
	}
	else
	{
		switch (stv2_current_sprite.CMDPMOD&0x0038)
		{
			case 0x0000: // mode 0 16 colour bank mode (4bits) (hanagumi blocks)
				// most of the shienryu sprites use this mode
				pix = gfxdata[patterndata+offsetcnt/2];
				pix = offsetcnt&1 ? (pix & 0x0f):((pix & 0xf0)>>4) ;
				pix = pix+((stv2_current_sprite.CMDCOLR&0xfff0));
				mode = 0;
				transmask = 0xf;
				break;
			case 0x0008: // mode 1 16 colour lookup table mode (4bits)
				// shienryu explosisons (and some enemies) use this mode
				pix2 = gfxdata[patterndata+offsetcnt/2];
				pix2 = offsetcnt&1 ?  (pix2 & 0x0f):((pix2 & 0xf0)>>4);
				pix = pix2&1 ?
				((((stv_vdp1_vram[(((stv2_current_sprite.CMDCOLR&0xffff)*8)>>2)+((pix2&0xfffe)/2)])) & 0x0000ffff) >> 0):
				((((stv_vdp1_vram[(((stv2_current_sprite.CMDCOLR&0xffff)*8)>>2)+((pix2&0xfffe)/2)])) & 0xffff0000) >> 16);

				mode = 1;
				transmask = 0xf;

				if (pix2 & 0xf)
				{
					if (pix & 0x8000)
					{
						mode = 5;
						transmask = 0x8000;
					}
				}
				else
				{
					pix=pix2; // this is messy .. but just ensures that pen 0 isn't drawn
				}
				break;
			case 0x0010: // mode 2 64 colour bank mode (8bits) (character select portraits on hanagumi)
				pix = gfxdata[patterndata+offsetcnt];
				mode = 2;
				pix = pix+(stv2_current_sprite.CMDCOLR&0xffc0);
				transmask = 0x3f;
				break;
			case 0x0018: // mode 3 128 colour bank mode (8bits) (little characters on hanagumi use this mode)
				pix = gfxdata[patterndata+offsetcnt];
				pix = pix+(stv2_current_sprite.CMDCOLR&0xff80);
				transmask = 0x7f;
				mode = 3;
			//  pix = rand();
				break;
			case 0x0020: // mode 4 256 colour bank mode (8bits) (hanagumi title)
				pix = gfxdata[patterndata+offsetcnt];
				pix = pix+(stv2_current_sprite.CMDCOLR&0xff00);
				transmask = 0xff;
				mode = 4;
				break;
			case 0x0028: // mode 5 32,768 colour RGB mode (16bits)
				pix = gfxdata[patterndata+offsetcnt*2+1] | (gfxdata[patterndata+offsetcnt*2]<<8) ;
				mode = 5;
				transmask = 0x8000;
				break;
			default: // other settings illegal
				pix = rand();
				mode = 0;
				transmask = 0xff;
		}


		// preliminary end code disable support
		if ( ((stv2_current_sprite.CMDPMOD & 0x80) == 0) &&
			((pix & transmask) == transmask) )
		{
			return;
		}
	}

	/* MSBON */
	pix |= stv2_current_sprite.CMDPMOD & 0x8000;
	if ( mode != 5 )
	{
		if ( (pix & transmask) || spd )
		{
			*dest = pix;
		}
	}
	else
	{
		if ( (pix & transmask) || spd )
		{
			switch( stv2_current_sprite.CMDPMOD & 0x7 )
			{
				case 0:	/* replace */
					*dest = pix;
					break;
				case 1: /* shadow */
					if ( *dest & 0x8000 )
					{
						*dest = ((*dest & ~0x8421) >> 1) | 0x8000;
					}
					break;
				case 2: /* half luminance */
					*dest = ((pix & ~0x8421) >> 1) | 0x8000;
					break;
				case 3: /* half transparent */
					if ( *dest & 0x8000 )
					{
						*dest = alpha_blend_r16( *dest, pix, 0x80 ) | 0x8000;
					}
					else
					{
						*dest = pix;
					}
					break;
				case 4: /* Gouraud shading */
					*dest = stv_apply_gouraud_shading( pix );
					break;
				default:
					*dest = pix;
					//ui_popup( "Unsupported VDP1 draw mode %x", stv2_current_sprite.CMDPMOD & 0x7 );
					break;
			}
		}
	}

}


enum { FRAC_SHIFT = 16 };

struct spoint {
	INT32 x, y;
	INT32 u, v;
};

static void vdp1_fill_slope(mame_bitmap *bitmap, const rectangle *cliprect, int patterndata, int xsize,
							INT32 x1, INT32 x2, INT32 sl1, INT32 sl2, INT32 *nx1, INT32 *nx2,
							INT32 u1, INT32 u2, INT32 slu1, INT32 slu2, INT32 *nu1, INT32 *nu2,
							INT32 v1, INT32 v2, INT32 slv1, INT32 slv2, INT32 *nv1, INT32 *nv2,
							INT32 _y1, INT32 y2)
{
	if(_y1 > cliprect->max_y)
		return;

	if(y2 <= cliprect->min_y) {
		int delta = y2-_y1;
		*nx1 = x1+delta*sl1;
		*nu1 = u1+delta*slu1;
		*nv1 = v1+delta*slv1;
		*nx2 = x2+delta*sl2;
		*nu2 = u2+delta*slu2;
		*nv2 = v2+delta*slv2;
		return;
	}

	if(y2 > cliprect->max_y)
		y2 = cliprect->max_y+1;

	if(_y1 < cliprect->min_y) {
		int delta = cliprect->min_y - _y1;
		x1 += delta*sl1;
		u1 += delta*slu1;
		v1 += delta*slv1;
		x2 += delta*sl2;
		u2 += delta*slu2;
		v2 += delta*slv2;
		_y1 = cliprect->min_y;
	}

	if(x1 > x2 || (x1==x2 && sl1 > sl2)) {
		INT32 t, *tp;
		t = x1;
		x1 = x2;
		x2 = t;
		t = sl1;
		sl1 = sl2;
		sl2 = t;
		tp = nx1;
		nx1 = nx2;
		nx2 = tp;

		t = u1;
		u1 = u2;
		u2 = t;
		t = slu1;
		slu1 = slu2;
		slu2 = t;
		tp = nu1;
		nu1 = nu2;
		nu2 = tp;

		t = v1;
		v1 = v2;
		v2 = t;
		t = slv1;
		slv1 = slv2;
		slv2 = t;
		tp = nv1;
		nv1 = nv2;
		nv2 = tp;
	}

	while(_y1 < y2) {
		if(_y1 >= cliprect->min_y) {
			INT32 slux = 0, slvx = 0;
			int xx1 = x1>>FRAC_SHIFT;
			int xx2 = x2>>FRAC_SHIFT;
			INT32 u = u1;
			INT32 v = v1;
			if(xx1 != xx2) {
				int delta = xx2-xx1;
				slux = (u2-u1)/delta;
				slvx = (v2-v1)/delta;
			}
			if(xx1 <= cliprect->max_x || xx2 >= cliprect->min_x) {
				if(xx1 < cliprect->min_x) {
					int delta = cliprect->min_x-xx1;
					u += slux*delta;
					v += slvx*delta;
					xx1 = cliprect->min_x;
				}
				if(xx2 > cliprect->max_x)
					xx2 = cliprect->max_x;

				while(xx1 <= xx2) {
					drawpixel(stv_framebuffer_draw_lines[_y1]+xx1,
							  patterndata,
							  (v>>FRAC_SHIFT)*xsize+(u>>FRAC_SHIFT));
					xx1++;
					u += slux;
					v += slvx;
				}
			}
		}

		x1 += sl1;
		u1 += slu1;
		v1 += slv1;
		x2 += sl2;
		u2 += slu2;
		v2 += slv2;
		_y1++;
	}
	*nx1 = x1;
	*nu1 = u1;
	*nv1 = v1;
	*nx2 = x2;
	*nu2 = u2;
	*nv2 = v2;
}

static void vdp1_fill_line(mame_bitmap *bitmap, const rectangle *cliprect, int patterndata, int xsize, INT32 y,
						   INT32 x1, INT32 x2, INT32 u1, INT32 u2, INT32 v1, INT32 v2)
{
	int xx1 = x1>>FRAC_SHIFT;
	int xx2 = x2>>FRAC_SHIFT;

	if(y > cliprect->max_y || y < cliprect->min_y)
		return;

	if(xx1 <= cliprect->max_x || xx2 >= cliprect->min_x) {
		INT32 slux = 0, slvx = 0;
		INT32 u = u1;
		INT32 v = v1;
		if(xx1 != xx2) {
			int delta = xx2-xx1;
			slux = (u2-u1)/delta;
			slvx = (v2-v1)/delta;
		}
		if(xx1 < cliprect->min_x) {
			int delta = cliprect->min_x-xx1;
			u += slux*delta;
			v += slvx*delta;
			xx1 = cliprect->min_x;
		}
		if(xx2 > cliprect->max_x)
			xx2 = cliprect->max_x;

		while(xx1 <= xx2) {
			drawpixel(stv_framebuffer_draw_lines[y]+xx1,
					  patterndata,
					  (v>>FRAC_SHIFT)*xsize+(u>>FRAC_SHIFT));
			xx1++;
			u += slux;
			v += slvx;
		}
	}
}

static void vdp1_fill_quad(mame_bitmap *bitmap, const rectangle *cliprect, int patterndata, int xsize, const struct spoint *q)
{
	INT32 sl1, sl2, slu1, slu2, slv1, slv2, cury, limy, x1, x2, u1, u2, v1, v2, delta;
	int pmin, pmax, i, ps1, ps2;
	struct spoint p[8];

	for(i=0; i<4; i++) {
		p[i].x = p[i+4].x = q[i].x << FRAC_SHIFT;
		p[i].y = p[i+4].y = q[i].y;
		p[i].u = p[i+4].u = q[i].u << FRAC_SHIFT;
		p[i].v = p[i+4].v = q[i].v << FRAC_SHIFT;
	}

	pmin = pmax = 0;
	for(i=1; i<4; i++) {
		if(p[i].y < p[pmin].y)
			pmin = i;
		if(p[i].y > p[pmax].y)
			pmax = i;
	}

	cury = p[pmin].y;
	limy = p[pmax].y;

	if(cury == limy) {
		x1 = x2 = p[0].x;
		u1 = u2 = p[0].u;
		v1 = v2 = p[0].v;
		for(i=1; i<4; i++) {
			if(p[i].x < x1) {
				x1 = p[i].x;
				u1 = p[i].u;
				v1 = p[i].v;
			}
			if(p[i].x > x2) {
				x2 = p[i].x;
				u2 = p[i].u;
				v2 = p[i].v;
			}
		}
		vdp1_fill_line(bitmap, cliprect, patterndata, xsize, cury, x1, x2, u1, u2, v1, v2);
		return;
	}

	if(cury > cliprect->max_y)
		return;
	if(limy <= cliprect->min_y)
		return;

	if(limy > cliprect->max_y)
		limy = cliprect->max_y;

	ps1 = pmin+4;
	ps2 = pmin;

	goto startup;

	for(;;) {
		if(p[ps1-1].y == p[ps2+1].y) {
			vdp1_fill_slope(bitmap, cliprect, patterndata, xsize,
							x1, x2, sl1, sl2, &x1, &x2,
							u1, u2, slu1, slu2, &u1, &u2,
							v1, v2, slv1, slv2, &v1, &v2,
							cury, p[ps1-1].y);
			cury = p[ps1-1].y;
			if(cury >= limy)
				break;
			ps1--;
			ps2++;

		startup:
			while(p[ps1-1].y == cury)
				ps1--;
			while(p[ps2+1].y == cury)
				ps2++;
			x1 = p[ps1].x;
			u1 = p[ps1].u;
			v1 = p[ps1].v;
			x2 = p[ps2].x;
			u2 = p[ps2].u;
			v2 = p[ps2].v;

			delta = cury-p[ps1-1].y;
			sl1 = (x1-p[ps1-1].x)/delta;
			slu1 = (u1-p[ps1-1].u)/delta;
			slv1 = (v1-p[ps1-1].v)/delta;

			delta = cury-p[ps2+1].y;
			sl2 = (x2-p[ps2+1].x)/delta;
			slu2 = (u2-p[ps2+1].u)/delta;
			slv2 = (v2-p[ps2+1].v)/delta;
		} else if(p[ps1-1].y < p[ps2+1].y) {
			vdp1_fill_slope(bitmap, cliprect, patterndata, xsize,
							x1, x2, sl1, sl2, &x1, &x2,
							u1, u2, slu1, slu2, &u1, &u2,
							v1, v2, slv1, slv2, &v1, &v2,
							cury, p[ps1-1].y);
			cury = p[ps1-1].y;
			if(cury >= limy)
				break;
			ps1--;
			while(p[ps1-1].y == cury)
				ps1--;
			x1 = p[ps1].x;
			u1 = p[ps1].u;
			v1 = p[ps1].v;

			delta = cury-p[ps1-1].y;
			sl1 = (x1-p[ps1-1].x)/delta;
			slu1 = (u1-p[ps1-1].u)/delta;
			slv1 = (v1-p[ps1-1].v)/delta;
		} else {
			vdp1_fill_slope(bitmap, cliprect, patterndata, xsize,
							x1, x2, sl1, sl2, &x1, &x2,
							u1, u2, slu1, slu2, &u1, &u2,
							v1, v2, slv1, slv2, &v1, &v2,
							cury, p[ps2+1].y);
			cury = p[ps2+1].y;
			if(cury >= limy)
				break;
			ps2++;
			while(p[ps2+1].y == cury)
				ps2++;
			x2 = p[ps2].x;
			u2 = p[ps2].u;
			v2 = p[ps2].v;

			delta = cury-p[ps2+1].y;
			sl2 = (x2-p[ps2+1].x)/delta;
			slu2 = (u2-p[ps2+1].u)/delta;
			slv2 = (v2-p[ps2+1].v)/delta;
		}
	}
	if(cury == limy)
		vdp1_fill_line(bitmap, cliprect, patterndata, xsize, cury, x1, x2, u1, u2, v1, v2);
}

static int x2s(int v)
{
	/*int r = v & 0x7ff;
    if (r & 0x400)
        r -= 0x800;
    return r + stvvdp1_local_x;*/
	return (INT32)(INT16)v + stvvdp1_local_x;
}

static int y2s(int v)
{
	/*int r = v & 0x7ff;
    if (r & 0x400)
        r -= 0x800;
    return r + stvvdp1_local_y;*/
	return (INT32)(INT16)v + stvvdp1_local_y;
}

void stv_vdp1_draw_line( mame_bitmap *bitmap, const rectangle *cliprect)
{
	struct spoint q[4];

	q[0].x = x2s(stv2_current_sprite.CMDXA);
	q[0].y = y2s(stv2_current_sprite.CMDYA);
	q[1].x = x2s(stv2_current_sprite.CMDXB);
	q[1].y = y2s(stv2_current_sprite.CMDYB);
	q[2].x = x2s(stv2_current_sprite.CMDXA);
	q[2].y = y2s(stv2_current_sprite.CMDYA);
	q[3].x = x2s(stv2_current_sprite.CMDXB);
	q[3].y = y2s(stv2_current_sprite.CMDYB);

	q[0].u = q[3].u = q[1].u = q[2].u = 0;
	q[0].v = q[1].v = q[2].v = q[3].v = 0;

	vdp1_fill_quad(bitmap, cliprect, 0, 1, q);
}

void stv_vpd1_draw_distorted_sprite(mame_bitmap *bitmap, const rectangle *cliprect)
{
	struct spoint q[4];

	int xsize, ysize;
	int direction;
	int patterndata;

	direction = (stv2_current_sprite.CMDCTRL & 0x0030)>>4;

	if ( stv2_current_sprite.ispoly )
	{
		xsize = ysize = 1;
		patterndata = 0;
	}
	else
	{
		xsize = (stv2_current_sprite.CMDSIZE & 0x3f00) >> 8;
		xsize = xsize * 8;
		if (xsize == 0) return; /* setting prohibited */

		ysize = (stv2_current_sprite.CMDSIZE & 0x00ff);
		if (ysize == 0) return; /* setting prohibited */

		patterndata = (stv2_current_sprite.CMDSRCA) & 0xffff;
		patterndata = patterndata * 0x8;

	}


	q[0].x = x2s(stv2_current_sprite.CMDXA);
	q[0].y = y2s(stv2_current_sprite.CMDYA);
	q[1].x = x2s(stv2_current_sprite.CMDXB);
	q[1].y = y2s(stv2_current_sprite.CMDYB);
	q[2].x = x2s(stv2_current_sprite.CMDXC);
	q[2].y = y2s(stv2_current_sprite.CMDYC);
	q[3].x = x2s(stv2_current_sprite.CMDXD);
	q[3].y = y2s(stv2_current_sprite.CMDYD);

	if(direction & 1) { // xflip
		q[0].u = q[3].u = xsize-1;
		q[1].u = q[2].u = 0;
	} else {
		q[0].u = q[3].u = 0;
		q[1].u = q[2].u = xsize-1;
	}
	if(direction & 2) { // yflip
		q[0].v = q[1].v = ysize-1;
		q[2].v = q[3].v = 0;
	} else {
		q[0].v = q[1].v = 0;
		q[2].v = q[3].v = ysize-1;
	}

	vdp1_fill_quad(bitmap, cliprect, patterndata, xsize, q);
}

void stv_vpd1_draw_scaled_sprite(mame_bitmap *bitmap, const rectangle *cliprect)
{
	struct spoint q[4];

	int xsize, ysize;
	int direction;
	int patterndata;
	int zoompoint;
	int x,y;
	int x2,y2;
	int screen_width,screen_height,screen_height_negative = 0;

	direction = (stv2_current_sprite.CMDCTRL & 0x0030)>>4;

	xsize = (stv2_current_sprite.CMDSIZE & 0x3f00) >> 8;
	xsize = xsize * 8;

	ysize = (stv2_current_sprite.CMDSIZE & 0x00ff);

	patterndata = (stv2_current_sprite.CMDSRCA) & 0xffff;
	patterndata = patterndata * 0x8;

	zoompoint = (stv2_current_sprite.CMDCTRL & 0x0f00)>>8;

	x = stv2_current_sprite.CMDXA;
	y = stv2_current_sprite.CMDYA;

	screen_width = (INT16)stv2_current_sprite.CMDXB;
	if ( (screen_width < 0) && zoompoint)
	{
		screen_width = -screen_width;
		direction |= 1;
	}

	screen_height = (INT16)stv2_current_sprite.CMDYB;
	if ( (screen_height < 0) && zoompoint )
	{
		screen_height_negative = 1;
		screen_height = -screen_height;
		direction |= 2;
	}

	x2 = stv2_current_sprite.CMDXC; // second co-ordinate set x
	y2 = stv2_current_sprite.CMDYC; // second co-ordinate set y

	switch (zoompoint)
	{
		case 0x0: // specified co-ordinates
			break;
		case 0x5: // up left
			break;
		case 0x6: // up center
			x -= screen_width/2 ;
			break;
		case 0x7: // up right
			x -= screen_width;
			break;

		case 0x9: // center left
			y -= screen_height/2 ;
			break;
		case 0xa: // center center
			y -= screen_height/2 ;
			x -= screen_width/2 ;

			break;

		case 0xb: // center right
			y -= screen_height/2 ;
			x -= screen_width;
			break;

		case 0xd: // center left
			y -= screen_height;
			break;

		case 0xe: // center center
			y -= screen_height;
			x -= screen_width/2 ;
			break;

		case 0xf: // center right
			y -= screen_height;
			x -= screen_width;
			break;

		default: // illegal
			break;

	}

	/*  0----1
        |    |
        |    |
        3----2   */

	if (zoompoint)
	{
		q[0].x = x2s(x);
		q[0].y = y2s(y);
		q[1].x = x2s(x)+screen_width;
		q[1].y = y2s(y);
		q[2].x = x2s(x)+screen_width;
		q[2].y = y2s(y)+screen_height;
		q[3].x = x2s(x);
		q[3].y = y2s(y)+screen_height;

		if ( screen_height_negative )
		{
			q[0].y += screen_height;
			q[1].y += screen_height;
			q[2].y += screen_height;
			q[3].y += screen_height;
		}
	}
	else
	{
		q[0].x = x2s(x);
		q[0].y = y2s(y);
		q[1].x = x2s(x2);
		q[1].y = y2s(y);
		q[2].x = x2s(x2);
		q[2].y = y2s(y2);
		q[3].x = x2s(x);
		q[3].y = y2s(y2);
	}


	if(direction & 1) { // xflip
		q[0].u = q[3].u = xsize-1;
		q[1].u = q[2].u = 0;
	} else {
		q[0].u = q[3].u = 0;
		q[1].u = q[2].u = xsize-1;
	}
	if(direction & 2) { // yflip
		q[0].v = q[1].v = ysize-1;
		q[2].v = q[3].v = 0;
	} else {
		q[0].v = q[1].v = 0;
		q[2].v = q[3].v = ysize-1;
	}

	vdp1_fill_quad(bitmap, cliprect, patterndata, xsize, q);
}


void stv_vpd1_draw_normal_sprite(mame_bitmap *bitmap, const rectangle *cliprect, int sprite_type)
{
	UINT16 *destline;

	int y, ysize, ycnt, drawypos;
	int x, xsize, xcnt, drawxpos;
	int direction;
	int patterndata;
	UINT8 shading;

	shading = stv_read_gouraud_table();

	x = x2s(stv2_current_sprite.CMDXA);
	y = y2s(stv2_current_sprite.CMDYA);

	direction = (stv2_current_sprite.CMDCTRL & 0x0030)>>4;

	xsize = (stv2_current_sprite.CMDSIZE & 0x3f00) >> 8;
	xsize = xsize * 8;

	ysize = (stv2_current_sprite.CMDSIZE & 0x00ff);


	patterndata = (stv2_current_sprite.CMDSRCA) & 0xffff;
	patterndata = patterndata * 0x8;




	if (vdp1_sprite_log) loginfo (2, "Drawing Normal Sprite x %04x y %04x xsize %04x ysize %04x patterndata %06x\n",x,y,xsize,ysize,patterndata);

	if ( x > cliprect->max_x ) return;
	if ( y > cliprect->max_y ) return;

	if ( shading == 0 )
	{
		int su, u, dux, duy;
		int maxdrawypos, maxdrawxpos;

		u = 0;
		dux = 1;
		duy = xsize;
		if ( direction & 0x1 ) //xflip
		{
			dux = -1;
			u = xsize - 1;
		}
		if ( direction & 0x2 ) //yflip
		{
			duy = -xsize;
			u += xsize*(ysize-1);
		}
		if ( y < cliprect->min_y ) //clip y
		{
			u += xsize*(cliprect->min_y - y);
			ysize -= (cliprect->min_y - y);
			y = cliprect->min_y;
		}
		if ( x < cliprect->min_x ) //clip x
		{
			u += dux*(cliprect->min_x - x);
			xsize -= (cliprect->min_x - x);
			x = cliprect->min_x;
		}
		maxdrawypos = MIN(y+ysize-1,cliprect->max_y);
		maxdrawxpos = MIN(x+xsize-1,cliprect->max_x);
		for (drawypos = y; drawypos <= maxdrawypos; drawypos++ )
		{
			destline = stv_framebuffer_draw_lines[drawypos];
			su = u;
			for (drawxpos = x; drawxpos <= maxdrawxpos; drawxpos++ )
			{
				drawpixel( destline + drawxpos, patterndata, u );
				u += dux;
			}
			u = su + duy;
		}
	}
	else
	{
		for (ycnt = 0; ycnt != ysize; ycnt++) {

			stv_setup_shading_for_line( xsize,
					stv_compute_shading_at_pixel( ysize, ycnt, stv_gouraud_shading.GA, stv_gouraud_shading.GD ),
					stv_compute_shading_at_pixel( ysize, ycnt, stv_gouraud_shading.GB, stv_gouraud_shading.GC ));

			if (direction & 0x2) // 'yflip' (reverse direction)
			{
				drawypos = y+((ysize-1)-ycnt);
			}
			else
			{
				drawypos = y+ycnt;
			}

			if ((drawypos >= cliprect->min_y) && (drawypos <= cliprect->max_y))
			{
				destline = stv_framebuffer_draw_lines[drawypos];

				for (xcnt = 0; xcnt != xsize; xcnt ++)
				{
					if (direction & 0x1) // 'xflip' (reverse direction)
					{
						drawxpos = x+((xsize-1)-xcnt);
					}
					else
					{
						drawxpos = x+xcnt;
					}
					if ((drawxpos >= cliprect->min_x) && (drawxpos <= cliprect->max_x))
					{
						int offsetcnt;

						offsetcnt = ycnt*xsize+xcnt;

						drawpixel(destline+drawxpos, patterndata, offsetcnt);
					} // drawxpos
					stv_compute_shading_for_next_point();

				} // xcnt

			} // if drawypos

		} // ycny

		stv_clear_gouraud_shading();
	}
}

void stv_vdp1_process_list(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int position;
	int spritecount;
	int vdp1_nest;

	spritecount = 0;
	position = 0;

	if (vdp1_sprite_log) loginfo (2, "Sprite List Process START\n");

	vdp1_nest = -1;

	stv_clear_gouraud_shading();

	/*Set CEF bit to 0*/
	SET_CEF_FROM_1_TO_0;

	while (spritecount<10000) // if its drawn this many sprites something is probably wrong or sega were crazy ;-)
	{
		int draw_this_sprite;

		draw_this_sprite = 1;

	//  if (position >= ((0x80000/0x20)/4)) // safety check
	//  {
	//      if (vdp1_sprite_log) loginfo (2, "Sprite List Position Too High!\n");
	//      position = 0;
	//  }

		stv2_current_sprite.CMDCTRL = (stv_vdp1_vram[position * (0x20/4)+0] & 0xffff0000) >> 16;

		if (stv2_current_sprite.CMDCTRL == 0x8000)
		{
			if (vdp1_sprite_log) loginfo (2, "List Terminator (0x8000) Encountered, Sprite List Process END\n");
			goto end; // end of list
		}

		stv2_current_sprite.CMDLINK = (stv_vdp1_vram[position * (0x20/4)+0] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDPMOD = (stv_vdp1_vram[position * (0x20/4)+1] & 0xffff0000) >> 16;
		stv2_current_sprite.CMDCOLR = (stv_vdp1_vram[position * (0x20/4)+1] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDSRCA = (stv_vdp1_vram[position * (0x20/4)+2] & 0xffff0000) >> 16;
		stv2_current_sprite.CMDSIZE = (stv_vdp1_vram[position * (0x20/4)+2] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDXA   = (stv_vdp1_vram[position * (0x20/4)+3] & 0xffff0000) >> 16;
		stv2_current_sprite.CMDYA   = (stv_vdp1_vram[position * (0x20/4)+3] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDXB   = (stv_vdp1_vram[position * (0x20/4)+4] & 0xffff0000) >> 16;
		stv2_current_sprite.CMDYB   = (stv_vdp1_vram[position * (0x20/4)+4] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDXC   = (stv_vdp1_vram[position * (0x20/4)+5] & 0xffff0000) >> 16;
		stv2_current_sprite.CMDYC   = (stv_vdp1_vram[position * (0x20/4)+5] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDXD   = (stv_vdp1_vram[position * (0x20/4)+6] & 0xffff0000) >> 16;
		stv2_current_sprite.CMDYD   = (stv_vdp1_vram[position * (0x20/4)+6] & 0x0000ffff) >> 0;
		stv2_current_sprite.CMDGRDA = (stv_vdp1_vram[position * (0x20/4)+7] & 0xffff0000) >> 16;
//      stv2_current_sprite.UNUSED  = (stv_vdp1_vram[position * (0x20/4)+7] & 0x0000ffff) >> 0;

		/* proecess jump / skip commands, set position for next sprite */
		switch (stv2_current_sprite.CMDCTRL & 0x7000)
		{
			case 0x0000: // jump next
				if (vdp1_sprite_log) loginfo (2, "Sprite List Process + Next (Normal)\n");
				position++;
				break;
			case 0x1000: // jump assign
				if (vdp1_sprite_log) loginfo (2, "Sprite List Process + Jump Old %06x New %06x\n", position, (stv2_current_sprite.CMDLINK>>2));
				position= (stv2_current_sprite.CMDLINK>>2);
				break;
			case 0x2000: // jump call
				if (vdp1_nest == -1)
				{
					if (vdp1_sprite_log) loginfo (2, "Sprite List Process + Call Old %06x New %06x\n",position, (stv2_current_sprite.CMDLINK>>2));
					vdp1_nest = position+1;
					position = (stv2_current_sprite.CMDLINK>>2);
				}
				else
				{
					if (vdp1_sprite_log) loginfo (2, "Sprite List Nested Call, ignoring\n");
					position++;
				}
				break;
			case 0x3000:
				if (vdp1_nest != -1)
				{
					if (vdp1_sprite_log) loginfo (2, "Sprite List Process + Return\n");
					position = vdp1_nest;
					vdp1_nest = -1;
				}
				else
				{
					if (vdp1_sprite_log) loginfo (2, "Attempted return from no subroutine, aborting\n");
					position++;
					goto end; // end of list
				}
				break;
			case 0x4000:
				draw_this_sprite = 0;
				position++;
				break;
			case 0x5000:
				if (vdp1_sprite_log) loginfo (2, "Sprite List Skip + Jump Old %06x New %06x\n", position, (stv2_current_sprite.CMDLINK>>2));
				draw_this_sprite = 0;
				position= (stv2_current_sprite.CMDLINK>>2);

				break;
			case 0x6000:
				draw_this_sprite = 0;
				if (vdp1_nest == -1)
				{
					if (vdp1_sprite_log) loginfo (2, "Sprite List Skip + Call To Subroutine Old %06x New %06x\n",position, (stv2_current_sprite.CMDLINK>>2));

					vdp1_nest = position+1;
					position = (stv2_current_sprite.CMDLINK>>2);
				}
				else
				{
					if (vdp1_sprite_log) loginfo (2, "Sprite List Nested Call, ignoring\n");
					position++;
				}
				break;
			case 0x7000:
				draw_this_sprite = 0;
				if (vdp1_nest != -1)
				{
					if (vdp1_sprite_log) loginfo (2, "Sprite List Skip + Return from Subroutine\n");

					position = vdp1_nest;
					vdp1_nest = -1;
				}
				else
				{
					if (vdp1_sprite_log) loginfo (2, "Attempted return from no subroutine, aborting\n");
					position++;
					goto end; // end of list
				}
				break;
		}

		/* continue to draw this sprite only if the command wasn't to skip it */
		if (draw_this_sprite ==1)
		{
			switch (stv2_current_sprite.CMDCTRL & 0x000f)
			{
				case 0x0000:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Normal Sprite\n");
					stv2_current_sprite.ispoly = 0;
					stv_vpd1_draw_normal_sprite(bitmap,cliprect, 0);
					break;

				case 0x0001:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Scaled Sprite\n");
					stv2_current_sprite.ispoly = 0;
					stv_vpd1_draw_scaled_sprite(bitmap,cliprect);
					break;

				case 0x0002:
					if ( (stv2_current_sprite.CMDPMOD & 0x7) == 4 )
					{
						//turn off Gouraud shading atm
						stv2_current_sprite.CMDPMOD &= 0xfff8;
					}
					if (vdp1_sprite_log) loginfo (2, "Sprite List Distorted Sprite\n");
					stv2_current_sprite.ispoly = 0;
					stv_vpd1_draw_distorted_sprite(bitmap,cliprect);
					break;

				case 0x0004:
					if ( (stv2_current_sprite.CMDPMOD & 0x7) == 4 )
					{
						//turn off Gouraud shading atm
						stv2_current_sprite.CMDPMOD &= 0xfff8;
					}
					if (vdp1_sprite_log) loginfo (2, "Sprite List Polygon\n");
					stv2_current_sprite.ispoly = 1;
					stv_vpd1_draw_distorted_sprite(bitmap,cliprect);
					break;

				case 0x0005:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Polyline\n");
					break;

				case 0x0006:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Line\n");
					stv2_current_sprite.ispoly = 1;
					stv_vdp1_draw_line(bitmap,cliprect);
					break;

				case 0x0008:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Set Command for User Clipping\n");
					break;

				case 0x0009:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Set Command for System Clipping\n");
					break;

				case 0x000a:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Local Co-Ordinate Set\n");
					stvvdp1_local_x = (INT16)stv2_current_sprite.CMDXA;
					stvvdp1_local_y = (INT16)stv2_current_sprite.CMDYA;
					break;

				default:
					if (vdp1_sprite_log) loginfo (2, "Sprite List Illegal!\n");
					break;


			}


		}

		spritecount++;

	}


	end:
	/*set CEF to 1*/
	SET_CEF_FROM_0_TO_1;

	/* not here! this is done every frame drawn even if the cpu isn't running eg in the debugger */
//  if(!(stv_scu[40] & 0x2000)) /*Sprite draw end irq*/
//      cpunum_set_input_line_and_vector(0, 2, HOLD_LINE , 0x4d);

	if (vdp1_sprite_log) loginfo (2, "End of list processing!\n");
}

void video_update_vdp1(mame_bitmap *bitmap, const rectangle *cliprect)
{
	int framebufer_changed = 0;

//  int enable;
//  if (code_pressed (KEYCODE_R)) vdp1_sprite_log = 1;
//  if (code_pressed (KEYCODE_T)) vdp1_sprite_log = 0;

//  if (code_pressed (KEYCODE_Y)) vdp1_sprite_log = 0;
//  {
//      FILE *fp;
//
//      fp=fopen("vdp1_ram.dmp", "w+b");
//      if (fp)
//      {
//          fwrite(stv_vdp1, 0x00100000, 1, fp);
//          fclose(fp);
//      }
//  }
	if (vdp1_sprite_log) loginfo(2,"video_update_vdp1 called\n");
	if (vdp1_sprite_log) loginfo(2, "FBCR = %0x, accessed = %d\n", STV_VDP1_FBCR, stv_vdp1_fbcr_accessed );

	if ( stv_vdp1_clear_framebuffer_on_next_frame )
	{
		if ( ((STV_VDP1_FBCR & 0x3) == 3) &&
			stv_vdp1_fbcr_accessed )
		{
			stv_clear_framebuffer(stv_vdp1_current_display_framebuffer);
			stv_vdp1_clear_framebuffer_on_next_frame = 0;
		}
	}

	switch( STV_VDP1_FBCR & 0x3 )
	{
		case 0: /* Automatic mode */
			stv_vdp1_change_framebuffers();
			stv_clear_framebuffer(stv_vdp1_current_draw_framebuffer);
			framebufer_changed = 1;
			break;
		case 1: /* Setting prohibited */
			break;
		case 2: /* Manual mode - erase */
			if ( stv_vdp1_fbcr_accessed )
			{
				stv_vdp1_clear_framebuffer_on_next_frame = 1;
			}
			break;
		case 3: /* Manual mode - change */
			if ( stv_vdp1_fbcr_accessed )
			{
				stv_vdp1_change_framebuffers();
				if ( STV_VDP1_VBE )
				{
					stv_clear_framebuffer(stv_vdp1_current_draw_framebuffer);
				}
				framebufer_changed = 1;
			}
			break;
	}
	stv_vdp1_fbcr_accessed = 0;

	if (vdp1_sprite_log) loginfo(2, "PTM = %0x, TVM = %x\n", STV_VDP1_PTM, STV_VDP1_TVM );
	switch(STV_VDP1_PTM & 3)
	{
		case 0:/*Idle Mode*/
			break;
		case 1:/*Draw by request*/
			break;
		case 2:/*Automatic Draw*/
			if ( framebufer_changed )
				stv_vdp1_process_list(bitmap,cliprect);
			break;
		case 3:	/*<invalid>*/
			loginfo(2,"Warning: Invalid PTM mode set for VDP1!\n");
			break;
	}
	//ui_popup("%04x %04x",STV_VDP1_EWRR_X3,STV_VDP1_EWRR_Y3);
}
