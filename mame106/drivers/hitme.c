/* Hit Me driver by the EMUL8, led by Dan Boris */

/*

    Hit Me  (c) Ramtek  1976
---------------------------------------

    Memory map

    0000-07ff r    Rom
    0c00-0eff w    Video Ram
    1000-13ff r/w  Scratch Ram


*/

#include "driver.h"
#include "hitme.h"
#include "sound/discrete.h"

static tilemap *hitme_tilemap;
static mame_time timeout_time;
static UINT8 *hitme_vidram;



/*************************************
 *
 *  Video RAM access
 *
 *************************************/

static void get_hitme_tile_info(int tile_index)
{
	/* the code is the low 6 bits */
	UINT8 code = hitme_vidram[tile_index] & 0x3f;
	SET_TILE_INFO(0, code, 0, 0);
}


WRITE8_HANDLER( hitme_vidram_w )
{
	/* mark this tile dirty */
	hitme_vidram[offset] = data;
	tilemap_mark_tile_dirty(hitme_tilemap, offset);
}



/*************************************
 *
 *  Video start/update
 *
 *************************************/

static VIDEO_START(hitme)
{
	hitme_tilemap = tilemap_create(get_hitme_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE, 8,10, 40,19);
	return 0;
}


static VIDEO_START(barricad)
{
	hitme_tilemap = tilemap_create(get_hitme_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE, 8,8, 32,24);
	return 0;
}


static VIDEO_UPDATE(hitme)
{
	/* the card width resistor comes from an input port, scaled to the range 0-25 kOhms */
	double width_resist = readinputport(7) * 25000 / 100;
	/* this triggers a oneshot for the following length of time */
	double width_duration = 0.45 * 1000e-12 * width_resist;
	/* the dot clock runs at the standard horizontal frequency * 320+16 clocks per scanline */
	double dot_freq = 15750 * 336;
	/* the number of pixels is the duration times the frequency */
	int width_pixels = width_duration * dot_freq;
	int x, y, xx, inv;
	offs_t offs = 0;

	/* start by drawing the tilemap */
	tilemap_draw(bitmap,cliprect,hitme_tilemap,0,0);

	/* now loop over and invert anything */
	for (y = 0; y < 19; y++)
	{
		int dy = bitmap->rowpixels;
		for (inv = x = 0; x < 40; x++, offs++)
		{
			/* if the high bit is set, reset the oneshot */
			if (hitme_vidram[y*40+x] & 0x80)
				inv = width_pixels;

			/* invert pixels until we run out */
			for (xx = 0; xx < 8 && inv; xx++, inv--)
			{
				UINT16 *dest = (UINT16 *)bitmap->line[y*10] + x*8 + xx;
				dest[0*dy] ^= 1;
				dest[1*dy] ^= 1;
				dest[2*dy] ^= 1;
				dest[3*dy] ^= 1;
				dest[4*dy] ^= 1;
				dest[5*dy] ^= 1;
				dest[6*dy] ^= 1;
				dest[7*dy] ^= 1;
				dest[8*dy] ^= 1;
				dest[9*dy] ^= 1;
			}
		}
	}
}


static VIDEO_UPDATE(barricad)
{
	tilemap_draw(bitmap,cliprect,hitme_tilemap,0,0);
}



/*************************************
 *
 *  Input ports
 *
 *************************************/

static UINT8 read_port_and_t0(int port)
{
	UINT8 val = readinputport(port);
	if (compare_mame_times(mame_timer_get_time(), timeout_time) > 0)
		val ^= 0x80;
	return val;
}


static UINT8 read_port_and_t0_and_hblank(int port)
{
	UINT8 val = read_port_and_t0(port);
	if (cpu_gethorzbeampos() < (Machine->drv->screen_width * 9 / 10))
		val ^= 0x04;
	return val;
}


static READ8_HANDLER( hitme_port_0_r )
{
	return read_port_and_t0_and_hblank(0);
}


