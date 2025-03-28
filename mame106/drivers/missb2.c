/* Miss Bubble 2

A rather odd bootleg of Bubble Bobble with level select, redesigned levels,
redesigned (8bpp!) graphics and different sound hardware... Crazy

The Sound NMI and/or Interrupts aren't likely to be right. The Sound CPU
starts writing to unusual memory ports - either because the NMI/Interrupt
timing is out, or the sheer fact that the Sound CPU code is rather poorly
written, so it may be normal behaviour.

Also, the OKI M6295 seems to be playing the wrong samples, however the current
OKI M6295 sound ROM dump is bad.

*/

#include "driver.h"
#include "sound/okim6295.h"
#include "sound/3812intf.h"

static UINT8 *bg_paletteram, *missb2_bgvram;

/* vidhrdw/bublbobl.c */
extern UINT8 *bublbobl_objectram;
extern size_t bublbobl_objectram_size;

/* machine/bublbobl.c */
WRITE8_HANDLER( bublbobl_bankswitch_w );
WRITE8_HANDLER( bublbobl_nmitrigger_w );
WRITE8_HANDLER( bublbobl_sound_command_w );
WRITE8_HANDLER( bublbobl_sh_nmi_disable_w );
WRITE8_HANDLER( bublbobl_sh_nmi_enable_w );
extern int bublbobl_video_enable;

/* Video Hardware */

VIDEO_UPDATE( missb2 )
{
	int offs;
	int sx,sy,xc,yc;
	int gfx_num,gfx_attr,gfx_offs;
	const UINT8 *prom_line;
	UINT16 bg_offs;

	/* Bubble Bobble doesn't have a real video RAM. All graphics (characters */
	/* and sprites) are stored in the same memory region, and information on */
	/* the background character columns is stored in the area dd00-dd3f */

	/* This clears & redraws the entire screen each pass */
	fillbitmap(bitmap,Machine->pens[255],&Machine->visible_area);

	if (!bublbobl_video_enable) return;

	/* background map register */
	//ui_popup("%02x",(*missb2_bgvram) & 0x1f);
	
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
		0xff, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for(bg_offs = ((*missb2_bgvram) << 4);bg_offs<(((*missb2_bgvram)<< 4)|0xf);bg_offs++)
	{
		
		dgp0.code = bg_offs;
		dgp0.sy = (bg_offs & 0xf) * 0x10;
		drawgfx(&dgp0);
	}
	} // end of patch paragraph



	sx = 0;

	
	{ 
	struct drawgfxParams dgp1={
		bitmap, 	// dest
		Machine->gfx[0], 	// gfx
		0, 	// code
		0, 	// color
		0, 	// flipx
		0, 	// flipy
		0, 	// sx
		0, 	// sy
		&Machine->visible_area, 	// clip
		TRANSPARENCY_PEN, 	// transparency
		0xff, 	// transparent_color
		0, 	// scalex
		0, 	// scaley
		NULL, 	// pri_buffer
		0 	// priority_mask
	  };
	for (offs = 0;offs < bublbobl_objectram_size;offs += 4)
    {
		/* skip empty sprites */
		/* this is dword aligned so the UINT32 * cast shouldn't give problems */
		/* on any architecture */
		if (*(UINT32 *)(&bublbobl_objectram[offs]) == 0)
			continue;

		gfx_num = bublbobl_objectram[offs + 1];
		gfx_attr = bublbobl_objectram[offs + 3];
		prom_line = memory_region(REGION_PROMS) + 0x80 + ((gfx_num & 0xe0) >> 1);

		gfx_offs = ((gfx_num & 0x1f) * 0x80);
		if ((gfx_num & 0xa0) == 0xa0)
			gfx_offs |= 0x1000;

		sy = -bublbobl_objectram[offs + 0];

		for (yc = 0;yc < 32;yc++)
		{
			if (prom_line[yc/2] & 0x08)	continue;	/* NEXT */

			if (!(prom_line[yc/2] & 0x04))	/* next column */
			{
				sx = bublbobl_objectram[offs + 2];
				if (gfx_attr & 0x40) sx -= 256;
			}

			for (xc = 0;xc < 2;xc++)
			{
				int goffs,code,color,flipx,flipy,x,y;

				goffs = gfx_offs + xc * 0x40 + (yc & 7) * 0x02 +
						(prom_line[yc/2] & 0x03) * 0x10;
				code = videoram[goffs] + 256 * (videoram[goffs + 1] & 0x03) + 1024 * (gfx_attr & 0x0f);
				color = (videoram[goffs + 1] & 0x3c) >> 2;
				flipx = videoram[goffs + 1] & 0x40;
				flipy = videoram[goffs + 1] & 0x80;
				x = sx + xc * 8;
				y = (sy + yc * 8) & 0xff;

				if (flip_screen)
				{
					x = 248 - x;
					y = 248 - y;
					flipx = !flipx;
					flipy = !flipy;
				}

				
				dgp1.code = code;
				dgp1.flipx = flipx;
				dgp1.flipy = flipy;
				dgp1.sx = x;
				dgp1.sy = y;
				drawgfx(&dgp1);
			}
		}

		sx += 16;
	}
	} // end of patch paragraph

}

