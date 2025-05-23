/***************************************************************************

  Crude Buster (World version FX)       (c) 1990 Data East Corporation
  Crude Buster (World version FU)       (c) 1990 Data East Corporation
  Crude Buster (Japanese version)       (c) 1990 Data East Corporation
  Two Crude (USA version)               (c) 1990 Data East USA

  The 'FX' board is filled with 'FU' roms except for the 4 program roms,
  both boards have 'export' stickers which usually indicates a World version.
  Maybe one is a UK or European version.

  Emulation by Bryan McPhail, mish@tendril.co.uk

***************************************************************************/

#include "driver.h"
#include "cpu/h6280/h6280.h"
#include "sound/2203intf.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"
#include "deco16ic.h"

VIDEO_START( twocrude );
VIDEO_UPDATE( twocrude );

WRITE16_HANDLER( twocrude_palette_24bit_rg_w );
WRITE16_HANDLER( twocrude_palette_24bit_b_w );

static UINT16 *twocrude_ram;
extern void twocrude_pri_w(int pri);
WRITE16_HANDLER( twocrude_update_sprites_w );
static UINT16 prot;

/******************************************************************************/

static WRITE16_HANDLER( twocrude_control_w )
{
	switch (offset<<1) {
	case 0: /* DMA flag */
		buffer_spriteram16_w(0,0,0);
		return;

	case 6: /* IRQ ack */
		return;

    case 2: /* Sound CPU write */
		soundlatch_w(0,data & 0xff);
		cpunum_set_input_line(1,0,HOLD_LINE);
    	return;

	case 4: /* Protection, maybe this is a PAL on the board?

            80046 is level number
            stop at stage and enter.
            see also 8216..

                9a 00 = pf4 over pf3 (normal) (level 0)
                9a f1 =  (level 1 - water), pf3 over ALL sprites + pf4
                9a 80 = pf3 over pf4 (Level 2 - copter)
                9a 40 = pf3 over ALL sprites + pf4 (snow) level 3
                9a c0 = doesn't matter?
                9a ff = pf 3 over pf4

            I can't find a priority register, I assume it's tied to the
            protection?!

        */
		if ((data&0xffff)==0x9a00) prot=0;
		if ((data&0xffff)==0xaa) prot=0x74;
		if ((data&0xffff)==0x0200) prot=0x63<<8;
		if ((data&0xffff)==0x9a) prot=0xe;
		if ((data&0xffff)==0x55) prot=0x1e;
		if ((data&0xffff)==0x0e) {prot=0x0e;twocrude_pri_w(0);} /* start */
		if ((data&0xffff)==0x00) {prot=0x0e;twocrude_pri_w(0);} /* level 0 */
		if ((data&0xffff)==0xf1) {prot=0x36;twocrude_pri_w(1);} /* level 1 */
		if ((data&0xffff)==0x80) {prot=0x2e;twocrude_pri_w(1);} /* level 2 */
		if ((data&0xffff)==0x40) {prot=0x1e;twocrude_pri_w(1);} /* level 3 */
		if ((data&0xffff)==0xc0) {prot=0x3e;twocrude_pri_w(0);} /* level 4 */
		if ((data&0xffff)==0xff) {prot=0x76;twocrude_pri_w(1);} /* level 5 */

		break;
	}
	logerror("Warning %04x- %02x written to control %02x\n",activecpu_get_pc(),data,offset);
}

READ16_HANDLER( twocrude_control_r )
{
	switch (offset<<1)
	{
		case 0: /* Player 1 & Player 2 joysticks & fire buttons */
			return (readinputport(0) + (readinputport(1) << 8));

		case 2: /* Dip Switches */
			return (readinputport(3) + (readinputport(4) << 8));

		case 4: /* Protection */
			logerror("%04x : protection control read at 30c000 %d\n",activecpu_get_pc(),offset);
			return prot;

		case 6: /* Credits, VBL in byte 7 */
			return readinputport(2);
	}

	return ~0;
}

/******************************************************************************/

