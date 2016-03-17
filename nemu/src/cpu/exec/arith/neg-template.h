#include "cpu/exec/template-start.h"

#define instr neg

static void do_execute() {
	DATA_TYPE result = -op_src->val;
	OPERAND_W(op_src, result);

	/* There is no need to update EFLAGS, since no other instructions 
	 * in PA will test the flags updated by this instruction.
	 */
	//OF Not set

	influence_zf(result);
	influence_sf(result);
	influence_pf(result);
	reg_flag_set(EFLAGS_CF,!!result);

	print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
