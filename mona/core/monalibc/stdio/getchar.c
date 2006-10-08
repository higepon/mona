#include "stdio_p.h"

int getchar()
{
	return fgetc(stdout);
}

