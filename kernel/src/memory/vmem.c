#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

/* Use the function to get the start address of user page directory. */
inline PDE* get_updir();
inline PDE* get_kpdir();
//static PTE screenpage[NR_PTE] align_to_page;//Second Level Page

void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	/*PDE* pde=get_updir()+0x0;//Create First Level Page At dir=0
	pde->val=make_pde(va_to_pa(screenpage));
	uint32_t start_pos=0xa0000;
	//Create Second Level Page Of Range [0xa0000, 0xa0000 + SCR_SIZE) 
	for(;start_pos<0xa0000 + SCR_SIZE;start_pos+=PAGE_SIZE)
		screenpage[start_pos/PAGE_SIZE].val=make_pte(start_pos);
	//set_bp();*/
	//Instant copy version
	memcpy(get_updir(), get_kpdir(), sizeof(PDE));
	//panic("please implement me");
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