static ADDRESS_MAP_START( twocrude_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x080000, 0x083fff) AM_READ(MRA16_RAM)

	AM_RANGE(0x0a0000, 0x0a1fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0a2000, 0x0a2fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0a4000, 0x0a47ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0a6000, 0x0a67ff) AM_READ(MRA16_RAM)

	AM_RANGE(0x0a8000, 0x0a8fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0aa000, 0x0aafff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0ac000, 0x0ac7ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0ae000, 0x0ae7ff) AM_READ(MRA16_RAM)

	AM_RANGE(0x0b0000, 0x0b07ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0b8000, 0x0b8fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0b9000, 0x0b9fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0bc000, 0x0bc00f) AM_READ(twocrude_control_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( twocrude_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x080000, 0x083fff) AM_WRITE(MWA16_RAM) AM_BASE(&twocrude_ram)

	AM_RANGE(0x0a0000, 0x0a1fff) AM_WRITE(deco16_pf1_data_w) AM_BASE(&deco16_pf1_data)
	AM_RANGE(0x0a2000, 0x0a2fff) AM_WRITE(deco16_pf2_data_w) AM_BASE(&deco16_pf2_data)
	AM_RANGE(0x0a4000, 0x0a47ff) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf1_rowscroll)
	AM_RANGE(0x0a6000, 0x0a67ff) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf2_rowscroll)

	AM_RANGE(0x0a8000, 0x0a8fff) AM_WRITE(deco16_pf3_data_w) AM_BASE(&deco16_pf3_data)
	AM_RANGE(0x0aa000, 0x0aafff) AM_WRITE(deco16_pf4_data_w) AM_BASE(&deco16_pf4_data)
	AM_RANGE(0x0ac000, 0x0ac7ff) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf3_rowscroll)
	AM_RANGE(0x0ae000, 0x0ae7ff) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf4_rowscroll)

	AM_RANGE(0x0b0000, 0x0b07ff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0x0b4000, 0x0b4001) AM_WRITE(MWA16_NOP)

	AM_RANGE(0x0b5000, 0x0b500f) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf12_control)
	AM_RANGE(0x0b6000, 0x0b600f) AM_WRITE(MWA16_RAM) AM_BASE(&deco16_pf34_control)

	AM_RANGE(0x0b8000, 0x0b8fff) AM_WRITE(twocrude_palette_24bit_rg_w) AM_BASE(&paletteram16)
	AM_RANGE(0x0b9000, 0x0b9fff) AM_WRITE(twocrude_palette_24bit_b_w) AM_BASE(&paletteram16_2)
	AM_RANGE(0x0bc000, 0x0bc00f) AM_WRITE(twocrude_control_w)
ADDRESS_MAP_END

/******************************************************************************/

static WRITE8_HANDLER( YM2151_w )
{
	switch (offset) {
	case 0:
		YM2151_register_port_0_w(0,data);
		break;
	case 1:
		YM2151_data_port_0_w(0,data);
		break;
	}
}

static WRITE8_HANDLER( YM2203_w )
{
	switch (offset) {
	case 0:
		YM2203_control_port_0_w(0,data);
		break;
	case 1:
		YM2203_write_port_0_w(0,data);
		break;
	}
}

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_READ(MRA8_ROM)
	AM_RANGE(0x100000, 0x100001) AM_READ(YM2203_status_port_0_r)
	AM_RANGE(0x110000, 0x110001) AM_READ(YM2151_status_port_0_r)
	AM_RANGE(0x120000, 0x120001) AM_READ(OKIM6295_status_0_r)
	AM_RANGE(0x130000, 0x130001) AM_READ(OKIM6295_status_1_r)
	AM_RANGE(0x140000, 0x140001) AM_READ(soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_READ(MRA8_BANK8)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x100000, 0x100001) AM_WRITE(YM2203_w)
	AM_RANGE(0x110000, 0x110001) AM_WRITE(YM2151_w)
	AM_RANGE(0x120000, 0x120001) AM_WRITE(OKIM6295_data_0_w)
	AM_RANGE(0x130000, 0x130001) AM_WRITE(OKIM6295_data_1_w)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_WRITE(MWA8_BANK8)
	AM_RANGE(0x1fec00, 0x1fec01) AM_WRITE(H6280_timer_w)
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(H6280_irq_status_w)
ADDRESS_MAP_END

