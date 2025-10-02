/**************************************************************************
 *
 * Copyright (C) 2025 Krb
 *
 *
 *
 *************************************************************************/

/*
 krb note:
 mame's input.h and amiga intuition.h collides because of KEYCODE_XXX defines.
 let's make a point not including both in the same files.
*/
// from mame
extern "C" {
    #include "osdepend.h"
    #include "driver.h"
    #include "unzip.h"

}
//#define STATCPUINSTR 1
#ifdef STATCPUINSTR
extern "C" {
    #include "cpustats.h"
}
#endif
#include <stdio.h>
#include <strings.h>
//#include <locale.h>
//#include <locale>
//#include <clocale>
//#include <iostream>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/timer.h>
#include <proto/misc.h>

#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/keymap.h>
#include "version.h"

extern "C" {
    // all C amiga stuffs should be included from C++ in extern "C" paragraph
    #include <cybergraphx/cybergraphics.h>
    #include <resources/misc.h>
    #include <workbench/startup.h>

    #include "asmmacros.h"
}

#include "version.h"
#include "video.h"

#include "amiga_locale.h"
#include "amiga106_config.h"
#include "amiga106_inputs.h"
#include "gui_mui.h"

#define MIN_STACK (14*1024)

#define ITEM_NEW    0
#define ITEM_SAVE_ILBM  1
#define ITEM_ABOUT    3
#define ITEM_QUIT   5
#define NUM_ITEMS   6

void     StartGame(void);  /* In amiga/amiga.c. */


#ifdef POWERUP
struct _game_driver **Drivers;
#endif

LONG        MenuSelect[NUM_ITEMS];


extern "C" {
#ifdef USE_OWN_DOSBASE
struct DosLibrary   *DOSBase    = NULL;
#endif
struct GfxBase      *GfxBase    = NULL;
struct IntuitionBase  *IntuitionBase  = NULL;
struct Library      *AslBase    = NULL;
struct Library      *LayersBase    = NULL;
struct Library      *KeymapBase   = NULL;
struct Library      *UtilityBase  = NULL;
struct Library      *CyberGfxBase = NULL;
struct Library      *IconBase = NULL;
// will only work on Amiga Classic and must be optional. Library opened by OpenResource(), not OpenLibrary
struct Library    *MiscBase=NULL;

struct DiskObject *AppDiskObject = NULL;
//struct Library      *P96Base = NULL;
 int verbose=0;
}

struct FileRequester  *FileRequester  = NULL;

static struct StackSwapStruct StackSwapStruct;

void initTimers();
void closeTimers();

int libs_init1()
{
#ifdef USE_OWN_DOSBASE
    if(!(DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 36))) return(1);
#endif
    if(!(GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39)))
    {
        printf("need at least OS3.0\n");
        return(1);
    }
    if(!(IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39)))
    {
        return(1);
    }
    if(!(UtilityBase = OpenLibrary("utility.library",0))) return(1);
    if(!(KeymapBase = OpenLibrary("keymap.library", 36))) return(1);
    if(!(LayersBase = OpenLibrary("layers.library", 39))) return(1);
    if(!(IconBase = OpenLibrary("icon.library", 39))) return(1);
    return(0);
}
int libs_init2()
{
    if(verbose) printf("try open asl\n");
    if(!(AslBase = OpenLibrary("asl.library", 36))) return(1);
    if(verbose) printf("try open cybergraphics\n");
//    InitLowLevelLib();
    // optional:
    CyberGfxBase  = OpenLibrary("cybergraphucs.library", 1);
    if(verbose && (CyberGfxBase == NULL)) printf("can't open cybergraphics\n");
//    P96Base  = OpenLibrary("Picasso96API.library", 0);

    // mui is done elsewhere.

    // also, optional, used for parallel pads:
    // "There is no CloseResource()"
    if(verbose) printf("try open misc\n");
    MiscBase = (struct Library *)OpenResource(MISCNAME);
    if(verbose) printf("init Timers\n");
    initTimers();
    if(verbose) printf("init libs ok\n");
    return(0);
}

extern "C" {
void mameExitCleanCtrlC(void);
}
// exit code that is called from any exit() and ctrl-c breaks:
void main_close()
{
#ifdef STATCPUINSTR
    cpustats_log();
#endif

   // printf("does main_close\n");

    mameExitCleanCtrlC(); // flush game allocs, ahcked from mame.c, in case stopped during game.

    osd_close_display(); // also useful when ctrl-C

    osd_stop_audio_stream();

    unzip_cache_clear();

    closeTimers();

    FreeGUI();

    if(AppDiskObject) FreeDiskObject(AppDiskObject);

    CloseLowLevelLib();

//    if(P96Base) CloseLibrary(P96Base);

    if(CyberGfxBase) CloseLibrary(CyberGfxBase);

    if(IconBase) CloseLibrary(IconBase);

    if(KeymapBase) CloseLibrary(KeymapBase);

    if(LayersBase) CloseLibrary(LayersBase);
    if(UtilityBase) CloseLibrary(UtilityBase);

    if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);

    if(GfxBase) CloseLibrary((struct Library *)GfxBase);

