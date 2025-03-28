/*******************************************************************************

     Mouser

     Driver by Frank Palazzolo (palazzol@comcast.net)

    - This driver was done with only flyer shots to go by.
    - Colors are a good guess (might be perfect)
    - Clock and interrupt speeds for the sound CPU is a guess, but seem
      reasonable, especially because the graphics seem to be synched
    - Sprite priorities are unknown

*******************************************************************************/

#include "driver.h"
#include "sound/ay8910.h"

unsigned char mouser_sound_byte;
unsigned char mouser_nmi_enable;

/* From "vidhrdw/mouser.c" */
PALETTE_INIT( mouser );
WRITE8_HANDLER( mouser_flip_screen_x_w );
WRITE8_HANDLER( mouser_flip_screen_y_w );
WRITE8_HANDLER( mouser_spriteram_w );
WRITE8_HANDLER( mouser_colorram_w );
VIDEO_UPDATE( mouser );

/* Mouser has external masking circuitry around
 * the NMI input on the main CPU */

WRITE8_HANDLER( mouser_nmi_enable_w )
{
	mouser_nmi_enable = data;
}

INTERRUPT_GEN( mouser_nmi_interrupt )
{
	if ((mouser_nmi_enable & 1) == 1)
		nmi_line_pulse();
}

/* Sound CPU interrupted on write */

WRITE8_HANDLER( mouser_sound_interrupt_w )
{
	mouser_sound_byte = data;
	cpunum_set_input_line(1, 0, PULSE_LINE);
}

READ8_HANDLER( mouser_sound_byte_r )
{
	return mouser_sound_byte;
}

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x6000, 0x6bff) AM_READ(MRA8_RAM)
	AM_RANGE(0x9000, 0x93ff) AM_READ(videoram_r)
	AM_RANGE(0xa000, 0xa000) AM_READ(input_port_0_r)
	AM_RANGE(0xa800, 0xa800) AM_READ(input_port_1_r)
	AM_RANGE(0xb000, 0xb000) AM_READ(input_port_2_r)
	AM_RANGE(0xb800, 0xb800) AM_READ(input_port_3_r)
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x6000, 0x6bff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8800, 0x88ff) AM_WRITE(MWA8_NOP) /* unknown */
	AM_RANGE(0x9000, 0x93ff) AM_WRITE(videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x9800, 0x9cff) AM_WRITE(mouser_spriteram_w) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x9c00, 0x9fff) AM_WRITE(mouser_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xa000, 0xa000) AM_WRITE(mouser_nmi_enable_w) /* bit 0 = NMI Enable */
	AM_RANGE(0xa001, 0xa001) AM_WRITE(mouser_flip_screen_x_w)
	AM_RANGE(0xa002, 0xa002) AM_WRITE(mouser_flip_screen_y_w)
	AM_RANGE(0xb800, 0xb800) AM_WRITE(mouser_sound_interrupt_w) /* byte to sound cpu */

ADDRESS_MAP_END


static ADDRESS_MAP_START( readmem2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x2000, 0x23ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x3000, 0x3000) AM_READ(mouser_sound_byte_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem2, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x2000, 0x23ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x4000, 0x4000) AM_WRITE(MWA8_NOP)	/* watchdog? */
ADDRESS_MAP_END

static ADDRESS_MAP_START( readport2, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
ADDRESS_MAP_END

static ADDRESS_MAP_START( writeport2, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x01, 0x01) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x80, 0x80) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x81, 0x81) AM_WRITE(AY8910_control_port_1_w)
ADDRESS_MAP_END

