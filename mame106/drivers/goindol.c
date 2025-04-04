/***************************************************************************
  GOINDOL

  Driver provided by Jarek Parchanski (jpdev@friko6.onet.pl)

Notes:
- byte at 7f87 controls region:
  0 = Japan
  1 = USA
  2 = World
  Regardless of the setting of this byte, the startup notice in Korean is
  always displayed.
  After the title screen, depending on the byte you get "for use only in Japan",
  "for use only in USA", or the Korean notice again! So 2 might actually mean
  Korea instead of World... but that version surely got to Europe since Gerald
  has three boards with this ROM.

***************************************************************************/

#include "driver.h"
#include "sound/2203intf.h"

VIDEO_START( goindol );
WRITE8_HANDLER( goindol_fg_videoram_w );
WRITE8_HANDLER( goindol_bg_videoram_w );
VIDEO_UPDATE( goindol );

extern UINT8 *goindol_fg_scrollx;
extern UINT8 *goindol_fg_scrolly;
extern UINT8 *goindol_fg_videoram;
extern UINT8 *goindol_bg_videoram;
extern size_t goindol_fg_videoram_size;
extern size_t goindol_bg_videoram_size;
extern int goindol_char_bank;


WRITE8_HANDLER( goindol_bankswitch_w )
{
	int bankaddress;
	UINT8 *RAM = memory_region(REGION_CPU1);

	bankaddress = 0x10000 + ((data & 3) * 0x4000);
	memory_set_bankptr(1,&RAM[bankaddress]);

	if (goindol_char_bank != ((data & 0x10) >> 4))
	{
		goindol_char_bank = (data & 0x10) >> 4;
		tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);
	}

	flip_screen_set(data & 0x20);
}



static READ8_HANDLER( prot_f422_r )
{
	static int toggle;

	/* bit 7 = vblank? */
	toggle ^= 0x80;

	return toggle;
}


static UINT8 *ram;

static WRITE8_HANDLER( prot_fc44_w )
{
logerror("%04x: prot_fc44_w(%02x)\n",activecpu_get_pc(),data);
	ram[0x0419] = 0x5b;
	ram[0x041a] = 0x3f;
	ram[0x041b] = 0x6d;
}

static WRITE8_HANDLER( prot_fd99_w )
{
logerror("%04x: prot_fd99_w(%02x)\n",activecpu_get_pc(),data);
	ram[0x0421] = 0x3f;
}

static WRITE8_HANDLER( prot_fc66_w )
{
logerror("%04x: prot_fc66_w(%02x)\n",activecpu_get_pc(),data);
	ram[0x0423] = 0x06;
}

static WRITE8_HANDLER( prot_fcb0_w )
{
logerror("%04x: prot_fcb0_w(%02x)\n",activecpu_get_pc(),data);
	ram[0x0425] = 0x06;
}



