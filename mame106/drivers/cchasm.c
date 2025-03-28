/***************************************************************************

    Cinematronics Cosmic Chasm hardware

    driver by Mathis Rosenhauer

    Games supported:
        * Cosmic Chasm

    Known bugs:
        * none at this time

***************************************************************************/

#include "driver.h"
#include "vidhrdw/vector.h"
#include "machine/z80ctc.h"
#include "cchasm.h"
#include "sound/custom.h"
#include "sound/ay8910.h"
#include "cpu/z80/z80daisy.h"


/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x00ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x040000, 0x04000f) AM_READ(cchasm_6840_r)
	AM_RANGE(0x060000, 0x060001) AM_READ(input_port_0_word_r)
	AM_RANGE(0xf80000, 0xf800ff) AM_READ(cchasm_io_r)
	AM_RANGE(0xffb000, 0xffffff) AM_READ(MRA16_RAM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x00ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x040000, 0x04000f) AM_WRITE(cchasm_6840_w)
	AM_RANGE(0x050000, 0x050001) AM_WRITE(cchasm_refresh_control_w)
	AM_RANGE(0x060000, 0x060001) AM_WRITE(cchasm_led_w)
	AM_RANGE(0x070000, 0x070001) AM_WRITE(watchdog_reset16_w)
	AM_RANGE(0xf80000, 0xf800ff) AM_WRITE(cchasm_io_w)
	AM_RANGE(0xffb000, 0xffffff) AM_WRITE(MWA16_RAM) AM_BASE(&cchasm_ram)
ADDRESS_MAP_END



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x4000, 0x43ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x5000, 0x53ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x6000, 0x6fff) AM_READ(cchasm_snd_io_r)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x4000, 0x43ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x5000, 0x53ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x6000, 0x6fff) AM_WRITE(cchasm_snd_io_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x03) AM_READ(z80ctc_0_r)
ADDRESS_MAP_END


