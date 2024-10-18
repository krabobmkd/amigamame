/**************************************************************************
 *
 * Copyright (C) 2024 Vic Ferry
 * Forked from 1999 Mats Eirik Hansen
 *
 *************************************************************************/


// krb:
/* mui includes generated with:
 /opt/amiga/bin/fd2sfd muimaster_lib.fd ../C/Include/clib/muimaster_protos.h -o muimaster_lib.sfd
  /opt/amiga/bin/sfdc muimaster_lib.sfd --mode clib >../../../../MUI/include/clib/muimaster_protos.h
  /opt/amiga/bin/sfdc muimaster_lib.sfd --mode macros >../../../../MUI/include/inline/muimaster.h
*/
#include <ctype.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include "intuiuncollide.h"
#include <proto/utility.h>
#include <proto/keymap.h>
#include <proto/timer.h>

extern "C" {
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <inline/muimaster.h>
}
#define CATCOMP_NUMBERS
#include "messages.h"

#include "asmmacros.h"

extern "C" {
    #include "driver.h"
    #include "mamecore.h"
}

#include "gui_mui.h"
#include "amiga106_config.h"
#include "serializer_mui.h"

#include "version.h"

#include <vector>
#include <string>
#include <cstdio>

typedef ULONG (*RE_HOOKFUNC)();

#define RID_Start      1
#define RID_CloseAbout 2
#define RID_Scan       3

#define MID_About     104
#define MID_AboutMUI  105

//#define SMT_DISPLAYID 0
//#define SMT_DEPTH     1

#define CFGS_ALL 0
#define CFGS_FOUND 1

#ifndef GIT_BRANCH
#define GIT_BRANCH
#endif
const char *TextAbout =
"\33c\n\33b\33uMAME - Multiple Arcade Machine Emulator\33n\n\n"
"0."REVISION" \n\n"
"Copyright (C) 1997-2024 by Nicola Salmoria and the MAME team\n"
"http://mamedev.org\n\n"
"Amiga port by Vic 'Krb' Ferry (2024) source:\n"
" https://github.com/krabobmkd/amigamame\n"
"compiled branch:" GIT_BRANCH " " REVDATE " " REVTIME "\n"
"Partly based on Mats Eirik Hansen Mame060(1999)\n"
" http://www.triumph.no/mame\n"
"This program uses libexpat,zlib,\n"
" MUI - Magic User Interface\n";


// https://github.com/krabobmkd
// "CGXHooks routines by Trond Werner Hansen\n\n"
// "Chunky to planar routine by Mikael Kalms\n\n"
struct DriverData
{
  struct MUI_EventHandlerNode EventHandler;
  ULONG           CurrentEntry;
  ULONG           CharIndex;
  ULONG           Seconds;
  ULONG           Micros;
  APTR            List;
};

// this is from mui.h, but unreachable from C++ there
#define get(obj,attr,store) GetAttr(attr,(Object *)obj,(ULONG *)store)
#define set(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)

struct Library *MUIMasterBase = NULL;

static struct MUI_CustomClass *DriverClass;

static Object *App=NULL;
static Object *MainWin=NULL;
static Object *AboutWin=NULL;
static Object *DisplayName=NULL;

static Object * RE_Options=NULL;

//static Object * CM_Allow16Bit=NULL;
//static Object * CM_FlipX=NULL;
//static Object * CM_FlipY=NULL;
//static Object * CM_DirtyLines=NULL;
//static Object * CM_AutoFrameSkip=NULL;

static Object * CY_Show=NULL;
//static Object *CM_UseDefaults;
static Object * BU_Scan=NULL;

//static Object * CM_Antialiasing;
//static Object * CM_Translucency;
//static Object * SL_BeamWidth;
//static Object * SL_VectorFlicker;

// audio
//static Object * SL_AudioChannel[4];
//static Object * SL_MinFreeChip;

//static Object * SL_FrameSkip;

//static Object * ST_Width;
//static Object * ST_Height;
//static Object * ST_RomPath;
//static Object * ST_SamplePath;
//static Object * CY_ScreenType;
//static Object * CY_DirectMode;
//static Object * CY_Sound;
//static Object * CY_Buffering;
//static Object * CY_Rotation;



//static Object * PA_ScreenMode;
//static Object * PA_RomPath;
//static Object * PA_SamplePath;
static Object *LI_Driver=NULL;
static Object * LV_Driver=NULL;
static Object * BU_Start=NULL;
static Object * BU_Quit=NULL;
static Object * BU_About_OK=NULL;
static Object * PU_ScreenMode=NULL;

MUISerializer muiConfigCreator;
static int columnToSort = 0;

//  MUIM_Notify,  MUIA_List_Active, MUIV_EveryTime,
static ULONG ASM DriverSelect(struct Hook *hook REG(a0), APTR obj REG(a2), LONG *par REG(a1))
{
    if(!par)
    {
        printf("DriverSelect:par 0\n");
        return 0;
     }
        MameConfig &config = getMainConfig();
    printf("DriverSelect:%d\n",*par);
    // get(LI_Driver, MUIA_List_Active, &v);
    int listindex = *par;
    if(listindex>=0)
    {
        const game_driver **ppdrv=NULL; // int entry; // driver ptr ptr
        DoMethod((Object*)LI_Driver,(ULONG) MUIM_List_GetEntry, (ULONG)listindex, (ULONG)&ppdrv);
        if(ppdrv && *ppdrv)
        {
            //printf("DriverSelect:%s\n",(*ppdrv)->name);
            std::string screenconf;
            int videoAttribs;
            config.getDriverScreenModestring(ppdrv, screenconf,videoAttribs);

            muiConfigCreator.selectGroup("Display.Per Screen Mode",screenconf);

            set(BU_Start,  MUIA_Disabled, FALSE);
            //   printf("screen:%s:\n",screenconf.c_str());

        }
    }
  return(0);
}
static struct Hook DriverSelectHook;

//static UBYTE           DisplayNameBuffer[256];
std::string DisplayNameBuffer;

static STRPTR Shows[] =
{
  (STRPTR) MSG_ALL,
  (STRPTR) MSG_FOUND,
  NULL
};

static STRPTR ScreenTypes[] =
{
  (STRPTR) MSG_BEST,
  (STRPTR) MSG_WORKBENCH,
  (STRPTR) MSG_CUSTOM,
  (STRPTR) MSG_USER_SELECT,
  NULL
};

static APTR DirectModes[] =
{
  (STRPTR) MSG_OFF,
  (STRPTR) MSG_DRAW,
  (STRPTR) MSG_COPY,
  NULL
};

static STRPTR Sounds[] =
{
  (STRPTR) MSG_NONE,
  (STRPTR) MSG_PAULA,
  (STRPTR) MSG_AHI,
  NULL
};

static STRPTR Joy1Types[] =
{
  (STRPTR) MSG_NONE,
  (STRPTR) MSG_JOYSTICK_PORT_2,
  (STRPTR) MSG_JOYPAD_PORT_2,
  (STRPTR) MSG_MOUSE_PORT_1,
  NULL
};

