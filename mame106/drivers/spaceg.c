#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/dac.h"
#include "sound/sn76496.h"


/*
Space Guerrilla PCB Layout
--------------------------

All PCBs slot into a metal frame. Each PCB is joined by the 14way/28way edge connectors
on one side, and the 50 pin connectors via flat cables on the other side.

                       14-way
OEC-5            |----------------|
|----------------|----------------|-----------------|
|      1       2       3       4       5      6     |
|A   74273   7417             555                   |
|                                                   |
|B   74273   7417     555     7493    76477  LM380  |
|                                                   |
|C   DIP16   74121    4066    7400           4030   |
|                                                   |
|D   LM324   LM324    LM3900  LM3900  458    4006   |
|                                                   |
|                                                   |
|                                                   |
|                                                   |
| VR1   VR2   VR3   VR4   VR5   VR6   VR7    VOL    |
|---------------------------------------------------|

Notes:
      DIP16 - Joins to DIP16 socket at 3F on IOC2-A PCB via a 16-wire flat cable.
      VR1-7 - Potentiometers for volume of sounds
      VOL   - Master Volume
      Note PCB also contains a lot of resistors, transistors, capacitors etc



                            28-way
IOC2-A           |-----------------------------|
|----------------|-----------------------------|----------------|
|      1         2       3       4       5      6      7        |
|A     DIP16   DIP16   DIP16   DIP16                            |
|                                                           VR1 |
|B     D1-8    7404    7404    7404    LM324          4066      |
|                                                               |
|C     D9-16   74240   7406    DIP8           LM324         VR2 |
|                                                               |
|D     74240   74240   74175   75472   555           555        |
|                                                               |
|E     DSW1    74240   74273   7417                         VR3 |
|                                                               |
|F     DIP16   DIP16   DIP16   555     458    LM3900   LM3900   |
|                                                               |
|G     74244   74244   7432    555                          VR4 |
|                                                               |
|H                                                              |
|                                                               |
|                                                               |
|  |-------------|                                              |
|--|-------------|----------------------------------------------|
      50 pin

Notes:
      DIP8 - Unpopulated position for DIP8 IC
      DIP16- Unpopulated position for DIP16 IC
      D1-16- Diodes
      VR1-4- Potentiometers for volume of sounds



                            28-way
CRTC1-A          |-----------------------------|
|----------------|-----------------------------|----------------|
|   1      2      3      4      5      6      7      8      9   |
|A 20MHz  7455   Diodes 1-4   DIP16  DIP16  DIP16  4027   4027  |
|                      &                                        |
|B 74163  7404   Transtrs 1-7 DIP16  DIP16  DIP16  4027   4027  |
|                                                               |
|C 7486   74161  7404         DIP16  DIP16  DIP16  4027   4027  |
|                                                               |
|D 7486   74161  7408      82S09     74374  74157  4027   4027  |
|                                                               |
|E 7486   74161  7402  7411   74157  74377  7408   4027   4027  |
|                                                               |
|F 7486   74161  7411  74157  74244  74377  7400   74153  74157 |
|                                                               |
|G 74107  7404   7474  74175  74107  74157  7404   74377  74157 |
|                                                               |
|H 7404   7404   74244 74139  7404   7427   7474   74164  7404  |
|                                                               |
|                                                               |
|  |-------------|       |-------------|       |-------------|  |
|--|-------------|-------|-------------|-------|-------------|--|
      50 pin                 50 pin                50 pin

Notes:
      4027 - MOSTEK MK4027-4 RAM (DIP16)
      DIP16- Unpopulated position for DIP16 IC



                            28-way
CRTC2-A          |-----------------------------|
|----------------|-----------------------------|----------------|
|   1      2      3      4      5      6      7      8      9   |
|A 7402  74377  74157  7400   74194  74S288        4027   4027  |
|                                                               |
|B 7486  74153  7486   7400   74194  74S288        4027   4027  |
|                                                               |
|C 7408  74374  74158         74273  7427          4027   4027  |
|                                                               |
|D 7400  74244  25S10  74181  7430   7408          4027   4027  |
|                                                               |
|E 7400  74153  25S10  74181         7402          4027   4027  |
|                                                               |
|F 7408  74153         7411   7404   7404          4027   4027  |
|                                                               |
|G 7404  74153  DIP16  74153  74153  74283         4027   4027  |
|                                                               |
|H 74138 74244  7474   74153  74153  74157         4027   4027  |
|                                                               |
|                                                               |
|  |-------------|       |-------------|       |-------------|  |
|--|-------------|-------|-------------|-------|-------------|--|
      50 pin                 50 pin                50 pin

Notes:
      74S288 - 32bytes x8 Bipolar PROM (DIP16, both PROMs contain identical contents)
      4027   - MOSTEK MK4027-4 RAM (DIP16)



                            28-way
SBC-A            |-----------------------------|
|----------------|-----------------------------|----------------|
|   1      2      3      4      5      6      7      8      9   |
|A 74161        DIP18  4045   4045                 ROM2   ROM1  |
|                                                               |
|B 7404  7402   74139  DIP18  DIP18                ROM4   ROM3  |
|                                                               |
|C 7404  7404   7420   4045   4045                 ROM6   ROM5  |
|                                                               |
|D              7420   DIP18  DIP18                ROM8   ROM7  |
|                                                               |
|E 74241 74241         8216   8216                 DIP24  DIP24 |
|                                                               |
|F    Z80              74241                       DIP24  DIP24 |
|                                                               |
|G       7407          74241                       ROM14  ROM13 |
|                                                               |
|H                     74241  7442   7442   74244  ROM16  ROM15 |
|                                                               |
|                                                               |
|  |-------------|       |-------------|                        |
|--|-------------|-------|-------------|------------------------|
      50 pin                 50 pin

Notes:
      4045 - TMS4045-45NL RAM (DIP18)
      8216 - Mitsubishi M5L8216 RAM (DIP16)
      DIP18- Unpopulated position for DIP18 IC
      DIP24- Unpopulated position for DIP24 IC
      All ROMs type 2708 (DIP24)

*/




