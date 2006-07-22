// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi/string.h>
#include "utils.h"

unsigned int xtoui(const char* str)
{
	if (str == NULL) return 0;
	
	unsigned int ret = 0;
	int len = strlen(str);
	for (int i = len - 1, sh = 0; i >= 0; i--, sh += 4)
	{
		char ch = str[i];
		unsigned int v = 0;
		if ('0' <= ch && ch <= '9')
		{
			v = ch - '0';
		}
		else if ('A' <= ch && ch <= 'F')
		{
			v = ch - 'A' + 10;
		}
		else if ('a' <= ch && ch <= 'f')
		{
			v = ch - 'a' + 10;
		}
		else
		{
			return 0;
		}
		ret += v << sh;
	}
	return ret;
}
