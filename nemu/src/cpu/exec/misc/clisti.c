#include "cpu/exec/helper.h"

make_helper(cli) {
	reg_flag_set(EFLAGS_IF,0);
	return 1;
}

make_helper(sti) {
	reg_flag_set(EFLAGS_IF,1);
	return 1;
}