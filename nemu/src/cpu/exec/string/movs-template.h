#include "cpu/exec/template-start.h"


make_helper(concat(movs_, SUFFIX)) {
	MEM_W(reg_l(R_EDI),MEM_R(reg_l(R_ESI),SREG_DS),SREG_ES);
	string_advance(reg_l(R_EDI),DATA_BYTE);
	string_advance(reg_l(R_ESI),DATA_BYTE);
	print_asm("movs" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
