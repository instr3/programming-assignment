#include "cpu/exec/template-start.h"


make_helper(concat(stos_, SUFFIX)) {
	MEM_W(reg_l(R_EDI),REG(R_EAX),SREG_DS);
	string_advance(reg_l(R_EDI),DATA_BYTE);
	print_asm("stos" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
