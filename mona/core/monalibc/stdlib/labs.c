#include <stdlib.h>

long int labs(long int x)
{
	return ( x < 0 ) ? -x : x;
}
