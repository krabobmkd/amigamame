/*====================================================================*/
/*               TX-1/Buggy Boy  (Tatsumi) Hardware                   */
/*                      Philip J Bennett 2005                         */
/*                                                                    */
/*                         Video Emulation                            */
/*====================================================================*/

#include "driver.h"

extern UINT8 *buggyb1_vram;
extern UINT8 *buggyboy_vram;
extern UINT8 *bb_objram;
extern UINT8 *bb_sky;
extern UINT8 *bb_rcram;

extern tilemap *buggyb1_tilemap;
extern tilemap *buggyboy_tilemap;
extern size_t bb_objectram_size;
extern size_t bb_rcram_size;


extern UINT8 *tx1_vram;
extern UINT8 *tx1_object_ram;

extern tilemap *tx1_tilemap;
extern size_t tx1_objectram_size;

/*********/
/* TX-1 */
/********/

WRITE8_HANDLER( tx1_vram_w )
{
        if (tx1_vram[offset]!=data)
        {
        	tilemap_mark_tile_dirty(tx1_tilemap,offset/2);
        }
	tx1_vram[offset] = data;
}

static void get_tx1_tile_info(int tile_index)
{
	int bit15, upper, lower, tileno;

	tile_index <<= 1;
	bit15 = (tx1_vram[tile_index+1] & 0x80)<<6;
	upper = (tx1_vram[tile_index+1]&0x03)<<11;
	lower = (tx1_vram[tile_index]<<3);
	tileno = (bit15 | upper | lower)/8;

	SET_TILE_INFO(0,tileno,0,0)
}

VIDEO_START( tx1 )
{
	tx1_tilemap = tilemap_create(get_tx1_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,128,64);
	tilemap_set_transparent_pen(tx1_tilemap,0xff);
	return 0;
}

VIDEO_UPDATE( tx1 )
{

        tilemap_draw(bitmap,cliprect,tx1_tilemap,0,0);
}

/*************/
/* Buggy Boy */
/*************/

/***************************************************************************

  Convert the color PROMs into a more useable format.

  IC39, BB12 = Blue
  IC40, BB11 = Green
  IC41, BB10 = Red

  IC42, BB13 = Brightness

  bit 3 -- 220 ohm resistor  -- RED/GREEN/BLUE
        -- 470 ohm resistor  -- RED/GREEN/BLUE
        -- 1.0kohm resistor  -- RED/GREEN/BLUE
  bit 0 -- 2.2kohm resistor  -- RED/GREEN/BLUE

  bit 0 -- 4.7kohm resistor  -- BLUE
  bit 1 -- 4.7kohm resistor  -- GREEN
  bit 2 -- 4.7kohm resistor  -- RED

***************************************************************************/
PALETTE_INIT( buggyboy )
{
        int i;

	for (i = 0; i < 256;i++)
	{
		int bit0,bit1,bit2,bit3,bit4,r,g,b;

		bit0 = color_prom[i] & 1;
		bit1 = (color_prom[i] >> 1) & 1;
		bit2 = (color_prom[i] >> 2) & 1;
		bit3 = (color_prom[i] >> 3) & 1;
		bit4 = (color_prom[i+0x300] >> 2) & 1;
                r = 0x06 * bit4 + 0x0d * bit0 + 0x1e * bit1 + 0x41 * bit2 + 0x8a * bit3;

		bit0 = color_prom[i+0x100] & 1;
		bit1 = (color_prom[i+0x100] >> 1) & 1;
		bit2 = (color_prom[i+0x100] >> 2) & 1;
		bit3 = (color_prom[i+0x100] >> 3) & 1;
		bit4 = (color_prom[i+0x300] >> 1) & 1;
                g = 0x06 * bit4 + 0x0d * bit0 + 0x1e * bit1 + 0x41 * bit2 + 0x8a * bit3;

		bit0 = color_prom[i+0x200] & 1;
		bit1 = (color_prom[i+0x200] >> 1) & 1;
		bit2 = (color_prom[i+0x200] >> 2) & 1;
		bit3 = (color_prom[i+0x200] >> 3) & 1;
		bit4 = (color_prom[i+0x300]) & 1;
                b = 0x06 * bit4 + 0x0d * bit0 + 0x1e * bit1 + 0x41 * bit2 + 0x8a * bit3;

		palette_set_color(i,r,g,b);
	}


       /** Set up the colour lookups **/

       /* Objects use colours 0-63 */
       /* There are 2048 palette, however, this is expanded to 8192 */

	for (i = 0; i < 2048; i++)
		colortable[256+i] = ((0xf-color_prom[i + 0x500]) & 0xf) + 48;

	for (i = 0; i < 2048; i++)
	        colortable[256+2048+i] = ((0xf-color_prom[i + 0x500]) & 0xf) + 32;

	for (i = 0; i < 2048; i++)
        	colortable[256+2048+2048+i] = ((0xf-color_prom[i + 0x500]) & 0xf) + 16;

         /* Only this is used? */
	for (i = 0; i < 2048; i++)
		colortable[256+2048+2048+2048+i] = ((0xf-color_prom[i + 0x500]) & 0xf);

	/* Road uses 64-127 */
	/* Colour PROM only constitutes bits 0-3 - so expand 4-6*/
       	for (i = 0; i < 256; i++)
		colortable[256+8192+i] = (color_prom[i + 0x1500] & 0xf) + 64;

       	for (i = 0; i < 256; i++)
		colortable[256+8192+256+i] = (color_prom[i + 0x1500] & 0xf) + 64 + 16;

       	for (i = 0; i < 256; i++)
		colortable[256+8192+256*2+i] = (color_prom[i + 0x1500] & 0xf) + 64 + 32;

       	for (i = 0; i < 256; i++)
		colortable[256+8192+256*3+i] = (color_prom[i + 0x1500] & 0xf) + 64 + 48;

        /* Sky uses colours 128-191 directly - no lookup */

        /* Characters use colours 192-255 */
       	for (i = 0; i < 256; i++)
		colortable[i] = (color_prom[i + 0x400] & 0xf) + 192;

}


