#include "cpu/exec/template-start.h"

#define instr call

#if DATA_BYTE == 2 || DATA_BYTE ==4
static void concat(do_execute,direct)(uint32_t len)
{
	push_data(cpu.eip+len,DATA_BYTE);
	cpu.eip=op_src->val-len;
#if DATA_BYTE==2
	cpu.eip&=0xFFFF;
#endif
#ifdef DEBUG
	swaddr_t newop=cpu.eip+len;
#if DATA_BYTE==2
	newop&=0xFFFF;
#endif
	extern char *PrintAddressInFunction(swaddr_t,bool);//elf.c
	char *infun=PrintAddressInFunction(newop,true);
	//if(newop!=0xc01004b0&&newop!=0xc01003b0){
	if(infun)
		print_asm("call %x %s", newop,infun);
	else
		print_asm("call %x", newop);
	//}
#endif

}

static void concat(do_execute,relative)(uint32_t len) 
{
	op_src->val+=cpu.eip+len;
	concat(do_execute,direct)(len);
}

make_instr_helper2(rm,direct)
make_instr_helper2(i,relative)
#endif

#include "cpu/exec/template-end.h"
