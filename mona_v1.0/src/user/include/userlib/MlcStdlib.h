/*!
  \file   MlcStdlib.h
  \brief  mona c standard library

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/29
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_STDLIB_
#define _MONA_LIB_STDLIB_

#include <types.h>

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
#define __FBSDID(s)

typedef __SIZE_TYPE__ size_t;

#ifdef __cplusplus
extern "C" {
#endif

#define S_FORMAT_STAR 0x01
#define S_FORMAT_LONG 0x02
#define S_FORMAT_LONGLONG 0x04
#define S_FORMAT_UNSIGNED 0x08

long int strtol(const char *s, char **endptr, int base);
unsigned long int strtoul(const char *s, char **endptr, int base);
size_t strtoi(const char *s, char **endptr, int base, int width, char flag);
int atoi(const char *s);

#ifdef __cplusplus
}
#endif

#endif
