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
    printf(" ***** StartGame:%d\n",idriver);
    if(idriver<0)
    {
      //logerror no driver
      return;
    }

    conf.applyToMameOptions(options);
    int gameorientation = drivers[idriver]->flags & ORIENTATION_MASK;
    int uiorientation = ROT0;
    if(gameorientation == ROT90) uiorientation=ROT270;
    if(gameorientation == ROT270) uiorientation=ROT90;
    if(gameorientation == ROT180) uiorientation=ROT180;
    // done here
    if(drivers[idriver])
        options.ui_orientation = uiorientation;

    /* Clear the zip filename caches. */

    osd_set_mastervolume(0);
    printf("before run_game\n");

    run_game(idriver);
    printf("after run_game\n");

    unzip_cache_clear();
    mame_pause(0);// remove pause that could be set while quiting previous game.


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

/* called while loading ROMs. It is called a last time with name == 0 to signal */
/* that the ROM loading process is finished. */
/* return non-zero to abort loading */
int osd_display_loading_rom_message(const char *name,rom_load_data *romdata)
{
  return(0);
}
// -  - - - -

/* called when the game is paused/unpaused, so the OS dependant code can do special */
/* things like changing the title bar or darkening the display. */
/* Note that the OS dependant code must NOT stop processing input, since the user */
/* interface is still active while the game is paused. */
void osd_pause(int paused)
{
    printf("osd_pause:%d\n",paused);
}



