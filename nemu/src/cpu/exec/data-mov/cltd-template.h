#include "cpu/exec/template-start.h"


#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(cltd_a_, SUFFIX)) {

	REG(R_EDX)=(DATA_TYPE_S)REG(R_EAX)>=0?0:-1;
	print_asm("cltd");
	return 1;
}
#endif

#include "cpu/exec/template-end.h"
