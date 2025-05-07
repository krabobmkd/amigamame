
// really hard with this one:
#pragma GCC optimize ("O0")
extern "C" {
    struct Library;
    struct Library *MUIMasterBase=0L;
}

#include "serializer_mui.h"
#include "amiga_locale.h"

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include "intuiuncollide.h"
#include <proto/keymap.h>

#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>
#ifdef __GNUC__
#define REG(r) __asm(#r)
#else
#define REG(r)
#endif

extern "C" {
    #include "gui_mui.h"
    #include <libraries/mui.h>
    #include <libraries/asl.h>
    #include <intuition/classusr.h>
    #include <intuition/classes.h>

    // just for NM_BARLABEL
    #include <libraries/gadtools.h>

    // this one has bugs with -O2
    #include <proto/muimaster.h>
    // we point MUI50 in almigacommonlibs repository ....
    #include <../../MUI50/include/mui/Guigfx_mcc.h>
    #include <../../MUI50/include/mui/GIFAnim_mcc.h>

    #include <devices/timer.h>
}
extern "C" {
    #include "driver.h"
    #include "mamecore.h"
    #include "version.h"
}
#include "amiga106_config.h"
#include "serializer_mui.h"

inline Object * MUINewObject(CONST_STRPTR cl, Tag tags, ...)
{
    return MUI_NewObjectA((char *)cl, (struct TagItem *) &tags);
}


// MUI5 thing
#ifndef MUIA_List_SortColumn
#define MUIA_List_SortColumn                0x8042cafb /* V21 isg LONG              */
#endif
// this is from mui.h, but unreachable from C++ there
#define get(obj,attr,store) GetAttr(attr,(Object *)obj,(ULONG *)store)
#define set(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)

struct DriverData
{
    struct MUI_EventHandlerNode EventHandler;

    char    typedAccum[8];
    UWORD   nbTypedAccum;

    // last typed time
    ULONG           Seconds;
    ULONG           Micros;

  APTR            List;
};


using namespace std;

MUISerializer muiConfigCreator;

typedef ULONG (*RE_HOOKFUNC)(); // because C++ type issue.

#define MUI5_API_SINCE_VERSION  21

#define CFGS_ALL 0
#define CFGS_FOUND 1

#define RID_Start      1
#define RID_CloseAbout 2
#define RID_Scan       3

#define MID_About     104
#define MID_AboutMUI  105


STRPTR ShowCycleValues[] =
{
  (STRPTR)"All",
  (STRPTR)"Found",
  NULL
};

MameUI *_pMameUI=NULL;

inline Object *OHCenter(Object *obj)
{
  return MUINewObject(MUIC_Group,
                       MUIA_Group_Horiz,TRUE,
                       MUIA_Group_Spacing,0,
                       Child,(ULONG) MUI_MakeObject(MUIO_HSpace,0),
                       Child, (ULONG)(obj),
                       Child,(ULONG) MUI_MakeObject(MUIO_HSpace,0),
                       TAG_DONE,0);
}


static inline int GetEntryDriverIndex(ULONG entry)
{
  int index;

  index = (entry - ((ULONG) &drivers[0])) / sizeof(struct _game_driver *);

  return(index);
}

void AllocGUI(void)
{
    MUIMasterBase = OpenLibrary("muimaster.library", 16);
    if(!MUIMasterBase)
    {
        printf(" no MUI interface found\n install MUI or launch a game installed in roms/romname.zip with:\n>Mame106 romname\n\n");
        return;
    }
    if(!_pMameUI)
    {
        _pMameUI = new MameUI;
        _pMameUI->init();
    }

}


MameUI::MameUI(){
}
MameUI::~MameUI() {

    if(App)
      MUI_DisposeObject(App);

    if(DriverClass)
      MUI_DeleteCustomClass(DriverClass);

}

void FreeGUI(void)
{
    if(_pMameUI)
    {
        delete _pMameUI;
        _pMameUI = NULL;
    }
    if(MUIMasterBase)
      CloseLibrary(MUIMasterBase);

}


std::string DisplayNameBuffer;

//log_setCallback
static void muilog(int iwe,const char *pmessage)
{
    if(!_pMameUI) return;
    if(!_pMameUI->LA_statusbar || !pmessage) return;
    // got to treat line by line, here just display last line.
    int linestart=0;
    int i=0;
    while(pmessage[i] != 0)
    {
        if(pmessage[i]=='\n' && pmessage[i+1]!=0) linestart = i+1;
        i++;
    }

    set(_pMameUI->LA_statusbar, MUIA_Text_Contents,(ULONG)(pmessage+linestart));
}


//static struct _game_driver *GetDriver(void)
//{
//  struct _game_driver *drv;