static STRPTR Joy2Types[] =
{
  (STRPTR) MSG_NONE,
  (STRPTR) MSG_JOYSTICK_PORT_1,
  (STRPTR) MSG_JOYPAD_PORT_1,
  NULL
};

static STRPTR Rotations[] =
{
  (STRPTR) MSG_NO,
  (STRPTR) MSG_LEFT,
  (STRPTR) MSG_RIGHT,
  NULL
};

//static STRPTR RegisterTitles[] =
//{
//  (STRPTR) MSG_DRIVERS,
//  (STRPTR) MSG_DISPLAY,
//  (STRPTR) MSG_SOUND,
//  (STRPTR) MSG_INPUT,
//  (STRPTR) MSG_PATHS,
//  NULL
//};

static STRPTR Bufferings[] =
{
  (STRPTR) MSG_SINGLE,
  (STRPTR) MSG_DOUBLE,
  (STRPTR) MSG_TRIPLE,
  NULL
};

static struct TagItem ScreenModeTags[] =
{
  { ASLSM_InitialDisplayID,   0 },
  { ASLSM_InitialDisplayDepth,  8 },
  { ASLSM_DoDepth,        TRUE  },
  { TAG_END }
};

static struct Hook DirectModeNotifyHook={0};
static struct Hook SoundNotifyHook={0};
static struct Hook DriverDisplayHook={0};
static struct Hook DriverSortHook={0};
static struct Hook DriverNotifyHook={0};
#ifndef MESS
static struct Hook ShowNotifyHook={0};
#endif

#ifndef MESS
// list column names
static char *String_Driver=(char *)"Driver";
static char *String_Archive=(char *)"Archive";
static char *String_Parent=(char *)"Parent";
static char *String_Screen=(char *)"Screen";
static char *String_Players=(char *)"Players.";
static char *String_Year=(char *)"Year";
static char *String_Comment=(char *)"Comment";
/*

static char *DirectoryString;
static char *TypeString;
static char *WidthString;
static char *HeightString;
static char *ColorsString;

*/
static char *NotWorkingString;
static char *WrongColorsString;
static char *ImperfectColorsString;
//static char *BitmapString;
//static char *VectorString;
//static char *BitmapGameDefaultsString;
//static char *VectorGameDefaultsString;
#endif

static void CreateApp(void);
static void SetDisplayName(ULONG);
//static ULONG ASM ScreenModeStart(struct Hook *hook REG(a0), APTR popasl REG(a2), struct TagItem *taglist REG(a1));
//static ULONG ASM ScreenModeStop(struct Hook *hook REG(a0), APTR popasl REG(a2), struct ScreenModeRequester *smreq REG(a1));
static ULONG ASM DirectModeNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
static ULONG ASM SoundNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
static ULONG ASM DriverNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
#ifndef MESS
static ULONG ASM ShowNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
#endif

/* Convert an index in SortedDrivers into an index for the same
 * driver in Drivers. */

//static inline int GetSortedDriverIndex(int sorted_index)
//{
//  int index;

//  if(sorted_index < DRIVER_OFFSET)
//    index = sorted_index - DRIVER_OFFSET;
//  else
//    index = (((ULONG) SortedDrivers[sorted_index]) - ((ULONG) &Drivers[0])) / sizeof(struct _game_driver *);

//  return(index);
//}

static inline int GetEntryDriverIndex(ULONG entry)
{
  int index;

  index = (entry - ((ULONG) &drivers[0])) / sizeof(struct _game_driver *);

  return(index);
}

static int GetDriverIndex(void)
{
  ULONG entry;
  ULONG list_index;
  int   index;

  get(LI_Driver, MUIA_List_Active, &list_index);

  if(list_index == MUIV_List_Active_Off)
  {
    index = - 1;
  }
  else if(list_index < 0)
  {
    index = list_index;
  }
  else
  {
    DoMethod((Object*)LI_Driver,(ULONG) MUIM_List_GetEntry, (ULONG)list_index, (ULONG)&entry);

    index = GetEntryDriverIndex(entry);
  }

  return(index);
}

static struct _game_driver *GetDriver(void)
{
  struct _game_driver *drv;

  ULONG entry;
  ULONG list_index;

  get(LI_Driver, MUIA_List_Active, &list_index);

  if((list_index == MUIV_List_Active_Off) || (list_index < 0))
    return(NULL);

  DoMethod((Object*)LI_Driver, MUIM_List_GetEntry, list_index, &entry);

  if(!entry)
    return(NULL);

  drv = *((struct _game_driver **) entry);

  return(drv);
}

static void ShowFound(void)
{
    MameConfig &config = getMainConfig();
    const std::vector<const _game_driver *const*> &roms = config.romsFound();
    //MUIM_List_Insert can insert everything in a blow.
        DoMethod((Object *)LI_Driver, MUIM_List_Insert,
         (ULONG)roms.data(),(int)roms.size(),  /*MUIV_List_Insert_Bottom*/MUIV_List_Insert_Sorted);

// no need because MUIV_List_Insert_Sorted   DoMethod(LI_Driver,MUIM_List_Sort);

    // ensure cycle is in correct state
    int cyclestate=0;
    get(CY_Show, MUIA_Cycle_Active, &cyclestate);
    if(cyclestate != 1)
    {
        set(CY_Show, MUIA_Cycle_Active,1);
    }


}
static void ShowAll(void)
{
    MameConfig &config = getMainConfig();
    std::vector<const _game_driver *const*> roms;
    config.buildAllRomsVector(roms);

    //MUIM_List_Insert can insert everything in a blow.
        DoMethod((Object *)LI_Driver, MUIM_List_Insert,
         (ULONG)roms.data(),(int)roms.size(),  /*MUIV_List_Insert_Bottom*/MUIV_List_Insert_Sorted);

   // no need because MUIV_List_Insert_Sorted DoMethod(LI_Driver,MUIM_List_Sort);

    // ensure cycle is in correct state
    int cyclestate=0;
    get(CY_Show, MUIA_Cycle_Active, &cyclestate);
    if(cyclestate != 0)
    {
        set(CY_Show, MUIA_Cycle_Active,0);
    }
}

static int DriverCompareNames(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return(stricmp((*drv1)->description, (*drv2)->description));
}
static int DriverCompareNbPlayers(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return((*drv1)->nbplayers < (*drv2)->nbplayers);
}
static int DriverCompareYear(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return(strcmp((*drv1)->year, (*drv2)->year));
}
static int DriverCompareArchive(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return(stricmp((*drv1)->name, (*drv2)->name));
}
static int DriverCompareParent(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return(stricmp((*drv1)->parent, (*drv2)->parent));
}


