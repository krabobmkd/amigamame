
extern "C" {
    struct Library;
    extern struct Library *MUIMasterBase;
}

#include "serializer_mui.h"

#include <vector>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdio.h>
#include <string.h>

#ifdef __GNUC__
#define REG(r) __asm(#r)
#else
#define REG(r)
#endif



extern "C" {
    #include <libraries/mui.h>
    #include <proto/muimaster.h>
    #include <libraries/asl.h>
}

typedef ULONG (*RE_HOOKFUNC)(); // because C++ type issue.

#define OString(contents,maxlen)\
	MUI_NewObject(MUIC_String,\
		StringFrame,\
		MUIA_String_MaxLen  , maxlen,\
		MUIA_String_Contents, contents,\
		TAG_DONE)

using namespace std;

MUISerializer::MUISerializer()
    : ASerializer()
    , _irecurse(0)
    , _pGrower(&_pRoot)
    , _pRoot(NULL)
{
}
MUISerializer::~MUISerializer()
{
    // tested ok
    for(Level *plv : _stack) if(plv) delete plv;
}
// ---------------------------------------------------------------------------------
// -------      Fullfil Serializer API by creating bridge Objects to mirror MUI ----
// --------------------------------------------------------------------------------
void MUISerializer::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    Level *plevel;
    // let's add a sub level.
    if(_irecurse==0)
    {
        plevel = new LTabs(*this,&subconf);
    } else
    {
        plevel = new LGroup(*this,&subconf,flags);
    }
    if(!plevel) return;

    _stack.push_back(plevel);
    plevel->_pMemberName = sMemberName;
    // attach
    *_pGrower = plevel;
    // next will attach down
    _pGrower = &plevel->_pFirstChild;

    _irecurse++;

        subconf.serialize(*this);

    _irecurse--;

    // next wil be bro
    _pGrower = &plevel->_pNextBrother;

}
void MUISerializer::operator()(const char *sMemberName, std::string &str,int flags)
{
    LString *plevel = new LString(*this,str,flags);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;

}
// for sliders
void MUISerializer::operator()(const char *sMemberName, int &v, int min, int max, int defv)
{
    LSlider *plevel = new LSlider(*this,v,min,max);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;

}
// for cycling
void MUISerializer::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
	 LCycle *plevel = new LCycle(*this,v,values);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
// for checkbox
void MUISerializer::operator()(const char *sMemberName, bool &v)
{
    LCheckBox *plevel = new LCheckBox(*this,v);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
// for table of flags
void MUISerializer::operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)
{
    LFlags *plevel = new LFlags(*this,v,valdef,values);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}