//  ULONG entry;
//  ULONG list_index;

//  get(LI_Driver, MUIA_List_Active, &list_index);

//  if((list_index == (ULONG)MUIV_List_Active_Off) || (list_index < 0))
//    return(NULL);

//  DoMethod((Object*)LI_Driver, MUIM_List_GetEntry, list_index, &entry);

//  if(!entry)
//    return(NULL);

//  drv = *((struct _game_driver **) entry);

//  return(drv);
//}

extern STRPTR ShowCycleValues[];



// LONG __asm cmpfunc(_a1 char *s1,_a2 char *s2)

#define UCheckMark(selected)\
(ULONG)MUINewObject(MUIC_Image,\
                     ImageButtonFrame,\
                     MUIA_InputMode        , MUIV_InputMode_Toggle,\
                     MUIA_Image_Spec       , MUII_CheckMark,\
                     MUIA_Image_FreeVert   , TRUE,\
                     MUIA_Selected         , selected,\
                     MUIA_Background       , MUII_ButtonBack,\
                     MUIA_ShowSelState     , FALSE,\
                     TAG_DONE)
#define OCheckMark(selected)\
    MUINewObject(MUIC_Image,\
                 ImageButtonFrame,\
                 MUIA_InputMode        , MUIV_InputMode_Toggle,\
                 MUIA_Image_Spec       , MUII_CheckMark,\
                 MUIA_Image_FreeVert   , TRUE,\
                 MUIA_Selected         , selected,\
                 MUIA_Background       , MUII_ButtonBack,\
                 MUIA_ShowSelState     , FALSE,\
                 TAG_DONE)
#define OString(contents,maxlen)\
    MUINewObject(MUIC_String,\
                 StringFrame,\
                 MUIA_String_MaxLen  , maxlen,\
                 MUIA_String_Contents, contents,\
                 TAG_DONE)



    Object *MameUI::createPanel_Drivers()
{
    const char *ListFormat = "BAR,BAR,BAR,BAR,BAR,BAR,";
    if(MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
    {
        ListFormat = "SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,";
    }

    if(DriverClass!=NULL)
    {
        LV_Driver = (Object *) NewObject(DriverClass->mcc_Class, NULL,
                                         MUIA_Listview_Input,    TRUE,
                                         MUIA_Listview_List, (ULONG)( LI_Driver = MUINewObject(MUIC_List,
                                                                           MUIA_List_Title,    TRUE,
                                                                           MUIA_List_Format,  (ULONG)ListFormat ,
                                                                           MUIA_List_DisplayHook,(ULONG)  &DriverDisplayHook,
                                                                           MUIA_List_CompareHook,(ULONG)  &DriverSortHook,
                                                                           InputListFrame,
                                                                           TAG_DONE)),
                                         TAG_END);
    } else
    {
        LV_Driver = (Object *)MUINewObject(MUIC_Listview,
                                            MUIA_Listview_Input, TRUE,
                                            MUIA_Listview_List, (ULONG)( LI_Driver = MUINewObject(MUIC_List,
                                                                              MUIA_List_Title, TRUE,
                                                                              InputListFrame,
                                                                              MUIA_List_Format,(ULONG)ListFormat,
                                                                              MUIA_List_DisplayHook,(ULONG)  &DriverDisplayHook,
                                                                              MUIA_List_CompareHook,(ULONG)  &DriverSortHook,

                                                                              TAG_DONE)),
                                            TAG_DONE);
    }
    if(!LV_Driver) exit(1);

    Object *panel = MUINewObject(MUIC_Group,
     Child,(ULONG)( LV_Driver )
     ,
     Child, (ULONG)MUINewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
              Child,(ULONG) Label((ULONG)GetMessagec("Show")),
              Child, (ULONG) (CY_Show = MUINewObject(MUIC_Cycle,
                  MUIA_Cycle_Entries, (ULONG) ShowCycleValues,
                  TAG_DONE)),
              Child,(ULONG)(BU_Scan = SimpleButton((ULONG)GetMessagec("Scan"))),
              TAG_DONE),
     TAG_DONE);

    return panel;
}


ULONG MameUI::createOptionTabGroup()
{
    MameConfig &config = getMainConfig();

    muiConfigCreator("Main",(ASerializable &)config);

    muiConfigCreator.insertFirstPanel(createPanel_Drivers(),GetMessagec("Drivers"));

    RE_Options = muiConfigCreator.compile();

    return (ULONG)RE_Options;
}


std::stringstream drivdispdbg;

