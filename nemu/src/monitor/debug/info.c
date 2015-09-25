#include "nemu.h"


CPU_state cpu;
int totalRegisterCount;

inline void printRegLine(const char *regName, uint32_t regValue)
{
	printf("%s\t\t0x%X\t\t%u\n", regName, regValue, regValue);
}

void printRegInfoByID(uint32_t regID)
{
	assert(regID < totalRegisterCount);
	if (regID < 8)//General Registers
	{
		printRegLine(regsl[regID], reg_l(regID));
	}
	if (regID == 8)//eip
	{
		printRegLine("eip", cpu.eip);
	}
}
void printRegInfo()
{
	int i;
	for (i = 0; i < totalRegisterCount; ++i)
	{
		printRegInfoByID(i);
	}
}