/**************************************************************************

Based on drivers from Juno First emulator by Chris Hardy (chris@junofirst.freeserve.co.uk)

To enter service mode, keep 1&2 pressed on reset

***************************************************************************/

#include "driver.h"
#include "cpu/m6809/m6809.h"
#include "cpu/i8039/i8039.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"
#include "sound/dac.h"
#include "sound/flt_rc.h"


void konami1_decode(void);

extern unsigned char *megazone_scrollx;
extern unsigned char *megazone_scrolly;

static unsigned char *megazone_sharedram;

extern unsigned char *megazone_videoram2;
extern unsigned char *megazone_colorram2;
extern size_t megazone_videoram2_size;

static int i8039_status;

VIDEO_START( megazone );

WRITE8_HANDLER( megazone_flipscreen_w );
PALETTE_INIT( megazone );
WRITE8_HANDLER( megazone_sprite_bank_select_w );
VIDEO_UPDATE( megazone );



READ8_HANDLER( megazone_portA_r )
{
	int clock,timer;


	/* main xtal 14.318MHz, divided by 8 to get the AY-3-8910 clock, further */
	/* divided by 1024 to get this timer */
	/* The base clock for the CPU and 8910 is NOT the same, so we have to */
	/* compensate. */
	/* (divide by (1024/2), and not 1024, because the CPU cycle counter is */
	/* incremented every other state change of the clock) */

	clock = activecpu_gettotalcycles() * 7159/12288;	/* = (14318/8)/(18432/6) */
	timer = (clock / (1024/2)) & 0x0f;

	/* low three bits come from the 8039 */

	return (timer << 4) | i8039_status;
}

static WRITE8_HANDLER( megazone_portB_w )
{
	int i;


	for (i = 0;i < 3;i++)
	{
		int C;


		C = 0;
		if (data & 1) C +=  10000;	/*  10000pF = 0.01uF */
		if (data & 2) C += 220000;	/* 220000pF = 0.22uF */
		data >>= 2;
		filter_rc_set_RC(i,1000,2200,200,C);
	}
}

WRITE8_HANDLER( megazone_videoram2_w )
{
	if (megazone_videoram2[offset] != data)
	{
		megazone_videoram2[offset] = data;
	}
}

WRITE8_HANDLER( megazone_colorram2_w )
{
	if (megazone_colorram2[offset] != data)
	{
		megazone_colorram2[offset] = data;
	}
}

READ8_HANDLER( megazone_sharedram_r )
{
	return(megazone_sharedram[offset]);
}

WRITE8_HANDLER( megazone_sharedram_w )
{
	megazone_sharedram[offset] = data;
}

static WRITE8_HANDLER( megazone_i8039_irq_w )
{
	cpunum_set_input_line(2, 0, ASSERT_LINE);
}

WRITE8_HANDLER( i8039_irqen_and_status_w )
{
	if ((data & 0x80) == 0)
		cpunum_set_input_line(2, 0, CLEAR_LINE);
	i8039_status = (data & 0x70) >> 4;
}

static WRITE8_HANDLER( megazone_coin_counter_w )
{
	coin_counter_w(1-offset,data);		/* 1-offset, because coin counters are in reversed order */
}



