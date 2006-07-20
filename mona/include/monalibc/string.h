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

#ifndef _STRING_H_
#define _STRING_H_

#include <monalibc/stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


void *memccpy(void *dest, const void *src, int c, size_t n);
void *memrchr(const void *s, int c, size_t n);
//void *memcpy(void *dest, const void *src, size_t n);
//void *memmove(void *dest, const void *src, size_t n);
void *memchr(const void *s, int c, size_t n);
//void *memset(void *s, int c, size_t n);
int nidastrfind(const char *s, char c);
//char *strcat(char* dst, const char *src);
char *strchr(const char *s, char c);
//int strcmp(const char *s1, const char *s2);
//char *strcpy(char *dst, const char *src);
//size_t strcspn(const char *s, const char *reject);
char *strerror(int n);
int strcoll(const char *s1, const char *s2);
//size_t strlen(const char *s);
char *strncat(char *dst, const char *src, size_t n);
//int strncmp(const char *s1, const char *s2, size_t n);
//char *strncpy(char *dst, const char *src, size_t n);
char *strpbrk(const char *s, const char *accept);
char *strrchr(const char *s, char c);
//size_t strspn(const char *s, const char *accept);
//char *strstr(const char *haystack, const char *needle);
char *strtok_r(char *s, const char *delim, char **lasts);
//char *strtok(char *s, const char *delim);
size_t strxfrm(char *dest, const char *src, size_t n);
char *strdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif
