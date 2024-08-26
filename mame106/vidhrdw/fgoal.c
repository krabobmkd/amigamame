/***************************************************************************

    Taito Field Goal video emulation

***************************************************************************/

#include "driver.h"

extern UINT8* fgoal_video_ram;

static mame_bitmap *fgbitmap;
static mame_bitmap *bgbitmap;

extern int fgoal_player;

static UINT8 xpos;
static UINT8 ypos;

static int current_color;


WRITE8_HANDLER( fgoal_color_w )
{
	current_color = data & 3;
}


WRITE8_HANDLER( fgoal_ypos_w )
{
	ypos = data;
}


WRITE8_HANDLER( fgoal_xpos_w )
{
	xpos = data;
}


VIDEO_START( fgoal )
{
	fgbitmap = auto_bitmap_alloc(256, 256);
	bgbitmap = auto_bitmap_alloc(256, 256);

	return fgbitmap == 0 || bgbitmap == 0;
}


VIDEO_UPDATE( fgoal )
{
	const UINT8* VRAM = fgoal_video_ram;

	int x;
	int y;
	int n;

	/* draw color overlay foreground and background */
    {
    struct drawgfxParams dgpz0={
        fgbitmap, 	// dest
        Machine->gfx[0], 	// gfx
        0, 	// code
        0, 	// color
        0, 	// flipx
        0, 	// flipy
        0, 	// sx
        0, 	// sy
        cliprect, 	// clip
        TRANSPARENCY_NONE, 	// transparency
        0, 	// transparent_color
        0x40000, 	// scalex
        0x40000, 	// scaley
        NULL, 	// pri_buffer
        0 	// priority_mask
      };
        int flip=0;
        int sy=0;
        if (fgoal_player == 1 && (readinputport(1) & 0x40))
        {
            flip =1;
            sy = 16;
        }
        dgpz0.flipx = dgpz0.flipy = flip;
        dgpz0.sy = sy;

        dgpz0.dest = fgbitmap; 	// dest
        dgpz0.gfx = Machine->gfx[0];
        dgpz0.color = (fgoal_player << 2) | current_color;
        drawgfxzoom(&dgpz0);

        dgpz0.dest = bgbitmap; 	// dest
        dgpz0.gfx = Machine->gfx[1];
        dgpz0.color = 0;
        drawgfxzoom(&dgpz0);
    }

	/* the ball has a fixed color */

	for (y = ypos; y < ypos + 8; y++)
	{
		for (x = xpos; x < xpos + 8; x++)
		{
			if (y < 256 && x < 256)
			{
				plot_pixel(fgbitmap, x, y, 0x38);
			}
		}
	}

	/* draw bitmap layer */

	for (y = 0; y < 256; y++)
	{
		UINT16* p = bitmap->line[y];

		const UINT16* FG = fgbitmap->line[y];
		const UINT16* BG = bgbitmap->line[y];

		for (x = 0; x < 256; x += 8)
		{
			UINT8 v = *VRAM++;

			for (n = 0; n < 8; n++)
			{
				if (v & (1 << n))
				{
					p[x + n] = FG[x + n];
				}
				else
				{
					p[x + n] = BG[x + n];
				}
			}
		}
	}
}
