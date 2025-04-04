/***************************************************************************

                      -= Electronic Devices / International Games =-

                    driver by   Luca Elia (l.elia@tin.it)


CPUs    :   2 x 8086? / V20 + 8088
Sound   :   YM2151 [+ DAC] / 4 x MSM5205
Video   :   2 x I.G.1BB 48844758V

----------------------------------------------------------------------------------------
Year + Game             Main CPU    Sound CPU    Sound            Video
----------------------------------------------------------------------------------------
>=1987  Born To Fight   8086?       8086?        YM2151 + DAC     ?
>=1987  Fantasy Land    V20         8088         4 x MSM5205      2 x I.G.1BB 48844758V
1989    Galaxy Gunners  8086?       8086?        YM2151           ?
----------------------------------------------------------------------------------------

[fantland, galaxygn]

- Clocks are unknown and the cpu might be an 8088 or a later x86.

[fantland, borntofi]

- The year of creation isn't displayed, but it's no sooner than 1987 since
  embedded in the roms is: "MS Run-Time Library - Copyright (c) 1987, Microsoft Corp"

[fantland]

- Slowdowns on the ice level's boss

[borntofi]

- Verify sound. Also speech is a bit garbled / low volume (rom 15)
- Trackball controls don't work well

***************************************************************************/

#include "driver.h"
#include "sound/2151intf.h"
#include "sound/dac.h"
#include "sound/msm5205.h"

VIDEO_UPDATE( fantland );

/***************************************************************************

                            Memory Maps - Main CPU

***************************************************************************/

static UINT8 fantland_nmi_enable;

static WRITE8_HANDLER( fantland_nmi_enable_w )
{
	fantland_nmi_enable = data;

	if ((fantland_nmi_enable != 0) && (fantland_nmi_enable != 8))
		logerror("CPU #0 PC = %04X: nmi_enable = %02x\n", activecpu_get_pc(), data);
}

static WRITE8_HANDLER( fantland_soundlatch_w )
{
	soundlatch_w(0,data);
	cpunum_set_input_line(1, INPUT_LINE_NMI, PULSE_LINE);
}

/***************************************************************************
                                Fantasy Land
***************************************************************************/

static ADDRESS_MAP_START( fantland_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x07fff) AM_READ(MRA8_RAM			)
	AM_RANGE(0x00000, 0x7ffff) AM_READ(MRA8_ROM			)
	AM_RANGE(0xa2000, 0xa21ff) AM_READ(MRA8_RAM			)	// not actually read
	AM_RANGE(0xa3000, 0xa3000) AM_READ(input_port_0_r	)
	AM_RANGE(0xa3001, 0xa3001) AM_READ(input_port_1_r	)
	AM_RANGE(0xa3002, 0xa3002) AM_READ(input_port_2_r	)
	AM_RANGE(0xa3003, 0xa3003) AM_READ(input_port_3_r	)
	AM_RANGE(0xa4000, 0xa67ff) AM_READ(MRA8_RAM			)	// not actually read
	AM_RANGE(0xc0000, 0xcffff) AM_READ(MRA8_RAM			)	// ""
	AM_RANGE(0xe0000, 0xfffff) AM_READ(MRA8_ROM			)
ADDRESS_MAP_END

static ADDRESS_MAP_START( fantland_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x07fff) AM_WRITE(MWA8_RAM					)
	AM_RANGE(0xa2000, 0xa21ff) AM_WRITE(paletteram_xRRRRRGGGGGBBBBB_le_w) AM_BASE(&paletteram	)
	AM_RANGE(0xa3000, 0xa3000) AM_WRITE(fantland_nmi_enable_w	)
	AM_RANGE(0xa3002, 0xa3002) AM_WRITE(fantland_soundlatch_w	)
	AM_RANGE(0xa4000, 0xa67ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram		)
	AM_RANGE(0xc0000, 0xcffff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_2	)
ADDRESS_MAP_END


/***************************************************************************
                                Galaxy Gunners
***************************************************************************/

static ADDRESS_MAP_START( galaxygn_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x07fff) AM_READ(MRA8_RAM			)
	AM_RANGE(0x10000, 0x2ffff) AM_READ(MRA8_ROM			)
	AM_RANGE(0x52000, 0x521ff) AM_READ(MRA8_RAM			)	// not actually read
	AM_RANGE(0x53000, 0x53000) AM_READ(input_port_0_r	)
	AM_RANGE(0x53001, 0x53001) AM_READ(input_port_1_r	)
	AM_RANGE(0x53002, 0x53002) AM_READ(input_port_2_r	)
	AM_RANGE(0x53003, 0x53003) AM_READ(input_port_3_r	)
	AM_RANGE(0x54000, 0x567ff) AM_READ(MRA8_RAM			)	// not actually read
	AM_RANGE(0x60000, 0x6ffff) AM_READ(MRA8_RAM			)	// ""
	AM_RANGE(0x70000, 0x7ffff) AM_READ(MRA8_ROM			)
	AM_RANGE(0xf0000, 0xfffff) AM_READ(MRA8_ROM			)
ADDRESS_MAP_END

static ADDRESS_MAP_START( galaxygn_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x07fff) AM_WRITE(MWA8_RAM					)
	AM_RANGE(0x52000, 0x521ff) AM_WRITE(paletteram_xRRRRRGGGGGBBBBB_le_w) AM_BASE(&paletteram	)
	AM_RANGE(0x53000, 0x53000) AM_WRITE(fantland_nmi_enable_w	)
	AM_RANGE(0x53002, 0x53002) AM_WRITE(fantland_soundlatch_w	)
	AM_RANGE(0x54000, 0x567ff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram	)
	AM_RANGE(0x60000, 0x6ffff) AM_WRITE(MWA8_RAM) AM_BASE(&spriteram_2	)
ADDRESS_MAP_END


/***************************************************************************
                           Born To Fight
***************************************************************************/

static WRITE8_HANDLER( borntofi_nmi_enable_w )
{
	fantland_nmi_enable = data;

	// data & 0x31 changes when lightgun fires

	if ((fantland_nmi_enable != 0) && (fantland_nmi_enable != 8))
		logerror("CPU #0 PC = %04X: nmi_enable = %02x\n", activecpu_get_pc(), data);

//  ui_popup("%02X",data);
}

// Trackball doesn't work correctly

