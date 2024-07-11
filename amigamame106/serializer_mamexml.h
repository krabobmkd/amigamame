#ifndef KRB_SERIALIZER_MAMEXML_H_
#define KRB_SERIALIZER_MAMEXML_H_

#include "serializer.h"


struct xmlwriter : public ASerializer {
	
	xmlwriter();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str,int flags=0) override;
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;

};

struct xmlreader : public ASerializer {
	
	xmlreader();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str,int flags) override;
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;

};

#endif