int MameUI::MainGUI(void)
{
    ULONG rid;
    ULONG v;
    ULONG signals = 0;
    BOOL  loop  = TRUE;


    MameConfig &config = getMainConfig();
    //  printf("MainGUI: MUIMasterBase:%08x\n",(int)MUIMasterBase);

    if(MUIMasterBase)
    {
        if(!App)
        {
            CreateApp();
        }

        if(App)
        {
            if(!MainWin)
            {

                if(MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
                {
#define SCALEMODEMASK(u, d, p, s)	(((u) ? NISMF_SCALEUP : 0) | ((d) ? NISMF_SCALEDOWN : 0) | ((p) ? NISMF_KEEPASPECT_PICTURE : 0) | ((s) ? NISMF_KEEPASPECT_SCREEN : 0))
#define TRANSMASK(m, r)				(((m) ? NITRF_MASK : 0 ) | ((r) ? NITRF_RGB : 0))
                    // will be NULL if MUI<5, but just invalid if file not found.
                    const char *giffilepath = "PROGDIR:skin/cornerlogo.gif";
                    int exists=0;
                    BPTR hdl = Open(giffilepath, MODE_OLDFILE);
                    exists = (hdl!=NULL);
                    if(hdl) Close(hdl);
                    if(exists)
                    {
                        GIF_cornerlogo =
                            MUINewObject(MUIC_GIFAnim,MUIA_GIFAnim_File,(ULONG) giffilepath, TAG_DONE);
                    }
                }
                ULONG Child_Gif = (GIF_cornerlogo)?Child:TAG_DONE;

                Object *windowContent = MUINewObject(MUIC_Group, // vertical group because no horiz. specified.
                                                     // MUIA_Group_HorizSpacing,0,
                                                     // MUIA_Group_VertSpacing,0,
                                                     Child,createOptionTabGroup(),

                                                     Child, (ULONG)MUINewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                                                                          MUIA_Group_HorizSpacing,0,
                                                                          MUIA_Group_VertSpacing,0,
                                                                          //Child,(ULONG)(LA_statusbar = MLLabel((ULONG)GetMessagec("-"))),
                                                                          //Child,(ULONG)(BU_Start   = SimpleButton((ULONG)GetMessagec("Start"))),
                                                                          Child, (ULONG)MUINewObject(MUIC_Group,
                                                                                               MUIA_Group_HorizSpacing,0,
                                                                                               Child, BU_Start   = SimpleButton((ULONG)GetMessagec("Start")),
                                                                                               //  Child, MUI_MakeObject(MUIO_VSpace,0),
                                                                                               Child, LA_statusbar = LLabel((ULONG)GetMessagec("-")),
                                                                                               TAG_DONE,0),
                                                                          Child_Gif,(ULONG)GIF_cornerlogo,
                                                                          //olde    Child, BU_Quit    = SimpleButton((ULONG)GetMessagec("Quit")),
                                                                          TAG_DONE,0), // end WindowContent Group
                                                     TAG_DONE,0);

                //static  std::string appName(APPNAME);
                //  printf("go MUINewObject()\n");
                //  MainWin =  MUINewObject(MUIC_Window,
                struct TagItem mainwintags[] = {
                                                {MUIA_Window_Title,(ULONG)APPNAMEA},
                                                {MUIA_Window_ID   , MAKE_ID('M','A','I','N')},

                                                {MUIA_Window_Menustrip, (ULONG)MUINewObject(MUIC_Menustrip,
                                                                                             MUIA_Family_Child,(ULONG)MUINewObject(MUIC_Menu,MUIA_Menu_Title,(ULONG)GetMessagec("Menu"),
                                                                                                                  MUIA_Family_Child, (ULONG)MUINewObject(MUIC_Menuitem,
                                                                                                                                       MUIA_Menuitem_Title,  (ULONG)GetMessagec("About"),
                                                                                                                                       MUIA_Menuitem_Shortcut,(ULONG) "?",
                                                                                                                                       MUIA_UserData,      MID_About,
                                                                                                                                       TAG_DONE),
                                                                                                                  MUIA_Family_Child, (ULONG)MUINewObject(MUIC_Menuitem,
                                                                                                                                       MUIA_Menuitem_Title,  (ULONG)GetMessagec("About MUI..."),
                                                                                                                                       MUIA_UserData,      MID_AboutMUI,
                                                                                                                                       TAG_DONE),
                                                                                                                  MUIA_Family_Child, (ULONG)MUINewObject(MUIC_Menuitem,
                                                                                                                                       MUIA_Menuitem_Title,  NM_BARLABEL,
                                                                                                                                       TAG_DONE),
                                                                                                                  MUIA_Family_Child, (ULONG)MUINewObject(MUIC_Menuitem,
                                                                                                                                       MUIA_Menuitem_Title,  (ULONG)GetMessagec("Quit"),
                                                                                                                                       MUIA_Menuitem_Shortcut,(ULONG) "Q",
                                                                                                                                       MUIA_UserData,      MUIV_Application_ReturnID_Quit,
                                                                                                                                       TAG_DONE),
                                                                                                                  TAG_DONE),
                                                                                             TAG_DONE)},

                                                {WindowContents, (ULONG)windowContent},
                                                TAG_DONE,0};

                MainWin =  MUI_NewObjectA(MUIC_Window, (struct TagItem *) &mainwintags[0]);// MUINewObject(MUIC_Window,

                    //        printf("after MUINewObject():%08x\n",(int)MainWin);
                    // MUIA_Disabled

                    if(MainWin)
                {
                    DoMethod(App, OM_ADDMEMBER, MainWin);

                    DoMethod(LV_Driver, MUIM_Notify,  MUIA_Listview_DoubleClick, TRUE,
                             App, 2, MUIM_Application_ReturnID,  RID_Start);

                    DoMethod(LV_Driver, MUIM_Notify,  MUIA_List_Active, MUIV_EveryTime,
                             LV_Driver,3 , MUIM_CallHook,(ULONG) &DriverSelectHook,  MUIV_TriggerValue);

                    DoMethod(BU_Start, MUIM_Notify, MUIA_Pressed, FALSE,
                             App, 2, MUIM_Application_ReturnID, RID_Start);

                    //          DoMethod(BU_Quit, MUIM_Notify, MUIA_Pressed, FALSE,
                    //                   App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

                    DoMethod(MainWin, MUIM_Notify,  MUIA_Window_CloseRequest, TRUE,
                             App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


                    DoMethod(MainWin, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
                             App, 2, MUIM_Application_ReturnID, MUIV_TriggerValue);

                    DoMethod(LI_Driver, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
                             LI_Driver, 3, MUIM_CallHook, &DriverNotifyHook, MUIV_TriggerValue);

                    //          if(MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
                    //          {

                    //               // for MUI5 column sorting, listen the selected column attrib:
                    //               DoMethod(LI_Driver, MUIM_Notify, MUIA_List_SortColumn, MUIV_EveryTime,
                    //                   LI_Driver, 3, MUIM_CallHook, &DriverSortColumnNotifyHook, MUIV_TriggerValue);
                    //          }

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

                    log_setCallback(muilog);

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

               // printf("before MUI loop\n");
                do
                {          
                    std::string strdbg = drivdispdbg.str();
                    drivdispdbg = std::stringstream();
                    if(strdbg.size()>0) printf("%s",strdbg.c_str());

                    rid = DoMethod(App,MUIM_Application_NewInput,&signals);
                    switch(rid)
                    {
                    case RID_Start:
                    {

                        // game rom selected to start !
                        get(LI_Driver, MUIA_List_Active, &v);


                        //             printf("GUI start:%d \n",(int)v);

                        if(v != (ULONG)MUIV_List_Active_Off)
                        {
                            int driverptr=0;
                            DoMethod(LI_Driver, MUIM_List_GetEntry, v, &driverptr);

                            if(driverptr) config.setActiveDriver(GetEntryDriverIndex(driverptr));
                            loop = FALSE;

                        }
                    } // end case
                    break;

                    case RID_Scan:
                        // GetOptions(FALSE);

                        DoMethod(LI_Driver, MUIM_List_Clear);

                        config.scanDrivers();
                        ShowFound();

                        break;
                    case MUIV_Application_ReturnID_Quit:

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

void MameUI::AboutGUI(void)
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



static ULONG DriverDisplay(struct Hook *hook REG(a0), char **array REG(a2),const struct _game_driver **drv_indirect REG(a1));
static ULONG DriverSort(const struct _game_driver **drva REG(a1), const struct _game_driver **drvb REG(a2) );
static ULONG DriverNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
static ULONG ShowNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
static ULONG DriverSelect(struct Hook *hook REG(a0), APTR obj REG(a2), LONG *par REG(a1));
static ULONG DriverDispatcher(struct IClass *cclass REG(a0), Object * obj REG(a2), Msg msg REG(a1));
static ULONG DriverDispatcherMUI5(struct IClass *cclass REG(a0), Object * obj REG(a2), Msg msg REG(a1));









static int DriverCompareNames(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return(stricmp((*drv1)->description, (*drv2)->description));
}
static int DriverCompareNbPlayers(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  int a = (*drv1)->nbplayers;
  int b = (*drv2)->nbplayers;

    return (b-a);
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


static int columnToSort = 0;
static ULONG DriverSort(
    const struct _game_driver **drva REG(a1),
    const struct _game_driver **drvb REG(a2)
    )
{   // must return -1 0 1


    int icolumnToSort;
    if(MUIMasterBase->lib_Version<MUI5_API_SINCE_VERSION)
    {
        icolumnToSort = columnToSort;
    } else
    {
        get(_pMameUI->LI_Driver, MUIA_List_SortColumn, &icolumnToSort);
    }

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
static ULONG DriverDisplay(struct Hook *hook REG(a0), char **array REG(a2),const struct _game_driver **drv_indirect REG(a1))
{
    MameUI *ui = (MameUI *) hook->h_Data;
    MameConfig &config = getMainConfig();
    const struct _game_driver *drv;

    struct ColumnsString {
          char *_driver,*_players,*_screen,*_year,*_archive,*_parent,*_comment;
    };
    ColumnsString *pColumns = (ColumnsString *)array;

  static char driver[64];
  static char screen[32];
  static char archive[16];
  static char parent[16];
 static char players[16];
 static char year[12];
//  static char comment[128];
 static std::string strComment;
  if(!drv_indirect)
  {
    snprintf(driver,55,   "\033b\033u%s", ui->String_Driver);
    driver[55]=0;
    snprintf(screen,31,   "\033b\033u%s", ui->String_Screen);
    screen[31]=0;
    snprintf(archive,15,  "\033b\033u%s", ui->String_Archive);
    archive[15]=0;
    snprintf(parent,15,  "\033b\033u%s", ui->String_Parent);
    parent[15]=0;
    snprintf(players,15,    "\033b\033u%s", ui->String_Players);
    players[15]=0;
    snprintf(year,11,    "\033b\033u%s", ui->String_Year);
    year[11]=0;
   // snprintf(comment,127,  "\033b\033u%s", String_Comment);
   // comment[127]=0;
    strComment =  "\033b\033u";
    strComment += ui->String_Comment;

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

  const char *pnotworking =
      (drv->flags & GAME_NOT_WORKING) ?
         (ui->NotWorkingString) : "";

 if(config.isDriverFound(drv_indirect))
 {
     // if found: to bold
    snprintf(driver,63,"\033b%s %s", drv->description,pnotworking);
    driver[63]=0;
    pColumns->_driver = driver;
 } else
 {  // just to cut too long names
     snprintf(driver,63,"%s %s", drv->description,pnotworking);
     driver[63]=0;
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
      strComment = ui->NotWorkingString;
     else if(drv->flags & GAME_WRONG_COLORS)
      strComment = ui->WrongColorsString;
     else if(drv->flags & GAME_IMPERFECT_COLORS)
      strComment = ui->ImperfectColorsString;
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


//  Else is in timer device SubTime().
static inline timeval subtime( const timeval &a,const timeval &b)
{
    timeval dest = a;

    if (dest.tv_micro < b.tv_micro)
    {
        dest.tv_micro += 1000000;
        dest.tv_secs--;
    }
    dest.tv_micro -= b.tv_micro;
    dest.tv_secs  -= b.tv_secs;
    return dest;
}
static inline int nbcharOk(const char *pphrase,const char *ppattern)
{
    int i=0;
    while(*pphrase != 0 && *ppattern != 0)
    {
        char a = *pphrase;
        if(a>='A' && a<='Z') a+=32;

        if(a == *ppattern) i++;
        pphrase++;
        ppattern++;
    }
    return i;
}


// extend list class
static ULONG DriverDispatcher(struct IClass *cclass REG(a0), Object * obj REG(a2), Msg msg REG(a1))
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
//          if(MapRawKey(&ie,(STRPTR)&key, 1, NULL) && isalnum(key))
//          {
//            i = imsg->Seconds - data->Seconds;

//            if(imsg->Micros < data->Micros)
//              i--;

//            if(i < 1)
//            {
//              data->CharIndex++;
//              i = data->CurrentEntry;
//            }
//            else
//            {
//              data->CharIndex = 0;
//              i = 0;
//            }

//            data->Seconds = imsg->Seconds;
//            data->Micros  = imsg->Micros;

//            get(obj, MUIA_Listview_List, &list);

//            do
//            {
//              DoMethod(list, MUIM_List_GetEntry, i, &drv_indirect);

//              if(drv_indirect)
//              {
//                drv = *drv_indirect;

//                if(data->CharIndex < strlen(drv->description))
//                {
//                  if(key <= tolower(drv->description[data->CharIndex]))
//                  {
//                    data->CurrentEntry = i;

//                    set(list, MUIA_List_Active, i);

//                    break;
//                  }
//                }
//              }

//              i++;

//            } while(drv_indirect);

//            return(MUI_EventHandlerRC_Eat);
//          }
        }
      }
      return(0);
      //break;
      case MUIM_HandleInput: // krb added
        {
	#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
	#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))

            // if MUI<5, use this to get selected column.
            // if MUI>=5 use MUIA_List_SortColumn
             if(MUIMasterBase->lib_Version<MUI5_API_SINCE_VERSION)
             {
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
                           // finnaly, propagate. return(MUI_EventHandlerRC_Eat);
                        }
                    }
                }
                // Class
            }
//            if (msghi->muikey!=MUIKEY_NONE)
//            {
//                switch (msg->muikey)
//                {
//                    case MUIKEY_LEFT : data->sx=-1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                    case MUIKEY_RIGHT: data->sx= 1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                    case MUIKEY_UP   : data->sy=-1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                    case MUIKEY_DOWN : data->sy= 1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
//                }
//            }
            }

        } // end case
      break;
  }

  return(DoSuperMethodA(cclass, obj, msg));
}

static ULONG DriverDispatcherMUI5(struct IClass *cclass REG(a0), Object * obj REG(a2), Msg msg REG(a1))
{
  struct DriverData   *data;
  struct IntuiMessage *imsg;
  struct _game_driver   **drv_indirect;
  struct _game_driver   *drv;
  struct InputEvent   ie;

  Object  *list;
  APTR  active_obj;
  ULONG i;


//printf("msg->MethodID:%08x\n",msg->MethodID);
  switch(msg->MethodID)
  {
    case MUIM_Setup:
      data = (struct DriverData *)INST_DATA(cclass, obj);

      if(DoSuperMethodA(cclass, obj, msg))
      {
        data->Seconds = 0;
        data->Micros  = 0;
        data->nbTypedAccum = 0;

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
    {
      data =  (struct DriverData *) INST_DATA(cclass, obj);
      imsg = (struct IntuiMessage *) msg[1].MethodID;

// printf("MUIM_HandleEvent\n");
//      int vis;
//      get(obj, MUIA_List_Visible, &vis);
//      if(vis) break;

      get(_win(obj), MUIA_Window_ActiveObject, &active_obj);
      get(obj, MUIA_Listview_List, &list);

      if(obj == active_obj || list == active_obj)
      {
        if(imsg->Class == IDCMP_RAWKEY)
        {

          ie.ie_Class   = IECLASS_RAWKEY;
          ie.ie_SubClass  = 0;
          ie.ie_Code    = imsg->Code;
          ie.ie_Qualifier = 0;

          char key[8];
//  MapRawKey( CONST struct InputEvent *event, STRPTR buffer, LONG length, CONST struct KeyMap *keyMap );
          if(MapRawKey(&ie,(STRPTR)&key[0], 4, NULL) && isalnum(key[0]))
          {
            timeval diftime = subtime({imsg->Seconds,imsg->Micros},{data->Seconds,data->Micros});

            // addchar if time dif less than 0.8 sec, else restart.
            int doAddChar=( diftime.tv_sec==0 && diftime.tv_micro< (1000000) );

            data->Seconds = imsg->Seconds;
            data->Micros = imsg->Micros;

            if(doAddChar)
            {
                if(data->nbTypedAccum<4)
                {
                    data->typedAccum[data->nbTypedAccum] = key[0];
                    data->nbTypedAccum++;
                    data->typedAccum[data->nbTypedAccum] = 0;
                }
            } else
            {
                data->typedAccum[0] = key[0];
                data->typedAccum[1] = 0;
                data->nbTypedAccum = 1;
            }

//             char temp[64];
//             snprintf(temp,63,"key:%c\n",(char)key[0]);
//             drivdispdbg << temp;

            i = 0;
            int bestIndex = -1;
            int bestIndexScore = 0;
            // really loop anything (posibly 2000 items). It's one by key stroke anyway.
            // list items can be sorted in any manner.
            do
            {
              DoMethod(list, MUIM_List_GetEntry, i, &drv_indirect);
              if(! drv_indirect) break;

              drv = *drv_indirect;
              int nbcharok = nbcharOk(drv->description,data->typedAccum);
              if(nbcharok>bestIndexScore)
              {
                bestIndexScore = nbcharok;
                bestIndex = i;
              }

              i++;

            } while(1);

            if(bestIndex>=0)
            {
               set(list, MUIA_List_Active, bestIndex);
               return(MUI_EventHandlerRC_Eat);
            }
          }

        }
      }
    }
    //  return(0);
      break;
  }
  return(DoSuperMethodA(cclass, obj, msg));
}

static ULONG DriverNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    MameUI *ui = (MameUI *) hook->h_Data;
    MameConfig &config = getMainConfig();
   config.setActiveDriver(ui->GetDriverIndex());

  return(0);
}
static ULONG ShowNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    MameUI *ui = (MameUI *) hook->h_Data;
    ui->setDriverListShowState((int)*par);

  return(0);
}

static ULONG DriverSelect(struct Hook *hook REG(a0), APTR obj REG(a2), LONG *par REG(a1))
{
    if(!par || !hook)return 0;
    MameUI *ui = (MameUI *) hook->h_Data;

    MameConfig &config = getMainConfig();
//    printf("DriverSelect:%d\n",*par);
    // get(LI_Driver, MUIA_List_Active, &v);
    int listindex = *par;
    if(listindex>=0)
    {
        const game_driver **ppdrv=NULL; // int entry; // driver ptr ptr
        DoMethod((Object*)ui->LI_Driver,(ULONG) MUIM_List_GetEntry, (ULONG)listindex, (ULONG)&ppdrv);
        if(ppdrv && *ppdrv)
        {
            //printf("DriverSelect:%s\n",(*ppdrv)->name);
            std::string screenconf;           
            int videoAttribs;
            config.getDriverScreenModestring(ppdrv, screenconf,videoAttribs);

            muiConfigCreator.selectGroup("Display.Per Screen Mode",screenconf);
            muiConfigCreator.selectGroup("Display.Per Game",(*ppdrv)->name);

            set(ui->BU_Start,  MUIA_Disabled, FALSE);
            //   printf("screen:%s:\n",screenconf.c_str());

        }
    }
  return(0);
}
void MameUI::setDriverListShowState(int state)
{
  MameConfig &config = getMainConfig();

    if(LI_Driver)  DoMethod(LI_Driver, MUIM_List_Clear);

  config.setDriverListState(state);
  switch(state)
  {
    case CFGS_ALL:
      ShowAll();
      break;

    case CFGS_FOUND:
      ShowFound();
      break;
  }

}
// at init
void MameUI::UpdateUIToConfig()
{
    MameConfig &config = getMainConfig();

    // set list is in previous state, from configuration.
    int listShowState = config.driverListstate();
    setDriverListShowState(listShowState);

    muiConfigCreator.updateUI();
}


int MameUI::GetDriverIndex(void)
{
  ULONG entry;
  ULONG list_index;
  int   index;

  get(_pMameUI->LI_Driver, MUIA_List_Active, &list_index);

  if(list_index == (ULONG) MUIV_List_Active_Off)
  {
    index = - 1;
  }
  else if(list_index < 0)
  {
    index = list_index;
  }
  else
  {
    DoMethod((Object*)_pMameUI->LI_Driver,(ULONG) MUIM_List_GetEntry, (ULONG)list_index, (ULONG)&entry);

    index = GetEntryDriverIndex(entry);
  }

  return(index);
}

void MameUI::ShowFound(void)
{
    MameConfig &config = getMainConfig();
    const std::vector<const _game_driver *const*> &roms = config.romsFound();
    //MUIM_List_Insert can insert everything in a blow.
        DoMethod((Object *)_pMameUI->LI_Driver, MUIM_List_Insert,
         (ULONG)roms.data(),(int)roms.size(),  /*MUIV_List_Insert_Bottom*/MUIV_List_Insert_Sorted);

// no need because MUIV_List_Insert_Sorted (would ressort):   DoMethod(LI_Driver,MUIM_List_Sort);

    // ensure cycle is in correct state
    int cyclestate=0;
    get(_pMameUI->CY_Show, MUIA_Cycle_Active, &cyclestate);
    if(cyclestate != 1)
    {
        set(_pMameUI->CY_Show, MUIA_Cycle_Active,1);
    }


}
void MameUI::ShowAll(void)
{
    MameConfig &config = getMainConfig();
    std::vector<const _game_driver *const*> roms;
    config.buildAllRomsVector(roms);

    //MUIM_List_Insert can insert everything in a blow.
        DoMethod(_pMameUI->LI_Driver, MUIM_List_Insert,
         (ULONG)roms.data(),(int)roms.size(),  /*MUIV_List_Insert_Bottom*/MUIV_List_Insert_Sorted);

   // no need because MUIV_List_Insert_Sorted does the job: DoMethod(LI_Driver,MUIM_List_Sort);

    // ensure cycle is in correct state
    int cyclestate=0;
    get(_pMameUI->CY_Show, MUIA_Cycle_Active, &cyclestate);
    if(cyclestate != 0)
    {
        set(_pMameUI->CY_Show, MUIA_Cycle_Active,0);
    }
}


int MameUI::init()
{
    for(int i = 0; i<(int)(sizeof(ShowCycleValues)/sizeof(STRPTR)) -1; i++)
      ShowCycleValues[i] = (char *) GetMessagec(ShowCycleValues[i]);

    DriverDisplayHook.h_Entry    = (RE_HOOKFUNC) DriverDisplay;
    DriverDisplayHook.h_Data = (APTR)this;
    DriverSortHook.h_Entry    = (RE_HOOKFUNC) DriverSort;
    DriverSortHook.h_Data = (APTR)this;
    DriverNotifyHook.h_Entry     = (RE_HOOKFUNC) DriverNotify;
    DriverNotifyHook.h_Data = (APTR)this;
    ShowNotifyHook.h_Entry        = (RE_HOOKFUNC) ShowNotify;
    ShowNotifyHook.h_Data = (APTR)this;
    DriverSelectHook.h_Entry = (RE_HOOKFUNC) &DriverSelect;
    DriverSelectHook.h_Data = (APTR)this;

    // - - - -possibly locale managed strings for list
    NotWorkingString         = GetMessagec("Not working");
    WrongColorsString        = GetMessagec("Wrong colors");
    ImperfectColorsString    = GetMessagec("Imperfect colors");

    String_Driver=GetMessagec("Driver");
    String_Archive=GetMessagec("Archive");
    String_Parent=GetMessagec("Parent");
    String_Screen=GetMessagec("Screen");
    String_Players=GetMessagec("Players");
    String_Year=GetMessagec("Year");
    String_Comment=GetMessagec("Comment");

    if(MUIMasterBase->lib_Version<MUI5_API_SINCE_VERSION)
    {
        DriverClass = MUI_CreateCustomClass(NULL, MUIC_Listview, NULL, sizeof(struct DriverData),(APTR) DriverDispatcher);
    } else
    {
        DriverClass = MUI_CreateCustomClass(NULL, MUIC_Listview, NULL, sizeof(struct DriverData),(APTR) DriverDispatcherMUI5);
    }

    return 0;
}


std::string sTextAbout;
const char *staticGuiAppId = "MAME";
void MameUI::CreateApp(void)
{
    if(App) return; // already ok

    if(sTextAbout.length()==0)
    {
        sTextAbout =
            "\33c\n\33b\33uMAME - Multiple Arcade Machine Emulator\33n\n\n"
            "0." REVISION "\n\n"
            "Copyright (C) 1997-2025 by Nicola Salmoria and the MAME team\n"
            "http://mamedev.org\n\n"
            "Amiga port by Vic 'Krb' Ferry (2025) source:\n"
            " https://github.com/krabobmkd/amigamame\n"
            "compiled branch:" GIT_BRANCH " " REVDATE " " REVTIME "\n"
            "Partly based on Mats Eirik Hansen Mame060(1999)\n"
            " http://www.triumph.no/mame\n"
            "This program uses libexpat,zlib,\n"
            " MUI - Magic User Interface\n";
        if(MUIMasterBase->lib_Version<MUI5_API_SINCE_VERSION)
        {
            sTextAbout += "Upgrade to MUI5 for a better experience at:\n"
                        "https://github.com/amiga-mui/muidev/releases\n";
        }
    }

Object *ins2 = MUINewObject(MUIC_Text,
                MUIA_Text_Contents, (ULONG)sTextAbout.c_str(),
              TAG_DONE,0);

 Object *inner=MUINewObject(MUIC_Group,
              Child, (ULONG)ins2,
              Child, VSpace(0),
            TAG_DONE,0);

 Object *virtgroup = MUINewObject(MUIC_Virtgroup,
            VirtualFrame,
            MUIA_Background, MUII_TextBack,
            Child, (ULONG)inner,
          TAG_DONE,0);

 BU_About_OK = SimpleButton((ULONG)"_OK");
 Object *objout2 = MUINewObject(MUIC_Group,
        Child, (ULONG)MUINewObject(MUIC_Scrollgroup,
          MUIA_Scrollgroup_FreeHoriz, FALSE,
          MUIA_Scrollgroup_FreeVert, TRUE,
          MUIA_Scrollgroup_Contents,(ULONG)virtgroup ,
          TAG_DONE,0),
        Child,(ULONG) OHCenter(BU_About_OK),
      TAG_DONE,0);

 AboutWin = MUINewObject(MUIC_Window,
      MUIA_Window_Title, (ULONG)APPNAME,
      MUIA_Window_ID   , MAKE_ID('A','B','O','U'),
      WindowContents, (ULONG)objout2, // end group
    TAG_DONE,0);

  App = MUINewObject(MUIC_Application,
    MUIA_Application_Title      , (ULONG)APPNAME,
    //MUIA_Application_Version    , (ULONG)("$VER: " APPNAME " (" REVDATE ")"),
    MUIA_Application_Author     , (ULONG)AUTHOR,
    MUIA_Application_Base       , (ULONG)staticGuiAppId,
    SubWindow, (ULONG)AboutWin,
  TAG_DONE,0);

  if(App)
  {
    DoMethod(BU_About_OK, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, RID_CloseAbout);

    DoMethod(AboutWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             App, 2, MUIM_Application_ReturnID, RID_CloseAbout);
  }

}




