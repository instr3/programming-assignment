#include "cpu/exec/template-start.h"

#define instr push

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(push_reg_, SUFFIX)) {
	uint32_t regid = instr_fetch(eip, 1)-0x50;
	push_data(REG(regid),4);
	return 1;
}
#endif


#include "cpu/exec/template-end.h"
