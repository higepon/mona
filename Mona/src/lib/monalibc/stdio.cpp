/*!
  \file   MlcStdio.cpp
  \brief  mona c standard input/output

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/09
  \author  shadow

  $Revision$
  $Date$
*/

#include <monalibc/stdarg.h>
#include <monalibc/stdlib.h>
#include <monalibc/stdio.h>


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

/*!
  \brief sscanf

  \param s      buf printed characters
  \param format specifies how subsequent arguments
  \return  the number of characters printed (including '\\0' end of string)
*/
int sscanf(const char *s, const char *format, ...){
  int result;
  va_list args;

  va_start(args, format);
  result = vsscanf(s, format, args);
  va_end(args);

  return result;
}

