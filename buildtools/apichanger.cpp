#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <memory>


//#if (__cplusplus > 201402L)
#include <filesystem>
namespace fs = std::filesystem;
//#else
//#include <dirent.h>
//#endif




//namespace fs = std::filesystem;

using namespace std;

string sourcebase("../mame106/");


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
enum class ParType : int {
    eNone=0,
    eFor,
    eWhile,
    eDo,
    eIf,
    eSwitch,
    eFoundCall
};
// - - - - - - - - - -
struct CFileModifier {
    std::string _func; // drawgfx ()
    std::string _replacement;
    mutable int64_t _nbfuncfound;
};


struct fstr {
    std::vector<char> _b;

    string substr(size_t i, size_t l) const {
        string s;
        for(size_t j=0;j<l; j++)
        {
            s += _b[i+j];
        }
        return s;
    }


    size_t find_first_of(const char *ps,size_t i=0, size_t imax=string::npos) const {
        size_t si = i;
        while(si<_b.size() && si<imax)
        {
            char c = _b[si];
            size_t j=0;
            while(ps[j] != 0){
                if(c==ps[j]) return si;
                j++;
            }
            si++;
        }
        return string::npos;
    }      
    size_t find_first_of(const std::string &str,size_t i=0,size_t imax=string::npos) const {
        return find_first_of(str.c_str(),i,imax);
    }
    size_t rfind_first_of(const char *ps,size_t i=0) const {
        size_t si = i;
        while(si>0)
        {
            char c = _b[si];
            size_t j=0;
            while(ps[j] != 0){
                if(c==ps[j]) return si;
                j++;
            }
            si--;
        }
        return string::npos;
    }
    size_t rfind_first_of(const std::string &str,size_t i=0) const {
        return rfind_first_of(str.c_str(),i);
    }
    size_t find(const char *ps,size_t i=0, size_t imax=string::npos) const {
        size_t si = i;
        while(si<_b.size() && si<imax)
        {
            bool isok=true;
            size_t j=0;
            while(ps[j] != 0 && si+j<_b.size()){
                if(_b[si+j]!=ps[j]) {
                    isok=false;
                    break;
                }
                j++;
            }
            if(isok) return si;
            si++;
        }
        return string::npos;
    }
    size_t find(const std::string &str,size_t i=0, size_t imax=string::npos) const {
        return find(str.c_str(),i,imax);
    }
    size_t length() const {
     return _b.size()-1;
    }
    char operator[](size_t i) const {
        return _b[i];
    }
    // special: search if this parentesis belong to a loop or a call.
    ParType rfindTagBeforePar(std::string &s,size_t &wordStart,const CFileModifier *&modifierSelcted, size_t ipar,const vector<CFileModifier> &extralist) const {
        modifierSelcted = nullptr;
        if(ipar<2) return  ParType::eNone;
        ipar--;
        // trim and escape case loop
        while(ipar>4)
        {
            char c = _b[ipar];
            if(c ==' ' || c=='\t'|| c=='\n' || c=='\r')
            {
                ipar --;
                continue;
            }
            if(c =='(' || c==')'|| c=='{' || c=='}')
            {   // means not a loop parenthesis
                return ParType::eNone;
            }
            break;
        }
        size_t ilast=ipar;
        ilast++;
        ipar--;
        // should start an expression
        while(ipar>4)
        {
            char c = _b[ipar];
            if(c ==' ' || c=='\t'|| c=='\n' || c=='\r')
            {
                ipar++;
                break;
            }
           ipar--;
        }
        s.clear();
        wordStart = ipar;
        while(ipar!= ilast) {
            s += _b[ipar];
            ipar++;
        }
        if(s=="for") {
            return ParType::eFor;
        }
        if(s=="do") {
            return ParType::eDo;
        }
        if(s=="while") {
            return ParType::eWhile;
        }
        if(s=="if") {
            return ParType::eIf;
        }
        if(s=="switch") {
            return ParType::eSwitch;
        }
        for(const CFileModifier &md : extralist)
        {
            if(s==md._func)
            {
                modifierSelcted = &md;
                return ParType::eFoundCall;
            }
        }
        return ParType::eNone;
    }
};



