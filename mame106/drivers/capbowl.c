/***************************************************************************

    Coors Light Bowling/Bowl-O-Rama hardware

    driver by Zsolt Vasvari

    Games supported:
        * Capcom/Coors Light Bowling
        * Bowlorama

    Known issues:
        * none

****************************************************************************

    CPU Board:

    0000-3fff     3 Graphics ROMS mapped in using 0x4800 (Coors Light Bowling only)
    0000-001f       Turbo board area (Bowl-O-Rama only) See Below.
    4000          Display row selected
    4800          Graphics ROM select
    5000-57ff     Battery backed up RAM (Saves machine state after shut off)
               Enter setup menu by holding down the F2 key on the
               high score screen
    5800-5fff       TMS34061 area

               First 0x20 bytes of each row provide a 16 color palette for this
               row. 2 bytes per color: 0000RRRR GGGGBBBB.

               Remaining 0xe0 bytes contain 2 pixels each for a total of
               448 pixels, but only 360 seem to be displayed.
               (Each row appears vertically because the monitor is rotated)

    6000          Sound command
    6800            Trackball Reset. Double duties as a watchdog.
    7000          Input port 1    Bit 0-3 Trackball Vertical Position
                                Bit 4   Player 2 Hook Left
                                Bit 5   Player 2 Hook Right
                                Bit 6   Upright/Cocktail DIP Switch
                               Bit 7   Coin 2
    7800          Input port 2    Bit 0-3 Trackball Horizontal Positon
                               Bit 4   Player 1 Hook Left
                               Bit 5   Player 1 Hook Right
                               Bit 6   Start
                               Bit 7   Coin 1
    8000-ffff       ROM


    Sound Board:

    0000-07ff       RAM
    1000-1001       YM2203
                Port A D7 Read  is ticket sensor
                Port B D7 Write is ticket dispenser enable
                Port B D6 looks like a diagnostics LED to indicate that
                          the PCB is operating. It's pulsated by the
                          sound CPU. It is kind of pointless to emulate it.
    2000            Not hooked up according to the schematics
    6000            DAC write
    7000            Sound command read (0x34 is used to dispense a ticket)
    8000-ffff       ROM


    Turbo Board Layout (Plugs in place of GR0):

    Bowl-O-Rama Copyright 1991 P&P Marketing
                Marquee says "EXIT Entertainment"

                This portion: Mike Appolo with the help of Andrew Pines.
                Andrew was one of the game designers for Capcom Bowling,
                Coors Light Bowling, Strata Bowling, and Bowl-O-Rama.

                This game was an upgrade for Capcom Bowling and included a
                "Turbo PCB" that had a GAL address decoder / data mask

    Memory Map for turbo board (where GR0 is on Capcom Bowling PCBs:

    0000        Read Mask
    0001-0003       Unused
    0004        Read Data
    0005-0007       Unused
    0008        GR Address High Byte (GR17-16)
    0009-0016       Unused
    0017            GR Address Middle Byte (GR15-0 written as a word to 0017-0018)
    0018        GR address Low byte
    0019-001f       Unused

***************************************************************************/

#include "driver.h"
#include "machine/ticket.h"
#include "cpu/m6809/m6809.h"
#include "capbowl.h"
#include "sound/2203intf.h"
#include "sound/dac.h"

#define MASTER_CLOCK		8000000		/* 8MHz crystal */

static UINT8 last_trackball_val[2];



/*************************************
 *
 *  NMI is to trigger the self test.
 *  We use a fake input port to tie
 *  that event to a keypress
 *
 *************************************/

static INTERRUPT_GEN( capbowl_interrupt )
{
	if (readinputport(4) & 1)	/* get status of the F2 key */
		cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);	/* trigger self test */
}



/*************************************
 *
 *  Partial updating
 *
 *************************************/

static void capbowl_update(int scan)
{
	force_partial_update(scan - 1);
	scan += 32;
	if (scan > 240) scan = 32;
	timer_set(cpu_getscanlinetime(scan), scan, capbowl_update);
}


static MACHINE_RESET( capbowl )
{
	timer_set(cpu_getscanlinetime(32), 32, capbowl_update);
}



/*************************************
 *
 *  Graphics ROM banking
 *
 *************************************/

WRITE8_HANDLER( capbowl_rom_select_w )
{
	int bankaddress = ((data & 0x0c) << 13) + ((data & 0x01) << 14);
	memory_set_bankptr(1, memory_region(REGION_CPU1) + 0x10000 + bankaddress);
}



