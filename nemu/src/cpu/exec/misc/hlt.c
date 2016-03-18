#include "cpu/exec/helper.h"
extern uint8_t i8259_query_intr();
make_helper(hlt)
{
	print_asm("hlt");
	if(!(cpu.INTR & reg_flag(EFLAGS_IF)))return 0;
	return 1;
}