static unsigned char unkram[65536];

static int io9400;
static int io9401;

VIDEO_UPDATE( spaceg )
{
	copybitmap(bitmap,tmpbitmap,0,0,0,0,cliprect,TRANSPARENCY_NONE,0);

}

static PALETTE_INIT( spaceg )
{
	int i;

	for (i=0; i<128; i++)
		palette_set_color (i, 0x00,0x00,0x00);


	palette_set_color (0, 0x00,0x00,0x00);	//ok czarny
	palette_set_color (1, 0x7f,0x00,0x00);//???
	palette_set_color (2, 0xff,0xff,0xff);	//ok+ bialy
	palette_set_color (3, 0xff,0x00,0x00);	//ok j.czerw.
	palette_set_color (4, 0x3f,0x3f,0xff);	//ok j.niebieski
	palette_set_color (5, 0x3f,0xff,0x3f);	//ok j.zielony
	palette_set_color (6, 0xff,0xbf,0xbf);	//ok+ 'majtki'
	palette_set_color (7, 0xff,0xff,0x00);	//ok+ zolty

	palette_set_color (8, 0xff,0x7f,0x00);	//ok+ pomaranczowy
	palette_set_color (9, 0x3f,0xbf,0xff);	//ok j.niebieski (ciemniejszy od 13)
	palette_set_color (10,0x3f,0xbf,0x3f);	//ok+ c.zielony
	palette_set_color (11,0x00,0xff,0x00);	//ok j.zielony
	palette_set_color (12,0x7f,0x00,0x00);	//ok brazowy (c.czerw)
	palette_set_color (13,0x7f,0xbf,0xff);	//ok j.niebieski (jasniejszy od 9)
	palette_set_color (14,0x00,0xff,0xff);//???
	palette_set_color (15,0x7f,0x7f,0x7f);//???

}

static WRITE8_HANDLER( zvideoram_w )
{
	int i,x,y,col;

	col = unkram[0x400];

	if (col>0x0f) ui_popup("color>0x0f =%2d",col);

	col &=0x0f;

	switch(io9401)
	{
	case 0x0d:	/* 1101 */
		videoram[offset] &= ~data;
		data = videoram[offset];
		break;

	case 0x01:	/* 0001 */
	case 0x00:	/* 0000 */
		videoram[offset] = data;
		break;

	default:
		logerror("mode =%02x pc=%04x\n",io9401,activecpu_get_pc() );
		ui_popup("mode =%02x pc=%04x\n",io9401,activecpu_get_pc() );
		return;
		break;
	}


	unkram  [offset] = col;

	y= 255 - (offset & 0xff);
	x= 255 - ((offset>>8)*8);

	/* draw modified eight pixels */
	for (i = 0; i < 8; i++)
	{
		plot_pixel(tmpbitmap, x-((io9400&0xe0)>>5), y, (data&1)?Machine->pens[col]:Machine->pens[0]);
		x++;
		data >>= 1;
	}
}

