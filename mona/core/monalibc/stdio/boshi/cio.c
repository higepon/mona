/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "file.h"
#include "private_vars.h"

#include <string.h>

int __mlibc_fgetc(FILE *f)
{
	int readsize, c;
	readsize = fread(&c, 1, sizeof(char), f);
	if( readsize != 1 ) c = EOF;
	return c;
}

char *__mlibc_fgets(char *s, int n, FILE* f)
{
	int readsize, c, i = 0;
	char *p = s;
	do
	{
		readsize = fread(&c, 1, sizeof(char), f);
		if( readsize != 1 ){ *p == '\0'; break; }
		*p++ = (char)c;
		if( c == '\n' ) break;
	}
	while( i++ < n );
	return s;
}

int __mlibc_fputc(int c, FILE *f)
{
	int writtensize;
	writtensize = fwrite(&c, 1, sizeof(char), f);
	if( writtensize != 1 ) c = EOF;
	return c;
}

int __mlibc_fputs(const char *s, FILE *f)
{
	int len, writtensize;
	len = strlen(s);
	writtensize = fwrite(s, len, sizeof(char), f);
	if( writtensize != len ) return EOF;
	return 1;
}