INPUT_PORTS_START( mouser )
    PORT_START
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY

    PORT_START
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
    PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
    PORT_DIPNAME( 0x20, 0x00, DEF_STR( Difficulty ) )		/* guess ! - check code at 0x29a1 */
    PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
    PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

    PORT_START
    PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
    PORT_DIPSETTING(	0x00, "3" )
    PORT_DIPSETTING(	0x01, "4" )
    PORT_DIPSETTING(	0x02, "5" )
    PORT_DIPSETTING(	0x03, "6" )
    PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )
    PORT_DIPSETTING(    0x00, "20000" )
    PORT_DIPSETTING(    0x04, "40000" )
    PORT_DIPSETTING(    0x08, "60000" )
    PORT_DIPSETTING(    0x0c, "80000" )
    PORT_DIPNAME( 0x70, 0x00, DEF_STR( Coinage ) )
    PORT_DIPSETTING(    0x70, DEF_STR( 5C_1C ) )
    PORT_DIPSETTING(    0x50, DEF_STR( 4C_1C ) )
    PORT_DIPSETTING(    0x30, DEF_STR( 3C_1C ) )
    PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
    PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
    PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
    PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) )
    PORT_DIPSETTING(    0x60, DEF_STR( 1C_4C ) )
    PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
    PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
    PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )

    PORT_START
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,     /* 8*8 characters */
	1024,    /* 1024 characters */
	2,       /* 2 bits per pixel */
	{ 8192*8, 0 },
	{0, 1, 2, 3, 4, 5, 6, 7},
	{8*0, 8*1, 8*2, 8*3, 8*4, 8*5, 8*6, 8*7},
	8*8
};


static const gfx_layout spritelayout =
{
	16,16,   /* 16*16 characters */
	64,      /* 64 sprites (2 banks) */
	2,       /* 2 bits per pixel */
	{ 8192*8, 0 },
	{0,  1,  2,  3,  4,  5,  6,  7,
	 64+0, 64+1, 64+2, 64+3, 64+4, 64+5, 64+6, 64+7},
	{8*0, 8*1, 8*2, 8*3, 8*4, 8*5, 8*6, 8*7,
	 128+8*0, 128+8*1, 128+8*2, 128+8*3, 128+8*4, 128+8*5, 128+8*6, 128+8*7},
	16*16
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x0000, &charlayout,       0, 16 },
	{ REGION_GFX1, 0x1000, &spritelayout,     0, 16 },
	{ REGION_GFX1, 0x1800, &spritelayout,     0, 16 },
	{ -1 } /* end of array */
};


