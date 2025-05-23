/*

    Cherry Master II ver 8.41

    CPU : LH0080B Z80B CPU

    RAM : MB8128-15 x2

        MB8416-20 x2

        HM6116

    Sound : Winbond WF19054

    Crystal : 12.000 Mhz

    Dip SW :

    4x8 dip + 1 Switch (main test ???)

    ---

    AGEMAME driver by Curt Coder

Any fixes for this driver should be forwarded to AGEMAME at (http://www.mameworld.net/agemame)

*/

#include "driver.h"
#include "sound/ay8910.h"
/* vidhrdw */

#if 0 //Disabled
static tilemap *bg_tilemap;

static WRITE8_HANDLER( cm_videoram_w )
{
	if (videoram[offset] != data)
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

static WRITE8_HANDLER( cm_colorram_w )
{
	if (colorram[offset] != data)
	{
		colorram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap, offset);
	}
}

static void get_bg_tile_info(int tile_index)
{
	int attr = colorram[tile_index];
	int code = videoram[tile_index];
	int color = attr;

	SET_TILE_INFO(0, code, color, 0)
}
#endif

VIDEO_START(cm2)
{
#if 0
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		TILEMAP_OPAQUE, 8, 8, 32, 32);

	if ( !bg_tilemap )
		return 1;
#endif
	return 0;
}

VIDEO_UPDATE(cm2)
{
//  tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
}

