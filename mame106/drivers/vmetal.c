/*

Varia Metal
Excellent System Ltd, 1995

PCB Layout
----------
This game runs on Metro hardware.

ES-9309B-B
|--------------------------------------------|
| TA7222   8.U9     DSW1(8)  DSW2(8)         |
|VOL    M6295  1.000MHz                      |
|                   |------------|           |
|          7.U12    |   68000    |           |
|  uPC3403          |------------|           |
|J 640kHz  ES-8712                           |
|A M6585           EPM7032    6B.U18  5B.U19 |
|M       MM1035                              |
|M        26.666MHz  16MHz    62256   62256  |
|A                                           |
|                 |--------|          1.U29  |
|         62256   |Imagetek|                 |
|         62256   |14220   |          2.U31  |
|                 |        |                 |
|                 |--------|          3.U28  |
|                                            |
|                  6264               4.U30  |
|--------------------------------------------|
Notes:
      68000   - clock 16.000MHz
      ES-8712 - ES-8712 Single Channel ADPCM Samples Player. Clock ?? seems to be 16kHz?
                This chip is branded 'EXCELLENT', may be (or not??) manufactured by Ensonic (SDIP48)
      M6295   - clock 1.000MHz. Sample rate = 1000000/132
      M6585   - Oki M6585 ADPCM Voice Synthesizer IC (DIP18). Clock 640kHz.
                Sample rate = 16kHz (selection - pin 1 LOW, pin 2 HIGH = 16kHz)
                This is a version-up to the previous M5205 with some additional
                capabilies and improvements.
      MM1035  - Mitsumi Monolithic IC MM1035 System Reset and Watchdog Timer (DIP8)
      uPC3403 - NEC uPC3403 High Performance Quad Operational Amplifier (DIP14)
      62256   - 32k x8 SRAM (DIP28)
      6264    - 8k x8 SRAM (DIP28)
      TA7222  - Toshiba TA7222 5.8 Watt Audio Power Amplifier (SIP10)
      EPM7032 - Altera EPM7032LC44-15T High Performance EEPROM-based Programmable Logic Device (PLCC44)
      Custom  - Imagetek 14220 Graphics Controller (QFP208)
      VSync   - 58.2328Hz
      HSync   - 15.32kHz
      ROMs    -
                6B & 5B are 27C040 EPROM (DIP32)
                8 is 4M MaskROM (DIP32)
                All other ROMs are 16M MaskROM (DIP42)




Varia Metal

Notes:

*****
i should fully merge video with metro.c, it uses the same imagetek chip (although with 16x16 tiles)
this should fix most of the remaining gfx glitches
*****


It has Sega and Taito logos in the roms ?!

whats going on with the dipswitches

ES8712 sound may not be quite right. Samples are currently looped, but
whether they should and how, is unknown.

cleanup


*/

#include "driver.h"
#include "sound/okim6295.h"
#include "sound/es8712.h"

UINT16 *vmetal_texttileram;
UINT16 *vmetal_mid1tileram;
UINT16 *vmetal_mid2tileram;
UINT16 *vmetal_tlookup;
UINT16 *vmetal_videoregs;


static tilemap *vmetal_texttilemap;
static tilemap *vmetal_mid1tilemap;
static tilemap *vmetal_mid2tilemap;

/* vidhrdw/metro.c */
extern UINT16 *metro_videoregs;
extern void metro_draw_sprites(mame_bitmap *bitmap, const rectangle *cliprect);
WRITE16_HANDLER( metro_paletteram_w );

READ16_HANDLER ( varia_crom_read )
{
	/* game reads the cgrom, result is 7772, verified to be correct on the real board */

	UINT8 *cgrom = memory_region(REGION_GFX1);
	UINT16 retdat;
	offset = offset << 1;
	offset |= (vmetal_videoregs[0x0ab/2]&0x7f) << 16;
	retdat = ((cgrom[offset] <<8)| (cgrom[offset+1]))^0xffff; // invert because we inverted the data..
//  ui_popup("varia romread offset %06x data %04x",offset, retdat);

	return retdat;
}


