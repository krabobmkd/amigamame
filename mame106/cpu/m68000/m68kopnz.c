#include "m68kcpu.h"
extern void m68040_fpu_op0(M68KOPT_PARAMS);
extern void m68040_fpu_op1(M68KOPT_PARAMS);

/* ======================================================================== */
/* ========================= INSTRUCTION HANDLERS ========================= */
/* ======================================================================== */


void m68k_op_nbcd_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_nbcd_8_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint dst = p68k->mem.read8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - XFLAG_AS_1());

	if(res != 0x9a)
	{
		FLAG_V = ~res; /* Undefined V behavior */

		if((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		res = MASK_OUT_ABOVE_8(res);

		FLAG_V &= res; /* Undefined V behavior part II */

		m68ki_write_8(ea, MASK_OUT_ABOVE_8(res));

		FLAG_Z |= res;
		FLAG_C = CFLAG_SET;
		FLAG_X = XFLAG_SET;
	}
	else
	{
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		FLAG_X = XFLAG_CLEAR;
	}
	FLAG_N = NFLAG_8(res);	/* Undefined N behavior */
}


void m68k_op_neg_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = 0 - MASK_OUT_ABOVE_8(*r_dst);

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = *r_dst & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_neg_8_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_8_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_8(res);
	FLAG_C = FLAG_X = CFLAG_8(res);
	FLAG_V = src & res;
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_neg_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = 0 - MASK_OUT_ABOVE_16(*r_dst);

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (*r_dst & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_neg_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_16(res);
	FLAG_C = FLAG_X = CFLAG_16(res);
	FLAG_V = (src & res)>>8;
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_neg_32_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = 0 - *r_dst;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(*r_dst, 0, res);
	FLAG_V = (*r_dst & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
}


void m68k_op_neg_32_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_neg_32_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_neg_32_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_neg_32_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_neg_32_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_neg_32_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_neg_32_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - src;

	FLAG_N = NFLAG_32(res);
	FLAG_C = FLAG_X = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_negx_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = 0 - MASK_OUT_ABOVE_8(*r_dst) - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = *r_dst & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
}


void m68k_op_negx_8_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_8_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint src = p68k->mem.read8(ea);
	uint res = 0 - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = src & res;

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_negx_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = 0 - MASK_OUT_ABOVE_16(*r_dst) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (*r_dst & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
}


void m68k_op_negx_16_ai(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_16_pi(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_16_pd(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_16_di(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_16_ix(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_16_aw(M68KOPT_PARAMS)
{
	uint ea  = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_16_al(M68KOPT_PARAMS)
{
	uint ea  = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = 0 - MASK_OUT_ABOVE_16(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = (src & res)>>8;

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_negx_32_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = 0 - MASK_OUT_ABOVE_32(*r_dst) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(*r_dst, 0, res);
	FLAG_V = (*r_dst & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	*r_dst = res;
}


void m68k_op_negx_32_ai(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_AI_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_negx_32_pi(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_PI_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_negx_32_pd(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_PD_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_negx_32_di(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_DI_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_negx_32_ix(M68KOPT_PARAMS)
{
	uint ea  = EA_AY_IX_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_negx_32_aw(M68KOPT_PARAMS)
{
	uint ea  = EA_AW_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_negx_32_al(M68KOPT_PARAMS)
{
	uint ea  = EA_AL_32();
	uint src = p68k->mem.read32(ea);
	uint res = 0 - MASK_OUT_ABOVE_32(src) - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, 0, res);
	FLAG_V = (src & res)>>24;

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_nop(M68KOPT_PARAMS)
{
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
}


void m68k_op_not_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = MASK_OUT_ABOVE_8(~*r_dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_8_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint res = MASK_OUT_ABOVE_8(~p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = MASK_OUT_ABOVE_16(~*r_dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint res = MASK_OUT_ABOVE_16(~p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint res = *r_dst = MASK_OUT_ABOVE_32(~*r_dst);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_not_32_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();
	uint res = MASK_OUT_ABOVE_32(~p68k->mem.read32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= MASK_OUT_ABOVE_8(DY)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_ai(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AY_AI_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_pi(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AY_PI_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_pi7(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_A7_PI_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_pd(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AY_PD_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_pd7(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_A7_PD_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_di(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AY_DI_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_ix(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AY_IX_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_aw(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AW_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_al(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_AL_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_pcdi(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_PCDI_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_pcix(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_PCIX_8(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_er_i(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DX |= OPER_I_8(p68k)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= MASK_OUT_ABOVE_16(DY)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_ai(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AY_AI_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_pi(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AY_PI_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_pd(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AY_PD_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_di(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AY_DI_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_ix(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AY_IX_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_aw(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AW_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_al(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_AL_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_pcdi(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_PCDI_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_pcix(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_PCIX_16(M68KOPT_PASSPARAMS)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_er_i(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16((DX |= OPER_I_16(p68k)));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_d(M68KOPT_PARAMS)
{
	uint res = DX |= DY;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_ai(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AY_AI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_pi(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AY_PI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_pd(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AY_PD_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_di(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AY_DI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_ix(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AY_IX_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_aw(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AW_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_al(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_AL_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_pcdi(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_PCDI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_pcix(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_PCIX_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_er_i(M68KOPT_PARAMS)
{
	uint res = DX |= OPER_I_32(p68k);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_8_re_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint res = MASK_OUT_ABOVE_8(DX | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_16_re_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint res = MASK_OUT_ABOVE_16(DX | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_or_32_re_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();
	uint res = DX | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8((DY |= OPER_I_8(p68k)));

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_AI_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_PI_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_pi7(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_A7_PI_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_PD_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_pd7(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_A7_PD_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_DI_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_IX_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AW_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_8_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AL_8();
	uint res = MASK_OUT_ABOVE_8(src | p68k->mem.read8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY |= OPER_I_16(p68k));

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_AI_16();
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_PI_16(p68k,regir);
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_PD_16();
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_DI_16();
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_IX_16();
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AW_16();
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AL_16();
	uint res = MASK_OUT_ABOVE_16(src | p68k->mem.read16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_d(M68KOPT_PARAMS)
{
	uint res = DY |= OPER_I_32(p68k);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_AI_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_PI_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_PD_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_DI_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_IX_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AW_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_32_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AL_32();
	uint res = src | p68k->mem.read32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ori_16_toc(M68KOPT_PARAMS)
{
	m68ki_set_ccr(p68k, m68ki_get_ccr() | OPER_I_16(p68k));
}


void m68k_op_ori_16_tos(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint src = OPER_I_16(p68k);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(p68k, m68ki_get_sr() | src);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_pack_16_rr(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: DX and DY are reversed in Motorola's docs */
		uint src = DY + OPER_I_16(p68k);
		uint* r_dst = &DX;

		*r_dst = MASK_OUT_BELOW_8(*r_dst) | ((src >> 4) & 0x00f0) | (src & 0x000f);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_pack_16_mm_ax7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: AX and AY are reversed in Motorola's docs */
		uint ea_src = EA_AY_PD_8();
		uint src = p68k->mem.read8(ea_src);
		ea_src = EA_AY_PD_8();
		src = ((src << 8) | p68k->mem.read8(ea_src)) + OPER_I_16(p68k);

		m68ki_write_8(EA_A7_PD_8(), ((src >> 4) & 0x00f0) | (src & 0x000f));
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_pack_16_mm_ay7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: AX and AY are reversed in Motorola's docs */
		uint ea_src = EA_A7_PD_8();
		uint src = p68k->mem.read8(ea_src);
		ea_src = EA_A7_PD_8();
		src = ((src << 8) | p68k->mem.read8(ea_src)) + OPER_I_16(p68k);

		m68ki_write_8(EA_AX_PD_8(), ((src >> 4) & 0x00f0) | (src & 0x000f));
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_pack_16_mm_axy7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint ea_src = EA_A7_PD_8();
		uint src = p68k->mem.read8(ea_src);
		ea_src = EA_A7_PD_8();
		src = ((src << 8) | p68k->mem.read8(ea_src)) + OPER_I_16(p68k);

		m68ki_write_8(EA_A7_PD_8(), ((src >> 4) & 0x00f0) | (src & 0x000f));
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_pack_16_mm(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: AX and AY are reversed in Motorola's docs */
		uint ea_src = EA_AY_PD_8();
		uint src = p68k->mem.read8(ea_src);
		ea_src = EA_AY_PD_8();
		src = ((src << 8) | p68k->mem.read8(ea_src)) + OPER_I_16(p68k);

		m68ki_write_8(EA_AX_PD_8(), ((src >> 4) & 0x00f0) | (src & 0x000f));
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_pea_32_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pea_32_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pea_32_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pea_32_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pea_32_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pea_32_pcdi(M68KOPT_PARAMS)
{
	uint ea = EA_PCDI_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pea_32_pcix(M68KOPT_PARAMS)
{
	uint ea = EA_PCIX_32();

	m68ki_push_32(p68k, ea);
}


void m68k_op_pflush_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_040_PLUS(CPU_TYPE))
	{
		// Nothing to do, unless address translation cache is emulated
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_reset(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		p68k->m_cpu.reset_instr_callback();		   /* auto-disable (see m68kcpu.h) */
		USE_CYCLES(CYC_RESET);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_ror_8_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROR_8(src, shift);

	if(orig_shift != 0)
		USE_CYCLES(orig_shift<<CYC_SHIFT);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = src << (9-orig_shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROR_16(src, shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << (9-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_32_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint64 src = *r_dst;
	uint res = ROR_32(src, shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = src << (9-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_8_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROR_8(src, shift);

	if(orig_shift != 0)
	{
		USE_CYCLES(orig_shift<<CYC_SHIFT);

		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
		FLAG_C = src << (8-((shift-1)&7));
		FLAG_N = NFLAG_8(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_8(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 15;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROR_16(src, shift);

	if(orig_shift != 0)
	{
		USE_CYCLES(orig_shift<<CYC_SHIFT);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		FLAG_C = (src >> ((shift - 1) & 15)) << 8;
		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_16(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_32_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 31;
	uint64 src = *r_dst;
	uint res = ROR_32(src, shift);

	if(orig_shift != 0)
	{
		USE_CYCLES(orig_shift<<CYC_SHIFT);

		*r_dst = res;
		FLAG_C = (src >> ((shift - 1) & 31)) << 8;
		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_32(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_ror_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src << 8;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_8_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROL_8(src, shift);

	if(orig_shift != 0)
		USE_CYCLES(orig_shift<<CYC_SHIFT);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_C = src << orig_shift;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROL_16(src, shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> (8-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_32_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint64 src = *r_dst;
	uint res = ROL_32(src, shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_C = src >> (24-shift);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_8_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROL_8(src, shift);

	if(orig_shift != 0)
	{
		USE_CYCLES(orig_shift<<CYC_SHIFT);

		if(shift != 0)
		{
			*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
			FLAG_C = src << shift;
			FLAG_N = NFLAG_8(res);
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}
		FLAG_C = (src & 1)<<8;
		FLAG_N = NFLAG_8(src);
		FLAG_Z = src;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_8(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 15;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, shift));

	if(orig_shift != 0)
	{
		USE_CYCLES(orig_shift<<CYC_SHIFT);

		if(shift != 0)
		{
			*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
			FLAG_C = (src << shift) >> 8;
			FLAG_N = NFLAG_16(res);
			FLAG_Z = res;
			FLAG_V = VFLAG_CLEAR;
			return;
		}
		FLAG_C = (src & 1)<<8;
		FLAG_N = NFLAG_16(src);
		FLAG_Z = src;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_16(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_32_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 31;
	uint64 src = *r_dst;
	uint res = ROL_32(src, shift);

	if(orig_shift != 0)
	{
		USE_CYCLES(orig_shift<<CYC_SHIFT);

		*r_dst = res;

		FLAG_C = (src >> (32 - shift)) << 8;
		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = CFLAG_CLEAR;
	FLAG_N = NFLAG_32(src);
	FLAG_Z = src;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rol_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_C = src >> 7;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_8_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROR_9(src | (XFLAG_AS_1() << 8), shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	FLAG_C = FLAG_X = res;
	res = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_32_s(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	uint*  r_dst = &DY;
	uint   shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint64 src   = *r_dst;
	uint64 res   = src | (((uint64)XFLAG_AS_1()) << 32);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	res = ROR_33_64(res, shift);

	FLAG_C = FLAG_X = res >> 24;
	res = MASK_OUT_ABOVE_32(res);

	*r_dst =  res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;

#else

	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32((ROR_33(src, shift) & ~(1 << (32 - shift))) | (XFLAG_AS_1() << (32 - shift)));
	uint new_x_flag = src & (1 << (shift - 1));

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = res;

	FLAG_C = FLAG_X = (new_x_flag != 0)<<8;
	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;

#endif
}


void m68k_op_roxr_8_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;

	if(orig_shift != 0)
	{
		uint shift = orig_shift % 9;
		uint src   = MASK_OUT_ABOVE_8(*r_dst);
		uint res   = ROR_9(src | (XFLAG_AS_1() << 8), shift);

		USE_CYCLES(orig_shift<<CYC_SHIFT);

		FLAG_C = FLAG_X = res;
		res = MASK_OUT_ABOVE_8(res);

		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
		FLAG_N = NFLAG_8(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_8(*r_dst);
	FLAG_Z = MASK_OUT_ABOVE_8(*r_dst);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;

	if(orig_shift != 0)
	{
		uint shift = orig_shift % 17;
		uint src   = MASK_OUT_ABOVE_16(*r_dst);
		uint res   = ROR_17(src | (XFLAG_AS_1() << 16), shift);

		USE_CYCLES(orig_shift<<CYC_SHIFT);

		FLAG_C = FLAG_X = res >> 8;
		res = MASK_OUT_ABOVE_16(res);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_16(*r_dst);
	FLAG_Z = MASK_OUT_ABOVE_16(*r_dst);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_32_r(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	uint*  r_dst = &DY;
	uint   orig_shift = DX & 0x3f;

	if(orig_shift != 0)
	{
		uint   shift = orig_shift % 33;
		uint64 src   = *r_dst;
		uint64 res   = src | (((uint64)XFLAG_AS_1()) << 32);

		res = ROR_33_64(res, shift);

		USE_CYCLES(orig_shift<<CYC_SHIFT);

		FLAG_C = FLAG_X = res >> 24;
		res = MASK_OUT_ABOVE_32(res);

		*r_dst = res;
		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_32(*r_dst);
	FLAG_Z = *r_dst;
	FLAG_V = VFLAG_CLEAR;

#else

	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 33;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32((ROR_33(src, shift) & ~(1 << (32 - shift))) | (XFLAG_AS_1() << (32 - shift)));
	uint new_x_flag = src & (1 << (shift - 1));

	if(orig_shift != 0)
		USE_CYCLES(orig_shift<<CYC_SHIFT);

	if(shift != 0)
	{
		*r_dst = res;
		FLAG_X = (new_x_flag != 0)<<8;
	}
	else
		res = src;
	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;

#endif
}


void m68k_op_roxr_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxr_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROR_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_8_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROL_9(src | (XFLAG_AS_1() << 8), shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	FLAG_C = FLAG_X = res;
	res = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_s(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), shift);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_32_s(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	uint*  r_dst = &DY;
	uint   shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint64 src   = *r_dst;
	uint64 res   = src | (((uint64)XFLAG_AS_1()) << 32);

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	res = ROL_33_64(res, shift);

	FLAG_C = FLAG_X = res >> 24;
	res = MASK_OUT_ABOVE_32(res);

	*r_dst = res;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;

#else

	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32((ROL_33(src, shift) & ~(1 << (shift - 1))) | (XFLAG_AS_1() << (shift - 1)));
	uint new_x_flag = src & (1 << (32 - shift));

	if(shift != 0)
		USE_CYCLES(shift<<CYC_SHIFT);

	*r_dst = res;

	FLAG_C = FLAG_X = (new_x_flag != 0)<<8;
	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;

#endif
}


void m68k_op_roxl_8_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;


	if(orig_shift != 0)
	{
		uint shift = orig_shift % 9;
		uint src   = MASK_OUT_ABOVE_8(*r_dst);
		uint res   = ROL_9(src | (XFLAG_AS_1() << 8), shift);

		USE_CYCLES(orig_shift<<CYC_SHIFT);

		FLAG_C = FLAG_X = res;
		res = MASK_OUT_ABOVE_8(res);

		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
		FLAG_N = NFLAG_8(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_8(*r_dst);
	FLAG_Z = MASK_OUT_ABOVE_8(*r_dst);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_r(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;

	if(orig_shift != 0)
	{
		uint shift = orig_shift % 17;
		uint src   = MASK_OUT_ABOVE_16(*r_dst);
		uint res   = ROL_17(src | (XFLAG_AS_1() << 16), shift);

		USE_CYCLES(orig_shift<<CYC_SHIFT);

		FLAG_C = FLAG_X = res >> 8;
		res = MASK_OUT_ABOVE_16(res);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_16(*r_dst);
	FLAG_Z = MASK_OUT_ABOVE_16(*r_dst);
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_32_r(M68KOPT_PARAMS)
{
#if M68K_USE_64_BIT

	uint*  r_dst = &DY;
	uint   orig_shift = DX & 0x3f;

	if(orig_shift != 0)
	{
		uint   shift = orig_shift % 33;
		uint64 src   = *r_dst;
		uint64 res   = src | (((uint64)XFLAG_AS_1()) << 32);

		res = ROL_33_64(res, shift);

		USE_CYCLES(orig_shift<<CYC_SHIFT);

		FLAG_C = FLAG_X = res >> 24;
		res = MASK_OUT_ABOVE_32(res);

		*r_dst = res;
		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_32(*r_dst);
	FLAG_Z = *r_dst;
	FLAG_V = VFLAG_CLEAR;

#else

	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 33;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32((ROL_33(src, shift) & ~(1 << (shift - 1))) | (XFLAG_AS_1() << (shift - 1)));
	uint new_x_flag = src & (1 << (32 - shift));

	if(orig_shift != 0)
		USE_CYCLES(orig_shift<<CYC_SHIFT);

	if(shift != 0)
	{
		*r_dst = res;
		FLAG_X = (new_x_flag != 0)<<8;
	}
	else
		res = src;
	FLAG_C = FLAG_X;
	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;

#endif
}


void m68k_op_roxl_16_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_roxl_16_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = p68k->mem.read16(ea);
	uint res = ROL_17(src | (XFLAG_AS_1() << 16), 1);

	FLAG_C = FLAG_X = res >> 8;
	res = MASK_OUT_ABOVE_16(res);

	m68ki_write_16(ea, res);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_rtd_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_010_PLUS(CPU_TYPE))
	{
		uint new_pc = m68ki_pull_32(p68k);

		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + MAKE_INT_16(OPER_I_16(p68k)));
		m68ki_jump(p68k, new_pc);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_rte_32(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr;
		uint new_pc;
		uint format_word;

		m68ki_rte_callback();		   /* auto-disable (see m68kcpu.h) */
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */

		if(CPU_TYPE_IS_000(CPU_TYPE))
		{
			new_sr = m68ki_pull_16(p68k);
			new_pc = m68ki_pull_32(p68k);
			m68ki_jump(p68k, new_pc);
			m68ki_set_sr(p68k, new_sr);

			CPU_INSTR_MODE = INSTRUCTION_YES;
			CPU_RUN_MODE = RUN_MODE_NORMAL;

			return;
		}

		if(CPU_TYPE_IS_010(CPU_TYPE))
		{
			format_word = p68k->mem.read16(REG_A[7]+6) >> 12;
			if(format_word == 0)
			{
				new_sr = m68ki_pull_16(p68k);
				new_pc = m68ki_pull_32(p68k);
				m68ki_fake_pull_16(p68k);	/* format word */
				m68ki_jump(p68k, new_pc);
				m68ki_set_sr(p68k, new_sr);
				CPU_INSTR_MODE = INSTRUCTION_YES;
				CPU_RUN_MODE = RUN_MODE_NORMAL;
				return;
			}
			CPU_INSTR_MODE = INSTRUCTION_YES;
			CPU_RUN_MODE = RUN_MODE_NORMAL;
			/* Not handling bus fault (9) */
			m68ki_exception_format_error(p68k);
			return;
		}

		/* Otherwise it's 020 */
rte_loop:
		format_word = p68k->mem.read16(REG_A[7]+6) >> 12;
		switch(format_word)
		{
			case 0: /* Normal */
				new_sr = m68ki_pull_16(p68k);
				new_pc = m68ki_pull_32(p68k);
				m68ki_fake_pull_16(p68k);	/* format word */
				m68ki_jump(p68k, new_pc);
				m68ki_set_sr(p68k, new_sr);
				CPU_INSTR_MODE = INSTRUCTION_YES;
				CPU_RUN_MODE = RUN_MODE_NORMAL;
				return;
			case 1: /* Throwaway */
				new_sr = m68ki_pull_16(p68k);
				m68ki_fake_pull_32(p68k);	/* program counter */
				m68ki_fake_pull_16(p68k);	/* format word */
				m68ki_set_sr_noint(p68k, new_sr);
				goto rte_loop;
			case 2: /* Trap */
				new_sr = m68ki_pull_16(p68k);
				new_pc = m68ki_pull_32(p68k);
				m68ki_fake_pull_16(p68k);	/* format word */
				m68ki_fake_pull_32(p68k);	/* address */
				m68ki_jump(p68k, new_pc);
				m68ki_set_sr(p68k, new_sr);
				CPU_INSTR_MODE = INSTRUCTION_YES;
				CPU_RUN_MODE = RUN_MODE_NORMAL;
				return;
		}
		/* Not handling long or short bus fault */
		CPU_INSTR_MODE = INSTRUCTION_YES;
		CPU_RUN_MODE = RUN_MODE_NORMAL;
		m68ki_exception_format_error(p68k);
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_rtm_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_020_VARIANT(CPU_TYPE))
	{
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		M68K_DO_LOG((M68K_LOG_FILEHANDLE "%s at %08x: called unimplemented instruction %04x (%s)\n",
					 m68ki_cpu_names[CPU_TYPE], ADDRESS_68K(REG_PC - 2), REG_IR,
					 m68k_disassemble_quick(ADDRESS_68K(REG_PC - 2))));
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_rtr_32(M68KOPT_PARAMS)
{
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_set_ccr(p68k, m68ki_pull_16(p68k));
	m68ki_jump(p68k, m68ki_pull_32(p68k));
}


void m68k_op_rts_32(M68KOPT_PARAMS)
{
	m68ki_trace_t0();				   /* auto-disable (see m68kcpu.h) */
	m68ki_jump(p68k, m68ki_pull_32(p68k));
}


void m68k_op_sbcd_8_rr(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - XFLAG_AS_1();

//  FLAG_V = ~res; /* Undefined V behavior */
	FLAG_V = VFLAG_CLEAR;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to assume cleared. */

	if(res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if(res > 0x99)
	{
		res += 0xa0;
		FLAG_X = FLAG_C = CFLAG_SET;
		FLAG_N = NFLAG_SET;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to follow carry. */
	}
	else
		FLAG_N = FLAG_X = FLAG_C = 0;

	res = MASK_OUT_ABOVE_8(res);

//  FLAG_V &= res; /* Undefined V behavior part II */
//  FLAG_N = NFLAG_8(res); /* Undefined N behavior */
	FLAG_Z |= res;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
}


void m68k_op_sbcd_8_mm_ax7(M68KOPT_PARAMS)
{
	uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - XFLAG_AS_1();

//  FLAG_V = ~res; /* Undefined V behavior */
	FLAG_V = VFLAG_CLEAR;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to return zero. */

	if(res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if(res > 0x99)
	{
		res += 0xa0;
		FLAG_X = FLAG_C = CFLAG_SET;
		FLAG_N = NFLAG_SET;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to follow carry. */
	}
	else
		FLAG_N = FLAG_X = FLAG_C = 0;

	res = MASK_OUT_ABOVE_8(res);

//  FLAG_V &= res; /* Undefined V behavior part II */
//  FLAG_N = NFLAG_8(res); /* Undefined N behavior */
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_sbcd_8_mm_ay7(M68KOPT_PARAMS)
{
	uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_AX_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - XFLAG_AS_1();

//  FLAG_V = ~res; /* Undefined V behavior */
	FLAG_V = VFLAG_CLEAR;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to return zero. */

	if(res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if(res > 0x99)
	{
		res += 0xa0;
		FLAG_X = FLAG_C = CFLAG_SET;
		FLAG_N = NFLAG_SET;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to follow carry. */
	}
	else
		FLAG_N = FLAG_X = FLAG_C = 0;

	res = MASK_OUT_ABOVE_8(res);

//  FLAG_V &= res; /* Undefined V behavior part II */
//  FLAG_N = NFLAG_8(res); /* Undefined N behavior */
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_sbcd_8_mm_axy7(M68KOPT_PARAMS)
{
	uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - XFLAG_AS_1();

//  FLAG_V = ~res; /* Undefined V behavior */
	FLAG_V = VFLAG_CLEAR;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to return zero. */

	if(res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if(res > 0x99)
	{
		res += 0xa0;
		FLAG_X = FLAG_C = CFLAG_SET;
		FLAG_N = NFLAG_SET;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to follow carry. */
	}
	else
		FLAG_N = FLAG_X = FLAG_C = 0;

	res = MASK_OUT_ABOVE_8(res);

//  FLAG_V &= res; /* Undefined V behavior part II */
//  FLAG_N = NFLAG_8(res); /* Undefined N behavior */
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_sbcd_8_mm(M68KOPT_PARAMS)
{
	uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_AX_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - XFLAG_AS_1();

//  FLAG_V = ~res; /* Undefined V behavior */
	FLAG_V = VFLAG_CLEAR;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to return zero. */

	if(res > 9)
		res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if(res > 0x99)
	{
		res += 0xa0;
		FLAG_X = FLAG_C = CFLAG_SET;
		FLAG_N = NFLAG_SET;	/* Undefined in Motorola's M68000PM/AD rev.1 and safer to follow carry. */
	}
	else
		FLAG_N = FLAG_X = FLAG_C = 0;

	res = MASK_OUT_ABOVE_8(res);

//  FLAG_V &= res; /* Undefined V behavior part II */
//  FLAG_N = NFLAG_8(res); /* Undefined N behavior */
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_st_8_d(M68KOPT_PARAMS)
{
	DY |= 0xff;
}


void m68k_op_st_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), 0xff);
}


void m68k_op_st_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), 0xff);
}


void m68k_op_st_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), 0xff);
}


void m68k_op_st_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), 0xff);
}


void m68k_op_st_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), 0xff);
}


void m68k_op_st_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), 0xff);
}


void m68k_op_st_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), 0xff);
}


void m68k_op_st_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), 0xff);
}


void m68k_op_st_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), 0xff);
}


void m68k_op_sf_8_d(M68KOPT_PARAMS)
{
	DY &= 0xffffff00;
}


void m68k_op_sf_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), 0);
}


void m68k_op_sf_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), 0);
}


void m68k_op_sf_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), 0);
}


void m68k_op_sf_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), 0);
}


void m68k_op_sf_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), 0);
}


void m68k_op_sf_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), 0);
}


void m68k_op_sf_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), 0);
}


void m68k_op_sf_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), 0);
}


void m68k_op_sf_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), 0);
}


void m68k_op_shi_8_d(M68KOPT_PARAMS)
{
	if(COND_HI())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_sls_8_d(M68KOPT_PARAMS)
{
	if(COND_LS())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_scc_8_d(M68KOPT_PARAMS)
{
	if(COND_CC())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_scs_8_d(M68KOPT_PARAMS)
{
	if(COND_CS())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_sne_8_d(M68KOPT_PARAMS)
{
	if(COND_NE())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_seq_8_d(M68KOPT_PARAMS)
{
	if(COND_EQ())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_svc_8_d(M68KOPT_PARAMS)
{
	if(COND_VC())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_svs_8_d(M68KOPT_PARAMS)
{
	if(COND_VS())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_spl_8_d(M68KOPT_PARAMS)
{
	if(COND_PL())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_smi_8_d(M68KOPT_PARAMS)
{
	if(COND_MI())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_sge_8_d(M68KOPT_PARAMS)
{
	if(COND_GE())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_slt_8_d(M68KOPT_PARAMS)
{
	if(COND_LT())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_sgt_8_d(M68KOPT_PARAMS)
{
	if(COND_GT())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_sle_8_d(M68KOPT_PARAMS)
{
	if(COND_LE())
	{
		DY |= 0xff;
		USE_CYCLES(CYC_SCC_R_TRUE);
		return;
	}
	DY &= 0xffffff00;
}


void m68k_op_shi_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_shi_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_HI() ? 0xff : 0);
}


void m68k_op_sls_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_sls_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_LS() ? 0xff : 0);
}


void m68k_op_scc_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scc_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_CC() ? 0xff : 0);
}


void m68k_op_scs_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_scs_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_CS() ? 0xff : 0);
}


void m68k_op_sne_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_sne_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_NE() ? 0xff : 0);
}


void m68k_op_seq_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_seq_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_EQ() ? 0xff : 0);
}


void m68k_op_svc_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svc_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_VC() ? 0xff : 0);
}


void m68k_op_svs_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_svs_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_VS() ? 0xff : 0);
}


void m68k_op_spl_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_spl_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_PL() ? 0xff : 0);
}


void m68k_op_smi_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_smi_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_MI() ? 0xff : 0);
}


void m68k_op_sge_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_sge_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_GE() ? 0xff : 0);
}


void m68k_op_slt_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_slt_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_LT() ? 0xff : 0);
}


void m68k_op_sgt_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sgt_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_GT() ? 0xff : 0);
}


void m68k_op_sle_8_ai(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_AI_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_pi(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PI_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_pi7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PI_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_pd(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_PD_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_pd7(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_A7_PD_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_di(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_DI_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_ix(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AY_IX_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_aw(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AW_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_sle_8_al(M68KOPT_PARAMS)
{
	m68ki_write_8(EA_AL_8(), COND_LE() ? 0xff : 0);
}


void m68k_op_stop(M68KOPT_PARAMS)
{
	if(FLAG_S)
	{
		uint new_sr = OPER_I_16(p68k);
		m68ki_trace_t0();			   /* auto-disable (see m68kcpu.h) */
		CPU_STOPPED |= STOP_LEVEL_STOP;
		m68ki_set_sr(p68k, new_sr);
		m68ki_remaining_cycles = 0;
		return;
	}
	m68ki_exception_privilege_violation(M68KOPT_PASSPARAMS);
}


void m68k_op_sub_8_er_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = MASK_OUT_ABOVE_8(DY);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_ai(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_AI_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_pi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_PI_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_pi7(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_A7_PI_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_pd(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_pd7(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_di(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_DI_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_ix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_IX_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_aw(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AW_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_al(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AL_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_pcdi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_PCDI_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_pcix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_PCIX_8(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_8_er_i(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_I_8(p68k);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = MASK_OUT_ABOVE_16(DY);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_a(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = MASK_OUT_ABOVE_16(AY);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_ai(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_AI_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_pi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_PI_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_pd(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_di(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_ix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AY_IX_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_aw(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AW_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_al(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_AL_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_pcdi(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_PCDI_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_pcix(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_PCIX_16(M68KOPT_PASSPARAMS);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_sub_16_er_i(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = OPER_I_16(p68k);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


#ifdef OPTIM68K_USEDIRECT68KASM
static inline void m68k_op_sub32_asm(M68KOPT_PARAMS, register uint src __asm("d0") )
{
    uint *pDX = (int  *)&DX;
    uint dst = *pDX;

    asm volatile(
                // xor because FLAG_Z is inverted. -> no need xor.b #4,%1
       "sub.l %1,%0\n\tmove.w ccr,%1"
       : "+d" (dst),"+d" (src) // out
       : // in
       : // trashed
       );
    // src d0 with ccr, dst with result.
    *pDX = dst;
    FLAG_Z = dst;

    asm volatile(
        "lsl.l #4,%0\n"
        "\tmove.l %0,%c[n_flag](%1)\n"
        "\tlsl.l #2,%0\n"
        "\tmove.l %0,%c[v_flag](%1)\n"
        "\tlsl.l #2,%0\n"
        "\tmove.l %0,%c[c_flag](%1)\n"
        "\tmove.l %0,%c[x_flag](%1)"
       :
       : "d"(src),"a"(p68k), // in
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [v_flag] "n" (offsetof(struct m68ki_cpu_core, v_flag)),
         [x_flag] "n" (offsetof(struct m68ki_cpu_core, x_flag)),
         [c_flag] "n" (offsetof(struct m68ki_cpu_core, c_flag))
       :
       );
//    src<<=4;
//    FLAG_N = src;
//    src<<=2;
//    FLAG_V = src;
//    src<<=2;
//    FLAG_X = FLAG_C = src;
}
#endif

void m68k_op_sub_32_er_d(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,DY);
#else
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_a(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,AY);
#else
	uint* r_dst = &DX;
	uint src = AY;
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_ai(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AY_AI_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_AI_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_pi(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AY_PI_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_PI_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_pd(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AY_PD_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_di(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AY_DI_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_DI_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_ix(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AY_IX_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AY_IX_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_aw(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AW_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AW_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_al(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_AL_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_AL_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_pcdi(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_PCDI_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_PCDI_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_pcix(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_PCIX_32(M68KOPT_PASSPARAMS));
#else
	uint* r_dst = &DX;
	uint src = OPER_PCIX_32(M68KOPT_PASSPARAMS);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_32_er_i(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM
    m68k_op_sub32_asm(M68KOPT_PASSPARAMS,OPER_I_32(p68k));
#else
	uint* r_dst = &DX;
	uint src = OPER_I_32(p68k);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);

	*r_dst = FLAG_Z;
#endif
}


void m68k_op_sub_8_re_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_8_re_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint src = MASK_OUT_ABOVE_8(DX);
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_sub_16_re_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_16();
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_16_re_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_16(p68k,regir);
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_16_re_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_16();
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_16_re_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_16();
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_16_re_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_16();
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_16_re_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_16();
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_16_re_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_16();
	uint src = MASK_OUT_ABOVE_16(DX);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_sub_32_re_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_sub_32_re_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_sub_32_re_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_sub_32_re_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_sub_32_re_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_sub_32_re_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_sub_32_re_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_32();
	uint src = DX;
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_suba_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(DY));
}


void m68k_op_suba_16_a(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(AY));
}


void m68k_op_suba_16_ai(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AY_AI_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_pi(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AY_PI_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_pd(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AY_PD_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_di(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AY_DI_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_ix(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AY_IX_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_aw(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AW_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_al(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_AL_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_pcdi(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_PCDI_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_pcix(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_PCIX_16(M68KOPT_PASSPARAMS)));
}


void m68k_op_suba_16_i(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - MAKE_INT_16(OPER_I_16(p68k)));
}


void m68k_op_suba_32_d(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - DY);
}


void m68k_op_suba_32_a(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - AY);
}


void m68k_op_suba_32_ai(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AY_AI_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_pi(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AY_PI_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_pd(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AY_PD_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_di(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AY_DI_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_ix(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AY_IX_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_aw(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AW_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_al(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_AL_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_pcdi(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_PCDI_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_pcix(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_PCIX_32(M68KOPT_PASSPARAMS));
}


void m68k_op_suba_32_i(M68KOPT_PARAMS)
{
	uint* r_dst = &AX;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - OPER_I_32(p68k));
}


void m68k_op_subi_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint src = OPER_I_8(p68k);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_subi_8_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_AI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_PI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_pi7(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_A7_PI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_pd7(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_DI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AY_IX_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AW_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_8_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_8(p68k);
	uint ea = EA_AL_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subi_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint src = OPER_I_16(p68k);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_subi_16_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_AI_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_16_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_PI_16(p68k,regir);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_16_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_PD_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_16_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_DI_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_16_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AY_IX_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_16_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AW_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_16_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_16(p68k);
	uint ea = EA_AL_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subi_32_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint src = OPER_I_32(p68k);
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	*r_dst = FLAG_Z;
}


void m68k_op_subi_32_ai(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_AI_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subi_32_pi(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_PI_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subi_32_pd(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_PD_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subi_32_di(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_DI_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subi_32_ix(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AY_IX_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subi_32_aw(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AW_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subi_32_al(M68KOPT_PARAMS)
{
	uint src = OPER_I_32(p68k);
	uint ea = EA_AL_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | FLAG_Z;
}


void m68k_op_subq_8_ai(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_AI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_pi(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_PI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_pi7(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_A7_PI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_pd(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_pd7(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_di(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_DI_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_ix(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_IX_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_aw(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_8_al(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_8(res);
	FLAG_Z = MASK_OUT_ABOVE_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	m68ki_write_8(ea, FLAG_Z);
}


void m68k_op_subq_16_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | FLAG_Z;
}


void m68k_op_subq_16_a(M68KOPT_PARAMS)
{
	uint* r_dst = &AY;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - ((((REG_IR >> 9) - 1) & 7) + 1));
}


void m68k_op_subq_16_ai(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_AI_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_16_pi(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_PI_16(p68k,regir);
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_16_pd(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_PD_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_16_di(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_DI_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_16_ix(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_IX_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_16_aw(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_16_al(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_16(res);
	FLAG_Z = MASK_OUT_ABOVE_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	m68ki_write_16(ea, FLAG_Z);
}


void m68k_op_subq_32_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	*r_dst = FLAG_Z;
}


void m68k_op_subq_32_a(M68KOPT_PARAMS)
{
	uint* r_dst = &AY;

	*r_dst = MASK_OUT_ABOVE_32(*r_dst - ((((REG_IR >> 9) - 1) & 7) + 1));
}


void m68k_op_subq_32_ai(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_AI_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_32_pi(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_PI_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_32_pd(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_PD_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_32_di(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_DI_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_32_ix(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AY_IX_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_32_aw(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AW_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subq_32_al(M68KOPT_PARAMS)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea = EA_AL_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = MASK_OUT_ABOVE_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	m68ki_write_32(ea, FLAG_Z);
}


void m68k_op_subx_8_rr(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = MASK_OUT_ABOVE_8(DY);
	uint dst = MASK_OUT_ABOVE_8(*r_dst);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
}


void m68k_op_subx_16_rr(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = MASK_OUT_ABOVE_16(DY);
	uint dst = MASK_OUT_ABOVE_16(*r_dst);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
}


void m68k_op_subx_32_rr(M68KOPT_PARAMS)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	*r_dst = res;
}


void m68k_op_subx_8_mm_ax7(M68KOPT_PARAMS)
{
	uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_subx_8_mm_ay7(M68KOPT_PARAMS)
{
	uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_AX_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_subx_8_mm_axy7(M68KOPT_PARAMS)
{
	uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_subx_8_mm(M68KOPT_PARAMS)
{
	uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
	uint ea  = EA_AX_PD_8();
	uint dst = p68k->mem.read8(ea);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_8(res);
	FLAG_X = FLAG_C = CFLAG_8(res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);

	res = MASK_OUT_ABOVE_8(res);
	FLAG_Z |= res;

	m68ki_write_8(ea, res);
}


void m68k_op_subx_16_mm(M68KOPT_PARAMS)
{
	uint src = OPER_AY_PD_16(M68KOPT_PASSPARAMS);
	uint ea  = EA_AX_PD_16();
	uint dst = p68k->mem.read16(ea);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_16(res);
	FLAG_X = FLAG_C = CFLAG_16(res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);

	res = MASK_OUT_ABOVE_16(res);
	FLAG_Z |= res;

	m68ki_write_16(ea, res);
}


void m68k_op_subx_32_mm(M68KOPT_PARAMS)
{
	uint src = OPER_AY_PD_32(M68KOPT_PASSPARAMS);
	uint ea  = EA_AX_PD_32();
	uint dst = p68k->mem.read32(ea);
	uint res = dst - src - XFLAG_AS_1();

	FLAG_N = NFLAG_32(res);
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);

	res = MASK_OUT_ABOVE_32(res);
	FLAG_Z |= res;

	m68ki_write_32(ea, res);
}


void m68k_op_swap_32(M68KOPT_PARAMS)
{
#ifdef OPTIM68K_USEDIRECT68KASM_REWRITEMOVES
    asm volatile(
        "\tand.w #7,%0\n"
        "\tlea %c[dar](%1,%0.w*4),a0\n"
        "\tmove.l (a0),d0\n"
        "\tswap d0\n"
        "\tmove.l d0,(a0)\n"
        "\tmove.l d0,%c[not_z_flag](%1)\n"
        "\trol.l #8,d0\n"
        "\tmove.l d0,%c[n_flag](%1)\n"
        :
        : "d"(regir), "a"(p68k),
         [dar] "n" (offsetof(struct m68ki_cpu_core, dar)),
         [n_flag] "n" (offsetof(struct m68ki_cpu_core, n_flag)),
         [not_z_flag] "n" (offsetof(struct m68ki_cpu_core, not_z_flag))
        :  "d0","a0"
        );
    /*
    move.l d2,-(sp)
	moveq #7,d0
	and.l d0,d2
	lea (a2,d2.l*4),a0
	move.l (a0),d0
	swap d0
	move.l d0,(a0)
	move.l d0,(80,a2)
	moveq #24,d1
	lsr.l d1,d0
	move.l d0,(76,a2)
	move.l (sp)+,d2
*/
#else


	uint* r_dst = &DY;

	FLAG_Z = MASK_OUT_ABOVE_32(*r_dst<<16);
	*r_dst = (*r_dst>>16) | FLAG_Z;

	FLAG_Z = *r_dst;
	FLAG_N = NFLAG_32(*r_dst);
#endif
//	FLAG_C = CFLAG_CLEAR;
//	FLAG_V = VFLAG_CLEAR;
}


void m68k_op_tas_8_d(M68KOPT_PARAMS)
{
	uint* r_dst = &DY;

	FLAG_Z = MASK_OUT_ABOVE_8(*r_dst);
	FLAG_N = NFLAG_8(*r_dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	*r_dst |= 0x80;
}


void m68k_op_tas_8_ai(M68KOPT_PARAMS)
{
	uint ea = EA_AY_AI_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_pi(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PI_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_pi7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PI_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_pd(M68KOPT_PARAMS)
{
	uint ea = EA_AY_PD_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_pd7(M68KOPT_PARAMS)
{
	uint ea = EA_A7_PD_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_di(M68KOPT_PARAMS)
{
	uint ea = EA_AY_DI_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_ix(M68KOPT_PARAMS)
{
	uint ea = EA_AY_IX_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_aw(M68KOPT_PARAMS)
{
	uint ea = EA_AW_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_tas_8_al(M68KOPT_PARAMS)
{
	uint ea = EA_AL_8();
	uint dst = p68k->mem.read8(ea);

	FLAG_Z = dst;
	FLAG_N = NFLAG_8(dst);
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
	m68ki_write_8(ea, dst | 0x80);
}


void m68k_op_trap(M68KOPT_PARAMS)
{
	/* Trap#n stacks exception frame type 0 */
	m68ki_exception_trapN(p68k, EXCEPTION_TRAP_BASE + (REG_IR & 0xf));	/* HJB 990403 */
}


void m68k_op_trapt(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapt_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapt_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapf(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapf_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapf_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traphi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_HI())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapls(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LS())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapcc(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_CC())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapcs(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_CS())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapne(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_NE())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapeq(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_EQ())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapvc(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_VC())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapvs(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_VS())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trappl(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_PL())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapmi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_MI())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapge(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_GE())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traplt(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LT())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapgt(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_GT())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traple(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LE())
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traphi_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_HI())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapls_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LS())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapcc_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_CC())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapcs_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_CS())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapne_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_NE())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapeq_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_EQ())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapvc_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_VC())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapvs_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_VS())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trappl_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_PL())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapmi_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_MI())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapge_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_GE())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traplt_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LT())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapgt_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_GT())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traple_16(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LE())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 2;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traphi_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_HI())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapls_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LS())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapcc_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_CC())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapcs_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_CS())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapne_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_NE())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapeq_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_EQ())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapvc_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_VC())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapvs_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_VS())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trappl_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_PL())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapmi_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_MI())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapge_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_GE())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traplt_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LT())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapgt_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_GT())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_traple_32(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		if(COND_LE())
		{
			m68ki_exception_trap(p68k, EXCEPTION_TRAPV);	/* HJB 990403 */
			return;
		}
		REG_PC += 4;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_trapv(M68KOPT_PARAMS)
{
	if(COND_VC())
	{
		return;
	}
	m68ki_exception_trap(p68k, EXCEPTION_TRAPV);  /* HJB 990403 */
}


void m68k_op_tst_8_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_8(DY);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_pi7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PI_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_pd7(M68KOPT_PARAMS)
{
	uint res = OPER_A7_PD_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_8(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_8_pcdi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_PCDI_8(M68KOPT_PASSPARAMS);

		FLAG_N = NFLAG_8(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_8_pcix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_PCIX_8(M68KOPT_PASSPARAMS);

		FLAG_N = NFLAG_8(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_8_i(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_I_8(p68k);

		FLAG_N = NFLAG_8(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_16_d(M68KOPT_PARAMS)
{
	uint res = MASK_OUT_ABOVE_16(DY);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_a(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = MAKE_INT_16(AY);

		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_16_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}
void krb_outrun_m68k_op_tst_16_di(M68KOPT_PARAMS)
{
    // original opcode for this call, have been redirected to other free opcode for patch.
    // so "regir" is fake, need to retablish right value.
    regir = 0x4a6d;
	uint res = OPER_AY_DI_16(M68KOPT_PASSPARAMS);
    if(res == 0)
    {
        // means busy wait, other cpu should work asap, force quitting execute loop just next.
   		//CPU_INT_CYCLES = m68k_ICount;
    	SET_CYCLES(0); // means, qut CPU2 loop asap and go treat other CPU without busy loop that hogs.
    }
	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_16_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}
// specific to outrun main cpu waiting other cpu.
void krb_outrun_m68k_op_tst_16_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_16(M68KOPT_PASSPARAMS);
    // if !=0, let go.
    if(res == 0)
    {
        // means busy wait, other cpu should work asap, force quitting execute loop just next.

    	SET_CYCLES(0);
    	//m68k_ICount = 0;
    }
	FLAG_N = NFLAG_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}

void m68k_op_tst_16_pcdi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_PCDI_16(M68KOPT_PASSPARAMS);

		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_16_pcix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_PCIX_16(M68KOPT_PASSPARAMS);

		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_16_i(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_I_16(p68k);

		FLAG_N = NFLAG_16(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_32_d(M68KOPT_PARAMS)
{
	uint res = DY;

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_a(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = AY;

		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_32_ai(M68KOPT_PARAMS)
{
	uint res = OPER_AY_AI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_pi(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_pd(M68KOPT_PARAMS)
{
	uint res = OPER_AY_PD_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_di(M68KOPT_PARAMS)
{
	uint res = OPER_AY_DI_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_ix(M68KOPT_PARAMS)
{
	uint res = OPER_AY_IX_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_aw(M68KOPT_PARAMS)
{
	uint res = OPER_AW_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_al(M68KOPT_PARAMS)
{
	uint res = OPER_AL_32(M68KOPT_PASSPARAMS);

	FLAG_N = NFLAG_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_CLEAR;
	FLAG_C = CFLAG_CLEAR;
}


void m68k_op_tst_32_pcdi(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_PCDI_32(M68KOPT_PASSPARAMS);

		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_32_pcix(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_PCIX_32(M68KOPT_PASSPARAMS);

		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_tst_32_i(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint res = OPER_I_32(p68k);

		FLAG_N = NFLAG_32(res);
		FLAG_Z = res;
		FLAG_V = VFLAG_CLEAR;
		FLAG_C = CFLAG_CLEAR;
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_unlk_32_a7(M68KOPT_PARAMS)
{
	REG_A[7] = p68k->mem.read32(REG_A[7]);
}


void m68k_op_unlk_32(M68KOPT_PARAMS)
{
	uint* r_dst = &AY;

	REG_A[7] = *r_dst;
	*r_dst = m68ki_pull_32(p68k);
}


void m68k_op_unpk_16_rr(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: DX and DY are reversed in Motorola's docs */
		uint src = DY;
		uint* r_dst = &DX;

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | (((((src << 4) & 0x0f00) | (src & 0x000f)) + OPER_I_16(p68k)) & 0xffff);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_unpk_16_mm_ax7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: AX and AY are reversed in Motorola's docs */
		uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
		uint ea_dst;

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + OPER_I_16(p68k);
		ea_dst = EA_A7_PD_8();
		m68ki_write_8(ea_dst, (src >> 8) & 0xff);
		ea_dst = EA_A7_PD_8();
		m68ki_write_8(ea_dst, src & 0xff);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_unpk_16_mm_ay7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: AX and AY are reversed in Motorola's docs */
		uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
		uint ea_dst;

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + OPER_I_16(p68k);
		ea_dst = EA_AX_PD_8();
		m68ki_write_8(ea_dst, (src >> 8) & 0xff);
		ea_dst = EA_AX_PD_8();
		m68ki_write_8(ea_dst, src & 0xff);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_unpk_16_mm_axy7(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		uint src = OPER_A7_PD_8(M68KOPT_PASSPARAMS);
		uint ea_dst;

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + OPER_I_16(p68k);
		ea_dst = EA_A7_PD_8();
		m68ki_write_8(ea_dst, (src >> 8) & 0xff);
		ea_dst = EA_A7_PD_8();
		m68ki_write_8(ea_dst, src & 0xff);
		return;
	}
	m68ki_exception_illegal(p68k);
}


void m68k_op_unpk_16_mm(M68KOPT_PARAMS)
{
	if(CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
	{
		/* Note: AX and AY are reversed in Motorola's docs */
		uint src = OPER_AY_PD_8(M68KOPT_PASSPARAMS);
		uint ea_dst;

		src = (((src << 4) & 0x0f00) | (src & 0x000f)) + OPER_I_16(p68k);
		ea_dst = EA_AX_PD_8();
		m68ki_write_8(ea_dst, (src >> 8) & 0xff);
		ea_dst = EA_AX_PD_8();
		m68ki_write_8(ea_dst, src & 0xff);
		return;
	}
	m68ki_exception_illegal(p68k);
}


/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */


