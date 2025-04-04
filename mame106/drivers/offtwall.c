/***************************************************************************

    Atari "Round" hardware

    driver by Aaron Giles

    Games supported:
        * Off the Wall (1991) [2 sets]

    Known bugs:
        * none at this time

****************************************************************************

    Memory map (TBA)

***************************************************************************/


#include "driver.h"
#include "machine/atarigen.h"
#include "sndhrdw/atarijsa.h"
#include "offtwall.h"



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

static void update_interrupts(void)
{
	int newstate = 0;

	if (atarigen_scanline_int_state)
		newstate = 4;
	if (atarigen_sound_int_state)
		newstate = 6;

	if (newstate)
		cpunum_set_input_line(0, newstate, ASSERT_LINE);
	else
		cpunum_set_input_line(0, 7, CLEAR_LINE);
}



/*************************************
 *
 *  Initialization
 *
 *************************************/

static MACHINE_RESET( offtwall )
{
	atarigen_eeprom_reset();
	atarivc_reset(atarivc_eof_data, 1);
	atarigen_interrupt_reset(update_interrupts);
	atarijsa_reset();
}



/*************************************
 *
 *  I/O handling
 *
 *************************************/

static READ16_HANDLER( special_port3_r )
{
	int result = readinputport(3);
	if (atarigen_cpu_to_sound_ready) result ^= 0x0020;
	return result;
}


static WRITE16_HANDLER( io_latch_w )
{
	/* lower byte */
	if (ACCESSING_LSB)
	{
		/* bit 4 resets the sound CPU */
		cpunum_set_input_line(1, INPUT_LINE_RESET, (data & 0x10) ? CLEAR_LINE : ASSERT_LINE);
		if (!(data & 0x10)) atarijsa_reset();
	}

	logerror("sound control = %04X\n", data);
}



/*************************************
 *
 *  SLOOP workarounds
 *
 *************************************/


/*-------------------------------------------------------------------------

    Bankswitching

    Like the slapstic, the SoS bankswitches memory using A13 and A14.
    Unlike the slapstic, the exact addresses to trigger the bankswitch
    are unknown.

    Fortunately, Off the Wall uses a common routine for the important
    bankswitching. The playfield data is stored in the banked area of
    ROM, and by comparing the playfields to a real system, a mechanism
    to bankswitch at the appropriate time was discovered. Fortunately,
    it's really basic.

    OtW looks up the address to read playfield data from a table which
    is 58 words long. Word 0 assumes the bank is 0, word 1 assumes the
    bank is 1, etc. So we just trigger off of the table read and cause
    the bank to switch then.

    In addition, there is code which checksums longs from $40000 down to
    $3e000. The code wants that checksum to be $aaaa5555, but there is
    no obvious way for this to happen. To work around this, we watch for
    the final read from $3e000 and tweak the value such that the checksum
    will come out the $aaaa5555 magically.

-------------------------------------------------------------------------*/

static UINT16 *bankswitch_base;
static UINT16 *bankrom_base;
static UINT32 bank_offset;


static READ16_HANDLER( bankswitch_r )
{
	/* this is the table lookup; the bank is determined by the address that was requested */
	bank_offset = (offset & 3) * 0x1000;
	logerror("Bankswitch index %d -> %04X\n", offset, bank_offset);

	return bankswitch_base[offset];
}


static READ16_HANDLER( bankrom_r )
{
	/* this is the banked ROM read */
	logerror("%06X: %04X\n", activecpu_get_previouspc(), offset);

	/* if the values are $3e000 or $3e002 are being read by code just below the
        ROM bank area, we need to return the correct value to give the proper checksum */
	if ((offset == 0x3000 || offset == 0x3001) && activecpu_get_previouspc() > 0x37000)
	{
		unsigned int checksum = (program_read_word(0x3fd210) << 16) | program_read_word(0x3fd212);
		unsigned int us = 0xaaaa5555 - checksum;
		if (offset == 0x3001)
			return us & 0xffff;
		else
			return us >> 16;
	}

	return bankrom_base[(bank_offset + offset) & 0x3fff];
}



/*-------------------------------------------------------------------------

    Sprite Cache

    Somewhere in the code, if all the hardware tests are met properly,
    some additional dummy sprites are added to the sprite cache before
    they are copied to sprite RAM. The sprite RAM copy routine computes
    the total width of all sprites as they are copied and if the total
    width is less than or equal to 38, it adds a "HARDWARE ERROR" sprite
    to the end.

    Here we detect the read of the sprite count from within the copy
    routine, and add some dummy sprites to the cache ourself if there
    isn't enough total width.

-------------------------------------------------------------------------*/

