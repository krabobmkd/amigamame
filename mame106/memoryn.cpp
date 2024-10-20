
extern "C"
{
    #include "memory.h"
    #include <stdio.h>

    extern UINT8 *bank_ptr[STATIC_COUNT];
    extern address_space active_address_space[ADDRESS_SPACES];/* address space data */

}
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
#define DO_ENTRY_STATS 1
#ifdef DO_ENTRY_STATS

UINT32 entriesDirectReadA[256]={0};
UINT32 entriesDirectReadB[256]={0};
UINT32 entriesOverread[256]={0};

UINT32 entriesDirectWritesA[256]={0};
UINT32 entriesDirectWritesB[256]={0};
UINT32 entriesOverwrites[256]={0};

#define ENTRY_STAT_DirectReadA(a)  entriesDirectReadA[a]++;
#define ENTRY_STAT_DirectReadB(a)  entriesDirectReadB[a]++;
#define ENTRY_STAT_Overread(a)  entriesOverread[a]++;

#define ENTRY_STAT_DirectWritesA(a)  entriesDirectWritesA[a]++;
#define ENTRY_STAT_DirectWritesB(a)  entriesDirectWritesB[a]++;
#define ENTRY_STAT_Overwrite(a)  entriesOverwrites[a]++;


extern "C" {
    void logEntries();
}

void logEntries()
{
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


#else
#define ENTRY_STAT_DirectReadA(a)
#define ENTRY_STAT_DirectReadB(a)
#define ENTRY_STAT_Overread(a)

#define ENTRY_STAT_DirectWritesA(a)
#define ENTRY_STAT_DirectWritesB(a)
#define ENTRY_STAT_Overwrite(a)

#endif

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
UINT32 memory_readlong_d16_be(offs_t address REGM(d0))
{
    UINT32 entry;
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadA(entry);

        address = (address - active_address_space[0].readhandlers[entry].offset) &
                active_address_space[0].readhandlers[entry].mask;

        #ifdef LSB_FIRST
           UINT16 *p= (UINT16*)&bank_ptr[entry][address];
           return ((UINT32)p[0]<<16) | ((UINT32)p[1]);
        #else
            return *((UINT32 *)&bank_ptr[entry][address]);
        #endif

    }

	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].readlookup[LEVEL2_INDEX(entry,address)];

	address = (address - active_address_space[0].readhandlers[entry].offset) &
            active_address_space[0].readhandlers[entry].mask;

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
UINT16 memory_readword_d16_be(offs_t address REGM(d0))
{
    UINT32 entry;
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadA(entry);
        address = (address - active_address_space[0].readhandlers[entry].offset) &
                active_address_space[0].readhandlers[entry].mask;
        // mame already inverts adress bit one in intel mode so for this one,
         // no inversion and same code if mame if 68k compiled or intel compiled.
        return *((UINT16*)&bank_ptr[entry][address]);
    }

	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].readlookup[LEVEL2_INDEX(entry,address)];

	address = (address - active_address_space[0].readhandlers[entry].offset) &
            active_address_space[0].readhandlers[entry].mask;

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
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].readlookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectReadA(entry);
        address = (address - active_address_space[0].readhandlers[entry].offset) &
                active_address_space[0].readhandlers[entry].mask;
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

	address = (address - active_address_space[0].readhandlers[entry].offset) &
            active_address_space[0].readhandlers[entry].mask;

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


// - - - -  68k 16b-bus writes
void memory_writelong_d16_be(UINT32 address REGM(d0), UINT32 data REGM(d1) )
{
    UINT32 entry;
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
	{
        ENTRY_STAT_DirectWritesA(entry);
        address = (address - active_address_space[0].writehandlers[entry].offset) &
                active_address_space[0].writehandlers[entry].mask;
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

	address = (address - active_address_space[0].writehandlers[entry].offset) &
            active_address_space[0].writehandlers[entry].mask;

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
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
    if(entry<STATIC_RAM)
    {
        ENTRY_STAT_DirectWritesA(entry);
        address = (address - active_address_space[0].writehandlers[entry].offset) &
                active_address_space[0].writehandlers[entry].mask;
        // direct write
        UINT16 *pwrite = (UINT16 *) &bank_ptr[entry][address];
        *pwrite = (UINT16)data; // LE or BE according to compilation target.
        return;
    }


	if (entry >= SUBTABLE_BASE)
		entry = active_address_space[0].writelookup[LEVEL2_INDEX(entry,address)];

	address = (address - active_address_space[0].writehandlers[entry].offset) &
            active_address_space[0].writehandlers[entry].mask;

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
	address &= active_address_space[0].addrmask;
	entry = active_address_space[0].writelookup[LEVEL1_INDEX(address)];
    if (entry < STATIC_RAM)
    {
        ENTRY_STAT_DirectWritesA(entry);
        address = (address - active_address_space[0].writehandlers[entry].offset) &
                active_address_space[0].writehandlers[entry].mask;
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

	address = (address - active_address_space[0].writehandlers[entry].offset) &
            active_address_space[0].writehandlers[entry].mask;

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

