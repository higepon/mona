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
#include <monalibc/string.h>
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
