// memoryn.cpp

extern "C"
{
#include "memory.h"
#include <stdio.h>

extern UINT8 *bank_ptr[STATIC_COUNT];
extern address_space active_address_space[ADDRESS_SPACES];/* address space data */

UINT32 memory_readlong_d16_be(offs_t address REGM(d0));
UINT16 memory_readword_d16_be(offs_t address REGM(d0));
UINT8 memory_readbyte_d16_be(offs_t address REGM(d0));
void memory_writelong_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) );
void memory_writeword_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) );
void memory_writebyte_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) );

UINT32 memory_readmovem32_wr16(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) );
UINT32 memory_writemovem32_wr16_reverse(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) );

}

#include <fstream>
#include <iostream>
#include <stdio.h>
using namespace std;


// these are definitions from memory.c , which are meant to be priovate but well...

union _rwhandlers
{
	genf *					generic;				/* generic handler void */
	read_handlers			read;					/* read handlers */
	write_handlers			write;					/* write handlers */
};
typedef union _rwhandlers rwhandlers;

struct _handler_data
{
	rwhandlers				handler;				/* function pointer for handler */
	offs_t					offset;					/* base offset for handler */
	offs_t					top;					/* maximum offset for handler */
	offs_t					mask;					/* mask against the final address */
	const char *			name;					/* name of the handler */
};

//FILE *pdbg=NULL;

//static int nbt=0, nbtt=0,nbttmax=200000;
//void startdbg()
//{
//    if(!pdbg) {
//        pdbg = fopen("memlog.txt","wb");
//    }
//}
//void traceMem(UINT32 adr, UINT32 v)
//{
//    startdbg();
//    fprintf(pdbg,"a:%08x %08x ",adr,v);
//    nbt++; nbtt++;
//    if(nbt >=8) { nbt=0; fprintf(pdbg,"\n"); }
//    if(nbtt>=nbttmax) exit(1337);
//}
//void traceMem(UINT32 adr, UINT16 v)
//{
//    startdbg();
//    fprintf(pdbg,"a:%08x ____%04x ",adr,(UINT32)v);
//    nbt++; nbtt++;
//    if(nbt >=8) { nbt=0; fprintf(pdbg,"\n"); }
//    if(nbtt>=nbttmax) exit(1337);
//}
//void traceMem(UINT32 adr, UINT8 v)
//{
//    startdbg();
//    fprintf(pdbg,"a:%08x ______%02x ",adr,(UINT32)v);
//    nbt++; nbtt++;
//    if(nbt >=8) { nbt=0; fprintf(pdbg,"\n"); }
//    if(nbtt>=nbttmax) exit(1337);
//}


void analyseAdressSpace(address_space &space)
{
    // if game doesnt use useDirectReadB / useDirectWriteB cases,
    // we can just do one test per call.
    int useDirectReadB=0;
    int useDirectWriteB=0;

    // - - - - -reads - - - - -
    // 0-67: 16k slots where direct memory access spaces, ram or rom.
    INT16 i;
    for(i=0;i<STATIC_RAM ; i++)
    {

    }
    // 68-191: 16k slots that uses overrides functions because it's some chip, not ram.
    for(;i<SUBTABLE_BASE ; i++)
    {

    }
    // 192-255: when there is a fonction override for just one adress. "Index2" thing.
    // but then can redirect to previous case.
    for(;i<256 ; i++)
    {

    }
}
UINT32 adressspace_readmaskuse=0;
UINT32 read_maskuse=0;
UINT32 read_offsetuse=0;
UINT32 write_maskuse=0;
UINT32 write_offsetuse=0;
//#define DO_TESTMASKUSE 1
#ifdef DO_TESTMASKUSE

 static inline void testMaskUse(UINT32 mask, UINT32 adr, UINT32 &acc) { acc += (((~mask) & adr)!=0); }

static inline void testEntryShitMaskUse_directreadA( handler_data &hdata, offs_t a) {
    if(hdata.offset != 0) read_offsetuse++;
    testMaskUse(hdata.mask,a,read_maskuse);
}
static inline void testEntryShitMaskUse_directwriteA( handler_data &hdata, offs_t a) {
    if(hdata.offset != 0) write_offsetuse++;
    testMaskUse(hdata.mask,a,write_maskuse);
}

#else
 static inline void testMaskUse(UINT32 a, UINT32 b, UINT32 &acc) { }

