#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
using namespace std;

string sourcebase("../src/mame106/");

class TGameDriver {
    public:
     string _name;
     string _year;
     string _parent;
     string _bios;
     string _machine;
     string _input;
     string _init;
     string _monitor;
     string _company;
     string _fullname;
     string _flags;
};


class TMachine {
    public:
    string          _name;
    vector<string>  _sources;
    map<string,TGameDriver>  _gamedrivers;
    map<string,int>  _sound_defs;
    map<string,int>  _cpu_defs;
};

class TChip {
public:
    map<string,vector<string>> _vars;
   // vector<string> _sources;
};

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


int searchDrivers(TMachine &machine, map<string,vector<string>> &vars)
{
    const vector<string> &sounds = vars["SOUNDS"];
    const vector<string> &cpus = vars["CPUS"];

    for(const string &s : machine._sources)
    {
        string sourcepath = sourcebase+s;
        ifstream ifssrc(sourcepath);
        if(!ifssrc.good()) {
            cout << "didn't find source: " << sourcepath << endl;
            continue;
        }
        while(!ifssrc.eof())
        {
            string line = rgetline(ifssrc);
            trim(line);
            size_t isComment = line.find("//");
            if(isComment == 0) continue; // quick escape
            size_t isgamedriverline =line.find("GAME(");
            size_t isgamedriverlineb =line.find("GAMEB(");
            size_t is_soundinclude =line.find("\"sound/");
            size_t is_cpuinclude =line.find("\"cpu/");
            if(isgamedriverline != string::npos && !(isComment != string::npos && isComment<isgamedriverline ))
            {
                size_t stend = line.find(")",isgamedriverline+5);
                if(stend != string::npos) {
     // #define GAME(YEAR,NAME,PARENT,MACHINE,INPUT,INIT,MONITOR,COMPANY,FULLNAME,FLAGS)
                    vector<string> v= splitt(line.substr(isgamedriverline+5,isgamedriverline+5-stend),",");
                    if(v.size()>=10)
                    {
                        string name= trimquotes(v[1]);
                        TGameDriver &game = machine._gamedrivers[name];
                        game._name = name;
                        game._year = trimquotes(v[0]);
                        game._parent = trimquotes(v[2]);
                        game._machine = trimquotes(v[3]);
                        game._input = trimquotes(v[4]);
                        game._init = trimquotes(v[5]);
                        game._monitor = trimquotes(v[6]);
                        game._company = trimquotes(v[7]);
                        game._fullname = trimquotes(v[8]);
                        game._flags = trimquotes(v[9]);
                    }
                }
            } else
            if(isgamedriverlineb != string::npos && !(isComment != string::npos && isComment<isgamedriverlineb ))
            {
                size_t stend = line.find(")",isgamedriverlineb+6);
                if(stend != string::npos) {
// #define GAMEB(YEAR,NAME,PARENT,BIOS,MACHINE,INPUT,INIT,MONITOR,COMPANY,FULLNAME,FLAGS)
                    vector<string> v= splitt(line.substr(isgamedriverlineb+6,isgamedriverlineb+6-stend),",");
                    if(v.size()>=11)
                    {
                        string name= trimquotes(v[1]);
                        TGameDriver &game = machine._gamedrivers[name];
                        game._name = name;
                        game._year = trimquotes(v[0]);
                        game._parent = trimquotes(v[2]);
                        game._machine = trimquotes(v[4]);
                        game._input = trimquotes(v[5]);
                        game._init = trimquotes(v[6]);
                        game._monitor = trimquotes(v[7]);
                        game._company = trimquotes(v[8]);
                        game._fullname = trimquotes(v[9]);
                        game._flags = trimquotes(v[10]);
                    }
                }
            } // if GAMEB
            if(is_soundinclude != string::npos)
            {
                size_t endsi = line.find(".h\"",is_soundinclude+7);
                if(endsi !=  string::npos)
                {
                    string soundh = line.substr(is_soundinclude+7,endsi-(is_soundinclude+7));
                    // look if correspond to known sound cpu
                    toUpper(soundh);
                    // ugly but works
                    if(soundh=="3812INTF") soundh="YM3812";
                    if(soundh=="2413INTF") soundh="YM2413";
                    if(soundh=="2203INTF") soundh="YM2203";
                    if(soundh=="2610INTF") soundh="YM2610";

                    if(soundh=="5220INTF") soundh="TMS5220";
                    if(soundh=="5110INTF") soundh="TMS5110";

                    if(soundh=="2151INTF") soundh="YM2151";
                    if(soundh=="TIAINTF") soundh="TIA";

                    if(soundh=="262INTF") soundh="YMF262";

                    if(soundh=="PSX") soundh="PSXSPU";
                    if(soundh=="GAELCO") soundh="GAELCO_GAE1";
                    //
                    bool isInDefs = false;                 
                    for(const string &sounditem : sounds )
                    {
                        if(sounditem == soundh) {isInDefs = true; break;}
                    }
                    cout << "got sound:"<<soundh << " isin: "<< (isInDefs?"OK":"--")<< endl;
                    if(isInDefs) {
                        machine._sound_defs[soundh]=1;
                    } else {
                        cout << "not in defs" << endl;
                    }
                }
            } // end if sound include
            if(is_cpuinclude != string::npos)
            {
                size_t endsi = line.find("/",is_cpuinclude+5);
                if(endsi !=  string::npos)
                {
                    string cpuh = line.substr(is_cpuinclude+5,endsi-(is_cpuinclude+5));
                    // look if correspond to known sound cpu
                    toUpper(cpuh);
                    bool isInDefs = false;
                    for(const string &cpuitem : cpus )
                    {
                        if(cpuitem == cpuh) {isInDefs = true; break;}
                    }
                    cout << "got cpu:"<<cpuh << " isin: "<< (isInDefs?"OK":"--")<< endl;
                    if(isInDefs) {
                        machine._cpu_defs[cpuh]=1;
                    }
                }
            } // end if sound include

        } // end while line
    } // end loop per each c source
    return EXIT_SUCCESS;
}

