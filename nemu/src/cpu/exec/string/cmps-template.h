#include "cpu/exec/template-start.h"

make_helper(concat(cmps_, SUFFIX)) {
	DATA_TYPE result=MEM_R(reg_l(R_ESI))-MEM_R(reg_l(R_EDI));
	influence_zf(result);
	influence_pf(result);
	influence_sf(result);
	influence_of(result,MEM_R(reg_l(R_ESI)),~MEM_R(reg_l(R_EDI)));
	influence_cf(result,MEM_R(reg_l(R_ESI))+1ull+(~MEM_R(reg_l(R_EDI))),1);

	reg_l(R_EDI)+=DATA_BYTE;
	reg_l(R_ESI)+=DATA_BYTE;
	print_asm("cmps" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
