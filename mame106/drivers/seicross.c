/***************************************************************************

Seicross memory map (preliminary)

driver by Nicola Salmoria


0000-77ff ROM
7800-7fff RAM
9000-93ff videoram
9c00-9fff colorram

Read:
A000      Joystick + Players start button
A800      player #2 controls + coin + ?
B000      test switches
B800      watchdog reset

Write:
8820-887f Sprite ram
9800-981f Scroll control
9880-989f ? (always 0?)

I/O ports:
0         8910 control
1         8910 write
4         8910 read


There is a microcontroller on the board. Nichibutsu custom part marked
NSC81050-102  8127 E37 and labeled No. 00363.  It's a 40-pin IC at location 4F
on the (Seicross-) board. Looks like it is linked to the dips (and those are
on a very small daughterboard).


Differences in new/old version of Frisky Tom
- The lady wears bikini in new version
- Game config is backed up by 4.5v battery in old version
- Old version uses larger board

This info came from http://www.ne.jp/asahi/cc-sakura/akkun/old/fryski.html

***************************************************************************/

#include "driver.h"
#include "sound/ay8910.h"
#include "sound/dac.h"

extern UINT8 *seicross_row_scroll;

extern WRITE8_HANDLER( seicross_videoram_w );
extern WRITE8_HANDLER( seicross_colorram_w );

extern PALETTE_INIT( seicross );
extern VIDEO_START( seicross );
extern VIDEO_UPDATE( seicross );

static UINT8 *nvram;
static size_t nvram_size;

static UINT8 portb;


static NVRAM_HANDLER( seicross )
{
	if (read_or_write)
		mame_fwrite(file,nvram,nvram_size);
	else
	{
		if (file)
			mame_fread(file,nvram,nvram_size);
		else
		{
			/* fill in the default values */
			memset(nvram,0,nvram_size);
			nvram[0x0d] = nvram[0x0f] = nvram[0x11] = nvram[0x13] = nvram[0x15] = nvram[0x19] = 1;
			nvram[0x17] = 3;
		}
	}
}



static MACHINE_RESET( friskyt )
{
	/* start with the protection mcu halted */
	cpunum_set_input_line(1, INPUT_LINE_HALT, ASSERT_LINE);
}



static READ8_HANDLER( friskyt_portB_r )
{
	return (portb & 0x9f) | (readinputport(6) & 0x60);
}

static WRITE8_HANDLER( friskyt_portB_w )
{
//logerror("PC %04x: 8910 port B = %02x\n",activecpu_get_pc(),data);
	/* bit 0 is IRQ enable */
	cpu_interrupt_enable(0,data & 1);

	/* bit 1 flips screen */

	/* bit 2 resets the microcontroller */
	if (((portb & 4) == 0) && (data & 4))
	{
		/* reset and start the protection mcu */
		cpunum_set_input_line(1, INPUT_LINE_RESET, PULSE_LINE);
		cpunum_set_input_line(1, INPUT_LINE_HALT, CLEAR_LINE);
	}

	/* other bits unknown */
	portb = data;
}


static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x77ff) AM_ROM
	AM_RANGE(0x7800, 0x7fff) AM_RAM AM_SHARE(1)
	AM_RANGE(0x8820, 0x887f) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x9000, 0x93ff) AM_READWRITE(MRA8_RAM, seicross_videoram_w) AM_BASE(&videoram)	/* video RAM */
	AM_RANGE(0x9800, 0x981f) AM_RAM AM_BASE(&seicross_row_scroll)
	AM_RANGE(0x9880, 0x989f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_2) AM_SIZE(&spriteram_2_size)
	AM_RANGE(0x9c00, 0x9fff) AM_READWRITE(MRA8_RAM, seicross_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xa000, 0xa000) AM_READ(input_port_0_r)	/* IN0 */
	AM_RANGE(0xa800, 0xa800) AM_READ(input_port_1_r)	/* IN1 */
	AM_RANGE(0xb000, 0xb000) AM_READ(input_port_2_r)	/* test */
	AM_RANGE(0xb800, 0xb800) AM_READ(watchdog_reset_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( main_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x04, 0x04) AM_READ(AY8910_read_port_0_r)
	AM_RANGE(0x08, 0x08) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x09, 0x09) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x0c, 0x0c) AM_READ(AY8910_read_port_0_r)
