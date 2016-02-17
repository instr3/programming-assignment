#include <stdlib.h>

#define OFFSET_LEN (1 << OFFSET_BITS)
#define OFFSET_MASK ((1 << OFFSET_BITS) - 1)
#define BTAG_BITS (27 - OFFSET_BITS - BID_BITS)
#define CACHE_T concat(CACHE_ID,_t)
#define CACHEBLOCK_T concat(CACHE_ID,_block_t)


#define install_method(identifier,name,...) \
	identifier concat(CACHE_ID, name) __VA_ARGS__ \
	//CACHE_ID.name = &concat(CACHE_ID, name);

typedef struct
{
	uint8_t block[1 << OFFSET_BITS];
	bool valid,dirty;
	uint32_t tag;
}CACHEBLOCK_T;
struct CACHE_T
{
	CACHEBLOCK_T cache[1<<BID_BITS][WAY_NUM];
	union{
		//unalign_rw
		hwaddr_t addr;
#pragma pack (1)
		struct
		{
			uint32_t offset : OFFSET_BITS;	//in-cache offset
			uint32_t bid : BID_BITS;		//cache id
			uint32_t btag : BTAG_BITS;		//cache tag
		}ch;
#pragma pack ()
	}converter;
	void (*modify_cache_at)(struct CACHE_T *this,hwaddr_t addr);
	CACHEBLOCK_T * (*hit_or_create_cache_at)(struct CACHE_T *this,hwaddr_t addr);
	void (*cache_read_raw)(hwaddr_t addr,uint8_t *temp,CACHEBLOCK_T *ch);
	uint32_t (*read)(struct CACHE_T *this,hwaddr_t addr, size_t len);
	void (*write)(struct CACHE_T *this,hwaddr_t addr, size_t len);

};
struct CACHE_T CACHE_ID;
install_method(void,modify_cache_at,(struct CACHE_T *this,hwaddr_t addr)
{
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			this->cache[this->converter.ch.bid][i].valid=false;
			//TODO: Write back
			return;
		}
	}
})
install_method(CACHEBLOCK_T *,hit_or_create_cache_at,(struct CACHE_T *this,hwaddr_t addr)
{
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			//cache hit
			return &this->cache[this->converter.ch.bid][i];
		}
	}
	//cache miss
	int kick=rand()%WAY_NUM;
	//TODO: Write back
	this->cache[this->converter.ch.bid][kick].tag = this->converter.ch.btag;
	this->cache[this->converter.ch.bid][kick].valid = true;
	uint32_t base_addr=addr & ~OFFSET_MASK;
	for(i=0;i<OFFSET_LEN;i++)
	{
		this->cache[this->converter.ch.bid][kick].block[i]=dram_read(base_addr++,1);
	}
	return &this->cache[this->converter.ch.bid][kick];

})
install_method(void,cache_read_raw,(hwaddr_t addr,uint8_t *temp,CACHEBLOCK_T *ch)
{
	assert(OFFSET_LEN>=4);
	//OFFSET_LEN should be greater or equal than BURST_MASK
	uint32_t cache_burst_offset = addr & (OFFSET_MASK ^ 3);//0000111100
	memcpy(temp, &ch->block[cache_burst_offset],4);
})
install_method(uint32_t,read,(struct CACHE_T *this,hwaddr_t addr, size_t len) {
	uint32_t offset = addr & 3;
	uint8_t temp[2 * 4];
	//Use 3 instead of OFFSET_MASK to save space and time
	uint32_t cache_offset = addr & OFFSET_MASK;
	
	CACHEBLOCK_T *ch=this->hit_or_create_cache_at(this, addr);
	this->cache_read_raw(addr, temp, ch);
	if(cache_offset + len > OFFSET_LEN) {
		/* data cross the cache boundary */
		ch=this->hit_or_create_cache_at(this,addr + len - 1);
		this->cache_read_raw(addr + 4, temp + 4, ch);
	}
	return unalign_rw(temp + offset, 4);
})
install_method(void,write,(struct CACHE_T *this,hwaddr_t addr, size_t len, uint32_t data) {
	dram_write(addr, len, data);
	this->modify_cache_at(this,addr);
})