#include "common.h"
#include "nemu.h"
#include "memory/page.h"
#include "memory/segment.h"
#include "device/mmio.h"
uint8_t simple_memory[1<<23];
//#define swaddr_read simple_read
//#define swaddr_write simple_write
uint32_t simple_read(hwaddr_t addr, size_t len) {
	if(addr>=0xC0000000)addr-=0xC0000000;
	int map_no=is_mmio(addr);
	if(map_no!=-1)return mmio_read(addr,len,map_no);
	printf("%X\n",addr);
	assert(addr<(1<<23));
	if(len==4)return *(uint32_t *)(simple_memory+addr);
	if(len==1)return simple_memory[addr];
	return *(uint16_t *)(simple_memory+addr);
}
void simple_write(swaddr_t addr, size_t len, uint32_t data) {
	if(addr>=0xC0000000)addr-=0xC0000000;
	int map_no=is_mmio(addr);
	if(map_no!=-1){mmio_write(addr,len,data,map_no);return;}
	assert(addr<(1<<23));
	if(len==4)*(uint32_t *)(simple_memory+addr)=data;
	if(len==1)simple_memory[addr]=data;
	*(uint16_t *)(simple_memory+addr)=data;
}

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

//Test if cache is enabled
//Remove the #define in common.h
#ifdef USE_CACHE

//Create Second Level Cache
#define OFFSET_BITS 6
#define BID_BITS 12 //Block ID Bits = 4MB/16B/16Ways
#define WAY_NUM 16
#define CACHE_WRITE_BACK_AND_WRITE_TRHOUGH //Define the Write_back feature
#define CACHE_ID cache2
//Define the next level memory interface
#define slower_read(...) dram_read(__VA_ARGS__)
#define slower_write(...) dram_write(__VA_ARGS__)

#include "cache-template.h"

#undef OFFSET_BITS
#undef BID_BITS
#undef WAY_NUM
#undef CACHE_WRITE_BACK_AND_WRITE_ALLOCATE
#undef CACHE_ID
#undef slower_read
#undef slower_write

//Create First Level Cache
#define OFFSET_BITS 6
#define BID_BITS 7 
#define WAY_NUM 8
#define CACHE_ID cache1
//Define the next level memory interface
#define slower_read(...) cache2.read(&cache2,__VA_ARGS__)
#define slower_write(...) cache2.write(&cache2,__VA_ARGS__)

#include "cache-template.h"

#undef OFFSET_BITS
#undef BID_BITS
#undef WAY_NUM
#undef CACHE_ID
#undef slower_read
#undef slower_write

void init_caches()
{
	cache1_init(&cache1);
	cache2_init(&cache2);
}
void debug_cache_address(hwaddr_t addr)
{
	cache1.debug(&cache1,addr);
	cache2.debug(&cache2,addr);
}
#endif

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	//return simple_read(addr,len);
	int map_no=is_mmio(addr);
	if(map_no!=-1)return mmio_read(addr,len,map_no);
#ifdef USE_CACHE
	return cache1.read(&cache1,addr,len) & (~0u >> ((4 - len) << 3));
#else
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
#endif
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	//simple_write(addr,len,data);return;
	int map_no=is_mmio(addr);
	if(map_no!=-1)
	{
		//printf("Caught:%x,%x,%x,%x\n",addr,(int)len,data,map_no);
		mmio_write(addr,len,data,map_no);
	}
#ifdef USE_CACHE
	cache1.write(&cache1,addr,len,data);
#else
	dram_write(addr, len, data);
#endif
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	//return simple_read(addr,len);
	if (((addr+len-1)>>PAGE_OFFSET_LEN)!=(addr>>PAGE_OFFSET_LEN)) {
		uint32_t more=(addr+len)&PAGING_MASK;
		uint32_t tmp=(len-more)*8;
		//split into 2 parts
		return lnaddr_read(addr,len-more) | 
			(lnaddr_read((addr+len)&~PAGING_MASK,more)<<tmp);

	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	//simple_write(addr,len,data);return;
	if (((addr+len-1)>>PAGE_OFFSET_LEN)!=(addr>>PAGE_OFFSET_LEN)) {
		uint32_t more=(addr+len)&PAGING_MASK;
		uint32_t tmp=(len-more)*8;
		//split into 2 parts
		lnaddr_write(addr, len-more, data&((1<<tmp)-1));
		lnaddr_write((addr+len)&~PAGING_MASK, more, data>>tmp);

	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
}

extern CPU_state cpu;

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
	printf("%x\n",addr);
	//return simple_read(addr,len);
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
	//simple_write(addr,len,data);return;
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

