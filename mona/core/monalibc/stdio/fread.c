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
#include <monapi/messages.h>

size_t __nida_read_console(void *ptr, size_t size, FILE *stream)
{
//    syscall_print("hoge");
	size = (size_t)readStream(stream->_stream, ptr, (uint32_t)size);
	return size;
}

size_t __nida_nonebuf_fread(void *buf, size_t size, FILE *stream)
{
	size_t readsize = 0;

	readsize = stream->_read(stream, buf, size);
    _printf("done1");
	if( readsize == -1 )
	{
		stream->_flags |= __SERR;
		return 0;
	}
    _printf("done2");
	if( readsize < size )
	{
		stream->_flags |= __SEOF;
	}

	stream->_extra->offset += readsize;
//	fseek(stream, stream->_extra->offset, SEEK_SET);
    _printf("done3");
	return readsize;
}

size_t __nida_fullybuf_fread(void *buf, size_t size, FILE *stream)
{
	size_t readsize = 0;
	size_t retsize = 0;
//    _printf("done4");
//	_printf("offset = %d\n", stream->_extra->offset);
	if( stream->_bf._range == 0 )
	{
 //     _printf("[5]");
		readsize = stream->_read(stream, stream->_bf._base,
							stream->_bf._size);
  //    _printf("[6]");
		if( readsize == -1 )
		{
			stream->_flags |= __SERR;
			return (size_t)-1;
		}
   //   _printf("[7]");
		if( readsize < size )
		{
			stream->_flags |= __SEOF;
		}
//      _printf("[8]");
		memcpy(buf, stream->_bf._base, size);
		stream->_bf._offset = stream->_extra->offset;
		stream->_bf._range = readsize;
//      _printf("[9]");
		if( size > stream->_bf._size )
		{
			retsize = stream->_read(stream, buf+readsize, size-readsize);
			readsize += retsize;
		}
		else
		{
			readsize = size;
		}
 //     _printf("[10]");
	}
	else
	{
	#if 0
		_printf("stream->_bf._offset = %d\n", stream->_bf._offset);
		_printf("stream->_extra->offset = %d\n", stream->_extra->offset);
		_printf("stream->_bf._size = %d\n", stream->_bf._size);
		_printf("stream->_bf._range = %d\n", stream->_bf._range);
		_printf("size = %d\n", size);
	#endif	
//      _printf("[7]");
//		_printf("offset+range = %d, offset+size = %d", stream->_bf._offset+stream->_bf._range, stream->_extra->offset+size);
		if( stream->_bf._offset == stream->_extra->offset )
		{
			if( size <= stream->_bf._range )
			{
//			_printf("else1-if1\n");
	//		_printf("?%d\n", stream->_extra->offset);
				memcpy(buf, stream->_bf._base,
						stream->_bf._size);
				readsize = size;
			}
		}
		else if( stream->_bf._offset < stream->_extra->offset &&
	stream->_bf._offset+stream->_bf._range > stream->_extra->offset+size )
		{
	//	_printf("!%d\n", stream->_extra->offset-stream->_bf._offset);
//	_printf("else1-elseif1\n");
			memcpy(buf,
		stream->_bf._base+(stream->_extra->offset-stream->_bf._offset),
				size);
			readsize = size;
		}
		else
		{
//	_printf("else1-else2\n");
	//		_printf("/%d\n", stream->_extra->offset);
			stream->_seek(stream, stream->_extra->offset, SEEK_SET);
//      _printf("[8]");
			readsize = stream->_read(stream,
							stream->_bf._base,
							stream->_bf._size);
 //     _printf("[9]");
//		_printf("!!readsize = %x\n", readsize);
			if( readsize == -1 )
			{
				stream->_flags |= __SERR;
				return (size_t)-1;
			}
			if( readsize < size )
			{
				stream->_flags |= __SEOF;
				memcpy(buf, stream->_bf._base, readsize);
			}
			else
		//	if( readsize != 0 )
			{
				memcpy(buf, stream->_bf._base, size);
				stream->_bf._offset = stream->_extra->offset;
				stream->_bf._range = readsize;
				if( size > stream->_bf._size )
				{
					retsize = stream->_read(stream, buf+readsize, size-readsize);
	//				_printf("@%d\n", retsize);
					readsize += retsize;
			//		_printf("else1-else2-if-if\n");
				}
				else
				{
//				_printf("readsize = size\n");
					readsize = size;
			//		_printf("else1-else2-if-else\n");
				}
			}
		}
	}

//	_printf("offset = %d, readsize = %d\n", stream->_extra->offset ,readsize);
	stream->_extra->offset += readsize;

//	stream->_seek(stream->_file, stream->_extra->offset, SEEK_SET);
  //    _printf("[11]");
	return readsize;
}

size_t fread(void *buf, size_t size, size_t nmemb, FILE *stream)
{
/*
  if (stream->_extra == NULL)
    {
      syscall_print("panic");
    }
  else {
    //    syscall_print("not panic");
  }
*/
//_printf("filesize = %d, offset = %d\n", monapi_file_get_file_size(stream->_file), stream->_extra->offset);
//_printf("fread\n");
//_printf("buf = %x, size = %d, nmemb = %d\n", buf, size, nmemb);
//_printf("_read = %x\n", stream->_read);
/*
	if( monapi_file_get_file_size(stream->_file) <= stream->_extra->offset )
	{
		return 0;
	}
	*/
	if( !(stream->_flags & __SRD) )
	{
		errno = EBADF;
		return -1;
	}
	if( stream->_extra->stds == __STDIN )
	{
		return __nida_read_console(buf, size*nmemb, stream);
	}
	/*
	if( stream->_extra->offset == monapi_file_get_file_size(stream->_file) )
	{
		return 0;
	}
	*/
	else if( stream->_ungetcbuf != EOF )
	{
		{
			unsigned char *p = buf;
			p[0] = (unsigned char)stream->_ungetcbuf;
			stream->_ungetcbuf = EOF;
			return (size_t)1;
		}
	}
	else if( stream->_flags & __SNBF )
	{
		return __nida_nonebuf_fread(buf, size*nmemb, stream);
	}
	else if( stream->_flags & __SFBF )
	{
//	_printf("end of fread\n");
		return __nida_fullybuf_fread(buf, size*nmemb, stream);
	}
	else
	{
		return __nida_nonebuf_fread(buf, size*nmemb, stream);
	}
}
