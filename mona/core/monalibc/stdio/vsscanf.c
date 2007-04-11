/*!
  \file   sscanf.c
  \brief  
 
  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow
 
  $Revision: 3290 $
  $Date: 2006-06-19 00:27:38 +0900 (月, 19  6月 2006) $
*/

#include <monalibc/stdlib.h>
#include <monalibc/stdarg.h>
#include <monalibc/string.h>
#include <monapi/string.h>

/*!
  \brief equivalent to the function sscanf
 
  \param s      buf scanned characters
  \param format specifies how subsequent arguments
  \param arg    a variable number of arguments
  \return  the number of variables scanned
*/
int vsscanf(const char *s, const char *format, va_list arg) {
    int result = 0;
    int loop;
//  int i; /* comment out by higepon */
    int width;
//  int precision; /* comment out by higepon */
    char s_flag;
    const char *tmps = s;
    int *argiptr;
    char *argcptr;
    int length;
    char *tmpptr;

    while(*format != '\0') {
        if(*format == '%') {
            loop = 1;
            s_flag = 0;
            width = 0;

            format++;
            if(*format == '%') { /* %% */
                if(*tmps != '%') break;
                format++;
                tmps++;
                continue;
            }
            while(loop) {
                switch(*format) {
                case 's':
                    length =  strcspn(tmps, " ");
                    if((width == 0) || (width > length)) width = length;
                    argcptr = va_arg(arg, char *);
                    if((s_flag & S_FORMAT_STAR) || (argcptr == NULL)) { /* no output */
                        va_arg(arg, char *);
                    } else {
                        strncpy(argcptr, tmps, width);
                        argcptr[width] = '\0';
                        result++;
                    }
                    tmps += width;
                    if(*tmps == ' ') tmps++;
                    loop = 0;
                    break;
                case 'u':
                    s_flag |= S_FORMAT_UNSIGNED;
                case 'd':
                    argiptr = va_arg(arg, int *);
                    if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)) { /* no output */
                        strtoi(tmps, NULL, 10, width, s_flag);
                    } else {
                        *argiptr = strtoi(tmps, &tmpptr, 10, width, s_flag);
                        tmps = tmpptr;
                        result++;
                    }
                    loop = 0;
                    break;
                case 'o':
                    s_flag |= S_FORMAT_UNSIGNED;
                    argiptr = va_arg(arg, int *);
                    if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)) { /* no output */
                        strtoi(tmps, NULL, 8, width, s_flag);
                    } else {
                        *argiptr = strtoi(tmps, &tmpptr, 8, width, s_flag);
                        tmps = tmpptr;
                        result++;
                    }
                    loop = 0;
                    break;
                case 'X':
                case 'x':
                    s_flag |= S_FORMAT_UNSIGNED;
                    argiptr = va_arg(arg, int *);
                    if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)) { /* no output */
                        strtoi(tmps, NULL, 16, width, 0);
                    } else {
                        *argiptr = strtoi(tmps, &tmpptr, 16, width, s_flag);
                        tmps = tmpptr;
                        result++;
                    }
                    loop = 0;
                    break;
                case 'i':
                    argiptr = va_arg(arg, int *);
                    if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)) { /* no output */
                        strtoi(tmps, NULL, 0, width, s_flag);
                    } else {
                        *argiptr = strtoi(tmps, &tmpptr, 0, width, s_flag);
                        tmps = tmpptr;
                        result++;
                    }
                    loop = 0;
                    break;
                    /*
                    case 'f':
                      
                      loop = 0;
                      break;*/
                case 'c':
                    if(width == 0) width = 1;
                    while(width--) {
                        argcptr = va_arg(arg, char *);
                        if((s_flag & S_FORMAT_STAR) || (argcptr == NULL)) {
                            tmps++;
                        } else {
                            *argcptr = *tmps++;
                            result++;
                        }
                    }
                    loop = 0;
                    break;
                case 'n':

                    loop = 0;
                    break;
                case '*':
                    s_flag |= S_FORMAT_STAR;
                    break;
                case 'l':
                    if(s_flag & S_FORMAT_LONG) s_flag |= S_FORMAT_LONGLONG;
                    s_flag |= S_FORMAT_LONG;
                    break;
                case 'L':
                    s_flag |= S_FORMAT_LONGLONG;
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    width = width*10 + (int)(*format - '0');
                    break;
                case '\0':
                    format--;
                    loop = 0;
                    break;
                }
                format++;
            } /* end of while(loop) */
        } else { /* !(*format == '%') */
            if(*tmps++ != *format++) break;
        }
    } /* end of while(*format != '\0') */

    return result;
}

