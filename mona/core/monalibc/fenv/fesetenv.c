#include <fenv.h>

int fesetenv(const fenv_t *f)
{
	if( f == FE_PC64_ENV )
	{
		asm volatile("finit");
	}
	else if( f == FE_PC53_ENV )
	{
		f->__status_word = 0x2f;
		asm volatile("fldenv %0" : : "m"(*f));
	}
	else if( f == FE_DFL_ENV )
	{
		asm volatile("finit");
	}
	else
	{
		asm volatile("fldenv %0" : : "m"(*f));
	}
	return 0;
}
