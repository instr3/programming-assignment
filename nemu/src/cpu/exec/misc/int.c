#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include <setjmp.h>
#include "memory/memory.h"
extern jmp_buf jbuf;

void raise_intr(uint8_t NO) {

	idtitem_t gate;
	lnaddr_t address=(uint32_t)NO*8+cpu.idtr_base;
	assert(address<cpu.idtr_limit+cpu.idtr_base);
	gate.item=lnaddr_read(address,4)+((uint64_t)lnaddr_read(address+4,4)<<32);
	uint32_t offset=gate.offset_15_0+(gate.offset_31_16<<16);

	segmentselector_t seg;
	seg.selector=gate.segment;
	gdtitem_t gdt;
	address=(uint32_t)seg.index*8+cpu.gdtr_base;
	assert(address<cpu.gdtr_limit+cpu.gdtr_base);
	gdt.item=lnaddr_read(address,4)+((uint64_t)lnaddr_read(address+4,4)<<32);
	uint32_t base=gdt.seg_base_0_15+((uint32_t)gdt.seg_base_16_23<<16)+((uint32_t)gdt.seg_base_24_31<<24);
	//inv->limit=gdt.seg_limit_0_15+((uint32_t)gdt.seg_limit_16_19<<16);

	cpu.eip=base+offset;
	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}

make_helper(int_i) {
	extern void printRegInfo();
	printRegInfo();
	int i;
	if(cpu.eax==4)
	for(i=0;i<10;++i)
	printf("[%c]\n",(char)swaddr_read(cpu.ecx+i,1,SREG_DS));
	uint8_t id = instr_fetch(eip + 1, 1);
	push_data(cpu.eflags,4);
	push_data(cpu.cs,4);
	reg_flag_set(EFLAGS_IF,0);
	push_data(cpu.eip+2,4);
	raise_intr(id);
	//Well, can't process to here, sadly.
	print_asm("int $0x%X", id);
	return 2;
}
