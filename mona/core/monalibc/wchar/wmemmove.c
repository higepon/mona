#include <wchar.h>

wchar_t *wmemmove(wchar_t *s1, const wchar_t *s2, size_t size)
{
	wchar_t *p = s1;
	wchar_t *c1 = (wchar_t*)s1;
	wchar_t *c2 = (wchar_t*)s2;

	if( c1 > c2 )
	{
		c1 += size - 1;
		c2 += size - 1;
		while( 0 < size-- )
			*c1-- = *c2--;
	} else {
		while( 0 < size-- )
			*c1++ = *c2++;
	}

	return p;
}