// LONG __asm cmpfunc(_a1 char *s1,_a2 char *s2)
static ULONG ASM DriverSort(
    const struct _game_driver **drva REG(a1),
    const struct _game_driver **drvb REG(a2)
    )
{   // must return -1 0 1
    int icolumnToSort = columnToSort;
    if(icolumnToSort <0) icolumnToSort=0;
    switch(icolumnToSort)
    {
        case 0: return DriverCompareNames(drva,drvb);
        case 1: return DriverCompareNbPlayers(drva,drvb);
        case 2: return DriverCompareNames(drva,drvb); // screens TODO ?
        case 3: return DriverCompareYear(drva,drvb);
        case 4: return DriverCompareArchive(drva,drvb);
        case 5: return DriverCompareParent(drva,drvb);
    }

   return DriverCompareNames(drva,drvb);

}
static ULONG ASM DriverDisplay(struct Hook *hook REG(a0), char **array REG(a2),const struct _game_driver **drv_indirect REG(a1))
{

    MameConfig &config = getMainConfig();
    const struct _game_driver *drv;

    struct ColumnsString {
          char *_driver,*_players,*_screen,*_year,*_archive,*_parent,*_comment;
    };
    ColumnsString *pColumns = (ColumnsString *)array;

  static char driver[56];
  static char screen[32];
  static char archive[16];
  static char parent[16];
 static char players[12];
 static char year[12];
//  static char comment[128];
 static std::string strComment;
  if(!drv_indirect)
  {
    snprintf(driver,55,   "\033b\033u%s", String_Driver);
    driver[55]=0;
    snprintf(screen,31,   "\033b\033u%s", String_Screen);
    screen[31]=0;
    snprintf(archive,15,  "\033b\033u%s", String_Archive);
    archive[15]=0;
    snprintf(parent,15,  "\033b\033u%s", String_Parent);
    parent[15]=0;
    snprintf(players,11,    "\033b\033u%s", String_Players);
    players[11]=0;
    snprintf(year,11,    "\033b\033u%s", String_Year);
    year[11]=0;
   // snprintf(comment,127,  "\033b\033u%s", String_Comment);
   // comment[127]=0;
    strComment =  "\033b\033u";
    strComment += String_Comment;

    pColumns->_driver = driver;
    pColumns->_players = players;
    pColumns->_year = year;
    pColumns->_screen = screen;
    pColumns->_archive = archive;
    pColumns->_parent = parent;
    pColumns->_comment = (char *)strComment.c_str(); //comment;
    return(0);
  }

  drv = *drv_indirect;

 if(config.isDriverFound(drv_indirect))
 {
     // if found: to bold
    snprintf(driver,55,"\033b%s", drv->description);
    driver[55]=0;
    pColumns->_driver = driver;
 } else
 {  // just to cut too long names
     snprintf(driver,54,"%s", drv->description);
     driver[54]=0;
     pColumns->_driver = driver;
 }

 static std::string str_screen;
 int video_attribs;

 config.getDriverScreenModestring(drv_indirect,str_screen,video_attribs);

 pColumns->_screen = (char*) str_screen.c_str();

   pColumns->_archive = (char *) drv->name;
   if(!drv->parent || (drv->parent[0]=='0' &&drv->parent[1]==0 ))
    pColumns->_parent = (char*)"";
   else
    pColumns->_parent = (char*)drv->parent;

   if(drv->nbplayers == 0) players[0]=0;
   else snprintf(players,7,"%d", (int) drv->nbplayers);
pColumns->_players = players;

    pColumns->_year = (char *)drv->year;

//  if(drv->flags & GAME_NOT_WORKING)
//   pColumns->_comment = NotWorkingString;
//  else if(drv->flags & GAME_WRONG_COLORS)
//   pColumns->_comment = WrongColorsString;
//  else if(drv->flags & GAME_IMPERFECT_COLORS)
//   pColumns->_comment = ImperfectColorsString;
//  else
//   pColumns->_comment =(char*) "";
     if(drv->flags & GAME_NOT_WORKING)
      strComment = NotWorkingString;
     else if(drv->flags & GAME_WRONG_COLORS)
      strComment = WrongColorsString;
     else if(drv->flags & GAME_IMPERFECT_COLORS)
      strComment = ImperfectColorsString;
     else
        strComment.clear();

     if(video_attribs & VIDEO_RGB_DIRECT)
     {
        if(video_attribs & VIDEO_NEEDS_6BITS_PER_GUN)
            strComment += " RGB32";
        else  strComment += " RGB15";
      }
     int orientation = drv->flags & ORIENTATION_MASK;
     if(orientation)
     {
      if(orientation == ROT90)  strComment += " ROT90";
      if(orientation == ROT180)  strComment += " ROT180";
      if(orientation == ROT270)  strComment += " ROT270";
      if(orientation == ORIENTATION_FLIP_X)  strComment += " FLIPX";
      if(orientation == ORIENTATION_FLIP_Y)  strComment += " FLIPY";
      if(orientation == ORIENTATION_SWAP_XY)  strComment += " SWAPXY";

     }
   pColumns->_comment = (char *)strComment.c_str(); //comment;
  return(0);
}
// extend list class
static ULONG ASM DriverDispatcher(struct IClass *cclass REG(a0), Object * obj REG(a2), Msg msg REG(a1))
{
  struct DriverData   *data;
  struct IntuiMessage *imsg;
  struct _game_driver   **drv_indirect;
  struct _game_driver   *drv;
  struct InputEvent   ie;

  Object  *list;
  APTR  active_obj;
  ULONG i;
  UBYTE key;

//printf("msg->MethodID:%08x\n",msg->MethodID);
  switch(msg->MethodID)
  {
    case MUIM_Setup:
      data = (struct DriverData *)INST_DATA(cclass, obj);

      if(DoSuperMethodA(cclass, obj, msg))
      {
        data->Seconds = 0;
        data->Micros  = 0;

        data->EventHandler.ehn_Priority = 0;
        data->EventHandler.ehn_Flags    = 0;
        data->EventHandler.ehn_Object   = obj;
        data->EventHandler.ehn_Class    = cclass;
        data->EventHandler.ehn_Events   = IDCMP_RAWKEY;
        DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->EventHandler);

        return(TRUE);
      }

      return(FALSE);

    case MUIM_Cleanup:
      data = (struct DriverData *)INST_DATA(cclass, obj);

      DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->EventHandler);
      break;

    case MUIM_HandleEvent:
      data =  (struct DriverData *) INST_DATA(cclass, obj);
      imsg = (struct IntuiMessage *) msg[1].MethodID;

 printf("MUIM_HandleEvent\n");

      get(_win(obj), MUIA_Window_ActiveObject, &active_obj);

      if(obj == active_obj)
      {
        if(imsg->Class == IDCMP_RAWKEY)
        {
          ie.ie_Class   = IECLASS_RAWKEY;
          ie.ie_SubClass  = 0;
          ie.ie_Code    = imsg->Code;
          ie.ie_Qualifier = 0;

//  MapRawKey( CONST struct InputEvent *event, STRPTR buffer, LONG length, CONST struct KeyMap *keyMap );
          if(MapRawKey(&ie,(STRPTR)&key, 1, NULL) && isalnum(key))
          {
            i = imsg->Seconds - data->Seconds;

            if(imsg->Micros < data->Micros)
              i--;

            if(i < 1)
            {
              data->CharIndex++;
              i = data->CurrentEntry;
            }
            else
            {
              data->CharIndex = 0;
              i = 0;
            }

            data->Seconds = imsg->Seconds;
            data->Micros  = imsg->Micros;

            get(obj, MUIA_Listview_List, &list);

            do
            {
              DoMethod(list, MUIM_List_GetEntry, i, &drv_indirect);

              if(drv_indirect)
              {
                drv = *drv_indirect;

                if(data->CharIndex < strlen(drv->description))
                {
                  if(key <= tolower(drv->description[data->CharIndex]))
                  {
                    data->CurrentEntry = i;

                    set(list, MUIA_List_Active, i);

                    break;
                  }
                }
              }

              i++;

            } while(drv_indirect);

            return(MUI_EventHandlerRC_Eat);
          }
        }
      }
      return(0);
      //break;
      case MUIM_HandleInput: // krb added
        {
	#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
	#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))


            // - - - check clicks on title bar
            //  struct IntuiMessage *imsg; LONG muikey;
            struct MUIP_HandleInput *msghi = (struct MUIP_HandleInput *)msg;
            if(msghi->imsg && msghi->imsg->Class == IDCMP_MOUSEBUTTONS &&
             msghi->imsg->Code == SELECTDOWN )
            {
                // receive all clicks on the window.
                // we have to check list bounds
                if(_isinobject( msghi->imsg->MouseX,msghi->imsg->MouseY))
                {
                    struct MUI_List_TestPos_Result res;
                    DoMethod(obj,MUIM_List_TestPos,
                        msghi->imsg->MouseX,msghi->imsg->MouseY, &res);
                    if(res.column>=0 && res.entry == -1)
                    {
                        // would indicate clicking titles
                        if(res.column != columnToSort)
                        {
                            columnToSort = res.column;
                            DoMethod(obj,MUIM_List_Sort);
                        }
                    }
                }
                // Class
            }
            if (msghi->muikey!=MUIKEY_NONE)
            {
//                switch (msg->muikey)
//                {
//                    case MUIKEY_LEFT : data->sx=-1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                    case MUIKEY_RIGHT: data->sx= 1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                    case MUIKEY_UP   : data->sy=-1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                    case MUIKEY_DOWN : data->sy= 1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                }
            }

            struct MUI_List_TestPos_Result res;
           // DoMethod(obj,MUIM_List_TestPos,LONG x, LONG y, &res);

            return(DoSuperMethodA(cclass, obj, msg));
        }
      break;
  }

  return(DoSuperMethodA(cclass, obj, msg));
}

void AllocGUI(void)
{
#ifdef DOMAMELOG
    printf("Open muimaster 16\n");
#endif
    MUIMasterBase = OpenLibrary("muimaster.library", 16);
    if(!MUIMasterBase)
    {
        printf(" no MUI interface found\n install MUI or launch a game installed in roms/romname.zip with:\n>Mame106 romname\n\n");
        return;
    }
    LONG  i;

//  for(NumDrivers = 0; Drivers[NumDrivers]; NumDrivers++);

//  SortedDrivers = ( struct _game_driver ***)malloc((NumDrivers + DRIVER_OFFSET) * sizeof(struct _game_driver **));

//  if(SortedDrivers)
//  {
//#ifndef MESS
//    SortedDrivers[0]  = (struct _game_driver **) 1;
//    SortedDrivers[1]  = (struct _game_driver **) 2;
//#endif
//    for(i = 0; i < NumDrivers; i++)
//      SortedDrivers[i+DRIVER_OFFSET] = const_cast<struct _game_driver **>(&Drivers[i]);

//    qsort(&SortedDrivers[DRIVER_OFFSET], NumDrivers, sizeof(struct _game_driver **), (int (*)(const void *, const void *)) DriverCompare);

#ifdef DOMAMELOG
    printf("old locale init\n");
#endif
    App     = NULL;
    MainWin   = NULL;
    AboutWin  = NULL;

    for(i = 0; Shows[i]; i++)
      Shows[i] = GetMessage((LONG) Shows[i]);

    for(i = 0; ScreenTypes[i]; i++)
      ScreenTypes[i] = GetMessage((LONG) ScreenTypes[i]);

    for(i = 0; DirectModes[i]; i++)
      DirectModes[i] = GetMessage((LONG) DirectModes[i]);

    for(i = 0; Sounds[i]; i++)
      Sounds[i] = GetMessage((LONG) Sounds[i]);

    for(i = 0; Joy1Types[i]; i++)
      Joy1Types[i] = GetMessage((LONG) Joy1Types[i]);

    for(i = 0; Joy2Types[i]; i++)
      Joy2Types[i] = GetMessage((LONG) Joy2Types[i]);

    for(i = 0; Rotations[i]; i++)
      Rotations[i] = GetMessage((LONG) Rotations[i]);

//    for(i = 0; RegisterTitles[i]; i++)
//      RegisterTitles[i] = GetMessage((LONG) RegisterTitles[i]);

    for(i = 0; Bufferings[i]; i++)
      Bufferings[i] = GetMessage((LONG) Bufferings[i]);

//    ScreenModeStartHook.h_Entry  = (RE_HOOKFUNC) ScreenModeStart;
//    ScreenModeStopHook.h_Entry   = (RE_HOOKFUNC) ScreenModeStop;
    DirectModeNotifyHook.h_Entry = (RE_HOOKFUNC) DirectModeNotify;
    SoundNotifyHook.h_Entry      = (RE_HOOKFUNC) SoundNotify;
    DriverDisplayHook.h_Entry    = (RE_HOOKFUNC) DriverDisplay;
    DriverSortHook.h_Entry    = (RE_HOOKFUNC) DriverSort;
    DriverNotifyHook.h_Entry     = (RE_HOOKFUNC) DriverNotify;
#ifndef MESS
    ShowNotifyHook.h_Entry        = (RE_HOOKFUNC) ShowNotify;

//    DriverString             = GetMessage(MSG_DRIVER);
//    DirectoryString          = GetMessage(MSG_DIRECTORY);
//    TypeString               = GetMessage(MSG_TYPE);
//    WidthString              = GetMessage(MSG_WIDTH);
//    HeightString             = GetMessage(MSG_HEIGHT);
//    ColorsString             = GetMessage(MSG_COLORS);
//    CommentString            = GetMessage(MSG_COMMENT);
    NotWorkingString         = GetMessage(MSG_NOT_WORKING);
    WrongColorsString        = GetMessage(MSG_WRONG_COLORS);
    ImperfectColorsString    = GetMessage(MSG_IMPERFECT_COLORS);
//    BitmapString             = GetMessage(MSG_BITMAP);
//    VectorString             = GetMessage(MSG_VECTOR);
//    BitmapGameDefaultsString = GetMessage(MSG_BITMAP_GAME_DEFAULTS);
//    VectorGameDefaultsString = GetMessage(MSG_VECTOR_GAME_DEFAULTS);
#endif
#ifdef DOMAMELOG
    printf("before MUI_CreateCustomClass()\n");
#endif
    DriverClass = MUI_CreateCustomClass(NULL, MUIC_Listview, NULL, sizeof(struct DriverData),(APTR) DriverDispatcher);
#ifdef DOMAMELOG
    printf("after MUI_CreateCustomClass()\n");
#endif
}

void FreeGUI(void)
{

    if(App)
      MUI_DisposeObject(App);

    if(DriverClass)
      MUI_DeleteCustomClass(DriverClass);

    if(MUIMasterBase)
      CloseLibrary(MUIMasterBase);

}

