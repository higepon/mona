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

double sqrt(double num)
{
    double result;
    asm volatile ("fsqrt" : "=t"(result) : "0"(num));
    return result;
}
