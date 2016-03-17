#include "cpu/exec/template-start.h"

#define instr movsx
#if DATA_BYTE == 2 || DATA_BYTE == 4
static void do_execute() {
	DATA_TYPE_S res;
	char *opname;
	if(ops_decoded.opcode==0x1BF)//16->32
	{
		res=(int16_t)op_src->val;
		opname="movsw";
	}
	else
	{
		res=(int8_t)op_src->val;
		opname="movsb";
	}
	OPERAND_W(op_dest, res);
	//print_asm_template2();
	print_asm("%s" str(SUFFIX) " %s,%s", opname, op_src->str, op_dest->str);
}

make_instr_helper(rm2r)
#endif

#include "cpu/exec/template-end.h"