//int make_mini_machine(
//            map<string,vector<string>> &vars,
//            map<string,TMachine> &machinetargets,
//            string machinetominify,
//            std::vector<std::string> alloweddrivers
//                )
//{
//    //
//    auto f = machinetargets.find(machinetominify);
//    if(f == machinetargets.end()) return 1;
//    TMachine &src = f->second;

//    string nmachinename=string("mini")+machinetominify;
//    TMachine  &nmachine=machinetargets[nmachinename];
//    nmachine._name = nmachinename;
//    for(const string &dn : alloweddrivers)
//    {
//        nmachine._gamedrivers[dn] = src._gamedrivers[dn];
//    }
//    machinetargets[pkgname]._gamedrivers["tmnt"] = src._gamedrivers["tmnt"];
//    machinetargets[pkgname]._gamedrivers["tmnt2"] = src._gamedrivers["tmnt2"];
//}

// post read_mak_machines
int patchMiniMachines(
            map<string,vector<string>> &vars,
            map<string,TMachine> &machinetargets
            )
{
//    make_mini_machine(vars,machinetargets,
//    "konami",
//    {"tmnt","tmnt2","simpsons"});

//        string pkgname = "MINIKONAMI";
//        auto &src = machinetargets["KONAMI"];
//        machinetargets[pkgname]._name = pkgname;
//        machinetargets[pkgname]._gamedrivers["tmnt"] = src._gamedrivers["tmnt"];
//        machinetargets[pkgname]._gamedrivers["tmnt2"] = src._gamedrivers["tmnt2"];

    {
        TMachine  &src=machinetargets["konami"];
        string mname=string("minikonami");
        TMachine  &m=machinetargets[mname];
        m._name = mname;
        // same tmnt machine:
        m._gamedrivers["tmnt"] = src._gamedrivers["tmnt"];
        m._gamedrivers["tmnt2"] = src._gamedrivers["tmnt2"];
        m._gamedrivers["ssriders"] = src._gamedrivers["ssriders"];
        m._gamedrivers["prmrsocr"] = src._gamedrivers["prmrsocr"];
        m._gamedrivers["lgtnfght"] = src._gamedrivers["lgtnfght"];
        m._gamedrivers["lgtnfghu"] = src._gamedrivers["lgtnfghu"];
        m._gamedrivers["punkshot"] = src._gamedrivers["punkshot"];
        m._gamedrivers["glfgreat"] = src._gamedrivers["glfgreat"];
        m._gamedrivers["blswhstl"] = src._gamedrivers["blswhstl"];
        m._gamedrivers["detatwin"] = src._gamedrivers["detatwin"];

        // another machine
        m._gamedrivers["simpsons"] = src._gamedrivers["simpsons"];
        // drivers/simpsons.c machine/simpsons.c vidhrdw/simpsons.c
        // drivers/tmnt.c vidhrdw/tmnt.c
        m._sources = {
            "drivers/tmnt.c","vidhrdw/tmnt.c","vidhrdw/konamiic.c",
            "drivers/simpsons.c","machine/simpsons.c","vidhrdw/simpsons.c"
        };
        m._cpu_defs["KONAMI"]=1;
        m._cpu_defs["M68000"]=1;
        m._cpu_defs["Z80"]=1;

        m._sound_defs["YM2151"]=1;
        m._sound_defs["OKIM6295"]=1;
        m._sound_defs["SAMPLES"]=1;
        m._sound_defs["K053260"]=1;
        m._sound_defs["K054539"]=1;
        m._sound_defs["K007232"]=1;
        m._sound_defs["UPD7759"]=1;

        //MCPU_KONAMI
    }
    {
        TMachine  &src=machinetargets["midway"];
        string mname=string("midwaytunit");
        TMachine  &m=machinetargets[mname];
        m._name = mname;
        //
         m._gamedrivers["mk"] = src._gamedrivers["mk"];
         m._gamedrivers["mkr4"] = src._gamedrivers["mkr4"];

         m._gamedrivers["mk2"] = src._gamedrivers["mk2"];
         m._gamedrivers["mk2r32"] = src._gamedrivers["mk2r32"];
         m._gamedrivers["mk2r21"] = src._gamedrivers["mk2r21"];
         m._gamedrivers["mk2r14"] = src._gamedrivers["mk2r14"];
         m._gamedrivers["mk2r42"] = src._gamedrivers["mk2r42"];
         m._gamedrivers["mk2r91"] = src._gamedrivers["mk2r91"];
         m._gamedrivers["mk2chal"] = src._gamedrivers["mk2chal"];


         m._gamedrivers["jdreddp"] = src._gamedrivers["jdreddp"];

         m._gamedrivers["nbajam"] = src._gamedrivers["nbajam"];
         m._gamedrivers["nbajamr2"] = src._gamedrivers["nbajamr2"];
         m._gamedrivers["nbajamte"] = src._gamedrivers["nbajamte"];
         m._gamedrivers["nbajamt1"] = src._gamedrivers["nbajamt1"];
         m._gamedrivers["nbajamt2"] = src._gamedrivers["nbajamt2"];
         m._gamedrivers["nbajamt3"] = src._gamedrivers["nbajamt3"];
// yunit OMG all those games are horrible
         m._gamedrivers["narc"] = src._gamedrivers["narc"];
         m._gamedrivers["narc3"] = src._gamedrivers["narc3"];

         m._gamedrivers["trog"] = src._gamedrivers["trog"];
         m._gamedrivers["trog3"] = src._gamedrivers["trog3"];
         m._gamedrivers["trogpa6"] = src._gamedrivers["trogpa6"];
         m._gamedrivers["trogp"] = src._gamedrivers["trogp"];
         m._gamedrivers["strkforc"] = src._gamedrivers["strkforc"];

         m._gamedrivers["smashtv"] = src._gamedrivers["smashtv"];
         m._gamedrivers["smashtv6"] = src._gamedrivers["smashtv6"];
         m._gamedrivers["smashtv5"] = src._gamedrivers["smashtv5"];
         m._gamedrivers["smashtv4"] = src._gamedrivers["smashtv4"];
         m._gamedrivers["hiimpact"] = src._gamedrivers["hiimpact"];
         m._gamedrivers["hiimpac3"] = src._gamedrivers["hiimpac3"];
         m._gamedrivers["hiimpacp"] = src._gamedrivers["hiimpacp"];
         m._gamedrivers["shimpact"] = src._gamedrivers["shimpact"];
         m._gamedrivers["shimpacp"] = src._gamedrivers["shimpacp"];
         m._gamedrivers["term2"] = src._gamedrivers["term2"];
         m._gamedrivers["term2la2"] = src._gamedrivers["term2la2"];
         m._gamedrivers["term2la1"] = src._gamedrivers["term2la1"];
         m._gamedrivers["mkprot9"] = src._gamedrivers["mkprot9"];
         m._gamedrivers["mkla1"] = src._gamedrivers["mkla1"];
         m._gamedrivers["mkla2"] = src._gamedrivers["mkla2"];
         m._gamedrivers["mkla3"] = src._gamedrivers["mkla3"];
         m._gamedrivers["mkla4"] = src._gamedrivers["mkla4"];
         m._gamedrivers["mkyawdim"] = src._gamedrivers["mkyawdim"];
         m._gamedrivers["totcarn"] = src._gamedrivers["totcarn"];
         m._gamedrivers["totcarnp"] = src._gamedrivers["totcarnp"];


         m._sources = {
             "drivers/midtunit.c","machine/midtunit.c","vidhrdw/midtunit.c",
             "drivers/midyunit.c","machine/midyunit.c","vidhrdw/midyunit.c",
             "machine/midwayic.c",
             //"drivers/williams.c","machine/williams.c",
             "sndhrdw/williams.c",
             "sndhrdw/dcs.c",
             "sndhrdw/cage.c" // needed by midwayic.c
         };
        m._cpu_defs["M6800"]=1;
        m._cpu_defs["M6808"]=1;
        m._cpu_defs["M6809"]=1;
        m._cpu_defs["TMS34010"]=1;
        m._cpu_defs["ADSP2100"]=1;
        m._cpu_defs["ADSP2101"]=1;
        m._cpu_defs["ADSP2104"]=1;
        m._cpu_defs["ADSP2105"]=1;
        m._cpu_defs["ADSP2115"]=1;
        m._cpu_defs["ADSP2181"]=1;

        m._sound_defs["HC55516"]=1;
        m._sound_defs["DMADAC"]=1;
        // DMADAC
//
        /*

GAME( 1992, mk,       0,       tunit_adpcm, mk,       mktunit,  ROT0, "Midway",   "Mortal Kombat (rev 5.0 T-Unit 03/19/93)", 0 )
GAME( 1992, mkr4,     mk,      tunit_adpcm, mk,       mktunit,  ROT0, "Midway",   "Mortal Kombat (rev 4.0 T-Unit 02/11/93)", 0 )

GAME( 1993, mk2,      0,       tunit_dcs,   mk2,      mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L3.1)", 0 )
GAME( 1993, mk2r32,   mk2,     tunit_dcs,   mk2,      mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L3.2 (European))", 0 )
GAME( 1993, mk2r21,   mk2,     tunit_dcs,   mk2,      mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L2.1)", 0 )
GAME( 1993, mk2r14,   mk2,     tunit_dcs,   mk2,      mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L1.4)", 0 )
GAME( 1993, mk2r42,   mk2,     tunit_dcs,   mk2,      mk2,      ROT0, "hack",     "Mortal Kombat II (rev L4.2, hack)", 0 )
GAME( 1993, mk2r91,   mk2,     tunit_dcs,   mk2,      mk2,      ROT0, "hack",     "Mortal Kombat II (rev L9.1, hack)", 0 )
GAME( 1993, mk2chal,  mk2,     tunit_dcs,   mk2,      mk2,      ROT0, "hack",     "Mortal Kombat II Challenger (hack)", 0 )

GAME( 1993, jdreddp,  0,       tunit_adpcm, jdreddp,  jdreddp,  ROT0, "Midway",   "Judge Dredd (rev LA1, prototype)", 0 )

GAME( 1993, nbajam,   0,       tunit_adpcm, nbajam,   nbajam,   ROT0, "Midway",   "NBA Jam (rev 3.01 04/07/93)", 0 )
GAME( 1993, nbajamr2, nbajam,  tunit_adpcm, nbajam,   nbajam,   ROT0, "Midway",   "NBA Jam (rev 2.00 02/10/93)", 0 )
GAME( 1994, nbajamte, nbajam,  tunit_adpcm, nbajamte, nbajamte, ROT0, "Midway",   "NBA Jam TE (rev 4.0 03/23/94)", 0 )
GAME( 1994, nbajamt1, nbajam,  tunit_adpcm, nbajamte, nbajamte, ROT0, "Midway",   "NBA Jam TE (rev 1.0 01/17/94)", 0 )
GAME( 1994, nbajamt2, nbajam,  tunit_adpcm, nbajamte, nbajamte, ROT0, "Midway",   "NBA Jam TE (rev 2.0 01/28/94)", 0 )
GAME( 1994, nbajamt3, nbajam,  tunit_adpcm, nbajamte, nbajamte, ROT0, "Midway",   "NBA Jam TE (rev 3.0 03/04/94)", 0 )

*/
    }

    return 0;
}