ADDRESS_MAP_END


static ADDRESS_MAP_START( mcu_nvram_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x007f) AM_RAM
	AM_RANGE(0x1000, 0x10ff) AM_RAM AM_BASE(&nvram) AM_SIZE(&nvram_size)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(DAC_0_data_w)
	AM_RANGE(0x8000, 0xf7ff) AM_ROM
	AM_RANGE(0xf800, 0xffff) AM_RAM AM_SHARE(1)
ADDRESS_MAP_END

static ADDRESS_MAP_START( mcu_no_nvram_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x007f) AM_RAM
	AM_RANGE(0x1003, 0x1003) AM_READ(input_port_3_r)	/* DSW1 */
	AM_RANGE(0x1005, 0x1005) AM_READ(input_port_4_r)	/* DSW2 */
	AM_RANGE(0x1006, 0x1006) AM_READ(input_port_5_r)	/* DSW3 */
	AM_RANGE(0x2000, 0x2000) AM_WRITE(DAC_0_data_w)
	AM_RANGE(0x8000, 0xf7ff) AM_ROM
	AM_RANGE(0xf800, 0xffff) AM_RAM AM_SHARE(1)
ADDRESS_MAP_END




INPUT_PORTS_START( friskyt )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_SERVICE( 0x20, IP_ACTIVE_HIGH )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x00, "Counter Check" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START      /* Test */
	PORT_DIPNAME( 0x01, 0x00, "Test Mode" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "Connection Error" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )	/* probably unused */
INPUT_PORTS_END

INPUT_PORTS_START( radrad )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* Test */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x06, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x06, "5" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* DSW2 */
	PORT_DIPNAME( 0x0f, 0x01, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x0f, "7 Coins/2 Credits" )
	PORT_DIPSETTING(    0x0e, "6 Coins/2 Credits" )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x0d, "5 Coins/2 Credits" )
	PORT_DIPSETTING(    0x0c, DEF_STR( 4C_2C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* DSW3 */
	PORT_DIPNAME( 0x0f, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_8C ) )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

INPUT_PORTS_START( seicross )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL

	PORT_START      /* Test */
	PORT_SERVICE( 0x01, IP_ACTIVE_HIGH )
	PORT_DIPNAME( 0x02, 0x00, "Connection Error" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "20000 40000" )
	PORT_DIPSETTING(    0x04, "30000" )
	PORT_DIPSETTING(    0x08, "30000 50000" )
	PORT_DIPSETTING(    0x0c, "30000 60000 90000" )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* DSW2 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* DSW3 */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ) )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* Debug */
	PORT_BIT( 0x1f, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x20, 0x20, "Debug Mode" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Invulnerability" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	512,	/* 512 characters */
	2,	/* 2 bits per pixel */
	{ 0, 4 },	/* the two bitplanes are packed in one byte */
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8	/* every char takes 16 consecutive bytes */
};
static const gfx_layout spritelayout =
{
	16,16,	/* 16*16 sprites */
	256,	/* 256 sprites */
	2,	/* 2 bits per pixel */
	{ 0, 4 },	/* the two bitplanes are packed in one byte */
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			16*8+0, 16*8+1, 16*8+2, 16*8+3, 17*8+0, 17*8+1, 17*8+2, 17*8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			16*16, 17*16, 18*16, 19*16, 20*16, 21*16, 22*16, 23*16 },
	64*8	/* every sprite takes 64 consecutive bytes */
};



static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0, 16 },
	{ REGION_GFX1, 0, &spritelayout, 0, 16 },
	{ -1 } /* end of array */
};