static inline void testEntryShitMaskUse_directreadA( handler_data &hdata, offs_t a) {

}
static inline void testEntryShitMaskUse_directwriteA( handler_data &hdata, offs_t a) {

}

#endif



#define DO_ENTRY_STATS 1
#ifdef DO_ENTRY_STATS


//#define DO_TESTENTRYTYPE 1
//handler_data &hdata = active_address_space[0].readhandlers[entry];

//address -= hdata.offset;
//address &= hdata.mask;


#ifdef DO_TESTENTRYTYPE
#define ENTRY_STAT_DirectReadA(a)  entriesDirectReadA[a]++;
#define ENTRY_STAT_DirectReadB(a)  entriesDirectReadB[a]++;
#define ENTRY_STAT_Overread(a)  entriesOverread[a]++;

#define ENTRY_STAT_DirectWritesA(a)  entriesDirectWritesA[a]++;
#define ENTRY_STAT_DirectWritesB(a)  entriesDirectWritesB[a]++;
#define ENTRY_STAT_Overwrite(a)  entriesOverwrites[a]++;
#else
#define ENTRY_STAT_DirectReadA(a)
#define ENTRY_STAT_DirectReadB(a)
#define ENTRY_STAT_Overread(a)

#define ENTRY_STAT_DirectWritesA(a)
#define ENTRY_STAT_DirectWritesB(a)
#define ENTRY_STAT_Overwrite(a)
#endif
#ifdef DO_TESTENTRYTYPE
extern "C" {
    void logEntries();
}

void logEntries()
{

    printf("maskuse:%08x offsetuse:%08x adressspace_readmaskuse:%08x\n",read_maskuse,read_offsetuse,adressspace_readmaskuse);

    printf("  **** READ ****\n");
    for(int ic=0;ic<256;ic++) {
        if(entriesDirectReadA[ic]>0) {
            printf("DirectReadA %03d  %08x\n", ic, entriesDirectReadA[ic]);
        }
    }
    for(int ic=0;ic<256;ic++) {
        if(entriesOverread[ic]>0) {
            printf("*** overread %03d  %08x\n", ic, entriesOverread[ic]);
        }
    }
    for(int ic=0;ic<256;ic++) {
        if(entriesDirectReadB[ic]>0) {
            printf("*** DirectReadB %03d  %08x\n", ic, entriesDirectReadB[ic]);
        }
    }
    printf("  **** WRITE ****\n");
    for(int ic=0;ic<256;ic++) {
        if(entriesDirectWritesA[ic]>0) {
            printf("DirectWriteA %03d  %08x\n", ic, entriesDirectWritesA[ic]);
        }
    }
    for(int ic=0;ic<256;ic++) {
        if(entriesOverwrites[ic]>0) {
            printf("*** overwrite %03d  %08x\n", ic, entriesOverwrites[ic]);
        }
    }
    for(int ic=0;ic<256;ic++) {
        if(entriesDirectWritesB[ic]>0) {
            printf("*** DirectWriteB %03d  %08x\n", ic, entriesDirectWritesB[ic]);
        }
    }

}
#endif

#else
#define ENTRY_STAT_DirectReadA(a)
#define ENTRY_STAT_DirectReadB(a)
#define ENTRY_STAT_Overread(a)

#define ENTRY_STAT_DirectWritesA(a)
#define ENTRY_STAT_DirectWritesB(a)
#define ENTRY_STAT_Overwrite(a)

#endif

class HOffset {
public:
    static inline void applyHandlerOffset(offs_t &address, handler_data &hdata)
    {
        address -= hdata.offset;
        address &= hdata.mask;
    }
};
class HOffsetStat {
public:
    static inline void applyHandlerOffset(offs_t &address, handler_data &hdata)
    {
        address -= hdata.offset;
        testEntryShitMaskUse_directreadA(hdata,address);
       // testMaskUse(hdata.mask,address,readers_readmaskuse);
        address &= hdata.mask;
    }
};
class HOffsetNoMask {
public:
    static inline void applyHandlerOffset(offs_t &address, handler_data &hdata)
    {
        address -= hdata.offset;
    }
};


