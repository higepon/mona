/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is	 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************/

/* Please send bug reports to
	Shotaro Tsuji
	4-1010,
	Sakasedai 1-chome,
	Takaraduka-si,
	Hyogo-ken,
	665-0024
	Japan
	negi4d41@yahoo.co.jp
*/

#include <monapi/messages.h>
#include <stdlib.h>
#include <string.h>
#include "stdio_p.h"

FILE __sF[3];

int _read(int id, void *buf, size_t size)
{
	monapi_cmemoryinfo *cmi = NULL;
	unsigned char *p = buf;
	int readsize;
	int i;

	cmi = monapi_file_read((dword)id, (dword)size);
	if( cmi == NULL )
	{
		return -1;
	}

	readsize = (int)cmi->Size;

	for( i = 0 ; i < readsize ; i++ )
	{
		p[i] = cmi->Data[i];
	}

	monapi_cmemoryinfo_dispose(cmi);
	monapi_cmemoryinfo_delete(cmi);

	return readsize;
}

int _write(int id, void *buf, size_t size)
{
	dword result;
	monapi_cmemoryinfo* cmi;

	cmi = monapi_cmemoryinfo_new();
	cmi->Data = buf;
	cmi->Size = (dword)size;

	result = monapi_file_write((dword)id, cmi, (dword)size);

	monapi_cmemoryinfo_delete(cmi);

	return (int)result;
}

int _seek(int id, fpos_t pos, int whence)
{
	MONAPI_BOOL result;

	result = monapi_file_seek((dword)id, (dword)pos, (dword)whence);
	if( result == MONAPI_FALSE )
	{
		return -1;
	}

	return 0;
}

FILE *_open_stdin(void)
{
	FILE *fp;

	fp = malloc(sizeof(FILE));
	if( fp == NULL ) return NULL;
	memset(fp, 0, sizeof(FILE));

	fp->_flags = __SRD|__SLBF;
	fp->_extra = malloc(sizeof(struct __sFILEX));
	fp->_extra->stds = __STDIN;

	return fp;
}

FILE *_open_stdout(void)
{
	FILE *fp;

	fp = malloc(sizeof(FILE));
	if( fp == NULL ) return NULL;
	memset(fp, 0, sizeof(FILE));

	fp->_flags = __SWR|__SLBF;
	fp->_extra = malloc(sizeof(struct __sFILEX));
	fp->_extra->stds = __STDOUT;

	return fp;
}

FILE *_open_stderr(void)
{
	FILE *fp;

	fp = malloc(sizeof(FILE));
	if( fp == NULL ) return NULL;
	memset(fp, 0, sizeof(FILE));

	fp->_flags = __SWR|__SNBF;
	fp->_extra = malloc(sizeof(struct __sFILEX));
	fp->_extra->stds = __STDERR;

	return fp;
}

void init_stdio(void)
{
	__sF[0] = *(_open_stdin());
	__sF[1] = *(_open_stdout());
	__sF[2] = *(_open_stderr());
}

void fini_stdio(void)
{
	fclose(&__sF[2]);
	fclose(&__sF[1]);
	fclose(&__sF[0]);
}
