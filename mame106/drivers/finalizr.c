/***************************************************************************

Finalizer (GX523) (c) 1985 Konami

driver by Nicola Salmoria

***************************************************************************/

#include "driver.h"
#include "cpu/m6809/m6809.h"
#include "cpu/i8039/i8039.h"
#include "sound/sn76496.h"
#include "sound/dac.h"


void konami1_decode(void);

extern unsigned char *finalizr_scroll;
extern unsigned char *finalizr_videoram2,*finalizr_colorram2;
static unsigned char *finalizr_interrupt_enable;

int finalizr_T1_line;

PALETTE_INIT( finalizr );
VIDEO_START( finalizr );
WRITE8_HANDLER( finalizr_videoctrl_w );
VIDEO_UPDATE( finalizr );



static INTERRUPT_GEN( finalizr_interrupt )
{
	if (cpu_getiloops() == 0)
	{
		if (*finalizr_interrupt_enable & 2)
			cpunum_set_input_line(0, M6809_IRQ_LINE, HOLD_LINE);
	}
	else if (cpu_getiloops() % 2)
	{
		if (*finalizr_interrupt_enable & 1)
			cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
	}
}

static WRITE8_HANDLER( finalizr_coin_w )
{
	coin_counter_w(0,data & 0x01);
	coin_counter_w(1,data & 0x02);
}

WRITE8_HANDLER( finalizr_i8039_irq_w )
{
	cpunum_set_input_line(1, 0, ASSERT_LINE);
}

static WRITE8_HANDLER( i8039_irqen_w )
{
	/*  bit 0x80 goes active low, indicating that the
        external IRQ being serviced is complete
        bit 0x40 goes active high to enable the DAC ?
    */

	if ((data & 0x80) == 0)
		cpunum_set_input_line(1, 0, CLEAR_LINE);
}

static READ8_HANDLER( i8039_T1_r )
{
	/*  I suspect the clock-out from the I8039 T0 line should be connected
        here (See the i8039_T0_w handler below).
        The frequency of this clock cannot be greater than I8039 CLKIN / 45
        Accounting for the I8039 input clock, and internal/external divisors
        the frequency here should be 192KHz (I8039 CLKIN / 48)

        Here we apply a positive edge every 3.2 reads, to simulate 192KHz
        based on the I8039 main xtal clock input frequency of 9.216MHz
    */

	finalizr_T1_line++;
	if ((finalizr_T1_line % 3) == 0)
	{
		if (finalizr_T1_line == 15)
		{
			finalizr_T1_line = -1;
			return 0;
		}
		return 1;
	}
	return 0;
}

static WRITE8_HANDLER( i8039_T0_w )
{
	/*  This becomes a clock output at a frequency of 3.072MHz (derived
        by internally dividing the main xtal clock input by a factor of 3).
        This output is divided by a factor of 16, then used as a 192KHz
        input clock to the T1 input line.
        The I8039 core currently doesn't support clock out on this pin.
    */
}

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0800, 0x0800) AM_READ(input_port_5_r)
	AM_RANGE(0x0808, 0x0808) AM_READ(input_port_4_r)
	AM_RANGE(0x0810, 0x0810) AM_READ(input_port_0_r)
	AM_RANGE(0x0811, 0x0811) AM_READ(input_port_1_r)
	AM_RANGE(0x0812, 0x0812) AM_READ(input_port_2_r)
	AM_RANGE(0x0813, 0x0813) AM_READ(input_port_3_r)
	AM_RANGE(0x2000, 0x2fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x3fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x4000, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0001, 0x0001) AM_WRITE(MWA8_RAM) AM_BASE(&finalizr_scroll)
	AM_RANGE(0x0003, 0x0003) AM_WRITE(finalizr_videoctrl_w)
	AM_RANGE(0x0004, 0x0004) AM_WRITE(MWA8_RAM) AM_BASE(&finalizr_interrupt_enable)
