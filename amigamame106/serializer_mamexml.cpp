#include "serializer_mamexml.h"

// from mame
extern "C" {
    #include "xmlfile.h"
}


using namespace std;

xmlwriter::xmlwriter()
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

xmlreader::xmlreader()
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
