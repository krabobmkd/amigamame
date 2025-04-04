/***************************************************************************

  vidhrdw.c

  Functions to emulate the video hardware of the machine.

---

The Video system used in Space Tactics is unusual.
Here are my notes on how the video system works:

There are 4, 4K pages of Video RAM. (B,D,E & F)

The first 1K of each VRAM page contains the following:
0 0 V V V V V H H H H H   offset value for each 8x8 bitmap
     (v-tile)  (h-tile)

The offset values are used to generate an access into the
last 2K of the VRAM page:
1 D D D D D D D D V V V   here we find 8x8 character data
     (offset)    (line)

In addition, in Page B, the upper nibble of the offset is
also used to select the color palette for the tile.

Page B, D, E, and F all work similarly, except that pages
D, E, and F can be offset from Page B by a given
number of scanlines, based on the contents of the offset
RAM

The offset RAM is addressed in this format:
1 0 0 0 P P P V V V V V V V V V
        (Page)   (scanline)
Page 4=D, 5=E, 6=F

Page D, E, and F are drawn offset from the top of the screen,
starting on the first scanline which contains a 1 in the
appropriate memory location

---

The composited monitor image is seen in a mirror.  It appears
to move when the player moves the handle, due to motors which
tilt the mirror up and down, and the monitor left and right.

---

As if this wasn't enough, there are also "3D" fire beams made
of 120 green LED's which are on a mechanism in front of the mirror.
Along with a single red "sight" LED.  I am reading in the sequence
ROMS and building up a character set to simulate the LEDS with
conventional character graphics.

Finally, there is a score display made of 7-segment LEDS, along
with Credits, Barriers, and Rounds displays made of some other
type of LED bar graphs.  I'm displaying them the best I can on the
bottom line of the screen

***************************************************************************/

#include "driver.h"

/* These are defined in machine/stactics.c */
extern int stactics_vert_pos;
extern int stactics_horiz_pos;
extern unsigned char *stactics_motor_on;

/* These are needed by machine/stactics.c  */
int stactics_vblank_count;
int stactics_shot_standby;
int stactics_shot_arrive;

/* These are needed by driver/stactics.c   */
unsigned char *stactics_scroll_ram;
unsigned char *stactics_videoram_b;
unsigned char *stactics_chardata_b;
unsigned char *stactics_videoram_d;
unsigned char *stactics_chardata_d;
unsigned char *stactics_videoram_e;
unsigned char *stactics_chardata_e;
unsigned char *stactics_videoram_f;
unsigned char *stactics_chardata_f;
unsigned char *stactics_display_buffer;

static unsigned char *dirty_videoram_b;
static unsigned char *dirty_chardata_b;
static unsigned char *dirty_videoram_d;
static unsigned char *dirty_chardata_d;
static unsigned char *dirty_videoram_e;
static unsigned char *dirty_chardata_e;
static unsigned char *dirty_videoram_f;
static unsigned char *dirty_chardata_f;

static int d_offset;
static int e_offset;
static int f_offset;

static int palette_select;

static mame_bitmap *tmpbitmap2;
static mame_bitmap *bitmap_B;
static mame_bitmap *bitmap_D;
static mame_bitmap *bitmap_E;
static mame_bitmap *bitmap_F;

static unsigned char *beamdata;
static int states_per_frame;

#define DIRTY_CHARDATA_SIZE  0x100
#define BEAMDATA_SIZE        0x800

/* The first 16 came from the 7448 BCD to 7-segment decoder data sheet */
/* The rest are made up */

