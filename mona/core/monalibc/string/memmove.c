#include <string.h>

void *memmove(void *s1, const void *s2, size_t size)
{
	void *p = s1;
	char *c1 = (char*)s1;
	char *c2 = (char*)s2;

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
