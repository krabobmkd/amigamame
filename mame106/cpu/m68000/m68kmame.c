#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "driver.h"
#include "m68k.h"
#include "m68000.h"
#include "state.h"
#include "m68kkrbopt.h"
#include "m68kcpu.h"
#include "drivertuning.h"
#include <stdio.h>
/* global access */

//struct m68k_memory_interface m68k_memory_intf;
offs_t m68k_encrypted_opcode_start[MAX_CPU];
offs_t m68k_encrypted_opcode_end[MAX_CPU];


void m68k_set_encrypted_opcode_range(int cpunum, offs_t start, offs_t end)
{
// printf("m68k_set_encrypted_opcode_range: cpu:%d start:%d end:%d\n",cpunum,start,end);
	m68k_encrypted_opcode_start[cpunum] = start;
	m68k_encrypted_opcode_end[cpunum] = end;
}

/****************************************************************************
 * 8-bit data memory interface
 ****************************************************************************/

static UINT16 readword_d8(offs_t address REGM(d0))
{
	UINT16 result = program_read_byte_8(address) << 8;
	return result | program_read_byte_8(address + 1);
}

static void writeword_d8(offs_t address REGM(d0), UINT16 data REGM(d1))
{
	program_write_byte_8(address, data >> 8);
	program_write_byte_8(address + 1, data);
}

static UINT32 readlong_d8(offs_t address REGM(d0))
{
	UINT32 result = program_read_byte_8(address) << 24;
	result |= program_read_byte_8(address + 1) << 16;
	result |= program_read_byte_8(address + 2) << 8;
	return result | program_read_byte_8(address + 3);
}

static void writelong_d8(offs_t address REGM(d0), UINT32 data REGM(d1))
{
	program_write_byte_8(address, data >> 24);
	program_write_byte_8(address + 1, data >> 16);
	program_write_byte_8(address + 2, data >> 8);
	program_write_byte_8(address + 3, data);
}

/* interface for 20/22-bit address bus, 8-bit data bus (68008) */
static const struct m68k_memory_interface interface_d8 =
{
	0,
	program_read_byte_8,
	readword_d8,
	readlong_d8,
	program_write_byte_8,
	writeword_d8,
	writelong_d8
};


#ifndef A68K0

/****************************************************************************
 * 16-bit data memory interface
 ****************************************************************************/

UINT32 readlong_d16(offs_t address REGM(d0))
{
	UINT32 result = program_read_word_16be(address) << 16;
	return result | program_read_word_16be(address + 2);
}

void writelong_d16(offs_t address REGM(d0), UINT32 data REGM(d1))
{
	program_write_word_16be(address, data >> 16);
	program_write_word_16be(address + 2, data);
}

/* interface for 24-bit address bus, 16-bit data bus (68000, 68010) */
static const struct m68k_memory_interface interface_d16 =
{
	0,
	program_read_byte_16be,
	program_read_word_16be,
	readlong_d16,
	program_write_byte_16be,
	program_write_word_16be,
	writelong_d16,
    NULL, // changepc
    memory_readmovem32_wr16,
    memory_writemovem32_wr16_reverse
};


#endif // A68K0



// - - - -override to trace calls
//#define DOTRACEMEM 1
#if DOTRACEMEM
#include <stdio.h>
#include <stdlib.h>
FILE *traceFH = NULL;
UINT32 nbt=0;
int mintrace=(746265*32)+25;
UINT32 nbtt=0;
/*

746265 (*32)
+27

dernier bon:
r:ffffdf00 ____04bc

r:ffffe3bc ____de88 faux   12
r:ffffe3be ____deb0 bon    14


last bon:
m68k_op_adda_16_pi
	prev A value: ffffdf00


*/
void openTraceFH()
{
    nbtt++;
    nbt++;
 //   if(nbt==131072*16) exit(1);
    if(traceFH)
    {
        if(nbtt==32) {
        nbtt = 0;
        fprintf(traceFH,"\n");
        }
     return;
    }
    traceFH = fopen("memtrace.txt","wb");
}

UINT32 readlong_d16(offs_t address  REGM(d0));
void writelong_d16(offs_t address  REGM(d0), UINT32 data  REGM(d1));
UINT8 m68kmemtrace_read8(offs_t adr)
{
    UINT8 v = program_read_byte_16be(adr);
    openTraceFH();
//    fprintf(traceFH,"r:%08x ______%02x",adr,(int)v);

    return v;
}

UINT16 m68kmemtrace_read16(offs_t adr)
{
    UINT16 v = program_read_word_16be(adr);
    if(nbt == mintrace)
    {
        printf("?");
    }
    openTraceFH();
//    fprintf(traceFH,"r:%08x ____%04x",adr,(int)v);

    return v;
}

UINT32 m68kmemtrace_read32(offs_t adr)
{
    UINT32 v = readlong_d16(adr);
    openTraceFH();
//    fprintf(traceFH,"r:%08x %08x",adr,(int)v);

    return v;
}
// - - -
void m68kmemtrace_write8(offs_t adr,UINT8 data)
{
    adr = adr & 0x00ffffff;
    program_write_byte_16be(adr,data);
    openTraceFH();
//    fprintf(traceFH,"w:%08x ______%02x",adr,(int)data);

}

void m68kmemtrace_write16(offs_t adr,UINT16 data)
{
    adr = adr & 0x00ffffff;
    program_write_word_16be(adr,data);
    openTraceFH();
//    fprintf(traceFH,"w:%08x ____%04x",adr,(int)data);

}

void m68kmemtrace_write32(offs_t adr,UINT32 data)
{
    adr = adr & 0x00ffffff;
    writelong_d16(adr,data);
    openTraceFH();
//    fprintf(traceFH,"w:%08x %08x",adr,(int)data);

}
struct m68k_memory_interface m68k_memory_tracer_d16 ={
    0,
    m68kmemtrace_read8,
    m68kmemtrace_read16,
    m68kmemtrace_read32,
    m68kmemtrace_write8,
    m68kmemtrace_write16,
    m68kmemtrace_write32,
    NULL,
    memory_readmovem32_wr16,
    memory_writemovem32_wr16_reverse,
};
#endif

/****************************************************************************
 * 32-bit data memory interface
 ****************************************************************************/

#ifndef A68K2

/* potentially misaligned 16-bit reads with a 32-bit data bus (and 24-bit address bus) */
static UINT16 readword_d32(offs_t address REGM(d0))
{
	UINT16 result;

	if (!(address & 1))
		return program_read_word_32be(address);
	result = program_read_byte_32be(address) << 8;
	return result | program_read_byte_32be(address + 1);
}

/* potentially misaligned 16-bit writes with a 32-bit data bus (and 24-bit address bus) */
static void writeword_d32(offs_t address REGM(d0), UINT16 data REGM(d1))
{
	if (!(address & 1))
	{
		program_write_word_32be(address, data);
		return;
	}
	program_write_byte_32be(address, data >> 8);
	program_write_byte_32be(address + 1, data);
}

/* potentially misaligned 32-bit reads with a 32-bit data bus (and 24-bit address bus) */
static UINT32 readlong_d32(offs_t address REGM(d0))
{
	UINT32 result;

	if (!(address & 3))
		return program_read_dword_32be(address);
	else if (!(address & 1))
	{
		result = program_read_word_32be(address) << 16;
		return result | program_read_word_32be(address + 2);
	}
	result = program_read_byte_32be(address) << 24;
	result |= program_read_word_32be(address + 1) << 8;
	return result | program_read_byte_32be(address + 3);
}

/* potentially misaligned 32-bit writes with a 32-bit data bus (and 24-bit address bus) */
static void writelong_d32(offs_t address REGM(d0), UINT32 data REGM(d1))
{
    /* krb
	if (!(address & 3))
	{
		program_write_dword_32be(address, data);
		return;
	}
	else if (!(address & 1))
	{
		program_write_word_32be(address, data >> 16);
		program_write_word_32be(address + 2, data);
		return;
	}
	program_write_byte_32be(address, data >> 24);
	program_write_word_32be(address + 1, data >> 8);
	program_write_byte_32be(address + 3, data);
    */
    if (!(address & 3))
	{
		program_write_dword_32be(address, data);
		return;
	}
	//krb: in the 68k world, pair writing crash, let's economise a test.
	program_write_word_32be(address, data >> 16);
	program_write_word_32be(address + 2, data);

}
extern UINT32 memory_writemovem32_wr32_reverseSAFE(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) );

/* interface for 32-bit data bus (68EC020, 68020) */
static const struct m68k_memory_interface interface_d32 =
{
	WORD_XOR_BE(0),
	program_read_byte_32be,
	readword_d32,
	readlong_d32,
	program_write_byte_32be,
	writeword_d32,
	writelong_d32,
    NULL, // changepc
    memory_readmovem32_wr16,
    memory_writemovem32_wr32_reverseSAFE
};

/* krb */
static const struct m68k_memory_interface interface_xfast16 =
{ // force 32b bus to 68k, ...
	WORD_XOR_BE(0), // 2 if compilation target is LE, 0 if BE. (xor useless)
     memory_readbyte_d16_be, //program_read_byte_16be, // memory_readbyte_d16_be,
	 memory_readword_d16_be, // program_read_word_16be,
	 memory_readlong_d16_be, // readlong_d16
	 memory_writebyte_d16_be, // program_write_byte_16be, //memory_writebyte_d16_be,
	 memory_writeword_d16_be, //program_write_word_16be, //memory_writeword_d16_be,
	 memory_writelong_d16_be, // writelong_d16, // //memory_writelong_d16_be,
    NULL,
    memory_readmovem32_wr16,
    memory_writemovem32_wr16_reverse, // this one can fail when used between 2 incompatible "memory entries", yet memory space is anded, but should use the other entries. Affect neogeo and demonwld
};

/*
    0,
	program_read_byte_16be,
	program_read_word_16be,
	readlong_d16,
	program_write_byte_16be,
	program_write_word_16be,
	writelong_d16,
    NULL, // changepc
    memory_writemovem32_wr16_reverse
*/
/* krb */
static const struct m68k_memory_interface interface_fast32 =
{
	WORD_XOR_BE(0),
	program_read_byte_32be,
	program_read_word_32be,//readword_d32,
	readlong_d32,
	program_write_byte_32be,
	program_write_word_32be, //writeword_d32,
	writelong_d32,
    NULL,
    memory_readmovem32_wr16,
    memory_writemovem32_wr32_reverseSAFE,
};
/* original:
	WORD_XOR_BE(0),
	program_read_byte_32be,
	readword_d32,
	readlong_d32,
	program_write_byte_32be,
	writeword_d32,
	writelong_d32,
    NULL, // changepc
    memory_writemovem32_wr32_reverseSAFE
*/
/* global access */
//struct m68k_memory_interface m68k_memory_intf;

#endif // A68K2


static void set_irq_line(int irqline, int state)
{
	if (irqline == INPUT_LINE_NMI)
		irqline = 7;
	switch(state)
	{
		case CLEAR_LINE:
			m68k_set_irq(0);
			break;
		case ASSERT_LINE:
			m68k_set_irq(irqline);
			break;
		default:
			m68k_set_irq(irqline);
			break;
	}
}