INLINE void bg_changecolor_RRRRGGGGBBBBxxxx(pen_t color,int data)
{
	int r,g,b;


	r = (data >> 12) & 0x0f;
	g = (data >>  8) & 0x0f;
	b = (data >>  4) & 0x0f;

	r = (r << 4) | r;
	g = (g << 4) | g;
	b = (b << 4) | b;

	palette_set_color(color+256,r,g,b);
}

static WRITE8_HANDLER( bg_paletteram_RRRRGGGGBBBBxxxx_be_w )
{
	bg_paletteram[offset] = data;
	bg_changecolor_RRRRGGGGBBBBxxxx(offset / 2,bg_paletteram[offset | 1] | (bg_paletteram[offset & ~1] << 8));
}

static WRITE8_HANDLER( missb2_bg_bank_w )
{
	int bankaddress;
	UINT8 *RAM = memory_region(REGION_CPU2);

	// I don't know how this is really connected,bit 1 is always high afaik...
	bankaddress = ((data & 2) ? 0x1000 : 0x0000) | ((data & 1) ? 0x4000 : 0x0000) | (0x8000);
	memory_set_bankptr(2, &RAM[bankaddress]);
}

/* Memory Maps */

static ADDRESS_MAP_START( master_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xc000, 0xdcff) AM_RAM AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0xdd00, 0xdfff) AM_RAM AM_BASE(&bublbobl_objectram) AM_SIZE(&bublbobl_objectram_size)
	AM_RANGE(0xe000, 0xf7ff) AM_RAM AM_SHARE(1)
	AM_RANGE(0xf800, 0xf9ff) AM_RAM AM_WRITE(paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&paletteram)
	AM_RANGE(0xfa00, 0xfa00) AM_WRITE(bublbobl_sound_command_w)
	AM_RANGE(0xfa03, 0xfa03) AM_WRITENOP // sound cpu reset
	AM_RANGE(0xfa80, 0xfa80) AM_WRITENOP
	AM_RANGE(0xfb40, 0xfb40) AM_WRITE(bublbobl_bankswitch_w)
	AM_RANGE(0xfc00, 0xfcff) AM_RAM
	AM_RANGE(0xfd00, 0xfdff) AM_RAM			// ???
	AM_RANGE(0xfe00, 0xfe03) AM_RAM			// ???
	AM_RANGE(0xfe80, 0xfe83) AM_RAM			// ???
	AM_RANGE(0xff00, 0xff00) AM_READ(input_port_0_r)
	AM_RANGE(0xff01, 0xff01) AM_READ(input_port_1_r)
	AM_RANGE(0xff02, 0xff02) AM_READ(input_port_2_r)
	AM_RANGE(0xff03, 0xff03) AM_READ(input_port_3_r)
	AM_RANGE(0xff94, 0xff94) AM_WRITENOP	// ???
	AM_RANGE(0xff98, 0xff98) AM_WRITENOP	// ???
ADDRESS_MAP_END

