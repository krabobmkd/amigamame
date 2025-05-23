/***************************************************************************

                          -= Power Instinct =-
                            (C) 1993 Atlus

                driver by   Luca Elia (l.elia@tin.it)

Set 1
    CPU:    MC68000, Z80 (for sound)
    Sound:  2x OKI6295 + 1x YM2203
Set 2
    CPU:    MC68000
    Sound:  OKIM6295
Set 3
    CPU:    MC68000, Z80 (for sound)
    Sound:  2x OKI6295 (Sound code supports an additional YM2203, but it's not fitted)

- Note: To enter test mode press F2 (Test)
        Use 9 (Service Coin) to change page.

TODO:
- sprites flip y (not used by the game)


***************************************************************************/

#include "driver.h"
#include "machine/nmk112.h"
#include "sound/okim6295.h"
#include "sound/2203intf.h"

/* Variables that vidhrdw has access to */

/* Variables defined in vidhrdw */
extern UINT16 *powerins_vram_0, *powerins_vctrl_0;
extern UINT16 *powerins_vram_1, *powerins_vctrl_1;

/* Functions defined in vidhrdw */

WRITE16_HANDLER( powerins_flipscreen_w );
WRITE16_HANDLER( powerins_tilebank_w );

WRITE16_HANDLER( powerins_paletteram16_w );

WRITE16_HANDLER( powerins_vram_0_w );
WRITE16_HANDLER( powerins_vram_1_w );

VIDEO_START( powerins );
VIDEO_UPDATE( powerins );


/***************************************************************************

                                Memory Maps

***************************************************************************/

static int oki_bank;

static WRITE16_HANDLER( powerins_okibank_w )
{
	if (ACCESSING_LSB)
	{
		unsigned char *RAM = memory_region(REGION_SOUND1);
		int new_bank = data & 0x7;

		if (new_bank != oki_bank)
		{
			oki_bank = new_bank;
			memcpy(&RAM[0x30000],&RAM[0x40000 + 0x10000*new_bank],0x10000);
		}
	}
}

static WRITE16_HANDLER( powerins_soundlatch_w )
{
	if (ACCESSING_LSB)
	{
		soundlatch_w(0, data & 0xff);
	}
}
static READ8_HANDLER( powerinb_fake_ym2203_r )
{
	return 0x01;
}


