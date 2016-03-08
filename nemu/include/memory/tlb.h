#ifndef __TLB_H__
#define __TLB_H__

#include "common.h"
#define TLB_ITEM_CNT 64
typedef struct tlb_item
{
	bool valid;
	uint32_t tag;//20 bits
	uint32_t data;//20 bits
} tlb_item_t;
typedef struct tlb
{
	tlb_item_t item[TLB_ITEM_CNT];
} tlb_t;

uint32_t find_tlb(tlb_t tlb, uint32_t tag, bool *success);
void write_tlb(tlb_t tlb, uint32_t tag, uint32_t data);
void flush_tlb(tlb_t tlb);



#endif