static UINT16 *spritecache_count;


static READ16_HANDLER( spritecache_count_r )
{
	int prevpc = activecpu_get_previouspc();

	/* if this read is coming from $99f8 or $9992, it's in the sprite copy loop */
	if (prevpc == 0x99f8 || prevpc == 0x9992)
	{
		UINT16 *data = &spritecache_count[-0x100];
		int oldword = spritecache_count[0];
		int count = oldword >> 8;
		int i, width = 0;

		/* compute the current total width */
		for (i = 0; i < count; i++)
			width += 1 + ((data[i * 4 + 1] >> 4) & 7);

		/* if we're less than 39, keep adding dummy sprites until we hit it */
		if (width <= 38)
		{
			while (width <= 38)
			{
				data[count * 4 + 0] = (42 * 8) << 7;
				data[count * 4 + 1] = ((30 * 8) << 7) | (7 << 4);
				data[count * 4 + 2] = 0;
				width += 8;
				count++;
			}

			/* update the final count in memory */
			spritecache_count[0] = (count << 8) | (oldword & 0xff);
		}
	}

	/* and then read the data */
	return spritecache_count[offset];
}



/*-------------------------------------------------------------------------

    Unknown Verify

    In several places, the value 1 is stored to the byte at $3fdf1e. A
    fairly complex subroutine is called, and then $3fdf1e is checked to
    see if it was set to zero. If it was, "HARDWARE ERROR" is displayed.

    To avoid this, we just return 1 when this value is read within the
    range of PCs where it is tested.

-------------------------------------------------------------------------*/

static UINT16 *unknown_verify_base;