static ADDRESS_MAP_START( sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x03) AM_WRITE(z80ctc_0_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

INPUT_PORTS_START( cchasm )
	PORT_START /* DSW */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x06, 0x06, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x06, "40000" )
	PORT_DIPSETTING(    0x04, "60000" )
	PORT_DIPSETTING(    0x02, "80000" )
	PORT_DIPSETTING(    0x00, "100000" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "Once" )
	PORT_DIPSETTING(    0x10, "Every" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_1C ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START /* IN1 */
	PORT_BIT( 0xff, 0, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10)

	PORT_START /* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START /* IN3 */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Test 1") PORT_CODE(KEYCODE_F1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED ) /* Test 2, not used in cchasm */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED ) /* Test 3, not used in cchasm */
INPUT_PORTS_END



/*************************************
 *
 *  Sound interfaces
 *
 *************************************/

static struct CustomSound_interface custom_interface =
{
	cchasm_sh_start
};



/*************************************
 *
 *  CPU config
 *
 *************************************/

static struct z80_irq_daisy_chain daisy_chain[] =
{
	{ z80ctc_reset, z80ctc_irq_state, z80ctc_irq_ack, z80ctc_irq_reti, 0 }, /* CTC number 0 */
	{ 0,0,0,0,-1 } 		/* end mark */
};



/*************************************
 *
 *  Machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( cchasm )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,8000000)	/* 8 MHz (from schematics) */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)

	MDRV_CPU_ADD(Z80,3584229)		/* 3.58  MHz (from schematics) */
	MDRV_CPU_CONFIG(daisy_chain)
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_IO_MAP(sound_readport,sound_writeport)

	MDRV_FRAMES_PER_SECOND(40)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_VECTOR | VIDEO_RGB_DIRECT)
	MDRV_SCREEN_SIZE(400, 300)
	MDRV_VISIBLE_AREA(0, 1024-1, 0, 768-1)
	MDRV_PALETTE_LENGTH(32768)

	MDRV_VIDEO_START(cchasm)
	MDRV_VIDEO_UPDATE(vector)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1818182)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)

	MDRV_SOUND_ADD(AY8910, 1818182)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.20)

	MDRV_SOUND_ADD(CUSTOM, 0)
	MDRV_SOUND_CONFIG(custom_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( cchasm )
	ROM_REGION( 0x010000, REGION_CPU1, 0 )
    ROM_LOAD16_BYTE( "chasm.u4",  0x000000, 0x001000, CRC(19244f25) SHA1(79deaae82da8d1b16d05bbac43ba900c4b1d9f26) )
    ROM_LOAD16_BYTE( "chasm.u12", 0x000001, 0x001000, CRC(5d702c7d) SHA1(cbdceed45a1112594fbcbeb6976edc932b32d518) )
    ROM_LOAD16_BYTE( "chasm.u8",  0x002000, 0x001000, CRC(56a7ce8a) SHA1(14c790dcddb78d3b81b5a65fe3529e42c9708273) )
    ROM_LOAD16_BYTE( "chasm.u16", 0x002001, 0x001000, CRC(2e192db0) SHA1(1a8ff983295ab52b5099c089b3142cdc56d28aee) )
    ROM_LOAD16_BYTE( "chasm.u3",  0x004000, 0x001000, CRC(9c71c600) SHA1(900526eaff7483fc478ebfb3f14796ff8fd1d01f) )
    ROM_LOAD16_BYTE( "chasm.u11", 0x004001, 0x001000, CRC(a4eb59a5) SHA1(a7bb3ca8f1f000f224def6342ca9d1eabcb210e6) )
    ROM_LOAD16_BYTE( "chasm.u7",  0x006000, 0x001000, CRC(8308dd6e) SHA1(82ad7c27e9a41af5280ecd975d3530ff2ed27ad4) )
    ROM_LOAD16_BYTE( "chasm.u15", 0x006001, 0x001000, CRC(9d3abf97) SHA1(476d684182d92d66263df82e1b5c4ff24b6814e8) )
    ROM_LOAD16_BYTE( "u2",        0x008000, 0x001000, CRC(4e076ae7) SHA1(a72f5425b256785b810ee5f23917b44f778cfcd3) )
    ROM_LOAD16_BYTE( "u10",       0x008001, 0x001000, CRC(cc9e19ca) SHA1(6c46ec265c2cc0683470ed1df978b96b577c5ca1) )
    ROM_LOAD16_BYTE( "chasm.u6",  0x00a000, 0x001000, CRC(a96525d2) SHA1(1c41bc3bf051cf1830182cbde6fba4e56db7e431) )
    ROM_LOAD16_BYTE( "chasm.u14", 0x00a001, 0x001000, CRC(8e426628) SHA1(2d70a7717b18cc892332b9d5d2de3ceba6c1481d) )
    ROM_LOAD16_BYTE( "u1",        0x00c000, 0x001000, CRC(88b71027) SHA1(49fa676d7838c643d642fbc70579ce29e76ba724) )
    ROM_LOAD16_BYTE( "chasm.u9",  0x00c001, 0x001000, CRC(d90c9773) SHA1(4033f0579f0782db2157f6cbece53b0d74e61d4f) )
    ROM_LOAD16_BYTE( "chasm.u5",  0x00e000, 0x001000, CRC(e4a58b7d) SHA1(0e5f948cd110804e6119fafb4e3fa5904dd1390f) )
    ROM_LOAD16_BYTE( "chasm.u13", 0x00e001, 0x001000, CRC(877e849c) SHA1(bdeb97fcb7488e7f0866dd651204c362d2ec9f4f) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "2732.bin", 0x0000, 0x1000, CRC(715adc4a) SHA1(426be4f3334ef7f2e8eb4d533e64276c30812aa3) )
ROM_END

ROM_START( cchasm1 )
	ROM_REGION( 0x010000, REGION_CPU1, 0 )
    ROM_LOAD16_BYTE( "chasm.u4",  0x000000, 0x001000, CRC(19244f25) SHA1(79deaae82da8d1b16d05bbac43ba900c4b1d9f26) )
    ROM_LOAD16_BYTE( "chasm.u12", 0x000001, 0x001000, CRC(5d702c7d) SHA1(cbdceed45a1112594fbcbeb6976edc932b32d518) )
    ROM_LOAD16_BYTE( "chasm.u8",  0x002000, 0x001000, CRC(56a7ce8a) SHA1(14c790dcddb78d3b81b5a65fe3529e42c9708273) )
    ROM_LOAD16_BYTE( "chasm.u16", 0x002001, 0x001000, CRC(2e192db0) SHA1(1a8ff983295ab52b5099c089b3142cdc56d28aee) )
    ROM_LOAD16_BYTE( "chasm.u3",  0x004000, 0x001000, CRC(9c71c600) SHA1(900526eaff7483fc478ebfb3f14796ff8fd1d01f) )
    ROM_LOAD16_BYTE( "chasm.u11", 0x004001, 0x001000, CRC(a4eb59a5) SHA1(a7bb3ca8f1f000f224def6342ca9d1eabcb210e6) )
    ROM_LOAD16_BYTE( "chasm.u7",  0x006000, 0x001000, CRC(8308dd6e) SHA1(82ad7c27e9a41af5280ecd975d3530ff2ed27ad4) )
    ROM_LOAD16_BYTE( "chasm.u15", 0x006001, 0x001000, CRC(9d3abf97) SHA1(476d684182d92d66263df82e1b5c4ff24b6814e8) )
    ROM_LOAD16_BYTE( "chasm.u2",  0x008000, 0x001000, CRC(008b26ef) SHA1(6758d77bf48f466b8692bf7c678a597792d8cfdb) )
    ROM_LOAD16_BYTE( "chasm.u10", 0x008001, 0x001000, CRC(c2c532a3) SHA1(d29d40d42a2f69de0b1e2ee6a32633468a94fd85) )
    ROM_LOAD16_BYTE( "chasm.u6",  0x00a000, 0x001000, CRC(a96525d2) SHA1(1c41bc3bf051cf1830182cbde6fba4e56db7e431) )
    ROM_LOAD16_BYTE( "chasm.u14", 0x00a001, 0x001000, CRC(8e426628) SHA1(2d70a7717b18cc892332b9d5d2de3ceba6c1481d) )
    ROM_LOAD16_BYTE( "chasm.u1",  0x00c000, 0x001000, CRC(e02293f8) SHA1(136757b3c9e0ebfde6c13c57ac52f5fdbf5fd65b) )
    ROM_LOAD16_BYTE( "chasm.u9",  0x00c001, 0x001000, CRC(d90c9773) SHA1(4033f0579f0782db2157f6cbece53b0d74e61d4f) )
    ROM_LOAD16_BYTE( "chasm.u5",  0x00e000, 0x001000, CRC(e4a58b7d) SHA1(0e5f948cd110804e6119fafb4e3fa5904dd1390f) )
    ROM_LOAD16_BYTE( "chasm.u13", 0x00e001, 0x001000, CRC(877e849c) SHA1(bdeb97fcb7488e7f0866dd651204c362d2ec9f4f) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "2732.bin", 0x0000, 0x1000, CRC(715adc4a) SHA1(426be4f3334ef7f2e8eb4d533e64276c30812aa3) )
ROM_END




/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1983, cchasm,  0,      cchasm, cchasm, 0, ROT270, "Cinematronics / GCE", "Cosmic Chasm (set 1)", 0 ,2)
GAME( 1983, cchasm1, cchasm, cchasm, cchasm, 0, ROT270, "Cinematronics / GCE", "Cosmic Chasm (set 2)", 0 ,2)
