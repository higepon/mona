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

#include <monalibc/stdio.h>
#include <monalibc/stdarg.h>
#include <monalibc/stdlib.h>
#include <monalibc/string.h>
#include <monalibc/assert.h>
#include <monalibc.h>
#include <monapi/syscall.h>
#include <monapi/messages.h>

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

//extern monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, int prompt);

FILE *fopen(const char *path, const char *mode)
{
	FILE *ret = NULL;
	if (0 == strncmp(mode, "r", 1))
	{
		monapi_cmemoryinfo* mi = monapi_call_file_read_data(path, 0);
		if (mi == NULL) return NULL;
		
		ret = (FILE *)malloc(sizeof(FILE));
		ret->path  = path;
		ret->mode  = mode;
		ret->data  = mi;
		ret->pos   = 0;
		ret->error = 0;
	}
	return ret;
}

int fclose(FILE *fp)
{
	if (fp == NULL) return 0;
	
	monapi_cmemoryinfo_dispose(fp->data);
	monapi_cmemoryinfo_delete (fp->data);
	free(fp);
	return 0;
}

int fgetc(FILE *fp)
{
	if (fp == NULL || fp->pos >= fp->data->Size) return EOF;
	
	return fp->data->Data[fp->pos++];
}

char *fgets(char *buf, int n, FILE *fp)
{
	int i;
	if (fp == NULL || fp->pos >= fp->data->Size || n < 1)
	{
		if (n > 0) *buf = '\0';
		return buf;
	}
	
	for (i = 0; i < n - 1; i++)
	{
		int ch = fgetc(fp);
		if (ch == '\r')
		{
			continue;
		}
		else if (ch == EOF || ch == '\n')
		{
			buf[i] = '\0';
			return buf;
		}
		buf[i] = ch;
	}
	buf[n - 1] = '\0';
	return buf;
}

size_t fread(void *buf, size_t size, size_t count, FILE *fp)
{
	int ret = 0;
	if (fp == NULL || 0 != strncmp(fp->mode, "r", 1)) return 0;
	
	ret = (fp->data->Size - fp->pos) / size;
	if (ret > count) ret = count;
	if (ret > 0)
	{
		memcpy(buf, &fp->data->Data[fp->pos], ret * size);
		fp->pos += ret * size;
	}
	return ret;
}

int fseek(FILE *fp, long offset, int whence)
{
	if (fp == NULL) return 1;
	
	switch (whence)
	{
		case SEEK_SET:
			fp->pos = offset;
			break;
		case SEEK_CUR:
			fp->pos = fp->pos + offset;
			break;
		case SEEK_END:
			fp->pos = fp->data->Size + offset;
			break;
		default:
			return 1;
	}
	if (fp->pos < 0) fp->pos = 0;
	if (fp->pos > fp->data->Size) fp->pos = fp->data->Size;
	return 0;
}

int fscanf(FILE *fp, const char *format, ...)
{
	int result;
	va_list args;
	
	if (fp == NULL || 0 != strncmp(fp->mode, "r", 1)) return 0;
	
	va_start(args, format);
	result = vsscanf((const char *)&fp->data->Data[fp->pos], format, args);
	va_end(args);
	
	fp->pos += result;
	return result;
}

int ferror(FILE *fp)
{
	return fp == NULL ? 0 : fp->error;
}

int feof(FILE *fp)
{
	if (fp == NULL) return 0;
	return fp->pos >= fp->data->Size;
}

int fflush(FILE *fp)
{
	return 0;
}

int putchar(int ch)
{
	char s[] = { ch, '\0' };
	printf(s);
	return ch;
}

int putc(int ch, FILE *fp)
{
	return fp == NULL ? putchar(ch) : 0;
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
  \brief equivalent to the function sscanf

  \param s      buf scanned characters
  \param format specifies how subsequent arguments
  \param arg    a variable number of arguments
  \return  the number of variables scanned
*/
int vsscanf(const char *s, const char *format, va_list arg){
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

  while(*format != '\0'){
    if(*format == '%'){
      loop = 1;
      s_flag = 0;
      width = 0;

      format++;
      if(*format == '%'){ /* %% */
        if(*tmps != '%') break;
        format++;
        tmps++;
        continue;
      }
      while(loop){
        switch(*format){
          case 's':
            length =  strcspn(tmps, " ");
            if((width == 0) || (width > length)) width = length;
            argcptr = va_arg(arg, char *);
            if((s_flag & S_FORMAT_STAR) || (argcptr == NULL)){ /* no output */
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
            if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)){ /* no output */
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
            if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)){ /* no output */
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
            if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)){ /* no output */
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
            if((s_flag & S_FORMAT_STAR) || (argiptr == NULL)){ /* no output */
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
            while(width--){
              argcptr = va_arg(arg, char *);
              if((s_flag & S_FORMAT_STAR) || (argcptr == NULL)){
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

// ???
int vfprintf(FILE *stream, const char *format, va_list ap)
{
	char buf[5096];

	vsprintf(buf, format, ap);
	printf(buf);

	return strlen(buf);
}

int vsnprintf(char *str, size_t size, const char* format, va_list ap)
{
	int len;
	len = vsprintf(str, format, ap);
	assert(len <= size);

	return len;
}

