#include "cpu/exec/template-start.h"

#define instr jmp

static void concat(do_execute,direct)(uint32_t len)
{
	cpu.eip=op_src->val;
	swaddr_t newop=cpu.eip+len;
#if DATA_BYTE==2
	cpu.eip&=0xFFFF;
	newop&=0xFFFF;
#endif
	extern char *PrintAddressInFunction(swaddr_t,bool);//elf.c
	char *infun=PrintAddressInFunction(newop,true);
	if(infun)
		print_asm("jmp %x %s", newop,infun);
	else
		print_asm("jmp %x", newop);

}

static void concat(do_execute,relative)(uint32_t len) 
{
	op_src->val+=cpu.eip;
	concat(do_execute,direct)(len);
}

#if DATA_BYTE == 2 || DATA_BYTE ==4
make_instr_helper2(rm,direct)
#endif
make_instr_helper2(i,relative)

#include "cpu/exec/template-end.h"
