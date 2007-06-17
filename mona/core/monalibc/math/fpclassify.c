#include <math.h>

union __nc_ieee_754_double
{
	double value;
	struct
	{
		unsigned int frac1:32;
		unsigned int frac0:20;
		unsigned int exp:11;
		unsigned int sign:1;
	} n;
};

union __nc_ieee_754_single
{
	double value;
	struct
	{
		unsigned int frac:23;
		unsigned int exp:8;
		unsigned int sign:1;
	} n;
};

int fpclassify_d(double x)
{
	union __nc_ieee_754_double u;
	u.value = x;
	if( u.n.exp == 0 && u.n.frac1 == 0 && u.n.frac0 == 0 ) return FP_ZERO;
	if( u.n.exp == 255 && u.n.frac1 == 0 && u.n.frac0 == 0 ) return FP_INFINITE;
	if( u.n.exp == 255 && u.n.frac1 != 0 && u.n.frac0 != 0 ) return FP_NAN;
	return FP_NORMAL;
}

int fpclassify_f(float x)
{
	union __nc_ieee_754_single u;
	u.value = x;
	if( u.n.exp == 0 && u.n.frac == 0 ) return FP_ZERO;
	if( u.n.exp == 255 && u.n.frac == 0 ) return FP_INFINITE;
	if( u.n.exp == 255 && u.n.frac != 0 ) return FP_NAN;
	return FP_NORMAL;
}

