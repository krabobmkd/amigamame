#ifndef KRB_SERIALIZER_MAMEXML_H_
#define KRB_SERIALIZER_MAMEXML_H_

#include "serializer.h"


extern "C" {
struct _xml_data_node;
typedef struct _xml_data_node xml_data_node;
}

struct xmlwriter : public ASerializer {
	
	xmlwriter(xml_data_node *rootnode);
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str,int flags=0) override;
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;

    xml_data_node *_rootnode;
};

struct xmlreader : public ASerializer {
	
	xmlreader(xml_data_node *rootnode);
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str,int flags) override;
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;
    xml_data_node *_rootnode;
};

#endif