// for screen ids
void MUISerializer::operator()(const char *sMemberName, ULONG_SCREENMODEID &v)
{
    LScreenModeReq *plevel = new LScreenModeReq(*this,v);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
// per optional subconf
void MUISerializer::operator()(const char *sMemberName, AStringMap &confmap)
{
    ASerializable &subconf = confmap.get("current"); // we need one to build UI.

    Level *plevel;
    // let's add a sub level.
    plevel = new LSwitchGroup(*this,&subconf,0,confmap);
    if(!plevel) return;

    _stack.push_back(plevel);
    plevel->_pMemberName = sMemberName;
    // attach
    *_pGrower = plevel;
    // next will attach down
    _pGrower = &plevel->_pFirstChild;

    _irecurse++;
        subconf.serialize(*this);
    _irecurse--;

    // next wil be bro
    _pGrower = &plevel->_pNextBrother;

}
void MUISerializer::operator()(const char *sMemberName, strText &str)
{
    Level *plevel = new LInfoText(*this,str,0);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
#ifdef MUISERIALIZER_USES_FLOAT
// for sliders
void MUISerializer::operator()(const char *sMemberName, float &v, float min, float max,float step,float defval)
{
    LSliderF *plevel = new LSliderF(*this,v,min,max,step,defval);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
#endif

// - - - -rules
void MUISerializer::listenChange(const char *sMemberName,std::function<void(ASerializer &serializer, void *p)> condition)
{
    std::list<Level *>::reverse_iterator rit = _stack.rbegin();
    while(rit != _stack.rend())
    {
        Level *pl = *rit++;
        if(pl && pl->_pMemberName && strcmp(sMemberName, pl->_pMemberName)==0) {
            pl->_rules.push_back(condition);
            break;
        }
    }
}
void MUISerializer::enable(std::string memberUrl, int enable)
{
    Level *p = getByUrl(memberUrl);
    if(!p || !p->_Object) return;
     SetAttrs(p->_Object, MUIA_Disabled,enable?0:1,TAG_DONE);

    // valueUpdated
}
 void MUISerializer::update(std::string memberUrl)
 {
     Level *p = getByUrl(memberUrl);
     if(!p || !p->_Object) return;
     p->update();
 }
ASerializable *MUISerializer::getObject(std::string memberUrl)
{
    Level *p = getByUrl(memberUrl);
    if(!p ) return NULL;
    //hopefully
    LGroup *pgroup = (LGroup *)p;
    return (ASerializable *)pgroup->_serialized;
}
struct MUISerializer::Level *MUISerializer::getByUrl(const std::string &memberUrl)
{
    struct Level *p = _pRoot;
    if(!p) return NULL;
    size_t i=0;
    while(i != string::npos)
    {
        size_t j=memberUrl.find(".",i);

        string str;
        if(j != string::npos) str = memberUrl.substr(i,(j-i));
        else str = memberUrl.substr(i);
        p = p->getChild(str.c_str());

        if(!p) return NULL;
        if(j != string::npos) i = j+1;
        else i=j;
    }
    return p;

}

// -----------------------------------------------------------------------------
// -------   MUISerializer compile function to finalize bridge objects   --------
// ----------------------------------------------------------------------------
// optional hack
void MUISerializer::insertFirstPanel(Object *pPanel,const char *pName)
{
    if(!_pRoot) return;
    Level *plevel = new Level(*this);
    if(!plevel) return;

    _stack.insert(_stack.begin(),plevel); // maybe not exact place but would work and will be deleted.

    plevel->_Object = pPanel;
    plevel->_pMemberName = pName;
    plevel->_pNextBrother = _pRoot->_pFirstChild;
    _pRoot->_pFirstChild = plevel;

}
// finalize
Object *MUISerializer::compile()
{
    if(!_pRoot) return NULL;
   // printf("compile stacksize:%d\n",(int)_stack.size());
    //  leaf to root widget creation
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    int i=0;
    while(rit != _stack.rend())
    {
        Level *level = *rit++;
     //   printf("compile:%d\n",i);
        if(!level->_Object) level->compile(); // create object if not done
        i++;
    }
  // printf("end compile\n");
    return _pRoot->_Object;
}
void MUISerializer::updateUI()
{
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    while(rit != _stack.rend())
    {
        Level *plevel = *rit++;
        if(plevel->_Object) plevel->update();
    }

}
void MUISerializer::selectGroup(std::string groupurl,std::string selection)
{
    // "Display.Per Screen Mode"
    Level *p = _pRoot;
    if(!p) return;
    size_t i=0;
    while(i != string::npos)
    {
        size_t j=groupurl.find(".",i);

        string str;
        if(j != string::npos) str = groupurl.substr(i,(j-i));
        else str = groupurl.substr(i);
       // printf("str:%s\n",str.c_str());

        p = p->getChild(str.c_str());

        if(!p) return;
        if(j != string::npos) i = j+1;
        else i=j;
    }
    if(!p) return;
    LSwitchGroup *pGroup = (LSwitchGroup *)p;
  //  printf("got from url::%s \n",pGroup->_pMemberName);
    pGroup->setGroup(selection.c_str());

}
// -----------------------------------------------------------------------------
// -------   The inner bridge classes that links MUI Gadgets to configs .... ----
// ----------------------------------------------------------------------------
// - - - - - - - - - - - - - - -
// post compilation of mui gadgets...
MUISerializer::Level::Level(MUISerializer &ser)
 : _ser(ser)
 , _Object(NULL)
 , _pMemberName(NULL)
 , _pFirstChild(NULL)
 , _pNextBrother(NULL)
{}
MUISerializer::Level::~Level() {}

MUISerializer::Level *MUISerializer::Level::getChild(const char *pMemberName)
{
    Level *plevel = _pFirstChild;

    while(plevel)
    {
        if(plevel->_Object && plevel->_pMemberName &&
                strcmp(pMemberName,plevel->_pMemberName)==0)
        {
            return plevel;
        }
        plevel = plevel->_pNextBrother;
    }
    return NULL;
}
void MUISerializer::Level::update()
{
}
// - - - - - - - - - - - - - - -
MUISerializer::LTabs::LTabs(MUISerializer &ser,ASerializable *pconf) : LGroup(ser,pconf,0)
{

}

void MUISerializer::LTabs::compile()
{
    _registerTitles.clear();

    Level *plevel = _pFirstChild;
    while(plevel)
    {
         if(plevel->_Object)
         {
            _registerTitles.push_back(plevel->_pMemberName);
         }
        plevel = plevel->_pNextBrother;
    }
    _registerTitles.push_back(NULL);

    std::vector<ULONG> tagitems={
        MUIA_Register_Titles,(ULONG)_registerTitles.data()
    };

    plevel = _pFirstChild;
    while(plevel)
    {
        if(plevel->_Object)
        {
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)plevel->_Object);
        }
        plevel = plevel->_pNextBrother;
    }
    tagitems.push_back(TAG_DONE);

    _Object = MUI_NewObjectA(MUIC_Register, (struct TagItem *) tagitems.data());

}
// - - - - - - - - - - - - - - -
MUISerializer::LGroup::LGroup(MUISerializer &ser,ASerializable *pconf,int flgs) : Level(ser), _flgs(flgs),
_serialized(pconf)
{
}
void MUISerializer::LGroup::compile()
{
    int nbcolumns = 2;
    if(_flgs & SERFLAG_GROUP_2COLUMS) nbcolumns=4;
    vector<ULONG> tagitems= {MUIA_Group_Columns,nbcolumns,MUIA_HorizWeight,1000};
    Level *plevel = _pFirstChild;
    int nbadded=0;
    while(plevel)
    {
        if(plevel->_Object && plevel->_pMemberName)
        {
       // printf("group: add member:%s\n",plevel->_pMemberName);
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)Label((ULONG)plevel->_pMemberName));
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)plevel->_Object);
            nbadded++;
        }
        plevel = plevel->_pNextBrother;
    }
    if(nbadded==0)
    {
        tagitems.push_back(Child);
        tagitems.push_back((ULONG)Label((ULONG)"-"));
        tagitems.push_back(Child);
        tagitems.push_back((ULONG)Label((ULONG)"-"));
    }
    tagitems.push_back(TAG_DONE);

    Object *InnerGroup = MUI_NewObjectA(MUIC_Group,(struct TagItem *) tagitems.data());

    _Object = compileOuterFrame(InnerGroup);

}
Object *MUISerializer::LGroup::compileOuterFrame(Object *pinnerGroup)
{


    if(_flgs & SERFLAG_GROUP_SCROLLER)
    {
        Object *ob = MUI_NewObject(MUIC_Virtgroup,VirtualFrame,
                MUIA_Background, MUII_TextBack,
               Child, (ULONG)HVSpace,
               Child, (ULONG)MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                                     Child, (ULONG)HSpace(0),
                                     Child,(ULONG)pinnerGroup,
                                     Child, (ULONG)HSpace(0),
                                     TAG_DONE
                                     ),
               Child, (ULONG)HVSpace,
               TAG_DONE
               );
        if(!ob) return NULL;


        Object *scrollgroup  =
            MUI_NewObject(MUIC_Scrollgroup,
             MUIA_Scrollgroup_FreeHoriz, TRUE,
             MUIA_Scrollgroup_FreeVert, TRUE,
             MUIA_Scrollgroup_Contents, (ULONG)ob,
            TAG_DONE);
        return scrollgroup;
    }
   return MUI_NewObject(MUIC_Group,
                  Child, (ULONG)HVSpace,
                  Child, (ULONG)MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                                        Child, (ULONG)HSpace(0),
                                        Child,(ULONG)pinnerGroup,
                                        Child, (ULONG)HSpace(0),
                                        TAG_DONE
                                        ),
                  Child, (ULONG)HVSpace,
                  TAG_DONE
                  );

}

