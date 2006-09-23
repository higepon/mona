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

#include "stdio_p.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <monapi/messages.h>

FILE *fopen(const char *path, const char *mode)
{
	FILE *fp;
	dword fileno;

	fp = malloc(sizeof(FILE));
	if( fp == NULL )
	{
		errno = ENOMEM;
		return fp;
	}
	memset(fp, 0, sizeof(FILE));

	if( !strcmp(mode, "r") || !strcmp(mode, "rb") )
	{
		fp->_flags = __SRD;
	}
	else if( !strcmp(mode, "r+") || !strcmp(mode, "rb+") )
	{
		fp->_flags = __SRD|__SWR;
	}
	else if( !strcmp(mode, "w") || !strcmp(mode, "wb") )
	{
		fp->_flags = __SWR;
	}
	else if( !strcmp(mode, "w+") || !strcmp(mode, "rb+") )
	{
		fp->_flags = __SWR|__SRD;
	}
	else if( !strcmp(mode, "a") || !strcmp(mode, "ab") )
	{
		fp->_flags = __SAP;
	}
	else if( !strcmp(mode, "a+") || !strcmp(mode, "ab+") )
	{
		fp->_flags = __SAP|__SRD;
	}

	fileno = monapi_file_open(path, MONAPI_FALSE);
	if( fileno == MONA_FAILURE )
	{
		free(fp);
		errno = EUNKNOWN;
		return NULL;
	}
	fp->_file = fileno;

	fp->_read = _read;
	fp->_write = _write;
	fp->_seek = _seek;
	fp->_extra = malloc(sizeof(struct __sFILEX));
	if( fp->_extra == NULL )
	{
		free(fp);
		errno = ENOMEM;
		return NULL;
	}
	memset(fp->_extra, 0, sizeof(struct __sFILEX));
	fp->_bf._base = malloc(BUFSIZ);
	if( fp->_bf._base == NULL )
	{
		free(fp->_extra);
		free(fp);
		errno = ENOMEM;
		return NULL;
	}
	fp->_bf._size = BUFSIZ;
	fp->_flags |= __SALD|_IOFBF;

	return fp;
}