static READ8_HANDLER( hitme_port_1_r )
{
	return read_port_and_t0(1);
}


static READ8_HANDLER( hitme_port_2_r )
{
	return read_port_and_t0_and_hblank(2);
}


static READ8_HANDLER( hitme_port_3_r )
{
	return read_port_and_t0(3);
}



/*************************************
 *
 *  Output ports
 *
 *************************************/

static WRITE8_HANDLER( output_port_0_w )
{
	/*
        Note: We compute the timeout time on a write here. Unfortunately, the situation is
        kind of weird, because the discrete sound system is also affected by this timeout.
        In fact, it is very important that our timing calculation timeout AFTER the sound
        system's equivalent computation, or else we will hang notes.
    */
	UINT8 raw_game_speed = readinputport(6);
	double resistance = raw_game_speed * 25000 / 100;
	mame_time duration = make_mame_time(0, MAX_SUBSECONDS * 0.45 * 6.8e-6 * resistance * (data+1));
	timeout_time = add_mame_times(mame_timer_get_time(), duration);

	discrete_sound_w(HITME_DOWNCOUNT_VAL, data);
	discrete_sound_w(HITME_OUT0, 1);
}


static WRITE8_HANDLER( output_port_1_w )
{
	discrete_sound_w(HITME_ENABLE_VAL, data);
	discrete_sound_w(HITME_OUT1, 1);
}



/*************************************
 *
 *  Memory maps
 *
 *************************************/

/*
    Note: the 8080 puts I/O port addresses out on the upper 8 address bits and asserts
    IORQ. Most systems decode IORQ, but hitme doesn't, which means that all the I/O
    port accesses can also be made via memory mapped accesses with the port number in the
    upper 8 bits.
*/

static ADDRESS_MAP_START( hitme_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_FLAGS(AMEF_ABITS(13))
	AM_RANGE(0x0000, 0x07ff) AM_ROM
	AM_RANGE(0x0c00, 0x0eff) AM_READWRITE(MRA8_RAM, hitme_vidram_w) AM_BASE(&hitme_vidram)
	AM_RANGE(0x1000, 0x10ff) AM_MIRROR(0x300) AM_RAM
	AM_RANGE(0x1400, 0x14ff) AM_READ(hitme_port_0_r)
	AM_RANGE(0x1500, 0x15ff) AM_READ(hitme_port_1_r)
	AM_RANGE(0x1600, 0x16ff) AM_READ(hitme_port_2_r)
	AM_RANGE(0x1700, 0x17ff) AM_READ(hitme_port_3_r)
	AM_RANGE(0x1800, 0x18ff) AM_READ(input_port_4_r)
	AM_RANGE(0x1900, 0x19ff) AM_READ(input_port_5_r)
	AM_RANGE(0x1d00, 0x1dff) AM_WRITE(output_port_0_w)
	AM_RANGE(0x1e00, 0x1fff) AM_WRITE(output_port_1_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( hitme_portmap, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x14, 0x14) AM_READ(hitme_port_0_r)
	AM_RANGE(0x15, 0x15) AM_READ(hitme_port_1_r)
	AM_RANGE(0x16, 0x16) AM_READ(hitme_port_2_r)
	AM_RANGE(0x17, 0x17) AM_READ(hitme_port_3_r)
	AM_RANGE(0x18, 0x18) AM_READ(input_port_4_r)
	AM_RANGE(0x19, 0x19) AM_READ(input_port_5_r)
	AM_RANGE(0x1d, 0x1d) AM_WRITE(output_port_0_w)
	AM_RANGE(0x1e, 0x1f) AM_WRITE(output_port_1_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Graphics layouts
 *
 *************************************/

/*
    Note: the hitme video generator adds two blank lines to the beginning of each
    row. In order to simulate this, we decode an extra two lines at the top of each
    character.
*/

static const gfx_layout hitme_charlayout =
{
	8,10,
	RGN_FRAC(1,2),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0x200*8, 0x200*8, 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_decode hitme_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &hitme_charlayout, 0, 2  },
	{ -1 }
};


static const gfx_layout barricad_charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_decode barricad_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &barricad_charlayout,   0, 1  },
	{ -1 }
};




