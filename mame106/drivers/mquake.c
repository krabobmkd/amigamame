/**************************************************************************************

    Sente Super System

    Preliminary driver by Mariusz Wojcieszek

**************************************************************************************/


#include "driver.h"
#include "includes/amiga.h"
#include "sound/es5503.h"



/*************************************
 *
 *  CIA-A port A access:
 *
 *  PA7 = game port 1, pin 6 (fire)
 *  PA6 = game port 0, pin 6 (fire)
 *  PA5 = /RDY (disk ready)
 *  PA4 = /TK0 (disk track 00)
 *  PA3 = /WPRO (disk write protect)
 *  PA2 = /CHNG (disk change)
 *  PA1 = /LED (LED, 0=bright / audio filter control)
 *  PA0 = OVL (ROM/RAM overlay bit)
 *
 *************************************/

static UINT8 mquake_cia_0_porta_r(void)
{
	return readinputportbytag("CIA0PORTA");
}

static void mquake_cia_0_porta_w(UINT8 data)
{
	/* switch banks as appropriate */
	memory_set_bank(1, data & 1);

	/* swap the write handlers between ROM and bank 1 based on the bit */
	if ((data & 1) == 0)
		/* overlay disabled, map RAM on 0x000000 */
		memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x000000, 0x07ffff, 0, 0, MWA16_BANK1);

	else
		/* overlay enabled, map Amiga system ROM on 0x000000 */
		memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x000000, 0x07ffff, 0, 0, MWA16_ROM);
}



/*************************************
 *
 *  CIA-A port B access:
 *
 *  PB7 = parallel data 7
 *  PB6 = parallel data 6
 *  PB5 = parallel data 5
 *  PB4 = parallel data 4
 *  PB3 = parallel data 3
 *  PB2 = parallel data 2
 *  PB1 = parallel data 1
 *  PB0 = parallel data 0
 *
 *************************************/

static UINT8 mquake_cia_0_portb_r(void)
{
	/* parallel port */
	logerror("%06x:CIA0_portb_r\n", activecpu_get_pc());
	return 0xff;
}

static void mquake_cia_0_portb_w(UINT8 data)
{
	/* parallel port */
	logerror("%06x:CIA0_portb_w(%02x)\n", activecpu_get_pc(), data);
}



/*************************************
 *
 *  ES5503 access
 *
 *************************************/

static READ16_HANDLER( es5503_word_lsb_r )
{
	return (ACCESSING_LSB) ? (ES5503_reg_0_r(offset) | 0xff00) : 0xffff;
}

static WRITE16_HANDLER( es5503_word_lsb_w )
{
	if (ACCESSING_LSB)
		ES5503_reg_0_w(offset, data);
}


static WRITE16_HANDLER( output_w )
{
	if (ACCESSING_LSB)
		logerror("%06x:output_w(%x) = %02x\n", activecpu_get_pc(), offset, data);
}


static READ16_HANDLER( coin_chip_r )
{
	if (offset == 1)
		return readinputportbytag("COINCHIP");
	logerror("%06x:coin_chip_r(%02x) & %04x\n", activecpu_get_pc(), offset, mem_mask ^ 0xffff);
	return 0xffff;
}

static WRITE16_HANDLER( coin_chip_w )
{
	logerror("%06x:coin_chip_w(%02x) = %04x & %04x\n", activecpu_get_pc(), offset, data, mem_mask ^ 0xffff);
}

// inputs at 282000, 282002 (full word)
// outputs at 284000, 284002, 284004, 284006, 284008, 28400a, 28400c, 28400e (0=off, FF=on in LSB)
// coin chip I/O: read from 286002 (LSB), write to 28600A (low 4 bits)
//     write to 286008 (LSB) = F = reset?
// NVRAM at 200000-203FFF (both MSB and LSB)



