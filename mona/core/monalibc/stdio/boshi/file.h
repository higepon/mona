#ifndef _File_H_
#define _File_H_

#include <stdio.h>

enum
{
	F_READ    = 0x0001,
	F_WRITE   = 0x0002,
	F_APPEND  = 0x0004,
	F_BINARY  = 0x0008,
	F_LINEBUF = 0x0010,
	F_FULLBUF = 0x0020,
	F_NONEBUF = 0x0040,
	F_BUFSELF = 0x0080,

	F_EOF     = 0x1000,
	F_BUFEOF  = 0x2000,
	F_BUFALCD = 0x4000,
	F_BUFDIRTY= 0x8000,
	F_CREATE  = 0x10000,
	F_TRUNC   = 0x20000,
};

struct cache_
{
	void *base;
	void *current;
	void *end;
	size_t offset;
	size_t size;
};

typedef struct cache_operators_
{
	int (*init)(void *, void *, size_t);
	int (*is_out_of_cache)(void *);
	int (*is_eof)(void *);
	void (*flush)(void *);
	void (*load)(void *);
	void (*move_current_ptr)(void *);
} cacheOp;

typedef struct __mlibc_file_operators_
{
	int (*open)(void *, const char *, int);
	int (*close)(void *, int);
	int (*read)(void *, void *, size_t);
	int (*write)(void *, void *, size_t);
	int (*seek)(void *, fpos_t, int);
	int (*is_valid_file)(void *, int);
} fileOp;

typedef struct file_
{
	int file; /* descriptor */
	int offset; /* current offset */
	int mode; /* open mode */
	int end;
	struct cache_ cache;
	cacheOp cachers;
	fileOp fileops;
	int ungetcbuf;
} File;

File* __mlibc_fopen(const char *path, const char *mode);
void __mlibc_fclose(File *f);
size_t __mlibc_fread(void *buf, size_t size, size_t nmemb, File *f);
size_t __mlibc_fwrite(void *buf, size_t size, size_t nmemb, File *f);
int __mlibc_fseek(File *f, long offset, int whence);
int __mlibc_fflush(File *f);

#endif