//struct cPart {
//    int _type;
//    std::string _content;
//    vector<cPart> _parts;
//};





size_t findNextOfInterest(int &type,const fstr &s, size_t istart,const vector<CFileModifier> &extralist)
{
    struct CMultiSearchPart {
       std::string _s;
       bool     _anyof;

    };

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
            ti = s.find_first_of(p._s,istart,ires);
        } else
        {
            ti = s.find(p._s,istart,ires);
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
    for(const CFileModifier &md : extralist)
    {
        size_t ti;
        ti = s.find(md._func,istart,ires);

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
    type = ityperes;
    return ires;

}

#define TYPE_MAIN 0
#define TYPE_BULK 1
#define TYPE_BRACE 2
#define TYPE_PAR 3
#define TYPE_COMMENT 4
#define TYPE_REPLACEABLECALL 5


struct CFileParse {
    CFileParse() : _type(TYPE_MAIN)
    ,_start(0),_end(0),_parType(ParType::eNone)
    , _pParent(nullptr),_pmodsel(nullptr)
    {}
    CFileParse(int _type, size_t istart, size_t iend,CFileParse *parent)
     : _type(TYPE_MAIN)
    ,_start(istart),_end(iend),_parType(ParType::eNone)
    , _pParent(parent),_pmodsel(nullptr)
    {}
    size_t parseStruct(const fstr &sfile,size_t ic,int recurse,const vector<CFileModifier> &m);

    void modify(const fstr &sfile);
    void apply(ostream &ofs,const fstr &sfile);

    void parse_correct_do_bound(const fstr &sfile);
    void parse_correct_while_bound(const fstr &sfile);
    void parse_correct_for_bound(const fstr &sfile);

    int _type;
    //std::string _content;
    size_t _start;
    size_t _end;
    //  if _type == TYPE_PAR, we want to tag "for","while"
    // if type ==brace , we want to tag "do"
    ParType _parType;
    CFileParse *_pParent;
    //recurse
    vector<CFileParse> _parts;
    string _indent;
    string _prepatch,_postpatch;

    const CFileModifier *_pmodsel;
};

void CFileParse::parse_correct_do_bound(const fstr &sfile)
{

}
void CFileParse::parse_correct_while_bound(const fstr &sfile)
{

}
void CFileParse::parse_correct_for_bound(const fstr &sfile)
{

}

size_t CFileParse::parseStruct(const fstr &sfile,size_t ic,int recurse,const vector<CFileModifier> &m)
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
        size_t icn = findNextOfInterest(typefound,sfile,ic,{});

// i1 != string::npos && i1<i2 && i1<i && i1<j
        if(typefound == 0)
        {   // comment1 on the way
            _parts.push_back({TYPE_BULK,ic,icn,this});
            size_t iend = sfile.find("*/",icn+2);
            if(iend != string::npos) { _parts.push_back({TYPE_COMMENT,icn,iend+2,this});
                    icn = iend+2; } else icn = sfile.length();
        } else
        if(typefound == 1)
       // if(i2 != string::npos && i2<i1 && i2<i && i2<j)
        {   // comment2 on the way
            _parts.push_back({TYPE_BULK,ic,icn,this});
            size_t iend = sfile.find("\n",icn+2);
            if(iend != string::npos) { _parts.push_back({TYPE_COMMENT,icn,iend+1,this});
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
                b._end = icn; // will be corrected in parseStruct() recusion.
                b._pParent = this;
                string par_prepend;
                b._parType = sfile.rfindTagBeforePar(par_prepend,b._start,b._pmodsel,icn,m);
                icn = b.parseStruct(sfile,icn+1,recurse+1,m);
                // got to have englobing bounds for loop
               if( b._parType == ParType::eWhile ||
                    b._parType == ParType::eFor
                    )
               {
                    // keep indent before word
                    size_t linestart = sfile.rfind_first_of("\r\n",b._start-1);
                    if(linestart != string::npos) b._indent =sfile.substr(linestart+1,b._start-(linestart+1));

                    // got to extend this to next brace.
                    size_t nextbrace = sfile.find("{",b._end);
                    if(nextbrace != string::npos)
                    {
                        b._type = TYPE_BRACE;
                        //  b._parType will keep "for" and "while" spec.
                        // this will move b._end accordingly matching comments and recursion:
                        icn = b.parseStruct(sfile,nextbrace+1,recurse+1,m);

                    }
               }

                if(b._parType == ParType::eFoundCall)
                {
                    // treated on modify()
                }
            }
            if(sfile[icn]=='{' ) {
                _parts.push_back(CFileParse());
               CFileParse &b=_parts.back();
               b._type = TYPE_BRACE;
               b._start = icn;
               b._end = icn; // will be corrected in parseStruct() recusion.
               b._pParent = this;
               string par_prepend;
               b._parType = sfile.rfindTagBeforePar(par_prepend,b._start,b._pmodsel,icn,m);
               icn = b.parseStruct(sfile,icn+1,recurse+1,m);
               if( b._parType == ParType::eDo)
               {
                    // keep indent before "do"
                    size_t linestart = sfile.rfind_first_of("\r\n",b._start-1);
                    if(linestart != string::npos) b._indent =sfile.substr(linestart+1,b._start-(linestart+1));

                    // got to declare end after }while(...);
                    size_t doiend = sfile.find(";",icn);
                    if(doiend != string::npos) b._end = doiend+1;
                    icn = b._end+1;
               }
            }

        } // end if ({
        else
        if(typefound == 3)
        //if(j != string::npos && j<i1 && j<i2 && j<i)
        {   // either ( ot {
            if(sfile[icn]==')' ) {
                if(_type == TYPE_PAR)
                {
                   _end = icn+1;
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
                   _end = icn+1;
                } else {
                    printf("brace mixup\n");
                }
                return icn+1;
            }
        } else
        if(typefound > 3)
        {
            //printf("found drawgfx\n");
            m[typefound-4]._nbfuncfound++;

            icn++; // no, parse func call then jump all call.
        }
        else {
            printf("end file\n");
           _parts.push_back({TYPE_BULK,ic,sfile.length(),this});
           icn = sfile.length();
        }
        ic = icn;
    }

    return ic;
}

