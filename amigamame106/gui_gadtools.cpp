
#define CATCOMP_NUMBERS
#include "messages.h"

#include <proto/gadtools.h>

extern struct Library      *GadToolsBase;

static struct NewMenu NewMenu[] =
{
  { NM_TITLE, (STRPTR) MSG_MENU_GAME,      NULL, 0,  0,  NULL  },
  { NM_ITEM,  (STRPTR) MSG_MENU_NEW,       "N",  0,  0,  NULL  },
  { NM_ITEM,  (STRPTR) MSG_MENU_SAVE_ILBM, "S",  0,  0,  NULL  },
  { NM_ITEM,  (STRPTR) NM_BARLABEL,        NULL, 0,  0,  NULL  },
  { NM_ITEM,  (STRPTR) MSG_MENU_ABOUT,     "?",  0,  0,  NULL  },
  { NM_ITEM,  (STRPTR) NM_BARLABEL,        NULL, 0,  0,  NULL  },
  { NM_ITEM,  (STRPTR) MSG_MENU_QUIT,      "Q",  0,  0,  NULL  },
  { NM_END,   NULL,                        NULL, 0,  0,  NULL  },
};

struct Menu *g_gtMenu=NULL;


int gui_gadtools_init()
{
    if(!GadToolsBase) return 1;
    if(g_gtMenu) return 1;

    for(int i = 0; NewMenu[i].nm_Type != NM_END; i++)
    {
      if(((NewMenu[i].nm_Type == NM_TITLE) || (NewMenu[i].nm_Type == NM_ITEM))
      && (NewMenu[i].nm_Label != NM_BARLABEL))
        NewMenu[i].nm_Label = GetMessage((LONG) NewMenu[i].nm_Label);
    }
    g_gtMenu  = CreateMenus(NewMenu, GTMN_FullMenu, TRUE, TAG_END);

    return 0;
}
void gui_gadtools_close()
{
    if(!GadToolsBase) return;
   if(g_gtMenu) {
        FreeMenus(g_gtMenu);
        g_gtMenu = NULL;
    }
}
