/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _FILE_H_
#define _FILE_H_


#include <sys/types.h>
#include <monalibc/stdint.h>
#include <monalibc/stdarg.h>

#ifndef NULL
  #define NULL 0
#endif
#ifndef SEEK_SET
  #define SEEK_SET 0
#endif
#ifndef SEEK_CUR
  #define SEEK_CUR 1
#endif
#ifndef SEEK_END
  #define SEEK_END 2
#endif
#define _IOFBF       0
#define _IOLBF       1
#define _IONBF       2
#define BUFSIZ       1024
#define EOF          (-1)
#define FOPEN_MAX    20
#define FILENAME_MAX 127
#define L_tmpnam     FILENAME_MAX

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _fpos_t
{
	int offset;
	int state;
} fpos_t;

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

	F_EOF     = 0x001000,
	F_BUFEOF  = 0x002000,
	F_BUFALCD = 0x004000,
	F_BUFDIRTY= 0x008000,
	F_CREATE  = 0x010000,
	F_TRUNC   = 0x020000,
	F_ERROR   = 0x100000,
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
	int (*flush)(void *);
	int (*load)(void *);
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
} FILE;

FILE* __mlibc_fopen(const char *path, const char *mode);
void __mlibc_fclose(FILE *f);
size_t __mlibc_fread(void *buf, size_t size, size_t nmemb, FILE *f);
size_t __mlibc_fwrite(void *buf, size_t size, size_t nmemb, FILE *f);
int __mlibc_fflush(FILE *f);

int __mlibc_fgetpos(FILE *f, fpos_t *pos);
int __mlibc_fseek(FILE *f, long offset, int whence);
int __mlibc_fsetpos(FILE *f, fpos_t *pos);
long int __mlibc_ftell(FILE *f);
void __mlibc_rewind(FILE *f);

void __mlibc_clearerr(FILE *f);
int __mlibc_feof(FILE *f);
int __mlibc_ferror(FILE *f);
void __mlibc_perror(const char *s);

int __mlibc_fgetc(FILE *f);
char *__mlibc_fgets(char *s, int n, FILE *f);
int __mlibc_fputc(int c, FILE *f);
int __mlibc_fputs(const char *s, FILE *f);

#define getc(f) __mlibc_fgetc(f)
#define getchar() __mlibc_fgetc(STDIN)
#define putc(c, f) __mlibc_fputc(c, f)
#define putchar(c) __mlibc_fputc(c, STDOUT)
#define puts(s) __mlibc_fputs(s)

#ifdef __cplusplus
}
#endif

#endif

