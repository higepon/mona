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
#include <monapi/syscall.h>

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


int syscall_log_print(const char* msg)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_LOG_PRINT, result, msg);
    return result;
}

/*!
  \brief logprintf

  \author Higepon
  \param format specifies how subsequent arguments
  \return  none
*/
void logprintf(const char* format, ...) {

    char str[512];
    str[0] = '\0';
    va_list args;
    int result;

    va_start(args, format);
    result = vsprintf(str, format, args);
    va_end(args);
    if(result > 512){
        /* over flow */
        syscall_log_print("logprintf:overflow");
    }

    syscall_log_print(str);
}