void MUISerializer::LGroup::update()
{
    Level *plevel = _pFirstChild;
    while(plevel)
    {
        plevel->update();
        plevel = plevel->_pNextBrother;
    }
}
// - - - - - - - - - - - - - - -
ULONG MUISerializer::LFlags::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par ) return 0;
    MUISerializer::LFlags *plevel = (MUISerializer::LFlags *)hook->h_Data;
    if(!plevel->_pflugs) return 0;
    // got to identify checkbox clicked
    unsigned int ib=1;
    for(int iflag=0;iflag<(int)plevel->_flagNames.size() ;iflag++)
    {
        if(plevel->_buttons[iflag] == obj)
        {
            if(*par != 0)
                *(plevel->_pflugs) |= ib;
            else *(plevel->_pflugs) &= ~ib;
        }
        ib <<=1;
    }
    return 0;
}

MUISerializer::LFlags::LFlags(MUISerializer &ser,ULONG_FLAGS &flugs,
                              ULONG_FLAGS defval,const std::vector<std::string> &names)
: Level(ser), _pflugs(&flugs)
 , _defval((ULONG)defval)
 ,_flagNames(names),_notifyHook({})
{
    _buttons.resize(names.size());
    _notifyHook.h_Entry =(RE_HOOKFUNC)&MUISerializer::LFlags::HNotify;
    _notifyHook.h_Data = this;
}
void MUISerializer::LFlags::compile()
{
    int nbcolumns = 4;
   if(_defval & SERFLAG_GROUP_FLAGINT2COLUMS) nbcolumns=2;
    vector<ULONG> tagitems= {MUIA_Group_Columns,nbcolumns,MUIA_HorizWeight,1000};

    unsigned int startval = (_pflugs)?*_pflugs:0;
    unsigned int ib=1;
    for(int iflag=0;iflag<(int)_flagNames.size() ;iflag++)
    {
        _buttons[iflag] =
                MUI_NewObject(MUIC_Image,
                                ImageButtonFrame,
                                MUIA_InputMode        , MUIV_InputMode_Toggle,
                                MUIA_Image_Spec       , MUII_CheckMark,
                                MUIA_Image_FreeVert   , TRUE,
                                MUIA_Selected         , (ULONG)((startval&ib)?1:0),
                                MUIA_Background       , MUII_ButtonBack,
                                MUIA_ShowSelState     , FALSE,
                                TAG_DONE);

        tagitems.push_back(Child);
        tagitems.push_back((ULONG)Label((ULONG)_flagNames[iflag].c_str()));
        tagitems.push_back(Child);
        tagitems.push_back((ULONG)_buttons[iflag]);

        if(_buttons[iflag])
        {
            DoMethod(_buttons[iflag],MUIM_Notify,
                        MUIA_Selected, // attribute that triggers the notification.
                        MUIV_EveryTime, // TrigValue ,  every time when TrigAttr changes
                        _buttons[iflag], // object on which to perform the notification method. or MUIV_Notify_Self
                        3, // FollowParams  number of following parameters (in hook ?)
                        MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);
        }

        ib<<=1;
    }
    if(_flagNames.size()==0)
    {
        tagitems.push_back(Child);
        tagitems.push_back((ULONG)Label((ULONG)"-"));
        tagitems.push_back(Child);
        tagitems.push_back((ULONG)Label((ULONG)"-"));
    }
    tagitems.push_back(TAG_DONE);

    _Object= MUI_NewObjectA(MUIC_Group,(struct TagItem *) tagitems.data());
}
void MUISerializer::LFlags::update()
{
    if( !_pflugs) return;
    Level::update();
    unsigned int ib=1;
    for(int iflag=0;iflag<(int)_flagNames.size() ;iflag++)
    {
        SetAttrs(_buttons[iflag],MUIA_Selected,(ULONG)((*_pflugs & ib)?1:0),TAG_DONE);
        ib<<=1;
    }

}
//ULONG_FLAGS *_pflugs;

