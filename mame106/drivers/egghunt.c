/*

Egg Hunt

hardware seems close to mitchell.c
--
palette format is different
has a sound cpu
ports shuffled around a bit



the following string is at the start of the roms

INVI IMAGE COPYWRITE 1995 OCT SEOUL IN KOREA
TEL 02-569-5830
PROGRMED BY JANG-K.Y
GRAPHIC DESIGN  KIM-Y.H & LEE-H.M

(and in the sound program)
SOUND DIRECTOR  LEE-S.O

the screen says VH-K October 1995, are VH-K another company involved?

---------------------

Egghunt by Invi Image

PCB marked "Invi Image Co. 1995 IZ80B-1"
The pcb has poor quality and resemble a bootleg.

2x Z80
1x AD65 (oki 6295 probably)
1x OSc 12mhz
1x OSC 30mhz
1x FPGA
2x Dipswitch

Note: rom 3 has the 16th pin overlapped and soldered to 15th....it seems it's a manufacturer choice since pin 16 on the socket is not connected with any trace on the pcb
I dumped it with this configuration. In case I'll redump it desoldering pin 16 from 15

*/

#include "driver.h"
#include "sound/okim6295.h"

UINT8 *egghunt_bgram;
UINT8 *egghunt_atram;
UINT8 *egghunt_spram;
UINT8 egghunt_vidram_bank;

static tilemap *bg_tilemap;
static UINT8 egghunt_okibanking;
static UINT8 egghunt_gfx_banking;

static void draw_sprites(mame_bitmap *bitmap,const rectangle *cliprect)
{
	int flipscreen = 0;
	int offs,sx,sy;

	
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
		TRANSPARENCY_PEN, 	// transparency
		15, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0x1000-0x40;offs >= 0;offs -= 0x20)
	{
		int code = egghunt_spram[offs];
		int attr = egghunt_spram[offs+1];
		int color = attr & 0x0f;
		sx = egghunt_spram[offs+3] + ((attr & 0x10) << 4);
		sy = ((egghunt_spram[offs+2] + 8) & 0xff) - 8;
		code += (attr & 0xe0) << 3;

		if(attr & 0xe0)
		{
			switch(egghunt_gfx_banking & 0x30)
			{
	//          case 0x00:
	//          case 0x10: code += 0; break;
				case 0x20: code += 0x400; break;
				case 0x30: code += 0x800; break;
			}
		}

		if (flipscreen)
		{
			sx = 496 - sx;
			sy = 240 - sy;
		}
		
		dgp0.code = code;
		dgp0.color = color;
		dgp0.flipx = flipscreen;
		dgp0.flipy = flipscreen;
		dgp0.sx = sx;
		dgp0.sy = sy;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph

}

static void get_bg_tile_info(int tile_index)
{
	int code = ((egghunt_bgram[tile_index*2+1] << 8) | egghunt_bgram[tile_index*2]) & 0x3fff;
	int colour = egghunt_atram[tile_index];

	if(code & 0x2000)
	{
		if((egghunt_gfx_banking & 3) == 2)
			code += 0x2000;
		else if((egghunt_gfx_banking & 3) == 3)
			code += 0x4000;
//      else if((egghunt_gfx_banking & 3) == 1)
//          code += 0;
	}

	SET_TILE_INFO(0, code, colour, 0)
}

READ8_HANDLER( egghunt_bgram_r )
{
	if (egghunt_vidram_bank)
	{
		return egghunt_spram[offset];
	}
	else
	{
		return egghunt_bgram[offset];
	}
}

WRITE8_HANDLER( egghunt_bgram_w )
{
	if (egghunt_vidram_bank)
	{
		egghunt_spram[offset] = data;
	}
	else
	{
		egghunt_bgram[offset] = data;
		tilemap_mark_tile_dirty(bg_tilemap,offset/2);
	}
}

WRITE8_HANDLER( egghunt_atram_w )
{
	egghunt_atram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap,offset);
}


VIDEO_START(egghunt)
{
	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,64, 32);
	egghunt_bgram = auto_malloc(0x1000);
	egghunt_spram = auto_malloc(0x1000);

	return 0;
}

VIDEO_UPDATE(egghunt)
{
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect);
}

WRITE8_HANDLER( egghunt_gfx_banking_w )
{
	// data & 0x03 is used for tile banking
	// data & 0x30 is used for sprites banking
	egghunt_gfx_banking = data & 0x33;

	tilemap_mark_all_tiles_dirty(bg_tilemap);
}

WRITE8_HANDLER( egghunt_vidram_bank_w )
{
	egghunt_vidram_bank = data & 1;
}

WRITE8_HANDLER( egghunt_soundlatch_w )
{
	soundlatch_w(0,data);
	cpunum_set_input_line(1,0,HOLD_LINE);
}

READ8_HANDLER( egghunt_okibanking_r )
{
	return egghunt_okibanking;
}

