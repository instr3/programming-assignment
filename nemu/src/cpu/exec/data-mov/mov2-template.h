#include "cpu/exec/template-start.h"

#define instr mov2

static void do_execute() {
	if(ops_decoded.opcode==0x120)
	{
		OPERAND_W(op_src, cpu.cr0.val);
		print_asm("mov %%cr0 %s",op_src->str);
	}
	else
	{
		printf("%x",ops_decoded.opcode);
		fflush(stdout);
		assert(ops_decoded.opcode==0x122);
		cpu.cr0.val=op_src->val;
		//TODO: into vitrual mode
		print_asm("mov %s %%cr0",op_src->str);
	}

}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"


