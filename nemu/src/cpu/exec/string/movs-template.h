#include "cpu/exec/template-start.h"


make_helper(concat(movs_, SUFFIX)) {
	MEM_W(reg_l(R_EDI),MEM_R(reg_l(R_ESI)));
	reg_l(R_EDI)+=DATA_BYTE;
	reg_l(R_ESI)+=DATA_BYTE;
	print_asm("movs" str(SUFFIX));
	return 0;
}

#include "cpu/exec/template-end.h"
