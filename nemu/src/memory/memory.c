#include "common.h"
#include "nemu.h"
#include "memory/page.h"
#include "device/mmio.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#ifdef USE_CACHE

//Create Second Level Cache
#define OFFSET_BITS 6
#define BID_BITS 12 //Block ID Bits = 4MB/16B/16Ways
#define WAY_NUM 16
#define CACHE_WRITE_BACK_AND_WRITE_TRHOUGH //Define the Write_back feature
#define CACHE_ID cache2
//Define the next level cache
#define slower_read(...) dram_read(__VA_ARGS__)
#define slower_write(...) dram_write(__VA_ARGS__)

#include "cache-template.h"

#undef OFFSET_BITS
#undef BID_BITS
#undef WAY_NUM
#undef CACHE_WRITE_BACK_AND_WRITE_TRHOUGH
#undef CACHE_ID
#undef slower_read
#undef slower_write

//Create First Level Cache(Modified)
#define OFFSET_BITS 6
#define BID_BITS 7 
#define WAY_NUM 8
#define CACHE_ID cache1
//Define the next level cache
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
	int map_no=is_mmio(addr);
	if(map_no!=-1)return mmio_read(addr,len,map_no);
#ifdef USE_CACHE
	return cache1.read(&cache1,addr,len) & (~0u >> ((4 - len) << 3));
#else
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
#endif
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
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
	if (((addr+len-1)>>PAGE_OFFSET_LEN)!=(addr>>PAGE_OFFSET_LEN)) {
		uint32_t more=(addr+len)&PAGING_MASK;
		uint32_t tmp=(len-more)*8;
		//split into 2 parts
		//printf("more:%x\n",more);
		//fflush(stdout);
		return lnaddr_read(addr,len-more) | 
			(lnaddr_read((addr+len)&~PAGING_MASK,more)<<tmp);
		//assert(0);

	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		if(addr>=0xc0000000)
		printf("Translated:%x\n",hwaddr);
		return hwaddr_read(hwaddr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	if (((addr+len-1)>>PAGE_OFFSET_LEN)!=(addr>>PAGE_OFFSET_LEN)) {
		uint32_t more=(addr+len)&PAGING_MASK;
		uint32_t tmp=(len-more)*8;
		//printf("more:%x\n",more);
		//fflush(stdout);
		//split into 2 parts
		lnaddr_write(addr, len-more, data&((1<<tmp)-1));
		lnaddr_write((addr+len)&~PAGING_MASK, more, data>>tmp);
		//assert(0 && "Not Tested");

	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		if(addr>=0xc0000000)
		printf("Translated:%x\n",hwaddr);
		hwaddr_write(hwaddr, len, data);
	}
}

extern CPU_state cpu;
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	//When Protect disabled, segment traslation disabled.
	if(cpu.cr0.protect_enable==0)return addr;
	struct invisible_part inv;
	switch(sreg)
	{
		case SREG_CS:inv=cpu.cs_inv;break;
		case SREG_DS:inv=cpu.ds_inv;break;
		case SREG_ES:inv=cpu.es_inv;break;
		case SREG_SS:inv=cpu.ss_inv;break;
		default: assert(0);
	}
	//directly return from invisible part of segment registers.
	return inv.base+addr;
}


uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