static READ16_HANDLER( unknown_verify_r )
{
	int prevpc = activecpu_get_previouspc();
	if (prevpc < 0x5c5e || prevpc > 0xc432)
		return unknown_verify_base[offset];
	else
		return unknown_verify_base[offset] | 0x100;
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x037fff) AM_ROM
	AM_RANGE(0x038000, 0x03ffff) AM_READ(bankrom_r) AM_REGION(REGION_CPU1, 0x38000) AM_BASE(&bankrom_base)
	AM_RANGE(0x120000, 0x120fff) AM_READWRITE(atarigen_eeprom_r, atarigen_eeprom_w) AM_BASE(&atarigen_eeprom) AM_SIZE(&atarigen_eeprom_size)
	AM_RANGE(0x260000, 0x260001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x260002, 0x260003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x260010, 0x260011) AM_READ(special_port3_r)
	AM_RANGE(0x260012, 0x260013) AM_READ(input_port_4_word_r)
	AM_RANGE(0x260020, 0x260021) AM_READ(input_port_5_word_r)
	AM_RANGE(0x260022, 0x260023) AM_READ(input_port_6_word_r)
	AM_RANGE(0x260024, 0x260025) AM_READ(input_port_7_word_r)
	AM_RANGE(0x260030, 0x260031) AM_READ(atarigen_sound_r)
	AM_RANGE(0x260040, 0x260041) AM_WRITE(atarigen_sound_w)
	AM_RANGE(0x260050, 0x260051) AM_WRITE(io_latch_w)
	AM_RANGE(0x260060, 0x260061) AM_WRITE(atarigen_eeprom_enable_w)
	AM_RANGE(0x2a0000, 0x2a0001) AM_WRITE(watchdog_reset16_w)
	AM_RANGE(0x3e0000, 0x3e0fff) AM_READWRITE(MRA16_RAM, atarigen_666_paletteram_w) AM_BASE(&paletteram16)
	AM_RANGE(0x3effc0, 0x3effff) AM_READWRITE(atarivc_r, atarivc_w) AM_BASE(&atarivc_data)
	AM_RANGE(0x3f4000, 0x3f5eff) AM_READWRITE(MRA16_RAM, atarigen_playfield_latched_msb_w) AM_BASE(&atarigen_playfield)
	AM_RANGE(0x3f5f00, 0x3f5f7f) AM_RAM AM_BASE(&atarivc_eof_data)
	AM_RANGE(0x3f5f80, 0x3f5fff) AM_READWRITE(MRA16_RAM, atarimo_0_slipram_w) AM_BASE(&atarimo_0_slipram)
	AM_RANGE(0x3f6000, 0x3f7fff) AM_READWRITE(MRA16_RAM, atarigen_playfield_upper_w) AM_BASE(&atarigen_playfield_upper)
	AM_RANGE(0x3f8000, 0x3fcfff) AM_RAM
	AM_RANGE(0x3fd000, 0x3fd7ff) AM_READWRITE(MRA16_RAM, atarimo_0_spriteram_w) AM_BASE(&atarimo_0_spriteram)
	AM_RANGE(0x3fd800, 0x3fffff) AM_RAM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( offtwall )
	PORT_START	/* 260000 */
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)

	PORT_START	/* 260002 */
	PORT_BIT(  0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(3)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(3)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(3)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(3)
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(3)

	JSA_III_PORT	/* audio board port */

	PORT_START	/* 260010 */
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0002, 0x0000, DEF_STR( Controls ) )
	PORT_DIPSETTING(      0x0000, "Whirly-gigs" )	/* this is official Atari terminology! */
	PORT_DIPSETTING(      0x0002, "Joysticks" )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_UNUSED )	/* tested at a454 */
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_UNUSED )	/* tested at a466 */
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_UNUSED )	/* tested before writing to 260040 */
	PORT_SERVICE( 0x0040, IP_ACTIVE_LOW )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_BIT(  0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 260012 */
	PORT_BIT(  0xffff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 260020 */
    PORT_BIT( 0xff, 0, IPT_DIAL_V ) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(1)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 260022 */
    PORT_BIT( 0xff, 0, IPT_DIAL ) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(2)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 260024 */
    PORT_BIT( 0xff, 0, IPT_DIAL_V ) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_REVERSE PORT_PLAYER(3)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout pfmolayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0, 4 },
	{ 0, 1, 2, 3, 8, 9, 10, 11 },
	{ 0*8, 2*8, 4*8, 6*8, 8*8, 10*8, 12*8, 14*8 },
	16*8
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &pfmolayout,  256, 32 },		/* sprites & playfield */
	{ -1 }
};



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( offtwall )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, ATARI_CLOCK_14MHz/2)
	MDRV_CPU_PROGRAM_MAP(main_map,0)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(offtwall)
	MDRV_NVRAM_HANDLER(atarigen)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_NEEDS_6BITS_PER_GUN | VIDEO_UPDATE_BEFORE_VBLANK)
	MDRV_SCREEN_SIZE(42*8, 30*8)
	MDRV_VISIBLE_AREA(0*8, 42*8-1, 0*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(offtwall)
	MDRV_VIDEO_UPDATE(offtwall)

	/* sound hardware */
	MDRV_IMPORT_FROM(jsa_iii_mono_noadpcm)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( offtwall )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "otw2012.bin", 0x00000, 0x20000, CRC(d08d81eb) SHA1(5a72aa2e4fc6455b94aa59a7719d0ddc8bcc80f2) )
	ROM_LOAD16_BYTE( "otw2013.bin", 0x00001, 0x20000, CRC(61c2553d) SHA1(343d39f9b75fd236e9769ec21ab65310f85e31ca) )

	ROM_REGION( 0x14000, REGION_CPU2, 0 )	/* 64k for 6502 code */
	ROM_LOAD( "otw1020.bin", 0x10000, 0x4000, CRC(488112a5) SHA1(55e84855daacfa303d1031de8c9adb992a846e21) )
	ROM_CONTINUE(            0x04000, 0xc000 )

	ROM_REGION( 0xc0000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )
	ROM_LOAD( "otw1014.bin", 0x000000, 0x20000, CRC(4d64507e) SHA1(cb2ac41aecd2702cd57c746a6f5986cd753bc29e) )
	ROM_LOAD( "otw1016.bin", 0x020000, 0x20000, CRC(f5454f3a) SHA1(87d82bd227f7fcfd13b6f4ad88a573d1b96a4fc1) )
	ROM_LOAD( "otw1018.bin", 0x040000, 0x20000, CRC(17864231) SHA1(22f93fcb5d413281157ab8545647f3713f98c135) )
	ROM_LOAD( "otw1015.bin", 0x060000, 0x20000, CRC(271f7856) SHA1(928bc5e7dc589ceb5f55e536b5a05c3866116a24) )
	ROM_LOAD( "otw1017.bin", 0x080000, 0x20000, CRC(7f7f8012) SHA1(1123ea3c6cd2c73617a87d6a5bbb26fca8941af3) )
	ROM_LOAD( "otw1019.bin", 0x0a0000, 0x20000, CRC(9efe511b) SHA1(db1f1d8792bf497bc9ad652b0b7d78c3abf0e817) )
