#include "string.h"
#include <monapi/syscall.h>

char* strchr(const char* s, int c)
{
	const char* p = s;
	while(*p != '\0')
	{
		if(*p == c)
			return (char*)p;
		p++;
	}

	return NULL;
}

char *strncat(char *dest, const char *src, size_t n)
{
	memcpy(dest+strlen(dest), src, strlen(src) > n ? n : strlen(src));

	return dest;
}

char *strdup(const char *s)
{
	char *ret = (char *)malloc(strlen(s) + 1);
	strcpy(ret, s);
	return ret;
}