static unsigned char stactics_special_chars[32*8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Space */
    0x80, 0x80, 0x80, 0xf0, 0x80, 0x80, 0xf0, 0x00,   /* extras... */
    0xf0, 0x80, 0x80, 0xf0, 0x00, 0x00, 0xf0, 0x00,   /* extras... */
    0x90, 0x90, 0x90, 0xf0, 0x00, 0x00, 0x00, 0x00,   /* extras... */
    0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0xf0, 0x00,   /* extras... */
    0x00, 0x00, 0x00, 0xf0, 0x80, 0x80, 0xf0, 0x00,   /* extras... */
    0xf0, 0x90, 0x90, 0xf0, 0x10, 0x10, 0xf0, 0x00,   /* 9 */
    0xf0, 0x90, 0x90, 0xf0, 0x90, 0x90, 0xf0, 0x00,   /* 8 */
    0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00,   /* 7 */
    0xf0, 0x80, 0x80, 0xf0, 0x90, 0x90, 0xf0, 0x00,   /* 6 */
    0xf0, 0x80, 0x80, 0xf0, 0x10, 0x10, 0xf0, 0x00,   /* 5 */
    0x90, 0x90, 0x90, 0xf0, 0x10, 0x10, 0x10, 0x00,   /* 4 */
    0xf0, 0x10, 0x10, 0xf0, 0x10, 0x10, 0xf0, 0x00,   /* 3 */
    0xf0, 0x10, 0x10, 0xf0, 0x80, 0x80, 0xf0, 0x00,   /* 2 */
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00,   /* 1 */
    0xf0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0, 0x00,   /* 0 */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Space */
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 1 pip */
    0x60, 0x90, 0x80, 0x60, 0x10, 0x90, 0x60, 0x00,   /* S for Score */
    0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 2 pips */
    0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,   /* 3 pips */
    0x60, 0x90, 0x80, 0x80, 0x80, 0x90, 0x60, 0x00,   /* C for Credits */
    0xe0, 0x90, 0x90, 0xe0, 0x90, 0x90, 0xe0, 0x00,   /* B for Barriers */
    0xe0, 0x90, 0x90, 0xe0, 0xc0, 0xa0, 0x90, 0x00,   /* R for Rounds */
    0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,   /* 4 pips */
    0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x00, 0x00,   /* Colon */
    0x40, 0xe0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Sight */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Space (Unused) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Space (Unused) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Space (Unused) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* Space (Unused) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    /* Space */
};


static int firebeam_state;
static int old_firebeam_state;

PALETTE_INIT( stactics )
{
    int i,j;

    #define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
    #define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs*sizeof(unsigned short)])

    /* Now make the palette */

    for (i=0;i<16;i++)
    {
        int bit0,bit1,bit2, bit3,r,g,b;

        bit0 = i & 1;
        bit1 = (i >> 1) & 1;
        bit2 = (i >> 2) & 1;
        bit3 = (i >> 3) & 1;

        /* red component */
        r = 0xff * bit0;

        /* green component */
        g = 0xff * bit1 - 0xcc * bit3;

        /* blue component */
        b = 0xff * bit2;

		palette_set_color(i,r,g,b);
    }

    /* The color prom in Space Tactics is used for both   */
    /* color codes, and priority layering of the 4 layers */

    /* Since we are taking care of the layering by our    */
    /* drawing order, we don't need all of the color prom */
    /* entries */

    /* For each of 4 color schemes */
    for(i=0;i<4;i++)
    {
        /* For page B - Alphanumerics and alien shots */
        for(j=0;j<16;j++)
        {
            *(colortable++) = 0;
            *(colortable++) = color_prom[i*0x100+0x01*0x10+j];
        }
        /* For page F - Close Aliens (these are all the same color) */
        for(j=0;j<16;j++)
        {
            *(colortable++) = 0;
            *(colortable++) = color_prom[i*0x100+0x02*0x10];
        }
        /* For page E - Medium Aliens (these are all the same color) */
        for(j=0;j<16;j++)
        {
            *(colortable++) = 0;
            *(colortable++) = color_prom[i*0x100+0x04*0x10+j];
        }
        /* For page D - Far Aliens (these are all the same color) */
        for(j=0;j<16;j++)
        {
            *(colortable++) = 0;
            *(colortable++) = color_prom[i*0x100+0x08*0x10+j];
        }
    }
}

