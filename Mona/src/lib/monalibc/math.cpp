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
    return sin(angle) / cos(angle);
}

double sqrt(double num)
{
    double result;
    asm volatile ("fsqrt" : "=t"(result) : "0"(num));
    return result;
}

double atan(double x, double y)
{
    double result;
    asm volatile("fpatan" : "=t" (result) : "0" (x), "u" (y) : "st(1)");
    return result;
}