READ16_HANDLER ( varia_random )
{
//  return rand();  // dips etc.. weird
	return 0xffff;
}



static void get_vmetal_tlookup(UINT16 data, UINT16 *tileno, UINT16 *color)
{
	int idx = ((data & 0x7fff) >> 4)*2;
	UINT32 lookup = (vmetal_tlookup[idx]<<16) | vmetal_tlookup[idx+1];
	*tileno = (data & 0xf) | ((lookup>>2) & 0xfff0);
	*color = (lookup>>20) & 0xff;
}


WRITE16_HANDLER( vmetal_texttileram_w )
{
	COMBINE_DATA(&vmetal_texttileram[offset]);
	tilemap_mark_tile_dirty(vmetal_texttilemap,offset);
}

WRITE16_HANDLER( vmetal_mid1tileram_w )
{
	COMBINE_DATA(&vmetal_mid1tileram[offset]);
	tilemap_mark_tile_dirty(vmetal_mid1tilemap,offset);
}
WRITE16_HANDLER( vmetal_mid2tileram_w )
{
	COMBINE_DATA(&vmetal_mid2tileram[offset]);
	tilemap_mark_tile_dirty(vmetal_mid2tilemap,offset);
}


static READ16_HANDLER ( varia_dips_bit8_r ) { return ((readinputport(3) & 0x80) << 0) | ((readinputport(2) & 0x80) >> 1); }
static READ16_HANDLER ( varia_dips_bit7_r ) { return ((readinputport(3) & 0x40) << 1) | ((readinputport(2) & 0x40) >> 0); }
static READ16_HANDLER ( varia_dips_bit6_r ) { return ((readinputport(3) & 0x20) << 2) | ((readinputport(2) & 0x20) << 1); }
static READ16_HANDLER ( varia_dips_bit5_r ) { return ((readinputport(3) & 0x10) << 3) | ((readinputport(2) & 0x10) << 2); }
static READ16_HANDLER ( varia_dips_bit4_r ) { return ((readinputport(3) & 0x08) << 4) | ((readinputport(2) & 0x08) << 3); }
static READ16_HANDLER ( varia_dips_bit3_r ) { return ((readinputport(3) & 0x04) << 5) | ((readinputport(2) & 0x04) << 4); }
static READ16_HANDLER ( varia_dips_bit2_r ) { return ((readinputport(3) & 0x02) << 6) | ((readinputport(2) & 0x02) << 5); }
static READ16_HANDLER ( varia_dips_bit1_r ) { return ((readinputport(3) & 0x01) << 7) | ((readinputport(2) & 0x01) << 6); }

static WRITE16_HANDLER( vmetal_control_w )
{
	/* Lower nibble is the coin control bits shown in
       service mode, but in game mode they're different */
	coin_counter_w(0,data & 0x04);
	coin_counter_w(1,data & 0x08);	/* 2nd coin schute activates coin 0 counter in game mode?? */
//  coin_lockout_w(0,data & 0x01);  /* always on in game mode?? */
	coin_lockout_w(1,data & 0x02);	/* never activated in game mode?? */

	if ((data & 0x40) == 0)
		sndti_reset(SOUND_ES8712, 0);
	else
		ES8712_play(0);

	if (data & 0x10)
		ES8712_set_bank_base(0, 0x100000);
	else
		ES8712_set_bank_base(0, 0x000000);

	if (data & 0xa0)
		logerror("PC:%06x - Writing unknown bits %04x to $200000\n",activecpu_get_previouspc(),data);
}

