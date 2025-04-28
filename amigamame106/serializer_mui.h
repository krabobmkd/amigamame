#ifndef KRB_SERIALIZER_MUI_H_
#define KRB_SERIALIZER_MUI_H_

#include "serializer.h"
#include <vector>
#include <list>
extern "C" {
	#include <exec/types.h>
    #include <intuition/classusr.h>
    #include <utility/hooks.h>
}
#ifdef __GNUC__
#define REG(r) __asm(#r)
#else
#define REG(r)
#endif

#define USEGROUPTITLE 1

struct ScreenModeRequester;

/** Will mirror MUI panels and gadgets to serialized data.
*/
struct MUISerializer : public ASerializer {

	MUISerializer();
    virtual ~MUISerializer();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;

    void operator()(const char *sMemberName, std::string &str, int flags=0) override;
    // for sliders
    void operator()(const char *sMemberName, int &v, int min, int max, int defv) override;
    // for cycling
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    // for checkbox
    void operator()(const char *sMemberName, bool &v) override;
    // for many flags
    void operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)override;
    // for screen ids
    void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) override;
	// serialize abstract class string map
    void operator()(const char *sMemberName, AStringMap &m) override;

    void operator()(const char *sMemberName, strText &str) override;

#ifdef MUISERIALIZER_USES_FLOAT
    // for sliders
    void operator()(const char *sMemberName, float &v, float min, float max,float step,float defval) override;
#endif

    // - - - -rules
    void listenChange(const char *sMemberName,std::function<void(ASerializer &serializer, void *p)> condition) override;
    void enable(std::string memberUrl, int enable) override;
    //void update(std::string memberUrl) override;
	// - - - - - -
    // allow insertion of tabs before compile...
    void insertFirstPanel(Object *pPanel,const char *pName);

    Object *compile();

    void updateUI();

    // switch LGroupSubMap
    void selectGroup(std::string groupurl,std::string selection);

protected:
	struct Level {
    	Level(MUISerializer &ser);
    	virtual ~Level();
        MUISerializer &_ser;
        Object *_Object; // if NULL, need compilation at end, else leaf are done at once
        const char *_pMemberName;
        struct Level *_pFirstChild;
        struct Level *_pNextBrother;
        virtual void compile() {}
        virtual void update();
        Level *getChild(const char *pMemberName);
        // rules to apply at update.
        std::vector<std::function<void(ASerializer &serializer, void *p)>> _rules;
	};
    struct LGroup : public Level {
        LGroup(MUISerializer &ser,int flgs);
        void compile() override;
        void update() override;
        virtual Object *compileOuterFrame(Object *pinnerGroup);
        int _flgs;
	};
    struct LFlags : public Level {
        LFlags(MUISerializer &ser,ULONG_FLAGS &flugs,ULONG_FLAGS defval,const std::vector<std::string> &names);
        void compile() override;
        void update() override;
        ULONG_FLAGS *_pflugs;
        ULONG _defval;
        std::vector<std::string> _flagNames;
        std::vector<Object *> _buttons;
        struct Hook _notifyHook;
        static ULONG HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));

	};
    struct LSwitchGroup : public LGroup {
        LSwitchGroup(MUISerializer &ser,int flgs,AStringMap &map);
        Object *compileOuterFrame(Object *pinnerGroup) override;
        void setGroup(const char *pid);
        AStringMap *_map;
        std::string _displayName;
#ifndef USEGROUPTITLE
        Object *_SelectedItemText;
#endif
	};
    struct LTabs : public LGroup {
        LTabs(MUISerializer &ser);
        void compile() override;
        std::vector<const char *> _registerTitles;
	};

    struct LString : public Level {
        static ULONG HNotify(struct Hook *hook REG(a0), APTR obj REG(a2),const char **par REG(a1));
        LString(MUISerializer &ser,std::string &str, int flgs);
        void compile() override;
        void update() override;
        std::string *_str;
        int _flgs;
        Object *_STRING_Path;
        struct Hook _notifyHook;
	};
    struct LSlider : public Level {
        static ULONG HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        LSlider(MUISerializer &ser,int &value,int min,int max);
        void compile() override;
        void update() override;
        int *_value;
        int _min,_max;
        struct Hook _notifyHook;
	};
#ifdef MUISERIALIZER_USES_FLOAT
    struct LSliderF : public Level {
        static ULONG HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        static ULONG HNotifyDef(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        LSliderF(MUISerializer &ser,float &value,float min,float max,float step,float defval);
        void compile() override;
        void update() override;
        Object *_Slider,*_pValueLabel,*_pDefBt;
        float *_value;
        float _fmin,_fmax;
        float _fstep;
        float _defval;
        int _nbsteps;
        int _imin,_imax;
        char _display[16];
        struct Hook _notifyHook;
        struct Hook _defHook;
	};
#endif
    struct LCycle : public Level {
        static ULONG HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        LCycle(MUISerializer &ser,int &value,const std::vector<std::string> &values);
        void compile() override;
        void update() override;
        int *_value;
        std::vector<std::string> _values;
        std::vector<const char *> _valuesptr;
        struct Hook _notifyHook;
	};
    struct LCheckBox : public Level {
        LCheckBox(MUISerializer &ser,bool &value);
        void compile() override;
        void update() override;
        Object *_Button;
        bool *_value;
        static ULONG HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        struct Hook _notifyHook;
	};
    struct LScreenModeReq : public Level {
        LScreenModeReq(MUISerializer &ser,ULONG_SCREENMODEID &value);
        void compile() override;
        void update() override;
        ULONG_SCREENMODEID *_value;
        Object *_DisplayName;
        Object *_PopUpScreenMode;
        struct Hook _ScreenModeStartHook;
        struct Hook _ScreenModeStopHook;
        static ULONG PopupStart(struct Hook *hook REG(a0), APTR popasl REG(a2), struct TagItem *taglist REG(a1));
        static ULONG PopupStop(struct Hook *hook REG(a0), APTR popasl REG(a2), struct ScreenModeRequester *smreq REG(a1));
        void SetDisplayName(ULONG_SCREENMODEID &displayid);
        std::vector<struct TagItem> _ScreenModeTags;
        std::string _strDisplay;
	};
    struct LInfoText : public Level {
        LInfoText(MUISerializer &ser,strText &str, int flgs);
        void compile() override;
        strText *_str;
	};


    std::list<Level *> _stack;
    int _irecurse;
    Level **_pGrower;
    Level *_pRoot;

    struct Level *getByUrl(const std::string &memberUrl);

    // used to change asignment by LGroupSubMap.
    struct ReAssigner : public ASerializer {
        ReAssigner(Level &group);
        void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
        void operator()(const char *sMemberName, std::string &str, int flags=0) override;
        void operator()(const char *sMemberName, int &v, int min, int max, int defv) override;
        void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
        void operator()(const char *sMemberName, bool &v) override;
        void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) override;
        void operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)override;
        void operator()(const char *sMemberName, AStringMap &m) override;
#ifdef MUISERIALIZER_USES_FLOAT
        void operator()(const char *sMemberName, float &v, float min, float max,float step,float defval) override;
#endif
        void listenChange(const char *sMemberName,std::function<void(ASerializer &serializer, void *p)> condition) override;
        std::list<Level *> _stack;
    };

};


#endif
