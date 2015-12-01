#include "cpu/exec/template-start.h"

#define instr call

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(call_i_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	push_data(cpu.eip+1+DATA_BYTE,4);
	cpu.eip+=addr;
	extern char *PrintAddressInFunction(swaddr_t);//elf.c
	char *infun=PrintAddressInFunction(cpu.eip+1+DATA_BYTE);
	if(infun)
		print_asm("call %x %s", cpu.eip+1+DATA_BYTE,infun);
	else
		print_asm("call %x", cpu.eip+1+DATA_BYTE);

	return 1+DATA_BYTE;
}
#endif


#include "cpu/exec/template-end.h"
