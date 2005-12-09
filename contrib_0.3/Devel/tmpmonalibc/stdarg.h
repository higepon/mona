#ifndef _TMPMONA_LIB_STDARG_
#define _TMPMONA_LIB_STDARG_

#include <monalibc/stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int vfprintf(FILE *stream, const char *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif
