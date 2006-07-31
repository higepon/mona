#include <string.h>

int memcmp(const void *s1, const void *s2, size_t size)
{
	char *c1 = (char*)s1;
	char *c2 = (char*)s2;

	while( 0 < size-- )
	{
		if( *c1 != *c2 )
			return *c1 - *c2;
		c1++;
		c2++;
	}

	return 0;
}
