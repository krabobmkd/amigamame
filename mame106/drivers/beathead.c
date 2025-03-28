/***************************************************************************

    Atari "Stella on Steroids" hardware

    driver by Aaron Giles

    Games supported:
        * BeatHead

    Known bugs:
        * none known

****************************************************************************

    Memory map

    ===================================================================================================
    MAIN CPU
    ===================================================================================================
    00000000-0001FFFFF  R/W   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx   Main RAM
    01800000-01BFFFFFF  R     xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx   Main ROM
    40000000-4000007FF  R/W   -------- -------- -------- xxxxxxxx   EEPROM
    41000000            R     -------- -------- -------- xxxxxxxx   Data from sound board
    41000000              W   -------- -------- -------- xxxxxxxx   Data to sound board
    41000100            R     -------- -------- -------- -----xxx   Interrupt enables
                              -------- -------- -------- -----x--      (scanline int enable)
                              -------- -------- -------- ------x-      (unknown int enable)
                              -------- -------- -------- -------x      (unknown int enable)
    41000100              W   -------- -------- -------- --------   Interrupt acknowledge
    41000104              W   -------- -------- -------- --------   Unknown int disable
    41000108              W   -------- -------- -------- --------   Unknown int disable
    4100010c              W   -------- -------- -------- --------   Scanline int disable
    41000114              W   -------- -------- -------- --------   Unknown int enable
    41000118              W   -------- -------- -------- --------   Unknown int enable
    4100011c              W   -------- -------- -------- --------   Scanline int enable
    41000200            R     -------- -------- xxxx--xx xxxx--xx   Player 2/3 inputs
                        R     -------- -------- xxxx---- --------      (player 3 joystick UDLR)
                        R     -------- -------- ------x- --------      (player 3 button 1)
                        R     -------- -------- -------x --------      (player 3 button 2)
                        R     -------- -------- -------- xxxx----      (player 2 joystick UDLR)
                        R     -------- -------- -------- ------x-      (player 2 button 1)
                        R     -------- -------- -------- -------x      (player 2 button 2)
    41000204            R     -------- -------- xxxx--xx xxxx--xx   Player 1/4 inputs
                        R     -------- -------- xxxx---- --------      (player 1 joystick UDLR)
                        R     -------- -------- ------x- --------      (player 1 button 1)
                        R     -------- -------- -------x --------      (player 1 button 2)
                        R     -------- -------- -------- xxxx----      (player 4 joystick UDLR)
                        R     -------- -------- -------- ------x-      (player 4 button 1)
                        R     -------- -------- -------- -------x      (player 4 button 2)
    41000208              W   -------- -------- -------- --------   Sound /RESET assert
    4100020C              W   -------- -------- -------- --------   Sound /RESET deassert
    41000220              W   -------- -------- -------- --------   Coin counter assert
    41000224              W   -------- -------- -------- --------   Coin counter deassert
    41000300            R     -------- -------- xxxxxxxx -xxx----   DIP switches/additional inputs
                        R     -------- -------- xxxxxxxx --------      (debug DIP switches)
                        R     -------- -------- -------- -x------      (service switch)
                        R     -------- -------- -------- --x-----      (sound output buffer full)
                        R     -------- -------- -------- ---x----      (sound input buffer full)
    41000304            R     -------- -------- -------- xxxxxxxx   Coin/service inputs
                        R     -------- -------- -------- xxxx----      (service inputs: R,RC,LC,L)
                        R     -------- -------- -------- ----xxxx      (coin inputs: R,RC,LC,L)
    41000400              W   -------- -------- -------- -xxxxxxx   Palette select
    41000500              W   -------- -------- -------- --------   EEPROM write enable
    41000600              W   -------- -------- -------- ----xxxx   Finescroll, vertical SYNC flags
                          W   -------- -------- -------- ----x---      (VBLANK)
                          W   -------- -------- -------- -----x--      (VSYNC)
                          W   -------- -------- -------- ------xx      (fine scroll value)
    41000700              W   -------- -------- -------- --------   Watchdog reset
    42000000-4201FFFF   R/W   -------- -------- xxxxxxxx xxxxxxxx   Palette RAM
                        R/W   -------- -------- x------- --------      (LSB of all three components)
                        R/W   -------- -------- -xxxxx-- --------      (red component)
                        R/W   -------- -------- ------xx xxx-----      (green component)
                        R/W   -------- -------- -------- ---xxxxx      (blue component)
    43000000              W   -------- -------- ----xxxx xxxxxxxx   HSYNC RAM address latch
                          W   -------- -------- ----x--- --------      (counter enable)
                          W   -------- -------- -----xxx xxxxxxxx      (RAM address)
    43000004            R/W   -------- -------- -------- xxxxx---   HSYNC RAM data latch
                        R/W   -------- -------- -------- x-------      (generate IRQ)
                        R/W   -------- -------- -------- -x------      (VRAM shift enable)
                        R/W   -------- -------- -------- --x-----      (HBLANK)
                        R/W   -------- -------- -------- ---x----      (/HSYNC)
                        R/W   -------- -------- -------- ----x---      (release wait for sync)
    43000008              W   -------- -------- -------- ---x-xx-   HSYNC unknown control
    8DF80000            R     -------- -------- -------- --------   Unknown
    8F380000-8F3FFFFF     W   -------- -------- -------- --------   VRAM latch address
    8F900000-8F97FFFF     W   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx   VRAM transparent write
    8F980000-8F9FFFFF   R/W   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx   VRAM standard read/write
    8FB80000-8FBFFFFF     W   ----xxxx ----xxxx ----xxxx ----xxxx   VRAM "bulk" write
                          W   ----xxxx -------- -------- --------      (enable byte lanes for word 3?)
                          W   -------- ----xxxx -------- --------      (enable byte lanes for word 2?)
                          W   -------- -------- ----xxxx --------      (enable byte lanes for word 1?)
                          W   -------- -------- -------- ----xxxx      (enable byte lanes for word 0?)
    8FFF8000              W   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx   VRAM "bulk" data latch
    9E280000-9E2FFFFF     W   -------- -------- -------- --------   VRAM copy destination address latch
    ===================================================================================================

***************************************************************************/


