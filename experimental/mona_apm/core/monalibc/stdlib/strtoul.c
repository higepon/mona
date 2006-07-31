#include <stdlib.h>

unsigned long int strtoul(const char *s, char **endptr, int base)
{
	return strtoi(s, endptr, base, 0, S_FORMAT_LONG | S_FORMAT_UNSIGNED);
}