int read_mak_machines(
            map<string,vector<string>> &vars,
            map<string,TMachine> &machinetargets
        )
{
    ifstream ifsmak(sourcebase+"mame.mak");
    if(!ifsmak.good()) {
        cout << "need mame.mak" << endl;
        return EXIT_FAILURE;
    }

    string line, nextline;
    bool dolinknext = false;
    size_t il=0;
    while(!ifsmak.eof())
    {
        nextline = rgetline(ifsmak);
        il++;
        if(isJustComment(nextline) ) {
            continue;
        }
       // cout <<"l:"<<il << " : "<< nextline << endl;
        bool hasbackward = (!nextline.empty() && nextline.back()=='\\');
        if(hasbackward) nextline.pop_back();
        trim(nextline);

        if(dolinknext)
        {
            if(!line.empty()) line+=" ";
            line += nextline;
            dolinknext = false;
        }
        else
        {
            line = nextline;
        }
        if(hasbackward){
            dolinknext = true;
            continue;
        }

        // treat real line
        if(line.empty()) continue;
        cout << "line:"<< line << endl;
        if(line.find("+=") != string::npos){
            string skey,sval;
            sepline(line,"+=",skey,sval);
            if(!skey.empty())
            {
                vector<string> v = splitt(sval," ");
                vector<string> &mv = vars[skey];
                mv.insert(mv.end(),v.begin(),v.end());
            }

        } else if(line.find("=") != string::npos)
        {
            string skey,sval;
            sepline(line,"=",skey,sval);
            if(!skey.empty())
            {
                vector<string> v = splitt(sval," ");
                vector<string> &mv = vars[skey];
                mv.insert(mv.end(),v.begin(),v.end());
            }
        } else if(line.find(":") != string::npos)
        {
            string skey,sval;
            sepline(line,":",skey,sval);
            if(!skey.empty())
            {
                skey = replace(skey,"$(OBJ)/","");
                skey = replace(skey,".a","");
                sval = replace(sval,"$(OBJ)/","");
                sval = replace(sval,".o",".c");
                vector<string> v = splitt(sval," ");
                TMachine &m = machinetargets[skey];
                m._name = skey;
                m._sources.insert( m._sources.end(),v.begin(),v.end());
                searchDrivers(m,vars);
            }
        }
        line.clear();
        dolinknext = false;
    }
    return EXIT_SUCCESS;
}


