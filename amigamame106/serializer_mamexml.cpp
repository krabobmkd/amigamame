#include "serializer_mamexml.h"

//already #include <string>
#include <sstream>
// needed for sscanf
#include <stdio.h>
// from mame
extern "C" {
    #include "xmlfile.h"
}


using namespace std;


std::string checkXmlName(const char *p)
{
    stringstream ss;
    int i=0;
    while(*p) {
        char c =*p++;
        if(c != ' ' && c != '%' && c != '(' && c != ')' && c != '.')
        {
            if(c >= 'A' && c<='Z') c+=32;
            ss << c;
            i++;
        }
        if(i>=12) break;
    }
    return ss.str();
}


//    if(!_romsDir.empty()) xml_add_child(confignode,pcf_romsdir, _romsDir.c_str());
//    if(!_userDir.empty()) xml_add_child(confignode,pcf_userdir, _userDir.c_str());


XmlWriter::XmlWriter(xml_data_node *rootnode) : ASerializer()
, _error(0)
{
    _recursenode.push_back(rootnode);
}
void XmlWriter::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *pgroup = xml_add_child(_recursenode.back(),name.c_str(), NULL );
    if(!pgroup)
    {   // more likely would throw with gcc6.5
        return;
    }

    _recursenode.push_back(pgroup);

        subconf.serialize(*this);

    _recursenode.pop_back();

}
void XmlWriter::operator()(const char *sMemberName, std::string &str,int flags)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_add_child(_recursenode.back(),name.c_str(), str.c_str() );

}
void XmlWriter::operator()(const char *sMemberName, int &v, int min, int max, int defv)
{
    if(defv == v) return;
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_add_child(_recursenode.back(),name.c_str(),  NULL );
    if(p) xml_set_attribute_int(p,"v",v);

}
void XmlWriter::operator()(const char *sMemberName, float &v, float min, float max,float step,float defval)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_add_child(_recursenode.back(),name.c_str(),  NULL );
    // beta4: float read write problematic with gcc bebbo locale mess. -> cast to int.
    if(p) xml_set_attribute_int(p,"vi",(int)(v*1024.0f));
}


void XmlWriter::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
    string name = checkXmlName(sMemberName);

    int vv = v;
    if(vv<0) vv=0;

    if(values.size()>0 && vv>=(int)values.size()) vv = (int)values.size()-1;

    xml_data_node *p = xml_add_child(_recursenode.back(),name.c_str(), NULL/*values[vv].c_str()*/ );
    xml_set_attribute_int(p,"v",vv);

}
void XmlWriter::operator()(const char *sMemberName, bool &v)
{
    string name = checkXmlName(sMemberName);
    // true if tag present...
    if(v)
    {
        xml_add_child(_recursenode.back(),name.c_str(),NULL );
    }
}
void XmlWriter::operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)
{
    UINT32 ib=1;
    for(int i=0;i<values.size() ;i++)
    {
        string name = checkXmlName(values[i].c_str());
        if(ib & v) xml_add_child(_recursenode.back(),name.c_str(),NULL );
        ib<<=1;
    }
}


void XmlWriter::operator()(const char *sMemberName, ULONG_SCREENMODEID &v)
{
    string name = checkXmlName(sMemberName);
    // true if tag present...
    char temp[16];
    snprintf(temp,15,"0x%08x %d",(int)v._modeId,(int)v._depth);

    if(v._modeId != ~0 && v._modeId != 0 )
    {
        xml_data_node *p = xml_add_child(_recursenode.back(),name.c_str(),NULL );
        if(p) xml_set_attribute(p,"modeid",temp);
    }
}
// per optional subconf
void XmlWriter::operator()(const char *sMemberName, AStringMap &confmap)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_add_child(_recursenode.back(),name.c_str(),NULL );
    if(!p) return;
    confmap.begin();
    pair<string,ASerializable *> pr = confmap.getNext();
    while(pr.second)
    {
        if(pr.first.size()>0 && pr.second != NULL && !pr.second->isDefault())
        {
            xml_data_node *psub = xml_add_child(p,"conf",NULL );
            if(psub) xml_set_attribute(psub,"n",pr.first.c_str());
            _recursenode.push_back(psub);
                pr.second->serialize(*this);
            _recursenode.pop_back();
        }
        pr = confmap.getNext();
    }
}
// -------------------------------

