/*!
  \file   math.cpp
  \brief  math

  Copyright (c) 2002-2004 Higepon

  $Revision$
  $Date$
*/

#include <monalibc/math.h>
#include <monalibc/float.h>

double sin(double angle)
{
    double result;
    asm volatile ("fsin" : "=t"(result) : "0"(angle));
    return result;
}

double cos(double angle)
{
    double result;
    asm volatile ("fcos" : "=t"(result) : "0"(angle));
    return result;
}

double tan(double angle)
{
    double result;
    asm volatile ("fptan" : "=t"(result) : "0"(angle));
    return result;
}

double sqrt(double num)
{
    double result;
    asm volatile ("fsqrt" : "=t"(result) : "0"(num));
    return result;
}

double atan2(double x, double y)
{
    double result;
    asm volatile("fpatan" : "=t" (result) : "0" (x), "u" (y) : "st(1)");
    return result;
}

double atan(double x)
{
    double result;
    asm volatile("fld1; fpatan" : "=t"(result) : "0" (x) : "st(1)");
    return result;
}

#define PI 3.14159265358979323846

double asin(double x)
{
  double z;
  if(fabs(x) > 1){
    //errno = EDOM;
    return 0;
  }
  z = sqrt(1 - x*x);
  if(z != 0) return atan(x/z);
  if(x > 0) return PI/2;
  else return -PI/2;

}

/* acos = atan (sqrt(1 - x^2) / x) */
double acos(double x)
{
  double result;

  asm volatile("fld    %%st    \n"
               "fmul   %%st(0) \n"
               "fld1           \n"
               "fsubp          \n"
               "fsqrt          \n"
               "fxch   %%st(1) \n"
               "fpatan         \n"
               : "=t" (result) : "0" (x) : "st(1)");

  return result;
}

double sinh(double x)
{
  return (exp(x) - exp(-x))/2.0;
}

double cosh(double x)
{
  return (exp(x) + exp(-x))/2.0;
}

double tanh(double x)
{
  double p = exp(x);
  double m = exp(-x);
  //if(p+m == 0) no need?
  return (p-m)/(p+m);
}

double fabs(double x)
{
    return x < 0 ? -x : x;
}

double modf(double x, double *buf)
{
    double a = fabs(x);
    int i = (int)x;

    double f = a - (double)i;

    *buf = x < 0 ? -i : i;

    return x < 0 ? -f : f;
}

double floor(double x)
{
    if (x < 0)
    {
        if (modf(-x, &x) != 0) x++;
        return -x;
    }
    modf(x, &x);
    return x;
}

double log(double x){

  int n = 1;
  double tmp1, tmp2, result = 0;

  if(x <= 0) return 0;
  while(x >= 2){ /* to x<2 */ /* ln(a)=ln(a/2)+ln(2) */
    x /= 2;
    result += LN2;
  }
  while(x < 1){  /* to x>=1 */ /* ln(a)= ln(2a)-ln(2) */
    x *= 2;
    result -= LN2;
  }
  /* 1<=x<2(?): ln(x) = sigma (2/(2k+1))*(((x-1)/(x+1))^(2k+1)) */
  x = (x-1)/(x+1);
  tmp1 = 2*x;
  x = x*x;
  do{
    tmp2 = result;
    result += tmp1/n;
    tmp1 *= x;
    n += 2;
  } while(result != tmp2);

  return result;
}

double log10(double x){

  return log(x)/2.30258092994057; /* log10(x) = ln(x)/ln(10) */
}


double exp(double x){

  int ix, n = 1;
  double ax = fabs(x), a = 1.0;
  double tmp1, tmp2;
  double result = 1.0;

  /* exp(2.3) = exp(2)*exp(0.3) */
  /* exp(x) = 1 + x^1/1! + x^2/2! + x^3/3! ... + x^k/k! */
  if(ax < DBL_EPSILON) return 1;
  ix = (int)floor(ax); /* integer part */
  ax -= ix;       /* fraction part */

  do{
    tmp1 = result;
    a *= ax/n;
    result += a;
    n++;
  } while(result != tmp1);

  a = NAPIER;
  while(ix > 0){
    if(ix&1){ /* ix is odd */
      if(x >= 0 && a > DBL_MAX/result) return HUGE_VAL;
      if(x < 0 && a > DBL_MAX/result) return 0;
      result *= a;
    }
    ix /= 2;
    a = a*a;
  }

  if(x >= 0) return result;
  else return 1.0/result;
}

double ldexp(double x, int n){

  int *iptr = (int *)&x;
  n += (iptr[3]>>4) & 0x3ff;
  if(n < 1) x = 0;
  else if(n > 0x7fff){
    iptr[3] |= 0x7fff;
    iptr[2] = iptr[1] = iptr[0] = -1;
    //errno = ERANGE;
  }
  iptr[3] = (iptr[3] & 0x800f)|(n << 4);

  return x;

}

double pow(double x, double y){

  long int n = y;
  double result = 1;

  if(x == 0){
    if(y <= 0) return 1; /* 0^y = 1 (y <= 0) */
    return 0;            /* 0^y = 0 (y > 0)  */
  }

  if(y == n){ /* y is integer */
    char sign = 0;
    if(n < 0){
      sign = 1;
      n = -n;
      x = 1/x;
    }
    while(1){
      if(n&1) result *= x; /* n is odd */
      n /= 2;
      if(n == 0) break;
      x *= x;
    }
    return result;
  }

  if(x < 0) return 0; /* error: result is imaginary */

  return exp(y*log(x));
}

/*!
  \brief ceiling

  \param x double
  \return smallest integral value not less than argument
*/
double ceil(double x){

  return -floor(-x); /* -[-x] */
}

/* 
typedef union
{
	double d;
	struct
	{
		long low;
		long hi;
	};
	struct
	{
		long low:32;
		int hi:20;
		signed int exponent:11;
		int sign:1;
	};
} doublebit; */

/*
double fabs(double x)
{
	return ((doublebit)x).hi & 0x7fffffffLL;
}*/

// Damepo implementation.
/*double frexp(double x, int *exp)
{
	double n = ((doublebit)x).low + (double)((doublebit)x).hi*(double)0xffffffff;

	*exp = ((doublebit)x).exponent;

	return (double)n;
}*/

double frexp(double x, int *exp){

  int *t = (int *)&x;
  if(x == 0){
    *exp = 0;
    return x;
  }
  *exp = ((t[3] >> 4) & 0x7ff) - 0x3fe;
  t[3] = (t[3] & 0x800f) + 0x3fe0;

  return x;
}

double fmod(double x, double y)
{

	double result;
	if(x == 0 || y == 0) result = 0.0;
	else if(x > 0) result = x - (unsigned long int)fabs(x/y)*fabs(y);
	else result = x + (unsigned long int)fabs(x/y)*fabs(y);

	return result;
}
