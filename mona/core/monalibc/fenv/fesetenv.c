#include <fenv.h>

int fesetenv(const fenv_t *f)
{
	fenv_t *tmp;

	tmp = (fenv_t*)f;

	if( tmp == FE_PC64_ENV )
	{
		asm volatile("finit");
	}
	else if( tmp == FE_PC53_ENV )
	{
		tmp->__status_word = 0x2f;
		asm volatile("fldenv %0" : : "m"(*tmp));
	}
	else if( tmp == FE_DFL_ENV )
	{
		asm volatile("finit");
	}
	else
	{
		asm volatile("fldenv %0" : : "m"(*tmp));
	}
	return 0;
}
