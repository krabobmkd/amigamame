#include "drawCtrl.h"

#include "png.h"
#include "fileio.h"

//int loadPng(const char *ppath,struct extraBitmap *pbm )
//{

//}

static int open_and_read_png(const char *gamename, const char *filename, png_info *png)
{
	int result;
	mame_file *file;

	/* open the file */
	file = mame_fopen(gamename, filename, FILETYPE_ARTWORK, 0);
	if (!file)
		return 0;

	/* read the PNG data */
	result = png_read_file(file, png);
	mame_fclose(file);
	if (!result)
		return 0;

	/* verify we can handle this PNG */
	if (png->bit_depth > 8)
	{
		loginfo(2,"Unsupported bit depth %d (8 bit max)\n", png->bit_depth);
		free(png->image);
		return 0;
	}
	if (png->interlace_method != 0)
	{
		loginfo(2,"Interlace unsupported\n");
		free(png->image);
		return 0;
	}
	if (png->color_type != 0 && png->color_type != 3 && png->color_type != 2 && png->color_type != 6)
	{
		loginfo(2,"Unsupported color type %d\n", png->color_type);
		free(png->image);
		return 0;
	}

	/* if less than 8 bits, upsample */
	png_expand_buffer_8bit(png);
	return 1;
}

struct drawableExtra_lever *drawextra_createLever()
{
    struct drawableExtra_lever *p =
    (struct drawableExtra_lever *) auto_malloc(sizeof(struct drawableExtra_lever));
    if(!p) return NULL; // should have jmp throwed anyway.
    int r = open_and_read_png("skin/lever.png",&p->_img1._png );
    r = open_and_read_png("skin/lever2.png",&p->_img2._png );

    return p;
}

struct drawableExtra_steeringWheel *drawextra_createSteeringWheel()
{
    struct drawableExtra_steeringWheel *p =
    (struct drawableExtra_steeringWheel *) auto_malloc(sizeof(struct drawableExtra_steeringWheel));
    if(!p) return NULL; // should have jmp throwed anyway.
    int r = open_and_read_png("skin/swheel.png",&p->_img._png );

    return p;
}

void drawextra_setpos(struct drawableExtra *p, int x,int y)
{
    p->_xpos = x;
    p->_ypos = y;
}
// quick index remap on 32 first and 32 last colors...



void drawextra_leverCLUT16(mame_bitmap *bitmap,struct drawableExtra *, int value)
{
	
}
void drawextra_wheelCLUT16(mame_bitmap *bitmap,struct drawableExtra *, int value)
{
	// rotozoom !
	//bitmap->
	
}
