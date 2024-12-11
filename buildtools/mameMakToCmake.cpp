#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <filesystem>
using namespace std;

string sourcebase("../mame106/");

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

    // these ones are parsed from MDRV_ macros and should be quite exact.
    map<string,int>  _sound_defs;
    map<string,int>  _cpu_defs;
    int _useful = 0;
    TGameDriver *_parentptr = nullptr;
};

vector<string>::iterator findInV(vector<string> &v,string n)
{
    vector<string>::iterator it = v.begin();
    while(it != v.end())
    {
        if(*it == n) return it;
        it++;
    }
    return v.end();
}

// actully more "per constructor" list of many machines.
class TMachine {
    public:
    string          _name;
    vector<string>  _sources;
    map<string,vector<string>> _sourceWithDrivers; // int is nb working games.

    map<string,TGameDriver>  _gamedrivers;
    map<string,TGameDriver>  _machinedrivers; // some machines are not gamedrivers.
    // these ones are by
    map<string,int>  _sound_defs;
    map<string,int>  _cpu_defs;
    int _useful = 1;
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


// just for information
map<string,int> _cpustats;
map<string,int> _soundchipstats;
map<string,vector<string>> _cpu_use;
map<string,vector<string>> _soundchip_use;

int searchDrivers(TMachine &machine, map<string,vector<string>> &vars)
{
    const vector<string> &sounds = vars["SOUNDS"];
    const vector<string> &cpus = vars["CPUS"];

   // machine._sourceWithDrivers.clear();

    for(const string &s : machine._sources)
    {

        // these are heavy non working beta codes in v0.106, do not implement.
        if(s == "drivers/model2.c" ||
            s =="drivers/model3.c")
            {
             continue;
           }

        string sourcepath = sourcebase+s;
        ifstream ifssrc(sourcepath);
        if(!ifssrc.good()) {
            cout << "didn't find source: " << sourcepath << endl;
            continue;
        }
        std::string currentMachineDefInStream;

        while(!ifssrc.eof())
        {
            string line = rgetline(ifssrc);
            trim(line);
            size_t isComment = line.find("//");            
            if(isComment == 0) continue; // quick escape

            // simply parse official mame macros to know machine dfinition
            // MACHINE_DRIVER_START( drvname )
            //  MDRV_CPU_ADD(Z80,
            // MDRV_SOUND_ADD(
            // MACHINE_DRIVER_END
            string machineStart = getMacroFirstParam(line,"MACHINE_DRIVER_START");
            if(!machineStart.empty()) currentMachineDefInStream = machineStart;
            if(line.find("MACHINE_DRIVER_END")!= string::npos) currentMachineDefInStream.clear();
            if(!currentMachineDefInStream.empty())
            {
                string addcpu = getMacroFirstParam(line,"MDRV_CPU_ADD(");
                if(!addcpu.empty()) {
                    toUpper(addcpu);
                    // note: may be not a game:
                  machine._machinedrivers[currentMachineDefInStream]._cpu_defs[addcpu] = 1;
                     // but a dependency for sure:
                     machine._cpu_defs[addcpu] = 1;
                     _cpustats[addcpu]++;
                     _cpu_use[addcpu].push_back(currentMachineDefInStream);
                }
                // this also
                //
                addcpu = getMacroSecondParam(line,"MDRV_CPU_ADD_TAG(");
                if(!addcpu.empty()) {
                    toUpper(addcpu);
                    // note: may be not a game:
                  machine._machinedrivers[currentMachineDefInStream]._cpu_defs[addcpu] = 1;
                     // but a dependency for sure:
                     machine._cpu_defs[addcpu] = 1;
                     _cpustats[addcpu]++;
                     _cpu_use[addcpu].push_back(currentMachineDefInStream);
                }
                string addsound = getMacroFirstParam(line,"MDRV_SOUND_ADD(");
                if(!addsound.empty()) {
                    toUpper(addsound);
                     // note: may be not a game:
                     machine._machinedrivers[currentMachineDefInStream]._sound_defs[addsound] = 1;
                     // but a dependency for sure:
                     machine._sound_defs[addsound] = 1;
                     _soundchipstats[addsound]++;
                     _soundchip_use[addsound].push_back(currentMachineDefInStream);
                }
                addsound = getMacroSecondParam(line,"MDRV_SOUND_ADD_TAG(");
                if(!addsound.empty()) {
                    toUpper(addsound);
                    // note: may be not a game:
                  machine._machinedrivers[currentMachineDefInStream]._sound_defs[addsound] = 1;
                     // but a dependency for sure:
                     machine._sound_defs[addsound] = 1;
                     _soundchipstats[addsound]++;
                     _soundchip_use[addsound].push_back(currentMachineDefInStream);
                }
            }

            // ----------------- old parsing always OK
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

                        machine._sourceWithDrivers[s].push_back(name);
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

                        machine._sourceWithDrivers[s].push_back(name);
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
                  //  cout << "got sound:"<<soundh << " isin: "<< (isInDefs?"OK":"--")<< endl;
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
                 //   cout << "got cpu:"<<cpuh << " isin: "<< (isInDefs?"OK":"--")<< endl;
                    if(isInDefs) {
                        machine._cpu_defs[cpuh]=1;
                    }
                }
            } // end if sound include

        } // end while line
        // - - -
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


void copyDrivers(TMachine  &m,TMachine  &msrc,std::vector<std::string> drivers)
{
    for(string &name : drivers)
    {
        m._gamedrivers[name] = msrc._gamedrivers[name];
    }

}

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
        TMachine  &namco=machinetargets["namco"];
        string mname=string("justgalaga");
        TMachine  &m=machinetargets[mname];

