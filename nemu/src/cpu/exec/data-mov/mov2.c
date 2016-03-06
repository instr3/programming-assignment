//mov2: mov from/to CR0 or CR3
#include "cpu/exec/helper.h"

#define DATA_BYTE 4
#include "mov2-template.h"
#undef DATA_BYTE

make_helper(mov2_set_seg)
{
	uint8_t tmp = instr_fetch(eip + 1, 1);
	char *segname;
	switch(tmp)
	{
		void write_seg(uint8_t,uint16_t);
		case 0xd8:write_seg(SREG_DS,cpu.eax);segname="ds";break;
		case 0xc0:write_seg(SREG_ES,cpu.eax);segname="es";break;
		case 0xd0:write_seg(SREG_SS,cpu.eax);segname="ss";break;
	}
	print_asm("mov %%eax,%%%s",segname);
	return 2;
}