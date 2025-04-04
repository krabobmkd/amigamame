#ifndef GUI_H
#define GUI_H
/**************************************************************************
 *
 * Copyright (C) 2024 Vic Ferry
 * Forked from 1999 Mats Eirik Hansen
 *
 *************************************************************************/

extern "C" {
    #include <libraries/mui.h>

    void AllocGUI(void);
    void FreeGUI(void);
}





struct MameUI {
    MameUI();
    virtual ~MameUI();
    int init();

    Object *App=NULL;
    Object *MainWin=NULL;
    Object *AboutWin=NULL;
    struct MUI_CustomClass *DriverClass=NULL;

    struct Hook DriverSelectHook={};
    struct Hook DriverDisplayHook={};
    struct Hook DriverSortHook={};
    struct Hook DriverNotifyHook={};
    struct Hook ShowNotifyHook={};

    // list column names
    const char *String_Driver=NULL;
    const char *String_Archive=NULL;
    const char *String_Parent=NULL;
    const char *String_Screen=NULL;
    const char *String_Players=NULL;
    const char *String_Year=NULL;
    const char *String_Comment=NULL;

    const char *NotWorkingString=NULL;
    const char *WrongColorsString=NULL;
    const char *ImperfectColorsString=NULL;

    Object * RE_Options=NULL;
    Object * CY_Show=NULL;
    Object * BU_Scan=NULL;

    Object *LI_Driver=NULL;
    Object * LV_Driver=NULL;
    Object * BU_Start=NULL;
    //Object * BU_Quit;
    Object * BU_About_OK=NULL;

    Object *LA_statusbar=NULL;
    Object *GIF_cornerlogo=NULL;

    int MainGUI(void);
    void CreateApp(void);
    Object *createPanel_Drivers();
    ULONG createOptionTabGroup();
    void UpdateUIToConfig();
    void AboutGUI(void);
    int GetDriverIndex(void);
    void ShowFound(void);
    void ShowAll(void);

    void setDriverListShowState(int state);
};
extern MameUI *_pMameUI;


#endif
