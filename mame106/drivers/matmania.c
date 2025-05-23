/***************************************************************************

Mat Mania
Memetron, 1985
(copyright Taito, licensed by Technos, distributed by Memetron)

driver by Brad Oliver

MAIN BOARD:

0000-0fff RAM
1000-13ff Video RAM
1400-17ff Attribute RAM
1800-1fff ?? Only used in self-test ??
2000-21ff Background video RAM #1
2200-23ff Background attribute RAM #1
2400-25ff Background video RAM #2
2600-27ff Background attribute RAM #2
4000-ffff ROM

***************************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "cpu/m6809/m6809.h"
#include "sound/ay8910.h"
#include "sound/dac.h"
#include "sound/3812intf.h"

extern unsigned char *matmania_videoram2,*matmania_colorram2;
extern size_t matmania_videoram2_size;
extern unsigned char *matmania_videoram3,*matmania_colorram3;
extern size_t matmania_videoram3_size;
extern unsigned char *matmania_scroll;
extern unsigned char *matmania_pageselect;

WRITE8_HANDLER( matmania_paletteram_w );
PALETTE_INIT( matmania );
VIDEO_UPDATE( matmania );
VIDEO_UPDATE( maniach );
WRITE8_HANDLER( matmania_videoram3_w );
WRITE8_HANDLER( matmania_colorram3_w );
VIDEO_START( matmania );
VIDEO_UPDATE( matmania );

READ8_HANDLER( maniach_68705_portA_r );
WRITE8_HANDLER( maniach_68705_portA_w );
READ8_HANDLER( maniach_68705_portB_r );
WRITE8_HANDLER( maniach_68705_portB_w );
READ8_HANDLER( maniach_68705_portC_r );
WRITE8_HANDLER( maniach_68705_portC_w );
WRITE8_HANDLER( maniach_68705_ddrA_w );
WRITE8_HANDLER( maniach_68705_ddrB_w );
WRITE8_HANDLER( maniach_68705_ddrC_w );
WRITE8_HANDLER( maniach_mcu_w );
READ8_HANDLER( maniach_mcu_r );
READ8_HANDLER( maniach_mcu_status_r );



WRITE8_HANDLER( matmania_sh_command_w )
{
	soundlatch_w(offset,data);
	cpunum_set_input_line(1,M6502_IRQ_LINE,HOLD_LINE);
}

WRITE8_HANDLER( matmania_dac_w )
{
	DAC_signed_data_w(0,data);
}


WRITE8_HANDLER( maniach_sh_command_w )
{
	soundlatch_w(offset,data);
	cpunum_set_input_line(1,M6809_IRQ_LINE,HOLD_LINE);
}



static ADDRESS_MAP_START( matmania_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x077f) AM_READ(MRA8_RAM)
	AM_RANGE(0x1000, 0x17ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2000, 0x27ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x3000) AM_READ(input_port_0_r)
	AM_RANGE(0x3010, 0x3010) AM_READ(input_port_1_r)
	AM_RANGE(0x3020, 0x3020) AM_READ(input_port_2_r)
	AM_RANGE(0x3030, 0x3030) AM_READ(input_port_3_r)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( matmania_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x077f) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x0780, 0x07df) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x1000, 0x13ff) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_videoram2) AM_SIZE(&matmania_videoram2_size)
	AM_RANGE(0x1400, 0x17ff) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_colorram2)
	AM_RANGE(0x2000, 0x21ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2200, 0x23ff) AM_WRITE(colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x2400, 0x25ff) AM_WRITE(matmania_videoram3_w) AM_BASE(&matmania_videoram3) AM_SIZE(&matmania_videoram3_size)
	AM_RANGE(0x2600, 0x27ff) AM_WRITE(matmania_colorram3_w) AM_BASE(&matmania_colorram3)
	AM_RANGE(0x3000, 0x3000) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_pageselect)
	AM_RANGE(0x3010, 0x3010) AM_WRITE(matmania_sh_command_w)
	AM_RANGE(0x3020, 0x3020) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_scroll)
//  AM_RANGE(0x3030, 0x3030) AM_WRITE(MWA8_NOP) /* ?? */
	AM_RANGE(0x3050, 0x307f) AM_WRITE(matmania_paletteram_w) AM_BASE(&paletteram)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( maniach_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x077f) AM_READ(MRA8_RAM)
	AM_RANGE(0x1000, 0x17ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2000, 0x27ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x3000) AM_READ(input_port_0_r)
	AM_RANGE(0x3010, 0x3010) AM_READ(input_port_1_r)
	AM_RANGE(0x3020, 0x3020) AM_READ(input_port_2_r)
	AM_RANGE(0x3030, 0x3030) AM_READ(input_port_3_r)
	AM_RANGE(0x3040, 0x3040) AM_READ(maniach_mcu_r)
	AM_RANGE(0x3041, 0x3041) AM_READ(maniach_mcu_status_r)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( maniach_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x077f) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x0780, 0x07df) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x1000, 0x13ff) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_videoram2) AM_SIZE(&matmania_videoram2_size)
	AM_RANGE(0x1400, 0x17ff) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_colorram2)
	AM_RANGE(0x2000, 0x21ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2200, 0x23ff) AM_WRITE(colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x2400, 0x25ff) AM_WRITE(matmania_videoram3_w) AM_BASE(&matmania_videoram3) AM_SIZE(&matmania_videoram3_size)
	AM_RANGE(0x2600, 0x27ff) AM_WRITE(matmania_colorram3_w) AM_BASE(&matmania_colorram3)
	AM_RANGE(0x3000, 0x3000) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_pageselect)
	AM_RANGE(0x3010, 0x3010) AM_WRITE(maniach_sh_command_w)
	AM_RANGE(0x3020, 0x3020) AM_WRITE(MWA8_RAM) AM_BASE(&matmania_scroll)
	AM_RANGE(0x3030, 0x3030) AM_WRITE(MWA8_NOP)	/* ?? */
	AM_RANGE(0x3040, 0x3040) AM_WRITE(maniach_mcu_w)
	AM_RANGE(0x3050, 0x307f) AM_WRITE(matmania_paletteram_w) AM_BASE(&paletteram)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x01ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2007, 0x2007) AM_READ(soundlatch_r)
	AM_RANGE(0x8000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x01ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x2001, 0x2001) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x2002, 0x2002) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x2003, 0x2003) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x2004, 0x2004) AM_WRITE(matmania_dac_w)
	AM_RANGE(0x8000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( maniach_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x2004, 0x2004) AM_READ(soundlatch_r)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( maniach_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(YM3526_control_port_0_w)
	AM_RANGE(0x2001, 0x2001) AM_WRITE(YM3526_write_port_0_w)
	AM_RANGE(0x2002, 0x2002) AM_WRITE(matmania_dac_w)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( mcu_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(11) )
	AM_RANGE(0x0000, 0x0000) AM_READ(maniach_68705_portA_r)
	AM_RANGE(0x0001, 0x0001) AM_READ(maniach_68705_portB_r)
	AM_RANGE(0x0002, 0x0002) AM_READ(maniach_68705_portC_r)
	AM_RANGE(0x0010, 0x007f) AM_READ(MRA8_RAM)
	AM_RANGE(0x0080, 0x07ff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( mcu_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(11) )
	AM_RANGE(0x0000, 0x0000) AM_WRITE(maniach_68705_portA_w)
	AM_RANGE(0x0001, 0x0001) AM_WRITE(maniach_68705_portB_w)
	AM_RANGE(0x0002, 0x0002) AM_WRITE(maniach_68705_portC_w)
	AM_RANGE(0x0004, 0x0004) AM_WRITE(maniach_68705_ddrA_w)
	AM_RANGE(0x0005, 0x0005) AM_WRITE(maniach_68705_ddrB_w)
	AM_RANGE(0x0006, 0x0006) AM_WRITE(maniach_68705_ddrC_w)
	AM_RANGE(0x0010, 0x007f) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x0080, 0x07ff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END



INPUT_PORTS_START( matmania )
	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_IMPULSE(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_IMPULSE(1)

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* DSW1 */
	PORT_DIPNAME(0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(   0x03, DEF_STR( Easy ) )
	PORT_DIPSETTING(   0x02, DEF_STR( Medium ) )
	PORT_DIPSETTING(   0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(   0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME(0x0c, 0x0c, "Tournament Time" )    // Tournament time is always 3:00, but time per 1 second is shorter.
	PORT_DIPSETTING(   0x00, "2:12" )
	PORT_DIPSETTING(   0x04, "2:24" )
	PORT_DIPSETTING(   0x08, "2:30" )
	PORT_DIPSETTING(   0x0c, "2:36" )
	PORT_DIPNAME(0x10, 0x10, DEF_STR( Unused ) )
	PORT_DIPSETTING(   0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(   0x00, DEF_STR( On ) )
	PORT_DIPNAME(0x20, 0x20, DEF_STR( Unused ) )
	PORT_DIPSETTING(   0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(   0x00, DEF_STR( On ) )
	PORT_DIPNAME(0x40, 0x40, DEF_STR( Unused ) )
	PORT_DIPSETTING(   0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(   0x00, DEF_STR( On ) )
	PORT_DIPNAME(0x80, 0x80, DEF_STR( Unused ) )
	PORT_DIPSETTING(   0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(   0x00, DEF_STR( On ) )

	PORT_START	/* DSW2 */
	PORT_DIPNAME(0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(   0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(   0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(   0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(   0x01, DEF_STR( 1C_3C ) )
	PORT_DIPNAME(0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(   0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(   0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(   0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(   0x04, DEF_STR( 1C_3C ) )
	PORT_DIPNAME(0x10, 0x10, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(   0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(   0x10, DEF_STR( On ) )
	PORT_DIPNAME(0x20, 0x00, DEF_STR( Cabinet ) )    // The default setting should be cocktail.
	PORT_DIPSETTING(   0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(   0x20, DEF_STR( Cocktail ) )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	1024,	/* 1024 characters */
	3,	/* 3 bits per pixel */
	{ 2*1024*8*8, 1024*8*8, 0 },	/* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8	/* every char takes 8 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	16,16,  /* 16*16 tiles */
	512,    /* 512 tiles */
	3,	/* 3 bits per pixel */
	{ 2*512*16*16, 512*16*16, 0 },	/* the bitplanes are separated */
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8	/* every tile takes 16 consecutive bytes */
};

static const gfx_layout matmania_spritelayout =
{
	16,16,  /* 16*16 sprites */
	3584,    /* 3584 sprites */
	3,	/* 3 bits per pixel */
	{ 2*3584*16*16, 3584*16*16, 0 },	/* the bitplanes are separated */
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8	/* every sprite takes 16 consecutive bytes */
};

static const gfx_layout maniach_spritelayout =
{
	16,16,  /* 16*16 sprites */
	3584,    /* 3584 sprites */
	3,	/* 3 bits per pixel */
	{ 0, 3584*16*16, 2*3584*16*16 },	/* the bitplanes are separated */
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8	/* every sprite takes 16 consecutive bytes */
};

static const gfx_layout maniach_tilelayout =
{
	16,16,  /* 16*16 tiles */
	1024,    /* 1024 tiles */
	3,	/* 3 bits per pixel */
	{ 2*1024*16*16, 1024*16*16, 0 },	/* the bitplanes are separated */
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8	/* every tile takes 16 consecutive bytes */
};

static const gfx_decode matmania_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,              0, 4 },
	{ REGION_GFX2, 0, &tilelayout,            4*8, 4 },
	{ REGION_GFX3, 0, &matmania_spritelayout, 8*8, 2 },
	{ -1 } /* end of array */
};

static const gfx_decode maniach_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,             0, 4 },
	{ REGION_GFX2, 0, &maniach_tilelayout,   4*8, 4 },
	{ REGION_GFX3, 0, &maniach_spritelayout, 8*8, 2 },
	{ -1 } /* end of array */
};



static MACHINE_DRIVER_START( matmania )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1500000)	/* 1.5 MHz ???? */
	MDRV_CPU_PROGRAM_MAP(matmania_readmem,matmania_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(M6502, 1200000)
	/* audio CPU */	/* 1.2 MHz ???? */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,15)	/* ???? */
								/* IRQs are caused by the main CPU */
	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(10)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(matmania_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64+16)
	MDRV_COLORTABLE_LENGTH(64+16)

	MDRV_PALETTE_INIT(matmania)
	MDRV_VIDEO_START(matmania)
	MDRV_VIDEO_UPDATE(matmania)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)
MACHINE_DRIVER_END



/* handler called by the 3526 emulator when the internal timers cause an IRQ */
static void irqhandler(int linestate)
{
	cpunum_set_input_line(1,1,linestate);
}

static struct YM3526interface ym3526_interface =
{
	irqhandler
};


static MACHINE_DRIVER_START( maniach )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1500000)	/* 1.5 MHz ???? */
	MDRV_CPU_PROGRAM_MAP(maniach_readmem,maniach_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(M6809, 1500000)
	/* audio CPU */	/* 1.5 MHz ???? */
	MDRV_CPU_PROGRAM_MAP(maniach_sound_readmem,maniach_sound_writemem)
								/* IRQs are caused by the main CPU */
	MDRV_CPU_ADD(M68705, 500000)	/* .5 MHz (don't know really how fast, but it doesn't need to even be this fast) */
	MDRV_CPU_PROGRAM_MAP(mcu_readmem,mcu_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(100)	/* 100 CPU slice per frame - high interleaving to sync main and mcu */

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(maniach_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64+16)
	MDRV_COLORTABLE_LENGTH(64+16)

	MDRV_PALETTE_INIT(matmania)
	MDRV_VIDEO_START(matmania)
	MDRV_VIDEO_UPDATE(maniach)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3526, 3600000)
	MDRV_SOUND_CONFIG(ym3526_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)
MACHINE_DRIVER_END

/***************************************************************************

  Mat Mania driver

***************************************************************************/

ROM_START( matmania )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "k0-03",        0x4000, 0x4000, CRC(314ab8a4) SHA1(dc86b2f71a9af8524edad2317343b0d05fe5ef4a) )
	ROM_LOAD( "k1-03",        0x8000, 0x4000, CRC(3b3c3f08) SHA1(65f0c5dba0b8eeb5c2d42b050cac37c475e6a398) )
	ROM_LOAD( "k2-03",        0xc000, 0x4000, CRC(286c0917) SHA1(50d6133406e7db0694b02858c7d06725744cf243) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for audio code */
	ROM_LOAD( "k4-0",         0x8000, 0x4000, CRC(86dab489) SHA1(27f6eea29b0287e461e0e321fd7bfaada52c39dc) )
	ROM_LOAD( "k5-0",         0xc000, 0x4000, CRC(4c41cdba) SHA1(a0af0c019bd6d9456cbbe83ecdeee689bc5f1bea) )

	ROM_REGION( 0x06000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ku-02",        0x00000, 0x2000, CRC(613c8698) SHA1(07acb2fe150a64029fd15d177c8b6481fcd9eb0b) )	/* Character ROMs - 1024 chars, 3 bpp */
	ROM_LOAD( "kv-02",        0x02000, 0x2000, CRC(274ce14b) SHA1(58ed8c8fe0cc157d642aae596e41f2099c1ea6b1) )
	ROM_LOAD( "kw-02",        0x04000, 0x2000, CRC(7588a9c4) SHA1(0c197a8fea1acb6c9a99071845be54c949ec83b1) )

	ROM_REGION( 0x0c000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "kt-02",        0x00000, 0x4000, CRC(5d817c70) SHA1(f7759be40a8850d325440d336241ecd05b80c0bd) )	/* tile set */
	ROM_LOAD( "ks-02",        0x04000, 0x4000, CRC(2e9f3ba0) SHA1(21d6686580de6ecfe57e458821fa92e966a42d95) )
	ROM_LOAD( "kr-02",        0x08000, 0x4000, CRC(b057d3e3) SHA1(24216b22a69c1ecc7eabd7ae10de381e1ff0afc1) )

	ROM_REGION( 0x54000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "k6-00",        0x00000, 0x4000, CRC(294d0878) SHA1(0aaae97e35d504dbf6c479ddf04b981847a23ea6) )	/* sprites */
	ROM_LOAD( "k7-00",        0x04000, 0x4000, CRC(0908c2f5) SHA1(acc34c578f9a3521855ad4dd8fbd554e05c3f63c) )
	ROM_LOAD( "k8-00",        0x08000, 0x4000, CRC(ae8341e1) SHA1(ca198087b3aec320543a19921015861324ace8a2) )
	ROM_LOAD( "k9-00",        0x0c000, 0x4000, CRC(752ac2c6) SHA1(309fe4e396616b569b9b25654e3dc2751d7b1605) )
	ROM_LOAD( "ka-00",        0x10000, 0x4000, CRC(46a9cb16) SHA1(35e6bd4f33098c98bf2d0b1dfefec2f9d25444e7) )
	ROM_LOAD( "kb-00",        0x14000, 0x4000, CRC(bf016772) SHA1(c901fc2d553622b6dbfaaa9cd94759799d974c39) )
	ROM_LOAD( "kc-00",        0x18000, 0x4000, CRC(8d08bce7) SHA1(1433962c837f568cc1eb27464e243dc580a141de) )
	ROM_LOAD( "kd-00",        0x1c000, 0x4000, CRC(af1d6a60) SHA1(ae3131e3e1fcc9bb1d59db6b1668f6838849241d) )
	ROM_LOAD( "ke-00",        0x20000, 0x4000, CRC(614f19b0) SHA1(67e4687b9be36007c2e1fd504a2eb952fe098d53) )
	ROM_LOAD( "kf-00",        0x24000, 0x4000, CRC(bdf58c18) SHA1(a76c6984e4d4f88384e15d0b6b74093c3bc0fcda) )
	ROM_LOAD( "kg-00",        0x28000, 0x4000, CRC(2189f5cf) SHA1(48289263f7b9cc5b6d975742d45dd64ba45e38c8) )
	ROM_LOAD( "kh-00",        0x2c000, 0x4000, CRC(6b11ed1f) SHA1(8b5c52a14ac3f80ebf630fed8108df17106efd93) )
	ROM_LOAD( "ki-00",        0x30000, 0x4000, CRC(d7ac4ec5) SHA1(35b1503147cb521d2fcc756e6f90ef70d62e2d04) )
	ROM_LOAD( "kj-00",        0x34000, 0x4000, CRC(2caee05d) SHA1(51e0799312e4737bc6f6ae7b74d02f9e10f91c3b) )
	ROM_LOAD( "kk-00",        0x38000, 0x4000, CRC(eb54f010) SHA1(9ed8addd8a542299be2a8f0108447e68b9b33436) )
	ROM_LOAD( "kl-00",        0x3c000, 0x4000, CRC(fa4c7e0c) SHA1(365f5b60ac880928b49a254a5a49a9e9a766046d) )
	ROM_LOAD( "km-00",        0x40000, 0x4000, CRC(6d2369b6) SHA1(b3071cc27598045167681a00f41bf77b6d4bd5bd) )
	ROM_LOAD( "kn-00",        0x44000, 0x4000, CRC(c55733e2) SHA1(b550afd2ceb3b0159c11627ab31f49cc49785809) )
	ROM_LOAD( "ko-00",        0x48000, 0x4000, CRC(ed3c3476) SHA1(eb7bc7c72443d4e3bdfc535bfe460524c0f900d3) )
	ROM_LOAD( "kp-00",        0x4c000, 0x4000, CRC(9c84a969) SHA1(8492ba523e1c1ca94eeba1e53521dd74df854cb9) )
	ROM_LOAD( "kq-00",        0x50000, 0x4000, CRC(fa2f0003) SHA1(7327ce822be8aea360210bbd466a8129788a65c3) )

	ROM_REGION( 0x0080, REGION_PROMS, 0 )
	ROM_LOAD( "matmania.1",   0x0000, 0x0020, CRC(1b58f01f) SHA1(ffc098d85413777740a25c767096ba5b2aeaf5a8) ) /* char palette red and green components */
	ROM_LOAD( "matmania.5",   0x0020, 0x0020, CRC(2029f85f) SHA1(7825d42eed284ea0fe7fd60304b8a27a1b5a4075) ) /* tile palette red and green components */
	ROM_LOAD( "matmania.2",   0x0040, 0x0020, CRC(b6ac1fd5) SHA1(e312a8ff7317eb21320308400539a733c27e8fca) ) /* char palette blue component */
	ROM_LOAD( "matmania.16",  0x0060, 0x0020, CRC(09325dc2) SHA1(3d9ebdf73840a9603af2acc4bcc4339f3029d284) ) /* tile palette blue component */
ROM_END

ROM_START( excthour )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "e29",          0x04000, 0x4000, CRC(c453e855) SHA1(52ce83042ec04e7ba5b45a61316d6281cb10170a) )
	ROM_LOAD( "e28",          0x08000, 0x4000, CRC(17b63708) SHA1(01c868b7ea32c4857f7187ce73a4cab5b4def246) )
	ROM_LOAD( "e27",          0x0c000, 0x4000, CRC(269ab3bc) SHA1(f2f307c5fc6d50167be8904bef8c7ef21209be50) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for audio code */
	ROM_LOAD( "k4-0",         0x8000, 0x4000, CRC(86dab489) SHA1(27f6eea29b0287e461e0e321fd7bfaada52c39dc) )
	ROM_LOAD( "k5-0",         0xc000, 0x4000, CRC(4c41cdba) SHA1(a0af0c019bd6d9456cbbe83ecdeee689bc5f1bea) )

	ROM_REGION( 0x06000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "e30",          0x00000, 0x2000, CRC(b2875329) SHA1(b37a8b95eb09f1ddc422cc981184b3ea40a5730d) )	/* Character ROMs - 1024 chars, 3 bpp */
	ROM_LOAD( "e31",          0x02000, 0x2000, CRC(c9506de8) SHA1(1036f9acd8b391c03e6408fe1db3406e105373d9) )
	ROM_LOAD( "e32",          0x04000, 0x2000, CRC(00d1635f) SHA1(3a7a20ff949d333ec4d3c0287d73e15dcfefdc71) )

	ROM_REGION( 0x0c000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "e5",           0x00000, 0x4000, CRC(0604dc55) SHA1(dc4e36dac1a820d4e649132206a8b16603d08192) )	/* tile set */
	ROM_LOAD( "ks-02",        0x04000, 0x4000, CRC(2e9f3ba0) SHA1(21d6686580de6ecfe57e458821fa92e966a42d95) )
	ROM_LOAD( "e3",           0x08000, 0x4000, CRC(ebd273c6) SHA1(415f68ee10499583f5557aae6a41b5499013b5d2) )

	ROM_REGION( 0x54000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "k6-00",        0x00000, 0x4000, CRC(294d0878) SHA1(0aaae97e35d504dbf6c479ddf04b981847a23ea6) )	/* sprites */
	ROM_LOAD( "k7-00",        0x04000, 0x4000, CRC(0908c2f5) SHA1(acc34c578f9a3521855ad4dd8fbd554e05c3f63c) )
	ROM_LOAD( "k8-00",        0x08000, 0x4000, CRC(ae8341e1) SHA1(ca198087b3aec320543a19921015861324ace8a2) )
	ROM_LOAD( "k9-00",        0x0c000, 0x4000, CRC(752ac2c6) SHA1(309fe4e396616b569b9b25654e3dc2751d7b1605) )
	ROM_LOAD( "ka-00",        0x10000, 0x4000, CRC(46a9cb16) SHA1(35e6bd4f33098c98bf2d0b1dfefec2f9d25444e7) )
	ROM_LOAD( "kb-00",        0x14000, 0x4000, CRC(bf016772) SHA1(c901fc2d553622b6dbfaaa9cd94759799d974c39) )
	ROM_LOAD( "kc-00",        0x18000, 0x4000, CRC(8d08bce7) SHA1(1433962c837f568cc1eb27464e243dc580a141de) )
	ROM_LOAD( "kd-00",        0x1c000, 0x4000, CRC(af1d6a60) SHA1(ae3131e3e1fcc9bb1d59db6b1668f6838849241d) )
	ROM_LOAD( "ke-00",        0x20000, 0x4000, CRC(614f19b0) SHA1(67e4687b9be36007c2e1fd504a2eb952fe098d53) )
	ROM_LOAD( "kf-00",        0x24000, 0x4000, CRC(bdf58c18) SHA1(a76c6984e4d4f88384e15d0b6b74093c3bc0fcda) )
	ROM_LOAD( "kg-00",        0x28000, 0x4000, CRC(2189f5cf) SHA1(48289263f7b9cc5b6d975742d45dd64ba45e38c8) )
	ROM_LOAD( "kh-00",        0x2c000, 0x4000, CRC(6b11ed1f) SHA1(8b5c52a14ac3f80ebf630fed8108df17106efd93) )
	ROM_LOAD( "ki-00",        0x30000, 0x4000, CRC(d7ac4ec5) SHA1(35b1503147cb521d2fcc756e6f90ef70d62e2d04) )
	ROM_LOAD( "kj-00",        0x34000, 0x4000, CRC(2caee05d) SHA1(51e0799312e4737bc6f6ae7b74d02f9e10f91c3b) )
	ROM_LOAD( "kk-00",        0x38000, 0x4000, CRC(eb54f010) SHA1(9ed8addd8a542299be2a8f0108447e68b9b33436) )
	ROM_LOAD( "kl-00",        0x3c000, 0x4000, CRC(fa4c7e0c) SHA1(365f5b60ac880928b49a254a5a49a9e9a766046d) )
	ROM_LOAD( "km-00",        0x40000, 0x4000, CRC(6d2369b6) SHA1(b3071cc27598045167681a00f41bf77b6d4bd5bd) )
	ROM_LOAD( "kn-00",        0x44000, 0x4000, CRC(c55733e2) SHA1(b550afd2ceb3b0159c11627ab31f49cc49785809) )
	ROM_LOAD( "ko-00",        0x48000, 0x4000, CRC(ed3c3476) SHA1(eb7bc7c72443d4e3bdfc535bfe460524c0f900d3) )
	ROM_LOAD( "kp-00",        0x4c000, 0x4000, CRC(9c84a969) SHA1(8492ba523e1c1ca94eeba1e53521dd74df854cb9) )
	ROM_LOAD( "kq-00",        0x50000, 0x4000, CRC(fa2f0003) SHA1(7327ce822be8aea360210bbd466a8129788a65c3) )

	ROM_REGION( 0x0080, REGION_PROMS, 0 )
	ROM_LOAD( "matmania.1",   0x0000, 0x0020, CRC(1b58f01f) SHA1(ffc098d85413777740a25c767096ba5b2aeaf5a8) ) /* char palette red and green components */
	ROM_LOAD( "matmania.5",   0x0020, 0x0020, CRC(2029f85f) SHA1(7825d42eed284ea0fe7fd60304b8a27a1b5a4075) ) /* tile palette red and green components */
	ROM_LOAD( "matmania.2",   0x0040, 0x0020, CRC(b6ac1fd5) SHA1(e312a8ff7317eb21320308400539a733c27e8fca) ) /* char palette blue component */
	ROM_LOAD( "matmania.16",  0x0060, 0x0020, CRC(09325dc2) SHA1(3d9ebdf73840a9603af2acc4bcc4339f3029d284) ) /* tile palette blue component */
ROM_END

ROM_START( maniach )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "mc-mb2.bin",   0x04000, 0x4000, CRC(a6da1ba8) SHA1(d861c1c68b25272205939df43cdcca423ba7c937) )
	ROM_LOAD( "mc-ma2.bin",   0x08000, 0x4000, CRC(84583323) SHA1(f1512fec6f3e03dc633a96917a114b0b6369c577) )
	ROM_LOAD( "mc-m92.bin",   0x0c000, 0x4000, CRC(e209a500) SHA1(d1a3ab91ffbc321a51c99a2170aca3e217b22576) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for audio code */
	ROM_LOAD( "mc-m50.bin",   0x4000, 0x4000, CRC(ba415d68) SHA1(484af7a1f109cc9546f17d19b53d284c934705db) )
	ROM_LOAD( "mc-m40.bin",   0x8000, 0x4000, CRC(2a217ed0) SHA1(b06f7c9a2c96ffe78a7065e5edadfdbf985305a5) )
	ROM_LOAD( "mc-m30.bin",   0xc000, 0x4000, CRC(95af1723) SHA1(691ca3f7400d10897e805ff691c904fb2d5bb53a) )

	ROM_REGION( 0x0800, REGION_CPU3, 0 )	/* 8k for the microcontroller */
	ROM_LOAD( "01",           0x0000, 0x0800, CRC(00c7f80c) SHA1(d2216f660eb8310b1530fa5dc844d26ba90c5e9c) )

	ROM_REGION( 0x06000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mc-m60.bin",   0x00000, 0x2000, CRC(1cdbb117) SHA1(cce99c7380fa2a7ae070c7e2d64866866c976085) )	/* Character ROMs - 1024 chars, 3 bpp */
	ROM_LOAD( "mc-m70.bin",   0x02000, 0x2000, CRC(553f0780) SHA1(eacce92ae7b872a35f289f79b33383f5442082d5) )
	ROM_LOAD( "mc-m80.bin",   0x04000, 0x2000, CRC(9392ecb7) SHA1(fb4be39fc2f1c826b146bb5b4dd10eb56b23c300) )

	ROM_REGION( 0x18000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mc-m01.bin",   0x00000, 0x8000, CRC(da558e4d) SHA1(0635f4cded061b0b3649ed1497f087ecd53d54a3) )	/* tile set */
	ROM_LOAD( "mc-m10.bin",   0x08000, 0x8000, CRC(619a02f8) SHA1(18de76277c263c76b8d8d9093b3c1aebbf2b7ae4) )
	ROM_LOAD( "mc-m20.bin",   0x10000, 0x8000, CRC(a617c6c1) SHA1(dccae543daa9987f2778327145fc785472f41228) )

	ROM_REGION( 0x54000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "mc-mc0.bin",   0x00000, 0x4000, CRC(133d644f) SHA1(5378e0cb665c0aa65d7ad76c3f7c04a3bc301f64) )	/* sprites */
	ROM_LOAD( "mc-md0.bin",   0x04000, 0x4000, CRC(e387b036) SHA1(828a42789d9ced9f9fcdfd08a43530008dcbbf2f) )
	ROM_LOAD( "mc-me0.bin",   0x08000, 0x4000, CRC(b36b1283) SHA1(9d12ea9f7a0f12aad532c0f2d3608cf4a86933a6) )
	ROM_LOAD( "mc-mf0.bin",   0x0c000, 0x4000, CRC(2584d8a9) SHA1(f24b4cb827421cd51cb35b581622c41646f3f4d8) )
	ROM_LOAD( "mc-mg0.bin",   0x10000, 0x4000, CRC(cf31a714) SHA1(9740f36e279fc4404112145abb4ff4d138f46474) )
	ROM_LOAD( "mc-mh0.bin",   0x14000, 0x4000, CRC(6292d589) SHA1(a0bf50ebc7712d9bea082834025d3bf816e5afa6) )
	ROM_LOAD( "mc-mi0.bin",   0x18000, 0x4000, CRC(ee2e06e3) SHA1(fbe0457322f5ae03e7eb46b27f044a46f3ee36b5) )
	ROM_LOAD( "mc-mj0.bin",   0x1c000, 0x4000, CRC(7e73895b) SHA1(76861f16f6845e53b80564eb869cfc6767040aa5) )
	ROM_LOAD( "mc-mk0.bin",   0x20000, 0x4000, CRC(66c8bf75) SHA1(071c8635c0264397363d6a33297fd1404f0531a9) )
	ROM_LOAD( "mc-ml0.bin",   0x24000, 0x4000, CRC(88138a1d) SHA1(a669620d51495734f0270fe49c75663cc54e2e50) )
	ROM_LOAD( "mc-mm0.bin",   0x28000, 0x4000, CRC(a1a4260d) SHA1(fbeeac9929f0b273deb1add39db228d3a6d74e76) )
	ROM_LOAD( "mc-mn0.bin",   0x2c000, 0x4000, CRC(6bc61b58) SHA1(dd5e1f8e7299358c6684cb99c4b47b1cf1dc64a7) )
	ROM_LOAD( "mc-mo0.bin",   0x30000, 0x4000, CRC(f96ef600) SHA1(f739ae6c45b2d46a587f8ce32d7626669225ad57) )
	ROM_LOAD( "mc-mp0.bin",   0x34000, 0x4000, CRC(1259618e) SHA1(9f3169675f7add038746edae2ab83fc0a7746db6) )
	ROM_LOAD( "mc-mq0.bin",   0x38000, 0x4000, CRC(102a1666) SHA1(ab052e76a3cef68dd199b5ecf01b73a8abaa32a7) )
	ROM_LOAD( "mc-mr0.bin",   0x3c000, 0x4000, CRC(1e854453) SHA1(41d4997361132c63fcd52dba23885a10ae34bf82) )
	ROM_LOAD( "mc-ms0.bin",   0x40000, 0x4000, CRC(7bc9d878) SHA1(72689ec3263e179f76c7139ed4a82684781d7bb6) )
	ROM_LOAD( "mc-mt0.bin",   0x44000, 0x4000, CRC(09cea985) SHA1(805c58bf73ea19329aa2c8a88a0c35cfaceca985) )
	ROM_LOAD( "mc-mu0.bin",   0x48000, 0x4000, CRC(5421769e) SHA1(c662c53711acf28754a60aedb4637d7d528dc5ea) )
	ROM_LOAD( "mc-mv0.bin",   0x4c000, 0x4000, CRC(36fc3e2d) SHA1(6cd358f29536ff6d5087570cb3e26fd9e971b888) )
	ROM_LOAD( "mc-mw0.bin",   0x50000, 0x4000, CRC(135dce4c) SHA1(3e64a52400137d87b60adf9c307656eadbfe709c) )

	ROM_REGION( 0x0080, REGION_PROMS, 0 )
	ROM_LOAD( "prom.2",       0x0000, 0x0020, CRC(32db2cf4) SHA1(854b3226a4843a6db94c01c6571294f17a469acf) ) /* char palette red and green components */
	ROM_LOAD( "prom.16",      0x0020, 0x0020, CRC(18836d26) SHA1(950e1ea5184355501b41548d40732b96c5516fd7) ) /* tile palette red and green components */
	ROM_LOAD( "prom.3",       0x0040, 0x0020, CRC(c7925311) SHA1(6b997803eb630b79886cebbe3bc49db1c1ab3fd9) ) /* char palette blue component */
	ROM_LOAD( "prom.17",      0x0060, 0x0020, CRC(41f51d49) SHA1(7cfaf308752cbfddf5a37a31140119afc3febaa7) ) /* tile palette blue component */
ROM_END

ROM_START( maniach2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "ic40-mb1",     0x04000, 0x4000, CRC(b337a867) SHA1(a615283a70700028267e223df00e70bdd159ec09) )
	ROM_LOAD( "ic41-ma1",     0x08000, 0x4000, CRC(85ec8279) SHA1(dada5fa6981573a1fbb235becbc647e1e2d497e1) )
	ROM_LOAD( "ic42-m91",     0x0c000, 0x4000, CRC(a14b86dd) SHA1(73172dfeb34846beaa713c8886d56ed691139d06) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for audio code */
	ROM_LOAD( "mc-m50.bin",   0x4000, 0x4000, CRC(ba415d68) SHA1(484af7a1f109cc9546f17d19b53d284c934705db) )
	ROM_LOAD( "mc-m40.bin",   0x8000, 0x4000, CRC(2a217ed0) SHA1(b06f7c9a2c96ffe78a7065e5edadfdbf985305a5) )
	ROM_LOAD( "mc-m30.bin",   0xc000, 0x4000, CRC(95af1723) SHA1(691ca3f7400d10897e805ff691c904fb2d5bb53a) )

	ROM_REGION( 0x0800, REGION_CPU3, 0 )	/* 8k for the microcontroller */
	ROM_LOAD( "01",           0x0000, 0x0800, CRC(00c7f80c) SHA1(d2216f660eb8310b1530fa5dc844d26ba90c5e9c) )

	ROM_REGION( 0x06000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mc-m60.bin",   0x00000, 0x2000, CRC(1cdbb117) SHA1(cce99c7380fa2a7ae070c7e2d64866866c976085) )	/* Character ROMs - 1024 chars, 3 bpp */
	ROM_LOAD( "mc-m70.bin",   0x02000, 0x2000, CRC(553f0780) SHA1(eacce92ae7b872a35f289f79b33383f5442082d5) )
	ROM_LOAD( "mc-m80.bin",   0x04000, 0x2000, CRC(9392ecb7) SHA1(fb4be39fc2f1c826b146bb5b4dd10eb56b23c300) )

	ROM_REGION( 0x18000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "mc-m01.bin",   0x00000, 0x8000, CRC(da558e4d) SHA1(0635f4cded061b0b3649ed1497f087ecd53d54a3) )	/* tile set */
	ROM_LOAD( "mc-m10.bin",   0x08000, 0x8000, CRC(619a02f8) SHA1(18de76277c263c76b8d8d9093b3c1aebbf2b7ae4) )
	ROM_LOAD( "mc-m20.bin",   0x10000, 0x8000, CRC(a617c6c1) SHA1(dccae543daa9987f2778327145fc785472f41228) )

	ROM_REGION( 0x54000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "mc-mc0.bin",   0x00000, 0x4000, CRC(133d644f) SHA1(5378e0cb665c0aa65d7ad76c3f7c04a3bc301f64) )	/* sprites */
	ROM_LOAD( "mc-md0.bin",   0x04000, 0x4000, CRC(e387b036) SHA1(828a42789d9ced9f9fcdfd08a43530008dcbbf2f) )
	ROM_LOAD( "mc-me0.bin",   0x08000, 0x4000, CRC(b36b1283) SHA1(9d12ea9f7a0f12aad532c0f2d3608cf4a86933a6) )
	ROM_LOAD( "mc-mf0.bin",   0x0c000, 0x4000, CRC(2584d8a9) SHA1(f24b4cb827421cd51cb35b581622c41646f3f4d8) )
	ROM_LOAD( "mc-mg0.bin",   0x10000, 0x4000, CRC(cf31a714) SHA1(9740f36e279fc4404112145abb4ff4d138f46474) )
	ROM_LOAD( "mc-mh0.bin",   0x14000, 0x4000, CRC(6292d589) SHA1(a0bf50ebc7712d9bea082834025d3bf816e5afa6) )
	ROM_LOAD( "mc-mi0.bin",   0x18000, 0x4000, CRC(ee2e06e3) SHA1(fbe0457322f5ae03e7eb46b27f044a46f3ee36b5) )
	ROM_LOAD( "mc-mj0.bin",   0x1c000, 0x4000, CRC(7e73895b) SHA1(76861f16f6845e53b80564eb869cfc6767040aa5) )
	ROM_LOAD( "mc-mk0.bin",   0x20000, 0x4000, CRC(66c8bf75) SHA1(071c8635c0264397363d6a33297fd1404f0531a9) )
	ROM_LOAD( "mc-ml0.bin",   0x24000, 0x4000, CRC(88138a1d) SHA1(a669620d51495734f0270fe49c75663cc54e2e50) )
	ROM_LOAD( "mc-mm0.bin",   0x28000, 0x4000, CRC(a1a4260d) SHA1(fbeeac9929f0b273deb1add39db228d3a6d74e76) )
	ROM_LOAD( "mc-mn0.bin",   0x2c000, 0x4000, CRC(6bc61b58) SHA1(dd5e1f8e7299358c6684cb99c4b47b1cf1dc64a7) )
	ROM_LOAD( "mc-mo0.bin",   0x30000, 0x4000, CRC(f96ef600) SHA1(f739ae6c45b2d46a587f8ce32d7626669225ad57) )
	ROM_LOAD( "mc-mp0.bin",   0x34000, 0x4000, CRC(1259618e) SHA1(9f3169675f7add038746edae2ab83fc0a7746db6) )
	ROM_LOAD( "mc-mq0.bin",   0x38000, 0x4000, CRC(102a1666) SHA1(ab052e76a3cef68dd199b5ecf01b73a8abaa32a7) )
	ROM_LOAD( "mc-mr0.bin",   0x3c000, 0x4000, CRC(1e854453) SHA1(41d4997361132c63fcd52dba23885a10ae34bf82) )
	ROM_LOAD( "mc-ms0.bin",   0x40000, 0x4000, CRC(7bc9d878) SHA1(72689ec3263e179f76c7139ed4a82684781d7bb6) )
	ROM_LOAD( "mc-mt0.bin",   0x44000, 0x4000, CRC(09cea985) SHA1(805c58bf73ea19329aa2c8a88a0c35cfaceca985) )
	ROM_LOAD( "mc-mu0.bin",   0x48000, 0x4000, CRC(5421769e) SHA1(c662c53711acf28754a60aedb4637d7d528dc5ea) )
	ROM_LOAD( "mc-mv0.bin",   0x4c000, 0x4000, CRC(36fc3e2d) SHA1(6cd358f29536ff6d5087570cb3e26fd9e971b888) )
	ROM_LOAD( "mc-mw0.bin",   0x50000, 0x4000, CRC(135dce4c) SHA1(3e64a52400137d87b60adf9c307656eadbfe709c) )

	ROM_REGION( 0x0080, REGION_PROMS, 0 )
	ROM_LOAD( "prom.2",       0x0000, 0x0020, CRC(32db2cf4) SHA1(854b3226a4843a6db94c01c6571294f17a469acf) ) /* char palette red and green components */
	ROM_LOAD( "prom.16",      0x0020, 0x0020, CRC(18836d26) SHA1(950e1ea5184355501b41548d40732b96c5516fd7) ) /* tile palette red and green components */
	ROM_LOAD( "prom.3",       0x0040, 0x0020, CRC(c7925311) SHA1(6b997803eb630b79886cebbe3bc49db1c1ab3fd9) ) /* char palette blue component */
	ROM_LOAD( "prom.17",      0x0060, 0x0020, CRC(41f51d49) SHA1(7cfaf308752cbfddf5a37a31140119afc3febaa7) ) /* tile palette blue component */
ROM_END



GAME( 1985, matmania, 0,        matmania, matmania, 0, ROT270, "Technos (Taito America license)", "Mat Mania", 0 ,2)
GAME( 1985, excthour, matmania, matmania, matmania, 0, ROT270, "Technos (Taito license)", "Exciting Hour", 0 ,2)
GAME( 1986, maniach,  0,        maniach,  matmania, 0, ROT270, "Technos (Taito America license)", "Mania Challenge (set 1)", 0 ,2)
GAME( 1986, maniach2, maniach,  maniach,  matmania, 0, ROT270, "Technos (Taito America license)", "Mania Challenge (set 2)" , 0,2)	/* earlier version? */
