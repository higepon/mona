#include "file.h"
#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "private_vars.h"
#include "filelist.h"

int __mlibc_stdio_parse_mode(const char *s)
{
	int r = 0;
	if( s == NULL ) return 0;
	switch(*s++)
	{	case 'r': r |= F_READ; break;
		case 'w': r |= F_WRITE|F_CREATE|F_TRUNC; break;
		case 'a': r |= F_APPEND|F_WRITE|F_CREATE; break;
	default: return 0; } do { switch(*s)
	{	case '+': r |= F_READ|F_WRITE; break;
		case 'b': r |= F_BINARY; break;
		default: break; }} while( *s++ != '\0' );
	return r;
}

extern struct __mlibc_file_operators_ __file_ops;
void* opening_files = NULL;

FILE* __mlibc_fopen(const char *path, const char *mode)
{
	FILE* file = NULL;

	file = (FILE*)malloc(sizeof(FILE));
	if( file == NULL )
	{
		errno = ENOMEM;
		return NULL;
	}
	memset(file, 0, sizeof(FILE));

	file->mode = __mlibc_stdio_parse_mode(mode);
	if( file->mode == 0 )
	{
		errno = EINVAL;
		free(file);
		return NULL;
	}

	file->fileops = __file_ops;

	file->file = file->fileops.open(file, path, file->mode);
	if( !file->fileops.is_valid_file(file, file->file) )
	{
		errno = EUNKNOWN;
		free(file);
		return NULL;
	}

	file->end = file->fileops.seek(file, 0, SEEK_END);
	file->fileops.seek(file, 0 , SEEK_SET);

	file->offset = 0;
	file->cachers = fullbuf_operators;
	file->ungetcbuf = EOF;

	__mlibc_filelist_add(opening_files, file);

	return file;
}

int __mlibc_fflush(FILE *f)
{
	f->cachers.flush(f);
	return 0;
}

void __mlibc_fclose(FILE *f)
{
	__mlibc_fflush(f);

	f->fileops.close(f, f->file);
	if( f->mode & F_BUFSELF) free(f->cache.base);
	__mlibc_filelist_remove_by_element(opening_files, f);
	free(f);
	return;
}

