/***************************************************************************

Atari Drag Race Driver

***************************************************************************/

#include "driver.h"
#include "dragrace.h"
#include "sound/discrete.h"


static unsigned dragrace_misc_flags = 0;

static int dragrace_gear[2];

static void dragrace_frame_callback(int dummy)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		switch (readinputport(5 + i))
		{
		case 0x01: dragrace_gear[i] = 1; break;
		case 0x02: dragrace_gear[i] = 2; break;
		case 0x04: dragrace_gear[i] = 3; break;
		case 0x08: dragrace_gear[i] = 4; break;
		case 0x10: dragrace_gear[i] = 0; break;
		}
	}

	/* watchdog is disabled during service mode */
		watchdog_enable(readinputport(0) & 0x20);
}


static MACHINE_RESET( dragrace )
{
	timer_pulse(cpu_getscanlinetime(0), 0, dragrace_frame_callback);
}

static void dragrace_update_misc_flags(void)
{
	/* 0x0900 = set 3SPEED1         0x00000001
     * 0x0901 = set 4SPEED1         0x00000002
     * 0x0902 = set 5SPEED1         0x00000004
     * 0x0903 = set 6SPEED1         0x00000008
     * 0x0904 = set 7SPEED1         0x00000010
     * 0x0905 = set EXPLOSION1      0x00000020
     * 0x0906 = set SCREECH1        0x00000040
     * 0x0920 - 0x0927 = clear 0x0900 - 0x0907

     * 0x0909 = set KLEXPL1         0x00000200
     * 0x090b = set MOTOR1          0x00000800
     * 0x090c = set ATTRACT         0x00001000
     * 0x090d = set LOTONE          0x00002000
     * 0x090f = set Player 1 Start Lamp 0x00008000
     * 0x0928 - 0x092f = clear 0x0908 - 0x090f

     * 0x0910 = set 3SPEED2         0x00010000
     * 0x0911 = set 4SPEED2         0x00020000
     * 0x0912 = set 5SPEED2         0x00040000
     * 0x0913 = set 6SPEED2         0x00080000
     * 0x0914 = set 7SPEED2         0x00100000
     * 0x0915 = set EXPLOSION2      0x00200000
     * 0x0916 = set SCREECH2        0x00400000
     * 0x0930 = clear 0x0910 - 0x0917

     * 0x0919 = set KLEXPL2         0x02000000
     * 0x091b = set MOTOR2          0x08000000
     * 0x091d = set HITONE          0x20000000
     * 0x091f = set Player 2 Start Lamp 0x80000000
     * 0x0938 = clear 0x0918 - 0x091f
     */
	set_led_status(0, dragrace_misc_flags & 0x00008000);
	set_led_status(1, dragrace_misc_flags & 0x80000000);

	discrete_sound_w(DRAGRACE_MOTOR1_DATA,  ~dragrace_misc_flags & 0x0000001f);		// Speed1 data*
	discrete_sound_w(DRAGRACE_EXPLODE1_EN, (dragrace_misc_flags & 0x00000020) ? 1: 0);	// Explosion1 enable
	discrete_sound_w(DRAGRACE_SCREECH1_EN, (dragrace_misc_flags & 0x00000040) ? 1: 0);	// Screech1 enable
	discrete_sound_w(DRAGRACE_KLEXPL1_EN, (dragrace_misc_flags & 0x00000200) ? 1: 0);	// KLEXPL1 enable
	discrete_sound_w(DRAGRACE_MOTOR1_EN, (dragrace_misc_flags & 0x00000800) ? 1: 0);	// Motor1 enable

	discrete_sound_w(DRAGRACE_MOTOR2_DATA, (~dragrace_misc_flags & 0x001f0000) >> 0x10);	// Speed2 data*
	discrete_sound_w(DRAGRACE_EXPLODE2_EN, (dragrace_misc_flags & 0x00200000) ? 1: 0);	// Explosion2 enable
	discrete_sound_w(DRAGRACE_SCREECH2_EN, (dragrace_misc_flags & 0x00400000) ? 1: 0);	// Screech2 enable
	discrete_sound_w(DRAGRACE_KLEXPL2_EN, (dragrace_misc_flags & 0x02000000) ? 1: 0);	// KLEXPL2 enable
	discrete_sound_w(DRAGRACE_MOTOR2_EN, (dragrace_misc_flags & 0x08000000) ? 1: 0);	// Motor2 enable

	discrete_sound_w(DRAGRACE_ATTRACT_EN, (dragrace_misc_flags & 0x00001000) ? 1: 0);	// Attract enable
	discrete_sound_w(DRAGRACE_LOTONE_EN, (dragrace_misc_flags & 0x00002000) ? 1: 0);	// LoTone enable
	discrete_sound_w(DRAGRACE_HITONE_EN, (dragrace_misc_flags & 0x20000000) ? 1: 0);	// HiTone enable
}