// - - - - - - - - - - - - - - -
MUISerializer::LSwitchGroup::LSwitchGroup(MUISerializer &ser,ASerializable *pconf,int flgs,AStringMap &map) : LGroup(ser,pconf,flgs)
  ,_map(&map), _displayName("(select a driver)")
#ifndef USEGROUPTITLE
  ,_SelectedItemText(NULL)
#endif
{

}
Object *MUISerializer::LSwitchGroup::compileOuterFrame(Object *pinnerGroup)
{
#ifndef USEGROUPTITLE
    _SelectedItemText = MUI_NewObject(MUIC_Text,
                 //   TextFrame,
                  //  MUIA_Background, MUII_TextBack,
                    MUIA_Text_Contents,(ULONG)"...",
                  TAG_DONE);
    Object *obj = MUI_NewObject(MUIC_Group,
            GroupFrameT((ULONG)_displayName.c_str()),
            MUIA_Disabled, TRUE,
            Child,(ULONG)(_SelectedItemText),
            Child,(ULONG)pinnerGroup,
        TAG_DONE
        );
#else
     Object *obj = MUI_NewObject(MUIC_Group,
            GroupFrameT((ULONG)_displayName.c_str()),
            MUIA_Disabled, TRUE,
            Child,(ULONG)pinnerGroup,
        TAG_DONE
        );
#endif

    return obj;
}
void MUISerializer::LSwitchGroup::setGroup(const char *pid)
{
    if(!_map || !_Object) return;
#ifdef USEGROUPTITLE
    _displayName = pid;
#else
    _displayName = "Configuration for: ";
    _displayName += pid;
#endif
    ASerializable &actual = _map->get(pid); // the new current data
    _serialized = &actual;

    ReAssigner reassigner(*this);
    actual.serialize(reassigner); // change pointed data and does updates.

     SetAttrs(_Object, MUIA_Disabled, FALSE,TAG_DONE);

         // change title name
#ifdef USEGROUPTITLE
      SetAttrs(_Object,MUIA_FrameTitle,(ULONG)_displayName.c_str(),TAG_DONE);
#else
     SetAttrs(_Object,MUIA_FrameTitle,(ULONG)"",TAG_DONE);
     if(_SelectedItemText)
     {
        SetAttrs(_SelectedItemText,MUIA_Text_Contents,(ULONG)_displayName.c_str(),TAG_DONE);
     }
#endif

     update();
}
// - - - - - - - - - - - - - - -

