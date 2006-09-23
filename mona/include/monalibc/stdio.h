/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is	 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************/

/* Please send bug reports to
	Shotaro Tsuji
	4-1010,
	Sakasedai 1-chome,
	Takaraduka-si,
	Hyogo-ken,
	665-0024
	Japan
	negi4d41@yahoo.co.jp
*/

#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/types.h>
#include <monalibc/stdint.h>
#include <monapi/syscall.h>

#ifndef NULL
#define NULL 0
#endif

typedef int64_t fpos_t;

struct __sbuf
{
	unsigned char *_base;
	int _size;
	int _offset;
	int _range;
};

struct __sFILEX;

typedef struct __sFILE
{
	short	_flags;	/* flags */
	dword	_file;	/* fileno, if MonaOS ID, else -1 */
	struct	__sbuf _bf;
	int	_lbfpos;

	int (*_read)(int id, void *buf, size_t size);
	int (*_seek)(int id, fpos_t pos, int whence);
	int (*_write)(int id, void *buf, size_t size);

	struct __sFILEX *_extra;
} FILE;

enum
{
	__SLBF	= 0x0001,	/* line buffered */
	__SNBF	= 0x0002,	/* unbuffered */
	__SFBF	= 0x0004,	/* block buffered */
	__SEOF	= 0x0008,	/* eof */
	__SERR	= 0x0010,	/* error */
	__SRD	= 0x0020,	/* readable */
	__SWR	= 0x0040,	/* writable */
	__SAP	= 0x0080,	/* append */
	__SALD	= 0x0100,	/* buffer alloced */
};

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#define _IOFBF	0
#define _IOLBF	1
#define _IONBF	2

#define BUFSIZ	1024
#define EOF	(-1)

#define FOPEN_MAX	20
#define FILENAME_MAX	256

#define L_tmpnam	FILENAME_MAX

#define stdin	(&__sF[0])
#define stdout	(&__sF[1])
#define stderr	(&__sF[2])

#ifdef __cplusplus
extern "C"
{
#endif

extern FILE __sF[];

void	 clearerr(FILE *);
int	 fclose(FILE *);
int	 feof(FILE *);
int	 ferror(FILE *);
int	 fflush(FILE *);
FILE	*fopen(const char *, const char *);
int	 fseek(FILE *, long, int);
size_t	 fwrite(const void *, size_t, size_t, FILE *);
size_t	 fread(void *, size_t, size_t, FILE *);
void	 rewind(FILE *);
void	 perror(const char *);
void	 setbuf(FILE *, char *);

int	 fileno(FILE *);

int fgetc(FILE *fp);
char *fgets(char *buf, int n, FILE *fp);

int fscanf(FILE *fp, const char *format, ...);

int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

/* #define perror(err) printf(err) */
#define fprintf(n, ...) printf(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
