#include "cpu/exec/template-start.h"

#define instr setcc

#if DATA_BYTE == 1
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
	optest(7,"seta",f(CF)==0&&f(ZF)==0)
	optest(3,"setae",f(CF)==0)
	optest(2,"setb",f(CF)==1)
	optest(6,"setbe",f(CF)==1||f(ZF)==1)
	optest(4,"sete",f(ZF)==1)
	optest(5,"setne",f(ZF)==0)
	optest(F,"setg",f(ZF)==0&&f(SF)==f(OF))
	optest(D,"setge",f(SF)==f(OF))
	optest(C,"setl",f(ZF)==0&&f(SF)!=f(OF));
	optest(E,"setle",f(ZF)==1||f(SF)!=f(OF));
	optest(1,"setno",f(OF)==0);
	optest(B,"setnp",f(PF)==0);
	optest(9,"setns",f(SF)==0);
	optest(0,"seto",f(OF)==1);
	optest(A,"setp",f(PF)==1);
	optest(8,"sets",f(SF)==1);
#undef f
#undef optest
	OPERAND_W(op_src,cond);
	jname=jname;
	print_asm("%s %s",jname,op_src->str);
}
make_instr_helper(rm)
#endif
#include "cpu/exec/template-end.h"
