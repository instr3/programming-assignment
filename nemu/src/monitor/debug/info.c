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

inline void printMemoryByAddress(uint32_t address)
{
	printf("0x%.8X", swaddr_read(address, sizeof(uint32_t)));
}
inline void printMemoryPositionByAddress(uint32_t address)
{
	printf("0x%.8X:", address);
}
inline void printMemoryInfo(uint32_t address, int len)
{
	int i=0;
	while (i < len)
	{
		if (i % 8 == 0)
		{
			if (i)putchar('\n');
			printMemoryPositionByAddress(address);
		}
		putchar('\t');
		printMemoryByAddress(address);
		++i;
		address += sizeof(uint32_t);
	}
	putchar('\n');
}