static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1)
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xc800, 0xc800) AM_READ(MRA8_NOP)	// watchdog?
	AM_RANGE(0xd000, 0xefff) AM_READ(MRA8_RAM)
	AM_RANGE(0xf000, 0xf000) AM_READ(input_port_3_r)
	AM_RANGE(0xf422, 0xf422) AM_READ(prot_f422_r)
	AM_RANGE(0xf800, 0xf800) AM_READ(input_port_4_r)
	AM_RANGE(0xc834, 0xc834) AM_READ(input_port_1_r)
	AM_RANGE(0xc820, 0xc820) AM_READ(input_port_2_r)
	AM_RANGE(0xc830, 0xc830) AM_READ(input_port_0_r)
	AM_RANGE(0xe000, 0xefff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM) AM_BASE(&ram)
	AM_RANGE(0xc810, 0xc810) AM_WRITE(goindol_bankswitch_w)
	AM_RANGE(0xc820, 0xd820) AM_WRITE(MWA8_RAM) AM_BASE(&goindol_fg_scrolly)
	AM_RANGE(0xc830, 0xd830) AM_WRITE(MWA8_RAM) AM_BASE(&goindol_fg_scrollx)
	AM_RANGE(0xc800, 0xc800) AM_WRITE(soundlatch_w)
	AM_RANGE(0xd000, 0xd03f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xd040, 0xd7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xd800, 0xdfff) AM_WRITE(goindol_bg_videoram_w) AM_BASE(&goindol_bg_videoram) AM_SIZE(&goindol_bg_videoram_size)
	AM_RANGE(0xe000, 0xe03f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_2)
	AM_RANGE(0xe040, 0xe7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe800, 0xefff) AM_WRITE(goindol_fg_videoram_w) AM_BASE(&goindol_fg_videoram) AM_SIZE(&goindol_fg_videoram_size)
	AM_RANGE(0xfc44, 0xfc44) AM_WRITE(prot_fc44_w)
	AM_RANGE(0xfc66, 0xfc66) AM_WRITE(prot_fc66_w)
	AM_RANGE(0xfcb0, 0xfcb0) AM_WRITE(prot_fcb0_w)
	AM_RANGE(0xfd99, 0xfd99) AM_WRITE(prot_fd99_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xd800, 0xd800) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xa000, 0xa000) AM_WRITE(YM2203_control_port_0_w)
	AM_RANGE(0xa001, 0xa001) AM_WRITE(YM2203_write_port_0_w)
ADDRESS_MAP_END

#define GOINDOL_INPUT_BITS \
	PORT_START_TAG("IN0")\
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY\
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )\
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_IMPULSE(1)\
	PORT_START_TAG("IN1")\
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )\
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_IMPULSE(1)\
	PORT_START_TAG("IN2")      /* spinner */\
	PORT_BIT( 0xff, 0x00, IPT_DIAL  ) PORT_SENSITIVITY(40) PORT_KEYDELTA(10)

#define GOINDOL_DSW1 \
	PORT_START_TAG("DSW1")\
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Bonus_Life ) )\
	PORT_DIPSETTING(    0x04, "30k and every 50k" )\
	PORT_DIPSETTING(    0x05, "50k and every 100k" )\
	PORT_DIPSETTING(    0x06, "50k and every 200k" )\
	PORT_DIPSETTING(    0x07, "100k and every 200k" )\
	PORT_DIPSETTING(    0x01, "10000 only" )\
	PORT_DIPSETTING(    0x02, "30000 only" )\
	PORT_DIPSETTING(    0x03, "50000 only" )\
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )\
	PORT_DIPNAME( 0x38, 0x00, DEF_STR( Coinage ) )\
	PORT_DIPSETTING(    0x28, DEF_STR( 3C_1C ) )\
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )\
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )\
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_3C ) )\
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_4C ) )\
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_5C ) )\
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_6C ) )\
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Cabinet ) )\
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )\
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Flip_Screen ) )\
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )\
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

INPUT_PORTS_START( goindol )
GOINDOL_INPUT_BITS

	PORT_START_TAG("DSW0")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x1c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x18, "Very Very Easy" )
	PORT_DIPSETTING(    0x14, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, "Difficult" )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ))
 	PORT_DIPNAME( 0x40, 0x40, "Invulnerability (Cheat)")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

GOINDOL_DSW1
INPUT_PORTS_END

INPUT_PORTS_START( homo )
GOINDOL_INPUT_BITS

	PORT_START_TAG("DSW0")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x1c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x18, "Very Very Easy" )
	PORT_DIPSETTING(    0x14, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, "Difficult" )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ))
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ))
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

GOINDOL_DSW1
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{  RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout, 0, 32 },
	{ REGION_GFX2, 0, &charlayout, 0, 32 },
	{ -1 } /* end of array */
};



