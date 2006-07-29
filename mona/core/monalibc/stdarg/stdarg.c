/*!
  \file   MlcStdarg.cpp
  \brief  mona c standard argument

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision: 3290 $
  $Date: 2006-06-19 00:27:38 +0900 (月, 19  6月 2006) $
*/

#include <monalibc.h>
#include <monalibc/assert.h>
#include <monapi/string.h>
#include <monapi/syscall.h>

/*!
  \brief 

  \param stream file stream printed characters
  \param format specifies how subsequent arguments
  \param ap     a variable number of arguments
  \return  the number of characters printed
 */
int vfprintf(FILE *stream, const char *format, va_list ap)
{
	char buf[5096];

	vsprintf(buf, format, ap);
	printf(buf);

	return strlen(buf);
}

/*!
  \brief equivalent to the function sprintf

  \param s      buf printed characters
  \param format specifies how subsequent arguments
  \param arg    a variable number of arguments
  \return  the number of characters printed (including '\\0' end of string)
*/
int vsprintf(char *s, const char *format, va_list arg){
  int result = 0;
  int loop;
  int i;
  int width;
  char flag;
  int precision;
  char p_flag;

  if(s == NULL) return 0;
  for(i = 0; format[i] != '\0'; i++){
    if(format[i] == '%'){
      loop = 1;
      flag = 0;
      p_flag = 0;
      width = 0;
      precision = 0;
      while(loop){
        i++;
        switch(format[i]){
          case 's':
            result += strcpy2(&s[result], va_arg(arg, char *));
            result--;
            loop = 0;
            break;
          case 'd':
          case 'i':
            result += itos(&s[result], va_arg(arg, int), width, 10, flag);
            loop = 0;
            break;
          case 'o':
            flag |= P_FORMAT_UNSIGNED;
            result += itos(&s[result], va_arg(arg, int), width, 8, flag);
            loop = 0;
            break;
          case 'u':
            flag |= P_FORMAT_UNSIGNED;
            result += itos(&s[result], va_arg(arg, int), width, 10, flag);
            loop = 0;
            break;
          case 'X':
            flag |= P_FORMAT_CAPITAL;
          case 'x':
            flag |= P_FORMAT_UNSIGNED;
            result += itos(&s[result], va_arg(arg, int), width, 16, flag);
            loop = 0;
            break;
          case 'f':
            result += ftos(&s[result], va_arg(arg, double), width, precision, flag);
            loop = 0;
            break;
          case 'c':
            s[result++] = va_arg(arg, char);
            loop = 0;
            break;
          case '%':
            s[result++] = '%';
            loop = 0;
            break;
          case '-':
            flag |= P_FORMAT_MINUS;
            break;
          case '+':
            flag |= P_FORMAT_PLUS;
            break;
          case ' ':
            flag |= P_FORMAT_SPACE;
            break;
          case '0':
            if(!(p_flag & P_FLAG_WIDTH) && !(p_flag & P_FLAG_PRECISION)){
              flag |= P_FORMAT_ZERO;
              break;
            }
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            if(p_flag & P_FLAG_PRECISION){
              precision = precision*10 + (int)(format[i] - '0');
            } else {
              width = width*10 + (int)(format[i] - '0');
              p_flag |= P_FLAG_WIDTH;
            }
            break;
          case '.':
            p_flag |= P_FLAG_PRECISION;
            break;
          case '\0':
            i--;
            loop = 0;
            break;
        }
      }
    } else {
      s[result++] = format[i];
    }
  }
  s[result] = 0;
  return result;
}

/*!
  
 */
int vsnprintf(char *str, size_t size, const char* format, va_list ap)
{
	int len;
	len = vsprintf(str, format, ap);
	assert(len <= (int)size);

	return len;
}