/*************************************
 *
 *  Machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( hitme )

	/* basic machine hardware */
	MDRV_CPU_ADD(8080, 8945000/16)
	MDRV_CPU_PROGRAM_MAP(hitme_map,0)
	MDRV_CPU_IO_MAP(hitme_portmap,0)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(40*8, 19*10)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 0*8, 19*10-1)
	MDRV_GFXDECODE(hitme_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2)

	MDRV_PALETTE_INIT(black_and_white)
	MDRV_VIDEO_START(hitme)
	MDRV_VIDEO_UPDATE(hitme)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD_TAG("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG(hitme_discrete_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END



/*
    Note: The Barricade rom is using a resolution of 32x24 which suggests slightly
    different hardware from HitMe (40x19) however the screenshot on the arcade
    flyer is using a 40x19 resolution. So is this a different version of
    Barricade or is the resolution set by a dip switch?
*/

static MACHINE_DRIVER_START( barricad )
	MDRV_IMPORT_FROM(hitme)

	/* video hardware */
	MDRV_SCREEN_SIZE(32*8, 24*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 0*8, 24*8-1)
	MDRV_GFXDECODE(barricad_gfxdecodeinfo)

	MDRV_VIDEO_START(barricad)
	MDRV_VIDEO_UPDATE(barricad)
MACHINE_DRIVER_END



/*************************************
 *
 *  Input ports
 *
 *************************************/

INPUT_PORTS_START( hitme )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )					/* Start button */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )				/* Hblank */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)	/* P1 Stand button */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)	/* P1 Hit button */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)	/* P1 Bet button */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )				/* Time out counter (*TO) */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )					/* Aux 2 dipswitch - Unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)	/* P2 Stand button */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)	/* P2 Hit button */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)	/* P2 Bet button */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )				/* Time out counter (*TO) */

	PORT_START
	PORT_DIPNAME( 0x01, 0x00, "Extra Hand On Natural" )			/* Aux 1 dipswitch */
	PORT_DIPSETTING(    0x00, DEF_STR ( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR ( On )  )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )				/* Hblank */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)	/* P3 Stand button */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)	/* P3 Hit button */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)	/* P3 Bet button */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )				/* Time out counter (*TO) */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )				/* Time out counter (TOC1) */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )					/* Aux 2 dipswitch - Unused*/
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )					/* Always high */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)	/* P4 Stand button */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)	/* P4 Hit button */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(4)	/* P4 Bet button */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )				/* Time out counter (*TO) */

	PORT_START
	PORT_DIPNAME( 0x07, 0x07, "Number of Chips" )
	PORT_DIPSETTING(    0x00, "5 Chips" )
	PORT_DIPSETTING(    0x01, "10 Chips" )
	PORT_DIPSETTING(    0x02, "15 Chips" )
	PORT_DIPSETTING(    0x03, "20 Chips" )
	PORT_DIPSETTING(    0x04, "25 Chips" )
	PORT_DIPSETTING(    0x05, "30 Chips" )
	PORT_DIPSETTING(    0x06, "35 Chips" )
	PORT_DIPSETTING(    0x07, "40 Chips" )
	PORT_BIT( 0xf8, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START
	PORT_DIPNAME( 0x07, 0x00, "Number of Hands" )
	PORT_DIPSETTING(    0x00, "5 Hands" )
	PORT_DIPSETTING(    0x01, "10 Hands" )
	PORT_DIPSETTING(    0x02, "15 Hands" )
	PORT_DIPSETTING(    0x03, "20 Hands" )
	PORT_DIPSETTING(    0x04, "25 Hands" )
	PORT_DIPSETTING(    0x05, "30 Hands" )
	PORT_DIPSETTING(    0x06, "35 Hands" )
	PORT_DIPSETTING(    0x07, "40 Hands" )
	PORT_BIT( 0xf8, IP_ACTIVE_HIGH, IPT_UNUSED )

	/* this is actually a variable resistor */
	PORT_START
	PORT_ADJUSTER(30, "Game Speed")

	/* this is actually a variable resistor */
	PORT_START
	PORT_ADJUSTER(50, "Card Width")
INPUT_PORTS_END


INPUT_PORTS_START( barricad )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )							/* Start button */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )							/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )						/* Hblank */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT  ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP  ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )						/* Time out counter (*TO) */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )							/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )							/* Aux 2 dipswitch - Unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT  ) PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP  ) PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )						/* Time out counter (*TO) */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )						/* ??? */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )							/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )						/* Hblank */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT  ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP  ) PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )						/* Time out counter (*TO) */

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )						/* Time out counter (TOC1) */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )						/* Always high */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )						/* Aux 2 dipswitch - Unused*/
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT  ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP  ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )						/* Time out counter (*TO) */

	/* On the flyer it says that barricade has both user adjustable points per
        game, and speed. From experimenting it looks like points per game is the
        same dipswitch as hitme's chips, and speed is hitme's hands. The flyer
      says 1-7 points per games, but it really can go to 8. */

	PORT_START
	PORT_DIPNAME( 0x07, 0x07, "Points Per Game" )
	PORT_DIPSETTING(    0x00, "1 Point" )
	PORT_DIPSETTING(    0x01, "2 Points" )
	PORT_DIPSETTING(    0x02, "3 Points" )
	PORT_DIPSETTING(    0x03, "4 Points" )
	PORT_DIPSETTING(    0x04, "5 Points" )
	PORT_DIPSETTING(    0x05, "6 Points" )
	PORT_DIPSETTING(    0x06, "7 Points" )
	PORT_DIPSETTING(    0x07, "8 Points" )

	/* These are like lives, you lose a point if you crash. The last person with
        points wins the game. */

	PORT_START
	PORT_DIPNAME( 0x07, 0x00, "Game Speed" )
	PORT_DIPSETTING(    0x00, "Fast Fast" )
	PORT_DIPSETTING(    0x01, "7" )
	PORT_DIPSETTING(    0x02, "6" )
	PORT_DIPSETTING(    0x03, "5" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x05, "3" )
	PORT_DIPSETTING(    0x06, "2" )
	PORT_DIPSETTING(    0x07, "Slow Slow" )
