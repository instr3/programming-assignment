#include "common.h"
#include "nemu.h"
extern CPU_state cpu;
void printbtFormat(int id,swaddr_t addr,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t a4)
{
	extern char *PrintAddressInFunction(swaddr_t,bool);
	char *addinfun=PrintAddressInFunction(addr,false);
	printf("#%d\t0x%.8x in %s (%u,%u,%u,%u)\n",id,addr,addinfun,a1,a2,a3,a4);
}
void printbt()
{
	swaddr_t last=cpu.eip;
	swaddr_t ebp;
	int pid=0;
	for(ebp=cpu.ebp;ebp;ebp=swaddr_read(ebp,4))
	{
		printbtFormat(pid++,last,swaddr_read(ebp+8,4),swaddr_read(ebp+12,4),swaddr_read(ebp+16,4),swaddr_read(ebp+20,4));
		last=swaddr_read(ebp+4,4);
	}
}