#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute () {
	DATA_TYPE result = op_dest->val - op_src->val;
	printf("Look:%u\n",result);
	printf("Look2:%llu\n",op_dest->val+1ull+(~op_src->val));
	//printf("Highest Bit:%llu,%llu\n",1ull<<8*DATA_BYTE,result&1ull<<8*DATA_BYTE);
	//OPERAND_W(op_dest, result);
	influence_zf(result);
	influence_pf(result);
	influence_sf(result);
	influence_of(result,op_dest->val,~op_src->val);
	influence_cf(result,op_dest->val+1ull+(~op_src->val),1);

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
