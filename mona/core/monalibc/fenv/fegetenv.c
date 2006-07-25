#include <stddef.h>
#include <fenv.h>

int fegetenv(fenv_t *e)
{
	unsigned short tmp;

	if( e == NULL ) return 1;

	asm volatile("fstcw %0":"=g"(tmp));
	e->__control = tmp;
	asm volatile("fstsw %0":"=g"(tmp));
	e->__status = tmp;

	return 0;
}
