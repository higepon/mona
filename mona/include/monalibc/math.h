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

double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double x, double y);
double ceil(double x);
double cos(double angle);
double cosh(double x);
double exp(double x);
double fabs(double x);
double floor(double x);
double fmod(double x, double y);
double frexp(double value, int *exp);
double ldexp(double x, int n);
double log(double x);
double log10(double x);
double modf(double x, double *buf);
double nearbyint(double x);
double pow(double x, double y);
double round(double x);
double sin(double angle);
double sinh(double x);
double sqrt(double num);
double tan(double angle);
double tanh(double x);

#ifdef __cplusplus
}
#endif

#endif
