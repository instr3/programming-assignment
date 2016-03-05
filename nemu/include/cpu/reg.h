#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

typedef union CR0 {
	struct {
		uint32_t protect_enable      : 1;
		uint32_t monitor_coprocessor : 1;
		uint32_t emulation           : 1;
		uint32_t task_switched       : 1;
		uint32_t extension_type      : 1;
		uint32_t numeric_error       : 1;
		uint32_t pad0                : 10;
		uint32_t write_protect       : 1; 
		uint32_t pad1                : 1; 
		uint32_t alignment_mask      : 1;
		uint32_t pad2                : 10;
		uint32_t no_write_through    : 1;
		uint32_t cache_disable       : 1;
		uint32_t paging              : 1;
	};
	uint32_t val;
} CR0;

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
	//CR0
	CR0 cr0;
	//GDTR
	union
	{
		uint64_t gdtr : 48;
		#pragma pack(2)
		struct
		{
			uint16_t gdtr_limit;
			uint32_t gdtr_base;
		};
		#pragma pack()
	};
	//Segment registers
	uint16_t cs,ds,es,ss;
	//Invisible part for segment register used as cache
	struct invisible_part
	{
		uint32_t limit;
		uint32_t base;
	} cs_inv,ds_inv,es_inv,ss_inv;

} CPU_state;

typedef union
{
	uint16_t selector;
	struct
	{
		uint16_t rpl : 2;
		uint16_t ti : 1;
		uint16_t index : 13;
	};
} segmentselector_t;

typedef union
{
	uint64_t item;
	struct
	{
		uint16_t seg_limit_0_15;
		uint16_t seg_base_0_15;
		uint8_t seg_base_16_23;
		uint8_t type : 5;
		uint8_t dpl : 2;
		uint8_t p : 1;
		uint8_t seg_limit_16_19 : 4;
		uint8_t avl : 1;
		uint8_t o : 1;
		uint8_t x : 1;
		uint8_t g : 1;
		uint8_t seg_base_24_31;
	};
} gdtitem_t;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])
#define reg_flag(index) (!!(cpu.eflags&1<<(index)))
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
#define highestbit(res) ((res)&1ull<<(8*DATA_BYTE-1))

#define influence_zf(res) reg_flag_set(EFLAGS_ZF,!(res))
#define influence_pf(res) reg_flag_set(EFLAGS_PF,~__builtin_popcount(0xff&(res))&1)
#define influence_sf(res) reg_flag_set(EFLAGS_SF,highestbit(res))
#define influence_of(res,op1,op2) reg_flag_set(EFLAGS_OF,highestbit(op1)==highestbit(op2)&& \
	highestbit(op1)!=highestbit(res))
#define influence_cf(res,resull,sub) reg_flag_set(EFLAGS_CF,((res)!=(resull))^(sub))
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
#define push_data(word,len) do{cpu.esp-=(len);swaddr_write(cpu.esp,len,word,SREG_SS);}while(0)
#define pop_data(word,len) do{word=swaddr_read(cpu.esp,len,SREG_SS);cpu.esp+=(len);}while(0)
#define string_advance(word,step) (reg_flag(EFLAGS_DF)?((word)-=(step)):((word)+=(step)))


/*
Sreg: a segment register. The segment register bit assignments are ES=0,
CS=1, SS=2, DS=3, FS=4, and GS=5.
*/
#define NO_SREG -1
#define SREG_ES 0
#define SREG_CS 1
#define SREG_SS 2
#define SREG_DS 3
#define SREG_FS 4
#define SREG_GS 5

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];
extern const char* regsflag[];

#endif
