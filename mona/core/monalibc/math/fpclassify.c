#include <math.h>
#include "ieee_754.h"

int fpclassify_d(double x)
{
	union __nc_ieee_754_double u;
	u.value = x;
	if( u.n.exp == 0 && u.n.frac1 == 0 && u.n.frac0 == 0 ) return FP_ZERO;
	if( u.n.exp == 2047 && u.n.frac1 == 0 && u.n.frac0 == 0 ) return FP_INFINITE;
	if( u.n.exp == 2047/* && u.n.frac1 != 0 && u.n.frac0 != 0*/ ) return FP_NAN;
	if( u.n.sign == 0 && !(u.n.frac1 == 0 && u.n.frac0 == 0) ) return FP_SUBNORMAL;
	return FP_NORMAL;
}

int fpclassify_f(float x)
{
	union __nc_ieee_754_single u;
	u.value = x;
	if( u.n.exp == 0 && u.n.frac == 0 ) return FP_ZERO;
	if( u.n.exp == 255 && u.n.frac == 0 ) return FP_INFINITE;
	if( u.n.exp == 255 && u.n.frac != 0 ) return FP_NAN;
	if( u.n.sign == 0 && u.n.frac != 0 ) return FP_SUBNORMAL;
	return FP_NORMAL;
}