static MACHINE_DRIVER_START( mouser )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 4000000)	/* 4 MHz ? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT(mouser_nmi_interrupt,1) /* NMI is masked externally */

	MDRV_CPU_ADD(Z80, 4000000)	/* ??? */
	MDRV_CPU_PROGRAM_MAP(readmem2,writemem2)
	MDRV_CPU_IO_MAP(readport2,writeport2)
	/* audio CPU */
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,4) /* ??? This controls the sound tempo */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(64)

	MDRV_PALETTE_INIT(mouser)
	MDRV_VIDEO_START(generic)
	MDRV_VIDEO_UPDATE(mouser)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 4000000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD(AY8910, 4000000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


ROM_START( mouser )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* 64K for data, 64K for encrypted opcodes */
	ROM_LOAD( "m0.5e",         0x0000, 0x2000, CRC(b56e00bc) SHA1(f3b23212590d91f1d19b1c7a98c560fbe5943185) )
	ROM_LOAD( "m1.5f",         0x2000, 0x2000, CRC(ae375d49) SHA1(8422f5a4d8560425f0c8612cf6f76029fcfe267c) )
	ROM_LOAD( "m2.5j",         0x4000, 0x2000, CRC(ef5817e4) SHA1(5cadc19f20fadf97c95852b280305fe4c75f1d19) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "m5.3v",         0x0000, 0x1000, CRC(50705eec) SHA1(252cea3498722318638f0c98ae929463ffd7d0d6) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "m3.11h",        0x0000, 0x2000, CRC(aca2834e) SHA1(c4f457fd8ea46386431ef8dffe54a232631870be) )
	ROM_LOAD( "m4.11k",        0x2000, 0x2000, CRC(943ab2e2) SHA1(ef9fc31dc8fe7a62f7bc6c817ce0d65091cb9a03) )

	/* Opcode Decryption PROMS */
	ROM_REGION( 0x0100, REGION_USER1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_HIGH( "bprom.4b",0x0000,0x0100,CRC(dd233851) SHA1(25eab1ec2227910c6fcd2803986f1cf206624da7) )
	ROM_LOAD_NIB_LOW(  "bprom.4c",0x0000,0x0100,CRC(60aaa686) SHA1(bb2ad555da51f6b30ab8b55833fe8d461a1e67f4) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "bprom.5v", 0x0000, 0x0020, CRC(7f8930b2) SHA1(8d0fe14b770fcf7088696d7b80d64507c6ee7364) )
	ROM_LOAD( "bprom.5u", 0x0020, 0x0020, CRC(0086feed) SHA1(b0b368e5fb7380cf09abd60c0933b405daf1c36a) )
ROM_END


ROM_START( mouserc )
	ROM_REGION( 0x20000, REGION_CPU1, 0 ) /* 64K for data, 64K for encrypted opcodes */
	ROM_LOAD( "83001.0",       0x0000, 0x2000, CRC(e20f9601) SHA1(f559a470784bda0bee9cab257a548238365acaa6) )
	ROM_LOAD( "m1.5f",         0x2000, 0x2000, CRC(ae375d49) SHA1(8422f5a4d8560425f0c8612cf6f76029fcfe267c) )	// 83001.1
	ROM_LOAD( "m2.5j",         0x4000, 0x2000, CRC(ef5817e4) SHA1(5cadc19f20fadf97c95852b280305fe4c75f1d19) )	// 83001.2

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "m5.3v",         0x0000, 0x1000, CRC(50705eec) SHA1(252cea3498722318638f0c98ae929463ffd7d0d6) )	// 83001.5

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "m3.11h",        0x0000, 0x2000, CRC(aca2834e) SHA1(c4f457fd8ea46386431ef8dffe54a232631870be) )	// 83001.3
	ROM_LOAD( "m4.11k",        0x2000, 0x2000, CRC(943ab2e2) SHA1(ef9fc31dc8fe7a62f7bc6c817ce0d65091cb9a03) )	// 83001.4

	/* Opcode Decryption PROMS (originally from the UPL romset!) */
	ROM_REGION( 0x0100, REGION_USER1, ROMREGION_DISPOSE )
	ROM_LOAD_NIB_HIGH( "bprom.4b",0x0000,0x0100,CRC(dd233851) SHA1(25eab1ec2227910c6fcd2803986f1cf206624da7) )
	ROM_LOAD_NIB_LOW(  "bprom.4c",0x0000,0x0100,CRC(60aaa686) SHA1(bb2ad555da51f6b30ab8b55833fe8d461a1e67f4) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "bprom.5v", 0x0000, 0x0020, CRC(7f8930b2) SHA1(8d0fe14b770fcf7088696d7b80d64507c6ee7364) )	// clr.5v
	ROM_LOAD( "bprom.5u", 0x0020, 0x0020, CRC(0086feed) SHA1(b0b368e5fb7380cf09abd60c0933b405daf1c36a) )	// clr.5u
ROM_END


DRIVER_INIT( mouser )
{
	/* Decode the opcodes */

	offs_t i;
	UINT8 *rom = memory_region(REGION_CPU1);
	UINT8 *decrypted = auto_malloc(0x6000);
	UINT8 *table = memory_region(REGION_USER1);

	memory_set_decrypted_region(0, 0x0000, 0x5fff, decrypted);

	for (i = 0;i < 0x6000;i++)
	{
		decrypted[i] = table[rom[i]];
	}
}


GAME( 1983, mouser,   0,      mouser, mouser, mouser, ROT90, "UPL", "Mouser", 0 ,0)
GAME( 1983, mouserc,  mouser, mouser, mouser, mouser, ROT90, "[UPL] (Cosmos license)", "Mouser (Cosmos)", 0 ,0)
