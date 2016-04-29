#include "memory/tlb.h"
#include "common.h"
#include "stdlib.h"

//Test if TLB is enabled.
//Disable the #define at "common.h"
#ifdef USE_TLB
tlb_t tlb;
#endif

//Find a tag in the tlb. *success = 0 if failed.
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

//Write a tag in the tlb. 
//If tlb is full, use random replace algorithm.
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
//Flush the tlb, clear all valid bits.
//Occurs when changed CR3.
void flush_tlb(tlb_t tlb)
{
	int i;
	for(i=0;i<TLB_ITEM_CNT;++i)
	{
		tlb.item[i].valid=false;
	}
}

