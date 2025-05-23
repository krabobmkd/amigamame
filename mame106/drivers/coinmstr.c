/*

  Coinmaster trivia games

  preliminary driver by Pierpaolo Prazzoli

  TODO:
  - finish video emulation (colors, missing banking bits)
  - where is palette ?
  - finish inputs
  - finish question roms reading
  - hook up all the PIAs

  Notes:
  - Some trivia seems to accept 2 type of eproms for question roms:
    0x4000 or 0x8000 bytes long. This check is done with the 1st read
    from the rom (I think from offset 0) and if it's 0x10, it means a
    0x4000 bytes eprom or if it's 0x20, it means a 0x8000 one.
    Also supnudg2 only tests 0x20 as 1st byte, so accepting only
    the 2nd type of eproms.

*/


#include "driver.h"
#include "machine/6821pia.h"
#include "vidhrdw/crtc6845.h"
#include "sound/ay8910.h"


static UINT8 *attr_ram1, *attr_ram2;
static tilemap *bg_tilemap;

static UINT8 question_adr[4];

static WRITE8_HANDLER( quizmstr_bg_w )
{
	videoram[offset] = data;

	if(offset >= crtc6845_start_addr)
		tilemap_mark_tile_dirty(bg_tilemap,offset - crtc6845_start_addr);
}

static WRITE8_HANDLER( quizmstr_attr1_w )
{
	attr_ram1[offset] = data;

	if(offset >= crtc6845_start_addr)
		tilemap_mark_tile_dirty(bg_tilemap,offset - crtc6845_start_addr);
}

static WRITE8_HANDLER( quizmstr_attr2_w )
{
	attr_ram2[offset] = data;

	if(offset >= crtc6845_start_addr)
		tilemap_mark_tile_dirty(bg_tilemap,offset - crtc6845_start_addr);
}

static READ8_HANDLER( question_r )
{
	int address;
	UINT8 *questions = memory_region(REGION_USER1);

	switch(question_adr[2])
	{
		case 0x38: address = 0x00000; break; // question_adr[3] == 7
		case 0x39: address = 0x08000; break; // question_adr[3] == 7
		case 0x3a: address = 0x10000; break; // question_adr[3] == 7
		case 0x3b: address = 0x18000; break; // question_adr[3] == 7
		case 0x3c: address = 0x20000; break; // question_adr[3] == 7
		case 0x3d: address = 0x28000; break; // question_adr[3] == 7
		case 0x3e: address = 0x30000; break; // question_adr[3] == 7
		case 0x07: address = 0x38000; break; // question_adr[3] == 7
		case 0x0f: address = 0x40000; break; // question_adr[3] == 7
		case 0x17: address = 0x48000; break; // question_adr[3] == 7
		case 0x1f: address = 0x50000; break; // question_adr[3] == 7
		case 0x27: address = 0x58000; break; // question_adr[3] == 7
		case 0x2f: address = 0x60000; break; // question_adr[3] == 7
		case 0x37: address = 0x68000; break; // question_adr[3] == 7
		case 0x3f: address = 0x70000 + question_adr[3] * 0x8000; break;

		default:
			address = 0;
			logerror("unknown question rom # = %02X\n",question_adr[2]);
	}

	if(question_adr[3] == 6 || question_adr[3] > 7)
		logerror("question_adr[3] = %02X\n",question_adr[3]);

/*
    in these offsets they set 0x80... why?

    if( (question_adr[0] & 0x5f) == 0x00 ||
        (question_adr[0] & 0x5f) == 0x01 ||
        (question_adr[0] & 0x5f) == 0x0f ||
        (question_adr[0] & 0x5f) == 0x56 )
*/


//  don't know...
//  address |= ((question_adr[0] & 0x7f) << 8) | question_adr[1];
	address |= (question_adr[1] << 7) | (question_adr[0] & 0x7f);

	return questions[address];
}

