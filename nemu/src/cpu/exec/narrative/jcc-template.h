#include "cpu/exec/template-start.h"

#define instr jcc

make_helper(concat(jcc_, SUFFIX)) {
	uint32_t op = instr_fetch(eip, 1);
	DATA_TYPE_S addr = instr_fetch(eip + 1, DATA_BYTE);
#if DATA_BYTE==1
	op-=0x70;
#else
	op-=0x80;
#endif
	bool cond=false;
	char *jname;
#define f(a) reg_flag(concat(EFLAGS_,a))
#define optest(opid,name,...)\
	if(op==concat(0x,opid))\
	{\
		if(__VA_ARGS__)cond=true;jname=name;\
	}
	optest(7,"ja",f(CF)==0&&f(ZF)==0)
	optest(3,"jae",f(CF)==0)
	optest(2,"jb",f(CF)==1)
	optest(6,"jbe",f(CF)==1||f(ZF)==1)
	optest(4,"je",f(ZF)==1)
	optest(5,"jne",f(ZF)==0)
	optest(F,"jg",f(ZF)==0&&f(SF)==f(OF))
	optest(D,"jge",f(SF)==f(OF))
	optest(C,"jl",f(ZF)==0&&f(SF)!=f(OF));
	optest(E,"jle",f(ZF)==1||f(SF)!=f(OF));
	optest(1,"jno",f(OF)==0);
	optest(B,"jnp",f(PF)==0);
	optest(9,"jns",f(SF)==0);
	optest(0,"jo",f(OF)==1);
	optest(A,"jp",f(PF)==1);
	optest(8,"js",f(SF)==1);
#undef f
#undef optest
	extern char *PrintAddressInFunction(uint32_t);//elf.c
	char *infun=PrintAddressInFunction(cpu.eip+addr+1+DATA_BYTE);
	if(infun)
		print_asm("%s %x %s",jname,cpu.eip+addr+1+DATA_BYTE,infun);
	else
		print_asm("%s %x",jname,cpu.eip+addr+1+DATA_BYTE);
	if(cond)
	{
		cpu.eip+=addr;
#if DATA_BYTE==2
		eip&=0x0000ffff;
#endif
	}
	return 1 + DATA_BYTE;
}

#include "cpu/exec/template-end.h"