static struct AY8910interface ay8910_interface =
{
	0,
	friskyt_portB_r,
	0,
	friskyt_portB_w
};


static MACHINE_DRIVER_START( nvram )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 3072000)	/* 3.072 MHz? */
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_IO_MAP(main_portmap,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD_TAG("mcu", NSC8105, 6000000/4)	/* ??? */
	MDRV_CPU_PROGRAM_MAP(mcu_nvram_map,0)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)	/* frames per second, vblank duration */
	MDRV_INTERLEAVE(20)	/* 20 CPU slices per frame - an high value to ensure proper */
						/* synchronization of the CPUs */
	MDRV_MACHINE_RESET(friskyt)
	MDRV_NVRAM_HANDLER(seicross)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64)

	MDRV_PALETTE_INIT(seicross)
	MDRV_VIDEO_START(seicross)
	MDRV_VIDEO_UPDATE(seicross)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1536000)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( no_nvram )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(nvram)
	MDRV_CPU_MODIFY("mcu")
	MDRV_CPU_PROGRAM_MAP(mcu_no_nvram_map,0)

	MDRV_NVRAM_HANDLER(NULL)
MACHINE_DRIVER_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( friskyt )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "ftom.01",      0x0000, 0x1000, CRC(bce5d486) SHA1(b3226d5737490f18092227a663e89ad48f39d82c) )
	ROM_LOAD( "ftom.02",      0x1000, 0x1000, CRC(63157d6e) SHA1(2792f3d918ffee3818eca98f52192a069ab60678) )
	ROM_LOAD( "ftom.03",      0x2000, 0x1000, CRC(c8d9ef2c) SHA1(43dd6bfd93188004b977b97120df28c028e8582b) )
	ROM_LOAD( "ftom.04",      0x3000, 0x1000, CRC(23a01aac) SHA1(db514c54c1a089a900abf954035ae4d1093e778d) )
	ROM_LOAD( "ftom.05",      0x4000, 0x1000, CRC(bfaf702a) SHA1(d42fa3e935bfc5bfbab582343aaafc86ebcbfda2) )
	ROM_LOAD( "ftom.06",      0x5000, 0x1000, CRC(bce70b9c) SHA1(85d2811f15cba7d0424d5ca024c0c26ee0b2a32a) )
	ROM_LOAD( "ftom.07",      0x6000, 0x1000, CRC(b2ef303a) SHA1(a7150457b454e15c06fa832d42dd1f0e165fcd6e) )
	ROM_LOAD( "ft8_8.rom",    0x7000, 0x0800, CRC(10461a24) SHA1(c1f98316a4e90a2a6ef4953708b90c9546caaedd) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the protection mcu */
	ROM_COPY( REGION_CPU1, 0x0000, 0x8000, 0x8000 )	/* shares the main program ROMs and RAM with the main CPU. */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ftom.11",      0x0000, 0x1000, CRC(1ec6ff65) SHA1(aab589c89cd14549b35f4dece5d3c231033c0c1a) )
	ROM_LOAD( "ftom.12",      0x1000, 0x1000, CRC(3b8f40b5) SHA1(08e0c1fce11ee6c507c28b0d659c5b010f2f2b6f) )
	ROM_LOAD( "ftom.09",      0x2000, 0x1000, CRC(60642f25) SHA1(2d179a9ea99014065f578bbec4fbfbda5aead98b) )
	ROM_LOAD( "ftom.10",      0x3000, 0x1000, CRC(07b9dcfc) SHA1(0a573065b6b08745b91fb47ce477d76be7a01750) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "ft.9c",        0x0000, 0x0020, CRC(0032167e) SHA1(9df3c7bbf6b700bfa51b8b82c45b60c10bdcd1a0) )
	ROM_LOAD( "ft.9b",        0x0020, 0x0020, CRC(6b364e69) SHA1(abfcab884e8a50f872f862a421b8e8c5e16ff62c) )
ROM_END

ROM_START( friskyta )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "ft.01",        0x0000, 0x1000, CRC(0ea46e19) SHA1(3feb3ee882926c0efa602cf92e6879e84a6050ed) )
	ROM_LOAD( "ft.02",        0x1000, 0x1000, CRC(4f7b8662) SHA1(400c47d7ab5f3a749dbadb2286255b969ec48348) )
	ROM_LOAD( "ft.03",        0x2000, 0x1000, CRC(1eb1b77c) SHA1(c08d6c1f1bbe2d41b0f6336a0c53ec993556e6b4) )
	ROM_LOAD( "ft.04",        0x3000, 0x1000, CRC(b5c5400d) SHA1(9fa87dd287457c61599214469aad095ddb5f8742) )
	ROM_LOAD( "ft.05",        0x4000, 0x1000, CRC(b465be8a) SHA1(0b0da2c83c2362d062b12312285076956d62e4b4) )
	ROM_LOAD( "ft.06",        0x5000, 0x1000, CRC(90141317) SHA1(d59489e4e35308858e0548d5861b1781acfc3c05) )
	ROM_LOAD( "ft.07",        0x6000, 0x1000, CRC(0ba02b2e) SHA1(1260c16d589fca37bf58ee28a4795f4b6333d0b9) )
	ROM_LOAD( "ft8_8.rom",    0x7000, 0x0800, CRC(10461a24) SHA1(c1f98316a4e90a2a6ef4953708b90c9546caaedd) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the protection mcu */
	ROM_COPY( REGION_CPU1, 0x0000, 0x8000, 0x8000 )	/* shares the main program ROMs and RAM with the main CPU. */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ft.11",        0x0000, 0x1000, CRC(956d924a) SHA1(e61bf5f187932c6cb676b4120cd95fe422f6a1a6) )
	ROM_LOAD( "ft.12",        0x1000, 0x1000, CRC(c028d3b8) SHA1(9e8768b9658f8b05ade4dd5fb2ecde4a52627bc1) )
	ROM_LOAD( "ftom.09",      0x2000, 0x1000, CRC(60642f25) SHA1(2d179a9ea99014065f578bbec4fbfbda5aead98b) )
	ROM_LOAD( "ftom.10",      0x3000, 0x1000, CRC(07b9dcfc) SHA1(0a573065b6b08745b91fb47ce477d76be7a01750) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "ft.9c",        0x0000, 0x0020, CRC(0032167e) SHA1(9df3c7bbf6b700bfa51b8b82c45b60c10bdcd1a0) )
	ROM_LOAD( "ft.9b",        0x0020, 0x0020, CRC(6b364e69) SHA1(abfcab884e8a50f872f862a421b8e8c5e16ff62c) )
ROM_END

ROM_START( radrad )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "1.3a",         0x0000, 0x1000, CRC(b1e958ca) SHA1(3ab5fc3314f202ba527470eacbb76d52fe969bca) )
	ROM_LOAD( "2.3b",         0x1000, 0x1000, CRC(30ba76b3) SHA1(e6af1fc35fdc71d5436f0d29e5722cbcb4409196) )
	ROM_LOAD( "3.3c",         0x2000, 0x1000, CRC(1c9f397b) SHA1(7f556c5bef5309d5048c3b9671b88ad646a8b648) )
	ROM_LOAD( "4.3d",         0x3000, 0x1000, CRC(453966a3) SHA1(dd1bfeb8956c4670a5d4a5e981413b47701f6233) )
	ROM_LOAD( "5.3e",         0x4000, 0x1000, CRC(c337c4bd) SHA1(a5d29e9ba629d23f8c084fdb0ce4a83513648e82) )
	ROM_LOAD( "6.3f",         0x5000, 0x1000, CRC(06e15b59) SHA1(0c7748abba29362c92724e601d90ad1711b23f86) )
	ROM_LOAD( "7.3g",         0x6000, 0x1000, CRC(02b1f9c9) SHA1(6b857ae477d3c92a58494140ffa3337dba8e77cc) )
	ROM_LOAD( "8.3h",         0x7000, 0x0800, CRC(911c90e8) SHA1(94fa91e767ab27a1616f1768f97a44a59a3f3294) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the protection mcu */
	ROM_COPY( REGION_CPU1, 0x0000, 0x8000, 0x8000 )	/* shares the main program ROMs and RAM with the main CPU. */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "11.l7",        0x0000, 0x1000, CRC(4ace7afb) SHA1(3c495f106505d5dfed93393db1f1b3842f603448) )
	ROM_LOAD( "12.n7",        0x1000, 0x1000, CRC(b19b8473) SHA1(42160f978f8e209a89be097b5cfc7ac0aeec49c5) )
	ROM_LOAD( "9.j7",         0x2000, 0x1000, CRC(229939a3) SHA1(4ee050798871823314952e34938233e2cf9e7341) )
	ROM_LOAD( "10.j7",        0x3000, 0x1000, CRC(79237913) SHA1(b07dd531d06ef01f756169e87a8cccda35ed38d3) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "clr.9c",       0x0000, 0x0020, CRC(c9d88422) SHA1(626216bac1a6317a32f2a51b89375043f58b5503) )
	ROM_LOAD( "clr.9b",       0x0020, 0x0020, CRC(ee81af16) SHA1(e1bab9738d37dea0473a7184a4303234b75e6cc6) )
