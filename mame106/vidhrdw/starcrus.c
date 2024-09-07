
/* Ramtek - Star Cruiser */

#include "driver.h"
#include "sound/samples.h"

static mame_bitmap *ship1_vid;
static mame_bitmap *ship2_vid;
static mame_bitmap *proj1_vid;
static mame_bitmap *proj2_vid;

static int s1_x = 0;
static int s1_y = 0;
static int s2_x = 0;
static int s2_y = 0;
static int p1_x = 0;
static int p1_y = 0;
static int p2_x = 0;
static int p2_y = 0;

static int p1_sprite = 0;
static int p2_sprite = 0;
static int s1_sprite = 0;
static int s2_sprite = 0;

static int engine1_on = 0;
static int engine2_on = 0;
static int explode1_on = 0;
static int explode2_on = 0;
static int launch1_on = 0;
static int launch2_on = 0;

/* The collision detection techniques use in this driver
   are well explained in the comments in the sprint2 driver */

static int collision_reg = 0x00;

/* I hate to have sound in vidhrdw, but the sprite and
   audio bits are in the same bytes, and there are so few
   samples... */

int starcrus_engine_sound_playing = 0;
int starcrus_explode_sound_playing = 0;
int starcrus_launch1_sound_playing = 0;
int starcrus_launch2_sound_playing = 0;

WRITE8_HANDLER( starcrus_s1_x_w ) { s1_x = data^0xff; }
WRITE8_HANDLER( starcrus_s1_y_w ) { s1_y = data^0xff; }
WRITE8_HANDLER( starcrus_s2_x_w ) { s2_x = data^0xff; }
WRITE8_HANDLER( starcrus_s2_y_w ) { s2_y = data^0xff; }
WRITE8_HANDLER( starcrus_p1_x_w ) { p1_x = data^0xff; }
WRITE8_HANDLER( starcrus_p1_y_w ) { p1_y = data^0xff; }
WRITE8_HANDLER( starcrus_p2_x_w ) { p2_x = data^0xff; }
WRITE8_HANDLER( starcrus_p2_y_w ) { p2_y = data^0xff; }

VIDEO_START( starcrus )
{
	if ((ship1_vid = auto_bitmap_alloc(16,16)) == 0)
		return 1;

	if ((ship2_vid = auto_bitmap_alloc(16,16)) == 0)
		return 1;

	if ((proj1_vid = auto_bitmap_alloc(16,16)) == 0)
		return 1;

	if ((proj2_vid = auto_bitmap_alloc(16,16)) == 0)
		return 1;

	return 0;
}

WRITE8_HANDLER( starcrus_ship_parm_1_w )
{
    s1_sprite = data&0x1f;
    engine1_on = ((data&0x20)>>5)^0x01;

    if (engine1_on || engine2_on)
    {
		if (starcrus_engine_sound_playing == 0)
		{
        	starcrus_engine_sound_playing = 1;
        	sample_start(0,0,1);	/* engine sample */

		}
    }
    else
    {
		if (starcrus_engine_sound_playing == 1)
		{
        	starcrus_engine_sound_playing = 0;
			sample_stop(0);
		}
	}
}

WRITE8_HANDLER( starcrus_ship_parm_2_w )
{
    s2_sprite = data&0x1f;
    set_led_status(2,~data & 0x80); 		/* game over lamp */
    coin_counter_w(0, ((data&0x40)>>6)^0x01); 	/* coin counter */
    engine2_on = ((data&0x20)>>5)^0x01;

    if (engine1_on || engine2_on)
    {
		if (starcrus_engine_sound_playing == 0)
		{
        	starcrus_engine_sound_playing = 1;
        	sample_start(0,0,1);	/* engine sample */
		}
    }
    else
    {
		if (starcrus_engine_sound_playing == 1)
		{
        	starcrus_engine_sound_playing = 0;
			sample_stop(0);
		}
	}

}

WRITE8_HANDLER( starcrus_proj_parm_1_w )
{
    p1_sprite = data&0x0f;
    launch1_on = ((data&0x20)>>5)^0x01;
    explode1_on = ((data&0x10)>>4)^0x01;

    if (explode1_on || explode2_on)
    {
		if (starcrus_explode_sound_playing == 0)
		{
			starcrus_explode_sound_playing = 1;
			sample_start(1,1,1);	/* explosion initial sample */
		}
	}
	else
    {
		if (starcrus_explode_sound_playing == 1)
		{
			starcrus_explode_sound_playing = 0;
			sample_start(1,2,0);	/* explosion ending sample */
		}
	}

	if (launch1_on)
	{
		if (starcrus_launch1_sound_playing == 0)
		{
			starcrus_launch1_sound_playing = 1;
			sample_start(2,3,0);	/* launch sample */
		}
	}
	else
	{
		starcrus_launch1_sound_playing = 0;
	}
}

