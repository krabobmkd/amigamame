/* One Shot One Kill & Maddonna
   Driver by David Haywood and Paul Priest
   Dip Switches and Inputs by Stephane Humbert

Notes :
  - The YM3812 is used only for timing. All sound is played with ADPCM samples.

  - It doesn't seem possible to make 2 consecutive shots at the same place !
    Ingame bug or is there something missing in the emulation ?
    * several gun games are like this, changed the driver so it doesn't try

  - Gun X range is 0x0000-0x01ff and gun Y range is 0x0000-0x00ff, so you
    can shoot sometimes out of the "visible area" ... NOT A BUG !
  - Player 1 and 2 guns do NOT use the same routine to determine the
    coordonates of an impact on the screen : position both guns in the
    "upper left" corner in the "gun test" to see what I mean.
  - I've assumed that the shot was right at the place the shot was made,
    but I don't have any more information about that
    (what the hell is "Gun X Shift Left" REALLY used for ?)

TO DO :

  - fix some priorities for some tiles
  - verify the parameters for the guns (analog ports)
  - figure out year and manufacturer
    (NOTHING is displayed in "demo mode", nor when you complete ALL levels !)
  - sound too fast in Maddonna?
  - layer order register?

*/

#include "driver.h"
#include "sound/okim6295.h"
#include "sound/3812intf.h"


UINT16 *oneshot_sprites;
UINT16 *oneshot_bg_videoram;
UINT16 *oneshot_mid_videoram;
UINT16 *oneshot_fg_videoram;
UINT16 *oneshot_scroll;

int gun_x_p1,gun_y_p1,gun_x_p2,gun_y_p2;
int gun_x_shift;

WRITE16_HANDLER( oneshot_bg_videoram_w );
WRITE16_HANDLER( oneshot_mid_videoram_w );
WRITE16_HANDLER( oneshot_fg_videoram_w );
VIDEO_START( oneshot );
VIDEO_UPDATE( oneshot );
VIDEO_UPDATE( maddonna );


static READ16_HANDLER( oneshot_in0_word_r )
{
	int data = readinputport(0);

	switch (data & 0x0c)
	{
		case 0x00 :
			gun_x_shift = 35;
			break;
		case 0x04 :
			gun_x_shift = 30;
			break;
		case 0x08 :
			gun_x_shift = 40;
			break;
		case 0x0c :
			gun_x_shift = 50;
			break;
	}

	return data;
}

static READ16_HANDLER( oneshot_gun_x_p1_r )
{
	/* shots must be in a different location to register */
	static int wobble = 0;
	wobble ^= 1;

	return gun_x_p1 ^ wobble;
}

static READ16_HANDLER( oneshot_gun_y_p1_r )
{
	return gun_y_p1;
}

static READ16_HANDLER( oneshot_gun_x_p2_r )
{
	/* shots must be in a different location to register */
	static int wobble = 0;
	wobble ^= 1;

	return gun_x_p2 ^ wobble;
}

static READ16_HANDLER( oneshot_gun_y_p2_r )
{
	return gun_y_p2;
}

static WRITE16_HANDLER( soundbank_w )
{
	if (ACCESSING_LSB)
	{
		OKIM6295_set_bank_base(0, 0x40000 * ((data & 0x03) ^ 0x03));
	}
}