static READ8_HANDLER( borntofi_inputs_r )
{
	int x,y,f;
	static int old_x[2], old_y[2], old_f[2];
	static UINT8 ret[2];

	switch (readinputport(7) & 0x03)
	{
		case 3:
		case 1:	return readinputport(0 + offset);	// Lightgun buttons
		case 2:	return readinputport(4 + offset);	// Joystick
	}

	// Trackball

	x = readinputport(13 + offset * 2);
	y = readinputport(12 + offset * 2);
	f = cpu_getcurrentframe();

	ret[offset]	=	(ret[offset] & 0x14) | (readinputport(2 + offset) & 0xc3);

	x =  (x & 0x7f) - (x & 0x80);
	y =  (y & 0x7f) - (y & 0x80);

	if		(old_x[offset] > 0)	{	ret[offset]	=	(ret[offset] ^ 0x04) | ((  ret[offset]  & 0x04) << 1);	old_x[offset]--;	}
	else if	(old_x[offset] < 0)	{	ret[offset]	=	(ret[offset] ^ 0x04) | (((~ret[offset]) & 0x04) << 1);	old_x[offset]++;	}

	if		(old_y[offset] > 0)	{	ret[offset]	=	(ret[offset] ^ 0x10) | ((  ret[offset]  & 0x10) << 1);	old_y[offset]--;	}
	else if	(old_y[offset] < 0)	{	ret[offset]	=	(ret[offset] ^ 0x10) | (((~ret[offset]) & 0x10) << 1);	old_y[offset]++;	}

//  if (offset == 0)    ui_popup("x %02d y %02d",old_x[offset], old_y[offset]);

	if ((f - old_f[offset]) > 0)
	{
		old_x[offset] = x;
		old_y[offset] = y;
		old_f[offset] = f;
	}

	return ret[offset];
}

static ADDRESS_MAP_START( borntofi_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x07fff) AM_READ( MRA8_RAM			)
	AM_RANGE(0x10000, 0x2ffff) AM_READ( MRA8_ROM			)
//  AM_RANGE(0x52000, 0x521ff) AM_READ( MRA8_RAM            )   // not actually read
	AM_RANGE(0x53000, 0x53001) AM_READ( borntofi_inputs_r	)
	AM_RANGE(0x53002, 0x53002) AM_READ( input_port_6_r		)
	AM_RANGE(0x53003, 0x53003) AM_READ( input_port_7_r		)
//  AM_RANGE(0x54000, 0x567ff) AM_READ( MRA8_RAM            )   // not actually read
	AM_RANGE(0x57000, 0x57000) AM_READ( input_port_8_r		)
	AM_RANGE(0x57001, 0x57001) AM_READ( input_port_9_r		)
	AM_RANGE(0x57002, 0x57002) AM_READ( input_port_10_r		)
	AM_RANGE(0x57003, 0x57003) AM_READ( input_port_11_r		)
//  AM_RANGE(0x60000, 0x6ffff) AM_READ( MRA8_RAM            )   // not actually read
	AM_RANGE(0x70000, 0x7ffff) AM_READ( MRA8_ROM			)
	AM_RANGE(0xf0000, 0xfffff) AM_READ( MRA8_ROM			)
ADDRESS_MAP_END

static ADDRESS_MAP_START( borntofi_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x07fff) AM_WRITE( MWA8_RAM						)
	AM_RANGE(0x52000, 0x521ff) AM_WRITE( paletteram_xRRRRRGGGGGBBBBB_le_w	) AM_BASE(&paletteram	)
	AM_RANGE(0x53000, 0x53000) AM_WRITE( borntofi_nmi_enable_w			)
	AM_RANGE(0x53002, 0x53002) AM_WRITE( fantland_soundlatch_w			)
	AM_RANGE(0x54000, 0x567ff) AM_WRITE( MWA8_RAM						) AM_BASE(&spriteram	)
	AM_RANGE(0x60000, 0x6ffff) AM_WRITE( MWA8_RAM						) AM_BASE(&spriteram_2	)
ADDRESS_MAP_END


/***************************************************************************

                            Memory Maps - Sound CPU

***************************************************************************/

static ADDRESS_MAP_START( fantland_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x01fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x80000, 0x9ffff) AM_READ(MRA8_ROM	)
	AM_RANGE(0xc0000, 0xfffff) AM_READ(MRA8_ROM	)
ADDRESS_MAP_END

static ADDRESS_MAP_START( fantland_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x01fff) AM_WRITE(MWA8_RAM	)
	AM_RANGE(0x80000, 0x9ffff) AM_WRITE(MWA8_ROM	)
	AM_RANGE(0xc0000, 0xfffff) AM_WRITE(MWA8_ROM	)
ADDRESS_MAP_END

static ADDRESS_MAP_START( fantland_sound_readport, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x0080, 0x0080) AM_READ(soundlatch_r				)
	AM_RANGE(0x0101, 0x0101) AM_READ(YM2151_status_port_0_r	)
ADDRESS_MAP_END

static ADDRESS_MAP_START( fantland_sound_writeport, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x0100, 0x0100) AM_WRITE(YM2151_register_port_0_w	)
	AM_RANGE(0x0101, 0x0101) AM_WRITE(YM2151_data_port_0_w		)
	AM_RANGE(0x0180, 0x0180) AM_WRITE(DAC_0_data_w				)
ADDRESS_MAP_END


/***************************************************************************
                           Born To Fight
***************************************************************************/

static struct
{

	int playing;
	int addr[2];
	int nibble;

}	borntofi_adpcm[4];

static void borntofi_adpcm_start(int voice)
{
	MSM5205_reset_w(voice,0);
	borntofi_adpcm[voice].playing = 1;
	borntofi_adpcm[voice].nibble  = 0;
//  logerror("CPU #0 PC = %04X: adpcm start = %06x, stop = %06x\n", activecpu_get_pc(), borntofi_adpcm[voice].addr[0], borntofi_adpcm[voice].addr[1]);
}

static void borntofi_adpcm_stop(int voice)
{
	MSM5205_reset_w(voice,1);
	borntofi_adpcm[voice].playing = 0;
}

static WRITE8_HANDLER( borntofi_msm5205_w )
{
	int voice = offset / 8;
	int reg   = offset % 8;

	if (reg == 0)
	{
		// Play / Stop
		switch(data)
		{
			case 0x00:		borntofi_adpcm_stop(voice);			break;
			case 0x03:		borntofi_adpcm_start(voice);		break;
			default:		logerror("CPU #0 PC = %04X: adpcm reg %d <- %02x\n", activecpu_get_pc(), reg, data);
		}
	}
	else
	{
		int shift = (reg - 1) * 4;
		int mask  = ~(0xf << shift);

		borntofi_adpcm[voice].addr[0] = (borntofi_adpcm[voice].addr[0] & mask) | (((data & 0xf0) >> 4) << shift);
		borntofi_adpcm[voice].addr[1] = (borntofi_adpcm[voice].addr[1] & mask) | (((data & 0x0f) >> 0) << shift);
	}
}

static void borntofi_adpcm_int(int voice)
{
	UINT8 *rom;
	size_t   len;
	int start, stop;

	if (!borntofi_adpcm[voice].playing)
		return;

	rom = memory_region( REGION_SOUND1 );
	len = memory_region_length( REGION_SOUND1 ) * 2;

	start = borntofi_adpcm[voice].addr[0] + borntofi_adpcm[voice].nibble;
	stop  = borntofi_adpcm[voice].addr[1];

	if (start >= len)
	{
		borntofi_adpcm_stop(voice);
		logerror("adpcm address out of range: %06x\n", start);
		return;
	}

	if (start >= stop)
	{
		borntofi_adpcm_stop(voice);
	}
	else
	{
        MSM5205_data_w( voice, rom[start/2] >> ((start & 1) * 4) );
		borntofi_adpcm[voice].nibble++;
	}
}

