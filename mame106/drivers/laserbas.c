/********************************************
 Laser Base / Future Flash driver

 IC marked as Z1 is probably protection device
 mapped in memory region f800-fbff
 (simil. to the one used in Parallel Turn)

 Reads form this device depends on previous
 writes (adr, data), address and previous
 operation (read or write).
 Pinout is almost identical to 2716 - type EPROM,
 except separated /RD and /WR   signals and lacks
 of /CS

 Tomasz Slanina analog [at] op.pl

********************************************/

#include "driver.h"

static UINT8 *vram1,*vram2;
static int vrambank=0;

VIDEO_START(laserbas)
{
	vram1=auto_malloc(0x8000);
	vram2=auto_malloc(0x8000);
	tmpbitmap = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height);
	return 0;
}

VIDEO_UPDATE(laserbas)
{
	int x,y;
 	for(y=0;y<256;y++)
		for(x=0;x<128;x++)
		{
			plot_pixel(tmpbitmap, x*2, y,  Machine->pens[vram1[y*128+x]&0xf]+16);
			plot_pixel(tmpbitmap, x*2+1, y, Machine->pens[vram1[y*128+x]>>4]+16);
		}
	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_NONE,0);
	for(y=0;y<256;y++)
		for(x=0;x<128;x++)
		{
			plot_pixel(tmpbitmap, x*2, y,  Machine->pens[vram2[y*128+x]&0xf]);
			plot_pixel(tmpbitmap, x*2+1, y, Machine->pens[vram2[y*128+x]>>4]);
		}
	copybitmap(bitmap,tmpbitmap,0,0,0,0,&Machine->visible_area,TRANSPARENCY_PEN,0);
}

static READ8_HANDLER(vram_r)
{
	if(!vrambank)
		return vram1[offset];
	else
		return vram2[offset];
}

static WRITE8_HANDLER(vram_w)
{
	if(!vrambank)
		vram1[offset]=data;
	else
		vram2[offset]=data;
}

static READ8_HANDLER( read_unk )
{
	static int count=0;
	count ^= 0x80;
	return count|0x7f;
}

static WRITE8_HANDLER(palette_w)
{
	palette_set_color(offset,(data&0xe0),(data<<3)&0xe0,(data<<6)&0xe0);
}

static WRITE8_HANDLER(vrambank_w)
{
	if((offset&0xf1)==0x10)
		vrambank=data&0x40;
}

static ADDRESS_MAP_START( laserbas_memory, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0xbfff) AM_READ(vram_r) AM_WRITE(vram_w)
	AM_RANGE(0xc000, 0xf7ff) AM_ROM
	AM_RANGE(0xf800, 0xfbff) AM_RAM /* protection device */
	AM_RANGE(0xfc00, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( laserbas_io, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x1f) AM_WRITE(vrambank_w)
	AM_RANGE(0x20, 0x20) AM_READ(read_unk) AM_WRITENOP//write = ram/rom bank ? at fc00-f800 ?
	AM_RANGE(0x21, 0x21) AM_READ(input_port_0_r)
	AM_RANGE(0x80, 0x9f) AM_WRITE(palette_w)
ADDRESS_MAP_END

INPUT_PORTS_START( laserbas )
	PORT_START
		PORT_DIPNAME( 0x01, 0x00, "0-0" )
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x01, DEF_STR( On ) )
		PORT_DIPNAME( 0x02, 0x00, "0-1" )
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x02, DEF_STR( On ) )
		PORT_DIPNAME( 0x04, 0x00, "0-2" )
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x04, DEF_STR( On ) )
		PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START1 )
		PORT_DIPNAME( 0x010, 0x10, "0-3" )
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x010, DEF_STR( On ) )
		PORT_DIPNAME( 0x020, 0x20, "Test Mode" )
		PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x000, DEF_STR( On ) )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )
INPUT_PORTS_END

INTERRUPT_GEN( laserbas_interrupt )
{
	if(cpu_getvblank())
		 cpunum_set_input_line(0, 0, HOLD_LINE);
	else
		cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
}

static MACHINE_DRIVER_START( laserbas )
	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(laserbas_memory,0)
	MDRV_CPU_IO_MAP(laserbas_io,0)
	MDRV_CPU_VBLANK_INT(laserbas_interrupt,2)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 0*8, 32*8-1)
	MDRV_PALETTE_LENGTH(32)
	MDRV_VIDEO_START(laserbas)
	MDRV_VIDEO_UPDATE(laserbas)

MACHINE_DRIVER_END

/*
Amstar LaserBase 1981 (Hoei)

XBC-101-00-1 - CPU board
  Z80A (D780C-1)
  2 NB5K8253
  2 2114 Rams
  8 Dipswitches

XBC-102-01-1 - RAM board?
  HD46505R
  32 MB8118 Rams

------------------------------------

Filename  Label Type   CSum Description
--------- ----- ------ ---- -----------------------------
MB8532.1    1   2532   9316
MB8532.2    2   2532   5662
MB8532.3    3   2532   7E9F
MB8532.4    4   2532   7CAD
MB8532.5    5   2532   C7D4 (Marked F.F.)
MB8532.6    6   2532   16BE
MB8532.7    7   2532   CF6C (Marked F.F.)
MB8716.8    8   2716   9601 (Marked F.F.)
TI2716.Z1  Z1   TI2716 D925
--------- ----- ------ ---- -----------------------------

I believe the F.F. markings on these chips show that
these roms have been changed to Future Flash.

It is unknown what the Z1 chip is, but the label screened
on the board under the socket says 2716.  All the identifying
numbers have been scratched off and has Z1 stamped on it.
It appears that each one was then numbered by hand in red
marker and stamped with white ink with Z1.

The Z1 chip was read from 3 different boards, it is Valid.
The chips were numbered 69, 82 & 624, all three read the same.
Turns out it was a TI2716.  The TI chip has A10 on a different
pin than a standard 2716.

*/