static ADDRESS_MAP_START( slave_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x9000, 0xafff) AM_ROMBANK(2)	// ROM data for the background palette ram
	AM_RANGE(0xb000, 0xb1ff) AM_ROM			// banked ???
	AM_RANGE(0xc000, 0xc1ff) AM_RAM AM_WRITE(bg_paletteram_RRRRGGGGBBBBxxxx_be_w) AM_BASE(&bg_paletteram)
	AM_RANGE(0xc800, 0xcfff) AM_RAM			// main ???
	AM_RANGE(0xd000, 0xd000) AM_WRITE(missb2_bg_bank_w)
	AM_RANGE(0xd002, 0xd002) AM_WRITENOP
	AM_RANGE(0xd003, 0xd003) AM_RAM AM_BASE(&missb2_bgvram)
	AM_RANGE(0xe000, 0xf7ff) AM_RAM AM_SHARE(1)
ADDRESS_MAP_END

// Looks like the original bublbobl code modified to support the OKI M6295.

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x8fff) AM_RAM
	AM_RANGE(0x9000, 0x9000) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)
	AM_RANGE(0xa000, 0xa000) AM_READWRITE(YM3526_status_port_0_r, YM3526_control_port_0_w)
	AM_RANGE(0xa001, 0xa001) AM_WRITE(YM3526_write_port_0_w)
	AM_RANGE(0xb000, 0xb000) AM_READ(soundlatch_r) AM_WRITENOP // message for main cpu
	AM_RANGE(0xb001, 0xb001) AM_READNOP AM_WRITE(bublbobl_sh_nmi_enable_w)	// bit 0: message pending for main cpu, bit 1: message pending for sound cpu
	AM_RANGE(0xb002, 0xb002) AM_WRITE(bublbobl_sh_nmi_disable_w)
	AM_RANGE(0xe000, 0xefff) AM_ROM			// space for diagnostic ROM?
ADDRESS_MAP_END

/* Input Ports */

INPUT_PORTS_START( missb2 )
	PORT_START_TAG("DSW0")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Japanese ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )

	PORT_START_TAG("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x08, "20K 80K" )
	PORT_DIPSETTING(    0x0c, "30K 100K" )
	PORT_DIPSETTING(    0x04, "40K 200K" )
	PORT_DIPSETTING(    0x00, "50K 250K" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x10, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x20, "5" )
	PORT_DIPNAME( 0xc0, 0x00, "Monster Speed" )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x80, DEF_STR( High ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Very_High ) )

	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_2WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_2WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_TILT ) // ???
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

