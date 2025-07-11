#include "m68kcpu.h"
#include "memory.h"
#include "m68kkrbopt.h"
extern void m68040_fpu_op0(M68KOPT_PARAMS);
extern void m68040_fpu_op1(M68KOPT_PARAMS);



/* ======================================================================== */
/* ========================= INSTRUCTION HANDLERS ========================= */
/* ======================================================================== */


void m68k_op_dbt_16(M68KOPT_PARAMS)
{
	REG_PC += 2;
}

void m68k_op_dbf_16(M68KOPT_PARAMS)
{

#ifdef LSB_FIRST
	uint* r_dst = &DY;
	uint res = MASK_OUT_ABOVE_16(*r_dst - 1);
	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
#else
    uint16 * r_dst = ((uint16 *)&DY)+1; //krb:  just shift to get value, no mask needed.
    uint16 res = *r_dst -1;
    *r_dst = res;
#endif
    if(res != 0xffff)
    {
		uint offset = OPER_I_16(p68k);
		REG_PC -= 2;
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_16(p68k, offset);
		USE_CYCLES(CYC_DBCC_F_NOEXP);
		return;
	}
	REG_PC += 2;
	USE_CYCLES(CYC_DBCC_F_EXP);
}

void krb_gforce2_m68k_op_dbf_16(M68KOPT_PARAMS)
{
    regir = 0x51c9;
#ifdef LSB_FIRST
	uint* r_dst = &DY;
	uint res = 0xffff; // MASK_OUT_ABOVE_16(*r_dst - 1);
	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
#else
    uint16 * r_dst = ((uint16 *)&DY)+1; //krb:  just shift to get value, no mask needed.
    uint16 res =  0xffff;
    *r_dst = res;
#endif
    SET_CYCLES(0);

	REG_PC += 2;
	USE_CYCLES(CYC_DBCC_F_EXP);
}


