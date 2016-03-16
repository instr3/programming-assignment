#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include <setjmp.h>
extern jmp_buf jbuf;

void raise_intr(uint8_t NO) {
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */

	
	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}

make_helper(int_i) {
	uint8_t id = instr_fetch(eip + 1, 1);
	raise_intr(id);
	print_asm("int $0x%X", id);
	return 2;
}