/*************************************
 *
 *  Trackball input handlers
 *
 *************************************/

static READ8_HANDLER( track_0_r )
{
	return (input_port_0_r(offset) & 0xf0) | ((input_port_2_r(offset) - last_trackball_val[0]) & 0x0f);
}


static READ8_HANDLER( track_1_r )
{
	return (input_port_1_r(offset) & 0xf0) | ((input_port_3_r(offset) - last_trackball_val[1]) & 0x0f);
}


static WRITE8_HANDLER( track_reset_w )
{
	/* reset the trackball counters */
	last_trackball_val[0] = input_port_2_r(offset);
	last_trackball_val[1] = input_port_3_r(offset);

	watchdog_reset_w(offset,data);
}



/*************************************
 *
 *  Sound commands
 *
 *************************************/

static WRITE8_HANDLER( capbowl_sndcmd_w )
{
	cpunum_set_input_line(1, M6809_IRQ_LINE, HOLD_LINE);
	soundlatch_w(offset, data);
}



/*************************************
 *
 *  Handler called by the 2203 emulator
 *  when the internal timers cause an IRQ
 *
 *************************************/

static void firqhandler(int irq)
{
	cpunum_set_input_line(1, 1, irq ? ASSERT_LINE : CLEAR_LINE);
}



/*************************************
 *
 *  NVRAM
 *
 *************************************/

static NVRAM_HANDLER( capbowl )
{
	if (read_or_write)
		mame_fwrite(file, generic_nvram, generic_nvram_size);
	else if (file)
		mame_fread(file, generic_nvram, generic_nvram_size);
	else
	{
		/* invalidate nvram to make the game initialize it.
           A 0xff fill will cause the game to malfunction, so we use a
           0x01 fill which seems OK */
		memset(generic_nvram,0x01,generic_nvram_size);
	}
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( capbowl_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROMBANK(1)
	AM_RANGE(0x4000, 0x4000) AM_WRITE(MWA8_RAM) AM_BASE(&capbowl_rowaddress)
	AM_RANGE(0x4800, 0x4800) AM_WRITE(capbowl_rom_select_w)
	AM_RANGE(0x5000, 0x57ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x5800, 0x5fff) AM_READWRITE(capbowl_tms34061_r, capbowl_tms34061_w)
	AM_RANGE(0x6000, 0x6000) AM_WRITE(capbowl_sndcmd_w)
	AM_RANGE(0x6800, 0x6800) AM_WRITE(track_reset_w)	/* + watchdog */
	AM_RANGE(0x7000, 0x7000) AM_READ(track_0_r)			/* + other inputs */
	AM_RANGE(0x7800, 0x7800) AM_READ(track_1_r)			/* + other inputs */
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( bowlrama_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x001f) AM_READWRITE(bowlrama_blitter_r, bowlrama_blitter_w)
	AM_RANGE(0x4000, 0x4000) AM_WRITE(MWA8_RAM) AM_BASE(&capbowl_rowaddress)
	AM_RANGE(0x5000, 0x57ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x5800, 0x5fff) AM_READWRITE(capbowl_tms34061_r, capbowl_tms34061_w)
	AM_RANGE(0x6000, 0x6000) AM_WRITE(capbowl_sndcmd_w)
	AM_RANGE(0x6800, 0x6800) AM_WRITE(track_reset_w)	/* + watchdog */
	AM_RANGE(0x7000, 0x7000) AM_READ(track_0_r)			/* + other inputs */
	AM_RANGE(0x7800, 0x7800) AM_READ(track_1_r)			/* + other inputs */
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_RAM
	AM_RANGE(0x1000, 0x1000) AM_READWRITE(YM2203_status_port_0_r, YM2203_control_port_0_w)
	AM_RANGE(0x1001, 0x1001) AM_READWRITE(YM2203_read_port_0_r, YM2203_write_port_0_w)
	AM_RANGE(0x2000, 0x2000) AM_WRITENOP  				/* Not hooked up according to the schematics */
	AM_RANGE(0x6000, 0x6000) AM_WRITE(DAC_0_data_w)
	AM_RANGE(0x7000, 0x7000) AM_READ(soundlatch_r)
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( capbowl )
	PORT_START	/* IN0 */
	/* low 4 bits are for the trackball */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Cabinet ) ) /* This version of Bowl-O-Rama */
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )			   /* is Upright only */
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START	/* IN1 */
	/* low 4 bits are for the trackball */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START	/* FAKE */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(20) PORT_KEYDELTA(40) PORT_REVERSE

	PORT_START	/* FAKE */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(20) PORT_KEYDELTA(40)

	PORT_START	/* FAKE */
	/* This fake input port is used to get the status of the F2 key, */
	/* and activate the test mode, which is triggered by a NMI */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_SERVICE ) PORT_NAME( DEF_STR( Service_Mode )) PORT_CODE(KEYCODE_F2)
