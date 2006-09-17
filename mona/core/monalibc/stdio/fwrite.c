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
#include <monapi/syscall.h>

size_t __nida_write_console(const void *ptr, size_t size, FILE *stream)
{
	char* p = malloc(size+1);
	if( p == NULL )
	{
		errno = ENOMEM;
		return 0;
	}
	memset(p, 0, size+1);
	memcpy(p, ptr, size);
	print(p);

	return size;
}

size_t __nida_fullybuf_fwrite(const void *ptr, size_t size, FILE *stream)
{
	size_t writesize;
	int offdiff;

	if( stream->_bf._offset <= stream->_extra->offset &&
	stream->_bf._offset+stream->_bf._range >= stream->_extra->offset+size);
	{
		offdiff = stream->_extra->offset-stream->_bf._offset;
		writesize = size;
		if( stream->_bf._size < size )
		{
			writesize = stream->_bf._size;
		}
		memcpy(stream->_bf._base+offdiff, ptr, writesize);
	}

	stream->_write(stream->_file, (void*)ptr, size);

	return writesize;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if( !(stream->_flags & __SWR) )
	{
		errno = EBADF;
		return 0;
	}
	if( stream->_flags & __SAP )
	{
		fseek(stream, 0, SEEK_END);
	}
	if( stream->_extra->stds == __STDOUT
				|| stream->_extra->stds == __STDERR )
	{
		return __nida_write_console(ptr, size*nmemb, stream);
	}
	if( stream->_flags & __SFBF )
	{
		return __nida_fullybuf_fwrite(ptr, size*nmemb, stream);
	}


	errno = EBADF;
	return 0;
}
