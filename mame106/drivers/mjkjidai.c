/***************************************************************************

Mahjong Kyou Jidai     (c)1986 Sanritsu

CPU :Z80
Sound   :SN76489*2 CUSTOM
OSC :10MHz ??MHz

driver by Nicola Salmoria

TODO:
- Dip switches.

- Several imperfections with sprites rendering:
  - some sprites are misplaced by 1pixel vertically
  - during the tile distribution at the beginning of a match, there's something
    wrong with the stacks moved around, they aremisaligned and something is
    missing.

- unknown reads from port 01. Only the top two bits seem to be used.

***************************************************************************/

#include "driver.h"
#include "streams.h"
#include "sound/sn76496.h"
#include "sound/okim6295.h"
#include "sound/custom.h"

extern UINT8 *mjkjidai_videoram;

VIDEO_START( mjkjidai );
VIDEO_UPDATE( mjkjidai );
WRITE8_HANDLER( mjkjidai_videoram_w );
WRITE8_HANDLER( mjkjidai_ctrl_w );


/* Start of ADPCM custom chip code */
static struct mjkjidai_adpcm_state
{
	struct adpcm_state adpcm;
	sound_stream *stream;
	UINT32 current, end;
	UINT8 nibble;
	UINT8 playing;
	UINT8 *base;
} mjkjidai_adpcm;

static void mjkjidai_adpcm_callback (void *param, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	struct mjkjidai_adpcm_state *state = param;
	stream_sample_t *dest = outputs[0];

	while (state->playing && samples > 0)
	{
		int val = (state->base[state->current] >> state->nibble) & 15;

		state->nibble ^= 4;
		if (state->nibble == 4)
		{
			state->current++;
			if (state->current >= state->end)
				state->playing = 0;
		}

		*dest++ = clock_adpcm(&state->adpcm, val);
		samples--;
	}
	while (samples > 0)
	{
		*dest++ = 0;
		samples--;
	}
}

void *mjkjidai_adpcm_start (int clock, const struct CustomSound_interface *config)
{
	struct mjkjidai_adpcm_state *state = &mjkjidai_adpcm;
	state->playing = 0;
	state->stream = stream_create(0, 1, clock, state, mjkjidai_adpcm_callback);
	state->base = memory_region(REGION_SOUND1);
	reset_adpcm(&state->adpcm);
	return state;
}

static void mjkjidai_adpcm_play (int offset, int lenght)
{
	mjkjidai_adpcm.current = offset;
	mjkjidai_adpcm.end = offset + lenght/2;
	mjkjidai_adpcm.nibble = 4;
	mjkjidai_adpcm.playing = 1;
}

static WRITE8_HANDLER( adpcm_w )
{
	mjkjidai_adpcm_play ((data & 0x07) * 0x1000, 0x1000 * 2);
}
/* End of ADPCM custom chip code */


static int keyb,nvram_init_count;

static READ8_HANDLER( keyboard_r )
{
	int res = 0x3f,i;

//  logerror("%04x: keyboard_r\n",activecpu_get_pc());

	for (i = 0;i < 12;i++)
	{
		if (~keyb & (1 << i))
		{
			res = readinputport(4+i) & 0x3f;
			break;
		}
	}

	res |= (readinputport(3) & 0xc0);

	if (nvram_init_count)
	{
		nvram_init_count--;
		res &= 0xbf;
	}

	return res;
}

static WRITE8_HANDLER( keyboard_select_w )
{
//  logerror("%04x: keyboard_select %d = %02x\n",activecpu_get_pc(),offset,data);

	switch (offset)
	{
		case 0: keyb = (keyb & 0xff00) | (data);      break;
		case 1: keyb = (keyb & 0x00ff) | (data << 8); break;
	}
}

static UINT8 *nvram;
static size_t nvram_size;

void nvram_handler_mjkjidai(mame_file *file, int read_or_write)
{
	if (read_or_write)
		mame_fwrite(file, nvram, nvram_size);
	else if (file)
		mame_fread(file, nvram, nvram_size);
	else
	{
		nvram_init_count = 1;
	}
}