//  AM_RANGE(0x0020, 0x003f) AM_WRITE(MWA8_RAM) AM_BASE(&finalizr_scroll)
	AM_RANGE(0x0818, 0x0818) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x0819, 0x0819) AM_WRITE(finalizr_coin_w)
	AM_RANGE(0x081a, 0x081a) AM_WRITE(SN76496_0_w)	/* This address triggers the SN chip to read the data port. */
	AM_RANGE(0x081b, 0x081b) AM_WRITE(MWA8_NOP)		/* Loads the snd command into the snd latch */
	AM_RANGE(0x081c, 0x081c) AM_WRITE(finalizr_i8039_irq_w)	/* custom sound chip */
	AM_RANGE(0x081d, 0x081d) AM_WRITE(soundlatch_w)			/* custom sound chip */
	AM_RANGE(0x2000, 0x23ff) AM_WRITE(colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x2400, 0x27ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2800, 0x2bff) AM_WRITE(MWA8_RAM) AM_BASE(&finalizr_colorram2)
	AM_RANGE(0x2c00, 0x2fff) AM_WRITE(MWA8_RAM) AM_BASE(&finalizr_videoram2)
	AM_RANGE(0x3000, 0x31ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x3200, 0x37ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x3800, 0x39ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_2)
	AM_RANGE(0x3a00, 0x3fff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_readport, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x00, 0xff) AM_READ(soundlatch_r)
	AM_RANGE(I8039_t1, I8039_t1) AM_READ(i8039_T1_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( i8039_writeport, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(I8039_p1, I8039_p1) AM_WRITE(DAC_0_data_w)
	AM_RANGE(I8039_p2, I8039_p2) AM_WRITE(i8039_irqen_w)
	AM_RANGE(I8039_t0, I8039_t0) AM_WRITE(i8039_T0_w)
ADDRESS_MAP_END



INPUT_PORTS_START( finalizr )
	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
/*  PORT_DIPSETTING(    0x00, "Invalid" ) */

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x18, "30000 150000" )
	PORT_DIPSETTING(    0x10, "50000 300000" )
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Controls ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Dual ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( finalizb )
	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
/*  PORT_DIPSETTING(    0x00, "Invalid" ) */

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x18, "20000 100000" )
	PORT_DIPSETTING(    0x10, "30000 150000" )
	PORT_DIPSETTING(    0x08, "20000" )
	PORT_DIPSETTING(    0x00, "30000" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Controls ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Dual ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			32*8+0*4, 32*8+1*4, 32*8+2*4, 32*8+3*4, 32*8+4*4, 32*8+5*4, 32*8+6*4, 32*8+7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
	32*32
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,        0, 16 },
	{ REGION_GFX1, 0, &spritelayout,  16*16, 16 },
	{ REGION_GFX1, 0, &charlayout,    16*16, 16 },  /* to handle 8x8 sprites */
	{ -1 } /* end of array */
};



static MACHINE_DRIVER_START( finalizr )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6809,18432000/6)	/* ??? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(finalizr_interrupt,16)	/* 1 IRQ + 8 NMI (generated by a custom IC) */

	MDRV_CPU_ADD(I8039,(18432000/2)/I8039_CLOCK_DIVIDER)	/* 9.216MHz clkin ?? */
	/* audio CPU */	/* DAC */
	MDRV_CPU_PROGRAM_MAP(i8039_readmem,i8039_writemem)
	MDRV_CPU_IO_MAP(i8039_readport,i8039_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(36*8, 32*8)
	MDRV_VISIBLE_AREA(1*8, 35*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(32)
	MDRV_COLORTABLE_LENGTH(2*16*16)

	MDRV_PALETTE_INIT(finalizr)
	MDRV_VIDEO_START(finalizr)
	MDRV_VIDEO_UPDATE(finalizr)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(SN76496, 18432000/12)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.65)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( finalizr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "523k01.9c",    0x4000, 0x4000, CRC(716633cb) SHA1(9c21e608b6a73967688fa6aeb5995c20c1b48c74) )
	ROM_LOAD( "523k02.12c",   0x8000, 0x4000, CRC(1bccc696) SHA1(3c29f4a030e76660b5a25347e042e344b0653343) )
	ROM_LOAD( "523k03.13c",   0xc000, 0x4000, CRC(c48927c6) SHA1(9cf6b285034670370ba0246c33e1fe0a057457e7) )

	ROM_REGION( 0x1000, REGION_CPU2, 0 )	/* 8039 */
	ROM_LOAD( "d8749hd.bin",  0x0000, 0x0800, CRC(978dfc33) SHA1(13d24ce577b88bf6ec2e970d36dc67a7ec691c55) )	/* this comes from the bootleg, */
															/* the original has a custom IC */

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "523h04.5e",    0x00000, 0x4000, CRC(c056d710) SHA1(3fe0ab7ef3bce7298c2a073d0985c33f9dc40062) )
	ROM_LOAD16_BYTE( "523h07.5f",    0x00001, 0x4000, CRC(50e512ba) SHA1(f916afb9df1872f9de571d20b9045b20d9172eaa) )
	ROM_LOAD16_BYTE( "523h05.6e",    0x08000, 0x4000, CRC(ae0d0f76) SHA1(6dd0119e4ba7ebb32ba1ca6395f80d18f1617ce8) )
	ROM_LOAD16_BYTE( "523h08.6f",    0x08001, 0x4000, CRC(79f44e17) SHA1(cb32edc4df9f2209f13fc258fec4e67ee91badef) )
	ROM_LOAD16_BYTE( "523h06.7e",    0x10000, 0x4000, CRC(d2db9689) SHA1(ceb5913716b4da2ddff2e837ddaa04d91e52f9e1) )
	ROM_LOAD16_BYTE( "523h09.7f",    0x10001, 0x4000, CRC(8896dc85) SHA1(91493c6b69655de482f0c2a0cb3662fc0d1b6e45) )
	/* 18000-1ffff empty */

	ROM_REGION( 0x0240, REGION_PROMS, 0 )
	ROM_LOAD( "523h10.2f",    0x0000, 0x0020, CRC(ec15dd15) SHA1(710384b154a9363fdc88edffda252f1d60e000dc) ) /* palette */
	ROM_LOAD( "523h11.3f",    0x0020, 0x0020, CRC(54be2e83) SHA1(3200abc7f2238d62d7204ef57a6daa2df150538d) ) /* palette */
	ROM_LOAD( "523h12.10f",   0x0040, 0x0100, CRC(53166a2a) SHA1(6cdde206036df7176679711f7888d72acee27c8f) ) /* sprites */
	ROM_LOAD( "523h13.11f",   0x0140, 0x0100, CRC(4e0647a0) SHA1(fb87f878456b8b76bb2c028cb890d2a5c1c3e388) ) /* characters */