#include "driver.h"
#include "cpu/asap/asap.h"
#include "machine/atarigen.h"
#include "sndhrdw/atarijsa.h"
#include "beathead.h"



/*************************************
 *
 *  Statics
 *
 *************************************/

static UINT32 *	ram_base;
static UINT32 *	rom_base;

static double		hblank_offset;

static UINT8		irq_line_state;
static UINT8		irq_enable[3];
static UINT8		irq_state[3];

static UINT8		eeprom_enabled;


#define MAX_SCANLINES	262



/*************************************
 *
 *  Machine init
 *
 *************************************/

static void update_interrupts(void);

static void scanline_callback(int scanline)
{
	/* update the video */
	beathead_scanline_update(scanline);

	/* on scanline zero, clear any halt condition */
	if (scanline == 0)
		cpunum_set_input_line(0, INPUT_LINE_HALT, CLEAR_LINE);

	/* wrap around at 262 */
	scanline++;
	if (scanline >= MAX_SCANLINES)
		scanline = 0;

	/* set the scanline IRQ */
	irq_state[2] = 1;
	update_interrupts();

	/* set the timer for the next one */
	timer_set(cpu_getscanlinetime(scanline) - hblank_offset, scanline, scanline_callback);
}


static MACHINE_RESET( beathead )
{
	/* reset the common subsystems */
	atarigen_eeprom_reset();
	atarigen_interrupt_reset(update_interrupts);
	atarijsa_reset();

	/* the code is temporarily mapped at 0 at startup */
	/* just copying the first 0x40 bytes is sufficient */
	memcpy(ram_base, rom_base, 0x40);

	/* compute the timing of the HBLANK interrupt and set the first timer */
	hblank_offset = cpu_getscanlineperiod() * ((455. - 336. - 25.) / 455.);
	timer_set(cpu_getscanlinetime(0) - hblank_offset, 0, scanline_callback);

	/* reset IRQs */
	irq_line_state = CLEAR_LINE;
	irq_state[0] = irq_state[1] = irq_state[2] = 0;
	irq_enable[0] = irq_enable[1] = irq_enable[2] = 0;
}



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

