#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jmp-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(jmp_rm)
make_helper_v(jmp_i)

make_helper(ljmp)
{
	cpu.eip = instr_fetch(eip + 1, 4);
	cpu.cs = instr_fetch(eip + 5, 2);
	print_asm("ljmp $0x%x,$0x%x",(uint32_t)cpu.cs,cpu.eip);
	cpu.eip -= 7;
	return 7;
}