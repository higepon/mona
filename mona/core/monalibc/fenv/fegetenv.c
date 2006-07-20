#include <stddef.h>
#include <fenv.h>

int fegetenv(fenv_t *e)
{
	unsigned short cw;

	if( e == NULL ) return 1;

	asm volatile("fstcw -8(%ebp)");

	return 0;
}
