/*!
  \file   sprintf.c
  \brief  sprintf

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/09
  \author  shadow

  $Revision: 3290 $
  $Date: 2006-06-19 00:27:38 +0900 (月, 19  6月 2006) $
*/

#include <stdarg.h>
#include <stdio.h>

/*!
  \brief sprintf

  \param s      buf printed characters
  \param format specifies how subsequent arguments
  \return  the number of characters printed (including '\\0' end of string)
*/
int sprintf(char *s, const char *format, ...){
  int result;
  va_list args;

  va_start(args, format);
  result = vsprintf(s, format, args);
  va_end(args);

  return result;
}

int init_libc()
{
    syscall_print("init_libc");
    return MONA_SUCCESS;
}

int fini_libc()
{
    syscall_print("fini_libc");
    return MONA_SUCCESS;
}