MUISerializer::LString::LString(MUISerializer &ser,std::string &str,int flgs): Level(ser)
 , _str(&str),_flgs(flgs),_STRING_Path(NULL),_notifyHook({})
{

}
ULONG MUISerializer::LString::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), const char **par REG(a1))
{
    if(!par || !hook) return 0;

    LString *plevel = (LString *)hook->h_Data;
    (*plevel->_str) = *par;

    return 0;
}
void MUISerializer::LString::compile()
{
    if(_flgs & (SERFLAG_STRING_ISPATH|SERFLAG_STRING_ISFILE))
    {
        // if manage path, have a requester and all.
        _Object = MUI_NewObject(MUIC_Popasl,
                  MUIA_Popstring_String,(ULONG)(_STRING_Path = OString(0, 2048)),
                  MUIA_Popstring_Button, (ULONG)(PopButton(MUII_PopDrawer)),
                  ASLFR_DrawersOnly, ( _flgs & SERFLAG_STRING_ISPATH )?TRUE:FALSE,
                TAG_DONE);
    } else
    {
        // if just a simple editable string
        _STRING_Path = OString(0, 2048);
        _Object = _STRING_Path;
    }

    if(_STRING_Path)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&MUISerializer::LString::HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_STRING_Path,MUIM_Notify,
           MUIA_String_Contents/*MUIA_String_Acknowledge*/,
           MUIV_EveryTime,
           _Object,
           3, //?
            MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue
           );
    }


}
void  MUISerializer::LString::update()
{
    if(!_str || !_Object || !_STRING_Path) return;
    Level::update();
    SetAttrs(_STRING_Path,MUIA_String_Contents,(ULONG)_str->c_str(),TAG_DONE);
}
// - - - - - - - - - - - - - - -
ULONG MUISerializer::LSlider::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par) return 0;
    LSlider *plevel = (LSlider *)hook->h_Data;
    if(*plevel->_value != (int) *par)
    {
        *plevel->_value = *par;
        plevel->update();
    }
   return 0;
}
MUISerializer::LSlider::LSlider(MUISerializer &ser,int &value,int min,int max): Level(ser)
 , _value(&value),_min(min),_max(max),_notifyHook({})
{

}
void MUISerializer::LSlider::compile()
{
    _Object = MUI_NewObject(MUIC_Slider,
              MUIA_Slider_Min,    _min,
              MUIA_Slider_Max,    _max,
            TAG_DONE);
    if(_Object)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Object,MUIM_Notify,
                    MUIA_Slider_Level, // attribute that triggers the notification.
                    MUIV_EveryTime, // TrigValue ,  every time when TrigAttr changes
                    _Object, // object on which to perform the notification method. or MUIV_Notify_Self
                    3, // FollowParams  number of following parameters (in hook ?)
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }

}
void MUISerializer::LSlider::update()
{
    if(!_Object) return;
    Level::update();
    SetAttrs(_Object,MUIA_Slider_Level,*_value,TAG_DONE);
}
// - - - - - - - - - - - - - - -
#ifdef MUISERIALIZER_USES_FLOAT
ULONG MUISerializer::LSliderF::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par) return 0;
    LSliderF *plevel = (LSliderF *)hook->h_Data;
    int iv = *par;
    float v = plevel->_fmin + plevel->_fstep * (float)iv;

    if(plevel && plevel->_value &&  *plevel->_value != v)
    {
        *plevel->_value = v;
        plevel->update();
    }
   return 0;
}
ULONG MUISerializer::LSliderF::HNotifyDef(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par) return 0;
    LSliderF *plevel = (LSliderF *)hook->h_Data;

    if(plevel && plevel->_value)
    {
        *plevel->_value = plevel->_defval ;
        plevel->update();
    }
   return 0;
}
MUISerializer::LSliderF::LSliderF(MUISerializer &ser,float &value,float min,float max,float step
    ,float defval): Level(ser)
    ,_Slider(NULL)
    , _pValueLabel(NULL)
    , _value(&value),_fmin(min),_fmax(max),_fstep(step)
    ,_defval(defval)
    ,_nbsteps(0),_imin(0),_imax(0)
    ,_notifyHook({})
    ,_defHook({})
{
    _nbsteps = (int)((_fmax-_fmin)/_fstep);
}
void MUISerializer::LSliderF::compile()
{
    _Slider = MUI_NewObject(MUIC_Slider,
              MUIA_Slider_Min, 0  /* _min*/,
              MUIA_Slider_Max,    _nbsteps,
              MUIA_Slider_Quiet, TRUE, // we don't display the int value.
            TAG_DONE);

    _pValueLabel = Label((ULONG)"-");

    _pDefBt = SimpleButton((ULONG)"Default");

   _Object =MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
            Child,(ULONG)_Slider,
            Child,(ULONG)_pValueLabel,
            Child,(ULONG)_pDefBt,
            TAG_DONE
            );

    if(_Slider)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Slider,MUIM_Notify,
                    MUIA_Slider_Level, // attribute that triggers the notification.
                    MUIV_EveryTime, // TrigValue ,  every time when TrigAttr changes
                    _Slider, // object on which to perform the notification method. or MUIV_Notify_Self
                    3, // FollowParams  number of following parameters (in hook ?)
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }
    if(_pDefBt)
    {
        _defHook.h_Entry =(RE_HOOKFUNC)&HNotifyDef;
        _defHook.h_Data = this;
        DoMethod(_pDefBt, MUIM_Notify, MUIA_Pressed, MUIV_EveryTime,
            _pDefBt,  3,
            MUIM_CallHook,(ULONG) &_defHook,  MUIV_TriggerValue );

    }



}
void MUISerializer::LSliderF::update()
{
    if(!_Slider) return;
    Level::update();
    float v = *_value;
    int i= (int)((v-_fmin)/_fstep);

    int icurrent=0;
    GetAttr(MUIA_Slider_Level,_Slider,(ULONG *)&icurrent);
    if(icurrent != i)
    {
        SetAttrs(_Slider,MUIA_Slider_Level,i,TAG_DONE);
    }
    if(_pValueLabel)
    {
        snprintf(_display,15,"%.3f",v);
        SetAttrs(_pValueLabel,MUIA_Text_Contents,(ULONG)&_display[0],TAG_DONE);
    }


}
#endif
// - - - - - - - - - - - - - - -
MUISerializer::LCycle::LCycle(MUISerializer &ser,int &value,const std::vector<std::string> &values): Level(ser)
 ,_value(&value),_values(values),_notifyHook({})
{
    _valuesptr.reserve(_values.size()+1);
    for(const string &str : _values) _valuesptr.push_back(str.c_str());
    _valuesptr.push_back(NULL);
}

