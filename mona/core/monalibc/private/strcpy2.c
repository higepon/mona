#include <monalibc.h>
/*!
  \file   strcpy2.c
  \brief  mona c library

  Copyright (c) 2002,2003,2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/17
  \author  shadow

  $Revision: 3290 $
  $Date: 2006-06-19 00:27:38 +0900 (月, 19  6月 2006) $
*/
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
