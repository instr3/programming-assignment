#include "cpu/exec/helper.h"

make_helper(cld) {
	reg_flag_set(EFLAGS_DF,0);
	return 1;
}
