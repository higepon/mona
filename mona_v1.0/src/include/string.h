/*!
    \file  string.h
    \brief standard library

    standard library

    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/15 update:$Date$
*/
#include<types.h>

#ifndef _MONA_STRING_
#define _MONA_STRING_

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
#define __FBSDID(s)

typedef __SIZE_TYPE__ size_t;

void* memset(void*, int, size_t);
size_t strlen(const char*);
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
void* memmove(void*, const void*, size_t);
int memcmp(const void*, const void*, size_t);
void* memcpy(void* s1, const void* s2, size_t size);
char* strncpy(char* s1, const char* s2, size_t n);
size_t strcspn(const char* str1, const char* str2);
size_t strspn(const char* str1, const char* str2);
char* strtok(char* str, const char* delim);

#endif
