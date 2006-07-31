#include <fenv.h>

int fesetround(int r)
{
	unsigned short cw;
	
	asm volatile("fstcw %0" : "=m"(cw));
	cw = (cw & ~0x0C00) | (unsigned short)r;
	asm volatile("fldcw %0" : : "m"(cw));

	return 0;
}
