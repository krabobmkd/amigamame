#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <memory>
#include <algorithm>

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;


std::string trim(const std::string& str) {
    const std::string whitespace = " \t\r";
    auto begin = str.find_first_not_of(whitespace);
    if (begin == std::string::npos) begin=0;

    auto end = str.find_last_not_of(whitespace);
    string nstr =str.substr(begin, end - begin + 1);
  //  str = nstr;
    return nstr;
}
inline void sepline(const string &line,const string sep,string &partA, string &partB)
{
    size_t i = line.find(sep);
    if(i == string::npos ) {
        partA = line;
        return;
    }
    partA = line.substr(0,i);
    partB = line.substr(i+sep.length());
    trim(partA);
    trim(partB);
}
vector<string> splitt(const string &s, const string sep)
{
    vector<string> v;
    size_t i=0;
    size_t in = s.find(sep);
    do {
        v.push_back(s.substr(i,in-i));
        if(in == string::npos) break;
        i = in+sep.length();
        in = s.find(sep,i+sep.length());
    } while(i != string::npos);
    return v;
}

string replace(const string &s, const string orig, const string rep)
{
    stringstream ss;
    size_t i=0;
    size_t in = s.find(orig);
    do {
        ss << s.substr(i,in-i);
        if(in == string::npos) break;
        i = in+orig.length();
        ss << rep;
        in = s.find(orig,i+orig.length());
    } while(i != string::npos);
    return ss.str();
}
vector<string> splitt_trim(const string &s, const string sep)
{
    vector<string> v;
    size_t i=0;
    size_t in = s.find(sep);
    do {
        string ss = s.substr(i,in-i);
        ss = replace(ss,"\r","");
        ss = replace(ss,"\n","");
        ss = trim(ss);
        v.push_back(ss);
        if(in == string::npos) break;
        i = in+sep.length();
        in = s.find(sep,i+sep.length());
    } while(i != string::npos);
    return v;
}

// caution, sucks.
string trimquotes(std::string s)
{
    trim(s);
    replace(s,"\"","");
    return s;
}
void toUpper(string &s) {
    transform(s.begin(), s.end(), s.begin(),
              ::toupper);
}

string rgetline(istream &ifs)
{
    string s;
    getline(ifs,s);
    if(s.length()>0 && s.back()=='\r') s.pop_back();
    return s;
}



struct paragr {
    int _level;
    bool _isdot=false;
    string _tit;
    vector<string> _t;

    vector<paragr> _prgs;
};

class Agdoc {
public:
    void treatmdfile( string &mdpath);

    void export_ag(ofstream &ofs);
    vector< shared_ptr<paragr> > _prgs;

};
bool startWith(const string &s,const string &t)
{
    return(s.find(t) != 0);
}

void Agdoc::treatmdfile(  string &mdpath)
{
    ifstream ifsmd(mdpath);
    if(!ifsmd.good()) return;

    shared_ptr<paragr> curprg = make_shared<paragr>();
    _prgs.push_back(curprg);

    string l;
    getline(ifsmd,l);
    while(!ifsmd.eof())
    {
       l = trim(l);
       if(l.length()==0 )
       {
        getline(ifsmd,l);
        continue;
       }
       if(startWith(l,"###"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);

            curprg->_tit = trim(l.substr(3));
            curprg->_level = 3;
       } else if(startWith(l,"##"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);

            curprg->_tit = trim(l.substr(2));
            curprg->_level = 2;
       } else if(startWith(l,"#"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);

            curprg->_tit = trim(l.substr(1));
            curprg->_level = 1;
       } else if(startWith(l,"**"))
       {
            curprg->_t.push_back(l);
       }else if(startWith(l,"*"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);
            curprg->_isdot = true;
            curprg->_t.push_back( l);

       }else if(startWith(l,"-"))
       {
           curprg = make_shared<paragr>();
            _prgs.push_back(curprg);
            curprg->_isdot = true;
            curprg->_t.push_back(l);
       } else
       {
            curprg->_t.push_back(l);
       }

        getline(ifsmd,l);
    }
}
void toLower(string &s) {
    transform(s.begin(), s.end(), s.begin(),
              ::tolower);
}

string titleToMdId(string t)
{
    string trimt = trim(t);
 trimt = replace(trimt," ","-");
 toLower(trimt);
 return trimt;
}

void Agdoc::export_ag(ofstream &ofs)
{
    ofs << "@DATABASE\n";
 bool nodeison=false;
 bool isFirstNode=true;
 vector<shared_ptr<paragr>>::iterator it = _prgs.begin();

 while(it != _prgs.end() )
 {
    shared_ptr<paragr> p = *it++;
    if(p->_level>0) {
        nodeison = true;
        if(nodeison) ofs << "@ENDNODE\n";
         string nodename= titleToMdId(p->_tit);
         if(isFirstNode) nodename = "MAIN";

         ofs << "@NODE "<<nodename<<" \""<< p->_tit <<"\"\n";

    }

// titleToMdId

    it++;
 }
 if(nodeison) ofs << "@ENDNODE\n";
/*
  @DATABASE
  @NODE MAIN "Intuition Table of Contents"
  @{"Introduction" LINK Intro}
  @{"Screens" LINK Screen}
  @{"Windows" LINK Window}
  @{"Gadgets" LINK Gadget}
  @{"Menus" LINK Menu}
  @ENDNODE
*/

}

int main(int argc, char **argv)
{
    if(argc <2) return 0;


      //  curprg = make_shared<paragr>();

    Agdoc d;
    string ppth(argv[1]);
    d.treatmdfile(ppth);

    ofstream ofsag("Mame106Minimix.guide");
    if(ofsag.good()) d.export_ag(ofsag);

    return 0;
}
