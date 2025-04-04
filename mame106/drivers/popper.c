/*
Popper

Omori Electric CAD (OEC) 1983


PPR-12


18.432MHz                    6148 6148
                             6148 4148
                2128
                2128
                2128
 2128
            p3                p6
z80A        p2                p5
  p0        p1             p4
            z80A
   8910
 8910              SW2 SW1    p.m4 p.m3

Notes:

- a lower interleave causes some sounds to miss, different interleave values
  change random elements eg position of falling logs, it may be enough to
  synchronise the cpus on read/writes to the 0xdd00/0f range

- it's possible that the two score rows shouldn't be overlaid or shouldn't be
  drawn at all times, the only time there is anything underneath is the colour
  bars on the startup screen

- on odd/bonus levels the chars are not aligned with the sprites when the pylons
  are initially drawn (too low) resulting in the top 3 pylons being black, as
  the pylons move up the screen the chars go out of alignment again (this time
  too high) for a single frame to show another glitch

- front to back ordering of object/enemy sprites appears wrong

- on even levels the front most balloon carrying new enemies swaps between the
  left and middle one, clipping to 16 pixel strips solves this but causes other
  obvious sprite drawing errors

- balloon sprites not displayed on high score entry due to the way videoram is
  being drawn, possibly trying to create a stencil effect

- bursting a balloon on bonus levels just before dying from a log causes a
  sprite glitch for a few frames and then a 9999 point bonus

- reads from 0xffff appear to be a harmless bug in the code that initialises
  memory blocks for enemies on odd levels, it goes on to read from 0x001f,
  0x003f, 0x005f, 0x007f in the rom before giving up

- a reset while a game is in progress or with credits inserted shows a
  "sorry!! over run" message instead of the test screen during startup,
  looks like the game assumes the reset was caused by the watchdog after a
  hang/crash and is apologising to the player for the loss of their game

- title screen sprite colour glitch in lower right corner of the large pylon
  when an even level was shown before, caused by the game trying to colour
  the top of the pylon in the same way as for odd levels

- enemies stop moving on even levels on rare occasions

- enemy sprites entering from the bridges on odd levels are clipped by a
  few pixels at the top, this would happen even without the overlay as the
  first few rows of videoram are high priority solid chars most of the time

- unknown 0xe002 writes, on for title and level 2 off for all else, looks
  to be graphic related

- bottom of screen is scrappy on odd levels, amongst other things the sprites
  leave early, screen size could be clipped tighter or there could be some
  special handling needed, possibly tied to the 0xe002 writes

- the game freezes in much the same way as the stop dip switch when the coin
  inputs are high (a short PORT_IMPULSE duration will still cause a hitch),
  so potentially there's a coin lockout mechanism
*/

#include "driver.h"
#include "sound/ay8910.h"

extern UINT8 *popper_videoram, *popper_attribram, *popper_ol_videoram, *popper_ol_attribram, *popper_spriteram;
extern size_t popper_spriteram_size;

WRITE8_HANDLER( popper_videoram_w );
WRITE8_HANDLER( popper_attribram_w );
WRITE8_HANDLER( popper_ol_videoram_w );
WRITE8_HANDLER( popper_ol_attribram_w );
WRITE8_HANDLER( popper_flipscreen_w );
WRITE8_HANDLER( popper_e002_w );
WRITE8_HANDLER( popper_gfx_bank_w );

PALETTE_INIT( popper );
VIDEO_START( popper );
VIDEO_UPDATE( popper );
static UINT8 *popper_sharedram;

static READ8_HANDLER( popper_sharedram_r )
{
	return popper_sharedram[offset];
}