WRITE8_HANDLER( egghunt_okibanking_w )
{
	egghunt_okibanking = data;
	OKIM6295_set_bank_base(0, (data & 0x10) ? 0x40000 : 0);
}

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xd000, 0xdfff) AM_READ(egghunt_bgram_r)
	AM_RANGE(0xe000, 0xffff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(paletteram_xRRRRRGGGGGBBBBB_le_w) AM_BASE(&paletteram)
	AM_RANGE(0xc800, 0xcfff) AM_WRITE(egghunt_atram_w) AM_BASE(&egghunt_atram)
	AM_RANGE(0xd000, 0xdfff) AM_WRITE(egghunt_bgram_w)
	AM_RANGE(0xe000, 0xffff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(input_port_0_r)
	AM_RANGE(0x01, 0x01) AM_READ(input_port_1_r)
	AM_RANGE(0x02, 0x02) AM_READ(input_port_2_r)
	AM_RANGE(0x03, 0x03) AM_READ(input_port_3_r)
	AM_RANGE(0x04, 0x04) AM_READ(input_port_4_r)
	AM_RANGE(0x06, 0x06) AM_READ(input_port_5_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(egghunt_vidram_bank_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(egghunt_gfx_banking_w)
	AM_RANGE(0x03, 0x03) AM_WRITE(egghunt_soundlatch_w)
	AM_RANGE(0x06, 0x06) AM_WRITENOP
	AM_RANGE(0x07, 0x07) AM_WRITENOP
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xe000, 0xe000) AM_READ(soundlatch_r)
	AM_RANGE(0xe004, 0xe004) AM_READ(OKIM6295_status_0_r)
	AM_RANGE(0xf000, 0xffff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xe001, 0xe001) AM_READWRITE(egghunt_okibanking_r, egghunt_okibanking_w)
	AM_RANGE(0xe004, 0xe004) AM_WRITE(OKIM6295_data_0_w)
	AM_RANGE(0xf000, 0xffff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END


INPUT_PORTS_START( egghunt )
	PORT_START	/* 8bit */
	PORT_DIPNAME( 0x01, 0x01, "Debug Mode" ) // Run all the animations
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
	PORT_DIPNAME( 0x60, 0x60, "Credits per Player" )
	PORT_DIPSETTING(    0x60, "1" )
//  PORT_DIPSETTING(    0x20, "2" ) /* One of these maybe 2 to start 1 to continue */
	PORT_DIPSETTING(    0x40, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )


	PORT_START	/* 8bit */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY

	PORT_START      /* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)

	PORT_START	/* 8bit */
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
	PORT_DIPNAME( 0x80, 0x00, "Censor Pictures" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START	/* 8bit */
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



static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(2,4),RGN_FRAC(3,4),RGN_FRAC(0,4),RGN_FRAC(1,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout tiles16x16_layout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(2,4),RGN_FRAC(3,4),RGN_FRAC(0,4),RGN_FRAC(1,4) },
	{  4, 5, 6, 7,0, 1, 2, 3, 16*8+4,16*8+5,16*8+6,16*8+7,16*8+0,16*8+1,16*8+2,16*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	16*16
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tiles8x8_layout, 0, 64 },
	{ REGION_GFX2, 0, &tiles16x16_layout, 0, 64 },
	{ -1 }
};


static MACHINE_RESET( egghunt )
{
	egghunt_gfx_banking = 0;
	egghunt_okibanking = 0;
}

static MACHINE_DRIVER_START( egghunt )
	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,12000000/2)		 /* 6 MHz ?*/
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1) // or 2 like mitchell.c?

	MDRV_CPU_ADD(Z80,12000000/2)		 /* 6 MHz ?*/
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(egghunt)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER )
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, 56*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(0x800)

	MDRV_VIDEO_START(egghunt)
	MDRV_VIDEO_UPDATE(egghunt)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 8000) // ?
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

ROM_START( egghunt )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )
	ROM_LOAD( "prg.bin", 0x00000, 0x20000, CRC(eb647145) SHA1(792951b76b5fac01e72ae34a2fe2108e373c5b62) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "rom2.bin", 0x00000, 0x10000, CRC(88a71bc3) SHA1(cf5acccfda9fda0d55af91a415a54391d0d0b7a2) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_INVERT )
	ROM_LOAD( "rom3.bin", 0x00000, 0x40000, CRC(9d51ac49) SHA1(b0785d746fb2872a04386016ffdee80e6174dfc0) )
	ROM_LOAD( "rom4.bin", 0x40000, 0x40000, CRC(41c63041) SHA1(24e9a21d448c144db2356329cf87dc99598c96dc) )
	ROM_LOAD( "rom5.bin", 0x80000, 0x40000, CRC(6f96cb97) SHA1(7dde7d2aec6b5f9929b98d06c07bb07bf7bd59dd) )
	ROM_LOAD( "rom6.bin", 0xc0000, 0x40000, CRC(b5a41d4b) SHA1(1b4cf9c944e3eb7dc2d26d8a73bf5efb7b53253a) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_INVERT )
	ROM_LOAD( "rom7.bin", 0x00000, 0x20000, CRC(1b43fb57) SHA1(f06e186bf514f2ad655df23636eab72e6fafd815) )
	ROM_LOAD( "rom8.bin", 0x20000, 0x20000, CRC(f8122d0d) SHA1(78551c689b9e4eeed5e1ae97d8c7a907a388a9ff) )
	ROM_LOAD( "rom9.bin", 0x40000, 0x20000, CRC(dbfa0ffe) SHA1(2aa759c0bd3945473a6d8fa48226ce6c6c94d740) )
	ROM_LOAD( "rom10.bin",0x60000, 0x20000, CRC(14f5fc74) SHA1(769bccf9c1b42c35c3aee3866ed015de7c83b710) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 )
	ROM_LOAD( "rom1.bin", 0x00000, 0x80000, CRC(f03589bc) SHA1(4d9c8422ac3c4c3ecba3bcf0ed47b8c7d5903f8c) )
ROM_END

GAME( 1995, egghunt, 0, egghunt, egghunt, 0, ROT0, "Invi Image", "Egg Hunt", 0 ,0)