WRITE8_HANDLER( dragrace_misc_w )
{
	/* Set/clear individual bit */
	UINT32 mask = 1 << offset;
	if (data & 0x01)
		dragrace_misc_flags |= mask;
	else
		dragrace_misc_flags &= (~mask);
	logerror("Set   %#6x, Mask=%#10x, Flag=%#10x, Data=%x\n", 0x0900+offset, mask, dragrace_misc_flags, data & 0x01);
	dragrace_update_misc_flags();
	}

WRITE8_HANDLER( dragrace_misc_clear_w )
{
	/* Clear 8 bits */
	UINT32 mask = 0xff << (((offset >> 3) & 0x03) * 8);
	dragrace_misc_flags &= (~mask);
	logerror("Clear %#6x, Mask=%#10x, Flag=%#10x, Data=%x\n", 0x0920+offset, mask, dragrace_misc_flags, data & 0x01);
	dragrace_update_misc_flags();
}

READ8_HANDLER( dragrace_input_r )
{
	int val = readinputport(2);

	UINT8 maskA = 1 << (offset % 8);
	UINT8 maskB = 1 << (offset / 8);

	int i;

	for (i = 0; i < 2; i++)
	{
		int in = readinputport(i);

		if (dragrace_gear[i] != 0)
		{
			in &= ~(1 << dragrace_gear[i]);
		}

		if (in & maskA)
		{
			val |= 1 << i;
		}
	}

	return (val & maskB) ? 0xFF : 0x7F;
}


READ8_HANDLER( dragrace_steering_r )
{
	int bitA[2];
	int bitB[2];

	int i;

	for (i = 0; i < 2; i++)
	{
		int dial = readinputport(3 + i);

		bitA[i] = ((dial + 1) / 2) & 1;
		bitB[i] = ((dial + 0) / 2) & 1;
	}

	return
		(bitA[0] << 0) | (bitB[0] << 1) |
		(bitA[1] << 2) | (bitB[1] << 3);
}


READ8_HANDLER( dragrace_scanline_r )
{
	return (cpu_getscanline() ^ 0xf0) | 0x0f;
}


static ADDRESS_MAP_START( dragrace_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0080, 0x00ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x0800, 0x083f) AM_READ(dragrace_input_r)
	AM_RANGE(0x0c00, 0x0c00) AM_READ(dragrace_steering_r)
	AM_RANGE(0x0d00, 0x0d00) AM_READ(dragrace_scanline_r)
	AM_RANGE(0x1000, 0x1fff) AM_READ(MRA8_ROM) /* program */
	AM_RANGE(0xf800, 0xffff) AM_READ(MRA8_ROM) /* program mirror */
ADDRESS_MAP_END

