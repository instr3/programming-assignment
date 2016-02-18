#include "nemu.h"


extern CPU_state cpu;
extern int totalRegisterCount;

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
	else if (regID == 8)//eip
	{
		printRegLine("eip", cpu.eip);
	}
	else if(regID == 9)//eflags
	{
		printf("%s\t\t0x%X\t\t", "eflags", cpu.eflags);
		printf("[ ");
		int i;
		for(i=0;i<31;++i)
		{
			if(reg_flag(i)&&regsflag[i][0]!='_')
			{
				printf("%s ",regsflag[i]);
			}
		}
		printf("]\n");
	}
}
void printSegmentRegistersInfo()
{
	printRegLine("cr0",cpu.cr0);
	printRegLine("gdtr.limit",cpu.gdtr_limit);
	printRegLine("gdtr.base",cpu.gdtr_base);
	printRegLine("cs",cpu.cs);
	printRegLine("ds",cpu.ds);
	printRegLine("es",cpu.es);
	printRegLine("ss",cpu.ss);
}
void printRegInfo()
{
	int i;
	for (i = 0; i < totalRegisterCount; ++i)
	{
		printRegInfoByID(i);
	}
	printSegmentRegistersInfo();
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
		putchar(' ');
		printMemoryByAddress(address);
		++i;
		address += sizeof(uint32_t);
	}
	putchar('\n');
}