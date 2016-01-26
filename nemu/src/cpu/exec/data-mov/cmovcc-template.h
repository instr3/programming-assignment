#include "cpu/exec/template-start.h"

#define instr cmovcc

#if DATA_BYTE == 2 || DATA_BYTE == 4
static void do_execute () {
	uint32_t op = ops_decoded.opcode&0xf;
	uint32_t cond=false;
	char *jname;
#define f(a) reg_flag(concat(EFLAGS_,a))
#define optest(opid,name,...)\
	if(op==concat(0x,opid))\
	{\
		if(__VA_ARGS__)cond=true;jname=name;\
	}
	optest(7,"cmova",f(CF)==0&&f(ZF)==0)
	optest(3,"cmovae",f(CF)==0)
	optest(2,"cmovb",f(CF)==1)
	optest(6,"cmovbe",f(CF)==1||f(ZF)==1)
	optest(4,"cmove",f(ZF)==1)
	optest(5,"cmovne",f(ZF)==0)
	optest(F,"cmovg",f(ZF)==0&&f(SF)==f(OF))
	optest(D,"cmovge",f(SF)==f(OF))
	optest(C,"cmovl",f(ZF)==0&&f(SF)!=f(OF));
	optest(E,"cmovle",f(ZF)==1||f(SF)!=f(OF));
	optest(1,"cmovno",f(OF)==0);
	optest(B,"cmovnp",f(PF)==0);
	optest(9,"cmovns",f(SF)==0);
	optest(0,"cmovo",f(OF)==1);
	optest(A,"cmovp",f(PF)==1);
	optest(8,"cmovs",f(SF)==1);
#undef f
#undef optest
	if(cond)
	{
		OPERAND_W(op_dest, op_src->val);
	}
	OPERAND_W(op_src,cond);
	print_asm("%s %s,%s",jname,op_src->str,op_dest->str);
}
make_instr_helper(r2rm)
#endif
#include "cpu/exec/template-end.h"
