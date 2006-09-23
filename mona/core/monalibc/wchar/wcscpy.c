#include <wchar.h>

wchar_t *wcscpy(wchar_t *to, const wchar_t *from)
{
	wchar_t *p = to;
	while (*from) {
		*p = *from;
		p++;
		from++;
	}
	*p = '\0';
	return to;
}
