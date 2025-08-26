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

string sourcebase("../../../mame106/");
string genresbase("../../../Genres");
string playersbase("../../../Players");

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
    size_t findceiq(const char s,size_t i=0, size_t imax=string::npos) const {
        size_t si = i;
         bool iq=false;
         int nbpar=0;
        while(si<_b.size() && si<imax)
        {
            char c = _b[si];
            if(c=='"') iq = !iq;
            if(c==s && !iq && nbpar==0) return si;
             if(c=='('&& !iq) nbpar++;
             if(c==')'&& !iq) nbpar--;
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

struct romcontrols {
    int _nbp;
 //   string
};

map<string,romcontrols> _controls;

int getNbPlayer(string name, string parent)
{
    auto i = _controls.find(name);
    if(i == _controls.end())
    {
        i = _controls.find(parent);
    }
    if(i == _controls.end()) return 0;
    return i->second._nbp;
}


string getAttrib(fstr &xmlfile,size_t i, const char *pattrib)
{
    size_t iend = xmlfile.find("</game>",i);
    if(iend == string::npos) return "";

    size_t iatr = xmlfile.find(pattrib,i);
    if(iatr == string::npos) return "";
    size_t ieg = xmlfile.find("=",iatr+1);
    if(ieg == string::npos) return "";
    iatr = xmlfile.find("\"",ieg+1);
    if(iatr == string::npos) return "";
    size_t iatr2 = xmlfile.find("\"",iatr+1);
    if(iatr2 == string::npos) return "";
    if(iatr2>iend) return "";
    string s = xmlfile.substr(iatr+1,iatr2-iatr-1);
    return s;

}


#define UNROLL_GENRENUM(tfunc) \
tfunc(egg_Unknown)\
tfunc(egg_Platform)\
tfunc(egg_Climbing)\
tfunc(egg_ShootEmUp)\
tfunc(egg_Shooter)\
tfunc(egg_BeatNUp)\
tfunc(egg_Fighter)\
tfunc(egg_Driving)\
tfunc(egg_Motorcycle)\
tfunc(egg_Flying)\
tfunc(egg_LightGuns)\
tfunc(egg_BallNPaddles)\
tfunc(egg_Pinballs)\
tfunc(egg_Maze)\
\
tfunc(egg_Tabletop)\
tfunc(egg_Puzzle)\
tfunc(egg_CardBattle)\
tfunc(egg_Mahjong)\
tfunc(egg_Quizz)\
tfunc(egg_ChiFouMi)\
\
tfunc(egg_Casino)\
tfunc(egg_HorseRacing)\
tfunc(egg_PoolNDart)\
\
tfunc(egg_sport_)\
tfunc(egg_sport_Baseball)\
tfunc(egg_sport_Basketball)\
tfunc(egg_sport_Volleyball)\
tfunc(egg_sport_Football)\
tfunc(egg_sport_Soccer)\
tfunc(egg_sport_Golf)\
tfunc(egg_sport_Hockey)\
tfunc(egg_sport_Rugby)\
tfunc(egg_sport_Tennis)\
tfunc(egg_sport_TrackNField)\
tfunc(egg_sport_Boxing)\
tfunc(egg_sport_Wrestling)\
\
tfunc(egg_sport_Bowling)\
tfunc(egg_sport_Skiing)\
tfunc(egg_sport_Skate)\
tfunc(egg_sport_Rythm)\
\
tfunc(egg_Fishing)\
\
tfunc(egg_Compilation)\
tfunc(egg_Miscellaneous)\
tfunc(egg_Mature)

#define tenum(a) a,
typedef enum {
    UNROLL_GENRENUM(tenum)
} eGameGenre;
#undef tenum


#define tenum(a) #a,
const char *enumCnames[]={
    UNROLL_GENRENUM(tenum)
};
#undef tenum



eGameGenre numBHyName(std::string name)
{
    if(name.find("Ball & Paddle Games") == 0) return egg_BallNPaddles;
    if(name.find("Baseball Games") == 0) return egg_sport_Baseball;
    if(name.find("Basketball Games") == 0) return egg_sport_Basketball;
    if(name.find("Beat-'Em-Up Games") == 0) return egg_BeatNUp;
    if(name.find("Bowling Games") == 0) return egg_sport_Bowling;
    if(name.find("Boxing Games") == 0) return egg_sport_Boxing;
    if(name.find("Card Battle Games") == 0) return egg_CardBattle;
    if(name.find("Casino Games") == 0) return egg_Casino;
    if(name.find("Climbing Games") == 0) return egg_Climbing;
    if(name.find("Compilation Games") == 0) return egg_Compilation;
    if(name.find("Driving Games") == 0) return egg_Driving;
    if(name.find("Fighter Games") == 0) return egg_Fighter;
    if(name.find("Fishing Games") == 0) return egg_Fishing;
    if(name.find("Flying Games") == 0) return egg_Flying;
    if(name.find("Football Games") == 0) return egg_sport_Football;
    if(name.find("Fruit Machines Games") == 0) return egg_Casino;
    if(name.find("Golf Games") == 0) return egg_sport_Golf;
    if(name.find("Gun Games") == 0) return egg_LightGuns;
    if(name.find("Hockey Games") == 0) return egg_sport_Hockey;
    if(name.find("Horse Racing Games") == 0) return egg_HorseRacing;
    if(name.find("Mahjong Games") == 0) return egg_Mahjong;
    if(name.find("Mature Games") == 0) return egg_Mature;
    if(name.find("Maze Games") == 0) return egg_Maze;
    if(name.find("Mini-Games Games") == 0) return egg_Compilation;
    if(name.find("Miscellaneous Games") == 0) return egg_Miscellaneous;
    if(name.find("Motorcycle Games") == 0) return egg_Motorcycle;
    if(name.find("Pinball Games") == 0) return egg_Pinballs;
    if(name.find("Platform Games") == 0) return egg_Platform;
    if(name.find("Pool and Dart Games") == 0) return egg_PoolNDart;
    if(name.find("Puzzle Games") == 0) return egg_Puzzle;
    if(name.find("Quiz Games") == 0) return egg_Quizz;
    if(name.find("Rhythm Games") == 0) return egg_sport_Rythm;
    if(name.find("Rugby Games") == 0) return egg_sport_Rugby;
    if(name.find("Shoot-'Em-Up Games") == 0) return egg_ShootEmUp;
    if(name.find("Shooter Games") == 0) return egg_Shooter;
    if(name.find("Skateboarding Games") == 0) return egg_sport_Skate;
    if(name.find("Skiing Games") == 0) return egg_sport_Skiing;
    if(name.find("Soccer Games") == 0) return egg_sport_Soccer;
    if(name.find("Sports Games") == 0) return egg_sport_;
    if(name.find("Tabletop Games") == 0) return egg_Tabletop;
    if(name.find("Tennis Games") == 0) return egg_sport_Tennis;
    if(name.find("Track & Field Games") == 0) return egg_sport_TrackNField;
    if(name.find("Volleyball Games") == 0) return egg_sport_Volleyball;
    if(name.find("Water Games") == 0) return egg_sport_;
    if(name.find("Wrestling Games") == 0) return egg_sport_Wrestling;

    return egg_Unknown;
}

#define GF_HOR 1
#define GF_VER 2
#define GF_3D  4

#define GF_UViolent  8
#define GF_Childish 16
#define GF_Girly 32
#define GF_Sexy 64
#define GF_Funny 128



struct GGenre {
    string name;
    int nbPlayersAlt=0;
    int nbPlayersSim=0;

    vector<int> genreEnums;
    int flags=0;
};

map<string,GGenre> ggenres;

GGenre &getGenre(string archive,string parent)
{
    map<string,GGenre>::iterator fit = ggenres.find(archive);
    if(fit != ggenres.end())
    {
        return fit->second;
    }

    fit = ggenres.find(parent);
    if(fit != ggenres.end())
    {
        return fit->second;
    }
    return ggenres[""];
}

void readgenrefile(std::string genrefilepath,int uenum)
{
    fstr xmlfile;
    { //
        ifstream ifs(genrefilepath);
        if(!ifs.good()) return;
        ifs.seekg(0,ios::end);
        size_t sz = (size_t)ifs.tellg();
        ifs.seekg(0,ios::beg);
        ifs.clear();

        xmlfile._b.reserve(sz+1);
        xmlfile._b.resize(sz+1);
        ifs.read(xmlfile._b.data(),sz);
        xmlfile._b[sz] = 0;
    }

/*
<game name="swimmer" index="" image="">
	<description>Swimmer (set 1)</description>
	<cloneof/>
	<crc/>
	<manufacturer>Tehkan</manufacturer>
	<year>1982</year>
	<genre>Water</genre>
	<rating/>
	<enabled>Yes</enabled>
</game>
*/
  string keyn = "<game";
   size_t igame = xmlfile.find(keyn);
    while(igame != string::npos)
    {
        igame+=keyn.length();
        string name = getAttrib(xmlfile,igame, "name");
        if(name.size()==0) {
             igame = xmlfile.find(keyn,igame);
             continue;
        }
        ggenres[name].genreEnums.push_back(uenum);

        igame = xmlfile.find(keyn,igame);
    }
}

void readplayerfile(std::string genrefilepath,int nbplayer,int bSimult)
{
    fstr xmlfile;
    { //
        ifstream ifs(genrefilepath);
        if(!ifs.good()) return;
        ifs.seekg(0,ios::end);
        size_t sz = (size_t)ifs.tellg();
        ifs.seekg(0,ios::beg);
        ifs.clear();

        xmlfile._b.reserve(sz+1);
        xmlfile._b.resize(sz+1);
        ifs.read(xmlfile._b.data(),sz);
        xmlfile._b[sz] = 0;
    }

/*
<game name="swimmer" index="" image="">
	<description>Swimmer (set 1)</description>
	<cloneof/>
	<crc/>
	<manufacturer>Tehkan</manufacturer>
	<year>1982</year>
	<genre>Water</genre>
	<rating/>
	<enabled>Yes</enabled>
</game>
*/
  string keyn = "<game";
   size_t igame = xmlfile.find(keyn);
    while(igame != string::npos)
    {
        igame+=keyn.length();
        string name = getAttrib(xmlfile,igame, "name");
        if(name.size()==0) {
             igame = xmlfile.find(keyn,igame);
             continue;
        }
        if(bSimult)  ggenres[name].nbPlayersSim =nbplayer;
        else ggenres[name].nbPlayersAlt =nbplayer;

        igame = xmlfile.find(keyn,igame);
    }
}
void readGenres()
{
   string sdir =genresbase;

    vector<string> entries;
   for (const auto & entry : fs::directory_iterator(sdir))
   {
        string sname =  entry.path().filename().string();
      //entries.push_back(entry.path().filename().string());
      if(sname.rfind(".xml") != sname.length()-4 ) continue;

        eGameGenre utype = numBHyName(sname);
        readgenrefile(sdir+"/"+sname,(int)utype);

        // string ofilepath = sourcebase+"driverso/" + sname;
        // string nfilepath = sourcebase+"drivers/" + sname;
        // changeapi(ofilepath,nfilepath);
   }
}
void readPlayers()
{
   string sdir =playersbase;
    readplayerfile(sdir+"/1P Games.xml",1,0);
    readplayerfile(sdir+"/2P sim Games.xml",2,1);
    readplayerfile(sdir+"/2P alt Games.xml",2,0);
    readplayerfile(sdir+"/3P sim Games.xml",3,1);
    readplayerfile(sdir+"/3P alt Games.xml",3,0);
    readplayerfile(sdir+"/4P sim Games.xml",4,1);
    readplayerfile(sdir+"/4P alt Games.xml",4,0);
    readplayerfile(sdir+"/6P sim Games.xml",6,1);
    readplayerfile(sdir+"/6P alt Games.xml",6,0);
    readplayerfile(sdir+"/8P sim Games.xml",8,1);
    readplayerfile(sdir+"/8P alt Games.xml",8,0);

}

// , nbp) ->
// , nbpsim,nbpalt, enumgenre,0|flag| flag|flag)
bool changeapi(std::string ofilepath,std::string nfilepath)
{
cout << "check: " <<ofilepath << endl;
//if(ofilepath.find("neogeo") != string::npos)
//{
//    printf("?");
//}
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
    if(!ofsb.good()) return 2;



    size_t i=0;
    while(i<bfile.length())
    {
         size_t iadd=0;
         size_t inxt = bfile.find("GAME(",i);
         if(inxt != string::npos) iadd = 5;

         size_t inxtb = bfile.find("GAMEB(",i);
         if(inxtb != string::npos && (inxt == string::npos || inxtb<inxt) )
         {
             inxt = inxtb;
             iadd = 6;
         }

         inxtb = bfile.find("GAME (",i);
         if(inxtb != string::npos && (inxt == string::npos || inxtb<inxt) )
         {
             inxt = inxtb;
             iadd = 6;
         }

        if(inxt != string::npos )
        {
            inxt+=iadd;
            size_t iend = bfile.findceiq(')',inxt);
            if(iend == string::npos) { i= inxt; continue; }
            // get one coma back
            size_t iendcoma = bfile.rfind_first_of(",",iend);

            string sparams =bfile.substr(inxt,iend-inxt);
            vector<string> prms = splitt_trim(sparams, ",");
            string name = prms[1];
            string parent = prms[2];
            //int nbp = getNbPlayer( name,  parent);

           // bfile.substros(ofsb,i,iend-i);
            bfile.substros(ofsb,i,iendcoma-i+1);


            GGenre &g = getGenre(name,parent);
            int gnum = 0;
            if(g.genreEnums.size()>0) gnum = g.genreEnums[0];
            if(g.genreEnums.size()>1)
            {
                if(gnum ==0) gnum = g.genreEnums[1];
                if(gnum == (int)egg_sport_) {
                    gnum = g.genreEnums[1];
                }
            }
            const char *penumname = enumCnames[gnum];
            ofsb << g.nbPlayersSim <<","<<g.nbPlayersAlt<<"," << penumname << ",0)";

            i=iend+1;
        } else
            break;
    }
    bfile.substros(ofsb,i,bfile.length()-i);

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
    // - - - - - - read database in map.
    readGenres();
    cout << "found genres for: " << (int)ggenres.size() << endl;
    readPlayers();

    if(ggenres.size()==0)
    {
        cout << "couldn't get genre" <<endl;
        return 1;

    }
    // - - - - - -
    string sdir =sourcebase+"driverso";

    vector<string> entries;
   for (const auto & entry : fs::directory_iterator(sdir))
   {
        string sname =  entry.path().filename().string();
      //entries.push_back(entry.path().filename().string());
      if(sname.rfind(".c") != sname.length()-2 &&
      sname.rfind(".h") != sname.length()-2
      ) continue;

        string ofilepath = sourcebase+"driverso/" + sname;
        string nfilepath = sourcebase+"drivers/" + sname;
        changeapi(ofilepath,nfilepath);
   }

/*
    fstr xmlfile;
    xmlfile._filename = "../controls.xml";
    { //
        ifstream ifs("../controls.xml");
        if(!ifs.good()) return 1;
        ifs.seekg(0,ios::end);
        size_t sz = (size_t)ifs.tellg();
        ifs.seekg(0,ios::beg);
        ifs.clear();

        xmlfile._b.reserve(sz+1);
        xmlfile._b.resize(sz+1);
        ifs.read(xmlfile._b.data(),sz);
        xmlfile._b[sz] = 0;
    }
    size_t igame = xmlfile.find("<game");
    while(igame != string::npos)
    {
        igame+=5;
        string name = getAttrib(xmlfile,igame, "romname");
        if(name.size()==0) {
             igame = xmlfile.find("<game",igame);
             continue;
        }
        string snumPlayers = getAttrib(xmlfile,igame, "numPlayers");
        int inbp=-1;
        if(snumPlayers.size()>0)
        {
            inbp = stoi(snumPlayers);
        }
        _controls[name]._nbp = inbp;

// 	<game romname="koshien" gamename="Ah Eikou no Koshien (Japan)" numPlayers="2" alternating="0" mirrored="1" usesService="0" tilt="1" cocktail="0">

        igame = xmlfile.find("<game",igame);
    }
    printf("controls found:%d\n",(int)_controls.size());
*/
/* TODO
    string sdir =sourcebase+"driverso";

    vector<string> entries;
   for (const auto & entry : fs::directory_iterator(sdir))
   {
        string sname =  entry.path().filename().string();
      //entries.push_back(entry.path().filename().string());
      if(sname.rfind(".c") != sname.length()-2 &&
      sname.rfind(".h") != sname.length()-2
      ) continue;

        string ofilepath = sourcebase+"driverso/" + sname;
        string nfilepath = sourcebase+"drivers/" + sname;
        changeapi(ofilepath,nfilepath);
   }
*/

    return EXIT_SUCCESS;
}
