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
	cpu.cs = instr_fetch(eip + 1, 2);
	cpu.eip = instr_fetch(eip + 3, 4);
	cpu.eip -= 7;
	print_asm("ljmp $0x%x,$0x%x",(uint32_t)cpu.cs,cpu.eip);
	return 7;
}