static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x2000, 0x2fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x33ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3800, 0x3fff) AM_READ(megazone_sharedram_r)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)		/* 4000->5FFF is a debug rom */
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0001) AM_WRITE(megazone_coin_counter_w) /* coin counter 2, coin counter 1 */
	AM_RANGE(0x0005, 0x0005) AM_WRITE(megazone_flipscreen_w)
	AM_RANGE(0x0007, 0x0007) AM_WRITE(interrupt_enable_w)
	AM_RANGE(0x0800, 0x0800) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x1800, 0x1800) AM_WRITE(MWA8_RAM) AM_BASE(&megazone_scrollx)
	AM_RANGE(0x1000, 0x1000) AM_WRITE(MWA8_RAM) AM_BASE(&megazone_scrolly)
	AM_RANGE(0x2000, 0x23ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2400, 0x27ff) AM_WRITE(megazone_videoram2_w) AM_BASE(&megazone_videoram2) AM_SIZE(&megazone_videoram2_size)
	AM_RANGE(0x2800, 0x2bff) AM_WRITE(colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x2c00, 0x2fff) AM_WRITE(megazone_colorram2_w) AM_BASE(&megazone_colorram2)
	AM_RANGE(0x3000, 0x33ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x3800, 0x3fff) AM_WRITE(megazone_sharedram_w) AM_BASE(&megazone_sharedram)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x6000, 0x6000) AM_READ(input_port_0_r) /* IO Coin */
	AM_RANGE(0x6001, 0x6001) AM_READ(input_port_1_r) /* P1 IO */
	AM_RANGE(0x6002, 0x6002) AM_READ(input_port_2_r) /* P2 IO */
	AM_RANGE(0x8000, 0x8000) AM_READ(input_port_3_r) /* DIP 1 */
	AM_RANGE(0x8001, 0x8001) AM_READ(input_port_4_r) /* DIP 2 */
	AM_RANGE(0xe000, 0xe7ff) AM_READ(megazone_sharedram_r)  /* Shared with $3800->3fff of main CPU */
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(megazone_i8039_irq_w)	/* START line. Interrupts 8039 */
	AM_RANGE(0x4000, 0x4000) AM_WRITE(soundlatch_w)			/* CODE  line. Command Interrupts 8039 */
	AM_RANGE(0xa000, 0xa000) AM_WRITE(MWA8_RAM)				/* INTMAIN - Interrupts main CPU (unused) */
	AM_RANGE(0xc000, 0xc000) AM_WRITE(MWA8_RAM)				/* INT (Actually is NMI) enable/disable (unused)*/
	AM_RANGE(0xc001, 0xc001) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0xe000, 0xe7ff) AM_WRITE(megazone_sharedram_w)	/* Shared with $3800->3fff of main CPU */
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x02) AM_READ(AY8910_read_port_0_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x02, 0x02) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_readport, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x00, 0xff) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_writeport, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(I8039_p1, I8039_p1) AM_WRITE(DAC_0_data_w)
	AM_RANGE(I8039_p2, I8039_p2) AM_WRITE(i8039_irqen_and_status_w)
ADDRESS_MAP_END

INPUT_PORTS_START( megazone )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "Infinite (Cheat)")
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x18, "20k 70k" )
	PORT_DIPSETTING(    0x10, "20k 80k" )
	PORT_DIPSETTING(    0x08, "30k 90k" )
	PORT_DIPSETTING(    0x00, "30k 100k" )
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, "Difficult" )
	PORT_DIPSETTING(    0x00, "Very Difficult" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW2")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	512,    /* 512 characters */
	4,      /* 4 bits per pixel */
	{ 0, 1, 2, 3 }, /* the four bitplanes are packed in one nibble */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8    /* every char takes 8 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,	/* 16*16 sprites */
	256,	/* 256 sprites */
	4,	/* 4 bits per pixel */
	{ 0x4000*8+4, 0x4000*8+0, 4, 0 },
	{ 0, 1, 2, 3, 8*8+0, 8*8+1, 8*8+2, 8*8+3,
			16*8+0, 16*8+1, 16*8+2, 16*8+3, 24*8+0, 24*8+1, 24*8+2, 24*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 ,
		32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8 },
	64*8	/* every sprite takes 64 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,       0, 16 },
	{ REGION_GFX2, 0, &spritelayout, 16*16, 16 },
	{ -1 } /* end of array */
};



static struct AY8910interface ay8910_interface =
{
	megazone_portA_r,
	0,
	0,
	megazone_portB_w
};



