/*!
  \file   math.h
  \brief  Math

  Copyright (c) 2002-2004 Higepon

  \author  Higepon
  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_MATH_
#define _MONA_LIB_MATH_

#include <monalibc/float.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define HUGE_VAL (__builtin_huge_val())
#define HUGE_VAL DBL_MAX

#define LN2 0.6931471805599453094172321 /* log(2) */
#define NAPIER 2.71828182845904523536

double sin(double angle);
double cos(double angle);
double tan(double angle);
double sqrt(double num);
double atan2(double x, double y);
double atan(double x);
double asin(double x);
double acos(double x);
double sinh(double x);
double cosh(double x);
double tanh(double x);

double modf(double x, double *buf);
double floor(double x);
double log(double x);
double log10(double x);
double exp(double x);
double ldexp(double x, int n);
double pow(double x, double y);
double ceil(double x);

double fabs(double x);
double frexp(double x, int *exp);
double fmod(double x, double y);

#ifdef __cplusplus
}
#endif

#endif