/******************************************************************************/

INPUT_PORTS_START( twocrude )
	PORT_START	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* Credits */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* Dip switch bank 1 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* Dip switch bank 2 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x01, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 24,16,8,0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	8*32
};

static const gfx_layout tilelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 24, 16, 8, 0 },
	{ 64*8+0, 64*8+1, 64*8+2, 64*8+3, 64*8+4, 64*8+5, 64*8+6, 64*8+7,
		0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	128*8
};

static const gfx_layout spritelayout =
{
	16,16,
	(4096*2)+2048,  /* Main bank + 4 extra roms */
	4,
	{ 0xa0000*8+8, 0xa0000*8, 8, 0 },
	{ 32*8+0, 32*8+1, 32*8+2, 32*8+3, 32*8+4, 32*8+5, 32*8+6, 32*8+7,
		0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	64*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout, 	   0, 0x500 },	/* Characters 8x8 */
	{ REGION_GFX1, 0, &tilelayout,     0, 0x500 },	/* Tiles 16x16 */
	{ REGION_GFX2, 0, &tilelayout,     0, 0x500 },	/* Tiles 16x16 */
	{ REGION_GFX3, 0, &spritelayout, 0x100, 80 },	/* Sprites 16x16 */
	{ -1 } /* end of array */
};

/******************************************************************************/

static void sound_irq(int state)
{
	cpunum_set_input_line(1,1,state); /* IRQ 2 */
}

static struct YM2151interface ym2151_interface =
{
	sound_irq
};