WRITE8_HANDLER( buggyb1_vram_w )
{
        if (buggyb1_vram[offset]!=data)
        {
        	tilemap_mark_tile_dirty(buggyb1_tilemap,offset/2);
        }
	buggyb1_vram[offset] = data;
}

WRITE8_HANDLER( buggyboy_vram_w )
{
        if (buggyboy_vram[offset]!=data)
        {
        	tilemap_mark_tile_dirty(buggyboy_tilemap,offset/2);
        }
	buggyboy_vram[offset] = data;
}


static void get_buggyb1_tile_info(int tile_index)
{
	int color, bit15, upper, lower, tileno;

	tile_index <<= 1;
	color = ((buggyb1_vram[tile_index+1] >>2) & 0x3f);
	bit15 = (buggyb1_vram[tile_index+1] & 0x80);
	upper = (buggyb1_vram[tile_index+1]&0x03)<<11;
	lower = (buggyb1_vram[tile_index]<<3);
	tileno = ((bit15 << 6) | upper | lower)/8;

	SET_TILE_INFO(0,tileno,color,0);
}

static void get_buggyboy_tile_info(int tile_index)
{
  	int color, bit15, upper, lower, tileno;

	tile_index <<= 1;
	color = ((buggyboy_vram[tile_index+1] >>2) & 0x3f);
	bit15 = (buggyboy_vram[tile_index+1] & 0x80)<<6;
	upper = (buggyboy_vram[tile_index+1]&0x03)<<11;
	lower = (buggyboy_vram[tile_index]<<3);
	tileno = (bit15 | upper | lower)/8;

	SET_TILE_INFO(0,tileno,color,0)
}


/*

 Applies to both versions of Buggy Boy

 Each object entry occupies 16 bytes:

 Byte 0: Sprite code
 Byte 1: Y-Position (bit 15 has some significance)

 Byte 2: Scale
 Byte 3: Scale

 Byte 4: Scale     0=Tiny  0x7f=Normal   0xff=Huge   ?
 Byte 5: Bit 7 = X-Flip, Bit 4 = chunk bank, Bit 5-6 = palette select, Bit 0-1 = palette related (PC_TMP)

 Byte 6: Scale
 Byte 7: Scale

 Byte 8: X position bits 0-7
 Byte 9: X position bits 8-9

 Remaining bytes are unusued.

*/

