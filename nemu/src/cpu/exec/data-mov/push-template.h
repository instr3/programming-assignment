#include "cpu/exec/template-start.h"

#define instr push
/*
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(push_reg_, SUFFIX)) {
	uint32_t regid = instr_fetch(eip, 1)-0x50;
	push_data(REG(regid),DATA_BYTE);
	print_asm("push %%%s", REG_NAME(regid));
	return 1;
}
#endif*/
static void do_execute() {
	if(cpu.esp>0xc0160000&&cpu.esp<0xc7000000)
	printf("%x\n",cpu.esp);
	push_data(op_src->val,ops_decoded.is_data_size_16?2:4);
	print_asm_template1();
}
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm)
make_instr_helper(r)
#endif
make_instr_helper(i)



#include "cpu/exec/template-end.h"