static ADDRESS_MAP_START( oneshot_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x080000, 0x087fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0c0000, 0x0c07ff) AM_READ(MRA16_RAM)
	AM_RANGE(0x120000, 0x120fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x180000, 0x182fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x190002, 0x190003) AM_READ(soundlatch_word_r)
	AM_RANGE(0x190026, 0x190027) AM_READ(oneshot_gun_x_p1_r)
	AM_RANGE(0x19002e, 0x19002f) AM_READ(oneshot_gun_x_p2_r)
	AM_RANGE(0x190036, 0x190037) AM_READ(oneshot_gun_y_p1_r)
	AM_RANGE(0x19003e, 0x19003f) AM_READ(oneshot_gun_y_p2_r)
	AM_RANGE(0x19c020, 0x19c021) AM_READ(oneshot_in0_word_r)
	AM_RANGE(0x19c024, 0x19c025) AM_READ(input_port_1_word_r)
	AM_RANGE(0x19c02c, 0x19c02d) AM_READ(input_port_2_word_r)
	AM_RANGE(0x19c030, 0x19c031) AM_READ(input_port_3_word_r)
	AM_RANGE(0x19c034, 0x19c035) AM_READ(input_port_4_word_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( oneshot_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x080000, 0x087fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x0c0000, 0x0c07ff) AM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x120000, 0x120fff) AM_WRITE(MWA16_RAM) AM_BASE(&oneshot_sprites)
	AM_RANGE(0x180000, 0x180fff) AM_WRITE(oneshot_mid_videoram_w) AM_BASE(&oneshot_mid_videoram) // some people , girl etc.
	AM_RANGE(0x181000, 0x181fff) AM_WRITE(oneshot_fg_videoram_w) AM_BASE(&oneshot_fg_videoram) // credits etc.
	AM_RANGE(0x182000, 0x182fff) AM_WRITE(oneshot_bg_videoram_w) AM_BASE(&oneshot_bg_videoram) // credits etc.
	AM_RANGE(0x188000, 0x18800f) AM_WRITE(MWA16_RAM) AM_BASE(&oneshot_scroll)	// scroll registers???
	AM_RANGE(0x190010, 0x190011) AM_WRITE(soundlatch_word_w)
	AM_RANGE(0x190018, 0x190019) AM_WRITE(soundbank_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( snd_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x8000) AM_READ(soundlatch_r)
	AM_RANGE(0x8001, 0x87ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xe000) AM_READ(YM3812_status_port_0_r)
	AM_RANGE(0xe010, 0xe010) AM_READ(OKIM6295_status_0_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( snd_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x8000, 0x8000) AM_WRITE(soundlatch_w)
	AM_RANGE(0x8001, 0x87ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(YM3812_control_port_0_w)
	AM_RANGE(0xe001, 0xe001) AM_WRITE(YM3812_write_port_0_w)
	AM_RANGE(0xe010, 0xe010) AM_WRITE(OKIM6295_data_0_w)
ADDRESS_MAP_END

INPUT_PORTS_START( oneshot )
	PORT_START	/* DSW 1    (0x19c020.l -> 0x08006c.l) */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coinage ) )		// 0x080084.l : credits (00-09)
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x0c, 0x00, "Gun X Shift Left" )		// 0x0824ec.l (not in "test mode")
	PORT_DIPSETTING(    0x04, "30" )
	PORT_DIPSETTING(    0x00, "35" )
	PORT_DIPSETTING(    0x08, "40" )
	PORT_DIPSETTING(    0x0c, "50" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Demo_Sounds ) )	// 0x082706.l - to be confirmed
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_SERVICE( 0x20, IP_ACTIVE_HIGH )			// 0x0824fe.l
	PORT_DIPNAME( 0x40, 0x00, "Start Round" )			// 0x08224e.l
	PORT_DIPSETTING(    0x00, "Gun Trigger" )
	PORT_DIPSETTING(    0x40, "Start Button" )
	PORT_DIPNAME( 0x80, 0x00, "Gun Test" )			// 0x082286.l
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START	/* DSW 2    (0x19c024.l -> 0x08006e.l) */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )		// 0x082500.l
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x03, "5" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )	// 0x082506.l
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )				// 0
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )			// 1
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )				// 2
	PORT_DIPSETTING(    0x30, DEF_STR( Hardest ) )			// 3
	PORT_DIPNAME( 0x40, 0x00, "Round Select" )		// 0x082f16.l - only after 1st stage
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )		// "On"  in the "test mode"
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )			// "Off" in the "test mode"
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Free_Play ) )	// 0x0800ca.l
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START	/* Credits  (0x19c02c.l -> 0x08007a.l) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* Player 1 Gun Trigger (0x19c030.l) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* Player 2 Gun Trigger (0x19c034.l) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* Player 1 Gun X       ($190026.l) */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(15) PORT_PLAYER(1)

	PORT_START	/* Player 1 Gun Y       ($190036.l) */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(15) PORT_PLAYER(1)

	PORT_START	/* Player 2 Gun X       ($19002e.l) */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(15) PORT_PLAYER(2)

	PORT_START	/* Player 2 Gun Y       ($19003e.l) */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_MINMAX(0,0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(15) PORT_PLAYER(2)

INPUT_PORTS_END

INPUT_PORTS_START( maddonna )
	PORT_START /* DSW A */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x04, 0x04, "Girl Pictures" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )		// Not defined in the manual
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )		// Not defined in the manual
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "Invulnerability" )		// This one was not defined in the manual
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_HIGH )

	PORT_START /* DSW B */
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )				// 2 Monsters at start, but "dumber"??
	PORT_DIPSETTING(    0x01, DEF_STR( Normal ) )			// 2 Monsters at start
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )				// 3 Monsters at start
	PORT_DIPSETTING(    0x03, DEF_STR( Hardest ) )			// 4 Monsters at start
	PORT_DIPNAME( 0x0c, 0x08, "Time Per Round" )
	PORT_DIPSETTING(    0x08, "80 Seconds" )
	PORT_DIPSETTING(    0x04, "90 Seconds" )
	PORT_DIPSETTING(    0x00, "100 Seconds" )
