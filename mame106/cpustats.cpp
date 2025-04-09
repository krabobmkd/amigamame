
extern "C" {
    #include "cpustats.h"
    #include "mame.h"
}
#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;


struct cpuinstr{
    UINT32 _adr=0;
    UINT32 _instr=0;
    UINT32 _nbrun=0;
};
struct cpustat{
    map<UINT32,cpuinstr> _s; // adr, instr
};

map<int,cpustat> _stats;


void cpustats_reset()
{
    _stats.clear();
}
void cpustats_add(UINT32 ncpu, UINT32 adr, UINT32 instr)
{
    cpuinstr & cpui = _stats[ncpu]._s[adr];
    cpui._adr = adr;
    cpui._instr = instr;
    cpui._nbrun++;
}
static int sortfunc(const cpuinstr &a,const cpuinstr &b)
{
    return (a._nbrun>b._nbrun);
}

void cpustats_log(int icpu)
{
    cpustat &cs = _stats[icpu];
    vector<cpuinstr> v;

    map<UINT32,cpuinstr>::iterator it = cs._s.begin();
    while(it !=cs._s.end())
    {
        const pair<UINT32,cpuinstr> &p = *it++;
        v.push_back(p.second);
    }

    std::sort(v.begin(),v.end(),sortfunc);

   printf(" ----- cpu %d ----- \n",icpu);

    size_t maxl = 12;
    for(size_t i=0 ; i<v.size() && i<maxl ; i++)
    {
        cpuinstr &ci = v[i];
        printf(" adr:%08x nbr:%08x regir:%08x\n",ci._adr,ci._nbrun,ci._instr);
    }
    printf(" -------\n");

}

void cpustats_log()
{
    map<int,cpustat>::iterator it = _stats.begin();
    while(it !=_stats.end())
    {
        const pair<int,cpustat> &p = *it++;
        cpustats_log(p.first);
    }

}
