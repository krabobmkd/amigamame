/*
	this is it

*/


MACHIN( truc)


void mafunc1()
{
	blubliu;
}

void mafunc()
{
	int off;
	bu bu bu;
	
	
	
	while(blbla)
	{
		int ti=0;
		if(ti==4)
		{
		
		}
		do
		{
			drawgfx(bitmap,Machine->gfx[2],
					code + i,col,
					flip_screen,flip_screen,
					sx,sy + 16 * i * dir,
					cliprect,TRANSPARENCY_PEN,15);

			i--;
		} while (i >= 0);
		
	}

}


VIDEO_UPDATE( 1942 )
{
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	draw_sprites(bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
}
