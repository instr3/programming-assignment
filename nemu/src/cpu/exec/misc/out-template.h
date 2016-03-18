#include "cpu/exec/template-start.h"

extern void pio_write(ioaddr_t addr, size_t len, uint32_t data);
make_helper(concat(out_, SUFFIX)){
	pio_write(reg_w(R_EDX),DATA_BYTE,REG(R_EAX));
	print_asm("out" str(SUFFIX) " %%%s, (%%dx)", REG_NAME(R_EAX));
	return 1;
}
make_helper(concat(out_i_, SUFFIX)){
	uint8_t addr=instr_fetch(eip+1,1);
	pio_write(addr,DATA_BYTE,REG(R_EAX));
	print_asm("out" str(SUFFIX) " %%%s, $0x%X", REG_NAME(R_EAX), addr);
	return 2;
}

#include "cpu/exec/template-end.h"