void m68k_op_dbhi_16(M68KOPT_PARAMS)
{
	if(COND_NOT_HI())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbls_16(M68KOPT_PARAMS)
{
	if(COND_NOT_LS())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbcc_16(M68KOPT_PARAMS)
{
	if(COND_NOT_CC())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbcs_16(M68KOPT_PARAMS)
{
	if(COND_NOT_CS())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbne_16(M68KOPT_PARAMS)
{
	if(COND_NOT_NE())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbeq_16(M68KOPT_PARAMS)
{
	if(COND_NOT_EQ())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbvc_16(M68KOPT_PARAMS)
{
	if(COND_NOT_VC())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbvs_16(M68KOPT_PARAMS)
{
	if(COND_NOT_VS())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbpl_16(M68KOPT_PARAMS)
{
	if(COND_NOT_PL())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbmi_16(M68KOPT_PARAMS)
{
	if(COND_NOT_MI())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbge_16(M68KOPT_PARAMS)
{
	if(COND_NOT_GE())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dblt_16(M68KOPT_PARAMS)
{
	if(COND_NOT_LT())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dbgt_16(M68KOPT_PARAMS)
{
	if(COND_NOT_GT())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


void m68k_op_dble_16(M68KOPT_PARAMS)
{
	if(COND_NOT_LE())
	{
		uint* r_dst = &DY;
		uint res = MASK_OUT_ABOVE_16(*r_dst - 1);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		if(res != 0xffff)
		{
			uint offset = OPER_I_16(p68k);
			REG_PC -= 2;
			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_16(p68k, offset);
			USE_CYCLES(CYC_DBCC_F_NOEXP);
			return;
		}
		REG_PC += 2;
		USE_CYCLES(CYC_DBCC_F_EXP);
		return;
	}
	REG_PC += 2;
}


#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
static inline void m68k_op_divs_dx_asm(M68KOPT_PARAMS, register uint src __asm("d0") )
{
    uint *pDX = (uint  *)&DX;
    uint dst = *pDX;
//    if(src==0) m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);

    asm volatile( "divs.w %1,%0\n\tmove.w ccr,%1" : "+d" (dst), "+d" (src) : : );
    *pDX = dst; // have remain<<16|dividend

    FLAG_Z = dst & 0x0000ffff;
    FLAG_C = CFLAG_CLEAR;

    // correct CCR is in src now.
    asm volatile(
        "lsl.l #4,%0\n"
        "\tmove.l %0,%c[n_flag](%1)\n"
        "\tlsl.l #2,%0\n"
        "\tmove.l %0,%c[v_flag](%1)"
       :
       : "d"(src),"a"(p68k), // in
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [v_flag] "n" (offsetof(struct m68ki_cpu_core, v_flag))
       :
       );

}

static inline void m68k_op_divu_dx_asm(M68KOPT_PARAMS, register uint src __asm("d0") )
{
    uint *pDX = (uint  *)&DX;
    uint dst = *pDX;
//    if(src==0) m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);

    asm volatile( "divu.w %1,%0\n\tmove.w ccr,%1" : "+d" (dst), "+d" (src) : : );
    *pDX = dst;
    FLAG_Z = dst & 0x0000ffff;
    FLAG_C = CFLAG_CLEAR;
    // correct CCR is in src now.
    asm volatile(
        "lsl.l #6,%0\n"
        "\tmove.l %0,%c[v_flag](%1)"
       :
       : "d"(src),"a"(p68k), // in
         [v_flag] "n" (offsetof(struct m68ki_cpu_core, v_flag))
       :
       );


}
#endif

void m68k_op_divs_16_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
    m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,DY);
#else

	uint* r_dst = &DX;
	sint src = MAKE_INT_16(DY);
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}

void m68k_op_divs_16_ai(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AY_AI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AY_AI_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_pi(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AY_PI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AY_PI_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_pd(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AY_PD_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AY_PD_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_di(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AY_DI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AY_DI_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_ix(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AY_IX_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AY_IX_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_aw(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AW_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AW_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_al(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_AL_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_AL_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_pcdi(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_PCDI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_PCDI_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_pcix(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_PCIX_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_PCIX_16(M68KOPT_PASSPARAMS));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divs_16_i(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divs_dx_asm(M68KOPT_PASSPARAMS,OPER_I_16(p68k));
#else
	uint* r_dst = &DX;
	sint src = MAKE_INT_16(OPER_I_16(p68k));
	sint quotient;
	sint remainder;

	if(src != 0)
	{
		if((uint32)*r_dst == 0x80000000 && src == -1)
		{
			FLAG_Z = 0;
			FLAG_N = NFLAG_CLEAR;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = 0;
			return;
		}

		quotient = MAKE_INT_32(*r_dst) / src;
		remainder = MAKE_INT_32(*r_dst) % src;

		if(quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}
void m68k_op_divu_16_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS,DY);
#else

	uint* r_dst = &DX;
	uint src = MASK_OUT_ABOVE_16(DY);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_ai(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AY_AI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_AI_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_pi(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AY_PI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_PI_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_pd(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AY_PD_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_PD_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_di(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AY_DI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_DI_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_ix(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AY_IX_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_IX_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_aw(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AW_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AW_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_al(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_AL_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AL_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_pcdi(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_PCDI_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_PCDI_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_pcix(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_PCIX_16(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_PCIX_16(M68KOPT_PASSPARAMS);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divu_16_i(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_DIVS
       m68k_op_divu_dx_asm(M68KOPT_PASSPARAMS, OPER_I_16(p68k));
#else
	uint* r_dst = &DX;
	uint src = OPER_I_16(p68k);

	if(src != 0)
	{
		uint quotient = *r_dst / src;
		uint remainder = *r_dst % src;

		if(quotient < 0x10000)
		{
			FLAG_Z = quotient;
			FLAG_N = NFLAG_16(quotient);
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			*r_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			return;
		}
		FLAG_V = VFLAG_SET;
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
#endif
}


void m68k_op_divl_32_d(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor   = DY;
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = DY;
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_ai(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_pi(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_pd(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_di(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_ix(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_aw(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AW_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AW_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_al(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_AL_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_AL_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_pcdi(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_PCDI_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_PCDI_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_pcix(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_PCIX_32(M68KOPT_PASSPARAMS);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_PCIX_32(M68KOPT_PASSPARAMS);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_divl_32_i(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 divisor = OPER_I_32(p68k);
		uint64 dividend  = 0;
		uint64 quotient  = 0;
		uint64 remainder = 0;

		if(divisor != 0)
		{
			if(BIT_A(word2))	/* 64 bit */
			{
				dividend = REG_D[word2 & 7];
				dividend <<= 32;
				dividend |= REG_D[(word2 >> 12) & 7];

				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)dividend / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)dividend % (sint64)((sint32)divisor));
					if((sint64)quotient != (sint64)((sint32)quotient))
					{
						FLAG_V = VFLAG_SET;
						return;
					}
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					if(quotient > 0xffffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					remainder = dividend % divisor;
				}
			}
			else	/* 32 bit */
			{
				dividend = REG_D[(word2 >> 12) & 7];
				if(BIT_B(word2))	   /* signed */
				{
					quotient  = (uint64)((sint64)((sint32)dividend) / (sint64)((sint32)divisor));
					remainder = (uint64)((sint64)((sint32)dividend) % (sint64)((sint32)divisor));
				}
				else					/* unsigned */
				{
					quotient = dividend / divisor;
					remainder = dividend % divisor;
				}
			}

			REG_D[word2 & 7] = remainder;
			REG_D[(word2 >> 12) & 7] = quotient;

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint divisor = OPER_I_32(p68k);
		uint dividend_hi = REG_D[word2 & 7];
		uint dividend_lo = REG_D[(word2 >> 12) & 7];
		uint quotient = 0;
		uint remainder = 0;
		uint dividend_neg = 0;
		uint divisor_neg = 0;
		sint i;
		uint overflow;

		if(divisor != 0)
		{
			/* quad / long : long quotient, long remainder */
			if(BIT_A(word2))
			{
				if(BIT_B(word2))	   /* signed */
				{
					/* special case in signed divide */
					if(dividend_hi == 0 && dividend_lo == 0x80000000 && divisor == 0xffffffff)
					{
						REG_D[word2 & 7] = 0;
						REG_D[(word2 >> 12) & 7] = 0x80000000;

						FLAG_N = NFLAG_SET;
						FLAG_Z = ZFLAG_CLEAR;
						FLAG_V = VFLAG_CLEAR;
						FLAG_C = CFLAG_CLEAR;
						return;
					}
					if(GET_MSB_32(dividend_hi))
					{
						dividend_neg = 1;
						dividend_hi = (uint)MASK_OUT_ABOVE_32((-(sint)dividend_hi) - (dividend_lo != 0));
						dividend_lo = (uint)MASK_OUT_ABOVE_32(-(sint)dividend_lo);
					}
					if(GET_MSB_32(divisor))
					{
						divisor_neg = 1;
						divisor = (uint)MASK_OUT_ABOVE_32(-(sint)divisor);

					}
				}

				/* if the upper long is greater than the divisor, we're overflowing. */
				if(dividend_hi >= divisor)
				{
					FLAG_V = VFLAG_SET;
					return;
				}

				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					remainder = (remainder << 1) + ((dividend_hi >> i) & 1);
					if(remainder >= divisor)
					{
						remainder -= divisor;
						quotient++;
					}
				}
				for(i = 31; i >= 0; i--)
				{
					quotient <<= 1;
					overflow = GET_MSB_32(remainder);
					remainder = (remainder << 1) + ((dividend_lo >> i) & 1);
					if(remainder >= divisor || overflow)
					{
						remainder -= divisor;
						quotient++;
					}
				}

				if(BIT_B(word2))	   /* signed */
				{
					if(quotient > 0x7fffffff)
					{
						FLAG_V = VFLAG_SET;
						return;
					}
					if(dividend_neg)
					{
						remainder = (uint)MASK_OUT_ABOVE_32(-(sint)remainder);
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
					}
					if(divisor_neg)
						quotient = (uint)MASK_OUT_ABOVE_32(-(sint)quotient);
				}

				REG_D[word2 & 7] = remainder;
				REG_D[(word2 >> 12) & 7] = quotient;

				FLAG_N = NFLAG_32(quotient);
				FLAG_Z = quotient;
				FLAG_V = VFLAG_CLEAR;
				FLAG_C = CFLAG_CLEAR;
				return;
			}

			/* long / long: long quotient, maybe long remainder */
			if(BIT_B(word2))	   /* signed */
			{
				/* Special case in divide */
				if(dividend_lo == 0x80000000 && divisor == 0xffffffff)
				{
					FLAG_N = NFLAG_SET;
					FLAG_Z = ZFLAG_CLEAR;
					FLAG_V = VFLAG_CLEAR;
					FLAG_C = CFLAG_CLEAR;
					REG_D[(word2 >> 12) & 7] = 0x80000000;
					REG_D[word2 & 7] = 0;
					return;
				}
				REG_D[word2 & 7] = MAKE_INT_32(dividend_lo) % MAKE_INT_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MAKE_INT_32(dividend_lo) / MAKE_INT_32(divisor);
			}
			else
			{
				REG_D[word2 & 7] = MASK_OUT_ABOVE_32(dividend_lo) % MASK_OUT_ABOVE_32(divisor);
				quotient = REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(dividend_lo) / MASK_OUT_ABOVE_32(divisor);
			}

			FLAG_N = NFLAG_32(quotient);
			FLAG_Z = quotient;
			FLAG_V = VFLAG_CLEAR;
			FLAG_C = CFLAG_CLEAR;
			return;
		}
		m68ki_exception_trap(p68k, EXCEPTION_ZERO_DIVIDE);
		return;
	}
	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_eor_8_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY ^= MASK_OUT_ABOVE_8(DX));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_8_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint res = MASK_OUT_ABOVE_8(DX ^ p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY ^= MASK_OUT_ABOVE_16(DX));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint res = MASK_OUT_ABOVE_16(DX ^ p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_d(M68KOPT_PARAMS)
{
	uint res = DY ^= DX;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eor_32_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();
	uint res = DX ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY ^= OPER_I_8(M68KOPT_PASSPARAMS));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AY_AI_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AY_PI_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_pi7(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AY_PD_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_pd7(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AY_DI_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AY_IX_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_8_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();
	uint res = src ^ p68k->mem.read8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY ^= OPER_I_16(p68k));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_AI_16();
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_PI_16(p68k,regir);
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_PD_16();
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_DI_16();
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_IX_16();
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AW_16();
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AL_16();
	uint res = src ^ p68k->mem.read16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_d(M68KOPT_PARAMS)
{
	uint res = DY ^= OPER_I_32(p68k);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_AI_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_PI_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_PD_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_DI_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_IX_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AW_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_32_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AL_32();
	uint res = src ^ p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_eori_16_toc(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, m68ki_get_ccr() ^ OPER_I_16(p68k));
}


void m68k_op_eori_16_tos(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint src = OPER_I_16(p68k);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, m68ki_get_sr() ^ src);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_exg_32_dd(M68KOPT_PARAMS)
{
	uint* reg_a = &DX;
	uint* reg_b = &DY;
	uint tmp = *reg_a;
	*reg_a = *reg_b;
	*reg_b = tmp;
}


void m68k_op_exg_32_aa(M68KOPT_PARAMS)
{
	uint* reg_a = &AX;
	uint* reg_b = &AY;
	uint tmp = *reg_a;
	*reg_a = *reg_b;
	*reg_b = tmp;
}


void m68k_op_exg_32_da(M68KOPT_PARAMS)
{
	uint* reg_a = &DX;
	uint* reg_b = &AY;
	uint tmp = *reg_a;
	*reg_a = *reg_b;
	*reg_b = tmp;
}


void m68k_op_ext_16(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | MASK_OUT_ABOVE_8(*r_dst) | (GET_MSB_8(*r_dst) ? 0xff00 : 0);

	FLAG_N = NFLAG_16(*r_dst);
	FLAG_Z = MASK_OUT_ABOVE_16(*r_dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_ext_32(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;

	*r_dst = MASK_OUT_ABOVE_16(*r_dst) | (GET_MSB_16(*r_dst) ? 0xffff0000 : 0);

	FLAG_N = NFLAG_32(*r_dst);
	FLAG_Z = *r_dst;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_extb_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint* r_dst = &DY;

		*r_dst = MASK_OUT_ABOVE_8(*r_dst) | (GET_MSB_8(*r_dst) ? 0xffffff00 : 0);

		FLAG_N = NFLAG_32(*r_dst);
		FLAG_Z = *r_dst;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_illegal(M68KOPT_PARAMS)
{
	m68ki_exception_illegal(p68k);
}


void m68k_op_jmp_32_ai(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_AY_AI_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
    if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jmp_32_di(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_AY_DI_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
    if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jmp_32_ix(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_AY_IX_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
    if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jmp_32_aw(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_AW_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
    if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jmp_32_al(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_AL_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
	if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jmp_32_pcdi(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_PCDI_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
    if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jmp_32_pcix(M68KOPT_PARAMS)
{
	m68ki_jump(p68k, EA_PCIX_32());
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
#ifndef OPTIM68K_SQUEEZEPPCREG
	if(REG_PC == REG_PPC)
		USE_ALL_CYCLES();
#endif
}


void m68k_op_jsr_32_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_jsr_32_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_jsr_32_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_jsr_32_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_jsr_32_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_jsr_32_pcdi(M68KOPT_PARAMS)
{
	uint ea = EA_PCDI_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_jsr_32_pcix(M68KOPT_PARAMS)
{
	uint ea = EA_PCIX_32();
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(p68k, REG_PC);
	m68ki_jump(p68k, ea);
}


void m68k_op_lea_32_ai(M68KOPT_PARAMS)
{
	AX = EA_AY_AI_32();
}


void m68k_op_lea_32_di(M68KOPT_PARAMS)
{
	AX = EA_AY_DI_32();
}


void m68k_op_lea_32_ix(M68KOPT_PARAMS)
{
	AX = EA_AY_IX_32();
}


void m68k_op_lea_32_aw(M68KOPT_PARAMS)
{
	AX = EA_AW_32();
}


void m68k_op_lea_32_al(M68KOPT_PARAMS)
{
	AX = EA_AL_32();
}


void m68k_op_lea_32_pcdi(M68KOPT_PARAMS)
{
	AX = EA_PCDI_32();
}


void m68k_op_lea_32_pcix(M68KOPT_PARAMS)
{
	AX = EA_PCIX_32();
}


void m68k_op_link_16_a7(M68KOPT_PARAMS)
{
	REG_A[7] -= 4;
	m68ki_write_32(REG_A[7], REG_A[7]);
	REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + MAKE_INT_16(OPER_I_16(p68k)));
}


void m68k_op_link_16(M68KOPT_PARAMS)
{
	uint* r_dst = &AY;

	m68ki_push_32(p68k, *r_dst);
	*r_dst = REG_A[7];
	REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + MAKE_INT_16(OPER_I_16(p68k)));
}


void m68k_op_link_32_a7(M68KOPT_PARAMS)
{
	//if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		REG_A[7] -= 4;
		m68ki_write_32(REG_A[7], REG_A[7]);
		REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + OPER_I_32(p68k));
		return;
	}
	//m68ki_exception_illegal(p68k);
}


void m68k_op_link_32(M68KOPT_PARAMS)
{
	//if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint* r_dst = &AY;

		m68ki_push_32(p68k, *r_dst);
		*r_dst = REG_A[7];
		REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + OPER_I_32(p68k));
		return;
	}
	//m68ki_exception_illegal(p68k);
}


void m68k_op_lsr_8_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = src >> shift;

    //	if(shift != 0)
    //		USE_CYCLES(shift<<CYC_SHIFT);
        USE_CYCLES(shift);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_X = FLAG_C = src << (9-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = src >> shift;

    //	if(shift != 0)
    //		USE_CYCLES(shift<<CYC_SHIFT);
        USE_CYCLES(shift);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_X = FLAG_C = src << (9-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_32_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *r_dst;
	uint res = src >> shift;

//	if(shift != 0)
//		USE_CYCLES(shift<<CYC_SHIFT);
    USE_CYCLES(shift);

	*r_dst = res;

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_X = FLAG_C = src << (9-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_8_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = src >> shift;

	//if(shift != 0)
	{
		USE_CYCLES(shift);

		if(shift <= 8)
		{
			*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
			FLAG_X = FLAG_C = src << (9-shift);
			FLAG_N = NFLAG_CLEAR;
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		*r_dst &= 0xffffff00;
		FLAG_X = XFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_N = NFLAG_CLEAR;
		FLAG_Z = ZFLAG_SET;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_8(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = src >> shift;

	//if(shift != 0)
	{
		USE_CYCLES(shift);

		if(shift <= 16)
		{
			*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
			FLAG_C = FLAG_X = (src >> (shift - 1))<<8;
			FLAG_N = NFLAG_CLEAR;
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		*r_dst &= 0xffff0000;
		FLAG_X = XFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_N = NFLAG_CLEAR;
		FLAG_Z = ZFLAG_SET;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_16(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_32_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = *r_dst;
	uint res = src >> shift;

	if(shift != 0)
	{
		USE_CYCLES(shift<<CYC_SHIFT);

		if(shift < 32)
		{
			*r_dst = res;
			FLAG_C = FLAG_X = (src >> (shift - 1))<<8;
			FLAG_N = NFLAG_CLEAR;
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		*r_dst = 0;
		FLAG_X = FLAG_C = (shift == 32 ? GET_MSB_32(src)>>23 : 0);
		FLAG_N = NFLAG_CLEAR;
		FLAG_Z = ZFLAG_SET;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_32(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsr_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_CLEAR;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_8_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src << shift;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> (8-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_32_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> (24-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_8_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	if(shift != 0)
	{
		USE_CYCLES(shift<<CYC_SHIFT);

		if(shift <= 8)
		{
			*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
			FLAG_X = FLAG_C = src << shift;
			FLAG_N = NFLAG_8(res);
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		*r_dst &= 0xffffff00;
		FLAG_X = XFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_N = NFLAG_CLEAR;
		FLAG_Z = ZFLAG_SET;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_8(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	if(shift != 0)
	{
		USE_CYCLES(shift<<CYC_SHIFT);

		if(shift <= 16)
		{
			*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
			FLAG_X = FLAG_C = (src << shift) >> 8;
			FLAG_N = NFLAG_16(res);
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		*r_dst &= 0xffff0000;
		FLAG_X = XFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_N = NFLAG_CLEAR;
		FLAG_Z = ZFLAG_SET;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_16(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_32_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	if(shift != 0)
	{
		USE_CYCLES(shift<<CYC_SHIFT);

		if(shift < 32)
		{
			*r_dst = res;
			FLAG_X = FLAG_C = (src >> (32 - shift)) << 8;
			FLAG_N = NFLAG_32(res);
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		*r_dst = 0;
		FLAG_X = FLAG_C = ((shift == 32 ? src & 1 : 0))<<8;
		FLAG_N = NFLAG_CLEAR;
		FLAG_Z = ZFLAG_SET;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_32(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_lsl_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}

/* original:
_m68k_op_move_8_d_d:
	move.l d2,-(sp)
	move.l (140,a2),d0 reg ir
	moveq #7,d1
	and.l d0,d1   DY
	moveq #0,d2
	not.b d2
	and.l (a2,d1.l*4),d2
	bfextu d0{#20:#3},d0  ; d0
	move.b d2,(3,a2,d0.l*4)
	move.l d2,(76,a2)
	move.l d2,(80,a2)
	clr.l (84,a2)
	clr.l (88,a2)
	move.l (sp)+,d2
	rts

*/
void m68k_op_move_8_d_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
    // 14 instructions -> 7 instructions :)
    asm volatile(
        "move.w %0,d0\n"
        "\tand.l #7,d0\n"
        "\tmove.b 3+%c[dar](%1,d0.w*4),d0\n"
        "\tbfextu %0{#20:#3},d1\n"
        "\tmove.b d0,3+%c[dar](%1,d1.w*4)\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        "\tmove.l d0,%c[not_z_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else

	uint res = MASK_OUT_ABOVE_8(DY);
	uint* r_dst = &DX;
	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
#endif

	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_d_i(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
 // does move.w #value,dx

// %0 regir d2   %1 p68k a2
// 12 instr -> 9 instr.
    asm volatile(
        "move.l %c[progcount](%1),d1\n" // p68k->pc
        "\tmove.w ([_opcode_base],d1.l),d0\n"
        "\taddq.l #2,d1\n" // because 2 alignment
        "\tmove.l d1,%c[progcount](%1)\n"
        "\tand.l #255,d0\n" // for ccr c coherency

        "\tbfextu %0{#20:#3},d1\n" // x in dx
        "\tmove.b d0,%c[dar]+3(%1,d1.l*4)\n"

        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [progcount] "n" (offsetof(struct m68ki_cpu_core, pc)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ai_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi7_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pi_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd7_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_A7_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_pd_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_di_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_ix_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_aw_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_8_al_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_8(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_8();

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
    asm volatile(
        "move.w %0,d0\n"
        "\tand.l #7,d0\n"
        "\tmove.w 2+%c[dar](%1,d0.w*4),d0\n"
        "\tbfextu %0{#20:#3},d1\n"
        "\tmove.w d0,2+%c[dar](%1,d1.w*4)\n"
        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\tlsr.w #8,d0\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else

	uint res = MASK_OUT_ABOVE_16(DY);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_ix(M68KOPT_PARAMS)
{


	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_d_i(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
 // does move.w #value,dx

// %0 regir d2   %1 p68k a2
// 13 instr -> 10 instr.
    asm volatile(
        "move.l %c[progcount](%1),d1\n" // p68k->pc
        "\tclr.l d0\n" // for ccr c coherency
        "\tmove.w ([_opcode_base],d1.l),d0\n"
        "\taddq.l #2,d1\n"
        "\tmove.l d1,%c[progcount](%1)\n"

        "\tbfextu %0{#20:#3},d1\n" // x in dx
        "\tmove.w d0,%c[dar]+2(%1,d1.l*4)\n"

        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\tlsr.l #8,d0\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [progcount] "n" (offsetof(struct m68ki_cpu_core, pc)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else
	uint res = OPER_I_16(p68k);
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
    // note galaxy force 2 test negs after this.
// %0 regir d2   %1 p68k a2
// does move.w dx,(ax)
    asm volatile(
        "moveq #7,d0\n"
        "\tand.l %0,d0\n"
//#ifndef OPTIM68K_USEDIRECT68KASM_MOVEWR_SQUEEZE_NZ
        "\tclr.l    d1\n"  // just for ccr
//#endif
        "\tmove.w %c[dar]+2(%1,d0.l*4),d1\n" // d1 LOW value of dx
//#ifndef OPTIM68K_USEDIRECT68KASM_MOVEWR_SQUEEZE_NZ
       "\tmove.l d1,%c[not_z_flag](%1)\n" // zero ccr
       "\tbfextu d1{#16:#8},d0\n\tmove.w d0,%c[n_flag]+2(%1)\n" // ax
//        "\tmove.w d1,d0\n\tlsr.w #8,d0\n\tmove.w d0,%c[n_flag]+2(%1)\n"
//#endif
        "\tbfextu %0{#20:#3},d0\n" // ax
        "\tmove.l (%c[writer16],a2),a0\n" // writer
        "\tmove.l (%c[dar]+(8*4),a2,d0.l*4),d0\n" // (ax) value
        "\tjmp (a0)\n" // d0 adress, d1 value.  jsr -> jmp can replace jsr+rts if last instruction.
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag)),
         [writer16] "n" (offsetof(struct m68k_cpu_instance, mem)+offsetof(struct m68k_memory_interface, write16))
        :  "d0","d1","a0"
        );
#else
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;

	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
#endif

}


void m68k_op_move_16_ai_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ai_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AX_AI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pi_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AX_PI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_pd_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AX_PD_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_di_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AX_DI_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_ix_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AX_IX_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_aw_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AW_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_a(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_16_al_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_16(p68k);
	uint ea = EA_AL_16();

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
    asm volatile(
        "move.w %0,d0\n"
        "\tand.w #7,d0\n"
        "\tmove.l %c[dar](%1,d0.w*4),d0\n"
        "\tbfextu %0{#20:#3},d1\n"
        "\tmove.l d0,%c[dar](%1,d1.w*4)\n"
        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\trol.l #8,d0\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else
	uint res = DY;
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_a(M68KOPT_PARAMS)
{
    // actually move.l ax,dx
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
    asm volatile(
        "move.w %0,d0\n"
        "\tand.w #7,d0\n"
        "\tmove.l %c[dar]+(8*4)(%1,d0.w*4),d0\n"
        "\tbfextu %0{#20:#3},d1\n"
        "\tmove.l d0,%c[dar](%1,d1.w*4)\n"
        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\trol.l #8,d0\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else
	uint res = AY;
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_di(M68KOPT_PARAMS)
{

	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;

	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_d_i(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
 // does move.l #value,dx

// %0 regir d2   %1 p68k a2
// 13 instr -> 9 instr.
    asm volatile(
        "move.l %c[progcount](%1),d1\n" // p68k->pc
        "\tmove.l ([_opcode_base],d1.l),d0\n"
        "\taddq.l #4,d1\n"
        "\tmove.l d1,%c[progcount](%1)\n"

        "\tbfextu %0{#20:#3},d1\n" // x in dx
        "\tmove.l d0,%c[dar](%1,d1.l*4)\n"

        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\trol.l #8,d0\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [progcount] "n" (offsetof(struct m68ki_cpu_core, pc)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","d1"
        );
#else
	uint res = OPER_I_32(p68k);
	uint* r_dst = &DX;

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_d(M68KOPT_PARAMS)
{
    // does move.l dx,(ax)
    // 17 instructions -> 8 instructions, but N flag not set !!!
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
// %0 regir d2   %1 p68k a2
    asm volatile(
        "moveq #7,d0\n"
        "\tand.l %0,d0\n"
        "\tmove.l %c[dar](%1,d0.l*4),d1\n" // d1 value of dx
#ifndef OPTIM68K_USEDIRECT68KASM_MOVEWR_SQUEEZE_NZ
        "\tmove.l d1,%c[not_z_flag](%1)\n" // zero ccr
#endif
        "\tbfextu %0{#20:#3},d0\n" // ax
        "\tmove.l (%c[writer32],a2),a0\n" // writer
        "\tmove.l (%c[dar]+(8*4),a2,d0.l*4),d0\n" // (ax) value
        "\tjmp (a0)\n" // d0 adress, d1 value.  ->optim jsr -> jmp if last instruction.
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag)),
         [writer32] "n" (offsetof(struct m68k_cpu_instance, mem)+offsetof(struct m68k_memory_interface, write32))
        :  "d0","d1","a0"
        );
#else

	uint res = DY;
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
#endif
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ai_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AX_AI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_d(M68KOPT_PARAMS)
{
	uint res = DY;
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pi_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AX_PI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_d(M68KOPT_PARAMS)
{
	uint res = DY;
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_pd_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AX_PD_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_d(M68KOPT_PARAMS)
{
	uint res = DY;
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_di_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AX_DI_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_d(M68KOPT_PARAMS)
{
	uint res = DY;
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_ix_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AX_IX_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_d(M68KOPT_PARAMS)
{
	uint res = DY;
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_aw_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AW_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_d(M68KOPT_PARAMS)
{
	uint res = DY;
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_a(M68KOPT_PARAMS)
{
	uint res = AY;
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_pcdi(M68KOPT_PARAMS)
{
	uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_pcix(M68KOPT_PARAMS)
{
	uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_move_32_al_i(M68KOPT_PARAMS)
{
	uint res = OPER_I_32(p68k);
	uint ea = EA_AL_32();

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	DO_MOVED_CLEARV();
	DO_MOVED_CLEARC();
}


void m68k_op_movea_16_d(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(DY);
}


void m68k_op_movea_16_a(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(AY);
}


void m68k_op_movea_16_ai(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AY_AI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_pi(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AY_PI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_pd(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AY_PD_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_di(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AY_DI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_ix(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AY_IX_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_aw(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AW_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_al(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_AL_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_pcdi(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_PCDI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_pcix(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_PCIX_16(M68KOPT_PASSPARAMS));
}


void m68k_op_movea_16_i(M68KOPT_PARAMS)
{
	AX = MAKE_INT_16(OPER_I_16(p68k));
}


void m68k_op_movea_32_d(M68KOPT_PARAMS)
{
	AX = DY;
}


void m68k_op_movea_32_a(M68KOPT_PARAMS)
{
	AX = AY;
}


void m68k_op_movea_32_ai(M68KOPT_PARAMS)
{
	AX = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_pi(M68KOPT_PARAMS)
{
	AX = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_pd(M68KOPT_PARAMS)
{
	AX = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_di(M68KOPT_PARAMS)
{
	AX = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_ix(M68KOPT_PARAMS)
{
	AX = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_aw(M68KOPT_PARAMS)
{
	AX = OPER_AW_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_al(M68KOPT_PARAMS)
{
	AX = OPER_AL_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_pcdi(M68KOPT_PARAMS)
{
	AX = OPER_PCDI_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_pcix(M68KOPT_PARAMS)
{
	AX = OPER_PCIX_32(M68KOPT_PASSPARAMS);
}


void m68k_op_movea_32_i(M68KOPT_PARAMS)
{
	AX = OPER_I_32(p68k);
}


void m68k_op_move_16_frc_d(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		DY = MASK_OUT_BELOW_16(DY) | m68ki_get_ccr();
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_ai(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AY_AI_16(), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_pi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AY_PI_16(p68k,regir), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_pd(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AY_PD_16(), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_di(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AY_DI_16(), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_ix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AY_IX_16(), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_aw(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AW_16(), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_frc_al(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		m68ki_write_16(EA_AL_16(), m68ki_get_ccr());
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_move_16_toc_d(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, DY);
}


void m68k_op_move_16_toc_ai(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AY_AI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_pi(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AY_PI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_pd(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AY_PD_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_di(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AY_DI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_ix(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AY_IX_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_aw(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AW_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_al(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_AL_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_pcdi(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_PCDI_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_pcix(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_PCIX_16(M68KOPT_PASSPARAMS));
}


void m68k_op_move_16_toc_i(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, OPER_I_16(p68k));
}


void m68k_op_move_16_frs_d(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		DY = MASK_OUT_BELOW_16(DY) | m68ki_get_sr();
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_ai(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AY_AI_16();
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_pi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AY_PI_16(p68k,regir);
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_pd(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AY_PD_16();
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_di(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AY_DI_16();
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_ix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AY_IX_16();
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_aw(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AW_16();
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_frs_al(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_000(CPU_TYPE) || FLAG_S)	/* NS990408 */
	{
		uint ea = EA_AL_16();
		m68ki_write_16(ea, m68ki_get_sr());
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_d(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		m68ki_set_sr(p68k, DY);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_ai(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_pi(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_pd(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_di(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_ix(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_aw(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AW_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_al(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_AL_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_pcdi(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_PCDI_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_pcix(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_PCIX_16(M68KOPT_PASSPARAMS);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_16_tos_i(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_I_16(p68k);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, new_sr);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_32_fru(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		AY = REG_USP;
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_move_32_tou(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		REG_USP = AY;
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_movec_32_cr(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);

			m68ki_trace_t0();		   /* auto-disable (see m68kcpu.h) */
			switch (word2 & 0xfff)
			{
			case 0x000:			   /* SFC */
				REG_DA[(word2 >> 12) & 15] = REG_SFC;
				return;
			case 0x001:			   /* DFC */
				REG_DA[(word2 >> 12) & 15] = REG_DFC;
				return;
			case 0x002:			   /* CACR */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					REG_DA[(word2 >> 12) & 15] = REG_CACR;
					return;
				}
				return;
			case 0x800:			   /* USP */
				REG_DA[(word2 >> 12) & 15] = REG_USP;
				return;
			case 0x801:			   /* VBR */
				REG_DA[(word2 >> 12) & 15] = REG_VBR;
				return;
			case 0x802:			   /* CAAR */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					REG_DA[(word2 >> 12) & 15] = REG_CAAR;
					return;
				}
				m68ki_exception_illegal(p68k);
				break;
			case 0x803:			   /* MSP */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					REG_DA[(word2 >> 12) & 15] = FLAG_M ? REG_SP : REG_MSP;
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x804:			   /* ISP */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					REG_DA[(word2 >> 12) & 15] = FLAG_M ? REG_ISP : REG_SP;
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x003:				/* TC */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x004:				/* ITT0 */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x005:				/* ITT1 */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x006:				/* DTT0 */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x007:				/* DTT1 */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x805:				/* MMUSR */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x806:				/* URP */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x807:				/* SRP */
				if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			default:
				m68ki_exception_illegal(p68k);
				return;
			}
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_movec_32_rc(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);

			m68ki_trace_t0();		   /* auto-disable (see m68kcpu.h) */
			switch (word2 & 0xfff)
			{
			case 0x000:			   /* SFC */
				REG_SFC = REG_DA[(word2 >> 12) & 15] & 7;
				return;
			case 0x001:			   /* DFC */
				REG_DFC = REG_DA[(word2 >> 12) & 15] & 7;
				return;
			case 0x002:			   /* CACR */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					REG_CACR = REG_DA[(word2 >> 12) & 15];
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x800:			   /* USP */
				REG_USP = REG_DA[(word2 >> 12) & 15];
				return;
			case 0x801:			   /* VBR */
				REG_VBR = REG_DA[(word2 >> 12) & 15];
				return;
			case 0x802:			   /* CAAR */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					REG_CAAR = REG_DA[(word2 >> 12) & 15];
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x803:			   /* MSP */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					/* we are in supervisor mode so just check for M flag */
					if(!FLAG_M)
					{
						REG_MSP = REG_DA[(word2 >> 12) & 15];
						return;
					}
					REG_SP = REG_DA[(word2 >> 12) & 15];
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x804:			   /* ISP */
				if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
				{
					if(!FLAG_M)
					{
						REG_SP = REG_DA[(word2 >> 12) & 15];
						return;
					}
					REG_ISP = REG_DA[(word2 >> 12) & 15];
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x003:			/* TC */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x004:			/* ITT0 */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x005:			/* ITT1 */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x006:			/* DTT0 */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x007:			/* DTT1 */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x805:			/* MMUSR */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x806:			/* URP */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			case 0x807:			/* SRP */
				if (CPU_TYPE_IS_040_PLUS(CPU_TYPE))
				{
					/* TODO */
					return;
				}
				m68ki_exception_illegal(p68k);
				return;
			default:
				m68ki_exception_illegal(p68k);
				return;
			}
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_movem_16_re_pd(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = AY;
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			ea -= 2;
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[15-i]));
			count++;
		}
	AY = ea;

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_re_ai(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_AI_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[i]));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_re_di(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_DI_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[i]));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_re_ix(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_IX_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[i]));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_re_aw(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AW_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[i]));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_re_al(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AL_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[i]));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}
//extern UINT32 memory_writemovem32rr(UINT32 address /*REG(d0)*/, UINT32 bits /*REG(d1)*/, UINT32 *preg /*REG(a0)*/ );

void m68k_op_movem_32_re_pd(M68KOPT_PARAMS)
{
/*
    for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			ea -= 2;
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[15-i]));
			count++;
		}
	AY = ea;
*/
#ifndef OPTIM68K_USEFASTMOVEMWRITE
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = AY;
	uint count = 0;

	for(; i < 16; i++)
	{
		if(register_list & 1)
		{
			ea -= 4;
            m68ki_write_32(ea, REG_DA[15-i]);
			count++;
          
		}
		register_list>>=1;
	}
	AY = ea;

	USE_CYCLES(count<<CYC_MOVEM_L);
#else

    uint register_list = OPER_I_16(p68k);
	uint ea = AY;

    uint count = p68k->mem.writemovem32reverse(ea,register_list,&REG_DA[0]);
    AY = ea - (count<<2);
    USE_CYCLES(count<<CYC_MOVEM_L);

#endif
}


void m68k_op_movem_32_re_ai(M68KOPT_PARAMS)
{

	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_AI_32();
	uint count = 0;

	for(; i < 16; i++)
	{
		if(register_list & 1)
		{
			m68ki_write_32(ea, REG_DA[i]);
			ea += 4;
			count++;
		}
		register_list>>=1;
	}
	USE_CYCLES(count<<CYC_MOVEM_L);

//    uint register_list = OPER_I_16(p68k);
//	uint ea = EA_AY_AI_32();
//    uint count = memory_writemovem32(ea,register_list,&REG_DA[0]);
//    USE_CYCLES(count<<CYC_MOVEM_L);
}


void m68k_op_movem_32_re_di(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_DI_32();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_32(ea, REG_DA[i]);
			ea += 4;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_L);
    /*
    uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_DI_32();
    uint count = memory_writemovem32(ea,register_list,&REG_DA[0]);
    USE_CYCLES(count<<CYC_MOVEM_L);*/
}


void m68k_op_movem_32_re_ix(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_IX_32();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_32(ea, REG_DA[i]);
			ea += 4;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_L);
/*    uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_IX_32();
    uint count = memory_writemovem32(ea,register_list,&REG_DA[0]);
    USE_CYCLES(count<<CYC_MOVEM_L);*/
}


void m68k_op_movem_32_re_aw(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AW_32();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_32(ea, REG_DA[i]);
			ea += 4;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_L);
    /*
    uint register_list = OPER_I_16(p68k);
	uint ea = EA_AW_32();
    uint count = memory_writemovem32(ea,register_list,&REG_DA[0]);
    USE_CYCLES(count<<CYC_MOVEM_L);*/
}


void m68k_op_movem_32_re_al(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AL_32();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			m68ki_write_32(ea, REG_DA[i]);
			ea += 4;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_L);
    /*
    uint register_list = OPER_I_16(p68k);
	uint ea = EA_AL_32();
    uint count = memory_writemovem32(ea,register_list,&REG_DA[0]);
    USE_CYCLES(count<<CYC_MOVEM_L);
    */
}


void m68k_op_movem_16_er_pi(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = AY;
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(p68k->mem.read16(ea)));
			ea += 2;
			count++;
		}
	AY = ea;

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_pcdi(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_PCDI_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_pcrel_16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_pcix(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_PCIX_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_pcrel_16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_ai(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_AI_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(p68k->mem.read16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_di(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_DI_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(p68k->mem.read16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_ix(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_IX_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(p68k->mem.read16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_aw(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AW_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(p68k->mem.read16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_16_er_al(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AL_16();
	uint count = 0;

	for(; i < 16; i++)
		if(register_list & (1 << i))
		{
			REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(p68k->mem.read16(ea)));
			ea += 2;
			count++;
		}

	USE_CYCLES(count<<CYC_MOVEM_W);
}


void m68k_op_movem_32_er_pi(M68KOPT_PARAMS)
{
#ifndef OPTIM68K_USEFASTMOVEMREAD
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
#else
	// m68ki_read_32 -> (*p68k->mem.read32)(address)
	// -> program_read_dword_32be
	// ->    READDWORD32(program_read_dword_32be,

	uint register_list = OPER_I_16(p68k);
	uint ea = AY;
	uint count =  p68k->mem.readmovem32(ea, register_list, &REG_DA[0] );
    AY = ea+(4*count);
    USE_CYCLES(count<<CYC_MOVEM_L);
#endif
}


void m68k_op_movem_32_er_pcdi(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_PCDI_32();
	uint count = 0;
	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = m68ki_read_pcrel_32(ea);
			ea += 4;
			count++;
		}
        register_list>>=1;
    }
	USE_CYCLES(count<<CYC_MOVEM_L);
}


void m68k_op_movem_32_er_pcix(M68KOPT_PARAMS)
{
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_PCIX_32();
	uint count = 0;
	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = m68ki_read_pcrel_32(ea);
			ea += 4;
			count++;
		}
        register_list>>=1;
    }
	USE_CYCLES(count<<CYC_MOVEM_L);
}


void m68k_op_movem_32_er_ai(M68KOPT_PARAMS)
{
#ifndef OPTIM68K_USEFASTMOVEMREAD
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_AI_32();
	uint count = 0;

	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = p68k->mem.read32(ea);
			ea += 4;
			count++;
		}
        register_list>>=1;
    }

	USE_CYCLES(count<<CYC_MOVEM_L);
#else

	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_AI_32();
	uint count = p68k->mem.readmovem32(ea, register_list, &REG_DA[0] );
    USE_CYCLES(count<<CYC_MOVEM_L);
#endif
}


void m68k_op_movem_32_er_di(M68KOPT_PARAMS)
{
#ifndef OPTIM68K_USEFASTMOVEMREAD
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_DI_32();
	uint count = 0;
	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = p68k->mem.read32(ea);
			ea += 4;
			count++;            
		}
        register_list>>=1;
    }
	USE_CYCLES(count<<CYC_MOVEM_L);

#else
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_DI_32();
	uint count = p68k->mem.readmovem32(ea, register_list, &REG_DA[0] );
    USE_CYCLES(count<<CYC_MOVEM_L);
#endif
}


void m68k_op_movem_32_er_ix(M68KOPT_PARAMS)
{
#ifndef OPTIM68K_USEFASTMOVEMREAD
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_IX_32();
	uint count = 0;

	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = p68k->mem.read32(ea);
			ea += 4;
			count++;
		}
        register_list>>=1;
    }
	USE_CYCLES(count<<CYC_MOVEM_L);
#else

	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AY_IX_32();
	uint count = p68k->mem.readmovem32(ea, register_list, &REG_DA[0] );
    USE_CYCLES(count<<CYC_MOVEM_L);
#endif
}


void m68k_op_movem_32_er_aw(M68KOPT_PARAMS)
{
#ifndef OPTIM68K_USEFASTMOVEMREAD
	uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AW_32();
	uint count = 0;

	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = p68k->mem.read32(ea);
			ea += 4;
			count++;
		}
        register_list>>=1;
    }
	USE_CYCLES(count<<CYC_MOVEM_L);
#else

	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AW_32();
	uint count = p68k->mem.readmovem32(ea, register_list, &REG_DA[0] );
    USE_CYCLES(count<<CYC_MOVEM_L);
#endif
}


void m68k_op_movem_32_er_al(M68KOPT_PARAMS)
{
#ifndef OPTIM68K_USEFASTMOVEMREAD
    uint i = 0;
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AL_32();
	uint count = 0;

	for(; i < 16; i++)
    {
		if(register_list & 1)
		{
			REG_DA[i] = p68k->mem.read32(ea);
			ea += 4;
			count++;
		}
        register_list>>=1;
    }
	USE_CYCLES(count<<CYC_MOVEM_L);
#else
	uint register_list = OPER_I_16(p68k);
	uint ea = EA_AL_32();
	uint count = p68k->mem.readmovem32(ea, register_list, &REG_DA[0] );
    USE_CYCLES(count<<CYC_MOVEM_L);
#endif
}


void m68k_op_movep_16_re(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = DX;

	m68ki_write_8(ea, MASK_OUT_ABOVE_8(src >> 8));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src));
}


void m68k_op_movep_32_re(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	uint src = DX;

	m68ki_write_8(ea, MASK_OUT_ABOVE_8(src >> 24));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src >> 16));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src >> 8));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src));
}


void m68k_op_movep_16_er(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint* r_dst = &DX;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | ((p68k->mem.read8(ea) << 8) + p68k->mem.read8(ea + 2));
}


void m68k_op_movep_32_er(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();

	DX = (p68k->mem.read8(ea) << 24) + (p68k->mem.read8(ea + 2) << 16)
		+ (p68k->mem.read8(ea + 4) << 8) + p68k->mem.read8(ea + 6);
}


void m68k_op_moves_8_ai(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_AI_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_pi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_PI_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_pi7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_A7_PI_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_pd(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_PD_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_pd7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_A7_PD_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_di(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_DI_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_ix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_IX_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_aw(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AW_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_8_al(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AL_8();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_8(ea, MASK_OUT_ABOVE_8(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_8(m68k_read_memory_8(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_8(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_8(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_ai(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_AI_16();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_pi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_PI_16(p68k,regir);

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_pd(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_PD_16();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_di(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_DI_16();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_ix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_IX_16();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_aw(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AW_16();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_16_al(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AL_16();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_16(ea, MASK_OUT_ABOVE_16(REG_DA[(word2 >> 12) & 15]));
				return;
			}
			if(BIT_F(word2))		   /* Memory to address register */
			{
				REG_A[(word2 >> 12) & 7] = MAKE_INT_16(m68k_read_memory_16(ea));
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to data register */
			REG_D[(word2 >> 12) & 7] = MASK_OUT_BELOW_16(REG_D[(word2 >> 12) & 7]) | m68k_read_memory_16(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_ai(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_AI_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_pi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_PI_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_pd(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_PD_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_di(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_DI_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_ix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AY_IX_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_aw(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AW_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moves_32_al(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		if(FLAG_S)
		{
			uint word2 = OPER_I_16(p68k);
			uint ea = EA_AL_32();

			m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
			if(BIT_B(word2))		   /* Register to memory */
			{
				m68k_write_memory_32(ea, REG_DA[(word2 >> 12) & 15]);
				if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
					USE_CYCLES(2);
				return;
			}
			/* Memory to register */
			REG_DA[(word2 >> 12) & 15] = m68k_read_memory_32(ea);
			if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
				USE_CYCLES(2);
			return;
		}
		m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_moveq_32(M68KOPT_PARAMS)
{
// #ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
//     // 12 instructions -> 6 instructions :)
//     asm volatile(
//         "move.b %0,d0\n"
//         "\textb.l d0\n"
//         "\tbfextu %0{#20:#3},d1\n"
//         "\tmove.l d0,%c[dar](%1,d1.l*4)\n"
//         "\tmove.l d0,%c[not_z_flag](%1)\n"
//         "\tmove.l d0,%c[n_flag](%1)\n"  // bit 7 is neg, no need rol !
//         :
//         : "d"(regir), "a"(p68k),
//          [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
//          [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
//          [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
//         :  "d0","d1"
//         );
// #else
	uint res = DX = MAKE_INT_8(MASK_OUT_ABOVE_8(REG_IR));

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
//#endif
	DO_MOVED_CLEARV();
//	DO_MOVED_CLEARC();
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_move16_32(M68KOPT_PARAMS)
{
	UINT16 w2 = OPER_I_16(p68k);
	int ax = REG_IR & 7;
	int ay = (w2 >> 12) & 7;

    UINT32 v = p68k->mem.read32(REG_A[ax]);

    m68ki_write_32(REG_A[ay],    v);

	m68ki_write_32(REG_A[ay]+4,  p68k->mem.read32(REG_A[ax]+4));
	m68ki_write_32(REG_A[ay]+8,  p68k->mem.read32(REG_A[ax]+8));
	m68ki_write_32(REG_A[ay]+12, p68k->mem.read32(REG_A[ax]+12));

	REG_A[ax] += 16;
	REG_A[ay] += 16;
}


void m68k_op_muls_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(DY) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_ai(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AY_AI_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_pi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AY_PI_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_pd(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AY_PD_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_di(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AY_DI_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_ix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AY_IX_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_aw(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AW_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_al(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_AL_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_pcdi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_PCDI_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_pcix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_PCIX_16(M68KOPT_PASSPARAMS)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_muls_16_i(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_32(MAKE_INT_16(OPER_I_16(p68k)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*r_dst)));

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = MASK_OUT_ABOVE_16(DY) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_ai(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_pi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_pd(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_di(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_ix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_aw(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_al(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_pcdi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_pcix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mulu_16_i(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint res = OPER_I_16(p68k) * MASK_OUT_ABOVE_16(*r_dst);

	*r_dst = res;

	FLAG_Z = res;
	FLAG_N = NFLAG_32(res);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_mull_32_d(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = DY;
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = DY;
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_ai(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_pi(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_pd(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_di(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_ix(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_aw(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AW_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AW_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_al(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_AL_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_AL_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_pcdi(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_PCDI_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_PCDI_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_pcix(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_PCIX_32(M68KOPT_PASSPARAMS);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

//	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
//	{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_PCIX_32(M68KOPT_PASSPARAMS);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


void m68k_op_mull_32_i(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint word2 = OPER_I_16(p68k);
		uint64 src = OPER_I_32(p68k);
		uint64 dst = REG_D[(word2 >> 12) & 7];
		uint64 res;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			res = (sint64)((sint32)src) * (sint64)((sint32)dst);
			if(!BIT_A(word2))
			{
				FLAG_Z = MASK_OUT_ABOVE_32(res);
				FLAG_N = NFLAG_32(res);
				FLAG_V = ((sint64)res != (sint32)res)<<7;
				REG_D[(word2 >> 12) & 7] = FLAG_Z;
				return;
			}
			FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
			FLAG_N = NFLAG_64(res);
			FLAG_V = VFLAG_CLEAR;
			REG_D[word2 & 7] = (res >> 32);
			REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
			return;
		}

		res = src * dst;
		if(!BIT_A(word2))
		{
			FLAG_Z = MASK_OUT_ABOVE_32(res);
			FLAG_N = NFLAG_32(res);
			FLAG_V = (res > 0xffffffff)<<7;
			REG_D[(word2 >> 12) & 7] = FLAG_Z;
			return;
		}
		FLAG_Z = MASK_OUT_ABOVE_32(res) | (res>>32);
		FLAG_N = NFLAG_64(res);
		FLAG_V = VFLAG_CLEAR;
		REG_D[word2 & 7] = (res >> 32);
		REG_D[(word2 >> 12) & 7] = MASK_OUT_ABOVE_32(res);
		return;
	}
	m68ki_exception_illegal(p68k);

#else

	//if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	//{
		uint word2 = OPER_I_16(p68k);
		uint src = OPER_I_32(p68k);
		uint dst = REG_D[(word2 >> 12) & 7];
		uint neg = GET_MSB_32(src ^ dst);
		uint src1;
		uint src2;
		uint dst1;
		uint dst2;
		uint r1;
		uint r2;
		uint r3;
		uint r4;
		uint lo;
		uint hi;

		FLAG_C = CFLAG_CLEAR;

		if(BIT_B(word2))			   /* signed */
		{
			if(GET_MSB_32(src))
				src = (uint)MASK_OUT_ABOVE_32(-(sint)src);
			if(GET_MSB_32(dst))
				dst = (uint)MASK_OUT_ABOVE_32(-(sint)dst);
		}

		src1 = MASK_OUT_ABOVE_16(src);
		src2 = src>>16;
		dst1 = MASK_OUT_ABOVE_16(dst);
		dst2 = dst>>16;


		r1 = src1 * dst1;
		r2 = src1 * dst2;
		r3 = src2 * dst1;
		r4 = src2 * dst2;

		lo = r1 + (MASK_OUT_ABOVE_16(r2)<<16) + (MASK_OUT_ABOVE_16(r3)<<16);
		hi = r4 + (r2>>16) + (r3>>16) + (((r1>>16) + MASK_OUT_ABOVE_16(r2) + MASK_OUT_ABOVE_16(r3)) >> 16);

		if(BIT_B(word2) && neg)
		{
			hi = (uint)MASK_OUT_ABOVE_32((-(sint)hi) - (lo != 0));
			lo = (uint)MASK_OUT_ABOVE_32(-(sint)lo);
		}

		if(BIT_A(word2))
		{
			REG_D[word2 & 7] = hi;
			REG_D[(word2 >> 12) & 7] = lo;
			FLAG_N = NFLAG_32(hi);
			FLAG_Z = hi | lo;
			FLAG_V = VFLAG_CLEAR;
			return;
		}

		REG_D[(word2 >> 12) & 7] = lo;
		FLAG_N = NFLAG_32(lo);
		FLAG_Z = lo;
		if(BIT_B(word2))
			FLAG_V = (!((GET_MSB_32(lo) && hi == 0xffffffff) || (!GET_MSB_32(lo) && !hi)))<<7;
		else
			FLAG_V = (hi != 0) << 7;
		return;
//	}
//	m68ki_exception_illegal(p68k);

#endif
}


/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */


