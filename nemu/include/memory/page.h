#ifndef __PAGE_H__
#define __PAGE_H__

#include "common.h"
#include "../../lib-common/x86-inc/mmu.h"
#define PAGE_OFFSET_LEN 12
#define PAGING_MASK ((1<<PAGE_OFFSET_LEN)-1)
#define PAGE_DIR_LEN 10
#define PAGE_TABLE_LEN 10
typedef union linearaddress
{
	struct
	{
		uint32_t offset:12;
		uint32_t page:10;
		uint32_t dir:10;
	};
	uint32_t val;
} linear_paged_addr_t;
hwaddr_t page_translate(lnaddr_t);
void page_info(lnaddr_t);
#endif