int createCmake(map<string,TMachine> machinetargets,
                map<string,TChip> &soundsources,
                map<string,TChip> &cpusources)
{
    ofstream ofs("gamedrivers.cmake", ios::binary|ios::out);
    if(!ofs.good()) return 1;

    for(const auto &p: machinetargets)
    {
        string upname = p.first;
        toUpper(upname);
        bool onShouldBeDefault=false;
        // this is actually a common dependance lib that most machine use.
        if(upname == "SHARED") onShouldBeDefault=true;
        // this is optional
        if(upname == "SEGA" ) onShouldBeDefault = true;
        if(upname == "NEOGEO" ) onShouldBeDefault = true;
        if(upname == "CAPCOM" ) onShouldBeDefault = true;
        if(upname == "TAITO" ) onShouldBeDefault = true;
        // just for rtype, rtypeleo -> THEY DONT WORK !!
//        if(upname == "IREM" ) onShouldBeDefault = true;
//        if(upname == "DATAEAST" ) onShouldBeDefault = true;
        // just for buggy boy :) ->DOESNT WORK On 0.106 :(
//        if(upname == "TATSUMI" ) onShouldBeDefault = true; // tested ok

//        // just for double dragon1/2/3
        if(upname == "TECHNOS" ) onShouldBeDefault = true; // tested ok
//        // just for gals panic.
        if(upname == "KANEKO" ) onShouldBeDefault = true; // tested ok
//        // just for paddle mania :)
//        if(upname == "ALPHA" ) onShouldBeDefault = true; // tested ok
//        // just for silkworm and rygar :)  tekhan=tecmo
        if(upname == "TEHKAN" ) onShouldBeDefault = true; // tested ok
//        // just for  snow bros and slap fight :)
        if(upname == "TOAPLAN" ) onShouldBeDefault = true;
        // konami is huge, need ninja turtles and simpsons.
        if(upname == "MINIKONAMI" ) onShouldBeDefault = true;

        // some midway machines, mk mk2 nbajam
        if(upname == "MIDWAYTUNIT" ) onShouldBeDefault = true;

        ofs << "option(OPT_"<< upname<< " \"\" "<<(onShouldBeDefault?"ON":"OFF")<< ")\n";
    }

    // active sound chips sources
    map<string,int> cpueverused,soundeverused;

    for(const auto &p: machinetargets)
    {
        string upname = p.first;
        const TMachine &machine=p.second;
        toUpper(upname);
        ofs << "if(OPT_"<< upname<< ")\n";
        ofs << "\tadd_compile_definitions(LINK_"<< upname<< "=1)\n";
        ofs << "\tlist(APPEND MAME_DRIVERS_SRC\n";
        int nbinline=0;
        for(const string &src : machine._sources)
        {
            if(nbinline==0) ofs << "\t\t";
            ofs << src << " ";
            nbinline++;
            if(nbinline==4) {
                ofs << "\n";
            nbinline=0;
            }
        }
        ofs << "\t)\n";

#ifdef ADDCPUANDSOUNDINMACHINE
        // - - - add sound sources
        if(machine._sound_defs.size()>0)
        {
            ofs << "\tlist(APPEND MAME_SOUND_SRC\n";
            nbinline=0;
            for(const auto &defs : machine._sound_defs)
            {
                string updefs = defs.first;
                toUpper(updefs);
                // search if sources known for that chip
                if(soundsources.find(updefs)!=soundsources.end())
                {
                    TChip &schip = soundsources[updefs];
                    if(schip._vars.find("SOUNDOBJS")!=schip._vars.end() )
                    {
                        for(const string &src : schip._vars["SOUNDOBJS"])
                        {
                            if(nbinline==0) ofs << "\t\t";
                            ofs << src << " ";
                            nbinline++;
                            if(nbinline==4) {
                                ofs << "\n";
                            nbinline=0;
                            }

                        }
                    }
                } else
                {
                    cout << " **** no source found for soundchip: "<<updefs << endl;
                }
            }
            ofs << "\t)\n";
        }
        // - - - add cpu sources
        if(machine._cpu_defs.size()>0)
        {
            ofs << "\tlist(APPEND MAME_CPU_SRC\n";
            nbinline=0;
            for(const auto &defs : machine._cpu_defs)
            {
                string updefs = defs.first;
                toUpper(updefs);
                // search if sources known for that chip
                if(cpusources.find(updefs)!=cpusources.end())
                {
                    TChip &schip = cpusources[updefs];
                    if(schip._vars.find("CPUOBJS")!=schip._vars.end() )
                    {
                        for(const string &src : schip._vars["CPUOBJS"])
                        {
                            if(nbinline==0) ofs << "\t\t";
                            ofs << src << " ";
                            nbinline++;
                            if(nbinline==4) {
                                ofs << "\n";
                            nbinline=0;
                            }

                        }
                    }
                } else
                {
                    cout << " **** no source found for cpuchip: "<<updefs << endl;
                }
            }
            ofs << "\t)\n";
        }
#else
        // - - use cmake vars to activate CPU & Soundchip or not, and source swill be added at the end.
        // activate sound chips sources
        for(const auto &defs : machine._sound_defs)
        {
            string updefs = defs.first;
            toUpper(updefs);
            soundeverused[updefs]=1;
            ofs << "\tset(MSND_"<<updefs<<" ON)\n";
        }
        // active CPU sources
        for(const auto &defs : machine._cpu_defs)
        {
            string updefs = defs.first;
            toUpper(updefs);
            cpueverused[updefs]=1;
            ofs << "\tset(MCPU_"<<updefs<<" ON)\n";
        }
#endif
        // - - - add sound defs
        if(machine._sound_defs.size()>0)
        {
            ofs << "\tlist(APPEND CPU_DEFS\n";
            nbinline=0;
            for(const auto &defs : machine._sound_defs)
            {
                string updefs = defs.first;
                toUpper(updefs);
                if(nbinline==0) ofs << "\t\t";
                ofs << "HAS_" << updefs << "=1 ";
                nbinline++;
                if(nbinline==4) {
                    ofs << "\n";
                nbinline=0;
                }
            }
            ofs << "\t)\n";
        }
        if(machine._cpu_defs.size()>0)
        {
            ofs << "\tlist(APPEND CPU_DEFS\n";
            nbinline=0;
            for(const auto &defs : machine._cpu_defs)
            {
                string updefs = defs.first;
                toUpper(updefs);
                if(nbinline==0) ofs << "\t\t";
                ofs << "HAS_" << updefs << "=1 ";
                nbinline++;
                if(nbinline==4) {
                    ofs << "\n";
                nbinline=0;
                }
            }
            ofs << "\t)\n";
        }

        ofs << "endif()\n";
    }
#ifndef ADDCPUANDSOUNDINMACHINE
    // add cpu & soundchip sources

    for(const auto &p : soundeverused )
    {
        string soundid = p.first;
        toUpper(soundid);
        TChip &schip = soundsources[soundid];
         // search if sources known for that chip
        if(schip._vars.find("SOUNDOBJS")!=schip._vars.end() )
        {
            ofs << "if(MSND_"<<soundid<<")\n";
            ofs << "\tlist(APPEND MAME_SOUND_SRC ";
            for(const string &src : schip._vars["SOUNDOBJS"])
            {
                ofs << src << " ";
            }
            ofs << ")\nendif()\n";
        } else
        {
            cout << " **** no source found for soundchip: "<<soundid << endl;
        }
    }
    for(const auto &p : cpueverused )
    {
        string cpuid = p.first;
        toUpper(cpuid);
        TChip &schip = cpusources[cpuid];
         // search if sources known for that chip
        if(schip._vars.find("CPUOBJS")!=schip._vars.end() )
        {
            ofs << "if(MCPU_"<<cpuid<<")\n";
            ofs << "\tlist(APPEND MAME_CPU_SRC ";
            for(const string &src : schip._vars["CPUOBJS"])
            {
                ofs << src << " ";
            }
            ofs << ")\nendif()\n";
        } else
        {
            cout << " **** no source found for cpu: "<<cpuid << endl;
        }
    }

#endif

    ofs <<
    "list(REMOVE_DUPLICATES MAME_DRIVERS_SRC)\n"
    "list(REMOVE_DUPLICATES MAME_SOUND_SRC)\n"
    "list(REMOVE_DUPLICATES MAME_CPU_SRC)\n"
    "list(REMOVE_DUPLICATES CPU_DEFS)\n"
    ;

    return EXIT_SUCCESS;
}