/*************************************
 *
 *  Memory map
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_FLAGS( AMEF_UNMAP(1) )
	AM_RANGE(0x000000, 0x07ffff) AM_RAMBANK(1) AM_BASE(&amiga_chip_ram)	AM_SIZE(&amiga_chip_ram_size)
	AM_RANGE(0xbfd000, 0xbfefff) AM_READWRITE(amiga_cia_r, amiga_cia_w)
	AM_RANGE(0xc00000, 0xdfffff) AM_READWRITE(amiga_custom_r, amiga_custom_w) AM_BASE(&amiga_custom_regs)
	AM_RANGE(0xe80000, 0xe8ffff) AM_READWRITE(amiga_autoconfig_r, amiga_autoconfig_w)
	AM_RANGE(0xfc0000, 0xffffff) AM_ROM AM_REGION(REGION_USER1, 0)			/* System ROM */

	AM_RANGE(0x200000, 0x203fff) AM_RAM AM_BASE(&generic_nvram16) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x204000, 0x2041ff) AM_READWRITE(es5503_word_lsb_r, es5503_word_lsb_w)
	AM_RANGE(0x282000, 0x282001) AM_READ(port_tag_to_handler16("SW.LO"))
	AM_RANGE(0x282002, 0x282003) AM_READ(port_tag_to_handler16("SW.HI"))
	AM_RANGE(0x284000, 0x28400f) AM_WRITE(output_w)
	AM_RANGE(0x286000, 0x28600f) AM_READWRITE(coin_chip_r, coin_chip_w)
	AM_RANGE(0x300000, 0x3bffff) AM_ROM AM_REGION(REGION_USER2, 0)
	AM_RANGE(0xf00000, 0xfbffff) AM_ROM AM_REGION(REGION_USER2, 0)			/* Custom ROM */
ADDRESS_MAP_END



/*************************************
 *
 *  Input ports
 *
 *************************************/

INPUT_PORTS_START( mquake )
	PORT_START_TAG("CIA0PORTA")
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)			/* JS0SW */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)			/* JS1SW */

	PORT_START_TAG("JOY0DAT")
	PORT_BIT( 0x0303, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(amiga_joystick_convert, "P1JOY")
	PORT_BIT( 0xfcfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("JOY1DAT")
	PORT_BIT( 0x0303, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(amiga_joystick_convert, "P2JOY")
	PORT_BIT( 0xfcfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("P1JOY")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)

	PORT_START_TAG("P2JOY")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)

	PORT_START_TAG("COINCHIP")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START4 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("SW.LO")
	PORT_DIPNAME( 0x0001, 0x0001, "SW3.1" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, "SW3.2" )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, "SW3.3" )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, "SW3.4" )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, "SW2.1" )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "SW2.2" )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, "SW2.3" )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, "SW2.4" )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, "SW1.1" )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, "SW1.2" )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, "SW1.3" )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, "SW1.4" )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, "I/O SW00" )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, "I/O SW01" )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, "I/O SW02" )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Service_Mode ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START_TAG("SW.HI")
	PORT_DIPNAME( 0x0001, 0x0001, "SW3.5" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, "SW3.6" )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, "SW3.7" )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, "SW3.8" )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, "SW2.5" )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "SW2.6" )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, "SW2.7" )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, "SW2.8" )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, "SW1.5" )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, "SW1.6" )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, "SW1.7" )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, "SW1.8" )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, "I/O SW20" )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, "I/O SW21" )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, "I/O SW22" )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, "I/O SW23" )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static struct CustomSound_interface amiga_custom_interface =
{
	amiga_sh_start
};