INPUT_PORTS_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( hitme )
	ROM_REGION( 0x2000, REGION_CPU1, ROMREGION_INVERT )
	ROM_LOAD( "hm0.b7", 0x0000, 0x0200, CRC(6c48c50f) SHA1(42dc7c3461687e5be4393cc21d695bc84ae4f5dc) )
	ROM_LOAD( "hm2.c7", 0x0200, 0x0200, CRC(25d47ba4) SHA1(6f3bb4ca6918dc07f37d0c0c7fe5ec53aa7171a5) )
	ROM_LOAD( "hm4.d7", 0x0400, 0x0200, CRC(f8bfda8d) SHA1(48bbc106f8d80d6c1ad1a2c1575ce7d6452fbe9d) )
	ROM_LOAD( "hm6.e7", 0x0600, 0x0200, CRC(8aa87118) SHA1(aca395a4f6a1981cd89ca99e05935d72adcb69ca) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_ERASE00 )
    ROM_LOAD( "hmcg.h7", 0x0000, 0x0200, CRC(818f5fbe) SHA1(e2b3349e51ba57d14f3388ba93891bc6274b7a14) )
ROM_END


ROM_START( mblkjack )
	ROM_REGION( 0x2000, REGION_CPU1, ROMREGION_INVERT )
	ROM_LOAD( "mirco1.bin", 0x0000, 0x0200, CRC(aa796ad7) SHA1(2908bdb4ab17a2f5bc4da2f957906bf2b57afa50) )
	ROM_LOAD( "hm2.c7", 0x0200, 0x0200, CRC(25d47ba4) SHA1(6f3bb4ca6918dc07f37d0c0c7fe5ec53aa7171a5) )
	ROM_LOAD( "hm4.d7", 0x0400, 0x0200, CRC(f8bfda8d) SHA1(48bbc106f8d80d6c1ad1a2c1575ce7d6452fbe9d) )
	ROM_LOAD( "hm6.e7", 0x0600, 0x0200, CRC(8aa87118) SHA1(aca395a4f6a1981cd89ca99e05935d72adcb69ca) )

	ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_ERASE00 )
    ROM_LOAD( "hmcg.h7", 0x0000, 0x0200, CRC(818f5fbe) SHA1(e2b3349e51ba57d14f3388ba93891bc6274b7a14) )