static void update_interrupts(void)
{
	int gen_int;

	/* compute the combined interrupt signal */
	gen_int  = irq_state[0] & irq_enable[0];
	gen_int |= irq_state[1] & irq_enable[1];
	gen_int |= irq_state[2] & irq_enable[2];
	gen_int  = gen_int ? ASSERT_LINE : CLEAR_LINE;

	/* if it's changed since the last time, call through */
	if (irq_line_state != gen_int)
	{
		irq_line_state = gen_int;
//      if (irq_line_state != CLEAR_LINE)
			cpunum_set_input_line(0, ASAP_IRQ0, irq_line_state);
//      else
//          asap_set_irq_line(ASAP_IRQ0, irq_line_state);
	}
}


static WRITE32_HANDLER( interrupt_control_w )
{
	int irq = offset & 3;
	int control = (offset >> 2) & 1;

	/* offsets 1-3 seem to be the enable latches for the IRQs */
	if (irq != 0)
		irq_enable[irq - 1] = control;

	/* offset 0 seems to be the interrupt ack */
	else
		irq_state[0] = irq_state[1] = irq_state[2] = 0;

	/* update the current state */
	update_interrupts();
}


static READ32_HANDLER( interrupt_control_r )
{
	/* return the enables as a bitfield */
	return (irq_enable[0]) | (irq_enable[1] << 1) | (irq_enable[2] << 2);
}



/*************************************
 *
 *  EEPROM handling
 *
 *************************************/

static WRITE32_HANDLER( eeprom_data_w )
{
	if (eeprom_enabled)
	{
		mem_mask |= 0xffffff00;
		COMBINE_DATA(generic_nvram32 + offset);
		eeprom_enabled = 0;
	}
}


static WRITE32_HANDLER( eeprom_enable_w )
{
	eeprom_enabled = 1;
}



/*************************************
 *
 *  Input handling
 *
 *************************************/

static READ32_HANDLER( input_0_r )
{
	return readinputport(0);
}


static READ32_HANDLER( input_1_r )
{
	return readinputport(1);
}


static READ32_HANDLER( input_2_r )
{
	int result = readinputport(2);
	if (atarigen_sound_to_cpu_ready) result ^= 0x10;
	if (atarigen_cpu_to_sound_ready) result ^= 0x20;
	return result;
}


static READ32_HANDLER( input_3_r )
{
	return readinputport(3);
}



/*************************************
 *
 *  Sound communication
 *
 *************************************/

static READ32_HANDLER( sound_data_r )
{
	return atarigen_sound_r(offset,0);
}


static WRITE32_HANDLER( sound_data_w )
{
	if (ACCESSING_LSB32)
		atarigen_sound_w(offset, data, mem_mask);
}


static WRITE32_HANDLER( sound_reset_w )
{
	logerror("Sound reset = %d\n", !offset);
	cpunum_set_input_line(1, INPUT_LINE_RESET, offset ? CLEAR_LINE : ASSERT_LINE);
}



/*************************************
 *
 *  Misc other I/O
 *
 *************************************/