XmlReader::XmlReader(xml_data_node *rootnode) : ASerializer()
, _error(0)
{
    _recursenode.push_back(rootnode);
}

void XmlReader::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    string name = checkXmlName(sMemberName);

    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(p)
    {
        _recursenode.push_back(p);
            subconf.serialize(*this);
        _recursenode.pop_back();
    }
}
void XmlReader::operator()(const char *sMemberName, std::string &str, int flags)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(p && p->value) str = p->value;

}
void XmlReader::operator()(const char *sMemberName, int &v, int min, int max, int defv)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(p)
    {
        int vv =  xml_get_attribute_int(p,"v",defv);
        if(vv<min) vv=min;
        if(vv>max) vv=max;
        v = vv;
    } else
    {
        v = defv;
    }
}
void XmlReader::operator()(const char *sMemberName,  float &v, float min, float max,float step,float defval )
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(p)
    {
        // now cast int to float because locale are problematics
        int iv =  xml_get_attribute_int(p,"vi",defval);
        float vv = (float)iv * (1.0f/1024.0f);
        if(vv<min) vv=min;
        if(vv>max) vv=max;
        v = vv;
    } else {
        v = defval;
    }
}

void XmlReader::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(p)
    {
        int vv =  xml_get_attribute_int(p,"v",0);
        if(vv<0) vv=0;
        if(values.size()>0 && vv>=(int)values.size()) vv = values.size()-1;
        v = vv;
    }
}
void XmlReader::operator()(const char *sMemberName, bool &v)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    v = (p!=NULL);
}
void XmlReader::operator()(const char *sMemberName, ULONG_SCREENMODEID &v)
{
    string name = checkXmlName(sMemberName);
    xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(p)
    {
        xml_attribute_node * pattrib =  xml_get_attribute(p,"modeid");
        if(pattrib && pattrib->value)
        {
            unsigned int vv,dd;
            int done = sscanf(pattrib->value,"0x%08x %d",&vv,&dd);
            if(done >=1) v._modeId=vv;
            if(done >=2) v._depth = dd;
            else v._depth = 8;
        }
    }
}
void XmlReader::operator()(const char *sMemberName, ULONG_FLAGS &v,ULONG_FLAGS valdef,const std::vector<std::string> &values)
{
    UINT32 ib=1;
    ULONG_FLAGS fv=0;
    for(int i=0;i<values.size() ;i++)
    {
        string name = checkXmlName(values[i].c_str());
        xml_data_node *p = xml_get_sibling(_recursenode.back()->child, name.c_str());
        if(p)
        {   // means on
            fv |=ib;
        }
        ib<<=1;
    }
    v = fv;
}

// per optional subconf

void XmlReader::operator()(const char *sMemberName, AStringMap &confmap)
{
    confmap.clear();
    string name = checkXmlName(sMemberName);

    xml_data_node *pmapnode = xml_get_sibling(_recursenode.back()->child, name.c_str());
    if(!pmapnode) return;

    xml_data_node *p = xml_get_sibling(pmapnode->child, "conf");

    while(p)
    {
        const char *pScreenConfId = xml_get_attribute_string(p,"n",NULL);

        if(pScreenConfId)
        {
            // create and get
            ASerializable &s = confmap.get(pScreenConfId);

            _recursenode.push_back(p);
                s.serialize(*this);
            _recursenode.pop_back();

        }
        p = xml_get_sibling(p->next, "conf");
    }
}