static ADDRESS_MAP_START( dragrace_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0080, 0x00ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x0900, 0x091f) AM_WRITE(dragrace_misc_w)
	AM_RANGE(0x0920, 0x093f) AM_WRITE(dragrace_misc_clear_w)
	AM_RANGE(0x0a00, 0x0aff) AM_WRITE(MWA8_RAM) AM_BASE(&dragrace_playfield_ram)
	AM_RANGE(0x0b00, 0x0bff) AM_WRITE(MWA8_RAM) AM_BASE(&dragrace_position_ram)
	AM_RANGE(0x0e00, 0x0eff) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x1000, 0x1fff) AM_WRITE(MWA8_ROM) /* program */
	AM_RANGE(0xf800, 0xffff) AM_WRITE(MWA8_ROM) /* program mirror */
ADDRESS_MAP_END


INPUT_PORTS_START( dragrace )
	PORT_START /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Player 1 Gas") PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 1 gear 1 */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 1 gear 2 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 1 gear 3 */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 1 gear 4 */
	PORT_SERVICE( 0x20, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0xc0, 0x80, "Extended Play" )
	PORT_DIPSETTING( 0x00, "6.9 seconds" )
	PORT_DIPSETTING( 0x80, "5.9 seconds" )
	PORT_DIPSETTING( 0x40, "4.9 seconds" )
	PORT_DIPSETTING( 0xc0, "Never" )

	PORT_START /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Player 2 Gas") PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 2 gear 1 */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 2 gear 2 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 2 gear 3 */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) /* player 2 gear 4 */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0xc0, 0x80, "Number Of Heats" )
	PORT_DIPSETTING( 0xc0, "3" )
	PORT_DIPSETTING( 0x80, "4" )
	PORT_DIPSETTING( 0x00, "5" )

	PORT_START /* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED ) /* IN0 connects here */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED ) /* IN1 connects here */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Coinage ) )
	PORT_DIPSETTING( 0xc0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING( 0x40, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING( 0x80, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING( 0x00, DEF_STR( Free_Play ) )

	PORT_START /* IN3 */
	PORT_BIT( 0xff, 0x00, IPT_DIAL_V ) PORT_SENSITIVITY(25) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START /* IN4 */
	PORT_BIT( 0xff, 0x00, IPT_DIAL_V ) PORT_SENSITIVITY(25) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START /* IN5 */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Player 1 Gear 1") PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Player 1 Gear 2") PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Player 1 Gear 3") PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Player 1 Gear 4") PORT_PLAYER(1)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Player 1 Neutral") PORT_PLAYER(1)

	PORT_START /* IN6 */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Player 2 Gear 1") PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Player 2 Gear 2") PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Player 2 Gear 3") PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Player 2 Gear 4") PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Player 2 Neutral") PORT_PLAYER(2)

	PORT_START
	PORT_ADJUSTER( 81, "Motor 1 RPM" )

	PORT_START
	PORT_ADJUSTER( 85, "Motor 2 RPM" )
INPUT_PORTS_END


static const gfx_layout dragrace_tile_layout1 =
{
	16, 16,   /* width, height */
	0x40,     /* total         */
	1,        /* planes        */
	{ 0 },    /* plane offsets */
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87
	},
	{
		0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
		0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78
	},
	0x100      /* increment */
};


static const gfx_layout dragrace_tile_layout2 =
{
	16, 16,   /* width, height */
	0x20,     /* total         */
	2,        /* planes        */
	{         /* plane offsets */
		0x0000, 0x2000
	},
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87
	},
	{
		0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
		0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78
	},
	0x100      /* increment */
};


static const gfx_decode dragrace_gfx_decode_info[] =
{
	{ REGION_GFX1, 0, &dragrace_tile_layout1, 0, 4 },
	{ REGION_GFX2, 0, &dragrace_tile_layout2, 8, 2 },
	{ -1 } /* end of array */
};