ROM_END


ROM_START( offtwalc )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "090-2612.rom", 0x00000, 0x20000, CRC(fc891a3f) SHA1(027815a20fbc6c0c9242768581b97362b39941c2) )
	ROM_LOAD16_BYTE( "090-2613.rom", 0x00001, 0x20000, CRC(805d79d4) SHA1(943ec9f408ba875bdf1794ce7d24803043480401) )

	ROM_REGION( 0x14000, REGION_CPU2, 0 )	/* 64k for 6502 code */
	ROM_LOAD( "otw1020.bin", 0x10000, 0x4000, CRC(488112a5) SHA1(55e84855daacfa303d1031de8c9adb992a846e21) )
	ROM_CONTINUE(            0x04000, 0xc000 )

	ROM_REGION( 0xc0000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )
	ROM_LOAD( "090-1614.rom", 0x000000, 0x20000, CRC(307ed447) SHA1(acee15e58cd8def8e52a7586aa14240e1f8be319) )
	ROM_LOAD( "090-1616.rom", 0x020000, 0x20000, CRC(a5bd3d9b) SHA1(756d96eac2398dc68679b7641acbf0e79204eebb) )
	ROM_LOAD( "090-1618.rom", 0x040000, 0x20000, CRC(c7d9df5d) SHA1(d5e5fbb7faf42d865862b9ac60f94d20820b00f3) )
	ROM_LOAD( "090-1615.rom", 0x060000, 0x20000, CRC(ac3642c7) SHA1(bb57e039c113c4ce5702983c8e01dbe286d7b58e) )
	ROM_LOAD( "090-1617.rom", 0x080000, 0x20000, CRC(15208a89) SHA1(124484ab54959a1e6d9022a4f3ee4288a79c768b) )
	ROM_LOAD( "090-1619.rom", 0x0a0000, 0x20000, CRC(8a5d79b3) SHA1(0a202d20e6c86989ce2223e10eadf9009dd6ca8e) )
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static const UINT16 default_eeprom[] =
{
	0x0001,0x011A,0x012A,0x0146,0x0100,0x0168,0x0300,0x011E,
	0x0700,0x0122,0x0600,0x0120,0x0400,0x0102,0x0300,0x017E,
	0x0200,0x0128,0x0104,0x0100,0x014E,0x0100,0x013E,0x0122,
	0x011A,0x012A,0x0146,0x0100,0x0168,0x0300,0x011E,0x0700,
	0x0122,0x0600,0x0120,0x0400,0x0102,0x0300,0x017E,0x0200,
	0x0128,0x0104,0x0100,0x014E,0x0100,0x013E,0x0122,0x1A00,
	0x0154,0x0125,0x01DC,0x0100,0x0192,0x0105,0x01DC,0x0181,
	0x012E,0x0106,0x0100,0x0105,0x0179,0x0132,0x0101,0x0100,
	0x01D3,0x0105,0x0116,0x0127,0x0134,0x0100,0x0104,0x01B0,
	0x0165,0x0102,0x1600,0x0000
};


static DRIVER_INIT( offtwall )
{
	atarigen_eeprom_default = default_eeprom;
	atarijsa_init(1, 2, 3, 0x0040);

	/* install son-of-slapstic workarounds */
	spritecache_count = memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x3fde42, 0x3fde43, 0, 0, spritecache_count_r);
	bankswitch_base = memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x037ec2, 0x037f39, 0, 0, bankswitch_r);
	unknown_verify_base = memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x3fdf1e, 0x3fdf1f, 0, 0, unknown_verify_r);
}


static DRIVER_INIT( offtwalc )
{
	atarigen_eeprom_default = default_eeprom;
	atarijsa_init(1, 2, 3, 0x0040);

	/* install son-of-slapstic workarounds */
	spritecache_count = memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x3fde42, 0x3fde43, 0, 0, spritecache_count_r);
	bankswitch_base = memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x037eca, 0x037f43, 0, 0, bankswitch_r);
	unknown_verify_base = memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x3fdf24, 0x3fdf25, 0, 0, unknown_verify_r);
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1991, offtwall, 0,        offtwall, offtwall, offtwall, ROT0, "Atari Games", "Off the Wall (2/3-player upright)", 0 ,0)
GAME( 1991, offtwalc, offtwall, offtwall, offtwall, offtwalc, ROT0, "Atari Games", "Off the Wall (2-player cocktail)", 0 ,0)
