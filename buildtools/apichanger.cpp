#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <memory>
#include <dirent.h>
//namespace fs = std::filesystem;

using namespace std;

string sourcebase("../mame106/");

inline bool isJustComment(const string &s)
{
    if(s.empty()) return false;
    for(char c : s)
    {
        if(c=='#') return true;
        if(c==' ' || c=='\t') continue;
        return false;
    }
    return false; // shnhap.
}

std::string trim(std::string& str) {
    const std::string whitespace = " \t\r";
    auto begin = str.find_first_not_of(whitespace);
    if (begin == std::string::npos) begin=0;

    auto end = str.find_last_not_of(whitespace);
    string nstr =str.substr(begin, end - begin + 1);
    str = nstr;
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

string getMacroFirstParam(string line,string macroname)
{
    size_t i =line.find(macroname);
    if(i == string::npos) return string();

    size_t ifp = line.find_first_not_of(" \t(",i+macroname.length());
    if(ifp == string::npos) return string();

    size_t ifpe = line.find_first_of(" ,\t)",ifp);
    if(ifpe == string::npos) return string();

    return line.substr(ifp,ifpe-ifp);

}
string getMacroSecondParam(string line,string macroname)
{
    size_t i =line.find(macroname);
    if(i == string::npos) return string();

    size_t ifp = line.find_first_not_of(" \t(",i+macroname.length());
    if(ifp == string::npos) return string();

    size_t ifp2 = line.find_first_of(",",ifp);
    if(ifp2 == string::npos) return string();

    size_t ifp3 = line.find_first_not_of(" \t",ifp2+1);
    if(ifp3 == string::npos) return string();

    size_t ifpe = line.find_first_of(" ,\t)",ifp3);
    if(ifpe == string::npos) return string();

    return line.substr(ifp3,ifpe-ifp3);

}


void removeInVector(vector<string> &v, string s)
{
    vector<string>::iterator it = v.begin();
    while(it != v.end())
    {
        if(*it != s) ++it;
        else
        {
            it = v.erase(it);
        }
    }
}
// - - - - - - - - - -
#define TYPE_MAIN 0
#define TYPE_BULK 1
#define TYPE_BRACE 2
#define TYPE_PAR 3
#define TYPE_COMMENT 4

//struct cPart {
//    int _type;
//    std::string _content;
//    vector<cPart> _parts;
//};


struct CMultiSearchPart {
   std::string _s;
   bool     _anyof;

};
//struct CMultiSearch {
//    vector<CMultiSearchPart> _l= {
//        {"/*",false},
//        {"//",false},
//        {"//",false},
//    };
size_t findNextOfInterest(int &type,const std::string &s, size_t istart, vector<string> extralist)
{
    static vector<CMultiSearchPart> l= {
        {"/*",false},
        {"//",false},
        {"{(",true},
        {"})",true},
    };
    size_t ires=string::npos;
    int ityperes=-1;
    int itype=0;
    for(const CMultiSearchPart &p : l)
    {
        size_t ti;
        if(p._anyof)
        {
            ti = s.find_first_of(p._s,istart);
        } else
        {
            ti = s.find(p._s,istart);
        }
        if(ti != string::npos) {
            if(ires == string::npos) {
                ires = ti;
                ityperes = itype;
            }else
            {
                if(ti<ires)
                {
                    ires = ti;
                    ityperes = itype;
                }
            }
        }
        itype++;
    }
    for(const string &s : extralist)
    {
        size_t ti;
        ti = s.find(s,istart);

        if(ti != string::npos) {
            if(ires == string::npos) {
                ires = ti;
                ityperes = itype;
            }else
            {
                if(ti<ires)
                {
                    ires = ti;
                    ityperes = itype;
                }
            }
        }
        itype++;
    }
    return ires;
    type = ityperes;
}

//};


struct CFileModifier {
    std::string _func; // drawgfx ()


};

struct CFileParse {
    size_t parseStruct(const string &sfile,size_t ic,int recurse,const CFileModifier &m);
    int _type;
    //std::string _content;
    size_t _start;
    size_t _end;
    vector<CFileParse> _parts;
};

size_t CFileParse::parseStruct(const string &sfile,size_t ic,int recurse,const CFileModifier &m)
{
//    size_t startpar = string::npos;
//    size_t startbra = string::npos;

    while(ic<sfile.length())
    {
//        size_t i1 = sfile.find("/*",ic);
//        size_t i2 = sfile.find("//",ic);
//        size_t i = sfile.find_first_of("{(",ic);
//        size_t j = sfile.find_first_of("})",ic);
//        size_t mi = sfile.find(m._func,ic);
        int typefound;
        size_t icn = findNextOfInterest(typefound,sfile,ic,{m._func});

// i1 != string::npos && i1<i2 && i1<i && i1<j
        if(typefound == 0)
        {   // comment1 on the way
            _parts.push_back({TYPE_BULK,ic,icn});
            size_t iend = sfile.find("*/",icn+2);
            if(iend != string::npos) { _parts.push_back({TYPE_COMMENT,icn,iend+2});
                    icn = iend+2; } else icn = sfile.length();
        } else
        if(typefound == 1)
       // if(i2 != string::npos && i2<i1 && i2<i && i2<j)
        {   // comment2 on the way
            _parts.push_back({TYPE_BULK,ic,icn});
            size_t iend = sfile.find("\n",icn+2);
            if(iend != string::npos) { _parts.push_back({TYPE_COMMENT,icn,iend+1});
                    icn = iend+1; } else icn = sfile.length();
        } else
        if(typefound == 2)
       // if(i != string::npos && i<i1 && i<i2 && i<j)
        {   // either ( ot {
            if(sfile[icn]=='(' ) {
                 _parts.push_back(CFileParse());
                CFileParse &b=_parts.back();
                b._type = TYPE_PAR;
                b._start = icn;
                b._end = icn; // to be changed
                ic = b.parseStruct(sfile,icn+1,recurse+1,m);
            }
            if(sfile[icn]=='{' ) {
                _parts.push_back(CFileParse());
               CFileParse &b=_parts.back();
               b._type = TYPE_BRACE;
               b._start = icn;
               b._end = icn; // to be changed
               ic = b.parseStruct(sfile,icn+1,recurse+1,m);
            }

        } // end if ({
        else
        if(typefound == 3)
        //if(j != string::npos && j<i1 && j<i2 && j<i)
        {   // either ( ot {
            if(sfile[icn]==')' ) {
                if(_type == TYPE_PAR)
                {
                   _end = icn;
                } else {
                    printf("parenthesis mixup\n");
                }
                return icn+1;
               // _parts.push_back({TYPE_COMMENT,sfile.substr(i+1}
              //  startpar = i;
            }
            if(sfile[icn]=='}' ) {
                if(_type == TYPE_BRACE)
                {
                   _end = icn;
                } else {
                    printf("brace mixup\n");
                }
                return icn+1;
            }
        } else
        if(typefound > 3)
        {
            printf("found drawgfx\n");
        }
        else {
            printf("end file\n");
           _parts.push_back({TYPE_BULK,ic,sfile.length()});
           icn = sfile.length();
        }
        ic = icn;
    }

    return ic;
}


int  changeapi(std::string ofilepath,std::string nfilepath)
{
    ifstream ifs(ofilepath,ios::binary|ios::in);
    if(!ifs.good()) return 1;

    stringstream buffer;
    buffer << ifs.rdbuf();

    string sfile = buffer.str();

    CFileModifier mod;
    mod._func = "drawgfx";

    CFileParse fp;
    fp._type = TYPE_MAIN;
    fp.parseStruct(sfile,0,0,mod);


    return 0;
}

int main(int argc, char **argv)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir((sourcebase+"vidhrdw").c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        string sname =   ent->d_name;
        if(sname.rfind(".c") != sname.length()-2) continue;
        string ofilepath = sourcebase+"vidhrdw/" + sname;
        string nfilepath = sourcebase+"vidhrdw2/" + sname;
        //printf ("%s\n", ent->d_name);
        changeapi(ofilepath,nfilepath);
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
      return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
