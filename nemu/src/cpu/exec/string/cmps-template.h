#include "cpu/exec/template-start.h"

extern bool temp_cmps_matches;
make_helper(concat(cmps_, SUFFIX)) {
	reg_l(R_EDI)+=DATA_BYTE;
	reg_l(R_ESI)+=DATA_BYTE;
	temp_cmps_matches=MEM_R(reg_l(R_ESI))==MEM_R(reg_l(R_EDI));
	print_asm("cmps" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
