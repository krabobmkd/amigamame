#include "serializer_mamexml.h"

// from mame
extern "C" {
    #include "xmlfile.h"
}


using namespace std;

//    if(!_romsDir.empty()) xml_add_child(confignode,pcf_romsdir, _romsDir.c_str());
//    if(!_userDir.empty()) xml_add_child(confignode,pcf_userdir, _userDir.c_str());


xmlwriter::xmlwriter(xml_data_node *rootnode) : ASerializer()
 ,_rootnode(rootnode)
{
}
void xmlwriter::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
}
void xmlwriter::operator()(const char *sMemberName, std::string &str,int flags)
{
}
void xmlwriter::operator()(const char *sMemberName, int &v, int min, int max) 
{
}
void xmlwriter::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) 
{
}
void xmlwriter::operator()(const char *sMemberName, bool &v) 
{
}

// -------------------------------

//   node = xml_get_sibling(confignode->child, pcf_romsdir);
//    if(node && node->value) _romsDir = node->value;

//    node = xml_get_sibling(confignode->child, pcf_userdir);
//    if(node && node->value) _userDir = node->value;


xmlreader::xmlreader(xml_data_node *rootnode) : ASerializer()
 ,_rootnode(rootnode)
{
}
void xmlreader::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
}
void xmlreader::operator()(const char *sMemberName, std::string &str, int flags)
{
}
void xmlreader::operator()(const char *sMemberName, int &v, int min, int max) 
{
}
void xmlreader::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
}
void xmlreader::operator()(const char *sMemberName, bool &v) 
{
}
