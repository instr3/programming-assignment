#include "cpu/exec/template-start.h"


make_helper(concat(movs_, SUFFIX)) {
	MEM_W(REG(R_EDI),MEM_R(REG(R_ESI)));
	REG(R_EDI)+=DATA_BYTE;
	REG(R_ESI)+=DATA_BYTE;
	return 0;
}

#include "cpu/exec/template-end.h"