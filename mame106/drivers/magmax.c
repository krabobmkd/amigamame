/***************************************************************************

MAGMAX
(c)1985 NihonBussan Co.,Ltd.

Driver by Takahiro Nogi (nogi@kt.rim.or.jp) 1999/11/05 -
Additional tweaking by Jarek Burczynski

***************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "sound/ay8910.h"

PALETTE_INIT( magmax );
VIDEO_UPDATE( magmax );
VIDEO_START( magmax );

extern unsigned short magmax_vreg;
extern UINT16 *magmax_scroll_x;
extern UINT16 *magmax_scroll_y;


static unsigned char sound_latch = 0;
static unsigned char LS74_clr = 0;
static unsigned char LS74_q   = 0;

static WRITE16_HANDLER( magmax_sound_w )
{
	if (ACCESSING_LSB)
	{
		sound_latch = (data & 0xff) << 1;
		cpunum_set_input_line(1, 0, ASSERT_LINE);
	}
}

static READ8_HANDLER( magmax_sound_irq_ack )
{
	cpunum_set_input_line(1, 0, CLEAR_LINE);
	return 0;
}

static READ8_HANDLER( magmax_sound_r )
{
	return (sound_latch | LS74_q);
}

WRITE8_HANDLER( ay8910_portB_0_w )
{
	/*bit 0 is input to CLR line of the LS74*/
	LS74_clr = data & 1;
	if (LS74_clr == 0)
		LS74_q = 0;
}

static void scanline_callback(int scanline)
{
	/* bit 0 goes hi whenever line V6 from video part goes lo->hi */
	/* that is when scanline is 64 and 192 accordingly */
	if (LS74_clr != 0)
		LS74_q = 1;

	scanline += 128;
	scanline &= 255;

	timer_set( cpu_getscanlinetime( scanline ), scanline, scanline_callback );
}

static MACHINE_RESET( magmax )
{
	timer_set(cpu_getscanlinetime( 64 ), 64, scanline_callback );
#if 0
	{
		int i;
		for (i=0; i<9; i++)
			logerror("SOUND Chan#%i name=%s\n", i, mixer_get_name(i) );
	}
#endif
}


static int gain_control = 0;

WRITE8_HANDLER( ay8910_portA_0_w )
{
float percent;

/*There are three AY8910 chips and four(!) separate amplifiers on the board
* Each of AY channels is hardware mapped in following way:
* amplifier 0 gain x 1.00 <- AY0 CHA
* amplifier 1 gain x 1.00 <- AY0 CHB + AY0 CHC + AY1 CHA + AY1 CHB
* amplifier 2 gain x 4.54 (150K/33K) <- AY1 CHC + AY2 CHA
* amplifier 3 gain x 4.54 (150K/33K) <- AY2 CHB + AY2 CHC
*
* Each of the amps has its own analog cuircit:
* amp0, amp1 and amp2 are different from each other; amp3 is the same as amp2
*
* Outputs of those amps are inputs to post amps, each having own cuircit
* that is partially controlled by AY #0 port A.
* PORT A BIT 0 - control postamp 0 (gain x10.0 | gain x 5.00)
* PORT A BIT 1 - control postamp 1 (gain x4.54 | gain x 2.27)
* PORT A BIT 2 - control postamp 2 (gain x1.00 | gain x 0.50)
* PORT A BIT 3 - control postamp 3 (gain x1.00 | gain x 0.50)
*
* The "control" means assert/clear input pins on chip called 4066 (it is analog switch)
* which results in volume gain (exactly 2 times).
* I use mixer_set_volume() to emulate the effect.

gain summary:
port A control ON         OFF
amp0 = *1*10.0=10.0  *1*5.0   = 5.0
amp1 = *1*4.54=4.54  *1*2.27  = 2.27
amp2 = *4.54*1=4.54  *4.54*0.5= 2.27
amp3 = *4.54*1=4.54  *4.54*0.5= 2.27
*/

/*
bit0 - SOUND Chan#0 name=AY-3-8910 #0 Ch A

bit1 - SOUND Chan#1 name=AY-3-8910 #0 Ch B
bit1 - SOUND Chan#2 name=AY-3-8910 #0 Ch C
bit1 - SOUND Chan#3 name=AY-3-8910 #1 Ch A
bit1 - SOUND Chan#4 name=AY-3-8910 #1 Ch B

bit2 - SOUND Chan#5 name=AY-3-8910 #1 Ch C
bit2 - SOUND Chan#6 name=AY-3-8910 #2 Ch A

bit3 - SOUND Chan#7 name=AY-3-8910 #2 Ch B
bit3 - SOUND Chan#8 name=AY-3-8910 #2 Ch C
*/

	if (gain_control == (data & 0x0f))
		return;

	gain_control = data & 0x0f;

	/*ui_popup("gain_ctrl = %2x",data&0x0f);*/

	percent = (gain_control & 1) ? 1.0 : 0.50;
	sndti_set_output_gain(SOUND_AY8910, 0, 0, percent);
//fixme:    set_RC_filter(0,10000,100000000,0,10000);   /* 10K, 10000pF = 0.010uF */

	percent = (gain_control & 2) ? 0.45 : 0.23;
	sndti_set_output_gain(SOUND_AY8910, 0, 1, percent);
	sndti_set_output_gain(SOUND_AY8910, 0, 2, percent);
	sndti_set_output_gain(SOUND_AY8910, 1, 0, percent);
	sndti_set_output_gain(SOUND_AY8910, 1, 1, percent);
//fixme:    set_RC_filter(1,4700,100000000,0,4700); /*  4.7K, 4700pF = 0.0047uF */
//fixme:    set_RC_filter(2,4700,100000000,0,4700); /*  4.7K, 4700pF = 0.0047uF */
//fixme:    set_RC_filter(3,4700,100000000,0,4700); /*  4.7K, 4700pF = 0.0047uF */
//fixme:    set_RC_filter(4,4700,100000000,0,4700); /*  4.7K, 4700pF = 0.0047uF */

	percent = (gain_control & 4) ? 0.45 : 0.23;
	sndti_set_output_gain(SOUND_AY8910, 1, 2, percent);
	sndti_set_output_gain(SOUND_AY8910, 2, 0, percent);

	percent = (gain_control & 8) ? 0.45 : 0.23;
	sndti_set_output_gain(SOUND_AY8910, 2, 1, percent);
	sndti_set_output_gain(SOUND_AY8910, 2, 2, percent);
}

