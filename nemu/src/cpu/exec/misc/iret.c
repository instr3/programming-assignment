#include "cpu/exec/helper.h"

make_helper(iret) {
	pop_data(cpu.eip,4);
	pop_data(cpu.cs,4);
	pop_data(cpu.eflags,4);
	--cpu.eip;
	print_asm("iret");
	return 1;
}