static ADDRESS_MAP_START( borntofi_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x003ff) AM_READ( MRA8_RAM		)
	AM_RANGE(0x04000, 0x04000) AM_READ( soundlatch_r	)
	AM_RANGE(0x08000, 0x0ffff) AM_READ( MRA8_ROM		)
	AM_RANGE(0xf8000, 0xfffff) AM_READ( MRA8_ROM		)
ADDRESS_MAP_END

static ADDRESS_MAP_START( borntofi_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x003ff) AM_WRITE( MWA8_RAM			)
	AM_RANGE(0x04000, 0x0401f) AM_WRITE( borntofi_msm5205_w	)
ADDRESS_MAP_END



/***************************************************************************

                                Input Ports

***************************************************************************/

/***************************************************************************
                                Fantasy Land
***************************************************************************/

INPUT_PORTS_START( fantland )
	PORT_START	/* IN0 - a3000 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1			)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1			)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP		)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN	)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT	)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1			)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2			)

	PORT_START	/* IN1 - a3001 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN2			)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2			)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP		) PORT_PLAYER(2)	// used in test mode only
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN	) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT	) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1			) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2			) PORT_PLAYER(2)

	PORT_START	/* IN2 - a3002 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, "Invulnerability" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x60, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START      /* IN3 - a3003 */
	PORT_DIPNAME( 0x01, 0x01, "Test Sound" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0e, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x0e, "1" )
	PORT_DIPSETTING(    0x0c, "2" )
	PORT_DIPSETTING(    0x0a, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x06, "5" )
	PORT_DIPSETTING(    0x04, "6" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x00, "8" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "800k" )
	PORT_DIPSETTING(    0x20, "1600k" )
	PORT_DIPSETTING(    0x10, "2400k" )
	PORT_DIPSETTING(    0x00, "3200k" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )	//unused?
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )	//unused?
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/***************************************************************************
                                Galaxy Gunners
***************************************************************************/

INPUT_PORTS_START( galaxygn )
	PORT_START	/* IN0 - 53000 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1			)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1			)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP		)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN	)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT	)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1			)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2			)

	PORT_START	/* IN1 - 53001 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN2			)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2			)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP		) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN	) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT	) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1			) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2			) PORT_PLAYER(2)

	PORT_START	/* IN2 - 53002 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, "Invulnerability" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) ) 	// Demo Sounds? doesn't work
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )	// Allow Continue? doesn't work
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x60, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START      /* IN3 - 53003 */
	PORT_DIPNAME( 0x01, 0x01, "Test Sound" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0e, 0x08, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x0e, "1" )
	PORT_DIPSETTING(    0x0c, "2" )
	PORT_DIPSETTING(    0x0a, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x06, "5" )
	PORT_DIPSETTING(    0x04, "6" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x00, "8" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "10k" )
	PORT_DIPSETTING(    0x20, "20k" )
	PORT_DIPSETTING(    0x10, "30k" )
	PORT_DIPSETTING(    0x00, "40k" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )	//unused?
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )	//unused?
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


/***************************************************************************
                           Born To Fight
***************************************************************************/

INPUT_PORTS_START( borntofi )
	PORT_START	/* IN0 - 53000 (Lightgun) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1	)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1	)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2	)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1	)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN	)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN	)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN	)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN	)

	PORT_START	/* IN1 - 53001 (Lightgun) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN2	)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2	)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2	) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1	) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN	)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN	)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN	)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN	)

	PORT_START	/* IN2 - 53000 (Trackball) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1	)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_START1	)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball x
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball x
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball y
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball y
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_BUTTON2	)
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_BUTTON1	)

	PORT_START	/* IN3 - 53001 (Trackball) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN2	)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_START2	)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball x
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball x
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball y
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SPECIAL	)	// trackball y
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_BUTTON2	) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_BUTTON1	) PORT_PLAYER(2)

	PORT_START	/* IN4 - 53000 (Joystick) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1			)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1			)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP		)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN	)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT	)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2			)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1			)

	PORT_START	/* IN5 - 53001 (Joystick) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN2			)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2			)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP		) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN	) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT	) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT	) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2			) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1			) PORT_PLAYER(2)

	PORT_START	/* IN6 - 53002 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, "Invulnerability" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x60, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START_TAG( "Controls" )	/* IN7 - 53003 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Controls ) )
//  PORT_DIPSETTING(    0x01, "Lightgun" )
	PORT_DIPSETTING(    0x03, "Lightgun" )
	PORT_DIPSETTING(    0x00, DEF_STR( Trackball ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Joystick ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )	// tested
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, "180" )
	PORT_DIPSETTING(    0x00, "0" )
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

	PORT_START_TAG("P1 Lightgun Y")	/* IN8 - 57000 */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_MINMAX(0x80,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START_TAG("P1 Lightgun X")	/* IN9 - 57001 */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START_TAG("P2 Lightgun Y")	/* IN10 - 57002 */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_MINMAX(0x80,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START_TAG("P2 Lightgun X")	/* IN11 - 57003 */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START	/* IN12 - 53000 */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(1) PORT_RESET

	PORT_START	/* IN13 - 53000 */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(1) PORT_RESET

	PORT_START	/* IN14 - 53001 */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START	/* IN15 - 53001 */
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(10) PORT_KEYDELTA(5) PORT_PLAYER(2)
INPUT_PORTS_END


/***************************************************************************

                            Graphics Layouts

***************************************************************************/

static const gfx_layout layout16x16x6 =
{
	16,16,
	RGN_FRAC(1,1),
	6,
	{ 0,1,2,3,4,5 },
	{ STEP4(3*6,-6),STEP4(7*6,-6),STEP4(11*6,-6),STEP4(15*6,-6) },
	{ STEP16(0,16*6) },
	16*16*6
};

static const gfx_decode fantland_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &layout16x16x6, 0, 4 }, // [0] Sprites
	{ -1 }
};

/***************************************************************************

                                Machine Drivers

***************************************************************************/

static MACHINE_RESET( fantland )
{
	fantland_nmi_enable = 0;
}

static INTERRUPT_GEN( fantland_irq )
{
	if (fantland_nmi_enable & 8)
		cpunum_set_input_line(0, INPUT_LINE_NMI, PULSE_LINE);
}

static INTERRUPT_GEN( fantland_sound_irq )
{
	cpunum_set_input_line_and_vector(1, 0, HOLD_LINE, 0x80/4);
}