/*
   Object scale and chunk end-of-data handling, derived from the completed
   MAME 0.144 tx1 driver's buggyboy_draw_objs (a from-scratch per-pixel
   rasterizer with fixed-point X/Y accumulators). Here we keep 0.106's
   existing per-8x8-chunk drawgfxzoom approach and just feed it correctly
   computed values instead of the old hardcoded "no zoom, always 16 rows":

   - x_scale (object RAM byte 4) and y_step (word at bytes 6-7) are fed
     through drawgfxzoom's scalex/scaley (MAME convention: 0x10000 = 1:1).
     x_scale=0x80 is documented as "Normal" (1:1) and 0xff as "Huge" (~2x),
     which pins down scalex = x_scale << 9 (0x80<<9 == 0x10000). y_step
     is the 0.144 accumulator's per-scanline step in the same fixed-point
     units (0x100 == one source row per scanline == 1:1), which pins down
     scaley = 0x1000000 / y_step.
   - 0.144's bug13 ROM lookup returns 0xff to mark "reached the bottom of
     the object" per chunk-row, and bug17s (ROM_LUTB here) bit 0x40 marks
     end-of-row horizontally. 0.106 never checked either and always drew
     a fixed 16x16 chunk grid, which is what caused sprites to sometimes
     render as if the next unrelated object in the LUT was appended below
     the correct one.
*/
static void bb_draw_objects(mame_bitmap *bitmap,const rectangle *cliprect)
{
        int offs;

        UINT8 PROM_lookup;
        UINT16 ROM_lookup;

        UINT8 *rom_lut  = (UINT8 *)memory_region(REGION_USER3);             /* Object index ROM */
        UINT8 *prom_lut = (UINT8 *)memory_region(REGION_PROMS)+0x1600;      /* Object index PROM */

        UINT8 *ROM_LUTA = (UINT8 *)memory_region(REGION_USER2);             /* Object LUT (lower byte) */
        UINT8 *ROM_LUTB = (UINT8 *)memory_region(REGION_USER2)+0x8000;      /* Object LUT (lower byte) */
        UINT8 *ROM_CLUT = (UINT8 *)memory_region(REGION_USER3)+0x2000;      /* Object palette LUT */

	for (offs = 0x0; offs < (bb_objectram_size); offs += 16)
	{
		int inc,last;
		int bit_12, PSA0_12, PSA, object_flip_x;
		int index_y,index_x,index;
		int code, x_scale, y_step, scalex, scaley, x_spacing, y_spacing;
		int x_start, y_start;


		  if(bb_objram[offs+1] == 0xff)   /* End of object list marker? */
                return;

          code = bb_objram[offs];

          x_scale = bb_objram[offs+4];
          if (x_scale == 0)             /* Not observed on hardware - just don't draw */
               continue;

          y_step = bb_objram[offs+6] | (bb_objram[offs+7]<<8);
          if (y_step == 0)
               y_step = 1;

          scalex = x_scale << 9;
          scaley = (int)(0x1000000 / (unsigned)y_step);

          x_spacing = (8 * scalex) >> 16;
          y_spacing = (8 * scaley) >> 16;
          if (x_spacing < 1) x_spacing = 1;
          if (y_spacing < 1) y_spacing = 1;

          /* Only 10 bits of X are wired up (byte8 + low 2 bits of byte9) - mask
             off the rest, matching 0.144's "x & x_mask". Without this, a stray
             high bit could shift an object by exactly 128px or more. */
          x_start = ((bb_objram[offs+8])+(bb_objram[offs+9]<<8)) & 0x3ff;
          y_start = bb_objram[offs+1];

          /* The object code is fed into ROM and PROM to generate a lookup into a pair of ROMs */
          PROM_lookup = bb_objram[offs];
	  ROM_lookup = (bb_objram[offs] << 4)  | ((bb_objram[offs+3]>>3) & 0xf);

	  if(rom_lut[ROM_lookup] == 0xff) /* Do not draw object  */
           	continue;


          /* Calculate 13-bit index into object lookup ROMs that holds 8x8 chunk sequence */
          bit_12 = (((bb_objram[offs]>>7)&0x1) | ((bb_objram[offs]>>6)&0x1)) <<12;
          PSA0_12 = ( ( (prom_lut[PROM_lookup]&0xf)<<8) | rom_lut[ROM_lookup] | bit_12) & 0x1fff;

          PSA = (PSA0_12 << 2);

          object_flip_x = (bb_objram[offs+5]>>7)&0x1;

          for (index_y=0; index_y<16; index_y++)
          {
                int dataend = 0;
                int row = (ROM_lookup + index_y) & 0xf;    /* ROM_lookup's low nibble is the object's starting chunk-row */

                /* bug13/rom_lut signals "reached bottom of object" with 0xff */
                if (rom_lut[(code<<4) | row] == 0xff)
                     break;

                /* Each chunk-row occupies a fixed 16 slots in the LUT ROMs -
                   re-seed here rather than letting it carry over from the
                   previous row, since that row may have ended early below. */
                index = index_y * 16;

                if (object_flip_x)
                {
                    index_x=16;
                    inc=-1;
                    last=0;
                }
                else
                {
                    index_x=0;
                    inc=1;
                    last=16;
                }

                while(index_x!=last)
                {
                        /* Bit 14 of chunk_number = data_end, related to end of line */
		        int chunk_number = (ROM_LUTB[PSA+index]<<8) | ROM_LUTA[PSA+index];    // PSBB0-15
		        int this_end = ROM_LUTB[PSA+index] & 0x40;

	        	int sx = x_start+(index_x*x_spacing);
			int sy = y_start+(index_y*y_spacing);


                        /* Calculate the 14-bit CLUT ROM address */

                        int bit13 = (bb_objram[offs+5] & 0x10) << 9;
                        int bit12 = (chunk_number & 0x2000) >> 1;

                        /* Tile Number bit 12 -> 1 = Bits 6-7 of BUG16s or bits 8-9 of PC_TMP */
                        int bits6_and_7 = (chunk_number & 0x1000 ? chunk_number : bb_objram[offs+5] << 6) & 0xc0;
			int CLUT_ROM_ADDR = (chunk_number & 0xf3f) + bits6_and_7 + bit12 + bit13;

                        /* Now form the 12 bit OPCD */
                        int bits10_and_11 = 0xc00 - ((bb_objram[offs+5] << 8) & 0xc00);
                        int OPCS = (bb_objram[offs+5] & 0x60) << 3;                     // bits 8 and 9
                        int OPCD = (ROM_CLUT[CLUT_ROM_ADDR] + OPCS + bits10_and_11) & 0xfff;

                        int tmp = (OPCD&0x7f);      // bits 0-6
                        int tmp2= (OPCD&0x300)>>1;  // bits 9,8  (bit 7 is not there)
                        int tmp3 = bits10_and_11 >> 1;

                        int color = (tmp + tmp2 + tmp3);
                        int trans;


                        /* 8x8 chunk ROM bank: raw value is 2 bits (0-3), only banks
                           0-2 have real ROM data (Machine->gfx[1..3] via REGION_GFX2-4)
                           - real hardware wires bank 3 to unpopulated/zero ROM space,
                           which 0.144 reads as blank. We don't have a blank gfx_element
                           to point at, so just skip drawing that tile instead of
                           reading Machine->gfx[4] (out of bounds -> garbage rectangle). */
                        int bank_raw = ((bb_objram[offs+5]>>3)&0x2) | ((chunk_number>>13)&0x1);
			int flipx = ((chunk_number>>15) & 0x1) ^ object_flip_x;
			int flipy = 0;

                        if(!(OPCD & 0x80))  /* Seems to work! */
                             trans = TRANSPARENCY_PEN;
                        else
                             trans = TRANSPARENCY_NONE;

                        index_x+=inc;
                	index++;

                        if (bank_raw != 3)
    {
            const gfx_element *gfx = Machine->gfx[bank_raw+1];
            struct drawgfxParams params;
            params.dest = bitmap;
            params.gfx = gfx;
            params.code = chunk_number;
            params.color = color;
            params.flipx = flipx;
            params.flipy = flipy;
            params.sx = sx;
            params.sy = sy;
            params.clip = cliprect;
            params.transparency = trans;
            params.transparent_color = 0;
            params.scalex = scalex;
            params.scaley = scaley;
            params.pri_buffer = NULL;
            params.priority_mask = 0;

            drawgfxzoom(&params);
            }

                        /* bug17s/ROM_LUTB end-of-row marker: draw one more tile past
                           the first occurrence, then stop (matches 0.144's dataend/lasttile) */
                        if (this_end && dataend)
                             break;
                        dataend |= this_end;
                }
           }
    }
}


