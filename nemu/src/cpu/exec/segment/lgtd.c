#include "cpu/exec/helper.h"

make_helper(lgtd) {
	cpu.gdtr_limit=swaddr_read(cpu.eip+1,4);
	cpu.gdtr_base=swaddr_read(cpu.eip+5,2);
	if(ops_decoded.is_data_size_16)
	{
		cpu.gdtr_base&=0xFFFFFF;
	}
	print_asm("lgtd");
	return 7;
}