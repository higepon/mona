#ifndef _TMPMONA_LIB_STRING_
#define _TMPMONA_LIB_STRING_

#include <monapi/string.h>

#ifdef __cplusplus
extern "C" {
#endif

char *strncat(char *dest, const char *src, size_t n);
char *strchr(const char *s, int c);
char *strdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif
