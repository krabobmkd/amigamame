/***************************************************************************

    Goal! '92

    driver by Pierpaolo Prazzoli
    and some bits by David Haywood

***************************************************************************/

#include "driver.h"
#include "sound/2203intf.h"
#include "sound/okim6295.h"
#include "sound/msm5205.h"

extern WRITE16_HANDLER( goal92_background_w );
extern WRITE16_HANDLER( goal92_foreground_w );
extern WRITE16_HANDLER( goal92_text_w );
extern WRITE16_HANDLER( goal92_fg_bank_w );
extern READ16_HANDLER( goal92_fg_bank_r );

extern VIDEO_START( goal92 );
extern VIDEO_UPDATE( goal92 );
extern VIDEO_EOF( goal92 );

extern UINT16 *goal92_back_data,*goal92_fore_data,*goal92_textram,*goal92_scrollram16;

static int msm5205next;

static WRITE16_HANDLER( goal92_sound_command_w )
{
	if (ACCESSING_MSB)
	{
		soundlatch_w(0, (data >> 8) & 0xff);
		cpunum_set_input_line(1,0,HOLD_LINE);
	}
}

static READ16_HANDLER( goal92_inputs_r )
{
	switch(offset)
	{
		case 0:
			return readinputport(0);
		case 1:
			return readinputport(1);
		case 2:
			return readinputport(2);
		case 3:
			return readinputport(3);
		case 7:
			return readinputport(4);

		default:
			loginfo(2,"reading unhandled goal92 inputs %04X %04X @ PC = %04X\n",offset, mem_mask,activecpu_get_pc());
	}

	return 0;
}

static ADDRESS_MAP_START( goal92_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x100000, 0x13ffff) AM_READ(MRA16_RAM)
	AM_RANGE(0x180000, 0x18000f) AM_READ(goal92_inputs_r)
	AM_RANGE(0x18001c, 0x18001d) AM_READ(goal92_fg_bank_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( goal92_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x100000, 0x1007ff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x100800, 0x100fff) AM_WRITE(goal92_background_w) AM_BASE(&goal92_back_data)
	AM_RANGE(0x101000, 0x1017ff) AM_WRITE(goal92_foreground_w) AM_BASE(&goal92_fore_data)
	AM_RANGE(0x101800, 0x101fff) AM_WRITE(MWA16_RAM) // it has tiles for clouds, but they aren't used
	AM_RANGE(0x102000, 0x102fff) AM_WRITE(goal92_text_w) AM_BASE(&goal92_textram)
	AM_RANGE(0x103000, 0x103fff) AM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x104000, 0x13ffff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x140000, 0x1407ff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0x140800, 0x140801) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x140802, 0x140803) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x180008, 0x180009) AM_WRITE(goal92_sound_command_w)
	AM_RANGE(0x18000a, 0x18000b) AM_WRITE(MWA16_NOP)
	AM_RANGE(0x180010, 0x180017) AM_WRITE(MWA16_RAM) AM_BASE(&goal92_scrollram16)
	AM_RANGE(0x18001c, 0x18001d) AM_WRITE(goal92_fg_bank_w)
ADDRESS_MAP_END

/* Sound CPU */

static WRITE8_HANDLER( adpcm_control_w )
{
	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU2);

	/* the code writes either 2 or 3 in the bottom two bits */
	bankaddress = 0x10000 + (data & 0x01) * 0x4000;
	memory_set_bankptr(1,&RAM[bankaddress]);

	MSM5205_reset_w(0,data & 0x08);
}

static WRITE8_HANDLER( adpcm_data_w )
{
	msm5205next = data;
}

static ADDRESS_MAP_START( sound_cpu, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(adpcm_control_w)
	AM_RANGE(0xe400, 0xe400) AM_WRITE(adpcm_data_w)
	AM_RANGE(0xe800, 0xe800) AM_READWRITE(YM2203_status_port_0_r, YM2203_control_port_0_w)
	AM_RANGE(0xe801, 0xe801) AM_READWRITE(YM2203_read_port_0_r, YM2203_write_port_0_w)
	AM_RANGE(0xec00, 0xec00) AM_READWRITE(YM2203_status_port_1_r, YM2203_control_port_1_w)
	AM_RANGE(0xec01, 0xec01) AM_READWRITE(YM2203_read_port_1_r, YM2203_write_port_1_w)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM
	AM_RANGE(0xf800, 0xf800) AM_READ(soundlatch_r)
ADDRESS_MAP_END

INPUT_PORTS_START( goal92 )
	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x0007, 0x0007, "Coin A / Coin C" )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0005, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0038, 0x0038, "Coin B / Coin D" )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0038, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0028, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0018, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0040, 0x0040, "Starting Coin" )
	PORT_DIPSETTING(      0x0040, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0000, "X 2" )
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

	PORT_START_TAG("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW,  IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW,  IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW,  IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW,  IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW,  IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW,  IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW,  IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW,  IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW,  IPT_COIN2 )

	PORT_START_TAG("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW,  IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW,  IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW,  IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW,  IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW,  IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW,  IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW,  IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW,  IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW,  IPT_BUTTON3 ) PORT_PLAYER(4)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW,  IPT_COIN4 )

	PORT_START_TAG("IN3")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW,  IPT_START1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW,  IPT_START2 )
	PORT_BIT( 0x00fc, IP_ACTIVE_LOW,  IPT_UNKNOWN ) // unused?
	PORT_BIT( 0x0100, IP_ACTIVE_LOW,  IPT_START3 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW,  IPT_START4 )
	PORT_BIT( 0xfc00, IP_ACTIVE_LOW,  IPT_UNKNOWN ) // unused?

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Easy ) )
	PORT_DIPSETTING(	  0x0003, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x000c, 0x0000, "Players / Coin Mode" )
	PORT_DIPSETTING(      0x0000, "4 Players / 1 Coin Slot" )
	PORT_DIPSETTING(      0x0004, "4 Players / 4 Coin Slots" )
	PORT_DIPSETTING(      0x0008, "4 Players / 2 Coin Slots" )
	PORT_DIPSETTING(      0x000c, "2 Players" )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Service_Mode ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0xffc0, 0xffc0, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0xffc0, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

/* handler called by the 2203 emulator when the internal timers cause an IRQ */
static void irqhandler(int irq)
{
	cpunum_set_input_line(1, INPUT_LINE_NMI, irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2203interface ym2203_interface =
{
	0,
	0,
	0,
	0,
	irqhandler
};

static void goal92_adpcm_int(int data)
{
	static int toggle = 0;

	MSM5205_data_w (0,msm5205next);
	msm5205next>>=4;

	toggle ^= 1;
	if(toggle)
		cpunum_set_input_line(1, INPUT_LINE_NMI, PULSE_LINE);
}

static struct MSM5205interface msm5205_interface =
{
	goal92_adpcm_int,	/* interrupt function */
	MSM5205_S96_4B		/* 4KHz 4-bit */
};

static const gfx_layout layout_8x8x4 =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(3,4),RGN_FRAC(2,4),RGN_FRAC(1,4),RGN_FRAC(0,4) },
	{ STEP8(0,1) },
	{ STEP8(0,8) },
	8*8
};

static const gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(3,4),RGN_FRAC(2,4),RGN_FRAC(1,4),RGN_FRAC(0,4) },
	{ STEP8(0,1), STEP8(8*8*2,1) },
	{ STEP8(0,8), STEP8(8*8*1,8) },
	16*16
};

static const gfx_layout layout_16x16x4_2 =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4,0,12,8,20,16,28,24,
		512+4,512+0,512+12,512+8,512+20,512+16,512+28,512+24 },
	{ 0*32,1*32,2*32,3*32,4*32,5*32,6*32,7*32,
		256+0*32,256+1*32,256+2*32,256+3*32,256+4*32,256+5*32,256+6*32,256+7*32 },
	1024
};

static const gfx_decode goal92_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_16x16x4,		   0*16, 8*16 }, // Sprites
	{ REGION_GFX2, 0, &layout_8x8x4,		  48*16,   16 }, // Text Layer
	{ REGION_GFX2, 0, &layout_16x16x4,		   0*16,   16 }, // BG Layer
	{ REGION_GFX2, 0, &layout_16x16x4,		  16*16,   16 }, // Mid Layer
	{ REGION_GFX2, 0, &layout_16x16x4,		  32*16,   16 }, // FG Layer
	{ -1 } /* end of array */
};

