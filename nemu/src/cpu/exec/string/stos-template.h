#include "cpu/exec/template-start.h"


make_helper(concat(stos_, SUFFIX)) {
	MEM_W(REG(R_EDI),MEM_R(REG(R_EAX)));
	REG(R_EDI)+=DATA_BYTE;
	print_asm("stos" str(SUFFIX));
	return 0;
}

#include "cpu/exec/template-end.h"