/****************************************************************************
 * 68000 section
 ****************************************************************************/

#ifndef A68K0

static UINT8 m68000_reg_layout[] = {
	M68K_PC, M68K_ISP, -1,
	M68K_SR, M68K_USP, -1,
	M68K_D0, M68K_A0, -1,
	M68K_D1, M68K_A1, -1,
	M68K_D2, M68K_A2, -1,
	M68K_D3, M68K_A3, -1,
	M68K_D4, M68K_A4, -1,
	M68K_D5, M68K_A5, -1,
	M68K_D6, M68K_A6, -1,
	M68K_D7, M68K_A7, 0
};

static UINT8 m68000_win_layout[] = {
	48, 0,32,13,	/* register window (top right) */
	 0, 0,47,13,	/* disassembler window (top left) */
	 0,14,47, 8,	/* memory #1 window (left, middle) */
	48,14,32, 8,	/* memory #2 window (right, middle) */
	 0,23,80, 1 	/* command line window (bottom rows) */
};

extern UINT32 memory_readmovem32_16SAFE(UINT32 address REGM(d0), UINT32 bits REGM(d1), UINT32 *preg REGM(a0) );

static void check_xfast16(struct m68k_memory_interface *pmem)
{
    pmem->writemovem32reverse = &memory_writemovem32_wr16_reverse; // default, fast.
    if(!Machine) return;

    sDriverTuning *ptuning =getDriverTuning(Machine->gamedrv);
    if(ptuning && (ptuning->_flags & MDTF_M68K_SAFE_MOVEMWRITE)!=0)
    {
        pmem->writemovem32reverse = &memory_writemovem32_wr16_reverseSAFE;
    }
    if(ptuning && (ptuning->_flags & MDTF_M68K_SAFE_MOVEMREAD)!=0)
    {
        pmem->readmovem32 = &memory_readmovem32_16SAFE;
    }

}

static void m68000_init(int index, int clock, const void *config, int (*irqcallback)(int))
{
//    printf(" ***** m68000_init\n");
	m68k_init(index);
    struct m68k_cpu_instance *p68k = m68k_getcpu(index);

	m68k_set_cpu_type(p68k,M68K_CPU_TYPE_68000);

//#ifdef OPTIM68K_USEFAST32INTRF
//    p68k->mem = interface_fast16;
//#else
//    p68k->mem = interface_d16;
//#endif
#if DOTRACEMEM
    p68k->mem =   m68k_memory_tracer_d16;
#else
    p68k->mem =   interface_xfast16;
    check_xfast16(&p68k->mem);
#endif



    m68k_state_register(p68k,"m68000", index);
	m68k_set_int_ack_callback(p68k, irqcallback);
}

static void m68000_reset(void)
{
	m68k_pulse_reset();
}

static void m68000_exit(void)
{
	/* nothing to do */
}

//static int m68000_execute(int cycles)
//{
//	return m68k_execute(cycles);
//}

//static void m68000_get_context(void *dst)
//{
//	m68k_get_context(dst);
//}

//static void m68000_set_context(void *src)
//{
////    printf("m68000_set_context\n");
////#ifdef OPTIM68K_USEFAST32INTRF
////    if (m68k_memory_intf.writemovem32reverse != memory_writemovem32_wr16_reverse)
////		m68k_memory_intf = interface_fast16;
////#else
////    if (m68k_memory_intf.read8 != program_read_byte_16be)
////		m68k_memory_intf = interface_d16;
////#endif


//	m68k_set_context(src);
//}

static offs_t m68000_dasm(char *buffer, offs_t pc, UINT8 *oprom, UINT8 *opram, int bytes)
{
	M68K_SET_PC_CALLBACK(pc);
#ifdef MAME_DEBUG
	return m68k_disassemble_raw(buffer, pc, oprom, opram, bytes, M68K_CPU_TYPE_68000);
#else
	sprintf( buffer, "$%04X", m68k_read_immediate_16(pc) );
	return 2;
#endif
}

/****************************************************************************
 * M68008 section
 ****************************************************************************/
#if HAS_M68008

static void m68008_init(int index, int clock, const void *config, int (*irqcallback)(int))
{
	m68k_init(index);
    struct m68k_cpu_instance *p68k = m68k_getcpu(index);

	m68k_set_cpu_type(p68k,M68K_CPU_TYPE_68008);

    p68k->mem = interface_d8;

    m68k_state_register(p68k,"m68008", index);
	m68k_set_int_ack_callback(p68k, irqcallback);
}

static void m68008_reset(void)
{
	m68k_pulse_reset();
}

static void m68008_exit(void)
{
	/* nothing to do */
}

//static int m68008_execute(int cycles)
//{
//	return m68k_execute(cycles);
//}

static void m68008_get_context(void *dst)
{
	m68k_get_context(dst);
}

//static void m68008_set_context(void *src)
//{
////	if (m68k_memory_intf.read8 != program_read_byte_8)
////		m68k_memory_intf = interface_d8;
//	m68k_set_context(src);
//}

static offs_t m68008_dasm(char *buffer, offs_t pc, UINT8 *oprom, UINT8 *opram, int bytes)
{
	M68K_SET_PC_CALLBACK(pc);
#ifdef MAME_DEBUG
	return m68k_disassemble_raw(buffer, pc, oprom, opram, bytes, M68K_CPU_TYPE_68008);
#else
	sprintf( buffer, "$%04X", m68k_read_immediate_16(pc) );
	return 2;
#endif
}

#endif

/****************************************************************************
 * M68010 section
 ****************************************************************************/
#if HAS_M68010

static UINT8 m68010_reg_layout[] = {
	M68K_PC,  M68K_ISP, -1,
	M68K_SR,  M68K_USP, -1,
	M68K_SFC, M68K_VBR, -1,
	M68K_DFC, -1,
	M68K_D0,  M68K_A0, -1,
	M68K_D1,  M68K_A1, -1,
	M68K_D2,  M68K_A2, -1,
	M68K_D3,  M68K_A3, -1,
	M68K_D4,  M68K_A4, -1,
	M68K_D5,  M68K_A5, -1,
	M68K_D6,  M68K_A6, -1,
	M68K_D7,  M68K_A7, 0
};

void m68010_init(int index, int clock, const void *config, int (*irqcallback)(int))
{
//    printf(" ***** m68000_init\n");
	m68k_init(index);
    struct m68k_cpu_instance *p68k = m68k_getcpu(index);

	m68k_set_cpu_type(p68k,M68K_CPU_TYPE_68010);

//#ifdef OPTIM68K_USEFAST32INTRF
//    p68k->mem = interface_fast16;
//#else
//    p68k->mem = interface_d16;
//#endif
    p68k->mem = interface_xfast16;
    check_xfast16(&p68k->mem);

    m68k_state_register(p68k,"m68010", index);
	m68k_set_int_ack_callback(p68k, irqcallback);

}

static offs_t m68010_dasm(char *buffer, offs_t pc, UINT8 *oprom, UINT8 *opram, int bytes)
{
	M68K_SET_PC_CALLBACK(pc);
#ifdef MAME_DEBUG
	return m68k_disassemble_raw(buffer, pc, oprom, opram, bytes, M68K_CPU_TYPE_68010);
#else
	sprintf( buffer, "$%04X", m68k_read_immediate_16(pc) );
	return 2;
#endif
}

#endif /* HAS_M68010 */

#endif // A68K0



#ifndef A68K2

/****************************************************************************
 * M68020 section
 ****************************************************************************/

static UINT8 m68020_reg_layout[] = {
	M68K_PC,  M68K_MSP, -1,
	M68K_SR,  M68K_ISP, -1,
	M68K_SFC, M68K_USP, -1,
	M68K_DFC, M68K_VBR, -1,
	M68K_D0,  M68K_A0, -1,
	M68K_D1,  M68K_A1, -1,
	M68K_D2,  M68K_A2, -1,
	M68K_D3,  M68K_A3, -1,
	M68K_D4,  M68K_A4, -1,
	M68K_D5,  M68K_A5, -1,
	M68K_D6,  M68K_A6, -1,
	M68K_D7,  M68K_A7, 0
};

static UINT8 m68020_win_layout[] = {
	48, 0,32,13,	/* register window (top right) */
	 0, 0,47,13,	/* disassembler window (top left) */
	 0,14,47, 8,	/* memory #1 window (left, middle) */
	48,14,32, 8,	/* memory #2 window (right, middle) */
	 0,23,80, 1 	/* command line window (bottom rows) */
};


static void m68020_init(int index, int clock, const void *config, int (*irqcallback)(int))
{
//    printf(" ***** m68020_init\n");

	m68k_init(index);
    struct m68k_cpu_instance *p68k = m68k_getcpu(index);

	m68k_set_cpu_type(p68k,M68K_CPU_TYPE_68020);

#ifdef OPTIM68K_USEFAST32INTRF
    p68k->mem = interface_fast32;
#else
    p68k->mem = interface_d32;
#endif

	m68k_state_register(p68k,"m68020", index);
	m68k_set_int_ack_callback(p68k,irqcallback);
}

static void m68020_reset(void)
{
	m68k_pulse_reset();
}

static void m68020_exit(void)
{
	/* nothing to do */
}

//static int m68020_execute(int cycles)
//{
//	return m68k_execute(cycles);
//}

static void m68020_get_context(void *dst)
{
	m68k_get_context(dst);
}

//static void m68020_set_context(void *src)
//{
////    printf("m68020_set_context\n");
////#ifdef OPTIM68K_USEFAST32INTRF
////    if (m68k_memory_intf.writemovem32reverse != memory_writemovem32_wr32_reverseSAFE)
////        m68k_memory_intf = interface_fast32;
////#else
////    if (m68k_memory_intf.read8 != program_read_byte_32be)
////		m68k_memory_intf = interface_d32;
////#endif
//	m68k_set_context(src);
//}

static offs_t m68020_dasm(char *buffer, offs_t pc, UINT8 *oprom, UINT8 *opram, int bytes)
{
	M68K_SET_PC_CALLBACK(pc);
#ifdef MAME_DEBUG
	return m68k_disassemble_raw(buffer, pc, oprom, opram, bytes, M68K_CPU_TYPE_68020);
#else
	sprintf( buffer, "$%04X", m68k_read_immediate_16(pc) );
	return 2;
#endif
}


/****************************************************************************
 * M680EC20 section
 ****************************************************************************/

#if HAS_M68EC020

static void m68ec020_init(int index, int clock, const void *config, int (*irqcallback)(int))
{   
//    printf(" **** m68ec020_init\n");
	m68k_init(index);
    struct m68k_cpu_instance *p68k = m68k_getcpu(index);

	m68k_set_cpu_type(p68k,M68K_CPU_TYPE_68EC020);
	p68k->mem = interface_d32;
	m68k_state_register(p68k,"m68ec020", index);
	m68k_set_int_ack_callback(p68k,irqcallback);
}

