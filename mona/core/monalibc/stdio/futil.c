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

//asm(".section .drectve");
//asm(".ascii \"-export:__sF\"");

FILE __sF[3];

int _read(int id, void *buf, size_t size)
{
	monapi_cmemoryinfo *cmi = NULL;
	unsigned char *p = buf;
	int readsize;
	int i;

	cmi = monapi_file_read((uint32_t)id, (uint32_t)size);
	if( cmi == NULL )
	{
		return -1;
	}

	readsize = (int)cmi->Size;

	memcpy(p, cmi->Data, readsize);

	monapi_cmemoryinfo_dispose(cmi);
	monapi_cmemoryinfo_delete(cmi);

	return readsize;
}

int _write(int id, void *buf, size_t size)
{
	uint32_t result;
	monapi_cmemoryinfo* cmi;

	cmi = monapi_cmemoryinfo_new();
	if( !monapi_cmemoryinfo_create(cmi, size, 0) )
	{
		monapi_cmemoryinfo_delete(cmi);
		return -1;
	}
	memcpy(cmi->Data, buf, cmi->Size);

	result = monapi_file_write((uint32_t)id, cmi, cmi->Size);

	monapi_cmemoryinfo_dispose(cmi);
	monapi_cmemoryinfo_delete(cmi);

	return (int)result;
}

int _seek(int id, fpos_t pos, int whence)
{
	MONAPI_BOOL result;

	result = monapi_file_seek((uint32_t)id, (uint32_t)pos, (uint32_t)whence);
	if( result == MONAPI_FALSE )
	{
		return -1;
	}

	return 0;
}

void _open_stdin(FILE *fp)
{
	memset(fp, 0, sizeof(FILE));

	fp->_flags = 0;
	fp->_flags = __SRD|__SLBF;
	fp->_extra = malloc(sizeof(struct __sFILEX));
	fp->_extra->stds = __STDIN;
}

void _open_stdout(FILE *fp)
{
	memset(fp, 0, sizeof(FILE));

	fp->_flags = 0;
	fp->_flags = __SWR|__SLBF;
	_logprintf("monalibc: fp->_flags = %x\n", fp->_flags);
	fp->_extra = malloc(sizeof(struct __sFILEX));
	fp->_extra->stds = __STDOUT;
}

void _open_stderr(FILE *fp)
{
	memset(fp, 0, sizeof(FILE));

	fp->_flags = 0;
	fp->_flags = __SWR|__SNBF;
	fp->_extra = malloc(sizeof(struct __sFILEX));
	fp->_extra->stds = __STDERR;
}

void init_stdio(void)
{
	_open_stdin(&(__sF[0]));
	_open_stdout(&(__sF[1]));
	_open_stderr(&(__sF[2]));
	stream_opener();
	_logprintf("monalibc: stdin = %x\n", stdin);
	_logprintf("monalibc: stdout= %x\n", stdout);
	_logprintf("monalibc: stderr= %x\n", stderr);
	_logprintf("monalibc: __sF[0]._extra = %x\n", __sF[0]._extra);
	_logprintf("monalibc: __sF[1]._extra = %x\n", __sF[1]._extra);
	_logprintf("monalibc: __sF[2]._extra = %x\n", __sF[2]._extra);
	logprintf("monalibc: stdin = %x\n", stdin);
	logprintf("monalibc: stdout= %x\n", stdout);
	logprintf("monalibc: stderr= %x\n", stderr);
}

void fini_stdio(void)
{
	fclose(&__sF[2]);
	fclose(&__sF[1]);
	fclose(&__sF[0]);
}