static MACHINE_DRIVER_START( goindol )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 6000000)        /* 6 MHz (?) */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(goindol)
	MDRV_VIDEO_UPDATE(goindol)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, 2000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( goindol )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "r1w", 0x00000, 0x8000, CRC(df77c502) SHA1(15d111e38d63a8a800fbf5f15c4fb72efb0e5cf4) ) /* Code 0000-7fff */
	ROM_LOAD( "r2",  0x10000, 0x8000, CRC(1ff6e3a2) SHA1(321d32b5236f8fadc55b00412081cd17fbdb42bf) ) /* Paged data */
	ROM_LOAD( "r3",  0x18000, 0x8000, CRC(e9eec24a) SHA1(d193dd23b8bee3a788114e6bb86902dddf6fdd99) ) /* Paged data */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "r10", 0x00000, 0x8000, CRC(72e1add1) SHA1(e8bdaffbbbf8ed22eb161cb8d7945ff09420f68f) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "r4", 0x00000, 0x8000, CRC(1ab84225) SHA1(47494d03fb8d153335203155e61d90108db62961) ) /* Characters */
	ROM_LOAD( "r5", 0x08000, 0x8000, CRC(4997d469) SHA1(60c482b2408079bc8b2ffb86bc01927d5cad66ea) )
	ROM_LOAD( "r6", 0x10000, 0x8000, CRC(752904b0) SHA1(6ff44bd45b000bccae4fd67eefce936aacd971fc) )

	ROM_REGION( 0x18000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "r7", 0x00000, 0x8000, CRC(362f2a27) SHA1(9b8232a9ce7d752a749897fb2231a005c734239d) )
	ROM_LOAD( "r8", 0x08000, 0x8000, CRC(9fc7946e) SHA1(89100fae14826ad4f6735770827cbfe97562038c) )
	ROM_LOAD( "r9", 0x10000, 0x8000, CRC(e6212fe4) SHA1(f42b5ddbdb6599ba4ff5e6ef7d86e55f58a671b6) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "am27s21.pr1", 0x0000, 0x0100, CRC(361f0868) SHA1(aea681a2e168aca327a998db7b537c7b82dbc433) )	/* palette red bits   */
	ROM_LOAD( "am27s21.pr2", 0x0100, 0x0100, CRC(e355da4d) SHA1(40ebdbf6519b2817402ea716aae838c315da4fcb) )	/* palette green bits */
	ROM_LOAD( "am27s21.pr3", 0x0200, 0x0100, CRC(8534cfb5) SHA1(337b6d5e9ceb2116aea73a7a4ac7e70716460323) )	/* palette blue bits  */
ROM_END

ROM_START( goindolu )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "r1", 0x00000, 0x8000, CRC(3111c61b) SHA1(6cc3834f946566646f06efe0b65c4704574ec6f1) ) /* Code 0000-7fff */
	ROM_LOAD( "r2", 0x10000, 0x8000, CRC(1ff6e3a2) SHA1(321d32b5236f8fadc55b00412081cd17fbdb42bf) ) /* Paged data */
	ROM_LOAD( "r3", 0x18000, 0x8000, CRC(e9eec24a) SHA1(d193dd23b8bee3a788114e6bb86902dddf6fdd99) ) /* Paged data */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "r10", 0x00000, 0x8000, CRC(72e1add1) SHA1(e8bdaffbbbf8ed22eb161cb8d7945ff09420f68f) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "r4", 0x00000, 0x8000, CRC(1ab84225) SHA1(47494d03fb8d153335203155e61d90108db62961) ) /* Characters */
	ROM_LOAD( "r5", 0x08000, 0x8000, CRC(4997d469) SHA1(60c482b2408079bc8b2ffb86bc01927d5cad66ea) )
	ROM_LOAD( "r6", 0x10000, 0x8000, CRC(752904b0) SHA1(6ff44bd45b000bccae4fd67eefce936aacd971fc) )

	ROM_REGION( 0x18000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "r7", 0x00000, 0x8000, CRC(362f2a27) SHA1(9b8232a9ce7d752a749897fb2231a005c734239d) )
	ROM_LOAD( "r8", 0x08000, 0x8000, CRC(9fc7946e) SHA1(89100fae14826ad4f6735770827cbfe97562038c) )
	ROM_LOAD( "r9", 0x10000, 0x8000, CRC(e6212fe4) SHA1(f42b5ddbdb6599ba4ff5e6ef7d86e55f58a671b6) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "am27s21.pr1", 0x0000, 0x0100, CRC(361f0868) SHA1(aea681a2e168aca327a998db7b537c7b82dbc433) )	/* palette red bits   */
	ROM_LOAD( "am27s21.pr2", 0x0100, 0x0100, CRC(e355da4d) SHA1(40ebdbf6519b2817402ea716aae838c315da4fcb) )	/* palette green bits */
	ROM_LOAD( "am27s21.pr3", 0x0200, 0x0100, CRC(8534cfb5) SHA1(337b6d5e9ceb2116aea73a7a4ac7e70716460323) )	/* palette blue bits  */
