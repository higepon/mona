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

int fseek(FILE *stream, long offset, int whence)
{
	register int result;
	switch(whence)
	{
		case SEEK_SET:
		{
			if( offset < 0 )
			{
				errno = EINVAL;
				return 1;
			}
			stream->_extra->offset = offset;
			break;
		}
		case SEEK_CUR:
		{
			stream->_extra->offset += offset;
			break;
		}
		case SEEK_END:
		{
			stream->_extra->offset = stream->_extra->filesize-offset;
			break;
		}
		default:
		{
			errno = EINVAL;
			return 1;
		}
	}
	result = stream->_seek(stream->_file, offset, whence);
	if( result != 0 )
	{
		errno = EBADF;
		return 1;
	}
	return 0;
}