/* fast16 uses:
	program_read_byte_16be, // program_read_byte_32be,
	program_read_word_16be, // program_read_word_32be,
	memory_readlong_d16, // memory_readlong_d16B, // readlong_d16, //program_read_dword_32be, -> if 32b replaced, no more sound in arkretrn.

	program_write_byte_16be, // program_write_byte_32be,
	program_write_word_16be,//program_write_word_32be,
	memory_writelong_d16, //writelong_d16, //memory_writelong_d16, // writelong_d16,//program_write_dword_32be,
    NULL,
    memory_writemovem32_wr16_reverse,
*/
// - - - - 68k 16b-bus reads




// #define READBYTE(name,spacenum,xormacro,handlertype,ignorebits,shiftbytes,masktype)
template<class HOffst=HOffset>
UINT32 memory_readlong_d16_beT(offs_t address REGM(d0))
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	address &= active_address_space[0].addrmask;

	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadA(entry);


        HOffst::applyHandlerOffset(address,active_address_space[0].readhandlers[entry]);

        #ifdef LSB_FIRST
           UINT16 *p= (UINT16*)&bank_ptr[entry][address];
           return ((UINT32)p[0]<<16) | ((UINT32)p[1]);
        #else
            return *((UINT32 *)&bank_ptr[entry][address]);
        #endif

    }

	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].readlookup[LEVEL2_INDEX(entry,address)];

    HOffst::applyHandlerOffset(address,active_address_space[0].readhandlers[entry]);

	if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadB(entry);
#ifdef LSB_FIRST
       // mame rewrites memory as a vector of words (possibly to mimic a 16bit bus?)
       // so when mame is intel compiled it would invert bytes by 2,
        // but all accessors would continue to treat it as a vector of byte.
       // so to make a UINT32 BE to LE read we just need to inverted bit 2 like this:
       UINT16 *p= (UINT16*)&bank_ptr[entry][address];
       return ((UINT32)p[0]<<16) | ((UINT32)p[1]);
#else
        //... hopefully when mame is 68k "BE" compiled,
        // the original 68k memory is alreday in the right original order .
        return *((UINT32 *)&bank_ptr[entry][address]);
#endif
    }
        ENTRY_STAT_Overread(entry);
    read16_handler reader = active_address_space[0].readhandlers[entry].handler.read.handler16;
    address >>= 1;

    return(
           (((UINT32)(*reader)(address,0))<<16) |
           ((UINT32)(*reader)(address+1,0))
                );

}
UINT32 memory_readlong_d16_be(offs_t address REGM(d0))
{
    UINT32 v = memory_readlong_d16_beT<HOffset>(address);
   // traceMem(address,v);
    return v;
}


UINT16 memory_readword_d16_be(offs_t address REGM(d0))
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadA(entry);
        handler_data &hdata = active_address_space[0].readhandlers[entry];

        address -= hdata.offset;
        address &= hdata.mask;
        // mame already inverts adress bit one in intel mode so for this one,
         // no inversion and same code if mame if 68k compiled or intel compiled.
        return *((UINT16*)&bank_ptr[entry][address]);
    }

	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].readlookup[LEVEL2_INDEX(entry,address)];

    handler_data &hdata = active_address_space[0].readhandlers[entry];
    address -= hdata.offset;
    address &= hdata.mask;

	if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadB(entry);
       // mame already inverts adress bit one in intel mode so for this one,
        // no inversion and same code if mame if 68k compiled or intel compiled.
       return *((UINT16*)&bank_ptr[entry][address]);
    }
        ENTRY_STAT_Overread(entry);
    read16_handler reader = active_address_space[0].readhandlers[entry].handler.read.handler16;
    address >>= 1;

    return( (UINT32)(*reader)(address,0) );

}

// this is a read8 that needs to use handler16 instead of handler 8 !! hence the mask/shift horror.
UINT8 memory_readbyte_d16_be(offs_t address REGM(d0))
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	address &= active_address_space[0].addrmask;

	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadA(entry);
        handler_data &hdata = active_address_space[0].readhandlers[entry];
        address -= hdata.offset;
        address &= hdata.mask;
        #ifdef LSB_FIRST

               // LE to BE need a bit inverted.
               return bank_ptr[entry][address^1];
        #else
                //... hopefully BE is original order
                return bank_ptr[entry][address];
        #endif
    }

	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].readlookup[LEVEL2_INDEX(entry,address)];

    handler_data &hdata = active_address_space[0].readhandlers[entry];
    address -= hdata.offset;
    address &= hdata.mask;

	if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadB(entry);

        #ifdef LSB_FIRST
               // LE to BE need a bit inverted.
               return bank_ptr[entry][address^1];
        #else
                //... hopefully BE is original order
                return bank_ptr[entry][address];
        #endif
    }
        ENTRY_STAT_Overread(entry);
    read16_handler reader = active_address_space[0].readhandlers[entry].handler.read.handler16;
    int shift = (~address & 1)<<3;
    address >>= 1;
    return reader(address,
                  ~((UINT16)0xff << shift)
                  ) >> shift;

}

