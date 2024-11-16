/**************************************************************************
 *
 * Copyright (C) 2024 Vic Krb Ferry
 *
 *************************************************************************/

// Amiga includes, proto manages __cplusplus__
//#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/graphics.h>
//#include <proto/cybergraphics.h>
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
    #include "memory.h"
}
#include "amiga106_inputs.h"
#include "amiga106_config.h"
#include "amiga106_video.h"
#include "amiga106_video_intuition.h"

/** some abstact display management */

#include <stdio.h>

AbstractDisplay::AbstractDisplay() {
}
AbstractDisplay::~AbstractDisplay(){}

// - - - - from driver.h
/* is the video hardware raser or vector base? */
#define	VIDEO_TYPE_RASTER				0x0000
#define	VIDEO_TYPE_VECTOR				0x0001

/* should VIDEO_UPDATE by called at the start of VBLANK or at the end? */
#define	VIDEO_UPDATE_BEFORE_VBLANK		0x0000
#define	VIDEO_UPDATE_AFTER_VBLANK		0x0002

/* set this to use a direct RGB bitmap rather than a palettized bitmap */
#define VIDEO_RGB_DIRECT	 			0x0004

//static void waitsec(int s)
//{
//    for(int j=0;j<s;j++)
//    for(int i=0;i<50;i++)
//    {
//        WaitTOF();
//    }
//}
AbstractDisplay *g_pMameDisplay=NULL;

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
static ULONG FrameCounterUpdate=0;
static INT64 FrameCounter=0;
INT64 StartTime = 0;
ULONG GetStartTime=0;
ULONG TotalFrameCounter = 0;


//ledBitmap _ledBitmap(3,4); // nbleds, ledwidth
bool SwitchWindowFullscreen()
{
    if(!g_pMameDisplay) return false;
    bool didit = g_pMameDisplay->switchFullscreen();
    if(!didit)
    {   // fail, try to get back.
        didit = g_pMameDisplay->switchFullscreen();
    }
    return didit;
}
void ResetWatchTimer()
{
    FrameCounterUpdate = 0;
    FrameCounter = 0;
    StartTime = 0;
    GetStartTime = 1;
}




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


cycles_t gameCyclePerFrame=-1;
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
int osd_create_display(const _osd_create_params *pparams, UINT32 *rgb_components)
{
    if(g_pMameDisplay) osd_close_display();
    if(!pparams || !Machine || !Machine->gamedrv) return 1; // fail

    MameConfig &mainConfig = getMainConfig();
    MameConfig::Display &config = mainConfig.display();

    MameConfig::Display_PerScreenMode &screenModeConf = config.getActiveMode();

    if((pparams->video_attributes &VIDEO_TYPE_VECTOR)==0) // means not vector,= bitmap
    {
        // keep the 3 orientation bits
        AbstractDisplay::params params;


        // if just flip and no rot at start, special case
        UINT32 startflags = Machine->gamedrv->flags & 7;
        UINT32 reportFlags = 0;  // swaps that are not rotation must be reported
        UINT32 actualRotation = 0;
        if(startflags == ORIENTATION_FLIP_X || startflags == ORIENTATION_FLIP_Y )
        {
            reportFlags = startflags;
            actualRotation = 0;
        }
        params._flags = shiftRotationBits(Machine->gamedrv->flags,(int)screenModeConf._rotateMode);
        params._flags ^= reportFlags;
        params._flags &= ORIENTATION_MASK;

        if(screenModeConf._ScreenModeChoice == MameConfig::ScreenModeChoice::Choose)
            params._forcedModeID = (ULONG) screenModeConf._modeid;
         else  params._forcedModeID = ~0; // undefined.

        params._width = pparams->width;
        params._height = pparams->height;
        params._colorsIndexLength = pparams->colors;
        params._video_attributes = pparams->video_attributes;
        params._driverDepth = pparams->depth;
        // this will decide video implemntation against available hardware and config.
        g_pMameDisplay = new IntuitionDisplay();

        if(config._flags & CONFDISPLAYFLAGS_ONWORKBENCH )
            params._flags |= DISPFLAG_STARTWITHWINDOW;

        // these 2 are exclusive:
        if(config._buffering == MameConfig::ScreenBufferMode::TripleBufferCSB )
             params._flags |= DISPFLAG_USETRIPLEBUFFER;
        if(config._buffering == MameConfig::ScreenBufferMode::DoubleBufferSVP )
             params._flags |= DISPFLAG_USEHEIGHTBUFFER;

        if(config._drawEngine == MameConfig::DrawEngine::CgxScalePixelArray)
                params._flags |= DISPFLAG_USESCALEPIXARRAY;

        bool screenok = g_pMameDisplay->open(params);
        if(!screenok) {
            loginfo(2,"couldn't open screen.");
            return 1; // fail.
        }
    } // end if bitmap

    if(!g_pMameDisplay || !g_pMameDisplay->good())
    {
        //
        loginfo(2,"couldn't find a graphic mode.");
        return 1; // fail.
    }
    // for drivers with RGB modes, we have to describe our pixel format
    if((pparams->video_attributes & VIDEO_RGB_DIRECT) && (rgb_components))
    {
        g_pMameDisplay->init_rgb_components(rgb_components);
    }


    AllocInputs(); // input object depends of screen or window.

    TotalFrameCounter = 0;
    FrameCounterUpdate = 0;
    FrameCounter = 0;
    StartTime = 0;
    GetStartTime = 1;

    MameConfig::Misc &configMisc = getMainConfig().misc();

 //   printf("osd_create_display fps:%f\n",pparams->fps);

    float speedlimit = configMisc._speedlimit;
    if(speedlimit<80.0f ) speedlimit = 80.0f;
    else if(speedlimit>125.0f) speedlimit = 125.0f;
    speedlimit = 100.0f/speedlimit;

    gameCyclePerFrame =  (cycles_t) (1000000.0 * speedlimit / pparams->fps );
//    printf("gameCyclePerFrame fps:%d\n",(int)gameCyclePerFrame);
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
    {
        // no more 64b division !
        cycles_t cyclethatShouldBeNow = StartTime + (gameCyclePerFrame * FrameCounter );
        cycles_t cnow = osd_cycles();

        // if OS paused (window moving, menu bt, intuition hogs, reset timer)
        //if(FrameCounter+(igamefps>>1)<framesThatShouldbeNow)
        if(cnow>(cyclethatShouldBeNow+(1000000LL>>1))) // + half a second.
        {
            ResetWatchTimer();
        }

        while(cnow<cyclethatShouldBeNow)
        {
            // some functions that knowns how to actually pass priority to other tasks.
            // Graphics/WaitTOF() does but is 50 or 60Hz, and will wait a random time on first call.
            // Graphics/WaitBOVP(vp) should fit exact screen frame but is commonly bad implemented and hogs cpu.
            g_pMameDisplay->WaitFrame();

            cnow = osd_cycles();
        }
    }

    g_pMameDisplay->draw(display);

    MsgPort *userport = g_pMameDisplay->userPort();
    if(userport) UpdateInputs(userport);

    // - - - -
    FrameCounterUpdate++;
    FrameCounter++;

}
// for progressbar pass
static void checkExitSimple(MsgPort *userport )
{
  struct IntuiMessage *im;
    int doExit=0;
    while((im = (struct IntuiMessage *) GetMsg(userport)))
    {
        ULONG imclass = im->Class;
        UWORD imcode  = im->Code;
        UWORD imqual  = im->Qualifier;

        ReplyMsg((struct Message *) im); // the faster the better.

        switch(imclass)
        {
            case IDCMP_RAWKEY:
            if(!(imqual & IEQUALIFIER_REPEAT) )
            {
              if( (imcode & 0x037f)==0x45) doExit = 1; // esc key.
            }
            break;
        case IDCMP_CLOSEWINDOW:
            doExit = 1;
        break;
         default:
            break;
        }
    }
    if(doExit) fatalerror("User cancelled."); // setjmp to end of mame loop

}