//e000                  e001                  e002                  e003
//76543210              76543210              76543210              76543210
//x-------  unused      x-------  unused      x-------  unused      x-------  unused
//-x------  dsw1:1      -x------  dsw1:2      -x------  dsw1:3      -x------  dsw1:4
//--x-----  unused      --x-----  unused      --x-----  unused      --x-----  unused
//---x----  dsw2:1      ---x----  dsw2:2      ---x----  dsw2:3      ---x----  dsw2:4
//----x---  service     ----xxxx  p1 udlr     ----x---  unused      ----xxxx  p2 udlr
//-----x--  coin a                            -----x--  coin b
//------x-  start 1                           ------x-  start 2
//-------x  p1 b1                             -------x  p2 b1
//
//e004                  e005                  e006                  e007
//x-------  dsw1:5      x-------  dsw1:6      x-------  dsw1:7      x-------  dsw1:8
//-x------  unused      -x------  unused      -x------  unused      -x------  unused
//--x-----  dsw2:5      --x-----  dsw2:6      --x-----  dsw2:7      --x-----  dsw2:8
//---xxxxx  unused      ---xxxxx  unused      ---xxxxx  unused      ---xxxxx  unused
//
//dsw1                  dsw2
//87654321              87654321
//xx------  extra       x-------  stop
//--xx----  poppers     -x------  clear (current level)
//----xx--  coin b      --x-----  upright (cabinet)
//------xx  coin a      ---x----  crt dir. (flip screen)
//                      ----x---  pass (unlimited lives)
//                      -----x--  free play
//                      ------x-  continue
//                      -------x  sound
static READ8_HANDLER( popper_input_ports_r )
{
	UINT8 data=0;
	switch (offset)
	{
		//           player inputs        dsw1                           dsw2
		case 0: data=readinputport(0) | ((readinputport(4)&0x02)<<5) | ((readinputport(5)&0x01)<<4); break;
		case 1: data=readinputport(1) | ((readinputport(4)&0x01)<<6) | ((readinputport(5)&0x02)<<3); break;
		case 2: data=readinputport(2) | ((readinputport(4)&0x08)<<3) | ((readinputport(5)&0x04)<<2); break;
		case 3: data=readinputport(3) | ((readinputport(4)&0x04)<<4) | ((readinputport(5)&0x08)<<1); break;
		case 4: data=                   ((readinputport(4)&0x20)<<2) | ((readinputport(5)&0x10)<<1); break;
		case 5: data=                   ((readinputport(4)&0x10)<<3) | ((readinputport(5)&0x20)<<0); break;
		case 6: data=                   ((readinputport(4)&0x80)<<0) | ((readinputport(5)&0x40)>>1); break;
		case 7: data=                   ((readinputport(4)&0x40)<<1) | ((readinputport(5)&0x80)>>2); break;
	}
	return data;
}

static READ8_HANDLER( popper_soundcpu_nmi_r )
{
	cpunum_set_input_line(1, INPUT_LINE_NMI, PULSE_LINE);
	return 0;
}

static WRITE8_HANDLER( popper_sharedram_w )
{
	popper_sharedram[offset]=data;
}

static ADDRESS_MAP_START( popper_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xd7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xd800, 0xdfff) AM_READ(MRA8_RAM)					//shared with sound cpu
	AM_RANGE(0xe000, 0xe007) AM_READ(popper_input_ports_r)
	AM_RANGE(0xe400, 0xe400) AM_READ(popper_soundcpu_nmi_r)
	AM_RANGE(0xf800, 0xf800) AM_READ(MRA8_NOP)					//?? read once at startup
	AM_RANGE(0xfc00, 0xfc00) AM_READ(MRA8_NOP)					//?? possibly watchdog
	AM_RANGE(0xffff, 0xffff) AM_READ(MRA8_NOP)
ADDRESS_MAP_END

static ADDRESS_MAP_START( popper_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc1bf) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xc1c0, 0xc1ff) AM_WRITE(popper_ol_videoram_w) AM_BASE(&popper_ol_videoram)
	AM_RANGE(0xc200, 0xc61f) AM_WRITE(popper_videoram_w) AM_BASE(&popper_videoram)
	AM_RANGE(0xc620, 0xc9bf) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xc9c0, 0xc9ff) AM_WRITE(popper_ol_attribram_w) AM_BASE(&popper_ol_attribram)
	AM_RANGE(0xca00, 0xce1f) AM_WRITE(popper_attribram_w) AM_BASE(&popper_attribram)
	AM_RANGE(0xce20, 0xcfff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xd000, 0xd7ff) AM_WRITE(MWA8_RAM) AM_BASE(&popper_spriteram) AM_SIZE(&popper_spriteram_size)
	AM_RANGE(0xd800, 0xdfff) AM_WRITE(MWA8_RAM) AM_BASE(&popper_sharedram)	//shared with sound cpu
	AM_RANGE(0xe000, 0xe000) AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xe001, 0xe001) AM_WRITE(popper_flipscreen_w)
	AM_RANGE(0xe002, 0xe002) AM_WRITE(popper_e002_w)				//?? seems to be graphic related
	AM_RANGE(0xe003, 0xe003) AM_WRITE(popper_gfx_bank_w)
	AM_RANGE(0xe004, 0xe007) AM_WRITE(MWA8_NOP)					//?? range cleared once when the SP is set
ADDRESS_MAP_END

static ADDRESS_MAP_START( popper_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8002, 0x8002) AM_READ(MRA8_NOP)					//?? all read once at startup and the
	AM_RANGE(0x8003, 0x8003) AM_READ(MRA8_NOP)					//?? result ignored, looks like part
	AM_RANGE(0xa002, 0xa002) AM_READ(MRA8_NOP)					//?? of AY8910 initialisation
	AM_RANGE(0xd800, 0xdfff) AM_READ(popper_sharedram_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( popper_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x8000, 0x8000) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x8001, 0x8001) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x8002, 0x8002) AM_WRITE(MWA8_NOP)					//?? same writes as 0x8000 (mostly)
	AM_RANGE(0xa000, 0xa000) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0xa001, 0xa001) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0xa002, 0xa002) AM_WRITE(MWA8_NOP)					//?? same writes as 0xa000
	AM_RANGE(0xd800, 0xdfff) AM_WRITE(popper_sharedram_w)
