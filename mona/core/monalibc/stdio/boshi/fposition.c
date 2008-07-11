/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "file.h"
#include "cache.h"
#include <errno.h>

int __mlibc_fseek(FILE *f, long offset, int whence)
{
	switch( whence )
	{
		case SEEK_SET: f->offset = offset; break;
		case SEEK_CUR: f->offset += offset; break;
		case SEEK_END: f->offset = f->end + offset;
			if( f->offset < 0 ) f->offset = 0;
			if( f->offset < f->cache.offset
				+ (f->cache.current-f->cache.base) )
					f->offset = f->cache.offset
					+ (f->cache.current-f->cache.base);
			break;
		defalut: errno = EINVAL; return -1;
	}

	f->mode &= ~F_EOF;
	return 0;
}

int __mlibc_fgetpos(FILE *f, fpos_t *pos)
{
	if( f == NULL || pos == NULL )
	{
		errno = EINVAL;
		return 11;
	}

	pos->offset = f->offset;

	return 0;
}

int __mlibc_fsetpos(FILE *f, fpos_t *pos)
{
	if( f == NULL || pos == NULL )
	{
		errno = EINVAL;
		return 11;
	}

	f->offset = pos->offset;

	return 0;
}

long int __mlibc_ftell(FILE *f)
{
	if( f == NULL ){ errno = EINVAL; return -1L; }

	return (long)f->offset;
}

void __mlibc_rewind(FILE *f)
{
	(void)fseek(f, 0L, SEEK_SET);
}