static WRITE32_HANDLER( coin_count_w )
{
	coin_counter_w(0, !offset);
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x00000000, 0x0001ffff) AM_RAM AM_BASE(&ram_base)
	AM_RANGE(0x01800000, 0x01bfffff) AM_ROM AM_REGION(REGION_USER1, 0) AM_BASE(&rom_base)
	AM_RANGE(0x40000000, 0x400007ff) AM_READWRITE(MRA32_RAM, eeprom_data_w) AM_BASE(&generic_nvram32) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x41000000, 0x41000003) AM_READWRITE(sound_data_r, sound_data_w)
	AM_RANGE(0x41000100, 0x41000103) AM_READ(interrupt_control_r)
	AM_RANGE(0x41000100, 0x4100011f) AM_WRITE(interrupt_control_w)
	AM_RANGE(0x41000200, 0x41000203) AM_READ(input_0_r)
	AM_RANGE(0x41000204, 0x41000207) AM_READ(input_1_r)
	AM_RANGE(0x41000208, 0x4100020f) AM_WRITE(sound_reset_w)
	AM_RANGE(0x41000220, 0x41000227) AM_WRITE(coin_count_w)
	AM_RANGE(0x41000300, 0x41000303) AM_READ(input_2_r)
	AM_RANGE(0x41000304, 0x41000307) AM_READ(input_3_r)
	AM_RANGE(0x41000400, 0x41000403) AM_WRITE(MWA32_RAM) AM_BASE(&beathead_palette_select)
	AM_RANGE(0x41000500, 0x41000503) AM_WRITE(eeprom_enable_w)
	AM_RANGE(0x41000600, 0x41000603) AM_WRITE(beathead_finescroll_w)
	AM_RANGE(0x41000700, 0x41000703) AM_WRITE(watchdog_reset32_w)
	AM_RANGE(0x42000000, 0x4201ffff) AM_READWRITE(MRA32_RAM, beathead_palette_w) AM_BASE(&paletteram32)
	AM_RANGE(0x43000000, 0x43000007) AM_READWRITE(beathead_hsync_ram_r, beathead_hsync_ram_w)
	AM_RANGE(0x8df80000, 0x8df80003) AM_READ(MRA32_NOP)	/* noisy x4 during scanline int */
	AM_RANGE(0x8f380000, 0x8f3fffff) AM_WRITE(beathead_vram_latch_w)
	AM_RANGE(0x8f900000, 0x8f97ffff) AM_WRITE(beathead_vram_transparent_w)
	AM_RANGE(0x8f980000, 0x8f9fffff) AM_RAM AM_BASE(&videoram32)
	AM_RANGE(0x8fb80000, 0x8fbfffff) AM_WRITE(beathead_vram_bulk_w)
	AM_RANGE(0x8fff8000, 0x8fff8003) AM_WRITE(MWA32_RAM) AM_BASE(&beathead_vram_bulk_latch)
	AM_RANGE(0x9e280000, 0x9e2fffff) AM_WRITE(beathead_vram_copy_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( beathead )
	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)

	PORT_START
	PORT_BIT( 0x000f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_SPECIAL )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_SPECIAL )
	PORT_SERVICE( 0x0040, IP_ACTIVE_LOW )
	PORT_BIT( 0xff80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0006, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0xfff0, IP_ACTIVE_LOW, IPT_UNUSED )

	JSA_III_PORT	/* audio board port */