static WRITE16_HANDLER( vmetal_es8712_w )
{
	/* Many samples in the ADPCM ROM are actually not used.

    Snd         Offset Writes                 Sample Range
         0000 0004 0002 0006 000a 0008 000c
    --   ----------------------------------   -------------
    00   006e 0001 00ab 003c 0002 003a 003a   01ab6e-023a3c
    01   003d 0002 003a 001d 0002 007e 007e   023a3d-027e1d
    02   00e2 0003 0005 002e 0003 00f3 00f3   0305e2-03f32e
    03   000a 0005 001e 00f6 0005 00ec 00ec   051e0a-05ecf6
    04   00f7 0005 00ec 008d 0006 0060 0060   05ecf7-06608d
    05   0016 0008 002e 0014 0009 0019 0019   082e16-091914
    06   0015 0009 0019 0094 000b 0015 0015   091915-0b1594
    07   0010 000d 0012 00bf 000d 0035 0035   0d1210-0d35bf
    08   00ce 000e 002f 0074 000f 0032 0032   0e2fce-0f3274
    09   0000 0000 0000 003a 0000 007d 007d   000000-007d3a
    0a   0077 0000 00fa 008d 0001 00b6 00b6   00fa77-01b68d
    0b   008e 0001 00b6 00b3 0002 0021 0021   01b68e-0221b3
    0c   0062 0002 00f7 0038 0003 00de 00de   02f762-03de38
    0d   00b9 0005 00ab 00ef 0006 0016 0016   05abb9-0616ef
    0e   00dd 0007 0058 00db 0008 001a 001a   0758dd-081adb
    0f   00dc 0008 001a 002e 0008 008a 008a   081adc-088a2e
    10   00db 0009 00d7 00ff 000a 0046 0046   09d7db-0a46ff
    11   0077 000c 0003 006d 000c 0080 0080   0c0377-0c806d
    12   006e 000c 0080 006c 000d 0002 0002   0c806e-0d026c
    13   006d 000d 0002 002b 000d 0041 0041   0d026d-0d412b
    14   002c 000d 0041 002a 000d 00be 00be   0d412c-0dbe2a
    15   002b 000d 00be 0029 000e 0083 0083   0dbe2b-0e8329
    16   002a 000e 0083 00ee 000f 0069 0069   0e832a-0f69ee
    */

	ES8712_data_0_lsb_w(offset, data, mem_mask);
	logerror("PC:%06x - Writing %04x to ES8712 offset %02x\n",activecpu_get_previouspc(),data,offset);
}