static ADDRESS_MAP_START( cm_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM AM_WRITENOP
//  AM_RANGE(0x8000, 0xffff) AM_RAM
#if 0
	AM_RANGE(0x8000, 0xd3ff) AM_RAM
	AM_RANGE(0xd400, 0xd7ff) AM_RAM AM_WRITE(cm_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xd800, 0xdbff) AM_RAM
	AM_RANGE(0xdc00, 0xdfff) AM_RAM AM_WRITE(cm_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe000, 0xffff) AM_RAM
#endif
	AM_RANGE(0xd000, 0xd7ff) AM_RAM
	AM_RANGE(0x8000, 0xcfff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( cm_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
//  AM_RANGE(0x01, 0x01) AM_READ(AY8910_read_port_0_r)
//  AM_RANGE(0x02, 0x02) AM_WRITE(AY8910_write_port_0_w)
//  AM_RANGE(0x03, 0x03) AM_WRITE(AY8910_control_port_0_w)
//  AM_RANGE(0x04, 0x04) AM_READ(input_port_0_r)
//  AM_RANGE(0x09, 0x09) AM_READ(input_port_1_r)
//  AM_RANGE(0x0a, 0x0a) AM_READ(input_port_2_r)

//  AM_RANGE(0x02, 0x02) AM_WRITENOP
//  AM_RANGE(0x03, 0x03) AM_WRITENOP
//  AM_RANGE(0x07, 0x07) AM_WRITENOP
//  AM_RANGE(0x0b, 0x0b) AM_WRITENOP
//  AM_RANGE(0x10, 0x10) AM_WRITENOP
//  AM_RANGE(0x12, 0x12) AM_WRITENOP
ADDRESS_MAP_END

INPUT_PORTS_START( cm2v841 )
	PORT_START_TAG("PLAYER")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 0") PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 1") PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 2") PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 3") PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 4") PORT_CODE(KEYCODE_T)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 5") PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 6") PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 7") PORT_CODE(KEYCODE_I)

	PORT_START_TAG("COIN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 0") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 1") PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 2") PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 3") PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 4") PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 5") PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 6") PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 7") PORT_CODE(KEYCODE_K)

	PORT_START_TAG("TEST")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 0") PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 1") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 2") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 3") PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 4") PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 5") PORT_CODE(KEYCODE_N)
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Bit 7") PORT_CODE(KEYCODE_L)

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START_TAG("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW4")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW5")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8, 8,
	4096,
	3,
	{ 0, 0x8000*8, 0x10000*8 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const UINT32 spritelayout_xoffset[80] =
{
	  3+0*64*8, 2+0*64*8, 1+0*64*8, 0+0*64*8, 3+0*64*8+8, 2+0*64*8+8, 1+0*64*8+8, 0+0*64*8+8,
	  3+1*64*8, 2+1*64*8, 1+1*64*8, 0+1*64*8, 3+1*64*8+8, 2+1*64*8+8, 1+1*64*8+8, 0+1*64*8+8,
	  3+2*64*8, 2+2*64*8, 1+2*64*8, 0+2*64*8, 3+2*64*8+8, 2+2*64*8+8, 1+2*64*8+8, 0+2*64*8+8,
	  3+3*64*8, 2+3*64*8, 1+3*64*8, 0+3*64*8, 3+3*64*8+8, 2+3*64*8+8, 1+3*64*8+8, 0+3*64*8+8,
	  3+4*64*8, 2+4*64*8, 1+4*64*8, 0+4*64*8, 3+4*64*8+8, 2+4*64*8+8, 1+4*64*8+8, 0+4*64*8+8,
	  3+5*64*8, 2+5*64*8, 1+5*64*8, 0+5*64*8, 3+5*64*8+8, 2+5*64*8+8, 1+5*64*8+8, 0+5*64*8+8,
	  3+6*64*8, 2+6*64*8, 1+6*64*8, 0+6*64*8, 3+6*64*8+8, 2+6*64*8+8, 1+6*64*8+8, 0+6*64*8+8,
	  3+7*64*8, 2+7*64*8, 1+7*64*8, 0+7*64*8, 3+7*64*8+8, 2+7*64*8+8, 1+7*64*8+8, 0+7*64*8+8,
	  3+8*64*8, 2+8*64*8, 1+8*64*8, 0+8*64*8, 3+8*64*8+8, 2+8*64*8+8, 1+8*64*8+8, 0+8*64*8+8,
	  3+9*64*8, 2+9*64*8, 1+9*64*8, 0+9*64*8, 3+9*64*8+8, 2+9*64*8+8, 1+9*64*8+8, 0+9*64*8+8
};

static const gfx_layout spritelayout =
{
	80, 32,
	12,
	4,
	{ 0, 4, 0x4000*8, 4+0x4000*8 },
	EXTENDED_XOFFS,
	{
		0*8,  2*8,  4*8,  6*8,  8*8, 10*8, 12*8, 14*8, 16*8, 18*8, 20*8, 22*8, 24*8, 26*8, 28*8, 30*8,
		32*8, 34*8, 36*8, 38*8, 40*8, 42*8, 44*8, 46*8, 48*8, 50*8, 52*8, 54*8, 56*8, 58*8, 60*8, 62*8
	},
	10*64*8,
	spritelayout_xoffset,
	NULL
};

static const UINT32 spritelayout2_xoffset[80] =
{
	  3+0*64*8, 2+0*64*8, 1+0*64*8, 0+0*64*8,
	  3+1*64*8, 2+1*64*8, 1+1*64*8, 0+1*64*8,
	  3+2*64*8, 2+2*64*8, 1+2*64*8, 0+2*64*8,
	  3+3*64*8, 2+3*64*8, 1+3*64*8, 0+3*64*8,
	  3+4*64*8, 2+4*64*8, 1+4*64*8, 0+4*64*8,
	  3+5*64*8, 2+5*64*8, 1+5*64*8, 0+5*64*8,
	  3+6*64*8, 2+6*64*8, 1+6*64*8, 0+6*64*8,
	  3+7*64*8, 2+7*64*8, 1+7*64*8, 0+7*64*8,
	  3+8*64*8, 2+8*64*8, 1+8*64*8, 0+8*64*8,
	  3+9*64*8, 2+9*64*8, 1+9*64*8, 0+9*64*8
};

static const UINT32 spritelayout2_yoffset[128] =
{
	0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4, 8*4, 9*4, 10*4,
	11*4, 12*4, 13*4, 14*4,	15*4, 16*4, 17*4, 18*4, 19*4, 20*4,
	21*4, 22*4, 23*4, 24*4, 25*4, 26*4, 27*4, 28*4,	29*4, 30*4,
	31*4, 32*4, 33*4, 34*4, 35*4, 36*4, 37*4, 38*4, 39*4, 40*4,
	41*4, 42*4, 43*4, 44*4, 45*4, 46*4, 47*4, 48*4, 49*4, 50*4,
	51*4, 52*4, 53*4, 54*4, 55*4, 56*4, 57*4, 58*4, 59*4, 60*4,
	61*4, 62*4, 63*4, 64*4, 65*4, 66*4, 67*4, 68*4, 69*4, 70*4,
	71*4, 72*4, 73*4, 74*4, 75*4, 76*4, 77*4, 78*4, 79*4, 80*4,
	81*4, 82*4, 83*4, 84*4,	85*4, 86*4, 87*4, 88*4, 89*4, 90*4,
	91*4, 92*4, 93*4, 94*4, 95*4, 96*4, 97*4, 98*4,	99*4, 100*4,
	101*4, 102*4, 103*4, 104*4, 105*4, 106*4, 107*4, 108*4, 109*4,
	110*4, 111*4, 112*4, 113*4, 114*4, 115*4, 116*4, 117*4, 118*4,
	119*4, 120*4, 121*4, 122*4, 123*4, 124*4, 125*4, 126*4, 127*4
};

static const gfx_layout spritelayout2 =
{
	80, 128,
	6,
	1,
	{ 0 },
	EXTENDED_XOFFS,
	EXTENDED_YOFFS,
	128*64*8,
	spritelayout2_xoffset,
	spritelayout2_yoffset
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,	   0, 16 },
	{ -1 }
};