ULONG MUISerializer::LCycle::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par) return 0;
    MUISerializer::LCycle *plevel = (MUISerializer::LCycle *)hook->h_Data;
    if(*plevel->_value != (int) *par)
    {
        *plevel->_value = *par;
        plevel->update();
    }
   return 0;
}

void MUISerializer::LCycle::compile()
{
    _Object = MUI_NewObject(MUIC_Cycle,MUIA_Cycle_Entries,(ULONG)_valuesptr.data(),TAG_DONE);
    if(_Object)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Object, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                   _Object,
                    3, //number of following parameters. If you e.g.
                    // have a notification method with three parts
                    // (maybe MUIM_Set,attr,val), you have to set
                    // FollowParams to 3.
                    // - - - notification methods
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }
}
void MUISerializer::LCycle::update()
{
    if(!_Object) return;
//    Level::update();
    int v = *_value;
    if(v<0) v=0;
    if(_values.size()>0 && v>=(int)_values.size()) v =(int)_values.size()-1;
    SetAttrs(_Object,MUIA_Cycle_Active,v,TAG_DONE);
    int anydone = 0;
    for(function<void(ASerializer &serializer, void *p)> &func : _rules)
    {
        func(_ser,_value);

        anydone = 1;
    }
    if(anydone && _pNextBrother) _pNextBrother->update(); // trick
}
// - - - - - - - - - - - - - - -
MUISerializer::LCheckBox::LCheckBox(MUISerializer &ser,bool &value): Level(ser)
,_Button(NULL), _value(&value),_notifyHook({})
{

}
void MUISerializer::LCheckBox::compile()
{
    _Button = MUI_NewObject(MUIC_Image,
                ImageButtonFrame,
                MUIA_InputMode        , MUIV_InputMode_Toggle,
                MUIA_Image_Spec       , MUII_CheckMark,
                MUIA_Image_FreeVert   , TRUE,
                MUIA_Selected         , (ULONG)(_value?1:0),
                MUIA_Background       , MUII_ButtonBack,
                MUIA_ShowSelState     , FALSE,
                TAG_DONE);

 _Object = MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
            Child,(ULONG)_Button,
            Child, (ULONG)HSpace(0),
            TAG_DONE
            );

    if(_Button)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&MUISerializer::LCheckBox::HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Button,MUIM_Notify,
                    MUIA_Selected, // attribute that triggers the notification.
                    MUIV_EveryTime, // TrigValue ,  every time when TrigAttr changes
                    _Button, // object on which to perform the notification method. or MUIV_Notify_Self
                    3, // FollowParams  number of following parameters (in hook ?)
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);
    }

}
void MUISerializer::LCheckBox::update()
{
    if(!_Button || !_value) return;
    Level::update();
    SetAttrs(_Button,MUIA_Selected,(ULONG)(*_value?1:0),TAG_DONE);

}
ULONG MUISerializer::LCheckBox::HNotify(
        struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1)
        )
{
    if(!hook || !par ) return 0;
    MUISerializer::LCheckBox *plevel = (MUISerializer::LCheckBox *)hook->h_Data;
    if(plevel->_value)
    {
        *plevel->_value = (*par != 0);
    }
    return 0;
}
// ---------------------

