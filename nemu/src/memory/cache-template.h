#include <stdlib.h>


#define BID_LEN (1 << BID_BITS)
#define OFFSET_LEN (1 << OFFSET_BITS)
#define OFFSET_MASK ((1 << OFFSET_BITS) - 1)
#define BTAG_BITS (27 - OFFSET_BITS - BID_BITS)
#define CACHE_T concat(CACHE_ID,_t)
#define CACHEBLOCK_T concat(CACHE_ID,_block_t)


#define install_method(name) \
	CACHE_ID.name = &concat(CACHE_ID, name);

typedef struct
{
	uint8_t block[OFFSET_LEN];
	bool valid,dirty;
	uint32_t tag;
}CACHEBLOCK_T;
struct CACHE_T
{
	int hit_count,miss_count;//Simulated clock
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
	void (*modify_cache_at)(struct CACHE_T *this,hwaddr_t addr);
	CACHEBLOCK_T * (*hit_or_create_cache_at)(struct CACHE_T *this,hwaddr_t addr);
	//void (*cache_read_raw)(hwaddr_t addr,uint8_t *temp,CACHEBLOCK_T *ch);
	uint32_t (*read)(struct CACHE_T *this,hwaddr_t addr, size_t len);
	void (*write)(struct CACHE_T *this,hwaddr_t addr, size_t len, uint32_t data);
	void (*debug)(struct CACHE_T *this,hwaddr_t addr);

};
struct CACHE_T CACHE_ID;
void concat(CACHE_ID,modify_cache_at)(struct CACHE_T *this,hwaddr_t addr)
{
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
#ifdef CACHE_WRITE_BACK
			this->cache[this->converter.ch.bid][i].dirty=true;
#else
			this->cache[this->converter.ch.bid][i].valid=false;
#endif
			//TODO: Write back
			return;
		}
	}
}
CACHEBLOCK_T * concat(CACHE_ID,hit_or_create_cache_at)(struct CACHE_T *this,hwaddr_t addr)
{
	this->converter.addr=addr;
	uint32_t i;
	for(i=0;i<WAY_NUM;++i)
	{
		if(this->cache[this->converter.ch.bid][i].tag == this->converter.ch.btag && this->cache[this->converter.ch.bid][i].valid)
		{
			//cache hit
			//printf("HIT!");fflush(stdout);
			this->hit_count++;
			return &this->cache[this->converter.ch.bid][i];
		}
	}
	//cache miss
	//printf("miss!");fflush(stdout);
	this->miss_count++;
	int kick=rand()%WAY_NUM;
	//Swap new and old tags
	uint32_t temp=this->cache[this->converter.ch.bid][kick].tag;
	this->cache[this->converter.ch.bid][kick].tag=this->converter.ch.btag;
	this->converter.ch.btag=temp;
#ifdef CACHE_WRITE_BACK
	if(this->cache[this->converter.ch.bid][kick].dirty)
	{
		uint32_t base_addr=this->converter.addr & ~OFFSET_MASK;
		for(i=0;i<OFFSET_LEN;i++)
		{
			//printf("WriteCache:%x %x\n",base_addr,dram_read(base_addr,1)&0xff);
			slower_write(base_addr++,1,this->cache[this->converter.ch.bid][kick].block[i]);
		}
	}
#endif
	this->cache[this->converter.ch.bid][kick].valid = true;
	this->cache[this->converter.ch.bid][kick].dirty = false;
	uint32_t base_addr=addr & ~OFFSET_MASK;
	for(i=0;i<OFFSET_LEN;i++)
	{
		//printf("WriteCache:%x %x\n",base_addr,dram_read(base_addr,1)&0xff);
		this->cache[this->converter.ch.bid][kick].block[i]=slower_read(base_addr++,1);
	}
	return &this->cache[this->converter.ch.bid][kick];

}
//void concat(CACHE_ID,cache_read_raw)(hwaddr_t addr,uint8_t *temp,CACHEBLOCK_T *ch)
//{
	//OFFSET_LEN should be greater or equal than BURST_MASK
	//uint32_t cache_burst_offset = addr & (OFFSET_MASK ^ 3);//0000111100
	//printf("addr:%x OFFSET_MASK:%x cache_burst_offset:%x OFFSET_LEN:%x\n",addr,OFFSET_MASK,cache_burst_offset,OFFSET_LEN);
	
	//memcpy(temp, &ch->block[addr & OFFSET_MASK],4);
//}
uint32_t concat(CACHE_ID,read)(struct CACHE_T *this,hwaddr_t addr, size_t len) {
	//uint32_t offset = addr & 3;
	//printf("Read At:%x %d\n",addr,(int)len);
	fflush(stdout);
	uint8_t temp[4];
	uint32_t cache_offset = addr & OFFSET_MASK;
	
	CACHEBLOCK_T *ch=this->hit_or_create_cache_at(this, addr);
	//this->cache_read_raw(addr, temp, ch);
	memcpy(temp, &ch->block[addr & OFFSET_MASK],(4<OFFSET_LEN-cache_offset)?4:OFFSET_LEN-cache_offset);
	if(cache_offset + len > OFFSET_LEN) {
		//assert(false & 1);
		//printf("Warning:%x+%x>%x",cache_offset,(int)len,OFFSET_LEN);
		fflush(stdout);
		/* data cross the cache boundary */
		ch=this->hit_or_create_cache_at(this,addr + len - 1);
		int more=cache_offset + len - OFFSET_LEN;
		//this->cache_read_raw(addr + 4, temp + 4, ch);
		//printf("[%x %x %x %x]\n",temp[0],temp[1],temp[2],temp[3]);
		memcpy(temp + 4 - more, &ch->block[0],more);
		//printf("[%x %x %x %x]%d\n",temp[0],temp[1],temp[2],temp[3],OFFSET_LEN - cache_offset);
	
	}
	fflush(stdout);
	len = 0;//Infact, it's align_rw
	return unalign_rw(temp + len, 4);
}
void concat(CACHE_ID,write)(struct CACHE_T *this,hwaddr_t addr, size_t len, uint32_t data) {
	slower_write(addr, len, data);
	this->modify_cache_at(this,addr);
}
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
			printf("Hit:\tY\tValue:\t0x%XOffset:\t0x%X\nBlockID\t%X\tWayID:\t%X\tTag:\t%X\nValid:\t%c\tDirty:\t%c\n",
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

void concat(CACHE_ID,_init)(struct CACHE_T *this){
	install_method(modify_cache_at);
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