static MACHINE_DRIVER_START( megazone )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6809, 18432000/9)        /* 2 MHz */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80,18432000/6)     /* Z80 Clock is derived from the H1 signal */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_IO_MAP(sound_readport,sound_writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(I8039,(14318000/2)/I8039_CLOCK_DIVIDER)
	/* audio CPU */	/* 1/2 14MHz crystal */
	MDRV_CPU_PROGRAM_MAP(i8039_readmem,i8039_writemem)
	MDRV_CPU_IO_MAP(i8039_readport,i8039_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(15)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(36*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 36*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(32)
	MDRV_COLORTABLE_LENGTH(16*16+16*16)

	MDRV_PALETTE_INIT(megazone)
	MDRV_VIDEO_START(megazone)
	MDRV_VIDEO_UPDATE(megazone)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 14318000/8)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(0, "filter.0.0", 0.30)
	MDRV_SOUND_ROUTE(1, "filter.0.1", 0.30)
	MDRV_SOUND_ROUTE(2, "filter.0.2", 0.30)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD_TAG("filter.0.0", FILTER_RC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
	MDRV_SOUND_ADD_TAG("filter.0.1", FILTER_RC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
	MDRV_SOUND_ADD_TAG("filter.0.2", FILTER_RC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( megazone )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for code */
	ROM_LOAD( "319i07.bin",    0x6000, 0x2000, CRC(94b22ea8) SHA1(dc3ed2a0d1a12df51e46561324d78b7d655be313) )
	ROM_LOAD( "319i06.bin",    0x8000, 0x2000, CRC(0468b619) SHA1(a6755728fab37674749f9b77cb53f6f228102f2f) )
	ROM_LOAD( "319i05.bin",    0xa000, 0x2000, CRC(ac59000c) SHA1(c7568589f6b0e1706e996fdfed9c16755541951e) )
	ROM_LOAD( "319i04.bin",    0xc000, 0x2000, CRC(1e968603) SHA1(fd818f678a3dc8d48a30f9f7670bfcb42a3009a2) )
	ROM_LOAD( "319i03.bin",    0xe000, 0x2000, CRC(0888b803) SHA1(37249bfb14c6c3ce40ad68be457ab1f66fd7ea70) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for the audio CPU */
	ROM_LOAD( "319e02.bin",   0x0000, 0x2000, CRC(d5d45edb) SHA1(3808d1b58fe152f8f5b49bf0aa40c53e9c9dd4bd) )

	ROM_REGION( 0x1000, REGION_CPU3, 0 )     /* 4k for the 8039 DAC CPU */
	ROM_LOAD( "319e01.bin",   0x0000, 0x1000, CRC(ed5725a0) SHA1(64f54621487291fbfe827fb4cecca299fd0db781) )

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "319e12.bin",    0x0000, 0x2000, CRC(e0fb7835) SHA1(44ccaaf92bdb83323f45e08dbe118697720e9105) )
	ROM_LOAD( "319e13.bin",    0x2000, 0x2000, CRC(3d8f3743) SHA1(1f6fbf804dacfa44cd11b4cf41d0bedb7f2ff6b6) )

	ROM_REGION( 0x08000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "319e11.bin",    0x0000, 0x2000, CRC(965a7ff6) SHA1(210aae91a3838e5f7c78747d9b7419d266538ffc) )
	ROM_LOAD( "319e09.bin",    0x2000, 0x2000, CRC(5eaa7f3e) SHA1(4c038e80d575988407252897a1f1bc6b76af597c) )
	ROM_LOAD( "319e10.bin",    0x4000, 0x2000, CRC(7bb1aeee) SHA1(be2dd46cd0121cedad6dab90a22643798a3176ab) )
	ROM_LOAD( "319e08.bin",    0x6000, 0x2000, CRC(6add71b1) SHA1(fc8c0ecd3b7f03d63b6c3143143986883345fa38) )

	ROM_REGION( 0x0260, REGION_PROMS, 0 )
	ROM_LOAD( "319b18.a16",  0x0000, 0x020, CRC(23cb02af) SHA1(aba459826a75ec07bc6d97580e934f58aa22f4f4) ) /* palette */
	ROM_LOAD( "319b16.c6",   0x0020, 0x100, CRC(5748e933) SHA1(c1478c31533a11421cd4579ccfdbb430e193d17b) ) /* sprite lookup table */
	ROM_LOAD( "319b17.a11",  0x0120, 0x100, CRC(1fbfce73) SHA1(1c58eb91982d5f10511d54a83070e859ac57d2f1) ) /* character lookup table */
	ROM_LOAD( "319b14.e7",   0x0220, 0x020, CRC(55044268) SHA1(29cf4158314ed897daf045a7f07be865dd977663) ) /* timing (not used) */
	ROM_LOAD( "319b15.e8",   0x0240, 0x020, CRC(31fd7ab9) SHA1(04d6e51b4930619c8ee12fb0d7b5f981e4d6d8d3) ) /* timing (not used) */
ROM_END

ROM_START( megaznik )
	ROM_REGION( 2*0x10000, REGION_CPU1, 0 )     /* 64k for code */
	ROM_LOAD( "ic59_cpu.bin",  0x6000, 0x2000, CRC(f41922a0) SHA1(9f54509da18721a76593921c6e52085e62e6ea6b) )
	ROM_LOAD( "ic58_cpu.bin",  0x8000, 0x2000, CRC(7fd7277b) SHA1(e773247e0c9419cae49e04962ea362a2976c2db2) )
	ROM_LOAD( "ic57_cpu.bin",  0xa000, 0x2000, CRC(a4b33b51) SHA1(12bb4da0319a7fe355e5ea4945759c8709aed5fe) )
	ROM_LOAD( "ic56_cpu.bin",  0xc000, 0x2000, CRC(2aabcfbf) SHA1(f0054af98bd68158eab3328f8cf2a04b35e812c7) )
	ROM_LOAD( "ic55_cpu.bin",  0xe000, 0x2000, CRC(b33a3c37) SHA1(2f1fdf1b9f18fcc9bd97cc9adeecc4ce77dd30c9) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for the audio CPU */
	ROM_LOAD( "319e02.bin",   0x0000, 0x2000, CRC(d5d45edb) SHA1(3808d1b58fe152f8f5b49bf0aa40c53e9c9dd4bd) )

	ROM_REGION( 0x1000, REGION_CPU3, 0 )     /* 4k for the 8039 DAC CPU */
	ROM_LOAD( "319e01.bin",   0x0000, 0x1000, CRC(ed5725a0) SHA1(64f54621487291fbfe827fb4cecca299fd0db781) )

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ic40_vid.bin",  0x0000, 0x2000, CRC(07b8b24b) SHA1(faadcb20ee8b26b9ab0692df6a81e5423514863e) )
	ROM_LOAD( "319e13.bin",    0x2000, 0x2000, CRC(3d8f3743) SHA1(1f6fbf804dacfa44cd11b4cf41d0bedb7f2ff6b6) )

	ROM_REGION( 0x08000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "319e11.bin",    0x0000, 0x2000, CRC(965a7ff6) SHA1(210aae91a3838e5f7c78747d9b7419d266538ffc) )
	ROM_LOAD( "319e09.bin",    0x2000, 0x2000, CRC(5eaa7f3e) SHA1(4c038e80d575988407252897a1f1bc6b76af597c) )
	ROM_LOAD( "319e10.bin",    0x4000, 0x2000, CRC(7bb1aeee) SHA1(be2dd46cd0121cedad6dab90a22643798a3176ab) )
	ROM_LOAD( "319e08.bin",    0x6000, 0x2000, CRC(6add71b1) SHA1(fc8c0ecd3b7f03d63b6c3143143986883345fa38) )

	ROM_REGION( 0x0260, REGION_PROMS, 0 )
	ROM_LOAD( "319b18.a16",  0x0000, 0x020, CRC(23cb02af) SHA1(aba459826a75ec07bc6d97580e934f58aa22f4f4) ) /* palette */
	ROM_LOAD( "319b16.c6",   0x0020, 0x100, CRC(5748e933) SHA1(c1478c31533a11421cd4579ccfdbb430e193d17b) ) /* sprite lookup table */
	ROM_LOAD( "319b17.a11",  0x0120, 0x100, CRC(1fbfce73) SHA1(1c58eb91982d5f10511d54a83070e859ac57d2f1) ) /* character lookup table */
	ROM_LOAD( "319b14.e7",   0x0220, 0x020, CRC(55044268) SHA1(29cf4158314ed897daf045a7f07be865dd977663) ) /* timing (not used) */
	ROM_LOAD( "319b15.e8",   0x0240, 0x020, CRC(31fd7ab9) SHA1(04d6e51b4930619c8ee12fb0d7b5f981e4d6d8d3) ) /* timing (not used) */
ROM_END

/* Info provided with these alt sets

        MEGA ZONE   CHIP PLACEMENT

USES 69A09EP, Z80 CPU'S & AY-3-8910 SOUND CHIP W/8039 CPU

THERE ARE AT LEAST THREE VERSIONS OF MEGA ZONE, ALL THE ROMS ARE THE
SAME EXCEPT POSITION 6,7,8,9,11H IN SETS 1,2
ALL ROMS ARE 2764 EXCEPT H01 (E01) IS A 2732

CHIP #     POSITION                         VERS 3
-----------------------------------------------------
VER-1                VER-2
-----------------------------------------------------
319-E08    2D        E08      REAR BOARD    8  SAME
319-E09    2E        E09       "            9  SAME
319-E10    3D        E10       "           10  SAME
319-E11    3E        E11       "           11  SAME
319-G12    8C        G12       "           12
319-G13    10C       G13       "           13  SAME
319-E02    6D        E02      CONN BOARD    2  SAME
319-H03    6H        J03       "            3
319-H04    7H        J04       "            4
319-H05    8H        J05       "            5
319-H06    9H        J06       "            6
319-H07    11H       J07       "            7
319-H01    3A        E01       "            1  SAME
Z80        7E                                        IC#
AY-3-8910  8B                               PROM     98     TBP18S030 (82S123)
AO72       12F   KONAMI                     PROM     48       "
K824-501   8D    KONAMI                     PROM     42       "
8039       4B                               PROM     63     TBP24S10  (823126)
                                            PROM     33       "
                                            PAL16L8  63
                                            PAL16L8A 67

VERSION 3 IS ON THE SAME SIZE CONNECTOR BOARD, BUT THE BOTTOM
BOARD IS ABOUT 1 1/4" LONGER AND WIDER

THE CHIPS THAT HAVE THE DESIGNATION SCRATCHED OFF ON THE ORIGINAL
BOARDS ARE      NAME          CHIP TYPE
            ---------------------------
CONN BOARD      IC3             TMP8039P-6
 "              IC6             AY3-8910
 "              IC26            Z-80
 "              IC39            MC68A09EP (CUSTOM ON ORIGINAL)
 "              IC27            N/U       (CUSTOM ON ORIGINAL)
REAR BOARD      1C026           N/U       (CUSTOM ON ORIGINAL)

*/

ROM_START( megazona )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for code */
	ROM_LOAD( "7.12g",  0x6000, 0x2000, CRC(d42d67bf) SHA1(adac80d183ad26a9b1ec25a2da7ebbb33b441b63) )
	ROM_LOAD( "6.10g",  0x8000, 0x2000, CRC(692398eb) SHA1(518001d738c2fb9417e52edfe9a7b74a074af3b0) )
	ROM_LOAD( "5.9g",   0xa000, 0x2000, CRC(620ffec3) SHA1(e047beb29e0cda72126e8dcdd0b7504a202efba2) )
	ROM_LOAD( "4.8g",   0xc000, 0x2000, CRC(28650971) SHA1(25e405fb9f648b7118e3c7c7b3ba59a7b7c29c42) )
	ROM_LOAD( "3.6g",   0xe000, 0x2000, CRC(f264018f) SHA1(6ca0f7e26311799b0650a6c58567405bc2a7f922) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for the audio CPU */
	ROM_LOAD( "319-h02",   0x0000, 0x2000, CRC(d5d45edb) SHA1(3808d1b58fe152f8f5b49bf0aa40c53e9c9dd4bd) )

	ROM_REGION( 0x1000, REGION_CPU3, 0 )     /* 4k for the 8039 DAC CPU */
	ROM_LOAD( "319-h01",   0x0000, 0x1000, CRC(ed5725a0) SHA1(64f54621487291fbfe827fb4cecca299fd0db781) ) // same as e01?

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "319e12.bin",    0x0000, 0x2000, CRC(e0fb7835) SHA1(44ccaaf92bdb83323f45e08dbe118697720e9105) ) // 12.9c
	ROM_LOAD( "319-g13",  0x2000, 0x2000, CRC(3d8f3743) SHA1(1f6fbf804dacfa44cd11b4cf41d0bedb7f2ff6b6) ) // same as e13?

	ROM_REGION( 0x08000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "319e11.bin",    0x0000, 0x2000, CRC(965a7ff6) SHA1(210aae91a3838e5f7c78747d9b7419d266538ffc) )
	ROM_LOAD( "319e09.bin",    0x2000, 0x2000, CRC(5eaa7f3e) SHA1(4c038e80d575988407252897a1f1bc6b76af597c) )
	ROM_LOAD( "319e10.bin",    0x4000, 0x2000, CRC(7bb1aeee) SHA1(be2dd46cd0121cedad6dab90a22643798a3176ab) )
	ROM_LOAD( "319e08.bin",    0x6000, 0x2000, CRC(6add71b1) SHA1(fc8c0ecd3b7f03d63b6c3143143986883345fa38) )

	ROM_REGION( 0x0260, REGION_PROMS, 0 )
	ROM_LOAD( "319b18.a16",  0x0000, 0x020, CRC(23cb02af) SHA1(aba459826a75ec07bc6d97580e934f58aa22f4f4) ) /* palette */
	ROM_LOAD( "319b16.c6",   0x0020, 0x100, CRC(5748e933) SHA1(c1478c31533a11421cd4579ccfdbb430e193d17b) ) /* sprite lookup table */
	ROM_LOAD( "319b17.a11",  0x0120, 0x100, CRC(1fbfce73) SHA1(1c58eb91982d5f10511d54a83070e859ac57d2f1) ) /* character lookup table */
	ROM_LOAD( "319b14.e7",   0x0220, 0x020, CRC(55044268) SHA1(29cf4158314ed897daf045a7f07be865dd977663) ) /* timing (not used) */
	ROM_LOAD( "319b15.e8",   0x0240, 0x020, CRC(31fd7ab9) SHA1(04d6e51b4930619c8ee12fb0d7b5f981e4d6d8d3) ) /* timing (not used) */
ROM_END

ROM_START( megazonb )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for code */
	ROM_LOAD( "319-j07",  0x6000, 0x2000,  CRC(5161a523) SHA1(90b456c30bccaaca96c75c2f421af3a2875b0b6b) )
	ROM_LOAD( "319-j06",  0x8000, 0x2000,  CRC(7344c3de) SHA1(d3867738d4828afa50c8b43116d68cc6074d6cb5) )
	ROM_LOAD( "319-j05",  0xa000, 0x2000,  CRC(affa492b) SHA1(ee6789f293902716d65d08a89ae12dd96c75c885) )
	ROM_LOAD( "319-j04",  0xc000, 0x2000,  CRC(03544ab3) SHA1(efa034cc6976b47915601cf215758df23e308878) )
	ROM_LOAD( "319-j03",  0xe000, 0x2000,  CRC(0d95cc0a) SHA1(9aadadf09a4826da451ee35c89ee0254ec552d80) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for the audio CPU */
	ROM_LOAD( "319-h02",   0x0000, 0x2000, CRC(d5d45edb) SHA1(3808d1b58fe152f8f5b49bf0aa40c53e9c9dd4bd) )

	ROM_REGION( 0x1000, REGION_CPU3, 0 )     /* 4k for the 8039 DAC CPU */
	ROM_LOAD( "319-h01",   0x0000, 0x1000, CRC(ed5725a0) SHA1(64f54621487291fbfe827fb4cecca299fd0db781) ) // same as e01?

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "319e12.bin",    0x0000, 0x2000, CRC(e0fb7835) SHA1(44ccaaf92bdb83323f45e08dbe118697720e9105) ) // 12.9c
	ROM_LOAD( "319-g13",  0x2000, 0x2000, CRC(3d8f3743) SHA1(1f6fbf804dacfa44cd11b4cf41d0bedb7f2ff6b6) ) // same as e13?

	ROM_REGION( 0x08000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "319e11.bin",    0x0000, 0x2000, CRC(965a7ff6) SHA1(210aae91a3838e5f7c78747d9b7419d266538ffc) )
	ROM_LOAD( "319e09.bin",    0x2000, 0x2000, CRC(5eaa7f3e) SHA1(4c038e80d575988407252897a1f1bc6b76af597c) )
	ROM_LOAD( "319e10.bin",    0x4000, 0x2000, CRC(7bb1aeee) SHA1(be2dd46cd0121cedad6dab90a22643798a3176ab) )
	ROM_LOAD( "319e08.bin",    0x6000, 0x2000, CRC(6add71b1) SHA1(fc8c0ecd3b7f03d63b6c3143143986883345fa38) )

	ROM_REGION( 0x0260, REGION_PROMS, 0 )
	ROM_LOAD( "319b18.a16",  0x0000, 0x020, CRC(23cb02af) SHA1(aba459826a75ec07bc6d97580e934f58aa22f4f4) ) /* palette */
	ROM_LOAD( "319b16.c6",   0x0020, 0x100, CRC(5748e933) SHA1(c1478c31533a11421cd4579ccfdbb430e193d17b) ) /* sprite lookup table */
	ROM_LOAD( "319b17.a11",  0x0120, 0x100, CRC(1fbfce73) SHA1(1c58eb91982d5f10511d54a83070e859ac57d2f1) ) /* character lookup table */
	ROM_LOAD( "319b14.e7",   0x0220, 0x020, CRC(55044268) SHA1(29cf4158314ed897daf045a7f07be865dd977663) ) /* timing (not used) */
	ROM_LOAD( "319b15.e8",   0x0240, 0x020, CRC(31fd7ab9) SHA1(04d6e51b4930619c8ee12fb0d7b5f981e4d6d8d3) ) /* timing (not used) */
ROM_END

ROM_START( megazonc )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for code */
	ROM_LOAD( "319-h07",  0x6000, 0x2000,  CRC(8ca47f64) SHA1(1a20db5ac504b9b004116cfa6992d63a86a04cc5) )
	ROM_LOAD( "319-h06",  0x8000, 0x2000,  CRC(ed35b12e) SHA1(69e88c4801c838a24aba0a867af205a7169ad089) )
	ROM_LOAD( "319-h05",  0xa000, 0x2000,  CRC(c3655ccd) SHA1(b86b58a12c6ced9a7e0a6d0cdb3881a28220a650) )
	ROM_LOAD( "319-h04",  0xc000, 0x2000,  CRC(9e221177) SHA1(0c6fffd657d66090362108578aa78eb36bdcce6b) )
	ROM_LOAD( "319-h03",  0xe000, 0x2000,  CRC(9048955b) SHA1(d8a7b46d984832f566298d3b417b3a34c9fea6c7) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )     /* 64k for the audio CPU */
	ROM_LOAD( "319-h02",   0x0000, 0x2000, CRC(d5d45edb) SHA1(3808d1b58fe152f8f5b49bf0aa40c53e9c9dd4bd) )

	ROM_REGION( 0x1000, REGION_CPU3, 0 )     /* 4k for the 8039 DAC CPU */
	ROM_LOAD( "319-h01",   0x0000, 0x1000, CRC(ed5725a0) SHA1(64f54621487291fbfe827fb4cecca299fd0db781) ) // same as e01?

	ROM_REGION( 0x04000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "319e12.bin",    0x0000, 0x2000, CRC(e0fb7835) SHA1(44ccaaf92bdb83323f45e08dbe118697720e9105) ) // 12.9c
	ROM_LOAD( "319-g13",  0x2000, 0x2000, CRC(3d8f3743) SHA1(1f6fbf804dacfa44cd11b4cf41d0bedb7f2ff6b6) ) // same as e13?

	ROM_REGION( 0x08000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "319e11.bin",    0x0000, 0x2000, CRC(965a7ff6) SHA1(210aae91a3838e5f7c78747d9b7419d266538ffc) )
	ROM_LOAD( "319e09.bin",    0x2000, 0x2000, CRC(5eaa7f3e) SHA1(4c038e80d575988407252897a1f1bc6b76af597c) )
	ROM_LOAD( "319e10.bin",    0x4000, 0x2000, CRC(7bb1aeee) SHA1(be2dd46cd0121cedad6dab90a22643798a3176ab) )
	ROM_LOAD( "319e08.bin",    0x6000, 0x2000, CRC(6add71b1) SHA1(fc8c0ecd3b7f03d63b6c3143143986883345fa38) )

	ROM_REGION( 0x0260, REGION_PROMS, 0 )
	ROM_LOAD( "319b18.a16",  0x0000, 0x020, CRC(23cb02af) SHA1(aba459826a75ec07bc6d97580e934f58aa22f4f4) ) /* palette */
	ROM_LOAD( "319b16.c6",   0x0020, 0x100, CRC(5748e933) SHA1(c1478c31533a11421cd4579ccfdbb430e193d17b) ) /* sprite lookup table */
	ROM_LOAD( "319b17.a11",  0x0120, 0x100, CRC(1fbfce73) SHA1(1c58eb91982d5f10511d54a83070e859ac57d2f1) ) /* character lookup table */
	ROM_LOAD( "319b14.e7",   0x0220, 0x020, CRC(55044268) SHA1(29cf4158314ed897daf045a7f07be865dd977663) ) /* timing (not used) */
	ROM_LOAD( "prom.48",     0x0240, 0x020, CRC(796dea94) SHA1(bab3c2a5466e1c07ec27cccf7b1a21e9de4ed982) ) /* timing (not used) */
ROM_END


static DRIVER_INIT( megazone )
{
	konami1_decode();
}

/* these just display a Konami copyright, no logo */
GAME( 1983, megazone, 0,        megazone, megazone, megazone, ROT90, "Konami",                       "Mega Zone (Konami set 1)", 0 ,2)
GAME( 1983, megazona, megazone, megazone, megazone, megazone, ROT90, "Konami",                       "Mega Zone (Konami set 2)", 0 ,2)

/* these display Konami and Kosuka copyright, no logo */
GAME( 1983, megazonb, megazone, megazone, megazone, megazone, ROT90, "Konami / Kosuka",              "Mega Zone (Kosuka set 1)", 0 ,2)
GAME( 1983, megazonc, megazone, megazone, megazone, megazone, ROT90, "Konami / Kosuka",              "Mega Zone (Kosuka set 2)", 0 ,2)

/* this displays Konami and Kosuka copyright with a Konami / Interlogic logo */
GAME( 1983, megaznik, megazone, megazone, megazone, megazone, ROT90, "Konami / Interlogic + Kosuka", "Mega Zone (Interlogic + Kosuka)", 0 ,2)