// UINT8 memory_readbyte_d16_be(offs_t address REGM(d0))
// {
//     UINT8 v = memory_readbyte_d16_beT(address);
//    // traceMem(address,v);
//     return v;
// }

// - - - -  68k 16b-bus writes


void memory_writelong_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) )
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	address &= active_address_space[0].addrmask;

	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
	{
        ENTRY_STAT_DirectWritesA(entry);
        handler_data &hdata = active_address_space[0].writehandlers[entry];
        address -= hdata.offset;
        address &= hdata.mask;

        #ifdef LSB_FIRST
            // direct write
            UINT16 *pwrite = (UINT16 *) &bank_ptr[entry][address];
            pwrite[0] = (UINT16)(data>>16); // already inverted on bit 1, need inversion on bit 2. it's a joke, yes.
            pwrite[1] = (UINT16)data;
        #else
            // direct write
            UINT32 *pwrite = (UINT32 *) &bank_ptr[entry][address];
            *pwrite = data;
        #endif
        return;
    }
	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].writelookup[LEVEL2_INDEX(entry,address)];

    handler_data &hdata = active_address_space[0].writehandlers[entry];
    address -= hdata.offset;
    address &= hdata.mask;

	if (entry >= STATIC_RAM)
	{
        ENTRY_STAT_Overwrite(entry);
    	write16_handler writer = active_address_space[0].writehandlers[entry].handler.write.handler16;
        address>>=1;
        writer(address,data>>16,0);
        writer(address+1,data,0);
        return;
	}
    ENTRY_STAT_DirectWritesB(entry);
#ifdef LSB_FIRST
    // direct write
    UINT16 *pwrite = (UINT16 *) &bank_ptr[entry][address];
    pwrite[0] = (UINT16)(data>>16); // already inverted on bit 1, need inversion on bit 2. it's a joke, yes.
    pwrite[1] = (UINT16)data;
#else
    // direct write
    UINT32 *pwrite = (UINT32 *) &bank_ptr[entry][address];
    *pwrite = data;
#endif
}

void memory_writeword_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) )
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
    if(entry<STATIC_RAM)
    {
        ENTRY_STAT_DirectWritesA(entry);
        handler_data &hdata = active_address_space[0].writehandlers[entry];
        address -= hdata.offset;
        address &= hdata.mask;
        // direct write
        UINT16 *pwrite = (UINT16 *) &bank_ptr[entry][address];
        *pwrite = (UINT16)data; // LE or BE according to compilation target.
        return;
    }


	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].writelookup[LEVEL2_INDEX(entry,address)];

    handler_data &hdata = active_address_space[0].writehandlers[entry];
    address -= hdata.offset;
    address &= hdata.mask;

	if (entry >= STATIC_RAM)
	{
        ENTRY_STAT_Overwrite(entry);
    	write16_handler writer = active_address_space[0].writehandlers[entry].handler.write.handler16;
        address>>=1;
        writer(address,(UINT16)data,0);
        return;
	}
    ENTRY_STAT_DirectWritesB(entry);
    // direct write
    UINT16 *pwrite = (UINT16 *) &bank_ptr[entry][address];
    *pwrite = (UINT16)data; // LE or BE according to compilation target.

}

