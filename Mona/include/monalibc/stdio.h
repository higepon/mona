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

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int sprintf(char *s, const char *format, ...);
int sscanf(const char *s, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