INPUT_PORTS_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( beathead )

	/* basic machine hardware */
	MDRV_CPU_ADD(ASAP, ATARI_CLOCK_14MHz)
	MDRV_CPU_PROGRAM_MAP(main_map,0)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION((int)(((262. - 240.) / 262.) * 1000000. / 60.))

	MDRV_MACHINE_RESET(beathead)
	MDRV_NVRAM_HANDLER(generic_1fill)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_NEEDS_6BITS_PER_GUN | VIDEO_UPDATE_BEFORE_VBLANK)
	MDRV_SCREEN_SIZE(42*8, 30*8)
	MDRV_VISIBLE_AREA(0*8, 42*8-1, 0*8, 30*8-1)
	MDRV_PALETTE_LENGTH(32768)

	MDRV_VIDEO_START(beathead)
	MDRV_VIDEO_UPDATE(beathead)

	/* sound hardware */
	MDRV_IMPORT_FROM(jsa_iii_mono)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( beathead )
	ROM_REGION( 0x14000, REGION_CPU2, 0 )			/* 64k + 16k for 6502 code */
	ROM_LOAD( "bhsnd.bin",  0x10000, 0x4000, CRC(dfd33f02) SHA1(479a4838c89691d5a4654a4cd84b6433a9e86109) )
	ROM_CONTINUE(           0x04000, 0xc000 )

	ROM_REGION32_LE( 0x400000, REGION_USER1, 0 )	/* 4MB for ASAP code */
	ROM_LOAD32_BYTE( "bhprog0.bin", 0x000000, 0x80000, CRC(87975721) SHA1(862cb3a290c829aedea26ee7100c50a12e9517e7) )
	ROM_LOAD32_BYTE( "bhprog1.bin", 0x000001, 0x80000, CRC(25d89743) SHA1(9ff9a41355aa6914efc4a44909026e648a3c40f3) )
	ROM_LOAD32_BYTE( "bhprog2.bin", 0x000002, 0x80000, CRC(87722609) SHA1(dbd766fa57f4528702a98db28ae48fb5d2a7f7df) )
	ROM_LOAD32_BYTE( "bhprog3.bin", 0x000003, 0x80000, CRC(a795d616) SHA1(d3b201be62486f3b12e1b20c4694eeff0b4e3fca) )
	ROM_LOAD32_BYTE( "bhpics0.bin", 0x200000, 0x80000, CRC(926bf65d) SHA1(49f25a2844ca1cd940d17fc56c0d2698e95e0e1d) )
	ROM_LOAD32_BYTE( "bhpics1.bin", 0x200001, 0x80000, CRC(a8f12e41) SHA1(693cb7a2510f34af5442870a6ae4d19445d991f9) )
	ROM_LOAD32_BYTE( "bhpics2.bin", 0x200002, 0x80000, CRC(00b96481) SHA1(39daa46321c1d4f8bce8c25d0450b97f1f19dedb) )
	ROM_LOAD32_BYTE( "bhpics3.bin", 0x200003, 0x80000, CRC(99c4f1db) SHA1(aba4440c5cdf413f970a0c65457e2d1b37caf2d6) )

	ROM_REGION( 0x100000, REGION_SOUND1, 0 )		/* 1MB for ADPCM */
	ROM_LOAD( "bhpcm0.bin",  0x80000, 0x20000, CRC(609ca626) SHA1(9bfc913fc4c3453b132595f8553245376bce3a51) )
	ROM_LOAD( "bhpcm1.bin",  0xa0000, 0x20000, CRC(35511509) SHA1(41294b81e253db5d2f30f8589dd59729a31bb2bb) )
	ROM_LOAD( "bhpcm2.bin",  0xc0000, 0x20000, CRC(f71a840a) SHA1(09d045552704cd1434307f9a36ce03c5c06a8ff6) )
	ROM_LOAD( "bhpcm3.bin",  0xe0000, 0x20000, CRC(fedd4936) SHA1(430ed894fa4bfcd56ee5a8a8ef5e161246530e2d) )
ROM_END



/*************************************
 *
 *  Driver speedups
 *
 *************************************/

/*
    In-game hotspot @ 0180F8D8
*/


static UINT32 *speedup_data;
static READ32_HANDLER( speedup_r )
{
	int result = *speedup_data;
	if ((activecpu_get_previouspc() & 0xfffff) == 0x006f0 && result == activecpu_get_reg(ASAP_R3))
		cpu_spinuntil_int();
	return result;
}


static UINT32 *movie_speedup_data;
static READ32_HANDLER( movie_speedup_r )
{
	int result = *movie_speedup_data;
	if ((activecpu_get_previouspc() & 0xfffff) == 0x00a88 && (activecpu_get_reg(ASAP_R28) & 0xfffff) == 0x397c0 &&
		movie_speedup_data[4] == activecpu_get_reg(ASAP_R1))
	{
		UINT32 temp = (INT16)result + movie_speedup_data[4] * 262;
		if (temp - (UINT32)activecpu_get_reg(ASAP_R15) < (UINT32)activecpu_get_reg(ASAP_R23))
			cpu_spinuntil_int();
	}
	return result;
}



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( beathead )
{
	/* initialize the common systems */
	atarigen_eeprom_default = NULL;
	atarijsa_init(1, 4, 2, 0x0040);
	atarijsa3_init_adpcm(REGION_SOUND1);

	/* prepare the speedups */
	speedup_data = memory_install_read32_handler(0, ADDRESS_SPACE_PROGRAM, 0x00000ae8, 0x00000aeb, 0, 0, speedup_r);
	movie_speedup_data = memory_install_read32_handler(0, ADDRESS_SPACE_PROGRAM, 0x00000804, 0x00000807, 0, 0, movie_speedup_r);
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1993, beathead, 0, beathead, beathead, beathead, ROT0, "Atari Games", "BeatHead (prototype)", 0 ,2)