static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0xbfff) AM_READ(MRA8_BANK1)
	AM_RANGE(0xc000, 0xdfff) AM_READ(MRA8_RAM)
	AM_RANGE(0xe000, 0xf7ff) AM_READ(MRA8_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xcfff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xd000, 0xdfff) AM_WRITE(MWA8_RAM) AM_BASE(&nvram) AM_SIZE(&nvram_size)	// cleared and initialized on startup if bit 6 if port 00 is 0
	AM_RANGE(0xe000, 0xe01f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram)	// shared with tilemap ram
	AM_RANGE(0xe800, 0xe81f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_2)	// shared with tilemap ram
	AM_RANGE(0xf000, 0xf01f) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_3)	// shared with tilemap ram
	AM_RANGE(0xe000, 0xf7ff) AM_WRITE(mjkjidai_videoram_w) AM_BASE(&mjkjidai_videoram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(keyboard_r)
	AM_RANGE(0x01, 0x01) AM_READ(MRA8_NOP)	// ???
	AM_RANGE(0x02, 0x02) AM_READ(input_port_2_r)
	AM_RANGE(0x11, 0x11) AM_READ(input_port_0_r)
	AM_RANGE(0x12, 0x12) AM_READ(input_port_1_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x01, 0x02) AM_WRITE(keyboard_select_w)
	AM_RANGE(0x10, 0x10) AM_WRITE(mjkjidai_ctrl_w)	// rom bank, coin counter, flip screen etc
	AM_RANGE(0x20, 0x20) AM_WRITE(SN76496_0_w)
	AM_RANGE(0x30, 0x30) AM_WRITE(SN76496_1_w)
	AM_RANGE(0x40, 0x40) AM_WRITE(adpcm_w)
ADDRESS_MAP_END



INPUT_PORTS_START( mjkjidai )
	PORT_START
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE )	// service mode
	PORT_DIPNAME( 0x20, 0x20, "Statistics" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START4 )

	PORT_START
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_TILT )	// reinitialize NVRAM and reset the game
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	/* player 2 inputs (same as player 1) */
	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x3e, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_MAHJONG_KAN )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_RON )
	PORT_BIT( 0x38, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_MAHJONG_M )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_N )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_PON )
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_MAHJONG_I )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_J )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_K )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L )
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_MAHJONG_E )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_F )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_G )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_H )
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_MAHJONG_A )
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_MAHJONG_B )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_MAHJONG_C )
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_MAHJONG_D )
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
		8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0, 32 },
	{ REGION_GFX1, 0, &spritelayout, 0, 16 },
	{ -1 } /* end of array */
};


static struct CustomSound_interface adpcm_interface =
{
	mjkjidai_adpcm_start
};



static MACHINE_DRIVER_START( mjkjidai )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,10000000/2)	/* 5 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_IO_MAP(readport,writeport)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_NVRAM_HANDLER(mjkjidai)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER|VIDEO_PIXEL_ASPECT_RATIO_1_2)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(3*8, 61*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(0x100)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(mjkjidai)
	MDRV_VIDEO_UPDATE(mjkjidai)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(SN76496, 10000000/4)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD(SN76496, 10000000/4)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD(CUSTOM, 6000)
	MDRV_SOUND_CONFIG(adpcm_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( mjkjidai )
	ROM_REGION( 0x1c000, REGION_CPU1, 0 )
	ROM_LOAD( "mkj-00.14g",   0x00000, 0x8000, CRC(188a27e9) SHA1(2306ad112aaf8d9ac77a89d0e4c3a17f36945130) )
	ROM_LOAD( "mkj-01.15g",   0x08000, 0x4000, CRC(a6a5e9c7) SHA1(974f4343f4347a0065f833c1fdcc47e96d42932d) )	/* banked, there is code flowing from 7fff to this bank */
	ROM_CONTINUE(             0x10000, 0x4000 )
	ROM_LOAD( "mkj-02.16g",   0x14000, 0x8000, CRC(fb312927) SHA1(b71db72ba881474f9c2523d0617757889af9f28e) )

	ROM_REGION( 0x30000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mkj-20.4e",    0x00000, 0x8000, CRC(8fc66bce) SHA1(4f1006bc5168e39eb7a1f6a4b3c3f5aaa3c1c7dd) )
	ROM_LOAD( "mkj-21.5e",    0x08000, 0x8000, CRC(4dd41a9b) SHA1(780f9e5bbf9dc47e931cebd67d89122209f573a2) )
	ROM_LOAD( "mkj-22.6e",    0x10000, 0x8000, CRC(70ac2bd7) SHA1(8ddb00a24f2b49b9eb1a70ae95fcd6bb0820be50) )
	ROM_LOAD( "mkj-23.7e",    0x18000, 0x8000, CRC(f9313dde) SHA1(787577ccdc7e7030439159c194ca6719df80ad2f) )
	ROM_LOAD( "mkj-24.8e",    0x20000, 0x8000, CRC(aa5130d0) SHA1(1dbaf2ba9ed97c22dc74d12471fc54b0f7ce2f25) )
	ROM_LOAD( "mkj-25.9e",    0x28000, 0x8000, CRC(c12c3fe0) SHA1(0acd3f8e8d849a09b187cd83852593a64aa87451) )

	ROM_REGION( 0x0300, REGION_PROMS, ROMREGION_DISPOSE )
	ROM_LOAD( "mkj-60.13a",   0x0000, 0x0100, CRC(5dfaba60) SHA1(7c821a5e951ccf9d86d98aa8dc75d847ab579496) )
	ROM_LOAD( "mkj-61.14a",   0x0100, 0x0100, CRC(e9e90d55) SHA1(a14177df3bab59e0f9ce41094e03ef3593329149) )
	ROM_LOAD( "mkj-62.15a",   0x0200, 0x0100, CRC(934f1d53) SHA1(2b3b2dc77789b814810b25cda3f5adcfd7e0e57e) )

	ROM_REGION( 0x8000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "mkj-40.14c",   0x00000, 0x8000, CRC(4d8fcc4a) SHA1(24c2b8031367035c89c6649a084bce0714f3e8d4) )
ROM_END


GAME( 1986, mjkjidai, 0, mjkjidai, mjkjidai, 0, ROT0, "Sanritsu",  "Mahjong Kyou Jidai (Japan)", GAME_IMPERFECT_GRAPHICS ,1)
