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
	base=cpu.cr3.val & 0xfffff000;//Get PAGE DIRECTORY start position
	pte.val=hwaddr_read(base+tmp.dir*4,4);
	//printf("0x%x*4+0x%x\n",tmp.dir,base);
	//printf("*=0x%x\n",pte.val);
	//fflush(stdout);
	assert(pte.present);
	base=pte.page_frame << PAGE_OFFSET_LEN;
	pte.val=hwaddr_read(base+tmp.page*4,4);
	assert(pte.present);
	return (pte.page_frame<<PAGE_OFFSET_LEN)+tmp.offset;
}
void page_info(lnaddr_t addr)
{
	printf("=======================\nlnaddr:\t0x%X\n",addr);
	if(cpu.cr0.paging==0)
	{
		printf("Note: Paging not enabled!\n");
		printf("hwaddr:\t0x%X",addr);
	}
	linear_paged_addr_t tmp;
	PTE pte;
	tmp.val=addr;
	printf("dir:\t0x%X\tpage:\t0x%X\toffset:\t0x%X\n",tmp.dir,tmp.page,tmp.offset);
	hwaddr_t base;
	base=cpu.cr3.val & 0xfffff000;//Get PAGE DIRECTORY start position
	pte.val=hwaddr_read(base+tmp.dir*4,4);
	printf("First Level Paging:\n");
	printf("Present:\t%c\n",pte.present?'Y':'N');
	if(!pte.present)return;
	printf("Frame:\t0x%X",pte.page_frame);
	base=pte.page_frame << PAGE_OFFSET_LEN;
	pte.val=hwaddr_read(base+tmp.page*4,4);
	printf("Second Level Paging:\n");
	printf("Present:\t%c\n",pte.present?'Y':'N');
	if(!pte.present)return;
	printf("Frame:\t0x%X",pte.page_frame);
	printf("hwaddr:\t0x%X",(pte.page_frame<<PAGE_OFFSET_LEN)+tmp.offset);

}