static ADDRESS_MAP_START( varia_program_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_ROM
	AM_RANGE(0x100000, 0x11ffff) AM_READWRITE(MRA16_RAM, vmetal_texttileram_w) AM_BASE(&vmetal_texttileram)
	AM_RANGE(0x120000, 0x13ffff) AM_READWRITE(MRA16_RAM, vmetal_mid1tileram_w) AM_BASE(&vmetal_mid1tileram)
	AM_RANGE(0x140000, 0x15ffff) AM_READWRITE(MRA16_RAM, vmetal_mid2tileram_w) AM_BASE(&vmetal_mid2tileram)

	AM_RANGE(0x160000, 0x16ffff) AM_READ(varia_crom_read) // cgrom read window ..

	AM_RANGE(0x170000, 0x173fff) AM_READWRITE(MRA16_RAM,metro_paletteram_w) AM_BASE(&paletteram16	)	// Palette
	AM_RANGE(0x174000, 0x174fff) AM_RAM AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0x175000, 0x177fff) AM_RAM
	AM_RANGE(0x178000, 0x1787ff) AM_RAM AM_BASE(&vmetal_tlookup)
	AM_RANGE(0x178800, 0x1796ff) AM_RAM AM_BASE(&vmetal_videoregs)
	AM_RANGE(0x179700, 0x179713) AM_WRITE(MWA16_RAM) AM_BASE(&metro_videoregs	)	// Video Registers

	AM_RANGE(0x200000, 0x200001) AM_READWRITE(input_port_0_word_r, vmetal_control_w)
	AM_RANGE(0x200002, 0x200003) AM_READ(input_port_1_word_r )

	/* i have no idea whats meant to be going on here .. it seems to read one bit of the dips from some of them, protection ??? */
	AM_RANGE(0x30fffe, 0x30ffff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x317ffe, 0x317fff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31bffe, 0x31bfff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31dffe, 0x31dfff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31effe, 0x31efff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31f7fe, 0x31f7ff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31fbfe, 0x31fbff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31fdfe, 0x31fdff) AM_READ(varia_random )  // nothing?
	AM_RANGE(0x31fefe, 0x31feff) AM_READ(varia_dips_bit8_r )  // 0x40 = dip1-8 , 0x80 = dip2-8
	AM_RANGE(0x31ff7e, 0x31ff7f) AM_READ(varia_dips_bit7_r )  // 0x40 = dip1-7 , 0x80 = dip2-7
	AM_RANGE(0x31ffbe, 0x31ffbf) AM_READ(varia_dips_bit6_r )  // 0x40 = dip1-6 , 0x80 = dip2-6
	AM_RANGE(0x31ffde, 0x31ffdf) AM_READ(varia_dips_bit5_r )  // 0x40 = dip1-5 , 0x80 = dip2-5
	AM_RANGE(0x31ffee, 0x31ffef) AM_READ(varia_dips_bit4_r )  // 0x40 = dip1-4 , 0x80 = dip2-4
	AM_RANGE(0x31fff6, 0x31fff7) AM_READ(varia_dips_bit3_r )  // 0x40 = dip1-3 , 0x80 = dip2-3
	AM_RANGE(0x31fffa, 0x31fffb) AM_READ(varia_dips_bit2_r )  // 0x40 = dip1-2 , 0x80 = dip2-2
	AM_RANGE(0x31fffc, 0x31fffd) AM_READ(varia_dips_bit1_r )  // 0x40 = dip1-1 , 0x80 = dip2-1
	AM_RANGE(0x31fffe, 0x31ffff) AM_READ(varia_random )  // nothing?

	AM_RANGE(0x400000, 0x400001) AM_READWRITE(OKIM6295_status_0_lsb_r, OKIM6295_data_0_lsb_w )
	AM_RANGE(0x400002, 0x400003) AM_WRITE(OKIM6295_data_0_lsb_w )	// Volume/channel info
	AM_RANGE(0x500000, 0x50000d) AM_WRITE(vmetal_es8712_w)

	AM_RANGE(0xff0000, 0xffffff) AM_RAM
ADDRESS_MAP_END



INPUT_PORTS_START( varia )
	PORT_START		/* IN0 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START2  )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START		/* IN1 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_SERVICE2 ) // 'Test'
	PORT_BIT( 0xffe0, IP_ACTIVE_LOW, IPT_UNKNOWN ) // unused?

	PORT_START		/* Dips 1 */
	PORT_DIPNAME( 0x0007, 0x0007, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0005, DEF_STR( 3C_1C )  )
	PORT_DIPSETTING(      0x0006, DEF_STR( 2C_1C )  )
	PORT_DIPSETTING(      0x0007, DEF_STR( 1C_1C )  )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_2C )  )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_3C )  )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_4C )  )
	PORT_DIPSETTING(      0x0001, DEF_STR( 1C_5C )  )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C )  )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START		/* Dips 2 */
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0008, "0"  )
	PORT_DIPSETTING(      0x0004, "1"  )
	PORT_DIPSETTING(      0x000c, "2"  )
	PORT_DIPSETTING(      0x0000, "3"  )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_SERVICE( 0x0080, IP_ACTIVE_LOW )
INPUT_PORTS_END



static const gfx_layout char16x16layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4, 0, 12, 8, 20, 16, 28, 24, 36, 32, 44, 40, 52, 48, 60, 56 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	16*64
};

static const gfx_layout char8x8layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4, 0, 12, 8, 20, 16, 28, 24 },
	{ 0*32,1*32,2*32,3*32,4*32,5*32,6*32,7*32 },
	8*32
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &char16x16layout,   0x1000, 512  }, /* bg tiles */
	{ REGION_GFX1, 0, &char8x8layout,   0x1000, 512  }, /* bg tiles */
	{ -1 } /* end of array */
};


