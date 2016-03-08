#include "memory/tlb.h"
#include "common.h"
#include "stdlib.h"

#ifdef USE_TLB
tlb_t tlb;
#endif

uint32_t find_tlb(tlb_t tlb, uint32_t tag, bool *success)
{
	int i;
	for(i=0;i<TLB_ITEM_CNT;++i)
	{
		if(tlb.item[i].valid&&tlb.item[i].tag==tag)
		{
			*success=true;
			return tlb.item[i].data;
		}
	}
	*success=false;
	return 0;
}
void write_tlb(tlb_t tlb, uint32_t tag, uint32_t data)
{
	int i;
	int place=-1;
	for(i=0;i<TLB_ITEM_CNT;++i)
	{
		if(!tlb.item[i].valid){place=i;break;}
	}
	if(place==-1)place=rand()%TLB_ITEM_CNT;
	tlb.item[i].tag=tag;
	tlb.item[i].data=data;
	tlb.item[i].valid=1;
}
void flush_tlb(tlb_t tlb)
{
	int i;
	for(i=0;i<TLB_ITEM_CNT;++i)
	{
		tlb.item[i].valid=false;
	}
}