/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( stactics )
{
    int i,j;
    const unsigned char *firebeam_data;
    unsigned char firechar[256*8*9];

    if ((tmpbitmap  = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0) return 1;
    if ((tmpbitmap2 = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0) return 1;
    if ((bitmap_B = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)   return 1;
    if ((bitmap_D = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)   return 1;
    if ((bitmap_E = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)   return 1;
    if ((bitmap_F = auto_bitmap_alloc(Machine->drv->screen_width,Machine->drv->screen_height)) == 0)   return 1;

	/* Allocate dirty buffers */
	dirty_videoram_b = auto_malloc(videoram_size);
	dirty_videoram_d = auto_malloc(videoram_size);
	dirty_videoram_e = auto_malloc(videoram_size);
	dirty_videoram_f = auto_malloc(videoram_size);
	dirty_chardata_b = auto_malloc(DIRTY_CHARDATA_SIZE);
	dirty_chardata_d = auto_malloc(DIRTY_CHARDATA_SIZE);
	dirty_chardata_e = auto_malloc(DIRTY_CHARDATA_SIZE);
	dirty_chardata_f = auto_malloc(DIRTY_CHARDATA_SIZE);

    memset(dirty_videoram_b,1,videoram_size);
    memset(dirty_videoram_d,1,videoram_size);
    memset(dirty_videoram_e,1,videoram_size);
    memset(dirty_videoram_f,1,videoram_size);
    memset(dirty_chardata_b,1,DIRTY_CHARDATA_SIZE);
    memset(dirty_chardata_d,1,DIRTY_CHARDATA_SIZE);
    memset(dirty_chardata_e,1,DIRTY_CHARDATA_SIZE);
    memset(dirty_chardata_f,1,DIRTY_CHARDATA_SIZE);

    d_offset = 0;
    e_offset = 0;
    f_offset = 0;

    palette_select = 0;
    stactics_vblank_count = 0;
    stactics_shot_standby = 1;
    stactics_shot_arrive = 0;
    firebeam_state = 0;
    old_firebeam_state = 0;

    /* Create a fake character set for LED fire beam */

    memset(firechar,0,sizeof(firechar));
    for(i=0;i<256;i++)
    {
        for(j=0;j<8;j++)
        {
            if ((i>>j)&0x01)
            {
                firechar[i*9+(7-j)]   |= (0x01<<(7-j));
                firechar[i*9+(7-j)+1] |= (0x01<<(7-j));
            }
        }
    }

    for(i=0;i<256;i++)
    {
        decodechar(Machine->gfx[4],
                   i,
                   firechar,
                   Machine->drv->gfxdecodeinfo[4].gfxlayout);
    }

    /* Decode the Fire Beam ROM for later      */
    /* (I am basically just juggling the bytes */
    /* and storing it again to make it easier) */

	beamdata = auto_malloc(BEAMDATA_SIZE);

    firebeam_data = memory_region(REGION_GFX1);

    for(i=0;i<256;i++)
    {
        beamdata[i*8]   = firebeam_data[i                   ];
        beamdata[i*8+1] = firebeam_data[i + 1024            ];
        beamdata[i*8+2] = firebeam_data[i              + 256];
        beamdata[i*8+3] = firebeam_data[i + 1024       + 256];
        beamdata[i*8+4] = firebeam_data[i        + 512      ];
        beamdata[i*8+5] = firebeam_data[i + 1024 + 512      ];
        beamdata[i*8+6] = firebeam_data[i        + 512 + 256];
        beamdata[i*8+7] = firebeam_data[i + 1024 + 512 + 256];
    }

    /* Build some characters for simulating the LED displays */

    for(i=0;i<32;i++)
    {
        decodechar(Machine->gfx[5],
                   i,
                   stactics_special_chars,
                   Machine->drv->gfxdecodeinfo[5].gfxlayout);
    }

    stactics_vblank_count = 0;
    stactics_vert_pos = 0;
    stactics_horiz_pos = 0;
    *stactics_motor_on = 0;

    return 0;
}


WRITE8_HANDLER( stactics_palette_w )
{
    int old_palette_select = palette_select;

    switch (offset)
    {
        case 0:
            palette_select = (palette_select & 0x02) | (data&0x01);
            break;
        case 1:
            palette_select = (palette_select & 0x01) | ((data&0x01)<<1);
            break;
        default:
            return;
    }

    if (old_palette_select != palette_select)
    {
        memset(dirty_videoram_b,1,videoram_size);
        memset(dirty_videoram_d,1,videoram_size);
        memset(dirty_videoram_e,1,videoram_size);
        memset(dirty_videoram_f,1,videoram_size);
    }
    return;
}


WRITE8_HANDLER( stactics_scroll_ram_w )
{
    int temp;

    if (stactics_scroll_ram[offset] != data)
    {
        stactics_scroll_ram[offset] = data;
        temp = (offset&0x700)>>8;
        switch(temp)
        {
            case 4:  // Page D
            {
                if (data&0x01)
                    d_offset = offset&0xff;
                break;
            }
            case 5:  // Page E
            {
                if (data&0x01)
                    e_offset = offset&0xff;
                break;
            }
            case 6:  // Page F
            {
                if (data&0x01)
                    f_offset = offset&0xff;
                break;
            }
        }
    }
}

WRITE8_HANDLER( stactics_speed_latch_w )
{
    /* This writes to a shift register which is clocked by   */
    /* a 555 oscillator.  This value determines the speed of */
    /* the LED fire beams as follows:                        */

    /*   555_freq / bits_in_SR * edges_in_SR / states_in_PR67 / frame_rate */
    /*      = num_led_states_per_frame  */
    /*   36439 / 8 * x / 32 / 60 ~= 19/8*x */

    /* Here, we will count the number of rising edges in the shift register */

    int i;
    int num_rising_edges = 0;

    for(i=0;i<8;i++)
    {
        if ( (((data>>i)&0x01) == 1) && (((data>>((i+1)%8))&0x01) == 0))
            num_rising_edges++;
    }

    states_per_frame = num_rising_edges*19/8;
}

WRITE8_HANDLER( stactics_shot_trigger_w )
{
    stactics_shot_standby = 0;
}

WRITE8_HANDLER( stactics_shot_flag_clear_w )
{
    stactics_shot_arrive = 0;
}

WRITE8_HANDLER( stactics_videoram_b_w )
{
    if (stactics_videoram_b[offset] != data)
    {
        stactics_videoram_b[offset] = data;
        dirty_videoram_b[offset] = 1;
    }
}

WRITE8_HANDLER( stactics_chardata_b_w )
{
    if (stactics_chardata_b[offset] != data)
    {
        stactics_chardata_b[offset] = data;
        dirty_chardata_b[offset>>3] = 1;
    }
}

WRITE8_HANDLER( stactics_videoram_d_w )
{
    if (stactics_videoram_d[offset] != data)
    {
        stactics_videoram_d[offset] = data;
        dirty_videoram_d[offset] = 1;
    }
}

WRITE8_HANDLER( stactics_chardata_d_w )
{
    if (stactics_chardata_d[offset] != data)
    {
        stactics_chardata_d[offset] = data;
        dirty_chardata_d[offset>>3] = 1;
    }
}

WRITE8_HANDLER( stactics_videoram_e_w )
{
    if (stactics_videoram_e[offset] != data)
    {
        stactics_videoram_e[offset] = data;
        dirty_videoram_e[offset] = 1;
    }
}

WRITE8_HANDLER( stactics_chardata_e_w )
{
    if (stactics_chardata_e[offset] != data)
    {
        stactics_chardata_e[offset] = data;
        dirty_chardata_e[offset>>3] = 1;
    }
}

WRITE8_HANDLER( stactics_videoram_f_w )
{
    if (stactics_videoram_f[offset] != data)
    {
        stactics_videoram_f[offset] = data;
        dirty_videoram_f[offset] = 1;
    }
}

WRITE8_HANDLER( stactics_chardata_f_w )
{
    if (stactics_chardata_f[offset] != data)
    {
        stactics_chardata_f[offset] = data;
        dirty_chardata_f[offset>>3] = 1;
    }
}

/* Actual area for visible monitor stuff is only 30*8 lines */
/* The rest is used for the score, etc. */

static const rectangle visible_screen_area = {0*8, 32*8, 0*8, 30*8};

/***************************************************************************

  Draw the game screen in the given mame_bitmap.
  Do NOT call osd_update_display() from this function, it will be called by
  the main emulation engine.

***************************************************************************/

VIDEO_UPDATE( stactics )

{ 
struct drawgfxParams dgp4={
	bitmap, 	// dest
	Machine->gfx[5], 	// gfx
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
    int offs, sx, sy, i;
    int char_number;
    int color_code;
    int pixel_x, pixel_y;

    int palette_offset = palette_select * 64;

    
    { 
    struct drawgfxParams dgp0={
    	bitmap_D, 	// dest
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
    struct drawgfxParams dgp1={
    	bitmap_E, 	// dest
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
    	bitmap_F, 	// dest
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
    struct drawgfxParams dgp3={
    	bitmap_B, 	// dest
    	Machine->gfx[0], 	// gfx
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
    for(offs=0x400-1; offs>=0; offs--)
    {
        sx = offs%32;
        sy = offs/32;

        color_code = palette_offset + (stactics_videoram_b[offs]>>4);

        /* Draw aliens in Page D */

        char_number = stactics_videoram_d[offs];

        if (dirty_chardata_d[char_number] == 1)
        {
            decodechar(Machine->gfx[3],
                       char_number,
                       stactics_chardata_d,
                       Machine->drv->gfxdecodeinfo[3].gfxlayout);
            dirty_chardata_d[char_number] = 2;
            dirty_videoram_d[offs] = 1;
        }
        else if (dirty_chardata_d[char_number] == 2)
        {
            dirty_videoram_d[offs] = 1;
        }

        if (dirty_videoram_d[offs])
        {
            
            dgp0.code = char_number;
            dgp0.color = color_code;
            dgp0.sx = sx*8;
            dgp0.sy = sy*8;
            drawgfx(&dgp0);
            dirty_videoram_d[offs] = 0;
        }

        /* Draw aliens in Page E */

        char_number = stactics_videoram_e[offs];

        if (dirty_chardata_e[char_number] == 1)
        {
            decodechar(Machine->gfx[2],
                       char_number,
                       stactics_chardata_e,
                       Machine->drv->gfxdecodeinfo[2].gfxlayout);
            dirty_chardata_e[char_number] = 2;
            dirty_videoram_e[offs] = 1;
        }
        else if (dirty_chardata_e[char_number] == 2)
        {
            dirty_videoram_e[offs] = 1;
        }

        if (dirty_videoram_e[offs])
        {
            
            dgp1.code = char_number;
            dgp1.color = color_code;
            dgp1.sx = sx*8;
            dgp1.sy = sy*8;
            drawgfx(&dgp1);
            dirty_videoram_e[offs] = 0;
        }

        /* Draw aliens in Page F */

        char_number = stactics_videoram_f[offs];

        if (dirty_chardata_f[char_number] == 1)
        {
            decodechar(Machine->gfx[1],
                       char_number,
                       stactics_chardata_f,
                       Machine->drv->gfxdecodeinfo[1].gfxlayout);
            dirty_chardata_f[char_number] = 2;
            dirty_videoram_f[offs] = 1;
        }
        else if (dirty_chardata_f[char_number] == 2)
        {
            dirty_videoram_f[offs] = 1;
        }

        if (dirty_videoram_f[offs])
        {
            
            dgp2.code = char_number;
            dgp2.color = color_code;
            dgp2.sx = sx*8;
            dgp2.sy = sy*8;
            drawgfx(&dgp2);
            dirty_videoram_f[offs] = 0;
        }

        /* Draw the page B stuff */

        char_number = stactics_videoram_b[offs];

        if (dirty_chardata_b[char_number] == 1)
        {
            decodechar(Machine->gfx[0],
                       char_number,
                       stactics_chardata_b,
                       Machine->drv->gfxdecodeinfo[0].gfxlayout);
            dirty_chardata_b[char_number] = 2;
            dirty_videoram_b[offs] = 1;
        }
        else if (dirty_chardata_b[char_number] == 2)
        {
            dirty_videoram_b[offs] = 1;
        }

        if (dirty_videoram_b[offs])
        {
            
            dgp3.code = char_number;
            dgp3.color = color_code;
            dgp3.sx = sx*8;
            dgp3.sy = sy*8;
            drawgfx(&dgp3);
            dirty_videoram_b[offs] = 0;
        }

    }
    } // end of patch paragraph


    /* Now, composite the four layers together */

    copyscrollbitmap(tmpbitmap2,bitmap_D,0,0,1,&d_offset,
                     &Machine->visible_area,TRANSPARENCY_NONE,0);
    copyscrollbitmap(tmpbitmap2,bitmap_E,0,0,1,&e_offset,
                     &Machine->visible_area,TRANSPARENCY_COLOR,0);
    copyscrollbitmap(tmpbitmap2,bitmap_F,0,0,1,&f_offset,
                     &Machine->visible_area,TRANSPARENCY_COLOR,0);
    copybitmap(tmpbitmap2,bitmap_B,0,0,0,0,
                     &Machine->visible_area,TRANSPARENCY_COLOR,0);

    /* Now flip X & simulate the monitor motion */
    fillbitmap(bitmap, Machine->pens[0], &Machine->visible_area);
    copybitmap(bitmap,tmpbitmap2,1,0,stactics_horiz_pos,stactics_vert_pos,
                &visible_screen_area,TRANSPARENCY_NONE,0);

    /* Finally, draw stuff that is on the console or on top of the monitor (LED's) */

    /***** Draw Score Display *****/

    pixel_x = 16;
    pixel_y = 248;

    /* Draw an S */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw a colon */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw the digits */
    
    { 
    struct drawgfxParams dgp6={
    	bitmap, 	// dest
    	Machine->gfx[5], 	// gfx
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
    for(i=1;i<7;i++)
    {
        
        dgp6.code = stactics_display_buffer[i]&0x0f;
        dgp6.sx = pixel_x;
        dgp6.sy = pixel_y;
        drawgfx(&dgp6);
        pixel_x+=6;
    }
    } // end of patch paragraph


    /***** Draw Credits Indicator *****/

    pixel_x = 64+16;

    /* Draw a C */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw a colon */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw the pips */
    
    { 
    struct drawgfxParams dgp9={
    	bitmap, 	// dest
    	Machine->gfx[5], 	// gfx
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
    for(i=7;i<9;i++)
    {
        
        dgp9.code = 16 + (~stactics_display_buffer[i]&0x0f);
        dgp9.sx = pixel_x;
        dgp9.sy = pixel_y;
        drawgfx(&dgp9);
        pixel_x+=2;
    }
    } // end of patch paragraph


    /***** Draw Rounds Indicator *****/

    pixel_x = 128+16;

    /* Draw an R */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw a colon */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw the pips */
    
    { 
    struct drawgfxParams dgp12={
    	bitmap, 	// dest
    	Machine->gfx[5], 	// gfx
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
    for(i=9;i<12;i++)
    {
        
        dgp12.code = 16 + (~stactics_display_buffer[i]&0x0f);
        dgp12.sx = pixel_x;
        dgp12.sy = pixel_y;
        drawgfx(&dgp12);
        pixel_x+=2;
    }
    } // end of patch paragraph


    /***** Draw Barriers Indicator *****/

    pixel_x = 192+16;
    /* Draw a B */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw a colon */
    
    dgp4.sx = pixel_x;
    dgp4.sy = pixel_y;
    drawgfx(&dgp4);
    pixel_x+=6;
    /* Draw the pips */
    
    { 
    struct drawgfxParams dgp15={
    	bitmap, 	// dest
    	Machine->gfx[5], 	// gfx
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
    for(i=12;i<16;i++)
    {
        
        dgp15.code = 16 + (~stactics_display_buffer[i]&0x0f);
        dgp15.sx = pixel_x;
        dgp15.sy = pixel_y;
        drawgfx(&dgp15);
        pixel_x+=2;
    }
    } // end of patch paragraph


    /* An LED fire beam! */
    /* (There were 120 green LEDS mounted in the cabinet in the game, */
    /*  and one red one, for the sight)                               */

    /* First, update the firebeam state */

    old_firebeam_state = firebeam_state;
    if (stactics_shot_standby == 0)
    {
        firebeam_state = (firebeam_state + states_per_frame)%512;
    }

    /* These are thresholds for the two shots from the LED fire ROM */
    /* (Note: There are two more for sound triggers, */
    /*        whenever that gets implemented)        */

    if ((old_firebeam_state < 0x8b) & (firebeam_state >= 0x8b))
        stactics_shot_arrive = 1;

    if ((old_firebeam_state < 0xca) & (firebeam_state >= 0xca))
        stactics_shot_arrive = 1;

    if (firebeam_state > 255)
    {
        firebeam_state = 0;
        stactics_shot_standby = 1;
    }

    /* Now, draw the beam */

    pixel_x = 15;
    pixel_y = 166;

    
    { 
    struct drawgfxParams dgp16={
    	bitmap, 	// dest
    	Machine->gfx[4], 	// gfx
    	0, 	// code
    	0, 	// color
    	0, 	// flipx
    	0, 	// flipy
    	0, 	// sx
    	0, 	// sy
    	&Machine->visible_area, 	// clip
    	TRANSPARENCY_COLOR, 	// transparency
    	0, 	// transparent_color
    	0, 	// scalex
    	0, 	// scaley
    	NULL, 	// pri_buffer
    	0 	// priority_mask
      };
    for(i=0;i<8;i++)
    {
        if ((i%2)==1)
        {
            /* Draw 7 LEDS on each side */
            
            dgp16.code = beamdata[firebeam_state*8+i]&0x7f;
            dgp16.color = 16*2;
            dgp16.flipx = /* Make it green */                    0;
            dgp16.sx = pixel_x;
            dgp16.sy = pixel_y;
            drawgfx(&dgp16);
            
            dgp16.code = beamdata[firebeam_state*8+i]&0x7f;
            dgp16.color = 16*2;
            dgp16.flipx = /* Make it green */                    1;
            dgp16.sx = 255-pixel_x;
            dgp16.sy = pixel_y;
            drawgfx(&dgp16);
            pixel_x+=14;
            pixel_y-=7;
        }
        else
        {
            /* Draw 8 LEDS on each side */
            
            dgp16.code = beamdata[firebeam_state*8+i];
            dgp16.color = 16*2;
            dgp16.flipx = /* Make it green */                    0;
            dgp16.sx = pixel_x;
            dgp16.sy = pixel_y;
            drawgfx(&dgp16);
            
            dgp16.code = beamdata[firebeam_state*8+i];
            dgp16.color = 16*2;
            dgp16.flipx = /* Make it green */                    1;
            dgp16.sx = 255-pixel_x;
            dgp16.sy = pixel_y;
            drawgfx(&dgp16);
            pixel_x+=16;
            pixel_y-=8;
        }

    }
    } // end of patch paragraph


    /* Red Sight LED */

    pixel_x = 134;
    pixel_y = 112;

    
    { 
    struct drawgfxParams dgp20={
    	bitmap, 	// dest
    	Machine->gfx[5], 	// gfx
    	0, 	// code
    	0, 	// color
    	0, 	// flipx
    	0, 	// flipy
    	0, 	// sx
    	0, 	// sy
    	&Machine->visible_area, 	// clip
    	TRANSPARENCY_COLOR, 	// transparency
    	0, 	// transparent_color
    	0, 	// scalex
    	0, 	// scaley
    	NULL, 	// pri_buffer
    	0 	// priority_mask
      };
    if (*stactics_motor_on & 0x01)
    {
        
        dgp20.flipx = /* red */                0;
        dgp20.sx = pixel_x;
        dgp20.sy = pixel_y;
        drawgfx(&dgp20);
    }
    } // end of patch paragraph


    /* Update vblank counter */
    stactics_vblank_count++;

    /* reset dirty flags */
    for(i=0;i<0xff;i++)
    {
        dirty_chardata_b[i] &= 0x01;
        dirty_chardata_d[i] &= 0x01;
        dirty_chardata_e[i] &= 0x01;
        dirty_chardata_f[i] &= 0x01;
    }

}
} // end of patch paragraph

