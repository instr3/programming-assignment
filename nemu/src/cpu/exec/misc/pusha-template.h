#include "cpu/exec/template-start.h"

make_helper(concat(pusha_, SUFFIX)){
	DATA_TYPE temp=REG(R_ESP);
	push_data(REG(R_EAX),DATA_BYTE);
	push_data(REG(R_ECX),DATA_BYTE);
	push_data(REG(R_EDX),DATA_BYTE);
	push_data(REG(R_EBX),DATA_BYTE);
	push_data(temp,DATA_BYTE);
	push_data(REG(R_EBP),DATA_BYTE);
	push_data(REG(R_ESI),DATA_BYTE);
	push_data(REG(R_EDI),DATA_BYTE);
	print_asm_template1();
	print_asm("pusha");
	return 1;
}

#include "cpu/exec/template-end.h"
