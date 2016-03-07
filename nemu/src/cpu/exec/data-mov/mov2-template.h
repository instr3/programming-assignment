//mov2: mov from/to CR0 or CR3
#include "cpu/exec/template-start.h"

#define instr mov2

static void do_execute() {
	uint8_t destcode=instr_fetch(cpu.eip+2,1);
	assert(destcode==0xc0||destcode==0xd8);//CR0 or CR3 concerned.
	char* destname=destcode==0xc0?"%cr0":"%cr3";
	if(ops_decoded.opcode==0x120)
	{
		OPERAND_W(op_src, destcode==0xc0?cpu.cr0.val:cpu.cr3.val);
		print_asm("mov %s %s",destname,op_src->str);
	}
	else
	{
		//printf("%x",ops_decoded.opcode);
		//fflush(stdout);
		assert(ops_decoded.opcode==0x122);
		if(destcode==0xc0)
			cpu.cr0.val=op_src->val;
		else
			cpu.cr3.val=op_src->val;
		//TODO: into vitrual mode
		print_asm("mov %s %s",op_src->str,destname);
	}

}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"


