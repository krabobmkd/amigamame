#ifndef _CPUSTATS_H_
#define _CPUSTATS_H_


void cpustats_reset();
void cpustats_add(unsigned int  ncpu,unsigned int adr, unsigned int instr);
void cpustats_log();

#endif
