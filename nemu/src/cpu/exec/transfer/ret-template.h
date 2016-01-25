#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(ret_, SUFFIX)) {
#if DATA_BYTE == 2
	uint16_t res;
	pop_data(res,2);
	cpu.eip = res;
	--cpu.eip;
#else //DATA_BYTE == 4
	pop_data(cpu.eip,4);
	--cpu.eip;
#endif
	print_asm("ret");
	return 1;
}
make_helper(concat(ret_i_, SUFFIX)) {
#if DATA_BYTE == 2
	uint16_t res;
	pop_data(res,2);
	cpu.eip = res;
	--cpu.eip;
#else //DATA_BYTE == 4
	pop_data(cpu.eip,4);
	--cpu.eip;
#endif
	uint16_t imm=instr_fetch(cpu.eip+1,2);
	cpu.esp+=imm;
	print_asm("ret $0x%x", imm);
	return 1;
}
#endif


#include "cpu/exec/template-end.h"