ROM_START( laserbas )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "mb8532.1",   0x0000, 0x1000, CRC(ef85e0c5) SHA1(c26da919c206a23eb6e53ffe39acd5a5dfd47c18) )
	ROM_LOAD( "mb8532.2",   0x1000, 0x1000, CRC(0ba2236c) SHA1(416e4be957c395b05537d2e513e0c4561d8ca7c5) )
	ROM_LOAD( "mb8532.3",   0x2000, 0x1000, CRC(83998e0b) SHA1(ac435fb8dd67aec0737d6c750c527841b2b91a5b) )
	ROM_LOAD( "mb8532.4",   0x3000, 0x1000, CRC(00f9d909) SHA1(90b800cc5fcea53454584f8ad93eebbd193bdd21) )
	ROM_LOAD( "lb2532.5",   0xc000, 0x1000, CRC(6459073e) SHA1(78b8a23534826dd2d3b3c6c5d5708c8a78a4b6bf) )
	ROM_LOAD( "lb2532.6",   0xd000, 0x1000, CRC(a2dc1e7e) SHA1(78643a3aa852c73dab12e09a6cfc53141c936d12) )
	ROM_LOAD( "mb8532.7",   0xe000, 0x1000, CRC(9d2148d7) SHA1(24954d82a09d9fcfdc61e91b7c824daa5dd701c3) )
	ROM_LOAD( "mb8516.8",   0xf000, 0x0800, CRC(623f558f) SHA1(be6c6565df658555f21c43a8c2459cf399794a84) )
ROM_END

ROM_START( laserbsa )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "2732.u1",       0x0000, 0x1000, CRC(f3ab00dc) SHA1(4730b13b55c93c71ed483463e180e71e506cfbd6) )
	ROM_LOAD( "2732.u2",       0x1000, 0x1000, CRC(0ba2236c) SHA1(416e4be957c395b05537d2e513e0c4561d8ca7c5) )
	ROM_LOAD( "mb8532.u3",     0x2000, 0x1000, CRC(c58a7745) SHA1(382e2235d89520860335c1c2760339e116c0466f) )
	ROM_LOAD( "mbm2732.u4",    0x3000, 0x1000, CRC(00f9d909) SHA1(90b800cc5fcea53454584f8ad93eebbd193bdd21) )
	ROM_LOAD( "2732.u5",       0xc000, 0x1000, CRC(6459073e) SHA1(78b8a23534826dd2d3b3c6c5d5708c8a78a4b6bf) )
	ROM_LOAD( "2732.u6",       0xd000, 0x1000, CRC(a2dc1e7e) SHA1(78643a3aa852c73dab12e09a6cfc53141c936d12) )
	ROM_LOAD( "2732.u7",       0xe000, 0x1000, CRC(9d2148d7) SHA1(24954d82a09d9fcfdc61e91b7c824daa5dd701c3) )
	ROM_LOAD( "mb8516.u8",     0xf000, 0x0800, CRC(623f558f) SHA1(be6c6565df658555f21c43a8c2459cf399794a84) )
ROM_END

/*
It was unclear what type of device FF.9 was. The silkscreen on the PCB said
2716,
but the device is a masked ROM with its identifying marks rubbed off.
I dumped it
as a 2716 (FF.9), a 2532 like the others (FF.9A) and a 2732 (FF.9B).
*/

ROM_START( futflash )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "ff.1",    	  0x0000, 0x1000, CRC(bcd6b998) SHA1(4a210c40ce6015e2b921558b7571b7f2a27e1815) )
	ROM_LOAD( "ff.2",         0x1000, 0x1000, CRC(1b1f6953) SHA1(8cd7b7e2236700ce63c60b4d2286099c8091bdbd) )
	ROM_LOAD( "ff.3",         0x2000, 0x1000, CRC(30008f04) SHA1(e03b2dbcb6d2615650cdd47ecf1d587906ce149b) )
	ROM_LOAD( "ff.4",         0x3000, 0x1000, CRC(e559aa12) SHA1(0fecfb60b0147e8060c640f684f69503478200ff) )
	ROM_LOAD( "ff.5",         0xc000, 0x1000, CRC(6459073e) SHA1(78b8a23534826dd2d3b3c6c5d5708c8a78a4b6bf) )
	ROM_LOAD( "ff.6",         0xd000, 0x1000, CRC(a8b17f49) SHA1(aea349bd19d001233bfb1805e586c950275010b4) )
	ROM_LOAD( "ff.7",         0xe000, 0x1000, CRC(9d2148d7) SHA1(24954d82a09d9fcfdc61e91b7c824daa5dd701c3) )
	ROM_LOAD( "ff.8",         0xf000, 0x0800, CRC(623f558f) SHA1(be6c6565df658555f21c43a8c2459cf399794a84) )
ROM_END

GAME( 1981, laserbas, 0,        laserbas, laserbas, 0, ROT270, "Amstar/HOEI", "Laser Base (set 1)", GAME_NO_SOUND | GAME_NOT_WORKING,0)
GAME( 1981, laserbsa, laserbas, laserbas, laserbas, 0, ROT270, "Amstar/HOEI", "Laser Base (set 2)", GAME_NO_SOUND | GAME_NOT_WORKING ,0)
GAME( 19??, futflash, laserbas, laserbas, laserbas, 0, ROT270, "HOEI",        "Future Flash",       GAME_NO_SOUND | GAME_NOT_WORKING,0)
