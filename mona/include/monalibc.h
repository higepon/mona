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

#include <sys/types.h>
#include <monalibc/ctype.h>
#include <monalibc/limits.h>
#include <monalibc/stdarg.h>
#include <monalibc/stdlib.h>
#include <monalibc/stdio.h>
#include <monalibc/math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define P_FLAG_WIDTH 0x01
#define P_FLAG_PRECISION 0x02

#define P_FORMAT_ZERO 0x01
#define P_FORMAT_MINUS 0x02
#define P_FORMAT_PLUS 0x04
#define P_FORMAT_SPACE 0x08
#define P_FORMAT_SHARP 0x10
#define P_FORMAT_UNSIGNED 0x20
#define P_FORMAT_CAPITAL 0x40
#define P_FORMAT_TERMINATE 0x80
	
size_t __power(size_t x, size_t y);
int strcpy2(char *s1, const char *s2);
int strncpy2(char *s1, const char *s2, int n);
void memswap(char *a, char *b, size_t size);
int itos(char *s, int n, int width, unsigned int base, char flag);
int itosn(char *s, int max_width, int n, int width, unsigned int base, char flag);
int uitosn(char* s, int max_width, unsigned int n, int real_width, unsigned int base, char flag);

int ftos(char *s, double n, int width, int precision, char flag);
size_t strtoi(const char *s, char **endptr, int base, int width, char flag);

int init_libc();
int fini_libc();


#ifdef __cplusplus
}
#endif

#endif