static void get_vmetal_texttilemap_tile_info(int tile_index)
{
	UINT32 tile;
	UINT16 color, data = vmetal_texttileram[tile_index];
	int idx = ((data & 0x7fff) >> 4)*2;
	UINT32 lookup = (vmetal_tlookup[idx]<<16) | vmetal_tlookup[idx+1];
	tile = (data & 0xf) | (lookup & 0x7fff0);
	color = ((lookup>>20) & 0x1f)+0xe0;
	if (data & 0x8000) tile = 0;
	SET_TILE_INFO(1, tile, color^0xf, TILE_FLIPYX(0x0));
}


static void get_vmetal_mid1tilemap_tile_info(int tile_index)
{
	UINT16 tile, color, data = vmetal_mid1tileram[tile_index];
	get_vmetal_tlookup(data, &tile, &color);
	if (data & 0x8000) tile = 0;
	SET_TILE_INFO(0, tile, color^0xf, TILE_FLIPYX(0x0));
}
static void get_vmetal_mid2tilemap_tile_info(int tile_index)
{
	UINT16 tile, color, data = vmetal_mid2tileram[tile_index];
	get_vmetal_tlookup(data, &tile, &color);
	if (data & 0x8000) tile = 0;
	SET_TILE_INFO(0, tile, color^0xf, TILE_FLIPYX(0x0));
}

VIDEO_START(varia)
{
	vmetal_texttilemap = tilemap_create(get_vmetal_texttilemap_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,      8, 8, 256,256);
	vmetal_mid1tilemap = tilemap_create(get_vmetal_mid1tilemap_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,      16,16, 256,256);
	vmetal_mid2tilemap = tilemap_create(get_vmetal_mid2tilemap_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT,      16,16, 256,256);
	tilemap_set_transparent_pen(vmetal_texttilemap,0);
	tilemap_set_transparent_pen(vmetal_mid1tilemap,0);
	tilemap_set_transparent_pen(vmetal_mid2tilemap,0);

	return 0;
}

VIDEO_UPDATE(varia)
{
	fillbitmap(bitmap, get_black_pen(), cliprect);
	fillbitmap(priority_bitmap,0,cliprect);

	tilemap_set_scrollx(vmetal_mid2tilemap,0, vmetal_videoregs[0x06a/2]-64 /*+ vmetal_videoregs[0x066/2]*/);
	tilemap_set_scrollx(vmetal_mid1tilemap,0, vmetal_videoregs[0x07a/2]-64 /*+ vmetal_videoregs[0x076/2]*/);
	tilemap_set_scrollx(vmetal_texttilemap,0, -64 /*+ vmetal_videoregs[0x076/2]*/);

	tilemap_set_scrolly(vmetal_mid2tilemap,0, -64 );
	tilemap_set_scrolly(vmetal_mid1tilemap,0, -64 );
	tilemap_set_scrolly(vmetal_texttilemap,0, -64 );

	tilemap_draw(bitmap,cliprect,vmetal_mid1tilemap,0,0);
	tilemap_draw(bitmap,cliprect,vmetal_mid2tilemap,0,0);
	metro_draw_sprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,vmetal_texttilemap,0,0);
}

