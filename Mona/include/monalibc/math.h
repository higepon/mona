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

#ifdef __cplusplus
extern "C" {
#endif

double sin(double angle);
double cos(double angle);
double tan(double angle);
double sqrt(double num);
double atan2(double x, double y);
double acos(double x);

#ifdef __cplusplus
}
#endif

#endif
