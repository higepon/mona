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
#include <types.h>

#ifndef _MONA_LIB_C_
#define _MONA_LIB_C_

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
#define __FBSDID(s)

typedef __SIZE_TYPE__ size_t;

#ifdef __cplusplus
extern "C" {
#endif

#define P_FORMAT_ZERO 0x01
#define P_FORMAT_MINUS 0x02
#define P_FORMAT_PLUS 0x04
#define P_FORMAT_SPACE 0x08
#define P_FORMAT_SHARP 0x10
#define P_FORMAT_UNSIGNED 0x20
#define P_FORMAT_CAPITAL 0x40
#define P_FORMAT_TERMINATE 0x80

typedef char *va_list;
#define va_start(ap,last) (void)((ap)=(va_list)&(last)+sizeof(last))
#define va_arg(ap,type) (*((type*)(ap))++)
#define va_end(ap) (void)((ap)=NULL)

int vsprintf(char *s, const char *format, va_list arg);
int sprintf(char *s, const char *format, ...);
int itos(char *s, int n, int width, int base, char flag);
int atoi(const char *s);
int ftos(char *s, double n, int width, int precision, char flag);
int strcpy2(char *s1, const char *s2);

#ifdef __cplusplus
}
#endif

#endif
