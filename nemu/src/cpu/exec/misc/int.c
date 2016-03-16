#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include <setjmp.h>
#include "memory/memory.h"
extern jmp_buf jbuf;

void raise_intr(uint8_t NO) {
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */
	
	idtitem_t gate;
	lnaddr_t address=(uint32_t)NO*8+cpu.idtr_base;
	assert(address<cpu.idtr_limit+cpu.idtr_base);
	printf("%x\n",address);
	fflush(stdout);
	gate.item=lnaddr_read(address,4)+((uint64_t)lnaddr_read(address+4,4)<<32);
	uint32_t offset=gate.offset_15_0+(gate.offset_31_16<<16);
	printf("%x\n",gate.segment>>2);
	fflush(stdout);
	cpu.eip=swaddr_read(offset,4,gate.segment);
	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}

make_helper(int_i) {
	uint8_t id = instr_fetch(eip + 1, 1);
	raise_intr(id);
	print_asm("int $0x%X", id);
	return 2;
}