static PALETTE_INIT( dragrace )
{
	palette_set_color(0, 0xFF, 0xFF, 0xFF);   /* 2 color tiles */
	palette_set_color(1, 0x00, 0x00, 0x00);
	palette_set_color(2, 0x00, 0x00, 0x00);
	palette_set_color(3, 0xFF, 0xFF, 0xFF);
	palette_set_color(4, 0x00, 0x00, 0x00);
	palette_set_color(5, 0x00, 0x00, 0x00);
	palette_set_color(6, 0xFF, 0xFF, 0xFF);
	palette_set_color(7, 0xFF, 0xFF, 0xFF);
	palette_set_color(8, 0xFF, 0xFF, 0xFF);   /* 4 color tiles */
	palette_set_color(9, 0xB0, 0xB0, 0xB0);
	palette_set_color(10,0x5F, 0x5F, 0x5F);
	palette_set_color(11,0x00, 0x00, 0x00);
	palette_set_color(12,0xFF, 0xFF, 0xFF);
	palette_set_color(13,0x5F, 0x5F, 0x5F);
	palette_set_color(14,0xB0, 0xB0, 0xB0);
	palette_set_color(15,0x00, 0x00, 0x00);
}


static MACHINE_DRIVER_START( dragrace )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6800, 12096000 / 12)
	MDRV_CPU_PROGRAM_MAP(dragrace_readmem, dragrace_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 4)
	MDRV_WATCHDOG_VBLANK_INIT(8)
	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION((int) ((22. * 1000000) / (262. * 60) + 0.5))

	MDRV_MACHINE_RESET(dragrace)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 240)
	MDRV_VISIBLE_AREA(0, 255, 0, 239)
	MDRV_GFXDECODE(dragrace_gfx_decode_info)
	MDRV_PALETTE_LENGTH(16)
	MDRV_PALETTE_INIT(dragrace)
	MDRV_VIDEO_START(dragrace)
	MDRV_VIDEO_UPDATE(dragrace)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD_TAG("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(dragrace_discrete_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END


ROM_START( dragrace )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "8513.c1", 0x1000, 0x0800, CRC(543bbb30) SHA1(646a41d1124c8365f07a93de38af007895d7d263) )
	ROM_LOAD( "8514.a1", 0x1800, 0x0800, CRC(ad218690) SHA1(08ba5f4fa4c75d8dad1a7162888d44b3349cbbe4) )
	ROM_RELOAD(          0xF800, 0x0800 )

	ROM_REGION( 0x800, REGION_GFX1, ROMREGION_DISPOSE )   /* 2 color tiles */
	ROM_LOAD( "8519dr.j0", 0x000, 0x200, CRC(aa221ba0) SHA1(450acbf349d77a790a25f3e303c31b38cc426a38) )
	ROM_LOAD( "8521dr.k0", 0x200, 0x200, CRC(0cb33f12) SHA1(d50cb55391aec03e064eecad1624d50d4c30ccab) )
	ROM_LOAD( "8520dr.r0", 0x400, 0x200, CRC(ee1ae6a7) SHA1(83491095260c8b7c616ff17ec1e888d05620f166) )

	ROM_REGION( 0x800, REGION_GFX2, ROMREGION_DISPOSE )   /* 4 color tiles */
	ROM_LOAD( "8515dr.e0", 0x000, 0x200, CRC(9510a59e) SHA1(aea0782b919279efe55a07007bd55a16f7f59239) )
	ROM_LOAD( "8517dr.h0", 0x200, 0x200, CRC(8b5bff1f) SHA1(fdcd719c66bff7c4b9f3d56d1e635259dd8add61) )
	ROM_LOAD( "8516dr.l0", 0x400, 0x200, CRC(d1e74af1) SHA1(f55a3bfd7d152ac9af128697f55c9a0c417779f5) )
	ROM_LOAD( "8518dr.n0", 0x600, 0x200, CRC(b1369028) SHA1(598a8779982d532c9f34345e793a79fcb29cac62) )
ROM_END


GAME( 1977, dragrace, 0, dragrace, dragrace, 0, 0, "Atari", "Drag Race", 0 ,2)
