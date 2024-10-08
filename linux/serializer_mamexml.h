#ifndef KRB_SERIALIZER_MAMEXML_H_
#define KRB_SERIALIZER_MAMEXML_H_

#include "serializer.h"
#include <vector>

extern "C" {
struct _xml_data_node;
typedef struct _xml_data_node xml_data_node;
}

struct XmlWriter : public ASerializer {
	
	XmlWriter(xml_data_node *rootnode);
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str,int flags=0) override;
    void operator()(const char *sMemberName, int &v, int min, int max, int defvlt) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;
    void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) override;
    void operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)override;

    void operator()(const char *sMemberName, AStringMap &m) override;
    void operator()(const char *sMemberName, float &v, float min, float max,float step,float defval) override;


    std::vector<xml_data_node *> _recursenode;
    int                     _error;
};

struct XmlReader : public ASerializer {
	
	XmlReader(xml_data_node *rootnode);
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str,int flags) override;
    void operator()(const char *sMemberName, int &v, int min, int max, int vdef) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;
    void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) override;
    void operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)override;

    void operator()(const char *sMemberName, AStringMap &m) override;
    void operator()(const char *sMemberName, float &v, float min, float max,float step,float defval) override;

    std::vector<xml_data_node *> _recursenode;
    int                     _error;
};

#endif
