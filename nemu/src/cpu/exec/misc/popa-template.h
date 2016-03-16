#include "cpu/exec/template-start.h"

make_helper(concat(popa_, SUFFIX)){
	DATA_TYPE throwaway;
	pop_data(REG(R_EDI),DATA_BYTE);
	pop_data(REG(R_ESI),DATA_BYTE);
	pop_data(REG(R_EBP),DATA_BYTE);
	pop_data(throwaway,DATA_BYTE);
	pop_data(REG(R_EBX),DATA_BYTE);
	pop_data(REG(R_EDX),DATA_BYTE);
	pop_data(REG(R_ECX),DATA_BYTE);
	pop_data(REG(R_EAX),DATA_BYTE);
	throwaway=throwaway;
	print_asm("popa");
	return 1;
}

#include "cpu/exec/template-end.h"