int createMameDrivc( map<string,TMachine> &machinetargets)
{
    ifstream ifs(sourcebase+"mamedriv.c");
    if(!ifs.good()) return 1;

    ofstream ofs("mamedriv.c", ios::binary|ios::out);
    if(!ofs.good()) return 1;

    while(!ifs.eof())
    {
        string line = rgetline(ifs);
        ofs << line << "\n";
        if(line.find("#else	/* DRIVER_RECURSIVE */")==0)
        {
            break;
        }
    }
    // - - - -from there on,
    for(const auto &p : machinetargets)
    {
        const TMachine &machine = p.second;
        string upName = p.first;
        toUpper(upName);
        ofs << "#ifdef LINK_"<<upName<<"\n";
        for(const auto &pgd : machine._gamedrivers)
        {
            const TGameDriver &gd = pgd.second;
            ofs << "\tDRIVER( " <<pgd.first<< " ) /* "<< gd._year << " " << gd._company<< " "  <<gd._fullname<< " */\n";
        }

        ofs << "#endif\n";
    }

    // end is easy:
    ofs << "#endif	/* DRIVER_RECURSIVE */" << endl;

//#else	/* DRIVER_RECURSIVE */
//#ifdef OTHERDRIVERS
//	/* "Pacman hardware" games */
//	DRIVER( puckman )	/* (c) 1980 Namco */
//	DRIVER( puckmana )	/* (c) 1980 Namco */
    return 0;
}