/* Graphics Layouts */

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,4),
	8,
	{ RGN_FRAC(0,4)+0, RGN_FRAC(0,4)+4, RGN_FRAC(1,4)+0, RGN_FRAC(1,4)+4, RGN_FRAC(2,4)+0, RGN_FRAC(2,4)+4, RGN_FRAC(3,4)+0, RGN_FRAC(3,4)+4 },
	{ 3, 2, 1, 0, 8+3, 8+2, 8+1, 8+0 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static const UINT32 bglayout_xoffset[256] =
{
		0*8,      1*8, 2048*8, 2049*8,    8*8,    9*8, 2056*8, 2057*8,
		4*8,      5*8, 2052*8, 2053*8,   12*8,   13*8, 2060*8, 2061*8,
		256*8 , 257*8, 2304*8, 2305*8,  264*8,  265*8, 2312*8, 2313*8,
		260*8 , 261*8, 2308*8, 2309*8,  268*8,  269*8, 2316*8, 2317*8,
	   1024*8, 1025*8, 3072*8, 3073*8, 1032*8, 1033*8, 3080*8, 3081*8,
	   1028*8, 1029*8, 3076*8, 3077*8, 1036*8, 1037*8, 3084*8, 3085*8,
	   1280*8, 1281*8, 3328*8, 3329*8, 1288*8, 1289*8, 3336*8, 3337*8,
	   1284*8, 1285*8, 3332*8, 3333*8, 1292*8, 1293*8, 3340*8, 3341*8,
		512*8,  513*8, 2560*8, 2561*8,  520*8,  521*8, 2568*8, 2569*8,
		516*8,  517*8, 2564*8, 2565*8,	524*8,  525*8, 2572*8, 2573*8,
		768*8,  769*8, 2816*8, 2817*8,  776*8,  777*8, 2824*8, 2825*8,
		772*8,  773*8, 2820*8, 2821*8,  780*8,  781*8, 2828*8, 2829*8,
	   1536*8, 1537*8, 3584*8, 3585*8, 1544*8, 1545*8, 3592*8, 3593*8,
	   1540*8, 1541*8, 3588*8, 3589*8, 1548*8, 1549*8, 3596*8, 3597*8,
	   1792*8, 1793*8, 3840*8, 3841*8, 1800*8, 1801*8, 3848*8, 3849*8,
	   1796*8, 1797*8, 3844*8, 3845*8, 1804*8, 1805*8, 3852*8, 3853*8,
		  2*8,    3*8, 2050*8, 2051*8,   10*8,   11*8, 2058*8, 2059*8,
		  6*8,    7*8, 2054*8, 2055*8,	 14*8,   15*8, 2062*8, 2063*8,
		258*8,  259*8, 2306*8, 2307*8,  266*8,  267*8, 2314*8, 2315*8,
		262*8,  263*8, 2310*8, 2311*8,	270*8,  271*8, 2318*8, 2319*8,
	   1026*8, 1027*8, 3074*8, 3075*8, 1034*8, 1035*8, 3082*8, 3083*8,
	   1030*8, 1031*8, 3078*8, 3079*8, 1038*8, 1039*8, 3086*8, 3087*8,
	   1282*8, 1283*8, 3330*8, 3331*8, 1290*8, 1291*8, 3338*8, 3339*8,
	   1286*8, 1287*8, 3334*8, 3335*8, 1294*8, 1295*8, 3342*8, 3343*8,
		514*8,  515*8, 2562*8, 2563*8, 	522*8,  523*8, 2570*8, 2571*8,
		518*8,  519*8, 2566*8, 2567*8,  526*8,  527*8, 2574*8, 2575*8,
		770*8,  771*8, 2818*8, 2819*8,  778*8,  779*8, 2826*8, 2827*8,
		774*8,  775*8, 2822*8, 2823*8,  782*8,  783*8, 2830*8, 2831*8,
	   1538*8, 1539*8, 3586*8, 3587*8, 1546*8, 1547*8, 3594*8, 3595*8,
	   1542*8, 1543*8, 3590*8, 3591*8, 1550*8, 1551*8, 3598*8, 3599*8,
	   1794*8, 1795*8, 3842*8, 3843*8, 1802*8, 1803*8, 3850*8, 3851*8,
	   1798*8, 1799*8, 3846*8, 3847*8, 1806*8, 1807*8, 3854*8, 3855*8
};

static const gfx_layout bglayout =
{
	256,16,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	EXTENDED_XOFFS,
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128, 8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
	256*128,
	bglayout_xoffset,
	NULL
};

/* Graphics Decode Information */

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x00000, &charlayout, 0, 1 },
	{ REGION_GFX2, 0x00000, &bglayout,   0, 2 },
	{ -1 }
};


#define MAIN_XTAL 24000000	// not sure about this

/* Sound Interfaces */

