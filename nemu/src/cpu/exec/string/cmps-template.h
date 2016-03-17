#include "cpu/exec/template-start.h"

make_helper(concat(cmps_, SUFFIX)) {
	DATA_TYPE dest=MEM_R(reg_l(R_ESI),SREG_DS),src=MEM_R(reg_l(R_EDI),SREG_ES);

	DATA_TYPE result=dest-src;
	//printf("Compare:[%x]-[%x]\n",MEM_R(reg_l(R_ESI)),MEM_R(reg_l(R_EDI)));
	influence_zf(result);
	influence_pf(result);
	influence_sf(result);
	influence_of(result,dest,~src);
	influence_cf(dest+1ull+(DATA_TYPE)~src,1);
	string_advance(reg_l(R_EDI),DATA_BYTE);
	string_advance(reg_l(R_ESI),DATA_BYTE);
	print_asm("cmps" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
