#include "cpu/exec/template-start.h"


#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(cwtl_a_, SUFFIX)) {
#if DATA_BYTE == 2
	REG(R_EAX)=(int16_t)(int8_t)reg_b(R_EAX);
#else
	REG(R_EAX)=(int32_t)(int16_t)reg_w(R_EAX);
#endif
	print_asm("cwtl");
	return 1;
}
#endif

#include "cpu/exec/template-end.h"
