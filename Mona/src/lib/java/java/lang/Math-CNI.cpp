// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/lang/Math.h>
#include <gcj/cni.h>
#define MONA
#ifdef MONA
#define LN2 0.6931471805599453094172321 /* log(2) */
#define NAPIER 2.71828182845904523536
#define DBL_EPSILON  2.2204460492503131e-016
#define DBL_MAX  1.7976931348623158e+308
#define HUGE_VAL DBL_MAX
#else
#include <math.h>
#endif

jdouble
java::lang::Math::sqrt (jdouble a)
{
#ifdef MONA
	jdouble result;
	asm volatile ("fsqrt" : "=t"(result) : "0"(a));
	return result;
#else
	return ::sqrt(a);
#endif
}


jdouble
java::lang::Math::sin (jdouble a)
{
#ifdef MONA
	jdouble result;
	asm volatile ("fsin" : "=t"(result) : "0"(a));
	return result;
#else
	return ::sin(a);
#endif
}


jdouble
java::lang::Math::cos (jdouble a)
{
#ifdef MONA
	jdouble result;
	asm volatile ("fcos" : "=t"(result) : "0"(a));
	return result;
#else
	return ::cos(a);
#endif
}


jdouble
java::lang::Math::tan (jdouble a)
{
#ifdef MONA
	jdouble result;
	asm volatile ("fptan" : "=t"(result) : "0"(a));
	return result;
#else
	return ::tan(a);
#endif
}


jdouble
java::lang::Math::acos (jdouble a)
{
#ifdef MONA
	jdouble result;

    asm volatile("fld    %%st    \n"
                 "fmul   %%st(0) \n"
                 "fld1           \n"
                 "fsubp          \n"
                 "fsqrt          \n"
                 "fxch   %%st(1) \n"
                 "fpatan         \n"
                 : "=t" (result) : "0" (a) : "st(1)");

	return result;
#else
	return ::acos(a);
#endif
}


jdouble
java::lang::Math::atan (jdouble a)
{
#ifdef MONA
	jdouble result;
	asm volatile("fld1; fpatan" : "=t"(result) : "0" (a) : "st(1)");
	return result;
#else
	return ::atan(a);
#endif
}


jdouble
java::lang::Math::atan2 (jdouble y, jdouble x)
{
#ifdef MONA
	jdouble result;
	asm volatile("fpatan" : "=t" (result) : "0" (y), "u" (x) : "st(1)");
	return result;
#else
	return ::atan2(y, x);
#endif
}


jdouble
java::lang::Math::log (jdouble a)
{
#ifdef MONA
	int n = 1;
	jdouble tmp1, tmp2, result = 0;

	if (a <= 0) return 0;
	while (a >= 2) { /* to a<2 */ /* ln(a)=ln(a/2)+ln(2) */
		a /= 2;
		result += LN2;
	}
	while (a < 1) {  /* to a>=1 */ /* ln(a)= ln(2a)-ln(2) */
		a *= 2;
		result -= LN2;
	}
	/* 1 <= a< 2 (?) : ln(a) = sigma (2/(2k+1))*(((a-1)/(a+1))^(2k+1)) */
	a = (a-1)/(a+1);
	tmp1 = 2 * a;
	a = a * a;
	do {
		tmp2 = result;
		result += tmp1/n;
		tmp1 *= a;
		n += 2;
	} while (result != tmp2);

	return result;
#else
	return ::log(a);
#endif
}


jdouble
java::lang::Math::floor (jdouble a)
{
#ifdef MONA
	if (a < 0) {
		jdouble b = -a;
		jdouble c = (b >= 0) ? b : -b;
		int i = (int)b;
		jdouble f = c - (jdouble)i;
		a = (b < 0) ? -i : i;
		jdouble result = (b < 0) ? -f : f;
		if (result != 0) a++;
		return -a;
	} else {
		jdouble b = a;
		int i = (int)b;
		a = (b < 0) ? -i : i;
		return a;
	}
#else
	return ::floor(a);
#endif
}


jdouble
java::lang::Math::exp (jdouble a)
{
#ifdef MONA
	int ix, n = 1;
	jdouble bx = abs(a);
	jdouble b = 1.0;
	jdouble tmp1;
	jdouble result = 1.0;

	/* exp(2.3) = exp(2)*exp(0.3) */
	/* exp(a) = 1 + a^1/1! + a^2/2! + a^3/3! ... + a^k/k! */
	if (bx < DBL_EPSILON) return 1;
	ix = (int)floor(bx); /* integer part */
	bx -= ix;            /* fraction part */

	do {
		tmp1 = result;
		b *= bx/n;
		result += a;
		n++;
	} while (result != tmp1);

	b = NAPIER;
	while (ix > 0) {
		if(ix & 1) { /* ix is odd */
			if(a >= 0 && b > DBL_MAX/result) return HUGE_VAL;
			if(a < 0 && b > DBL_MAX/result) return 0;
			result *= a;
		}
		ix /= 2;
		b = b * b;
	}

	if(a >= 0) return result;
	else return 1.0/result;
#else
	return ::exp(a);
#endif
}


jdouble
java::lang::Math::pow (jdouble a, jdouble b)
{
#ifdef MONA
	long int n = (long int)b;
	jdouble result = 1;

	if (a == 0) {
		if(b <= 0) return 1; /* 0^b = 1 (b <= 0) */
		return 0;            /* 0^b = 0 (b > 0)  */
	}

	if (b == n) { /* b is integer */
		char sign = 0;
		if (n < 0) {
			sign = 1;
			n = -n;
			a = 1/a;
		}
		while (1) {
			if (n & 1) result *= a; /* n is odd */
			n /= 2;
			if (n == 0) break;
			a *= a;
		}
		return result;
	}

	if(a < 0) return 0; /* error: result is imaginary */

	return exp(b * log(a));
#else
	return ::pow(a, b);
#endif
}