static const gfx_decode cupsocbl_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout_16x16x4_2,	   0*16, 8*16 }, // Sprites
	{ REGION_GFX2, 0, &layout_8x8x4,	      48*16,   16 }, // Text Layer
	{ REGION_GFX3, 0, &layout_16x16x4,	       0*16,   16 }, // BG Layer
	{ REGION_GFX4, 0, &layout_16x16x4,	      16*16,   16 }, // Mid Layer
	{ REGION_GFX4, 0, &layout_16x16x4,  	  32*16,   16 }, // FG Layer
	{ -1 } /* end of array */
};

static MACHINE_DRIVER_START( goal92 )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,12000000)
	MDRV_CPU_PROGRAM_MAP(goal92_readmem,goal92_writemem)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1) /* VBL */

	MDRV_CPU_ADD(Z80, 2500000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_cpu,0)
								/* IRQs are triggered by the main CPU */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1) // black border at bottom is a game bug...
	MDRV_GFXDECODE(goal92_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(128*16)

	MDRV_VIDEO_START(goal92)
	MDRV_VIDEO_UPDATE(goal92)
	MDRV_VIDEO_EOF(goal92)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, 2500000/2)
	MDRV_SOUND_CONFIG(ym2203_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(YM2203, 2500000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(MSM5205, 384000)
	MDRV_SOUND_CONFIG(msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( cupsocbl )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,12000000)
	MDRV_CPU_PROGRAM_MAP(goal92_readmem,goal92_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1) /* VBL */

	MDRV_CPU_ADD(Z80, 2510000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_cpu,0)
								/* IRQs are triggered by the main CPU */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(cupsocbl_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(128*16)

	MDRV_VIDEO_START(goal92)
	MDRV_VIDEO_UPDATE(goal92)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(OKIM6295, 8000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

/*

Goal '92 (bootleg of Seibu Cup Soccer)


PCB Layout

----------------------------------------------------------
| 400KHz  Z80                 10MHz            68000     |
|   6116   1                                   2   3     |
|  YM2203           6116                    681000 681000|
| 5205              6116                                 |
|                   6116                                 |
| 10MHz             6116                       24.0MHz   |
|                         6264                           |
|                         6264        TPC1020            |
|                                                        |
|     6116                                               |
|                                                        |
|     6116                                               |
|                                                        |
| DSW1  DSW3                                             |
|                                                        |
| DSW2                                                   |
|                             4             8            |
|                   6116      5             9            |
|                             6             10           |
|                   6116      7             11           |
|                                                        |
----------------------------------------------------------

Notes:
Z80 clock: 2.51MHz
68k clock: 12.0MHz
    VSync: 60Hz
    HSync: 15.27kHz

*/

ROM_START( goal92 )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "2.bin", 0x00000, 0x80000, CRC(db0a6c7c) SHA1(b609db7806b99bc921806d8b3e5e515b4651c375) )
	ROM_LOAD16_BYTE( "3.bin", 0x00001, 0x80000, CRC(e4c45dee) SHA1(542749bd1ff51220a151fe66acdadac83df8f0ee) )

	ROM_REGION( 0x18000, REGION_CPU2, 0 )	/* Z80 code */
	ROM_LOAD( "1.bin",        0x00000, 0x8000, CRC(3d317622) SHA1(ae4e8c5247bc215a2769786cb8639bce2f80db22) )
	ROM_CONTINUE(             0x10000, 0x8000 ) /* banked at 8000-bfff */

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "6.bin",        0x000000, 0x040000, CRC(83cadc8f) SHA1(1d3309750347c5d6d661f5cf452235e5a83a7483) )
	ROM_LOAD( "7.bin",        0x040000, 0x040000, CRC(067e10fc) SHA1(9831b8dc9b8efa6f7797b2946ee5be03fb36de7b) )
	ROM_LOAD( "5.bin",        0x080000, 0x040000, CRC(9a390af2) SHA1(8bc46f8cc7823b8caf381866bea016ebfad9d5d3) )
	ROM_LOAD( "4.bin",        0x0c0000, 0x040000, CRC(69b118d5) SHA1(80ab6f03e1254ba47c27299ce11559b244a024ad) )

	ROM_REGION( 0x200000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "11.bin",       0x000000, 0x080000, CRC(5701e626) SHA1(e6915714e9ca90be8fa8ab1bf7fd1f23a83fb82c) )
	ROM_LOAD( "10.bin",       0x080000, 0x080000, CRC(ebb359cc) SHA1(b2f724ef7a91fca0ff0b7d7abe1c37816464b37d) )
	ROM_LOAD( "9.bin",        0x100000, 0x080000, CRC(c9f0dd07) SHA1(d70cdb24b7df521255b5841f01dd9e8344ab7bdb) )
	ROM_LOAD( "8.bin",        0x180000, 0x080000, CRC(aeab3534) SHA1(af91238f412bfcff3a52232278d81276584614a7) )
ROM_END

/*

Seibu Cup Soccer - Seibu - Bootleg

2 boards

1st board

(snd)
1 x z80
1 x oki 6295
sc_01 (prg)
sc_02 and sc_03 (data)

(prg)
1 x 68000
sc_04 and sc_05

(gfx)
2 x ti tpc1020
from sc_06 to sc_11

2nd board

(gfx)
1 x actel pl84c
from sc_12 to sc_15

*/

ROM_START( cupsocbl )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "sc_04.bin", 0x00001, 0x80000, CRC(22566087) SHA1(4392f46ca50cc9947823a5190aa25f5e9654aa0d) )
	ROM_LOAD16_BYTE( "sc_05.bin", 0x00000, 0x80000, CRC(2f977dff) SHA1(4d8d6e7d06ce17bb7292072965911f8b1f1067e2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Z80 code */
	ROM_LOAD( "sc_01.bin",    0x000000, 0x08000, CRC(cea39d6d) SHA1(f0b79c03ffafdd1e57673d6d4836becbe415110b) )
	ROM_CONTINUE(			  0x000000, 0x08000 )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "sc_07.bin", 0x000000, 0x80000, CRC(dcb29d01) SHA1(72b4234622605f0ab03f21fdb6a61c6dac36000d) )
	ROM_LOAD( "sc_06.bin", 0x080000, 0x80000, CRC(2dc70e05) SHA1(f1d0beb8428a7e1d7c7818e6719abdc543b2fa80) )

	ROM_REGION( 0x020000, REGION_GFX2, ROMREGION_DISPOSE ) // text layer ?
	ROM_LOAD( "sc_13.bin",    0x000000, 0x010000, CRC(229bddd8) SHA1(0924bf29db9c5a970546f154e7752697fdce6a58) )
	ROM_LOAD( "sc_12.bin",    0x010000, 0x010000, CRC(dabfa826) SHA1(0db587c846755491b169ef7751ba8e7cdc2607e6) )

	ROM_REGION( 0x200000, REGION_GFX3, ROMREGION_DISPOSE ) // background ?
	ROM_LOAD( "sc_08.bin",    0x000000, 0x080000, CRC(637120f3) SHA1(b4b2ad192e46ff80d4cb440d7fb6dac215a353ed) )
	ROM_LOAD( "sc_09.bin",    0x080000, 0x080000, CRC(695b6342) SHA1(dfccb43789021ba2568b9284ae61e64f7f89b152) )
	ROM_LOAD( "sc_10.bin",    0x100000, 0x080000, CRC(27e172b8) SHA1(ed86db2f42c8061607d46f2407b0130aaf692a02) )
	ROM_LOAD( "sc_11.bin",    0x180000, 0x080000, CRC(0cd5ca5e) SHA1(a59665e543e9383355de2576e6693348ec356591) )

	ROM_REGION( 0x100000, REGION_GFX4, ROMREGION_DISPOSE ) // foreground ?
	ROM_LOAD( "sc_14.bin",    0x000000, 0x080000, CRC(566086c2) SHA1(b7d09ce978f99ecc0d1975b31330ed49317701d5) )
	ROM_LOAD( "sc_15.bin",    0x080000, 0x080000, CRC(8fd87e65) SHA1(acc9fd0289fa9ab60bec16d3e642039380e5180a) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "sc_02.bin",    0x000000, 0x020000, CRC(a70d4f03) SHA1(c2482e624c8a828a94206a36d10c1021ad8ca1d0) )

	ROM_REGION( 0x080000, REGION_USER1, 0 ) // sound related ?
	ROM_LOAD( "sc_03.bin",    0x000000, 0x080000, CRC(6e254d12) SHA1(857779dbd276b688201a8ea3afd5817e38acad2e) )
ROM_END

GAME( 1992, goal92,   cupsoc, goal92,   goal92, 0, ROT0, "bootleg", "Goal! '92", GAME_IMPERFECT_SOUND ,0)
GAME( 1992, cupsocbl, cupsoc, cupsocbl, goal92, 0, ROT0, "bootleg", "Seibu Cup Soccer (bootleg)", GAME_NOT_WORKING | GAME_NO_SOUND ,0)
