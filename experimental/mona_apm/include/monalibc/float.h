/*!
  \file   float.h
  \brief  monalibc float

  Copyright (c) 2002-2005 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2005/05/03
  \author  shadow

  $Revision$
  $Date$
*/

#ifndef _FLOAT_H_
#define _FLOAT_H_

#ifdef	__cplusplus
extern "C" {
#endif

#define FLT_ROUNDS    1
#define FLT_RADIX     2

/* FLT_RADIX DIGit in MANTissa */
#define FLT_MANT_DIG  24
#define DBL_MANT_DIG  53
#define LDBL_MANT_DIG 53

/* decimal DIGit precision */
#define FLT_DIG   6
#define DBL_DIG  15
#define LDBL_DIG 15

/* distinction limit: min e : 1+e != 1 */
#define FLT_EPSILON  1.192092896e-07F
#define DBL_EPSILON  2.2204460492503131e-016
#define LDBL_EPSILON 2.2204460492503131e-016

/* min n : FLT_RADIX^n is expressible as float */
#define FLT_MIN_EXP   -125
#define DBL_MIN_EXP  -1021
#define LDBL_MIN_EXP -1021

/* max n : FLT_RADIX^n is expressible as float */
#define FLT_MAX_EXP   128
#define DBL_MAX_EXP  1024
#define LDBL_MAX_EXP 1024

/* min n : 10^n is expressible as float */
#define FLT_MIN_10_EXP   -37
#define DBL_MIN_10_EXP  -307
#define LDBL_MIN_10_EXP -307

/* max n : 10^n is expressible as float */
#define FLT_MAX_10_EXP   38
#define DBL_MAX_10_EXP  308
#define LDBL_MAX_10_EXP 308

/* min of float */
#define FLT_MIN  1.175494351e-38F
#define DBL_MIN  2.2250738585072014e-308
#define LDBL_MIN 2.2250738585072014e-308

/* max of float */
#define FLT_MAX  3.402823466e+38F
#define DBL_MAX  1.7976931348623158e+308
#define LDBL_MAX 1.7976931348623158e+308

#ifdef	__cplusplus
}
#endif

#endif /* _MONA_LIB_FLOAT_ */
