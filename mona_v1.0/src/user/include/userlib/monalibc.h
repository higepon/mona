/*!
  \file   monalibc.h
  \brief  standard library

  Copyright (c) 2002,2003,2004 shadow
  All rights reserved.
  License=NYSL

  \author  shadow
  \version $Revision$
  \date   create:  update:$Date$
*/
#ifndef _MONA_LIB_C_
#define _MONA_LIB_C_

#include <types.h>
#include <MlcCtype.h>
#include <MlcLimits.h>
#include <MlcStdarg.h>
#include <MlcStdlib.h>
#include <MlcStdio.h>

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;

#define __FBSDID(s)

#ifdef __cplusplus
extern "C" {
#endif

size_t __power(size_t x, size_t y);
int strcpy2(char *s1, const char *s2);
int strncpy2(char *s1, const char *s2, int n);

#ifdef __cplusplus
}
#endif

#endif
