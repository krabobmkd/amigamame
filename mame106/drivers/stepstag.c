/*  Jaleco 'Stepping Stage'

 dump is incomplete, these are leftovers from an upgrade
 music roms are missing at least

 is it a 3 screen game (1 horizontal, 2 vertical) ?

 */

#include "driver.h"
//#include "random.h"

READ16_HANDLER( unknown_read_0xc00000 )
{
	return mame_rand();
}

READ16_HANDLER( unknown_read_0xd00000 )
{
	return mame_rand();
}

READ16_HANDLER( unknown_read_0xffff00 )
{
	return mame_rand();
}

static ADDRESS_MAP_START( stepstag_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x200000, 0x20ffff) AM_READ(MRA16_RAM)

	AM_RANGE(0x300000, 0x33ffff) AM_READ(MRA16_RAM)

	AM_RANGE(0x400000, 0x43ffff) AM_READ(MRA16_RAM)

	AM_RANGE(0x500000, 0x53ffff) AM_READ(MRA16_RAM)


	AM_RANGE(0x800000, 0x87ffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x900000, 0x97ffff) AM_READ(MRA16_RAM)
	AM_RANGE(0xa00000, 0xa7ffff) AM_READ(MRA16_RAM)
	AM_RANGE(0xc00000, 0xc00001) AM_READ(unknown_read_0xc00000)
	AM_RANGE(0xd00000, 0xd00001) AM_READ(unknown_read_0xd00000)
	AM_RANGE(0xffff00, 0xffff01) AM_READ(unknown_read_0xffff00)
ADDRESS_MAP_END

static ADDRESS_MAP_START( stepstag_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x200000, 0x20ffff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0x300000, 0x3fffff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0x400000, 0x43ffff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0x500000, 0x53ffff) AM_WRITE(MWA16_RAM)

	AM_RANGE(0x700000, 0x700001) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0x700002, 0x700003) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0x700004, 0x700005) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0x700006, 0x700007) AM_WRITE(MWA16_NOP) //??

	AM_RANGE(0x800000, 0x87ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x880000, 0x880001) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0x900000, 0x97ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x980000, 0x980001) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0xa00000, 0xa7ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0xa80000, 0xa80001) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0xc00000, 0xc00001) AM_WRITE(MWA16_NOP) //??
	AM_RANGE(0xf00000, 0xf00001) AM_WRITE(MWA16_NOP) //??
ADDRESS_MAP_END

READ16_HANDLER( unknown_sub_read_0xbe0004 )
{
	return mame_rand();
}


static ADDRESS_MAP_START( stepstag_sub_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_READ(MRA16_ROM)
	AM_RANGE(0xbe0004, 0xbe0005) AM_READ(unknown_sub_read_0xbe0004)
ADDRESS_MAP_END

static ADDRESS_MAP_START( stepstag_sub_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_WRITE(MWA16_ROM)
ADDRESS_MAP_END

INPUT_PORTS_START( stepstag )
INPUT_PORTS_END

static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 0,1, 2,3, 4,5,6,7 },
	{ 0, 8, 16, 24, 32, 40, 48, 56 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64 },
	64*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX2, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX3, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX4, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX5, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX6, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX7, 0, &tiles8x8_layout, 0, 2 },
	{ REGION_GFX8, 0, &tiles8x8_layout, 0, 2 },

	{ -1 }
};

VIDEO_START(stepstag)
{
	return 0;
}
VIDEO_UPDATE(stepstag)
{

}

static MACHINE_DRIVER_START( stepstag )
	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 16000000 ) //??
	MDRV_CPU_PROGRAM_MAP(stepstag_readmem,stepstag_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1) // 4 & 6 valid

	MDRV_CPU_ADD(M68000, 16000000 ) //??
	MDRV_CPU_PROGRAM_MAP(stepstag_sub_readmem,stepstag_sub_writemem)


	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 0*8, 32*8-1)
	MDRV_PALETTE_LENGTH(0x200)
	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_START(stepstag)
	MDRV_VIDEO_UPDATE(stepstag)
MACHINE_DRIVER_END


ROM_START( stepstag )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68k */
	ROM_LOAD16_BYTE( "vj98348ver11.11", 0x00000, 0x80000, CRC(29b7f848) SHA1(c4d89e5c9be622b2d9038c359a5f65ce0dd461b0) )
	ROM_LOAD16_BYTE( "vj98348ver11.14", 0x00001, 0x80000, CRC(e3314c6c) SHA1(61b0e9f9d0126d9f475304866a03cfa21701d9aa) )

	ROM_REGION( 0x100000, REGION_CPU2, 0 ) /* 68k */
	ROM_LOAD16_BYTE( "vj98344ver11.1", 0x00001, 0x80000, CRC(aedcb225) SHA1(f167c390e79ffbf7c019c326384ae656ae8b7d13) )
	ROM_LOAD16_BYTE( "vj98344ver11.4", 0x00000, 0x80000, CRC(391ca913) SHA1(2cc329aa6419f8a0d7e0fb8a9f4c2b8ca25197b3) )


	ROM_REGION( 0x400000, REGION_GFX1, 0 ) /* */
	ROM_LOAD( "mr99001-06", 0x00000, 0x400000, CRC(cfa27c93) SHA1(a0837877736e8e898f3acc64bc87ee0cc4d9f243) )

	ROM_REGION( 0x400000, REGION_GFX2, 0 ) /* */
	ROM_LOAD( "mr99001-05", 0x00000, 0x400000, CRC(3958473b) SHA1(12279a587263290945744b22aafb80460eea77f7) )

	ROM_REGION( 0x400000, REGION_GFX3, 0 ) /* */
	ROM_LOAD( "mr99001-04", 0x00000, 0x400000, CRC(d6837981) SHA1(56709d73304f0b186c70844ae96f73400b541609) )

	ROM_REGION( 0x400000, REGION_GFX4, 0 ) /* */
	ROM_LOAD( "mr99001-03", 0x00000, 0x400000, CRC(40fee0df) SHA1(94c3567e82f8039b3169bf4dcb1fcd9e39c6eb27) )

	ROM_REGION( 0x400000, REGION_GFX5, 0 ) /* */
	ROM_LOAD( "mr99001-02", 0x00000, 0x400000, CRC(12c65d86) SHA1(7fe5853fa3ba086f8da15702b126eb13c6ea30a9) )

	ROM_REGION( 0x400000, REGION_GFX6, 0 ) /* */
	ROM_LOAD( "mr99001-01", 0x00000, 0x400000,  CRC(aa92cebf) SHA1(2ccc0d2ef9bc92c27f0a625819154bbcf9cfde0c) )

	ROM_REGION( 0x400000, REGION_GFX7, 0 ) /* */
	ROM_LOAD( "s.s.s._vj-98348_19_pr99021-02", 0x00000, 0x400000, CRC(2d98da1a) SHA1(b09375fa1b4b2e0794632d6e237459009f40310d) )

	ROM_REGION( 0x400000, REGION_GFX8, 0 ) /* */
	ROM_LOAD( "s.s.s._vj-98348_26_pr99021-01", 0x00000, 0x400000, CRC(fefb3777) SHA1(df624e105ab1dea52317e318ad29caa02b900788) )
	ROM_LOAD( "s.s.s._vj-98348_3_pr99021-01", 0x00000, 0x400000, CRC(e0fbc6f1) SHA1(7ca4507702f3f81bb9de3f9b5d270d379e439633) )
ROM_END


GAME( 1999, stepstag, 0, stepstag, stepstag, 0, ROT0, "Jaleco", "Stepping Stage", GAME_NO_SOUND| GAME_NOT_WORKING,0)
