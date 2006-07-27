#include <fenv.h>

int fegetround(void)
{
	unsigned short cw;
	
	asm volatile("fstcw %0" : "=m"(cw));
	
	return cw & ~0xF3FF;
}