static ADDRESS_MAP_START( powerins_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_READ(MRA16_ROM				)	// ROM
	AM_RANGE(0x100000, 0x100001) AM_READ(input_port_0_word_r		)	// Coins + Start Buttons
	AM_RANGE(0x100002, 0x100003) AM_READ(input_port_1_word_r		)	// P1 + P2
	AM_RANGE(0x100008, 0x100009) AM_READ(input_port_2_word_r		)	// DSW 1
	AM_RANGE(0x10000a, 0x10000b) AM_READ(input_port_3_word_r		)	// DSW 2
	AM_RANGE(0x10003e, 0x10003f) AM_READ(OKIM6295_status_0_lsb_r	)	// OKI Status (used by powerina)
	AM_RANGE(0x120000, 0x120fff) AM_READ(MRA16_RAM				)	// Palette
/**/AM_RANGE(0x130000, 0x130007) AM_READ(MRA16_RAM				)	// VRAM 0 Control
	AM_RANGE(0x140000, 0x143fff) AM_READ(MRA16_RAM				)	// VRAM 0
	AM_RANGE(0x170000, 0x170fff) AM_READ(MRA16_RAM				)	// VRAM 1
	AM_RANGE(0x180000, 0x18ffff) AM_READ(MRA16_RAM				)	// RAM + Sprites
ADDRESS_MAP_END

static ADDRESS_MAP_START( powerins_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_WRITE(MWA16_ROM								)	// ROM
	AM_RANGE(0x100014, 0x100015) AM_WRITE(powerins_flipscreen_w					)	// Flip Screen
	AM_RANGE(0x100016, 0x100017) AM_WRITE(MWA16_NOP								)	// ? always 1
	AM_RANGE(0x100018, 0x100019) AM_WRITE(powerins_tilebank_w						)	// Tiles Banking (VRAM 0)
	AM_RANGE(0x10001e, 0x10001f) AM_WRITE(powerins_soundlatch_w					)	// Sound Latch
	AM_RANGE(0x100030, 0x100031) AM_WRITE(powerins_okibank_w						)	// Sound
	AM_RANGE(0x10003e, 0x10003f) AM_WRITE(OKIM6295_data_0_lsb_w					)	// used by powerina
	AM_RANGE(0x120000, 0x120fff) AM_WRITE(powerins_paletteram16_w) AM_BASE(&paletteram16	)	// Palette
	AM_RANGE(0x130000, 0x130007) AM_WRITE(MWA16_RAM) AM_BASE(&powerins_vctrl_0			)	// VRAM 0 Control
	AM_RANGE(0x140000, 0x143fff) AM_WRITE(powerins_vram_0_w) AM_BASE(&powerins_vram_0		)	// VRAM 0
	AM_RANGE(0x170000, 0x170fff) AM_WRITE(powerins_vram_1_w) AM_BASE(&powerins_vram_1		)	// VRAM 1
	AM_RANGE(0x171000, 0x171fff) AM_WRITE(powerins_vram_1_w						)	// Mirror of VRAM 1?
	AM_RANGE(0x180000, 0x18ffff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16					)	// RAM + Sprites
ADDRESS_MAP_END

/* In powerina there is a hidden test mode screen because it's a bootleg
   without a sound CPU. Set 18ff08 to 4 during test mode that calls the
   data written to $10001e "sound code". */

static ADDRESS_MAP_START( readmem_snd, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xdfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xe000) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_snd, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xdfff) AM_WRITE(MWA8_RAM)
//  AM_RANGE(0xe000, 0xe000) AM_WRITE(MWA8_NOP) // ? written only once ?
//  AM_RANGE(0xe001, 0xe001) AM_WRITE(MWA8_NOP) // ?
ADDRESS_MAP_END

static ADDRESS_MAP_START( powerins_io_snd, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READWRITE(YM2203_status_port_0_r, YM2203_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_READWRITE(YM2203_read_port_0_r, YM2203_write_port_0_w)
	AM_RANGE(0x80, 0x80) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)
	AM_RANGE(0x88, 0x88) AM_READWRITE(OKIM6295_status_1_r, OKIM6295_data_1_w)
	AM_RANGE(0x90, 0x97) AM_WRITE(NMK112_okibank_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( powerinb_io_snd, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READWRITE(powerinb_fake_ym2203_r, MWA8_NOP)
	AM_RANGE(0x01, 0x01) AM_NOP
	AM_RANGE(0x80, 0x80) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)
	AM_RANGE(0x88, 0x88) AM_READWRITE(OKIM6295_status_1_r, OKIM6295_data_1_w)
	AM_RANGE(0x90, 0x97) AM_WRITE(NMK112_okibank_w)
ADDRESS_MAP_END

/***************************************************************************

                                Input Ports

***************************************************************************/

INPUT_PORTS_START( powerins )
	PORT_START	// IN0 - $100000 - Coins
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(0x0020, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME( DEF_STR( Service_Mode )) PORT_CODE(KEYCODE_F2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START	// IN1 - $100002 - Player 1 & 2
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	 ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	 ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)

	PORT_START	// IN2 - $100008 - DSW 1
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x000e, 0x000e, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 2C_1C ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( 2C_1C ) ) /* 2C to start, 1C to continue */
	PORT_DIPSETTING(      0x000e, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x000a, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x0070, 0x0070, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0060, DEF_STR( 2C_1C ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( 2C_1C ) ) /* 2C to start, 1C to continue */
	PORT_DIPSETTING(      0x0070, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0050, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On )  )

	PORT_START	// IN3 - $10000a - DSW 2
	PORT_DIPNAME( 0x0001, 0x0001, "Coin Chutes" )
	PORT_DIPSETTING(      0x0001, "1 Chute" )
	PORT_DIPSETTING(      0x0000, "2 Chutes" )
	PORT_DIPNAME( 0x0002, 0x0002, "Join In Mode" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
/*
    In "Join In" mode, a second player can join even if one player has aready
    begun to play.  Please refer to chart below:

    Join In Mode    Credit          Join In     Game Over
    -----------------------------------------------------------------------------------------------
    Join In ON  1C per Player       Anytime     Winner of VS Plays Computer
    Join In OFF 1C = VS Mode 2 players  Cannot      After win VS Game Over for both players

*/
	PORT_DIPNAME( 0x0004, 0x0000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, "Blood Color" )
	PORT_DIPSETTING(      0x0010, "Red" )
	PORT_DIPSETTING(      0x0000, "Blue" )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Game_Time ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0000, "Short" )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x00c0, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
INPUT_PORTS_END



/***************************************************************************

                                Graphics Layouts

***************************************************************************/

/* 8x8x4 tiles */
static const gfx_layout layout_8x8x4 =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{0,1,2,3},
	{0*4,1*4,2*4,3*4,4*4,5*4,6*4,7*4},
	{0*32,1*32,2*32,3*32,4*32,5*32,6*32,7*32},
	8*8*4
};


/* 16x16x4 tiles (made of four 8x8 tiles) */
static const gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{0,1,2,3},
	{0*4,1*4,2*4,3*4,4*4,5*4,6*4,7*4,
	 128*4,129*4,130*4,131*4,132*4,133*4,134*4,135*4},
	{0*32,1*32,2*32,3*32,4*32,5*32,6*32,7*32,
	 8*32,9*32,10*32,11*32,12*32,13*32,14*32,15*32},
	16*16*4
};


/* 16x16x4 tiles (made of four 8x8 tiles). The bytes are swapped */
static const gfx_layout layout_16x16x4_swap =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{0,1,2,3},
	{2*4,3*4,0*4,1*4,6*4,7*4,4*4,5*4,
	 130*4,131*4,128*4,129*4,134*4,135*4,132*4,133*4},
	{0*32,1*32,2*32,3*32,4*32,5*32,6*32,7*32,
	 8*32,9*32,10*32,11*32,12*32,13*32,14*32,15*32},
	16*16*4
};


