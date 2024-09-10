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


struct fstr {
    string _filename;
    vector<char> _b;

    string substr(size_t i, size_t l) const {
        string s;
        for(size_t j=0;j<l; j++)
        {
            s += _b[i+j];
        }
        return s;
    }
    void substros(ostream &os, size_t i, size_t l) const {
        for(size_t j=0;j<l && i+j<_b.size(); j++)
        {

            os << (char) _b[i+j];
        }
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
    size_t find_first_not_of(const char *ps,size_t i=0) const {
        size_t si = i;
        while(si<_b.size())
        {
            char c = _b[si];
            size_t j=0;
             bool ispartoflist = false;
            while(ps[j] != 0){
                if(c==ps[j]){ ispartoflist = true;
                    break; }
                j++;
            }
            if(!ispartoflist) return si;
            si++;
        }
        return string::npos;
    }
    size_t find_first_not_of(const std::string &str,size_t i=0) const {
        return find_first_not_of(str.c_str(),i);
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

};



bool changeapi(std::string ofilepath,std::string nfilepath)
{
cout << "check: " <<ofilepath << endl;
    fstr bfile;
    bfile._filename = ofilepath;
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
    ofstream ofsb(nfilepath);

    size_t i=0;
    while(i<bfile.length())
    {
        size_t inxt = bfile.find("m68k_op_",i); // m68k_op_add_32_er_d
        size_t l,c;

        if(inxt ==string::npos)
        {
            inxt = bfile.length();
            bfile.substros(ofsb,i,inxt-i);
        } else {
        bfile.getPosition(l,c,inxt);
        cout << "l:" << l << " c:"<< c << endl;
            bfile.substros(ofsb,i,inxt-i);
            i = inxt;
            inxt = bfile.find("(void)",i);
            if(inxt != string::npos && (inxt-i<48))
            {
                bfile.substros(ofsb,inxt,inxt-i);
                ofsb<< "(M68KOPT_PARAMS)";
                inxt = inxt+6;
            } else
            {
                //ok, fill next
            }
        }
        i = inxt;
    }


//    vector<CFileModifier> modifiers;

//    CFileParse fp;
//    fp._type = BaseType::eMain;
//    fp._start = 0;
//    fp._end = bfile._b.size()-1;
//    fp.parseStruct(bfile,0,0,modifiers);

//    fp.findsyntax(bfile);

    return false;
//    return didchange;
}

//bool compareFunction (std::string a, std::string b) {return a<b;}
int main(int argc, char **argv)
{

    string sdir =sourcebase+"cpu/m68000";

    vector<string> entries;
   for (const auto & entry : fs::directory_iterator(sdir))
   {
        string sname =  entry.path().filename().string();
      //entries.push_back(entry.path().filename().string());
      if(sname.rfind(".c") != sname.length()-2 &&
      sname.rfind(".h") != sname.length()-2
      ) continue;

        string ofilepath = sourcebase+"cpu/m68000o/" + sname;
        string nfilepath = sourcebase+"cpu/m68000/" + sname;
        changeapi(ofilepath,nfilepath);

   }


    return EXIT_SUCCESS;
}
