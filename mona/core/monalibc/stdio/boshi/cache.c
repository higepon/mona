#include "file.h"
#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "private_vars.h"
#include "filelist.h"

void __mlibc_init_cache(FILE *file, char *buf, size_t size, int mode)
{
	file->cache.base = buf;
	if( file->cache.base == NULL )
	{
		__mlibc_init_cache(file, malloc(size), size, mode|F_BUFSELF);
		return;
	}
	file->cache.current = file->cache.base;
	file->cache.end = file->cache.base + (size - 1);
	file->cache.size = size;
	file->cache.offset = 0;

	file->mode |= mode|F_BUFALCD;

	return;
}

int __mlibc_is_out_of_cache(FILE *f)
{
	int x = 0, p1 = 0, p2 = 0;
	x = f->offset;
	p1 = f->cache.offset;
//	p2 = f->cache.offset + f->cache.size;
	p2 = f->cache.offset + (f->cache.end-f->cache.base);
	if( x < p1 || p2 <= x ) return 1;
	return 0;
}

int __mlibc_cache_flush(FILE *f)
{
	int result = 0;
	if( f->mode & F_BUFDIRTY )
	{
		f->fileops.seek(f, f->cache.offset, SEEK_SET);
		/* NEED TO CHECK THE ERROR */
		result = f->fileops.write(f, f->cache.base, f->cache.current-f->cache.base);
	}
	f->cache.current = f->cache.base;
	return result;
}

int __mlibc_is_eof(FILE *f)
{
//	puts(__func__);
//	_logprintf("current = %x, end = %x\n", f->cache.current, f->cache.end);
	if( !(f->mode & F_BUFEOF) ) return 0;
	if( f->cache.current >= f->cache.end ) return 1;
	return 0;
}

void __mlibc_move_cache_current_ptr(FILE* f)
{
	if( __mlibc_is_out_of_cache(f) ) return;
	f->cache.current = f->cache.base + (f->offset-f->cache.offset);
}

/** fully buffer */

void __mlibc_init_cache_fully(FILE *f, void *p, size_t size)
{
	__mlibc_init_cache(f, p, size, F_FULLBUF);
}

int __mlibc_cache_load_fully(FILE *f)
{
//	_logprintf("%s\n", __func__);
	size_t readsize = 0;

	if( !(f->mode & F_READ) )
	{
		f->cache.current = f->cache.base;
		f->cache.end = f->cache.end;
		f->cache.offset = f->offset;
		return 0;
	}

//	_logprintf("\tseek : offset = %x\n", f->offset);
	int result = f->fileops.seek(f, f->offset, SEEK_SET);
//	_logprintf("\t     : result = %x\n", result);
//	_logprintf("\tread : size = %d\n", f->cache.size);
	readsize = f->fileops.read(f, f->cache.base, f->cache.size);
	if( readsize == -1 )
	{
		return -1;
	}
//	_logprintf("\t     : readsize = %d\n", readsize);
	f->cache.current = f->cache.base;
	f->cache.end = f->cache.base + readsize;
	f->mode &= ~F_BUFEOF;
	f->cache.offset = f->offset;
//	_logprintf("\tcur = %x, end = %x, off = %x\n", f->cache.current, f->cache.end, f->cache.offset);
	if( readsize < f->cache.size )
	{
		f->mode |= F_BUFEOF;
	}
	return readsize;
}

struct cache_operators_ fullbuf_operators =
{
	(int(*)(void*,void*,size_t))__mlibc_init_cache_fully,
	(int(*)(void*))__mlibc_is_out_of_cache,
	(int(*)(void*))__mlibc_is_eof,
	(int(*)(void*))__mlibc_cache_flush,
	(int(*)(void*))__mlibc_cache_load_fully,
	(void(*)(void*))__mlibc_move_cache_current_ptr
};

/** line buffer */

void __mlibc_init_cache_line(FILE *f, void *p, size_t size)
{
	__mlibc_init_cache(f, p, size, F_LINEBUF);
}

int __mlibc_get_line_iter(FILE *f, char *buf, size_t size, size_t rsize)
{
	if( size <= 0 ) {
		return rsize;
	} else { switch(f->fileops.read(f, buf, 1)) {
		case 0: /* End of file */
			f->mode |= F_BUFEOF; return rsize;
		case -1: /* Error */
			return rsize;
	}}
	if( *buf == '\n') return rsize;
	return __mlibc_get_line_iter(f, buf+1, size-1, rsize+1);
}

int __mlibc_get_line(FILE *f)
{
//	puts(__func__);
	size_t readsize = 0;

	if( !(f->mode & F_READ) )
	{
		f->cache.current = f->cache.base;
		f->cache.end = f->cache.end;
		f->cache.offset = f->offset;
		return 0;
	}

//	printf("offset = %d\n", f->offset);
	int result = f->fileops.seek(f, f->offset, SEEK_SET);
	f->mode &= ~F_BUFEOF;
	readsize = __mlibc_get_line_iter(f, f->cache.base, f->cache.size, 0);
//	printf("readsize = %d\n", readsize);
	f->cache.current = f->cache.base;
	f->cache.end = f->cache.base + readsize;
	f->cache.offset = f->offset;
	return readsize;
}

struct cache_operators_ linebuf_operators =
{
	(int(*)(void*,void*,size_t))__mlibc_init_cache_line,
	(int(*)(void*))__mlibc_is_out_of_cache,
	(int(*)(void*))__mlibc_is_eof,
	(int(*)(void*))__mlibc_cache_flush,
	(int(*)(void*))__mlibc_get_line,
	(void(*)(void*))__mlibc_move_cache_current_ptr
};