static offs_t m68ec020_dasm(char *buffer, offs_t pc, UINT8 *oprom, UINT8 *opram, int bytes)
{
	M68K_SET_PC_CALLBACK(pc);
#ifdef MAME_DEBUG
	return m68k_disassemble_raw(buffer, pc, oprom, opram, bytes, M68K_CPU_TYPE_68EC020);
#else
	sprintf( buffer, "$%04X", m68k_read_immediate_16(pc) );
	return 2;
#endif
}
#endif /* HAS_M68EC020 */

#endif // A68K2

#if HAS_M68040

/****************************************************************************
 * M68040 section
 ****************************************************************************/

static UINT8 m68040_reg_layout[] = {
	M68K_PC,  M68K_MSP, -1,
	M68K_SR,  M68K_ISP, -1,
	M68K_SFC, M68K_USP, -1,
	M68K_DFC, M68K_VBR, -1,
	M68K_D0,  M68K_A0, -1,
	M68K_D1,  M68K_A1, -1,
	M68K_D2,  M68K_A2, -1,
	M68K_D3,  M68K_A3, -1,
	M68K_D4,  M68K_A4, -1,
	M68K_D5,  M68K_A5, -1,
	M68K_D6,  M68K_A6, -1,
	M68K_D7,  M68K_A7, 0
};

static UINT8 m68040_win_layout[] = {
	48, 0,32,13,	/* register window (top right) */
	 0, 0,47,13,	/* disassembler window (top left) */
	 0,14,47, 8,	/* memory #1 window (left, middle) */
	48,14,32, 8,	/* memory #2 window (right, middle) */
	 0,23,80, 1 	/* command line window (bottom rows) */
};


static void m68040_init(int index, int clock, const void *config, int (*irqcallback)(int))
{
	m68k_init(index);
    struct m68k_cpu_instance *p68k = m68k_getcpu(index);

	m68k_set_cpu_type(p68k,M68K_CPU_TYPE_68040);
	p68k->mem = interface_d32;
	m68k_state_register(p68k,"m68040", index);
	m68k_set_int_ack_callback(p68k,irqcallback);
}

static void m68040_reset(void)
{
	m68k_pulse_reset();
}

static void m68040_exit(void)
{
	/* nothing to do */
}

//static int m68040_execute(int cycles)
//{
//	return m68k_execute(cycles);
//}

static void m68040_get_context(void *dst)
{
	m68k_get_context(dst);
}

//static void m68040_set_context(void *src)
//{
//	if (m68k_memory_intf.read8 != program_read_byte_32be)
//		m68k_memory_intf = interface_d32;
//	m68k_set_context(src);
//}

static offs_t m68040_dasm(char *buffer, offs_t pc, UINT8 *oprom, UINT8 *opram, int bytes)
{
	M68K_SET_PC_CALLBACK(pc);
#ifdef MAME_DEBUG
	return m68k_disassemble_raw(buffer, pc, oprom, opram, bytes, M68K_CPU_TYPE_68040);
#else
	sprintf( buffer, "$%04X", m68k_read_immediate_16(pc) );
	return 2;
#endif
}

#endif


#ifndef A68K0

/**************************************************************************
 * Generic set_info
 **************************************************************************/