// = = = = = = = objects contructors = = = = = =
/*ok but
inline Object *cMenuItem(STRPTR text,STRPTR shortcut,void *userData)
{
    vector<struct TagItem> tags;
    tags.reserve(4);

    if(text) tags.push_back({MUIA_Menuitem_Title,(ULONG)text});
    if(shortcut) tags.push_back({MUIA_Menuitem_Shortcut,(ULONG)shortcut});
    if(userData) tags.push_back({MUIA_UserData,(ULONG)userData});
    tags.push_back({TAG_END,NULL});

    return MUI_NewObjectA(MUIC_Menuitem,tags.data() );
}
inline Object *cMenu1()
{
    return MUI_NewObject(MUIC_Menu,
MUIA_Menu_Title,(ULONG)GetMessage(MSG_MENU_GAME),
MUIA_Family_Child,(ULONG)cMenuItem(GetMessage(MSG_MENU_ABOUT),"?",MID_About),
MUIA_Family_Child,(ULONG)cMenuItem("About MUI...",NULL,MID_AboutMUI),
MUIA_Family_Child,(ULONG)MUI_NewObject(MUIC_Menuitem,MUIA_Menuitem_Title,NM_BARLABEL,End),
MUIA_Family_Child,(ULONG)cMenuItem( GetMessage(MSG_MENU_QUIT),"Q",MUIV_Application_ReturnID_Quit),
            End
            );

}
inline Object *cMainVLayout()
{
// #define RegisterGroup(t)    MUI_NewObject(MUIC_Register,MUIA_Register_Titles,(t)
  return MUI_NewObject(MUIC_Group,
        Child, RE_Options =MUI_NewObject(MUIC_Register,MUIA_Register_Titles,(RegisterTitles)
    TODO
  );
}
inline Object *cMainWin()
{
    Object *pMenu1 = cMenu1();
    Object *pMenuStrip = MUI_NewObject(MUIC_Menustrip,MUIA_Family_Child,(ULONG)pMenu1);

    Object *pMainVLayout = cMainVLayout();

    return MUI_NewObject(MUIC_Window,
          MUIA_Window_Title, APPNAME,
          MUIA_Window_ID   , MAKE_ID('M','A','I','N'),
          MUIA_Window_Menustrip, pMenuStrip,
          WindowContents,(ULONG) pMainVLayout,
            End
            );
}
*/
// use less macros
inline Object *MUINewObject( char *pclassname,ULONG tag1, ... )
{
   return MUI_NewObjectA(pclassname, (struct TagItem *) &tag1);
}
inline ULONG UMUINO(char *pclassname,ULONG tag1, ... )
{
   return (ULONG)MUI_NewObjectA(pclassname, (struct TagItem *) &tag1);
}
inline Object *OMUINO(char *pclassname,ULONG tag1, ... )
{
   return MUI_NewObjectA(pclassname, (struct TagItem *) &tag1);
}
#define UCheckMark(selected)\
	(ULONG)MUI_NewObject(MUIC_Image,\
		ImageButtonFrame,\
		MUIA_InputMode        , MUIV_InputMode_Toggle,\
		MUIA_Image_Spec       , MUII_CheckMark,\
		MUIA_Image_FreeVert   , TRUE,\
		MUIA_Selected         , selected,\
		MUIA_Background       , MUII_ButtonBack,\
		MUIA_ShowSelState     , FALSE,\
		TAG_DONE)
#define OCheckMark(selected)\
	MUI_NewObject(MUIC_Image,\
		ImageButtonFrame,\
		MUIA_InputMode        , MUIV_InputMode_Toggle,\
		MUIA_Image_Spec       , MUII_CheckMark,\
		MUIA_Image_FreeVert   , TRUE,\
		MUIA_Selected         , selected,\
		MUIA_Background       , MUII_ButtonBack,\
		MUIA_ShowSelState     , FALSE,\
		TAG_DONE)
#define OString(contents,maxlen)\
	MUI_NewObject(MUIC_String,\
		StringFrame,\
		MUIA_String_MaxLen  , maxlen,\
		MUIA_String_Contents, contents,\
		TAG_DONE)

Object *createPanel_Drivers()
{
  return MUINewObject(MUIC_Group,
        Child, LV_Driver = (Object *)(DriverClass!=NULL)?

            ((Object *)NewObject(DriverClass->mcc_Class, NULL,
          MUIA_Listview_Input,    TRUE,
            MUIA_Listview_List, (ULONG)( LI_Driver = MUINewObject(MUIC_List,
              MUIA_List_Title,    TRUE,
              MUIA_List_Format,   "BAR,BAR,BAR,BAR,BAR,BAR,",
              MUIA_List_DisplayHook,(ULONG)  &DriverDisplayHook,
              MUIA_List_CompareHook,(ULONG)  &DriverSortHook,
            InputListFrame,
          TAG_DONE)),
        TAG_END)
             )
        :(Object *) OMUINO(MUIC_Listview,
          MUIA_Listview_Input, TRUE,
            MUIA_Listview_List, (ULONG)( LI_Driver = MUINewObject(MUIC_List,
              MUIA_List_Title, TRUE,
              MUIA_List_Format, "BAR,BAR,BAR,BAR,BAR,BAR,",
              MUIA_List_DisplayHook,  &DriverDisplayHook,
              MUIA_List_CompareHook,(ULONG)  &DriverSortHook,
            InputListFrame,
          TAG_DONE)),
        TAG_DONE)
        ,
        Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,          
          Child, Label((ULONG)GetMessage(MSG_SHOW)),
          Child, (ULONG) (CY_Show = OMUINO(MUIC_Cycle,
            MUIA_Cycle_Entries, (ULONG) Shows,
          TAG_DONE)),
          Child,(ULONG)(BU_Scan = SimpleButton((ULONG)GetMessage(MSG_SCAN))),
        TAG_DONE),
    TAG_DONE);

}

//ULONG createPanel_Display()
//{
//    ULONG w= UMUINO(MUIC_Group,
//    Child, HVSpace,
//    Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,
//      Child, HSpace(0),

//      // column group of 4 column, name/checkbox/name/checkbox.
////      Child, UMUINO(MUIC_Group,MUIA_Group_Columns,4,
////        MUIA_HorizWeight, 1000,
////        Child, Label((ULONG)GetMessage(MSG_ALLOW16BIT)),
////        Child, (ULONG)(CM_Allow16Bit = OCheckMark(/*Config[CFG_ALLOW16BIT]*/1)),
////        Child, Label((ULONG)GetMessage(MSG_AUTO_FRAMESKIP)),
////        Child, (ULONG)(CM_AutoFrameSkip = OCheckMark(/*Config[CFG_AUTOFRAMESKIP]*/1)),
////        Child, Label((ULONG)GetMessage(MSG_FLIPX)),
////        Child, (ULONG)(CM_FlipX = OCheckMark(/*Config[CFG_FLIPX]*/0)),
////        Child, Label((ULONG)GetMessage(MSG_ANTIALIAS)),
////        Child, (ULONG)(CM_Antialiasing = OCheckMark(/*Config[CFG_ANTIALIASING]*/1)),
////        Child, Label((ULONG)GetMessage(MSG_FLIPY)),
////        Child, (ULONG)(CM_FlipY = OCheckMark(/*Config[CFG_FLIPY]*/0)),
////        Child, Label((ULONG)GetMessage(MSG_TRANSLUCENCY)),
////        Child, (ULONG)(CM_Translucency = OCheckMark(/*Config[CFG_TRANSLUCENCY]*/1)),
////        Child, Label((ULONG)GetMessage(MSG_DIRTY_LINES)),
////        Child, (ULONG)(CM_DirtyLines = OCheckMark(/*Config[CFG_DIRTYLINES]*/1)),
////      TAG_DONE), // end colgroup 4

