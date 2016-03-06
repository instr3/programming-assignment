#include "cpu/exec/template-start.h"

make_helper(concat(cmps_, SUFFIX)) {
	DATA_TYPE result=MEM_R(reg_l(R_ESI),SREG_DS)-MEM_R(reg_l(R_EDI),SREG_ES);
	influence_zf(result);
	influence_pf(result);
	influence_sf(result);
	influence_of(result,MEM_R(reg_l(R_ESI),SREG_DS),~MEM_R(reg_l(R_EDI),SREG_ES));
	influence_cf(result,MEM_R(reg_l(R_ESI),SREG_DS)+1ull+(~MEM_R(reg_l(R_EDI),SREG_ES)),1);

	string_advance(reg_l(R_EDI),DATA_BYTE);
	string_advance(reg_l(R_ESI),DATA_BYTE);
	print_asm("cmps" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