static WRITE16_HANDLER( magmax_vreg_w )
{
	/* VRAM CONTROL REGISTER */
	/* bit0 - coin counter 1    */
	/* bit1 - coin counter 2    */
	/* bit2 - flip screen (INV) */
	/* bit3 - page bank to be displayed (PG) */
	/* bit4 - sprite bank LSB (DP0) */
	/* bit5 - sprite bank MSB (DP1) */
	/* bit6 - BG display enable (BE)*/
	COMBINE_DATA(&magmax_vreg);
}



static ADDRESS_MAP_START( magmax_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x013fff) AM_READ(MRA16_ROM)
	AM_RANGE(0x018000, 0x018fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x020000, 0x0207ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x028000, 0x0281ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x030000, 0x030001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x030002, 0x030003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x030004, 0x030005) AM_READ(input_port_2_word_r)
	AM_RANGE(0x030006, 0x030007) AM_READ(input_port_3_word_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( magmax_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x013fff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x018000, 0x018fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x020000, 0x0207ff) AM_WRITE(MWA16_RAM) AM_BASE(&videoram16) AM_SIZE(&videoram_size)
	AM_RANGE(0x028000, 0x0281ff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0x030010, 0x030011) AM_WRITE(magmax_vreg_w)
	AM_RANGE(0x030012, 0x030013) AM_WRITE(MWA16_RAM) AM_BASE(&magmax_scroll_x)
	AM_RANGE(0x030014, 0x030015) AM_WRITE(MWA16_RAM) AM_BASE(&magmax_scroll_y)
	AM_RANGE(0x03001c, 0x03001d) AM_WRITE(magmax_sound_w)
	AM_RANGE(0x03001e, 0x03001f) AM_WRITE(MWA16_NOP)	/* IRQ ack */
ADDRESS_MAP_END

static ADDRESS_MAP_START( magmax_soundreadmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x4000, 0x4000) AM_READ(magmax_sound_irq_ack)
	AM_RANGE(0x6000, 0x67ff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( magmax_soundwritemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x6000, 0x67ff) AM_WRITE(MWA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( magmax_soundreadport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x06, 0x06) AM_READ(magmax_sound_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( magmax_soundwriteport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x02, 0x02) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x03, 0x03) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x04, 0x04) AM_WRITE(AY8910_control_port_2_w)
	AM_RANGE(0x05, 0x05) AM_WRITE(AY8910_write_port_2_w)
ADDRESS_MAP_END


INPUT_PORTS_START( magmax )
	PORT_START	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )	/*Maybe this is just a test button and as such is not available to player*/
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP  ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* Coin, Start, Test, Dipswitch */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE( 0x20, IP_ACTIVE_LOW )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* Dipswitch */
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0003, "3" )
	PORT_DIPSETTING(      0x0002, "4" )
	PORT_DIPSETTING(      0x0001, "5" )
	PORT_DIPSETTING(      0x0000, "6" )
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(      0x000c, "30000 every" )
	PORT_DIPSETTING(      0x0004, "70000 every" )
	PORT_DIPSETTING(      0x0008, "50000 every" )
	PORT_DIPSETTING(      0x0000, "90000 every" )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0000, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Upright ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8, 8,	/* 8*8 characters */
	256,	/* 256 characters */
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 4, 0, 12, 8, 20, 16, 28, 24 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout spritelayout =
{
	16, 16,	/* 16*16 characters */
	512,	/* 512 characters */
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 4, 0, 4+512*64*8, 0+512*64*8, 12, 8, 12+512*64*8, 8+512*64*8,
	  20, 16, 20+512*64*8, 16+512*64*8, 28, 24, 28+512*64*8, 24+512*64*8 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
		8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	64*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,           0,  1 }, /*no color codes*/
	{ REGION_GFX2, 0, &spritelayout,      1*16, 16 }, /*16 color codes*/
	{ -1 }
};


