#include <stdlib.h>

long int strtol(const char *nptr, char **endptr, int base)
{
	return strtoi(nptr, endptr, base, 0, S_FORMAT_LONG);
}