//      Child, HSpace(0),
//    TAG_DONE),
////    Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,
////      Child, HSpace(0),
////      Child, UMUINO(MUIC_Group,MUIA_Group_Columns,2,
////        MUIA_HorizWeight, 1000,
////        Child, Label((ULONG)GetMessage(MSG_BEAM)),
////        Child,(ULONG)( SL_BeamWidth = OMUINO(MUIC_Slider,
////          MUIA_Slider_Min, 1,
////          MUIA_Slider_Max, 16,
////        TAG_DONE)),
////        Child, Label((ULONG)GetMessage(MSG_FLICKER)),
////        Child,(ULONG)( SL_VectorFlicker = OMUINO(MUIC_Slider,
////          MUIA_Slider_Min, 0,
////          MUIA_Slider_Max, 100,
////        TAG_DONE)),
////        Child, Label((ULONG)GetMessage(MSG_FRAMESKIP)),
////        Child,(ULONG)( SL_FrameSkip = OMUINO(MUIC_Slider,
////          MUIA_Slider_Min, 0,
////          MUIA_Slider_Max, 3,
////        TAG_DONE)),
////        Child, Label((ULONG)GetMessage(MSG_WIDTH)),
////        Child,(ULONG)( ST_Width = MUI_NewObject(MUIC_String,
////          StringFrame,
////          MUIA_String_Accept, (ULONG) "0123456789",
////        TAG_DONE)),
////        Child, Label((ULONG)GetMessage(MSG_HEIGHT)),
////        Child,(ULONG)( ST_Height = MUI_NewObject(MUIC_String,
////          StringFrame,
////          MUIA_String_Accept, (ULONG) "0123456789",
////        TAG_DONE)),
////      TAG_DONE),
////      Child, HSpace(0),
////    TAG_DONE),

//    Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,
//      Child, HSpace(0),
//      Child, UMUINO(MUIC_Group,MUIA_Group_Columns,2,
//        MUIA_HorizWeight, 1000,
//        Child, Label((ULONG)GetMessage(MSG_SCREEN_TYPE)),
//        Child, CY_ScreenType = OMUINO(MUIC_Cycle,
//          MUIA_Cycle_Entries, (ULONG) ScreenTypes,
//        TAG_DONE),



////        Child, Label((ULONG)GetMessage(MSG_DIRECT_MODE)),
////        Child, CY_DirectMode = OMUINO(MUIC_Cycle,
////          MUIA_Cycle_Entries, (ULONG) DirectModes,
////        TAG_DONE),
////        Child, Label((ULONG)GetMessage(MSG_BUFFERING)),
////        Child, CY_Buffering = OMUINO(MUIC_Cycle,
////          MUIA_Cycle_Entries, (ULONG) Bufferings,
////        TAG_DONE),
////        Child, Label((ULONG)GetMessage(MSG_ROTATION)),
////        Child, CY_Rotation = OMUINO(MUIC_Cycle,
////          MUIA_Cycle_Entries, (ULONG) Rotations,
////        TAG_DONE),
//      TAG_DONE),
//      Child, HSpace(0),
//    TAG_DONE),
//    Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,
//      Child, HSpace(0),
//      Child, UMUINO(MUIC_Group,MUIA_Group_Columns,2,
//        MUIA_HorizWeight, 1000,

//        Child, Label((ULONG)GetMessage(MSG_SCREENMODE)),
//        Child,(ULONG)( PA_ScreenMode = MUI_NewObject(MUIC_Popasl,

//          MUIA_Popstring_String,(ULONG)( DisplayName = MUI_NewObject(MUIC_Text,
//            TextFrame,
//            MUIA_Background, MUII_TextBack,
//          TAG_DONE)),

//          MUIA_Popstring_Button,(ULONG)(PU_ScreenMode = PopButton(MUII_PopUp)),
//          MUIA_Popasl_Type,     ASL_ScreenModeRequest,
//          MUIA_Popasl_StartHook,  (ULONG) &ScreenModeStartHook,
//          MUIA_Popasl_StopHook, (ULONG) &ScreenModeStopHook,
//          MUIA_Disabled, (ULONG) (/*Config[CFG_SCREENTYPE] != CFGST_CUSTOM*/1),
//        TAG_DONE)),
//      TAG_DONE),
//      Child, HSpace(0),
//    TAG_DONE),
//    Child, HVSpace,
//  TAG_DONE);

//  return w;
//}



ULONG createOptionTabGroup()
{
    MameConfig &config = getMainConfig();

    muiConfigCreator("Main",(ASerializable &)config);

    muiConfigCreator.insertFirstPanel(createPanel_Drivers(),"Drivers");

    RE_Options = muiConfigCreator.compile();

    return (ULONG)RE_Options;
}

// at init
void UpdateUIToConfig()
{
    MameConfig &config = getMainConfig();

    // set list is in previous state, from configuration.
    int listShowState = config.driverListstate();
    ShowNotify(NULL, NULL,(ULONG*) &listShowState);

    muiConfigCreator.updateUI();

}