void CFileParse::modify(const fstr &sfile)
{
    for(CFileParse &p : _parts)
    {
        p._pParent = this;
        p.modify(sfile);
    }
    if(_type == TYPE_PAR &&
        _parType == ParType::eFoundCall)
    {
        // search immediate external loop
        CFileParse *pParent = _pParent;
        CFileParse *pPrevParent = pParent;
        while(pParent)
        {
            if( pParent->_parType == ParType::eIf ||
                pParent->_parType == ParType::eSwitch
                )
                {
                    pPrevParent = pParent;
                    pParent = pParent->_pParent;
                    continue;
                }
            if(pParent->_parType == ParType::eWhile ||
                pParent->_parType == ParType::eDo ||
                pParent->_parType == ParType::eFor
                )
            {
                    pPrevParent = pParent;
                    pParent = pParent->_pParent;
                    continue;
            }
            break;
        }
        // actually patch pPrevParent
        pPrevParent->_prepatch = "\n";
        pPrevParent->_prepatch += pPrevParent->_indent +"{ struct drawgfxParams dgp={\n";
        //....
         pPrevParent->_prepatch += pPrevParent->_indent+ "  };\n";
    pPrevParent->_prepatch += pPrevParent->_indent;

        pPrevParent->_postpatch += string("\n")+ pPrevParent->_indent+ "} // end of patch paragraph\n";

    } // end if this is patchable call.

}