// Handler called by the 3526 emulator when the internal timers cause an IRQ
static void irqhandler(int irq)
{
	loginfo(2,"YM3526 firing an IRQ\n");
//  cpunum_set_input_line(2,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM3526interface ym3526_interface =
{
	irqhandler
};

/* Interrupt Generator */

static INTERRUPT_GEN( missb2_interrupt )
{
	cpunum_set_input_line(2, 0, HOLD_LINE);
}

/* Machine Driver */

static MACHINE_DRIVER_START( missb2 )
	// basic machine hardware
	MDRV_CPU_ADD(Z80, MAIN_XTAL/4)	// 6 MHz
	MDRV_CPU_PROGRAM_MAP(master_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)

	MDRV_CPU_ADD(Z80, MAIN_XTAL/4)	// 6 MHz
	MDRV_CPU_PROGRAM_MAP(slave_map, 0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)

	MDRV_CPU_ADD(Z80, MAIN_XTAL/8)
	/* audio CPU */	// 3 MHz
	MDRV_CPU_PROGRAM_MAP(sound_map, 0)
//  MDRV_CPU_VBLANK_INT(irq0_line_hold, 1)
	MDRV_CPU_VBLANK_INT(missb2_interrupt, 1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(100) // 100 CPU slices per frame - a high value to ensure proper synchronization of the CPUs

	// video hardware
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(512)

	MDRV_VIDEO_UPDATE(missb2)

	// sound hardware
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3526, MAIN_XTAL/8)
	MDRV_SOUND_CONFIG(ym3526_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)

	MDRV_SOUND_ADD(OKIM6295, 8000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.4)
MACHINE_DRIVER_END

/* ROMs */

ROM_START( missb2 )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )
	ROM_LOAD( "msbub2-u.204", 0x00000, 0x10000, CRC(b633bdde) SHA1(29a389c52ff06718f1c4c39f6a854856c237356b) ) /* FIRST AND SECOND HALF IDENTICAL */
	/* ROMs banked at 8000-bfff */
	ROM_LOAD( "msbub2-u.203", 0x10000, 0x10000, CRC(29fd8afe) SHA1(94ead80d20cd3974dd4fb0358915e3bd8b793158) )
	/* 20000-2ffff empty */

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* 64k for the second CPU */
	ROM_LOAD( "msbub2-u.11",  0x0000, 0x10000, CRC(003dc092) SHA1(dff3c2b31d0804a308e5c42cf9705cd3d6144ad7) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* 64k for the third CPU */
	ROM_LOAD( "msbub2-u.211", 0x0000, 0x08000, CRC(08e5d846) SHA1(8509a71df984f0348bdc6ab60eb2ba7ceb9b1246) )

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE | ROMREGION_INVERT )
	ROM_LOAD( "msbub2-u.14",  0x00000, 0x40000, CRC(b3164b47) SHA1(083a63010515b0aa43b482938ae302b2df985312) )
	ROM_LOAD( "msbub2-u.126", 0x40000, 0x40000, CRC(b0a9a353) SHA1(40d7f4c970d8571de319231c295fa0d2836efcf7) )
	ROM_LOAD( "msbub2-u.124", 0x80000, 0x40000, CRC(4b0d8e5b) SHA1(218da3edcfea228e6df1ac59bc24217713d79410) )
	ROM_LOAD( "msbub2-u.125", 0xc0000, 0x40000, CRC(77b710e2) SHA1(f6f46804a23de6c930bc40a3f45ac70e160f0645) )

	ROM_REGION( 0x200000, REGION_GFX2, 0 ) /* background images */
	ROM_LOAD16_BYTE( "msbub2-u.ic1", 0x100001, 0x80000, CRC(d621cbc3) SHA1(36343d85bdde0e40dfe0f0e4e646546f175903f8) )
	ROM_LOAD16_BYTE( "msbub2-u.ic3", 0x100000, 0x80000, CRC(90e56035) SHA1(8fa18d97a05890178c52b97ff75aed300344a93e) )
	ROM_LOAD16_BYTE( "msbub2-u.ic2", 0x000001, 0x80000, CRC(694c2783) SHA1(401dc8713a02130289f364786c38e70c4c4f9b2e) )
	ROM_LOAD16_BYTE( "msbub2-u.ic4", 0x000000, 0x80000, CRC(be71c9f0) SHA1(1961e931017f644486cea0ce431d50973679c848) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* samples */
	ROM_LOAD( "msbub2-u.13", 0x00000, 0x20000, BAD_DUMP CRC(14f07386) SHA1(097897d92226f900e11dbbdd853aff3ac46ff016) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "a71-25.bin",  0x0000, 0x0100, CRC(2d0f8545) SHA1(089c31e2f614145ef2743164f7b52ae35bc06808) )	/* video timing - taken from bublbobl */
ROM_END

/* Driver Initialization */

static DRIVER_INIT( missb2 )
{
	UINT8 *ROM = memory_region(REGION_CPU1);

	/* in Bubble Bobble, bank 0 has code falling from 7fff to 8000,
       so I have to copy it there because bank switching wouldn't catch it */
	memcpy(ROM+0x08000,ROM+0x10000,0x4000);

}

/* Game Drivers */

GAME( 1996, missb2, bublbobl, missb2, missb2, missb2, ROT0,  "Alpha Co", "Miss Bubble 2", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND ,2)