// - -  update screen before boot.
void osd_update_boot_progress(int per256, int enm)
{
    if(!g_pMameDisplay) return;
    g_pMameDisplay->drawProgress(per256,enm);

    // like main thread is under the rungame init ,
    // galme not started yet,
    // but test possible exit.
    MsgPort *userport = g_pMameDisplay->userPort();
    if(!userport) return ;
    checkExitSimple(userport);

}

extern ULONG _bootframeskip;
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
    if(TotalFrameCounter< _bootframeskip && TotalFrameCounter>0) return 1;
    TotalFrameCounter++;
    MameConfig::Display &config = getMainConfig().display();
    if(config._flags & CONFDISPLAYFLAGS_FRAMESKIP) return FrameCounterUpdate & 1;
    return 0 ;
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
    snprintf(perfo_line,27,"speed:%.01f%% fps:%.03f",performance->game_speed_percent,
             performance->frames_per_second);
    perfo_line[27]=0;
    return perfo_line;
}
// memory.c 2569
// #define WRITEBYTE8(name,spacenum)
//  WRITEBYTE8(program_write_byte_8,     ADDRESS_SPACE_PROGRAM)
//  WRITEBYTE8(data_write_byte_8,     ADDRESS_SPACE_DATA)

//#define ADDRESS_SPACES			3						/* maximum number of address spaces */
//#define ADDRESS_SPACE_PROGRAM	0						/* program address space */
//#define ADDRESS_SPACE_DATA		1						/* data address space */
//#define ADDRESS_SPACE_IO		2						/* I/O address space */
//struct _address_space
//{
//	offs_t				addrmask;			/* address mask */
//	UINT8 *				readlookup;			/* read table lookup */
//	UINT8 *				writelookup;		/* write table lookup */
//	handler_data *		readhandlers;		/* read handlers */
//	handler_data *		writehandlers;		/* write handlers */
//	data_accessors *	accessors;			/* pointers to the data access handlers */
//};
//extern "C" {
//void mywrite8(UINT32 adress,UINT8 data)
//{
//    struct _address_space &space = [ADDRESS_SPACE_PROGRAM];
//    /* perform lookup */
//    // PERFORM_LOOKUP(lookup,space,extraand)
//    // PERFORM_LOOKUP(writelookup,active_address_space[spacenum],~0);
//	address &= space.addrmask & extraand;
//	entry = space.lookup[LEVEL1_INDEX(address)];
//	if (entry >= SUBTABLE_BASE)
//		entry = space.lookup[LEVEL2_INDEX(entry,address)];

//}

//}
