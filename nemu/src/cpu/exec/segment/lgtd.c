#include "cpu/exec/helper.h"

make_helper(lgtd) {
	swaddr_t addr=swaddr_read(cpu.eip+3,4,NO_SREG);
	cpu.gdtr_limit=swaddr_read(addr,2,NO_SREG);
	cpu.gdtr_base=swaddr_read(addr+2,4,NO_SREG);
	if(ops_decoded.is_data_size_16)
	{
		cpu.gdtr_base&=0xFFFFFF;
	}
	print_asm("lgtd 0x%X",cpu.gdtr_base);
	return 6;
}