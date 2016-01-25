#include "cpu/exec/template-start.h"

make_helper(concat(cmps_, SUFFIX)) {
	reg_l(R_EDI)+=DATA_BYTE;
	reg_l(R_ESI)+=DATA_BYTE;
	influence_zf(MEM_R(reg_l(R_ESI))-MEM_R(reg_l(R_EDI)));
	print_asm("cmps" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
