#include <string.h>

size_t strspn(const char *str1, const char *str2)
{
	const char* const head = str1;

	for(; *str1; str1++) {

		const char* t;
		for(t = str2; *t != *str1; t++) {

			if(*t == '\0') return str1 - head;
		}
	}
	return str1 - head;
}