ROM_END

ROM_START( seicross )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "smc1",         0x0000, 0x1000, CRC(f6c3aeca) SHA1(d57019e80f7e3d47ca74f54604e92d40ba9819fc) )
	ROM_LOAD( "smc2",         0x1000, 0x1000, CRC(0ec6c218) SHA1(d8cffea48d8afd229f2008399afe3858c13653e5) )
	ROM_LOAD( "smc3",         0x2000, 0x1000, CRC(ceb3c8f4) SHA1(e49f834637b4addcf362cd010e31802c3e145cbe) )
	ROM_LOAD( "smc4",         0x3000, 0x1000, CRC(3112af59) SHA1(3d4e5a74a13bdeaf07f059f8c3a0d2ca8cbb3d32) )
	ROM_LOAD( "smc5",         0x4000, 0x1000, CRC(b494a993) SHA1(ed60cbaef2ac780c11426d29a612d34e76b29a0e) )
	ROM_LOAD( "smc6",         0x5000, 0x1000, CRC(09d5b9da) SHA1(636a8d4717df4ed1fc02fa83782fa8d96b88f969) )
	ROM_LOAD( "smc7",         0x6000, 0x1000, CRC(13052b03) SHA1(2866f2533a788f734310a74789f762f3fa17a57a) )
	ROM_LOAD( "smc8",         0x7000, 0x0800, CRC(2093461d) SHA1(0d640bc7ee1e9ffe32580e3143677475145b06d2) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the protection mcu */
	ROM_COPY( REGION_CPU1, 0x0000, 0x8000, 0x8000 )	/* shares the main program ROMs and RAM with the main CPU. */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "sz11.7k",      0x0000, 0x1000, CRC(fbd9b91d) SHA1(6b3581f4b518c058b970d569ced07dd7dc6a87e6) )
	ROM_LOAD( "smcd",         0x1000, 0x1000, CRC(c3c953c4) SHA1(a96937a48b59b7e992e53d279c10a5f3ea7f9a6f) )
	ROM_LOAD( "sz9.7j",       0x2000, 0x1000, CRC(4819f0cd) SHA1(fa8d371efc3198daf76ff1264e22673c5521becf) )
	ROM_LOAD( "sz10.7h",      0x3000, 0x1000, CRC(4c268778) SHA1(a1444fb3eb397c8167d769aa1f935c5f19df4d6d) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "sz73.10c",     0x0000, 0x0020, CRC(4d218a3c) SHA1(26364dfdb7e13080357328a06c3bcf504778defd) )
	ROM_LOAD( "sz74.10b",     0x0020, 0x0020, CRC(c550531c) SHA1(d564aeb8a99861d29e00cf968242fe6c6cec478b) )
