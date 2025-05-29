#ifndef KRB_SERIALIZER_H_
#define KRB_SERIALIZER_H_

#include <vector>
#include <map>
#include <string>
#include <functional>
struct ASerializer;

#define SERFLAG_STRING_ISPATH 1
#define SERFLAG_STRING_ISFILE 2

#define SERFLAG_GROUP_TABS 1
#define SERFLAG_GROUP_2COLUMS 2
#define SERFLAG_GROUP_FLAGINT2COLUMS (1<<24)
#define SERFLAG_GROUP_SCROLLER 4

struct ASerializable {
    ASerializable() {}
    virtual ~ASerializable() {}
    virtual void serialize(ASerializer &serializer)=0;
    // - - - - - - - - - to apply rules...
    virtual void valueUpdated(std::string upatedValue) {}
    // usefull in some case when in container
    //virtual void youAreCreatedWithId(const char *pId) {}
    // to optimize serialization
    virtual bool isDefault() { return false; }
};

// specific serializable types
typedef std::string strcomment;
typedef std::string strText;
//typedef unsigned int ULONG_SCREENMODEID;
typedef struct {
    unsigned int _modeId;
    unsigned int _depth;
} ULONG_SCREENMODEID;
// list of booleans
typedef unsigned int ULONG_FLAGS;

// just to have write file optimized
typedef int boolDefFalse;

struct ASerializer {
    ASerializer()
    {}
    virtual ~ASerializer()
    {}

    virtual void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) = 0;
    virtual void operator()(const char *sMemberName, std::string &str, int flags) = 0;
    // for sliders
    virtual void operator()(const char *sMemberName, int &v, int min, int max, int vdeflt) = 0;
    // for cycling
    virtual void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) = 0;
    // for many checkbox flags
    virtual void operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values) = 0;

    // for checkbox
    virtual void operator()(const char *sMemberName, bool &v) = 0;
    virtual void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) = 0;
   //re? virtual void operator()(const char *sMemberName, strcomment &str) = 0;
    virtual void operator()(const char *sMemberName, strText &str) {}

#ifdef MUISERIALIZER_USES_FLOAT
    // for sliders
    virtual void operator()(const char *sMemberName, float &v, float min, float max,float step,float defval) =0;
#endif

    // - - - -rules
    virtual void listenChange(const char *sMemberName,std::function<void(ASerializer &serializer, void *p)> condition) {}
    virtual void enable(std::string memberUrl, int enable) {}
    virtual void update(std::string memberUrl) {}
    virtual ASerializable *getObject(std::string memberUrl) { return NULL; }
    // - - - - -  serialize abstract class string map - - - - -
    // first use to serialize confs per screen mode.
    struct AStringMap {
        virtual void clear() = 0;
        virtual bool contains(const char *pid)=0;
        virtual ASerializable &get(const char *pid)=0;
        virtual void remove(const char *pid) = 0;
        // simple way to iterate
        virtual void begin() = 0;
        virtual std::pair<std::string,ASerializable *> getNext() = 0;
    };
    virtual void operator()(const char *sMemberName, AStringMap &m) = 0;

     template<class ASER>
    struct StringMap : public AStringMap {
        StringMap(std::map<std::string,ASER> &v) : AStringMap() ,_v(v){}
         void clear() override { _v.clear(); }
        bool contains(const char *pid) override { return (_v.find(pid)!=_v.end()); }
        ASerializable &get(const char *pid) override {
            //bool isCreated = !contains(pid);
            //ASerializable &o = (ASerializable &)_v[pid];
            //if(isCreated) o.youAreCreatedWithId(pid);
            return (ASerializable &)_v[pid]; }
        void remove(const char *pid) override {
            typename std::map<std::string,ASER>::iterator fit = _v.find(pid);
            if(fit != _v.end()) _v.erase(fit);
        }
        void begin() override { _it = _v.begin(); }
        std::pair<std::string,ASerializable *> getNext() override {
            if(_it == _v.end()) return std::make_pair<std::string,ASerializable *>("",NULL);
            typename std::pair<const std::string,ASER> &p = *_it++;
            return {p.first,(ASerializable *)&p.second};
                    //std::make_pair<std::string,ASerializable *>(p.first,(ASerializable *)&p.second);
        }
        void setActive(std::string strid) { _active = strid; }
        ASER &getActive(){ return _v[_active]; };
        std::map<std::string,ASER> &_v;
        std::string _active; // set here because suits fine, we map and tell current selection in map.
        typename std::map<std::string,ASER>::iterator _it;
    };

//    template<class ASER>
//    void operator()(const char *sMemberName,std::map<std::string,ASER> &v) {

//        StringMap<ASER> m(v);
//        this->operator()(sMemberName, (AStringMap &)m);
//    }

};



#endif