int MainGUI(void)
{
  ULONG rid;
  ULONG v;
  ULONG signals = 0;
  BOOL  loop  = TRUE;

  MameConfig &config = getMainConfig();
//  printf("MainGUI: MUIMasterBase:%08x\n",(int)MUIMasterBase);
//
  if(MUIMasterBase)
  {
    if(!App)
    {
#ifdef DOMAMELOG
    printf("before CreateApp()\n");
#endif
      CreateApp();
#ifdef DOMAMELOG
    printf("after CreateApp()\n");
#endif
    }
//  printf("after CreateApp()\n");
    if(App)
    {
      if(!MainWin)
      {
//static  std::string appName(APPNAME);
//  printf("go MUINewObject()\n");
      //  MainWin =  MUINewObject(MUIC_Window,
        struct TagItem mainwintags[] = {
          {MUIA_Window_Title,(ULONG)APPNAMEA},
          {MUIA_Window_ID   , MAKE_ID('M','A','I','N')},

        {MUIA_Window_Menustrip, UMUINO(MUIC_Menustrip,
            MUIA_Family_Child,UMUINO(MUIC_Menu,MUIA_Menu_Title,(ULONG)GetMessage(MSG_MENU_GAME),
              MUIA_Family_Child, UMUINO(MUIC_Menuitem,
                MUIA_Menuitem_Title,  (ULONG)GetMessage(MSG_MENU_ABOUT),
                MUIA_Menuitem_Shortcut,(ULONG) "?",
                MUIA_UserData,      MID_About,
              TAG_DONE),
              MUIA_Family_Child, UMUINO(MUIC_Menuitem,
                MUIA_Menuitem_Title, (ULONG) "About MUI...",
                MUIA_UserData,      MID_AboutMUI,
              TAG_DONE),
              MUIA_Family_Child, UMUINO(MUIC_Menuitem,
                MUIA_Menuitem_Title,  NM_BARLABEL,
              TAG_DONE),
              MUIA_Family_Child, UMUINO(MUIC_Menuitem,
                MUIA_Menuitem_Title,  (ULONG)GetMessage(MSG_MENU_QUIT),
                MUIA_Menuitem_Shortcut,(ULONG) "Q",
                MUIA_UserData,      MUIV_Application_ReturnID_Quit,
              TAG_DONE),
            TAG_DONE),
          TAG_DONE)},

        {WindowContents, UMUINO(MUIC_Group, // vertical group because no horiz. specified.

            Child,createOptionTabGroup(),

            Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,
              Child, BU_Start   = SimpleButton((ULONG)GetMessage(MSG_START)),
              Child, BU_Quit    = SimpleButton((ULONG)GetMessage(MSG_QUIT)),
            TAG_DONE,0), // end WindowContent Group
          TAG_DONE,0)},
        TAG_DONE,0};
        MainWin =  MUI_NewObjectA(MUIC_Window, (struct TagItem *) &mainwintags[0]);// MUINewObject(MUIC_Window,
//        printf("after MUINewObject():%08x\n",(int)MainWin);
// MUIA_Disabled
        if(MainWin)
        {
          DoMethod(App, OM_ADDMEMBER, MainWin);

          DoMethod(LV_Driver, MUIM_Notify,  MUIA_Listview_DoubleClick, TRUE,
                   App, 2, MUIM_Application_ReturnID,  RID_Start);

          DriverSelectHook.h_Entry = (RE_HOOKFUNC) &DriverSelect;
          DoMethod(LV_Driver, MUIM_Notify,  MUIA_List_Active, MUIV_EveryTime,
                   LV_Driver,3 , MUIM_CallHook,(ULONG) &DriverSelectHook,  MUIV_TriggerValue);

          DoMethod(BU_Start, MUIM_Notify, MUIA_Pressed, FALSE,
                   App, 2, MUIM_Application_ReturnID, RID_Start);

          DoMethod(BU_Quit, MUIM_Notify, MUIA_Pressed, FALSE,
                   App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

          DoMethod(MainWin, MUIM_Notify,  MUIA_Window_CloseRequest, TRUE,
                   App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

//          DoMethod(CM_AutoFrameSkip, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
//                   SL_FrameSkip, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

//          DoMethod(SL_Joy1AutoFireRate, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
//                   SL_Joy1ButtonBTime, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

//          DoMethod(SL_Joy2AutoFireRate, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
//                   SL_Joy2ButtonBTime, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

          DoMethod(MainWin, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
                   App, 2, MUIM_Application_ReturnID, MUIV_TriggerValue);

//          DoMethod(CY_DirectMode, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
//                   CY_DirectMode, 3, MUIM_CallHook, &DirectModeNotifyHook, MUIV_TriggerValue);

//          DoMethod(CY_Sound, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
//                   CY_Sound, 3, MUIM_CallHook, &SoundNotifyHook, MUIV_TriggerValue);

          DoMethod(LI_Driver, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
                   LI_Driver, 3, MUIM_CallHook, &DriverNotifyHook, MUIV_TriggerValue);

          DoMethod(CY_Show, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                   CY_Show, 3, MUIM_CallHook, &ShowNotifyHook, MUIV_TriggerValue);

          DoMethod(BU_Scan, MUIM_Notify, MUIA_Pressed, FALSE,
                   App, 2, MUIM_Application_ReturnID, RID_Scan);


//re, todo but do that better please.

//          DoMethod(MainWin,  MUIM_Window_SetCycleChain,  LV_Driver,

//                   /*CM_UseDefaults,*/ CY_Show, BU_Scan,

//                   BU_Start, BU_Quit, CM_Allow16Bit, CM_FlipX, CM_FlipY, CM_DirtyLines, CM_AutoFrameSkip,
//                   CM_Antialiasing, CM_Translucency, SL_BeamWidth, SL_VectorFlicker, SL_FrameSkip,
//                   ST_Width, ST_Height, CY_ScreenType, CY_DirectMode, CY_Buffering, CY_Rotation,
//                   PU_ScreenMode, CY_Sound, SL_AudioChannel[0], SL_AudioChannel[1], SL_AudioChannel[2],
//                   SL_AudioChannel[3], SL_MinFreeChip, CY_Joy1Type, SL_Joy1ButtonBTime, SL_Joy1AutoFireRate,
//                   CY_Joy2Type, SL_Joy2ButtonBTime, SL_Joy2AutoFireRate, ST_RomPath, PA_RomPath,
//                   ST_SamplePath,/* PA_SamplePath,*/ RE_Options, NULL);

//        printf("before UpdateUIToConfig\n");

         UpdateUIToConfig();
//        printf("after UpdateUIToConfig\n");

        }
        // BU_Start disabled at init
        set(BU_Start,  MUIA_Disabled, TRUE);
      }
      if(MainWin)
      {
        set(MainWin,  MUIA_Window_ActiveObject, (ULONG) LV_Driver);
        set(MainWin,  MUIA_Window_Open,     TRUE);

        /* This must be done after the window has been opened. */
        DoMethod( LI_Driver, MUIM_List_Jump, MUIV_List_Jump_Active);

//printf("before MUI loop\n");
        do
        {
          rid = DoMethod(App,MUIM_Application_NewInput,&signals);
          switch(rid)
          {
            case RID_Start:
          {

              // game rom selected to start !
              get(LI_Driver, MUIA_List_Active, &v);


 //             printf("GUI start:%d \n",(int)v);

              if(v != MUIV_List_Active_Off)
              {
                  int driverptr=0;
                  DoMethod(LI_Driver, MUIM_List_GetEntry, v, &driverptr);

                  if(driverptr) config.setActiveDriver(GetEntryDriverIndex(driverptr));
                  loop = FALSE;

              }
            } // end case
              break;
#ifndef MESS
            case RID_Scan:
             // GetOptions(FALSE);

              DoMethod(LI_Driver, MUIM_List_Clear);

              config.scanDrivers();
              ShowFound();

              break;
#endif
            case MUIV_Application_ReturnID_Quit:
             // GetOptions(TRUE);

              loop = FALSE;

              break;

            case MID_About:
              set(AboutWin, MUIA_Window_Open, TRUE);

              break;

            case RID_CloseAbout:
              set(AboutWin, MUIA_Window_Open, FALSE);

              break;

            case MID_AboutMUI:
              DoMethod(App, MUIM_Application_AboutMUI,  MainWin);

              break;
          }

          if(signals && loop)
          {
            signals = Wait(signals | SIGBREAKF_CTRL_C);

            if(signals & SIGBREAKF_CTRL_C)
              loop = FALSE;
          }
        } while(loop);

        set(AboutWin, MUIA_Window_Open, FALSE);
        set(MainWin,  MUIA_Window_Open, FALSE);

        if(rid == RID_Start)
          return(0);
      }
    }
  }

  return(1);
};

void AboutGUI(void)
{
  ULONG signals = 0;

  if(MUIMasterBase)
  {
    if(!App)
      CreateApp();
    if(App)
    {
      set(AboutWin, MUIA_Window_ActiveObject, (ULONG) BU_About_OK);
      set(AboutWin, MUIA_Window_Open, TRUE);

      for(;DoMethod(App, MUIM_Application_NewInput, &signals) != RID_CloseAbout;)
      {
        if(signals)
        {
          /*if(Inputs)
            signals = Wait(signals|SIGBREAKF_CTRL_C|Inputs->SignalMask);
          else*/
            signals = Wait(signals|SIGBREAKF_CTRL_C);

          if((signals & SIGBREAKF_CTRL_C))
            break;

//          if(signals & Inputs->SignalMask)
//            IUpdate(Inputs);
        }
      }

      set(AboutWin, MUIA_Window_Open, FALSE);
    }
  }
}

inline Object *OHCenter(Object *obj)
{
  return MUI_NewObject(MUIC_Group,
                       MUIA_Group_Horiz,TRUE,
                       MUIA_Group_Spacing,0,
                       Child,(ULONG) MUI_MakeObject(MUIO_HSpace,0),
                       Child, (ULONG)(obj),
                       Child,(ULONG) MUI_MakeObject(MUIO_HSpace,0),
                       TAG_DONE);
}


static void CreateApp(void)
{
  App = MUI_NewObject(MUIC_Application,
    MUIA_Application_Title      , (ULONG)APPNAME,
    //MUIA_Application_Version    , (ULONG)("$VER: " APPNAME " (" REVDATE ")"),
    MUIA_Application_Author     , (ULONG)AUTHOR,
    MUIA_Application_Base       , (ULONG)"MAME",
    SubWindow, (ULONG)(AboutWin = MUI_NewObject(MUIC_Window,
      MUIA_Window_Title, (ULONG)APPNAME,
      MUIA_Window_ID   , MAKE_ID('A','B','O','U'),
      WindowContents, UMUINO(MUIC_Group,
        Child, UMUINO(MUIC_Scrollgroup,
          MUIA_Scrollgroup_FreeHoriz, FALSE,
          MUIA_Scrollgroup_FreeVert, TRUE,
          MUIA_Scrollgroup_Contents, UMUINO(MUIC_Virtgroup,
            VirtualFrame,
            MUIA_Background, MUII_TextBack,
            Child, UMUINO(MUIC_Group,
              Child, UMUINO(MUIC_Text,
                MUIA_Text_Contents, (ULONG)TextAbout,
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
          TAG_DONE),
          TAG_DONE),
        Child,(ULONG) OHCenter(BU_About_OK = SimpleButton((ULONG)"_OK")),
      TAG_DONE), // end group
    TAG_DONE)),
  TAG_DONE);

  if(App)
  {
    DoMethod(BU_About_OK, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, RID_CloseAbout);

    DoMethod(AboutWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             App, 2, MUIM_Application_ReturnID, RID_CloseAbout);
  }
}

static void SetDisplayName(ULONG displayid)
{
    if(!DisplayName) return;
  if(displayid == INVALID_ID)
    DisplayNameBuffer = GetMessage(MSG_INVALID);
  else
  {
    LONG v;
    struct NameInfo DisplayNameInfo;
    v = GetDisplayInfoData(NULL, (UBYTE *) &DisplayNameInfo, sizeof(DisplayNameInfo),
                         DTAG_NAME, displayid);

    if(v > sizeof(struct QueryHeader))
    {
        DisplayNameBuffer = (const char *) DisplayNameInfo.Name;
    }
    char temp[16];
    snprintf(temp,15," (0x%08x)",(int)displayid);
    DisplayNameBuffer += temp;
  }

  set(DisplayName, MUIA_Text_Contents, (ULONG) DisplayNameBuffer.c_str());
}

//static ULONG ASM ScreenModeStart(struct Hook *hook REG(a0), APTR popasl REG(a2), struct TagItem *taglist REG(a1))
//{
//  LONG  i;

//  for(i = 0; taglist[i].ti_Tag != TAG_END; i++);

//  taglist[i].ti_Tag = TAG_MORE;
//  taglist[i].ti_Data  = (ULONG) ScreenModeTags;

//  return(TRUE);
//}

//static ULONG ASM ScreenModeStop(struct Hook *hook REG(a0), APTR popasl REG(a2), struct ScreenModeRequester *smreq REG(a1))
//{
//  ScreenModeTags[SMT_DISPLAYID].ti_Data = smreq->sm_DisplayID;
//  ScreenModeTags[SMT_DEPTH].ti_Data   = smreq->sm_DisplayDepth;

//  SetDisplayName(smreq->sm_DisplayID);

//  return(0);
//}
// when driver just selected, not double clicked


#ifndef MESS
static ULONG ASM ShowNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
  MameConfig &config = getMainConfig();
  DoMethod(LI_Driver, MUIM_List_Clear);

  config.setDriverListState(*par);
  switch(*par)
  {
    case CFGS_ALL:
//      set(BU_Scan, MUIA_Disabled, TRUE);
//      DoMethod(LI_Driver, MUIM_List_Insert, SortedDrivers, NumDrivers + DRIVER_OFFSET, MUIV_List_Insert_Bottom);

      ShowAll();
      break;

    case CFGS_FOUND:
//      set(BU_Scan, MUIA_Disabled, FALSE);
      ShowFound();
      break;
  }

  return(0);
}
#endif

static ULONG ASM DirectModeNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    /*
  if(*par == CFGDM_DRAW)
    set(CM_DirtyLines, MUIA_Disabled, TRUE);
  else
    set(CM_DirtyLines, MUIA_Disabled, FALSE);
*/
  return(0);
}

static ULONG ASM SoundNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
//  switch(*par)
//  {
//    case 1:
//      set(SL_AudioChannel[0], MUIA_Disabled, FALSE);
//      set(SL_AudioChannel[1], MUIA_Disabled, FALSE);
//      set(SL_AudioChannel[2], MUIA_Disabled, FALSE);
//      set(SL_AudioChannel[3], MUIA_Disabled, FALSE);
//      set(SL_MinFreeChip, MUIA_Disabled, FALSE);
//      break;
//    default:
//      set(SL_AudioChannel[0], MUIA_Disabled, TRUE);
//      set(SL_AudioChannel[1], MUIA_Disabled, TRUE);
//      set(SL_AudioChannel[2], MUIA_Disabled, TRUE);
//      set(SL_AudioChannel[3], MUIA_Disabled, TRUE);
//      set(SL_MinFreeChip, MUIA_Disabled, TRUE);
//      break;
//  }

  return(0);
}

static ULONG ASM DriverNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    MameConfig &config = getMainConfig();
   config.setActiveDriver(GetDriverIndex());

  return(0);
}
