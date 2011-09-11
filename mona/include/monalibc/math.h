/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is	 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************/

/* Please send bug reports to
	Shotaro Tsuji
	4-1010,
	Sakasedai 1-chome,
	Takaraduka-si,
	Hyogo-ken,
	665-0024
	Japan
	negi4d41@yahoo.co.jp
*/

#ifndef _MATH_H_
#define _MATH_H_

#include <monalibc/float.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HUGE_VAL	1e500
#define HUGE_VALF	1e50f
#define HUGE_VALL	1e5000
#define INFINITY	HUGE_VALF
#define LN2		0.6931471805599453094172321
#define NAPIER		2.71828182845904523536

#define M_E		2.7182818284590452354	/* e */
#define M_LOG2E		1.4426950408889634074	/* log 2e */
#define M_LOG10E	0.43429448190325182765	/* log 10e */
#define M_LN2		0.69314718055994530942  /* log e2 */
#define M_LN10          2.30258509299404568402  /* log e10 */
#define M_PI		3.14159265358979323846  /* pi */
#define M_PI_2		1.57079632679489661923  /* pi/2 */
#define M_PI_4		0.78539816339744830962  /* pi/4 */
#define M_1_PI		0.31830988618379067154  /* 1/pi */
#define M_2_PI		0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI	1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2		1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2	0.70710678118654752440  /* 1/sqrt(2) */

#define FP_NAN         0
#define FP_INFINITE    1
#define FP_ZERO        2
#define FP_SUBNORMAL   3
#define FP_NORMAL      4

#define fpclassify(x)   (sizeof(x) == sizeof(double) ? fpclassify_d(x) : \
			 sizeof(x) == sizeof(float) ? fpclassify_f(x) : \
			 FP_NAN)

#define isfinite(x) ((fpclassify(x) != FP_NAN && fpclassify(x) != FP_INFINITE)?1:0)
#define isnormal(x) ((fpclassify(x) == FP_NORMAL) ? 1 : 0)
#define isnan(x)    ((fpclassify(x) == FP_NAN) ? 1 : 0)
#define isinf(x)    ((fpclassify(x) == FP_INFINITE) ? 1 : 0)

#define  signbit(x) (sizeof(x) == sizeof(double) ? signbit_d(x) : signbit_f(x))

extern int fpclassify_d(double x);
extern int fpclassify_f(float x);
extern int signbit_d(double x);
extern int signbit_f(float x);
extern double __nand();

#define NAN ((double)__nand())

double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double ceil(double x);
double cos(double angle);
double cosh(double x);
double exp(double x);
double fabs(double x);
float fabsf(float x);
double floor(double x);
double trunc(double x);
double fmod(double x, double y);
double frexp(double value, int *exp);
double ldexp(double x, int n);
double log(double x);
double log10(double x);
double modf(double x, double *buf);
double nearbyint(double x);
double pow(double x, double y);
double round(double x);
long int lroundf(float x);
float roundf(float x);
double sin(double angle);
double sinh(double x);
double sqrt(double num);
double tan(double angle);
double tanh(double x);
float acosf(float x);
long double acosl(long double x);
float asinf(float x);
long double asinl(long double x);
float atan2f(float y, float x);
long double atan2l(long double y, long double x);
float atanf(float x);
long double atanl( long double x);
float ceilf(float x);
long double ceill(long double x);
float cosf(float x);
long double cosl(long double x);
float coshf(float x);
long double coshl(long double x);
float expf(float x);
long double expl(long double x);
float fabsf(float x);
long double fabsl(long double x);
float floorf(float x);
long double floorl(long double x);
float fmodf(float x, float y);
long double fmodl(long double x, long double y);
float frexpf(float x, int *exp);
long double frexpl(long double x, int *exp);
double hypot(double x, double y);
float ldexpf(float x, int exp);
long double ldexpl(long double x, int exp);
float log10f(float x);
long double log10l(long double x);
float logf(float x);
long double logl(long double x);
float modff(float x, float *iptr);
long double modfl(long double x, long double *iptr);
float powf(float x, float y);
long double powl(long double x, long double y);
float sinf(float x);
long double sinl(long double x);
float sinhf(float x);
long double sinhl(long double x);
float sqrtf(float x);
long double sqrtl(long double x);
float tanf(float x);
long double tanl(long double x);
float tanhf(float x);
long double tanhl(long double x);

double nextafter(double x, double y);

#ifdef __cplusplus
}
#endif

#endif
