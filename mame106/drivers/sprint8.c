/***************************************************************************

Atari Sprint 8 driver

***************************************************************************/

#include "driver.h"

extern VIDEO_EOF( sprint8 );
extern VIDEO_START( sprint8 );
extern VIDEO_UPDATE( sprint8 );

extern WRITE8_HANDLER( sprint8_video_ram_w );

extern UINT8* sprint8_video_ram;
extern UINT8* sprint8_pos_h_ram;
extern UINT8* sprint8_pos_v_ram;
extern UINT8* sprint8_pos_d_ram;

static int steer_dir[8];
static int steer_flag[8];

static int collision_reset;
static int collision_index;


void sprint8_collision_callback(int n)
{
	if (collision_reset == 0)
	{
		cpunum_set_input_line(0, 0, ASSERT_LINE);

		collision_index = n;
	}
}


static void input_callback(int dummy)
{
	static UINT8 dial[8];

	int i;

	for (i = 0; i < 8; i++)
	{
		UINT8 val = readinputport(8 + i) >> 4;

		signed char delta = (val - dial[i]) & 15;

		if (delta & 8)
		{
			delta |= 0xf0; /* extend sign to 8 bits */
		}

		steer_flag[i] = (delta != 0);

		if (delta > 0)
		{
			steer_dir[i] = 0;
		}
		if (delta < 0)
		{
			steer_dir[i] = 1;
		}

		dial[i] = val;
	}
}


static void fill_palette(int team)
{
	int i;

	for (i = 0; i < 16; i += 8)
	{
		if (team)
		{
			palette_set_color(i + 0, 0xff, 0x00, 0x00); /* red     */
			palette_set_color(i + 1, 0x00, 0x00, 0xff); /* blue    */
			palette_set_color(i + 2, 0xff, 0x00, 0x00); /* red     */
			palette_set_color(i + 3, 0x00, 0x00, 0xff); /* blue    */
			palette_set_color(i + 4, 0xff, 0x00, 0x00); /* red     */
			palette_set_color(i + 5, 0x00, 0x00, 0xff); /* blue    */
			palette_set_color(i + 6, 0xff, 0x00, 0x00); /* red     */
			palette_set_color(i + 7, 0x00, 0x00, 0xff); /* blue    */
		}
		else
		{
			palette_set_color(i + 0, 0xff, 0x00, 0x00); /* red     */
			palette_set_color(i + 1, 0x00, 0x00, 0xff); /* blue    */
			palette_set_color(i + 2, 0xff, 0xff, 0x00); /* yellow  */
			palette_set_color(i + 3, 0x00, 0xff, 0x00); /* green   */
			palette_set_color(i + 4, 0xff, 0x00, 0xff); /* magenta */
			palette_set_color(i + 5, 0xe0, 0xc0, 0x70); /* puce    */
			palette_set_color(i + 6, 0x00, 0xff, 0xff); /* cyan    */
			palette_set_color(i + 7, 0xff, 0xaa, 0xaa); /* pink    */
		}
	}

	palette_set_color(16, 0x00, 0x00, 0x00);
	palette_set_color(17, 0xff, 0xff, 0xff);
}


static PALETTE_INIT( sprint8 )
{
	int i;

	fill_palette(0);

	for (i = 0; i < 16; i++)
	{
		colortable[2 * i + 0] = 16;
		colortable[2 * i + 1] = i;
	}

	colortable[32] = 16;
	colortable[33] = 16;
	colortable[34] = 16;
	colortable[35] = 17;
}


static MACHINE_RESET( sprint8 )
{
	collision_reset = 0;
	collision_index = 0;

	timer_pulse(TIME_IN_HZ(60), 0, input_callback);
}


static READ8_HANDLER( sprint8_collision_r )
{
	return collision_index;
}


static READ8_HANDLER( sprint8_input_r )
{
	UINT8 val = readinputport(offset);

	if (steer_dir[offset])
	{
		val |= 0x02;
	}
	if (steer_flag[offset])
	{
		val |= 0x04;
	}

	return val;
}