static void m68000_set_info(UINT32 state, union cpuinfo *info)
{
    struct m68k_cpu_instance *p68k = m68k_getActivecpu();
//    printf("m68000_set_info:%d\n",state);
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + 0:			set_irq_line(0, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 1:			set_irq_line(1, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 2:			set_irq_line(2, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 3:			set_irq_line(3, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 4:			set_irq_line(4, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 5:			set_irq_line(5, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 6:			set_irq_line(6, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 7:			set_irq_line(7, info->i);					break;

		case CPUINFO_INT_PC:  						m68k_set_reg(M68K_REG_PC, info->i&0x00ffffff); break;
		case CPUINFO_INT_REGISTER + M68K_PC:  		m68k_set_reg(M68K_REG_PC, info->i);			break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:  		m68k_set_reg(M68K_REG_SP, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_ISP: 		m68k_set_reg(M68K_REG_ISP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_USP: 		m68k_set_reg(M68K_REG_USP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_SR:  		m68k_set_reg(M68K_REG_SR, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D0:  		m68k_set_reg(M68K_REG_D0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D1:  		m68k_set_reg(M68K_REG_D1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D2:  		m68k_set_reg(M68K_REG_D2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D3:  		m68k_set_reg(M68K_REG_D3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D4:  		m68k_set_reg(M68K_REG_D4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D5:  		m68k_set_reg(M68K_REG_D5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D6:  		m68k_set_reg(M68K_REG_D6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D7:  		m68k_set_reg(M68K_REG_D7, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A0:  		m68k_set_reg(M68K_REG_A0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A1:  		m68k_set_reg(M68K_REG_A1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A2:  		m68k_set_reg(M68K_REG_A2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A3:  		m68k_set_reg(M68K_REG_A3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A4:  		m68k_set_reg(M68K_REG_A4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A5:  		m68k_set_reg(M68K_REG_A5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A6:  		m68k_set_reg(M68K_REG_A6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A7:  		m68k_set_reg(M68K_REG_A7, info->i);			break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_ADDR:	m68k_set_reg(M68K_REG_PREF_ADDR, info->i);	break;

		/* --- the following bits of info are set as pointers to data or functions --- */
		case CPUINFO_PTR_M68K_RESET_CALLBACK:		m68k_set_reset_instr_callback(p68k,info->f);		break;
		case CPUINFO_PTR_M68K_CMPILD_CALLBACK:		m68k_set_cmpild_instr_callback(p68k,(void (*)(unsigned int,int))(info->f));		break;
		case CPUINFO_PTR_M68K_RTE_CALLBACK:			m68k_set_rte_instr_callback(p68k,info->f);		break;
	}
}


/**************************************************************************
 * Generic get_info
 **************************************************************************/

void m68000_get_info(UINT32 state, union cpuinfo *info)
{
	int sr;
//    printf("m68000_get_info:%d\n",(int)state);
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = m68k_context_size();		break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 8;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = -1;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = CPU_IS_BE;					break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 1;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 2;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 10;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 4;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 158;							break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM:	info->i = 16;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 24;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO: 		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE + 0:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 1:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 2:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 3:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 4:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 5:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 6:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 7:				info->i = 0;	/* fix me */			break;

		case CPUINFO_INT_PREVIOUSPC:					info->i = m68k_get_reg(NULL, M68K_REG_PPC); break;

		case CPUINFO_INT_PC:							info->i = m68k_get_reg(NULL, M68K_REG_PC)&0x00ffffff; break;
		case CPUINFO_INT_REGISTER + M68K_PC:			info->i = m68k_get_reg(NULL, M68K_REG_PC); break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:			info->i = m68k_get_reg(NULL, M68K_REG_SP); break;
		case CPUINFO_INT_REGISTER + M68K_ISP:			info->i = m68k_get_reg(NULL, M68K_REG_ISP); break;
		case CPUINFO_INT_REGISTER + M68K_USP:			info->i = m68k_get_reg(NULL, M68K_REG_USP); break;
		case CPUINFO_INT_REGISTER + M68K_SR:			info->i = m68k_get_reg(NULL, M68K_REG_SR); break;
		case CPUINFO_INT_REGISTER + M68K_D0:			info->i = m68k_get_reg(NULL, M68K_REG_D0); break;
		case CPUINFO_INT_REGISTER + M68K_D1:			info->i = m68k_get_reg(NULL, M68K_REG_D1); break;
		case CPUINFO_INT_REGISTER + M68K_D2:			info->i = m68k_get_reg(NULL, M68K_REG_D2); break;
		case CPUINFO_INT_REGISTER + M68K_D3:			info->i = m68k_get_reg(NULL, M68K_REG_D3); break;
		case CPUINFO_INT_REGISTER + M68K_D4:			info->i = m68k_get_reg(NULL, M68K_REG_D4); break;
		case CPUINFO_INT_REGISTER + M68K_D5:			info->i = m68k_get_reg(NULL, M68K_REG_D5); break;
		case CPUINFO_INT_REGISTER + M68K_D6:			info->i = m68k_get_reg(NULL, M68K_REG_D6); break;
		case CPUINFO_INT_REGISTER + M68K_D7:			info->i = m68k_get_reg(NULL, M68K_REG_D7); break;
		case CPUINFO_INT_REGISTER + M68K_A0:			info->i = m68k_get_reg(NULL, M68K_REG_A0); break;
		case CPUINFO_INT_REGISTER + M68K_A1:			info->i = m68k_get_reg(NULL, M68K_REG_A1); break;
		case CPUINFO_INT_REGISTER + M68K_A2:			info->i = m68k_get_reg(NULL, M68K_REG_A2); break;
		case CPUINFO_INT_REGISTER + M68K_A3:			info->i = m68k_get_reg(NULL, M68K_REG_A3); break;
		case CPUINFO_INT_REGISTER + M68K_A4:			info->i = m68k_get_reg(NULL, M68K_REG_A4); break;
		case CPUINFO_INT_REGISTER + M68K_A5:			info->i = m68k_get_reg(NULL, M68K_REG_A5); break;
		case CPUINFO_INT_REGISTER + M68K_A6:			info->i = m68k_get_reg(NULL, M68K_REG_A6); break;
		case CPUINFO_INT_REGISTER + M68K_A7:			info->i = m68k_get_reg(NULL, M68K_REG_A7); break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_ADDR:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_ADDR); break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_DATA:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_DATA); break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = m68000_set_info;		break;
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = m68k_get_context;	break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = m68k_set_context;	break;
		case CPUINFO_PTR_INIT:							info->init = m68000_init;				break;
		case CPUINFO_PTR_RESET:							info->reset = m68000_reset;				break;
		case CPUINFO_PTR_EXIT:							info->exit = m68000_exit;				break;
		case CPUINFO_PTR_EXECUTE:						info->execute = m68k_execute;			break;
		case CPUINFO_PTR_BURN:							info->burn = NULL;						break;
		case CPUINFO_PTR_DISASSEMBLE_NEW:				info->disassemble_new = m68000_dasm;	break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &m68k_ICount;			break;
		case CPUINFO_PTR_REGISTER_LAYOUT:				info->p = m68000_reg_layout;			break;
		case CPUINFO_PTR_WINDOW_LAYOUT:					info->p = m68000_win_layout;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s = cpuintrf_temp_str(), "68000"); break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s = cpuintrf_temp_str(), "Motorola 68K"); break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s = cpuintrf_temp_str(), "3.2"); break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s = cpuintrf_temp_str(), __FILE__); break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s = cpuintrf_temp_str(), "Copyright 1999-2000 Karl Stenerud. All rights reserved. (2.1 fixes HJB)"); break;

		case CPUINFO_STR_FLAGS:
			sr = m68k_get_reg(NULL, M68K_REG_SR);
			sprintf(info->s = cpuintrf_temp_str(), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sr & 0x8000 ? 'T':'.',
				sr & 0x4000 ? '?':'.',
				sr & 0x2000 ? 'S':'.',
				sr & 0x1000 ? '?':'.',
				sr & 0x0800 ? '?':'.',
				sr & 0x0400 ? 'I':'.',
				sr & 0x0200 ? 'I':'.',
				sr & 0x0100 ? 'I':'.',
				sr & 0x0080 ? '?':'.',
				sr & 0x0040 ? '?':'.',
				sr & 0x0020 ? '?':'.',
				sr & 0x0010 ? 'X':'.',
				sr & 0x0008 ? 'N':'.',
				sr & 0x0004 ? 'Z':'.',
				sr & 0x0002 ? 'V':'.',
				sr & 0x0001 ? 'C':'.');
			break;

		case CPUINFO_STR_REGISTER + M68K_PC:			sprintf(info->s = cpuintrf_temp_str(), "PC :%08X", m68k_get_reg(NULL, M68K_REG_PC)); break;
		case CPUINFO_STR_REGISTER + M68K_SR:  			sprintf(info->s = cpuintrf_temp_str(), "SR :%04X", m68k_get_reg(NULL, M68K_REG_SR)); break;
		case CPUINFO_STR_REGISTER + M68K_SP:  			sprintf(info->s = cpuintrf_temp_str(), "SP :%08X", m68k_get_reg(NULL, M68K_REG_SP)); break;
		case CPUINFO_STR_REGISTER + M68K_ISP: 			sprintf(info->s = cpuintrf_temp_str(), "ISP:%08X", m68k_get_reg(NULL, M68K_REG_ISP)); break;
		case CPUINFO_STR_REGISTER + M68K_USP: 			sprintf(info->s = cpuintrf_temp_str(), "USP:%08X", m68k_get_reg(NULL, M68K_REG_USP)); break;
		case CPUINFO_STR_REGISTER + M68K_D0:			sprintf(info->s = cpuintrf_temp_str(), "D0 :%08X", m68k_get_reg(NULL, M68K_REG_D0)); break;
		case CPUINFO_STR_REGISTER + M68K_D1:			sprintf(info->s = cpuintrf_temp_str(), "D1 :%08X", m68k_get_reg(NULL, M68K_REG_D1)); break;
		case CPUINFO_STR_REGISTER + M68K_D2:			sprintf(info->s = cpuintrf_temp_str(), "D2 :%08X", m68k_get_reg(NULL, M68K_REG_D2)); break;
		case CPUINFO_STR_REGISTER + M68K_D3:			sprintf(info->s = cpuintrf_temp_str(), "D3 :%08X", m68k_get_reg(NULL, M68K_REG_D3)); break;
		case CPUINFO_STR_REGISTER + M68K_D4:			sprintf(info->s = cpuintrf_temp_str(), "D4 :%08X", m68k_get_reg(NULL, M68K_REG_D4)); break;
		case CPUINFO_STR_REGISTER + M68K_D5:			sprintf(info->s = cpuintrf_temp_str(), "D5 :%08X", m68k_get_reg(NULL, M68K_REG_D5)); break;
		case CPUINFO_STR_REGISTER + M68K_D6:			sprintf(info->s = cpuintrf_temp_str(), "D6 :%08X", m68k_get_reg(NULL, M68K_REG_D6)); break;
		case CPUINFO_STR_REGISTER + M68K_D7:			sprintf(info->s = cpuintrf_temp_str(), "D7 :%08X", m68k_get_reg(NULL, M68K_REG_D7)); break;
		case CPUINFO_STR_REGISTER + M68K_A0:			sprintf(info->s = cpuintrf_temp_str(), "A0 :%08X", m68k_get_reg(NULL, M68K_REG_A0)); break;
		case CPUINFO_STR_REGISTER + M68K_A1:			sprintf(info->s = cpuintrf_temp_str(), "A1 :%08X", m68k_get_reg(NULL, M68K_REG_A1)); break;
		case CPUINFO_STR_REGISTER + M68K_A2:			sprintf(info->s = cpuintrf_temp_str(), "A2 :%08X", m68k_get_reg(NULL, M68K_REG_A2)); break;
		case CPUINFO_STR_REGISTER + M68K_A3:			sprintf(info->s = cpuintrf_temp_str(), "A3 :%08X", m68k_get_reg(NULL, M68K_REG_A3)); break;
		case CPUINFO_STR_REGISTER + M68K_A4:			sprintf(info->s = cpuintrf_temp_str(), "A4 :%08X", m68k_get_reg(NULL, M68K_REG_A4)); break;
		case CPUINFO_STR_REGISTER + M68K_A5:			sprintf(info->s = cpuintrf_temp_str(), "A5 :%08X", m68k_get_reg(NULL, M68K_REG_A5)); break;
		case CPUINFO_STR_REGISTER + M68K_A6:			sprintf(info->s = cpuintrf_temp_str(), "A6 :%08X", m68k_get_reg(NULL, M68K_REG_A6)); break;
		case CPUINFO_STR_REGISTER + M68K_A7:			sprintf(info->s = cpuintrf_temp_str(), "A7 :%08X", m68k_get_reg(NULL, M68K_REG_A7)); break;
//		case CPUINFO_STR_REGISTER + M68K_PREF_ADDR:		sprintf(info->s = cpuintrf_temp_str(), "PAR:%08X", m68k_get_reg(NULL, M68K_REG_PREF_ADDR)); break;
//		case CPUINFO_STR_REGISTER + M68K_PREF_DATA:		sprintf(info->s = cpuintrf_temp_str(), "PDA:%08X", m68k_get_reg(NULL, M68K_REG_PREF_DATA)); break;
	}
}

#if (HAS_M68008)

static void m68008_set_info(UINT32 state, union cpuinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + 0:			set_irq_line(0, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 1:			set_irq_line(1, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 2:			set_irq_line(2, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 3:			set_irq_line(3, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 4:			set_irq_line(4, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 5:			set_irq_line(5, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 6:			set_irq_line(6, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 7:			set_irq_line(7, info->i);					break;

		case CPUINFO_INT_PC:  						m68k_set_reg(M68K_REG_PC, info->i&0x00ffffff); break;
		case CPUINFO_INT_REGISTER + M68K_PC:  		m68k_set_reg(M68K_REG_PC, info->i);			break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:  		m68k_set_reg(M68K_REG_SP, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_ISP: 		m68k_set_reg(M68K_REG_ISP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_USP: 		m68k_set_reg(M68K_REG_USP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_SR:  		m68k_set_reg(M68K_REG_SR, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D0:  		m68k_set_reg(M68K_REG_D0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D1:  		m68k_set_reg(M68K_REG_D1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D2:  		m68k_set_reg(M68K_REG_D2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D3:  		m68k_set_reg(M68K_REG_D3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D4:  		m68k_set_reg(M68K_REG_D4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D5:  		m68k_set_reg(M68K_REG_D5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D6:  		m68k_set_reg(M68K_REG_D6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D7:  		m68k_set_reg(M68K_REG_D7, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A0:  		m68k_set_reg(M68K_REG_A0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A1:  		m68k_set_reg(M68K_REG_A1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A2:  		m68k_set_reg(M68K_REG_A2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A3:  		m68k_set_reg(M68K_REG_A3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A4:  		m68k_set_reg(M68K_REG_A4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A5:  		m68k_set_reg(M68K_REG_A5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A6:  		m68k_set_reg(M68K_REG_A6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A7:  		m68k_set_reg(M68K_REG_A7, info->i);			break;

		/* --- the following bits of info are set as pointers to data or functions --- */
		case CPUINFO_PTR_M68K_RESET_CALLBACK:		m68k_set_reset_instr_callback(info->f);		break;
		case CPUINFO_PTR_M68K_CMPILD_CALLBACK:		m68k_set_cmpild_instr_callback((void (*)(unsigned int,int))(info->f));		break;
		case CPUINFO_PTR_M68K_RTE_CALLBACK:			m68k_set_rte_instr_callback(info->f);		break;
	}
}

void m68008_get_info(UINT32 state, union cpuinfo *info)
{
	int sr;

	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = m68k_context_size();		break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 8;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = -1;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = CPU_IS_BE;					break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 1;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 2;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 10;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 4;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 158;							break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM:	info->i = 8;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 22;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO: 		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE + 0:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 1:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 2:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 3:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 4:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 5:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 6:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 7:				info->i = 0;	/* fix me */			break;

		case CPUINFO_INT_PREVIOUSPC:					info->i = m68k_get_reg(NULL, M68K_REG_PPC); break;

		case CPUINFO_INT_PC:							info->i = m68k_get_reg(NULL, M68K_REG_PC)&0x00ffffff; break;
		case CPUINFO_INT_REGISTER + M68K_PC:			info->i = m68k_get_reg(NULL, M68K_REG_PC); break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:			info->i = m68k_get_reg(NULL, M68K_REG_SP); break;
		case CPUINFO_INT_REGISTER + M68K_ISP:			info->i = m68k_get_reg(NULL, M68K_REG_ISP); break;
		case CPUINFO_INT_REGISTER + M68K_USP:			info->i = m68k_get_reg(NULL, M68K_REG_USP); break;
		case CPUINFO_INT_REGISTER + M68K_SR:			info->i = m68k_get_reg(NULL, M68K_REG_SR); break;
		case CPUINFO_INT_REGISTER + M68K_D0:			info->i = m68k_get_reg(NULL, M68K_REG_D0); break;
		case CPUINFO_INT_REGISTER + M68K_D1:			info->i = m68k_get_reg(NULL, M68K_REG_D1); break;
		case CPUINFO_INT_REGISTER + M68K_D2:			info->i = m68k_get_reg(NULL, M68K_REG_D2); break;
		case CPUINFO_INT_REGISTER + M68K_D3:			info->i = m68k_get_reg(NULL, M68K_REG_D3); break;
		case CPUINFO_INT_REGISTER + M68K_D4:			info->i = m68k_get_reg(NULL, M68K_REG_D4); break;
		case CPUINFO_INT_REGISTER + M68K_D5:			info->i = m68k_get_reg(NULL, M68K_REG_D5); break;
		case CPUINFO_INT_REGISTER + M68K_D6:			info->i = m68k_get_reg(NULL, M68K_REG_D6); break;
		case CPUINFO_INT_REGISTER + M68K_D7:			info->i = m68k_get_reg(NULL, M68K_REG_D7); break;
		case CPUINFO_INT_REGISTER + M68K_A0:			info->i = m68k_get_reg(NULL, M68K_REG_A0); break;
		case CPUINFO_INT_REGISTER + M68K_A1:			info->i = m68k_get_reg(NULL, M68K_REG_A1); break;
		case CPUINFO_INT_REGISTER + M68K_A2:			info->i = m68k_get_reg(NULL, M68K_REG_A2); break;
		case CPUINFO_INT_REGISTER + M68K_A3:			info->i = m68k_get_reg(NULL, M68K_REG_A3); break;
		case CPUINFO_INT_REGISTER + M68K_A4:			info->i = m68k_get_reg(NULL, M68K_REG_A4); break;
		case CPUINFO_INT_REGISTER + M68K_A5:			info->i = m68k_get_reg(NULL, M68K_REG_A5); break;
		case CPUINFO_INT_REGISTER + M68K_A6:			info->i = m68k_get_reg(NULL, M68K_REG_A6); break;
		case CPUINFO_INT_REGISTER + M68K_A7:			info->i = m68k_get_reg(NULL, M68K_REG_A7); break;
		case CPUINFO_INT_REGISTER + M68K_PREF_ADDR:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_ADDR); break;
		case CPUINFO_INT_REGISTER + M68K_PREF_DATA:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_DATA); break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = m68008_set_info;		break;
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = m68008_get_context;	break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = m68k_set_context;	break;
		case CPUINFO_PTR_INIT:							info->init = m68008_init;				break;
		case CPUINFO_PTR_RESET:							info->reset = m68008_reset;				break;
		case CPUINFO_PTR_EXIT:							info->exit = m68008_exit;				break;
		case CPUINFO_PTR_EXECUTE:						info->execute = m68008_execute;			break;
		case CPUINFO_PTR_BURN:							info->burn = NULL;						break;
		case CPUINFO_PTR_DISASSEMBLE_NEW:				info->disassemble_new = m68008_dasm;	break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &m68k_ICount;			break;
		case CPUINFO_PTR_REGISTER_LAYOUT:				info->p = m68000_reg_layout;			break;
		case CPUINFO_PTR_WINDOW_LAYOUT:					info->p = m68000_win_layout;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s = cpuintrf_temp_str(), "68008"); break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s = cpuintrf_temp_str(), "Motorola 68K"); break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s = cpuintrf_temp_str(), "3.2"); break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s = cpuintrf_temp_str(), __FILE__); break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s = cpuintrf_temp_str(), "Copyright 1999-2000 Karl Stenerud. All rights reserved. (2.1 fixes HJB)"); break;

		case CPUINFO_STR_FLAGS:
			sr = m68k_get_reg(NULL, M68K_REG_SR);
			sprintf(info->s = cpuintrf_temp_str(), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sr & 0x8000 ? 'T':'.',
				sr & 0x4000 ? '?':'.',
				sr & 0x2000 ? 'S':'.',
				sr & 0x1000 ? '?':'.',
				sr & 0x0800 ? '?':'.',
				sr & 0x0400 ? 'I':'.',
				sr & 0x0200 ? 'I':'.',
				sr & 0x0100 ? 'I':'.',
				sr & 0x0080 ? '?':'.',
				sr & 0x0040 ? '?':'.',
				sr & 0x0020 ? '?':'.',
				sr & 0x0010 ? 'X':'.',
				sr & 0x0008 ? 'N':'.',
				sr & 0x0004 ? 'Z':'.',
				sr & 0x0002 ? 'V':'.',
				sr & 0x0001 ? 'C':'.');
			break;

		case CPUINFO_STR_REGISTER + M68K_PC:			sprintf(info->s = cpuintrf_temp_str(), "PC :%08X", m68k_get_reg(NULL, M68K_REG_PC)); break;
		case CPUINFO_STR_REGISTER + M68K_SR:  			sprintf(info->s = cpuintrf_temp_str(), "SR :%04X", m68k_get_reg(NULL, M68K_REG_SR)); break;
		case CPUINFO_STR_REGISTER + M68K_SP:  			sprintf(info->s = cpuintrf_temp_str(), "SP :%08X", m68k_get_reg(NULL, M68K_REG_SP)); break;
		case CPUINFO_STR_REGISTER + M68K_ISP: 			sprintf(info->s = cpuintrf_temp_str(), "ISP:%08X", m68k_get_reg(NULL, M68K_REG_ISP)); break;
		case CPUINFO_STR_REGISTER + M68K_USP: 			sprintf(info->s = cpuintrf_temp_str(), "USP:%08X", m68k_get_reg(NULL, M68K_REG_USP)); break;
		case CPUINFO_STR_REGISTER + M68K_D0:			sprintf(info->s = cpuintrf_temp_str(), "D0 :%08X", m68k_get_reg(NULL, M68K_REG_D0)); break;
		case CPUINFO_STR_REGISTER + M68K_D1:			sprintf(info->s = cpuintrf_temp_str(), "D1 :%08X", m68k_get_reg(NULL, M68K_REG_D1)); break;
		case CPUINFO_STR_REGISTER + M68K_D2:			sprintf(info->s = cpuintrf_temp_str(), "D2 :%08X", m68k_get_reg(NULL, M68K_REG_D2)); break;
		case CPUINFO_STR_REGISTER + M68K_D3:			sprintf(info->s = cpuintrf_temp_str(), "D3 :%08X", m68k_get_reg(NULL, M68K_REG_D3)); break;
		case CPUINFO_STR_REGISTER + M68K_D4:			sprintf(info->s = cpuintrf_temp_str(), "D4 :%08X", m68k_get_reg(NULL, M68K_REG_D4)); break;
		case CPUINFO_STR_REGISTER + M68K_D5:			sprintf(info->s = cpuintrf_temp_str(), "D5 :%08X", m68k_get_reg(NULL, M68K_REG_D5)); break;
		case CPUINFO_STR_REGISTER + M68K_D6:			sprintf(info->s = cpuintrf_temp_str(), "D6 :%08X", m68k_get_reg(NULL, M68K_REG_D6)); break;
		case CPUINFO_STR_REGISTER + M68K_D7:			sprintf(info->s = cpuintrf_temp_str(), "D7 :%08X", m68k_get_reg(NULL, M68K_REG_D7)); break;
		case CPUINFO_STR_REGISTER + M68K_A0:			sprintf(info->s = cpuintrf_temp_str(), "A0 :%08X", m68k_get_reg(NULL, M68K_REG_A0)); break;
		case CPUINFO_STR_REGISTER + M68K_A1:			sprintf(info->s = cpuintrf_temp_str(), "A1 :%08X", m68k_get_reg(NULL, M68K_REG_A1)); break;
		case CPUINFO_STR_REGISTER + M68K_A2:			sprintf(info->s = cpuintrf_temp_str(), "A2 :%08X", m68k_get_reg(NULL, M68K_REG_A2)); break;
		case CPUINFO_STR_REGISTER + M68K_A3:			sprintf(info->s = cpuintrf_temp_str(), "A3 :%08X", m68k_get_reg(NULL, M68K_REG_A3)); break;
		case CPUINFO_STR_REGISTER + M68K_A4:			sprintf(info->s = cpuintrf_temp_str(), "A4 :%08X", m68k_get_reg(NULL, M68K_REG_A4)); break;
		case CPUINFO_STR_REGISTER + M68K_A5:			sprintf(info->s = cpuintrf_temp_str(), "A5 :%08X", m68k_get_reg(NULL, M68K_REG_A5)); break;
		case CPUINFO_STR_REGISTER + M68K_A6:			sprintf(info->s = cpuintrf_temp_str(), "A6 :%08X", m68k_get_reg(NULL, M68K_REG_A6)); break;
		case CPUINFO_STR_REGISTER + M68K_A7:			sprintf(info->s = cpuintrf_temp_str(), "A7 :%08X", m68k_get_reg(NULL, M68K_REG_A7)); break;
		case CPUINFO_STR_REGISTER + M68K_PREF_ADDR:		sprintf(info->s = cpuintrf_temp_str(), "PAR:%08X", m68k_get_reg(NULL, M68K_REG_PREF_ADDR)); break;
		case CPUINFO_STR_REGISTER + M68K_PREF_DATA:		sprintf(info->s = cpuintrf_temp_str(), "PDA:%08X", m68k_get_reg(NULL, M68K_REG_PREF_DATA)); break;
	}
}

#endif


#if (HAS_M68010)
/**************************************************************************
 * CPU-specific set_info
 **************************************************************************/

static void m68010_set_info(UINT32 state, union cpuinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_REGISTER + M68K_VBR:  			m68k_set_reg(M68K_REG_VBR, info->i);	break;
		case CPUINFO_INT_REGISTER + M68K_SFC:  			m68k_set_reg(M68K_REG_SFC, info->i);	break;
		case CPUINFO_INT_REGISTER + M68K_DFC:  			m68k_set_reg(M68K_REG_DFC, info->i);	break;

		/* --- the following bits of info are set as pointers to data or functions --- */

		default:
			m68000_set_info(state, info);
			break;
	}
}

void m68010_get_info(UINT32 state, union cpuinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_REGISTER + M68K_VBR:  			info->i = m68k_get_reg(NULL, M68K_REG_VBR); break;
		case CPUINFO_INT_REGISTER + M68K_SFC:  			info->i = m68k_get_reg(NULL, M68K_REG_SFC); break;
		case CPUINFO_INT_REGISTER + M68K_DFC:  			info->i = m68k_get_reg(NULL, M68K_REG_DFC); break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = m68010_set_info;		break;
		case CPUINFO_PTR_INIT:							info->init = m68010_init;				break;
		case CPUINFO_PTR_DISASSEMBLE_NEW:				info->disassemble_new = m68010_dasm;	break;
		case CPUINFO_PTR_REGISTER_LAYOUT:				info->p = m68010_reg_layout;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s = cpuintrf_temp_str(), "68010"); break;
		case CPUINFO_STR_REGISTER + M68K_SFC:			sprintf(info->s = cpuintrf_temp_str(), "SFC:%X",   m68k_get_reg(NULL, M68K_REG_SFC)); break;
		case CPUINFO_STR_REGISTER + M68K_DFC:			sprintf(info->s = cpuintrf_temp_str(), "DFC:%X",   m68k_get_reg(NULL, M68K_REG_DFC)); break;
		case CPUINFO_STR_REGISTER + M68K_VBR:			sprintf(info->s = cpuintrf_temp_str(), "VBR:%08X", m68k_get_reg(NULL, M68K_REG_VBR)); break;

		default:
			m68000_get_info(state, info);
			break;
	}
}
#endif

#endif


#ifndef A68K2

/**************************************************************************
 * CPU-specific set_info
 **************************************************************************/

static void m68020_set_info(UINT32 state, union cpuinfo *info)
{
    struct m68k_cpu_instance *p68k = m68k_getActivecpu();
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + 0:			set_irq_line(0, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 1:			set_irq_line(1, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 2:			set_irq_line(2, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 3:			set_irq_line(3, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 4:			set_irq_line(4, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 5:			set_irq_line(5, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 6:			set_irq_line(6, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 7:			set_irq_line(7, info->i);					break;

		case CPUINFO_INT_PC:  						m68k_set_reg(M68K_REG_PC, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_PC:  		m68k_set_reg(M68K_REG_PC, info->i);			break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:  		m68k_set_reg(M68K_REG_SP, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_ISP: 		m68k_set_reg(M68K_REG_ISP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_USP: 		m68k_set_reg(M68K_REG_USP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_SR:  		m68k_set_reg(M68K_REG_SR, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D0:  		m68k_set_reg(M68K_REG_D0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D1:  		m68k_set_reg(M68K_REG_D1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D2:  		m68k_set_reg(M68K_REG_D2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D3:  		m68k_set_reg(M68K_REG_D3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D4:  		m68k_set_reg(M68K_REG_D4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D5:  		m68k_set_reg(M68K_REG_D5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D6:  		m68k_set_reg(M68K_REG_D6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D7:  		m68k_set_reg(M68K_REG_D7, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A0:  		m68k_set_reg(M68K_REG_A0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A1:  		m68k_set_reg(M68K_REG_A1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A2:  		m68k_set_reg(M68K_REG_A2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A3:  		m68k_set_reg(M68K_REG_A3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A4:  		m68k_set_reg(M68K_REG_A4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A5:  		m68k_set_reg(M68K_REG_A5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A6:  		m68k_set_reg(M68K_REG_A6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A7:  		m68k_set_reg(M68K_REG_A7, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_MSP:		m68k_set_reg(M68K_REG_MSP, info->i);		break; /* 68020+ */
		case CPUINFO_INT_REGISTER + M68K_CACR:		m68k_set_reg(M68K_REG_CACR, info->i);		break; /* 68020+ */
		case CPUINFO_INT_REGISTER + M68K_CAAR:		m68k_set_reg(M68K_REG_CAAR, info->i);		break; /* 68020+ */
		case CPUINFO_INT_REGISTER + M68K_VBR:		m68k_set_reg(M68K_REG_VBR, info->i);		break; /* 68010+ */
		case CPUINFO_INT_REGISTER + M68K_SFC:		m68k_set_reg(M68K_REG_SFC, info->i);		break; /* 68010+ */
		case CPUINFO_INT_REGISTER + M68K_DFC:		m68k_set_reg(M68K_REG_DFC, info->i);		break; /* 68010+ */

		/* --- the following bits of info are set as pointers to data or functions --- */
		case CPUINFO_PTR_M68K_RESET_CALLBACK:		m68k_set_reset_instr_callback(p68k,info->f);		break;
		case CPUINFO_PTR_M68K_CMPILD_CALLBACK:		m68k_set_cmpild_instr_callback(p68k,(void (*)(unsigned int,int))(info->f));		break;
		case CPUINFO_PTR_M68K_RTE_CALLBACK:			m68k_set_rte_instr_callback(p68k,info->f);		break;
	}
}

void m68020_get_info(UINT32 state, union cpuinfo *info)
{
	int sr;

	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = m68k_context_size();		break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 8;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = -1;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = CPU_IS_BE;					break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 1;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 2;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 10;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 4;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 158;							break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM:	info->i = 32;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 32;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO: 		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE + 0:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 1:				info->i = 1;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 2:				info->i = 2;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 3:				info->i = 3;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 4:				info->i = 4;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 5:				info->i = 5;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 6:				info->i = 6;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 7:				info->i = 7;	/* fix me */			break;

		case CPUINFO_INT_PREVIOUSPC:					info->i = m68k_get_reg(NULL, M68K_REG_PPC); break;

		case CPUINFO_INT_PC:							info->i = m68k_get_reg(NULL, M68K_REG_PC); break;
		case CPUINFO_INT_REGISTER + M68K_PC:			info->i = m68k_get_reg(NULL, M68K_REG_PC); break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:			info->i = m68k_get_reg(NULL, M68K_REG_SP); break;
		case CPUINFO_INT_REGISTER + M68K_ISP:			info->i = m68k_get_reg(NULL, M68K_REG_ISP); break;
		case CPUINFO_INT_REGISTER + M68K_USP:			info->i = m68k_get_reg(NULL, M68K_REG_USP); break;
		case CPUINFO_INT_REGISTER + M68K_SR:			info->i = m68k_get_reg(NULL, M68K_REG_SR); break;
		case CPUINFO_INT_REGISTER + M68K_D0:			info->i = m68k_get_reg(NULL, M68K_REG_D0); break;
		case CPUINFO_INT_REGISTER + M68K_D1:			info->i = m68k_get_reg(NULL, M68K_REG_D1); break;
		case CPUINFO_INT_REGISTER + M68K_D2:			info->i = m68k_get_reg(NULL, M68K_REG_D2); break;
		case CPUINFO_INT_REGISTER + M68K_D3:			info->i = m68k_get_reg(NULL, M68K_REG_D3); break;
		case CPUINFO_INT_REGISTER + M68K_D4:			info->i = m68k_get_reg(NULL, M68K_REG_D4); break;
		case CPUINFO_INT_REGISTER + M68K_D5:			info->i = m68k_get_reg(NULL, M68K_REG_D5); break;
		case CPUINFO_INT_REGISTER + M68K_D6:			info->i = m68k_get_reg(NULL, M68K_REG_D6); break;
		case CPUINFO_INT_REGISTER + M68K_D7:			info->i = m68k_get_reg(NULL, M68K_REG_D7); break;
		case CPUINFO_INT_REGISTER + M68K_A0:			info->i = m68k_get_reg(NULL, M68K_REG_A0); break;
		case CPUINFO_INT_REGISTER + M68K_A1:			info->i = m68k_get_reg(NULL, M68K_REG_A1); break;
		case CPUINFO_INT_REGISTER + M68K_A2:			info->i = m68k_get_reg(NULL, M68K_REG_A2); break;
		case CPUINFO_INT_REGISTER + M68K_A3:			info->i = m68k_get_reg(NULL, M68K_REG_A3); break;
		case CPUINFO_INT_REGISTER + M68K_A4:			info->i = m68k_get_reg(NULL, M68K_REG_A4); break;
		case CPUINFO_INT_REGISTER + M68K_A5:			info->i = m68k_get_reg(NULL, M68K_REG_A5); break;
		case CPUINFO_INT_REGISTER + M68K_A6:			info->i = m68k_get_reg(NULL, M68K_REG_A6); break;
		case CPUINFO_INT_REGISTER + M68K_A7:			info->i = m68k_get_reg(NULL, M68K_REG_A7); break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_ADDR:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_ADDR); break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_DATA:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_DATA); break;
		case CPUINFO_INT_REGISTER + M68K_MSP:			info->i = m68k_get_reg(NULL, M68K_REG_MSP); /* 68020+ */ break;
		case CPUINFO_INT_REGISTER + M68K_CACR: 			info->i = m68k_get_reg(NULL, M68K_REG_CACR); /* 68020+ */ break;
		case CPUINFO_INT_REGISTER + M68K_CAAR: 			info->i = m68k_get_reg(NULL, M68K_REG_CAAR); /* 68020+ */ break;
		case CPUINFO_INT_REGISTER + M68K_VBR:			info->i = m68k_get_reg(NULL, M68K_REG_VBR); /* 68010+ */ break;
		case CPUINFO_INT_REGISTER + M68K_SFC:			info->i = m68k_get_reg(NULL, M68K_REG_SFC); /* 68010" */ break;
		case CPUINFO_INT_REGISTER + M68K_DFC:			info->i = m68k_get_reg(NULL, M68K_REG_DFC); /* 68010+ */ break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = m68020_set_info;		break;
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = m68020_get_context;	break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = m68k_set_context;	break;
		case CPUINFO_PTR_INIT:							info->init = m68020_init;				break;
		case CPUINFO_PTR_RESET:							info->reset = m68020_reset;				break;
		case CPUINFO_PTR_EXIT:							info->exit = m68020_exit;				break;
		case CPUINFO_PTR_EXECUTE:						info->execute = m68k_execute;			break;
		case CPUINFO_PTR_BURN:							info->burn = NULL;						break;
		case CPUINFO_PTR_DISASSEMBLE_NEW:				info->disassemble_new = m68020_dasm;	break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &m68k_ICount;			break;
		case CPUINFO_PTR_REGISTER_LAYOUT:				info->p = m68020_reg_layout;			break;
		case CPUINFO_PTR_WINDOW_LAYOUT:					info->p = m68020_win_layout;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s = cpuintrf_temp_str(), "68020"); break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s = cpuintrf_temp_str(), "Motorola 68K"); break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s = cpuintrf_temp_str(), "3.2"); break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s = cpuintrf_temp_str(), __FILE__); break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s = cpuintrf_temp_str(), "Copyright 1999-2000 Karl Stenerud. All rights reserved. (2.1 fixes HJB)"); break;

		case CPUINFO_STR_FLAGS:
			sr = m68k_get_reg(NULL, M68K_REG_SR);
			sprintf(info->s = cpuintrf_temp_str(), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sr & 0x8000 ? 'T':'.',
				sr & 0x4000 ? 't':'.',
				sr & 0x2000 ? 'S':'.',
				sr & 0x1000 ? 'M':'.',
				sr & 0x0800 ? '?':'.',
				sr & 0x0400 ? 'I':'.',
				sr & 0x0200 ? 'I':'.',
				sr & 0x0100 ? 'I':'.',
				sr & 0x0080 ? '?':'.',
				sr & 0x0040 ? '?':'.',
				sr & 0x0020 ? '?':'.',
				sr & 0x0010 ? 'X':'.',
				sr & 0x0008 ? 'N':'.',
				sr & 0x0004 ? 'Z':'.',
				sr & 0x0002 ? 'V':'.',
				sr & 0x0001 ? 'C':'.');
			break;

		case CPUINFO_STR_REGISTER + M68K_PC:			sprintf(info->s = cpuintrf_temp_str(), "PC :%08X", m68k_get_reg(NULL, M68K_REG_PC)); break;
		case CPUINFO_STR_REGISTER + M68K_SR:  			sprintf(info->s = cpuintrf_temp_str(), "SR :%04X", m68k_get_reg(NULL, M68K_REG_SR)); break;
		case CPUINFO_STR_REGISTER + M68K_SP:  			sprintf(info->s = cpuintrf_temp_str(), "SP :%08X", m68k_get_reg(NULL, M68K_REG_SP)); break;
		case CPUINFO_STR_REGISTER + M68K_ISP: 			sprintf(info->s = cpuintrf_temp_str(), "ISP:%08X", m68k_get_reg(NULL, M68K_REG_ISP)); break;
		case CPUINFO_STR_REGISTER + M68K_USP: 			sprintf(info->s = cpuintrf_temp_str(), "USP:%08X", m68k_get_reg(NULL, M68K_REG_USP)); break;
		case CPUINFO_STR_REGISTER + M68K_D0:			sprintf(info->s = cpuintrf_temp_str(), "D0 :%08X", m68k_get_reg(NULL, M68K_REG_D0)); break;
		case CPUINFO_STR_REGISTER + M68K_D1:			sprintf(info->s = cpuintrf_temp_str(), "D1 :%08X", m68k_get_reg(NULL, M68K_REG_D1)); break;
		case CPUINFO_STR_REGISTER + M68K_D2:			sprintf(info->s = cpuintrf_temp_str(), "D2 :%08X", m68k_get_reg(NULL, M68K_REG_D2)); break;
		case CPUINFO_STR_REGISTER + M68K_D3:			sprintf(info->s = cpuintrf_temp_str(), "D3 :%08X", m68k_get_reg(NULL, M68K_REG_D3)); break;
		case CPUINFO_STR_REGISTER + M68K_D4:			sprintf(info->s = cpuintrf_temp_str(), "D4 :%08X", m68k_get_reg(NULL, M68K_REG_D4)); break;
		case CPUINFO_STR_REGISTER + M68K_D5:			sprintf(info->s = cpuintrf_temp_str(), "D5 :%08X", m68k_get_reg(NULL, M68K_REG_D5)); break;
		case CPUINFO_STR_REGISTER + M68K_D6:			sprintf(info->s = cpuintrf_temp_str(), "D6 :%08X", m68k_get_reg(NULL, M68K_REG_D6)); break;
		case CPUINFO_STR_REGISTER + M68K_D7:			sprintf(info->s = cpuintrf_temp_str(), "D7 :%08X", m68k_get_reg(NULL, M68K_REG_D7)); break;
		case CPUINFO_STR_REGISTER + M68K_A0:			sprintf(info->s = cpuintrf_temp_str(), "A0 :%08X", m68k_get_reg(NULL, M68K_REG_A0)); break;
		case CPUINFO_STR_REGISTER + M68K_A1:			sprintf(info->s = cpuintrf_temp_str(), "A1 :%08X", m68k_get_reg(NULL, M68K_REG_A1)); break;
		case CPUINFO_STR_REGISTER + M68K_A2:			sprintf(info->s = cpuintrf_temp_str(), "A2 :%08X", m68k_get_reg(NULL, M68K_REG_A2)); break;
		case CPUINFO_STR_REGISTER + M68K_A3:			sprintf(info->s = cpuintrf_temp_str(), "A3 :%08X", m68k_get_reg(NULL, M68K_REG_A3)); break;
		case CPUINFO_STR_REGISTER + M68K_A4:			sprintf(info->s = cpuintrf_temp_str(), "A4 :%08X", m68k_get_reg(NULL, M68K_REG_A4)); break;
		case CPUINFO_STR_REGISTER + M68K_A5:			sprintf(info->s = cpuintrf_temp_str(), "A5 :%08X", m68k_get_reg(NULL, M68K_REG_A5)); break;
		case CPUINFO_STR_REGISTER + M68K_A6:			sprintf(info->s = cpuintrf_temp_str(), "A6 :%08X", m68k_get_reg(NULL, M68K_REG_A6)); break;
		case CPUINFO_STR_REGISTER + M68K_A7:			sprintf(info->s = cpuintrf_temp_str(), "A7 :%08X", m68k_get_reg(NULL, M68K_REG_A7)); break;
//		case CPUINFO_STR_REGISTER + M68K_PREF_ADDR:		sprintf(info->s = cpuintrf_temp_str(), "PAR:%08X", m68k_get_reg(NULL, M68K_REG_PREF_ADDR)); break;
//		case CPUINFO_STR_REGISTER + M68K_PREF_DATA:		sprintf(info->s = cpuintrf_temp_str(), "PDA:%08X", m68k_get_reg(NULL, M68K_REG_PREF_DATA)); break;
		case CPUINFO_STR_REGISTER + M68K_MSP:			sprintf(info->s = cpuintrf_temp_str(), "MSP:%08X", m68k_get_reg(NULL, M68K_REG_MSP)); break;
		case CPUINFO_STR_REGISTER + M68K_CACR:			sprintf(info->s = cpuintrf_temp_str(), "CCR:%08X", m68k_get_reg(NULL, M68K_REG_CACR)); break;
		case CPUINFO_STR_REGISTER + M68K_CAAR:			sprintf(info->s = cpuintrf_temp_str(), "CAR:%08X", m68k_get_reg(NULL, M68K_REG_CAAR)); break;
		case CPUINFO_STR_REGISTER + M68K_SFC:			sprintf(info->s = cpuintrf_temp_str(), "SFC:%X",   m68k_get_reg(NULL, M68K_REG_SFC)); break;
		case CPUINFO_STR_REGISTER + M68K_DFC:			sprintf(info->s = cpuintrf_temp_str(), "DFC:%X",   m68k_get_reg(NULL, M68K_REG_DFC)); break;
		case CPUINFO_STR_REGISTER + M68K_VBR:			sprintf(info->s = cpuintrf_temp_str(), "VBR:%08X", m68k_get_reg(NULL, M68K_REG_VBR)); break;
	}
}


#if (HAS_M68EC020)
/**************************************************************************
 * CPU-specific set_info
 **************************************************************************/

static void m68ec020_set_info(UINT32 state, union cpuinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_PC:  							m68k_set_reg(M68K_REG_PC, info->i&0x00ffffff); break;

		/* --- the following bits of info are set as pointers to data or functions --- */

		default:
			m68020_set_info(state, info);
			break;
	}
}

void m68ec020_get_info(UINT32 state, union cpuinfo *info)
{
#define THAT_REG_PC  (CPUINFO_INT_PC - CPUINFO_INT_REGISTER)
    if(state == (CPUINFO_INT_REGISTER + THAT_REG_PC))
    {
        info->i = m68k_get_reg(NULL, M68K_REG_PC)&0x00ffffff;
        return;
    }
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 24;					break;
//		case CPUINFO_INT_REGISTER + THAT_REG_PC:				info->i = m68k_get_reg(NULL, M68K_REG_PC)&0x00ffffff; break;


		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = m68ec020_set_info;		break;
		case CPUINFO_PTR_INIT:							info->init = m68ec020_init;				break;
		case CPUINFO_PTR_DISASSEMBLE_NEW:				info->disassemble_new = m68ec020_dasm;	break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s = cpuintrf_temp_str(), "68EC020"); break;

		default:
			m68020_get_info(state, info);
			break;
	}
}
#endif

/**************************************************************************
 * CPU-specific set_info
 **************************************************************************/

static void m68040_set_info(UINT32 state, union cpuinfo *info)
{
    struct m68k_cpu_instance *p68k = m68k_getActivecpu();
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + 0:			set_irq_line(0, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 1:			set_irq_line(1, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 2:			set_irq_line(2, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 3:			set_irq_line(3, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 4:			set_irq_line(4, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 5:			set_irq_line(5, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 6:			set_irq_line(6, info->i);					break;
		case CPUINFO_INT_INPUT_STATE + 7:			set_irq_line(7, info->i);					break;

		case CPUINFO_INT_PC:  						m68k_set_reg(M68K_REG_PC, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_PC:  		m68k_set_reg(M68K_REG_PC, info->i);			break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:  		m68k_set_reg(M68K_REG_SP, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_ISP: 		m68k_set_reg(M68K_REG_ISP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_USP: 		m68k_set_reg(M68K_REG_USP, info->i);		break;
		case CPUINFO_INT_REGISTER + M68K_SR:  		m68k_set_reg(M68K_REG_SR, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D0:  		m68k_set_reg(M68K_REG_D0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D1:  		m68k_set_reg(M68K_REG_D1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D2:  		m68k_set_reg(M68K_REG_D2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D3:  		m68k_set_reg(M68K_REG_D3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D4:  		m68k_set_reg(M68K_REG_D4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D5:  		m68k_set_reg(M68K_REG_D5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D6:  		m68k_set_reg(M68K_REG_D6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_D7:  		m68k_set_reg(M68K_REG_D7, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A0:  		m68k_set_reg(M68K_REG_A0, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A1:  		m68k_set_reg(M68K_REG_A1, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A2:  		m68k_set_reg(M68K_REG_A2, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A3:  		m68k_set_reg(M68K_REG_A3, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A4:  		m68k_set_reg(M68K_REG_A4, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A5:  		m68k_set_reg(M68K_REG_A5, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A6:  		m68k_set_reg(M68K_REG_A6, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_A7:  		m68k_set_reg(M68K_REG_A7, info->i);			break;
		case CPUINFO_INT_REGISTER + M68K_MSP:		m68k_set_reg(M68K_REG_MSP, info->i);		break; /* 68020+ */
		case CPUINFO_INT_REGISTER + M68K_CACR:		m68k_set_reg(M68K_REG_CACR, info->i);		break; /* 68020+ */
		case CPUINFO_INT_REGISTER + M68K_CAAR:		m68k_set_reg(M68K_REG_CAAR, info->i);		break; /* 68020+ */
		case CPUINFO_INT_REGISTER + M68K_VBR:		m68k_set_reg(M68K_REG_VBR, info->i);		break; /* 68010+ */
		case CPUINFO_INT_REGISTER + M68K_SFC:		m68k_set_reg(M68K_REG_SFC, info->i);		break; /* 68010+ */
		case CPUINFO_INT_REGISTER + M68K_DFC:		m68k_set_reg(M68K_REG_DFC, info->i);		break; /* 68010+ */

		/* --- the following bits of info are set as pointers to data or functions --- */
		case CPUINFO_PTR_M68K_RESET_CALLBACK:		m68k_set_reset_instr_callback(p68k,info->f);		break;
		case CPUINFO_PTR_M68K_CMPILD_CALLBACK:		m68k_set_cmpild_instr_callback(p68k,(void (*)(unsigned int,int))(info->f));		break;
		case CPUINFO_PTR_M68K_RTE_CALLBACK:			m68k_set_rte_instr_callback(p68k,info->f);		break;
	}
}

#endif

#if (HAS_M68040)
void m68040_get_info(UINT32 state, union cpuinfo *info)
{
	int sr;

	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = m68k_context_size();		break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 8;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = -1;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = CPU_IS_BE;					break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 1;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 2;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 10;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 4;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 158;							break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM:	info->i = 32;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 32;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO: 		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE + 0:				info->i = 0;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 1:				info->i = 1;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 2:				info->i = 2;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 3:				info->i = 3;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 4:				info->i = 4;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 5:				info->i = 5;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 6:				info->i = 6;	/* fix me */			break;
		case CPUINFO_INT_INPUT_STATE + 7:				info->i = 7;	/* fix me */			break;

		case CPUINFO_INT_PREVIOUSPC:					info->i = m68k_get_reg(NULL, M68K_REG_PPC); break;

		case CPUINFO_INT_PC:							info->i = m68k_get_reg(NULL, M68K_REG_PC); break;
		case CPUINFO_INT_REGISTER + M68K_PC:			info->i = m68k_get_reg(NULL, M68K_REG_PC); break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + M68K_SP:			info->i = m68k_get_reg(NULL, M68K_REG_SP); break;
		case CPUINFO_INT_REGISTER + M68K_ISP:			info->i = m68k_get_reg(NULL, M68K_REG_ISP); break;
		case CPUINFO_INT_REGISTER + M68K_USP:			info->i = m68k_get_reg(NULL, M68K_REG_USP); break;
		case CPUINFO_INT_REGISTER + M68K_SR:			info->i = m68k_get_reg(NULL, M68K_REG_SR); break;
		case CPUINFO_INT_REGISTER + M68K_D0:			info->i = m68k_get_reg(NULL, M68K_REG_D0); break;
		case CPUINFO_INT_REGISTER + M68K_D1:			info->i = m68k_get_reg(NULL, M68K_REG_D1); break;
		case CPUINFO_INT_REGISTER + M68K_D2:			info->i = m68k_get_reg(NULL, M68K_REG_D2); break;
		case CPUINFO_INT_REGISTER + M68K_D3:			info->i = m68k_get_reg(NULL, M68K_REG_D3); break;
		case CPUINFO_INT_REGISTER + M68K_D4:			info->i = m68k_get_reg(NULL, M68K_REG_D4); break;
		case CPUINFO_INT_REGISTER + M68K_D5:			info->i = m68k_get_reg(NULL, M68K_REG_D5); break;
		case CPUINFO_INT_REGISTER + M68K_D6:			info->i = m68k_get_reg(NULL, M68K_REG_D6); break;
		case CPUINFO_INT_REGISTER + M68K_D7:			info->i = m68k_get_reg(NULL, M68K_REG_D7); break;
		case CPUINFO_INT_REGISTER + M68K_A0:			info->i = m68k_get_reg(NULL, M68K_REG_A0); break;
		case CPUINFO_INT_REGISTER + M68K_A1:			info->i = m68k_get_reg(NULL, M68K_REG_A1); break;
		case CPUINFO_INT_REGISTER + M68K_A2:			info->i = m68k_get_reg(NULL, M68K_REG_A2); break;
		case CPUINFO_INT_REGISTER + M68K_A3:			info->i = m68k_get_reg(NULL, M68K_REG_A3); break;
		case CPUINFO_INT_REGISTER + M68K_A4:			info->i = m68k_get_reg(NULL, M68K_REG_A4); break;
		case CPUINFO_INT_REGISTER + M68K_A5:			info->i = m68k_get_reg(NULL, M68K_REG_A5); break;
		case CPUINFO_INT_REGISTER + M68K_A6:			info->i = m68k_get_reg(NULL, M68K_REG_A6); break;
		case CPUINFO_INT_REGISTER + M68K_A7:			info->i = m68k_get_reg(NULL, M68K_REG_A7); break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_ADDR:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_ADDR); break;
//		case CPUINFO_INT_REGISTER + M68K_PREF_DATA:		info->i = m68k_get_reg(NULL, M68K_REG_PREF_DATA); break;
		case CPUINFO_INT_REGISTER + M68K_MSP:			info->i = m68k_get_reg(NULL, M68K_REG_MSP); /* 68020+ */ break;
		case CPUINFO_INT_REGISTER + M68K_CACR: 			info->i = m68k_get_reg(NULL, M68K_REG_CACR); /* 68020+ */ break;
		case CPUINFO_INT_REGISTER + M68K_CAAR: 			info->i = m68k_get_reg(NULL, M68K_REG_CAAR); /* 68020+ */ break;
		case CPUINFO_INT_REGISTER + M68K_VBR:			info->i = m68k_get_reg(NULL, M68K_REG_VBR); /* 68010+ */ break;
		case CPUINFO_INT_REGISTER + M68K_SFC:			info->i = m68k_get_reg(NULL, M68K_REG_SFC); /* 68010" */ break;
		case CPUINFO_INT_REGISTER + M68K_DFC:			info->i = m68k_get_reg(NULL, M68K_REG_DFC); /* 68010+ */ break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = m68040_set_info;		break;
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = m68040_get_context;	break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = m68k_set_context;	break;
		case CPUINFO_PTR_INIT:							info->init = m68040_init;				break;
		case CPUINFO_PTR_RESET:							info->reset = m68040_reset;				break;
		case CPUINFO_PTR_EXIT:							info->exit = m68040_exit;				break;
		case CPUINFO_PTR_EXECUTE:						info->execute = m68k_execute;			break;
		case CPUINFO_PTR_BURN:							info->burn = NULL;						break;
		case CPUINFO_PTR_DISASSEMBLE_NEW:				info->disassemble_new = m68040_dasm;	break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &m68k_ICount;			break;
		case CPUINFO_PTR_REGISTER_LAYOUT:				info->p = m68040_reg_layout;			break;
		case CPUINFO_PTR_WINDOW_LAYOUT:					info->p = m68040_win_layout;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s = cpuintrf_temp_str(), "68040"); break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s = cpuintrf_temp_str(), "Motorola 68K"); break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s = cpuintrf_temp_str(), "3.2"); break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s = cpuintrf_temp_str(), __FILE__); break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s = cpuintrf_temp_str(), "Copyright 1999-2000 Karl Stenerud. All rights reserved. (2.1 fixes HJB)"); break;

		case CPUINFO_STR_FLAGS:
			sr = m68k_get_reg(NULL, M68K_REG_SR);
			sprintf(info->s = cpuintrf_temp_str(), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sr & 0x8000 ? 'T':'.',
				sr & 0x4000 ? 't':'.',
				sr & 0x2000 ? 'S':'.',
				sr & 0x1000 ? 'M':'.',
				sr & 0x0800 ? '?':'.',
				sr & 0x0400 ? 'I':'.',
				sr & 0x0200 ? 'I':'.',
				sr & 0x0100 ? 'I':'.',
				sr & 0x0080 ? '?':'.',
				sr & 0x0040 ? '?':'.',
				sr & 0x0020 ? '?':'.',
				sr & 0x0010 ? 'X':'.',
				sr & 0x0008 ? 'N':'.',
				sr & 0x0004 ? 'Z':'.',
				sr & 0x0002 ? 'V':'.',
				sr & 0x0001 ? 'C':'.');
			break;

		case CPUINFO_STR_REGISTER + M68K_PC:			sprintf(info->s = cpuintrf_temp_str(), "PC :%08X", m68k_get_reg(NULL, M68K_REG_PC)); break;
		case CPUINFO_STR_REGISTER + M68K_SR:  			sprintf(info->s = cpuintrf_temp_str(), "SR :%04X", m68k_get_reg(NULL, M68K_REG_SR)); break;
		case CPUINFO_STR_REGISTER + M68K_SP:  			sprintf(info->s = cpuintrf_temp_str(), "SP :%08X", m68k_get_reg(NULL, M68K_REG_SP)); break;
		case CPUINFO_STR_REGISTER + M68K_ISP: 			sprintf(info->s = cpuintrf_temp_str(), "ISP:%08X", m68k_get_reg(NULL, M68K_REG_ISP)); break;
		case CPUINFO_STR_REGISTER + M68K_USP: 			sprintf(info->s = cpuintrf_temp_str(), "USP:%08X", m68k_get_reg(NULL, M68K_REG_USP)); break;
		case CPUINFO_STR_REGISTER + M68K_D0:			sprintf(info->s = cpuintrf_temp_str(), "D0 :%08X", m68k_get_reg(NULL, M68K_REG_D0)); break;
		case CPUINFO_STR_REGISTER + M68K_D1:			sprintf(info->s = cpuintrf_temp_str(), "D1 :%08X", m68k_get_reg(NULL, M68K_REG_D1)); break;
		case CPUINFO_STR_REGISTER + M68K_D2:			sprintf(info->s = cpuintrf_temp_str(), "D2 :%08X", m68k_get_reg(NULL, M68K_REG_D2)); break;
		case CPUINFO_STR_REGISTER + M68K_D3:			sprintf(info->s = cpuintrf_temp_str(), "D3 :%08X", m68k_get_reg(NULL, M68K_REG_D3)); break;
		case CPUINFO_STR_REGISTER + M68K_D4:			sprintf(info->s = cpuintrf_temp_str(), "D4 :%08X", m68k_get_reg(NULL, M68K_REG_D4)); break;
		case CPUINFO_STR_REGISTER + M68K_D5:			sprintf(info->s = cpuintrf_temp_str(), "D5 :%08X", m68k_get_reg(NULL, M68K_REG_D5)); break;
		case CPUINFO_STR_REGISTER + M68K_D6:			sprintf(info->s = cpuintrf_temp_str(), "D6 :%08X", m68k_get_reg(NULL, M68K_REG_D6)); break;
		case CPUINFO_STR_REGISTER + M68K_D7:			sprintf(info->s = cpuintrf_temp_str(), "D7 :%08X", m68k_get_reg(NULL, M68K_REG_D7)); break;
		case CPUINFO_STR_REGISTER + M68K_A0:			sprintf(info->s = cpuintrf_temp_str(), "A0 :%08X", m68k_get_reg(NULL, M68K_REG_A0)); break;
		case CPUINFO_STR_REGISTER + M68K_A1:			sprintf(info->s = cpuintrf_temp_str(), "A1 :%08X", m68k_get_reg(NULL, M68K_REG_A1)); break;
		case CPUINFO_STR_REGISTER + M68K_A2:			sprintf(info->s = cpuintrf_temp_str(), "A2 :%08X", m68k_get_reg(NULL, M68K_REG_A2)); break;
		case CPUINFO_STR_REGISTER + M68K_A3:			sprintf(info->s = cpuintrf_temp_str(), "A3 :%08X", m68k_get_reg(NULL, M68K_REG_A3)); break;
		case CPUINFO_STR_REGISTER + M68K_A4:			sprintf(info->s = cpuintrf_temp_str(), "A4 :%08X", m68k_get_reg(NULL, M68K_REG_A4)); break;
		case CPUINFO_STR_REGISTER + M68K_A5:			sprintf(info->s = cpuintrf_temp_str(), "A5 :%08X", m68k_get_reg(NULL, M68K_REG_A5)); break;
		case CPUINFO_STR_REGISTER + M68K_A6:			sprintf(info->s = cpuintrf_temp_str(), "A6 :%08X", m68k_get_reg(NULL, M68K_REG_A6)); break;
		case CPUINFO_STR_REGISTER + M68K_A7:			sprintf(info->s = cpuintrf_temp_str(), "A7 :%08X", m68k_get_reg(NULL, M68K_REG_A7)); break;
//		case CPUINFO_STR_REGISTER + M68K_PREF_ADDR:		sprintf(info->s = cpuintrf_temp_str(), "PAR:%08X", m68k_get_reg(NULL, M68K_REG_PREF_ADDR)); break;
//		case CPUINFO_STR_REGISTER + M68K_PREF_DATA:		sprintf(info->s = cpuintrf_temp_str(), "PDA:%08X", m68k_get_reg(NULL, M68K_REG_PREF_DATA)); break;
		case CPUINFO_STR_REGISTER + M68K_MSP:			sprintf(info->s = cpuintrf_temp_str(), "MSP:%08X", m68k_get_reg(NULL, M68K_REG_MSP)); break;
		case CPUINFO_STR_REGISTER + M68K_CACR:			sprintf(info->s = cpuintrf_temp_str(), "CCR:%08X", m68k_get_reg(NULL, M68K_REG_CACR)); break;
		case CPUINFO_STR_REGISTER + M68K_CAAR:			sprintf(info->s = cpuintrf_temp_str(), "CAR:%08X", m68k_get_reg(NULL, M68K_REG_CAAR)); break;
		case CPUINFO_STR_REGISTER + M68K_SFC:			sprintf(info->s = cpuintrf_temp_str(), "SFC:%X",   m68k_get_reg(NULL, M68K_REG_SFC)); break;
		case CPUINFO_STR_REGISTER + M68K_DFC:			sprintf(info->s = cpuintrf_temp_str(), "DFC:%X",   m68k_get_reg(NULL, M68K_REG_DFC)); break;
		case CPUINFO_STR_REGISTER + M68K_VBR:			sprintf(info->s = cpuintrf_temp_str(), "VBR:%08X", m68k_get_reg(NULL, M68K_REG_VBR)); break;
	}
}
#endif