void memory_writebyte_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) )
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectWritesA(entry);
        handler_data &hdata = active_address_space[0].writehandlers[entry];
        address -= hdata.offset;
        address &= hdata.mask;
        // direct write
        #ifdef LSB_FIRST
            UINT8 *pwrite = (UINT8 *) &bank_ptr[entry][address^1];
        #else
            UINT8 *pwrite = (UINT8 *) &bank_ptr[entry][address];
        #endif
        *pwrite = (UINT8)data; // LE or BE according to compilation target.
        return;
    }


	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].writelookup[LEVEL2_INDEX(entry,address)];

    handler_data &hdata = active_address_space[0].writehandlers[entry];
    address -= hdata.offset;
    address &= hdata.mask;

	if (entry >= STATIC_RAM)
	{
        ENTRY_STAT_Overwrite(entry);
    	write16_handler writer = active_address_space[0].writehandlers[entry].handler.write.handler16;
        int shift = (~address & 1)<<3;
        address>>=1;
        writer(address,(UINT16)data<<shift,~((UINT16)0xff << shift));
        return;
	}
    ENTRY_STAT_DirectWritesB(entry);
    // direct write
#ifdef LSB_FIRST
    UINT8 *pwrite = (UINT8 *) &bank_ptr[entry][address^1];
#else
    UINT8 *pwrite = (UINT8 *) &bank_ptr[entry][address];
#endif
    *pwrite = (UINT8)data; // LE or BE according to compilation target.

}

extern "C" {
UINT32 memory_readmovem32_16SAFE(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) );

}

UINT32 memory_readmovem32_16SAFE(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) )
{
    /*
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = AY;
	uint count = 0;
    uint countadd = 1<<CYC_MOVEM_L;
	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = p68k->mem.read32(ea);
			ea += 4;
			count+=countadd;
		}
        register_list>>=1;
    }
	AY = ea;

	USE_CYCLES(count);
*/

    UINT32 count = 0;
    for(UINT32 i = 0; i < 16; i++)
    {
        if(bits & 1)
        {
            preg[i] = memory_readlong_d16_be(address);
            count++;
            address += 4;
        }
        bits>>=1;
    }
    return count;

}

// return number of bits actually applied
// IS USED
UINT32 memory_readmovem32_wr16(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) )
{
	UINT32 entry;
	/* perform lookup */
	address &= active_address_space[0].addrmask ;
	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].readlookup[LEVEL2_INDEX(entry,address)];
	/* handle banks inline */
	address = (address - active_address_space[0].readhandlers[entry].offset)
            & active_address_space[0].readhandlers[entry].mask;
	if (entry >= STATIC_RAM)
	{
    	read16_handler reader = active_address_space[0].readhandlers[entry].handler.read.handler16;
        UINT16 i = 0;
        UINT32 count = 0;
        address>>=1;
        for(; i < 16; i++)
        {
            if(bits & 1)
            {
                preg[i] = ((*reader)(address,0)<<16)| (*reader)(address+1,0);
                address+=2;
                count++;
            }
            bits>>=1;
        }
        return count;
	}

    // - - - - - - - - -
#ifdef LSB_FIRST
    UINT16 *pread = (UINT16 *) &bank_ptr[entry][address];
#else
    UINT32 *pread = (UINT32 *) &bank_ptr[entry][address];
#endif


    UINT16 i = 0;
    UINT32 count = 0;
	for(; i < 16; i++)
    {
		if(bits & 1)
		{
        #ifdef LSB_FIRST
            UINT32 vh= (UINT32)*pread++;
            UINT32 vl= (UINT32)*pread++;
            preg[i] = (vh<<16)|vl;
        #else
			preg[i] = *pread++;
        #endif
			count++;
		}
        bits>>=1;
    }
    return count;

}
// needed by 68k interface because video drivers would just patch write16.
// THE ONE USED
UINT32 memory_writemovem32_wr16_reverse(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) )
{
    UINT32 entry;
    testMaskUse(active_address_space[0].addrmask,address,adressspace_readmaskuse);
	/* perform lookup */
	address &= active_address_space[0].addrmask ;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].writelookup[LEVEL2_INDEX(entry,address)];
	/* handle banks inline */
    address = (address - active_address_space[0].writehandlers[entry].offset);

    UINT32 entrymask = active_address_space[0].writehandlers[entry].mask ;

	if (entry >= STATIC_RAM)
	{
    	write16_handler writer16 = active_address_space[0].writehandlers[entry].handler.write.handler16;
        UINT16 i = 0;
        UINT32 count = 0;
        address>>=1; // writer16() funcs use adr/2 because adress words.
        entrymask>>=1;
        for(; i < 16; i++)
        {
            if(bits & 1)
            {   // some games minimix, ... need handled 16b writings
                UINT32 reg = preg[15-i];
                 //no lsb case needed here because we explicitly separate 16+16 for32.
                address--;
                address &=entrymask;
                (*writer16)(address,reg,0);
                address--;
                address &=entrymask;
                (*writer16)(address,reg>>16,0);

                count++;
            }
            bits>>=1;
        }
        return count;
	}

    // - - - - - - - - -
    UINT16 i = 0;
    UINT32 count = 0;

    // address
    UINT8 *pwrite = bank_ptr[entry];

	for(; i < 16; i++)
    {
		if(bits & 1)
		{
            UINT32 v = preg[15-i];
            address -= 4;
            address &= entrymask;
#ifdef LSB_FIRST
            UINT16 *pwr16 =  (UINT16*)(pwrite+address);
            pwr16[0]=(UINT16)(v>>16);
            pwr16[1]=(UINT16)(v);
#else

            *((UINT32*)(pwrite+address)) = v;
#endif
			count++;
		}
        bits>>=1;
    }

    return count;
}
// needed by 68k interface because video drivers would just patch write16.
// THE ONE also USED
UINT32 memory_writemovem32_wr16_reverseSAFE(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) )
{
    UINT16 i = 0;
    UINT32 count = 0;

    for(; i < 16; i++)
    {
        if(bits & 1)
        {
            address-=4;
            UINT32 v = preg[15-i];
            memory_writelong_d16_be(address, v );
            count++;
        }
        bits>>=1;
    }

    return count;
}