static const gfx_decode gfxdecodeinfoalt[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,	   0, 16 },
	{ REGION_GFX2, 0x0040, &spritelayout,  0, 16 },
	{ REGION_GFX3, 0x0100, &spritelayout2, 0, 16 },
	{ -1 }
};


static struct AY8910interface ay8910_interface =
{
	input_port_6_r,	//
	input_port_7_r,	//
	0,
	0
};

static MACHINE_DRIVER_START( cm2v841 )
	// basic machine hardware
	MDRV_CPU_ADD(Z80, 8000000/2)	// ???
	MDRV_CPU_PROGRAM_MAP(cm_map, 0)
	MDRV_CPU_IO_MAP(cm_io_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	//MDRV_NVRAM_HANDLER(cmv801)

	// video hardware
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)
	MDRV_COLORTABLE_LENGTH(256)

	MDRV_VIDEO_START(cm2)
	MDRV_VIDEO_UPDATE(cm2)

	// sound hardware
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD(AY8910,18432000/12)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( cm2841a )

	MDRV_IMPORT_FROM(cm2v841)
	MDRV_GFXDECODE(gfxdecodeinfoalt)
	MACHINE_DRIVER_END


ROM_START( cm2v841 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u81.9",  0x0000,  0x8000, CRC(09e44314) SHA1(dbb7e9afc9a1dc0d4ce7b150324077f3f3579c02) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "u4.5",   0x0000,  0x8000, CRC(e39fff9c) SHA1(22fdc517fa478441622c6245cecb5728c5595757) )
	ROM_LOAD( "u11.6",  0x8000,  0x8000, CRC(63b3df4e) SHA1(9bacd23da598805ec18ec5ad15cab95d71eb9262) )
	ROM_LOAD( "u16.7", 0x10000,  0x8000, CRC(19cc1d67) SHA1(47487f9362bfb36a32100ed772960628844462bf) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "u9.1",	0x0000,  0x2000, CRC(9678ead2) SHA1(e80aefa98b2363fe9e6b2415762695ace272e4d3) )
	ROM_LOAD( "u14.2",  0x2000,  0x2000, CRC(6dfcb188) SHA1(22430429c798954d9d979e62699b58feae7fdbf4) )
	ROM_LOAD( "u10.3",  0x4000,  0x2000, CRC(c32367be) SHA1(ff217021b9c58e23b2226f8b0a7f5da966225715) )
	ROM_LOAD( "u15.4",  0x6000,  0x2000, CRC(8607ffd9) SHA1(9bc94715554aa2473ae2ed249a47f29c7886b3dc) )

	ROM_REGION( 0x10000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "u53.8",  0x0000, 0x10000, CRC(e92443d3) SHA1(4b6ca4521841610054165f085ae05510e77af191) )
ROM_END

ROM_START( cm2841a )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "msii841.u81",  0x0000,  0x8000, CRC(977db602) SHA1(0fd3d6781b654ac6befdc9278f84ca708d5d448c) )
	/* there is another identical set, with the 2 halves of this rom swapped */

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "msii841.u4",   0x0000,  0x8000, CRC(e39fff9c) SHA1(22fdc517fa478441622c6245cecb5728c5595757) )
	ROM_LOAD( "msii841.u11",  0x8000,  0x8000, CRC(63b3df4e) SHA1(9bacd23da598805ec18ec5ad15cab95d71eb9262) )
	ROM_LOAD( "msii841.u16", 0x10000,  0x8000, CRC(19cc1d67) SHA1(47487f9362bfb36a32100ed772960628844462bf) )

	ROM_REGION( 0x8000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "msii841.u1",   0x0000,  0x4000, CRC(cf322ed2) SHA1(84df96229b7bdba0ab498e3bf9c77d7a7661f7b3) )
	ROM_LOAD( "msii841.u2",   0x4000,  0x4000, CRC(58c05653) SHA1(59454c07f4fe5b684d078cf97f2b1ee05b02f4ed) )

	ROM_REGION( 0x10000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "msii841.u53",  0x0000, 0x10000, CRC(e92443d3) SHA1(4b6ca4521841610054165f085ae05510e77af191) )
ROM_END

GAME( 198?, cm2v841, 0,       cm2v841, cm2v841, 0, ROT0, "Dyna Electronics", "Cherry Master II v8.41 (set 1)", GAME_NOT_WORKING ,0)
GAME( 198?, cm2841a, cm2v841, cm2841a, cm2v841, 0, ROT0, "Dyna Electronics", "Cherry Master II v8.41 (set 2)", GAME_NOT_WORKING ,0)
