#include "cpu/exec/template-start.h"

#define instr call

#if DATA_BYTE == 2 || DATA_BYTE ==4
static void concat(do_execute,direct)(uint32_t len)
{
	push_data(cpu.eip+len,DATA_BYTE);
	cpu.eip=op_src->val;
	swaddr_t newop=cpu.eip+len;
#if DATA_BYTE==2
	cpu.eip&=0xFFFF;
	newop&=0xFFFF;
#endif
	extern char *PrintAddressInFunction(swaddr_t);//elf.c
	char *infun=PrintAddressInFunction(newop);
	if(infun)
		print_asm("call %x %s", newop,infun);
	else
		print_asm("call %x", newop);

}

static void concat(do_execute,relative)(uint32_t len) 
{
	op_src->val+=cpu.eip;
	concat(do_execute,direct)(len);
}

make_instr_helper2(rm,direct)
make_instr_helper2(i,relative)
#endif

#include "cpu/exec/template-end.h"
