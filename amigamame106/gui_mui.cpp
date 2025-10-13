
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
#include <proto/icon.h>
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
    // we point MUI50 in amigacommonlibs repository ....
    #include <../../MUI50/include/mui/Guigfx_mcc.h>
    #include <../../MUI50/include/mui/GIFAnim_mcc.h>

    #include <devices/timer.h>
}
extern "C" {
    #include "driver.h"
    #include "mamecore.h"
    #include "version.h"

    extern struct DiskObject *AppDiskObject;
}
#include "amiga106_config.h"
#include "serializer_mui.h"

inline Object * MUINewObject(CONST_STRPTR cl, Tag tags, ...)
{
    return MUI_NewObjectA((char *)cl, (struct TagItem *) &tags);
}
extern "C" {
    extern int verbose;
}

// MUI5 thing
#ifndef MUIA_List_SortColumn
#define MUIA_List_SortColumn                0x8042cafb /* V21 isg LONG              */
#endif
#ifndef MUIA_List_ColumnOrder
// V20
#define MUIA_List_ColumnOrder               0x9d5100f6
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

#define MID_START_FILTER_MENU 256

#define MENU_CODE_START (MID_START_FILTER_MENU)
#define MENU_CODE_TAGSTART (MID_START_FILTER_MENU+256)
#define MENU_CODE_ALL (MID_START_FILTER_MENU+512)
#define MENU_CODE_NONE (MID_START_FILTER_MENU+513)
#define MENU_CODE_END (MID_START_FILTER_MENU+514)


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
    if(verbose) printf("try init MUI\n");

    MUIMasterBase = OpenLibrary("muimaster.library", 16);    
    if(!MUIMasterBase)
    {
        printf(" no MUI interface found\n install MUI or launch a game installed in roms/romname.zip with:\n>Mame106 romname\n\n");
        return;
    }
    if(!_pMameUI)
    {
        if(verbose) printf("init MUI, found version: %d.%d\n",MUIMasterBase->lib_Version,MUIMasterBase->lib_Revision);

        _pMameUI = new MameUI;
        _pMameUI->init();
        if(verbose) printf("after init MUI\n");
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
    const char *ListFormat = "BAR,BAR,BAR,BAR,BAR,BAR,BAR,";
    if(MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
    {
        ListFormat = "SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,SORTABLE BAR,";
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
                                                                            MUIA_CycleChain, TRUE,
                                                                            MUIA_ObjectID, MAKE_ID('D','L','S','T'),
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
                                                                                MUIA_CycleChain, TRUE,
                                                                                MUIA_ObjectID, MAKE_ID('D','L','S','T'),
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


static const char *genreNames[(int)egg_NumberOfGenre]={
    "", // unknown

    "Platform",
    "Climbing",

    "ShootEmUp",
    "Shooter", // actually pang or some joystick gun games..(?)

    "BeatNUp", // cooperative
    "Fighter", // versus
    "Driving", // would have tag 3D
    "Motorcycle",
    "Flying",
    "LightGuns",
    "BallNPaddles",
    "Pinballs",
    "Maze",

    "Tabletop",
    "Puzzle",
    "Card Battle",
    "Mahjong",
    "Quizz",
    "Rock Paper Scissors", // added for scud hammer

    "Casino",
    "HorseRacing",
    "PoolNDart",

    "Other Sport",
    "Baseball",
    "Basketball",
    "Volleyball",
    "Football",
    "Soccer",
    "Golf",
    "Hockey",
    "Rugby",
    "Tennis",
    "TrackNField",
    "Boxing",
    "Wrestling",

    "Bowling",
    "Skiing",
    "Skate",
    "Rythm",
    "Fishing",

    "Compilation",
    "Miscellaneous",
    "Mature",
    "Demoscene"
};
//#define nbGenreNames (sizeof(genreNames)/sizeof(char *))

// keep order of EGF_ tags
static const char *genreTagNames[]={
  " Horizontal",
  " Vertical",
  " 3D",
  " Isometric",
  " Girly",
  " Childish",
  " Sexy",
  " Funny",
  " Ultra Violent",
  " Hentai"
};




Object *MameUI::MenuGenreFilter()
{

    std::vector<ULONG> v={
        MUIA_Menu_Title,(ULONG)GetMessagec("List Filter"),
        MUIA_Family_Child,(ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)GetMessagec("All"),0,0,MENU_CODE_ALL),
        MUIA_Family_Child,(ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)GetMessagec("None"),0,0,MENU_CODE_NONE),
      //  MUIA_Family_Child,(ULONG)MUINewObject(MUIC_Menuitem, MUIA_Menuitem_Title,  NM_BARLABEL, TAG_DONE),


    };
// https://github.com/amiga-mui/muidev/wiki/MUI_Menuitem#MUIA_Menuitem_Toggle
    for(int i=1;i<(int)egg_NumberOfGenre ;i++)
    {
        if(i==(int) egg_Platform)
        {
            v.push_back(MUIA_Family_Child);
            v.push_back((ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)"--- Action ---",0,ITEMTEXT|NM_ITEMDISABLED,0));
        }
        if(i==(int) egg_Tabletop)
        {
            v.push_back(MUIA_Family_Child);
            v.push_back((ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)"--- Mind ---",0,ITEMTEXT|NM_ITEMDISABLED,0));
        }
        if(i==(int) egg_sport_)
        {
            v.push_back(MUIA_Family_Child);
            v.push_back((ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)"--- Sport ---",0,ITEMTEXT|NM_ITEMDISABLED,0));
        }
        if(i==(int) egg_Compilation)
        {
            v.push_back(MUIA_Family_Child);
            v.push_back((ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)"--- Other ---",0,ITEMTEXT|NM_ITEMDISABLED,0));
        }
        //

        Object *psub =
        MUI_MakeObject(MUIO_Menuitem,(ULONG)genreNames[i],0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_START+i);
        if(psub)
        {
            EnumFilters[i]=psub;
            v.push_back(MUIA_Family_Child);
            v.push_back((ULONG)psub);
        }
    }
    // put i==0 "Unknown" at the end...
    v.push_back(MUIA_Family_Child);
    EnumFilters[0]= MUI_MakeObject(MUIO_Menuitem,(ULONG)"Genre not set yet",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_START+0);
    v.push_back((ULONG)EnumFilters[0]);