ROM_END


ROM_START( barricad )
   ROM_REGION( 0x2000, REGION_CPU1, ROMREGION_INVERT )
   ROM_LOAD( "550806.7b",   0x0000, 0x0200, CRC(ea7f5da7) SHA1(c0ad37a0ffdb0500e8adc8fb9c4369e461307f84) )
   ROM_LOAD( "550807.7c",   0x0200, 0x0200, CRC(0afef174) SHA1(2a7be988262b855bc81a1b0036fa9f2481d4d53b) )
   ROM_LOAD( "550808.7d",   0x0400, 0x0200, CRC(6e02d260) SHA1(8a1640a1d56cbc34f74f07bc15e77db63635e8f5) )
   ROM_LOAD( "550809.7e",   0x0600, 0x0200, CRC(d834a63f) SHA1(ffb631cc4f51a670c7cd30df1c79bf51301d9e9a) )

   ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
   ROM_LOAD( "550805.7h",   0x0000, 0x0200, CRC(35197599) SHA1(3c49af89b1bc1d495e1d6265ff3feaf33c56facb) )
ROM_END


ROM_START( brickyrd )
   ROM_REGION( 0x2000, REGION_CPU1, ROMREGION_INVERT )
   ROM_LOAD( "550806.7b",   0x0000, 0x0200, CRC(ea7f5da7) SHA1(c0ad37a0ffdb0500e8adc8fb9c4369e461307f84) )
   ROM_LOAD( "barricad.7c", 0x0200, 0x0200, CRC(94e1d1c0) SHA1(f6e6f9a783867c3602ba8cff6a18c47c5df987a4) )
   ROM_LOAD( "550808.7d",   0x0400, 0x0200, CRC(6e02d260) SHA1(8a1640a1d56cbc34f74f07bc15e77db63635e8f5) )
   ROM_LOAD( "barricad.7e", 0x0600, 0x0200, CRC(2b1d914f) SHA1(f1a6631949a7c62f5de39d58821e1be36b98629e) )

   ROM_REGION( 0x0400, REGION_GFX1, ROMREGION_DISPOSE )
   ROM_LOAD( "barricad.7h", 0x0000, 0x0200, CRC(c676fd22) SHA1(c37bf92f5a146a93bd977b2a05485addc00ab066) )
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1976, hitme,    0,        hitme,    hitme,    0, ROT0, "RamTek", "Hit Me", 0 ,4)
GAME( 197?, mblkjack, hitme,    hitme,    hitme,    0, ROT0, "Mirco", "Black Jack (Mirco)", 0 ,4)
GAME( 1976, barricad, 0,        barricad, barricad, 0, ROT0, "RamTek", "Barricade", GAME_IMPERFECT_SOUND  ,4)
GAME( 1976, brickyrd, barricad, barricad, barricad, 0, ROT0, "RamTek", "Brickyard", GAME_IMPERFECT_SOUND  ,4)
