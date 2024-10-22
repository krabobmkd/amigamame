#ifndef M68KMAME__HEADER
#define M68KMAME__HEADER

/* ======================================================================== */
/* ============================== MAME STUFF ============================== */
/* ======================================================================== */
#include "memory.h"
#include "driver.h"
#include "debugger.h"
#include "m68000.h"

#include "m68kkrbopt.h"
//can't be included or define generate collisions: #include "m68kcpu.h"


/* Configuration switches (see m68kconf.h for explanation) */
#define M68K_SEPARATE_READS         OPT_ON

#define M68K_SIMULATE_PD_WRITES     OPT_ON

#define M68K_EMULATE_INT_ACK        OPT_ON
#define M68K_INT_ACK_CALLBACK(A)

#define M68K_EMULATE_BKPT_ACK       OPT_OFF
#define M68K_BKPT_ACK_CALLBACK()

#define M68K_EMULATE_TRACE          OPT_OFF

#define M68K_EMULATE_RESET          OPT_ON
#define M68K_RESET_CALLBACK()

#define M68K_CMPILD_HAS_CALLBACK     OPT_ON
#define M68K_CMPILD_CALLBACK()

#define M68K_RTE_HAS_CALLBACK       OPT_ON
#define M68K_RTE_CALLBACK()

#define M68K_EMULATE_FC             OPT_OFF
#define M68K_SET_FC_CALLBACK(A)

#define M68K_MONITOR_PC             OPT_SPECIFY_HANDLER
#define M68K_SET_PC_CALLBACK(A)     change_pc(A)

#define M68K_INSTRUCTION_HOOK       OPT_SPECIFY_HANDLER
#define M68K_INSTRUCTION_CALLBACK() CALL_MAME_DEBUG

#define M68K_EMULATE_PREFETCH       OPT_ON

#define M68K_LOG_ENABLE             OPT_OFF
#define M68K_LOG_1010_1111          OPT_OFF
#define M68K_LOG_FILEHANDLE         errorlog

//krb #define M68K_EMULATE_ADDRESS_ERROR  OPT_ON
#define M68K_EMULATE_ADDRESS_ERROR  OPT_OFF

#define M68K_USE_64_BIT             OPT_OFF


#define m68ki_remaining_cycles m68k_ICount


//extern struct m68k_memory_interface m68k_memory_intf;
extern offs_t m68k_encrypted_opcode_start[MAX_CPU];
extern offs_t m68k_encrypted_opcode_end[MAX_CPU];

#define m68k_read_memory_8(address)          (p68k->mem.read8)(address)
#define m68k_read_memory_16(address)         (p68k->mem.read16)(address)
#define m68k_read_memory_32(address)         (p68k->mem.read32)(address)

#define m68k_read_immediate_16(address)      m68kx_read_immediate_16(address)
#define m68k_read_immediate_32(address)      m68kx_read_immediate_32(address)
#define m68k_read_pcrelative_8(address)      m68kx_read_pcrelative_8(p68k,address)
#define m68k_read_pcrelative_16(address)     m68kx_read_pcrelative_16(p68k,address)
#define m68k_read_pcrelative_32(address)     m68kx_read_pcrelative_32(p68k,address)

#define m68k_read_disassembler_16(address)   m68kx_read_immediate_16(address)
#define m68k_read_disassembler_32(address)   m68kx_read_immediate_32(address)

#define m68k_write_memory_8(address, value)  (*p68k->mem.write8)(address, value)
#define m68k_write_memory_16(address, value) (*p68k->mem.write16)(address, value)
#define m68k_write_memory_32(address, value) (*p68k->mem.write32)(address, value)
#define m68k_write_memory_32_pd(address, value) m68kx_write_memory_32_pd(address, value)


INLINE unsigned int m68k_read_immediate_16(unsigned int address REGM(d0));
INLINE unsigned int m68k_read_immediate_32(unsigned int address REGM(d0));
//INLINE unsigned int m68k_read_pcrelative_8(struct m68k_cpu_instance *p68k COREREG,unsigned int address REGM(d0));
//INLINE unsigned int m68k_read_pcrelative_16(struct m68k_cpu_instance *p68k COREREG,unsigned int address REGM(d0));
//INLINE unsigned int m68k_read_pcrelative_32(struct m68k_cpu_instance *p68k COREREG,unsigned int address REGM(d0));
INLINE void m68k_write_memory_32_pd(unsigned int address REGM(d0), unsigned int value REGM(d1));


INLINE unsigned int m68kx_read_immediate_16(unsigned int address REGM(d0))
{
    return cpu_readop16(address);
//#ifdef LSB_FIRST
//	return cpu_readop16((address) /*  ^ m68k_memory_intf.opcode_xor );
//#else
//	return cpu_readop16((address));
//#endif
}

INLINE unsigned int m68kx_read_immediate_32(unsigned int address REGM(d0))
{
#ifdef LSB_FIRST
	return ((m68k_read_immediate_16(address) << 16) | m68k_read_immediate_16((address)+2));
#else
    return (*(uint *)&opcode_base[address & opcode_mask]);
#endif


}


/* Special call to simulate undocumented 68k behavior when move.l with a
 * predecrement destination mode is executed.
 * A real 68k first writes the high word to [address+2], and then writes the
 * low word to [address].
 */
//INLINE void m68kx_write_memory_32_pd(unsigned int address REGM(d0), unsigned int value REGM(d1))
//{
//	(*m68k_memory_intf.write16)(address+2, value>>16);
//	(*m68k_memory_intf.write16)(address, value&0xffff);
//}


void m68k_set_encrypted_opcode_range(int cpunum, offs_t start, offs_t end);


#ifdef A68K0
#define M68K_EMULATE_010            OPT_OFF
#else
/* M68K Variants */
#if HAS_M68008
#define M68K_EMULATE_008            OPT_ON
#else
#define M68K_EMULATE_008            OPT_OFF
#endif

#if HAS_M68010
#define M68K_EMULATE_010            OPT_ON
#else
#define M68K_EMULATE_010            OPT_OFF
#endif

#endif	// A68K0

#ifdef A68K2
#define M68K_EMULATE_EC020          OPT_OFF
#define M68K_EMULATE_020            OPT_OFF
#else
#undef  M68K_EMULATE_010
#define M68K_EMULATE_010            OPT_ON

#if HAS_M68EC020
#define M68K_EMULATE_EC020          OPT_ON
#else
#define M68K_EMULATE_EC020          OPT_OFF
#endif

#if HAS_M68020
#define M68K_EMULATE_020            OPT_ON
#else
#define M68K_EMULATE_020            OPT_OFF
#endif

#endif // A68K2

#if HAS_M68040
#define M68K_EMULATE_040			OPT_ON
#else
#define M68K_EMULATE_040			OPT_OFF
#endif

/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */

#endif /* M68KMAME__HEADER */