INPUT_PORTS_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static struct YM2203interface ym2203_interface =
{
	ticket_dispenser_r,
	0,
	0,
	ticket_dispenser_w,  /* Also a status LED. See memory map above */
	firqhandler
};



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( capbowl )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M6809E, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(capbowl_map,0)
	MDRV_CPU_VBLANK_INT(capbowl_interrupt,1)

	MDRV_CPU_ADD(M6809E, MASTER_CLOCK/4)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_map,0)

	MDRV_FRAMES_PER_SECOND(57)
	MDRV_VBLANK_DURATION(5000)

	MDRV_MACHINE_RESET(capbowl)
	MDRV_NVRAM_HANDLER(capbowl)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(360, 256)
	MDRV_VISIBLE_AREA(0, 359, 0, 244)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(capbowl)
	MDRV_VIDEO_UPDATE(capbowl)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, MASTER_CLOCK/2)
	MDRV_SOUND_CONFIG(ym2203_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.07)
	MDRV_SOUND_ROUTE(1, "mono", 0.07)
	MDRV_SOUND_ROUTE(2, "mono", 0.07)
	MDRV_SOUND_ROUTE(3, "mono", 0.75)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( bowlrama )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(capbowl)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(bowlrama_map,0)

	/* video hardware */
	MDRV_VISIBLE_AREA(0, 359, 0, 239)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( capbowl )
	ROM_REGION( 0x28000, REGION_CPU1, 0 )
	ROM_LOAD( "u6",           0x08000, 0x8000, CRC(14924c96) SHA1(d436c5115873c9c2bc7657acff1cf7d99c0c5d6d) )
	ROM_LOAD( "gr0",          0x10000, 0x8000, CRC(ef53ca7a) SHA1(219dc342595bfd23c1336f3e167e40ff0c5e7994) )
	ROM_LOAD( "gr1",          0x18000, 0x8000, CRC(27ede6ce) SHA1(14aa31cbcf089419b5b2ea8d57e82fc51895fc2e) )
	ROM_LOAD( "gr2",          0x20000, 0x8000, CRC(e49238f4) SHA1(ac76f1a761d6b0765437fb7367442667da7bb373) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "sound",        0x8000, 0x8000, CRC(8c9c3b8a) SHA1(f3cdf42ef19012817e6b7966845f9ede39f61b07) )
ROM_END


ROM_START( capbowl2 )
	ROM_REGION( 0x28000, REGION_CPU1, 0 )
	ROM_LOAD( "progrev3.u6",  0x08000, 0x8000, CRC(9162934a) SHA1(7542dd68a2aa55ad4f03b23ae2313ed6a34ae145) )
	ROM_LOAD( "gr0",          0x10000, 0x8000, CRC(ef53ca7a) SHA1(219dc342595bfd23c1336f3e167e40ff0c5e7994) )
	ROM_LOAD( "gr1",          0x18000, 0x8000, CRC(27ede6ce) SHA1(14aa31cbcf089419b5b2ea8d57e82fc51895fc2e) )
	ROM_LOAD( "gr2",          0x20000, 0x8000, CRC(e49238f4) SHA1(ac76f1a761d6b0765437fb7367442667da7bb373) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "sound",        0x8000, 0x8000, CRC(8c9c3b8a) SHA1(f3cdf42ef19012817e6b7966845f9ede39f61b07) )
ROM_END


ROM_START( capbowl3 )
	ROM_REGION( 0x28000, REGION_CPU1, 0 )
	ROM_LOAD( "bowl30.bin",   0x08000, 0x8000, CRC(32e30928) SHA1(db47b6ace949d86aa1cdd1e5c7a5981f30b590af) )
	ROM_LOAD( "bfb.gr0",      0x10000, 0x8000, CRC(2b5eb091) SHA1(43976bfa9fbe9694c7274f113641f671fa32bbb7) )
	ROM_LOAD( "bfb.gr1",      0x18000, 0x8000, CRC(880e4e1c) SHA1(9f88b26877596667f1ac4e0083795bf266712879) )
	ROM_LOAD( "bfb.gr2",      0x20000, 0x8000, CRC(f3d2468d) SHA1(0348ee5d0000b753ad90a525048d05bfb552bee1) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "sound-r2.bin",  0x8000, 0x8000, CRC(43ac1658) SHA1(1fab23d649d0c565ef1a7f45b30806f9d1bb4afd) )
