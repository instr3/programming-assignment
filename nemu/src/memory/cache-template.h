#include <stdlib.h>


#define BID_LEN (1 << BID_BITS)
#define OFFSET_LEN (1 << OFFSET_BITS)
#define OFFSET_MASK ((1 << OFFSET_BITS) - 1)
#define BTAG_BITS (27 - OFFSET_BITS - BID_BITS)
#define CACHE_T concat(CACHE_ID,_t)
#define CACHEBLOCK_T concat(CACHE_ID,_block_t)


#define install_method(name) \
	CACHE_ID.name = &concat(CACHE_ID, name);

//Define the cache block structure.
typedef struct
{
	uint8_t block[OFFSET_LEN];
	bool valid,dirty;
	uint32_t tag;
}CACHEBLOCK_T;
//Define the cache structure.
struct CACHE_T
{
	int hit_count,miss_count;//Simulated clock, used in PA3 report
	CACHEBLOCK_T cache[BID_LEN][WAY_NUM];
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
	//Define the cache's "functions".
	//void (*modify_cache_at)(struct CACHE_T *this,hwaddr_t addr);
	CACHEBLOCK_T* (*hit_or_create_cache_at)(struct CACHE_T *this,hwaddr_t addr);
	//void (*cache_read_raw)(hwaddr_t addr,uint8_t *temp,CACHEBLOCK_T *ch);
	uint32_t (*read)(struct CACHE_T *this,hwaddr_t addr, size_t len);
	void (*write)(struct CACHE_T *this,hwaddr_t addr, size_t len, uint32_t data);
	void (*debug)(struct CACHE_T *this,hwaddr_t addr);

};
struct CACHE_T CACHE_ID;

//Test if an addr hit the cache, return the block if hit.
//If miss, create a block for it and return the block.
//If n-way is full, use random replace algorithm.
CACHEBLOCK_T* concat(CACHE_ID,hit_or_create_cache_at)(struct CACHE_T *this,hwaddr_t addr)
{
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			//cache hit
			this->hit_count++;
			return &this->cache[this->converter.ch.bid][i];
		}
	}
	//cache miss
	this->miss_count++;
	int kick=rand()%WAY_NUM;
	//Swap new and old tags
	uint32_t temp=this->cache[this->converter.ch.bid][kick].tag;
	this->cache[this->converter.ch.bid][kick].tag=this->converter.ch.btag;
	this->converter.ch.btag=temp;
#ifdef CACHE_WRITE_BACK_AND_WRITE_ALLOCATE
	//If the block is dirty, write back to the next memory interface.
	if(this->cache[this->converter.ch.bid][kick].dirty)
	{
		uint32_t base_addr=this->converter.addr & ~OFFSET_MASK;
		for(i=0;i<OFFSET_LEN;i+=4)
		{
			uint32_t data;
			memcpy(&data,&this->cache[this->converter.ch.bid][kick].block[i],4);
			slower_write(base_addr,4,data);
			base_addr+=4;
		}
	
	}
#endif
	this->cache[this->converter.ch.bid][kick].valid = true;
	this->cache[this->converter.ch.bid][kick].dirty = false;
	uint32_t base_addr=addr & ~OFFSET_MASK;
	for(i=0;i<OFFSET_LEN;i+=4)
	{
		uint32_t data=slower_read(base_addr,4);
		base_addr+=4;
		memcpy(&this->cache[this->converter.ch.bid][kick].block[i],&data,4);
	}
	return &this->cache[this->converter.ch.bid][kick];

}
/*void concat(CACHE_ID,cache_read_raw)(hwaddr_t addr,uint8_t *temp,CACHEBLOCK_T *ch)
{
	OFFSET_LEN should be greater or equal than BURST_MASK
	uint32_t cache_burst_offset = addr & (OFFSET_MASK ^ 3);//0000111100
	printf("addr:%x OFFSET_MASK:%x cache_burst_offset:%x OFFSET_LEN:%x\n",addr,OFFSET_MASK,cache_burst_offset,OFFSET_LEN);
	
	memcpy(temp, &ch->block[addr & OFFSET_MASK],4);
}*/

