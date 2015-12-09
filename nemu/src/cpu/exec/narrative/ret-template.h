#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(ret_, SUFFIX)) {
#if DATA_BYTE == 2
	uint16_t res;
	pop_data(res,2);
	--res;
	cpu.eip = res;
	--cpu.eip;
#else //DATA_BYTE == 4
	pop_data(cpu.eip,4);
	--cpu.eip;
#endif
	print_asm("ret");
	return 1;
}
#endif


#include "cpu/exec/template-end.h"