        copyDrivers(m,namco,{
"bosco","boscoo","boscoo2","boscomd","boscomdo",
"galaga","galagao","galagamw","galagamk","gallag","gatsbee",
"xevious","xeviousa","xeviousb","xeviousc","xevios","battles","sxevious",
"digdug","digdugb","digdugat","digduga1","dzigzag"
});
        m._sources={
            "drivers/galaga.c","vidhrdw/galaga.c",
            "vidhrdw/bosco.c",
            "machine/namcoio.c",
            "machine/xevious.c","vidhrdw/xevious.c","vidhrdw/digdug.c",
            "machine/atari_vg.c",
//            "sound/namco52.c","sound/namco54.c",
//             "sound/namco.c",

           "machine/atarigen.c",  "machine/atarigen.h",
           "machine/atari_vg.c",
        "sndhrdw/atarijsa.c","sndhrdw/atarijsa.h"
        };
        m._sound_defs["NAMCO"]=1;
        m._sound_defs["NAMCO_52XX"]=1;
        m._sound_defs["NAMCO_54XX"]=1;
        m._sound_defs["FILTER"]=1;
        m._sound_defs["SAMPLES"]=1;

        m._cpu_defs["Z80"]=1;
/*
	MDRV_SOUND_ADD(NAMCO, 18432000/6/32)
	MDRV_SOUND_CONFIG(namco_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.90 * 10.0 / 16.0)

	MDRV_SOUND_ADD(NAMCO_52XX, 18432000/12)
        MDRV_SOUND_CONFIG(namco_52xx_interface)
        MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.90)

        MDRV_SOUND_ADD(NAMCO_54XX, 18432000/12)
        MDRV_SOUND_CONFIG(namco_54xx_interface)
        MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.90)

        MDRV_SOUND_ADD(SAMPLES, 0)
        MDRV_SOUND_CONFIG(samples_interface_bosco)
        MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.95)
*/
    }

    {
        TMachine  &src=machinetargets["nintendo"];
        string mname=string("justdkong");
        TMachine  &m=machinetargets[mname];
        m._name = mname;

        m._sources = {
            "drivers/dkong_short.c","sndhrdw/dkong.c","vidhrdw/dkong.c",
            // "machine/scramble.c", // this one  used by konami also
            // "machine/strtheat.c",
            // "machine/drakton.c"
        };

        m._cpu_defs["Z80"]=1;
        m._cpu_defs["I8035"]=1;
        m._cpu_defs["S2650"]=1;
        m._cpu_defs["N2A03"]=1;
        m._sound_defs["DAC"]=1;
        m._sound_defs["SAMPLES"]=1;
        m._sound_defs["NES"]=1;
// HAS_N2A03
        copyDrivers(m,src,{
                            "dkong","dkongo","dkongjp","dkongjr","dkongjrj",
                            "dkong3","dkong3j"
                            });


    }


    {
        TMachine  &src=machinetargets["atari"];
        string mname=string("miniatari");
        TMachine  &m=machinetargets[mname];
        m._name = mname;

       m._sources = {
            "drivers/ccastles.c",
            "vidhrdw/ccastles.c",

            "drivers/centiped.c",
            "vidhrdw/centiped.c",

            "drivers/gauntlet.c",
            "vidhrdw/gauntlet.c",

            // "machine/harddriv.c",  dontwork
            // "drivers/harddriv.c",
            // "sndhrdw/harddriv.c",
            // "vidhrdw/harddriv.c",

            "drivers/rampart.c",
            "vidhrdw/rampart.c",

            "vidhrdw/atarimo.c",
            "machine/slapstic.c",

            "drivers/klax.c",
            "vidhrdw/klax.c",
            // marbemadness, paperboy
            "drivers/atarisy1.c",  // M68010 M6502
            "drivers/atarisy2.c", // T11 M6502   s:POKEY  YM2151 TMS5220
            "vidhrdw/atarisy1.c",
            "vidhrdw/atarisy2.c",

            "machine/asic65.c",
            "machine/atari_vg.c",
            "machine/atarigen.c",  "machine/atarigen.h",
            "machine/atari_vg.c",
            "sndhrdw/atarijsa.c","sndhrdw/atarijsa.h"

        };
        copyDrivers(m,src,{
        "ccastles","ccastle3","ccastle2",
        "centiped","centipd2","centtime","milliped","warlords","bullsdrt",
        "rampart","ramprt2p","rampartj",
        "gauntlet","gaunts","gauntj","gauntg","gaunt2","gaunt2g","vindctr2",
        "klax","klaxj","klaxd",

// they don't work
//        "harddriv","harddrvc",
//        "stunrun","stunrunp",
        // "racedriv","racedrvc",
        // "steeltal",
        // "strtdriv",
        // "hdrivair",

        // atarisy1
        "marble","marble2","marble3","marble4",
        "peterpak","indytemp","indytem2","indytem3","indytem4","indytemd",
        "roadrunn","roadrun2","roadrun1",
        "roadblst","roadblsg","roadblsc","roadblcg","roadblc1",

        "paperboy","paperbr2","paperbr1",
        "720","720r3","720r2","720r1","720g","720gr1",
        "ssprint","ssprint3","ssprint1","ssprintg",
        "csprint","csprint2","csprintf","csprints","csprintg",

        "apb","apb6","apbg","apbf"
    });

        m._cpu_defs["M68010"]=1; // gauntlet + hardwrivin stun runner
        m._cpu_defs["M6502"]=1;
        m._cpu_defs["S2650"]=1;
        m._cpu_defs["M68000"]=1; // rampart
        m._cpu_defs["TMS34010"]=1; // hardwrivin , ssecond
        m._cpu_defs["TMS32010"]=1;

        m._cpu_defs["ADSP2100"]=1; // hardwrivin friends
        m._cpu_defs["ADSP2101"]=1;
        m._cpu_defs["ADSP2104"]=1;
        m._cpu_defs["ADSP2181"]=1;

        m._cpu_defs["DSP32C"]=1;

        m._cpu_defs["T11"]=1;

        m._sound_defs["AY8910"]=1;
        m._sound_defs["YM2151"]=1;
        m._sound_defs["TMS5220"]=1;
        m._sound_defs["POKEY"]=1;
        m._sound_defs["SN76496"]=1;

        m._sound_defs["YM2413"]=1; // rampart
        m._sound_defs["OKIM6295"]=1;

        m._sound_defs["DAC"]=1; // hardwrivin
    }
    {
        TMachine  &src=machinetargets["neogeo"];
        src._sources.push_back("vidhrdw/neogeodraw.cpp");
        src._sources.push_back("vidhrdw/neogeodraw.h");

    }

    {
        TMachine  &src=machinetargets["dataeast"];
        src._sources.push_back("vidhrdw/dec0n.cpp");

        string mname=string("minidtea");
        TMachine  &m=machinetargets[mname];
        m._name = mname;
        m._gamedrivers["supbtime"] = src._gamedrivers["supbtime"];
        m._gamedrivers["supbtimj"] = src._gamedrivers["supbtimj"];
        m._gamedrivers["chinatwn"] = src._gamedrivers["chinatwn"];

        m._gamedrivers["karnov"] = src._gamedrivers["karnov"];
        m._gamedrivers["karnovj"] = src._gamedrivers["karnovj"];
        m._gamedrivers["wndrplnt"] = src._gamedrivers["wndrplnt"];
        m._gamedrivers["chelnov"] = src._gamedrivers["chelnov"];
        m._gamedrivers["chelnovu"] = src._gamedrivers["chelnovu"];
        m._gamedrivers["chelnovj"] = src._gamedrivers["chelnovj"];

        // dec0 driver
        m._gamedrivers["hbarrel"] = src._gamedrivers["hbarrel"];
        m._gamedrivers["hbarrelw"] = src._gamedrivers["hbarrelw"];
        m._gamedrivers["baddudes"] = src._gamedrivers["baddudes"];
        m._gamedrivers["drgninja"] = src._gamedrivers["drgninja"];
        m._gamedrivers["birdtry"] = src._gamedrivers["birdtry"];
        m._gamedrivers["robocop"] = src._gamedrivers["robocop"];
        m._gamedrivers["robocopw"] = src._gamedrivers["robocopw"];
        m._gamedrivers["robocopj"] = src._gamedrivers["robocopj"];
        m._gamedrivers["robocopu"] = src._gamedrivers["robocopu"];

        m._gamedrivers["robocpu0"] = src._gamedrivers["robocpu0"];
        m._gamedrivers["robocopb"] = src._gamedrivers["robocopb"];

        m._gamedrivers["hippodrm"] = src._gamedrivers["hippodrm"];
        m._gamedrivers["ffantasy"] = src._gamedrivers["ffantasy"];
        m._gamedrivers["ffantasa"] = src._gamedrivers["ffantasa"];

        m._gamedrivers["slyspy"] = src._gamedrivers["slyspy"];
        m._gamedrivers["slyspy2"] = src._gamedrivers["slyspy2"];
        m._gamedrivers["secretag"] = src._gamedrivers["secretag"];
        m._gamedrivers["secretab"] = src._gamedrivers["secretab"];
        m._gamedrivers["midres"] = src._gamedrivers["midres"];
        m._gamedrivers["midresu"] = src._gamedrivers["midresu"];
        m._gamedrivers["midresj"] = src._gamedrivers["midresj"];
        m._gamedrivers["bouldash"] = src._gamedrivers["bouldash"];
        m._gamedrivers["bouldshj"] = src._gamedrivers["bouldshj"];

        m._sources = {
            "drivers/supbtime.c","vidhrdw/supbtime.c","vidhrdw/deco16ic.c",
            "drivers/karnov.c","vidhrdw/karnov.c",
            "drivers/dec0.c","machine/dec0.c","vidhrdw/dec0.c","vidhrdw/dec0n.cpp"
        };
        m._cpu_defs["M68000"]=1;
        m._cpu_defs["M6502"]=1;
        m._cpu_defs["H6280"]=1;

        m._sound_defs["YM2203"]=1;
        m._sound_defs["YM3526"]=1;

        m._sound_defs["YM3812"]=1;
        m._sound_defs["OKIM6295"]=1;

    } // end mini dteast

    {
        // just for pacmania
        TMachine  &src=machinetargets["namcos"];
        string mname=string("mininamcos1");
        TMachine  &m=machinetargets[mname];
        m._name = mname;
        m._sources = {"drivers/namcos1.c","machine/namcos1.c","vidhrdw/namcos1.c"
            };

        m._gamedrivers["shadowld"] = src._gamedrivers["shadowld"];
        m._gamedrivers["youkaidk"] = src._gamedrivers["youkaidk"];
        m._gamedrivers["yokaidko"] = src._gamedrivers["yokaidko"];
        m._gamedrivers["dspirit"] = src._gamedrivers["dspirit"];
        m._gamedrivers["dspirito"] = src._gamedrivers["dspirito"];
        m._gamedrivers["blazer"] = src._gamedrivers["blazer"];
        m._gamedrivers["quester"] = src._gamedrivers["quester"];
        m._gamedrivers["pacmania"] = src._gamedrivers["pacmania"];
        m._gamedrivers["pacmanij"] = src._gamedrivers["pacmanij"];

        m._gamedrivers["galaga88"] = src._gamedrivers["galaga88"];
        m._gamedrivers["galag88j"] = src._gamedrivers["galag88j"];
        m._gamedrivers["ws"] = src._gamedrivers["ws"];
        m._gamedrivers["berabohm"] = src._gamedrivers["berabohm"];
        m._gamedrivers["beraboho"] = src._gamedrivers["beraboho"];
        m._gamedrivers["mmaze"] = src._gamedrivers["mmaze"];
        m._gamedrivers["bakutotu"] = src._gamedrivers["bakutotu"];

        m._gamedrivers["wldcourt"] = src._gamedrivers["wldcourt"];
        m._gamedrivers["splatter"] = src._gamedrivers["splatter"];
        m._gamedrivers["splattej"] = src._gamedrivers["splattej"];
        m._gamedrivers["faceoff"] = src._gamedrivers["faceoff"];

        m._gamedrivers["rompers"] = src._gamedrivers["rompers"];
        m._gamedrivers["romperso"] = src._gamedrivers["romperso"];
        m._gamedrivers["blastoff"] = src._gamedrivers["blastoff"];
        m._gamedrivers["ws89"] = src._gamedrivers["ws89"];
        m._gamedrivers["dangseed"] = src._gamedrivers["dangseed"];
        m._gamedrivers["ws90"] = src._gamedrivers["ws90"];
        m._gamedrivers["pistoldm"] = src._gamedrivers["pistoldm"];
        m._gamedrivers["boxyboy"] = src._gamedrivers["boxyboy"];
        m._gamedrivers["soukobdx"] = src._gamedrivers["soukobdx"];
        m._gamedrivers["puzlclub"] = src._gamedrivers["puzlclub"];
        m._gamedrivers["tankfrce"] = src._gamedrivers["tankfrce"];
        m._gamedrivers["tankfrcj"] = src._gamedrivers["tankfrcj"];

        m._cpu_defs["M6809"]=1;
        m._cpu_defs["HD63701"]=1;

        m._sound_defs["YM2151"]=1;
        m._sound_defs["NAMCO_CUS30"]=1;
        m._sound_defs["DAC"]=1;

    }
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

    }
    {
        TMachine  &src=machinetargets["konami"];
        string mname=string("minikonami2");
        TMachine  &m=machinetargets[mname];
        m._name = mname;

        // added 12/2024
        m._gamedrivers["gberet"] = src._gamedrivers["gberet"];
        m._gamedrivers["gberetb"] = src._gamedrivers["gberetb"];
        m._gamedrivers["rushatck"] = src._gamedrivers["rushatck"];
        m._gamedrivers["mrgoemon"] = src._gamedrivers["mrgoemon"];

        m._gamedrivers["pingpong"] = src._gamedrivers["pingpong"];
        m._gamedrivers["merlinmm"] = src._gamedrivers["merlinmm"];

        m._gamedrivers["gyruss"] = src._gamedrivers["gyruss"];
        m._gamedrivers["gyrussce"] = src._gamedrivers["gyrussce"];

        //nemesis machines
        m._gamedrivers["nemesis"] = src._gamedrivers["nemesis"];
        m._gamedrivers["nemesuk"] = src._gamedrivers["nemesuk"];
        m._gamedrivers["konamigt"] = src._gamedrivers["konamigt"];
        m._gamedrivers["rf2"] = src._gamedrivers["rf2"];
        m._gamedrivers["twinbee"] = src._gamedrivers["twinbee"];
        m._gamedrivers["gradius"] = src._gamedrivers["gradius"];
        m._gamedrivers["gwarrior"] = src._gamedrivers["gwarrior"];
        m._gamedrivers["salamand"] = src._gamedrivers["salamand"];
        m._gamedrivers["salamanj"] = src._gamedrivers["salamanj"];
        m._gamedrivers["lifefrce"] = src._gamedrivers["lifefrce"];
        m._gamedrivers["lifefrcj"] = src._gamedrivers["lifefrcj"];
        m._gamedrivers["blkpnthr"] = src._gamedrivers["blkpnthr"];
        m._gamedrivers["citybomb"] = src._gamedrivers["citybomb"];
        m._gamedrivers["citybmrj"] = src._gamedrivers["citybmrj"];
        m._gamedrivers["hcrash"] = src._gamedrivers["hcrash"];
        m._gamedrivers["hcrashc"] = src._gamedrivers["hcrashc"];
        m._gamedrivers["kittenk"] = src._gamedrivers["kittenk"];
        m._gamedrivers["nyanpani"] = src._gamedrivers["nyanpani"];

        //twin16
        m._gamedrivers["devilw"] = src._gamedrivers["devilw"];
        m._gamedrivers["majuu"] = src._gamedrivers["majuu"];
        m._gamedrivers["darkadv"] = src._gamedrivers["darkadv"];
        m._gamedrivers["vulcan"] = src._gamedrivers["vulcan"];
        m._gamedrivers["gradius2"] = src._gamedrivers["gradius2"];
        m._gamedrivers["grdius2a"] = src._gamedrivers["grdius2a"];
        m._gamedrivers["grdius2b"] = src._gamedrivers["grdius2b"];
        m._gamedrivers["fround"] = src._gamedrivers["fround"];
        m._gamedrivers["froundl"] = src._gamedrivers["froundl"];
        m._gamedrivers["hpuncher"] = src._gamedrivers["hpuncher"];
        m._gamedrivers["miaj"] = src._gamedrivers["miaj"];
        m._gamedrivers["cuebrick"] = src._gamedrivers["cuebrick"];


        m._gamedrivers["gradius3"] = src._gamedrivers["gradius3"];

        //parodius
        m._gamedrivers["parodius"] = src._gamedrivers["parodius"];
        m._gamedrivers["parodisj"] = src._gamedrivers["parodisj"];

        m._sources = {
            "drivers/gberet.c","vidhrdw/gberet.c",
            "drivers/gyruss.c", "sndhrdw/gyruss.c", "vidhrdw/gyruss.c",
            "machine/konami.c",
            "drivers/pingpong.c","vidhrdw/pingpong.c",
            "drivers/gradius3.c","vidhrdw/gradius3.c",
            "drivers/nemesis.c","vidhrdw/nemesis.c",
            "drivers/twin16.c","vidhrdw/twin16.c", // gradius 2
            "drivers/parodius.c","vidhrdw/parodius.c"

        };
        m._cpu_defs["M68000"]=1;
        m._cpu_defs["Z80"]=1;
        m._cpu_defs["KONAMI"]=1; // parodius


        m._cpu_defs["M6809"]=1; // gyrus
        m._cpu_defs["I8039"]=1; // gyrus

        m._sound_defs["AY8910"]=1;  // nemesis, gyrus
        m._sound_defs["K005289"]=1;
        m._sound_defs["VLM5030"]=1;

        m._sound_defs["SN76496"]=1; // gberet / pingpong

        // gyrus has dreadful reverb !
        m._sound_defs["DAC"]=1; // gyruss

        // twin16
        m._sound_defs["YM2151"]=1;
        m._sound_defs["K007232"]=1;
        m._sound_defs["UPD7759"]=1;

        m._sound_defs["K053260"]=1; // parodius
        m._sound_defs["K051649"]=1; // parodius
    }
  {
        TMachine  &src=machinetargets["konami"];
        string mname=string("minikonami3");
        TMachine  &m=machinetargets[mname];
        m._name = mname;

        // added 12/2024
        m._gamedrivers["dbz"] = src._gamedrivers["dbz"];
        m._gamedrivers["dbz2"] = src._gamedrivers["dbz2"];

        // parodius, konamigx many non working games areremoved
        // yet does not work either -> raus.
#ifdef USE_KONAMIGX
        m._gamedrivers["konamigx"] = src._gamedrivers["konamigx"];
        m._gamedrivers["le2"] = src._gamedrivers["le2"];
        m._gamedrivers["le2u"] = src._gamedrivers["le2u"];
        m._gamedrivers["gokuparo"] = src._gamedrivers["gokuparo"];
        m._gamedrivers["puzldama"] = src._gamedrivers["puzldama"];
        m._gamedrivers["tbyahhoo"] = src._gamedrivers["tbyahhoo"];
        m._gamedrivers["tkmmpzdm"] = src._gamedrivers["tkmmpzdm"];
        m._gamedrivers["dragoona"] = src._gamedrivers["dragoona"];
        m._gamedrivers["dragoonj"] = src._gamedrivers["dragoonj"];
        m._gamedrivers["sexyparo"] = src._gamedrivers["sexyparo"];
        m._gamedrivers["daiskiss"] = src._gamedrivers["daiskiss"];
        m._gamedrivers["tokkae"] = src._gamedrivers["tokkae"];
        m._gamedrivers["salmndr2"] = src._gamedrivers["salmndr2"];
        m._gamedrivers["winspike"] = src._gamedrivers["winspike"];
        m._gamedrivers["winspikj"] = src._gamedrivers["winspikj"];
#endif
        m._sources = {
            "drivers/dbz.c","vidhrdw/dbz.c",
#ifdef USE_KONAMIGX
            "drivers/konamigx.c","machine/konamigx.c","vidhrdw/konamigx.c"
#endif
        };
        m._cpu_defs["M68EC020"]=1;
        m._cpu_defs["M68000"]=1;
        m._cpu_defs["Z80"]=1;

        m._sound_defs["K054539"]=1;
        m._sound_defs["YM2151"]=1;
        m._sound_defs["OKIM6295"]=1;

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
    {



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
         // pacmania,...
         if(upname == "MININAMCOS1") onShouldBeDefault=true;
         if(upname == "MINIATARI") onShouldBeDefault=true;
         if(upname == "JUSTDKONG") onShouldBeDefault=true;
         if(upname == "JALECO") onShouldBeDefault=true;

         if(upname == "CAPCOM" ) onShouldBeDefault = true;
        if(upname == "TAITO" ) onShouldBeDefault = true;
 //       if(upname == "NAMCO" ) onShouldBeDefault = true; -> too horizontaly dependent.
 //       if(upname == "PACMAN" ) onShouldBeDefault = true;
        if(upname == "IREM" ) onShouldBeDefault = true;
 // problem with filter
        if(upname == "JUSTGALAGA" ) onShouldBeDefault = true;
        if(upname == "PACMAN" ) onShouldBeDefault = true;
        // just for rtype, rtypeleo -> THEY DONT WORK !!
//        if(upname == "IREM" ) onShouldBeDefault = true;
//        if(upname == "DATAEAST" ) onShouldBeDefault = true; -> too big/shitty
        if(upname == "MINIDTEA" ) onShouldBeDefault = true;

        // because Q*BERT ! ->doesnt work
        if(upname == "GOTTLIEB" ) onShouldBeDefault = true;
        // just for buggy boy :) ->DOESNT WORK On 0.106 :(
//        if(upname == "TATSUMI" ) onShouldBeDefault = true; // tested ok

//        // just for double dragon1/2/3
        if(upname == "TECHNOS" ) onShouldBeDefault = true; // tested ok
//        // just for gals panic.
        if(upname == "KANEKO" ) onShouldBeDefault = true; // tested ok
//        // just for paddle mania :)
        if(upname == "ALPHA" ) onShouldBeDefault = true; // tested ok
//        // just for silkworm and rygar :)  tekhan=tecmo
        if(upname == "TEHKAN" ) onShouldBeDefault = true; // tested ok
//        // just for  snow bros and slap fight :)
        if(upname == "TOAPLAN" ) onShouldBeDefault = true;
        // konami is huge, need ninja turtles and simpsons.
        if(upname == "MINIKONAMI" ) onShouldBeDefault = true;
        if(upname == "MINIKONAMI2" ) onShouldBeDefault = true;
        if(upname == "MINIKONAMI3" ) onShouldBeDefault = true;
        if(upname == "NEOGEO" ) onShouldBeDefault = true;
        // toki & cabal
        if(upname == "TAD" ) onShouldBeDefault = true;

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

    soundeverused["FILTER"] = 1;

    soundsources["FILTER"]._vars["SOUNDOBJS"] = {"sound/filter.c","sound/filter.h"};

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


void removeSegaModel23(TMachine &m)
{
//    m._sources.erase("drivers/model2.c");
    std::remove(m._sources.begin(), m._sources.end(), "drivers/model2.c");
    std::remove(m._sources.begin(), m._sources.end(), "drivers/model3.c");
    std::remove(m._sources.begin(), m._sources.end(), "vidhrdw/model3.c");
    std::remove(m._sources.begin(), m._sources.end(), "machine/model3.c");

//    m._gamedrivers.erase(""); // drivers for model2 model3 empeached earlier.

    m._cpu_defs.erase("I960"); // intel only used in model2, doesnt work on 0.106
    m._cpu_defs.erase("ADSP21062");   // "sharc" only used in model2
    m._cpu_defs.erase("PPC603");   //  only used in model3

}

void completeDefinitionsByHand(
            map<string,TMachine> &machinetargets,
            map<string,TChip> &cpusources
            )
{
    machinetargets["sega"]._sound_defs["YM3438"]=1;
    machinetargets["sega"]._cpu_defs["I8039"]=1; // sound cpu ? ->no.
    machinetargets["sega"]._cpu_defs["ADSP21062"]=1; // cpu

    removeInVector(machinetargets["shared"]._sources,"vidhrdw/voodoo.c");

    machinetargets["shared"]._cpu_defs["ADSP21062"]=1; // cpu


    // on 106, all sega model2 and 3 are not working, just model1/ vfighter may work.
    removeSegaModel23(machinetargets["sega"]);



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
    machinetargets["taito"]._sound_defs["YM3526"]=1; // bubblbobbl
   machinetargets["taito"]._cpu_defs["M68705"]=1; // arkanoid need this controller

    //
    machinetargets["taito"]._sources.push_back("drivers/seta.c"); // needed by taito_x.c
    machinetargets["taito"]._sources.push_back("vidhrdw/seta.c");
    machinetargets["taito"]._sound_defs["YM3438"]=1;

    // the special ddragon cpu
    machinetargets["technos"]._cpu_defs["HD6309"]=1;


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

    // namco
    // HD63701
    machinetargets["namco"]._cpu_defs["HD63701"]=1;
    machinetargets["namco"]._cpu_defs["HD63705"]=1;
    // namco  wants some code from KONAMI !!
   // machinetargets["namco"]._sources.push_back("drivers/scramble.c");
    machinetargets["namco"]._sources.push_back("machine/scramble.c");

    // TAD
    machinetargets["tad"]._sources.push_back("sndhrdw/seibu.c");

//

//
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
//    machinetargets["tehkan"]._sound_defs["YM2608"]=1;

    // irem
//    machinetargets["irem"]._cpu_defs["M6803"]=1;
//    machinetargets["irem"]._cpu_defs["V30"]=1;
//    machinetargets["irem"]._cpu_defs["V33"]=1;
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

int recurseEverWork(std::string drv,
    map<string,TGameDriver *> &driverMap,
    map<string,vector<string>> &reverseDependencies)
{
    if(reverseDependencies[drv].size()==0) return 0;
    for(const string &name : reverseDependencies[drv])
    {
        TGameDriver *tgd =driverMap[name];
        if(!tgd) continue;
        int works = (tgd->_flags.find("GAME_NOT_WORKING")==string::npos);
        if(works) return 1;
        works = recurseEverWork(tgd->_name,driverMap,reverseDependencies);
        if(works) return 1;
    }
    return 0;

}
void removeUselessDrivers(
            map<string,TMachine> &machinetargets,
            map<string,TChip> &cpusources
            )
{
    // - --  init drivermap
    map<string,TGameDriver *> driverMap;
    map<string,vector<string>> reverseDependencies;


    for(pair<const string,TMachine> &p : machinetargets)
    {
        TMachine &machine = p.second;
        int nbGameDrivers=0;
        int nbWorkingDrivers=0;

        for(pair<const string,TGameDriver> &p : machine._gamedrivers)
        {
            TGameDriver &drv = p.second;
            drv._useful = 0;
            driverMap[p.first] = &drv;
            if(drv._parent.length()>0)
            {
                reverseDependencies[drv._parent].push_back(p.first);
            }
        }
        for(pair<const string,TGameDriver> &p : machine._machinedrivers)
        {
            TGameDriver &drv = p.second;
            drv._useful = 0;
            driverMap[p.first] = &drv;
            if(drv._parent.length()>0)
            {
                reverseDependencies[drv._parent].push_back(p.first);
            }
        }
    }
    // - - -tells who is usefull at all.
    for(pair<const string,TMachine> &p : machinetargets)
    {
        TMachine &machine = p.second;
        machine._useful = 0;
        if(p.first == "shared")
        {
            machine._useful = 1;
            continue;
        }

        for(pair<const string,TGameDriver> &p : machine._gamedrivers)
        {
            TGameDriver &drv = p.second;
            int works = (drv._flags.find("GAME_NOT_WORKING")==string::npos);
            drv._useful = works;
            // search if a dependence ever works...
            if(! drv._useful)
            {
                drv._useful = recurseEverWork(p.first,driverMap,reverseDependencies);

            }
            if(drv._useful) machine._useful = 1;
        }
        for(pair<const string,TGameDriver> &p : machine._machinedrivers)
        {
            TGameDriver &drv = p.second;
            int works = (drv._flags.find("GAME_NOT_WORKING")==string::npos);
            drv._useful = works;
            // search if a dependence ever works...
            if(! drv._useful)
            {
                drv._useful = recurseEverWork(p.first,driverMap,reverseDependencies);
            }
            if(drv._useful) machine._useful = 1;
        }                              
    }
    // - - - machine level


 //   map<string,TGameDriver *> driverMap;
//    for(pair<const string,TGameDriver *> &p :driverMap)
//    {
//        TGameDriver *pdrv = p.second;
//        if(pdrv->_useful==0)
//        {
//            // useless:
//            cout << "**** useless: " << pdrv->_name << " ";
//           cout << endl;

//            pdrv->_sound_defs.clear();
//            pdrv->_cpu_defs.clear();


//        }
//    }

// remove source
 for (map<const string,TMachine>::iterator mit = machinetargets.begin(); mit != machinetargets.end();)
    {
        TMachine &machine = mit->second;
        for(pair<const string,vector<string>> &p : machine._sourceWithDrivers)
        {
            int usefull=0;
            for(const string &g : p.second)
            {
                if( machine._gamedrivers.find(g) !=  machine._gamedrivers.end() )
                {
                    if(machine._gamedrivers[g]._useful) usefull=1;
                }
                if( machine._machinedrivers.find(g) !=  machine._machinedrivers.end() )
                {
                    if(machine._machinedrivers[g]._useful) usefull=1;

                }
                if(usefull) break;
            }
            if(usefull==0)
            {
                vector<string> vpath = splitt(p.first,"/");
                if(vpath.size()!=2) continue;
                vector<string>::iterator itf = findInV(machine._sources,string("drivers/")+vpath[1]);
                if(itf != machine._sources.end())
                {
                    cout << "remove source: "<< string("drivers/")+vpath[1] << endl;
                    machine._sources.erase(itf);
                 }
                 itf = findInV(machine._sources,string("machine/")+vpath[1]);
                if(itf != machine._sources.end())
                {
                cout << "remove source: "<< string("machine/")+vpath[1] << endl;
                    machine._sources.erase(itf);
                }
                 itf = findInV(machine._sources,string("vidhrdw/")+vpath[1]);
                if(itf != machine._sources.end())
                {
                cout << "remove source: "<< string("vidhrdw/")+vpath[1] << endl;
                    machine._sources.erase(itf);
                }
            }

        }
        mit++;
    }
  // for(pair<const string,TMachine> &p : machinetargets)
 for (map<const string,TMachine>::iterator mit = machinetargets.begin(); mit != machinetargets.end();)
    {
        TMachine &machine = mit->second;
        if(machine._useful ==0)
        {
            cout << " *** useless machine: "<< mit->first << " remove: ";
            for(const string &s : machine._sources)
            {
                cout << s << " ";
            }
            cout << endl;
           // machinesToremove.push_back(mit->first);
           machinetargets.erase(mit++);
        } else
        {
            // games to remove
            for (auto it = machine._gamedrivers.begin(); it != machine._gamedrivers.end();)
            {
                if(it->second._name == "tx1")
                {
                    cout << "tx1" << endl;
                }
              if (it->second._useful==0)
              {
                cout << "remove NW game: "<<it->first << endl;
                machine._gamedrivers.erase(it++);    // or "it = m.erase(it)" since C++11
              }
              else
              {
                ++it;
              }
            }
            for (auto it = machine._machinedrivers.begin(); it != machine._machinedrivers.end();)
            {
              if (it->second._useful==0)
              {
                cout << "remove NW machine: "<<it->first << endl;
                machine._machinedrivers.erase(it++);    // or "it = m.erase(it)" since C++11
              }
              else
              {
                ++it;
              }
            }
            mit++;
        }

//        // erase source
//        for(pair<const string,int> &p: machine._sourceWithDrivers)
//        {
//            if(p.second >0) continue;
//            vector<string> vpath = splitt(p.first,"/");
//            if(vpath.size()==2 )
//            {
//               // machine._sources.remove(p.first);
//                vector<string>::iterator itf = findInV(machine._sources,string("drivers/")+vpath[1]);
//                if(itf != machine._sources.end())
//                {
//                    cout << "remove source: "<< string("drivers/")+vpath[1] << endl;
//                    machine._sources.erase(itf);
//                 }
//                 itf = findInV(machine._sources,string("machine/")+vpath[1]);
//                if(itf != machine._sources.end())
//                {
//                cout << "remove source: "<< string("machine/")+vpath[1] << endl;
//                    machine._sources.erase(itf);
//                }
//                 itf = findInV(machine._sources,string("vidhrdw/")+vpath[1]);
//                if(itf != machine._sources.end())
//                {
//                cout << "remove source: "<< string("vidhrdw/")+vpath[1] << endl;
//                    machine._sources.erase(itf);
//                }

//            }
//        }


    } // end loop per machine





}

void addHeadersWhenPossible(map<string,TChip> &sources)
{
    for(pair<const string,TChip> &p : sources)
    {
        for(pair<const string,vector<string>> &pp : p.second._vars)
        {
            vector<string> srcsh;
            vector<string> &srcs = pp.second;
            for(string sc : srcs)
            {
//                if(sc.find("cpu/m68000")==0)
//                {
//                    cout <<"";
//                }
                //cout << "source:" << endl;
                if((sc.rfind(".c") == sc.length()-2)
                        ||
                   (sc.rfind(".cpp") == sc.length()-4)
                        )
                {
                    string phpath = sc;
                    phpath = replace(phpath,".cpp",".h");
                    phpath = replace(phpath,".c",".h");

                   if( filesystem::exists(sourcebase+phpath) )
                   {
                        srcsh.push_back(phpath);
                   }


                }

            }
            if(srcsh.size()>0)
            {
                srcs.insert(srcs.end(),srcsh.begin(),srcsh.end());
            }

        }
    }
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

    removeUselessDrivers(machinetargets,cpusources);

    addHeadersWhenPossible(soundsources);
    addHeadersWhenPossible(cpusources);

    createCmake(machinetargets,soundsources,cpusources);

    createMameDrivc(machinetargets);

//    {
//        for(const auto &p : _soundchipstats)
//        cout << "soundchip: "<<p.first <<"\t nb: "<< p.second<< endl;

//        for(const auto &p : _cpustats)
//        cout << "cpu: "<<p.first <<"\t nb: "<< p.second<< endl;

//    }


    cout << "\nEverything went extremely well, gamedrivers.cmake and mamedriv.c generated, may copy it into mame106/ :).\n" << endl;

    return EXIT_SUCCESS;
}