static WRITE8_HANDLER( sprint8_lockout_w )
{
	coin_lockout_w(offset, !(data & 1));
}


static WRITE8_HANDLER( sprint8_team_w )
{
	fill_palette(!(data & 1));
}


static WRITE8_HANDLER( sprint8_int_reset_w )
{
	collision_reset = !(data & 1);

	if (collision_reset)
	{
		cpunum_set_input_line(0, 0, CLEAR_LINE);
	}
}


/* names of sound effects taken from Tank 8, might differ for Sprint 8 */

static WRITE8_HANDLER( sprint8_crash_w ) {}
static WRITE8_HANDLER( sprint8_explosion_w ) {}
static WRITE8_HANDLER( sprint8_bugle_w ) {}
static WRITE8_HANDLER( sprint8_bug_w ) {}
static WRITE8_HANDLER( sprint8_attract_w ) {}
static WRITE8_HANDLER( sprint8_motor_w ) {}


static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x00ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x1c00, 0x1c00) AM_READ(sprint8_collision_r)
	AM_RANGE(0x1c01, 0x1c08) AM_READ(sprint8_input_r)
	AM_RANGE(0x1c09, 0x1c09) AM_READ(input_port_16_r)
	AM_RANGE(0x1c0a, 0x1c0a) AM_READ(input_port_17_r)
	AM_RANGE(0x1c0f, 0x1c0f) AM_READ(input_port_18_r)
	AM_RANGE(0x2000, 0x3fff) AM_READ(MRA8_ROM)
	AM_RANGE(0xf800, 0xffff) AM_READ(MRA8_ROM)
ADDRESS_MAP_END


static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x00ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x1800, 0x1bff) AM_WRITE(sprint8_video_ram_w) AM_BASE(&sprint8_video_ram)
	AM_RANGE(0x1c00, 0x1c0f) AM_WRITE(MWA8_RAM) AM_BASE(&sprint8_pos_h_ram)
	AM_RANGE(0x1c10, 0x1c1f) AM_WRITE(MWA8_RAM) AM_BASE(&sprint8_pos_v_ram)
	AM_RANGE(0x1c20, 0x1c2f) AM_WRITE(MWA8_RAM) AM_BASE(&sprint8_pos_d_ram)
	AM_RANGE(0x1c30, 0x1c37) AM_WRITE(sprint8_lockout_w)
	AM_RANGE(0x1d00, 0x1d00) AM_WRITE(sprint8_int_reset_w)
	AM_RANGE(0x1d01, 0x1d01) AM_WRITE(sprint8_crash_w)
	AM_RANGE(0x1d02, 0x1d02) AM_WRITE(sprint8_explosion_w)
	AM_RANGE(0x1d03, 0x1d03) AM_WRITE(sprint8_bugle_w)
	AM_RANGE(0x1d04, 0x1d04) AM_WRITE(sprint8_bug_w)
	AM_RANGE(0x1d05, 0x1d05) AM_WRITE(sprint8_team_w)
	AM_RANGE(0x1d06, 0x1d06) AM_WRITE(sprint8_attract_w)
	AM_RANGE(0x1e00, 0x1e07) AM_WRITE(sprint8_motor_w)
	AM_RANGE(0x1f00, 0x1f00) AM_WRITE(MWA8_NOP) /* probably a watchdog, disabled in service mode */
	AM_RANGE(0x2000, 0x3fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xf800, 0xffff) AM_WRITE(MWA8_ROM)
ADDRESS_MAP_END