static MACHINE_DRIVER_START( fantland )
	/* basic machine hardware */
	MDRV_CPU_ADD(I86, 8000000)        // ?
	MDRV_CPU_PROGRAM_MAP(fantland_readmem, fantland_writemem)
	MDRV_CPU_VBLANK_INT(fantland_irq,1)

	/* audio CPU */
	MDRV_CPU_ADD(I86, 8000000)        // ?
	MDRV_CPU_PROGRAM_MAP(fantland_sound_readmem, fantland_sound_writemem)
	MDRV_CPU_IO_MAP(fantland_sound_readport,fantland_sound_writeport)
	MDRV_CPU_PERIODIC_INT(fantland_sound_irq,TIME_IN_HZ(8000))
	// NMI when soundlatch is written

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(fantland)

	MDRV_INTERLEAVE(8000/60)	// sound irq must feed the DAC at 8kHz

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(352,256)
	MDRV_VISIBLE_AREA(0, 352-1, 0, 256-1)
	MDRV_GFXDECODE(fantland_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_UPDATE(fantland)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2151, 3000000)
	MDRV_SOUND_ROUTE(0, "mono", 0.35)
	MDRV_SOUND_ROUTE(1, "mono", 0.35)

	MDRV_SOUND_ADD(DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


static void galaxygn_sound_irq(int line)
{
	cpunum_set_input_line_and_vector(1, 0, line ? ASSERT_LINE : CLEAR_LINE, 0x80/4);
}

static struct YM2151interface galaxygn_ym2151_interface =
{
	galaxygn_sound_irq
};

static MACHINE_DRIVER_START( galaxygn )
	/* basic machine hardware */
	MDRV_CPU_ADD(I86, 8000000)        // ?
	MDRV_CPU_PROGRAM_MAP(galaxygn_readmem, galaxygn_writemem)
	MDRV_CPU_VBLANK_INT(fantland_irq,1)

	/* audio CPU */
	MDRV_CPU_ADD(I86, 8000000)        // ?
	MDRV_CPU_PROGRAM_MAP(fantland_sound_readmem, fantland_sound_writemem)
	MDRV_CPU_IO_MAP(fantland_sound_readport,fantland_sound_writeport)
	// IRQ by YM2151, NMI when soundlatch is written

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(fantland)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(352,256)
	MDRV_VISIBLE_AREA(0, 352-1, 0, 256-1)
	MDRV_GFXDECODE(fantland_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_UPDATE(fantland)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM2151, 3000000)
	MDRV_SOUND_CONFIG(galaxygn_ym2151_interface)
	MDRV_SOUND_ROUTE(0, "mono", 1.0)
	MDRV_SOUND_ROUTE(1, "mono", 1.0)
MACHINE_DRIVER_END


// OKI M5205 running at 384kHz [18.432/48]. Sample rate = 384000 / 48
static struct MSM5205interface msm5205_interface =
{
	borntofi_adpcm_int,	/* IRQ handler */
	MSM5205_S48_4B		/* 8 kHz, 4 Bits  */
};

static MACHINE_RESET( borntofi )
{
	int voice;

	machine_reset_fantland();

	for (voice = 0; voice < 4; voice++)
		borntofi_adpcm_stop(voice);
}

static MACHINE_DRIVER_START( borntofi )
	/* basic machine hardware */
	MDRV_CPU_ADD(V20, 16000000/2)        // D701080C-8 - NEC D70108C-8 V20 CPU, running at 8.000MHz [16/2]
	MDRV_CPU_PROGRAM_MAP(borntofi_readmem, borntofi_writemem)
	MDRV_CPU_VBLANK_INT(fantland_irq,1)

	/* audio CPU */
	MDRV_CPU_ADD(I88, 18432000/3)        // 8088 - AMD P8088-2 CPU, running at 6.144MHz [18.432/3]
	MDRV_CPU_PROGRAM_MAP(borntofi_sound_readmem, borntofi_sound_writemem)

	MDRV_FRAMES_PER_SECOND(54)	// 54 Hz
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_RESET(borntofi)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(352,256)
	MDRV_VISIBLE_AREA(0, 352-1, 0, 256-1)
	MDRV_GFXDECODE(fantland_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_UPDATE(fantland)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(MSM5205, 384000) MDRV_SOUND_CONFIG(msm5205_interface) MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
	MDRV_SOUND_ADD(MSM5205, 384000) MDRV_SOUND_CONFIG(msm5205_interface) MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
	MDRV_SOUND_ADD(MSM5205, 384000) MDRV_SOUND_CONFIG(msm5205_interface) MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
	MDRV_SOUND_ADD(MSM5205, 384000) MDRV_SOUND_CONFIG(msm5205_interface) MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

/***************************************************************************

                                ROMs Loading

***************************************************************************/

/***************************************************************************

Fantasy Land by Electronic Devices of Italy

Fantasyl.ev2  od2  d0  d1 s1  s2 :are 27c010 devices
Fantasyl.ev1 &  od1  are 27c512 devices

s1 & s2 are sound files.  (labeled 5 and 6 on devices)
d0 & d1 are next to the image banks  (labeled  7 and 8 on devices)
ev1 od1  od2  ev2  are believed to be program eproms
     (labeled 2, 1, 3, 4 on devices respectively)
     (also labeled under the eprom, location U3, U8, U7, U2 respectively)


3) - 23c4000 mask roms  "05, 06, 07"           !!!! [ 32 pin devices!!! ]
5) - 23c4100 mask roms  "00, 01, 02, 03, 04"   !!!! [ 40 pin devices!!! ]

Fantasyl.01, 00, 02, 03, 04  was read as if it was a 27c400

Fantasy.05, 06, 07 was read as if it was a 27c040

***************************************************************************/

ROM_START( fantland )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )					// Main CPU
	ROMX_LOAD( "fantasyl.ev2", 0x00000, 0x20000, CRC(f5bdca0e) SHA1(d05cf6f68d4d1a3dcc0171f7cf220c4920bd47bb) , ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.od2", 0x00001, 0x20000, CRC(9db35023) SHA1(81e2accd67dcf8563a68b2c4e35526f23a40150c) , ROM_SKIP(1) )
	ROM_COPY( REGION_CPU1,     0x00000, 0x40000, 0x40000 )
	ROMX_LOAD( "fantasyl.ev1", 0xe0000, 0x10000, CRC(70e0ee30) SHA1(5253213da56b3f97e2811f2b10927d0e542447f0) , ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.od1", 0xe0001, 0x10000, CRC(577b4bd7) SHA1(1f08202d99c3e39e0dd1ed4947b928b695a5b411) , ROM_SKIP(1) )

	ROM_REGION( 0x100000, REGION_CPU2, 0 )					// Sound CPU
	ROM_LOAD16_WORD( "fantasyl.s2", 0x80000, 0x20000, CRC(f23837d8) SHA1(4048784f759781e50ae445ea61f1ca908e8e6ac1) )	// samples (8 bit unsigned)
	ROM_LOAD16_WORD( "fantasyl.s1", 0xc0000, 0x20000, CRC(1a324a69) SHA1(06f6877af6cd19bfaac8a4ea8057ef8faee276f5) )
	ROM_COPY( REGION_CPU2,          0xc0000, 0xe0000, 0x20000 )

	ROM_REGION( 0x480000, REGION_GFX1, ROMREGION_DISPOSE )	// Sprites
	ROMX_LOAD( "fantasyl.m00", 0x000000, 0x80000, CRC(82d819ff) SHA1(2b5b0759de8260eaa84ddded9dc2d12a6e0f5ec9) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.m01", 0x0c0000, 0x80000, CRC(70a94139) SHA1(689fbfa267d60821cde13d5dc2dfe1dea67b434a) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.05",  0x000002, 0x80000, CRC(62b9a00b) SHA1(ecd18e5e7a5e3535956fb693d2f7e35d2bb7ede9) , ROM_SKIP(2) )

	ROMX_LOAD( "fantasyl.m02", 0x180000, 0x80000, CRC(ae52bf37) SHA1(60daa24d1f456cfeb643fa2107119d2939af0ffa) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.m03", 0x240000, 0x80000, CRC(f3f534a1) SHA1(9d47cc5b5a40146ed1d9e57a16d67a1d92f3b5be) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.06",  0x180002, 0x80000, CRC(867fa549) SHA1(9777b4837e5bb25a39639597e88b713d43361a80) , ROM_SKIP(2) )

	ROMX_LOAD( "fantasyl.m04", 0x300000, 0x80000, CRC(e7b1918c) SHA1(97230b21bb54c4c928dced83e0b3396068ab72db) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.d0",  0x3c0001, 0x20000, CRC(0f907f19) SHA1(eea90e7d7e2e29db809e867d9b1205f4fbb7ada8) , ROM_SKIP(2) )
	ROMX_LOAD( "fantasyl.d1",  0x3c0000, 0x20000, CRC(10d10389) SHA1(3a5639050c769eedc62924dfde57c1bf020970c8) , ROM_SKIP(2) )
	ROMX_LOAD( "fantasyl.07",  0x300002, 0x80000, CRC(162ad422) SHA1(0d3609e630481018d1326a908d1d4c204dfcdf13) , ROM_SKIP(2) )
ROM_END

/* this dump had several roms half size however they all appear to be data & gfx roms, the main program looks ok */
ROM_START( fantlnda )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )					// Main CPU
	ROMX_LOAD( "fantasyl.ev2", 0x00000, 0x20000, CRC(f5bdca0e) SHA1(d05cf6f68d4d1a3dcc0171f7cf220c4920bd47bb) , ROM_SKIP(1) ) // 04.bin (was first half only)
	ROMX_LOAD( "fantasyl.od2", 0x00001, 0x20000, CRC(9db35023) SHA1(81e2accd67dcf8563a68b2c4e35526f23a40150c) , ROM_SKIP(1) ) // 03.bin (was first half only)
	ROM_COPY( REGION_CPU1,     0x00000, 0x40000, 0x40000 )
	ROMX_LOAD( "02.bin",       0xe0000, 0x10000, CRC(8b835eed) SHA1(6a6b3fe116145f685b91dcd5301165f17973697c) , ROM_SKIP(1) )
	ROMX_LOAD( "01.bin",       0xe0001, 0x10000, CRC(4fa3eb8b) SHA1(56da42a4e2972a696ef28811116cbc20bb5ba3e8) , ROM_SKIP(1) )

	ROM_REGION( 0x100000, REGION_CPU2, 0 )					// Sound CPU
	ROM_LOAD16_WORD( "fantasyl.s2", 0x80000, 0x20000, CRC(f23837d8) SHA1(4048784f759781e50ae445ea61f1ca908e8e6ac1) ) // 05.bin (was first half only)
	ROM_LOAD16_WORD( "fantasyl.s1", 0xc0000, 0x20000, CRC(1a324a69) SHA1(06f6877af6cd19bfaac8a4ea8057ef8faee276f5) ) // 06.bin (was first half only)
	ROM_COPY( REGION_CPU2,          0xc0000, 0xe0000, 0x20000 )

	ROM_REGION( 0x480000, REGION_GFX1, ROMREGION_DISPOSE )	// Sprites
	ROMX_LOAD( "fantasyl.m00", 0x000000, 0x80000, CRC(82d819ff) SHA1(2b5b0759de8260eaa84ddded9dc2d12a6e0f5ec9) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.m01", 0x0c0000, 0x80000, CRC(70a94139) SHA1(689fbfa267d60821cde13d5dc2dfe1dea67b434a) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.05",  0x000002, 0x80000, CRC(62b9a00b) SHA1(ecd18e5e7a5e3535956fb693d2f7e35d2bb7ede9) , ROM_SKIP(2) )

	ROMX_LOAD( "fantasyl.m02", 0x180000, 0x80000, CRC(ae52bf37) SHA1(60daa24d1f456cfeb643fa2107119d2939af0ffa) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.m03", 0x240000, 0x80000, CRC(f3f534a1) SHA1(9d47cc5b5a40146ed1d9e57a16d67a1d92f3b5be) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.06",  0x180002, 0x80000, CRC(867fa549) SHA1(9777b4837e5bb25a39639597e88b713d43361a80) , ROM_SKIP(2) )

	ROMX_LOAD( "fantasyl.m04", 0x300000, 0x80000, CRC(e7b1918c) SHA1(97230b21bb54c4c928dced83e0b3396068ab72db) , ROM_GROUPWORD | ROM_SKIP(1) )
	ROMX_LOAD( "fantasyl.d0",  0x3c0001, 0x20000, CRC(0f907f19) SHA1(eea90e7d7e2e29db809e867d9b1205f4fbb7ada8) , ROM_SKIP(2) ) // 07.bin (was first half only)
	ROMX_LOAD( "fantasyl.d1",  0x3c0000, 0x20000, CRC(10d10389) SHA1(3a5639050c769eedc62924dfde57c1bf020970c8) , ROM_SKIP(2) ) // 08.bin (was first half only)
	ROMX_LOAD( "fantasyl.07",  0x300002, 0x80000, CRC(162ad422) SHA1(0d3609e630481018d1326a908d1d4c204dfcdf13) , ROM_SKIP(2) )
ROM_END

/***************************************************************************
                                Galaxy Gunners
***************************************************************************/

ROM_START( galaxygn )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )					// Main CPU
	ROM_LOAD( "gg03.bin", 0x10000, 0x10000, CRC(9e469189) SHA1(07e5d36ca9665bdd13e3bb4241d34b9042371b79) )
	ROM_LOAD( "gg02.bin", 0x20000, 0x10000, CRC(b87a438f) SHA1(96c39cc4d51a2fc0779f148971220117967173c0) )
	ROM_LOAD( "gg01.bin", 0xf0000, 0x10000, CRC(ad0e5b29) SHA1(f9a7ebce9f47a009af213e4e10811bb1c26f891a) )
	ROM_COPY( REGION_CPU1,0xf0000, 0x70000, 0x10000 )

	ROM_REGION( 0x100000, REGION_CPU2, 0 )					// Sound CPU
	ROM_LOAD( "gg20.bin", 0xc0000, 0x10000, CRC(f5c65a85) SHA1(a094fa9531ea4e68ec0a448568e7d4b2307c8185) )
	ROM_COPY( REGION_CPU2, 0xc0000, 0xd0000, 0x10000 )
	ROM_COPY( REGION_CPU2, 0xc0000, 0xe0000, 0x10000 )
	ROM_COPY( REGION_CPU2, 0xc0000, 0xf0000, 0x10000 )

	ROM_REGION( 0x1b0000, REGION_GFX1, ROMREGION_DISPOSE )	// Sprites
	ROMX_LOAD( "gg54.bin", 0x000000, 0x10000, CRC(b3621119) SHA1(66ade772077e57f872ef1c8f45e244f4006023f0) , ROM_SKIP(2) )
	ROMX_LOAD( "gg38.bin", 0x000001, 0x10000, CRC(52b70f3e) SHA1(65f11d5700337d6d9b6325ff70c86d076e1bdc26) , ROM_SKIP(2) )
	ROMX_LOAD( "gg22.bin", 0x000002, 0x10000, CRC(ea49fee4) SHA1(29ae3e5dfade421a5e97efe5be1cb17862fdcea1) , ROM_SKIP(2) )
	ROMX_LOAD( "gg55.bin", 0x030000, 0x10000, CRC(bffe278f) SHA1(b8077794c4af5aa13ea6f231988e698c1bf229bd) , ROM_SKIP(2) )
	ROMX_LOAD( "gg39.bin", 0x030001, 0x10000, CRC(3f7df1e6) SHA1(c1ef3a2f7aaf2e7e850c884dc5d3c1f1545a2526) , ROM_SKIP(2) )
	ROMX_LOAD( "gg23.bin", 0x030002, 0x10000, CRC(4dcbbc99) SHA1(57ba915043a2c3867bb28875a0d0bc3f81ae69d5) , ROM_SKIP(2) )
	ROMX_LOAD( "gg56.bin", 0x060000, 0x10000, CRC(0306069e) SHA1(e10b142a23a93caac0b505d59b6d5d6a4e195d4b) , ROM_SKIP(2) )
	ROMX_LOAD( "gg40.bin", 0x060001, 0x10000, CRC(f635aa7e) SHA1(3e5ce1c3b25cb2c0387ae5dfe650040ccc353d8a) , ROM_SKIP(2) )
	ROMX_LOAD( "gg24.bin", 0x060002, 0x10000, CRC(733f5dcf) SHA1(e516537dcb3f18da2af307ffded3ee7914e28e20) , ROM_SKIP(2) )
	ROMX_LOAD( "gg57.bin", 0x090000, 0x10000, CRC(c3919bef) SHA1(1eebe888135c51c053d689fda3031769f1efa70a) , ROM_SKIP(2) )
	ROMX_LOAD( "gg41.bin", 0x090001, 0x10000, CRC(1f2757de) SHA1(c853f5ce08141e0197988a9d9e5c0032a29a5824) , ROM_SKIP(2) )
	ROMX_LOAD( "gg25.bin", 0x090002, 0x10000, CRC(1d094f95) SHA1(bcb654c7edd8eec9a655261ebc7c812144d4ff6d) , ROM_SKIP(2) )
	ROMX_LOAD( "gg58.bin", 0x0c0000, 0x10000, CRC(4a459cb8) SHA1(e17de49c56d91942a274206d005dd7bf0f074a7f) , ROM_SKIP(2) )
	ROMX_LOAD( "gg42.bin", 0x0c0001, 0x10000, CRC(ae7a8e1e) SHA1(8714d2b58a26138a9644f9117fcdd89256135a98) , ROM_SKIP(2) )
	ROMX_LOAD( "gg26.bin", 0x0c0002, 0x10000, CRC(c2f310b4) SHA1(510e3bc773456b69609c4e29583753f21dac6165) , ROM_SKIP(2) )
	ROMX_LOAD( "gg59.bin", 0x0f0000, 0x10000, CRC(c8d4fbc2) SHA1(f8e9e7d31fa4c7920a3db36295999ef0ee86dbe9) , ROM_SKIP(2) )
	ROMX_LOAD( "gg43.bin", 0x0f0001, 0x10000, CRC(74d3a0df) SHA1(c8d611c969898f135a254ea53b465305715625c6) , ROM_SKIP(2) )
	ROMX_LOAD( "gg27.bin", 0x0f0002, 0x10000, CRC(c2cfd3f9) SHA1(8c2ad28aa64a124d3c97fde804bf167378ba4c20) , ROM_SKIP(2) )
	ROMX_LOAD( "gg60.bin", 0x120000, 0x10000, CRC(6e32b549) SHA1(541860ad2f2b197fdf3877d8aeded0609ccb7fb0) , ROM_SKIP(2) )
	ROMX_LOAD( "gg44.bin", 0x120001, 0x10000, CRC(fcda6efa) SHA1(b4eb575dee8f78c4f0d3ae26204315924d4ce9bd) , ROM_SKIP(2) )
	ROMX_LOAD( "gg28.bin", 0x120002, 0x10000, CRC(4d4fc01c) SHA1(1ab5186ac440dc004140d5a8bf19df521b60e62d) , ROM_SKIP(2) )
	ROMX_LOAD( "gg61.bin", 0x150000, 0x10000, CRC(177a767a) SHA1(09e2883eaefeb20782bdd5aad069fe35b06b8329) , ROM_SKIP(2) )
	ROMX_LOAD( "gg45.bin", 0x150001, 0x10000, CRC(2ba49d47) SHA1(380943bde589dd2ea079a54fa7bcf327da2990a7) , ROM_SKIP(2) )
	ROMX_LOAD( "gg29.bin", 0x150002, 0x10000, CRC(c1c68148) SHA1(171d25aa20accf6638e1b0886a15db9fac2d8b85) , ROM_SKIP(2) )
	ROMX_LOAD( "gg62.bin", 0x180000, 0x10000, CRC(0fb2d41a) SHA1(41b179e4e9ae142b3057e7cdad6eee8efcd952c4) , ROM_SKIP(2) )
	ROMX_LOAD( "gg46.bin", 0x180001, 0x10000, CRC(5f1bf8ad) SHA1(b831ea433ff008377b522a3be4666d6d1b86cbb4) , ROM_SKIP(2) )
	ROMX_LOAD( "gg30.bin", 0x180002, 0x10000, CRC(ded7cacf) SHA1(adbfaa8f46e5ce8df264d5b5a201d75ca2b3dbeb) , ROM_SKIP(2) )
ROM_END

/*
Born To Fight
?, ?

PCB Layout
----------

Bottom Board

|-------------------------------------------------------------------|
|     PAL                                                           |
|-|   PAL    PAL                     4464    4464                   |
| |          PAL          PAL                                       |
| |          PAL   PAL               4464    4464                   |
| |                                                                 |
| |CN1                    PAL        4464    4464    2018           |
| |                PAL    PAL                                       |
| |                                  4464    4464    2018         |-|
|-|                      |---------|                  |---------| | |
|                        |I.G.1BB  |                  |I.G.1BB  | | |
|J                       |48844758V|                  |48844758V| | |
|A                       |         |                  |         | | |
|M                       |         |                  |         | | |CN2
|M                       |---------|                  |---------| | |
|A             16MHz                                              |-|
|                                PAL        PAL      PAL            |
|    |----------|                PAL                                |
|    |D70108C-8 |                                                   |
|    |----------|                                                   |
|                                                                   |
|          1.BIN                            62256                   |
|          2.BIN  PAL                       62256     2063          |
|DSW1      3.BIN  2018                                              |
|DSW2      62256  PAL                       PAL       PAL       PAL |
|-------------------------------------------------------------------|
Notes:
      D701080C-8 - NEC D70108C-8 V20 CPU, running at 8.000MHz [16/2]
      I.G.1BB    - PLCC84 FPGA, PCB labelled 'INGA 1' & 'INGA 2'
      2018       - 2K x8 SRAM (DIP24)
      2063       - 8K x8 SRAM (DIP28)
      62256      - 32K x8 SRAM (DIP28)
      4464       - 64K x4 DRAM (DIP18)
      CN1/2      - Flat cable connectors joining bottom board to top board
      VSync      - 54Hz

Top Board

|--------------------------------------------------------|
|  VOL                                                   |
|-|                                                      |
| |                                                      |
| |   8088              6.BIN   22.BIN  38.BIN   54.BIN  |
| |                     7.BIN   23.BIN  39.BIN   55.BIN  |
| |   5.BIN             8.BIN   24.BIN  40.BIN   56.BIN  |
| |   6116              9.BIN   25.BIN  41.BIN   57.BIN  |
| |                     10.BIN  26.BIN  42.BIN   58.BIN |-|
| |               PAL   11.BIN  27.BIN  43.BIN   59.BIN | |
|-|                     12.BIN  28.BIN  44.BIN   60.BIN | |
|     6116              13.BIN  29.BIN  45.BIN   61.BIN | |
|                       14.BIN  30.BIN  46.BIN   62.BIN | |
|                       15.BIN  31.BIN  47.BIN   63.BIN | |
|          PAL    M5205 16.BIN    *       *        *    | |
|PAL                    17.BIN    *       *        *    | |
|          PAL    M5205   *       *       *        *    |-|
|                         *       *       *        *     |
|          M5205  PAL     *       *       *        *     |
|18.432MHz                *       *       *        *     |
|          M5205  PAL                                    |
|                                                        |
|          PAL                                           |
|--------------------------------------------------------|
Notes:
      8088 - AMD P8088-2 CPU, running at 6.144MHz [18.432/3]
      M5205- OKI M5205 running at 384kHz [18.432/48]. Sample rate = 384000 / 48
      *    - Unpopulated positions for ROMs

*/

ROM_START( borntofi )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* V20 */
	ROM_LOAD( "3.bin", 0x10000, 0x10000, CRC(5f07f4a2) SHA1(240864d1d5d9e26d229bc21aa39ee03f4bd25814) )
	ROM_LOAD( "2.bin", 0x20000, 0x10000, CRC(5d2b3395) SHA1(ac87f352f380b67802c26232824663063549ac7b) )
	ROM_LOAD( "1.bin", 0xf0000, 0x10000, CRC(0a5e2f32) SHA1(5167a85329e5ea35c686af85e44d62227cf5800e) )
	ROM_COPY( REGION_CPU1, 0xf0000, 0x70000, 0x10000 )

	ROM_REGION( 0x100000, REGION_CPU2, 0 ) /* 8088 */
	ROM_LOAD( "5.bin", 0xf8000, 0x08000, CRC(b5d587ce) SHA1(07687abd264ec80a6eb473cb3f3ab97ec6b365a2) )
	ROM_COPY( REGION_CPU2, 0xf8000, 0x08000, 0x08000 )

	ROM_REGION( 0xc0000, REGION_SOUND1, 0 ) /* m5205 samples */
	ROM_LOAD( "6.bin",  0x00000, 0x10000, CRC(731c523b) SHA1(b649a838ce70d5af607f8b9faf8b012e2ff1104b) )
	ROM_LOAD( "7.bin",  0x10000, 0x10000, CRC(a0cbdf10) SHA1(d33c157aceb52683db36d2f666f1e2d952654633) )
	ROM_LOAD( "8.bin",  0x20000, 0x10000, CRC(82fa8592) SHA1(81510160d645a3496131255c11c94fe47bb75988) )
	ROM_LOAD( "9.bin",  0x30000, 0x10000, CRC(dca5d205) SHA1(01c212fc0dbf9cd1ad0c633288925add853640a1) )
	ROM_LOAD( "10.bin", 0x40000, 0x10000, CRC(0ecd5714) SHA1(4dbc840f9b2707e28cb0e03220a51ad8d9268fe7) )
	ROM_LOAD( "11.bin", 0x50000, 0x10000, CRC(4fd86d07) SHA1(025ecaf7c51882286e2d25ab1636ad79dfe3250d) )
	ROM_LOAD( "12.bin", 0x60000, 0x10000, CRC(cb81236e) SHA1(96423a41e851900ad764e7ec9fbe9a9e34d188ef) )
	ROM_LOAD( "13.bin", 0x70000, 0x10000, CRC(cfcc4bdb) SHA1(b7e1bf189ac964958254e6e8cb7c466b6e7b473f) )
	ROM_LOAD( "14.bin", 0x80000, 0x10000, CRC(c6fa0707) SHA1(831b27f24fee641c30ffd39723a24e7a9a966dc6) )
	ROM_LOAD( "15.bin", 0x90000, 0x10000, CRC(101cbd6b) SHA1(22668f362499f7ae017d9334f426ac644498e0b7) )
	ROM_LOAD( "16.bin", 0xa0000, 0x10000, CRC(edab01a9) SHA1(ce9c6b48807c9b312067b27928d27b4532319c60) )
	ROM_LOAD( "17.bin", 0xb0000, 0x10000, CRC(ea361ea5) SHA1(a2b38a250ab477226da5d56bd07ea3b2f3aa9625) )

	ROM_REGION( 0x1e0000, REGION_GFX1,0 ) /* gfx */
	ROMX_LOAD( "22.bin",  0x000002, 0x10000, CRC(a3afc57f) SHA1(2713fa4b6ad571748f47d25c72a0d40d80f8fcf6), ROM_SKIP(2) )
	ROMX_LOAD( "38.bin",  0x000001, 0x10000, CRC(1c64d329) SHA1(6bb82143de07548b90bc7ba70d12fd0959e56545), ROM_SKIP(2) )
	ROMX_LOAD( "54.bin",  0x000000, 0x10000, CRC(56209405) SHA1(e1f5fd709cde965c400f94837a34d5b414e0118e), ROM_SKIP(2) )
	ROMX_LOAD( "23.bin",  0x030002, 0x10000, CRC(df385140) SHA1(202ef05ffad58ae9be2b264208188722154ec022), ROM_SKIP(2) )
	ROMX_LOAD( "39.bin",  0x030001, 0x10000, CRC(7f789bdb) SHA1(c4220a60d8c11d77037c1f4bfc12254ee024e375), ROM_SKIP(2) )
	ROMX_LOAD( "55.bin",  0x030000, 0x10000, CRC(93f82275) SHA1(132372c2301b57ed2ad47d7e7684f3caa455dc3f), ROM_SKIP(2) )
	ROMX_LOAD( "24.bin",  0x060002, 0x10000, CRC(becc5891) SHA1(0aeb6f2d2d39dd237ef5929674de62bbe0f6e2e6), ROM_SKIP(2) )
	ROMX_LOAD( "40.bin",  0x060001, 0x10000, CRC(707a6ddd) SHA1(30ae010bb2bd41b847790d1612a0702bc0c8b646), ROM_SKIP(2) )
	ROMX_LOAD( "56.bin",  0x060000, 0x10000, CRC(a4e8c94a) SHA1(2514097d449a7883c6442a8d42376bfcc1a87e93), ROM_SKIP(2) )
	ROMX_LOAD( "25.bin",  0x090002, 0x10000, CRC(500d4d3b) SHA1(54a448e5e784633513b94fdd4ad79e66b5210108), ROM_SKIP(2) )
	ROMX_LOAD( "41.bin",  0x090001, 0x10000, CRC(e3bf2b57) SHA1(04619e9de339f4fb5269336ebd2a6749e402243e), ROM_SKIP(2) )
	ROMX_LOAD( "57.bin",  0x090000, 0x10000, CRC(b8d57360) SHA1(5133a5db3d8ee05662662d19b31f0045e833ed42), ROM_SKIP(2) )
	ROMX_LOAD( "26.bin",  0x0c0002, 0x10000, CRC(13801b0e) SHA1(7f830af2d9bf5201c321637ea2400c6b42948ebd), ROM_SKIP(2) )
	ROMX_LOAD( "42.bin",  0x0c0001, 0x10000, CRC(4725835e) SHA1(d60108aa5e7cd7434dd07a5d0553be56c591f4b1), ROM_SKIP(2) )
	ROMX_LOAD( "58.bin",  0x0c0000, 0x10000, CRC(391f220a) SHA1(66de5034b31f99e99005e224ee2e60cf870f1bcb), ROM_SKIP(2) )
	ROMX_LOAD( "27.bin",  0x0f0002, 0x10000, CRC(d8a47ed9) SHA1(4f5f68c35d198f1fdbbcf288bf25c548876ba823), ROM_SKIP(2) )
	ROMX_LOAD( "43.bin",  0x0f0001, 0x10000, CRC(815be7ea) SHA1(478cabf2a683be14e75cbe1b1d63aae521469315), ROM_SKIP(2) )
	ROMX_LOAD( "59.bin",  0x0f0000, 0x10000, CRC(7966e68f) SHA1(13608b781ae10e96eb2802479e082b87f55c3ec1), ROM_SKIP(2) )
	ROMX_LOAD( "28.bin",  0x120002, 0x10000, CRC(e8846d67) SHA1(f3b155836a2f96a3b0b0dfb2034212def0b1dc92), ROM_SKIP(2) )
	ROMX_LOAD( "44.bin",  0x120001, 0x10000, CRC(c5d29821) SHA1(ce45e2e039f3ee7965cc9354a98c77f3db83b950), ROM_SKIP(2) )
	ROMX_LOAD( "60.bin",  0x120000, 0x10000, CRC(47d2a385) SHA1(0b275abe7811972ac475c2dff2e99f2a1f951ee5), ROM_SKIP(2) )
	ROMX_LOAD( "29.bin",  0x150002, 0x10000, CRC(169cba64) SHA1(d881c1cd802da5929688fdaae8c63e581e396dd7), ROM_SKIP(2) )
	ROMX_LOAD( "45.bin",  0x150001, 0x10000, CRC(1a58b8d0) SHA1(c1002e16581face369a11e4308596ba419d82c4d), ROM_SKIP(2) )
	ROMX_LOAD( "61.bin",  0x150000, 0x10000, CRC(f20b88b3) SHA1(f6b4f0a6cf0cdc6bab9f0a580a866477a0925a28), ROM_SKIP(2) )
	ROMX_LOAD( "30.bin",  0x180002, 0x10000, CRC(15529cdd) SHA1(7b80d820476fefb51434870bd9fc4955de1a7323), ROM_SKIP(2) )
	ROMX_LOAD( "46.bin",  0x180001, 0x10000, CRC(2f145546) SHA1(10677effd27ee8702be488fe1485582da465aaf8), ROM_SKIP(2) )
	ROMX_LOAD( "62.bin",  0x180000, 0x10000, CRC(5bbc0154) SHA1(0c7b144cd2be1f8bcd5316093d6f9f0071746593), ROM_SKIP(2) )
	ROMX_LOAD( "31.bin",  0x1b0002, 0x10000, CRC(1446ddc5) SHA1(7bd2ae336514d939c71bdf52a72e710e28d14bd9), ROM_SKIP(2) )
	ROMX_LOAD( "47.bin",  0x1b0001, 0x10000, CRC(b3147a84) SHA1(dfb9c8293a477697017f5632e203c9fb88cadc81), ROM_SKIP(2) )
	ROMX_LOAD( "63.bin",  0x1b0000, 0x10000, CRC(5f530559) SHA1(d1d3edc2082985ccec9fe8ca0b27810623cb5b89), ROM_SKIP(2) )
ROM_END

GAME( 19??, borntofi, 0,        borntofi, borntofi, 0, ROT0,  "International Games",      "Born To Fight"       , 0 ,0)
GAME( 19??, fantland, 0,        fantland, fantland, 0, ROT0,  "Electronic Devices Italy", "Fantasy Land (set 1)", 0 ,0)
GAME( 19??, fantlnda, fantland, fantland, fantland, 0, ROT0,  "Electronic Devices Italy", "Fantasy Land (set 2)", 0 ,0)
GAME( 1989, galaxygn, 0,        galaxygn, galaxygn, 0, ROT90, "Electronic Devices Italy", "Galaxy Gunners"      , 0 ,0)