static const gfx_decode powerins_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_16x16x4,      0x000, 0x20 }, // [0] Tiles
	{ REGION_GFX2, 0, &layout_8x8x4,        0x200, 0x10 }, // [1] Tiles
	{ REGION_GFX3, 0, &layout_16x16x4_swap, 0x400, 0x40 }, // [2] Sprites
	{ -1 }
};






/***************************************************************************

                                Machine Drivers

***************************************************************************/

MACHINE_RESET( powerins )
{
	oki_bank = -1;	// samples bank "unitialised"
}


static void irqhandler(int irq)
{
	cpunum_set_input_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2203interface ym2203_interface =
{
	0, 0, 0, 0,
	irqhandler
};

static MACHINE_DRIVER_START( powerins )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 12000000)	/* 12MHz */
	MDRV_CPU_PROGRAM_MAP(powerins_readmem,powerins_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD_TAG("sound", Z80, 6000000) /* 6 MHz */
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_snd,writemem_snd)
	MDRV_CPU_IO_MAP(powerins_io_snd,0)

	MDRV_FRAMES_PER_SECOND(56)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(powerins)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(320, 256)
	MDRV_VISIBLE_AREA(0, 320-1, 0+16, 256-16-1)
	MDRV_GFXDECODE(powerins_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(powerins)
	MDRV_VIDEO_UPDATE(powerins)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD_TAG("oki1", OKIM6295, 4000000 / 165)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD_TAG("oki2", OKIM6295, 4000000 / 165)
	MDRV_SOUND_CONFIG(okim6295_interface_region_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD_TAG("ym2203", YM2203, 12000000 / 8)
	MDRV_SOUND_CONFIG(ym2203_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.5)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( powerina )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(powerins)

	MDRV_FRAMES_PER_SECOND(60)

	MDRV_CPU_REMOVE("sound")

	MDRV_SOUND_REPLACE("oki1", OKIM6295, 6000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_REMOVE("oki2")
	MDRV_SOUND_REMOVE("ym2203")
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( powerinb )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(powerins)

	MDRV_FRAMES_PER_SECOND(60)

	MDRV_CPU_MODIFY("sound") /* 6 MHz */
	/* audio CPU */
	MDRV_CPU_IO_MAP(powerinb_io_snd,0)
	MDRV_CPU_PERIODIC_INT(irq0_line_hold, TIME_IN_HZ(120))	// YM2203 rate is at 150??

	MDRV_SOUND_REMOVE("ym2203")	// Sound code talks to one, but it's not fitted on the board
MACHINE_DRIVER_END


/***************************************************************************

                                ROMs Loading

***************************************************************************/

/*

Gouketsuji Ichizoku (Power Instinct Japan)
Atlus, 1993

PCB Layout
----------

OS93095 (C) ATLUS 1993 MADE IN JAPAN
|---------------------------------------------------------------|
|LA4460  VOL  YM2203 Z80   93095-2  12MHz      AAA64K1P-35(x8)  |
|4558 YM3014  M6295          6264                               |
|                  93095-11 |------|          CXK58258BP-35L(x8)|
|          M6295   93095-10 |NMK112|                            |
|                  93095-9  |      |                            |
|   16MHz          93095-8  |------|           |------||------| |
|    DSW1  DSW2   |------|    22               |NMK009||NMK009| |
|J                |NMK005|                     |      ||      | |
|A                |      |                     |------||------| |
|M                |------|            |------|                  |
|M                            6116    |NMK008|         93095-19 |
|A |------|6116               6116    |      |                  |
|  |NMK111|6116                       |------|         93095-18 |
|  |      |       |------|                                      |
|  |------|       |NMK901|    62256    93095-4         93095-17 |
| 6264            |      |    62256    93095-3J                 |
| 6264    93095-7 |------|          |------------|     93095-16 |
||------|                    20     |   68000    |              |
||NMK111| 93095-6 |---| |---|       |------------|     93095-15 |
||      |         |NMK| |NMK|                                   |
||------| 93095-5 |903| |903|                          93095-14 |
||------|         |---| |---||---| 21                           |
||NMK111| 93095-1            |NMK|                     93095-13 |
||      |          6116      |902|    14MHz                     |
||------|          6116      |---|                     93095-12 |
|---------------------------------------------------------------|
Notes:
      68000 clock - 12.000MHz
      Z80 clock   - 6.000MHz [12/2]
      6295 clocks - 4.000MHz [16/4], sample rate = 4000000 / 165
      YM2203 clock- 1.500MHz [12/8]
      VSync       - 56Hz
      HSync       - 15.35kHz

      ROMs -
            -1, -2   : 27C1001 EPROM
            -3, -4   : 27C4096 EPROM
            -5, -6   : 8M 42 pin MASKROM (578200)
            -7       : 4M 40 pin MASKROM (574200)
            -8 to -19: 8M 42 pin MASKROM (578200)
            20       : 82S129 PROM
            21       : 82S135 PROM
            22       : 82S123 PROM

*/

ROM_START( powerins )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_WORD_SWAP( "93095-3j.u108", 0x00000, 0x80000, CRC(3050a3fb) SHA1(e7e729bf62266e2e78ccd84cf937abb99de18ad5) )
	ROM_LOAD16_WORD_SWAP( "93095-4.u109",  0x80000, 0x80000, CRC(d3d7a782) SHA1(7846de0ebb09bd9b2534cd451ff9aa5175e60647) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "93095-2.u90",  0x00000, 0x20000, CRC(4b123cc6) SHA1(ed61d3a2ab20c86b91fd7bafa717be3ce26159be) )

	ROM_REGION( 0x280000, REGION_GFX1, ROMREGION_DISPOSE )	/* Layer 0 */
	ROM_LOAD( "93095-5.u16",  0x000000, 0x100000, CRC(b1371808) SHA1(15fca313314ff2e0caff35841a2fdda97f6235a8) )
	ROM_LOAD( "93095-6.u17",  0x100000, 0x100000, CRC(29c85d80) SHA1(abd54f9c8bade21ea918a426627199da04193165) )
	ROM_LOAD( "93095-7.u18",  0x200000, 0x080000, CRC(2dd76149) SHA1(975e4d371fdfbbd9a568da4d4c91ffd3f0ae636e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 1 */
	ROM_LOAD( "93095-1.u15",  0x000000, 0x020000, CRC(6a579ee0) SHA1(438e87b930e068e0cf7352e614a14049ebde6b8a) )

	ROM_REGION( 0x800000, REGION_GFX3, ROMREGION_DISPOSE )	/* Sprites */
	ROM_LOAD( "93095-12.u116", 0x000000, 0x100000, CRC(35f3c2a3) SHA1(70efebfe248401ba3d766dc0e4bcc2846cd0d9a0) )
	ROM_LOAD( "93095-13.u117", 0x100000, 0x100000, CRC(1ebd45da) SHA1(99b0ac734890673064b2a4b4b57ff2694e338dea) )
	ROM_LOAD( "93095-14.u118", 0x200000, 0x100000, CRC(760d871b) SHA1(4887122ad0518c90f08c11a7a6b694f3fd218498) )
	ROM_LOAD( "93095-15.u119", 0x300000, 0x100000, CRC(d011be88) SHA1(837409a2584abdf22e022b0f06181a600a974cbe) )
	ROM_LOAD( "93095-16.u120", 0x400000, 0x100000, CRC(a9c16c9c) SHA1(a34e81324c875c2a57f778d1dbdda8da81850a29) )
	ROM_LOAD( "93095-17.u121", 0x500000, 0x100000, CRC(51b57288) SHA1(821473d51565bc0a8b9a979723ce1307b97e517e) )
	ROM_LOAD( "93095-18.u122", 0x600000, 0x100000, CRC(b135e3f2) SHA1(339fb4007ca0f379b7554a1c4f711f494a371fb2) )
	ROM_LOAD( "93095-19.u123", 0x700000, 0x100000, CRC(67695537) SHA1(4c78ce3e36f27d2a6a9e50e8bf896335d4d0958a) )

	ROM_REGION( 0x240000, REGION_SOUND1, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "93095-10.u48", 0x040000, 0x100000, CRC(329ac6c5) SHA1(e809b94e2623141f5a48995cfa97fe1ead7ab40b) )
	ROM_LOAD( "93095-11.u49", 0x140000, 0x100000, CRC(75d6097c) SHA1(3c89a7c9b12087e2d969b822419d3e5f98f5cb1d) )

	ROM_REGION( 0x240000, REGION_SOUND2, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "93095-8.u46",  0x040000, 0x100000, CRC(f019bedb) SHA1(4b6e10f85671c75b666e547887d403d6e607cec8) )
	ROM_LOAD( "93095-9.u47",  0x140000, 0x100000, CRC(adc83765) SHA1(9e760443f9de21c1bb7e33eaa1541023fcdc60ab) )

	ROM_REGION( 0x0220, REGION_PROMS, 0 )		/* unknown */
	ROM_LOAD( "22.u81",       0x000000, 0x0020, CRC(67d5ec4b) SHA1(87d32948a0c88277dcdd0eaa035bde40fc7db5fe) )
	ROM_LOAD( "21.u71",       0x000020, 0x0100, CRC(182cd81f) SHA1(3a76bea81b34ea7ccf56044206721058aa5b03e6) )
	ROM_LOAD( "20.u54",       0x000100, 0x0100, CRC(38bd0e2f) SHA1(20d311869642cd96bb831fdf4a458e0d872f03eb) )
ROM_END

/***************************************************************************

                                Power Instinct

Location     Device       File ID     Checksum
----------------------------------------------
             27C240        ROM1         4EA1    [ MAIN PROGRAM ]
             27C240        ROM2         FE60    [ PROGRAM DATA ]
             27C010        ROM3         B9F7    [  CHARACTER   ]
             27C040        ROM4         2780    [  BACKGROUND  ]
             27C040        ROM5         98E0    [   PCM DATA   ]
            23C1600        ROM6         D9E9    [  BACKGROUND  ]
            23C1600        ROM7         8B04    [  MOTION OBJ  ]
            23C1600        ROM8         54B2    [  MOTION OBJ  ]
            23C1600        ROM9         C7C8    [  MOTION OBJ  ]
            23C1600        ROM10        852A    [  MOTION OBJ  ]

Notes:  This archive is of a bootleg version

Brief hardware overview
-----------------------

Main processor  -  68000
                -  TPC1020AFN-084C (CPLD)

Sound processor -  Main processor
                -  K-665-9249      (M6295)

***************************************************************************/

ROM_START( powerina )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_WORD_SWAP( "rom1", 0x000000, 0x080000, CRC(b86c84d6) SHA1(2ec0933130925dfae859ea6abe62a8c92385aee8) )
	ROM_LOAD16_WORD_SWAP( "rom2", 0x080000, 0x080000, CRC(d3d7a782) SHA1(7846de0ebb09bd9b2534cd451ff9aa5175e60647) )

	ROM_REGION( 0x280000, REGION_GFX1, ROMREGION_DISPOSE )	/* Layer 0 */
	ROM_LOAD( "rom6",  0x000000, 0x200000, CRC(b6c10f80) SHA1(feece0aeaa01a455d0c4885a3699f8bda14fe00f) )
	ROM_LOAD( "rom4",  0x200000, 0x080000, CRC(2dd76149) SHA1(975e4d371fdfbbd9a568da4d4c91ffd3f0ae636e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 1 */
	ROM_LOAD( "rom3",  0x000000, 0x020000, CRC(6a579ee0) SHA1(438e87b930e068e0cf7352e614a14049ebde6b8a) )

	ROM_REGION( 0x800000, REGION_GFX3, ROMREGION_DISPOSE )	/* Sprites */
	ROM_LOAD( "rom10", 0x000000, 0x200000, CRC(efad50e8) SHA1(89e8c307b927e987a32d22ab4ab7f3be037cca03) )
	ROM_LOAD( "rom9",  0x200000, 0x200000, CRC(08229592) SHA1(759679e89832b475adfdc783630d9ee2c105b0f3) )
	ROM_LOAD( "rom8",  0x400000, 0x200000, CRC(b02fdd6d) SHA1(1e2c52b4e9999f0b564fcf13ff41b097ad7d0c39) )
	ROM_LOAD( "rom7",  0x600000, 0x200000, CRC(92ab9996) SHA1(915ec8f383cc3652c3816a9b56ee54e22e104a5c) )

	ROM_REGION( 0x090000, REGION_SOUND1, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "rom5", 0x000000, 0x030000, CRC(88579c8f) SHA1(13083934ab294c9b08d3e36f55c00a6a2e5a0507) )
	ROM_CONTINUE(     0x040000, 0x050000 )
ROM_END

/***************************************************************************

Power Instinct
Atlus, 1993

This is a bootleg US version with different sound hardware to the existing bootleg set.
The PCB is very large and has 2 plug-in daughterboards and many MASK ROMs.
The addition of the contents of the MASK ROMs would probably equal the contents of presumably
larger MASK ROMs found on the original PCB....

PCB Layout

|-------------------------------------------------------------|
|   M6295    4A 5A                             62256  62256   |
|   M6295    4B 5B                             62256  62256   |
|            4C 5C                             62256  62256   |
|            4D 5D                             62256  62256   |
| Z80        16MHz                             62256  62256   |
| 1F                                           62256  62256   |
| 6264       6116                              62256  62256   |
|            6116                              62256  62256   |
|J                                                            |
|A                                                            |
|M                                                            |
|M                                                            |
|A                                    82S123  11G 12G 13G  14G|
|                                             11I             |
|                           TPC1020  6116     11J             |
|DSW1        6116  6N                6116     11K     13K     |
|DSW2        6116              6264           11L     13L  14M|
|                      82S147  6264           11O     13O  14N|
|                                             11P 12P 13P  14P|
|      2Q    62256                            11Q     13Q     |
|      2R    62256                                    13R     |
|      68000                                                  |
| 12MHz              14.31818MHz                              |
|-------------------------------------------------------------|

Notes:
      68000 clock: 12.000MHz
      Z80 clock  :  6.000MHz
      M6295 clock:  4.000MHz (both); sample rate = 4000000/165 (both)
      VSync      :  60Hz

      ROMs 1F and 6N are 1M MASK (MX27C1000), all other ROMs are 4M MASK (MX27C4000).
      ROMS at 5* are located on a plug-in daughterboard.
      ROMS at 11*, 12*, 13G, 13P and 14* are located on a plug-in daughterboard.
      82S123 and 82S147 are PROMs.

***************************************************************************/

ROM_START( powerinb )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )		/* 68000 Code */
	ROM_LOAD16_BYTE( "2q.bin", 0x000000, 0x80000, CRC(11bf3f2a) SHA1(c840add78da9b19839c667f9bbd77e0a7c560ed7) )
	ROM_LOAD16_BYTE( "2r.bin", 0x000001, 0x80000, CRC(d8d621be) SHA1(91d501ac661c1ff52c85eee96c455c008a7dad1c) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "1f.bin",  0x000000, 0x20000, CRC(4b123cc6) SHA1(ed61d3a2ab20c86b91fd7bafa717be3ce26159be) )

	ROM_REGION( 0x280000, REGION_GFX1, ROMREGION_DISPOSE )	/* Layer 0 */
	ROM_LOAD( "13k.bin", 0x000000, 0x80000, CRC(1975b4b8) SHA1(cb400967744fa602df1bd2d88950dfdbdc77073f) )
	ROM_LOAD( "13l.bin", 0x080000, 0x80000, CRC(376e4919) SHA1(12baa17382c176838df1b5ef86f1fa6dbcb978dd) )
	ROM_LOAD( "13o.bin", 0x100000, 0x80000, CRC(0d5ff532) SHA1(4febdb9cdacd85903a4a28e8df945dee0ce85558) )
	ROM_LOAD( "13q.bin", 0x180000, 0x80000, CRC(99b25791) SHA1(82f4bb5780826773d2e5f7143afb3ba209f57652) )
	ROM_LOAD( "13r.bin", 0x200000, 0x80000, CRC(2dd76149) SHA1(975e4d371fdfbbd9a568da4d4c91ffd3f0ae636e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 1 */
	ROM_LOAD( "6n.bin", 0x000000, 0x20000, CRC(6a579ee0) SHA1(438e87b930e068e0cf7352e614a14049ebde6b8a) )

	ROM_REGION( 0x800000, REGION_GFX3, ROMREGION_DISPOSE )	/* Sprites */
	ROM_LOAD16_BYTE( "14g.bin", 0x000000, 0x80000, CRC(8b9b89c9) SHA1(f1d39d1a62e40a14642d8f22fc38b764465a8daa) )
	ROM_LOAD16_BYTE( "11g.bin", 0x000001, 0x80000, CRC(4d127bdf) SHA1(26a7c277e7660a7c7c0c11cacadf815d2487ba8a) )
	ROM_LOAD16_BYTE( "13g.bin", 0x100000, 0x80000, CRC(298eb50e) SHA1(2b922c1473bb559a1e8bd6221619141658179bb9) )
	ROM_LOAD16_BYTE( "11i.bin", 0x100001, 0x80000, CRC(57e6d283) SHA1(4701576c8663ba47f388a02e61ef078a9dbbd212) )
	ROM_LOAD16_BYTE( "12g.bin", 0x200000, 0x80000, CRC(fb184167) SHA1(20924d3f35509f2f6af61f565b852ea72326d02c) )
	ROM_LOAD16_BYTE( "11j.bin", 0x200001, 0x80000, CRC(1b752a4d) SHA1(1b13f28af208542bee9da298d6e9db676cbc0845) )
	ROM_LOAD16_BYTE( "14m.bin", 0x300000, 0x80000, CRC(2f26ba7b) SHA1(026f960fa4de09ed940dd83a3db467c3676c5024) )
	ROM_LOAD16_BYTE( "11k.bin", 0x300001, 0x80000, CRC(0263d89b) SHA1(526b8ed05dffcbe98a44372bd55ad7b0ba91fc0f) )
	ROM_LOAD16_BYTE( "14n.bin", 0x400000, 0x80000, CRC(c4633294) SHA1(9578f516eaf09e743ee0262ce227f811bea1be8f) )
	ROM_LOAD16_BYTE( "11l.bin", 0x400001, 0x80000, CRC(5e4b5655) SHA1(f86509e75ec0c68f728715a5a325f6d1a30cfd93) )
	ROM_LOAD16_BYTE( "14p.bin", 0x500000, 0x80000, CRC(4d4b0e4e) SHA1(782c5edc533f10757cb18d2411046e44aa075ba1) )
	ROM_LOAD16_BYTE( "11o.bin", 0x500001, 0x80000, CRC(7e9f2d2b) SHA1(cfee03c38a6c781ad370638748244a164b83d588) )
	ROM_LOAD16_BYTE( "13p.bin", 0x600000, 0x80000, CRC(0e7671f2) SHA1(301af5c4229451cba9fdf40285dd7243626ffed4) )
	ROM_LOAD16_BYTE( "11p.bin", 0x600001, 0x80000, CRC(ee59b1ec) SHA1(437bc50c3b32c2edee549f5021345f1c924896b4) )
	ROM_LOAD16_BYTE( "12p.bin", 0x700000, 0x80000, CRC(9ab1998c) SHA1(fadaa4a46cefe0093ee1ebeddbae63143fa7bb5a) )
	ROM_LOAD16_BYTE( "11q.bin", 0x700001, 0x80000, CRC(1ab0c88a) SHA1(8bc72732f5911e0d4e0cf12fd2fb12d67e03299e) )

	ROM_REGION( 0x240000, REGION_SOUND1, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "4a.bin", 0x040000, 0x80000, CRC(8cd6824e) SHA1(aa6d8917558de4f2aa8d80527209b9fe91122eb3) )
	ROM_LOAD( "4b.bin", 0x0c0000, 0x80000, CRC(e31ae04d) SHA1(c08d58a4250d8bdb68b8e5012624f345936520e1) )
	ROM_LOAD( "4c.bin", 0x140000, 0x80000, CRC(c4c9f599) SHA1(1d74acd626406052bec533a918ca24e14a2578f2) )
	ROM_LOAD( "4d.bin", 0x1c0000, 0x80000, CRC(f0a9f0e1) SHA1(4221e0824cdc8bcd6ea1c3811f4e3b7cd99478f2) )

	ROM_REGION( 0x240000, REGION_SOUND2, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "5a.bin", 0x040000, 0x80000, CRC(62557502) SHA1(d72abdaec1c6f55f9b0099b7a8a297e0e14f920c) )
	ROM_LOAD( "5b.bin", 0x0c0000, 0x80000, CRC(dbc86bd7) SHA1(6f1bc3c7e6976fdcd4b2341cea07002fb0cefb14) )
	ROM_LOAD( "5c.bin", 0x140000, 0x80000, CRC(5839a2bd) SHA1(53988086ef97b2671044f6da9d97b1886900b64d) )
	ROM_LOAD( "5d.bin", 0x1c0000, 0x80000, CRC(446f9dc3) SHA1(5c81eb9a7cbea995db9a10d3b6460d02e104825f) )

	ROM_REGION( 0x0220, REGION_PROMS, 0 )		/* unknown */
	ROM_LOAD( "82s123.bin", 0x0000, 0x0020, CRC(67d5ec4b) SHA1(87d32948a0c88277dcdd0eaa035bde40fc7db5fe) )
	ROM_LOAD( "82s147.bin", 0x0020, 0x0200, CRC(d7818542) SHA1(e94f8004c804f260874a117d59dfa0637c5d3d73) )
ROM_END

/* set only contained the 2 program roms */
ROM_START( powernjb )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )		/* 68000 Code */
	/* 32_WORD on a 68k? strange, maybe the bootleg was just hooked up in an unusual way */
	ROM_LOAD32_WORD( "j21.a3", 0x000000, 0x80000, CRC(59594382) SHA1(de89257f15cbd3051d15d4496e8f0105ae8810e3) )
	ROM_LOAD32_WORD( "j22.a3", 0x000002, 0x80000, CRC(598e47e3) SHA1(86b216b52bbe77baf957917d16baf1180a120088) )

	ROM_REGION( 0x20000, REGION_CPU2, 0 )		/* Z80 Code */
	ROM_LOAD( "1f.bin",  0x000000, 0x20000, CRC(4b123cc6) SHA1(ed61d3a2ab20c86b91fd7bafa717be3ce26159be) )

	ROM_REGION( 0x280000, REGION_GFX1, ROMREGION_DISPOSE )	/* Layer 0 */
	ROM_LOAD( "13k.bin", 0x000000, 0x80000, CRC(1975b4b8) SHA1(cb400967744fa602df1bd2d88950dfdbdc77073f) )
	ROM_LOAD( "13l.bin", 0x080000, 0x80000, CRC(376e4919) SHA1(12baa17382c176838df1b5ef86f1fa6dbcb978dd) )
	ROM_LOAD( "13o.bin", 0x100000, 0x80000, CRC(0d5ff532) SHA1(4febdb9cdacd85903a4a28e8df945dee0ce85558) )
	ROM_LOAD( "13q.bin", 0x180000, 0x80000, CRC(99b25791) SHA1(82f4bb5780826773d2e5f7143afb3ba209f57652) )
	ROM_LOAD( "13r.bin", 0x200000, 0x80000, CRC(2dd76149) SHA1(975e4d371fdfbbd9a568da4d4c91ffd3f0ae636e) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )	/* Layer 1 */
	ROM_LOAD( "6n.bin", 0x000000, 0x20000, CRC(6a579ee0) SHA1(438e87b930e068e0cf7352e614a14049ebde6b8a) )

	ROM_REGION( 0x800000, REGION_GFX3, ROMREGION_DISPOSE )	/* Sprites */
	ROM_LOAD16_BYTE( "14g.bin", 0x000000, 0x80000, CRC(8b9b89c9) SHA1(f1d39d1a62e40a14642d8f22fc38b764465a8daa) )
	ROM_LOAD16_BYTE( "11g.bin", 0x000001, 0x80000, CRC(4d127bdf) SHA1(26a7c277e7660a7c7c0c11cacadf815d2487ba8a) )
	ROM_LOAD16_BYTE( "13g.bin", 0x100000, 0x80000, CRC(298eb50e) SHA1(2b922c1473bb559a1e8bd6221619141658179bb9) )
	ROM_LOAD16_BYTE( "11i.bin", 0x100001, 0x80000, CRC(57e6d283) SHA1(4701576c8663ba47f388a02e61ef078a9dbbd212) )
	ROM_LOAD16_BYTE( "12g.bin", 0x200000, 0x80000, CRC(fb184167) SHA1(20924d3f35509f2f6af61f565b852ea72326d02c) )
	ROM_LOAD16_BYTE( "11j.bin", 0x200001, 0x80000, CRC(1b752a4d) SHA1(1b13f28af208542bee9da298d6e9db676cbc0845) )
	ROM_LOAD16_BYTE( "14m.bin", 0x300000, 0x80000, CRC(2f26ba7b) SHA1(026f960fa4de09ed940dd83a3db467c3676c5024) )
	ROM_LOAD16_BYTE( "11k.bin", 0x300001, 0x80000, CRC(0263d89b) SHA1(526b8ed05dffcbe98a44372bd55ad7b0ba91fc0f) )
	ROM_LOAD16_BYTE( "14n.bin", 0x400000, 0x80000, CRC(c4633294) SHA1(9578f516eaf09e743ee0262ce227f811bea1be8f) )
	ROM_LOAD16_BYTE( "11l.bin", 0x400001, 0x80000, CRC(5e4b5655) SHA1(f86509e75ec0c68f728715a5a325f6d1a30cfd93) )
	ROM_LOAD16_BYTE( "14p.bin", 0x500000, 0x80000, CRC(4d4b0e4e) SHA1(782c5edc533f10757cb18d2411046e44aa075ba1) )
	ROM_LOAD16_BYTE( "11o.bin", 0x500001, 0x80000, CRC(7e9f2d2b) SHA1(cfee03c38a6c781ad370638748244a164b83d588) )
	ROM_LOAD16_BYTE( "13p.bin", 0x600000, 0x80000, CRC(0e7671f2) SHA1(301af5c4229451cba9fdf40285dd7243626ffed4) )
	ROM_LOAD16_BYTE( "11p.bin", 0x600001, 0x80000, CRC(ee59b1ec) SHA1(437bc50c3b32c2edee549f5021345f1c924896b4) )
	ROM_LOAD16_BYTE( "12p.bin", 0x700000, 0x80000, CRC(9ab1998c) SHA1(fadaa4a46cefe0093ee1ebeddbae63143fa7bb5a) )
	ROM_LOAD16_BYTE( "11q.bin", 0x700001, 0x80000, CRC(1ab0c88a) SHA1(8bc72732f5911e0d4e0cf12fd2fb12d67e03299e) )

	ROM_REGION( 0x240000, REGION_SOUND1, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "4a.bin", 0x040000, 0x80000, CRC(8cd6824e) SHA1(aa6d8917558de4f2aa8d80527209b9fe91122eb3) )
	ROM_LOAD( "4b.bin", 0x0c0000, 0x80000, CRC(e31ae04d) SHA1(c08d58a4250d8bdb68b8e5012624f345936520e1) )
	ROM_LOAD( "4c.bin", 0x140000, 0x80000, CRC(c4c9f599) SHA1(1d74acd626406052bec533a918ca24e14a2578f2) )
	ROM_LOAD( "4d.bin", 0x1c0000, 0x80000, CRC(f0a9f0e1) SHA1(4221e0824cdc8bcd6ea1c3811f4e3b7cd99478f2) )

	ROM_REGION( 0x240000, REGION_SOUND2, 0 )	/* 8 bit adpcm (banked) */
	ROM_LOAD( "5a.bin", 0x040000, 0x80000, CRC(62557502) SHA1(d72abdaec1c6f55f9b0099b7a8a297e0e14f920c) )
	ROM_LOAD( "5b.bin", 0x0c0000, 0x80000, CRC(dbc86bd7) SHA1(6f1bc3c7e6976fdcd4b2341cea07002fb0cefb14) )
	ROM_LOAD( "5c.bin", 0x140000, 0x80000, CRC(5839a2bd) SHA1(53988086ef97b2671044f6da9d97b1886900b64d) )
	ROM_LOAD( "5d.bin", 0x1c0000, 0x80000, CRC(446f9dc3) SHA1(5c81eb9a7cbea995db9a10d3b6460d02e104825f) )

	ROM_REGION( 0x0220, REGION_PROMS, 0 )		/* unknown */
	ROM_LOAD( "82s123.bin", 0x0000, 0x0020, CRC(67d5ec4b) SHA1(87d32948a0c88277dcdd0eaa035bde40fc7db5fe) )
	ROM_LOAD( "82s147.bin", 0x0020, 0x0200, CRC(d7818542) SHA1(e94f8004c804f260874a117d59dfa0637c5d3d73) )
ROM_END


/* all supported sets give a 93.10.20 date */
GAME( 1993, powerins, 0,        powerins, powerins, 0, ROT0, "Atlus", "Gouketsuji Ichizoku (Japan)", 0 ,0)
GAME( 1993, powerina, powerins, powerina, powerins, 0, ROT0, "Atlus", "Power Instinct (USA, bootleg set 1)", 0 ,0)
GAME( 1993, powerinb, powerins, powerinb, powerins, 0, ROT0, "Atlus", "Power Instinct (USA, bootleg set 2)", 0 ,0)
GAME( 1993, powernjb, powerins, powerinb, powerins, 0, ROT0, "Atlus", "Gouketsuji Ichizoku (Japan, bootleg)", 0 ,0)