static struct ES5503interface es5503_intf =
{
	NULL,
	NULL
};



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( mquake )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 7159090)        /* 7.15909 Mhz (NTSC) */
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_VBLANK_INT(amiga_scanline_callback, 262)

	MDRV_FRAMES_PER_SECOND(59.997)
	MDRV_VBLANK_DURATION(0)

	MDRV_MACHINE_RESET(amiga)
	MDRV_NVRAM_HANDLER(generic_0fill)

    /* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_UPDATE_BEFORE_VBLANK)
	MDRV_SCREEN_SIZE(512*2, 262)
	MDRV_VISIBLE_AREA((129-8)*2, (449+8-1)*2, 44-8, 244+8-1)
	MDRV_PALETTE_LENGTH(4096)
	MDRV_PALETTE_INIT(amiga)

	MDRV_VIDEO_START(amiga)
	MDRV_VIDEO_UPDATE(generic_bitmapped)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(CUSTOM, 3579545)
	MDRV_SOUND_CONFIG(amiga_custom_interface)
	MDRV_SOUND_ROUTE(0, "left", 0.50)
	MDRV_SOUND_ROUTE(1, "right", 0.50)
	MDRV_SOUND_ROUTE(2, "right", 0.50)
	MDRV_SOUND_ROUTE(3, "left", 0.50)

	MDRV_SOUND_ADD(ES5503, 7159090)
	MDRV_SOUND_CONFIG(es5503_intf)
	MDRV_SOUND_ROUTE(0, "left", 0.50)
	MDRV_SOUND_ROUTE(1, "right", 0.50)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( mquake )
	ROM_REGION(0x80000, REGION_USER1, 0)
	ROM_LOAD16_WORD_SWAP( "kick12.rom", 0x000000, 0x40000, CRC(a6ce1636) SHA1(11f9e62cf299f72184835b7b2a70a16333fc0d88) )
	ROM_COPY( REGION_USER1, 0x000000, 0x040000, 0x040000 )

	ROM_REGION(0xc0000, REGION_USER2, 0)
	ROM_LOAD16_BYTE( "rom0l.bin",    0x00000, 0x10000, CRC(60c35ec3) SHA1(84fe88af54903cbd46044ef52bb50e8f94a94dcd) )
	ROM_LOAD16_BYTE( "rom0h.bin",    0x00001, 0x10000, CRC(11551a68) SHA1(bc17e748cc7a4a547de230431ea08f0355c0eec8) )
	ROM_LOAD16_BYTE( "rom1l.bin",    0x20000, 0x10000, CRC(0128c423) SHA1(b0465069452bd11b67c9a2f2b9021c91788bedbb) )
	ROM_LOAD16_BYTE( "rom1h.bin",    0x20001, 0x10000, CRC(95119e65) SHA1(29f3c32ca110c9687f38fd03ccb979c1e7c7a87e) )
	ROM_LOAD16_BYTE( "rom2l.bin",    0x40000, 0x10000, CRC(f8b8624a) SHA1(cb769581f78882a950be418dd4b35bbb6fd78a34) )
	ROM_LOAD16_BYTE( "rom2h.bin",    0x40001, 0x10000, CRC(46e36e0d) SHA1(0813430137a31d5af2cadbd712a418e9ff339a21) )
	ROM_LOAD16_BYTE( "rom3l.bin",    0x60000, 0x10000, CRC(c00411a2) SHA1(960d3539914f587c2186ec6eefb81b3cdd9325a0) )
	ROM_LOAD16_BYTE( "rom3h.bin",    0x60001, 0x10000, CRC(4540c681) SHA1(cb0bc6dc506ed0c9561687964e57299a472c5cd8) )
	ROM_LOAD16_BYTE( "rom4l.bin",    0x80000, 0x10000, CRC(f48d0730) SHA1(703a8ed47f64b3824bc6e5a4c5bdb2895f8c3d37) )
	ROM_LOAD16_BYTE( "rom4h.bin",    0x80001, 0x10000, CRC(eee39fec) SHA1(713e24fa5f4ba0a8bc7bf67ed2d9e079fd3aa5d6) )
	ROM_LOAD16_BYTE( "rom5l.bin",    0xa0000, 0x10000, CRC(7b6ec532) SHA1(e19005269673134431eb55053d650f747f614b89) )
	ROM_LOAD16_BYTE( "rom5h.bin",    0xa0001, 0x10000, CRC(ed8ec9b7) SHA1(510416bc88382e7a548635dcba53a2b615272e0f) )

	ROM_REGION(0x040000, REGION_SOUND1, 0)
	ROM_LOAD( "qrom0.bin",    0x000000, 0x010000, CRC(753e29b4) SHA1(4c7ccff02d310c7c669aa170e8efb6f2cb996432) )
	ROM_LOAD( "qrom1.bin",    0x010000, 0x010000, CRC(e9e15629) SHA1(a0aa60357a13703f69a2a13e83f2187c9a1f63c1) )
	ROM_LOAD( "qrom2.bin",    0x020000, 0x010000, CRC(837294f7) SHA1(99e383998105a63896096629a51b3a0e9eb16b17) )
	ROM_LOAD( "qrom3.bin",    0x030000, 0x010000, CRC(530fd1a9) SHA1(e3e5969f0880de0a6cdb443a82b85d34ab8ff4f8) )
ROM_END



/*************************************
 *
 *  Driver init
 *
 *************************************/

DRIVER_INIT(mquake)
{
	static const amiga_machine_interface mquake_intf =
	{
		ANGUS_CHIP_RAM_MASK,
		mquake_cia_0_porta_r, mquake_cia_0_portb_r,
		mquake_cia_0_porta_w, mquake_cia_0_portb_w
	};
	amiga_machine_config(&mquake_intf);

	/* set up memory */
	memory_configure_bank(1, 0, 1, amiga_chip_ram, 0);
	memory_configure_bank(1, 1, 1, memory_region(REGION_USER1), 0);
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1987, mquake, 0, mquake, mquake, mquake, 0, "Sente", "Moonquake", GAME_NOT_WORKING | GAME_NO_SOUND | GAME_IMPERFECT_GRAPHICS ,0)
