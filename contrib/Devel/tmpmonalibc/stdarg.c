#include "stdarg.h"
#include <stdio.h>
#include <string.h>

int vfprintf(FILE *stream, const char *format, va_list ap)
{
	char buf[5096];

	vsprintf(buf, format, ap);
	printf(buf);

	return strlen(buf);
}
