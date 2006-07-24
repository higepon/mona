/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software withoution, including without limitation the rights
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
#include <stdlib.h>
#include <errno.h>

#include <monapi/messages.h>

int setvbuf(FILE * stream, char *buf, int mode, size_t size)
{
	fflush(stream);

	if( buf == NULL && mode != __SNBF )
	{
		buf = malloc(size);
		if( buf == NULL )
		{
			errno = ENOMEM;
			return -1;
		}
		syscall_print("buf alloced by setvbuf.\n");
	}

	stream->_bf._base = buf;
	stream->_bf._size = size;

	stream->_flags &= ~(__SLBF|__SNBF|__SFBF);
	stream->_flags |= __SALD;
	switch(mode)
	{
		case _IOFBF: stream->_flags |= __SFBF; break;
		case _IONBF: stream->_flags |= __SNBF; break;
		case _IOLBF: stream->_flags |= __SLBF; break;
		default: break;
	}

	return 0;
}