MUISerializer::LScreenModeReq::LScreenModeReq(MUISerializer &ser,ULONG_SCREENMODEID &value) : Level(ser)
 ,_value(&value),_ScreenModeStartHook({}),_ScreenModeStopHook({})
{
    _ScreenModeTags =
    {
      { ASLSM_InitialDisplayID,   0 },
      { ASLSM_InitialDisplayDepth,   8 },
      { ASLSM_DoDepth,        TRUE  },
      { ASLSM_MinDepth, 2},
      { TAG_END,0 }
    };
#define SMT_DISPLAYID 0
#define SMT_DEPTH     1
}
void MUISerializer::LScreenModeReq::compile()
{
    _ScreenModeStartHook.h_Entry = (RE_HOOKFUNC) PopupStart;
    _ScreenModeStartHook.h_Data = this;
    _ScreenModeStopHook.h_Entry = (RE_HOOKFUNC) PopupStop;
    _ScreenModeStopHook.h_Data = this;

     _Object = MUI_NewObject(MUIC_Popasl,

              MUIA_Popstring_String,(ULONG)( _DisplayName = MUI_NewObject(MUIC_Text,
                    TextFrame,
                    MUIA_Background, MUII_TextBack,
                  TAG_DONE)),

              MUIA_Popstring_Button,(ULONG)(_PopUpScreenMode = PopButton(MUII_PopUp)),
              MUIA_Popasl_Type,     ASL_ScreenModeRequest,
              MUIA_Popasl_StartHook,  (ULONG) &_ScreenModeStartHook,
              MUIA_Popasl_StopHook, (ULONG) &_ScreenModeStopHook,
            TAG_DONE);


}
void MUISerializer::LScreenModeReq::update()
{
    if(!_value) return;
    Level::update();
    _ScreenModeTags[SMT_DISPLAYID].ti_Data = (*_value)._modeId;
    _ScreenModeTags[SMT_DEPTH].ti_Data = (*_value)._depth;
    SetDisplayName(*_value);
}


ULONG MUISerializer::LScreenModeReq::PopupStart(struct Hook *hook REG(a0), APTR popasl REG(a2), struct TagItem *taglist REG(a1))
{
    if(!hook || !taglist || !hook->h_Data) return 1;
    MUISerializer::LScreenModeReq *plevel = (MUISerializer::LScreenModeReq *)hook->h_Data;

    LONG  i;
    for(i = 0; taglist[i].ti_Tag != TAG_END; i++);

    taglist[i].ti_Tag = TAG_MORE;
    taglist[i].ti_Data  = (ULONG) plevel->_ScreenModeTags.data();

    return(TRUE);
}

