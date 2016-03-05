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
		case 0xd8:write_seg(cpu.ds,cpu.eax);segname="ds";break;
		case 0xc0:write_seg(cpu.es,cpu.eax);segname="es";break;
		case 0xd0:write_seg(cpu.ss,cpu.eax);segname="ss";break;
	}
	print_asm("mov %%eax,%%%s",segname);
	return 2;
}