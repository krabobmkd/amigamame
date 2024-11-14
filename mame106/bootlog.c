#include "bootlog.h"
#include "osdepend.h"
#include "mame.h"
#include "romload.h"
static UINT16 romstotal=0;
static UINT16 romsloaded=0;

int osd_display_loading_rom_message(const char *name,rom_load_data *romdata)
{

    romstotal = (UINT16)romdata->romstotal;
    romsloaded = (UINT16)romdata->romsloaded;

    bootlog_setprogress(ebRomLoad);
    // 9B [N] 46 "\x1b[\x9b\x01m"
   // if(romdata->romsloaded>1) printf("\r");
//    loginfo(0,"load rom: %d/%d\n",romdata->romsloaded,romdata->romstotal);
   return(0);
}
static UINT32 gfxtotal=0;
static UINT32 gfxdecoded=0;
void bootlog_setvideodecode(int i, int nbgfx)
{
    gfxtotal = (UINT32)nbgfx;
    gfxdecoded = (UINT32)i;
     bootlog_setprogress(eSoundVideo);
}

void bootlog_setprogress(eProgress e)
{
    if(e == ebStart )
    {
        romstotal = gfxtotal = 1;
        romsloaded = gfxdecoded = 0;
    }
    if((int)e>= (int)eProgressEnd)
    {   // case where init finished
        osd_update_boot_progress(256,(int)eProgressEnd);
        return;
    }
    // enum to per256.
    int romloadweight=80;
    int initvidweight=64;
    int restweigth = 256-(romloadweight+initvidweight);

    int divider = (int)eProgressEnd -2;
    if(!options.cheat) divider--;

    int enumpercent = ((int)e * restweigth) / divider;
    if(romstotal>0)
    {
        enumpercent += (romsloaded * romloadweight) / romstotal;
    }
    if(gfxdecoded>0)
    {
        enumpercent += (gfxdecoded * initvidweight) / gfxtotal;
    }
    if(enumpercent>256) enumpercent=256;

    osd_update_boot_progress(enumpercent,(int)e);
}
