/*!
  \file   MlcLimits.h
  \brief  mona c limits

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/29
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_LIMITS_
#define _MONA_LIB_LIMITS_

#ifdef __cplusplus
extern "C" {
#endif

#define M_CHAR_BIT 8
#define M_SCHAR_MIN -127
#define M_SCHAR_MAX 127
#define M_UCHAR_MAX 255
#define M_CHAR_MIN M_SCHAR_MIN
#define M_CHAR_MAX M_UCHAR_MAX

#define M_INT_MIN -2147483647
#define M_INT_MAX 2147483647
#define M_UINT_MAX 4294967295

#define M_LONG_MIN -2147483647
#define M_LONG_MAX 2147483647
#define M_ULONG_MAX 4294967295

#ifdef __cplusplus
}
#endif

#endif
