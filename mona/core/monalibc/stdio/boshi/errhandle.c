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
