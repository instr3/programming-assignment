#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jcc-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */
make_helper_v(jcc);
/*
make_helper_v(ja_CF0_ZF0)	//77
make_helper_v(jae_CF0)		//73
make_helper_v(jb_CF1)		//72
make_helper_v(jbe_CF1oZF1)	//76
make_helper_v(je_ZF1)		//74
make_helper_v(jne_ZF0)		//75
make_helper_v(jg_ZF0_SFOF)	//7F
make_helper_v(jge_SFOF)		//7D
make_helper_v(jl_ZF0_SFxOF)	//7C
make_helper_v(jle_ZF1oSFxOF)//7E
make_helper_v(jno_OF0)		//71
make_helper_v(jnp_PF0)		//7B
make_helper_v(jns_SF0)		//79
make_helper_v(jo_OF1)		//70
make_helper_v(jp_PF1)		//7A
make_helper_v(js_SF1)		//78
*/