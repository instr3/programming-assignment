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
		case 0xd8:cpu.ds=cpu.eax;segname="ds";break;
		case 0xc0:cpu.es=cpu.eax;segname="es";break;
		case 0xd0:cpu.ss=cpu.eax;segname="ss";break;
	}
	print_asm("mov %%eax,%%%s",segname);
	return 2;
}