/*===================================================================*/
/*                        Buggy Boy Road Hardware                    */
/*                                                                    */
/*  Ported from MAME 0.144's buggybjr_draw_road/buggyboy_get_roadpix, */
/*  which is a transcription of the real PAL/counter-chip logic on   */
/*  the road board (three TZ1113 accumulators driving camber, scale  */
/*  and speed). Kept as close to the reference as possible - this is */
/*  gate-level logic, not an algorithm we can re-derive from first   */
/*  principles, so faithfulness to the original matters more than    */
/*  usual here.                                                      */
/*                                                                    */
/*  0.106 has no intermediate-bitmap/layer-combine system, so instead */
/*  of writing a raw "0x40|..." layer value and combining it later,   */
/*  we write pens directly: the reference's combine step for the road */
/*  layer reduces to plain pen = 0x40 | (wave?0:0x20) | (dirt?0x10:0) */
/*  | rcsd0_3 (range 0x40-0x7f) - which is exactly what 0.106's own   */
/*  untouched PALETTE_INIT(buggyboy) already built for "colours       */
/*  64-127" (its four colortable[256+8192+...] blocks are the same    */
/*  four wave/dirt bit combinations). So we can plot_pixel() straight */
/*  into the final bitmap, same as draw_sky() already does, with no   */
/*  new buffers needed.                                              */
/*===================================================================*/

typedef struct
{
	UINT16	scol;		/* Road colours */
	UINT8	flags;		/* Road flags */

	UINT32	ba_val;		/* Banking/camber accumulator */
	UINT32	ba_inc;
	UINT32	bank_mode;

	UINT16	h_val;		/* Road speed/position accumulator */
	UINT16	h_inc;

	UINT8	slin_val;	/* Starting-line accumulator */
	UINT8	slin_inc;

	UINT8	wa8;
	UINT8	wa4;

	UINT16	wave_lfsr;
	UINT16	gas;
	UINT8	shift;
} bb_vregs_t;

static bb_vregs_t bb_vregs;

#define BB_RDFLAG_WAVE1		7
#define BB_RDFLAG_WAVE0		6
#define BB_RDFLAG_TNLMD1	5
#define BB_RDFLAG_TNLMD0	4
#define BB_RDFLAG_TNLF		3
#define BB_RDFLAG_LINF		2
#define BB_RDFLAG_RVA7		1
#define BB_RDFLAG_WANGL		0

/* Road/common RAM (bb_rcram) is addressed here in the same byte-offset
   terms the reference uses for its UINT16* (word-index<<1 == this byte
   offset, since every offset used below is already even) */
static UINT16 rcram_word(int byte_off)
{
	return bb_rcram[byte_off & (bb_rcram_size-1)] | (bb_rcram[(byte_off+1) & (bb_rcram_size-1)] << 8);
}

static void buggyboy_get_roadpix(int ls161, UINT8 rva0_6, UINT8 sld, UINT32 *_rorev,
				  UINT8 *rc0, UINT8 *rc1, UINT8 *rc2, UINT8 *rc3,
				  const UINT8 *rom, const UINT8 *prom0, const UINT8 *prom1, const UINT8 *prom2)
{
	/* Counter Q10-7 are added to 384 (screen=1: centre screen only, matches buggybjr) */
	UINT16 ls283_159 = (ls161 & 0x780) + 128 + 256;
	UINT32 ls283_159_co = ls283_159 & 0x800;
	UINT32 rom_flip = ls283_159 & 0x200 ? 0 : 1;
	UINT32 rom_en = !(ls283_159 & 0x400) && !(ls283_159_co ^ (ls161 & 0x800));
	UINT8 d0 = 0;
	UINT8 d1 = 0;

	*_rorev = !( (rom_en && rom_flip) || (!rom_en && (ls161 & 0x4000)) );

	if (rom_en)
	{
		UINT8  rom_data;
		UINT16 prom_addr;

		UINT16 rha = (ls283_159 & 0x180) | (ls161 & 0x78);

		if (rom_flip)
			rha ^= 0x1f8;

		rom_data = rom[(1 << 13) | (rha << 4) | rva0_6];
		prom_addr = (rom_flip ? 0x80 : 0) | (rom_data & 0x7f);

		*rc0 = prom0[prom_addr];
		*rc1 = prom1[prom_addr];
		*rc2 = prom2[prom_addr];

		rom_data = rom[(rha << 4) | rva0_6];
		prom_addr = 0x100 | rom_data;

		d0 = prom0[prom_addr];
		d1 = prom1[prom_addr];
	}
	else
	{
		*rc0 = *rc1 = *rc2 = *rc3 = 0;
	}

	if (BIT(sld, 4))
	{
		if (BIT(sld, 5))
			d1 = ~d1;

		*rc3 = d0 & d1;

		if (rom_flip)
			*rc3 = BITSWAP8(*rc3, 0, 1, 2, 3, 4, 5, 6, 7);
	}
	else
		*rc3 = 0;
}

#define LOAD_HPOS_COUNTER(NUM)													\
	ram_val = rcram_word(rva_offs + 0x1f8 + (2*NUM));							\
	rcrs10 = ram_val & 0xfc00 ? 0x0400 : 0x0000;								\
	hp = bb_vregs.wa8 + ((BIT(ram_val, 15) << 11) | rcrs10 | (ram_val & 0x03ff));	\
	hp##NUM = hp & 0xff;														\
	hp >>= 8;																	\
	hps##NUM##0 = (BIT(hp, 0) || BIT(hp, 2)) && !BIT(hp, 3);					\
	hps##NUM##1 = (BIT(hp, 1) || BIT(hp, 2)) && !BIT(hp, 3);					\
	hps##NUM##2 = BIT(hp, 2);