// recreate source with modifiers.
void CFileParse::apply(ostream &ofs,const fstr &sfile)
{
    if(_prepatch.length()>0) ofs << _prepatch;

    size_t i=_start;
    if(_parts.size()==0)
    {
        for( ; i<_end ; i++)
        {
            if(sfile._b[i]==0) return;
            ofs<< sfile._b[i];
        }
        return;
    }
    for(CFileParse &p : _parts)
    {
        while(i<p._start) {
            ofs << sfile._b[i];
            i++;
        }
        if(p._parType == ParType::eFoundCall)
        {
            if( p._pmodsel)
            {
                ofs << p._pmodsel->_replacement;
            }
            //
            //ofs << "BLUBLU";
        } else{
            p.apply(ofs,sfile);
        }
        i = p._end;
    }
    while(i<_end) {
        if(sfile._b[i]==0) return;
       ofs << sfile._b[i];
       i++;
    }
    if(_postpatch.length()>0) ofs << _postpatch;
}




int  changeapi(std::string ofilepath,std::string nfilepath)
{
    fstr bfile;
    { //
        ifstream ifs(ofilepath);
        if(!ifs.good()) return 1;
        ifs.seekg(0,ios::end);
        size_t sz = (size_t)ifs.tellg();
        ifs.seekg(0,ios::beg);
        ifs.clear();

        bfile._b.reserve(sz+1);
        bfile._b.resize(sz+1);
        ifs.read(bfile._b.data(),sz);
        bfile._b[sz] = 0;
    }

    //stringstream buffer;
    //buffer << ifs.rdbuf();
    //string sfile = buffer.str();

    vector<CFileModifier> modifiers=
    {
  //       {"mdrawgfx","drawgfx(&dgp)",0},
  //      {"pdrawgfx","drawgfx(&dgp)",0},
        {"drawgfx","drawgfx(&dgp)",0},
    };

    CFileParse fp;
    fp._type = TYPE_MAIN;
    fp._start = 0;
    fp._end = bfile._b.size()-1;
    fp.parseStruct(bfile,0,0,modifiers);

    ofstream ofs(nfilepath);
    fp.modify(bfile);
    fp.apply(ofs,bfile);

    return 0;
}

int main(int argc, char **argv)
{

//#if (__cplusplus > 201402L)
    string sdir =sourcebase+"vidhrdw";
    /*ok
    for (const auto & entry : fs::directory_iterator(sdir))
    {
        string sname =  entry.path().filename().string();
        if(sname.rfind(".c") != sname.length()-2) continue;
        string ofilepath = sourcebase+"vidhrdw/" + sname;
        string nfilepath = sourcebase+"vidhrdw2/" + sname;
        changeapi(ofilepath,nfilepath);
        break; // test
    }*/

    string ofilepath = sourcebase+"vidhrdw/" + "1111.c";
    string nfilepath = sourcebase+"vidhrdw/" + "1111n.c";
    changeapi(ofilepath,nfilepath);

//#else
//    DIR *dir;
//    struct dirent *ent;
//    if ((dir = opendir((sourcebase+"vidhrdw").c_str())) != NULL) {
//      /* print all the files and directories within directory */
//      while ((ent = readdir (dir)) != NULL) {
//        string sname =   ent->d_name;
//        if(sname.rfind(".c") != sname.length()-2) continue;
//        string ofilepath = sourcebase+"vidhrdw/" + sname;
//        string nfilepath = sourcebase+"vidhrdw2/" + sname;
//        //printf ("%s\n", ent->d_name);
//        changeapi(ofilepath,nfilepath);
//      }
//      closedir (dir);
//    } else {
//      /* could not open directory */
//      perror ("");
//      return EXIT_FAILURE;
//    }
//#endif

    return EXIT_SUCCESS;
}
