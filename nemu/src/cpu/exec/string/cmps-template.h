#include "cpu/exec/template-start.h"

extern bool temp_cmps_matches;
make_helper(concat(cmps_, SUFFIX)) {
	REG(R_EDI)+=DATA_BYTE;
	REG(R_ESI)+=DATA_BYTE;
	temp_cmps_matches=MEM_R(REG(R_ESI))==MEM_R(REG(R_EDI));
	print_asm("cmps" str(SUFFIX));
	return 0;
}

#include "cpu/exec/template-end.h"