static WRITE8_HANDLER( question_w )
{
	if(data != question_adr[offset])
	{
		logerror("offset = %d data = %02X\n",offset,data);
	}

	question_adr[offset] = data;
}

// Common memory map

static ADDRESS_MAP_START( coinmstr_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xdfff) AM_RAM // supnudg2 writes here...
	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_WRITE(quizmstr_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xe800, 0xefff) AM_RAM AM_WRITE(quizmstr_attr1_w) AM_BASE(&attr_ram1)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM AM_WRITE(quizmstr_attr2_w) AM_BASE(&attr_ram2)
	AM_RANGE(0xf800, 0xffff) AM_RAM // supnudg2 writes here...
ADDRESS_MAP_END

// Different I/O mappping for every game

static ADDRESS_MAP_START( quizmstr_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) | AMEF_UNMAP(1) )
	AM_RANGE(0x00, 0x00) AM_READ(question_r)
	AM_RANGE(0x00, 0x03) AM_WRITE(question_w)
	AM_RANGE(0x40, 0x40) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x41, 0x41) AM_READWRITE(AY8910_read_port_0_r, AY8910_write_port_0_w)
	AM_RANGE(0x48, 0x4b) AM_READWRITE(pia_0_r, pia_0_w)
	AM_RANGE(0x50, 0x53) AM_READNOP
	AM_RANGE(0x50, 0x53) AM_WRITENOP
	AM_RANGE(0x58, 0x5b) AM_READWRITE(pia_2_r, pia_2_w)
	AM_RANGE(0x70, 0x70) AM_WRITE(crtc6845_address_w)
	AM_RANGE(0x71, 0x71) AM_WRITE(crtc6845_register_w)
	AM_RANGE(0xc0, 0xc3) AM_READNOP
	AM_RANGE(0xc0, 0xc3) AM_WRITENOP
ADDRESS_MAP_END

static ADDRESS_MAP_START( trailblz_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x00, 0x00) AM_READ(question_r)
	AM_RANGE(0x00, 0x03) AM_WRITE(question_w)
	AM_RANGE(0x40, 0x40) AM_WRITE(crtc6845_address_w)
	AM_RANGE(0x41, 0x41) AM_WRITE(crtc6845_register_w)
	AM_RANGE(0x48, 0x48) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x49, 0x49) AM_READWRITE(AY8910_read_port_0_r, AY8910_write_port_0_w)
	AM_RANGE(0x50, 0x53) AM_READWRITE(pia_0_r, pia_0_w) //?
	AM_RANGE(0x60, 0x63) AM_READWRITE(pia_1_r, pia_1_w)
	AM_RANGE(0x70, 0x73) AM_READWRITE(pia_2_r, pia_2_w)
	AM_RANGE(0xc1, 0xc3) AM_WRITENOP
ADDRESS_MAP_END

static ADDRESS_MAP_START( supnudg2_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) | AMEF_UNMAP(1) )
	AM_RANGE(0x00, 0x00) AM_READ(question_r)
	AM_RANGE(0x00, 0x03) AM_WRITE(question_w)
	AM_RANGE(0x40, 0x41) AM_READNOP
	AM_RANGE(0x40, 0x43) AM_WRITENOP
	AM_RANGE(0x43, 0x43) AM_READNOP
	AM_RANGE(0x48, 0x48) AM_WRITE(crtc6845_address_w)
	AM_RANGE(0x49, 0x49) AM_WRITE(crtc6845_register_w)
	AM_RANGE(0x50, 0x51) AM_READNOP
	AM_RANGE(0x50, 0x53) AM_WRITENOP
	AM_RANGE(0x53, 0x53) AM_READNOP
	AM_RANGE(0x68, 0x69) AM_READNOP
	AM_RANGE(0x68, 0x6b) AM_WRITENOP
	AM_RANGE(0x6b, 0x6b) AM_READNOP
	AM_RANGE(0x78, 0x78) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x79, 0x79) AM_READWRITE(AY8910_read_port_0_r, AY8910_write_port_0_w)
	AM_RANGE(0xc0, 0xc1) AM_READNOP
	AM_RANGE(0xc0, 0xc3) AM_WRITENOP
