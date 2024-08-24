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
struct ModifiedFuncParam {
    int   _callindex;
    string _name;
    int    _factorize;
    int    _extra;
    string _extraDefaultValue;
};

struct CFileModifier {
    std::string _func; // drawgfx ()
    std::string _structtype;
    std::string _structbasename;
    std::string _replacement;

    vector<ModifiedFuncParam> _params;

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
    void getPosition(size_t &line,size_t &col,size_t i) const
     {
        line = 0;
        col=0;
        while( i>0 && _b[i] != '\n' )
        {
            col++;
            i--;
        }
        while( i>0 )
        {
            if( _b[i] == '\n' ) line++;
            i--;
        }
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
    // this is sourceof bug if comment
    ParType rfindTagBeforePar(std::string &s,size_t &wordStart,const CFileModifier *&modifierSelcted, size_t ipar,const vector<CFileModifier> &extralist) const {
        modifierSelcted = nullptr;
        if(ipar<2) return  ParType::eNone;
        ipar--;
        // trim and escape case loop
        while(ipar>4)
        {
            char c = _b[ipar];
            if( c=='\n' || c=='\r')
            {
                // special case if // comment
                ipar--;
                if(ipar>0 && _b[ipar]=='\r' ) ipar--;
                bool hascpcomment=false;
                size_t ipp = ipar-1;
                while(ipp>0 && _b[ipp] != '\n')
                {
                    if(_b[ipp]=='/' && _b[ipp+1]=='/') return  ParType::eNone;
                    ipp--;
                }
                continue;
            }

            if(c ==' ' || c=='\t'|| c=='\n' || c=='\r')
            {
                ipar --;
                continue;
            }
            if(c =='(' || c==')'|| c=='{' || c=='}' || c=='/' || c==','|| c==';'
                        || c=='=') // '/' because of '*/'
            {   // means not a loop parenthesis
                return ParType::eNone;
            }
            break;
        }
        size_t ilast=ipar;
        ilast++;
        ipar--;
        // should start an expression... or not.
        while(ipar>0)
        {
            char c = _b[ipar];
            if(c ==' ' || c=='\t'|| c=='\n' || c=='\r'
                || c=='/'|| c=='=' || c==')'|| c=='('
                || c=='}'|| c=='{'|| c=='='|| c==','
                || c==';'
            )
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





size_t findNextOfInterest(int &type,const fstr &s, size_t istart)
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
        {"\"",false},
    };

 //   size_t ires=string::npos;
    size_t bestindex=string::npos;
    int besttype=-1;

    int icurrenttestedtype=0;
    for(const CMultiSearchPart &p : l)
    {
        size_t ti;
        if(p._anyof)
        {
            ti = s.find_first_of(p._s,istart,bestindex);
        } else
        {
            ti = s.find(p._s,istart,bestindex);
        }
        if(ti != string::npos) {
            if(bestindex == string::npos) {
                bestindex = ti;
                besttype = icurrenttestedtype;
            }else
            {
                if(ti<bestindex)
                {
                    bestindex = ti;
                    besttype = icurrenttestedtype;
                }
            }
        }
        icurrenttestedtype++;
    }

    type = besttype;
    return bestindex;

}

// describe a struct actually prepended to a paragraph
// this is used to look if we can re-use the samefor multiple calls.
struct AppliedPatch {
    const CFileModifier *_pmodifier; // because of this one.
    string _name; // applied unique struct name.
    // mirror struct member list _pmodifier->_params
    // we can only reuse same struct if factorized the same way.
    vector<int> _paramFactorized;
    // we must keep caller values for factorized params
    vector<string> _paramCallerValue;

    bool operator==( const AppliedPatch&l) const {
        if(_pmodifier->_structtype != l._pmodifier->_structtype) return false;
        if(_paramFactorized.size() != l._paramFactorized.size()) return false;
        for(size_t i=0;i<_paramFactorized.size();i++)
        {
            if(_paramFactorized[i] != l._paramFactorized[i]) return false;
            if(_paramFactorized[i])
            {
                if(_paramCallerValue[i] != l._paramCallerValue[i]) return false;
            }
        }
        return true;
    }
};

enum class BaseType : int {
    eMain=0,
    eBulk,
    eBrace,
    ePar,
    eComment,
    eQuote,
    eReplaceableCall
};

struct CFileParse {
    CFileParse() : _type(BaseType::eMain)
    ,_start(0),_end(0),_parType(ParType::eNone)
    , _pParent(nullptr),_pmodsel(nullptr)
    {}
    CFileParse(BaseType type, size_t istart, size_t iend,CFileParse *parent)
     : _type(type)
    ,_start(istart),_end(iend),_parType(ParType::eNone)
    , _pParent(parent),_pmodsel(nullptr)
    {}
    size_t parseStruct(const fstr &sfile,size_t ic,int recurse,const vector<CFileModifier> &m);

    void modify(const fstr &sfile);
    void apply(ostream &ofs,const fstr &sfile,size_t imin);


    BaseType _type;
    //std::string _content;
    size_t _start;
    size_t _end;
    //  if _type == BaseType::ePar, we want to tag "for","while"
    // if type ==brace , we want to tag "do"
    ParType _parType;
    CFileParse *_pParent;
    //recurse
    vector<CFileParse> _parts;
    string _indent;

   // used to modify:
    vector<AppliedPatch> _appliedPatches;
    string _prepatch,_postpatch,_replacement;

    void generatePatches();

    // if foundcall, parse params
    vector<string> _callParams;
    vector<int> _callParamsConstant;
    void testIfParamsConstant();

    // applied modifier if any
    const CFileModifier *_pmodsel;


};

/*
pPrevParent->_prepatch,  pPrevParent->_postpatch,
                pPrevParent->_indent,
               pPrevParent->_appliedPatches
*/
void CFileParse::generatePatches()
{
    _prepatch.clear();
    _postpatch.clear();
    if(_appliedPatches.size()==0) return;

   // prepend a paragraph
    stringstream ssp;
    ssp <<  "\n" << _indent << "{ \n";

    // add as many structs
    for(AppliedPatch &ptch : _appliedPatches)
    {
        const CFileModifier *pmd = ptch._pmodifier;
        ssp << _indent << pmd->_structtype << " " << ptch._name << "={\n";
        for(size_t i=0;i<pmd->_params.size() ;i++)
        {
            const ModifiedFuncParam &prm = pmd->_params[i];
            int icallindex = prm._callindex;
            ssp << _indent << "\t";
            if( prm._factorize && ptch._paramCallerValue[i].length()>0 ) {
                ssp << ptch._paramCallerValue[i];
             } else {
                 ssp << prm._extraDefaultValue;
             }
             if(i<pmd->_params.size()-1) ssp << ",";
             // add name of members in comment will ease things
             ssp << " \t// " << prm._name;

             ssp << "\n";
        }
        ssp << _indent << "  };\n";
    } // end by struct

    //....
    ssp << _indent ;

    _prepatch = ssp.str();

    // - - -
    stringstream sspo;
    sspo << "\n" <<_indent<<"} // end of patch paragraph\n";
    _postpatch = sspo.str();

}


int isOnlyNumbers(const string &s)
{
    for(char c : s)
    {
        if(c<'0' || c >'9') return 0;
    }
    return 1;
}

void CFileParse::testIfParamsConstant()
{
    _callParamsConstant.resize(_callParams.size());
    for(size_t i=0;i<_callParams.size();i++)
    {
        // easy way
        _callParamsConstant[i] = isOnlyNumbers(_callParams[i]);
    }

}

size_t CFileParse::parseStruct(const fstr &sfile,size_t ic,int recurse,const vector<CFileModifier> &m)
{
    while(ic<sfile.length())
    {
        int typefound; //0 comment 1 comment, 2 par/brace open  3 parbrace close.
        size_t icn = findNextOfInterest(typefound,sfile,ic);

        if(typefound == 0) // comment
        {   // comment1 on the way
           // if(icn>ic) _parts.push_back({TYPE_BULK,ic,icn,this});
            size_t iend = sfile.find("*/",icn+2);
            if(iend != string::npos) {
                _parts.push_back({BaseType::eComment,icn,iend+2,this});
                    icn = iend+2;
            } else icn = sfile.length();
        } else
        if(typefound == 1) // comment
        {   // comment2 on the way
           // if(icn>ic) _parts.push_back({TYPE_BULK,ic,icn,this});
            size_t iend = sfile.find("\n",icn+2);
            if(iend != string::npos) {
                _parts.push_back({BaseType::eComment,icn,iend+1,this});
                    icn = iend+1;
            } else icn = sfile.length();
        } else
        if(typefound == 2) // par/brace open
        {   // either ( ot {
            if(sfile[icn]=='(' ) {
                 _parts.push_back(CFileParse());
                CFileParse &b=_parts.back();
                b._type = BaseType::ePar;
                size_t startpar = icn;
                b._start = icn;
                b._end = icn; // will be corrected in parseStruct() recusion.
                b._pParent = this;
                string par_prepend;
                b._parType = sfile.rfindTagBeforePar(par_prepend,b._start,b._pmodsel,icn,m);
                icn = b.parseStruct(sfile,icn+1,recurse+1,m);
                size_t endpar = icn;
                // got to have englobing bounds for loop
               if( b._parType == ParType::eWhile ||
                    b._parType == ParType::eFor
                    )
               {
                    // keep indent before word
                    size_t linestart = sfile.rfind_first_of("\r\n",b._start-1);
                    if(linestart != string::npos) b._indent =sfile.substr(linestart+1,b._start-(linestart+1));

                    // got to extend this to next brace.... if this loop use brace.
                    // important to not treat "for() i++;" as recursive
                    // this was very wrong:
                   // size_t nextbrace = sfile.find("{",b._end);
                    //if(nextbrace != string::npos)

                     size_t inextbrace = b._end;
                     char c = sfile[inextbrace];
                     while(c == ' ' ||
                           c == '\t' ||
                           c == '\r' ||
                          c == '\n'
                           )
                     {  inextbrace++;
                         c =  sfile[inextbrace];
                     }

                    if(sfile[inextbrace] == '{') {
                        b._type = BaseType::eBrace;
                        //  b._parType will keep "for" and "while" spec.
                        // this will move b._end accordingly matching comments and recursion:
                        icn = b.parseStruct(sfile,inextbrace+1,recurse+1,m);
                    }


               }

                if(b._parType == ParType::eFoundCall)
                {
                    size_t linestart = sfile.rfind_first_of("\r\n",b._start-1);
                    if(linestart != string::npos) b._indent =sfile.substr(linestart+1,b._start-(linestart+1));

                    // treated on modify()
                    string params = sfile.substr(startpar+1,(endpar-startpar)-2);
                    b._callParams = splitt_trim(params, ",");
                    b.testIfParamsConstant();
                }
            }
            if(sfile[icn]=='{' ) {
                _parts.push_back(CFileParse());
               CFileParse &b=_parts.back();
               b._type = BaseType::eBrace;
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
        if(typefound == 3) //  par/brace close
        {   // either ( ot {
            if(sfile[icn]==')' ) {
                if(_type == BaseType::ePar)
                {
                   _end = icn+1;

                } else {
                    size_t line,col, line_s,col_s;
                    sfile.getPosition(line,col,icn);
                    sfile.getPosition(line_s,col_s,_start);
                    cout << "parenthesis end mixup: l:"<<line <<" c:"<<col << endl;
                     cout << "start l:"<<line_s <<" c:"<<col_s << endl;
                    if(_pParent)
                    {
                        size_t linep1,colp1; size_t linep2,colp2;
                        sfile.getPosition(linep1,colp1,_pParent->_start);
                        sfile.getPosition(linep2,colp2,_pParent->_end);
                        cout << "parent: start l:"<<linep1 << " c:"<<colp1 << endl;
                        cout << "parent: end l:"<<linep2 << " c:"<<colp2 << endl;


                    }                                        
                }

                return icn+1;
               // _parts.push_back({BaseType::eComment,sfile.substr(i+1}
              //  startpar = i;
            }
            if(sfile[icn]=='}' ) {
                if(_type == BaseType::eBrace)
                {
                   _end = icn+1;
                } else {
                    size_t line,col, line_s,col_s;
                    sfile.getPosition(line,col,icn);
                    sfile.getPosition(line_s,col_s,_start);
                    cout << "brace end mixup: l:"<<line <<" c:"<<col << endl;
                     cout << "start l:"<<line_s <<" c:"<<col_s << endl;
                    if(_pParent)
                    {
                        size_t linep1,colp1; size_t linep2,colp2;
                        sfile.getPosition(linep1,colp1,_pParent->_start);
                        sfile.getPosition(linep2,colp2,_pParent->_end);
                        cout << "parent: start l:"<<linep1 << " c:"<<colp1 << endl;
                        cout << "parent: end l:"<<linep2 << " c:"<<colp2 << endl;


                    }
                }
                return icn+1;
            }
        } else if(typefound == 4) // quotes
        {
            //
         //no need  if(icn>ic) _parts.push_back({TYPE_BULK,ic,icn,this}); // put what was before
            size_t iend = sfile.find("\"",icn+1); // find end of quote
            if(iend != string::npos) { _parts.push_back({BaseType::eQuote,icn,iend+1,this});
                    icn = iend+1; } else icn = sfile.length();
        } else
        if(typefound > 4)
        {
            //printf("found drawgfx\n");
          //  m[typefound-4]._nbfuncfound++;

            icn++; // no, parse func call then jump all call.
        }
        else {
           // printf("end file\n");
           _parts.push_back({BaseType::eBulk,ic,sfile.length(),this});
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
    if(_type == BaseType::ePar &&
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
        AppliedPatch structpatch;

        // - - - -describe what the patch should look like
        structpatch._pmodifier = _pmodsel;
        stringstream sspname;
        sspname << _pmodsel->_structbasename <<  _pmodsel->_nbfuncfound ;
        structpatch._name = sspname.str();
        structpatch._paramFactorized.resize(_pmodsel->_params.size());
        structpatch._paramCallerValue.resize(_pmodsel->_params.size());
        for(size_t i=0;i<_pmodsel->_params.size() ;i++)
        {
            const ModifiedFuncParam &prm = _pmodsel->_params[i];
            int icallindex = prm._callindex;
            if( prm._factorize ==0 && (icallindex !=-1 &&_callParamsConstant[icallindex]==0) ) {

//                ssr << _indent << _pmodsel->_structbasename << _pmodsel->_nbfuncfound
//                 << "."<< prm._name << " = " << _callParams[icallindex] << ";\n";
                structpatch._paramFactorized[i]=0;
            } else structpatch._paramFactorized[i]=1;

            if( prm._factorize ==1 && (icallindex !=-1 && icallindex<_callParams.size()) ) {
                structpatch._paramCallerValue[i] = _callParams[icallindex];
            }
        }
        // - -- - look if there is already something that match
        AppliedPatch *pActuallyUsed=&structpatch;

        for(AppliedPatch &ptch : pPrevParent->_appliedPatches)
        {
            if(ptch == structpatch) {
                pActuallyUsed = &ptch;

                break;
            }
        }
        if(pActuallyUsed == &structpatch)
        {   // if wasn't replace, add to list
            pPrevParent->_appliedPatches.push_back(structpatch);
        }

        // note: will be regenerated with more structs or not if change,
        // before being applied in apply().
        pPrevParent->generatePatches();

        // - - - -


         stringstream ssr;
         ssr << "\n";
         // do the update for dynamic members
         for(size_t i=0;i<_pmodsel->_params.size() ;i++)
         {
              const ModifiedFuncParam &prm = _pmodsel->_params[i];
                int icallindex = prm._callindex;
              if( prm._factorize ==0 && (icallindex !=-1 &&_callParamsConstant[icallindex]==0) ) {
                 ssr << _indent << pActuallyUsed->_name
                     << "."<< prm._name << " = " << _callParams[icallindex] << ";\n";
              }
         }
         ssr << _indent << _pmodsel->_replacement << "(&" << pActuallyUsed->_name << ")";
        _replacement = ssr.str();

        _pmodsel->_nbfuncfound++;
    } // end if this is patchable call.

}

// recreate source with modifiers.
void CFileParse::apply(ostream &ofs,const fstr &sfile,size_t imin)
{
    if(_prepatch.length()>0) ofs << _prepatch;

    // dbg
    if(_pParent)
    {
        if(_start<_pParent->_start )
        {
            cout << "woot?" << endl;
        }
        if(_end>_pParent->_end )
        {
            cout << "rewoot?" << endl;
        }
    }


    size_t i=_start;
    if(imin>i) i=imin;
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
                ofs << p._replacement ;
            }
        } else{
            p.apply(ofs,sfile,i);
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


bool  changeapi(std::string ofilepath,std::string nfilepath)
{
cout << "check: " <<ofilepath << endl;
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

    /*
    struct ModifiedFuncParam {
        string _name;
        int    _factorize;
        int    _extra;
        string _extraDefaultValue;
    };

    struct CFileModifier {
        std::string _func; // drawgfx ()
        std::string _replacement;

        vector<ModifiedFuncParam> _params;

        mutable int64_t _nbfuncfound;
    };
    */
    vector<CFileModifier> modifiers;

    /*
    void drawgfx(mame_bitmap *dest,const gfx_element *gfx,
            unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy,
            const rectangle *clip,int transparency,int transparent_color);

    void pdrawgfx(mame_bitmap *dest,const gfx_element *gfx,
            unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy,
            const rectangle *clip,int transparency,int transparent_color,

            UINT32 priority_mask);

    void mdrawgfx(mame_bitmap *dest,const gfx_element *gfx,
            unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy,
            const rectangle *clip,int transparency,int transparent_color,

            UINT32 priority_mask);
    */
    {
        CFileModifier pdrawgfxModifier;
        pdrawgfxModifier._func = "pdrawgfx";
        pdrawgfxModifier._structtype = "struct drawgfxParams";
        pdrawgfxModifier._structbasename = "dgp";
        pdrawgfxModifier._replacement = "drawgfx";
        pdrawgfxModifier._nbfuncfound = 0;
        pdrawgfxModifier._params = {
            // index in call,name in struct, forcefactorize, isextra, defaultval.
            {0,"dest",1,0,"NULL"},
            {1,"gfx",1,0,"NULL"},
            {2,"code",0,0,"0"},
            {3,"color",0,0,"0"},
            {4,"flipx",0,0,"0"},
            {5,"flipy",0,0,"0"},
            {6,"sx",0,0,"0"},
            {7,"sy",0,0,"0"},
            {8,"clip",1,0,"NULL"},
            {9,"transparency",1,0,"TRANSPARENCY_NONE"},
            {10,"transparent_color",1,0,"0"},
            // extra params used or not
            {-1,"scalex",1,1,"0"}, // for scale draw else not used
            {-1,"scaley",1,1,"0"},

            {-1,"pri_buffer",1,1,"priority_bitmap"}, // priority_bitmap,priority_mask | (1<<31)
            {11,"priority_mask",1,1,"| (1<<31)"},
        };

        modifiers.push_back(pdrawgfxModifier);
    }
    {
        CFileModifier mdrawgfxModifier;
        mdrawgfxModifier._func = "pdrawgfx";
        mdrawgfxModifier._structtype = "struct drawgfxParams";
        mdrawgfxModifier._structbasename = "dgp";
        mdrawgfxModifier._replacement = "drawgfx";
        mdrawgfxModifier._nbfuncfound = 0;
        mdrawgfxModifier._params = {
            // index in call,name in struct, forcefactorize, isextra, defaultval.
            {0,"dest",1,0,"NULL"},
            {1,"gfx",1,0,"NULL"},
            {2,"code",0,0,"0"},
            {3,"color",0,0,"0"},
            {4,"flipx",0,0,"0"},
            {5,"flipy",0,0,"0"},
            {6,"sx",0,0,"0"},
            {7,"sy",0,0,"0"},
            {8,"clip",1,0,"NULL"},
            {9,"transparency",1,0,"TRANSPARENCY_NONE"},
            {10,"transparent_color",1,0,"0"},
            // extra params used or not
            {-1,"scalex",1,1,"0"}, // for scale draw else not used
            {-1,"scaley",1,1,"0"},

            {-1,"pri_buffer",1,1,"priority_bitmap"}, // priority_bitmap,priority_mask | (1<<31)
            {11,"priority_mask",1,1,""},
        };

        modifiers.push_back(mdrawgfxModifier);
    }
    {
        CFileModifier drawgfxModifier;
        drawgfxModifier._func = "drawgfx";
        drawgfxModifier._structtype = "struct drawgfxParams";
        drawgfxModifier._structbasename = "dgp";
        drawgfxModifier._replacement = "drawgfx";
        drawgfxModifier._nbfuncfound = 0;
        drawgfxModifier._params = {
            // index in call,name in struct, forcefactorize, isextra, defaultval.
            {0,"dest",1,0,"NULL"},
            {1,"gfx",1,0,"NULL"},
            {2,"code",0,0,"0"},
            {3,"color",0,0,"0"},
            {4,"flipx",0,0,"0"},
            {5,"flipy",0,0,"0"},
            {6,"sx",0,0,"0"},
            {7,"sy",0,0,"0"},
            {8,"clip",1,0,"NULL"},
            {9,"transparency",1,0,"TRANSPARENCY_NONE"},
            {10,"transparent_color",1,0,"0"},
            // extra params used or not
            {-1,"scalex",1,1,"0"}, // for scale draw else not used
            {-1,"scaley",1,1,"0"},

            {-1,"pri_buffer",1,1,"NULL"},
            {-1,"priority_mask",1,1,"0"},
        };

        modifiers.push_back(drawgfxModifier);
    }
    CFileParse fp;
    fp._type = BaseType::eMain;
    fp._start = 0;
    fp._end = bfile._b.size()-1;
    fp.parseStruct(bfile,0,0,modifiers);

    fp.modify(bfile);
    bool didchange=false;
    for(const CFileModifier &m : modifiers)
    {
        if(m._nbfuncfound>0) didchange=true;
    }
    if(didchange)
    {
        cout << "modify: " << nfilepath ;
        for(const CFileModifier &m : modifiers)
        {
            if(m._nbfuncfound>0)
            {
              cout << "   "<<m._func<<": "<<m._nbfuncfound;
            }
        }
        cout << endl;
    }
    if(didchange)
    {
        ofstream ofs(nfilepath);
        fp.apply(ofs,bfile,0);
    }
    return didchange;
}

int main(int argc, char **argv)
{

//#if (__cplusplus > 201402L)
    string sdir =sourcebase+"drivers2";
   // stringstream ssgit;
   ofstream gitofs("gitcommands.sh");

//re
    for (const auto & entry : fs::directory_iterator(sdir))
    {
        string sname =  entry.path().filename().string();
        if(sname == "battlane.c" ||
sname == "bogeyman.c" ||
sname == "ddragon.c" ||
sname == "ddragon3.c" ||
sname == "dogfgt.c" ||
//sname == "generic.c" ||
sname == "matmania.c"
        ) continue;
//        "battlane.c","bogeyman.c","ddragon.c","ddragon3.c","dogfgt.c","generic.c","matmania.c",
        if(sname.rfind(".c") != sname.length()-2) continue;
        string ofilepath = sourcebase+"drivers2/" + sname;
        string nfilepath = sourcebase+"drivers/" + sname;
        bool didchange = changeapi(ofilepath,nfilepath);
        if(didchange) {
            gitofs << "git add drivers/"<<sname<<"\n";
        }

    }

//    string ofilepath = sourcebase+"vidhrdw2/" + "40love.c";
//    string nfilepath = sourcebase+"vidhrdw/" + "40love.c";

//    string ofilepath = sourcebase+"vidhrdw/" + "test.c";
//    string nfilepath = sourcebase+"vidhrdw/" + "testn.c";

//    changeapi(ofilepath,nfilepath);

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
