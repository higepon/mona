#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif

typedef __SIZE_TYPE__ size_t;

void* memset(void*, int, size_t);
size_t strlen(const char*);
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