ADDRESS_MAP_END

INPUT_PORTS_START( quizmstr )
	PORT_START
	PORT_DIPNAME( 0x01, 0x01, "0" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Show Information" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_DIPNAME( 0x20, 0x20, "Question Roms Test" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON3 )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_DIPNAME( 0x02, 0x02, "1" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, "2" )
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
	PORT_DIPNAME( 0x01, 0x01, "3" )
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
	PORT_DIPNAME( 0x01, 0x01, "4" )
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
	PORT_DIPNAME( 0x40, 0x40, "NVRAM Reset?" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Test?" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

INPUT_PORTS_START( trailblz )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_HIGH )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_DIPNAME( 0x02, 0x02, "1" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Cont")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Pass")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Sel")
	PORT_DIPNAME( 0x40, 0x40, "Show Refill" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Show Stats" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, "2" )
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
	PORT_DIPNAME( 0x01, 0x01, "3" )
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
	PORT_DIPNAME( 0x01, 0x01, "4" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Tests" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "NVRAM Reset" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout, 0, 32 },
	{ -1 }
};

static void get_bg_tile_info(int tile_index)
{
	int tile = videoram[tile_index + crtc6845_start_addr];
	int color = 0;

	tile |= (attr_ram1[tile_index + crtc6845_start_addr] & 0x80) << 1;
	tile |= (attr_ram2[tile_index + crtc6845_start_addr] & 0x80) << 2;

	SET_TILE_INFO(0,tile,color,0)
}

VIDEO_START( coinmstr )
{
	bg_tilemap = tilemap_create(get_bg_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE, 8, 8, 46, 64);

	if(!bg_tilemap)
		return 1;

	return 0;
}

VIDEO_UPDATE( coinmstr )
{
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
}

/* Declare PIA structure */

/* PIA 0 */
static const pia6821_interface quizmstr_pia_0_intf =
{
	/*inputs : A/B,CA/B1,CA/B2 */ input_port_0_r, 0, 0, 0, 0, 0,
	/*outputs: A/B,CA/B2       */ 0, 0, 0, 0,
	/*irqs   : A/B             */ 0, 0
};

/* PIA 1 */
static const pia6821_interface quizmstr_pia_1_intf =
{
	/*inputs : A/B,CA/B1,CA/B2 */ 0, 0, 0, 0, 0, 0,
	/*outputs: A/B,CA/B2       */ 0, 0, 0, 0,
	/*irqs   : A/B             */ 0, 0
};

/* PIA 2 */
static const pia6821_interface quizmstr_pia_2_intf =
{
	/*inputs : A/B,CA/B1,CA/B2 */ input_port_1_r, 0, 0, 0, 0, 0,
	/*outputs: A/B,CA/B2       */ 0, 0, 0, 0,
	/*irqs   : A/B             */ 0, 0
};


/* PIA 0 */
static const pia6821_interface trailblz_pia_0_intf =
{
	/*inputs : A/B,CA/B1,CA/B2 */ 0, 0, 0, 0, 0, 0,
	/*outputs: A/B,CA/B2       */ 0, 0, 0, 0,
	/*irqs   : A/B             */ 0, 0
};

/* PIA 1 */
static const pia6821_interface trailblz_pia_1_intf =
{
	/*inputs : A/B,CA/B1,CA/B2 */ input_port_1_r, 0, 0, 0, 0, 0,
	/*outputs: A/B,CA/B2       */ 0, 0, 0, 0,
	/*irqs   : A/B             */ 0, 0
};

/* PIA 2 */
static const pia6821_interface trailblz_pia_2_intf =
{
	/*inputs : A/B,CA/B1,CA/B2 */ input_port_0_r, 0, 0, 0, 0, 0,
	/*outputs: A/B,CA/B2       */ 0, 0, 0, 0,
	/*irqs   : A/B             */ 0, 0
};

static MACHINE_START( quizmstr )
{
	pia_config(0, PIA_STANDARD_ORDERING, &quizmstr_pia_0_intf);
	pia_config(1, PIA_STANDARD_ORDERING, &quizmstr_pia_1_intf);
	pia_config(2, PIA_STANDARD_ORDERING, &quizmstr_pia_2_intf);
	return 0;
}

static MACHINE_RESET( quizmstr )
{
	pia_reset();
}

static MACHINE_START( trailblz )
{
	pia_config(0, PIA_STANDARD_ORDERING, &trailblz_pia_0_intf);
	pia_config(1, PIA_STANDARD_ORDERING, &trailblz_pia_1_intf);
	pia_config(2, PIA_STANDARD_ORDERING, &trailblz_pia_2_intf);
	return 0;
}

static MACHINE_RESET( trailblz )
{
	pia_reset();
}

struct AY8910interface ay8912_interface =
{
	input_port_4_r
};

static MACHINE_DRIVER_START( coinmstr )
	MDRV_CPU_ADD_TAG("cpu",Z80,8000000) // ?
	MDRV_CPU_PROGRAM_MAP(coinmstr_map,0)
	MDRV_CPU_VBLANK_INT(irq0_line_pulse,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_VISIBLE_AREA(0*8, 46*8-1, 0*8, 32*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(coinmstr)
	MDRV_VIDEO_UPDATE(coinmstr)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8912_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( quizmstr )
	MDRV_IMPORT_FROM(coinmstr)
	MDRV_CPU_MODIFY("cpu")
	MDRV_CPU_IO_MAP(quizmstr_io_map,0)

	MDRV_MACHINE_START(quizmstr)
	MDRV_MACHINE_RESET(quizmstr)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( trailblz )
	MDRV_IMPORT_FROM(coinmstr)
	MDRV_CPU_MODIFY("cpu")
	MDRV_CPU_IO_MAP(trailblz_io_map,0)

	MDRV_MACHINE_START(trailblz)
	MDRV_MACHINE_RESET(trailblz)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( supnudg2 )
	MDRV_IMPORT_FROM(coinmstr)
	MDRV_CPU_MODIFY("cpu")
	MDRV_CPU_IO_MAP(supnudg2_io_map,0)

	MDRV_MACHINE_START(quizmstr)
	MDRV_MACHINE_RESET(quizmstr)
MACHINE_DRIVER_END

/*

Quizmaster
Coinmaster, 1985

PCB Layout
----------


PCB 001-POK                                            5MBYTE MEMORY EXPANSION
|--------------------------------------------------|   |------------------------------|
|   ULN2803      6821                              |   |                              |
|                                                  |   |                              |
|                                NPC_QM4_11.IC45   |   |GESCHICH2.IC19  GESCHICH1.IC20|
|   ULN2003      6821                              |   |                              |
|     ULN2003                    NPC_QM4_21.IC41   |   |      *         GESCHICH3.IC18|
|                                                  |   |                              |
|7               6821                              |   |POPMUSIK2.IC15  POPMUSIK1.IC16|
|2                               HD465055          |   |                              |
|W                                                 |   |      *               *       |
|A                                                 |   |                              |
|Y           AY3-8912                       6116   |   |SPORT2.IC11     SPORT1.IC12   |
|                                                  |   |                              |
|                                                  |   |SPORT4.IC9      SPORT3.IC10   |
|                BATTERY                    6116   |   |                              |
|     DSW1(8)              NM_QM4_11.IC9           |   |GEOGRAPH2.IC7   GEOGRAPH1.IC8 |
|                  NE555                           |   |                              |
|            VOL           NP_QM4_21.IC6    6116   |   |      *               *       |
| LM380                                  PAL       |   |                              |
|                                                  |   |ALLGEME2.IC3    ALLGEME1.IC4  |
|                                        PAL       |   |                              |
|                                                  |   |      *         ALLGEME3.IC2  |
|                                        PAL       |   |                              |
|                                            14MHz |   |                              |
|                              Z80       CN1       |   |                       CN2    |
|                                                  |   |     PAL                      |
|--------------------------------------------------|   |------------------------------|
CN1/2 is connector for top ROM board                    * - unpopulated socket


*/

ROM_START( quizmstr )
	ROM_REGION( 0x10000, REGION_CPU1, ROMREGION_ERASEFF )
	ROM_LOAD( "nm_qm4_11.ic9", 0x0000, 0x4000, CRC(3a233bf0) SHA1(7b91b6f19093e67dd5513a000138421d4ef6f0af) )
	ROM_LOAD( "np_qm4_21.ic6", 0x4000, 0x4000, CRC(a1cd39e4) SHA1(420b0726577471c762ae470bc2138c035f295ad9) )
	/* 0x8000 - 0xbfff empty */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "npc_qm4_11.ic45", 0x0000, 0x2000, CRC(ed48582a) SHA1(0aa2434a43af2990b8ad1cd3fc9f2e5e962f99c7) )
	ROM_LOAD( "npc_qm4_21.ic41", 0x2000, 0x2000, CRC(b67b0183) SHA1(018cabace593e795edfe914cdaedb9ebdf158567) )

	ROM_REGION( 0xa0000, REGION_USER1, ROMREGION_ERASEFF ) /* Question roms */
	/* empty ic1 */
	ROM_LOAD( "allgeme3.ic2",   0x08000, 0x8000, CRC(e9ead7f0) SHA1(c0b8e4e7905f31b74c8d217f0afc91f73d52927b) )
	ROM_LOAD( "allgeme2.ic3",   0x10000, 0x8000, CRC(ac4d2ee8) SHA1(3a64fba8a24ae2e8bfd9d1c27804342e1779bcf6) )
	ROM_LOAD( "allgeme1.ic4",   0x18000, 0x8000, CRC(896e619b) SHA1(6f0faf0ae206f20387024a4a426b3e92429b3b1d) )
	/* empty ic5 */
	/* empty ic6 */
	ROM_LOAD( "geograph2.ic7",  0x30000, 0x8000, CRC(d809eeb6) SHA1(c557cecd3dd641a9c293f1865a423dafcd71af82) )
	ROM_LOAD( "geograph1.ic8",  0x38000, 0x8000, CRC(8984e83c) SHA1(d22c02e9297f804f8560e2e46793e4b6654d0785) )
	ROM_LOAD( "sport4.ic9",     0x40000, 0x8000, CRC(3c37de48) SHA1(bee26e9b15cec0b8e81af59810db17a8f2bdc299) )
	ROM_LOAD( "sport3.ic10",    0x48000, 0x8000, CRC(24abe1e7) SHA1(77373b1fafa4b117b3a1e4c6e8b530e0bb3b4f42) )
	ROM_LOAD( "sport2.ic11",    0x50000, 0x8000, CRC(26645e8e) SHA1(4922dcd417f7d098aaaa6a0320ed1d3e488d3e63) )
	ROM_LOAD( "sport1.ic12",    0x58000, 0x8000, CRC(7be41758) SHA1(8e6452fd902d25a73d3fa89bd7b4c5563669cc92) )
	/* empty ic13 */
	/* empty ic14 */
	ROM_LOAD( "popmusik2.ic15", 0x70000, 0x8000, CRC(d3b9ea70) SHA1(0a92ecdc4e2ddd3c0f40682a46a88bc617829481) )
	ROM_LOAD( "popmusik1.ic16", 0x78000, 0x8000, CRC(685f047e) SHA1(c0254130d57f60435a70effe6376e0cb3f50223f) )
	/* empty ic17 */
	ROM_LOAD( "geschich3.ic18", 0x88000, 0x8000, CRC(26c3ceec) SHA1(bf6fd24576c6159bf7730b04d2ac451bfcf3f757) )
	ROM_LOAD( "geschich2.ic19", 0x90000, 0x8000, CRC(387d166e) SHA1(14edac9ef550ce64fd81567520f3009612aa7221) )
	ROM_LOAD( "geschich1.ic20", 0x98000, 0x8000, CRC(bf4c097f) SHA1(eb14e7bad713d3b03fa3978a7f0087312517cf9e) )
ROM_END


ROM_START( trailblz )
	ROM_REGION( 0x10000, REGION_CPU1, ROMREGION_ERASEFF )
	ROM_LOAD( "1-4.09",       0x0000, 0x4000, CRC(7c34749c) SHA1(3847188a734b32979f376f51f74dff050b610dfb) )
	ROM_LOAD( "2-4.06",       0x4000, 0x4000, CRC(81a9809b) SHA1(4d2bfd5223713a9e2e15130a3176118d400ee63e) )
	/* 0x8000 - 0xbfff empty */

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "1-2.45",       0x0000, 0x2000, CRC(b4a807b1) SHA1(f00a4790adb0c25917a0dc8c98c9b65526304fd3) )
	ROM_LOAD( "2-2.41",       0x2000, 0x2000, CRC(756dd230) SHA1(6d6f440bf1f48cc33d5e46cfc645809d5f8b1f3a) )

	ROM_REGION( 0xa0000, REGION_USER1, ROMREGION_ERASEFF ) /* Question roms */
	ROM_LOAD( "questions.bin", 0x00000, 0xa0000, NO_DUMP )
ROM_END


ROM_START( supnudg2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u3.bin",       0x0000, 0x8000, CRC(ed04e2cc) SHA1(7d90a588cca2d113487710e897771f9d99e37e62) )
	ROM_LOAD( "u4.bin",       0x8000, 0x8000, CRC(0551e859) SHA1(b71640097cc75b78f3013f0e77de328bf1a205b1) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "u23.bin",      0x0000, 0x8000, CRC(726a48ac) SHA1(cd17840067294812edf5bfa88d71fc967388df8e) )
	ROM_LOAD( "u25.bin",      0x8000, 0x8000, CRC(1f7cef5e) SHA1(3abc31d400a0f5dc29c70d8aac42fd6302290cc9) )

	ROM_REGION( 0xa0000, REGION_USER1, 0 ) /* Question roms */
	ROM_LOAD( "q1.bin",       0x00000, 0x8000, CRC(245d679a) SHA1(2d3fbed8c1b3d0bffe7f3bd9088e0a5d207654c7) )
	ROM_LOAD( "q2.bin",       0x08000, 0x8000, CRC(e41ae8fb) SHA1(526c7b60e6ee4dfe05bbabf0e1e986e04ac2f544) )
	ROM_LOAD( "q3.bin",       0x10000, 0x8000, CRC(692218a2) SHA1(b9548dd835d9f3fb3e09bd018c7f9cbecafaee28) )
	ROM_LOAD( "q4.bin",       0x18000, 0x8000, CRC(ce4be482) SHA1(4fd8f24d22d3f1789fc728445cbc5339ed454bb4) )
	ROM_LOAD( "q5.bin",       0x20000, 0x8000, CRC(805672bf) SHA1(0fa68cad0d1c2b11a04a364b5ff64facfa573bbc) )
	ROM_LOAD( "q6.bin",       0x28000, 0x8000, CRC(b4405848) SHA1(5f8ca8b017966e6f358f603efde83f45897f3476) )
	ROM_LOAD( "q7.bin",       0x30000, 0x8000, CRC(32329b78) SHA1(114f097678be734355b8f36f6af7f1cb75ece191) )
	ROM_LOAD( "q8.bin",       0x38000, 0x8000, CRC(25c2aa26) SHA1(7f95553bf98381ced086b6606345bef62fe89a3a) )
	ROM_LOAD( "q9.bin",       0x40000, 0x8000, CRC(c98cb15a) SHA1(7d12064c2bcb34668299cadae3072c7f8434c405) )
	ROM_LOAD( "q10.bin",      0x48000, 0x8000, CRC(0c6c2df5) SHA1(49c92e498a0556032bb8ca56ff5afb9f69a80b3f) )
	ROM_LOAD( "q11.bin",      0x50000, 0x8000, CRC(1c53a264) SHA1(c10cc32b032bd4f890497bdc942e7e8c75ea1d6f) )
	ROM_LOAD( "q12.bin",      0x58000, 0x8000, CRC(c9535bff) SHA1(9c9873642c62971f805dc629f8d1006e35a675f9) )
	ROM_LOAD( "q13.bin",      0x60000, 0x8000, CRC(7a9b9f61) SHA1(7e39fef67fc3c29604ae68358e01330cf5130c06) )
	ROM_LOAD( "q14.bin",      0x68000, 0x8000, CRC(ec35e800) SHA1(0e0ca6fec760f31f464b282a1d7341cc4a29c064) )
	ROM_LOAD( "q15.bin",      0x70000, 0x8000, CRC(9f3738eb) SHA1(e841958f37167e7f9adcd3c965d31e2b7e02f52c) )
	ROM_LOAD( "q16.bin",      0x78000, 0x8000, CRC(af92277c) SHA1(093079fab28e3de443b640d2777cc2980b20af6c) )
	ROM_LOAD( "q17.bin",      0x80000, 0x8000, CRC(522fd485) SHA1(6c2a2626c00015962c460eac0dcb46ea263a4a23) )
	ROM_LOAD( "q18.bin",      0x88000, 0x8000, CRC(54d50510) SHA1(2a8ad2a2e1735f9c7d606b99b3653f823f09d1e8) )
	ROM_LOAD( "q19.bin",      0x90000, 0x8000, CRC(30aa2ff5) SHA1(4a2b4fc9c0c5cab3d374ee4738152209589e0807) )
	ROM_LOAD( "q20.bin",      0x98000, 0x8000, CRC(0845b450) SHA1(c373839ee1ad983e2df41cb22f625c14972372b0) )
ROM_END

DRIVER_INIT( coinmstr )
{
	UINT8 *rom = memory_region(REGION_USER1);
	int length = memory_region_length(REGION_USER1);
	UINT8 *buf = malloc(length);
	int i;

	if (buf)
	{
		memcpy(buf,rom,length);

		for(i = 0; i < length; i++)
		{
			int adr = BITSWAP24(i, 23,22,21,20,19,18,17,16,15, 14,8,7,2,5,12,10,9,11,13,3,6,0,1,4);
			rom[i] = BITSWAP8(buf[adr],3,2,4,1,5,0,6,7);
		}

		free(buf);
	}
}


GAME( 1985, quizmstr, 0, quizmstr, quizmstr, coinmstr, ROT0, "Coinmaster", "Quizmaster",                     GAME_WRONG_COLORS | GAME_UNEMULATED_PROTECTION | GAME_NOT_WORKING ,0)
GAME( 1987, trailblz, 0, trailblz, trailblz, coinmstr, ROT0, "Coinmaster", "Trail Blazer",                   GAME_WRONG_COLORS | GAME_UNEMULATED_PROTECTION | GAME_NOT_WORKING ,0) // or Trail Blazer 2 ?
GAME( 1989, supnudg2, 0, supnudg2, quizmstr, coinmstr, ROT0, "Coinmaster", "Super Nudger II (Version 5.21)", GAME_WRONG_COLORS | GAME_UNEMULATED_PROTECTION | GAME_NOT_WORKING ,0)
