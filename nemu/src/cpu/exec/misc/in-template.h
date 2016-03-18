#include "cpu/exec/template-start.h"

extern uint32_t pio_read(ioaddr_t addr, size_t len);
make_helper(concat(in_, SUFFIX)){
	REG(R_EAX)=pio_read(reg_w(R_EDX),DATA_BYTE);
	print_asm("in" str(SUFFIX) " %%%s, (%%dx)", REG_NAME(R_EAX));
	return 1;
}
make_helper(concat(in_i_, SUFFIX)){
	uint8_t addr=instr_fetch(eip+1,1);
	REG(R_EAX)=pio_read(addr,DATA_BYTE);
	printf("in" str(SUFFIX) " %%%s, $0x%X", REG_NAME(R_EAX), addr);
	print_asm("in" str(SUFFIX) " %%%s, $0x%X", REG_NAME(R_EAX), addr);
	return 2;
}

#include "cpu/exec/template-end.h"
