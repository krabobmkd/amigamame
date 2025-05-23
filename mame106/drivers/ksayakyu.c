/*
Kusayakyuu (Sandlot Baseball)
(c)1985 Taito

driver by Tomasz Slanina

TODO:
- colors
- correct communication between main and the sound cpu
- DAC
- sprite glitches (sometimes) .. missing vertical flip flag?
- sound cpu int freq (timer ? $a008/$a010 writes ?)

M6100097A

---------------
CPU/Sound board
---------------
AGC-1 ECB-405010 ASY-410011

CPU  : Zilog Z8400APS(Z80A)
Sound: Zilog Z8400APS(Z80A) AY-3-8910(x2) DAC
OSC  : 18.432MHz

ROMs:
 1.3A [6607976d] - Main program (27128)
 2.3B [a289de5c] |
 3.3C [bb4104a5] /
 4.3D [db0ca023] - Main program (2764)

 5.5F [b0b21817] - Samples
 6.5H [17986662] |
 7.5J [ad242eda] /
 8.5L [3fbae535] - Samples + Sound program?

SRAM:
Hitachi HM6116 (x1)

-----------
Video Board
-----------
AGC-1 ECB-405011 ASY-410011

ROMs:
 9.3J [ef8411dd] - Graphics (2764)
10.3K [1bdee573] |
11.3L [c5859887] /

12.7N [ae8d6ed2] - Graphics (2764)
13.7R [0acb8c61] /

14.9J [982d06f0] - Graphics (2764)
15.9K [dc126df9] |
16.9M [574a172d] |
17.9N [a4c4e4ce] |
18.9R [9d75b104] /

PROM:
6309-1.9F

SRAM:
93422(256x4) (x4)

*/

#include "driver.h"
#include "sound/ay8910.h"
#include "sound/dac.h"

WRITE8_HANDLER( ksayakyu_videoram_w );
WRITE8_HANDLER( ksayakyu_videoctrl_w );
PALETTE_INIT( ksayakyu );
VIDEO_START( ksayakyu );
VIDEO_UPDATE( ksayakyu );

static int sound_status=0xff;

static WRITE8_HANDLER( bank_select_w )
{
	/*
        bits:
        76543210
               x - ROM bank
        xxxxxxx  - unused ?

    */
	memory_set_bankptr( 1, memory_region(REGION_CPU1) + ((data&1) * 0x4000) + 0x10000 );
}

static WRITE8_HANDLER( latch_w )
{
	sound_status&=~0x80;
	soundlatch_w(0,data|0x80);
}

static READ8_HANDLER (sound_status_r)
{
	return sound_status|4;
}

static WRITE8_HANDLER(tomaincpu_w)
{
	sound_status|=0x80;
}

static ADDRESS_MAP_START( maincpu_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x7fff) AM_READWRITE(MRA8_BANK1, MWA8_NOP)
	AM_RANGE(0x8000, 0x9fff) AM_ROM
	AM_RANGE(0xa000, 0xa7ff) AM_RAM
	AM_RANGE(0xa800, 0xa800) AM_READ(input_port_0_r)
	AM_RANGE(0xa801, 0xa801) AM_READ(input_port_1_r)
	AM_RANGE(0xa802, 0xa802) AM_READ(input_port_2_r)
	AM_RANGE(0xa803, 0xa803) AM_READ(MRA8_NOP) /* watchdog ? */
	AM_RANGE(0xa804, 0xa804) AM_WRITE(ksayakyu_videoctrl_w)
	AM_RANGE(0xa805, 0xa805) AM_WRITE(latch_w)
	AM_RANGE(0xa806, 0xa806) AM_READ(sound_status_r)
	AM_RANGE(0xa807, 0xa807) AM_READ(MRA8_NOP) /* watchdog ? */
	AM_RANGE(0xa808, 0xa808) AM_WRITE(bank_select_w)
	AM_RANGE(0xb000, 0xb7ff) AM_READWRITE(videoram_r, ksayakyu_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0xb800, 0xbfff) AM_READWRITE(MRA8_RAM, MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
ADDRESS_MAP_END

static ADDRESS_MAP_START( soundcpu_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x83ff) AM_RAM
	AM_RANGE(0xa001, 0xa001) AM_READ(AY8910_read_port_0_r)
	AM_RANGE(0xa002, 0xa002) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0xa003, 0xa003) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0xa006, 0xa006) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0xa007, 0xa007) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0xa008, 0xa008) AM_WRITE(MWA8_NOP)
	AM_RANGE(0xa00c, 0xa00c) AM_WRITE(tomaincpu_w)
	AM_RANGE(0xa010, 0xa010) AM_WRITE(MWA8_NOP)
ADDRESS_MAP_END

INPUT_PORTS_START( ksayakyu )
	PORT_START
	  PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	  PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  )
	  PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	  PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	  PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2 )

	  PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	  PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START
	  PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT) PORT_COCKTAIL
	  PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT) PORT_COCKTAIL
	  PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN) PORT_COCKTAIL
	  PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP) PORT_COCKTAIL
	  PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_COCKTAIL

	  PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_COCKTAIL
	  PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START
	  PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coinage ) )
	  PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	  PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	  PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	  PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
  	  PORT_DIPNAME( 0x04, 0x00, DEF_STR( Flip_Screen ) )
	  PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	  PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	  PORT_DIPNAME( 0x08, 0x08, DEF_STR( Cabinet ) )
	  PORT_DIPSETTING(    0x08, DEF_STR( Upright ) )
	  PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	  PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )
	  PORT_DIPNAME( 0x040, 0x00, "Continue" )
	  PORT_DIPSETTING(    0x00, "7th inning" )
	  PORT_DIPSETTING(    0x040, "1st inning" )
	  PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN1 )
