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
void init_eflags()
{
	cpu.eflags=0;
	reg_flag_set(1);//_1
	reg_flag_set(EFLAGS_CF);
}

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

