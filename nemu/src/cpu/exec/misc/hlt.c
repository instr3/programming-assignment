#include "cpu/exec/helper.h"
extern uint8_t i8259_query_intr();
make_helper(hlt)
{
	while(i8259_query_intr()==-1);
	print_asm("hlt");
	return 1;
}