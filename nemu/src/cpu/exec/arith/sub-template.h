#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute () {
	DATA_TYPE result = op_dest->val - op_src->val;
	DATA_TYPE src=op_src->val,dest=op_dest->val;
	OPERAND_W(op_dest, result);
	influence_zf(result);
	influence_pf(result);
	influence_sf(result);
	influence_of(result,dest,~src);
	influence_cf(dest+1ull+(DATA_TYPE)~src,1);

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
