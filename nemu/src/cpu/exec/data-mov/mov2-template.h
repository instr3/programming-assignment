#include "cpu/exec/template-start.h"

#define instr mov2

static void do_execute() {
	if(ops_decoded.opcode==0x20)
	{
		OPERAND_W(op_src, cpu.cr0);
		print_asm("mov cr0 %%%s",op_src->str);
	}
	else
	{
		assert(ops_decoded.opcode==0x22);
		cpu.cr0=op_src->val;
		print_asm("mov %%%s cr0",op_src->str);
	}

}

make_instr_helper(r)

#include "cpu/exec/template-end.h"


