/***************************************************************************

  Nintendo 8080 video emulation

***************************************************************************/

#include "driver.h"
#include <math.h>

int spacefev_red_screen;
int spacefev_red_cannon;

int helifire_flash;

static mame_timer* cannon_timer;

static int sheriff_color_mode;
static int sheriff_color_data;

static UINT8 helifire_LSFR[63];

static unsigned helifire_mv;
static unsigned helifire_sc; /* IC56 */


/* following data is based on screen shots */

static const UINT8 sheriff_color_PROM[] =
{
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x0, 0x0, 0x0, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x0, 0x0, 0x0, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x9, 0x9, 0xb,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xe, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xe, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xe, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xa, 0xa, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x2, 0x2, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xa, 0xa, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x2, 0x2, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0x9, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xa, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x3, 0x3, 0x3, 0x3, 0x3, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xa, 0xb,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xa, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x9, 0x9, 0xb,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x6, 0x6, 0x6, 0xe, 0xe, 0xd, 0xf, 0xf, 0xf,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x0, 0x0, 0x0, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x0, 0x0, 0x0, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
};


WRITE8_HANDLER( n8080_video_control_w )
{
	sheriff_color_mode = (data >> 3) & 3;
	sheriff_color_data = (data >> 0) & 7;

	flip_screen = data & 0x20;
}


PALETTE_INIT( n8080 )
{
	int i;

	for (i = 0; i < 8; i++)
	{
		palette_set_color(i,
			(i & 1) ? 255 : 0,
			(i & 2) ? 255 : 0,
			(i & 4) ? 255 : 0);
	}
}


PALETTE_INIT( helifire )
{
	int i;

	palette_init_n8080(NULL, NULL);

	for (i = 0; i < 0x100; i++)
	{
		int level = 0xff * exp(-3 * i / 255.); /* capacitor discharge */

		palette_set_color(0x000 + 8 + i, 0x00, 0x00, level);   /* shades of blue */
		palette_set_color(0x100 + 8 + i, 0x00, 0xC0, level);   /* shades of blue w/ green star */

		palette_set_color(0x200 + 8 + i, level, 0x00, 0x00);   /* shades of red */
		palette_set_color(0x300 + 8 + i, level, 0xC0, 0x00);   /* shades of red w/ green star */
	}
}


void spacefev_start_red_cannon(void)
{
	spacefev_red_cannon = 1;

	timer_adjust(cannon_timer, TIME_IN_MSEC(0.55 * 68 * 10), 0, 0);
}


void spacefev_stop_red_cannon(int dummy)
{
	spacefev_red_cannon = 0;

	timer_adjust(cannon_timer, TIME_NEVER, 0, 0);
}


void helifire_next_line(void)
{
	helifire_mv++;

	if (helifire_sc % 4 == 2)
	{
		helifire_mv %= 256;
	}
	else
	{
		if (flip_screen)
		{
			helifire_mv %= 255;
		}
		else
		{
			helifire_mv %= 257;
		}
	}

	if (helifire_mv == 128)
	{
		helifire_sc++;
	}
}


VIDEO_START( spacefev )
{
	cannon_timer = timer_alloc(spacefev_stop_red_cannon);

	flip_screen = 0;

	spacefev_red_screen = 0;
	spacefev_red_cannon = 0;

	return 0;
}


VIDEO_START( sheriff )
{
	flip_screen = 0;

	sheriff_color_mode = 0;
	sheriff_color_data = 0;

	return 0;
}


VIDEO_START( helifire )
{
	UINT8 data = 0;

	int i;

	helifire_mv = 0;
	helifire_sc = 0;

	for (i = 0; i < 63; i++)
	{
		int bit =
			(data >> 6) ^
			(data >> 7) ^ 1;

		data = (data << 1) | (bit & 1);

		helifire_LSFR[i] = data;
	}

	flip_screen = 0;

	helifire_flash = 0;

	return 0;
}


VIDEO_UPDATE( spacefev )
{
	UINT8 mask = flip_screen ? 0xff : 0x00;

	int x;
	int y;

	const UINT8* pRAM = videoram;

	for (y = 0; y < 256; y++)
	{
		UINT16* pLine = bitmap->line[y ^ mask];

		for (x = 0; x < 256; x += 8)
		{
			int n;

			UINT8 color = 0;

			if (spacefev_red_screen)
			{
				color = 1;
			}
			else
			{
				UINT8 val = memory_region(REGION_PROMS)[x >> 3];

				if ((x >> 3) == 0x06)
				{
					color = spacefev_red_cannon ? 1 : 7;
				}

				if ((x >> 3) == 0x1b)
				{
					static const UINT8 ufo_color[] =
					{
						1, /* red     */
						2, /* green   */
						7, /* white   */
						3, /* yellow  */
						5, /* magenta */
						6, /* cyan    */
					};

					int cycle = cpu_getcurrentframe() / 32;

					color = ufo_color[cycle % 6];
				}

				for (n = color + 1; n < 8; n++)
				{
					if (~val & (1 << n))
					{
						color = n;
					}
				}
			}

			for (n = 0; n < 8; n++)
			{
				pLine[(x + n) ^ mask] = (pRAM[x >> 3] & (1 << n)) ? color : 0;
			}
		}

		pRAM += 32;
	}
}


