#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
/*
31                  23                  15               7             0
+-------------------+---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                   |V|R| |N|I O|O|D|I|T|S|Z| |A| |P| |C|
| 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | | |0| |   | | | | | | |0| |0| |1| |
|                                   |M|F| |T|P L|F|F|F|F|F|F| |F| |F| |F|
+-------------------+---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
const char *regsflag[]={
	"CF","_1","PF","_0","AF","_0","ZF","SF",
	"TF","IF","DF","OF","OL","IP","NT","_0",
	"RF","VM","_0","_0","_0","_0","_0","_0",
	"_0","_0","_0","_0","_0","_0","_0","_0"};
int totalRegisterCount = 10;
uint32_t GetFlagByName(char *flag,bool *success)
{
	int i;
	for (i = 0; i < 32; ++i)
	{
		if (strcmp(flag, regsflag[i]) == 0)
		{
			*success=true;
			return reg_flag(i);
		}
	}
	*success=false;
	return 0;

}

uint32_t GetRegByName(char *reg,bool *success)
{
	if (strcmp(reg, "eip") == 0)
	{
		*success = true;
		return cpu.eip;
	}
	if (strcmp(reg, "eflags") == 0)
	{
		*success = true;
		return cpu.eflags;
	}
	int i;
	for (i = 0; i < 8; ++i)
	{
		if (strcmp(reg, regsl[i]) == 0)
		{
			*success = true;
			return reg_l(i);
		}
		else if (strcmp(reg, regsw[i]) == 0)
		{
			*success = true;
			return reg_w(i);
		}
		else if (strcmp(reg, regsb[i]) == 0)
		{
			*success = true;
			return reg_b(i);
		}
	}
	return GetFlagByName(reg,success);
}


//Cache for the invisible part of the segment registers.
void seg_cache(uint8_t sreg){
	segmentselector_t seg;
	struct invisible_part *inv;
	switch(sreg)
	{
		case SREG_CS:seg.selector=cpu.cs;inv=&cpu.cs_inv;break;
		case SREG_DS:seg.selector=cpu.ds;inv=&cpu.ds_inv;break;
		case SREG_ES:seg.selector=cpu.es;inv=&cpu.es_inv;break;
		case SREG_SS:seg.selector=cpu.ss;inv=&cpu.ss_inv;break;
		default: assert(0);
	}
	gdtitem_t gdt;
	lnaddr_t address=(uint32_t)seg.index*8+cpu.gdtr_base;
	assert(address<cpu.gdtr_limit+cpu.gdtr_base);
	gdt.item=lnaddr_read(address,4)+((uint64_t)lnaddr_read(address+4,4)<<32);
	inv->base=gdt.seg_base_0_15+((uint32_t)gdt.seg_base_16_23<<16)+((uint32_t)gdt.seg_base_24_31<<24);
	inv->limit=gdt.seg_limit_0_15+((uint32_t)gdt.seg_limit_16_19<<16);
	//Todo : Test
	//return base+addr;
}

//Cache-enabled write segment register
void write_seg(uint8_t sreg,uint16_t data)
{

	switch(sreg)
	{
		case SREG_CS:cpu.cs=data;break;
		case SREG_DS:cpu.ds=data;break;
		case SREG_ES:cpu.es=data;break;
		case SREG_SS:cpu.ss=data;break;
		default: assert(0);
	}
	seg_cache(sreg);
}

void reg_test() {
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;

	int i;
	for(i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);

	assert(eip_sample == cpu.eip);
}