WRITE8_HANDLER( starcrus_proj_parm_2_w )
{
    p2_sprite = data&0x0f;
    launch2_on = ((data&0x20)>>5)^0x01;
    explode2_on = ((data&0x10)>>4)^0x01;

    if (explode1_on || explode2_on)
    {
		if (starcrus_explode_sound_playing == 0)
		{
			starcrus_explode_sound_playing = 1;
			sample_start(1,1,1);	/* explosion initial sample */
		}
	}
	else
    {
		if (starcrus_explode_sound_playing == 1)
		{
			starcrus_explode_sound_playing = 0;
			sample_start(1,2,0);	/* explosion ending sample */
		}
	}

	if (launch2_on)
	{
		if (starcrus_launch2_sound_playing == 0)
		{
			starcrus_launch2_sound_playing = 1;
			sample_start(3,3,0);	/* launch sample */
		}
	}
	else
	{
		starcrus_launch2_sound_playing = 0;
	}
}

int starcrus_collision_check_s1s2(void)

{ 
struct drawgfxParams dgp0={
	ship1_vid, 	// dest
	Machine->gfx[8+((s1_sprite&0x04)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
struct drawgfxParams dgp1={
	ship2_vid, 	// dest
	Machine->gfx[10+((s2_sprite&0x04)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
	int org_x, org_y;
	int sx, sy;
	rectangle clip;

    clip.min_x=0;
    clip.max_x=15;
    clip.min_y=0;
    clip.max_y=15;

    fillbitmap(ship1_vid,Machine->pens[0],&clip);
    fillbitmap(ship2_vid,Machine->pens[0],&clip);

	/* origin is with respect to ship1 */

	org_x = s1_x;
	org_y = s1_y;

	/* Draw ship 1 */
    
    dgp0.code = (s1_sprite&0x03)^0x03;
    dgp0.flipx = (s1_sprite&0x08)>>3;
    dgp0.flipy = (s1_sprite&0x10)>>4;
    dgp0.sx = s1_x-org_x;
    dgp0.sy = s1_y-org_y;
    drawgfx(&dgp0);

	/* Draw ship 2 */
    
    dgp1.code = (s2_sprite&0x03)^0x03;
    dgp1.flipx = (s2_sprite&0x08)>>3;
    dgp1.flipy = (s2_sprite&0x10)>>4;
    dgp1.sx = s2_x-org_x;
    dgp1.sy = s2_y-org_y;
    drawgfx(&dgp1);

    /* Now check for collisions */
    for (sy=0;sy<16;sy++)
    {
        for (sx=0;sx<16;sx++)
        {
        	if (read_pixel(ship1_vid, sx, sy)==Machine->pens[1])
           	{
        		/* Condition 1 - ship 1 = ship 2 */
				if (read_pixel(ship2_vid, sx, sy)==Machine->pens[1])
                	return 1;
			}
        }
    }

    return 0;
}
} // end of patch paragraph


int starcrus_collision_check_p1p2(void)
{
	int org_x, org_y;
	int sx, sy;
	rectangle clip;

	/* if both are scores, return */
	if ( ((p1_sprite & 0x08) == 0) &&
         ((p2_sprite & 0x08) == 0) )
	{
		return 0;
	}

    clip.min_x=0;
    clip.max_x=15;
    clip.min_y=0;
    clip.max_y=15;

    fillbitmap(proj1_vid,Machine->pens[0],&clip);
    fillbitmap(proj2_vid,Machine->pens[0],&clip);

	/* origin is with respect to proj1 */

	org_x = p1_x;
	org_y = p1_y;

	if (p1_sprite & 0x08)	/* if p1 is a projectile */
	
{ 
struct drawgfxParams dgp2={
	proj1_vid, 	// dest
	Machine->gfx[(p1_sprite&0x0c)>>2], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
		/* Draw score/projectile 1 */
		
		dgp2.code = (p1_sprite&0x03)^0x03;
		dgp2.sx = p1_x-org_x;
		dgp2.sy = p1_y-org_y;
		drawgfx(&dgp2);
	}
} // end of patch paragraph


	if (p2_sprite & 0x08)	/* if p2 is a projectile */
	
{ 
struct drawgfxParams dgp3={
	proj2_vid, 	// dest
	Machine->gfx[4+((p2_sprite&0x0c)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
		/* Draw score/projectile 2 */
		
		dgp3.code = (p2_sprite&0x03)^0x03;
		dgp3.sx = p2_x-org_x;
		dgp3.sy = p2_y-org_y;
		drawgfx(&dgp3);
	}
} // end of patch paragraph


    /* Now check for collisions */
    for (sy=0;sy<16;sy++)
    {
        for (sx=0;sx<16;sx++)
        {
        	if (read_pixel(proj1_vid, sx, sy)==Machine->pens[1])
           	{
        		/* Condition 1 - proj 1 = proj 2 */
				if (read_pixel(proj2_vid, sx, sy)==Machine->pens[1])
                	return 1;
			}
        }
    }

    return 0;
}

int starcrus_collision_check_s1p1p2(void)

{ 
struct drawgfxParams dgp4={
	ship1_vid, 	// dest
	Machine->gfx[8+((s1_sprite&0x04)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
	int org_x, org_y;
	int sx, sy;
	rectangle clip;

	/* if both are scores, return */
	if ( ((p1_sprite & 0x08) == 0) &&
         ((p2_sprite & 0x08) == 0) )
	{
		return 0;
	}

    clip.min_x=0;
    clip.max_x=15;
    clip.min_y=0;
    clip.max_y=15;

    fillbitmap(ship1_vid,Machine->pens[0],&clip);
    fillbitmap(proj1_vid,Machine->pens[0],&clip);
    fillbitmap(proj2_vid,Machine->pens[0],&clip);

	/* origin is with respect to ship1 */

	org_x = s1_x;
	org_y = s1_y;

	/* Draw ship 1 */
    
    dgp4.code = (s1_sprite&0x03)^0x03;
    dgp4.flipx = (s1_sprite&0x08)>>3;
    dgp4.flipy = (s1_sprite&0x10)>>4;
    dgp4.sx = s1_x-org_x;
    dgp4.sy = s1_y-org_y;
    drawgfx(&dgp4);

	if (p1_sprite & 0x08)	/* if p1 is a projectile */
	
{ 
struct drawgfxParams dgp5={
	proj1_vid, 	// dest
	Machine->gfx[(p1_sprite&0x0c)>>2], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
		/* Draw projectile 1 */
		
		dgp5.code = (p1_sprite&0x03)^0x03;
		dgp5.sx = p1_x-org_x;
		dgp5.sy = p1_y-org_y;
		drawgfx(&dgp5);
	}
} // end of patch paragraph


	if (p2_sprite & 0x08)	/* if p2 is a projectile */
	
{ 
struct drawgfxParams dgp6={
	proj2_vid, 	// dest
	Machine->gfx[4+((p2_sprite&0x0c)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
		/* Draw projectile 2 */
		
		dgp6.code = (p2_sprite&0x03)^0x03;
		dgp6.sx = p2_x-org_x;
		dgp6.sy = p2_y-org_y;
		drawgfx(&dgp6);
	}
} // end of patch paragraph


    /* Now check for collisions */
    for (sy=0;sy<16;sy++)
    {
        for (sx=0;sx<16;sx++)
        {
        	if (read_pixel(ship1_vid, sx, sy)==Machine->pens[1])
           	{
        		/* Condition 1 - ship 1 = proj 1 */
				if (read_pixel(proj1_vid, sx, sy)==Machine->pens[1])
                	return 1;
        		/* Condition 2 - ship 1 = proj 2 */
        		if (read_pixel(proj2_vid, sx, sy)==Machine->pens[1])
                	return 1;
            }
        }
    }

    return 0;
}
} // end of patch paragraph


int starcrus_collision_check_s2p1p2(void)

{ 
struct drawgfxParams dgp7={
	ship2_vid, 	// dest
	Machine->gfx[10+((s2_sprite&0x04)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
	int org_x, org_y;
	int sx, sy;
	rectangle clip;

	/* if both are scores, return */
	if ( ((p1_sprite & 0x08) == 0) &&
         ((p2_sprite & 0x08) == 0) )
	{
		return 0;
	}

    clip.min_x=0;
    clip.max_x=15;
    clip.min_y=0;
    clip.max_y=15;

    fillbitmap(ship2_vid,Machine->pens[0],&clip);
    fillbitmap(proj1_vid,Machine->pens[0],&clip);
    fillbitmap(proj2_vid,Machine->pens[0],&clip);

	/* origin is with respect to ship2 */

	org_x = s2_x;
	org_y = s2_y;

	/* Draw ship 2 */
    
    dgp7.code = (s2_sprite&0x03)^0x03;
    dgp7.flipx = (s2_sprite&0x08)>>3;
    dgp7.flipy = (s2_sprite&0x10)>>4;
    dgp7.sx = s2_x-org_x;
    dgp7.sy = s2_y-org_y;
    drawgfx(&dgp7);

	if (p1_sprite & 0x08)	/* if p1 is a projectile */
	
{ 
struct drawgfxParams dgp8={
	proj1_vid, 	// dest
	Machine->gfx[(p1_sprite&0x0c)>>2], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
		/* Draw projectile 1 */
		
		dgp8.code = (p1_sprite&0x03)^0x03;
		dgp8.sx = p1_x-org_x;
		dgp8.sy = p1_y-org_y;
		drawgfx(&dgp8);
	}
} // end of patch paragraph


	if (p2_sprite & 0x08)	/* if p2 is a projectile */
	
{ 
struct drawgfxParams dgp9={
	proj2_vid, 	// dest
	Machine->gfx[4+((p2_sprite&0x0c)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&clip, 	// clip
	TRANSPARENCY_NONE, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
		/* Draw projectile 2 */
		
		dgp9.code = (p2_sprite&0x03)^0x03;
		dgp9.sx = p2_x-org_x;
		dgp9.sy = p2_y-org_y;
		drawgfx(&dgp9);
	}
} // end of patch paragraph


    /* Now check for collisions */
    for (sy=0;sy<16;sy++)
    {
        for (sx=0;sx<16;sx++)
        {
        	if (read_pixel(ship2_vid, sx, sy)==Machine->pens[1])
           	{
        		/* Condition 1 - ship 2 = proj 1 */
				if (read_pixel(proj1_vid, sx, sy)==Machine->pens[1])
                	return 1;
        		/* Condition 2 - ship 2 = proj 2 */
        		if (read_pixel(proj2_vid, sx, sy)==Machine->pens[1])
                	return 1;
            }
        }
    }

    return 0;
}
} // end of patch paragraph


VIDEO_UPDATE( starcrus )

{ 
struct drawgfxParams dgp10={
	bitmap, 	// dest
	Machine->gfx[8+((s1_sprite&0x04)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
struct drawgfxParams dgp11={
	bitmap, 	// dest
	Machine->gfx[10+((s2_sprite&0x04)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
struct drawgfxParams dgp12={
	bitmap, 	// dest
	Machine->gfx[(p1_sprite&0x0c)>>2], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
struct drawgfxParams dgp13={
	bitmap, 	// dest
	Machine->gfx[4+((p2_sprite&0x0c)>>2)], 	// gfx
	0, 	// code
	0, 	// color
	0, 	// flipx
	0, 	// flipy
	0, 	// sx
	0, 	// sy
	&Machine->visible_area, 	// clip
	TRANSPARENCY_PEN, 	// transparency
	0, 	// transparent_color
	0, 	// scalex
	0, 	// scaley
	NULL, 	// pri_buffer
	0 	// priority_mask
  };
{
    fillbitmap(bitmap,Machine->pens[0],&Machine->visible_area);

	/* Draw ship 1 */
    
    dgp10.code = (s1_sprite&0x03)^0x03;
    dgp10.flipx = (s1_sprite&0x08)>>3;
    dgp10.flipy = (s1_sprite&0x10)>>4;
    dgp10.sx = s1_x;
    dgp10.sy = s1_y;
    drawgfx(&dgp10);

	/* Draw ship 2 */
    
    dgp11.code = (s2_sprite&0x03)^0x03;
    dgp11.flipx = (s2_sprite&0x08)>>3;
    dgp11.flipy = (s2_sprite&0x10)>>4;
    dgp11.sx = s2_x;
    dgp11.sy = s2_y;
    drawgfx(&dgp11);

	/* Draw score/projectile 1 */
	
	dgp12.code = (p1_sprite&0x03)^0x03;
	dgp12.sx = p1_x;
	dgp12.sy = p1_y;
	drawgfx(&dgp12);

	/* Draw score/projectile 2 */
	
	dgp13.code = (p2_sprite&0x03)^0x03;
	dgp13.sx = p2_x;
	dgp13.sy = p2_y;
	drawgfx(&dgp13);

    /* Collision detection */

    collision_reg = 0x00;

	/* Check for collisions between ship1 and ship2 */
    if (starcrus_collision_check_s1s2())
    {
        collision_reg |= 0x08;
    }
	/* Check for collisions between ship1 and projectiles */
    if (starcrus_collision_check_s1p1p2())
    {
        collision_reg |= 0x02;
    }
    /* Check for collisions between ship1 and projectiles */
    if (starcrus_collision_check_s2p1p2())
    {
        collision_reg |= 0x01;
    }
    /* Check for collisions between ship1 and projectiles */
    /* Note: I don't think this is used by the game */
    if (starcrus_collision_check_p1p2())
    {
        collision_reg |= 0x04;
    }

}
} // end of patch paragraph


READ8_HANDLER( starcrus_coll_det_r )
{
    return collision_reg ^ 0xff;
}
