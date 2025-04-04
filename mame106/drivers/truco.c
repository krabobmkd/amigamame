/*

  Truco-Tron - (c) 198? Playtronic SRL, Argentina

  Written by Ernesto Corvi

  Notes:
  - The board uses a battery backed ram for protection, mapped at $7c00-$7fff.
  - If the battery backup data is corrupt, it comes up with some sort of code entry screen.
    As far as I can tell, you can't do anything with it.
  - Replacing the battery backed ram with an eeprom is not really an option since the game stores the
    current credits count in the battery backed ram.
  - System clock is 12 Mhz. The CPU clock is unknown.
  - The Alternate Gfx mode is funky. Not only it has different bitmaps, but also the strings with the
    game options are truncated. Title is also truncated.
*/

#include "driver.h"
#include "cpu/m6809/m6809.h"
#include "sound/dac.h"

static UINT8 *battery_ram;

/* from vidhrdw */
VIDEO_UPDATE( truco );
PALETTE_INIT( truco );


/***************************************************************************/


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x17ff) AM_READ(MRA8_RAM)		/* general purpose ram */
	AM_RANGE(0x1800, 0x7bff) AM_READ(MRA8_RAM)		/* video ram */
	AM_RANGE(0x7c00, 0x7fff) AM_READ(MRA8_RAM)		/* battery backed ram */
	AM_RANGE(0x8000, 0x8000) AM_READ(input_port_0_r)	/* controls (and irq ack?) */
	AM_RANGE(0x8001, 0x8001) AM_READ(MRA8_NOP)		/* unknown */
	AM_RANGE(0x8002, 0x8002) AM_READ(input_port_1_r)	/* dipswitches */
	AM_RANGE(0x8003, 0x8007) AM_READ(MRA8_NOP)		/* unknown */
	AM_RANGE(0x8008, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x17ff) AM_WRITE(MWA8_RAM)		/* general purpose ram */
	AM_RANGE(0x1800, 0x7bff) AM_WRITE(MWA8_RAM)	AM_BASE(&videoram)	/* video ram */
	AM_RANGE(0x7c00, 0x7fff) AM_WRITE(MWA8_RAM) AM_BASE(&battery_ram)		/* battery backed ram */
	AM_RANGE(0x8000, 0x8001) AM_WRITE(MWA8_NOP)		/* unknown */
	AM_RANGE(0x8002, 0x8002) AM_WRITE(DAC_0_data_w)
	AM_RANGE(0x8003, 0x8007) AM_WRITE(MWA8_NOP)		/* unknown */
	AM_RANGE(0x8008, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

INPUT_PORTS_START( truco )
	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )

	PORT_START /* DSW1 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x01, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Alt. Graphics" )
	PORT_DIPSETTING (	0x02, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x04, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x08, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x10, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x20, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x40, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING (	0x80, DEF_STR( Off ) )
	PORT_DIPSETTING (	0x00, DEF_STR( On ) )

	PORT_START	/* IN1 - FAKE - Used for coinup */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static MACHINE_RESET( truco )
{
	int a;

	/* Setup the data on the battery backed RAM */

	/* IRQ check */
	battery_ram[0x002] = 0x51;
	battery_ram[0x024] = 0x49;
	battery_ram[0x089] = 0x04;
	battery_ram[0x170] = 0x12;
	battery_ram[0x1a8] = 0xd5;

	/* Mainloop check */
	battery_ram[0x005] = 0x04;
	battery_ram[0x22B] = 0x46;
	battery_ram[0x236] = 0xfb;
	battery_ram[0x2fe] = 0x1D;
	battery_ram[0x359] = 0x5A;

	/* Boot check */
	a = ( battery_ram[0x000] << 8 ) | battery_ram[0x001];

	a += 0x4d2;

	battery_ram[0x01d] = ( a >> 8 ) & 0xff;
	battery_ram[0x01e] = a & 0xff;
	battery_ram[0x020] = battery_ram[0x011];
}

static INTERRUPT_GEN( truco_interrupt )
{
	/* coinup */
	static int trigger = 0;

	if ( readinputport( 2 ) & 1 )
	{
		if ( trigger == 0 )
		{
			cpunum_set_input_line( 0, M6809_IRQ_LINE, PULSE_LINE );
			trigger++;
		}
	} else
		trigger = 0;
}


static MACHINE_DRIVER_START( truco )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6809, 750000)        /* ?? guess */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_CPU_VBLANK_INT(truco_interrupt,1)

	MDRV_MACHINE_RESET(truco);

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 192)
	MDRV_VISIBLE_AREA(0, 256-1, 0, 192-1)
	MDRV_PALETTE_LENGTH(16)

	MDRV_PALETTE_INIT(truco)
	MDRV_VIDEO_UPDATE(truco)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( truco )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "truco.u3",   0x08000, 0x4000, CRC(4642fb96) SHA1(e821f6fd582b141a5ca2d5bd53f817697048fb81) )
	ROM_LOAD( "truco.u2",   0x0c000, 0x4000, CRC(ff355750) SHA1(1538f20b1919928ffca439e4046a104ddfbc756c) )
ROM_END

GAME( 198?, truco,  0, truco, truco, 0, ROT0, "Playtronic SRL", "Truco-Tron", GAME_IMPERFECT_SOUND ,0)
