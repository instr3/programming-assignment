#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct 
{
	union
	{
		union 
		{
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];
	
		/* Do NOT change the order of the GPRs' definitions. */
		struct 
		{
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};
	swaddr_t eip;
	uint32_t eflags;

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])
#define reg_flag(index) (cpu.eflags&1<<(index))
#define reg_flag_set(index,res) if(res)cpu.eflags|=1<<(index);else cpu.eflags&=~(1<<(index))
/*
31                  23                  15               7             0
+-------------------+---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                   |V|R| |N|I O|O|D|I|T|S|Z| |A| |P| |C|
| 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | | |0| |   | | | | | | |0| |0| |1| |
|                                   |M|F| |T|P L|F|F|F|F|F|F| |F| |F| |F|
+-------------------+---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
#define EFLAGS_CF 0
#define EFLAGS_PF 2
#define EFLAGS_ZF 6
#define EFLAGS_SF 7
#define EFLAGS_IF 9
#define EFLAGS_DF 10
#define EFLAGS_OF 11
#define highestbit(res) ((res)&1ull<<8*DATA_BYTE)

#define influence_zf(res) reg_flag_set(EFLAGS_ZF,!res)
#define influence_pf(res) reg_flag_set(EFLAGS_PF,~__builtin_popcount(res)&1)
#define influence_sf(res) reg_flag_set(EFLAGS_SF,highestbit(res))
#define influence_of(res,op1,op2) reg_flag_set(EFLAGS_OF,highestbit(op1)==highestbit(op2)&& \
	highestbit(op1)!=highestbit(res))
#define influence_cf(res,resull) reg_flag_set(EFLAGS_CF,res!=resull)
/*Bit Name Function page 419
0 CF Carry Flag ── Set on high-order bit carry or borrow; cleared
otherwise.
2 PF Parity Flag ── Set if low-order eight bits of result contain
an even number of 1 bits; cleared otherwise.
4 AF Adjust flag ── Set on carry from or borrow to the low order
four bits of AL; cleared otherwise. Used for decimal
arithmetic.
6 ZF Zero Flag ── Set if result is zero; cleared otherwise.
7 SF Sign Flag ── Set equal to high-order bit of result (0 is
positive, 1 if negative).
11 OF Overflow Flag ── Set if result is too large a positive number
or too small a negative number (excluding sign-bit) to fit in
destination operand; cleared otherwise.*/
#define push_data(word,len) do{cpu.esp-=len;swaddr_write(cpu.esp,len,word);}while(0)
#define pop_data(word,len) do{panic("Please Implement me!");}while(0)


extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];
extern const char* regsflag[];

#endif
