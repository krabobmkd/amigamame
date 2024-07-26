/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

// Amiga includes, proto manages __cplusplus__
//#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>

#include <proto/utility.h>

extern "C" {
    #include <exec/types.h>
    #include <exec/memory.h>

    #include <graphics/gfxbase.h>
    #include <graphics/rastport.h>
    #include <graphics/modeid.h>

    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <cybergraphx/cybergraphics.h>

}
#include "intuiuncollide.h"
// from mame
extern "C" {
    #include "osdepend.h"
    #include "video.h"
    // for logerror
    #include "mame.h"
    // for orientation flags
    #include "driver.h"
}
#include "amiga106_inputs.h"
#include "amiga106_config.h"
#include "amiga106_video.h"
#include "amiga106_video_cgx.h"

/** some abstact display management */

#include <stdio.h>

extern struct Library *CyberGfxBase;
extern struct Library *P96Base;

AmigaDisplay::AmigaDisplay() {
}
AmigaDisplay::~AmigaDisplay(){}

// - - - - from driver.h
/* is the video hardware raser or vector base? */
#define	VIDEO_TYPE_RASTER				0x0000
#define	VIDEO_TYPE_VECTOR				0x0001

/* should VIDEO_UPDATE by called at the start of VBLANK or at the end? */
#define	VIDEO_UPDATE_BEFORE_VBLANK		0x0000
#define	VIDEO_UPDATE_AFTER_VBLANK		0x0002

/* set this to use a direct RGB bitmap rather than a palettized bitmap */
#define VIDEO_RGB_DIRECT	 			0x0004

/*
  Create a display screen, or window, of the given dimensions (or larger). It is
  acceptable to create a smaller display if necessary, in that case the user must
  have a way to move the visibility window around.

  The params contains all the information the
  Attributes are the ones defined in driver.h, they can be used to perform
  optimizations, e.g. dirty rectangle handling if the game supports it, or faster
  blitting routines with fixed palette if the game doesn't change the palette at
  run time. The VIDEO_PIXEL_ASPECT_RATIO flags should be honored to produce a
  display of correct proportions.
  Orientation is the screen orientation (as defined in driver.h) which will be done
  by the core. This can be used to select thinner screen modes for vertical games
  (ORIENTATION_SWAP_XY set), or even to ask the user to rotate the monitor if it's
  a pivot model. Note that the OS dependent code must NOT perform any rotation,
  this is done entirely in the core.
  Depth can be 8 or 16 for palettized modes, meaning that the core will store in the
  bitmaps logical pens which will have to be remapped through a palette at blit time,
  and 15 or 32 for direct mapped modes, meaning that the bitmaps will contain RGB
  triplets (555 or 888). For direct mapped modes, the VIDEO_RGB_DIRECT flag is set
  in the attributes field.

  Returns 0 on success.
*/
//static void waitsec(int s)
//{
//    for(int j=0;j<s;j++)
//    for(int i=0;i<50;i++)
//    {
//        WaitTOF();
//    }
//}
AmigaDisplay *g_pMameDisplay=NULL;

//struct ledBitmap {
//    ledBitmap(int nbleds,int ledwidth) {
//        _ledwidth = ledwidth;
//        _ledmarge = ledwidth>>1;
//        _width = (nbleds+1)*_ledmarge + ledwidth*nbleds;
//        _height = 2*_ledmarge + ledwidth;
//        _nbleds = nbleds;
//        int mem = _width*_height;
//        _bm.reserve(mem);
//        _bm.resize(mem,1); // fill with BG
//        //
//    }
//    std::vector<int> rgbpalette={0,0x00505050,
//                0x00d00000,0x0000d000,0x000000d0};
//    void update(int ledbits) {
//        if(_bm.size()==0) return;
//        // 0black, 1bg, red,green,blue
//        int bmofs = _ledmarge*_width+_ledmarge;
//        for(int i=0;i<_nbleds;i++)
//        {
//            int ledcolor = (ledbits&1)?(2+i):0;
//            int bmofs2 = bmofs;
//            for(int y=0;y<_ledwidth;y++)
//            {
//                for(int x=0;x<_ledwidth;x++)
//                {
//                    _bm[bmofs2+x] = ledcolor;
//                }
//                bmofs2 +=_width;
//            }
//            bmofs +=_ledwidth+_ledmarge;
//            ledbits>>=1;
//        }
//    }
//    int _nbleds;
//    int _ledwidth;
//    int _ledmarge;