//  PORT_DIPSETTING(    0x0c, "?? Seconds" )        // Not Defined for On+On
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x10, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )		// Not defined in the manual
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0x00, "Hurry Up!" )			// Controls "Hurry Up!" banner & Vampire - Not defined the in manual
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )		// No Hurry up
	PORT_DIPSETTING(    0x40, "On - 10" )			// The rest show the banner but is there a difference in how the Vampire shows up???
	PORT_DIPSETTING(    0x80, "On - 01" )
	PORT_DIPSETTING(    0xc0, "On - 11" )

	PORT_START	/* Credits */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* Player 1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* Player 1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END


static const gfx_layout oneshot16x16_layout =
{
	16,16,
	RGN_FRAC(1,8),
	8,
	{ RGN_FRAC(0,8),RGN_FRAC(1,8),RGN_FRAC(2,8),RGN_FRAC(3,8),RGN_FRAC(4,8),RGN_FRAC(5,8),RGN_FRAC(6,8),RGN_FRAC(7,8) },
	{ 0,1,2,3,4,5,6,7,
	 64+0,64+1,64+2,64+3,64+4,64+5,64+6,64+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
	 128+0*8, 128+1*8, 128+2*8, 128+3*8, 128+4*8, 128+5*8, 128+6*8, 128+7*8 },
	16*16
};

static const gfx_layout oneshot8x8_layout =
{
	8,8,
	RGN_FRAC(1,8),
	8,
	{ RGN_FRAC(0,8),RGN_FRAC(1,8),RGN_FRAC(2,8),RGN_FRAC(3,8),RGN_FRAC(4,8),RGN_FRAC(5,8),RGN_FRAC(6,8),RGN_FRAC(7,8) },
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &oneshot16x16_layout,   0x00, 4  }, /* sprites */
	{ REGION_GFX1, 0, &oneshot8x8_layout,     0x00, 4  }, /* sprites */
	{ -1 } /* end of array */
};

