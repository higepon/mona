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

  size_t result = x;
  size_t i;
  for(i = 1; i < y; i++){
    result *= x;
  }
  return result;
}

int strcpy2(char *s1, const char *s2){
  char *tmp = s1;

  while((*tmp++ = *s2++));

  return (int)(tmp - s1);
}
