#include "cpu/exec/helper.h"

make_helper(cld) {
	reg_flag_set(EFLAGS_DF,0);
	return 1;
}

make_helper(std) {
	reg_flag_set(EFLAGS_DF,1);
	return 1;
}