static struct AY8910interface ay8910_interface =
{
	0,
	0,
	ay8910_portA_0_w, /*write port A*/
	ay8910_portB_0_w  /*write port B*/
};


static MACHINE_DRIVER_START( magmax )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 8000000)	/* 8 MHz */
	MDRV_CPU_PROGRAM_MAP(magmax_readmem,magmax_writemem)
	MDRV_CPU_VBLANK_INT(irq1_line_hold,1)

	MDRV_CPU_ADD(Z80,10000000/4)
	/* audio CPU */	/* 2.5 MHz */
	MDRV_CPU_PROGRAM_MAP(magmax_soundreadmem,magmax_soundwritemem)
	MDRV_CPU_IO_MAP(magmax_soundreadport,magmax_soundwriteport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(10)

	MDRV_MACHINE_RESET(magmax)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)
	MDRV_COLORTABLE_LENGTH(1*16 + 16*16)

	MDRV_PALETTE_INIT(magmax)
	MDRV_VIDEO_START(magmax)
	MDRV_VIDEO_UPDATE(magmax)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 10000000/8)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)

	MDRV_SOUND_ADD(AY8910, 10000000/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)

	MDRV_SOUND_ADD(AY8910, 10000000/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)
MACHINE_DRIVER_END


ROM_START( magmax )
	ROM_REGION( 0x14000, REGION_CPU1, 0 ) /* 68000 (main) cpu code */
	ROM_LOAD16_BYTE( "1.3b", 0x00001, 0x4000, CRC(33793cbb) SHA1(a0bc0e4be434d9fc8115de8d63c92e942334bc85) )
	ROM_LOAD16_BYTE( "6.3d", 0x00000, 0x4000, CRC(677ef450) SHA1(9003ff1c1c455970c1bd036b0b5e44dae2e379a5) )
	ROM_LOAD16_BYTE( "2.5b", 0x08001, 0x4000, CRC(1a0c84df) SHA1(77ff21de33392a148d7ca69a77acc654260af0db) )
	ROM_LOAD16_BYTE( "7.5d", 0x08000, 0x4000, CRC(01c35e95) SHA1(4f1a0d0463a956d8f9ed425cbeaed6186eb130a5) )
	ROM_LOAD16_BYTE( "3.6b", 0x10001, 0x2000, CRC(d06e6cae) SHA1(94047b2bcf030d34295ff8107f95097ce57efe6b) )
	ROM_LOAD16_BYTE( "8.6d", 0x10000, 0x2000, CRC(790a82be) SHA1(9a25d5a7c87aeef5e736b0f2fb8dde1c9be70039) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 (sound) cpu code */
	ROM_LOAD( "15.17b", 0x00000, 0x2000, CRC(19e7b983) SHA1(b1cd0b728e7cce87d9b1039be179d0915d939a4f) )
	ROM_LOAD( "16.18b", 0x02000, 0x2000, CRC(055e3126) SHA1(8c9b03eb7588512ef17f8c1b731a2fd7cf372bf8) )

	ROM_REGION( 0x02000, REGION_GFX1, ROMREGION_DISPOSE ) /* chars */
	ROM_LOAD( "23.15g", 0x00000, 0x2000, CRC(a7471da2) SHA1(ec2815a5801bc55955e612173a845399fd493eb7) )

	ROM_REGION( 0x10000, REGION_GFX2, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "17.3e",  0x00000, 0x2000, CRC(8e305b2e) SHA1(74c318089f6bebafbee31c22302e93a09d3ffa32) )
	ROM_LOAD( "18.5e",  0x02000, 0x2000, CRC(14c55a60) SHA1(fd2a1b434bb65502f0f791995caf1cd869ccd254) )
	ROM_LOAD( "19.6e",  0x04000, 0x2000, CRC(fa4141d8) SHA1(a5279d1ada5a13df14a8bbc18ceeea79f82a4c23) )
	ROM_LOAD( "20.3g",  0x08000, 0x2000, CRC(6fa3918b) SHA1(658bdbdc581732922c986b07746a9601d86ec5a2) )
	ROM_LOAD( "21.5g",  0x0a000, 0x2000, CRC(dd52eda4) SHA1(773e92c918f5b076ce3cae55a33a27c38d958edf) )
	ROM_LOAD( "22.6g",  0x0c000, 0x2000, CRC(4afc98ff) SHA1(a34d63befdb3c749460d1cfb62e15ced52859b9b) )

	ROM_REGION( 0x10000, REGION_USER1, 0 ) /* surface scroll control */
	ROM_LOAD16_BYTE( "4.18b",  0x00000, 0x2000, CRC(1550942e) SHA1(436424d63ca576d13b0f4a3713f009a38e33f2f3) )
	ROM_LOAD16_BYTE( "5.20b",  0x00001, 0x2000, CRC(3b93017f) SHA1(b1b67c2050c8033c29bb74ab909075c39e4f7c6a) )
	/* BG control data */
	ROM_LOAD( "9.18d",  0x04000, 0x2000, CRC(9ecc9ab8) SHA1(ea5fbd9e9ce09e25f532dc74623e0f7e8464b7f3) ) /* surface */
	ROM_LOAD( "10.20d", 0x06000, 0x2000, CRC(e2ff7293) SHA1(d93c30f7edac53747efcf840325a8ce5f5e47b32) ) /* underground */
	/* background tiles */
	ROM_LOAD( "11.15f", 0x08000, 0x2000, CRC(91f3edb6) SHA1(64e8008cad0e9c42c2ee972c2ee867c7c51cae27) ) /* surface */
	ROM_LOAD( "12.17f", 0x0a000, 0x2000, CRC(99771eff) SHA1(5a1e2316b4055a1332d9d1f02edee5bc6aae90ac) ) /* underground */
	ROM_LOAD( "13.18f", 0x0c000, 0x2000, CRC(75f30159) SHA1(d188ccf926e7a842e90ebc1aad3dc20c37d84b98) ) /* surface of mechanical level */
	ROM_LOAD( "14.20f", 0x0e000, 0x2000, CRC(96babcba) SHA1(fec58ccc1e5cc2cec56658a412b94fe7b989541d) ) /* underground of mechanical level */

	ROM_REGION( 0x0200, REGION_USER2, 0 ) /* BG control data */
	ROM_LOAD( "mag_b.14d",  0x0000, 0x0100, CRC(a0fb7297) SHA1(e6461050e7e586475343156aae1066b944ceab66) ) /* background control PROM */
	ROM_LOAD( "mag_c.15d",  0x0100, 0x0100, CRC(d84a6f78) SHA1(f2ce329b1adf39bde6df2eb79be6d144adea65d0) ) /* background control PROM */

	ROM_REGION( 0x0500, REGION_PROMS, 0 ) /* color PROMs */
	ROM_LOAD( "mag_e.10f",  0x0000, 0x0100, CRC(75e4f06a) SHA1(cdaccc3e56df4ac9ace04b93b3bab9a62f1ea6f5) ) /* red */
	ROM_LOAD( "mag_d.10e",  0x0100, 0x0100, CRC(34b6a6e3) SHA1(af254ccf0d38e1f4644375cd357d468ad4efe450) ) /* green */
	ROM_LOAD( "mag_a.10d",  0x0200, 0x0100, CRC(a7ea7718) SHA1(4789586d6795644517a18f179b4ae5f23737b21d) ) /* blue */
	ROM_LOAD( "mag_g.2e",   0x0300, 0x0100, CRC(830be358) SHA1(f412587718040a783c4e6453619930c90daf385e) ) /* sprites color lookup table */
	ROM_LOAD( "mag_f.13b",  0x0400, 0x0100, CRC(4a6f9a6d) SHA1(65f1e0bfacd1f354ece1b18598a551044c27c4d1) ) /* state machine data used for video signals generation (not used in emulation)*/
ROM_END


GAME( 1985, magmax, 0, magmax, magmax, 0, ROT0, "Nichibutsu", "Mag Max", 0 ,0)
