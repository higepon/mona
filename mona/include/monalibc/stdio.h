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
#include <monalibc/stdarg.h>

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
	int	_flags;	/* flags */
	uint32_t	_file;	/* fileno, if MonaOS ID, else -1 */
	struct	__sbuf _bf;
	int	_lbfpos;

	int (*_read)(void *self, void *buf, size_t size);
	int (*_seek)(void *self, fpos_t pos, int whence);
	int (*_write)(void *self, void *buf, size_t size);

	struct __sFILEX *_extra;
	int _ungetcbuf;
	void* _stream;
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
#define FILENAME_MAX	127

#define L_tmpnam	FILENAME_MAX

/*
#define stdin	0xFFFFFFFF
#define stdout	0xFFFFFFFE
#define stderr	0xFFFFFFFD
*/

#define stdin	(&__sF[0])
#define stdout	(&__sF[1])
#define stderr	(&__sF[2])

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BUILD_MONALIBC
extern __declspec(dllimport) FILE __sF[];
#else
extern __declspec(dllexport) FILE __sF[];
#endif
/*
extern FILE* __stdin;
extern FILE* __stdout;
extern FILE* __stderr;
*/

/*
 * C99's functions totals 46.
 * This library is implementing 30 functions.
 * 65.2% functions are implemented.
 */

/* ISO/IEC 9899:1999 7.19.4 Operatoins on files */
/*int remove(const char *filename); //stub */
/*int rename(const char *old, const char *new); //stub */
/*FILE *tmpfile(void); //stub */
/*char *tmpnam(char *s); //stub */

/* ISO/IEC 9899:1999 7.19.5 File access functions */
int	 fclose(FILE *);
int	 fflush(FILE *);
FILE	*fopen(const char *, const char *);
FILE	*freopen(const char *, const char *, FILE *);
void	 setbuf(FILE *, char *);
int	 setvbuf(FILE *, char *, int, size_t);

/* ISO/IEC 9899:1999 7.19.6 Formatted input/output functions*/
/*int	 fprintf(FILE *, const char *, ...); //stub */
int fscanf(FILE *fp, const char *format, ...);
int	 printf(const char *, ...); //stub
/*int	 scanf(const char *, ...); //stub */
/*int	 snprintf(char *, size_t, char *, ...); //stub */
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list ap);
/*int vfscanf(FILE *, const char *, va_list); //stub */
/*int vprintf(const char *, va_list); //stub */
/*int vscanf(const char *, va_list); //stub */
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int vsscanf(const char *str, const char *format, va_list ap);


/* ISO/IEC 9899:1999 7.19.7 Character input/output functions*/
int	 fgetc(FILE *fp);
char	*fgets(char *buf, int n, FILE *fp);
int	 fputc(int c, FILE *stream);
int	 fputs(const char * s, FILE * stream);
int	 getc(FILE *stream);
int	 getchar(void);
/*char	*gets(char *s); //stub */
int	 putc(int c, FILE *stream);
int	 putchar(int c);
int	 puts(const char *s);
int	 ungetc(int c, FILE *stream);

/* ISO/IEC 9899:1999 7.19.8 Direct input/output functions*/
size_t	 fread(void *, size_t, size_t, FILE *);
size_t	 fwrite(const void *, size_t, size_t, FILE *);

/* ISO/IEC 9899:1999 7.19.9 File positioning functions*/
int	 fgetpos(FILE * stream, fpos_t * pos);
int	 fseek(FILE *, long, int);
int	 fsetpos(FILE *stream, const fpos_t *pos);
long int ftell(FILE *stream);
void	 rewind(FILE *);

/* ISO/IEC 9899:1999 7.19.10 Error-handling functions*/
void	 clearerr(FILE *);
int	 feof(FILE *);
int	 ferror(FILE *);
void	 perror(const char *);


/* Not ANSI */
int	 fileno(FILE *);


/* #define perror(err) printf(err) */
#define fprintf(n, ...) printf(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
