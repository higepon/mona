/*!
  \file   MlcStdarg.h
  \brief  mona c standard argument

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_STDARG_
#define _MONA_LIB_STDARG_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char *va_list;
#define va_start(ap,last) (void)((ap)=(va_list)&(last)+sizeof(last))
//#define va_arg(ap,type) (*(type*)(ap++))
#define va_arg(ap,type) ((type*)(ap+=sizeof(type)))[-1]
#define va_end(ap) (void)((ap)=NULL)

int vsprintf(char *s, const char *format, va_list arg);
int vsscanf(const char *s, const char *format, va_list arg);

#ifdef __cplusplus
}
#endif

#endif