//};

//ledBitmap _ledBitmap(3,4); // nbleds, ledwidth
void SwitchWindowFullscreen()
{
    if(!g_pMameDisplay) return;
    g_pMameDisplay->switchFullscreen();
}
static ULONG FrameCounterUpdate=0;
static INT64 FrameCounter=0;
//
//static ULONG FastFrameTarget=0;
//static ULONG FastFrameStep=0; // if 0, slow, if >0 emu is faster than original game.
//static ULONG FastFrameCounter=0;
INT64 StartTime = 0;
ULONG GetStartTime=0;

ULONG UseBrakes = 0,UseFrameskip=0;


static ULONG shiftRotationBits(ULONG orientation, int rotationShift)
{
    static const int rots[4]={ ROT0,ROT90,ROT180,ROT270};
    orientation &= ORIENTATION_MASK;
    int i = 0;
    if(orientation ==ROT90) i=1;
     if(orientation ==ROT180) i=2;
      if(orientation ==ROT270) i=3;
    i = (i+rotationShift)&3;
    return rots[i];
}

int osd_create_display(const _osd_create_params *pparams, UINT32 *rgb_components)
{      
    if(g_pMameDisplay) osd_close_display();
    if(!pparams || !Machine || !Machine->gamedrv) return 1; // fail

    MameConfig::Display &config = getMainConfig().display();
    std::string screenId;
    int vattribs;
    MameConfig::getDriverScreenModestring(Machine->gamedrv, screenId,vattribs);

    MameConfig::Display_PerScreenMode &screenModeConf = config._perScreenMode[screenId];

    if((pparams->video_attributes &VIDEO_TYPE_VECTOR)==0) // means not vector,= bitmap
    {
        // keep the 3 orientation bits
        AmigaDisplay::params params;
        params._flags = shiftRotationBits(Machine->gamedrv->flags,(int)screenModeConf._rotateMode);

        if(screenModeConf._ScreenModeChoice == MameConfig::ScreenModeChoice::Choose)
            params._forcedModeID = (ULONG) screenModeConf._modeid;
         else  params._forcedModeID = ~0; // undefined.

        params._width = pparams->width;
        params._height = pparams->height;
        params._colorsIndexLength = pparams->colors;
        params._video_attributes = pparams->video_attributes;
        params._driverDepth = pparams->depth;
        //try RTG  drivers first:
        // Display_CGX try RTG then AGA.
        g_pMameDisplay = new Display_CGX();
        bool screenok = g_pMameDisplay->open(params);
        if(!screenok) {
            logerror("couldn't open screen.");
            return 1; // fail.
        }
    } // end if bitmap

//    if(P96Base)
//    {
//        g_pMameDisplay = new Display_P96(params->width, params->height);
//    }
    if(!g_pMameDisplay || !g_pMameDisplay->good())
    {
        //
        logerror("couldn't find a graphic mode.");
        return 1; // fail.
    }


    AllocInputs(); // input object depends of screen or window.

    FrameCounterUpdate = 0;
    FrameCounter = 0;
    StartTime = 0;
    GetStartTime = 1;
    UseBrakes = 0;
    UseFrameskip = 0;
    return 0; // success

}
void osd_close_display(void)
{
    FreeInputs();
    if(g_pMameDisplay) {
        delete g_pMameDisplay;
        g_pMameDisplay = NULL;
    }
}


