#include <fenv.h>

int fegetexceptflag(fexcept_t *f, int e)
{
	unsigned short sw;
	
	asm volatile("fstsw %0" : "=m"(sw));
	
	sw &= e;
	sw &= ~0xFFC0;
	*f = sw;
	
	return 0;
}
