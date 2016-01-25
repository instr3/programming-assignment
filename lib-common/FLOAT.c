#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	FLOAT c=FABS(a),d=FABS(b);
	FLOAT res=c*(long long)d/65536LL;
	return (FLESS0(a)^FLESS0(b))?FMINUS(res):res;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	FLOAT c=FABS(a),d=FABS(b);
	unsigned rem=c%d,res=c/d;
	int i;
	for(i=0;i<8;++i)
	{
		rem*=4;
		res=res*4+rem/d;
		rem=rem%d;
	}
	//FLOAT res=(long long)c*(1<<16)/(long long)d;
	return (FLESS0(a)^FLESS0(b))?FMINUS(res):res;
}

FLOAT f2F(float a) {
	int ua=*(int *)&a;
	int va=FABS(ua);
	FLOAT res=0;
	if(va!=0)
	{
		int ex=(va>>23)-127+16;
		if(ex>=0)
			res=(va&0x7fffff)<<ex;
		else
			res=(va&0x7fffff)>>-ex;
	}
	return FLESS0(ua)?FMINUS(res):res;
}

FLOAT Fabs(FLOAT a) {
	return FABS(a);
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

