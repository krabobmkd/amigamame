
#include "lconfig.h"

extern "C" {
    #include "osdepend.h"
    #include "mamecore.h"
    #include "mame.h"
    #include "driver.h"
    #include "input.h"
}
#ifdef __linux__
#include <sys/time.h>
#endif
void StartGame(int idriver)
{

    if(!drivers[idriver]) return;
    MameConfig &conf = getMainConfig();

    conf.applyToMameOptions(options);

    // - - games can have has rotation + we add our explicit rotation ,
    // and we must rotate the tab setting interface the other way to compensate.
    // silly isn't it ?
    int gameorientation = drivers[idriver]->flags & ORIENTATION_MASK;

    UINT32 uiapplied = 0;
//    int uiorientation = ROT0;
    if(gameorientation == ROT90) uiapplied=ROT270;
    if(gameorientation == ROT270) uiapplied=ROT90;
    if(gameorientation == ROT180) uiapplied=ROT180;
    if(gameorientation == ORIENTATION_FLIP_X) uiapplied =ORIENTATION_FLIP_X;
    if(gameorientation == ORIENTATION_FLIP_Y) uiapplied =ORIENTATION_FLIP_Y;

    // done here
//    if(drivers[idriver])
        options.ui_orientation = uiapplied &ORIENTATION_MASK;  // uiorientation;
options.skip_disclaimer = 1;
options.skip_gameinfo = 1;
options.skip_warnings = 1;

    /* Clear the zip filename caches. */

    osd_set_mastervolume(0);
//    printf("before run_game\n");

    run_game(idriver);
//    printf("after run_game\n");

 //re?   unzip_cache_clear();
    mame_pause(0);// remove pause that could be set while quiting previous game.

//todo ?
//  if(options.playback)
//    osd_fclose(options.playback);

//  if(options.record)
//    osd_fclose(options.record);
}


int main(int argc, char **argv)
{
   // if(argc<2) return 0;

    getMainConfig().init(argc,argv);
//    getMainConfig().load();

    int idriver = -1;
    // test if just "mame romname".
    int itest = getMainConfig().driverIndex().index(
    //"arkretrn"
    "mslug"
    );
    if(itest>0) idriver= itest;


    //  if game was explicit, no GUI
    if(idriver>0)
    {
        getMainConfig().setActiveDriver(idriver);
        StartGame(idriver);
        return 0;
    }



    return 0;
}
int osd_init()
{
  return(0);
}

int osd_create_display(const osd_create_params *params, UINT32 *rgb_components)
{
    return 0;
}
void osd_close_display(void)
{

}
int osd_skip_this_frame(void)
{
    return 0;
}
void osd_update_video_and_audio(struct _mame_display *display)
{
    static int nbframe = 0;
    nbframe++;
    if(nbframe==60*12) mame_schedule_exit();
}
mame_bitmap *osd_override_snapshot(mame_bitmap *bitmap, rectangle *bounds)
{
    return bitmap;
}
const char *osd_get_fps_text(const performance_info *performance)
{
    return "";
}
extern "C" { int amigamame_audio_forcemono=0;  }
int osd_start_audio_stream(int stereo)
{
    return 800;
}
int osd_update_audio_stream(INT16 *buffer)
{
    return 800;
}
void osd_stop_audio_stream(void)
{

}
void osd_set_mastervolume(int attenuation)
{

}
int osd_get_mastervolume(void)
{
    return 0;
}
void osd_sound_enable(int enable)
{

}

const os_code_info *osd_get_code_list(void)
{
    static os_code_info l[]={
        {"A",32,KEYCODE_A},
        {NULL,0,0},
    };
    return &l[0];
}
INT32 osd_get_code_value(os_code oscode)
{
    return 0;
}
int osd_readkey_unicode(int flush)
{
    return 0;
}
void osd_customize_inputport_list(input_port_default_entry *defaults)
{

}
int osd_joystick_needs_calibration(void)
{
    return 0;
}
void osd_joystick_start_calibration(void)
{

}
const char *osd_joystick_calibrate_next(void)
{
    return NULL;
}
void osd_joystick_calibrate(void)
{

}
void osd_joystick_end_calibration(void)
{

}



cycles_t osd_cycles(void)
{
#ifdef __linux__
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000000L + tv.tv_usec;
    #else
    static cycles_t o=0;
    o+= 5465;
return o;
    #endif
}
cycles_t osd_cycles_per_second(void)
{
    return 1000000LL;
}
cycles_t osd_profiling_ticks(void)
{
    return osd_cycles();
}


void *osd_alloc_executable(size_t size)
{
    return NULL;
}
void osd_free_executable(void *ptr)
{

}
// int osd_display_loading_rom_message(const char *name,rom_load_data *romdata)
// {
//     return 0;
// }
int osd_is_bad_read_ptr(const void *ptr, size_t size)
{
    return 0;
}


void osd_update_boot_progress(int per256, int enm)
{

}