ROM_END

ROM_START( goindolj )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "r1j", 0x00000, 0x8000, CRC(dde33ad3) SHA1(23cdb3494f5eeaeae2657a0101d5827aa32c526d) ) /* Code 0000-7fff */
	ROM_LOAD( "r2",  0x10000, 0x8000, CRC(1ff6e3a2) SHA1(321d32b5236f8fadc55b00412081cd17fbdb42bf) ) /* Paged data */
	ROM_LOAD( "r3",  0x18000, 0x8000, CRC(e9eec24a) SHA1(d193dd23b8bee3a788114e6bb86902dddf6fdd99) ) /* Paged data */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "r10", 0x00000, 0x8000, CRC(72e1add1) SHA1(e8bdaffbbbf8ed22eb161cb8d7945ff09420f68f) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "r4", 0x00000, 0x8000, CRC(1ab84225) SHA1(47494d03fb8d153335203155e61d90108db62961) ) /* Characters */
	ROM_LOAD( "r5", 0x08000, 0x8000, CRC(4997d469) SHA1(60c482b2408079bc8b2ffb86bc01927d5cad66ea) )
	ROM_LOAD( "r6", 0x10000, 0x8000, CRC(752904b0) SHA1(6ff44bd45b000bccae4fd67eefce936aacd971fc) )

	ROM_REGION( 0x18000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "r7", 0x00000, 0x8000, CRC(362f2a27) SHA1(9b8232a9ce7d752a749897fb2231a005c734239d) )
	ROM_LOAD( "r8", 0x08000, 0x8000, CRC(9fc7946e) SHA1(89100fae14826ad4f6735770827cbfe97562038c) )
	ROM_LOAD( "r9", 0x10000, 0x8000, CRC(e6212fe4) SHA1(f42b5ddbdb6599ba4ff5e6ef7d86e55f58a671b6) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "am27s21.pr1", 0x0000, 0x0100, CRC(361f0868) SHA1(aea681a2e168aca327a998db7b537c7b82dbc433) )	/* palette red bits   */
	ROM_LOAD( "am27s21.pr2", 0x0100, 0x0100, CRC(e355da4d) SHA1(40ebdbf6519b2817402ea716aae838c315da4fcb) )	/* palette green bits */
	ROM_LOAD( "am27s21.pr3", 0x0200, 0x0100, CRC(8534cfb5) SHA1(337b6d5e9ceb2116aea73a7a4ac7e70716460323) )	/* palette blue bits  */
ROM_END

