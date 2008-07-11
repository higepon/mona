/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "file.h"

#include <string.h>
#include <errno.h>

void __mlibc_clearerr(FILE *f)
{
	if( f == NULL ) return;
	f->mode &= ~(F_EOF|F_ERROR);
}

int __mlibc_feof(FILE *f)
{
	if( f == NULL ) return 0;
	if( f->mode & F_EOF ) return 1;
	return 0;
}

int __mlibc_ferror(FILE *f)
{
	if( f == NULL ) return 0;
	if( f->mode & F_ERROR ) return 1;
	return 0;
}

void __mlibc_perror(const char *s)
{
	int no;

	no = errno;

	printf("%s: %s\n", s, strerror(no));
}