UINT32 memory_writemovem32_wr32_reverseSAFE(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) )
{
    UINT16 i = 0;
    UINT32 count = 0;
    address &= active_address_space[0].addrmask ;
    if (!(address & 3))
    {
        for(; i < 16; i++)
        {
            if(bits & 1)
            {
                address-=4;
                program_write_dword_32be(address, preg[15-i]);
                count++;
            }
            bits>>=1;
        }

    } else
    {
        for(; i < 16; i++)
        {
            if(bits & 1)
            {
                address-=4;
                UINT32 v = preg[15-i];
                //krb: in the 68k world, pair writing crash, let's economise a test.
                program_write_word_32be(address, v >> 16);
                program_write_word_32be(address + 2, v);
                count++;
            }
            bits>>=1;
        }
    }
    return count;
}

// needed by 68020 interface because video drivers taito_f3 would just patch write32 !!
UINT32 memory_writemovem32_wr32_reverse(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) )
{
    UINT32 entry;
	/* perform lookup */
	address &= active_address_space[0].addrmask ;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].writelookup[LEVEL2_INDEX(entry,address)];
	/* handle banks inline */
	address = (address - active_address_space[0].writehandlers[entry].offset)
            & active_address_space[0].writehandlers[entry].mask;
	if (entry >= STATIC_RAM)
	{
        // arkanoid return has both a 68000 and a 68020 and needs something like that
       // if (!(address & 3))
       // {   // 4 bytes aligned
            //program_write_dword_32be(address, data);
            write32_handler writer = active_address_space[0].writehandlers[entry].handler.write.handler32;
            UINT16 i = 0;
            UINT32 count = 0;
            address>>=2;
            for(; i < 16; i++)
            {
                if(bits & 1)
                {
                    address--;
                    (*writer)(address,preg[15-i],0);
                    count++;
                }
                bits>>=1;
            }
            return count;
        /*} else
        //{
            // 16bit aligned ? we hope.
            write16_handler writer16 = active_address_space[0].writehandlers[entry].handler.write.handler16;
            UINT16 i = 0;
            UINT32 count = 0;
            address>>=1;
            for(; i < 16; i++)
            {
                if(bits & 1)
                {   // some games minimix, ... need handled 16b writings
                    UINT32 reg = preg[15-i];
                    address--;
                    (*writer16)(address,reg,0);
                    address--;
                    (*writer16)(address,reg>>16,0);
                    //(*writer)(address,preg[15-i],0);
                    count++;
                }
                bits>>=1;
            }
            return count;

        } // end 16b aligned
        */
	} // end if need handler.

    // - - - - - - - - -
    UINT32 *pwrite = (UINT32 *) &bank_ptr[entry][address];
    UINT16 i = 0;
    UINT32 count = 0;
	for(; i < 16; i++)
    {
		if(bits & 1)
		{
            pwrite--;
            *pwrite = preg[15-i];
			count++;
		}
        bits>>=1;
    }
    return count;
}