INPUT_PORTS_END

static WRITE8_HANDLER(dummy_w)
{
	//DAC ? communication with main cpu ?
}

static struct AY8910interface ay8910_interface_1 =
{
	soundlatch_r,
	0,
	0,
	dummy_w
};

static struct AY8910interface ay8910_interface_2 =
{
	0,
	0,
	dummy_w,
	dummy_w
};

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0,1,2,3, 4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};


static const gfx_layout charlayout2 =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(0,2), RGN_FRAC(1,2)},
	{ 0,1,2,3, 4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4,5, 6, 7,
		8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	8*8*4
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0, 16 },
	{ REGION_GFX3, 0, &charlayout2,  16*4, 32 },
	{ REGION_GFX2, 0, &spritelayout, 0, 16 },
	{ -1 }
};

static MACHINE_DRIVER_START( ksayakyu )
	MDRV_CPU_ADD(Z80,8000000/2)
	MDRV_CPU_PROGRAM_MAP(maincpu_map,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)
	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_CPU_ADD(Z80, 80000000/2)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(soundcpu_map,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)

	MDRV_INTERLEAVE(1000)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER )
	MDRV_SCREEN_SIZE(256, 256)

	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_INIT(ksayakyu)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(ksayakyu)
	MDRV_VIDEO_UPDATE(ksayakyu)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 2000000)
	MDRV_SOUND_CONFIG(ay8910_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 2000000)
	MDRV_SOUND_CONFIG(ay8910_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)
MACHINE_DRIVER_END

ROM_START( ksayakyu )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )

	ROM_LOAD( "1.3a", 0x00000, 0x4000, CRC(6607976d) SHA1(23b15cae04922e54faf35a5a499ba5064b18ed46) )
	ROM_LOAD( "2.3b", 0x04000, 0x4000, CRC(a289de5c) SHA1(d3b14364ef77ca74ac79c5099cf0e8f3baa97612) )
	ROM_RELOAD(               0x10000, 0x04000 )
	ROM_LOAD( "4.3d", 0x08000, 0x2000, CRC(db0ca023) SHA1(1356fa0239209dea6a4ac0af177fe8be47f12cd0) )
	ROM_LOAD( "3.3c", 0x14000, 0x4000, CRC(bb4104a5) SHA1(1f793c4431a3476eeb92556228bf855efb73fb83) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "8.5l",  0x0000, 0x2000, CRC(3fbae535) SHA1(87b66091e33eee66f4d63c213a5a19b70e7b1e5a) )
	ROM_LOAD( "7.5j",  0x2000, 0x2000, CRC(ad242eda) SHA1(928c04e4a48077ec9db148cb71d52ebfe01efa53) )
	ROM_LOAD( "6.5h",  0x4000, 0x2000, CRC(17986662) SHA1(81b65e381b923c5544f4708efef09f0894c716b2) )
	ROM_LOAD( "5.5f",  0x6000, 0x2000, CRC(b0b21817) SHA1(da2a1a6865dbc335775fa2e0ad0fb899be95af03) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE  )
	ROM_LOAD( "9.3j",  0x0000, 0x2000, CRC(ef8411dd) SHA1(1dbc959d3aec9face19b2a5ae873ca34bfeff5cd) )
	ROM_LOAD( "10.3k", 0x2000, 0x2000, CRC(1bdee573) SHA1(7b92a8133cb83404505d21f462e3ca6c85647dca) )
	ROM_LOAD( "11.3l", 0x4000, 0x2000, CRC(c5859887) SHA1(41685fb8f8e7c44acd5e0e3ccc629e5f64a59fbd) )

	ROM_REGION( 0x6000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "14.9j", 0x0000, 0x2000, CRC(982d06f0) SHA1(e107c56ee4f2695a790b8cec6d52337ba9d8b2ad) )
	ROM_LOAD( "15.9k", 0x2000, 0x2000, CRC(dc126df9) SHA1(368efb36bf197e3eac23ef543e25e9f4efba785e) )
	ROM_LOAD( "16.9m", 0x4000, 0x2000, CRC(574a172d) SHA1(7dee073b5c8ff5825062e30cff29343dc767daaa) )

	ROM_REGION( 0x4000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "17.9n", 0x0000, 0x2000, CRC(a4c4e4ce) SHA1(f8b0a8dfab972e23f268c69fd9ef30fc80f62533) )
	ROM_LOAD( "18.9r", 0x2000, 0x2000, CRC(9d75b104) SHA1(062884fdca9f705f555b828aff136d8f52fbf6eb) )

	ROM_REGION( 0x4000, REGION_USER1, 0 )
	ROM_LOAD( "13.7r", 0x0000, 0x2000, CRC(0acb8c61) SHA1(0ca3b07b21501d4c8feda2b4295f534da2a1c745) )
	ROM_LOAD( "12.7n", 0x2000, 0x2000, CRC(ae8d6ed2) SHA1(92d6484040a9b82f77b92f3dd452333fd618eb0d) )

	ROM_REGION( 0x100, REGION_PROMS, 0 )
	ROM_LOAD( "9f.bin", 0x0000, 0x0100, CRC(ff71b27f) SHA1(6aad2bd2be997595a05ddb81d24df8fe1435910b) )
ROM_END

GAME( 1985, ksayakyu, 0, ksayakyu, ksayakyu, 0, ORIENTATION_FLIP_Y, "Taito Corporation", "Kusayakyuu",GAME_IMPERFECT_SOUND|GAME_WRONG_COLORS ,0)