ROM_END

ROM_START( finalizb )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "finalizr.5",   0x4000, 0x8000, CRC(a55e3f14) SHA1(47f6da214b36cc56be547fa4313afcc5572508a2) )
	ROM_LOAD( "finalizr.6",   0xc000, 0x4000, CRC(ce177f6e) SHA1(034cbe0c1e2baf9577741b3c222a8b4a8ac8c919) )

	ROM_REGION( 0x1000, REGION_CPU2, 0 )	/* 8039 */
	ROM_LOAD( "d8749hd.bin",  0x0000, 0x0800, CRC(978dfc33) SHA1(13d24ce577b88bf6ec2e970d36dc67a7ec691c55) )

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "523h04.5e",    0x00000, 0x4000, CRC(c056d710) SHA1(3fe0ab7ef3bce7298c2a073d0985c33f9dc40062) )
	ROM_LOAD16_BYTE( "523h07.5f",    0x00001, 0x4000, CRC(50e512ba) SHA1(f916afb9df1872f9de571d20b9045b20d9172eaa) )
	ROM_LOAD16_BYTE( "523h05.6e",    0x08000, 0x4000, CRC(ae0d0f76) SHA1(6dd0119e4ba7ebb32ba1ca6395f80d18f1617ce8) )
	ROM_LOAD16_BYTE( "523h08.6f",    0x08001, 0x4000, CRC(79f44e17) SHA1(cb32edc4df9f2209f13fc258fec4e67ee91badef) )
	ROM_LOAD16_BYTE( "523h06.7e",    0x10000, 0x4000, CRC(d2db9689) SHA1(ceb5913716b4da2ddff2e837ddaa04d91e52f9e1) )
	ROM_LOAD16_BYTE( "523h09.7f",    0x10001, 0x4000, CRC(8896dc85) SHA1(91493c6b69655de482f0c2a0cb3662fc0d1b6e45) )
	/* 18000-1ffff empty */

	ROM_REGION( 0x0240, REGION_PROMS, 0 )
	ROM_LOAD( "523h10.2f",    0x0000, 0x0020, CRC(ec15dd15) SHA1(710384b154a9363fdc88edffda252f1d60e000dc) ) /* palette */
	ROM_LOAD( "523h11.3f",    0x0020, 0x0020, CRC(54be2e83) SHA1(3200abc7f2238d62d7204ef57a6daa2df150538d) ) /* palette */
	ROM_LOAD( "523h12.10f",   0x0040, 0x0100, CRC(53166a2a) SHA1(6cdde206036df7176679711f7888d72acee27c8f) ) /* sprites */
	ROM_LOAD( "523h13.11f",   0x0140, 0x0100, CRC(4e0647a0) SHA1(fb87f878456b8b76bb2c028cb890d2a5c1c3e388) ) /* characters */
ROM_END


static DRIVER_INIT( finalizr )
{
	konami1_decode();
}


GAME( 1985, finalizr, 0,        finalizr, finalizr, finalizr, ROT90, "Konami", "Finalizer - Super Transformation", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL ,0)
GAME( 1985, finalizb, finalizr, finalizr, finalizb, finalizr, ROT90, "bootleg", "Finalizer - Super Transformation (bootleg)", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL ,0)