INPUT_PORTS_START( sprint8 )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P1 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P1 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P2 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P2 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P3 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P3 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN4 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P4 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P4 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN5 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P5 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P5 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(5)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(5)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN6 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P6 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P6 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(6)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(6)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN7 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P7 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P7 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(7)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(7)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN8 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P8 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P8 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(8)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(8)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(3)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(4)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(5)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(6)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(7)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(8)

	PORT_START
	PORT_DIPNAME( 0x0f, 0x08, "Play Time" )
	PORT_DIPSETTING(    0x0f, "60 seconds" )
	PORT_DIPSETTING(    0x0e, "69 seconds" )
	PORT_DIPSETTING(    0x0d, "77 seconds" )
	PORT_DIPSETTING(    0x0c, "86 seconds" )
	PORT_DIPSETTING(    0x0b, "95 seconds" )
	PORT_DIPSETTING(    0x0a, "103 seconds" )
	PORT_DIPSETTING(    0x09, "112 seconds" )
	PORT_DIPSETTING(    0x08, "120 seconds" )
	PORT_DIPSETTING(    0x07, "129 seconds" )
	PORT_DIPSETTING(    0x06, "138 seconds" )
	PORT_DIPSETTING(    0x05, "146 seconds" )
	PORT_DIPSETTING(    0x04, "155 seconds" )
	PORT_DIPSETTING(    0x03, "163 seconds" )
	PORT_DIPSETTING(    0x02, "172 seconds" )
	PORT_DIPSETTING(    0x01, "181 seconds" )
	PORT_DIPSETTING(    0x00, "189 seconds" )
	PORT_BIT ( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Track Select") PORT_CODE(KEYCODE_SPACE)

	PORT_START
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

INPUT_PORTS_END


INPUT_PORTS_START( sprint8p )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P1 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P1 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P2 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P2 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P3 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P3 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN4 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P4 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P4 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN5 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P5 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P5 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(5)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(5)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN6 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P6 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P6 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(6)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(6)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN7 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P7 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P7 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(7)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(7)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT ( 0x01, IP_ACTIVE_LOW,  IPT_COIN8 )
	PORT_BIT ( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P8 */
	PORT_BIT ( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P8 */
	PORT_BIT ( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(8)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(8)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(3)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(4)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(5)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(6)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(7)

	PORT_START
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(8)

	PORT_START
	PORT_DIPNAME( 0x03, 0x03, "Play Time" )
	PORT_DIPSETTING(    0x00, "54 seconds" )
	PORT_DIPSETTING(    0x01, "108 seconds" )
	PORT_DIPSETTING(    0x03, "216 seconds" )
	PORT_BIT ( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_DIPNAME( 0x01, 0x01, "Play Mode" )
	PORT_DIPSETTING(    0x00, "Chase" )
	PORT_DIPSETTING(    0x01, "Tag" )

	PORT_START
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

INPUT_PORTS_END


static const gfx_layout tile_layout_1 =
{
	16, 8,
	64,
	1,
	{ 0 },
	{
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0
	},
	{
		0x000, 0x200, 0x400, 0x600, 0x800, 0xa00, 0xc00, 0xe00
	},
	8
};


static const gfx_layout tile_layout_2 =
{
	16, 8,
	64,
	1,
	{ 0 },
	{
		0x000, 0x000, 0x200, 0x200, 0x400, 0x400, 0x600, 0x600,
		0x800, 0x800, 0xa00, 0xa00, 0xc00, 0xc00, 0xe00, 0xe00

	},
	{
		0, 1, 2, 3, 4, 5, 6, 7
	},
	8
};


static const gfx_layout car_layout =
{
	16, 8,
	8,
	1,
	{ 0 },
	{
		0x07, 0x06, 0x05, 0x04, 0x0f, 0x0e, 0x0d, 0x0c,
		0x17, 0x16, 0x15, 0x14, 0x1f, 0x1e, 0x1d, 0x1c
	},
	{
		0x000, 0x020, 0x040, 0x060, 0x080, 0x0a0, 0x0c0, 0x0e0
	},
	0x100
};


static const gfx_decode gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tile_layout_1, 0, 18 },
	{ REGION_GFX1, 0, &tile_layout_2, 0, 18 },
	{ REGION_GFX2, 0, &car_layout, 0, 16 },
	{ -1 }
};


static MACHINE_DRIVER_START( sprint8 )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6800, 11055000 / 11) /* ? */
	MDRV_CPU_PROGRAM_MAP(readmem, writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(29 * 1000000 / 15750)
	MDRV_MACHINE_RESET(sprint8)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_UPDATE_AFTER_VBLANK)
	MDRV_SCREEN_SIZE(512, 261)
	MDRV_VISIBLE_AREA(0, 495, 0, 231)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(18)
	MDRV_COLORTABLE_LENGTH(36)

	MDRV_PALETTE_INIT(sprint8)
	MDRV_VIDEO_START(sprint8)
	MDRV_VIDEO_UPDATE(sprint8)
	MDRV_VIDEO_EOF(sprint8)

	/* sound hardware */
MACHINE_DRIVER_END


ROM_START( sprint8 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "7313.j1", 0x2400, 0x0800, CRC(1231f944) SHA1(d16c76da6a74513eb40811d806e0dd009f6dcafb) )
	ROM_LOAD( "7314.h1", 0x2c00, 0x0800, CRC(c77c0d49) SHA1(a57b5d340a41d02edb20fcb66875908110582bc5) )
	ROM_RELOAD(          0xf800, 0x0800 )

	ROM_REGION( 0x0200, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "7315-01.n6", 0x0000, 0x0200, CRC(e2f603d0) SHA1(8d82b72d2f4039afa3341774000105a745caf85f) )

	ROM_REGION( 0x0100, REGION_GFX2, ROMREGION_DISPOSE ) /* cars */
	ROM_LOAD( "7316-01.j5", 0x0000, 0x0100, CRC(32c028e3) SHA1(bfa76cf0981640d08e9c7fb15da134afe46afe31) )
ROM_END


ROM_START( sprint8a )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD_NIB_LOW ( "l2800s8", 0x2800, 0x0800, CRC(3926de69) SHA1(ec03d7684e393061d1d48ae73897e9dc38131c14) )
	ROM_LOAD_NIB_HIGH( "m2800s8", 0x2800, 0x0800, CRC(d009d6da) SHA1(3210806b0eb344d88d2cbcc46895f7224771c1f2) )
	ROM_LOAD_NIB_LOW ( "l3000s8", 0x3000, 0x0800, CRC(c78d9888) SHA1(a854b50b2cf0261c1f966ef1bd001084500b3545) )
	ROM_LOAD_NIB_HIGH( "m3000s8", 0x3000, 0x0800, CRC(9ebfe8f8) SHA1(9709f697a7f9cce7ff4edbdccbaf14931328a052) )
	ROM_LOAD_NIB_LOW ( "l3800s8", 0x3800, 0x0800, CRC(74a8f103) SHA1(0cc15006cbd4579feac0f07690f32b2b61f97ae9) )
	ROM_RELOAD(                   0xf800, 0x0800 )
	ROM_LOAD_NIB_HIGH( "m3800s8", 0x3800, 0x0800, CRC(90aadc75) SHA1(34ca21c37573d9a2df92d3a1e73fdc0a9885c0a0) )
	ROM_RELOAD(                   0xf800, 0x0800 )

	ROM_REGION( 0x0200, REGION_GFX1, ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "s8.n6", 0x0000, 0x0200, CRC(92cf9a7e) SHA1(6bd2d396e0a299c2e731425cabd578d569c2061b) )

	ROM_REGION( 0x0100, REGION_GFX2, ROMREGION_DISPOSE ) /* cars */
	ROM_LOAD( "s8.j5", 0x0000, 0x0100, CRC(d37fff36) SHA1(20a7a8caf2fbfe22e307fe8541d31784c8e39d1a) )
ROM_END


GAME( 1977, sprint8,  0,       sprint8, sprint8,  0, ROT0, "Atari", "Sprint 8",             GAME_NO_SOUND ,0)
GAME( 1977, sprint8a, sprint8, sprint8, sprint8p, 0, ROT0, "Atari", "Sprint 8 (play tag & chase)", GAME_NO_SOUND ,0)