#define UPDATE_HPOS(NUM)				\
	if (hp##NUM##_en)					\
	{									\
		if ((hp##NUM & 0xff) == 0xff)	\
			hp##NUM##_cy = 1;			\
		else							\
			hp##NUM = hp##NUM + 1;		\
	}

static void bb_draw_road(mame_bitmap *bitmap)
{
	INT32 x;
	UINT32 y;
	UINT16 rva_offs;
	UINT32 tnlmd0, tnlmd1, linf, tnlf, wangl, tcmd, wave0, wave1, rva20_6;

	const UINT8 *rcols = (const UINT8 *)memory_region(REGION_PROMS) + 0x1500;
	const UINT8 *rom   = (const UINT8 *)memory_region(REGION_GFX6);
	const UINT8 *prom0 = rom + 0x4000;
	const UINT8 *prom1 = rom + 0x4200;
	const UINT8 *prom2 = rom + 0x4400;
	const UINT8 *vprom = rom + 0x4600;

	/* Once-per-frame accumulator updates (real hardware does this at
	   /VSYNC; we only draw once per frame here too, so it's equivalent
	   whether done at the top or bottom of the frame) */
	bb_vregs.slin_val += bb_vregs.slin_inc;
	bb_vregs.wave_lfsr = 0;

	tcmd	 = ((bb_vregs.scol & 0xc000) >> 12) | ((bb_vregs.scol & 0x00c0) >> 6);
	tnlmd0   = BIT(bb_vregs.flags, BB_RDFLAG_TNLMD0);
	tnlmd1   = BIT(bb_vregs.flags, BB_RDFLAG_TNLMD1);
	linf     = BIT(bb_vregs.flags, BB_RDFLAG_LINF);
	tnlf     = BIT(bb_vregs.flags, BB_RDFLAG_TNLF);
	wangl    = BIT(bb_vregs.flags, BB_RDFLAG_WANGL);
	wave0    = BIT(bb_vregs.flags, BB_RDFLAG_WAVE0);
	wave1    = BIT(bb_vregs.flags, BB_RDFLAG_WAVE1);
	rva_offs = BIT(bb_vregs.flags, BB_RDFLAG_RVA7) ? 0x800 : 0xc00;

	for (y = 0; y < 240; ++y)
	{
		UINT8	rva0_6;
		UINT8	ram_addr;
		UINT16	rcrdb0_15;
		UINT16	rcrs10;
		UINT16	ls161_156_a;
		UINT16	ls161;
		UINT8	sld;
		UINT32	rva8;
		UINT32	rm0, rm1;
		UINT32	rcmd;
		UINT32	bnkcs = 1;
		UINT8	sf;

		UINT32	ram_val;
		UINT32	hp;
		UINT32	vp1, vp2, vp3, vp4, vp5, vp6, vp7;

		UINT32	ic4_o12, ic4_o13, ic149_o15, ic151_o14;

		UINT32	hp0, hp1, hp2, hp3;
		UINT8	hps00, hps01, hps02;
		UINT8	hps10, hps11, hps12;
		UINT8	hps20, hps21, hps22;
		UINT8	hps30, hps31, hps32;

		UINT8	rc0 = 0, rc1 = 0, rc2 = 0, rc3 = 0;

		UINT8	hp0_cy = 0, hp1_cy = 0, hp2_cy = 0, hp3_cy = 0;

		UINT32	bank_cnt;
		UINT32	_rorevcs = 0;

		rva8 = (bb_vregs.h_val & 0x8000) || !(bb_vregs.shift & 0x80);
		rva0_6 = (bb_vregs.h_val >> 7) & 0x7f;
		rva20_6 = ((rva0_6 >> 3) & 0xe) | ((rva0_6 & 2) >> 1);
		ram_addr = (~rva0_6 & 0x7f) << 1;

		rcrdb0_15 = rcram_word(rva_offs + ram_addr);

		rcrs10 = rcrdb0_15 & 0xfc00 ? 0x0400 : 0x0000;
		ls161_156_a = (rcrdb0_15 & 0xfc00) == 0xfc00 ? 0x800 : 0x0000;
		ls161 =  ((rcrdb0_15 & 0x8000) >> 1) | ls161_156_a | rcrs10 | (rcrdb0_15 & 0x03ff);

		sld = (vprom[rva0_6] + bb_vregs.slin_val) & 0x38;

		vp1 = rcram_word(rva_offs + 0x1e2) >= y ? 0 : 1;
		vp2 = rcram_word(rva_offs + 0x1e4) >= y ? 0 : 1;
		vp3 = rcram_word(rva_offs + 0x1e6) >= y ? 0 : 1;
		vp4 = rcram_word(rva_offs + 0x1e8) >= y ? 0 : 1;
		vp5 = rcram_word(rva_offs + 0x1ea) >= y ? 0 : 1;
		vp6 = rcram_word(rva_offs + 0x1ec) >= y ? 0 : 1;
		vp7 = rcram_word(rva_offs + 0x1ee) >= y ? 0 : 1;

		rm0 = vp7 ? BIT(bb_vregs.scol, 4) : BIT(bb_vregs.scol, 12);
		rm1 = vp7 ? BIT(bb_vregs.scol, 5) : BIT(bb_vregs.scol, 13);
		rcmd = (vp7 ? bb_vregs.scol : bb_vregs.scol >> 8) & 0xf;

		LOAD_HPOS_COUNTER(0);
		LOAD_HPOS_COUNTER(1);
		LOAD_HPOS_COUNTER(2);
		LOAD_HPOS_COUNTER(3);

		ic4_o12 = (!vp1 && !vp2 && !vp6) || (!vp1 && !vp2 && vp7) || (vp4 && !vp6) || (vp4 && vp7);
		ic4_o13 = (!vp1 && !vp2 && !vp5) || (!vp1 && !vp2 && vp7) || (vp3 && !vp5) || (vp3 && vp7);
		ic149_o15 = (!vp5 && !vp6) || vp7 || !linf;
		ic151_o14 = !BIT(sld, 3) || tnlmd0 || tnlmd1 || ic149_o15;

		bank_cnt = (bb_vregs.ba_val >> 5) & 0x3ff;

		if (ls161 & 7)
			buggyboy_get_roadpix(ls161, rva0_6, sld, &_rorevcs, &rc0, &rc1, &rc2, &rc3, rom, prom0, prom1, prom2);

		for (x = 0; x < 256; ++x)
		{
			UINT32	pix;
			UINT32	hp0_en, hp1_en, hp2_en, hp3_en;
			UINT32	ic149_o16;
			UINT32	ic4_o18;
			UINT32	ic3_o15;
			UINT32	ic150_o12 = 0;
			UINT32	ic150_o16;
			UINT32	ic150_o17;
			UINT32	ic150_o18;
			UINT32	ic150_o19;
			UINT32	ic151_o15;
			UINT32	ic151_o16;
			UINT32	ic151_o17;
			UINT32	rcsd0_3 = 0;
			UINT32	sld5 = BIT(sld, 5);
			UINT32	sld4 = BIT(sld, 4);
			UINT32	mux;
			UINT32	cprom_addr;
			UINT8	px0, px1, px2, px3;

			pix = (ls161 & 7) ^ 7;

			hp0_en = !(hp0_cy || hps02);
			hp1_en = !(hp1_cy || hps12);
			hp2_en = !(hp2_cy || hps22);
			hp3_en = !(hp3_cy || hps32);

			if (!(ls161 & 7))
				buggyboy_get_roadpix(ls161, rva0_6, sld, &_rorevcs, &rc0, &rc1, &rc2, &rc3, rom, prom0, prom1, prom2);

			if (bb_vregs.bank_mode == 0)
			{
				if (BIT(bb_vregs.ba_val, 23))
					bnkcs = 1;
				else if (bb_vregs.ba_val & 0x007f8000)
					bnkcs = 0;
				else
					bnkcs = bank_cnt < 0x300;
			}
			else
			{
				if (BIT(bb_vregs.ba_val, 23))
					bnkcs = 0;
				else if (bb_vregs.ba_val & 0x007f8000)
					bnkcs = 1;
				else
					bnkcs = bank_cnt >= 0x300;
			}

			px0 = BIT(rc0, pix);
			px1 = BIT(rc1, pix);
			px2 = BIT(rc2, pix);
			px3 = BIT(rc3, pix);

			if (vp2)
				ic4_o18 = (hps00 && hps01 && hp3_en && !hps30)		||
					  (!hp0_en && hps01 && hp3_en && !hps30)	||
					  (hps00 && hps01 && !hps31)			||
					  (!hp0_en && hps01 && !hps31)			||
					  vp7;
			else
				ic4_o18 = !vp1;

			if (tnlf)
				ic3_o15 = (vp4 && !vp6 && !hp2_en && hps21)		||
					  (vp4 && !vp6 && hps20 && hps21)		||
					  (vp1 && !vp4 && !tnlmd1 && !tnlmd0)		||
					  (vp1 && !vp3 && !tnlmd1 && !tnlmd0)		||
					  (hp1_en && !hps10 && vp3 && !vp5)		||
					  (!hps11 && vp3 && !vp5);
			else
				ic3_o15 = !ic4_o18;

			ic151_o17 = (_rorevcs && !tnlmd1 && tnlmd0)		||
				    (!_rorevcs && tnlmd1 && !tnlmd0)		||
				    (_rorevcs && ic4_o12)			||
				    (!_rorevcs && ic4_o13);

			if (!ic3_o15)
				ic151_o15 = (px0 && (bnkcs && wangl))	||
					    (px1 && (bnkcs && wangl))	||
					    ic151_o17			||
					    px2				||
					    !tnlf;
			else
				ic151_o15 = !tnlf;

			ic151_o16 = (px1 && !px0 && tnlmd1 && !tnlmd0)	||
				    (px2 && tnlmd1 && tnlmd0)		||
				    ic149_o15;

			mux = BIT(tcmd, 3) ? ic149_o15 : ic151_o16;

			ic150_o19 = (px2 && !rva8)	||
				    !bnkcs			||
				    !mux			||
				    !ic151_o15;

			if (ic150_o19)
			{
				UINT32 pen;

				ic149_o16 = (_rorevcs && !px2 && ic151_o15)								||
					    (tnlf && vp5 && !vp7 && px2 && !tnlmd0 && !tnlmd1 && ic151_o15)		||
					    (tnlf && vp6 && !vp7 && px2 && !tnlmd0 && !tnlmd1 && ic151_o15)		||
					    (tnlf && !ic4_o18);

				ic150_o16 = (px2 && mux && rm1)		||
					    (mux && rva8 && ic151_o15)		||
					    (!px0 && mux)			||
					    !ic151_o15;

				{
					UINT32 a = mux && ic151_o15;

					ic150_o17 = (a && !rm0 && px0)	||
						    (a && !px1)		||
						    (rva8 && a);

					ic150_o18 = (a && !px2) ||
						    (rva8 && a);
				}

				if (ic151_o14)
					ic150_o12 = rva8 || !mux || !ic151_o15			||
						    (px2 && px1 && px0 && rm1 && !rm0)		||
						    (!px2 && px1 && px0 && !sld4 && rm0)	||
						    (px2 && px0 && !sld5 && !rm1 && !rm0)	||
						    (px2 && !px1 && px0 && !sld5 && !rm1)	||
						    (px2 && px1 && px0 && !sld5 && !sld4)	||
						    (px2 && px1 && px0 && !sld4 && rm1)	||
						    (!px2 && !px3 && !rm0)			||
						    (!px1 && !px3 && rm1)			||
						    (!px2 && !px1 && !px3)			||
						    (!px0 && !px3);
				else
					ic150_o12 = 0;

				if (vp6 || ic151_o16)
				{
					UINT32 ic150_i5 = BIT(tcmd, 3) ? ic149_o15 : ic151_o16;

					if (!(ic151_o15 && ic150_i5))
						cprom_addr = (tcmd & 0x7) | (ic151_o16 ? 0x08 : 0);
					else
						cprom_addr = rcmd;

					cprom_addr = ((~cprom_addr) & 0xf) << 4;
				}
				else
					cprom_addr = 0xf0;

				cprom_addr |= (ic149_o16 ? 0x8 : 0) |
					      (ic150_o18 ? 0x4 : 0) |
					      (ic150_o17 ? 0x2 : 0) |
					      (ic150_o16 ? 0x1 : 0);

				rcsd0_3 = rcols[cprom_addr] & 0xf;

				{
					UINT32 lfsr = bb_vregs.wave_lfsr;
					UINT32 wave =
						(wave0 ^ BIT(lfsr, 0))	&&
						(wave1 ^ BIT(lfsr, 3))	&&
						BIT(lfsr, 5)		&&
						!BIT(lfsr, 15)		&&
						BIT(lfsr, 11)		&&
						BIT(lfsr, 13)		&&
						(rva20_6 < ((lfsr >> 8) & 0xf));

					pen = 0x40 | (wave ? 0 : 0x20) | (ic150_o12 ? 0x10 : 0) | rcsd0_3;
					plot_pixel(bitmap, x, y, Machine->pens[pen]);
				}
			}
			/* else: no road pixel here - leave whatever was drawn underneath (sky) */

			UPDATE_HPOS(0);
			UPDATE_HPOS(1);
			UPDATE_HPOS(2);
			UPDATE_HPOS(3);

			bb_vregs.wave_lfsr = (bb_vregs.wave_lfsr << 1) | (BIT(bb_vregs.wave_lfsr, 6) ^ !BIT(bb_vregs.wave_lfsr, 15));

			bank_cnt = (bank_cnt + 1) & 0x7ff;
			ls161 = (ls161 + 1) & 0x7fff;
		}

		if (wangl)
		{
			if (BIT(bb_vregs.flags, BB_RDFLAG_TNLMD0))
				--bb_vregs.wa8;
			else
				++bb_vregs.wa8;
		}

		if (bb_vregs.wa4 != 0xf)
			++bb_vregs.wa4;
		else
		{
			if (wangl)
			{
				if (BIT(bb_vregs.flags, BB_RDFLAG_TNLMD0))
					--bb_vregs.wa8;
				else
					++bb_vregs.wa8;
			}
			bb_vregs.wa4 = 1;
		}

		bb_vregs.h_val += bb_vregs.h_inc;

		sf = bb_vregs.shift;

		if ((bb_vregs.shift & 0x80) == 0)
		{
			bb_vregs.shift <<= 1;

			if ((sf & 0x08) == 0)
				bb_vregs.shift |= BIT(bb_vregs.h_val, 15);
		}

		if ((sf & 0x08) && !(bb_vregs.shift & 0x08))
			bb_vregs.h_inc = bb_vregs.gas;

		bb_vregs.ba_val = (bb_vregs.ba_val + bb_vregs.ba_inc) & 0x00ffffff;
	}
}

/* Road control register writes (0x2400-0x24ff, GAS_w in drivers/tx1.c),
   ported from buggyboy_gas_w. base_offset is the even byte address of
   the 16-bit word just completed (see GAS_w's byte-accumulate wrapper). */
void bb_gas_w(int base_offset, UINT16 data)
{
	switch (base_offset & 0xe0)
	{
		case 0x00:
			bb_vregs.ba_inc &= ~0x0000ffff;
			bb_vregs.ba_inc |= data;
			if (!(base_offset & 2))
				bb_vregs.ba_val &= ~0x0000ffff;
			break;

		case 0x20:
			data &= 0xff;
			bb_vregs.ba_inc &= ~0xffff0000;
			bb_vregs.ba_inc |= data << 16;
			bb_vregs.bank_mode = data & 1;
			if (!(base_offset & 2))
				bb_vregs.ba_val &= ~0xffff0000;
			break;

		case 0x40:
			if (base_offset & 2)
				bb_vregs.ba_val = (bb_vregs.ba_inc + bb_vregs.ba_val) & 0x00ffffff;
			break;

		case 0x60:
			bb_vregs.h_inc = data;
			bb_vregs.shift = 0;
			if (!(base_offset & 2))
				bb_vregs.h_val = 0;
			break;

		case 0x80:
			if (base_offset & 2)
				bb_vregs.h_val += bb_vregs.h_inc;
			break;

		case 0xa0:
			bb_vregs.wa8 = data >> 8;
			bb_vregs.wa4 = 0;
			break;

		case 0xe0:
			/* CPU-halt side effect for this case is handled by the
			   GAS_w wrapper in drivers/tx1.c (it already calls
			   halt_slave() for this address range) */
			bb_vregs.flags = (UINT8)data;
			break;
	}

	/* Value is latched by LS373 76/77 */
	bb_vregs.gas = data;
}

/* Note: /SKYCS needs no handler - bb_sky (drivers/tx1.c) is already a
   raw byte pointer read directly by draw_sky(), matching the reference's
   plain "vregs.sky = data" store. */

void bb_scolst_w(UINT16 data)
{
	bb_vregs.scol = data;
}

void bb_slincs_w(int word_index, UINT16 data)
{
	if (word_index == 1)
		bb_vregs.slin_inc = (UINT8)data;
	else
		bb_vregs.slin_inc = bb_vregs.slin_val = 0;
}


/*===================================================================*/
/*        Buggy Boy character/background layer with H/V scroll       */
/*                                                                    */
/*  Ported from MAME 0.144's buggyboy_draw_char. Replaces the plain   */
/*  static MAME tilemap 0.106 used for this layer: the reference's    */
/*  scroll addressing isn't a simple linear scroll - rows 0-63 (the   */
/*  HUD/text band) are never scrolled, while rows 64-239 sample a     */
/*  vertically-shifting source band (scroll_y) that also picks up a   */
/*  horizontal offset (scroll_x) only when that shifted source lands  */
/*  in tile-rows 8-15 specifically (the mountain/horizon strip) -     */
/*  none of which maps onto tilemap_set_scrollx/scrolly. VRAM here is */
/*  actually a 64-wide x 32-tall tile grid (2048 words matches VRAM   */
/*  size exactly), not the 64x64 the old tilemap_create guessed.      */
/*===================================================================*/

static UINT16 bb_vram_word(const UINT8 *vram, int word_index)
{
	int off = word_index * 2;
	return vram[off] | (vram[off + 1] << 8);
}

static void bb_draw_chars(mame_bitmap *bitmap, int opaque)
{
	UINT8 *chars = (UINT8 *)memory_region(REGION_GFX1);
	UINT8 *gfx2  = chars + 0x4000;
	const UINT8 *chr_pal = (const UINT8 *)memory_region(REGION_PROMS) + 0x400;

	UINT32 scroll_x, scroll_y;
	INT32 x, y;

	scroll_y = (bb_vram_word(buggyb1_vram, 0x7ff) >> 10) & 0x3f;
	scroll_x = bb_vram_word(buggyb1_vram, 0x7ff) & 0x1ff;

	for (y = 0; y < 240; ++y)
	{
		UINT32 d0 = 0, d1 = 0;
		UINT32 colour = 0;
		UINT32 y_offs, x_offs, y_gran;

		if (y < 64)
			y_offs = y;
		else
		{
			y_offs = (y + (scroll_y | 0xc0) + 1) & 0xff;
			if (y_offs < 64)
				y_offs |= 0xc0;
		}

		if ((y_offs >= 64) && (y_offs < 128))
			x_offs = scroll_x;
		else
			x_offs = 0;

		y_gran = y_offs & 7;

		if (x_offs & 7)
		{
			UINT32 tilenum;
			UINT16 ram_val = bb_vram_word(buggyb1_vram, ((y_offs << 3) & 0x7c0) + ((x_offs >> 3) & 0x3f));

			tilenum = (ram_val & 0x03ff) | ((ram_val & 0x8000) >> 5);
			colour = (ram_val & 0xfc00) >> 8;
			d0 = *(gfx2 + (tilenum << 3) + y_gran);
			d1 = *(chars + (tilenum << 3) + y_gran);
		}

		for (x = 0; x < 256; ++x)
		{
			UINT32 x_gran = x_offs & 7;
			UINT32 char_val, pen;

			if (!x_gran)
			{
				UINT32 tilenum;
				UINT16 ram_val = bb_vram_word(buggyb1_vram, ((y_offs << 3) & 0x7c0) + ((x_offs >> 3) & 0x3f));

				tilenum = (ram_val & 0x03ff) | ((ram_val & 0x8000) >> 5);
				colour = (ram_val & 0xfc00) >> 8;
				d0 = *(gfx2 + (tilenum << 3) + y_gran);
				d1 = *(chars + (tilenum << 3) + y_gran);
			}

			char_val = colour |
				   (((d1 >> (7 ^ x_gran)) & 1) << 1) |
				   ((d0 >> (7 ^ x_gran)) & 1);

			/* Transparent pen: pixel value 0 with no colour attribute set.
			   In opaque mode (matching the old TILEMAP_IGNORE_TRANSPARENCY
			   behaviour for non-driving/menu screens) draw everything. */
			if (opaque || (char_val & 3))
			{
				pen = 192 + ((char_val & 0xc0) >> 2) + (chr_pal[char_val] & 0xf);
				plot_pixel(bitmap, x, y, Machine->pens[pen]);
			}

			x_offs = (x_offs + 1) & 0x1ff;
		}
	}
}

VIDEO_START( buggyb1 )
{
	buggyb1_tilemap = tilemap_create(get_buggyb1_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,64,64);
        tilemap_set_transparent_pen(buggyb1_tilemap, 0);
	return 0;
}


VIDEO_START( buggyboy )
{
	buggyboy_tilemap = tilemap_create(get_buggyboy_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,128,64);
        tilemap_set_transparent_pen(buggyboy_tilemap, 0);
	return 0;
}



/* Gradient sky - 'scrolls' up and down */
static void draw_sky(mame_bitmap *bitmap)
{
	int x,y,colour;
	for (y = 0; y < 256; y++)
	{
		for (x = 0; x <= Machine->visible_area.max_x; x++)
		{
		        colour = (((*bb_sky & 0x7f) + y)>>2)&0x3f;
			plot_pixel(bitmap,x,y,Machine->pens[0x80 + colour]);
		}
	}
}


/*
The current layer mixing implementation is incorrect.

On the actual PCB, the GAME OVER sign chains should be behind the text but the
objects are often in front of the characters.

See schematic page 11 for mixing logic.

*/


VIDEO_UPDATE( buggyb1 )
{
            if(*bb_sky & 0x80)
            {
               /* Character/background layer goes behind the road - confirmed live
                  that drawing it after bb_draw_road (the original order, back when
                  this was still tilemap_draw) was painting its background/ground
                  tiles right over ~70% of the road. */
               draw_sky(bitmap);
               bb_draw_chars(bitmap, 0);
               bb_draw_road(bitmap);
               bb_draw_objects(bitmap,cliprect);
            }
            else
            {
              bb_draw_chars(bitmap, 1);
              bb_draw_objects(bitmap,cliprect);
            }
}

VIDEO_UPDATE( buggyboy )
{
            if(*bb_sky & 0x80)
            {
               draw_sky(bitmap);
               tilemap_draw(bitmap,cliprect,buggyboy_tilemap,0,0);
               bb_draw_objects(bitmap,cliprect);
            }
            else
            {
              tilemap_draw(bitmap,cliprect,buggyboy_tilemap,TILEMAP_IGNORE_TRANSPARENCY,0);
              bb_draw_objects(bitmap,cliprect);
            }
}
