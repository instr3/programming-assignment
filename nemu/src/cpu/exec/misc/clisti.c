#include "cpu/exec/helper.h"

make_helper(cli) {
	reg_flag_set(EFLAGS_IF,0);
	print_asm("cli");
	return 1;
}

make_helper(sti) {
	reg_flag_set(EFLAGS_IF,1);
	print_asm("sti");
	return 1;
}