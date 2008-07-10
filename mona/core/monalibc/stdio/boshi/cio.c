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

