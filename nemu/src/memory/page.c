#include "common.h"
#include "nemu.h"
#include "memory/page.h"
#include "memory/memory.h"

extern CPU_state cpu;
hwaddr_t page_translate(lnaddr_t addr)
{
	if(cpu.cr0.paging==0)return addr;//Page not enabled
	linear_paged_addr_t tmp;
	PTE pte;
	tmp.val=addr;
	hwaddr_t base;
	base=cpu.cr3.page_directory_base;
	fflush(stdout);
	pte.val=hwaddr_read(base+tmp.dir*4,4);
	printf("0x%x*4+0x%x\n",tmp.dir,base);
	printf("*=0x%x\n",pte.val);
	assert(pte.present);
	base=pte.page_frame;
	pte.val=hwaddr_read(base+tmp.page*4,4);
	assert(pte.present);
	return (pte.page_frame<<PAGE_OFFSET_LEN)+tmp.offset;
}