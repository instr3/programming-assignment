#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "and-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "and-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "and-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(and_i2a)
make_helper_v(and_i2rm)
make_helper_v(and_si2rm)
make_helper_v(and_r2rm)
make_helper_v(and_rm2r)
/*
24 ib AND AL,imm8 2 AND immediate byte to AL
25 iw AND AX,imm16 2 AND immediate word to AX
25 id AND EAX,imm32 2 AND immediate dword to EAX
80 /4 ib AND r/m8,imm8 2/7 AND immediate byte to r/m byte
81 /4 iw AND r/m16,imm16 2/7 AND immediate word to r/m word
81 /4 id AND r/m32,imm32 2/7 AND immediate dword to r/m dword
83 /4 ib AND r/m16,imm8 2/7 AND sign-extended immediate byte with r/m word
83 /4 ib AND r/m32,imm8 2/7 AND sign-extended immediate byte with r/m dword
20 /r AND r/m8,r8 2/7 AND byte register to r/m byte
21 /r AND r/m16,r16 2/7 AND word register to r/m word
21 /r AND r/m32,r32 2/7 AND dword register to r/m dword
22 /r AND r8,r/m8 2/6 AND r/m byte to byte register
23 /r AND r16,r/m16 2/6 AND r/m word to word register
23 /r AND r32,r/m32 2/6 AND r/m dword to dword register
*/