//Read addr from a cache. 
//If miss, create a cache block at range [addr & (OFFSET_MASK,addr & OFFSET_MASK) + len).
uint32_t concat(CACHE_ID,read)(struct CACHE_T *this,hwaddr_t addr, size_t len) {
	uint8_t temp[4];
	uint32_t cache_offset = addr & OFFSET_MASK;
	
	CACHEBLOCK_T *ch=this->hit_or_create_cache_at(this, addr);
	memcpy(temp, &ch->block[addr & OFFSET_MASK],(len<OFFSET_LEN-cache_offset)?len:OFFSET_LEN-cache_offset);
	if(cache_offset + len > OFFSET_LEN) {
		/* data cross the cache boundary */
		ch=this->hit_or_create_cache_at(this,addr + len - 1);
		int more=cache_offset + len - OFFSET_LEN;
		memcpy(temp + 4 - more, &ch->block[0],more);
	
	}
	len = 0;
	//In fact, it's align_rw
	return unalign_rw(temp + len, 4);
}
//Write addr from a cache. 
void concat(CACHE_ID,write)(struct CACHE_T *this,hwaddr_t addr, size_t len, uint32_t data) {
#ifdef CACHE_WRITE_BACK_AND_WRITE_ALLOCATE 
	//If the cache is write_back and write_allocate
	//If miss, create a cache block at range [addr & (OFFSET_MASK,addr & OFFSET_MASK) + len).
	//Then modified the bits and set the dirty flag.
	uint8_t temp[4];
	memcpy(temp,&data,4);
	uint32_t cache_offset = addr & OFFSET_MASK;
	CACHEBLOCK_T *ch=this->hit_or_create_cache_at(this,addr);
	ch->dirty=true;
	memcpy(&ch->block[addr & OFFSET_MASK], temp, (len<OFFSET_LEN-cache_offset)?len:OFFSET_LEN-cache_offset);
	if(cache_offset + len > OFFSET_LEN) {
		/* data cross the cache boundary */
		ch=this->hit_or_create_cache_at(this,addr + len - 1);
		ch->dirty=true;
		int more=cache_offset + len - OFFSET_LEN;
		memcpy(&ch->block[0], temp + 4 - more, more);
	}
#else
	//If write_through and not write_allocate, 
	//simply let it be done by next level memory interface.
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			this->cache[this->converter.ch.bid][i].valid=false;
		}
	}
	this->converter.addr=addr+len-1;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			this->cache[this->converter.ch.bid][i].valid=false;
		}
	}
	slower_write(addr, len, data);
#endif
}

//Print cache information of a hardware address
//Used by monitor debug command 'cache'
void concat(CACHE_ID,debug)(struct CACHE_T *this,hwaddr_t addr)
{
#define SNAME str(CACHE_ID)
	printf("=======================\nCache:\t%s\tHitCnt:\t%d\tMisCnt:\t%d\nAddr:\t0x%X\n",SNAME,this->hit_count,this->miss_count,addr);
#undef SNAME
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			printf("Hit:\tY\tValue:\t0x%X\tOffset:\t0x%X\nBlockID\t%X\tWayID:\t%X\tTag:\t%X\nValid:\t%c\tDirty:\t%c\n",
				this->cache[this->converter.ch.bid][i].block[this->converter.ch.offset],
				this->converter.ch.offset,
				this->converter.ch.bid,
				i,
				this->cache[this->converter.ch.bid][i].tag,
				this->cache[this->converter.ch.bid][i].valid?'Y':'N',
				this->cache[this->converter.ch.bid][i].dirty?'Y':'N'
			);
			return;
		}
	}
	printf("Hit:\tN\tOffset:\t0x%X\nBlockID\t%X\tTag:\t%X\n",
		this->converter.ch.offset,
		this->converter.ch.bid,
		this->converter.ch.btag
	);
}

//Install mothod for 'cache structure'
//Well, this is not a good idea when
//developing an actual project!!
void concat(CACHE_ID,_init)(struct CACHE_T *this){
	//install_method(modify_cache_at);
	install_method(hit_or_create_cache_at);
	//install_method(cache_read_raw);
	install_method(read);
	install_method(write);
	install_method(debug);
	int i,j;
	for(j=0;j<BID_LEN;++j)
	{
		for(i=0;i<WAY_NUM;++i)
		{
			this->cache[j][i].valid=false;
		}
	}
}