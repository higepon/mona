#include "stdio_p.h"

int getc(FILE *fp)
{
	return fgetc(fp);
}