// done in  by compiler runtime or not.
#ifdef USE_OWN_DOSBASE
   if(DOSBase) CloseLibrary((struct Library *)DOSBase);
#endif
}
//TODO  68060 68882" against options !! Well doenst work without 060 fpu anyway...
const char *pVersion="$VER:MAME 0.106 "
        APPVERNUM
        " 68060 68882";

// - - - - IF COMPILED WITH FLOAT, AND MACHINE HAS NO FPU, EXIT WITH MESSAGE - - - -
#if defined(MAME_USE_HARD_FLOAT)
int amiga_hasFPU()
{UWORD attnflags = SysBase->AttnFlags; return (int)((attnflags & (AFF_68881|AFF_68882|AFF_FPU40))!=0);
}
void amiga_cpucheck()
{   int hasFPU = amiga_hasFPU();
    if(!hasFPU) { printf("This tool was compiled for machines with a FPU.\n");
        exit(1);
    }
}
// make it happen before main() with a global constructor, it's less intrusive.
struct beforeMainInit{ beforeMainInit() { amiga_cpucheck(); } };
beforeMainInit _ginit;
#endif

// #define STACK_WATCH 1
int loadPaletteIlbm();

int main(int argc, char **argv)
{
//  setlocale(LC_NUMERIC, "POSIX"); // for C and C++ where synced with stdio
    {
    Task *task  = FindTask(NULL);
    int stacksize = ((int)task->tc_SPReg - (int)task->tc_SPLower);
#ifdef STACK_WATCH
    // the old stack reallocation looks messy,
    // this part is used to check the actual used stack depth.
    // by printing a const value on the whole stack alloc and verify what changed at exit.
    // anyway recursion needs are rare in Mame. some old string operations used 4kb temp
    //
 // if((task->tc_SPReg - task->tc_SPLower) < (MIN_STACK - 1024))

    int a=2;
    int *pa = &a;
    int *pa_far = (int *)(task->tc_SPLower+4) ;
    int *pa_near = (int *)( (int)pa-64);

    for(int i=0;i<(((int)pa_near-(int)pa_far))/sizeof(int*);i++)
    {
        pa_far[i]=0xCAFEBABE;
    }

#endif
        if(stacksize<MIN_STACK) {
            printf("This version of Mame needs 14Kb stack at least, use \"stack 16384\" or icon property.");
            return 1;
        }
    }

/* krb: old 1999 stack re-alloc, looks messy to me, original stack should be restored and alloc freed , in an atexit().
  task  = FindTask(NULL);
  if((task->tc_SPReg - task->tc_SPLower) < (MIN_STACK - 1024))
  {
    StackSwapStruct.stk_Lower = AllocVec(MIN_STACK, MEMF_PUBLIC);
    
    if(StackSwapStruct.stk_Lower)
    {
      StackSwapStruct.stk_Upper = (ULONG) StackSwapStruct.stk_Lower + MIN_STACK;
      StackSwapStruct.stk_Pointer = (APTR) StackSwapStruct.stk_Upper;

      StackSwap(&StackSwapStruct);
      
      Main(argc, argv);

      StackSwap(&StackSwapStruct);
    }
    //krb: where is FreeVec(StackSwapStruct.stk_Lower) ???
  }
  else
  */
    // any exit case will lead to close().
    atexit(&main_close);
    if(libs_init1()!=0)
    {
     printf("can't open common OS3 libs\n");
     exit(1);
    }

    int idriver=0; // romToLaunch;
    std::string userdir;
    std::string cheatfiletofilter;
    std::string rom;
    int romlist=0;
    int dohelp=0;
    int version=0;


    // this manages both command line args, and App icon tooltips... (if argc==0)
    // this is the "amiga.lib" way and actually uses icon.library.
    {
        // this manages both args by command line and args by icon tooltips.
        STRPTR *args = ArgArrayInit(argc,(const char **)argv);
        rom = ArgString((CONST_STRPTR*)args,"ROM","");

        userdir =  ArgString((CONST_STRPTR*)args,"USERDIR","PROGDIR:user");
        verbose = (ArgInt((CONST_STRPTR*)args,"VERBOSE",2)!=2);

        version = (ArgInt((CONST_STRPTR*)args,"VERSION",2)!=2);
        romlist = (ArgInt((CONST_STRPTR*)args,"-listfull",2)!=2);
        if(!romlist) romlist = (ArgInt((CONST_STRPTR*)args,"-ll",2)!=2);
        dohelp = (ArgInt((CONST_STRPTR*)args,"?",2)!=2);
        if(!dohelp) dohelp = (ArgInt((CONST_STRPTR*)args,"HELP",2)!=2);
        if(!dohelp) dohelp = (ArgInt((CONST_STRPTR*)args,"-h",2)!=2);
        if(!dohelp) dohelp = (ArgInt((CONST_STRPTR*)args,"--help",2)!=2);
        if(!dohelp) dohelp = (ArgInt((CONST_STRPTR*)args,"-v",2)!=2);

        cheatfiletofilter = ArgString((CONST_STRPTR*)args,"FILTERCHEAT",NULL);
        ArgArrayDone();
    }
    // this extra circus is to manage external project icons tooltips...
    // when launch from a "Project" icon, with tool set to "Mame106".
    // I though ArgArrayInit() would also take care of this.... but no.
    // we do it after main app icon so we can "override" values.
    if(argc ==0 && argv != 0)
    {
        struct WBStartup *WBenchMsg = (struct WBStartup *)argv; // Amiga C startup magic.
        struct WBArg *wbarg=WBenchMsg->sm_ArgList;
        for(int i=0 ;
            i < WBenchMsg->sm_NumArgs;
            i++, wbarg++)
        {
            struct DiskObject *dobj;

            if((*wbarg->wa_Name) && (dobj=GetDiskObject(wbarg->wa_Name)))
            {
                const char **toolarray = (const char **)dobj->do_ToolTypes;
                char *s;
                if(s=(char *)FindToolType(toolarray,"ROM")) rom = s;
                if(s=(char *)FindToolType(toolarray,"USERDIR")) userdir = s;
                if(s=(char *)FindToolType(toolarray,"VERBOSE")) verbose = 1;
                /* Free the diskobject we got */
                FreeDiskObject(dobj);
            }
        } // end loop by wbarg

    }

    // open extra libs that could fails
    if(libs_init2()!=0) exit(1);

    if(verbose) log_enableStdOut(1);


    getMainConfig().init(argc,argv); // init drivers map and read conf.


     // test if just "mame romname".
    if(argc>1)
    {
        int itest = getMainConfig().driverIndex().index(argv[1]);
        if(itest>=0) idriver= itest;
    }
    // test if romname in arg:
    if(rom.length()>0)  idriver = getMainConfig().driverIndex().index(rom.c_str());


    if(cheatfiletofilter.length()>0)
    {
        getMainConfig().filterCheatFile(cheatfiletofilter.c_str());
    }
    // command line things...
    if(version)
    {
        printf("%s\n",pVersion+5);
        return 0;
    }
    if(dohelp)
    {
        printf("%s\n",pVersion+5);
        printf("Install archive in roms/archive.zip and use with >Mame106 archive\n"
        " Mame106 uses P96 or Cybergraphics, MUI and AHI optionnals.\n"
        " commandline options (also can use icon's Tooltips):\n"
        "    -v version\n"
        "    -ll or --listfull list all archive names linked in this version.\n"
        "    verbose   log to standard output.\n"
        "    filtercheat=originalcheat.dat   Create new cheat files with only supported games. (with .new)");
        return 0;
    }
    if(romlist)
    {
        getMainConfig().listFull();
        return 0;
    }


    if(userdir.length()) getMainConfig().setUserDir(userdir.c_str());
    if(verbose) printf("try read main.cfg\n");
    getMainConfig().load();
    if(verbose) printf("after main.cfg\n");
    //  if game was explicit, no GUI
    if(idriver>0)
    {
        getMainConfig().setActiveDriver(idriver);
        StartGame();
        return 0;
    }

    AppDiskObject = GetDiskObject("skin/AppIcon"); // can be null or not.
    if(!AppDiskObject) AppDiskObject = GetDiskObjectNew("Mame106"); // can be null or not.

    AllocGUI();

    // go into interface loop
    ULONG quit=FALSE;

    // loop per emulation launched
    while(!quit)
    {
        quit = _pMameUI->MainGUI(); // select game with GUI.
        if(quit) break;

        StartGame();

    } // end loop by emulation launch

    getMainConfig().save();

#ifdef STACK_WATCH
    printf(" **** stack watch\n");
    int nbt = ((int)pa_near-(int)pa_far)/sizeof(int*);
    int i;
    for(i=0;i<nbt;i++)
    {
        if(*pa_far!=0xCAFEBABE) break;
        pa_far++;
    }
    int stackrealuse = (int) ((int)task->tc_SPReg - (int)pa_far);
    printf("stack size:%d   real use:%d \n",stacksize,stackrealuse);
#endif

    // end of main, will automatically reach main_close(), like any exit() call does.
    return(0);
}

// could be insteresting
//void ErrorRequest(LONG msg_id, ...)
//{
//  // intuition requester
//  struct EasyStruct es;
  
//  es.es_StructSize   = sizeof(struct EasyStruct);
//  es.es_Flags        = 0;
//  es.es_Title        =(CONST_STRPTR)(APPNAMEA);
//  es.es_TextFormat   = (CONST_STRPTR)(GetMessage(msg_id));
//  es.es_GadgetFormat = (CONST_STRPTR)(GetMessage(MSG_OK));

//  EasyRequestArgs(NULL, &es, NULL, &((&msg_id)[1]));
//}


