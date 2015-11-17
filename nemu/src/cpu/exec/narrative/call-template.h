#include "cpu/exec/template-start.h"

#define instr call

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(call_i_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	push_data(cpu.eip,4);
	cpu.eip+=5+addr;

	print_asm("call 0x%x", cpu.eip);
	return 5;
}
#endif


#include "cpu/exec/template-end.h"
