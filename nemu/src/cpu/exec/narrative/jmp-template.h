#include "cpu/exec/template-start.h"

#define instr jmp
static void concat(do_execute,direct)(uint32_t len)
{
	cpu.eip=op_src->val;
	extern char *PrintAddressInFunction(swaddr_t);//elf.c
	char *infun=PrintAddressInFunction(cpu.eip+len);
	if(infun)
		print_asm("jmp %x %s", cpu.eip+len,infun);
	else
		print_asm("jmp %x", cpu.eip+len);

}

static void concat(do_execute,relative)(uint32_t len) 
{
	op_src->val+=cpu.eip;
	concat(do_execute,direct)(len);
}

make_instr_helper2(i,relative)
make_instr_helper2(rm,direct)

#include "cpu/exec/template-end.h"