ADDRESS_MAP_END

INPUT_PORTS_START( popper )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 )			//ignored if held for 12 or more frames
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE1 )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY

	PORT_START_TAG("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN2 )			//ignored if held for 12 or more frames

	PORT_START_TAG("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL

	PORT_START_TAG("IN4")	/* FAKE DSW1 */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coin_A ) )		//SW1:1-2
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x0c, 0x08, DEF_STR( Coin_B ) )		//SW1:3-4
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x30, 0x10, DEF_STR( Lives ) )		//SW1:5-6
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x10, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPSETTING(    0x30, "5" )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Bonus_Life ) )	//SW1:7-8
	PORT_DIPSETTING(    0x00, "20k, then every 70k" )
	PORT_DIPSETTING(    0x40, "30k, then every 70k" )
	PORT_DIPSETTING(    0x80, "40k, then every 70k" )
	PORT_DIPSETTING(    0xc0, "50k, then every 70k" )

	PORT_START_TAG("IN5")	/* FAKE DSW2 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Demo_Sounds ) )	//SW2:1
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Allow_Continue ) )		//SW2:2 (stored in 0xd987, never read)
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Free_Play ) )	//SW2:3
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Pass (Unlimited Lives) (Cheat)")	//SW2:4
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Flip_Screen ) )	//SW2:5
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Cabinet ) )		//SW2:6
	PORT_DIPSETTING(    0x20, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x00, "Clear (Current Level) (Cheat)")	//SW2:7
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Stop" )					//SW2:8
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout popper_charlayout =
{
	8,8,
	RGN_FRAC(2,2),
	2,
	{ 0, 4 },
	{ STEP4(8,1), STEP4(0,1) },
	{ STEP8(0,16) },
	16*8
};

static const gfx_layout popper_spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	2,
	{ 0, RGN_FRAC(1,2) },
	{ STEP8(8,1), STEP8(0,1) },
	{ STEP16(0, 16) },
	16*2*8
};

static const gfx_decode popper_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &popper_charlayout,   0, 16 },
	{ REGION_GFX2, 0, &popper_spritelayout, 0, 16 },
	{ -1 } /* end of array */
};

static MACHINE_DRIVER_START( popper )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,18432000/6)
	MDRV_CPU_PROGRAM_MAP(popper_readmem,popper_writemem)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,1)

	MDRV_CPU_ADD(Z80,18432000/12)
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(popper_sound_readmem,popper_sound_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,4)		//NMIs caused by the main CPU

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(30)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(33*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 33*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(popper_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64)

	MDRV_PALETTE_INIT(popper)
	MDRV_VIDEO_START(popper)
	MDRV_VIDEO_UPDATE(popper)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 18432000/12)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD(AY8910, 18432000/12)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


ROM_START( popper )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "p1",   0x0000, 0x2000, CRC(56881b70) SHA1(d3ade7a54a6cb8a0babf0d667a6b27f492a739dc) )
	ROM_LOAD( "p2",   0x2000, 0x2000, CRC(a054d9d2) SHA1(fcd86e7247b40cf07ea595a64c104b99b0e93ced) )
	ROM_LOAD( "p3",   0x4000, 0x2000, CRC(6201928a) SHA1(53b571b9f2c0568f10cd974641863c2e00777b46) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for code */
	ROM_LOAD( "p0",   0x0000, 0x1000, CRC(ef5f7c5b) SHA1(c63a3d9ef2868ad7eaacddec810d62d2e124dc15) )

	ROM_REGION( 0x2000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "p4",   0x0000, 0x2000, CRC(86203349) SHA1(cce2dd3fa786c2fb3ca80e7b93adf94db3b46b01) )

	ROM_REGION( 0x4000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "p5",   0x0000, 0x2000, CRC(a21ac194) SHA1(2c0e3df8981a12d383b1c4619a0b95a7c2d176a7) )
	ROM_LOAD( "p6",   0x2000, 0x2000, CRC(d99fa790) SHA1(201271ee4fb812236a38cb5f9070ac29e8186097) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "p.m3", 0x0000, 0x0020, CRC(713217aa) SHA1(6083c3432bf94c9e983fcc79171529f519c86105) )
	ROM_LOAD( "p.m4", 0x0020, 0x0020, CRC(384de5c1) SHA1(892c89a01c11671c5708113b4e4c27b84be37ea6) )
ROM_END


GAME( 1983, popper, 0, popper, popper, 0, ROT90, "Omori Electric Co., Ltd.", "Popper", GAME_IMPERFECT_COLORS ,2)