ROM_START( homo )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )     /* 2*64k for code */
	ROM_LOAD( "homo.01", 0x00000, 0x8000, CRC(28c539ad) SHA1(64e950a4238a5656a9e0d0a699a6545da8c59548) ) /* Code 0000-7fff */
	ROM_LOAD( "r2", 0x10000, 0x8000, CRC(1ff6e3a2) SHA1(321d32b5236f8fadc55b00412081cd17fbdb42bf) ) /* Paged data */
	ROM_LOAD( "r3", 0x18000, 0x8000, CRC(e9eec24a) SHA1(d193dd23b8bee3a788114e6bb86902dddf6fdd99) ) /* Paged data */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for the audio CPU */
	ROM_LOAD( "r10", 0x00000, 0x8000, CRC(72e1add1) SHA1(e8bdaffbbbf8ed22eb161cb8d7945ff09420f68f) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "r4", 0x00000, 0x8000, CRC(1ab84225) SHA1(47494d03fb8d153335203155e61d90108db62961) ) /* Characters */
	ROM_LOAD( "r5", 0x08000, 0x8000, CRC(4997d469) SHA1(60c482b2408079bc8b2ffb86bc01927d5cad66ea) )
	ROM_LOAD( "r6", 0x10000, 0x8000, CRC(752904b0) SHA1(6ff44bd45b000bccae4fd67eefce936aacd971fc) )

	ROM_REGION( 0x18000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "r7", 0x00000, 0x8000, CRC(362f2a27) SHA1(9b8232a9ce7d752a749897fb2231a005c734239d) )
	ROM_LOAD( "r8", 0x08000, 0x8000, CRC(9fc7946e) SHA1(89100fae14826ad4f6735770827cbfe97562038c) )
	ROM_LOAD( "r9", 0x10000, 0x8000, CRC(e6212fe4) SHA1(f42b5ddbdb6599ba4ff5e6ef7d86e55f58a671b6) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
	ROM_LOAD( "am27s21.pr1", 0x0000, 0x0100, CRC(361f0868) SHA1(aea681a2e168aca327a998db7b537c7b82dbc433) )	/* palette red bits   */
	ROM_LOAD( "am27s21.pr2", 0x0100, 0x0100, CRC(e355da4d) SHA1(40ebdbf6519b2817402ea716aae838c315da4fcb) )	/* palette green bits */
	ROM_LOAD( "am27s21.pr3", 0x0200, 0x0100, CRC(8534cfb5) SHA1(337b6d5e9ceb2116aea73a7a4ac7e70716460323) )	/* palette blue bits  */
ROM_END



DRIVER_INIT( goindol )
{
	UINT8 *rom = memory_region(REGION_CPU1);


	/* I hope that's all patches to avoid protection */

	rom[0x18e9] = 0x18;	// ROM 1 check
	rom[0x1964] = 0x00; // ROM 9 error (MCU?)
	rom[0x1965] = 0x00; //
	rom[0x1966] = 0x00; //
//  rom[0x17c7] = 0x00; // c421 == 3f
//  rom[0x17c8] = 0x00; //
//  rom[0x16f0] = 0x18; // c425 == 06
//  rom[0x172c] = 0x18; // c423 == 06
//  rom[0x1779] = 0x00; // c419 == 5b 3f 6d
//  rom[0x177a] = 0x00; //
	rom[0x063f] = 0x18;	//->fc55
	rom[0x0b30] = 0x00;	// verify code at 0601-064b
	rom[0x1bdf] = 0x18;	//->fc49

	rom[0x04a7] = 0xc9;
	rom[0x0831] = 0xc9;
	rom[0x3365] = 0x00;	// verify code at 081d-0876
	rom[0x0c13] = 0xc9;
	rom[0x134e] = 0xc9;
	rom[0x333d] = 0xc9;
}



GAME( 1987, goindol,  0,       goindol, goindol, goindol, ROT90, "Sun a Electronics", "Goindol (World)", GAME_UNEMULATED_PROTECTION ,0)
GAME( 1987, goindolu, goindol, goindol, goindol, goindol, ROT90, "Sun a Electronics", "Goindol (US)",    GAME_UNEMULATED_PROTECTION ,0)
GAME( 1987, goindolj, goindol, goindol, goindol, goindol, ROT90, "Sun a Electronics", "Goindol (Japan)", GAME_UNEMULATED_PROTECTION ,0)
GAME( 1987, homo,     goindol, goindol, homo,    0,       ROT90, "bootleg", "Homo", 0 ,0)