ROM_END


ROM_START( capbowl4 )
	ROM_REGION( 0x28000, REGION_CPU1, 0 )
	ROM_LOAD( "bfb.u6",        0x08000, 0x8000, CRC(79f1d083) SHA1(36e9a90403fc9b876d7660ee46c5fbb855321769) )
	ROM_LOAD( "bfb.gr0",       0x10000, 0x8000, CRC(2b5eb091) SHA1(43976bfa9fbe9694c7274f113641f671fa32bbb7) )
	ROM_LOAD( "bfb.gr1",       0x18000, 0x8000, CRC(880e4e1c) SHA1(9f88b26877596667f1ac4e0083795bf266712879) )
	ROM_LOAD( "bfb.gr2",       0x20000, 0x8000, CRC(f3d2468d) SHA1(0348ee5d0000b753ad90a525048d05bfb552bee1) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "bfb.u30",     0x8000, 0x8000, CRC(6fe2c4ff) SHA1(862823264d243be590fd29a228a32e7a0a818e57) )
ROM_END


ROM_START( clbowl )
	ROM_REGION( 0x28000, REGION_CPU1, 0 )
	ROM_LOAD( "u6.cl",        0x08000, 0x8000, CRC(91e06bc4) SHA1(efa54328417f971cc482a4529d05331a3baffc1a) )
	ROM_LOAD( "gr0.cl",       0x10000, 0x8000, CRC(899c8f15) SHA1(dbb4a9c015b5e64c62140f0c99b87da2793ae5c1) )
	ROM_LOAD( "gr1.cl",       0x18000, 0x8000, CRC(0ac0dc4c) SHA1(61afa3af1f84818b940b5c6f6a8cfb58ca557551) )
	ROM_LOAD( "gr2.cl",       0x20000, 0x8000, CRC(251f5da5) SHA1(063001cfb68e3ec35baa24eed186214e26d55b82) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "sound.cl",     0x8000, 0x8000, CRC(1eba501e) SHA1(684bdc18cf5e01a86d8018a3e228ec34e5dec57d) )
ROM_END


ROM_START( bowlrama )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u6",           0x08000, 0x08000, CRC(7103ad55) SHA1(92dccc5e6df3e18fc8cdcb67ef14d50ce5eb8b2c) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "u30",          0x8000, 0x8000, CRC(f3168834) SHA1(40b7fbe9c15cc4442f4394b71c0666185afe4c8d) )

	ROM_REGION( 0x40000, REGION_GFX1, 0 )
	ROM_LOAD( "ux7",          0x00000, 0x40000, CRC(8727432a) SHA1(a81d366c5f8df0bdb97e795bba7752e6526ddba0) )
ROM_END



/*************************************
 *
 *  Driver init
 *
 *************************************/

static DRIVER_INIT( capbowl )
{
	/* Initialize the ticket dispenser to 100 milliseconds */
	/* (I'm not sure what the correct value really is) */
	ticket_dispenser_init(100, TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_LOW);
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1988, capbowl,  0,       capbowl,  capbowl, capbowl, ROT270, "Incredible Technologies", "Capcom Bowling (set 1)", 0 ,4)
GAME( 1988, capbowl2, capbowl, capbowl,  capbowl, capbowl, ROT270, "Incredible Technologies", "Capcom Bowling (set 2)", 0 ,4)
GAME( 1988, capbowl3, capbowl, capbowl,  capbowl, capbowl, ROT270, "Incredible Technologies", "Capcom Bowling (set 3)", 0 ,4)
GAME( 1988, capbowl4, capbowl, capbowl,  capbowl, capbowl, ROT270, "Incredible Technologies", "Capcom Bowling (set 4)", 0 ,4)
GAME( 1989, clbowl,   capbowl, capbowl,  capbowl, capbowl, ROT270, "Incredible Technologies", "Coors Light Bowling", 0 ,4)
GAME( 1991, bowlrama, 0,       bowlrama, capbowl, capbowl, ROT270, "P&P Marketing", "Bowl-O-Rama", 0 ,0)