static READ8_HANDLER(fake_r)
{
//  logerror("vb %04x\n", activecpu_get_pc() );
	return 0;
}


static READ8_HANDLER(spaceg_colorram_r)
{

int rgbcolor;

	if (offset<0x400)
	{
		rgbcolor = (unkram[offset]<<1) | ((offset &0x100)>>8);

		if ((offset>=0x200) && (offset<0x220)) /* 0xa200- 0xa21f */
		{
			/* palette 1 */
			int r,g,b,col_ind;
			r = (rgbcolor >> 0) & 7;
			b = (rgbcolor >> 3) & 7;
			g = (rgbcolor >> 6) & 7;
			col_ind = offset & 0x1f;
			palette_set_color(0x10 + 0x00 + col_ind, r*(255.0/7), g*(255.0/7), b*(255.0/7));
		}
		else
		if ((offset>=0x300) && (offset<0x320)) /* 0xa300- 0xa31f */
		{
			/* palette 2 */
			int r,g,b,col_ind;
			r = (rgbcolor >> 0) & 7;
			b = (rgbcolor >> 3) & 7;
			g = (rgbcolor >> 6) & 7;
			col_ind = offset & 0x1f;
			palette_set_color(0x10 + 0x00 + col_ind, r*(255.0/7), g*(255.0/7), b*(255.0/7));
		}
		else
			logerror("palette? read from unkram offset = %04x\n",offset);
	}

if (io9401!=0x40)
	logerror("unkram read in mode: 9401 = %02x (offset=%04x)\n",io9401,offset);


	return unkram[offset];
}

static WRITE8_HANDLER(spaceg_colorram_w)
{
	int i,x,y,col;

	unkram[offset] = data;


	/* refresh eight pixels because the color might have just been changed */
	data = videoram[offset];
	col  = unkram  [offset];

	y= 255 - (offset & 0xff);
	x= 255 - ((offset>>8)*8);

	/* draw modified eight pixels */
	for (i = 0; i < 8; i++)
	{
		plot_pixel(tmpbitmap, x-((io9400&0xe0)>>5), y, (data&1)?Machine->pens[col]:Machine->pens[0]);
		x++;
		data >>= 1;
	}
}


static READ8_HANDLER(zr_r)
{
	return videoram[offset];
}

static WRITE8_HANDLER(io9400_w)
{
	io9400=data;
	if (io9400 & 0x1f)
	{
		logerror("!!9400 =%02x pc=%04x\n",io9400,activecpu_get_pc() );
	}
}

static WRITE8_HANDLER(io9401_w)
{
	io9401 = data;


//0x40 - loads the palette on READ, address line 0x100 is the ninth bit (3rd bit of the red component)

	switch(io9401)
	{
	case 0x0d:
	case 0x01:
	case 0x00:
			break;
	default:
		logerror("!!9401 mode =%02x pc=%04x\n",io9401,activecpu_get_pc() );
		break;
	}
}

static ADDRESS_MAP_START( readwritemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x3000, 0x3fff) AM_ROM
	AM_RANGE(0x7000, 0x77ff) AM_RAM
	AM_RANGE(0xc000, 0xdfff) AM_READWRITE(zr_r, zvideoram_w) AM_BASE(&videoram)
	AM_RANGE(0xa000, 0xbfff) AM_READWRITE(spaceg_colorram_r, spaceg_colorram_w)


	AM_RANGE(0x9400, 0x9400) AM_WRITE(io9400_w) /* gfx ctrl */
	AM_RANGE(0x9401, 0x9401) AM_WRITE(io9401_w) /* gfx ctrl */
	/* 9402 -
        bits 0 and 1 probably control the lamps under the player 1 and player 2 start buttons
        bit 3 is probably a flip screen
        bit 7 - unknown - set to 1 during the gameplay (coinlock ?)
    */
	AM_RANGE(0x9402, 0x9407) AM_RAM

	AM_RANGE(0x9800, 0x9800) AM_READ(fake_r) //watchdog read ? nmi mask/ack ?
	AM_RANGE(0x9801, 0x9801) AM_READ(input_port_0_r)
	AM_RANGE(0x9802, 0x9802) AM_READ(input_port_1_r)
	AM_RANGE(0x9805, 0x9805) AM_READ(input_port_2_r) /* pl 1 */
	AM_RANGE(0x9806, 0x9806) AM_READ(input_port_3_r) /* pl 2 */
