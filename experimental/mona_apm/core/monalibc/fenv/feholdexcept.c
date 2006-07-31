#include <fenv.h>

int feholdexcept(fenv_t *f)
{
	unsigned short cw;
	asm volatile("fstenv %0" : "=m"(*f));
	asm volatile("fclex");
	asm volatile("fstcw %0" : "=m"(cw));
	cw &= ~0x3F;
	asm volatile("fldcw %0" : : "m"(cw));
	return 0;
}
