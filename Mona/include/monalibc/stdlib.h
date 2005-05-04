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

#include <sys/types.h>

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

#define S_FORMAT_STAR 0x01
#define S_FORMAT_LONG 0x02
#define S_FORMAT_LONGLONG 0x04
#define S_FORMAT_UNSIGNED 0x08

typedef struct{
  int quot; /* quotient */
  int rem;  /* remainder */
} div_t;

typedef struct{
  long int quot; /* quotient */
  long int rem;  /* remainder */
} ldiv_t;

long int strtol(const char *s, char **endptr, int base);
unsigned long int strtoul(const char *s, char **endptr, int base);
size_t strtoi(const char *s, char **endptr, int base, int width, char flag);
int atoi(const char *s);
int itos(char *s, int n, int width, unsigned int base, char flag);
int itosn(char *s, int max_width, int n, int width, unsigned int base, char flag);
int ftos(char *s, double n, int width, int precision, char flag);
int abs(int i);
long int labs(long int i);
div_t div(long int numer, long int denom);
ldiv_t ldiv(long int numer, long int denom);
void qsort(void *base, size_t n, size_t size, int (*fnc)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif
