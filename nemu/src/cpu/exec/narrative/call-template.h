#include "cpu/exec/template-start.h"

#define instr call

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(call_i_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	push_data(cpu.eip+1+DATA_BYTE,4);
	cpu.eip+=addr;
	extern char *PrintAddressInFunction(swaddr_t);//elf.c
	char *infun=PrintAddressInFunction(cpu.eip);
	if(infun)
		print_asm("call 0x%x %s", cpu.eip,infun);
	else
		print_asm("call 0x%x", cpu.eip);

	return 1+DATA_BYTE;
}
#endif


#include "cpu/exec/template-end.h"
