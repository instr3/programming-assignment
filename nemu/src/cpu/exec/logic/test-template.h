#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
	//OPERAND_W(op_dest, result);

	reg_flag_set(EFLAGS_CF,0);
	reg_flag_set(EFLAGS_OF,0);
	influence_zf(result);
	influence_sf(result);
	influence_pf(result);


	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
