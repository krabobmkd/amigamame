/****************************************************************************

Ace by Allied Leisure

Driver by Jarek Burczynski
2002.09.19



Allied Leisure 1976
"MAJOR MFG. INC. SUNNYVALE, CA" in PCB etch

18MHz
                                                          5MHz

8080


2101
2101


A5               3106          3106         3106
A4
A3                                                      3622.K4
A2                                   2101
A1                   2101            2101

                                                         [ RANGE ] [ TIME ]
                                                        (two 0-9 thumbwheel switches)


5x2101 - SRAM 256x4
3x3106 - SRAM 256x1
1x3622 - ROM 512x4


****************************************************************************/

#include "driver.h"


static unsigned char *ace_scoreram;
static unsigned char *ace_ram2;
static unsigned char *ace_characterram;


static int objpos[8];

static WRITE8_HANDLER( ace_objpos_w )
{
	objpos[offset]=data;
}

#if 0
static READ8_HANDLER( ace_objpos_r )
{
	return objpos[offset];
}
#endif

VIDEO_UPDATE( ace )

{ 
struct drawgfxParams dgp0={
	bitmap, 	// dest
	Machine->gfx[1], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
struct drawgfxParams dgp1={
	bitmap, 	// dest
	Machine->gfx[2], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
struct drawgfxParams dgp2={
	bitmap, 	// dest
	Machine->gfx[3], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
	int offs;

	decodechar(Machine->gfx[1], 0, ace_characterram, Machine->drv->gfxdecodeinfo[1].gfxlayout);
	decodechar(Machine->gfx[2], 0, ace_characterram, Machine->drv->gfxdecodeinfo[2].gfxlayout);
	decodechar(Machine->gfx[3], 0, ace_characterram, Machine->drv->gfxdecodeinfo[3].gfxlayout);

	for (offs = 0; offs < 8; offs++)
	{
		decodechar(Machine->gfx[4], offs, ace_scoreram, Machine->drv->gfxdecodeinfo[4].gfxlayout);
	}

	/* first of all, fill the screen with the background color */
	fillbitmap(bitmap, Machine->pens[0], &Machine->visible_area);


		
		dgp0.sx = objpos[0];
		dgp0.sy = objpos[1];
		drawgfx(&dgp0);

		
		dgp1.sx = objpos[2];
		dgp1.sy = objpos[3];
		drawgfx(&dgp1);

		
		dgp2.sx = objpos[4];
		dgp2.sy = objpos[5];
		drawgfx(&dgp2);

	
	{ 
	struct drawgfxParams dgp3={
		bitmap, 	// dest
		Machine->gfx[4], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		/* ?? */				&Machine->visible_area, 	// clip
		TRANSPARENCY_NONE, 	// transparency
		0, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0; offs < 8; offs++)
	{
		
		dgp3.code = offs;
		dgp3.sx = 10*8+offs*16;
		dgp3.sy = 256-16;
		drawgfx(&dgp3);
	}
	} // end of patch paragraph

}
} // end of patch paragraph



static PALETTE_INIT( ace )
{
	palette_set_color(0,0x10,0x20,0xd0); /* light bluish */
	palette_set_color(1,0xff,0xff,0xff); /* white */
}


static READ8_HANDLER( ace_characterram_r )
{
	return ace_characterram[offset];
}

static WRITE8_HANDLER( ace_characterram_w )
{
	if (ace_characterram[offset] != data)
	{
		if (data&(~0x07))
		{
			logerror("write to %04x data=%02x\n", 0x8000+offset, data);
			ui_popup("write to %04x data=%02x\n", 0x8000+offset, data);
		}
		ace_characterram[offset] = data;
	}
}


static READ8_HANDLER( unk_r )
{
	return rand()&0xff;
}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )

	AM_RANGE(0x0000, 0x09ff) AM_READ(MRA8_ROM)

	AM_RANGE(0x2000, 0x20ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8300, 0x83ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8000, 0x80ff) AM_READ(ace_characterram_r)

	/* players inputs */
	AM_RANGE(0xc008, 0xc008) AM_READ(input_port_0_r)
	AM_RANGE(0xc009, 0xc009) AM_READ(input_port_1_r)
	AM_RANGE(0xc00a, 0xc00a) AM_READ(input_port_2_r)
	AM_RANGE(0xc00b, 0xc00b) AM_READ(input_port_3_r)
	AM_RANGE(0xc00c, 0xc00c) AM_READ(input_port_4_r)
	AM_RANGE(0xc00d, 0xc00d) AM_READ(input_port_5_r)
	AM_RANGE(0xc00e, 0xc00e) AM_READ(input_port_6_r)
	AM_RANGE(0xc00f, 0xc00f) AM_READ(input_port_7_r)
	AM_RANGE(0xc010, 0xc010) AM_READ(input_port_8_r)
	AM_RANGE(0xc011, 0xc011) AM_READ(input_port_9_r)

	AM_RANGE(0xc012, 0xc012) AM_READ(unk_r)

	/* vblank */
	AM_RANGE(0xc014, 0xc014) AM_READ(input_port_10_r)

	/* coin */
	AM_RANGE(0xc015, 0xc015) AM_READ(input_port_11_r)

	/* start (must read 1 at least once to make the game run) */
	AM_RANGE(0xc016, 0xc016) AM_READ(input_port_12_r)

	AM_RANGE(0xc017, 0xc017) AM_READ(unk_r)
	AM_RANGE(0xc018, 0xc018) AM_READ(unk_r)
	AM_RANGE(0xc019, 0xc019) AM_READ(unk_r)

	AM_RANGE(0xc020, 0xc020) AM_READ(unk_r)
	AM_RANGE(0xc021, 0xc021) AM_READ(unk_r)
	AM_RANGE(0xc022, 0xc022) AM_READ(unk_r)
	AM_RANGE(0xc023, 0xc023) AM_READ(unk_r)
	AM_RANGE(0xc024, 0xc024) AM_READ(unk_r)
	AM_RANGE(0xc025, 0xc025) AM_READ(unk_r)
	AM_RANGE(0xc026, 0xc026) AM_READ(unk_r)

ADDRESS_MAP_END

/* 5x2101 - SRAM 256x4 */
/* 3x3106 - SRAM 256x1 */
/* 1x3622 - ROM 512x4  - doesn't seem to be used ????????????*/

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x09ff) AM_WRITE(MWA8_ROM)

	AM_RANGE(0x2000, 0x20ff) AM_WRITE(MWA8_RAM) AM_BASE(&ace_scoreram)	/* 2x2101 */
	AM_RANGE(0x8300, 0x83ff) AM_WRITE(MWA8_RAM) AM_BASE(&ace_ram2)		/* 2x2101 */
	AM_RANGE(0x8000, 0x80ff) AM_WRITE(ace_characterram_w) AM_BASE(&ace_characterram)	/* 3x3101 (3bits: 0, 1, 2) */

	AM_RANGE(0xc000, 0xc005) AM_WRITE(ace_objpos_w)
