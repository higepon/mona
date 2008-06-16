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

