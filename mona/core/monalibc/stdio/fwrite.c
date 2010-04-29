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
	size = monapi_stdout_write(ptr, size);
//	size = (size_t)writeStream(stream->_stream, ptr, (uint32_t)size);
	return size;
}

size_t __nida_nonebuf_fwrite(const void *ptr, size_t size, FILE *stream)
{
	size_t writesize;

	writesize = stream->_write(stream, (void*)ptr, size);

	return writesize;
}

static int target_inside_buffer(int bufoff, int bufsize, int targetoff, int targetsize)
{
	return ( bufoff <= targetoff &&
		bufoff+bufsize >= targetoff+targetsize);
}

size_t __nida_fullybuf_fwrite(const void *ptr, size_t size, FILE *stream)
{
	size_t writesize;
	int offdiff;

	struct __sbuf *bf = &stream->_bf;
	struct __sFILEX *extra = stream->_extra;
	if(!target_inside_buffer(bf->_offset, bf->_size, extra->offset, size))
	{
		fflush(stream);
		if(!target_inside_buffer(bf->_offset, bf->_size, extra->offset, size))
		{
			return stream->_write(stream, (void*)ptr, size);
		}
		/* fall through */
	}
	offdiff = extra->offset-bf->_offset;
	memcpy(bf->_base+offdiff, ptr, size);
	extra->offset += size;
	if(bf->_range < offdiff+size)
		bf->_range = offdiff+size;
	return size;

}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
#if 0
	_logprintf("monalibc: fwrite\n");
	_logprintf("monalibc: stream = %x\n", stream);
	_logprintf("monalibc: stream->_flags = %x\n", stream->_flags);
	_logprintf("monalibc: stream->_extra = %x\n", stream->_extra);
	_logprintf("monalibc: stream->_extra->stds = %x\n", stream->_extra->stds);
#endif
	if( stream->_extra->stds == __STDOUT
				|| stream->_extra->stds == __STDERR )
	{
		return __nida_write_console(ptr, size*nmemb, stream);
	}
	if( !(stream->_flags & __SWR) )
	{
		errno = EBADF;
		return 0;
	}
	if( stream->_flags & __SAP )
	{
		fseek(stream, 0, SEEK_END);
	}
	if( stream->_flags & __SFBF )
	{
		return __nida_fullybuf_fwrite(ptr, size*nmemb, stream);
	}
	else if( stream->_flags & __SNBF )
	{
		return __nida_nonebuf_fwrite(ptr, size*nmemb, stream);
	}

	errno = EBADF;
	return 0;
}