/*
  Update video and audio. game_bitmap contains the game display, while
  debug_bitmap an image of the debugger window (if the debugger is active; NULL
  otherwise). They can be shown one at a time, or in two separate windows,
  depending on the OS limitations. If only one is shown, the user must be able
  to toggle between the two by pressing IPT_UI_TOGGLE_DEBUG; moreover,
  osd_debugger_focus() will be used by the core to force the display of a
  specific bitmap, e.g. the debugger one when the debugger becomes active.

  leds_status is a bitmask of lit LEDs, usually player start lamps. They can be
  simulated using the keyboard LEDs, or in other ways e.g. by placing graphics
  on the window title bar.
*/

void osd_update_video_and_audio(struct _mame_display *display)
{


    if(GetStartTime)
    {
        StartTime = osd_cycles();
        GetStartTime = 0;
        FrameCounter = 0;
    }
// printf("osd_update_video_and_audio\n");
    if(!g_pMameDisplay) return;

    // apply eventual hard beam waiting (if too fast) just before draw.
    int igamefps = (int) display->game_refresh_rate;


   // if(UseBrakes>0)
    {
        INT64 framesThatShouldbeNow = ((osd_cycles() - StartTime)*igamefps)/osd_cycles_per_second();
        while(framesThatShouldbeNow<FrameCounter)
        {
            // something known to actually does pass priority to system
            // and waits between 1/50 hz or less.
            g_pMameDisplay->WaitFrame();

            framesThatShouldbeNow = ((osd_cycles() - StartTime)*igamefps)/osd_cycles_per_second();
        }
    }

    g_pMameDisplay->draw(display);

    MsgPort *userport = g_pMameDisplay->userPort();
    if(userport) UpdateInputs(userport);

    // - - - - -auto fps management, analysis

//    if(FrameCounterUpdate>=igamefps>>2)
//    {
//        FrameCounterUpdate -=igamefps>>2;
//        // performance are already computed by mame.
//        const performance_info *perfs = mame_get_performance_info();
//        if(perfs)
//        {
//            int icurrentfps = (int)perfs->frames_per_second;

//            // manage when too fast (uae jit)
////            UseBrakes = (int)( icurrentfps+1 > igamefps);
////            if(!UseBrakes && icurrentfps<igamefps-1)
////            {
////                // most amigas...
////                UseFrameskip = 1;
////            }
//        }
//    }

    // - - - -
    FrameCounterUpdate++;
    FrameCounter++;

}


/*
  osd_skip_this_frame() must return 0 if the current frame will be displayed.
  This can be used by drivers to skip cpu intensive processing for skipped
  frames, so the function must return a consistent result throughout the
  current frame. The function MUST NOT check timers and dynamically determine
  whether to display the frame: such calculations must be done in
  osd_update_video_and_audio(), and they must affect the FOLLOWING frames, not
  the current one. At the end of osd_update_video_and_audio(), the code must
  already know exactly whether the next frame will be skipped or not.
*/
int osd_skip_this_frame(void)
{
    return 0; //FrameCounterUpdate & 1;
}

/*
  Provides a hook to allow the OSD system to override processing of a
  snapshot.  This function will either return a new bitmap, for which the
  caller is responsible for freeing.
*/
mame_bitmap *osd_override_snapshot(mame_bitmap *bitmap, rectangle *bounds)
{
    return NULL;
}

/*
  Returns a pointer to the text to display when the FPS display is toggled.
  This normally includes information about the frameskip, FPS, and percentage
  of full game speed.
*/
static char perfo_line[28];
const char *osd_get_fps_text(const performance_info *performance)
{
    snprintf(perfo_line,27,"speed:%.01f fps:%.03f",performance->game_speed_percent,
             performance->frames_per_second);
    perfo_line[27]=0;
    return perfo_line;
}