ADDRESS_MAP_END



INPUT_PORTS_START( spaceg )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )

	PORT_START
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START /* player 1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON4 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON3 )

	PORT_START /* player 2 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON4 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON3 )


INPUT_PORTS_END


static MACHINE_DRIVER_START( spaceg )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,2500000)		 /* 2.5 MHz */
	MDRV_CPU_PROGRAM_MAP(readwritemem,0)
	MDRV_CPU_VBLANK_INT(nmi_line_pulse,1)	/* 60 Hz NMIs (verified) */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 0, 256-1-3*8)
	MDRV_PALETTE_LENGTH(16+128-16)
	MDRV_PALETTE_INIT( spaceg )
	MDRV_VIDEO_START(generic_bitmapped)
	MDRV_VIDEO_UPDATE( spaceg )

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

//  MDRV_SOUND_ADD(SN76496, 15468480/4)
//  MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

//  MDRV_SOUND_ADD(SN76496, 15468480/4)
//  MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

//  MDRV_SOUND_ADD(SN76496, 15468480/4)
//  MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

//  MDRV_SOUND_ADD(DAC, 0)
//  MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( spaceg )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "1.9a",		0x0000, 0x0400, CRC(d6f80b0e) SHA1(503e832c065bb302ec60ed239c4d99a605cb931a) )
	ROM_LOAD( "2.8a",		0x0400, 0x0400, CRC(708b8eec) SHA1(70f9b1506a653985d9d3eacba220f0a4eb241318) )
	ROM_LOAD( "3.9b",		0x0800, 0x0400, CRC(7f0b5cb4) SHA1(97a7125d046e7191b5d3be8f203d1bbb6f988209) )
	ROM_LOAD( "4.8b",		0x0c00, 0x0400, CRC(8b40a154) SHA1(f42bdd8e94090fc5eae58e833b8443300d3ce991) )
	ROM_LOAD( "5.9c",		0x1000, 0x0400, CRC(5279241c) SHA1(7278b6b037322b2f75311ed247f2de3c4816681b) )
	ROM_LOAD( "6.8c",		0x1400, 0x0400, CRC(9b84fe3a) SHA1(7ebeca10ee11d22f4af06be9f381f46864464ec2) )
	ROM_LOAD( "7.9d",		0x1800, 0x0400, CRC(95279b25) SHA1(367d129d4dd2cfea2a2f4703f41f24cc49453715) )
	ROM_LOAD( "8.8d",		0x1c00, 0x0400, CRC(6a824383) SHA1(7c43f2c7d1f070d93f6a8b5b4f7f97f3578bd91d) )
	ROM_LOAD( "13.9g",	0x3000, 0x0400, CRC(dccc386f) SHA1(5d493da3e7b8269314dd54f0b3ba9f71829a14da) )
	ROM_LOAD( "14.8g",	0x3400, 0x0400, CRC(dc9a10c2) SHA1(8fb2316d6e8aeef558d0da5029e2932abf47a6b4) )
	ROM_LOAD( "15.9h",	0x3800, 0x0400, CRC(55e2950d) SHA1(2241c3620c9a6df8b8bd234ccee9af5d3d19a5d4) )
	ROM_LOAD( "16.8h",	0x3c00, 0x0400, CRC(567259c4) SHA1(b2c3f7aaceabea075af6a43b89fb7331732278c8) )
ROM_END


GAME( 1979, spaceg, 0, spaceg, spaceg, 0, ROT90, "Omori", "Space Guerilla", GAME_IMPERFECT_GRAPHICS | GAME_NO_SOUND,0)
