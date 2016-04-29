#include "common.h"
#include "nemu.h"

//Performs segment translation from logical address to leaner address.
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	//When protect mode disabled, segment traslation disabled.
	if(cpu.cr0.protect_enable==0)return addr;
	struct invisible_part inv;
	//Select from segment register ID.
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