ROM_END

ROM_START( sectrzon )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "sz1.3a",       0x0000, 0x1000, CRC(f0a45cb4) SHA1(ab3b8d78e25cdbb2fd6a6c0718ae13767364994d) )
	ROM_LOAD( "sz2.3c",       0x1000, 0x1000, CRC(fea68ddb) SHA1(b9ed0cad9a2ded04bcc7042d975b77be63313070) )
	ROM_LOAD( "sz3.3d",       0x2000, 0x1000, CRC(baad4294) SHA1(e7fc3ccc940de6df8d786c986b602127c9db9ebb) )
	ROM_LOAD( "sz4.3e",       0x3000, 0x1000, CRC(75f2ca75) SHA1(fbf990edcb7b5a58f8dcee160883fde5e222ca6b) )
	ROM_LOAD( "sz5.3fg",      0x4000, 0x1000, CRC(dc14f2c8) SHA1(dcda8d6f7be458d0adcddc37bbe0eb636a5b0b06) )
	ROM_LOAD( "sz6.3h",       0x5000, 0x1000, CRC(397a38c5) SHA1(6189028376c1781aae107c5fe0aec181a1d885e1) )
	ROM_LOAD( "sz7.3i",       0x6000, 0x1000, CRC(7b34dc1c) SHA1(fb163a908c991cd214e0d2d685e74563a460a929) )
	ROM_LOAD( "sz8.3j",       0x7000, 0x0800, CRC(9933526a) SHA1(2178ef8653f1d60be28bcaebe1033ef7ae480157) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the protection mcu */
	ROM_COPY( REGION_CPU1, 0x0000, 0x8000, 0x8000 )	/* shares the main program ROMs and RAM with the main CPU. */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "sz11.7k",      0x0000, 0x1000, CRC(fbd9b91d) SHA1(6b3581f4b518c058b970d569ced07dd7dc6a87e6) )
	ROM_LOAD( "sz12.7m",      0x1000, 0x1000, CRC(2bdef9ad) SHA1(50fe41e81c1307317b4fb6b47bf0619d141c42ff) )
	ROM_LOAD( "sz9.7j",       0x2000, 0x1000, CRC(4819f0cd) SHA1(fa8d371efc3198daf76ff1264e22673c5521becf) )
	ROM_LOAD( "sz10.7h",      0x3000, 0x1000, CRC(4c268778) SHA1(a1444fb3eb397c8167d769aa1f935c5f19df4d6d) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "sz73.10c",     0x0000, 0x0020, CRC(4d218a3c) SHA1(26364dfdb7e13080357328a06c3bcf504778defd) )
	ROM_LOAD( "sz74.10b",     0x0020, 0x0020, CRC(c550531c) SHA1(d564aeb8a99861d29e00cf968242fe6c6cec478b) )
ROM_END



GAME( 1981, friskyt,  0,        nvram,    friskyt,  0, ROT0,  "Nichibutsu", "Frisky Tom (set 1)", GAME_NO_COCKTAIL ,0)
GAME( 1981, friskyta, friskyt,  nvram,    friskyt,  0, ROT0,  "Nichibutsu", "Frisky Tom (set 2)", GAME_NO_COCKTAIL ,0)
GAME( 1982, radrad,   0,        no_nvram, radrad,   0, ROT0,  "Nichibutsu USA", "Radical Radial", GAME_NO_COCKTAIL ,2)
GAME( 1984, seicross, 0,        no_nvram, seicross, 0, ROT90, "Nichibutsu + Alice", "Seicross", GAME_NO_COCKTAIL ,0)
GAME( 1984, sectrzon, seicross, no_nvram, seicross, 0, ROT90, "Nichibutsu + Alice", "Sector Zone", GAME_NO_COCKTAIL ,0)
