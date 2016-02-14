#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */


/*typedef struct cacheblock
{
	uint32_t valid;
	uint32_t data[];//Size decided by malloc
}cacheblock_t;*/
/*
typedef struct cache
{
	uint32_t mem_width, mem_size;//Memory size
	uint32_t block_width, block_size;//Cache block size
	uint32_t bid_width, bid_num;//Block ID count
	uint32_t way_num;//associative ways count
	uint8_t ***blocks;//blocks[block_id][way]
	bool **valid;

}cache_t;
cache_t cacheV1, cacheV2;//V1 and V2 cache
void init_cache(cache_t cache, int mem_width, int block_width, int bid_width, int way_num)//Memory address size, Block size, Block ID size
{
	//init parameters
	cache.mem_width=mem_width;
	cache.block_width=block_width;
	cache.bid_width=bid_width;

	cache.mem_size=1<<mem_width;
	cache.block_size=1<<block_width;
	cache.bid_num=1<<bid_width;
	cache.way_num=way_num;

	//Allocate cache blocks
	int i,j;
	cache.blocks=malloc(cache.bid_num * sizeof(uint8_t **));
	for(i=0;i<cache.bid_num;++i)
	{
		cache.blocks[i]=malloc(cache.way_num * sizeof(uint8_t *));
		for(j=0;j<way_num;++j)
		{
			cache.blocks[i][j]=malloc(cache.block_size * sizeof(uint8_t));
			cache.valid[i][j]=false;
		}
	}
}
uint32_t cache_read(cache_t cache,hwaddr_t addr, size_t len, bool *success)
{
	return 0;
	//uint32_t addr_offset=addr & (cache.block_size - 1);
	//uint32_t addr_bid=(addr>>(cache.mem_width - cache.bid_width));

	//int j;
	//for(j=0)
}

void init_cache_groups()
{
	//init_cache(cacheV1, 27, 6, 16 - 6 - 3);//64B per block, cache size 64KB, 8-way.
}*/
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	dram_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