static MACHINE_DRIVER_START( twocrude )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 12000000) /* Custom chip 59 */
	MDRV_CPU_PROGRAM_MAP(twocrude_readmem,twocrude_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)/* VBL */

	MDRV_CPU_ADD(H6280,32220000/4) /* Custom chip 45, Audio section crystal is 32.220 MHz */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(529)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_NEEDS_6BITS_PER_GUN | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(twocrude)
	MDRV_VIDEO_UPDATE(twocrude)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2203, 32220000/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)

	MDRV_SOUND_ADD(YM2151, 32220000/9)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.45)
	MDRV_SOUND_ROUTE(1, "mono", 0.45)

	MDRV_SOUND_ADD(OKIM6295, 32220000/32/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	MDRV_SOUND_ADD(OKIM6295, 32220000/16/132)
	MDRV_SOUND_CONFIG(okim6295_interface_region_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)
MACHINE_DRIVER_END

/******************************************************************************/

ROM_START( cbuster )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
  	ROM_LOAD16_BYTE( "fx01.rom", 0x00000, 0x20000, CRC(ddae6d83) SHA1(ce3fed1393b71821730fb8d87869a89c8e07c456) )
	ROM_LOAD16_BYTE( "fx00.rom", 0x00001, 0x20000, CRC(5bc2c0de) SHA1(fa9c357ae4a5c814b7113df3b2f12982077f3e6b) )
  	ROM_LOAD16_BYTE( "fx03.rom", 0x40000, 0x20000, CRC(c3d65bf9) SHA1(99dd650fd4b427bca25a0776fbd6221f93504106) )
 	ROM_LOAD16_BYTE( "fx02.rom", 0x40001, 0x20000, CRC(b875266b) SHA1(a76f8e061392e17394a3f975584823ad39e0097e) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fu11-.rom",     0x00000, 0x10000, CRC(65f20f10) SHA1(cf914893edd98a0f39bbf7068a469ed7d34bd90e) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-00",        0x00000, 0x80000, CRC(660eaabd) SHA1(e3d614e13fdb9af159d9758a869d9dae3dbe14e0) ) /* Tiles */
	ROM_LOAD16_BYTE( "fu05-.rom",     0x80000, 0x10000, CRC(8134d412) SHA1(9c70ff6f9f24ec89c0bb4645afdf2a5ca27e9a0c) ) /* Chars */
	ROM_LOAD16_BYTE( "fu06-.rom",     0x80001, 0x10000, CRC(2f914a45) SHA1(bb44ba4779e45ee77ef0006363df91aac1f4559a) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-01",        0x00000, 0x80000, CRC(1080d619) SHA1(68f33a1580d33e4dd0858248c12a0a10ac117249) ) /* Tiles */

	ROM_REGION( 0x180000,REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-02",        0x000000, 0x80000, CRC(58b7231d) SHA1(5b51a2fa42c67f23648be205295184a1fddc00f5) ) /* Sprites */
	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "mab-03",        0x0a0000, 0x80000, CRC(76053b9d) SHA1(093cd01a13509701ec9dd1a806132600a5bd1915) )
 	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "fu07-.rom",     0x140000, 0x10000, CRC(ca8d0bb3) SHA1(9262d6003cf0cb8c33d0f6c1d0ef35490b29f9b4) ) /* Extra sprites */
	ROM_LOAD( "fu08-.rom",     0x150000, 0x10000, CRC(c6afc5c8) SHA1(feddd546f09884c51e4d1802477de4e152a51082) )
	ROM_LOAD( "fu09-.rom",     0x160000, 0x10000, CRC(526809ca) SHA1(2cb9e7417211c1eb23d32e3fee71c5254d34a3ff) )
	ROM_LOAD( "fu10-.rom",     0x170000, 0x10000, CRC(6be6d50e) SHA1(b944db4b3a7c76190f6b40f71f033e16e7964f6a) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu12-.rom",     0x00000, 0x20000, CRC(2d1d65f2) SHA1(be3d57b9976ddf7ee6d20ee9e78fe826ee411d79) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu13-.rom",     0x00000, 0x20000, CRC(b8525622) SHA1(4a6ec5e3f64256b1383bfbab4167cbd2ec11b5c5) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.18e",   0x0000, 0x0100, CRC(3645b70f) SHA1(7d3831867362037892b43efb007e27d3bd5f6488) )	/* Priority (not used) */
ROM_END

ROM_START( cbusterw )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
  	ROM_LOAD16_BYTE( "fu01-.rom", 0x00000, 0x20000, CRC(0203e0f8) SHA1(7709636429f2cab43caba3422122dba970dfb50b) )
	ROM_LOAD16_BYTE( "fu00-.rom", 0x00001, 0x20000, CRC(9c58626d) SHA1(6bc950929391221755972658258937a1ef96c244) )
  	ROM_LOAD16_BYTE( "fu03-.rom", 0x40000, 0x20000, CRC(def46956) SHA1(e1f71a440430f8f9351ee9e1826ca2d0d5a372f8) )
 	ROM_LOAD16_BYTE( "fu02-.rom", 0x40001, 0x20000, CRC(649c3338) SHA1(06373b364283706f0b00ab6d014c674e4b9818fa) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fu11-.rom",     0x00000, 0x10000, CRC(65f20f10) SHA1(cf914893edd98a0f39bbf7068a469ed7d34bd90e) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-00",        0x00000, 0x80000, CRC(660eaabd) SHA1(e3d614e13fdb9af159d9758a869d9dae3dbe14e0) ) /* Tiles */
	ROM_LOAD16_BYTE( "fu05-.rom",     0x80000, 0x10000, CRC(8134d412) SHA1(9c70ff6f9f24ec89c0bb4645afdf2a5ca27e9a0c) ) /* Chars */
	ROM_LOAD16_BYTE( "fu06-.rom",     0x80001, 0x10000, CRC(2f914a45) SHA1(bb44ba4779e45ee77ef0006363df91aac1f4559a) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-01",        0x00000, 0x80000, CRC(1080d619) SHA1(68f33a1580d33e4dd0858248c12a0a10ac117249) ) /* Tiles */

	ROM_REGION( 0x180000,REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-02",        0x000000, 0x80000, CRC(58b7231d) SHA1(5b51a2fa42c67f23648be205295184a1fddc00f5) ) /* Sprites */
	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "mab-03",        0x0a0000, 0x80000, CRC(76053b9d) SHA1(093cd01a13509701ec9dd1a806132600a5bd1915) )
 	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "fu07-.rom",     0x140000, 0x10000, CRC(ca8d0bb3) SHA1(9262d6003cf0cb8c33d0f6c1d0ef35490b29f9b4) ) /* Extra sprites */
	ROM_LOAD( "fu08-.rom",     0x150000, 0x10000, CRC(c6afc5c8) SHA1(feddd546f09884c51e4d1802477de4e152a51082) )
	ROM_LOAD( "fu09-.rom",     0x160000, 0x10000, CRC(526809ca) SHA1(2cb9e7417211c1eb23d32e3fee71c5254d34a3ff) )
	ROM_LOAD( "fu10-.rom",     0x170000, 0x10000, CRC(6be6d50e) SHA1(b944db4b3a7c76190f6b40f71f033e16e7964f6a) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu12-.rom",     0x00000, 0x20000, CRC(2d1d65f2) SHA1(be3d57b9976ddf7ee6d20ee9e78fe826ee411d79) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu13-.rom",     0x00000, 0x20000, CRC(b8525622) SHA1(4a6ec5e3f64256b1383bfbab4167cbd2ec11b5c5) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.18e",   0x0000, 0x0100, CRC(3645b70f) SHA1(7d3831867362037892b43efb007e27d3bd5f6488) )	/* Priority (not used) */
ROM_END

ROM_START( cbusterj )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
  	ROM_LOAD16_BYTE( "fr01-1",   0x00000, 0x20000, CRC(af3c014f) SHA1(a7724c48f73e52b19f3688a413e2ed013e226c6b) )
	ROM_LOAD16_BYTE( "fr00-1",   0x00001, 0x20000, CRC(f666ad52) SHA1(6f7325bc3bb79fd8112df677250c4bae572dfa43) )
  	ROM_LOAD16_BYTE( "fr03",     0x40000, 0x20000, CRC(02c06118) SHA1(a251f936f80d8a9af033fe6d0d42e1e17ebbbf98) )
 	ROM_LOAD16_BYTE( "fr02",     0x40001, 0x20000, CRC(b6c34332) SHA1(c1215c72a03b368655e20f4557475a2fc4c46c9e) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fu11-.rom",     0x00000, 0x10000, CRC(65f20f10) SHA1(cf914893edd98a0f39bbf7068a469ed7d34bd90e) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-00",        0x00000, 0x80000, CRC(660eaabd) SHA1(e3d614e13fdb9af159d9758a869d9dae3dbe14e0) ) /* Tiles */
	ROM_LOAD16_BYTE( "fu05-.rom",     0x80000, 0x10000, CRC(8134d412) SHA1(9c70ff6f9f24ec89c0bb4645afdf2a5ca27e9a0c) ) /* Chars */
	ROM_LOAD16_BYTE( "fu06-.rom",     0x80001, 0x10000, CRC(2f914a45) SHA1(bb44ba4779e45ee77ef0006363df91aac1f4559a) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-01",        0x00000, 0x80000, CRC(1080d619) SHA1(68f33a1580d33e4dd0858248c12a0a10ac117249) ) /* Tiles */

	ROM_REGION( 0x180000,REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-02",        0x000000, 0x80000, CRC(58b7231d) SHA1(5b51a2fa42c67f23648be205295184a1fddc00f5) ) /* Sprites */
	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "mab-03",        0x0a0000, 0x80000, CRC(76053b9d) SHA1(093cd01a13509701ec9dd1a806132600a5bd1915) )
 	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "fr07",          0x140000, 0x10000, CRC(52c85318) SHA1(74032dac7cb7e7d3028aab4c5f5b0a4e2a7caa03) ) /* Extra sprites */
	ROM_LOAD( "fr08",          0x150000, 0x10000, CRC(ea25fbac) SHA1(d00dce24e94ffc212ab3880c00fcadb7b2116f01) )
	ROM_LOAD( "fr09",          0x160000, 0x10000, CRC(f8363424) SHA1(6a6b143a3474965ef89f75e9d7b15946ae26d0d4) )
	ROM_LOAD( "fr10",          0x170000, 0x10000, CRC(241d5760) SHA1(cd216ecf7e88939b91a6e0f02a23c8b875ac24dc) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu12-.rom",     0x00000, 0x20000, CRC(2d1d65f2) SHA1(be3d57b9976ddf7ee6d20ee9e78fe826ee411d79) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu13-.rom",     0x00000, 0x20000, CRC(b8525622) SHA1(4a6ec5e3f64256b1383bfbab4167cbd2ec11b5c5) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.18e",   0x0000, 0x0100, CRC(3645b70f) SHA1(7d3831867362037892b43efb007e27d3bd5f6488) )	/* Priority (not used) */
ROM_END

ROM_START( twocrude )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ft01",     0x00000, 0x20000, CRC(08e96489) SHA1(1e75893cc086d6d6b428ca055851b51d0bc367aa) )
	ROM_LOAD16_BYTE( "ft00",     0x00001, 0x20000, CRC(6765c445) SHA1(b2bbb86414eafe32ed66f3f8ab095a2bce3a1a4b) )
	ROM_LOAD16_BYTE( "ft03",     0x40000, 0x20000, CRC(28002c99) SHA1(6397b05a1a237bb17657bee6c8185f61c60c6a2c) )
	ROM_LOAD16_BYTE( "ft02",     0x40001, 0x20000, CRC(37ea0626) SHA1(ec1822eda83829c599cad217b6d5dd34fb970101) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fu11-.rom",     0x00000, 0x10000, CRC(65f20f10) SHA1(cf914893edd98a0f39bbf7068a469ed7d34bd90e) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-00",        0x00000, 0x80000, CRC(660eaabd) SHA1(e3d614e13fdb9af159d9758a869d9dae3dbe14e0) ) /* Tiles */
	ROM_LOAD16_BYTE( "fu05-.rom",     0x80000, 0x10000, CRC(8134d412) SHA1(9c70ff6f9f24ec89c0bb4645afdf2a5ca27e9a0c) ) /* Chars */
	ROM_LOAD16_BYTE( "fu06-.rom",     0x80001, 0x10000, CRC(2f914a45) SHA1(bb44ba4779e45ee77ef0006363df91aac1f4559a) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-01",        0x00000, 0x80000, CRC(1080d619) SHA1(68f33a1580d33e4dd0858248c12a0a10ac117249) ) /* Tiles */

	ROM_REGION( 0x180000,REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "mab-02",        0x000000, 0x80000, CRC(58b7231d) SHA1(5b51a2fa42c67f23648be205295184a1fddc00f5) ) /* Sprites */
	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "mab-03",        0x0a0000, 0x80000, CRC(76053b9d) SHA1(093cd01a13509701ec9dd1a806132600a5bd1915) )
 	/* Space for extra sprites to be copied to (0x20000) */
	ROM_LOAD( "ft07",          0x140000, 0x10000, CRC(e3465c25) SHA1(5369a87847e6f881efc8460e6e8efcf8ff46e87f) )
	ROM_LOAD( "ft08",          0x150000, 0x10000, CRC(c7f1d565) SHA1(d5dc55cf879f7feaff166a6708d60ef0bf31ddf5) )
	ROM_LOAD( "ft09",          0x160000, 0x10000, CRC(6e3657b9) SHA1(7e6a140e33f9bc18e35c255680eebe152a5d8858) )
	ROM_LOAD( "ft10",          0x170000, 0x10000, CRC(cdb83560) SHA1(8b258c4436ccea5a74edff1b6219ab7a5eac0328) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu12-.rom",     0x00000, 0x20000, CRC(2d1d65f2) SHA1(be3d57b9976ddf7ee6d20ee9e78fe826ee411d79) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fu13-.rom",     0x00000, 0x20000, CRC(b8525622) SHA1(4a6ec5e3f64256b1383bfbab4167cbd2ec11b5c5) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.18e",   0x0000, 0x0100, CRC(3645b70f) SHA1(7d3831867362037892b43efb007e27d3bd5f6488) )	/* Priority (not used) */
ROM_END

/******************************************************************************/

static DRIVER_INIT( twocrude )
{
	unsigned char *RAM = memory_region(REGION_CPU1);
	unsigned char *PTR;
	int i,j;

	/* Main cpu decrypt */
	for (i=0x00000; i<0x80000; i+=2) {
#ifdef LSB_FIRST
		RAM[i+1]=(RAM[i+1] & 0xcf) | ((RAM[i+1] & 0x10) << 1) | ((RAM[i+1] & 0x20) >> 1);
		RAM[i+1]=(RAM[i+1] & 0x5f) | ((RAM[i+1] & 0x20) << 2) | ((RAM[i+1] & 0x80) >> 2);

		RAM[i]=(RAM[i] & 0xbd) | ((RAM[i] & 0x2) << 5) | ((RAM[i] & 0x40) >> 5);
		RAM[i]=(RAM[i] & 0xf5) | ((RAM[i] & 0x2) << 2) | ((RAM[i] & 0x8) >> 2);
#else
		RAM[i]=(RAM[i] & 0xcf) | ((RAM[i] & 0x10) << 1) | ((RAM[i] & 0x20) >> 1);
		RAM[i]=(RAM[i] & 0x5f) | ((RAM[i] & 0x20) << 2) | ((RAM[i] & 0x80) >> 2);

		RAM[i+1]=(RAM[i+1] & 0xbd) | ((RAM[i+1] & 0x2) << 5) | ((RAM[i+1] & 0x40) >> 5);
		RAM[i+1]=(RAM[i+1] & 0xf5) | ((RAM[i+1] & 0x2) << 2) | ((RAM[i+1] & 0x8) >> 2);
#endif
	}

	/* Rearrange the 'extra' sprite bank to be in the same format as main sprites */
	RAM = memory_region(REGION_GFX3) + 0x080000;
	PTR = memory_region(REGION_GFX3) + 0x140000;
	for (i=0; i<0x20000; i+=64) {
		for (j=0; j<16; j+=1) { /* Copy 16 lines down */
			RAM[i+      0+j*2]=PTR[i/2+      0+j]; /* Pixels 0-7 for each plane */
			RAM[i+      1+j*2]=PTR[i/2+0x10000+j];
			RAM[i+0xa0000+j*2]=PTR[i/2+0x20000+j];
			RAM[i+0xa0001+j*2]=PTR[i/2+0x30000+j];
		}

		for (j=0; j<16; j+=1) { /* Copy 16 lines down */
			RAM[i+   0x20+j*2]=PTR[i/2+   0x10+j]; /* Pixels 8-15 for each plane */
			RAM[i+   0x21+j*2]=PTR[i/2+0x10010+j];
			RAM[i+0xa0020+j*2]=PTR[i/2+0x20010+j];
			RAM[i+0xa0021+j*2]=PTR[i/2+0x30010+j];
		}
	}
}

/******************************************************************************/

GAME( 1990, cbuster,  0,       twocrude, twocrude, twocrude, ROT0, "Data East Corporation", "Crude Buster (World FX version)", 0 ,2)
GAME( 1990, cbusterw, cbuster, twocrude, twocrude, twocrude, ROT0, "Data East Corporation", "Crude Buster (World FU version)", 0 ,2)
GAME( 1990, cbusterj, cbuster, twocrude, twocrude, twocrude, ROT0, "Data East Corporation", "Crude Buster (Japan)", 0 ,2)
GAME( 1990, twocrude, cbuster, twocrude, twocrude, twocrude, ROT0, "Data East USA", "Two Crude (US)", 0 ,2)
