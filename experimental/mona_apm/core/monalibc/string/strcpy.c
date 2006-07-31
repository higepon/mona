#include <string.h>

char *strcpy(char *to, const char *from)
{
	char *p = to;
	while (*from) {
		*p = *from;
		p++;
		from++;
	}
	*p = '\0';
	return to;
}
