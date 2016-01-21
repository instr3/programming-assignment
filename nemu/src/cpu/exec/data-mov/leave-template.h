#include "cpu/exec/template-start.h"

#define instr leave

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(leave_, SUFFIX)) {
	cpu.esp=cpu.ebp;
#if DATA_BYTE == 2
	uint16_t res;
	pop_data(res,2);
	cpu.ebp = (cpu.ebp & 0xFFFF0000)|res;
#else //DATA_BYTE == 4
	pop_data(cpu.ebp,4);
#endif
	print_asm("leave");
	return 1;
}
#endif


#include "cpu/exec/template-end.h"