static void irq_handler(int irq)
{
	cpunum_set_input_line(1, 0, irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM3812interface ym3812_interface =
{
	irq_handler
};

static MACHINE_DRIVER_START( oneshot )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 12000000)
	MDRV_CPU_PROGRAM_MAP(oneshot_readmem,oneshot_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD(Z80, 5000000)
	MDRV_CPU_PROGRAM_MAP(snd_readmem, snd_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_GFXDECODE(gfxdecodeinfo)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*16, 32*16)
	MDRV_VISIBLE_AREA(0*16, 20*16-1, 0*16, 15*16-1)
	MDRV_PALETTE_LENGTH(0x400)

	MDRV_VIDEO_START(oneshot)
	MDRV_VIDEO_UPDATE(oneshot)

	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3812, 3500000)
	MDRV_SOUND_CONFIG(ym3812_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD(OKIM6295, 8000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( maddonna )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(oneshot)

	/* video hardware */
	MDRV_VIDEO_UPDATE(maddonna) // no crosshair
MACHINE_DRIVER_END


ROM_START( oneshot )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "1shot-u.a24", 0x00000, 0x20000, CRC(0ecd33da) SHA1(d050e9a1900cd9f629818034b1445e034b6cf81c) )
	ROM_LOAD16_BYTE( "1shot-u.a22", 0x00001, 0x20000, CRC(26c3ae2d) SHA1(47e479abe06d508a9d9fe677d34d6a485bde5533) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 Code */
	ROM_LOAD( "1shot.ua2", 0x00000, 0x010000, CRC(f655b80e) SHA1(2574a812c35801755c187a47f46ccdb0983c5feb) )

	ROM_REGION( 0x400000, REGION_GFX1, 0 ) /* Sprites */
	ROM_LOAD( "1shot-ui.16a",0x000000, 0x080000, CRC(f765f9a2) SHA1(f6c386e0421fcb0e420585dd27d9dad951bb2556) )
	ROM_LOAD( "1shot-ui.13a",0x080000, 0x080000, CRC(3361b5d8) SHA1(f7db674d479765d4e58fb663aa5e13dde2abcce7) )
	ROM_LOAD( "1shot-ui.11a",0x100000, 0x080000, CRC(8f8bd027) SHA1(fbec952ab5604c8e20c5e7cfd2844f4fe5441186) )
	ROM_LOAD( "1shot-ui.08a",0x180000, 0x080000, CRC(254b1701) SHA1(163bfa70508fca20be70dd0af8b768ab6bf211b9) )
	ROM_LOAD( "1shot-ui.16", 0x200000, 0x080000, CRC(ff246b27) SHA1(fef6029030268174ef9648b8f437aeda68475346) )
	ROM_LOAD( "1shot-ui.13", 0x280000, 0x080000, CRC(80342e83) SHA1(2ac2b300382a607a539d2b0982ab596f05be3ad3) )
	ROM_LOAD( "1shot-ui.11", 0x300000, 0x080000, CRC(b8938345) SHA1(318cf0d070db786680a45811bbd765fa37caaf62) )
	ROM_LOAD( "1shot-ui.08", 0x380000, 0x080000, CRC(c9953bef) SHA1(21917a9dcc0afaeec20672ad863d0c9d583369e3) )

	ROM_REGION( 0x100000, REGION_SOUND1, 0 ) /* Samples */
	ROM_LOAD( "1shot.u15", 0x000000, 0x080000, CRC(e3759a47) SHA1(1159335924a6d68a0a24bfbe0c9182107f3f05f8) )
	ROM_LOAD( "1shot.u14", 0x080000, 0x080000, CRC(222e33f8) SHA1(2665afdf4cb1a29325df62efc1843a4b2cf34a4e) )

	ROM_REGION( 0x10000, REGION_USER1, 0 )
	ROM_LOAD( "1shot.mb", 0x00000, 0x10000, CRC(6b213183) SHA1(599c59d155d11edb151bfaed1d24ef964462a447) ) // motherboard rom, zooming?
ROM_END

ROM_START( maddonna )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "maddonna.b16", 0x00000, 0x20000, CRC(643f9054) SHA1(77907ecdb02a525f9beed7fee203431eda16c831) )
	ROM_LOAD16_BYTE( "maddonna.b15", 0x00001, 0x20000, CRC(e36c0e26) SHA1(f261b2c74eeca05df302aa4956f5d02121d42054) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 Code */
	ROM_LOAD( "x13.ua2", 0x00000, 0x010000, CRC(f2080071) SHA1(68cbae9559879b2dc19c41a7efbd13ab4a569d3f) ) // b13

	ROM_REGION( 0x400000, REGION_GFX1, 0 ) /* Sprites */
	ROM_LOAD( "maddonna.b5",  0x000000, 0x080000, CRC(838d3244) SHA1(7339143481ec043219825f282450ff53bb718f8c) )
	ROM_LOAD( "maddonna.b7",  0x080000, 0x080000, CRC(4920d2ec) SHA1(e72a374bca81ffa4f925326455e007df7227ae08) )
	ROM_LOAD( "maddonna.b9",  0x100000, 0x080000, CRC(3a8a3feb) SHA1(832654902963c163644134431fd1221e1895cfec) )
	ROM_LOAD( "maddonna.b11", 0x180000, 0x080000, CRC(6f9b7fdf) SHA1(14ced1d43eae3b6db4a0a4c12fb26cbd13eb7428) )
	ROM_LOAD( "maddonna.b6",   0x200000, 0x080000, CRC(b02e9e0e) SHA1(6e527a2bfda0f4f420c10139c75dac2704e08d08) )
	ROM_LOAD( "maddonna.b8",   0x280000, 0x080000, CRC(03f1de40) SHA1(bb0c0525155404c0740ac5f048f71ae7651a5941) )
	ROM_LOAD( "maddonna.b10",  0x300000, 0x080000, CRC(87936423) SHA1(dda42f3685427edad7686d9712ff07d2fd9bf57e) )
	ROM_LOAD( "maddonna.b12",  0x380000, 0x080000, CRC(879ab23c) SHA1(5288016542a10e60ccb28a930d8dfe4db41c6fc6) )

	ROM_REGION( 0x100000, REGION_SOUND1, ROMREGION_ERASE00 ) /* Samples */
	/* no samples for this game */

	ROM_REGION( 0x10000, REGION_USER1, 0 )
	ROM_LOAD( "x1", 0x00000, 0x10000, CRC(6b213183) SHA1(599c59d155d11edb151bfaed1d24ef964462a447) ) // motherboard rom, zooming?
ROM_END

ROM_START( maddonnb )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 Code */
	/* program roms missing in this dump, gfx don't seem 100% correct for other ones */
	ROM_LOAD16_BYTE( "maddonnb.b16", 0x00000, 0x20000, NO_DUMP )
	ROM_LOAD16_BYTE( "maddonnb.b15", 0x00001, 0x20000, NO_DUMP )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* Z80 Code */
	ROM_LOAD( "x13.ua2", 0x00000, 0x010000, CRC(f2080071) SHA1(68cbae9559879b2dc19c41a7efbd13ab4a569d3f) )

	ROM_REGION( 0x400000, REGION_GFX1, 0 ) /* Sprites */
	ROM_LOAD( "x5.16a",  0x000000, 0x080000, CRC(1aae0ad3) SHA1(a5afe699c66dcc5e7928807ae1c8be7ffdda798c) )
	ROM_LOAD( "x7.13a",  0x080000, 0x080000, CRC(39d13e25) SHA1(bfe8b187c7fc9dc1ac2cc3f840a686a25ec55340) )
	ROM_LOAD( "x9.11a",  0x100000, 0x080000, CRC(2027faeb) SHA1(cb8c697705ac70ec3cf74901a2becf6abd8be63d) )
	ROM_LOAD( "x11.08a", 0x180000, 0x080000, CRC(4afcfba6) SHA1(b3fff9217db2770e703bf8317a718aeee1e5c44d) )
	ROM_LOAD( "x6.16",   0x200000, 0x080000, CRC(7b893e78) SHA1(d38c5b159031976e7864021e59cc4fff61ffb53f) )
	ROM_LOAD( "x8.13",   0x280000, 0x080000, CRC(fed90a1f) SHA1(e2cff7ce24697308c50dadb0c042d87f3e46abdb) )
	ROM_LOAD( "x10.11",  0x300000, 0x080000, CRC(479d718c) SHA1(4fbc2568744cf78b15c6e0f3caba4d7109743cdd) )
	ROM_LOAD( "x12.08",  0x380000, 0x080000, CRC(d56ca9f8) SHA1(49bca5dbc048e7b7efa34e1c08ee1b76767ffe38) )

	ROM_REGION( 0x100000, REGION_SOUND1, ROMREGION_ERASE00 ) /* Samples */
	/* no samples for this game */

	ROM_REGION( 0x10000, REGION_USER1, 0 )
	ROM_LOAD( "x1", 0x00000, 0x10000, CRC(6b213183) SHA1(599c59d155d11edb151bfaed1d24ef964462a447) ) // motherboard rom, zooming?
ROM_END





GAME( 199?, oneshot,  0,        oneshot,  oneshot , 0, ROT0, "<unknown>", "One Shot One Kill", GAME_IMPERFECT_GRAPHICS ,2)
GAME( 1995, maddonna, 0,        maddonna, maddonna, 0, ROT0, "Tuning",  "Mad Donna (set 1)", 0 ,0)
GAME( 1995, maddonnb, maddonna, maddonna, maddonna, 0, ROT0, "Tuning",  "Mad Donna (set 2)", GAME_NOT_WORKING ,0)