// - - -
    std::vector<ULONG> vv={
    MUIA_Family_Child,(ULONG)MUINewObject(MUIC_Menuitem, MUIA_Menuitem_Title,  NM_BARLABEL, TAG_DONE),

   // MUIA_Family_Child, (ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)"",0,ITEMTEXT|NM_ITEMDISABLED,0),
    MUIA_Family_Child, (ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)" Select By Tags:",0,ITEMTEXT|NM_ITEMDISABLED,0),

// #define EGF_HOR 1
// #define EGF_VER 2  // just to seek vertical egg_ShootEmUp.
// #define EGF_P3D 4 // perspective 3D
// #define EGF_I3D 8 // isometric 3D

// #define EGF_Girly 16 // basically twinkle star and rodland.
// #define EGF_Childish 32
// #define EGF_Sexy 64 // would match egg_Mature ? -> not only.
// #define EGF_Funny 128
// #define EGF_UViolent 256 // all are violent, this is for blood and gore games.
// #define EGF_Hentai 512 // because "monster maulers"

        MUIA_Family_Child,
            (ULONG)(TagFilters[1]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Vertical",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+1)),
        MUIA_Family_Child,
            (ULONG)(TagFilters[2]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... 3D",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+2)),
        MUIA_Family_Child,
            (ULONG)(TagFilters[3]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Isometric",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+3)),

  //  MUIA_Family_Child,(ULONG)MUINewObject(MUIC_Menuitem, MUIA_Menuitem_Title,  NM_BARLABEL, TAG_DONE),
    MUIA_Family_Child, (ULONG)MUI_MakeObject(MUIO_Menuitem,(ULONG)"",0,ITEMTEXT|NM_ITEMDISABLED,0),

        MUIA_Family_Child,
            (ULONG)(TagFilters[4]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Girly",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+4)),
        MUIA_Family_Child,
            (ULONG)(TagFilters[5]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Childish",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+5)),
        MUIA_Family_Child,
            (ULONG)(TagFilters[7]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Funny",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+7)),
       MUIA_Family_Child,
            (ULONG)(TagFilters[8]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Ultra Violent",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+8)),
       MUIA_Family_Child,
            (ULONG)(TagFilters[9]=MUI_MakeObject(MUIO_Menuitem,(ULONG)"... Hentai",0,CHECKIT|CHECKED|MENUTOGGLE,MENU_CODE_TAGSTART+9)),

    TAG_DONE
    };

    v.insert(v.end(),vv.begin(),vv.end());

    Object *o = MUI_NewObjectA((char *)MUIC_Menu, (struct TagItem *) v.data());

   // MENU_GenreFilter = o;
    return o;
}
void MameUI::setAllFilterToggles(int values)
{
    // printf("nbGenreNames:%d\n",nbGenreNames);
    for(int i=0;i<(int)egg_NumberOfGenre;i++)
    {
        if(EnumFilters[i]==NULL) continue;
        set(EnumFilters[i], MUIA_Menuitem_Checked, values);
    }
    for(int i=0;i<EGFB_Numberof;i++)
    {
        if(TagFilters[i]==NULL) continue;
        set(TagFilters[i], MUIA_Menuitem_Checked, values);
    }

}
void MameUI::updateFiltersFromMenu()
{
    // read filter values
    unsigned long long enumsmask = 0ULL,cmask=1ULL;
    UWORD tagsmask = 0,tcmask=1;

    for(int i=0;i<(int)egg_NumberOfGenre;i++,cmask<<=1)
    {
        if(EnumFilters[i]==NULL) continue;
        ULONG value;
        get(EnumFilters[i], MUIA_Menuitem_Checked, &value);
        if(value) enumsmask |= cmask;
    }
    for(int i=0;i<EGFB_Numberof;i++,tcmask<<=1)
    {
        if(TagFilters[i]==NULL) continue;
        ULONG value;
        get(TagFilters[i], MUIA_Menuitem_Checked, &value);
        if(value) tagsmask |= tcmask;
    }

    MameConfig &config = getMainConfig();
    config.setDriverListFilters(enumsmask,tagsmask);
    UpdateList();

}

//trick to send debug string from interuptions
//std::stringstream drivdispdbg;

int MameUI::MainGUI(void)
{
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
                                                                                             MUIA_Family_Child,(ULONG)(MENU_GenreFilter=MenuGenreFilter()),
//                                                                                                                MUIA_Family_Child,(ULONG)(MENU_TagFilter=MUINewObject(MUIC_Menu,MUIA_Menu_Title,(ULONG)GetMessagec("Tag Filter")),
//                                                                                                                  TAG_DONE),
                                                                                             TAG_DONE)},

                                                {WindowContents, (ULONG)windowContent},
                                                TAG_DONE,0};

                MainWin =  MUI_NewObjectA(MUIC_Window, (struct TagItem *) &mainwintags[0]);// MUINewObject(MUIC_Window,

                    //        printf("after MUINewObject():%08x\n",(int)MainWin);
                    // MUIA_Disabled

                if(MainWin)
                {
                    //SetAttrs(LV_Driver,MUIA_CycleChain, TRUE,TAG_DONE);
                    //SetAttrs(BU_Start,MUIA_CycleChain, TRUE,TAG_DONE);

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

                        UpdateUIToConfig();

                    log_addCallback(muilog);

                    //        printf("after UpdateUIToConfig\n");

                }
                // BU_Start disabled at init
                set(BU_Start,  MUIA_Disabled, TRUE);
            }

            if(MainWin)
            {
                ULONG dostart=0;
                ULONG rid,prevrid,nbMenuCode=0;

                // if(LI_Driver && MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
                // {
                //   DoMethod(App,MUIM_Application_Load,MUIV_Application_Load_ENVARC);
                // }
                set(MainWin,  MUIA_Window_ActiveObject, (ULONG) LV_Driver);
                set(MainWin,  MUIA_Window_Open,     TRUE);

                /* This must be done after the window has been opened. */
                DoMethod( LI_Driver, MUIM_List_Jump, MUIV_List_Jump_Active);



               // printf("before MUI loop\n");
                do
                {          
                    // trick to receive debug strings from interuptions:
                    // std::string strdbg = drivdispdbg.str();
                    // drivdispdbg = std::stringstream();
                    // if(strdbg.size()>0) printf("%s",strdbg.c_str());

                    rid = DoMethod(App,MUIM_Application_NewInput,&signals);

                     // krb: all messages are received 2 times ??? Why ?
                    if(rid != prevrid) {

                    if(rid>=MENU_CODE_START && rid<MENU_CODE_END) {
                        if(rid == MENU_CODE_ALL) setAllFilterToggles(TRUE);
                        else if(rid == MENU_CODE_NONE) setAllFilterToggles(FALSE);
                        nbMenuCode++;
                       // printf("menu code:%d\n",rid);
                    } else
                    {
                        // ends a menu change serie
                        if(nbMenuCode>0)
                        {
                            updateFiltersFromMenu();
                            nbMenuCode = 0;
                        }
                    }

                    switch(rid)
                    {
                    case RID_Start:
                    {
                        // game rom selected to start !
                        get(LI_Driver, MUIA_List_Active, &v);

                        if(v != (ULONG)MUIV_List_Active_Off)
                        {
                            int driverptr=0;
                            DoMethod(LI_Driver, MUIM_List_GetEntry, v, &driverptr);

                            if(driverptr) config.setActiveDriver(GetEntryDriverIndex(driverptr));
                            loop = FALSE;
                            dostart = 1;
                        }
                    } // end case
                    break;

                    case RID_Scan:
                        config.scanDrivers();
                        setDriverListShowState(CFGS_FOUND);
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
                    }
                    prevrid = rid;

                    if(signals && loop)
                    {
                        signals = Wait(signals | SIGBREAKF_CTRL_C);

                        if(signals & SIGBREAKF_CTRL_C)
                            loop = FALSE;
                    }
                } while(loop);


                // save some window state
//                if(LI_Driver && MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
//                {
//                    // UIListState uiliststate={0};
//                    // uiliststate.nbcolumn = 8;
//                    // get(LI_Driver, MUIA_List_ColumnOrder, &uiliststate.columnOrder[0]);
//                    // get(LI_Driver,MUIA_List_SortColumn,&uiliststate.sortedcolumn );
//                    // config.setColumnOrder(uiliststate);
//// printf("save\n");
//                    DoMethod(App,MUIM_Application_Save,MUIV_Application_Save_ENVARC);
//                }

                set(AboutWin, MUIA_Window_Open, FALSE);
                set(MainWin,  MUIA_Window_Open, FALSE);

                if(dostart)
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
static int DriverCompareScreenMode(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
    MameConfig &config = getMainConfig();
    // the resolution mode string versions is cached in the config.
    return config.DriverCompareScreenMode(drv1,drv2);
}
static int DriverCompareNbPlayers(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
    int a = (*drv1)->nbplayersSim;
    int b = (*drv2)->nbplayersSim;
    int i = b-a;
    if(i!=0) return i;
    a = (*drv1)->nbplayersAlt;
    b = (*drv2)->nbplayersAlt;
    i = b-a;
    if(i!=0) return i;
    // else keep alphabetic
    return(stricmp((*drv1)->description, (*drv2)->description));
}
static int DriverCompareYear(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
    int i = strcmp((*drv1)->year, (*drv2)->year);
    if(i != 0) return i;
    // else keep alphabetic
    return(stricmp((*drv1)->description, (*drv2)->description));
}
static int DriverCompareArchive(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
  return(stricmp((*drv1)->name, (*drv2)->name)); // garanteed dfifferents
}
static int DriverCompareParent(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
    int  i = (stricmp((*drv1)->parent, (*drv2)->parent));
    if(i != 0) return i;
    // else keep alphabetic
    return(stricmp((*drv1)->description, (*drv2)->description));
}
static int DriverCompareMachine(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
    int  i = (stricmp((*drv1)->source_file, (*drv2)->source_file));
    if(i != 0) return i;
    // else keep alphabetic
    return(stricmp((*drv1)->description, (*drv2)->description));
}

static int DriverCompareGenre(const struct _game_driver **drv1,const  struct _game_driver **drv2)
{
    int a = (int)(*drv1)->genre;
    if(a== 0) a=50;
    int b = (int)(*drv2)->genre;
    if(b== 0) b=50;
    int i = a-b;
    if(i != 0) return i;
    a = (int)(*drv1)->genreflag;
    b = (int)(*drv2)->genreflag;
    i = a-b;
    if(i != 0) return i;
    // else keep alphabetic
    return(stricmp((*drv1)->description, (*drv2)->description));
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
        case 2: return DriverCompareScreenMode(drva,drvb);
        case 3: return DriverCompareGenre(drva,drvb);
        case 4: return DriverCompareYear(drva,drvb);
        case 5: return DriverCompareArchive(drva,drvb);
        case 6: return DriverCompareParent(drva,drvb);
        case 7: return DriverCompareMachine(drva,drvb);
        default: break;
    }

   return DriverCompareNames(drva,drvb);

}


static ULONG DriverDisplay(struct Hook *hook REG(a0), char **array REG(a2),const struct _game_driver **drv_indirect REG(a1))
{
    MameUI *ui = (MameUI *) hook->h_Data;
    MameConfig &config = getMainConfig();
    const struct _game_driver *drv;

    struct ColumnsString {
          char *_driver,*_players,*_screen,*_genre,*_year,*_archive,*_parent,*_machine,*_comment;
    };
    ColumnsString *pColumns = (ColumnsString *)array;

  static char driver[64];
  static char screen[32];
  static char archive[16];
  static char genre[24];
  static char parent[16];
 static char smachine[16];
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
    snprintf(genre,15,   "\033b\033u%s", ui->String_Genre);
    //genre[23]=0;
    snprintf(archive,15,  "\033b\033u%s", ui->String_Archive);
    archive[15]=0;
    snprintf(parent,15,  "\033b\033u%s", ui->String_Parent);
    parent[15]=0;
    snprintf(players,15,    "\033b\033u%s", ui->String_Players);
    players[15]=0;
    snprintf(year,11,    "\033b\033u%s", ui->String_Year);
    year[11]=0;
    snprintf(smachine,15,    "\033b\033u%s", ui->String_Machine);
    smachine[15]=0;
   // snprintf(comment,127,  "\033b\033u%s", String_Comment);
   // comment[127]=0;
    strComment =  "\033b\033u";
    strComment += ui->String_Comment;

    pColumns->_driver = driver;
    pColumns->_players = players;
    pColumns->_genre = genre;
    pColumns->_year = year;
    pColumns->_screen = screen;
    pColumns->_archive = archive;
    pColumns->_parent = parent;
    pColumns->_machine = smachine;
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

   if(drv->nbplayersSim  <2)
   {
    if(drv->nbplayersAlt <2)
        players[0]=0;
    else
    {
        snprintf(players,7,"%d(Alt)", (int) drv->nbplayersAlt);
    }
   }
   else snprintf(players,7,"%d", (int) drv->nbplayersSim);
    pColumns->_players = players;

    pColumns->_year = (char *)drv->year;
    pColumns->_machine = (char *)drv->source_file;

    int eGenre =  drv->genre;
    int fgenre =  drv->genreflag;
    if(eGenre ==0 || eGenre>=(int)egg_NumberOfGenre) {
        genre[0] = 0;
    } else
    {
        snprintf(genre,23,"%s%s%s%s%s%s%s%s%s",
                        genreNames[eGenre],
                        ((fgenre&EGF_VER)?" Vertical":""),
                        ((fgenre&EGF_P3D)?" 3D":""),
                        ((fgenre&EGF_I3D)?" Isometric":""),
                        ((fgenre&EGF_Girly)?" Girly":""),
                        ((fgenre&EGF_Childish)?" Childish":""),
                        ((fgenre&EGF_Funny)?" Funny":""),
                        ((fgenre&EGF_UViolent)?" Ultra Violent":""),
                       ((fgenre&EGF_Hentai)?" Hentai":"")
                        );
        genre[23]=0;
    }
/*
#define EGF_HOR 1
#define EGF_VER 2  // just to seek vertical egg_ShootEmUp.
#define EGF_P3D 4 // perspective 3D
#define EGF_I3D 8 // isometric 3D

#define EGF_Girly 16 // basically twinkle star and rodland.
#define EGF_Childish 32
#define EGF_Sexy 64 // would match egg_Mature ? -> not only.
#define EGF_Funny 128
#define EGF_UViolent 256 // all are violent, this is for blood and gore games.
#define EGF_Hentai 512 // because "monster maulers"

*/


//  if(drv->flags & GAME_NOT_WORKING)
//   pColumns->_comment = NotWorkingString;
//  else if(drv->flags & GAME_WRONG_COLORS)
//   pColumns->_comment = WrongColorsString;
//  else if(drv->flags & GAME_IMPERFECT_COLORS)
//   pColumns->_comment = ImperfectColorsString;
//  else
//   pColumns->_comment =(char*) "";

    strComment.clear();
     if(drv->flags & GAME_NOT_WORKING)
     {
      strComment += ui->NotWorkingString;
      strComment += " ";
    }
     else if(drv->flags & (GAME_WRONG_COLORS|GAME_IMPERFECT_COLORS|GAME_NO_SOUND|GAME_IMPERFECT_SOUND|GAME_SUPPORTS_SAVE))
     {
         if(drv->flags & GAME_SUPPORTS_SAVE)
         {
            strComment += ui->SupportSaveString;
            strComment += " ";
         }

         if(drv->flags & GAME_WRONG_COLORS)
         {
            strComment += ui->WrongColorsString;
            strComment += " ";
         }
         if(drv->flags & GAME_IMPERFECT_COLORS)
         {
            strComment += ui->ImperfectColorsString;
            strComment += " ";
         }
         if(drv->flags & GAME_NO_SOUND)
         {
            strComment += ui->NoSoundString;
            strComment += " ";
         }
         if(drv->flags & GAME_IMPERFECT_SOUND)
         {
            strComment += ui->ImperfectSoundString;
            strComment += " ";
         }

     }

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

// called from dispatcher, interupt-like context.
int manageKeyInListMsgDispatcher( Object * obj, struct DriverData   *data, struct IntuiMessage *imsg)
{
    APTR  active_obj;
    Object  *list;
    int nbvisisbles;

   if(imsg->Class != IDCMP_RAWKEY)  return 0;

    get(obj, MUIA_List_Visible, &nbvisisbles);
    // -1 means list not visible at all (not the right panel ?).
    // do not accaparate keyboard in that case
    if(nbvisisbles<=0) return 0;

//             char temp[64];
//             snprintf(temp,63,"nbvisisbles:%d\n",nbvisisbles);
//             drivdispdbg << temp;

    get(_win(obj), MUIA_Window_ActiveObject, &active_obj);
    get(obj, MUIA_Listview_List, &list);
    if(!list) return 0;

    if(obj != active_obj && list != active_obj) return 0;

    struct InputEvent   ie;
    ie.ie_Class   = IECLASS_RAWKEY;
    ie.ie_SubClass  = 0;
    ie.ie_Code    = imsg->Code;
    ie.ie_Qualifier = 0;

    char key[8];
    //  MapRawKey( CONST struct InputEvent *event, STRPTR buffer, LONG length, CONST struct KeyMap *keyMap );
    if(!MapRawKey(&ie,(STRPTR)&key[0], 4, NULL) || !isalnum(key[0])) return 0;

    timeval diftime = subtime({imsg->Seconds,imsg->Micros},{data->Seconds,data->Micros});


    // addchar if time dif less than 0.8 sec, else restart.
    int doAddChar=( diftime.tv_sec==0 && diftime.tv_micro< (1000000) );

// {
//     char temp[64];
//     snprintf(temp,63,"diftime:%d . %d   doadd:%d\n", diftime.tv_sec,diftime.tv_micro,doAddChar);
//     drivdispdbg << temp;
// }

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

                // char temp[64];
                // snprintf(temp,63,"key:%c  acc:%s\n",(char)key[0],&data->typedAccum[0]);
                // drivdispdbg << temp;

    int i = 0;
    int bestIndex = -1;
    int bestIndexScore = 0;
    // really loop anything (posibly 2000 items). It's one by key stroke anyway.
    // list items can be sorted in any manner.
    do
    {
       struct _game_driver   **drv_indirect;

      DoMethod(list, MUIM_List_GetEntry, i, &drv_indirect);
      if(! drv_indirect) break;

      struct _game_driver   *drv = *drv_indirect;
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
       return(1);
    }
    return 0;
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
    {
      data =  (struct DriverData *) INST_DATA(cclass, obj);
      imsg = (struct IntuiMessage *) msg[1].MethodID;

      int diduseit = manageKeyInListMsgDispatcher(obj,data,imsg);
      //if(diduseit) return(MUI_EventHandlerRC_Eat);
    }
      break;

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

// DriverDispatcherMUI5 does not patch column sorting , it lets MUI5 list do it.
static ULONG DriverDispatcherMUI5(struct IClass *cclass REG(a0), Object * obj REG(a2), Msg msg REG(a1))
{
  struct DriverData   *data;
  struct IntuiMessage *imsg;
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

      int diduseit = manageKeyInListMsgDispatcher(obj,data,imsg);
      //if(diduseit) return(MUI_EventHandlerRC_Eat);
    }
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
    // also sent at init
    //printf("ShowNotify setDriverListShowState\n");
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
            muiConfigCreator.selectGroup("Display.Vector Screen",screenconf);
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
    config.setDriverListShowMode(state);
    UpdateList();

}
// at init
void MameUI::UpdateUIToConfig()
{
    MameConfig &config = getMainConfig();

    // set list is in previous state, from configuration.
    int listShowState = config.driverListShowMode();
// printf("UpdateUIToConfig->setDriverListShowState:%d\n",listShowState);
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
static int dontreupdatetwice=0;
void MameUI::UpdateList(void)
{
    if(dontreupdatetwice) {
        dontreupdatetwice=0;
        return;
    }
    DoMethod(LI_Driver, MUIM_List_Clear);

    MameConfig &config = getMainConfig();
    int showmode = config.driverListShowMode();
    int useFilters = config.usesListFilter();
    // get list accoring to show mode and filters
    const std::vector<const _game_driver *const*> &roms = config.roms();

    int nbroms = (int)roms.size();
// printf("MameUI::UpdateList:%d\n",(int)roms.size());
    //MUIM_List_Insert can insert everything in a blow.
        DoMethod((Object *)_pMameUI->LI_Driver, MUIM_List_Insert,
         (ULONG)roms.data(),nbroms,  /*MUIV_List_Insert_Bottom*/MUIV_List_Insert_Sorted);


    if(LI_Driver && MUIMasterBase->lib_Version>=MUI5_API_SINCE_VERSION)
    {
        const struct UIListState &uiliststate = config.columnOrder();
       // set(LI_Driver,MUIA_List_SortColumn,(ULONG)&uiliststate.sortedcolumn );
//        set(LI_Driver,MUIA_List_ColumnOrder,(ULONG) &uiliststate.columnOrder[0]);

    }

// no need because MUIV_List_Insert_Sorted (would ressort):   DoMethod(LI_Driver,MUIM_List_Sort);

    // ensure cycle is in correct state
    int cyclestate=0;
    get(_pMameUI->CY_Show, MUIA_Cycle_Active, &cyclestate);
    if(cyclestate != showmode)
    {
        dontreupdatetwice = 1;
        set(_pMameUI->CY_Show, MUIA_Cycle_Active,showmode);
    }
    char temp[64];
    snprintf(temp,63,"Displaying %d%s%s driver%s.",nbroms,
            ((showmode)?" found":""),((useFilters)?" filtered":""),((nbroms>1)?"s":""));
    muilog(0,temp);

}
// void MameUI::ShowAll(void)
// {
//     MameConfig &config = getMainConfig();
//     std::vector<const _game_driver *const*> roms;
//     config.buildAllRomsVector(roms);

//     //MUIM_List_Insert can insert everything in a blow.
//         DoMethod(_pMameUI->LI_Driver, MUIM_List_Insert,
//          (ULONG)roms.data(),(int)roms.size(),  /*MUIV_List_Insert_Bottom*/MUIV_List_Insert_Sorted);

//    // no need because MUIV_List_Insert_Sorted does the job: DoMethod(LI_Driver,MUIM_List_Sort);

//     // ensure cycle is in correct state
//     int cyclestate=0;
//     get(_pMameUI->CY_Show, MUIA_Cycle_Active, &cyclestate);
//     if(cyclestate != 0)
//     {
//         set(_pMameUI->CY_Show, MUIA_Cycle_Active,0);
//     }
// }


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
    NoSoundString            = GetMessagec("No Sound");
    ImperfectSoundString     = GetMessagec("Imperfect Sound");
    SupportSaveString     = GetMessagec("Support Save");
    String_Driver=GetMessagec("Driver");
    String_Archive=GetMessagec("Archive");
    String_Parent=GetMessagec("Parent");
    String_Screen=GetMessagec("Screen");
    String_Genre=GetMessagec("Genre");
    String_Players=GetMessagec("Players");
    String_Year=GetMessagec("Year");
    String_Machine=GetMessagec("Machine");
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


const char *pguidepath =  (SysBase->LibNode.lib_Version>=47)?"PROGDIR:MameMinimixOS32.guide":"PROGDIR:MameMinimix.guide";

ULONG diskObjOrEnd = (AppDiskObject)? MUIA_Application_DiskObject : TAG_DONE;

  App = MUINewObject(MUIC_Application,
    MUIA_Application_Title      , (ULONG)APPNAME,
    //MUIA_Application_Version    , (ULONG)("$VER: " APPNAME " (" REVDATE ")"),
    MUIA_Application_Author     , (ULONG)AUTHOR,
    MUIA_Application_Base       , (ULONG)staticGuiAppId,
    // for appicon
    MUIA_Application_DiskObject,AppDiskObject,
    SubWindow, (ULONG)AboutWin,
    MUIA_Application_HelpFile, pguidepath,
    diskObjOrEnd,AppDiskObject,
  TAG_DONE,0);

  if(App)
  {
    DoMethod(BU_About_OK, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, RID_CloseAbout);

    DoMethod(AboutWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             App, 2, MUIM_Application_ReturnID, RID_CloseAbout);
  }

}




