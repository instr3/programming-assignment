
#define OFFSET_MASK ((1 << OFFSET_BITS) - 1)
#define BTAG_BITS (27 - OFFSET_BITS - BID_BITS)
struct
{
	struct
	{
		uint8_t block[1 << OFFSET_BITS];
		bool valid,dirty;
		uint32_t tag;
	}cache[1<<BID_BITS][WAY_NUM];
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
	//void *write_back()

	void modify_cache_at(hwaddr_t addr)
	{
		converter.addr=addr;
		uint32_t i;
		for(i=0;i<WAY_NUM;++i)
		{
			if(cache[converter.ch.bid][i].tag == converter.ch.btag && cache[converter.ch.bid][i].valid)
			{
				cache[converter.ch.bid][i].valid=false;
				//TODO: Write back
				return;
			}
		}
	}
	cache *hit_or_create_cache_at(hwaddr_t addr)
	{
		converter.addr=addr;
		uint32_t i;
		for(i=0;i<WAY_NUM;++i)
		{
			if(cache[converter.ch.bid][i].tag == converter.ch.btag && cache[converter.ch.bid][i].valid)
			{
				//cache hit
				return &cache[converter.ch.bid][i];
			}
		}
		//cache miss
		int kick=rand()%WAY_NUM;
		//TODO: Write back
		cache[converter.ch.bid][kick].tag = converter.ch.btag;
		cache[converter.ch.bid][kick].valid = true;
		uint32_t base_addr=addr & ~OFFSET_MASK;
		for(i=0;i<OFFSET_LEN;i++)
		{
			cache[converter.ch.bid][kick].block[i]=dram_read(base_addr++,1);
		}
		return &cache[converter.ch.bid][kick];

	}
	void cache_read_raw(hwaddr_t addr,uint8_t *temp,cache *ch)
	{
		assert(OFFSET_LEN>=4);
		//OFFSET_LEN should be greater or equal than BURST_MASK
		uint32_t cache_burst_offset = addr & (OFFSET_MASK ^ 3);//0000111100
		memcpy(temp, ch.block[cache_burst_offset],4);
	}
	uint32_t read(hwaddr_t addr, size_t len) {
		uint32_t offset = addr & 3;
		uint8_t temp[2 * 4];
		//Use 3 instead of OFFSET_MASK to save space and time
		uint32_t cache_offset = addr & OFFSET_MASK;
		
		ch=hit_or_create_cache_at(addr)
		cache_read_raw(addr, temp, ch);
		if(cache_offset + len > OFFSET_LEN) {
			/* data cross the cache boundary */
			ch=hit_or_create_cache_at(addr + len - 1);
			cache_read_raw(addr + 4, temp + 4, ch);
		}
		return unalign_rw(temp + offset, 4);
	}
	void write(hwaddr_t addr, size_t len) {
		dram_write(addr, len);
		modify_cache_at(addr);
	}

}