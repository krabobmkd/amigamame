/**************************************************************************
 *
 * Copyright (C) 1999 Mats Eirik Hansen (mats.hansen@triumph.no)
 *
 * $Id: amiga.c,v 1.1 1999/04/28 18:50:15 meh Exp meh $
 *
 * $Log: amiga.c,v $
 * Revision 1.1  1999/04/28 18:50:15  meh
 * Initial revision
 *
 *
 *************************************************************************/
#ifndef __stdargs
// shut up clangd.
#define __stdargs
#endif

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>


// mame includes
extern "C" {
    #include "mamecore.h"
    #include "mame.h"
    #include "driver.h"
    #include "osdepend.h"
    #include "unzip.h"
}

#include <stdio.h>

#include "amiga106_config.h"

void unzip_cache_clear();

void StartGame(void)
{
    MameConfig &conf = getMainConfig();
    int idriver = conf.activeDriver();
//    printf(" ***** StartGame:%d\n",idriver);
    if(idriver<0)
    {
      //logerror no driver
      return;
    }
    if(!drivers[idriver]) return;

    conf.applyToMameOptions(options,drivers[idriver]);

    // - - games can have has rotation + we add our explicit rotation ,
    // and we must rotate the tab setting interface the other way to compensate.
    // silly isn't it ?
    int gameorientation = drivers[idriver]->flags & ORIENTATION_MASK;

    static const UBYTE invrsetransform[8]={
        0,  // no transform, no transform
        ORIENTATION_FLIP_X,  // flipx
        ORIENTATION_FLIP_Y,  // flipy
        ROT180,  // ROT180

        ORIENTATION_SWAP_XY, // no game known, but obvious.
        ROT270, // ROT90
        ROT90, // ROT270
        ORIENTATION_FLIP_X|ORIENTATION_FLIP_Y|ORIENTATION_SWAP_XY,
    }; // so basically its 0,1,2,3,4,6,5,7 with just 5 6 inverted
    UINT32 uiapplied = (UINT32)invrsetransform[gameorientation];

    // done here
//    if(drivers[idriver])
        options.ui_orientation = uiapplied; // &ORIENTATION_MASK;  // uiorientation;

    /* Clear the zip filename caches. */

    osd_set_mastervolume(0);
//    printf("before run_game\n");

    run_game(idriver);
//    printf("after run_game\n");


    unzip_cache_clear();
    mame_pause(0);// remove pause that could be set while quiting previous game.
    // when fatal error need that
    osd_close_display();
//todo ?
//  if(options.playback)
//    osd_fclose(options.playback);
  
//  if(options.record)
//    osd_fclose(options.record);
}

int osd_init()
{
  return(0);
}

void osd_exit()
{
}


// -  - - - -

/* called when the game is paused/unpaused, so the OS dependant code can do special */
/* things like changing the title bar or darkening the display. */
/* Note that the OS dependant code must NOT stop processing input, since the user */
/* interface is still active while the game is paused. */
void osd_pause(int paused)
{
//    printf("osd_pause:%d\n",paused);
}



