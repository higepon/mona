#include <string.h>

int nidastrfind(const char *s, char c)
{
	while(*s)
	{
		if( c == *s )
			return 1;
		s++;
	}

	return 0;
}
