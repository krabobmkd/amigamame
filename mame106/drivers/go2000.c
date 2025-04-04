/*

Go 2000 - Korean Card game

Newer PCB, very sparce with newer surface mounted CPUs

MC68EC000FU10
Z84C0006FEC
TM29F550ZX
OSC: 32.000MHz
2 8-way Dipswitch banks
Ram:
 2 UM61256FK-15 (near 3 & 4 (68k program roms))
 3 Windbond W24257AK-15 (near TM29F550ZX)
 2 UM61256AK-15 (near Z80)

P1, P2 & P3 4-pin connectors (unkown purpose)

*/

#include "driver.h"

static UINT16* go2000_video;
static UINT16* go2000_video2;


static ADDRESS_MAP_START( go2000_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x200000, 0x203fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x600000, 0x60ffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x610000, 0x61ffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x800000, 0x800fff) AM_READ(MRA16_RAM)
	AM_RANGE(0xa00000, 0xa00001) AM_READ(input_port_0_word_r)
	AM_RANGE(0xa00002, 0xa00003) AM_READ(input_port_1_word_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( go2000_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x200000, 0x203fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x600000, 0x603fff) AM_WRITE(MWA16_RAM) AM_BASE(&go2000_video)
	AM_RANGE(0x604000, 0x60ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x610000, 0x613fff) AM_WRITE(MWA16_RAM) AM_BASE(&go2000_video2)
	AM_RANGE(0x614000, 0x61ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x800000, 0x800fff) AM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
ADDRESS_MAP_END


INPUT_PORTS_START( go2000 )
	PORT_START	/* 16bit */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) // continue
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON2 ) // korean symbol
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON3 ) // out
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) // high
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) // low
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED) // unused?
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED) // unused?
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNUSED) // unused?
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNUSED) // unused?
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON4 ) // m1
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON5 ) // m2
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON6 ) // m3
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED) // unused?
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_COIN2 ) // coin2

	PORT_START	/* dipswitches */
	PORT_DIPNAME( 0x0001, 0x0001, "1" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END


static const gfx_layout go2000_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 8,12,0,4 },
	{ 3,2,1,0, 19,18,17,16 },
	{ 0*32, 1*32, 2*32, 3*32,4*32,5*32,6*32,7*32 },
	8*32
};

VIDEO_START(go2000)
{
	return 0;
}

VIDEO_UPDATE(go2000)
{
	int x,y;
	int count = 0;

	/* 0x600000 - 0x601fff / 0x610000 - 0x611fff */
	
	{ 
	struct drawgfxParams dgp0={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (x=0;x<64;x++)
	{
		for (y=0;y<32;y++)
		{
			int tile = go2000_video[count];
			int attr = go2000_video2[count];
			
			dgp0.code = tile;
			dgp0.color = attr;
			dgp0.sx = x*8;
			dgp0.sy = y*8;
			drawgfx(&dgp0);
			count++;
		}
	}
	} // end of patch paragraph


	/* 0x602000 - 0x603fff / 0x612000 - 0x613fff */
	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		cliprect, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (x=0;x<64;x++)
	{
		for (y=0;y<32;y++)
		{
			int tile = go2000_video[count];
			int attr = go2000_video2[count];
			
			dgp1.code = tile;
			dgp1.color = attr;
			dgp1.sx = x*8;
			dgp1.sy = y*8;
			drawgfx(&dgp1);
			count++;
		}
	}
	} // end of patch paragraph

}
static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &go2000_layout,   0x0, 0x80  }, /* tiles */
	{ -1 } /* end of array */
};


static MACHINE_DRIVER_START( go2000 )
	MDRV_CPU_ADD_TAG("main", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(go2000_readmem,go2000_writemem)
	MDRV_CPU_VBLANK_INT(irq1_line_hold,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 48*8-1, 2*8, 30*8-1)
	MDRV_PALETTE_LENGTH(0x800)

	MDRV_VIDEO_START(go2000)
	MDRV_VIDEO_UPDATE(go2000)

	MDRV_SPEAKER_STANDARD_STEREO("left", "right")
MACHINE_DRIVER_END

ROM_START( go2000 )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "3.bin", 0x00000, 0x20000, CRC(fe1fb269) SHA1(266b8acddfcfd960b8e44f8606bf0873da42b9f8) )
	ROM_LOAD16_BYTE( "4.bin", 0x00001, 0x20000, CRC(d6246ae3) SHA1(f2618dcabaa0c0a6e377e4acd1cdec8bea90bea8) )

	ROM_REGION( 0x080000, REGION_CPU2, 0 ) /* Z80? */
	ROM_LOAD( "5.bin", 0x00000, 0x80000, CRC(a32676ee) SHA1(2dab73497c0818fce479be21ed589985db51560b) )

	ROM_REGION( 0x40000, REGION_GFX1, 0 )
	ROM_LOAD16_BYTE( "1.bin", 0x00000, 0x20000, CRC(96e50aba) SHA1(caa1aadab855c3a758378dc8c48eec859e8110a4) )
	ROM_LOAD16_BYTE( "2.bin", 0x00001, 0x20000, CRC(b0adf1cb) SHA1(2afb30691182dbf46be709f0d5b03b0f8ff52790) )
ROM_END


GAME( 2000, go2000,    0, go2000,    go2000,    0, ROT0,  "SA", "Go 2000", GAME_NO_SOUND|GAME_NOT_WORKING ,0)
