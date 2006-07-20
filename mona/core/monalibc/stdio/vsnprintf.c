#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>

struct __conv_spec
{
	int flag;
	int field;
	int prec;
	int length;
	int conv_spec;
};

struct __conv_spec parse_conv_spec(char *str, size_t n)
{
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	int i, j;
	int length;
	int result;
	char *p = str;
	char *n;
	struct __conv_spec conv;

	if( size == -1 )
		size = (size_t)INT_MAX;
	if( size == 0 )
		return 0;

	va_start(ap, format);

	length = strlen(format);
	for( i = 0 ; i < length ; i++ )
	{
		if( format[i] == '%' )
		{
			n = strchr(format+i, ';');
			conv = parse_conv_spec(format+i, (size_t)n-i);
		}
		else
		{
			*p++ = format[i];
		}
	}

	result = (int)(str-p);
	return result;
}
