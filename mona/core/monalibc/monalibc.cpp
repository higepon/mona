/*!
  \file   monalibc.cpp
  \brief  mona c library

  Copyright (c) 2002,2003,2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/17
  \author  shadow

  $Revision$
  $Date$
*/
#include <monalibc.h>

size_t __power(size_t x, size_t y){

  size_t result = 1;
  size_t i;
  for(i = 0; i < y; i++){
    result *= x;
  }
  return result;
}

int strcpy2(char *s1, const char *s2){
  char *tmp = s1;

  while((*tmp++ = *s2++));

  return (int)(tmp - s1);
}

int strncpy2(char *s1, const char *s2, int n){
  char *tmp = s1;
  int result;

  while((n-- > 0) && (*tmp++ = *s2++));
  result = (int)(tmp - s1);
  while(n-- > 0) *tmp++ = '\0';

  return result;
}

void memswap(char *a, char *b, size_t size){

  size_t i;
  char tmp;

  for(i = 0; i < size; i++){
    tmp = a[i];
    a[i] = b[i];
    b[i] = tmp;
  }

  return;
}
