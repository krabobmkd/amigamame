/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

#include "amiga106_video.h"

#include <proto/intuition.h>
#include <proto/Picasso96.h>

extern "C" {
    #include <libraries/Picasso96.h>
    #include <intuition/screens.h>

}

extern "C" {
    // from mame
    #include <mamecore.h>
    #include <video.h>

}

#include <stdio.h>
#include <stdlib.h>

extern struct Library *P96Base;

Display_P96::Display_P96(unsigned int width,
            unsigned int height) : MameDisplay() , _pWindow(NULL)
{
    struct Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))) return;

    _pWindow = (Window *)OpenWindowTags(NULL,
        WA_Left,120,
        WA_Top,120,
        WA_Width,  width,
        WA_Height, height,
    //    WA_MaxWidth,  WIDTH_SUPER,
    //    WA_MaxHeight, HEIGHT_SUPER,
        WA_IDCMP, IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_RAWKEY |
            IDCMP_NEWSIZE | IDCMP_INTUITICKS | IDCMP_CLOSEWINDOW,
        WA_Flags, WFLG_SIZEGADGET | WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM |
            WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE |
            /*WFLG_SUPER_BITMAP |*/ WFLG_GIMMEZEROZERO /*| WFLG_NOCAREREFRESH*/,
     //   WA_Gadgets, &(SideGad),
        WA_Title,(ULONG) "Mame", /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,
       // WA_SuperBitMap, bigBitMap,
        TAG_DONE
        );
    UnlockPubScreen(NULL,pWbScreen);
    if( _pWindow == NULL ) return;
    _pUserPort = _pWindow->UserPort;

}
Display_P96::~Display_P96()
{
    printf(" **** ~Display_P96 ***** %08x",(int)_pWindow);
    if(_pWindow){ CloseWindow(_pWindow); }
}
void Display_P96::draw(_mame_display *pmame_display)
{
    struct _mame_bitmap *bitmap = pmame_display->game_bitmap;
    if(!P96Base ||!_pWindow || !bitmap) return;

//    static int tt=1;
//    if(tt==1)
//    {
//        tt=0;
//        printf("bitmap->rowbytes:%d\n",bitmap->rowbytes);
//    }

    WORD *p = (WORD*)bitmap->base;
    p[160] = 0xffff;
    p[161] = 0xffff;
    p[160+4*bitmap->rowbytes] = 0xffff;
    p[160+5*bitmap->rowbytes] = 0xffff;

    RenderInfo ri;
    ri.Memory = (APTR)bitmap->base,
    ri.BytesPerRow = (WORD)bitmap->rowbytes;
    ri.pad = 0;
    ri.RGBFormat = RGBFB_R5G5B5;// RGBFB_R5G6B5;

//	APTR			Memory;
//	WORD			BytesPerRow;
//	WORD			pad;
//	RGBFTYPE		RGBFormat;
//    void p96WritePixelArray(struct RenderInfo *ri, UWORD SrcX, UWORD SrcY, struct RastPort *rp, UWORD DestX, UWORD DestY, UWORD SizeX, UWORD SizeY);

    p96WritePixelArray(&ri,
        pmame_display->game_visible_area.min_x,
        pmame_display->game_visible_area.min_y, // SrcX/Y
        _pWindow->RPort,
        0,0, // dest X/Y
        bitmap->width,
        bitmap->height
    );

}