int read_mak_sounds(map<string,TChip> &soundsources)
{
    ifstream ifsmak(sourcebase+"sound/sound.mak");
    if(!ifsmak.good()) {
        cout << "need sound.mak" << endl;
        return EXIT_FAILURE;
    }

    string currentSoundChip;
    vector<string> allcurrentsoundchips;

    string line, nextline;
    bool dolinknext = false;
    size_t il=0;
    while(!ifsmak.eof())
    {
        nextline = rgetline(ifsmak);
        il++;
        if(isJustComment(nextline) ) {
            continue;
        }
       // cout <<"l:"<<il << " : "<< nextline << endl;
        bool hasbackward = (!nextline.empty() && nextline.back()=='\\');
        if(hasbackward) nextline.pop_back();
        trim(nextline);

        if(dolinknext)
        {
            if(!line.empty()) line+=" ";
            line += nextline;
            dolinknext = false;
        }
        else
        {
            line = nextline;
        }
        if(hasbackward){
            dolinknext = true;
            continue;
        }

        // treat real line
        if(line.empty()) continue;
//        cout << "line:"<< line << endl;
        string filterpattern="ifneq ($(filter ";
        size_t iFilter = line.find(filterpattern);
        if(iFilter != string::npos)
        {
            size_t iend =line.find(",",iFilter+filterpattern.length());
            if(iend != string::npos)
            {
                currentSoundChip = line.substr(iFilter+filterpattern.length(),iend-(iFilter+filterpattern.length()));
            }
        }
        // - - if currentChip contains many chips, we have to consider this is the conf for each.
        if(currentSoundChip.find(" ") != string::npos)
        {
            allcurrentsoundchips = splitt(currentSoundChip," ");
            currentSoundChip = allcurrentsoundchips[0];
        }

        if(line.find("endif")==0 && !currentSoundChip.empty() )
        {
            // means end of currentchip, may apply equivalences (M68020 use same code as M68000, ...)
            if( allcurrentsoundchips.size()>1)
            {
                for(size_t ic=1;ic<allcurrentsoundchips.size();ic++)
                {
                    soundsources[allcurrentsoundchips[ic]] = soundsources[currentSoundChip];
                }
            }
            allcurrentsoundchips.clear();
            currentSoundChip.clear();
        }


        if(line.find("+=") != string::npos){
            string skey,sval;
            sepline(line,"+=",skey,sval);
            if(!skey.empty())
            {
                sval = replace(sval,"$(OBJ)/","");
                sval = replace(sval,".o",".c");
                vector<string> v = splitt(sval," ");
                vector<string> &mv = soundsources[currentSoundChip]._vars[skey];
                mv.insert(mv.end(),v.begin(),v.end());
            }

        } else if(line.find("=") != string::npos)
        {
            string skey,sval;
            sepline(line,"=",skey,sval);
            if(!skey.empty())
            {
                sval = replace(sval,"$(OBJ)/","");
                sval = replace(sval,".o",".c");
                vector<string> v = splitt(sval," ");
                vector<string> &mv = soundsources[currentSoundChip]._vars[skey];
                mv.insert(mv.end(),v.begin(),v.end());
            }
        }
        line.clear();
        dolinknext = false;
    }
    return 0;
}

