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

#ifndef _STDLIB_H_
#define _STDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <monalibc/stddef.h>

typedef struct
{
	int quot; /* 商 */
	int rem;  /* 余 */
} div_t;

typedef struct
{
	long int quot;
	long int rem;
} ldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define S_FORMAT_STAR		1
#define S_FORMAT_LONG		2
#define S_FORMAT_LONGLONG	4
#define S_FORMAT_UNSIGNED	8

/* これは関係ないニダ */
#define RAND_MAX 0x7fffffff

/* stdlib.hをインクルードするとmonapi.hもインクルードすることに（ry */
/* #include <monapi/syscall.h> */
void* malloc(unsigned long size);
void* calloc(unsigned long n, unsigned long s);
void* realloc(void* address, unsigned long size);
void free(void* address);
int exit(int);
int atexit(void (*function)(void));

void abort(void);
int abs(int);
double atof(const char*);
int atoi(const char*);
long int atol(const char*);
void *bsearch(const void *key, const void *base, size_t nmemb,
                        size_t size, int (*compar)(const void*, const void*));
div_t div(int, int);
ldiv_t ldiv(long int, long int);
char *getenv(const char*);
int putenv(char *string);
int mblen(const char*, size_t);
size_t mbstowcs(wchar_t*, const char*, size_t);
int mbtowc(wchar_t*, const char*, size_t);
long int labs(long int);
long long int llabs(long long int);
int rand(void);
void srand(unsigned int);
double strtod(const char*, char **endptr);
long int strtol(const char*, char **endptr, int base);
unsigned long int strtoul(const char*, char **endptr, int base);
size_t wcstombs(char *dest, const wchar_t *src, size_t n);
int wctomb(char *s, wchar_t wc);

void qsort(void *base, size_t n, size_t size, int (*fnc)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif
