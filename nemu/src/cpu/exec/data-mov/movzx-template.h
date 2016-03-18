#include "cpu/exec/template-start.h"

#define instr movzx
#if DATA_BYTE == 2 || DATA_BYTE == 4
static void do_execute() {
	DATA_TYPE res;
	char *opname;
	if(ops_decoded.opcode==0x1B7)//16->32
	{
		res=(uint16_t)op_src->val;
		opname="movzw";
	}
	else
	{
		//Some error formats
		if(instr_fetch(cpu.eip+1,2)==0xc6b6)
		{
			strcpy(op_src->str,"%dh");
			op_src->val=reg_w(R_EDX)>>8;
			printf("%x\n",reg_l(R_EDX));
		}
		if(instr_fetch(cpu.eip+1,2)==0xcfb6)
		{
			strcpy(op_src->str,"%bh");
			op_src->val=reg_w(R_EBX)>>8;
		}
		res=(uint8_t)op_src->val;
		opname="movzb";
	}
	OPERAND_W(op_dest, res);
	//print_asm_template2();
	print_asm("%s" str(SUFFIX) " %s,%s", opname, op_src->str, op_dest->str);
}

make_instr_helper(rm2r)
#endif

#include "cpu/exec/template-end.h"