VIDEO_UPDATE( sheriff )
{
	UINT8 mask = flip_screen ? 0xff : 0x00;

	int x;
	int y;

	const UINT8* pRAM = videoram;

	for (y = 0; y < 256; y++)
	{
		UINT16* pLine = bitmap->line[y ^ mask];

		for (x = 0; x < 256; x += 8)
		{
			int n;

			UINT8 color = sheriff_color_PROM[32 * (y >> 3) + (x >> 3)];

			if (sheriff_color_mode == 1 && !(color & 8))
			{
				color = sheriff_color_data ^ 7;
			}

			if (sheriff_color_mode == 2)
			{
				color = sheriff_color_data ^ 7;
			}

			if (sheriff_color_mode == 3)
			{
				color = 7;
			}

			for (n = 0; n < 8; n++)
			{
				pLine[(x + n) ^ mask] = ((pRAM[x >> 3] >> n) & 1) ? (color & 7) : 0;
			}
		}

		pRAM += 32;
	}
}


VIDEO_UPDATE( helifire )
{
	int SUN_BRIGHTNESS = readinputport(4);
	int SEA_BRIGHTNESS = readinputport(5);

	static const int wave[8] = { 0, 1, 2, 2, 2, 1, 0, 0 };

	unsigned saved_mv = helifire_mv;
	unsigned saved_sc = helifire_sc;

	int x;
	int y;

	for (y = 0; y < 256; y++)
	{
		UINT16* pLine = bitmap->line[y];

		int level = 120 + wave[helifire_mv & 7];

		/* draw sky */

		for (x = level; x < 256; x++)
		{
			pLine[x] = 0x200 + 8 + SUN_BRIGHTNESS + x - level;
		}

		/* draw stars */

		if (helifire_mv % 8 == 4) /* upper half */
		{
			int step = (320 * (helifire_mv - 0)) % sizeof helifire_LSFR;

			int data =
				((helifire_LSFR[step] & 1) << 6) |
				((helifire_LSFR[step] & 2) << 4) |
				((helifire_LSFR[step] & 4) << 2) |
				((helifire_LSFR[step] & 8) << 0);

			pLine[0x80 + data] |= 0x100;
		}

		if (helifire_mv % 8 == 5) /* lower half */
		{
			int step = (320 * (helifire_mv - 1)) % sizeof helifire_LSFR;

			int data =
				((helifire_LSFR[step] & 1) << 6) |
				((helifire_LSFR[step] & 2) << 4) |
				((helifire_LSFR[step] & 4) << 2) |
				((helifire_LSFR[step] & 8) << 0);

			pLine[0x00 + data] |= 0x100;
		}

		/* draw sea */

		for (x = 0; x < level; x++)
		{
			pLine[x] = 8 + SEA_BRIGHTNESS + x;
		}

		/* draw foreground */

		for (x = 0; x < 256; x += 8)
		{
			int offset = 32 * y + (x >> 3);

			int n;

			for (n = 0; n < 8; n++)
			{
				if (flip_screen)
				{
					if ((videoram[offset ^ 0x1fff] << n) & 0x80)
					{
						pLine[x + n] = colorram[offset ^ 0x1fff] & 7;
					}
				}
				else
				{
					if ((videoram[offset] >> n) & 1)
					{
						pLine[x + n] = colorram[offset] & 7;
					}
				}
			}
		}

		/* next line */

		helifire_next_line();
	}

	helifire_mv = saved_mv;
	helifire_sc = saved_sc;
}


VIDEO_EOF( helifire )
{
	int n = (cpu_getcurrentframe() >> 1) % sizeof helifire_LSFR;

	int i;

	for (i = 0; i < 8; i++)
	{
		int R = (i & 1);
		int G = (i & 2);
		int B = (i & 4);

		if (helifire_flash)
		{
			if (helifire_LSFR[n] & 0x20)
			{
				G |= B;
			}

			if (cpu_getcurrentframe() & 0x04)
			{
				R |= G;
			}
		}

		palette_set_color(i,
			R ? 255 : 0,
			G ? 255 : 0,
			B ? 255 : 0);
	}

	for (i = 0; i < 256; i++)
	{
		helifire_next_line();
	}
}