int read_mak_cpus(map<string,TChip> &sources)
{
    ifstream ifsmak(sourcebase+"cpu/cpu.mak");
    if(!ifsmak.good()) {
        cout << "need cpu.mak" << endl;
        return EXIT_FAILURE;
    }

    string currentChip;
    vector<string> allcurrentchips;

    string line, nextline;
    bool dolinknext = false;
    size_t il=0;
    while(!ifsmak.eof())
    {
        nextline = rgetline(ifsmak);
        il++;
        if(isJustComment(nextline) ) {
            continue;
        }
       // cout <<"l:"<<il << " : "<< nextline << endl;
        bool hasbackward = (!nextline.empty() && nextline.back()=='\\');
        if(hasbackward) nextline.pop_back();
        trim(nextline);

        if(dolinknext)
        {
            if(!line.empty()) line+=" ";
            line += nextline;
            dolinknext = false;
        }
        else
        {
            line = nextline;
        }
        if(hasbackward){
            dolinknext = true;
            continue;
        }

        // treat real line
        if(line.empty()) continue;


//        cout << "line:"<< line << endl;
        string filterpattern="ifneq ($(filter ";
        size_t iFilter = line.find(filterpattern);
        if(iFilter != string::npos)
        {
            size_t iend =line.find(",",iFilter+filterpattern.length());
            if(iend != string::npos)
            {
                currentChip = line.substr(iFilter+filterpattern.length(),iend-(iFilter+filterpattern.length()));
                trim(currentChip);
            }
        }
        // - - if currentChip contains many chips, we have to consider this is the conf for each.

        if(currentChip.find(" ") != string::npos)
        {
            allcurrentchips = splitt(currentChip," ");
            currentChip = allcurrentchips[0];
        }

        if(line.find("endif")==0 && !currentChip.empty())
        {
            // means end of currentchip, may apply equivalences (M68020 use same code as M68000, ...)
            if(allcurrentchips.size()>1)
            for(size_t ic=1;ic<allcurrentchips.size();ic++)
            {
                sources[allcurrentchips[ic]] = sources[currentChip];
            }
            allcurrentchips.clear();
            currentChip.clear();
        }

        if(line.find("+=") != string::npos){
            string skey,sval;
            sepline(line,"+=",skey,sval);
            if(!skey.empty())
            {
                sval = replace(sval,"$(OBJ)/","");
                sval = replace(sval,".o",".c");
                vector<string> v = splitt(sval," ");
                vector<string> &mv = sources[currentChip]._vars[skey];
                mv.insert(mv.end(),v.begin(),v.end());
            }

        } else if(line.find("=") != string::npos)
        {
            string skey,sval;
            sepline(line,"=",skey,sval);
            if(!skey.empty())
            {
                sval = replace(sval,"$(OBJ)/","");
                sval = replace(sval,".o",".c");
                vector<string> v = splitt(sval," ");
                vector<string> &mv = sources[currentChip]._vars[skey];
                mv.insert(mv.end(),v.begin(),v.end());
            }
        }
        line.clear();
        dolinknext = false;
    }
    return 0;
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

void completeDefinitionsByHand(
            map<string,TMachine> &machinetargets,
            map<string,TChip> &cpusources
            )
{
    machinetargets["sega"]._sound_defs["YM3438"]=1;
    machinetargets["sega"]._cpu_defs["I8039"]=1; // sound cpu ? ->no.
    machinetargets["sega"]._cpu_defs["ADSP21062"]=1; // cpu

    machinetargets["neogeo"]._sound_defs["YM2610B"]=1;

    machinetargets["capcom"]._sound_defs["YM2610B"]=1;
    machinetargets["capcom"]._cpu_defs["PSXCPU"]=1;

    // original makefile set that in general sources, link it has it is due:
    machinetargets["capcom"]._sources.push_back("machine/psx.c");
    machinetargets["capcom"]._sources.push_back("vidhrdw/psx.c");
    machinetargets["capcom"]._sources.push_back("sndhrdw/taitosnd.c");

    // taito
    machinetargets["taito"]._cpu_defs["M6802"]=1;
    machinetargets["taito"]._sound_defs["YM2610B"]=1;
    machinetargets["taito"]._sound_defs["YM3526"]=1;

    //
    machinetargets["taito"]._sources.push_back("drivers/seta.c"); // needed by taito_x.c
    machinetargets["taito"]._sources.push_back("vidhrdw/seta.c");
    machinetargets["taito"]._sound_defs["YM3438"]=1;
    //
    // wiz is actually in "seibu", rollrace picks in it.
   // machinetargets["taito"]._sources.push_back("vidhrdw/wiz.c");

    // we can remove specific driver that way:
    auto &taitosrc =  machinetargets["taito"]._sources;
    removeInVector(taitosrc,"drivers/rollrace.c");
    removeInVector(taitosrc,"vidhrdw/rollrace.c");
    machinetargets["taito"]._gamedrivers.erase("fightrol");
    machinetargets["taito"]._gamedrivers.erase("rollace");
    machinetargets["taito"]._gamedrivers.erase("rollace2");
    machinetargets["dataeast"]._cpu_defs["DECO16"]=1; // m6502 variant, need HAS_DECO16.
    machinetargets["dataeast"]._cpu_defs["M65C02"]=1;

    // - - - - - - --  -technos:
    // we can remove specific driver that way:
    auto &technossrc =  machinetargets["technos"]._sources;


//    machinetargets["technos"]._gamedrivers.erase("renegade"); // need YM3526
//    removeInVector(technossrc,"drivers/renegade.c");
//    removeInVector(technossrc,"vidhrdw/renegade.c");

//    machinetargets["technos"]._gamedrivers.erase("battlane"); // need YM3526
//    removeInVector(technossrc,"drivers/battlane.c");
//    removeInVector(technossrc,"vidhrdw/battlane.c");

//    machinetargets["technos"]._gamedrivers.erase("matmania"); // need YM3526, looks cool.
//    removeInVector(technossrc,"drivers/matmania.c");
//    removeInVector(technossrc,"vidhrdw/matmania.c");
    machinetargets["technos"]._sound_defs["YM3526"]=1;

    // old ugly game that needs lots of special things...
    removeInVector(technossrc,"drivers/scregg.c");
    machinetargets["technos"]._gamedrivers.erase("eggs");
    machinetargets["technos"]._gamedrivers.erase("scregg");
    machinetargets["technos"]._gamedrivers.erase("dommy");
    // - - - - - - end technos

    // thekan/tecmo (silkworm,...)
    machinetargets["tehkan"]._sound_defs["YM2608"]=1;

    // irem
    machinetargets["irem"]._cpu_defs["M6803"]=1;

    // try patch just a package
    /*todo, good idea
    {
        string pkgname = "MINIKONAMI";
        auto &src = machinetargets["KONAMI"];
        machinetargets[pkgname]._name = pkgname;
        machinetargets[pkgname]._gamedrivers["tmnt"] = src._gamedrivers["tmnt"];
        machinetargets[pkgname]._gamedrivers["tmnt2"] = src._gamedrivers["tmnt2"];
    }*/
}
int main(int argc, char **argv)
{

    map<string,vector<string>> vars;
    map<string,TMachine> machinetargets;

    int r = read_mak_machines(vars,machinetargets);
    if(r) return r;

    patchMiniMachines(vars,machinetargets);

    map<string,TChip> soundsources;
    r = read_mak_sounds(soundsources);
    if(r) return r;

    map<string,TChip> cpusources;
    r = read_mak_cpus(cpusources);
    if(r) return r;

//    TMachine &tm = machinetargets["sega"];

    completeDefinitionsByHand(machinetargets,cpusources);

    createCmake(machinetargets,soundsources,cpusources);

    createMameDrivc(machinetargets);

    cout << "\nEverything went extremely well :).\n" << endl;

    return EXIT_SUCCESS;
}
