/*!
  \file   MlcStdio.h
  \brief  mona c standard Input/Output

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_STDIO_
#define _MONA_LIB_STDIO_

#include <types.h>

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
#define __FBSDID(s)

typedef __SIZE_TYPE__ size_t;

#ifdef __cplusplus
extern "C" {
#endif

int sprintf(char *s, const char *format, ...);
int sscanf(const char *s, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
