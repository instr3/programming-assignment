
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
	cache *find_cache(hwaddr_t addr)
	{
		converter.addr=addr;
		uint32_t i;
		for(i=0;i<WAY_NUM;++i)
		{
			if(cache[converter.ch.bid][i].tag == btag && cache[converter.ch.bid][i].valid)
			{
				return &cache[converter.ch.bid][i];
			}
		}
		return 0;
	}
	uint32_t read(hwaddr_t addr, size_t len) 
	{
		uint32_t offset = addr & OFFSET_MASK;
		uint8_t temp[2 * (1 << OFFSET_BITS)];
		int rbits=min((1 << OFFSET_BITS)-offset,4);
		cache *ch;
		if(ch=find_cache(addr))//cache hit
		{
			memcpy(temp+offset,ch+offset,rbits);
		}
		else
		{
			dram_read(addr, temp);
		}

		if(offset + len > BURST_LEN) {
			/* data cross the burst boundary */
			ddr3_read(addr + BURST_LEN, temp + BURST_LEN);
		}

}CACHE_NAME;