static MACHINE_DRIVER_START( varia )
	MDRV_CPU_ADD(M68000, 16000000)
	MDRV_CPU_PROGRAM_MAP(varia_program_map, 0)
	MDRV_CPU_VBLANK_INT(irq1_line_hold,1) // also level 3

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(2048, 2048)
	MDRV_VISIBLE_AREA(0+64, 319+64, 0+64, 223+64)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(0x4000)

	MDRV_VIDEO_START(varia)
	MDRV_VIDEO_UPDATE(varia)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(OKIM6295, 10000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.75)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.75)

	MDRV_SOUND_ADD(ES8712, 12000)
	MDRV_SOUND_CONFIG(es8712_interface_region_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END


ROM_START( vmetal )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "5b.u19", 0x00001, 0x80000, CRC(4933ac6c) SHA1(1a3303e32fcb08854d4d6e13f36ca99d92aed4cc) )
	ROM_LOAD16_BYTE( "6b.u18", 0x00000, 0x80000, CRC(4eb939d5) SHA1(741ab05043fc3bd886162d878630e45da9359718) )

	ROM_REGION( 0x800000, REGION_GFX1, ROMREGION_INVERT )
	ROMX_LOAD( "1.u29", 0x000004, 0x200000, CRC(b470c168) SHA1(c30462dc134da1e71a94b36ef96ecd65c325b07e) , ROM_GROUPWORD | ROM_SKIP(6))
	ROMX_LOAD( "2.u31", 0x000000, 0x200000, CRC(b36f8d60) SHA1(1676859d0fee4eb9897ce1601a2c9fd9a6dc4a43) , ROM_GROUPWORD | ROM_SKIP(6))
	ROMX_LOAD( "3.u28", 0x000006, 0x200000, CRC(00fca765) SHA1(ca9010bd7f59367e483868018db9a9abf871386e) , ROM_GROUPWORD | ROM_SKIP(6))
	ROMX_LOAD( "4.u30", 0x000002, 0x200000, CRC(5a25a49c) SHA1(c30781202ec882e1ec6adfb560b0a1075b3cce55) , ROM_GROUPWORD | ROM_SKIP(6))

	ROM_REGION( 0x080000, REGION_SOUND1, 0 ) /* OKI6295 Samples */
	/* Second half is junk */
	ROM_LOAD( "8.u9", 0x00000, 0x80000, CRC(c14c001c) SHA1(bad96b5cd40d1c34ef8b702262168ecab8192fb6) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* Samples */
	ROM_LOAD( "7.u12", 0x00000, 0x200000, CRC(a88c52f1) SHA1(d74a5a11f84ba6b1042b33a2c156a1071b6fbfe1) )
ROM_END

ROM_START( vmetaln )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "vm5.bin", 0x00001, 0x80000, CRC(43ef844e) SHA1(c673f34fcc9e406282c9008795b52d01a240099a) )
	ROM_LOAD16_BYTE( "vm6.bin", 0x00000, 0x80000, CRC(cb292ab1) SHA1(41fdfe67e6cb848542fd5aa0dfde3b1936bb3a28) )

	ROM_REGION( 0x800000, REGION_GFX1, ROMREGION_INVERT )
	ROMX_LOAD( "1.u29", 0x000004, 0x200000, CRC(b470c168) SHA1(c30462dc134da1e71a94b36ef96ecd65c325b07e) , ROM_GROUPWORD | ROM_SKIP(6))
	ROMX_LOAD( "2.u31", 0x000000, 0x200000, CRC(b36f8d60) SHA1(1676859d0fee4eb9897ce1601a2c9fd9a6dc4a43) , ROM_GROUPWORD | ROM_SKIP(6))
	ROMX_LOAD( "3.u28", 0x000006, 0x200000, CRC(00fca765) SHA1(ca9010bd7f59367e483868018db9a9abf871386e) , ROM_GROUPWORD | ROM_SKIP(6))
	ROMX_LOAD( "4.u30", 0x000002, 0x200000, CRC(5a25a49c) SHA1(c30781202ec882e1ec6adfb560b0a1075b3cce55) , ROM_GROUPWORD | ROM_SKIP(6))

	ROM_REGION( 0x080000, REGION_SOUND1, 0 ) /* OKI6295 Samples */
	/* Second half is junk */
	ROM_LOAD( "8.u9", 0x00000, 0x80000, CRC(c14c001c) SHA1(bad96b5cd40d1c34ef8b702262168ecab8192fb6) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* Samples */
	ROM_LOAD( "7.u12", 0x00000, 0x200000, CRC(a88c52f1) SHA1(d74a5a11f84ba6b1042b33a2c156a1071b6fbfe1) )
ROM_END

GAME( 1995, vmetal,  0,      varia, varia, 0, ROT270, "Excellent Systems",                        "Varia Metal",                        GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS ,0)
GAME( 1995, vmetaln, vmetal, varia, varia, 0, ROT270, "[Excellent Systems] New Ways Trading Co.", "Varia Metal (New Ways Trading Co.)", GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS ,0)