ADDRESS_MAP_END


INPUT_PORTS_START( ace )

	PORT_START_TAG("c008")	/* player thrust */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1) PORT_NAME("P1 Thrust")

	PORT_START_TAG("c009")	/* player slowdown */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1) PORT_NAME("P1 Slowdown")

	PORT_START_TAG("c00a")	/* player left */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)

	PORT_START_TAG("c00b")	/* player right */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)

	PORT_START_TAG("c00c")	/* player fire */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("P1 Fire")

	PORT_START_TAG("c00d")	/* enemy thrust */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2) PORT_NAME("P2 Thrust")

	PORT_START_TAG("c00e")	/* enemy slowdown */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2) PORT_NAME("P2 Slowdown")

	PORT_START_TAG("c00f")	/* enemy left  */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)

	PORT_START_TAG("c010")	/* enemy right */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)

	PORT_START_TAG("c011")	/* enemy fire */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("P2 Fire")

//c012


	PORT_START_TAG("c014")	/* VBLANK??? */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_VBLANK )

	PORT_START_TAG("c015")	/* coin input */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )

	PORT_START_TAG("c016")	/* game start */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )

INPUT_PORTS_END


static const gfx_layout charlayout =
{
	16,16,	/* 16*16 chars */
	8,	/* 8 characters */
	1,		/* 1 bit per pixel */
	{ 4 },	/* character rom is 512x4 bits (3622 type)*/
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3, 16+0, 16+1, 16+2, 16+3, 24+0, 24+1, 24+2, 24+3 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32, 8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	64*8	/* every char takes 64 consecutive bytes */
};

