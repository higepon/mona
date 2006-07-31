#include <string.h>

size_t strcspn(const char *str1, const char *str2)
{
	const char* const head = str1;
	const char* t;
	for (; *str1; str1++) {

		for (t = str2; *t; t++) if(*t == *str1) return str1 - head;
	}
	return str1 - head;
}
