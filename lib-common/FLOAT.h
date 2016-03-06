#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "trap.h"

typedef int FLOAT;
//#define FLESS0(a) ((a)&(1<<31))
//#define FMINUS(a) ((a)^(1<<31))
//#define FABS(a) ((a)&2147483647)
#define FLESS0(a) ((a)<0)
#define FMINUS(a) (-(a))
#define FABS(a) ((a)<0?-(a):(a))
#define ABS(a) ((a)<0?-(a):(a))
static inline int F2int(FLOAT a) {
	FLOAT b=FABS(a);
	int res=b>>16;
	return FLESS0(a)?-res:res;
}

static inline FLOAT int2F(int a) {
	int b=ABS(a);
	FLOAT res=b<<16;
	return a<0?FMINUS(res):res;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
	FLOAT c=FABS(a);
	FLOAT res=c*b;
	return FLESS0(a)?FMINUS(res):res;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
	FLOAT c=FABS(a);
	FLOAT res=c/b;
	return FLESS0(a)?FMINUS(res):res;
}
FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT sqrt(FLOAT);
FLOAT pow(FLOAT, FLOAT);

#endif