static const gfx_layout charlayout0 =
{
	16,16,	/* 16*16 chars */
	1,	/* 1 characters */
	1,		/* 1 bit per pixel */
	{ 7 },	/* bit 0 in character ram */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128, 8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
	256*8	/* every char takes 256 consecutive bytes */
};

static const gfx_layout charlayout1 =
{
	16,16,	/* 16*16 chars */
	1,	/* 1 characters */
	1,		/* 1 bit per pixel */
	{ 6 },	/* bit 1 in character ram */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128, 8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
	256*8	/* every char takes 256 consecutive bytes */
};

static const gfx_layout charlayout2 =
{
	16,16,	/* 16*16 chars */
	1,	/* 1 characters */
	1,		/* 1 bit per pixel */
	{ 5 },	/* bit 2 in character ram */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128, 8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
	256*8	/* every char takes 256 consecutive bytes */
};

static const gfx_layout scorelayout =
{
	16,16,	/* 16*16 chars */
	8,	/* 8 characters */
	1,		/* 1 bit per pixel */
	{ 0 },	/*  */
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16, 8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	32*8	/* every char takes 32 consecutive bytes */
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0     , &charlayout,  0, 2 },
	{ 0          , 0x8000, &charlayout0, 0, 2 },    /* the game dynamically modifies this */
	{ 0          , 0x8000, &charlayout1, 0, 2 },    /* the game dynamically modifies this */
	{ 0          , 0x8000, &charlayout2, 0, 2 },    /* the game dynamically modifies this */
	{ 0          , 0x8000, &scorelayout, 0, 2 },    /* the game dynamically modifies this */
	{ -1 } /* end of array */
};



static MACHINE_DRIVER_START( ace )

	/* basic machine hardware */
	MDRV_CPU_ADD(8080, 18000000 / 9)	/* 2 MHz ? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(4*8, 32*8-1, 2*8, 32*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2)
	MDRV_COLORTABLE_LENGTH(2*2)

	MDRV_PALETTE_INIT(ace)
	MDRV_VIDEO_UPDATE(ace)

	/* sound hardware */
	/* ???? */

MACHINE_DRIVER_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( ace )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )     /* 64k for main CPU */
	ROM_LOAD( "ace.a1",		0x0000, 0x0200, CRC(16811834) SHA1(5502812dd161908eea3fa8851d7e5c1e22b0f8ff) )
	ROM_LOAD( "ace.a2",		0x0200, 0x0200, CRC(f9eae80e) SHA1(8865b86c7b5d57c76312c16f8a614bf35ffaf532) )
	ROM_LOAD( "ace.a3",		0x0400, 0x0200, CRC(c5c63b8c) SHA1(2079dd12ff0c4aafec19aeb9baa70fc9b6788356) )
	ROM_LOAD( "ace.a4",		0x0600, 0x0200, CRC(ea4503aa) SHA1(fea610124b9f7ea18d29b4e4599253ba1ee067e1) )
	ROM_LOAD( "ace.a5",		0x0800, 0x0200, CRC(623c58e7) SHA1(a92418bc323a1ae76eae8e094e4d6ebd1e8da14e) )

	/* not used - I couldn't guess when this should be displayed */
	ROM_REGION( 0x0200, REGION_GFX1, 0 )
	ROM_LOAD( "ace.k4",		0x0000, 0x0200, CRC(daa05ec6) SHA1(8b71ffb802293dc93f6b492ff128a704e676a5fd) )

ROM_END

GAME( 1976, ace, 0, ace, ace, 0, ROT0, "Allied Leisure", "Ace", GAME_NO_SOUND | GAME_IMPERFECT_COLORS ,2)