ULONG MUISerializer::LScreenModeReq::PopupStop(struct Hook *hook REG(a0), APTR popasl REG(a2), struct ScreenModeRequester *smreq REG(a1))
{
    if(!hook || !popasl || !hook->h_Data) return 1;
    MUISerializer::LScreenModeReq *plevel = (MUISerializer::LScreenModeReq *)hook->h_Data;

    plevel->_ScreenModeTags[SMT_DISPLAYID].ti_Data = smreq->sm_DisplayID;
    plevel->_ScreenModeTags[SMT_DEPTH].ti_Data   = smreq->sm_DisplayDepth;

    if(plevel->_value)
    {
        plevel->_value->_modeId = smreq->sm_DisplayID;
        plevel->_value->_depth = smreq->sm_DisplayDepth;
    }
    plevel->SetDisplayName(*(plevel->_value));

    return 0;
}

void MUISerializer::LScreenModeReq::SetDisplayName(ULONG_SCREENMODEID &displayid)
{
  if(!_DisplayName) return;
  if(displayid._modeId == (ULONG) INVALID_ID)
  {
    _strDisplay = "-";
  }
  else
  {
    LONG v;
    struct NameInfo DisplayNameInfo;
    v = GetDisplayInfoData(NULL, (UBYTE *) &DisplayNameInfo, sizeof(DisplayNameInfo),
                         DTAG_NAME, displayid._modeId);

    if(v > (LONG)sizeof(struct QueryHeader))
    {
        _strDisplay = (const char *) DisplayNameInfo.Name;
        if(displayid._depth<= 8 && displayid._depth>0)
        {
            int nbc=1<<displayid._depth;
            char t[8];
            snprintf(&t[0],7," %dc",nbc);
            _strDisplay += t;
        }

    }

  }

  SetAttrs(_DisplayName, MUIA_Text_Contents, (ULONG) _strDisplay.c_str(),TAG_DONE);
}
// *- -  -- - -
MUISerializer::LInfoText::LInfoText(MUISerializer &ser,strText &str, int flgs)
 : Level(ser)
, _str(&str)
{}
void MUISerializer::LInfoText::compile()
{
    _Object = MUI_NewObject(MUIC_Text,
                    MUIA_Text_Contents,(ULONG)"...",
                  TAG_DONE);
    if(_Object && _str)
    {
        SetAttrs(_Object, MUIA_Text_Contents, (ULONG)_str->c_str(),TAG_DONE);
    }

}

// - - - - - - - - - - - - - - -
// used to change edited object assignment.
MUISerializer::ReAssigner::ReAssigner::ReAssigner(MUISerializer::Level &group)
{
    _stack.push_back(&group);
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    //TODO if you want recursion
    //    LGroup *pgroup = (LGroup *)_stack.back();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, std::string &str, int flags)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LString *pstr = (LString *) pgroup->getChild(sMemberName);
    if(!pstr) return;
    pstr->_str = &str;
    pstr->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, int &v, int min, int max, int defv)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LSlider *pslider = (LSlider *) pgroup->getChild(sMemberName);
    if(!pslider) return;
    pslider->_value  = &v;
    pslider->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LCycle *pcycle = (LCycle *) pgroup->getChild(sMemberName);
    if(!pcycle) return;
    pcycle->_value  = &v;
    pcycle->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, bool &v)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LCheckBox *pcb = (LCheckBox *) pgroup->getChild(sMemberName);
    if(!pcb) return;
    pcb->_value  = &v;
    pcb->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, ULONG_SCREENMODEID &v)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LScreenModeReq *preq = (LScreenModeReq *) pgroup->getChild(sMemberName);
    if(!preq) return;
    preq->_value  = &v;
    preq->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LFlags *pflags = (LFlags *) pgroup->getChild(sMemberName);
    if(!pflags) return;
    pflags->_pflugs  = &v;
    pflags->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, AStringMap &m)
{
    // todo... shouldnt be recursive...
}
#ifdef MUISERIALIZER_USES_FLOAT
void MUISerializer::ReAssigner::operator()(const char *sMemberName, float &v, float min, float max,float step,float defval)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LSliderF *pslider = (LSliderF *) pgroup->getChild(sMemberName);
    if(!pslider) return;
    pslider->_value  = &v;
    pslider->update();
}
#endif
void MUISerializer::ReAssigner::listenChange(const char *sMemberName,std::function<void(ASerializer &serializer, void *p)> condition)
{
    //printf("MUISerializer::listenChange:%s\n",sMemberName);
    std::list<Level *>::reverse_iterator rit = _stack.rbegin();
    while(rit != _stack.rend())
    {
        Level *pl = *rit++;
        if(pl && pl->_pMemberName && strcmp(sMemberName, pl->_pMemberName)==0) {
              //  printf("push condition\n");
            pl->_rules.clear();
            pl->_rules.push_back(condition);
            pl->update();
            break;
        }
    }
}

