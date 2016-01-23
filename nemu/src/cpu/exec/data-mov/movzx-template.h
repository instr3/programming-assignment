#include "cpu/exec/template-start.h"

#define instr movzx
#if DATA_BYTE == 2 || DATA_BYTE == 4
static void do_execute() {
	DATA_TYPE_S res;
	if(ops_decoded.opcode==0xBF)//16->32
		res=(uint16_t)op_src->val;
	else
		res=(uint8_t)op_src->val;
	OPERAND_W(op_dest, res);
	print_asm_template2();
}

make_instr_helper(rm2r)
#endif

#include "cpu/exec/template-end.h"
