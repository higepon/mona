/*!
  \file   math.cpp
  \brief  math

  Copyright (c) 2002-2004 Higepon

  $Revision$
  $Date$
*/

#include <monalibc/math.h>

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
