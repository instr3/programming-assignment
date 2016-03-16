#include "cpu/exec/template-start.h"

#define instr lidt

static void do_execute() {
	assert(0);
	DATA_TYPE addr=op_src->addr;
	cpu.gdtr_limit=lnaddr_read(addr,2);
	cpu.gdtr_base=lnaddr_read(addr+2,4);
	if(ops_decoded.is_data_size_16)
	{
		cpu.gdtr_base&=0xFFFFFF;
	}
	print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
