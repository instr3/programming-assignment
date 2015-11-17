#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "mov-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(mov_i2r)
make_helper_v(mov_i2rm)
make_helper_v(mov_r2rm)
// 88 /r MOV r/m8,r8 2/2 Move byte register to r/m byte
// 89 /r MOV r/m16,r16 2/2 Move word register to r/m word
// 89 /r MOV r/m32,r32 2/2 Move dword register to r/m dword
make_helper_v(mov_rm2r)
// 8A /r MOV r8,r/m8 2/4 Move r/m byte to byte register
// 8B /r MOV r16,r/m16 2/4 Move r/m word to word register
// 8B /r MOV r32,r/m32 2/4 Move r/m dword to dword register
make_helper_v(mov_a2moffs)
make_helper_v(mov_moffs2a)
